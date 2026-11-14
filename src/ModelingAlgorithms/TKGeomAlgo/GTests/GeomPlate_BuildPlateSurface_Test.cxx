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

#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_CurveConstraint.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Plane.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_ConstructionError.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
class ProgressObserver : public Message_ProgressIndicator
{
public:
  const NCollection_Sequence<TCollection_AsciiString>& Messages() const { return myMessages; }

protected:
  void Show(const Message_ProgressScope& theScope, const bool) override
  {
    TCollection_AsciiString aMessage;
    aMessage += static_cast<int>(100. * GetPosition() + 0.5);
    aMessage += "%";
    for (const Message_ProgressScope* aScope = &theScope; aScope != nullptr;
         aScope                              = aScope->Parent())
    {
      if (aScope->Name() != nullptr)
      {
        aMessage += " ";
        aMessage += aScope->Name();
      }
    }
    myMessages.Append(aMessage);
  }

private:
  NCollection_Sequence<TCollection_AsciiString> myMessages;
};

bool hasProgressText(const ProgressObserver& theObserver, const char* theText)
{
  for (NCollection_Sequence<TCollection_AsciiString>::Iterator anIterator(theObserver.Messages());
       anIterator.More();
       anIterator.Next())
  {
    if (anIterator.Value().Search(theText) != -1)
    {
      return true;
    }
  }
  return false;
}

occ::handle<Geom_BezierCurve> createBezierCurve(const gp_Pnt& theP1,
                                                const gp_Pnt& theP2,
                                                const gp_Pnt& theP3)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = theP1;
  aPoles(2) = theP2;
  aPoles(3) = theP3;
  return new Geom_BezierCurve(aPoles);
}

TopoDS_Edge createBezierEdge(const gp_Pnt& theP1, const gp_Pnt& theP2, const gp_Pnt& theP3)
{
  return BRepBuilderAPI_MakeEdge(createBezierCurve(theP1, theP2, theP3)).Edge();
}

void addCurveConstraint(GeomPlate_BuildPlateSurface& theBuilder, const TopoDS_Edge& theEdge)
{
  occ::handle<BRepAdaptor_Curve>      aCurve        = new BRepAdaptor_Curve(theEdge);
  const occ::handle<Adaptor3d_Curve>& aCurveAdaptor = aCurve;
  theBuilder.Add(new GeomPlate_CurveConstraint(aCurveAdaptor, 0));
}

//! Flat patch with the corners (0, 0), theCorner21, theCorner12 and (10, 10). The first derivative
//! of the patch along an edge is null where the two corners of the edge coincide.
occ::handle<Geom_BezierSurface> createFlatPatch(const gp_Pnt& theCorner21,
                                                const gp_Pnt& theCorner12)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0, 0, 0);
  aPoles(2, 1) = theCorner21;
  aPoles(1, 2) = theCorner12;
  aPoles(2, 2) = gp_Pnt(10, 10, 0);
  return new Geom_BezierSurface(aPoles);
}

//! Adds the G1 constraint along the straight segment of the parameter space of the surface.
void addSurfaceEdgeConstraint(GeomPlate_BuildPlateSurface&     theBuilder,
                              const occ::handle<Geom_Surface>& theSurface,
                              const gp_Pnt2d&                  theFrom,
                              const gp_Pnt2d&                  theTo)
{
  const gp_Vec2d                  aDirection(theFrom, theTo);
  const occ::handle<Geom2d_Line>  aLine = new Geom2d_Line(theFrom, gp_Dir2d(aDirection));
  const occ::handle<Geom2d_Curve> aPCurve =
    new Geom2d_TrimmedCurve(aLine, 0.0, aDirection.Magnitude());
  const occ::handle<Geom2dAdaptor_Curve>      aPCurveAdaptor  = new Geom2dAdaptor_Curve(aPCurve);
  const occ::handle<GeomAdaptor_Surface>      aSurfaceAdaptor = new GeomAdaptor_Surface(theSurface);
  const occ::handle<Adaptor3d_CurveOnSurface> anEdge =
    new Adaptor3d_CurveOnSurface(aPCurveAdaptor, aSurfaceAdaptor);
  theBuilder.Add(new GeomPlate_CurveConstraint(anEdge, 1, 10, 1.e-4, 0.01, 0.1));
}

