// Created on: 2006-11-06
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <VrmlData_Coordinate.hxx>
#include <VrmlData_Geometry.hxx>
#include <VrmlData_Group.hxx>
#include <VrmlData_IndexedFaceSet.hxx>
#include <VrmlData_IndexedLineSet.hxx>
#include <VrmlData_Scene.hxx>
#include <VrmlData_WorldInfo.hxx>
#include <VrmlData_InBuffer.hxx>
#include <NCollection_List.hxx>
#include <VrmlData_Node.hxx>
#include <VrmlData_ShapeNode.hxx>
#include <VrmlData_UnknownNode.hxx>
#include <Precision.hxx>
#include <gp_Ax1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlData_Group, VrmlData_Node)

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_DEPRECATE
  #pragma warning(disable : 4996)
#endif

//=================================================================================================

VrmlData_Group::VrmlData_Group(const VrmlData_Scene& theScene,
                               const char*           theName,
                               const bool            isTransform)
    : VrmlData_Node(theScene, theName),
      myIsTransform(isTransform),
      myNodes(theScene.Allocator())
{
}

//=================================================================================================

bool VrmlData_Group::RemoveNode(const occ::handle<VrmlData_Node>& theNode)
{
  bool aResult(false);
  for (Iterator anIter = NodeIterator(); anIter.More(); anIter.Next())
    if (anIter.Value() == theNode)
    {
      aResult = true;
      myNodes.Remove(anIter);
      break;
    }
  return aResult;
}

//=================================================================================================

bool VrmlData_Group::SetTransform(const gp_Trsf& theTrsf)
{
  bool aResult(false);
  if (myIsTransform)
  {
    myTrsf  = theTrsf;
    aResult = true;
  }
  return aResult;
}

//=================================================================================================

