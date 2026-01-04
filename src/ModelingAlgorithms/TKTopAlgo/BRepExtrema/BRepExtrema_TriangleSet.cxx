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
// function : BRepExtrema_TriangleSet
// purpose  : Creates empty triangle set
//=======================================================================
BRepExtrema_TriangleSet::BRepExtrema_TriangleSet()
{
  // Set default builder - linear BVH (LBVH)
  myBuilder =
    new BVH_LinearBuilder<double, 3>(BVH_Constants_LeafNodeSizeDefault, BVH_Constants_MaxTreeDepth);
}

//=======================================================================
// function : BRepExtrema_TriangleSet
// purpose  : Creates triangle set from the given face
//=======================================================================
BRepExtrema_TriangleSet::BRepExtrema_TriangleSet(const NCollection_Vector<TopoDS_Shape>& theFaces)
{
  // Set default builder - linear BVH (LBVH)
  myBuilder =
    new BVH_LinearBuilder<double, 3>(BVH_Constants_LeafNodeSizeDefault, BVH_Constants_MaxTreeDepth);

  Init(theFaces);
}

//=======================================================================
// function : ~BRepExtrema_TriangleSet
// purpose  : Releases resources of triangle set
//=======================================================================
BRepExtrema_TriangleSet::~BRepExtrema_TriangleSet() = default;

//=======================================================================
// function : Size
// purpose  : Returns total number of triangles
//=======================================================================
int BRepExtrema_TriangleSet::Size() const
{
  return static_cast<int>(myTriangles.size());
}

//=======================================================================
// function : Box
// purpose  : Returns AABB of the given triangle
//=======================================================================
BVH_Box<double, 3> BRepExtrema_TriangleSet::Box(const int theIndex) const
{
  const BVH_Vec4i& aTriangle = myTriangles[theIndex];

  BVH_Vec3d aMinPnt = myVertexArray[aTriangle.x()].cwiseMin(
    myVertexArray[aTriangle.y()].cwiseMin(myVertexArray[aTriangle.z()]));

  BVH_Vec3d aMaxPnt = myVertexArray[aTriangle.x()].cwiseMax(
    myVertexArray[aTriangle.y()].cwiseMax(myVertexArray[aTriangle.z()]));

  return BVH_Box<double, 3>(aMinPnt, aMaxPnt);
}

//=======================================================================
// function : Center
// purpose  : Returns centroid position along specified axis
//=======================================================================
double BRepExtrema_TriangleSet::Center(const int theIndex, const int theAxis) const
{
  const BVH_Vec4i& aTriangle = myTriangles[theIndex];

  if (theAxis == 0)
  {
    return (1.0 / 3.0)
           * (myVertexArray[aTriangle.x()].x() + myVertexArray[aTriangle.y()].x()
              + myVertexArray[aTriangle.z()].x());
  }
  else if (theAxis == 1)
  {
    return (1.0 / 3.0)
           * (myVertexArray[aTriangle.x()].y() + myVertexArray[aTriangle.y()].y()
              + myVertexArray[aTriangle.z()].y());
  }
  else
  {
    return (1.0 / 3.0)
           * (myVertexArray[aTriangle.x()].z() + myVertexArray[aTriangle.y()].z()
              + myVertexArray[aTriangle.z()].z());
  }
}

//=======================================================================
// function : Swap
// purpose  : Swaps indices of two specified triangles
//=======================================================================
void BRepExtrema_TriangleSet::Swap(const int theIndex1, const int theIndex2)
{
  std::swap(myTriangles[theIndex1], myTriangles[theIndex2]);

  std::swap(myTrgIdxMap(theIndex1), myTrgIdxMap(theIndex2));
}

//=======================================================================
// function : GetFaceID
// purpose  : Returns face ID of the given vertex
//=======================================================================
int BRepExtrema_TriangleSet::GetFaceID(const int theIndex) const
{
  return myTriangles[theIndex].w();
}

//=======================================================================
// function : GetShapeIDOfVtx
// purpose  : Returns shape ID of the given vertex index
//=======================================================================
int BRepExtrema_TriangleSet::GetShapeIDOfVtx(const int theIndex) const
{
  return myShapeIdxOfVtxVec.Value(theIndex);
}

//=======================================================================
// function : GetVtxIdxInShape
// purpose  : Returns vertex index in tringulation of the shape, which vertex belongs,
//           with the given vtx ID in whole set
//=======================================================================
int BRepExtrema_TriangleSet::GetVtxIdxInShape(const int theIndex) const
{
  int aShID        = myShapeIdxOfVtxVec.Value(theIndex);
  int aNumVertices = 0;

  for (int aSIdx = 0; aSIdx < aShID; aSIdx++)
  {
    aNumVertices += myNumVtxInShapeVec.Value(aSIdx);
  }

  return theIndex - aNumVertices;
}

//=======================================================================
// function : GetTrgIdxInShape
// purpose  :  Returns triangle index (before swapping) in tringulation of the shape, which triangle
// belongs,
//            with the given trg ID in whole set (after swapping)
//=======================================================================
int BRepExtrema_TriangleSet::GetTrgIdxInShape(const int theIndex) const
{
  int aShID         = GetFaceID(theIndex);
  int aNumTriangles = 0;

  for (int aSIdx = 0; aSIdx < aShID; aSIdx++)
  {
    aNumTriangles += myNumTrgInShapeVec.Value(aSIdx);
  }

  return myTrgIdxMap(theIndex) - aNumTriangles;
}