//! Adds the four edges of the patch as G1 constraints.
void addPatchBoundary(GeomPlate_BuildPlateSurface&     theBuilder,
                      const occ::handle<Geom_Surface>& theSurface)
{
  addSurfaceEdgeConstraint(theBuilder, theSurface, gp_Pnt2d(0, 0), gp_Pnt2d(1, 0));
  addSurfaceEdgeConstraint(theBuilder, theSurface, gp_Pnt2d(1, 0), gp_Pnt2d(1, 1));
  addSurfaceEdgeConstraint(theBuilder, theSurface, gp_Pnt2d(1, 1), gp_Pnt2d(0, 1));
  addSurfaceEdgeConstraint(theBuilder, theSurface, gp_Pnt2d(0, 1), gp_Pnt2d(0, 0));
}

void expectPlateProgress(const ProgressObserver& theObserver)
{
  EXPECT_GT(theObserver.Messages().Length(), 0);
  EXPECT_TRUE(hasProgressText(theObserver, "0%"));
  EXPECT_TRUE(hasProgressText(theObserver, "100%"));
  EXPECT_TRUE(hasProgressText(theObserver, "Calculating the surface filled"));
  EXPECT_TRUE(hasProgressText(theObserver, "Plate_Plate::SolveTI1()"));
  EXPECT_TRUE(hasProgressText(theObserver, "math_Gauss LU_Decompose"));
}
} // namespace

TEST(GeomPlate_BuildPlateSurface, OCC525_PerformWithoutConstraints)
{
  GeomPlate_BuildPlateSurface aBuilder;
  aBuilder.Perform();

  // TODO: IsDone() returns true due to Plate_Plate::Init() setting OK=true,
  // which is semantically wrong. Fixing this (finding #30) caused regressions
  // in blend/filling DRAW tests. Needs investigation.
  EXPECT_TRUE(aBuilder.IsDone());
  EXPECT_TRUE(aBuilder.Surface().IsNull())
    << "Surface should be null when Perform() is called without constraints";
}

// Regression test for bug #19: Surface() must be null after failed recomputation.
// After Init() + empty Perform(), stale surface must not be observable.
TEST(GeomPlate_BuildPlateSurface, Perform_ClearsStaleResult)
{
  GeomPlate_BuildPlateSurface aBuilder(3, 10, 3, 1.e-5, 1.e-4, 0.01, 0.1, false);

  // Add point constraints and solve.
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(0, 0, 0), 0));
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(1, 0, 0), 0));
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(0, 1, 0), 0));
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(1, 1, 0.1), 0));
  aBuilder.Perform();

  // Init clears constraints, then Perform with no constraints must
  // produce null surface (not stale result from the previous solve).
  aBuilder.Init();
  aBuilder.Perform();
  EXPECT_TRUE(aBuilder.IsDone());
  EXPECT_TRUE(aBuilder.Surface().IsNull()) << "Surface must be null after Init() + empty Perform()";
}

// A G1 constraint on an edge where the normal of the surface is null cannot orient the initial
// plane. The edge v = 0 or the edge u = 0 of a triangular patch collapses to a point.
TEST(GeomPlate_BuildPlateSurface, Perform_CollapsedEdgeConstraint_ThrowsConstructionError)
{
  const occ::handle<Geom_BezierSurface> aPatches[] = {
    createFlatPatch(gp_Pnt(0, 0, 0), gp_Pnt(0, 10, 0)),
    createFlatPatch(gp_Pnt(10, 0, 0), gp_Pnt(0, 0, 0))};
  for (const occ::handle<Geom_BezierSurface>& aPatch : aPatches)
  {
    GeomPlate_BuildPlateSurface aBuilder(3, 10, 3, 1.e-5, 1.e-4, 0.01, 0.1, false);
    addPatchBoundary(aBuilder, aPatch);
    EXPECT_THROW(aBuilder.Perform(), Standard_ConstructionError);
  }
}

TEST(GeomPlate_BuildPlateSurface, Perform_RegularEdgeConstraint_BuildsSurface)
{
  GeomPlate_BuildPlateSurface aBuilder(3, 10, 3, 1.e-5, 1.e-4, 0.01, 0.1, false);
  addPatchBoundary(aBuilder, createFlatPatch(gp_Pnt(10, 0, 0), gp_Pnt(0, 10, 0)));
  aBuilder.Perform();
  ASSERT_TRUE(aBuilder.IsDone());
  ASSERT_FALSE(aBuilder.Surface().IsNull());
  EXPECT_LT(aBuilder.G0Error(), 1.e-3);
}

