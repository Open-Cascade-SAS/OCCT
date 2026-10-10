// Created on: 2006-11-04
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

#include <VrmlData_IndexedFaceSet.hxx>
#include <VrmlData_InBuffer.hxx>
#include <VrmlData_UnknownNode.hxx>
#include <BRep_TFace.hxx>
#include <BRepMesh_Triangulator.hxx>
#include <VrmlData_Coordinate.hxx>
#include <VrmlData_Color.hxx>
#include <VrmlData_Normal.hxx>
#include <VrmlData_TextureCoordinate.hxx>
#include <VrmlData_Scene.hxx>
#include <Precision.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatMap.hxx>
#include <Poly.hxx>
#include <Standard_ShortReal.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlData_IndexedFaceSet, VrmlData_Faceted)

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_DEPRECATE
  #pragma warning(disable : 4996)
#endif

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Faceted::readData(VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus(VrmlData_EmptyData);
  bool                 aBool;
  if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "ccw"))
  {
    if (OK(aStatus, ReadBoolean(theBuffer, aBool)))
    {
      myIsCCW = aBool;
    }
  }
  else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "convex"))
  {
    if (OK(aStatus, ReadBoolean(theBuffer, aBool)))
    {
      myIsConvex = aBool;
    }
  }
  else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "solid"))
  {
    if (OK(aStatus, ReadBoolean(theBuffer, aBool)))
    {
      myIsSolid = aBool;
    }
  }
  else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "creaseAngle"))
  {
    double anAngle;
    if (OK(aStatus, Scene().ReadReal(theBuffer, anAngle, false, false)))
    {
      if (anAngle < -Precision::Confusion() * 0.001)
      {
        aStatus = VrmlData_IrrelevantNumber;
      }
      else
      {
        myCreaseAngle = anAngle;
      }
    }
  }
  return aStatus;
}

//=================================================================================================

