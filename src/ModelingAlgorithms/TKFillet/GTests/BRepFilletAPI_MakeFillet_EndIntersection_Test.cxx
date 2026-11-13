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

// A fillet that ends on the tangent line of an existing fillet of the same radius. The end of the
// new fillet is closed by the intersection of two cylinders of equal radius.

#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRep_Tool.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_IndexedMap.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>

namespace
{
constexpr double THE_PLATE_THICKNESS = 2.5;
constexpr double THE_LIP_HEIGHT      = 4.0;
constexpr double THE_ROOT_RADIUS     = 1.3;
constexpr double THE_SMALL_RADIUS    = 0.8;

//! A 10 x 10 x 2.5 plate with a lip on top for y < 5, and the concave edge where the end face of
//! the lip meets the plate top.
struct PlateWithLip
{
  TopoDS_Shape Solid;
  TopoDS_Edge  EndEdge;
};

//=================================================================================================

//! Returns the edge whose two ends both satisfy the predicate, or a null edge if there is not
//! exactly one such edge.
template <class Predicate>
TopoDS_Edge findUniqueEdge(const TopoDS_Shape& theShape, const Predicate& thePredicate)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges;
  TopExp::MapShapes(theShape, TopAbs_EDGE, anEdges);

  TopoDS_Edge aFound;
  int         aCount = 0;
  for (int anIndex = 1; anIndex <= anEdges.Extent(); ++anIndex)
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdges.FindKey(anIndex));
    BRepAdaptor_Curve  aCurve(anEdge);
    if (thePredicate(aCurve.Value(aCurve.FirstParameter()))
        && thePredicate(aCurve.Value(aCurve.LastParameter())))
    {
      aFound = anEdge;
      ++aCount;
    }
  }
  return aCount == 1 ? aFound : TopoDS_Edge();
}

//=================================================================================================

double volumeOf(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

//=================================================================================================

double maxVertexTolerance(const TopoDS_Shape& theShape)
{
  double aMax = 0.0;
  for (TopExp_Explorer anExp(theShape, TopAbs_VERTEX); anExp.More(); anExp.Next())
  {
    aMax = std::max(aMax, BRep_Tool::Tolerance(TopoDS::Vertex(anExp.Current())));
  }
  return aMax;
}

//=================================================================================================

//! The plate profile is extruded along y. The front face of the lip leans by theLeanDeg and meets
//! the plate in a root fillet. The lip is then cut to y < 5, which leaves a concave end edge at
//! y = 5 whose end vertex lies on the tangent line of the root fillet. The mirror image of the
//! plate in the plane x = 5 is the same shape with the opposite sense of the intersection of the
//! two fillets.
bool makePlateWithLip(const double  theLeanDeg,
                      const double  theRootRadius,
                      const bool    theMirror,
                      PlateWithLip& thePlate)
{
  const double aLipFootX =
    3.2 + (THE_LIP_HEIGHT - THE_PLATE_THICKNESS) * std::tan(theLeanDeg * M_PI / 180.0);

  BRepBuilderAPI_MakePolygon aProfile;
  aProfile.Add(gp_Pnt(0.0, 0.0, 0.0));
  aProfile.Add(gp_Pnt(10.0, 0.0, 0.0));
  aProfile.Add(gp_Pnt(10.0, 0.0, THE_PLATE_THICKNESS));
  aProfile.Add(gp_Pnt(aLipFootX, 0.0, THE_PLATE_THICKNESS));
  aProfile.Add(gp_Pnt(3.2, 0.0, THE_LIP_HEIGHT));
  aProfile.Add(gp_Pnt(0.0, 0.0, THE_LIP_HEIGHT));
  aProfile.Close();
  const TopoDS_Shape aPrism =
    BRepPrimAPI_MakePrism(BRepBuilderAPI_MakeFace(aProfile.Wire()).Face(), gp_Vec(0.0, 10.0, 0.0))
      .Shape();

  const TopoDS_Edge aRootEdge = findUniqueEdge(aPrism, [&](const gp_Pnt& thePnt) {
    return std::abs(thePnt.X() - aLipFootX) < 1.e-6
           && std::abs(thePnt.Z() - THE_PLATE_THICKNESS) < 1.e-6;
  });
  if (aRootEdge.IsNull())
  {
    return false;
  }
  BRepFilletAPI_MakeFillet aRootFillet(aPrism);
  aRootFillet.Add(theRootRadius, aRootEdge);
  aRootFillet.Build();
  if (!aRootFillet.IsDone())
  {
    return false;
  }

  const TopoDS_Shape aCut =
    BRepAlgoAPI_Cut(
      aRootFillet.Shape(),
      BRepPrimAPI_MakeBox(gp_Pnt(-1.0, 5.0, THE_PLATE_THICKNESS), gp_Pnt(11.0, 11.0, 10.0)).Shape())
      .Shape();
  ShapeUpgrade_UnifySameDomain anUnifier(aCut, true, true, false);
  anUnifier.Build();
  thePlate.Solid = anUnifier.Shape();

  thePlate.EndEdge = findUniqueEdge(thePlate.Solid, [&](const gp_Pnt& thePnt) {
    return std::abs(thePnt.Y() - 5.0) < 1.e-6 && std::abs(thePnt.Z() - THE_PLATE_THICKNESS) < 1.e-6
           && thePnt.X() < aLipFootX + theRootRadius + 1.e-3;
  });
  if (thePlate.EndEdge.IsNull())
  {
    return false;
  }

  if (theMirror)
  {
    gp_Trsf aMirror;
    aMirror.SetMirror(gp_Ax2(gp_Pnt(5.0, 5.0, 0.0), gp_Dir(1.0, 0.0, 0.0)));
    BRepBuilderAPI_Transform aTransform(thePlate.Solid, aMirror, true);
    thePlate.Solid   = aTransform.Shape();
    thePlate.EndEdge = TopoDS::Edge(aTransform.ModifiedShape(thePlate.EndEdge));
  }
  return true;
}

//=================================================================================================

//! Fillets the end edge of the plate and checks the result as a solid and as the operand of a cut.
void checkEndFillet(const PlateWithLip& thePlate, const double theRadius)
{
  SCOPED_TRACE(testing::Message() << "radius " << theRadius);

  BRepFilletAPI_MakeFillet aFillet(thePlate.Solid);
  aFillet.Add(theRadius, thePlate.EndEdge);
  aFillet.Build();
  ASSERT_TRUE(aFillet.IsDone());
  const TopoDS_Shape& aResult = aFillet.Shape();

  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
  EXPECT_LT(maxVertexTolerance(aResult), 1.e-3);

  // The slab lies inside the plate, so the cut removes exactly its volume.
  const TopoDS_Shape aSlabCut =
    BRepAlgoAPI_Cut(aResult,
                    BRepPrimAPI_MakeBox(gp_Pnt(0.0, 0.0, 1.0), gp_Pnt(10.0, 10.0, 2.0)).Shape())
      .Shape();
  EXPECT_NEAR(volumeOf(aResult) - volumeOf(aSlabCut), 100.0, 1.e-3);
  EXPECT_TRUE(BRepCheck_Analyzer(aSlabCut).IsValid());
}
} // namespace