// The normal is small but not null: the constraint is accepted.
TEST(GeomPlate_BuildPlateSurface, Perform_NearNullEdgeConstraint_BuildsSurface)
{
  GeomPlate_BuildPlateSurface aBuilder(3, 10, 3, 1.e-5, 1.e-4, 0.01, 0.1, false);
  addPatchBoundary(aBuilder, createFlatPatch(gp_Pnt(1.e-8, 0, 0), gp_Pnt(0, 10, 0)));
  aBuilder.Perform();
  ASSERT_TRUE(aBuilder.IsDone());
  ASSERT_FALSE(aBuilder.Surface().IsNull());
  EXPECT_LT(aBuilder.G0Error(), 1.e-3);
}

// Migrated from tests/lowalgos/progress/A1.  Validate the progress scopes
// emitted by the same GeomPlate operation rather than testing DRAW output.
TEST(GeomPlate_BuildPlateSurface, Progress_A1_FourBezierEdgesWithPlane)
{
  GeomPlate_BuildPlateSurface aBuilder(3, 15, 2);
  aBuilder.LoadInitSurface(new Geom_Plane(gp_Pln(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.))));

  const TopoDS_Edge anEdge1 =
    createBezierEdge(gp_Pnt(0., 0., 0.), gp_Pnt(1., 0., 1.), gp_Pnt(2., 0., 0.));
  const TopoDS_Edge anEdge2 =
    createBezierEdge(gp_Pnt(0., 2., 0.), gp_Pnt(1., 2., 1.), gp_Pnt(2., 2., 0.));
  const TopoDS_Edge anEdge3 =
    createBezierEdge(gp_Pnt(0., 0., 0.), gp_Pnt(0., 1., 1.), gp_Pnt(0., 2., 0.));
  const TopoDS_Edge anEdge4 =
    createBezierEdge(gp_Pnt(2., 0., 0.), gp_Pnt(2., 1., 1.), gp_Pnt(2., 2., 0.));
  addCurveConstraint(aBuilder, anEdge1);
  addCurveConstraint(aBuilder, anEdge2);
  addCurveConstraint(aBuilder, anEdge3);
  addCurveConstraint(aBuilder, anEdge4);

  occ::handle<ProgressObserver> aProgress = new ProgressObserver();
  aBuilder.Perform(Message_ProgressIndicator::Start(aProgress));

  EXPECT_TRUE(aBuilder.IsDone());
  EXPECT_FALSE(aBuilder.Surface().IsNull());
  expectPlateProgress(*aProgress);
}

// Migrated from tests/lowalgos/progress/A2.
TEST(GeomPlate_BuildPlateSurface, Progress_A2_FourBezierEdgesWithoutPlane)
{
  GeomPlate_BuildPlateSurface aBuilder(3, 15, 2);
  addCurveConstraint(aBuilder,
                     createBezierEdge(gp_Pnt(0., 0., 0.), gp_Pnt(1., 0., 1.), gp_Pnt(2., 0., 0.)));
  addCurveConstraint(aBuilder,
                     createBezierEdge(gp_Pnt(0., 2., 0.), gp_Pnt(1., 2., 1.), gp_Pnt(2., 2., 0.)));
  addCurveConstraint(aBuilder,
                     createBezierEdge(gp_Pnt(0., 0., 0.), gp_Pnt(0., 1., 2.), gp_Pnt(0., 2., 0.)));
  addCurveConstraint(aBuilder,
                     createBezierEdge(gp_Pnt(2., 0., 0.), gp_Pnt(2., 1., 2.), gp_Pnt(2., 2., 0.)));

  occ::handle<ProgressObserver> aProgress = new ProgressObserver();
  aBuilder.Perform(Message_ProgressIndicator::Start(aProgress));

  EXPECT_TRUE(aBuilder.IsDone());
  EXPECT_FALSE(aBuilder.Surface().IsNull());
  expectPlateProgress(*aProgress);
}