const occ::handle<TopoDS_TShape>& VrmlData_IndexedFaceSet::TShape()
{
  if (myNbPolygons == 0)
  {
    myTShape.Nullify();
    return myTShape;
  }
  else if (!myIsModified)
  {
    return myTShape;
  }

  if (myCoords.IsNull() || myArrPolygons == nullptr)
  {
    myTShape.Nullify();
    return myTShape;
  }

  const size_t aNbNodes = myCoords->Length();

  NCollection_FlatMap<size_t>                 aNodeIds;
  NCollection_FlatMap<size_t>                 aPolygonIds;
  NCollection_List<NCollection_Sequence<int>> aPolygons;
  NCollection_List<gp_Dir>                    aNorms;
  for (size_t aPolygonIdx = 0; aPolygonIdx < myNbPolygons; ++aPolygonIdx)
  {
    const int* aPolygonData = myArrPolygons[aPolygonIdx];
    if (aPolygonData == nullptr)
    {
      continue;
    }
    const int aNbPolygonNodes = aPolygonData[0];
    if (aNbPolygonNodes < 3)
    {
      // bad polygon
      continue;
    }
    NCollection_Sequence<int> aPolygon;
    int                       aPolygonNodeIdx = 1;
    for (; aPolygonNodeIdx <= aNbPolygonNodes; ++aPolygonNodeIdx)
    {
      const int aCoordIdx = aPolygonData[aPolygonNodeIdx];
      if (aCoordIdx < 0 || static_cast<size_t>(aCoordIdx) >= aNbNodes)
      {
        break;
      }
      aPolygon.Append(aCoordIdx);
    }
    if (aPolygonNodeIdx <= aNbPolygonNodes)
    {
      // bad index of node in polygon
      continue;
    }
    // calculate normal
    gp_XYZ aSum;
    gp_XYZ aPrevP = myCoords->Coordinate(static_cast<size_t>(aPolygon.First()));
    for (int aNodeIdx = 2; aNodeIdx < aPolygon.Length(); ++aNodeIdx)
    {
      gp_XYZ aP1           = myCoords->Coordinate(static_cast<size_t>(aPolygon(aNodeIdx)));
      gp_XYZ aP2           = myCoords->Coordinate(static_cast<size_t>(aPolygon(aNodeIdx + 1)));
      gp_XYZ aV1           = aP1 - aPrevP;
      gp_XYZ aV2           = aP2 - aPrevP;
      gp_XYZ aCrossProduct = aV1.Crossed(aV2);
      aSum += aCrossProduct;
    }
    if (aSum.Modulus() < Precision::Confusion())
    {
      // degenerate polygon
      continue;
    }
    gp_Dir aNormal(aSum);
    aPolygonIds.Add(aPolygonIdx);
    aPolygons.Append(aPolygon);
    aNorms.Append(aNormal);
    // collect info about used indices
    for (int aNodeIdx = 1; aNodeIdx <= aPolygon.Length(); ++aNodeIdx)
    {
      aNodeIds.Add(static_cast<size_t>(aPolygonData[aNodeIdx]));
    }
  }

  if (aNodeIds.IsEmpty())
  {
    myIsModified = false;
    myTShape.Nullify();
    return myTShape;
  }
  // prepare vector of nodes
  NCollection_DynamicArray<gp_XYZ> aNodes;
  NCollection_DataMap<size_t, int> aNodeIdMap;
  for (size_t aNodeIdx = 0; aNodeIdx < aNbNodes; ++aNodeIdx)
  {
    if (aNodeIds.Contains(aNodeIdx))
    {
      aNodeIdMap.Bind(aNodeIdx, aNodes.Length());
      aNodes.Append(myCoords->Coordinate(aNodeIdx));
    }
  }
  // update polygon indices
  NCollection_List<NCollection_Sequence<int>>::Iterator aPolygonIter(aPolygons);
  for (; aPolygonIter.More(); aPolygonIter.Next())
  {
    NCollection_Sequence<int>& aPolygon = aPolygonIter.ChangeValue();
    for (int aNodeIdx = 1; aNodeIdx <= aPolygon.Length(); ++aNodeIdx)
    {
      const int aNewNodeIdx = aNodeIdMap.Find(static_cast<size_t>(aPolygon.Value(aNodeIdx)));
      aPolygon.ChangeValue(aNodeIdx) = aNewNodeIdx;
    }
  }
  // calculate triangles
  NCollection_List<Poly_Triangle> aTriangles;
  aPolygonIter.Init(aPolygons);
  for (NCollection_List<gp_Dir>::Iterator aNormalIter(aNorms); aPolygonIter.More();
       aPolygonIter.Next(), aNormalIter.Next())
  {
    NCollection_List<Poly_Triangle> aTrias;
    try
    {
      NCollection_List<NCollection_Sequence<int>> aPList;
      aPList.Append(aPolygonIter.Value());
      BRepMesh_Triangulator aTriangulator(aNodes, aPList, aNormalIter.Value());
      aTriangulator.Perform(aTrias);
      aTriangles.Append(aTrias);
    }
    catch (...)
    {
      continue;
    }
  }
  if (aTriangles.IsEmpty())
  {
    return myTShape;
  }

  // Triangulation creation
  occ::handle<Poly_Triangulation> aTriangulation =
    new Poly_Triangulation(aNodes.Length(), aTriangles.Extent(), false);
  // Copy the triangulation vertices
  for (int aNodeIdx = 0; aNodeIdx < aNodes.Length(); ++aNodeIdx)
  {
    aTriangulation->SetNode(aNodeIdx + 1, gp_Pnt(aNodes(aNodeIdx)));
  }
  // Copy the triangles.
  NCollection_List<Poly_Triangle>::Iterator aTriangleIter(aTriangles);
  for (int aTriangleIdx = 1; aTriangleIter.More(); aTriangleIter.Next(), ++aTriangleIdx)
  {
    aTriangulation->SetTriangle(aTriangleIdx, aTriangleIter.Value());
  }

  occ::handle<BRep_TFace> aFace = new BRep_TFace();
  aFace->Triangulation(aTriangulation);
  myTShape = aFace;

  // Normals should be defined; if they are not, compute them
  if (myNormals.IsNull())
  {
    Poly::ComputeNormals(aTriangulation);
  }
  else
  {
    // Copy the normals. Currently only normals-per-vertex are supported.
    if (myNormalPerVertex)
    {
      bool hasValidNormals = true;
      if (myArrNormalInd == nullptr)
      {
        for (size_t aNodeIdx = 0; aNodeIdx < aNbNodes; ++aNodeIdx)
        {
          if (aNodeIdMap.IsBound(aNodeIdx) && aNodeIdx >= myNormals->Length())
          {
            hasValidNormals = false;
            break;
          }
        }
      }
      else
      {
        for (size_t aPolygonIdx = 0; aPolygonIdx < myNbPolygons && hasValidNormals; ++aPolygonIdx)
        {
          if (!aPolygonIds.Contains(aPolygonIdx))
          {
            continue;
          }
          if (aPolygonIdx >= myNbNormals || myArrNormalInd[aPolygonIdx] == nullptr)
          {
            hasValidNormals = false;
            break;
          }
          const int* aCoordIndices;
          const int  aNbPolygonNodes = Polygon(aPolygonIdx, aCoordIndices);
          const int* aNormalIndices;
          const int  aNbNormalIndices = IndiceNormals(aPolygonIdx, aNormalIndices);
          if (aNbNormalIndices != aNbPolygonNodes)
          {
            hasValidNormals = false;
            break;
          }
          for (size_t aNodeIdx = 0; aNodeIdx < static_cast<size_t>(aNbNormalIndices); ++aNodeIdx)
          {
            const int aNormalIdx = aNormalIndices[aNodeIdx];
            if (aNormalIdx < 0 || static_cast<size_t>(aNormalIdx) >= myNormals->Length())
            {
              hasValidNormals = false;
              break;
            }
          }
        }
      }

      if (!hasValidNormals)
      {
        Poly::ComputeNormals(aTriangulation);
      }
      else
      {
        aTriangulation->AddNormals();
        if (myArrNormalInd == nullptr)
        {
          for (size_t aNodeIdx = 0; aNodeIdx < aNbNodes; ++aNodeIdx)
          {
            if (aNodeIdMap.IsBound(aNodeIdx))
            {
              aTriangulation->SetNormal(aNodeIdMap.Find(aNodeIdx) + 1, myNormals->Normal(aNodeIdx));
            }
          }
        }
        else
        {
          for (size_t aPolygonIdx = 0; aPolygonIdx < myNbPolygons; ++aPolygonIdx)
          {
            if (aPolygonIds.Contains(aPolygonIdx)) // check to avoid previously skipped faces
            {
              const int* aCoordIndices;
              Polygon(aPolygonIdx, aCoordIndices);
              const int* aNormalIndices;
              const int  aNbNormalIndices = IndiceNormals(aPolygonIdx, aNormalIndices);
              for (size_t aNodeIdx = 0; aNodeIdx < static_cast<size_t>(aNbNormalIndices);
                   ++aNodeIdx)
              {
                const gp_XYZ& aNormal =
                  myNormals->Normal(static_cast<size_t>(aNormalIndices[aNodeIdx]));
                aTriangulation->SetNormal(aNodeIdMap(static_cast<size_t>(aCoordIndices[aNodeIdx]))
                                            + 1,
                                          aNormal);
              }
            }
          }
        }
      }
    }
    else
    {
      // TODO ..
    }
  }

  myIsModified = false;

  return myTShape;
}

