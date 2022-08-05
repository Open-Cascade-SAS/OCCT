// Created on: 2014-10-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <BRepExtrema_TriangleSet.hxx>

#include <BRep_Tool.hxx>
#include <BVH_LinearBuilder.hxx>
#include <Poly_Triangulation.hxx>
#include <TopoDS.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepExtrema_TriangleSet, BVH_PrimitiveSet3d)

//=======================================================================
//function : BRepExtrema_TriangleSet
//purpose  : Creates empty triangle set
//=======================================================================
BRepExtrema_TriangleSet::BRepExtrema_TriangleSet()
{
  // Set default builder - linear BVH (LBVH)
  myBuilder = new BVH_LinearBuilder<Standard_Real, 3> (BVH_Constants_LeafNodeSizeDefault, BVH_Constants_MaxTreeDepth);
}

//=======================================================================
//function : BRepExtrema_TriangleSet
//purpose  : Creates triangle set from the given face
//=======================================================================
BRepExtrema_TriangleSet::BRepExtrema_TriangleSet (const BRepExtrema_ShapeList& theFaces)
{
  // Set default builder - linear BVH (LBVH)
  myBuilder = new BVH_LinearBuilder<Standard_Real, 3> (BVH_Constants_LeafNodeSizeDefault, BVH_Constants_MaxTreeDepth);

  Init (theFaces);
}

//=======================================================================
//function : ~BRepExtrema_TriangleSet
//purpose  : Releases resources of triangle set
//=======================================================================
BRepExtrema_TriangleSet::~BRepExtrema_TriangleSet()
{
  //
}

//=======================================================================
//function : Size
//purpose  : Returns total number of triangles
//=======================================================================
Standard_Integer BRepExtrema_TriangleSet::Size() const
{
  return static_cast<Standard_Integer> (myTriangles.size());
}

//=======================================================================
//function : Box
//purpose  : Returns AABB of the given triangle
//=======================================================================
BVH_Box<Standard_Real, 3> BRepExtrema_TriangleSet::Box (const Standard_Integer theIndex) const
{
  const BVH_Vec4i& aTriangle = myTriangles[theIndex];

  BVH_Vec3d aMinPnt = myVertexArray[aTriangle.x()].cwiseMin (
    myVertexArray[aTriangle.y()].cwiseMin (myVertexArray[aTriangle.z()]));

  BVH_Vec3d aMaxPnt = myVertexArray[aTriangle.x()].cwiseMax (
    myVertexArray[aTriangle.y()].cwiseMax (myVertexArray[aTriangle.z()]));

  return BVH_Box<Standard_Real, 3> (aMinPnt, aMaxPnt);
}

//=======================================================================
//function : Center
//purpose  : Returns centroid position along specified axis
//=======================================================================
Standard_Real BRepExtrema_TriangleSet::Center (const Standard_Integer theIndex, const Standard_Integer theAxis) const
{
  const BVH_Vec4i& aTriangle = myTriangles[theIndex];

  if (theAxis == 0)
  {
    return (1.0 / 3.0) * (myVertexArray[aTriangle.x()].x() +
                          myVertexArray[aTriangle.y()].x() +
                          myVertexArray[aTriangle.z()].x());
  }
  else if (theAxis == 1)
  {
    return (1.0 / 3.0) * (myVertexArray[aTriangle.x()].y() +
                          myVertexArray[aTriangle.y()].y() +
                          myVertexArray[aTriangle.z()].y());
  }
  else
  {
    return (1.0 / 3.0) * (myVertexArray[aTriangle.x()].z() +
                          myVertexArray[aTriangle.y()].z() +
                          myVertexArray[aTriangle.z()].z());
  }
}

//=======================================================================
//function : Swap
//purpose  : Swaps indices of two specified triangles
//=======================================================================
void BRepExtrema_TriangleSet::Swap (const Standard_Integer theIndex1, const Standard_Integer theIndex2)
{
  std::swap (myTriangles[theIndex1],
             myTriangles[theIndex2]);

  std::swap (myTrgIdxMap (theIndex1),
             myTrgIdxMap (theIndex2));
}

