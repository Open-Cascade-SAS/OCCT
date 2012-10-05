// Created on: 2006-11-04
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <VrmlData_IndexedFaceSet.hxx>
#include <VrmlData_InBuffer.hxx>
#include <VrmlData_UnknownNode.hxx>
#include <Poly_Triangulation.hxx>
#include <BRep_TFace.hxx>
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

#ifdef WNT
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning (disable:4996)
#endif

IMPLEMENT_STANDARD_HANDLE  (VrmlData_Faceted, VrmlData_Geometry)
IMPLEMENT_STANDARD_RTTIEXT (VrmlData_Faceted, VrmlData_Geometry)
IMPLEMENT_STANDARD_HANDLE  (VrmlData_IndexedFaceSet, VrmlData_Faceted)
IMPLEMENT_STANDARD_RTTIEXT (VrmlData_IndexedFaceSet, VrmlData_Faceted)

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
                                      Standard_False, Standard_False)))
      if (anAngle < -Precision::Confusion()*0.001)
        aStatus = VrmlData_IrrelevantNumber;
      else
        myCreaseAngle = anAngle;
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
    myTShape.Nullify();
  else if (myIsModified) {
    // Create an empty topological Face
    const gp_XYZ * arrNodes = myCoords->Values();
    Standard_Integer i, nTri(0);

    NCollection_DataMap <int, int> mapNodeId;

    // Count non-degenerated triangles
    for (i = 0; i < (int)myNbPolygons; i++) {
      const Standard_Integer * arrIndice;
      if (Polygon(i, arrIndice) == 3) {
        if (arrIndice[0] < 0)
          continue;
        const gp_XYZ aVec[2] = {
          arrNodes[arrIndice[1]] - arrNodes[arrIndice[0]],
          arrNodes[arrIndice[2]] - arrNodes[arrIndice[0]]
        };
        if ((aVec[0] ^ aVec[1]).SquareModulus() >
            Precision::SquareConfusion())
          ++nTri;
        else {
          const_cast<Standard_Integer&> (arrIndice[0]) = -1;
          continue;
        }
      }
      if (mapNodeId.IsBound (arrIndice[0]) == Standard_False)
        mapNodeId.Bind (arrIndice[0], 0);
      if (mapNodeId.IsBound (arrIndice[1]) == Standard_False)
        mapNodeId.Bind (arrIndice[1], 0);
      if (mapNodeId.IsBound (arrIndice[2]) == Standard_False)
        mapNodeId.Bind (arrIndice[2], 0);
    }
    const Standard_Integer nbNodes (mapNodeId.Extent());
    if (!nbNodes)
    {
        myIsModified = Standard_False;
        myTShape.Nullify();
        return myTShape;
    }

    Handle(Poly_Triangulation) aTriangulation =
      new Poly_Triangulation (nbNodes, nTri, Standard_False);
    Handle(BRep_TFace) aFace = new BRep_TFace();
    aFace->Triangulation (aTriangulation);
    myTShape = aFace;

    // Copy the triangulation vertices
    TColgp_Array1OfPnt& aNodes = aTriangulation->ChangeNodes();
    NCollection_DataMap <int, int>::Iterator anIterN(mapNodeId);
    for (i = 1; anIterN.More(); anIterN.Next()) {
      const int aKey = anIterN.Key();
      const gp_XYZ& aNodePnt = arrNodes[aKey];
      aNodes(i) = gp_Pnt (aNodePnt);
      anIterN.ChangeValue() = i++;
    }

    // Copy the triangles. Only the triangle-type polygons are supported.
    // In this loop we also get rid of any possible degenerated triangles.
    Poly_Array1OfTriangle& aTriangles = aTriangulation->ChangeTriangles();
    nTri = 0;
    for (i = 0; i < (int)myNbPolygons; i++) {
      const Standard_Integer * arrIndice;
      if (Polygon (i, arrIndice) == 3)
        if (arrIndice[0] >= 0)  // check to avoid previously skipped faces
          aTriangles(++nTri).Set (mapNodeId(arrIndice[0]),
                                  mapNodeId(arrIndice[1]),
                                  mapNodeId(arrIndice[2]));
    }

    // Normals should be defined; if they are not, compute them
    if (myNormals.IsNull ()) {
      //aTriangulation->ComputeNormals();
      Poly::ComputeNormals(aTriangulation);
    }
    else {
      // Copy the normals. Currently only normals-per-vertex are supported.
      Handle(TShort_HArray1OfShortReal) Normals =
        new TShort_HArray1OfShortReal (1, 3*nbNodes);
      if (myNormalPerVertex) {
        if (myArrNormalInd == 0L) {
          NCollection_DataMap <int, int>::Iterator anIterNN (mapNodeId);
          for (; anIterNN.More (); anIterNN.Next ()) {
            Standard_Integer anIdx = (anIterNN.Value() - 1) * 3 + 1;
            const gp_XYZ& aNormal = myNormals->Normal (anIterNN.Key ());
            Normals->SetValue (anIdx + 0, Standard_ShortReal (aNormal.X ()));
            Normals->SetValue (anIdx + 1, Standard_ShortReal (aNormal.Y ()));
            Normals->SetValue (anIdx + 2, Standard_ShortReal (aNormal.Z ()));
          }
        } else {
          nTri = 0;
          for (i = 0; i < (int)myNbPolygons; i++) {
            const Standard_Integer * arrIndice;
            if (Polygon(i, arrIndice) == 3)
              if (arrIndice[0] >= 0)  // check to avoid previously skipped faces
                if (IndiceNormals(i, arrIndice) == 3) {
                  Standard_Integer anInd = (++nTri - 1) * 3 + 1;
                  for (Standard_Integer j = 0; j < 3; j++) {
                    const gp_XYZ& aNormal = myNormals->Normal (arrIndice[j]);
                    Normals->SetValue (anInd + 0 + j*3,
                                       Standard_ShortReal (aNormal.X ()));
                    Normals->SetValue (anInd + 1 + j*3,
                                       Standard_ShortReal (aNormal.Y ()));
                    Normals->SetValue (anInd + 2 + j*3,
                                       Standard_ShortReal (aNormal.Z ()));
                  }
                }
          }
        }
      } else {
        //TODO ..
      }
      aTriangulation->SetNormals(Normals);
    }

    myIsModified = Standard_False;
  }
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
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "coordIndex"))
      aStatus = aScene.ReadArrIndex (theBuffer, myArrPolygons, myNbPolygons);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "colorIndex"))
      aStatus = aScene.ReadArrIndex (theBuffer, myArrColorInd, myNbColors);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "normalIndex"))
      aStatus = aScene.ReadArrIndex (theBuffer, myArrNormalInd, myNbNormals);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "texCoordIndex"))
      aStatus = aScene.ReadArrIndex (theBuffer, myArrTextureInd, myNbTextures);
    // These four checks should be the last one to avoid their interference
    // with the other tokens (e.g., coordIndex)
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "texCoord"))
      aStatus = ReadNode (theBuffer, myTxCoords,
                          STANDARD_TYPE(VrmlData_TextureCoordinate));
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "color"))
      aStatus = ReadNode (theBuffer, myColors,
                          STANDARD_TYPE(VrmlData_Color));
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "coord"))
      aStatus = ReadNode (theBuffer, myCoords,
                          STANDARD_TYPE(VrmlData_Coordinate));
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "normal"))
      aStatus = ReadNode (theBuffer, myNormals,
                          STANDARD_TYPE(VrmlData_Normal));
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
      sprintf (buf, "%.9g", CreaseAngle());
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