occ::handle<VrmlData_Node> VrmlData_Group::Clone(const occ::handle<VrmlData_Node>& theOther) const
{
  occ::handle<VrmlData_Group> aResult =
    occ::down_cast<VrmlData_Group>(VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult =
      new VrmlData_Group(theOther.IsNull() ? Scene() : theOther->Scene(), Name(), myIsTransform);

  aResult->myIsTransform = myIsTransform;
  if (&aResult->Scene() == &Scene())
    aResult->myNodes = myNodes;
  else
  {
    // Create a dummy node to pass the different Scene instance to methods Clone
    const occ::handle<VrmlData_UnknownNode> aDummyNode = new VrmlData_UnknownNode(aResult->Scene());
    Iterator                                anIter(myNodes);
    for (; anIter.More(); anIter.Next())
    {
      const occ::handle<VrmlData_Node>& aNode = anIter.Value();
      if (aNode.IsNull() == false)
        aResult->myNodes.Append(aNode->Clone(aDummyNode));
    }
  }
  if (myIsTransform)
    aResult->SetTransform(myTrsf);
  aResult->SetBox(myBox);

  return aResult;
}

//=================================================================================================

occ::handle<VrmlData_Node> VrmlData_Group::FindNode(const char* theName, gp_Trsf& theLocation) const
{
  occ::handle<VrmlData_Node> aResult;
  Iterator                   anIter(myNodes);
  for (; anIter.More(); anIter.Next())
  {
    const occ::handle<VrmlData_Node>& aNode = anIter.Value();
    if (aNode.IsNull() == false)
    {
      if (strcmp(aNode->Name(), theName) == 0)
      {
        aResult     = aNode;
        theLocation = myTrsf;
        break;
      }
      // Try a Group type of node
      if (aNode->IsKind(STANDARD_TYPE(VrmlData_Group)))
      {
        const occ::handle<VrmlData_Group> aGroup = occ::down_cast<VrmlData_Group>(aNode);
        if (aGroup.IsNull() == false)
        {
          aResult = aGroup->FindNode(theName, theLocation);
          if (aResult.IsNull() == false)
          {
            // theLocation *= myTrsf;
            theLocation.PreMultiply(myTrsf);
            break;
          }
        }
      }
    }
  }
  return aResult;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Group::Read(VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  gp_XYZ               aBoxCenter(0., 0., 0.), aBoxSize(-1., -1., -1.);
  gp_XYZ               aCenter(0., 0., 0.), aScale(1., 1., 1.), aTrans(0., 0., 0.);
  gp_XYZ               aRotAxis(0., 0., 1.), aScaleAxis(0., 0., 1.);
  double               aRotAngle(0.), aScaleAngle(0.);

  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "bboxCenter"))
      aStatus = Scene().ReadXYZ(theBuffer, aBoxCenter, true, false);

    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "bboxSize"))
      aStatus = Scene().ReadXYZ(theBuffer, aBoxSize, true, false);

    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "children"))
    {
      bool isBracketed(false);
      // Read the opening bracket for the list of children
      if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
        break;
      if (theBuffer.LinePtr[0] == '[')
      {
        theBuffer.LinePtr++;
        if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
          break;
        isBracketed = true;
      }

      // Read the child nodes
      occ::handle<VrmlData_Node> aChildNode;
      while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
      {
        // read the end-of-list bracket
        if (isBracketed && theBuffer.LinePtr[0] == ']')
        {
          theBuffer.LinePtr++;
          break;
        }
        // otherwise read a node
        if (!OK(aStatus, ReadNode(theBuffer, aChildNode)))
          break;
        AddNode(aChildNode);
        if (isBracketed == false)
          break;
      }
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "collide"))
    {
      TCollection_AsciiString aDummy;
      aStatus = Scene().ReadWord(theBuffer, aDummy);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "Switch"))
    {
      bool isBracketed(false);
      // Read the opening bracket for the list of children
      if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
        break;

      if (theBuffer.LinePtr[0] == '{')
      {
        theBuffer.LinePtr++;
        if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
          break;
        isBracketed = true;
      }

      // Read the child nodes
      occ::handle<VrmlData_Node> aChildNode;
      while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
      {
        // read the end-of-list bracket
        if (isBracketed && theBuffer.LinePtr[0] == '}')
        {
          theBuffer.LinePtr++;
          break;
        }

        // otherwise read a node
        if (!OK(aStatus, ReadNode(theBuffer, aChildNode)))
          break;

        AddNode(aChildNode);
        if (isBracketed == false)
          break;
      }
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "TransformSeparator")
             || VRMLDATA_LCOMPARE(theBuffer.LinePtr, "Separator"))
    {
      occ::handle<VrmlData_Group> aGroupNode = new VrmlData_Group(Scene(), "");
      bool                        isBracketed(false);
      // Read the opening bracket for the list of children
      if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
        break;

      if (theBuffer.LinePtr[0] == '{')
      {
        theBuffer.LinePtr++;
        if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
          break;
        isBracketed = true;
      }

      // Read the child nodes
      occ::handle<VrmlData_Node> aChildNode;
      while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
      {
        // read the end-of-list bracket
        if (isBracketed && theBuffer.LinePtr[0] == '}')
        {
          theBuffer.LinePtr++;
          break;
        }
        // otherwise read a node
        if (!OK(aStatus, ReadNode(theBuffer, aChildNode)))
          break;

        aGroupNode->AddNode(aChildNode);
        if (isBracketed == false)
          break;
      }

      occ::handle<VrmlData_Coordinate>     aCoord;
      occ::handle<VrmlData_IndexedFaceSet> anIndFaceSet;
      occ::handle<VrmlData_IndexedLineSet> anIndLineSet;
      occ::handle<VrmlData_Appearance>     anAppearance;
      for (NCollection_List<occ::handle<VrmlData_Node>>::Iterator anIt = aGroupNode->NodeIterator();
           anIt.More();
           anIt.Next())
      {
        occ::handle<VrmlData_Node> aNode = anIt.Value();
        if (aNode.IsNull())
          continue;

        if (anIt.Value()->IsKind(STANDARD_TYPE(VrmlData_Coordinate)))
        {
          aCoord = occ::down_cast<VrmlData_Coordinate>(anIt.Value());
        }
        else if (anIt.Value()->IsKind(STANDARD_TYPE(VrmlData_IndexedFaceSet)))
        {
          anIndFaceSet = occ::down_cast<VrmlData_IndexedFaceSet>(anIt.Value());
          if (!aCoord.IsNull())
          {
            anIndFaceSet->SetCoordinates(aCoord);
          }
        }
        else if (anIt.Value()->IsKind(STANDARD_TYPE(VrmlData_IndexedLineSet)))
        {
          anIndLineSet = occ::down_cast<VrmlData_IndexedLineSet>(anIt.Value());
          if (!aCoord.IsNull())
          {
            anIndLineSet->SetCoordinates(aCoord);
          }
        }
        else if (anIt.Value()->IsKind(STANDARD_TYPE(VrmlData_Material)))
        {
          occ::handle<VrmlData_Material> aMaterial =
            occ::down_cast<VrmlData_Material>(anIt.Value());
          anAppearance = new VrmlData_Appearance();
          anAppearance->SetMaterial(aMaterial);
        }
      }

      if (!aCoord.IsNull())
      {
        occ::handle<VrmlData_ShapeNode> aShapeNode = new VrmlData_ShapeNode(Scene(), "");
        if (!anIndFaceSet.IsNull())
        {
          aShapeNode->SetGeometry(anIndFaceSet);
        }
        else if (!anIndLineSet.IsNull())
        {
          aShapeNode->SetGeometry(anIndLineSet);
        }
        if (!anAppearance.IsNull())
        {
          aShapeNode->SetAppearance(anAppearance);
        }
        aGroupNode->AddNode(aShapeNode);
      }

      AddNode(aGroupNode);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "ShapeHints")
             || VRMLDATA_LCOMPARE(theBuffer.LinePtr, "DirectionalLight"))
    {
      // Skip this tag
      if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
        break;

      if (theBuffer.LinePtr[0] == '{')
      {
        theBuffer.LinePtr++;
        if (!OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
          break;

        while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
        {
          // read the end-of-list bracket
          if (theBuffer.LinePtr[0] == '}')
          {
            theBuffer.LinePtr++;
            break;
          }
          theBuffer.LinePtr++;
        }
      }
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "center"))
      if (myIsTransform)
        aStatus = Scene().ReadXYZ(theBuffer, aCenter, true, false);
      else
      {
        aStatus = VrmlData_VrmlFormatError;
        break;
      }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "rotation"))
      if (myIsTransform)
      {
        if (OK(aStatus, Scene().ReadXYZ(theBuffer, aRotAxis, false, false)))
        {
          if (aRotAxis.SquareModulus() < Precision::Confusion())
            aRotAxis.SetZ(1.0);
          aStatus = Scene().ReadReal(theBuffer, aRotAngle, false, false);
        }
      }
      else
      {
        aStatus = VrmlData_VrmlFormatError;
        break;
      }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "scaleOrientation"))
      if (myIsTransform)
      {
        if (OK(aStatus, Scene().ReadXYZ(theBuffer, aScaleAxis, false, false)))
          aStatus = Scene().ReadReal(theBuffer, aScaleAngle, false, false);
      }
      else
      {
        aStatus = VrmlData_VrmlFormatError;
        break;
      }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "scale"))
      if (myIsTransform)
        aStatus = Scene().ReadXYZ(theBuffer, aScale, false, true);
      else
      {
        aStatus = VrmlData_VrmlFormatError;
        break;
      }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "translation"))
      if (myIsTransform)
        aStatus = Scene().ReadXYZ(theBuffer, aTrans, true, false);
      else
      {
        aStatus = VrmlData_VrmlFormatError;
        break;
      }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "DEF"))
    {
      TCollection_AsciiString aWord;
      if (OK(aStatus, VrmlData_Scene::ReadWord(theBuffer, aWord)))
      {
        setName(aWord.ToCString());
      }
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "url"))
    {
      NCollection_List<TCollection_AsciiString> lstURL;
      if (OK(aStatus, ReadMultiString(theBuffer, lstURL)))
      {
        NCollection_List<TCollection_AsciiString>::Iterator anIter(lstURL);
        for (; anIter.More(); anIter.Next())
        {
          std::ifstream                  aStream;
          const TCollection_AsciiString& aFileName = anIter.Value();
          if (!OK(aStatus, openFile(aStream, aFileName)))
            break;
          VrmlData_Scene aScene(Scene().Allocator());
          aScene.myLinearScale = Scene().myLinearScale;
          aScene.myVrmlDir     = Scene().myVrmlDir;
          aScene << aStream;
          if (!OK(aStatus, aScene.Status()))
            break;
          VrmlData_Scene::Iterator anIterN = aScene.GetIterator();
          for (; anIterN.More(); anIterN.Next())
            if (!anIterN.Value()->IsKind(STANDARD_TYPE(VrmlData_WorldInfo)))
              AddNode(anIterN.Value());
          VrmlData_Scene::Iterator anAllIter(aScene.myAllNodes);
          for (; anAllIter.More(); anAllIter.Next())
          {
            const occ::handle<VrmlData_Node>& aNode = anAllIter.Value();
            if (aNode->IsKind(STANDARD_TYPE(VrmlData_WorldInfo)))
              continue;
            const_cast<VrmlData_Scene&>(Scene()).myAllNodes.Append(aNode);
            aNode->myScene = &Scene();
            // The name of the imported node should be prefixed by the URL
            // because each name must remain unique in the global scene.
            if (aNode->Name())
              if (*aNode->Name() != '\0')
              {
                TCollection_AsciiString buf;
                buf += aFileName;
                int aCharLocation = buf.Location(1, '.', 1, buf.Length());
                if (aCharLocation != 0)
                {
                  buf.Remove(aCharLocation, buf.Length() - aCharLocation + 1);
                }
                buf += '_';
                buf += aNode->Name();
                const size_t len      = buf.Length();
                char*        aNewName = static_cast<char*>(Scene().Allocator()->Allocate(len));
                if (aNewName)
                {
                  aNode->myName = aNewName;
                  memcpy(aNewName, buf.ToCString(), len);
                }
              }
          }
        }
      }
    }
    else
      break;

    if (!OK(aStatus))
      break;
  }

  // Read the terminating (closing) brace
  if (OK(aStatus))
    aStatus = readBrace(theBuffer);
  if (OK(aStatus))
  {
    // Check if the Bounding Box has been imported
    if (aBoxSize.X() > -Precision::Confusion() && aBoxSize.Y() > -Precision::Confusion()
        && aBoxSize.Z() > -Precision::Confusion())
    {
      myBox.SetCenter(aBoxCenter);
      myBox.SetHSize(aBoxSize * 0.5);
    }
    if (myIsTransform)
    {
      // Create the corresponding transformation.
      gp_Trsf tRot, tCentInv;
      myTrsf.SetTranslation(aTrans + aCenter);
      gp_Ax1 aRotation(gp::Origin(), aRotAxis);
      tRot.SetRotation(gp_Ax1(gp::Origin(), aRotAxis), aRotAngle);
      myTrsf.Multiply(tRot);
      // Check that the scale is uniform (the same value in all 3 directions.
      // Only in this case the scaling is applied.
      const double aScaleDiff[2] = {aScale.X() - aScale.Y(), aScale.X() - aScale.Z()};
      if (aScaleDiff[0] * aScaleDiff[0] + aScaleDiff[1] * aScaleDiff[1] < Precision::Confusion())
      {
        gp_Trsf tScale;
        tScale.SetScale(gp::Origin(), (aScale.X() + aScale.Y() + aScale.Z()) / 3.);
        myTrsf.Multiply(tScale);
      }
      tCentInv.SetTranslation(aCenter.Reversed());
      myTrsf.Multiply(tCentInv);
    }
  }
  return aStatus;
}