// Migrated from tests/lowalgos/progress/A3.
TEST(GeomPlate_BuildPlateSurface, Progress_A3_LargePointContour)
{
  GeomPlate_BuildPlateSurface aBuilder(3, 15, 2);
  for (int anIndex = 0; anIndex < 300; ++anIndex)
  {
    aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(anIndex, 0., 0.), 0));
  }
  for (int anIndex = 1; anIndex < 300; ++anIndex)
  {
    aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(0., anIndex, 0.), 0));
  }
  for (int anIndex = 0; anIndex < 300; ++anIndex)
  {
    aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(300., anIndex, 0.), 0));
  }
  for (int anIndex = 0; anIndex <= 300; ++anIndex)
  {
    aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(anIndex, 300., 0.), 0));
  }

  occ::handle<ProgressObserver> aProgress = new ProgressObserver();
  aBuilder.Perform(Message_ProgressIndicator::Start(aProgress));

  EXPECT_TRUE(aBuilder.IsDone());
  EXPECT_FALSE(aBuilder.Surface().IsNull());
  expectPlateProgress(*aProgress);
}

// Migrated from tests/lowalgos/progress/A4.
TEST(GeomPlate_BuildPlateSurface, Progress_A4_OneHundredPointContour)
{
  GeomPlate_BuildPlateSurface aBuilder(3, 15, 2);
  for (int anIndex = 0; anIndex < 100; ++anIndex)
  {
    const double aParameter = 2. * M_PI * anIndex / 100.;
    aBuilder.Add(new GeomPlate_PointConstraint(
      gp_Pnt(std::sin(aParameter), std::cos(aParameter), std::abs(1. - 2. * anIndex / 100.)),
      0));
  }

  occ::handle<ProgressObserver> aProgress = new ProgressObserver();
  aBuilder.Perform(Message_ProgressIndicator::Start(aProgress));

  EXPECT_TRUE(aBuilder.IsDone());
  EXPECT_FALSE(aBuilder.Surface().IsNull());
  expectPlateProgress(*aProgress);
}

TEST(GeomPlate_BuildPlateSurface, PointOnlyCurveDiagnosticsAreZero)
{
  GeomPlate_BuildPlateSurface aBuilder;
  EXPECT_DOUBLE_EQ(aBuilder.G0Error(), 0.0);
  EXPECT_DOUBLE_EQ(aBuilder.G1Error(), 0.0);
  EXPECT_DOUBLE_EQ(aBuilder.G2Error(), 0.0);
  for (const gp_Pnt& aPoint :
       {gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0), gp_Pnt(10, 10, 0), gp_Pnt(0, 10, 0), gp_Pnt(5, 5, 2)})
  {
    aBuilder.Add(new GeomPlate_PointConstraint(aPoint, 0));
  }
  for (int aRun = 0; aRun < 2; ++aRun)
  {
    aBuilder.Perform();
    ASSERT_TRUE(aBuilder.IsDone());
    ASSERT_FALSE(aBuilder.Surface().IsNull());
    EXPECT_DOUBLE_EQ(aBuilder.G0Error(), 0.0);
    EXPECT_DOUBLE_EQ(aBuilder.G1Error(), 0.0);
    EXPECT_DOUBLE_EQ(aBuilder.G2Error(), 0.0);
  }
}

TEST(GeomPlate_BuildPlateSurface, CurveDiagnosticsResetForPointOnlyReuse)
{
  GeomPlate_BuildPlateSurface aBuilder(3, 15, 2);
  addCurveConstraint(aBuilder, createBezierEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 1), gp_Pnt(2, 0, 0)));
  addCurveConstraint(aBuilder, createBezierEdge(gp_Pnt(0, 2, 0), gp_Pnt(1, 2, 1), gp_Pnt(2, 2, 0)));
  aBuilder.Perform();
  ASSERT_TRUE(aBuilder.IsDone());
  ASSERT_FALSE(aBuilder.Surface().IsNull());
  ASSERT_GT(aBuilder.G0Error(), 0.0);

  aBuilder.Init();
  EXPECT_DOUBLE_EQ(aBuilder.G0Error(), 0.0);
  EXPECT_DOUBLE_EQ(aBuilder.G1Error(), 0.0);
  EXPECT_DOUBLE_EQ(aBuilder.G2Error(), 0.0);
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(0, 0, 0), 0));
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(2, 0, 0), 0));
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(0, 2, 0), 0));
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(2, 2, 1), 0));
  aBuilder.Perform();
  ASSERT_TRUE(aBuilder.IsDone());
  ASSERT_FALSE(aBuilder.Surface().IsNull());
  EXPECT_DOUBLE_EQ(aBuilder.G0Error(), 0.0);
  EXPECT_DOUBLE_EQ(aBuilder.G1Error(), 0.0);
  EXPECT_DOUBLE_EQ(aBuilder.G2Error(), 0.0);
}