//=================================================================================================

occ::handle<VrmlData_Node> VrmlData_IndexedFaceSet::Clone(
  const occ::handle<VrmlData_Node>& theOther) const
{
  occ::handle<VrmlData_IndexedFaceSet> aResult =
    occ::down_cast<VrmlData_IndexedFaceSet>(VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
  {
    aResult = new VrmlData_IndexedFaceSet(theOther.IsNull() ? Scene() : theOther->Scene(), Name());
  }

  if (&aResult->Scene() == &Scene())
  {
    aResult->SetCoordinates(myCoords);
    aResult->SetNormals(myNormals);
    aResult->SetColors(myColors);
    aResult->SetPolygons(myNbPolygons, myArrPolygons);
    aResult->SetNormalInd(myNbNormals, myArrNormalInd);
    aResult->SetColorInd(myNbColors, myArrColorInd);
    aResult->SetTextureCoordInd(myNbTextures, myArrTextureInd);
  }
  else
  {
    // Create a dummy node to pass the different Scene instance to methods Clone
    const occ::handle<VrmlData_UnknownNode> aDummyNode = new VrmlData_UnknownNode(aResult->Scene());
    if (!myCoords.IsNull())
    {
      aResult->SetCoordinates(occ::down_cast<VrmlData_Coordinate>(myCoords->Clone(aDummyNode)));
    }
    if (!myNormals.IsNull())
    {
      aResult->SetNormals(occ::down_cast<VrmlData_Normal>(myNormals->Clone(aDummyNode)));
    }
    if (!myColors.IsNull())
    {
      aResult->SetColors(occ::down_cast<VrmlData_Color>(myColors->Clone(aDummyNode)));
    }
    // TODO: Replace the following lines with the relevant copying
    aResult->SetPolygons(myNbPolygons, myArrPolygons);
    aResult->SetNormalInd(myNbNormals, myArrNormalInd);
    aResult->SetColorInd(myNbColors, myArrColorInd);
    aResult->SetTextureCoordInd(myNbTextures, myArrTextureInd);
  }
  aResult->SetNormalPerVertex(myNormalPerVertex);
  aResult->SetColorPerVertex(myColorPerVertex);
  return aResult;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_IndexedFaceSet::Read(VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus  aStatus;
  const VrmlData_Scene& aScene = Scene();
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (OK(aStatus, VrmlData_Faceted::readData(theBuffer)))
    {
      continue;
    }
    if (aStatus != VrmlData_EmptyData)
    {
      break;
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "colorPerVertex"))
    {
      aStatus = ReadBoolean(theBuffer, myColorPerVertex);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "normalPerVertex"))
    {
      aStatus = ReadBoolean(theBuffer, myNormalPerVertex);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "coordIndex"))
    {
      aStatus = aScene.ReadArrIndex(theBuffer, myArrPolygons, myNbPolygons);
      // for (int i = 0; i < myNbPolygons; i++)
      //{
      //   const int * anArray = myArrPolygons[i];
      //   int nbPoints = anArray[0];
      //   std::cout << "i = " << i << "  indexes:";
      //   for (int ip = 1; ip <= nbPoints; ip++)
      //   {
      //     std::cout << " " << anArray[ip];
      //   }
      //   std::cout << std::endl;
      // }
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "colorIndex"))
    {
      aStatus = aScene.ReadArrIndex(theBuffer, myArrColorInd, myNbColors);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "normalIndex"))
    {
      aStatus = aScene.ReadArrIndex(theBuffer, myArrNormalInd, myNbNormals);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "texCoordIndex"))
    {
      aStatus = aScene.ReadArrIndex(theBuffer, myArrTextureInd, myNbTextures);
      // These four checks should be the last one to avoid their interference
      // with the other tokens (e.g., coordIndex)
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "texCoord"))
    {
      occ::handle<VrmlData_Node> aNode;
      aStatus    = ReadNode(theBuffer, aNode, STANDARD_TYPE(VrmlData_TextureCoordinate));
      myTxCoords = occ::down_cast<VrmlData_TextureCoordinate>(aNode);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "color"))
    {
      occ::handle<VrmlData_Node> aNode;
      aStatus  = ReadNode(theBuffer, aNode, STANDARD_TYPE(VrmlData_Color));
      myColors = occ::down_cast<VrmlData_Color>(aNode);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "coord"))
    {
      occ::handle<VrmlData_Node> aNode;
      aStatus  = ReadNode(theBuffer, aNode, STANDARD_TYPE(VrmlData_Coordinate));
      myCoords = occ::down_cast<VrmlData_Coordinate>(aNode);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "normal"))
    {
      occ::handle<VrmlData_Node> aNode;
      aStatus   = ReadNode(theBuffer, aNode, STANDARD_TYPE(VrmlData_Normal));
      myNormals = occ::down_cast<VrmlData_Normal>(aNode);
    }
    if (!OK(aStatus))
    {
      break;
    }
  }
  // Read the terminating (closing) brace
  if (OK(aStatus) || aStatus == VrmlData_EmptyData)
  {
    if (OK(aStatus, readBrace(theBuffer)))
    {
      // Post-processing
      ;
    }
  }
  return aStatus;
}