//=======================================================================
//function : GetFaceID
//purpose  : Returns face ID of the given vertex
//=======================================================================
Standard_Integer BRepExtrema_TriangleSet::GetFaceID (const Standard_Integer theIndex) const
{
  return myTriangles[theIndex].w();
}

//=======================================================================
//function : GetShapeIDOfVtx
//purpose  : Returns shape ID of the given vertex index
//=======================================================================
Standard_Integer BRepExtrema_TriangleSet::GetShapeIDOfVtx (const Standard_Integer theIndex) const
{
  return myShapeIdxOfVtxVec.Value (theIndex);
}

//=======================================================================
//function : GetVtxIdxInShape
//purpose  : Returns vertex index in tringulation of the shape, which vertex belongs,
//           with the given vtx ID in whole set
//=======================================================================
Standard_Integer BRepExtrema_TriangleSet::GetVtxIdxInShape (const Standard_Integer theIndex) const
{
  Standard_Integer aShID = myShapeIdxOfVtxVec.Value (theIndex);
  Standard_Integer aNumVertices = 0;

  for (Standard_Integer aSIdx = 0; aSIdx < aShID; aSIdx++)
  {
    aNumVertices += myNumVtxInShapeVec.Value (aSIdx);
  }

  return theIndex - aNumVertices;
}

//=======================================================================
//function : GetTrgIdxInShape
//purpose  :  Returns triangle index (before swapping) in tringulation of the shape, which triangle belongs,
//            with the given trg ID in whole set (after swapping)
//=======================================================================
Standard_Integer BRepExtrema_TriangleSet::GetTrgIdxInShape (const Standard_Integer theIndex) const
{
  Standard_Integer aShID = GetFaceID (theIndex);
  Standard_Integer aNumTriangles = 0;

  for (Standard_Integer aSIdx = 0; aSIdx < aShID; aSIdx++)
  {
    aNumTriangles += myNumTrgInShapeVec.Value (aSIdx);
  }

  return myTrgIdxMap (theIndex) - aNumTriangles;
}

//=======================================================================
//function : GetVertices
//purpose  : Returns vertices of the given triangle
//=======================================================================
void BRepExtrema_TriangleSet::GetVertices (const Standard_Integer theIndex,
                                           BVH_Vec3d&             theVertex1,
                                           BVH_Vec3d&             theVertex2,
                                           BVH_Vec3d&             theVertex3) const
{
  BVH_Vec4i aTriangle = myTriangles[theIndex];

  theVertex1 = myVertexArray[aTriangle.x()];
  theVertex2 = myVertexArray[aTriangle.y()];
  theVertex3 = myVertexArray[aTriangle.z()];
}

//=======================================================================
//function : GetVertices
//purpose  : Returns vertices of the given triangle
//=======================================================================
void BRepExtrema_TriangleSet::GetVtxIndices (const Standard_Integer theIndex,
                                             NCollection_Array1<Standard_Integer>& theVtxIndices) const
{
  BVH_Vec4i aTriangle = myTriangles[theIndex];

  theVtxIndices = NCollection_Array1<Standard_Integer> (0, 2);
  theVtxIndices.SetValue (0, aTriangle.x());
  theVtxIndices.SetValue (1, aTriangle.y());
  theVtxIndices.SetValue (2, aTriangle.z());
}

//=======================================================================
//function : Clear
//purpose  : Clears triangle set data
//=======================================================================
void BRepExtrema_TriangleSet::Clear()
{
  BVH_Array4i anEmptyTriangles;
  myTriangles.swap (anEmptyTriangles);

  BVH_Array3d anEmptyVertexArray;
  myVertexArray.swap (anEmptyVertexArray);
}

