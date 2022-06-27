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
#include <TShort_HArray1OfShortReal.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TColStd_SequenceOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlData_IndexedFaceSet,VrmlData_Faceted)

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning (disable:4996)
#endif




//=======================================================================
//function : readData
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Faceted::readData (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus (VrmlData_EmptyData);
  Standard_Boolean aBool;
  if        (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "ccw")) {
    if (OK(aStatus, ReadBoolean (theBuffer, aBool)))
      myIsCCW = aBool;
  } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "convex")) {
    if (OK(aStatus, ReadBoolean (theBuffer, aBool)))
      myIsConvex = aBool;
  } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "solid")) {
    if (OK(aStatus, ReadBoolean (theBuffer, aBool)))
      myIsSolid = aBool;
  } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "creaseAngle")) {
    Standard_Real anAngle;
    if (OK(aStatus, Scene().ReadReal (theBuffer, anAngle,
                                      Standard_False, Standard_False))) {
      if (anAngle < -Precision::Confusion()*0.001)
        aStatus = VrmlData_IrrelevantNumber;
      else
        myCreaseAngle = anAngle;
    }
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_IndexedFaceSet::TShape
//purpose  : 
//=======================================================================

const Handle(TopoDS_TShape)& VrmlData_IndexedFaceSet::TShape ()
{
  if (myNbPolygons == 0)
  {
    myTShape.Nullify();
    return myTShape;
  }
  else if (!myIsModified) {
    return myTShape;
  }

  // list of nodes:
  const gp_XYZ * arrNodes = myCoords->Values();
  const int nNodes = (int)myCoords->Length();

  NCollection_Map <int> mapNodeId;
  NCollection_Map <int> mapPolyId;
  NCollection_List<TColStd_SequenceOfInteger> aPolygons;
  NCollection_List<gp_Dir> aNorms;
  Standard_Integer i = 0;
  for (; i < (int)myNbPolygons; i++)
  {
    const Standard_Integer * arrIndice = myArrPolygons[i];
    Standard_Integer nn = arrIndice[0];
    if (nn < 3)
    {
      // bad polygon
      continue;
    }
    TColStd_SequenceOfInteger aPolygon;
    int in = 1;
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
      gp_XYZ S = aV1.Crossed(aV2);
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

  const Standard_Integer nbNodes(mapNodeId.Extent());
  if (!nbNodes)
  {
    myIsModified = Standard_False;
    myTShape.Nullify();
    return myTShape;
  }
  // prepare vector of nodes
  NCollection_Vector<gp_XYZ> aNodes;
  NCollection_DataMap <int, int> mapIdId;
  for (i = 0; i < nNodes; i++)
  {
    if(mapNodeId.Contains(i))
    {
      const gp_XYZ& aN1 = arrNodes[i];
      mapIdId.Bind(i, aNodes.Length());
      aNodes.Append(aN1);
    }
  }
  // update polygon indices
  NCollection_List<TColStd_SequenceOfInteger>::Iterator itP(aPolygons);
  for (; itP.More(); itP.Next())
  {
    TColStd_SequenceOfInteger& aPolygon = itP.ChangeValue();
    for (int in = 1; in <= aPolygon.Length(); in++)
    {
      Standard_Integer newIdx = mapIdId.Find(aPolygon.Value(in));
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
      NCollection_List<TColStd_SequenceOfInteger> aPList;
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
  Handle(Poly_Triangulation) aTriangulation =
    new Poly_Triangulation(aNodes.Length(), aTriangles.Extent(), Standard_False);
  // Copy the triangulation vertices
  for (i = 0; i < aNodes.Length(); i++)
  {
    aTriangulation->SetNode (i + 1, gp_Pnt (aNodes (i)));
  }
  // Copy the triangles.
  NCollection_List<Poly_Triangle>::Iterator itT(aTriangles);
  for (i = 1; itT.More(); itT.Next(), i++)
  {
    aTriangulation->SetTriangle (i, itT.Value());
  }

  Handle(BRep_TFace) aFace = new BRep_TFace();
  aFace->Triangulation(aTriangulation);
  myTShape = aFace;

  // Normals should be defined; if they are not, compute them
  if (myNormals.IsNull()) {
    Poly::ComputeNormals(aTriangulation);
  }
  else
  {
    // Copy the normals. Currently only normals-per-vertex are supported.
    if (myNormalPerVertex)
    {
      aTriangulation->AddNormals();
      if (myArrNormalInd == 0L)
      {
        for (i = 0; i < nbNodes; i++)
        {
          const gp_XYZ& aNormal = myNormals->Normal (i);
          aTriangulation->SetNormal (i + 1, aNormal);
        }
      }
      else
      {
        for (i = 0; i < (int)myNbPolygons; i++)
        {
          if(mapPolyId.Contains(i)) // check to avoid previously skipped faces
          {
            const Standard_Integer * anArrNodes;
            Polygon(i, anArrNodes);
            const Standard_Integer * arrIndice;
            int nbn = IndiceNormals(i, arrIndice);
            for (Standard_Integer j = 0; j < nbn; j++)
            {
              const gp_XYZ& aNormal = myNormals->Normal(arrIndice[j]);
              aTriangulation->SetNormal (mapIdId (anArrNodes[j]) + 1, aNormal);
            }
          }
        }
      }
    }
    else {
      //TODO ..
    }
  }

  myIsModified = Standard_False;

  return myTShape;
}

//=======================================================================
//function : VrmlData_IndexedFaceSet::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_IndexedFaceSet::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_IndexedFaceSet) aResult =
    Handle(VrmlData_IndexedFaceSet)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult =
      new VrmlData_IndexedFaceSet(theOther.IsNull()? Scene(): theOther->Scene(),
                                  Name());

  if (&aResult->Scene() == &Scene()) {
    aResult->SetCoordinates     (myCoords);
    aResult->SetNormals         (myNormals);
    aResult->SetColors          (myColors);
    aResult->SetPolygons        (myNbPolygons, myArrPolygons);
    aResult->SetNormalInd       (myNbNormals, myArrNormalInd);
    aResult->SetColorInd        (myNbColors, myArrColorInd);
    aResult->SetTextureCoordInd (myNbTextures, myArrTextureInd);
  } else {
    // Create a dummy node to pass the different Scene instance to methods Clone
    const Handle(VrmlData_UnknownNode) aDummyNode =
      new VrmlData_UnknownNode (aResult->Scene());
    if (myCoords.IsNull() == Standard_False)
      aResult->SetCoordinates (Handle(VrmlData_Coordinate)::DownCast
                               (myCoords->Clone (aDummyNode)));
    if (myNormals.IsNull() == Standard_False)
      aResult->SetNormals (Handle(VrmlData_Normal)::DownCast
                           (myNormals->Clone (aDummyNode)));
    if (myColors.IsNull() == Standard_False)
      aResult->SetColors (Handle(VrmlData_Color)::DownCast
                          (myColors->Clone (aDummyNode)));
    //TODO: Replace the following lines with the relevant copying
    aResult->SetPolygons        (myNbPolygons, myArrPolygons);
    aResult->SetNormalInd       (myNbNormals, myArrNormalInd);
    aResult->SetColorInd        (myNbColors, myArrColorInd);
    aResult->SetTextureCoordInd (myNbTextures, myArrTextureInd);
  }
  aResult->SetNormalPerVertex (myNormalPerVertex);
  aResult->SetColorPerVertex  (myColorPerVertex);
  return aResult;
}

//=======================================================================
//function : VrmlData_IndexedFaceSet::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_IndexedFaceSet::Read(VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  const VrmlData_Scene& aScene = Scene();
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (OK(aStatus, VrmlData_Faceted::readData (theBuffer)))
      continue;
    if (aStatus != VrmlData_EmptyData)
      break;
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "colorPerVertex"))
      aStatus = ReadBoolean (theBuffer, myColorPerVertex);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "normalPerVertex"))
      aStatus = ReadBoolean (theBuffer, myNormalPerVertex);
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "coordIndex"))
    {
      aStatus = aScene.ReadArrIndex(theBuffer, myArrPolygons, myNbPolygons);
      //for (int i = 0; i < myNbPolygons; i++)
      //{
      //  const Standard_Integer * anArray = myArrPolygons[i];
      //  Standard_Integer nbPoints = anArray[0];
      //  std::cout << "i = " << i << "  indexes:";
      //  for (int ip = 1; ip <= nbPoints; ip++)
      //  {
      //    std::cout << " " << anArray[ip];
      //  }
      //  std::cout << std::endl;
      //}
    }
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "colorIndex"))
      aStatus = aScene.ReadArrIndex (theBuffer, myArrColorInd, myNbColors);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "normalIndex"))
      aStatus = aScene.ReadArrIndex (theBuffer, myArrNormalInd, myNbNormals);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "texCoordIndex"))
      aStatus = aScene.ReadArrIndex (theBuffer, myArrTextureInd, myNbTextures);
    // These four checks should be the last one to avoid their interference
    // with the other tokens (e.g., coordIndex)
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "texCoord"))
    {
      Handle(VrmlData_Node) aNode;
      aStatus = ReadNode (theBuffer, aNode,
                          STANDARD_TYPE(VrmlData_TextureCoordinate));
      myTxCoords = Handle(VrmlData_TextureCoordinate)::DownCast (aNode);
    }
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "color"))
    {
      Handle(VrmlData_Node) aNode;
      aStatus = ReadNode (theBuffer, aNode,
                          STANDARD_TYPE(VrmlData_Color));
      myColors = Handle(VrmlData_Color)::DownCast (aNode);
    }
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "coord"))
    {
      Handle(VrmlData_Node) aNode;
      aStatus = ReadNode (theBuffer, aNode,
                          STANDARD_TYPE(VrmlData_Coordinate));
      myCoords = Handle(VrmlData_Coordinate)::DownCast (aNode);
    }
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "normal"))
    {
      Handle(VrmlData_Node) aNode;
      aStatus = ReadNode (theBuffer, aNode,
                          STANDARD_TYPE(VrmlData_Normal));
      myNormals = Handle(VrmlData_Normal)::DownCast (aNode);
    }
    if (!OK(aStatus))
      break;
  }
  // Read the terminating (closing) brace
  if (OK(aStatus) || aStatus == VrmlData_EmptyData)
    if (OK(aStatus, readBrace (theBuffer))) {
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
//   Standard_Integer aLevelCounter (0);
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

//=======================================================================
//function : IsDefault
//purpose  : 
//=======================================================================

Standard_Boolean VrmlData_IndexedFaceSet::IsDefault () const
{
  Standard_Boolean aResult (Standard_True);
  if (myNbPolygons)
    aResult = Standard_False;
  else if (myCoords.IsNull() == Standard_False)
    aResult = myCoords->IsDefault();
  return aResult;
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_IndexedFaceSet::Write
                                                (const char * thePrefix) const
{
  static char header[] = "IndexedFaceSet {";
  const VrmlData_Scene& aScene = Scene();
  VrmlData_ErrorStatus aStatus;
  if (OK (aStatus, aScene.WriteLine (thePrefix, header, GlobalIndent()))) {

    // Write the attributes of interface "VrmlData_Faceted"
    if (IsCCW() == Standard_False)
      aStatus = aScene.WriteLine ("ccw         FALSE");
    if (OK(aStatus) && IsSolid() == Standard_False)
      aStatus = aScene.WriteLine ("solid       FALSE");
    if (OK(aStatus) && IsConvex() == Standard_False)
      aStatus = aScene.WriteLine ("convex      FALSE");
    if (OK(aStatus) && CreaseAngle() > Precision::Confusion()) {
      char buf[64];
      Sprintf (buf, "%.9g", CreaseAngle());
      aStatus = aScene.WriteLine ("creaseAngle", buf);
    }

    if (OK(aStatus) && myCoords.IsNull() == Standard_False)
      aStatus = aScene.WriteNode ("coord", myCoords);
    if (OK(aStatus))
      aStatus = aScene.WriteArrIndex ("coordIndex", myArrPolygons,myNbPolygons);

    if (OK(aStatus) && myNormalPerVertex == Standard_False)
      aStatus = aScene.WriteLine ("normalPerVertex FALSE");
    if (OK(aStatus) && myNormals.IsNull() == Standard_False)
      aStatus = aScene.WriteNode ("normal", myNormals);
    if (OK(aStatus))
      aStatus = aScene.WriteArrIndex ("normalIndex",myArrNormalInd,myNbNormals);

    if (OK(aStatus) && myColorPerVertex == Standard_False)
      aStatus = aScene.WriteLine ("colorPerVertex  FALSE");
    if (OK(aStatus) && myColors.IsNull() == Standard_False)
      aStatus = aScene.WriteNode ("color", myColors);
    if (OK(aStatus))
      aStatus = aScene.WriteArrIndex ("colorIndex", myArrColorInd, myNbColors);

    if (OK(aStatus) && myTxCoords.IsNull() == Standard_False)
      aStatus = aScene.WriteNode ("texCoord", myTxCoords);
    if (OK(aStatus))
      aStatus = aScene.WriteArrIndex ("texCoordIndex", myArrTextureInd,
                                      myNbTextures);

    aStatus = WriteClosing();
  }
  return aStatus;
}

//=======================================================================
//function : GetColor
//purpose  : 
//=======================================================================

Quantity_Color VrmlData_IndexedFaceSet::GetColor
                                        (const Standard_Integer /*iFace*/,
                                         const Standard_Integer /*iVertex*/)
{
  //TODO
  return Quantity_NOC_BLACK;
}

