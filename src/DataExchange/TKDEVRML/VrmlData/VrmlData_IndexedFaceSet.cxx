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
#include <NCollection_Vector.hxx>
#include <NCollection_DataMap.hxx>
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
      myIsCCW = aBool;
  }
  else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "convex"))
  {
    if (OK(aStatus, ReadBoolean(theBuffer, aBool)))
      myIsConvex = aBool;
  }
  else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "solid"))
  {
    if (OK(aStatus, ReadBoolean(theBuffer, aBool)))
      myIsSolid = aBool;
  }
  else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "creaseAngle"))
  {
    double anAngle;
    if (OK(aStatus, Scene().ReadReal(theBuffer, anAngle, false, false)))
    {
      if (anAngle < -Precision::Confusion() * 0.001)
        aStatus = VrmlData_IrrelevantNumber;
      else
        myCreaseAngle = anAngle;
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

  // list of nodes:
  const gp_XYZ* arrNodes = myCoords->Values();
  const int     nNodes   = (int)myCoords->Length();

  NCollection_Map<int>                        mapNodeId;
  NCollection_Map<int>                        mapPolyId;
  NCollection_List<NCollection_Sequence<int>> aPolygons;
  NCollection_List<gp_Dir>                    aNorms;
  int                                         i = 0;
  for (; i < (int)myNbPolygons; i++)
  {
    const int* arrIndice = myArrPolygons[i];
    int        nn        = arrIndice[0];
    if (nn < 3)
    {
      // bad polygon
      continue;
    }
    NCollection_Sequence<int> aPolygon;
    int                       in = 1;
    for (; in <= nn; in++)
    {
      if (arrIndice[in] > nNodes)
      {
        break;
      }
      aPolygon.Append(arrIndice[in]);
    }
    if (in <= nn)
    {
      // bad index of node in polygon
      continue;
    }
    // calculate normal
    gp_XYZ aSum;
    gp_XYZ aPrevP = arrNodes[aPolygon(1)];
    for (in = 2; in < aPolygon.Length(); in++)
    {
      gp_XYZ aP1 = arrNodes[aPolygon(in)];
      gp_XYZ aP2 = arrNodes[aPolygon(in + 1)];
      gp_XYZ aV1 = aP1 - aPrevP;
      gp_XYZ aV2 = aP2 - aPrevP;
      gp_XYZ S   = aV1.Crossed(aV2);
      aSum += S;
    }
    if (aSum.Modulus() < Precision::Confusion())
    {
      // degenerate polygon
      continue;
    }
    gp_Dir aNormal(aSum);
    mapPolyId.Add(i);
    aPolygons.Append(aPolygon);
    aNorms.Append(aNormal);
    // collect info about used indices
    for (in = 1; in <= aPolygon.Length(); in++)
    {
      mapNodeId.Add(arrIndice[in]);
    }
  }

  const int nbNodes(mapNodeId.Extent());
  if (!nbNodes)
  {
    myIsModified = false;
    myTShape.Nullify();
    return myTShape;
  }
  // prepare vector of nodes
  NCollection_Vector<gp_XYZ>    aNodes;
  NCollection_DataMap<int, int> mapIdId;
  for (i = 0; i < nNodes; i++)
  {
    if (mapNodeId.Contains(i))
    {
      const gp_XYZ& aN1 = arrNodes[i];
      mapIdId.Bind(i, aNodes.Length());
      aNodes.Append(aN1);
    }
  }
  // update polygon indices
  NCollection_List<NCollection_Sequence<int>>::Iterator itP(aPolygons);
  for (; itP.More(); itP.Next())
  {
    NCollection_Sequence<int>& aPolygon = itP.ChangeValue();
    for (int in = 1; in <= aPolygon.Length(); in++)
    {
      int newIdx               = mapIdId.Find(aPolygon.Value(in));
      aPolygon.ChangeValue(in) = newIdx;
    }
  }
  // calculate triangles
  NCollection_List<Poly_Triangle> aTriangles;
  itP.Init(aPolygons);
  for (NCollection_List<gp_Dir>::Iterator itN(aNorms); itP.More(); itP.Next(), itN.Next())
  {
    NCollection_List<Poly_Triangle> aTrias;
    try
    {
      NCollection_List<NCollection_Sequence<int>> aPList;
      aPList.Append(itP.Value());
      BRepMesh_Triangulator aTriangulator(aNodes, aPList, itN.Value());
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
  for (i = 0; i < aNodes.Length(); i++)
  {
    aTriangulation->SetNode(i + 1, gp_Pnt(aNodes(i)));
  }
  // Copy the triangles.
  NCollection_List<Poly_Triangle>::Iterator itT(aTriangles);
  for (i = 1; itT.More(); itT.Next(), i++)
  {
    aTriangulation->SetTriangle(i, itT.Value());
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
      aTriangulation->AddNormals();
      if (myArrNormalInd == nullptr)
      {
        for (i = 0; i < nbNodes; i++)
        {
          const gp_XYZ& aNormal = myNormals->Normal(i);
          aTriangulation->SetNormal(i + 1, aNormal);
        }
      }
      else
      {
        for (i = 0; i < (int)myNbPolygons; i++)
        {
          if (mapPolyId.Contains(i)) // check to avoid previously skipped faces
          {
            const int* anArrNodes;
            Polygon(i, anArrNodes);
            const int* arrIndice;
            int        nbn = IndiceNormals(i, arrIndice);
            for (int j = 0; j < nbn; j++)
            {
              const gp_XYZ& aNormal = myNormals->Normal(arrIndice[j]);
              aTriangulation->SetNormal(mapIdId(anArrNodes[j]) + 1, aNormal);
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
    aResult = new VrmlData_IndexedFaceSet(theOther.IsNull() ? Scene() : theOther->Scene(), Name());

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
    if (myCoords.IsNull() == false)
      aResult->SetCoordinates(occ::down_cast<VrmlData_Coordinate>(myCoords->Clone(aDummyNode)));
    if (myNormals.IsNull() == false)
      aResult->SetNormals(occ::down_cast<VrmlData_Normal>(myNormals->Clone(aDummyNode)));
    if (myColors.IsNull() == false)
      aResult->SetColors(occ::down_cast<VrmlData_Color>(myColors->Clone(aDummyNode)));
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
      continue;
    if (aStatus != VrmlData_EmptyData)
      break;
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "colorPerVertex"))
      aStatus = ReadBoolean(theBuffer, myColorPerVertex);
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "normalPerVertex"))
      aStatus = ReadBoolean(theBuffer, myNormalPerVertex);
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
      aStatus = aScene.ReadArrIndex(theBuffer, myArrColorInd, myNbColors);
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "normalIndex"))
      aStatus = aScene.ReadArrIndex(theBuffer, myArrNormalInd, myNbNormals);
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "texCoordIndex"))
      aStatus = aScene.ReadArrIndex(theBuffer, myArrTextureInd, myNbTextures);
    // These four checks should be the last one to avoid their interference
    // with the other tokens (e.g., coordIndex)
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
      break;
  }
  // Read the terminating (closing) brace
  if (OK(aStatus) || aStatus == VrmlData_EmptyData)
    if (OK(aStatus, readBrace(theBuffer)))
    {
      // Post-processing
      ;
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
    aResult = false;
  else if (myCoords.IsNull() == false)
    aResult = myCoords->IsDefault();
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
    if (IsCCW() == false)
      aStatus = aScene.WriteLine("ccw         FALSE");
    if (OK(aStatus) && IsSolid() == false)
      aStatus = aScene.WriteLine("solid       FALSE");
    if (OK(aStatus) && IsConvex() == false)
      aStatus = aScene.WriteLine("convex      FALSE");
    if (OK(aStatus) && CreaseAngle() > Precision::Confusion())
    {
      char buf[64];
      Sprintf(buf, "%.9g", CreaseAngle());
      aStatus = aScene.WriteLine("creaseAngle", buf);
    }

    if (OK(aStatus) && myCoords.IsNull() == false)
      aStatus = aScene.WriteNode("coord", myCoords);
    if (OK(aStatus))
      aStatus = aScene.WriteArrIndex("coordIndex", myArrPolygons, myNbPolygons);

    if (OK(aStatus) && myNormalPerVertex == false)
      aStatus = aScene.WriteLine("normalPerVertex FALSE");
    if (OK(aStatus) && myNormals.IsNull() == false)
      aStatus = aScene.WriteNode("normal", myNormals);
    if (OK(aStatus))
      aStatus = aScene.WriteArrIndex("normalIndex", myArrNormalInd, myNbNormals);

    if (OK(aStatus) && myColorPerVertex == false)
      aStatus = aScene.WriteLine("colorPerVertex  FALSE");
    if (OK(aStatus) && myColors.IsNull() == false)
      aStatus = aScene.WriteNode("color", myColors);
    if (OK(aStatus))
      aStatus = aScene.WriteArrIndex("colorIndex", myArrColorInd, myNbColors);

    if (OK(aStatus) && myTxCoords.IsNull() == false)
      aStatus = aScene.WriteNode("texCoord", myTxCoords);
    if (OK(aStatus))
      aStatus = aScene.WriteArrIndex("texCoordIndex", myArrTextureInd, myNbTextures);

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