//=======================================================================
//function : Init
//purpose  : Initializes triangle set
//=======================================================================
Standard_Boolean BRepExtrema_TriangleSet::Init (const BRepExtrema_ShapeList& theShapes)
{
  Clear();

  Standard_Boolean isOK = Standard_True;
  for (Standard_Integer aShapeIdx = 0; aShapeIdx < theShapes.Size() && isOK; ++aShapeIdx)
  {
    if (theShapes (aShapeIdx).ShapeType() == TopAbs_FACE)
      isOK = initFace (TopoDS::Face (theShapes(aShapeIdx)), aShapeIdx);
    else if (theShapes (aShapeIdx).ShapeType() == TopAbs_EDGE)
      isOK = initEdge (TopoDS::Edge (theShapes(aShapeIdx)), aShapeIdx);
  }

  Standard_Integer aNumTrg = static_cast<Standard_Integer> (myTriangles.size());
  myTrgIdxMap.Clear();
  myTrgIdxMap.ReSize (aNumTrg);

  for (Standard_Integer aTrgIdx = 0; aTrgIdx < aNumTrg; ++aTrgIdx)
  {
    myTrgIdxMap.Bind (aTrgIdx, aTrgIdx);
  }

  MarkDirty(); // needs BVH rebuilding

  Standard_ASSERT_RETURN (!BVH().IsNull(),
    "Error: Failed to build BVH for primitive set", Standard_False);

  return Standard_True;
}

//=======================================================================
//function : initFace
//purpose  : Initializes triangle set
//=======================================================================
Standard_Boolean BRepExtrema_TriangleSet::initFace (const TopoDS_Face& theFace, const Standard_Integer theIndex)
{
  TopLoc_Location aLocation;

  Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation (theFace, aLocation);
  if (aTriangulation.IsNull())
  {
    return Standard_False;
  }

  const Standard_Integer aVertOffset =
    static_cast<Standard_Integer> (myVertexArray.size()) - 1;

  initNodes (aTriangulation->MapNodeArray()->ChangeArray1(), aLocation.Transformation(), theIndex);

  for (Standard_Integer aTriIdx = 1; aTriIdx <= aTriangulation->NbTriangles(); ++aTriIdx)
  {
    Standard_Integer aVertex1;
    Standard_Integer aVertex2;
    Standard_Integer aVertex3;

    aTriangulation->Triangle (aTriIdx).Get (aVertex1,
                                            aVertex2,
                                            aVertex3);

    myTriangles.push_back (BVH_Vec4i (aVertex1 + aVertOffset,
                                      aVertex2 + aVertOffset,
                                      aVertex3 + aVertOffset,
                                      theIndex));
  }

  myNumTrgInShapeVec.SetValue (theIndex, aTriangulation->NbTriangles());

  return Standard_True;
}

//=======================================================================
//function : initEdge
//purpose  : Initializes triangle set
//=======================================================================
Standard_Boolean BRepExtrema_TriangleSet::initEdge (const TopoDS_Edge& theEdge, const Standard_Integer theIndex)
{
  TopLoc_Location aLocation;

  Handle(Poly_Polygon3D) aPolygon = BRep_Tool::Polygon3D (theEdge, aLocation);
  if (aPolygon.IsNull())
  {
    return Standard_False;
  }

  const Standard_Integer aVertOffset =
    static_cast<Standard_Integer> (myVertexArray.size()) - 1;

  initNodes (aPolygon->Nodes(), aLocation.Transformation(), theIndex);

  for (Standard_Integer aVertIdx = 1; aVertIdx < aPolygon->NbNodes(); ++aVertIdx)
  {
    // segment as degenerate triangle
    myTriangles.push_back (BVH_Vec4i (aVertIdx + aVertOffset,
                                      aVertIdx + aVertOffset + 1,
                                      aVertIdx + aVertOffset + 1,
                                      theIndex));
  }
  return Standard_True;
}

//=======================================================================
//function : initNodes
//purpose  : Initializes nodes
//=======================================================================
void BRepExtrema_TriangleSet::initNodes (const TColgp_Array1OfPnt& theNodes,
                                         const gp_Trsf& theTrsf,
                                         const Standard_Integer theIndex)
{
  for (Standard_Integer aVertIdx = 1; aVertIdx <= theNodes.Size(); ++aVertIdx)
  {
    gp_Pnt aVertex = theNodes.Value (aVertIdx);

    aVertex.Transform (theTrsf);

    myVertexArray.push_back (BVH_Vec3d (aVertex.X(),
                                        aVertex.Y(),
                                        aVertex.Z()));
    myShapeIdxOfVtxVec.Append (theIndex);
  }

  myNumVtxInShapeVec.SetValue (theIndex, theNodes.Size());
}
