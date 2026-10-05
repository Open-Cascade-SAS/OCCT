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
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRep_Tool.hxx>
#include <BVH_BoxSet.hxx>
#include <BVH_LinearBuilder.hxx>
#include <BVH_PairDistance.hxx>
#include <BVH_Tools.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <cmath>
#include <limits>

namespace
{
struct Triangle
{
  BVH_Vec3d myNode1;
  BVH_Vec3d myNode2;
  BVH_Vec3d myNode3;
};

typedef BVH_BoxSet<double, 3, Triangle> TriangleBoxSet;

double triangleTriangleSqDistance(const BVH_Vec3d& theNode11,
                                  const BVH_Vec3d& theNode12,
                                  const BVH_Vec3d& theNode13,
                                  const BVH_Vec3d& theNode21,
                                  const BVH_Vec3d& theNode22,
                                  const BVH_Vec3d& theNode23)
{
  double          aMinDistance = std::numeric_limits<double>::max();
  const BVH_Vec3d aNodes[2][3] = {{theNode11, theNode12, theNode13},
                                  {theNode21, theNode22, theNode23}};

  // This is the same bounded edge sampling used by QABVH_PairDistance.
  const int aNbSegments = 100;
  for (int aTriangleIndex = 0; aTriangleIndex < 2; ++aTriangleIndex)
  {
    for (int aPointIndex = 0; aPointIndex < 3; ++aPointIndex)
    {
      const double aDistance =
        BVH_Tools<double, 3>::PointTriangleSquareDistance(aNodes[aTriangleIndex][aPointIndex],
                                                          aNodes[(aTriangleIndex + 1) % 2][0],
                                                          aNodes[(aTriangleIndex + 1) % 2][1],
                                                          aNodes[(aTriangleIndex + 1) % 2][2]);
      if (aDistance < aMinDistance)
      {
        aMinDistance = aDistance;
        if (aMinDistance == 0.)
        {
          return aMinDistance;
        }
      }
    }

    const BVH_Vec3d aEdgeStarts[3] = {aNodes[aTriangleIndex][0],
                                      aNodes[aTriangleIndex][1],
                                      aNodes[aTriangleIndex][2]};
    const BVH_Vec3d aEdgeEnds[3]   = {aNodes[aTriangleIndex][1],
                                      aNodes[aTriangleIndex][2],
                                      aNodes[aTriangleIndex][0]};
    for (int anEdgeIndex = 0; anEdgeIndex < 3; ++anEdgeIndex)
    {
      const BVH_Vec3d aFirstPoint = aEdgeStarts[anEdgeIndex];
      const BVH_Vec3d aLastPoint  = aEdgeEnds[anEdgeIndex];
      BVH_Vec3d       anEdge      = aLastPoint - aFirstPoint;
      const double    aLength     = anEdge.Modulus();
      if (aLength < Precision::Confusion())
      {
        continue;
      }
      anEdge /= aLength;

      const double aDelta = aLength / aNbSegments;
      for (int aPointIndex = 1; aPointIndex < aNbSegments; ++aPointIndex)
      {
        const BVH_Vec3d aPoint = aFirstPoint + anEdge.Multiplied(aPointIndex * aDelta);
        const double    aDistance =
          BVH_Tools<double, 3>::PointTriangleSquareDistance(aPoint,
                                                            aNodes[(aTriangleIndex + 1) % 2][0],
                                                            aNodes[(aTriangleIndex + 1) % 2][1],
                                                            aNodes[(aTriangleIndex + 1) % 2][2]);
        if (aDistance < aMinDistance)
        {
          aMinDistance = aDistance;
          if (aMinDistance == 0.)
          {
            return aMinDistance;
          }
        }
      }
    }
  }
  return aMinDistance;
}

opencascade::handle<TriangleBoxSet> makeTriangleBoxSet(const TopoDS_Shape& theShape)
{
  const opencascade::handle<BVH_LinearBuilder<double, 3>> aBuilder =
    new BVH_LinearBuilder<double, 3>();
  const opencascade::handle<TriangleBoxSet> aSet = new TriangleBoxSet(aBuilder);

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaces;
  TopExp::MapShapes(theShape, TopAbs_FACE, aFaces);
  for (int aFaceIndex = 1; aFaceIndex <= aFaces.Extent(); ++aFaceIndex)
  {
    TopLoc_Location                   aLocation;
    const Handle(Poly_Triangulation)& aTriangulation =
      BRep_Tool::Triangulation(TopoDS::Face(aFaces(aFaceIndex)), aLocation);
    if (aTriangulation.IsNull())
    {
      return opencascade::handle<TriangleBoxSet>();
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

      Triangle aTriangle;
      aTriangle.myNode1 = BVH_Vec3d(aPoint1.X(), aPoint1.Y(), aPoint1.Z());
      aTriangle.myNode2 = BVH_Vec3d(aPoint2.X(), aPoint2.Y(), aPoint2.Z());
      aTriangle.myNode3 = BVH_Vec3d(aPoint3.X(), aPoint3.Y(), aPoint3.Z());
      BVH_Box<double, 3> aBox;
      aBox.Add(aTriangle.myNode1);
      aBox.Add(aTriangle.myNode2);
      aBox.Add(aTriangle.myNode3);
      aSet->Add(aTriangle, aBox);
    }
  }
  if (aSet->Size() == 0)
  {
    return opencascade::handle<TriangleBoxSet>();
  }
  aSet->Build();
  return aSet;
}

class MeshMeshDistance : public BVH_PairDistance<double, 3, TriangleBoxSet>
{
public:
  bool Accept(const int theIndex1, const int theIndex2) override
  {
    const Triangle& aTriangle1 = myBVHSet1->Element(theIndex1);
    const Triangle& aTriangle2 = myBVHSet2->Element(theIndex2);
    const double    aDistance  = triangleTriangleSqDistance(aTriangle1.myNode1,
                                                        aTriangle1.myNode2,
                                                        aTriangle1.myNode3,
                                                        aTriangle2.myNode1,
                                                        aTriangle2.myNode2,
                                                        aTriangle2.myNode3);
    if (aDistance < myDistance)
    {
      myDistance = aDistance;
      return true;
    }
    return false;
  }
};

void meshShape(const TopoDS_Shape& theShape, const double theDeflection)
{
  BRepMesh_IncrementalMesh aMesh(theShape, theDeflection);
  (void)aMesh;
}
} // namespace

