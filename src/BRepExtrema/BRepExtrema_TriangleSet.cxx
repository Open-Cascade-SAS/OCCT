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
#include <BRepAdaptor_Surface.hxx>
#include <BVH_LinearBuilder.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

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
}

//=======================================================================
//function : GetFaceID
//purpose  : Returns face ID of the given triangle
//=======================================================================
Standard_Integer BRepExtrema_TriangleSet::GetFaceID (const Standard_Integer theIndex) const
{
  return myTriangles[theIndex].w();
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
//function : Clear
//purpose  : Clears triangle set data
//=======================================================================
void BRepExtrema_TriangleSet::Clear()
{
  BVH_Array4i anEmptyTriangles;
  myTriangles.swap (anEmptyTriangles);

  BVH_Array2d anEmptyVertUVArray;
  myVertUVArray.swap (anEmptyVertUVArray);

  BVH_Array3d anEmptyVertexArray;
  myVertexArray.swap (anEmptyVertexArray);
}

//=======================================================================
//function : Init
//purpose  : Initializes triangle set
//=======================================================================
Standard_Boolean BRepExtrema_TriangleSet::Init (const BRepExtrema_ShapeList& theFaces)
{
  Clear();

  for (Standard_Integer aFaceIdx = 0; aFaceIdx < theFaces.Size(); ++aFaceIdx)
  {
    TopLoc_Location aLocation;

    Handle(Poly_Triangulation) aTriangulation =
      BRep_Tool::Triangulation (theFaces (aFaceIdx), aLocation);

    if (aTriangulation.IsNull())
    {
      return Standard_False;
    }

    BRepAdaptor_Surface aFaceAdaptor (theFaces (aFaceIdx), Standard_False);

    const Standard_Integer aVertOffset =
      static_cast<Standard_Integer> (myVertexArray.size()) - 1;

    for (Standard_Integer aVertIdx = 1; aVertIdx <= aTriangulation->NbNodes(); ++aVertIdx)
    {
      gp_Pnt aVertex = aTriangulation->Nodes().Value (aVertIdx);

      aVertex.Transform (aLocation.Transformation());

      myVertexArray.push_back (BVH_Vec3d (aVertex.X(),
                                          aVertex.Y(),
                                          aVertex.Z()));

      const Standard_Real aU = aTriangulation->UVNodes().Value (aVertIdx).X();
      const Standard_Real aV = aTriangulation->UVNodes().Value (aVertIdx).Y();

      myVertUVArray.push_back (BVH_Vec2d (aU, aV));
    }

    for (Standard_Integer aTriIdx = 1; aTriIdx <= aTriangulation->NbTriangles(); ++aTriIdx)
    {
      Standard_Integer aVertex1;
      Standard_Integer aVertex2;
      Standard_Integer aVertex3;

      aTriangulation->Triangles().Value (aTriIdx).Get (aVertex1,
                                                       aVertex2,
                                                       aVertex3);

      myTriangles.push_back (BVH_Vec4i (aVertex1 + aVertOffset,
                                        aVertex2 + aVertOffset,
                                        aVertex3 + aVertOffset,
                                        aFaceIdx));
    }
  }

  MarkDirty(); // needs BVH rebuilding

  Standard_ASSERT_RETURN (!BVH().IsNull(),
    "Error: Failed to build BVH for primitive set", Standard_False);

  return Standard_True;
}