// //=======================================================================
// //function : dummyReadBrackets
// //purpose  : static (local) function
// //=======================================================================

// VrmlData_ErrorStatus dummyReadBrackets (VrmlData_InBuffer& theBuffer)
// {
//   VrmlData_ErrorStatus aStatus;
//   int aLevelCounter (0);
//   // This loop searches for any opening bracket.
//   // Such bracket increments the level counter. A closing bracket decrements
//   // the counter. The loop terminates when the counter becomes zero.
//   while ((aStatus = VrmlData_Scene::ReadLine(theBuffer)) == VrmlData_StatusOK)
//   {
//     int aChar;
//     while ((aChar = theBuffer.LinePtr[0]) != '\0') {
//       theBuffer.LinePtr++;
//       if        (aChar == '[') {
//         aLevelCounter++;
//         break;
//       } else if (aChar == ']') {
//         aLevelCounter--;
//         break;
//       }
//     }
//     if (aLevelCounter <= 0)
//       break;
//   }
//   return aStatus;
// }

//=================================================================================================

bool VrmlData_IndexedFaceSet::IsDefault() const
{
  bool aResult(true);
  if (myNbPolygons)
  {
    aResult = false;
  }
  else if (!myCoords.IsNull())
  {
    aResult = myCoords->IsDefault();
  }
  return aResult;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_IndexedFaceSet::Write(const char* thePrefix) const
{
  static char           header[] = "IndexedFaceSet {";
  const VrmlData_Scene& aScene   = Scene();
  VrmlData_ErrorStatus  aStatus;
  if (OK(aStatus, aScene.WriteLine(thePrefix, header, GlobalIndent())))
  {

    // Write the attributes of interface "VrmlData_Faceted"
    if (!IsCCW())
    {
      aStatus = aScene.WriteLine("ccw         FALSE");
    }
    if (OK(aStatus) && !IsSolid())
    {
      aStatus = aScene.WriteLine("solid       FALSE");
    }
    if (OK(aStatus) && !IsConvex())
    {
      aStatus = aScene.WriteLine("convex      FALSE");
    }
    if (OK(aStatus) && CreaseAngle() > Precision::Confusion())
    {
      char buf[64];
      Sprintf(buf, "%.9g", CreaseAngle());
      aStatus = aScene.WriteLine("creaseAngle", buf);
    }

    if (OK(aStatus) && !myCoords.IsNull())
    {
      aStatus = aScene.WriteNode("coord", myCoords);
    }
    if (OK(aStatus))
    {
      aStatus = aScene.WriteArrIndex("coordIndex", myArrPolygons, myNbPolygons);
    }

    if (OK(aStatus) && !myNormalPerVertex)
    {
      aStatus = aScene.WriteLine("normalPerVertex FALSE");
    }
    if (OK(aStatus) && !myNormals.IsNull())
    {
      aStatus = aScene.WriteNode("normal", myNormals);
    }
    if (OK(aStatus))
    {
      aStatus = aScene.WriteArrIndex("normalIndex", myArrNormalInd, myNbNormals);
    }

    if (OK(aStatus) && !myColorPerVertex)
    {
      aStatus = aScene.WriteLine("colorPerVertex  FALSE");
    }
    if (OK(aStatus) && !myColors.IsNull())
    {
      aStatus = aScene.WriteNode("color", myColors);
    }
    if (OK(aStatus))
    {
      aStatus = aScene.WriteArrIndex("colorIndex", myArrColorInd, myNbColors);
    }

    if (OK(aStatus) && !myTxCoords.IsNull())
    {
      aStatus = aScene.WriteNode("texCoord", myTxCoords);
    }
    if (OK(aStatus))
    {
      aStatus = aScene.WriteArrIndex("texCoordIndex", myArrTextureInd, myNbTextures);
    }

    aStatus = WriteClosing();
  }
  return aStatus;
}

//=================================================================================================

Quantity_Color VrmlData_IndexedFaceSet::GetColor(const int /*iFace*/, const int /*iVertex*/)
{
  // TODO
  return Quantity_NOC_BLACK;
}