//=================================================================================================

TEST(BRepFilletAPI_MakeFilletTest, EndOnFilletTangentLine_RadiusEqualToExistingFillet_ResultIsValid)
{
  PlateWithLip aPlate;
  ASSERT_TRUE(makePlateWithLip(1.0, THE_ROOT_RADIUS, false, aPlate));
  ASSERT_TRUE(BRepCheck_Analyzer(aPlate.Solid).IsValid());

  checkEndFillet(aPlate, THE_ROOT_RADIUS);
}

TEST(BRepFilletAPI_MakeFilletTest, EndOnFilletTangentLine_RadiusNearExistingFillet_ResultIsValid)
{
  PlateWithLip aPlate;
  ASSERT_TRUE(makePlateWithLip(1.0, THE_ROOT_RADIUS, false, aPlate));

  checkEndFillet(aPlate, 1.299);
  checkEndFillet(aPlate, 1.301);
  checkEndFillet(aPlate, 1.35);
}

TEST(BRepFilletAPI_MakeFilletTest,
     EndOnFilletTangentLine_RadiusScanAroundExistingFillet_ResultsAreValid)
{
  PlateWithLip aPlate;
  ASSERT_TRUE(makePlateWithLip(1.0, THE_ROOT_RADIUS, false, aPlate));

  const double aDeltas[] = {-1.e-5, -1.e-7, 0.0, 1.e-7, 1.e-5};
  for (const double aDelta : aDeltas)
  {
    checkEndFillet(aPlate, THE_ROOT_RADIUS + aDelta);
  }
}

TEST(BRepFilletAPI_MakeFilletTest,
     EndOnFilletTangentLine_VerticalLip_RadiusEqualToExistingFillet_ResultIsValid)
{
  PlateWithLip aPlate;
  ASSERT_TRUE(makePlateWithLip(0.0, THE_ROOT_RADIUS, false, aPlate));

  checkEndFillet(aPlate, THE_ROOT_RADIUS);
}

TEST(BRepFilletAPI_MakeFilletTest,
     EndOnFilletTangentLine_MirrorImage_RadiusEqualToExistingFillet_ResultIsValid)
{
  PlateWithLip aPlate;
  ASSERT_TRUE(makePlateWithLip(1.0, THE_ROOT_RADIUS, true, aPlate));
  ASSERT_TRUE(BRepCheck_Analyzer(aPlate.Solid).IsValid());

  checkEndFillet(aPlate, THE_ROOT_RADIUS);
}

TEST(BRepFilletAPI_MakeFilletTest,
     EndOnFilletTangentLine_SmallerRadius_RadiusEqualToExistingFillet_ResultIsValid)
{
  PlateWithLip aPlate;
  ASSERT_TRUE(makePlateWithLip(1.0, THE_SMALL_RADIUS, false, aPlate));

  checkEndFillet(aPlate, THE_SMALL_RADIUS);
}

TEST(BRepFilletAPI_MakeFilletTest,
     EndOnFilletTangentLine_SmallerRadiusMirrorImage_RadiusEqualToExistingFillet_ResultIsValid)
{
  PlateWithLip aPlate;
  ASSERT_TRUE(makePlateWithLip(1.0, THE_SMALL_RADIUS, true, aPlate));

  checkEndFillet(aPlate, THE_SMALL_RADIUS);
}
