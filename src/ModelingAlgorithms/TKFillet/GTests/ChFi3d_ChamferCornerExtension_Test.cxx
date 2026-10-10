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

#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgoAPI_Check.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRep_Tool.hxx>
#include <ChFi3d_ChBuilder.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <Geom_Circle.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <gtest/gtest.h>
#include <cmath>
#include <tuple>

namespace
{
class CornerBuilder : public ChFi3d_ChBuilder
{
public:
  explicit CornerBuilder(const TopoDS_Shape& theShape)
      : ChFi3d_ChBuilder(theShape, Precision::Confusion())
  {
  }

  occ::handle<ChFiDS_Spine> ExtendClosedCorner()
  {
    if (myListStripe.IsEmpty())
    {
      return {};
    }
    const auto                                   aStripe = myListStripe.First();
    NCollection_List<occ::handle<ChFiDS_Stripe>> aStripes;
    aStripes.Append(aStripe);
    aStripes.Append(aStripe);
    ExtentTwoCorner(aStripe->Spine()->FirstVertex(), aStripes);
    return aStripe->Spine();
  }
};

// A circular bore with two tangent walls meeting at one sharp corner.
// Its rim is one tangent contour whose first and last vertex coincide.
TopoDS_Shape makeCorner(double theAngle, double theScale)
{
  const double            aRadius = 10. * theScale;
  const double            aHalf   = theAngle * M_PI / 360.;
  const double            aStart  = M_PI / 2. + aHalf;
  const gp_Pnt            aTip(-aRadius / std::sin(aHalf), 0., 0.);
  const gp_Pnt            aTop(aRadius * std::cos(aStart), aRadius * std::sin(aStart), 0.);
  const gp_Pnt            aBottom(aTop.X(), -aTop.Y(), 0.);
  BRepBuilderAPI_MakeWire aWire;
  aWire.Add(BRepBuilderAPI_MakeEdge(aTip, aBottom));
  aWire.Add(BRepBuilderAPI_MakeEdge(new Geom_Circle(gp_Ax2(gp_Pnt(), gp_Dir(0, 0, 1)), aRadius),
                                    -aStart,
                                    aStart));
  aWire.Add(BRepBuilderAPI_MakeEdge(aTop, aTip));
  const TopoDS_Shape aTool =
    BRepPrimAPI_MakePrism(BRepBuilderAPI_MakeFace(aWire.Wire()), gp_Vec(0, 0, 20. * theScale));
  const TopoDS_Shape aBox =
    BRepPrimAPI_MakeBox(gp_Pnt(aTip.X() - 10. * theScale, -20. * theScale, -5. * theScale),
                        30. * theScale - aTip.X(),
                        40. * theScale,
                        20. * theScale);
  return BRepAlgoAPI_Cut(aBox, aTool).Shape();
}

TopoDS_Edge rimEdge(const TopoDS_Shape& theShape, double theScale, bool theReversed)
{
  for (TopExp_Explorer anExp(theShape, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    TopoDS_Edge       anEdge = TopoDS::Edge(anExp.Current());
    BRepAdaptor_Curve aCurve(anEdge);
    if (aCurve.GetType() == GeomAbs_Circle
        && std::abs(aCurve.Circle().Location().Z() - 15. * theScale) < Precision::Confusion())
    {
      if (theReversed)
      {
        anEdge.Reverse();
      }
      return anEdge;
    }
  }
  return {};
}
} // namespace

class ChFi3d_ChamferCornerExtension
    : public testing::TestWithParam<std::tuple<double, double, bool, bool>>
{
protected:
  void MakeInput(TopoDS_Shape& theShape, TopoDS_Edge& theEdge)
  {
    const auto [anAngle, aScale, isReversed, isNurbs] = GetParam();
    theShape                                          = makeCorner(anAngle, aScale);
    theEdge                                           = rimEdge(theShape, aScale, isReversed);
    ASSERT_FALSE(theEdge.IsNull());
    if (isNurbs)
    {
      BRepBuilderAPI_NurbsConvert aConvert(theShape, true);
      theEdge  = TopoDS::Edge(aConvert.ModifiedShape(theEdge));
      theShape = aConvert.Shape();
    }
    ASSERT_FALSE(theEdge.IsNull());
    ASSERT_TRUE(BRepCheck_Analyzer(theShape).IsValid());
    ASSERT_TRUE(BRepAlgoAPI_Check(theShape).IsValid());
  }
};

TEST_P(ChFi3d_ChamferCornerExtension, ExtentTwoCorner_SharpClosedContour_ExtendsBothEnds)
{
  const double anAngle = std::get<0>(GetParam());
  const double aScale  = std::get<1>(GetParam());
  TopoDS_Shape aShape;
  TopoDS_Edge  anEdge;
  ASSERT_NO_FATAL_FAILURE(MakeInput(aShape, anEdge));
  CornerBuilder aBuilder(aShape);
  aBuilder.Add(.3 * aScale, anEdge);
  const auto aSpine = aBuilder.ExtendClosedCorner();
  ASSERT_FALSE(aSpine.IsNull());
  ASSERT_EQ(aSpine->NbEdges(), 3);
  ASSERT_TRUE(aSpine->FirstVertex().IsSame(aSpine->LastVertex()));
  const double aRequired = .3 * aScale / std::tan(anAngle * M_PI / 360.);
  const double aFirst    = -aSpine->FirstParameter();
  const double aLast     = aSpine->LastParameter() - aSpine->LastParameter(aSpine->NbEdges());
  EXPECT_TRUE(std::isfinite(aFirst));
  EXPECT_TRUE(std::isfinite(aLast));
  EXPECT_GE(aFirst, aRequired - Precision::Confusion());
  EXPECT_GE(aLast, aRequired - Precision::Confusion());
  // Existing extension policy uses safety factors 3 and 1.5, not an unbounded guide.
  EXPECT_LE(aFirst, 3. * aRequired + Precision::Confusion());
  EXPECT_LE(aLast, 3. * aRequired + Precision::Confusion());
}

TEST_P(ChFi3d_ChamferCornerExtension, Build_SharpClosedContour_ProducesValidSolid)
{
  const double anAngle = std::get<0>(GetParam());
  const double aScale  = std::get<1>(GetParam());
  TopoDS_Shape aShape;
  TopoDS_Edge  anEdge;
  ASSERT_NO_FATAL_FAILURE(MakeInput(aShape, anEdge));
  for (double aDistance : {.3, 2.})
  {
    SCOPED_TRACE(testing::Message() << "distance=" << aDistance);
    BRepFilletAPI_MakeChamfer aBuilder(aShape);
    aBuilder.Add(aDistance * aScale, anEdge);
    ASSERT_NO_THROW(aBuilder.Build());
    ASSERT_TRUE(aBuilder.IsDone());
    const auto& aResult = aBuilder.Shape();
    EXPECT_TRUE(BRepCheck_Analyzer(aResult, true, false, true).IsValid());
    EXPECT_TRUE(BRepAlgoAPI_Check(aResult).IsValid());
    int aSolids = 0, aShells = 0;
    for (TopExp_Explorer anExp(aResult, TopAbs_SOLID); anExp.More(); anExp.Next())
    {
      ++aSolids;
    }
    for (TopExp_Explorer anExp(aResult, TopAbs_SHELL); anExp.More(); anExp.Next())
    {
      ++aShells;
      EXPECT_TRUE(BRep_Tool::IsClosed(anExp.Current()));
    }
    EXPECT_EQ(aSolids, 1);
    EXPECT_EQ(aShells, 1);
    GProp_GProps aBefore, anAfter;
    // Gauss-Kronrod integration with knot spans handles the rational surfaces.
    ASSERT_GE(BRepGProp::VolumePropertiesGK(aShape, aBefore, 1.e-9, false, true), 0.);
    ASSERT_GE(BRepGProp::VolumePropertiesGK(aResult, anAfter, 1.e-9, false, true), 0.);
    EXPECT_GT(anAfter.Mass(), 0.);
    EXPECT_LT(anAfter.Mass(), aBefore.Mass());
    // The bore section has area C * r^2, where C = pi/2 + angle/2 + cot(angle/2).
    // At height z within the chamfer, its radius grows from r to r + d.
    // Integrating the added section area gives C * (r*d^2 + d^3/3).
    const double aHalfAngle     = anAngle * M_PI / 360.;
    const double aSectionFactor = M_PI / 2. + aHalfAngle + 1. / std::tan(aHalfAngle);
    const double aDist          = aDistance * aScale;
    const double aRemoved =
      aSectionFactor * (10. * aScale * aDist * aDist + aDist * aDist * aDist / 3.);
    // Bound the volume error by the chamfer area times the public builder's
    // 1.e-4 surface approximation tolerance. A relative volume threshold alone
    // would demand a smaller geometric error as the model or chamfer shrinks.
    const double aChamferArea =
      std::sqrt(2.) * aSectionFactor * (20. * aScale * aDist + aDist * aDist);
    EXPECT_NEAR(aBefore.Mass() - anAfter.Mass(), aRemoved, 1.e-4 * aChamferArea);
  }
}

INSTANTIATE_TEST_SUITE_P(AnglesScalesAndOrientation,
                         ChFi3d_ChamferCornerExtension,
                         testing::Combine(testing::Values(30., 60., 90., 120.),
                                          testing::Values(.1, 1., 10.),
                                          testing::Bool(),
                                          testing::Values(false)));

class ChFi3d_ChamferCornerReference : public ChFi3d_ChamferCornerExtension
{
};

TEST_P(ChFi3d_ChamferCornerReference, Build_SwappedReferenceFaces_ProducesEquivalentVolumes)
{
  TopoDS_Shape aShape;
  TopoDS_Edge  anEdge;
  ASSERT_NO_FATAL_FAILURE(MakeInput(aShape, anEdge));
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aMap;
  TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, aMap);
  const auto& aFaces = aMap.FindFromKey(anEdge);
  ASSERT_EQ(aFaces.Size(), 2);
  for (bool isUnequal : {false, true})
  {
    const double aFirst = .3, aSecond = isUnequal ? .6 : .3;
    double       aVolumes[2] = {};
    for (int anOrder = 0; anOrder < 2; ++anOrder)
    {
      SCOPED_TRACE(testing::Message() << "unequal=" << isUnequal << " order=" << anOrder);
      BRepFilletAPI_MakeChamfer aBuilder(aShape);
      aBuilder.Add(anOrder ? aSecond : aFirst,
                   anOrder ? aFirst : aSecond,
                   anEdge,
                   TopoDS::Face(anOrder ? aFaces.Last() : aFaces.First()));
      ASSERT_NO_THROW(aBuilder.Build());
      ASSERT_TRUE(aBuilder.IsDone());
      EXPECT_TRUE(BRepCheck_Analyzer(aBuilder.Shape(), true, false, true).IsValid());
      EXPECT_TRUE(BRepAlgoAPI_Check(aBuilder.Shape()).IsValid());
      GProp_GProps aProps;
      ASSERT_GE(BRepGProp::VolumePropertiesGK(aBuilder.Shape(), aProps, 1.e-9, false, true), 0.);
      aVolumes[anOrder] = aProps.Mass();
    }
    EXPECT_NEAR(aVolumes[0], aVolumes[1], 1.e-7 * aVolumes[0]);
  }
}

INSTANTIATE_TEST_SUITE_P(EquivalentSupports,
                         ChFi3d_ChamferCornerReference,
                         testing::Combine(testing::Values(60.),
                                          testing::Values(1.),
                                          testing::Bool(),
                                          testing::Values(false)));