//=======================================================================
// function : GetVertices
// purpose  : Returns vertices of the given triangle
//=======================================================================
void BRepExtrema_TriangleSet::GetVertices(const int  theIndex,
                                          BVH_Vec3d& theVertex1,
                                          BVH_Vec3d& theVertex2,
                                          BVH_Vec3d& theVertex3) const
{
  BVH_Vec4i aTriangle = myTriangles[theIndex];

  theVertex1 = myVertexArray[aTriangle.x()];
  theVertex2 = myVertexArray[aTriangle.y()];
  theVertex3 = myVertexArray[aTriangle.z()];
}

//=======================================================================
// function : GetVertices
// purpose  : Returns vertices of the given triangle
//=======================================================================
void BRepExtrema_TriangleSet::GetVtxIndices(const int                theIndex,
                                            NCollection_Array1<int>& theVtxIndices) const
{
  BVH_Vec4i aTriangle = myTriangles[theIndex];

  theVtxIndices = NCollection_Array1<int>(0, 2);
  theVtxIndices.SetValue(0, aTriangle.x());
  theVtxIndices.SetValue(1, aTriangle.y());
  theVtxIndices.SetValue(2, aTriangle.z());
}

//=======================================================================
// function : Clear
// purpose  : Clears triangle set data
//=======================================================================
void BRepExtrema_TriangleSet::Clear()
{
  BVH_Array4i anEmptyTriangles;
  myTriangles.swap(anEmptyTriangles);

  BVH_Array3d anEmptyVertexArray;
  myVertexArray.swap(anEmptyVertexArray);
}

//=======================================================================
// function : Init
// purpose  : Initializes triangle set
//=======================================================================
bool BRepExtrema_TriangleSet::Init(const NCollection_Vector<TopoDS_Shape>& theShapes)
{
  Clear();

  bool isOK = true;
  for (int aShapeIdx = 0; aShapeIdx < theShapes.Size() && isOK; ++aShapeIdx)
  {
    if (theShapes(aShapeIdx).ShapeType() == TopAbs_FACE)
      isOK = initFace(TopoDS::Face(theShapes(aShapeIdx)), aShapeIdx);
    else if (theShapes(aShapeIdx).ShapeType() == TopAbs_EDGE)
      isOK = initEdge(TopoDS::Edge(theShapes(aShapeIdx)), aShapeIdx);
  }

  int aNumTrg = static_cast<int>(myTriangles.size());
  myTrgIdxMap.Clear();
  myTrgIdxMap.ReSize(aNumTrg);

  for (int aTrgIdx = 0; aTrgIdx < aNumTrg; ++aTrgIdx)
  {
    myTrgIdxMap.Bind(aTrgIdx, aTrgIdx);
  }

  MarkDirty(); // needs BVH rebuilding

  Standard_ASSERT_RETURN(!BVH().IsNull(), "Error: Failed to build BVH for primitive set", false);

  return true;
}

//=======================================================================
// function : initFace
// purpose  : Initializes triangle set
//=======================================================================
bool BRepExtrema_TriangleSet::initFace(const TopoDS_Face& theFace, const int theIndex)
{
  TopLoc_Location aLocation;

  occ::handle<Poly_Triangulation> aTriangulation = BRep_Tool::Triangulation(theFace, aLocation);
  if (aTriangulation.IsNull())
  {
    return false;
  }

  const int aVertOffset = static_cast<int>(myVertexArray.size()) - 1;

  initNodes(aTriangulation->MapNodeArray()->ChangeArray1(), aLocation.Transformation(), theIndex);

  for (int aTriIdx = 1; aTriIdx <= aTriangulation->NbTriangles(); ++aTriIdx)
  {
    int aVertex1;
    int aVertex2;
    int aVertex3;

    aTriangulation->Triangle(aTriIdx).Get(aVertex1, aVertex2, aVertex3);

    myTriangles.push_back(
      BVH_Vec4i(aVertex1 + aVertOffset, aVertex2 + aVertOffset, aVertex3 + aVertOffset, theIndex));
  }

  myNumTrgInShapeVec.SetValue(theIndex, aTriangulation->NbTriangles());

  return true;
}

//=======================================================================
// function : initEdge
// purpose  : Initializes triangle set
//=======================================================================
bool BRepExtrema_TriangleSet::initEdge(const TopoDS_Edge& theEdge, const int theIndex)
{
  TopLoc_Location aLocation;

  occ::handle<Poly_Polygon3D> aPolygon = BRep_Tool::Polygon3D(theEdge, aLocation);
  if (aPolygon.IsNull())
  {
    return false;
  }

  const int aVertOffset = static_cast<int>(myVertexArray.size()) - 1;

  initNodes(aPolygon->Nodes(), aLocation.Transformation(), theIndex);

  for (int aVertIdx = 1; aVertIdx < aPolygon->NbNodes(); ++aVertIdx)
  {
    // segment as degenerate triangle
    myTriangles.push_back(BVH_Vec4i(aVertIdx + aVertOffset,
                                    aVertIdx + aVertOffset + 1,
                                    aVertIdx + aVertOffset + 1,
                                    theIndex));
  }
  return true;
}

//=================================================================================================

void BRepExtrema_TriangleSet::initNodes(const NCollection_Array1<gp_Pnt>& theNodes,
                                        const gp_Trsf&                    theTrsf,
                                        const int                         theIndex)
{
  for (int aVertIdx = 1; aVertIdx <= theNodes.Size(); ++aVertIdx)
  {
    gp_Pnt aVertex = theNodes.Value(aVertIdx);

    aVertex.Transform(theTrsf);

    myVertexArray.push_back(BVH_Vec3d(aVertex.X(), aVertex.Y(), aVertex.Z()));
    myShapeIdxOfVtxVec.Append(theIndex);
  }

  myNumVtxInShapeVec.SetValue(theIndex, theNodes.Size());
}