//=================================================================================================

void VrmlData_Group::Shape(
  TopoDS_Shape&                                                                      theShape,
  NCollection_DataMap<occ::handle<TopoDS_TShape>, occ::handle<VrmlData_Appearance>>* pMapApp)
{
  VrmlData_Scene::createShape(theShape, myNodes, pMapApp);
  theShape.Location(myTrsf, false);
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Group::openFile(Standard_IStream&              theStream,
                                              const TCollection_AsciiString& theFilename)
{
  std::ifstream&       aStream = static_cast<std::ifstream&>(theStream);
  VrmlData_ErrorStatus aStatus(VrmlData_EmptyData);
  NCollection_List<TCollection_ExtendedString>::Iterator aDirIter = Scene().VrmlDirIterator();
  for (; aDirIter.More(); aDirIter.Next())
  {
    if (!aDirIter.Value().IsAscii())
      continue;
    const TCollection_AsciiString aFullName =
      TCollection_AsciiString(aDirIter.Value()) + theFilename;
    aStream.open(aFullName.ToCString(), std::ios::in);
    if (aStream.fail())
      aStream.clear();
    else
    {
      aStatus = VrmlData_StatusOK;
      break;
    }
  }
  if (aStatus == VrmlData_EmptyData)
  {
    aStream.open(theFilename.ToCString(), std::ios::in);
    if (!aStream.fail())
      aStatus = VrmlData_StatusOK;
  }
  if (aStatus == VrmlData_EmptyData)
    aStatus = VrmlData_CannotOpenFile;
  return aStatus;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Group::Write(const char* thePrefix) const
{
  VrmlData_ErrorStatus aStatus(VrmlData_StatusOK);
  if (myNodes.IsEmpty() == false)
  {
    const VrmlData_Scene& aScene      = Scene();
    bool                  isTransform = myIsTransform;
    if (isTransform && myTrsf.Form() == gp_Identity)
      isTransform = false;
    static const char* header[2] = {"Group {", "Transform {"};
    if (OK(aStatus, aScene.WriteLine(thePrefix, header[isTransform ? 1 : 0], GlobalIndent())))
    {
      char buf[240];
      if (OK(aStatus) && aScene.IsDummyWrite() == false)
      {
        const gp_XYZ aBoxCorner[2] = {myBox.CornerMin(), myBox.CornerMax()};
        // Check that the box is not void
        if (aBoxCorner[0].X() < aBoxCorner[1].X() + Precision::Confusion())
        {
          Sprintf(buf,
                  "bboxCenter  %.9g %.9g %.9g",
                  0.5 * (aBoxCorner[0].X() + aBoxCorner[1].X()),
                  0.5 * (aBoxCorner[0].Y() + aBoxCorner[1].Y()),
                  0.5 * (aBoxCorner[0].Z() + aBoxCorner[1].Z()));
          aStatus = aScene.WriteLine(buf);
          if (OK(aStatus))
          {
            Sprintf(buf,
                    "bboxSize    %.9g %.9g %.9g",
                    aBoxCorner[1].X() - aBoxCorner[0].X(),
                    aBoxCorner[1].Y() - aBoxCorner[0].Y(),
                    aBoxCorner[1].Z() - aBoxCorner[0].Z());
            aStatus = aScene.WriteLine(buf);
          }
        }
      }
      if (OK(aStatus) && isTransform && aScene.IsDummyWrite() == false)
      {
        // Output the Scale
        const double aScaleFactor = myTrsf.ScaleFactor();
        if ((aScaleFactor - 1.) * (aScaleFactor - 1.) > 0.0001 * Precision::Confusion())
        {
          Sprintf(buf, "scale       %.12g %.12g %.12g", aScaleFactor, aScaleFactor, aScaleFactor);
          aStatus = aScene.WriteLine(buf);
        }

        // Output the Translation
        const gp_XYZ& aTrans = myTrsf.TranslationPart();
        if (aTrans.SquareModulus() > 0.0001 * Precision::Confusion())
        {
          Sprintf(buf, "translation %.12g %.12g %.12g", aTrans.X(), aTrans.Y(), aTrans.Z());
          aStatus = aScene.WriteLine(buf);
        }

        // Output the Rotation
        gp_XYZ anAxis;
        double anAngle;
        if (myTrsf.GetRotation(anAxis, anAngle))
        {
          // output the Rotation
          Sprintf(buf,
                  "rotation    %.12g %.12g %.12g %.9g",
                  anAxis.X(),
                  anAxis.Y(),
                  anAxis.Z(),
                  anAngle);
          aStatus = aScene.WriteLine(buf);
        }
      }

      if (OK(aStatus))
      {

        aStatus = aScene.WriteLine("children [", 0L, GlobalIndent());

        NCollection_List<occ::handle<VrmlData_Node>>::Iterator anIterChild(myNodes);
        for (; anIterChild.More() && OK(aStatus); anIterChild.Next())
        {
          const occ::handle<VrmlData_Node>& aNode = anIterChild.Value();
          aScene.WriteNode(0L, aNode);
        }

        if (OK(aStatus))
        {
          aStatus = aScene.WriteLine("]", 0L, -GlobalIndent());
        }
      }
      aStatus = WriteClosing();
    }
  }
  return aStatus;
}
