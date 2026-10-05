// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRep_Tool.hxx>
#include <BVH_DistanceField.hxx>
#include <BVH_Geometry.hxx>
#include <BVH_Triangulation.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <cmath>

namespace
{
class ShapeTriangulation : public BVH_Triangulation<double, 3>
{
public:
  bool Build(const TopoDS_Face& theFace)
  {
    TopLoc_Location                   aLocation;
    const Handle(Poly_Triangulation)& aTriangulation = BRep_Tool::Triangulation(theFace, aLocation);
    if (aTriangulation.IsNull())
    {
      return false;
    }

    for (int aTriangleIndex = 1; aTriangleIndex <= aTriangulation->NbTriangles(); ++aTriangleIndex)
    {
      int aNode1 = 0;
      int aNode2 = 0;
      int aNode3 = 0;
      aTriangulation->Triangle(aTriangleIndex).Get(aNode1, aNode2, aNode3);
      const gp_Pnt aPoint1 = aTriangulation->Node(aNode1).Transformed(aLocation.Transformation());
      const gp_Pnt aPoint2 = aTriangulation->Node(aNode2).Transformed(aLocation.Transformation());
      const gp_Pnt aPoint3 = aTriangulation->Node(aNode3).Transformed(aLocation.Transformation());

      const int aFirstVertex = static_cast<int>(Vertices.Size());
      Vertices.Append(BVH_Vec3d(aPoint1.X(), aPoint1.Y(), aPoint1.Z()));
      Vertices.Append(BVH_Vec3d(aPoint2.X(), aPoint2.Y(), aPoint2.Z()));
      Vertices.Append(BVH_Vec3d(aPoint3.X(), aPoint3.Y(), aPoint3.Z()));
      Elements.Append(BVH_Vec4i(aFirstVertex, aFirstVertex + 1, aFirstVertex + 2, aTriangleIndex));
    }

    MarkDirty();
    BVH();
    return Size() > 0;
  }
};

class ShapeGeometry : public BVH_Geometry<double, 3>
{
public:
  bool Build(const TopoDS_Shape& theShape)
  {
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaces;
    TopExp::MapShapes(theShape, TopAbs_FACE, aFaces);
    for (int aFaceIndex = 1; aFaceIndex <= aFaces.Extent(); ++aFaceIndex)
    {
      const opencascade::handle<ShapeTriangulation> aTriangulation = new ShapeTriangulation();
      if (!aTriangulation->Build(TopoDS::Face(aFaces(aFaceIndex))))
      {
        return false;
      }
      myObjects.Append(aTriangulation);
    }

    MarkDirty();
    BVH();
    return myObjects.Size() > 0;
  }
};
} // namespace

// Migrated from tests/lowalgos/bvh/bug30655_4.  The DRAW command only checked
// that the distance field could be built; the additional checks make its
// output contract explicit without relying on printed voxel data.
TEST(BVH_DistanceFieldTest, Bug30655_BoxDistanceField)
{
  const TopoDS_Shape       aBox = BRepPrimAPI_MakeBox(10., 10., 10.).Shape();
  BRepMesh_IncrementalMesh aMesh(aBox, 0.1);
  (void)aMesh;

  ShapeGeometry aGeometry;
  ASSERT_TRUE(aGeometry.Build(aBox));

  BVH_DistanceField<double, 3> aDistanceField(2, true);
  ASSERT_TRUE(aDistanceField.Build(aGeometry));
  EXPECT_EQ(aDistanceField.DimensionX(), 2);
  EXPECT_EQ(aDistanceField.DimensionY(), 2);
  EXPECT_EQ(aDistanceField.DimensionZ(), 2);
  EXPECT_NE(aDistanceField.VoxelSize().x(), 0.);
  EXPECT_NE(aDistanceField.VoxelSize().y(), 0.);
  EXPECT_NE(aDistanceField.VoxelSize().z(), 0.);

  bool hasInsideVoxel  = false;
  bool hasOutsideVoxel = false;
  for (int aZ = 0; aZ < aDistanceField.DimensionZ(); ++aZ)
  {
    for (int aY = 0; aY < aDistanceField.DimensionY(); ++aY)
    {
      for (int aX = 0; aX < aDistanceField.DimensionX(); ++aX)
      {
        const double aVoxel = aDistanceField.Voxel(aX, aY, aZ);
        EXPECT_TRUE(std::isfinite(aVoxel));
        hasInsideVoxel  = hasInsideVoxel || aVoxel < 0.;
        hasOutsideVoxel = hasOutsideVoxel || aVoxel > 0.;
      }
    }
  }
  EXPECT_TRUE(hasInsideVoxel);
  EXPECT_TRUE(hasOutsideVoxel);
}