// Migrated from tests/lowalgos/bvh/bug30655_2.
TEST(BVH_PairDistanceTest, Bug30655_BoxDistance)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10., 10., 10.).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(20., 0., 0.), 10., 10., 10.).Shape();
  meshShape(aBox1, 0.1);
  meshShape(aBox2, 0.1);

  const opencascade::handle<TriangleBoxSet> aSet1 = makeTriangleBoxSet(aBox1);
  const opencascade::handle<TriangleBoxSet> aSet2 = makeTriangleBoxSet(aBox2);
  ASSERT_FALSE(aSet1.IsNull());
  ASSERT_FALSE(aSet2.IsNull());

  MeshMeshDistance aDistance;
  aDistance.SetBVHSets(aSet1.operator->(), aSet2.operator->());
  const double aDistanceValue = std::sqrt(aDistance.ComputeDistance());
  ASSERT_TRUE(aDistance.IsDone());
  EXPECT_NEAR(aDistanceValue, 10., 1.e-2);
  EXPECT_NEAR(std::sqrt(aDistance.Distance()), 10., 1.e-2);
}

// Migrated from tests/lowalgos/bvh/bug30655_3.
TEST(BVH_PairDistanceTest, Bug30655_SphereDistance)
{
  const TopoDS_Shape aSphere1 = BRepPrimAPI_MakeSphere(100.).Shape();
  const TopoDS_Shape aSphere2 = BRepPrimAPI_MakeSphere(gp_Pnt(150., 150., 150.), 110.).Shape();
  meshShape(aSphere1, 0.01);
  meshShape(aSphere2, 0.01);

  const opencascade::handle<TriangleBoxSet> aSet1 = makeTriangleBoxSet(aSphere1);
  const opencascade::handle<TriangleBoxSet> aSet2 = makeTriangleBoxSet(aSphere2);
  ASSERT_FALSE(aSet1.IsNull());
  ASSERT_FALSE(aSet2.IsNull());

  MeshMeshDistance aDistance;
  aDistance.SetBVHSets(aSet1.operator->(), aSet2.operator->());
  const double aDistanceValue = std::sqrt(aDistance.ComputeDistance());
  ASSERT_TRUE(aDistance.IsDone());

  const double anAnalyticDistance = std::sqrt(3. * 150. * 150.) - 210.;
  EXPECT_NEAR(aDistanceValue, anAnalyticDistance, 1.e-2);
  EXPECT_NEAR(std::sqrt(aDistance.Distance()), anAnalyticDistance, 1.e-2);
}
