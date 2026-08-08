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
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Plane.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Sequence.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Pnt.hxx>

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
         aScope                     = aScope->Parent())
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
  occ::handle<BRepAdaptor_Curve> aCurve = new BRepAdaptor_Curve(theEdge);
  const occ::handle<Adaptor3d_Curve>& aCurveAdaptor = aCurve;
  theBuilder.Add(new GeomPlate_CurveConstraint(aCurveAdaptor, 0));
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

// Migrated from tests/lowalgos/progress/A1.  Validate the progress scopes
// emitted by the same GeomPlate operation rather than testing DRAW output.
TEST(GeomPlate_BuildPlateSurface, Progress_A1_FourBezierEdgesWithPlane)
{
  GeomPlate_BuildPlateSurface aBuilder(3, 15, 2);
  aBuilder.LoadInitSurface(new Geom_Plane(gp_Pln(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.))));

  const TopoDS_Edge anEdge1 = createBezierEdge(gp_Pnt(0., 0., 0.),
                                               gp_Pnt(1., 0., 1.),
                                               gp_Pnt(2., 0., 0.));
  const TopoDS_Edge anEdge2 = createBezierEdge(gp_Pnt(0., 2., 0.),
                                               gp_Pnt(1., 2., 1.),
                                               gp_Pnt(2., 2., 0.));
  const TopoDS_Edge anEdge3 = createBezierEdge(gp_Pnt(0., 0., 0.),
                                               gp_Pnt(0., 1., 1.),
                                               gp_Pnt(0., 2., 0.));
  const TopoDS_Edge anEdge4 = createBezierEdge(gp_Pnt(2., 0., 0.),
                                               gp_Pnt(2., 1., 1.),
                                               gp_Pnt(2., 2., 0.));
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
                     createBezierEdge(gp_Pnt(0., 0., 0.),
                                      gp_Pnt(1., 0., 1.),
                                      gp_Pnt(2., 0., 0.)));
  addCurveConstraint(aBuilder,
                     createBezierEdge(gp_Pnt(0., 2., 0.),
                                      gp_Pnt(1., 2., 1.),
                                      gp_Pnt(2., 2., 0.)));
  addCurveConstraint(aBuilder,
                     createBezierEdge(gp_Pnt(0., 0., 0.),
                                      gp_Pnt(0., 1., 2.),
                                      gp_Pnt(0., 2., 0.)));
  addCurveConstraint(aBuilder,
                     createBezierEdge(gp_Pnt(2., 0., 0.),
                                      gp_Pnt(2., 1., 2.),
                                      gp_Pnt(2., 2., 0.)));

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
      gp_Pnt(std::sin(aParameter),
             std::cos(aParameter),
             std::abs(1. - 2. * anIndex / 100.)),
      0));
  }

  occ::handle<ProgressObserver> aProgress = new ProgressObserver();
  aBuilder.Perform(Message_ProgressIndicator::Start(aProgress));

  EXPECT_TRUE(aBuilder.IsDone());
  EXPECT_FALSE(aBuilder.Surface().IsNull());
  expectPlateProgress(*aProgress);
}
