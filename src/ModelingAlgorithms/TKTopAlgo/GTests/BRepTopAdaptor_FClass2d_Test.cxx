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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <CSLib_Class2d.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

namespace
{
TopoDS_Wire makeRectangle(const double theXMin,
                          const double theYMin,
                          const double theXMax,
                          const double theYMax)
{
  const gp_Pnt            aPoints[] = {gp_Pnt(theXMin, theYMin, 0.0),
                                       gp_Pnt(theXMax, theYMin, 0.0),
                                       gp_Pnt(theXMax, theYMax, 0.0),
                                       gp_Pnt(theXMin, theYMax, 0.0)};
  BRepBuilderAPI_MakeWire aWireBuilder;
  for (int anIndex = 0; anIndex < 4; ++anIndex)
  {
    BRepBuilderAPI_MakeEdge anEdgeBuilder(aPoints[anIndex], aPoints[(anIndex + 1) % 4]);
    if (!anEdgeBuilder.IsDone())
    {
      return TopoDS_Wire();
    }
    aWireBuilder.Add(anEdgeBuilder.Edge());
  }
  return aWireBuilder.IsDone() ? aWireBuilder.Wire() : TopoDS_Wire();
}

TopoDS_Face makeFaceWithHole()
{
  const TopoDS_Wire anInnerWire = makeRectangle(3.0, 2.0, 9.0, 6.0);
  if (anInnerWire.IsNull())
  {
    return TopoDS_Face();
  }

  BRepBuilderAPI_MakeFace aFaceBuilder(gp_Pln(gp::XOY()), 0.0, 12.0, 0.0, 8.0);
  if (!aFaceBuilder.IsDone())
  {
    return TopoDS_Face();
  }
  aFaceBuilder.Add(TopoDS::Wire(anInnerWire.Reversed()));
  return aFaceBuilder.IsDone() ? aFaceBuilder.Face() : TopoDS_Face();
}

TopoDS_Face makeFaceWithDisconnectedWire()
{
  BRepBuilderAPI_MakeFace aFaceBuilder(gp_Pln(gp::XOY()), 0.0, 10.0, 0.0, 10.0);
  if (!aFaceBuilder.IsDone())
  {
    return TopoDS_Face();
  }

  TopoDS_Face aFace = aFaceBuilder.Face();
  TopoDS_Edge aFirstEdge;
  TopoDS_Edge aThirdEdge;
  int         anEdgeIndex = 0;
  for (TopExp_Explorer anExplorer(aFace, TopAbs_EDGE); anExplorer.More(); anExplorer.Next())
  {
    if (anEdgeIndex == 0)
    {
      aFirstEdge = TopoDS::Edge(anExplorer.Current());
    }
    else if (anEdgeIndex == 2)
    {
      aThirdEdge = TopoDS::Edge(anExplorer.Current());
    }
    ++anEdgeIndex;
  }
  if (aFirstEdge.IsNull() || aThirdEdge.IsNull())
  {
    return TopoDS_Face();
  }

  BRep_Builder aBuilder;
  TopoDS_Wire  aDisconnectedWire;
  aBuilder.MakeWire(aDisconnectedWire);
  aBuilder.Add(aDisconnectedWire, aFirstEdge);
  aBuilder.Add(aDisconnectedWire, aThirdEdge);
  aBuilder.Add(aFace, aDisconnectedWire);
  return aFace;
}
} // namespace

TEST(BRepTopAdaptorFClass2dTest, LinearVectorView_ZeroBasedOrderIsClassifiedCorrectly)
{
  NCollection_LinearVector<gp_Pnt2d> aPoints;
  aPoints.Append(gp_Pnt2d(0.0, 0.0));
  aPoints.Append(gp_Pnt2d(7.0, 0.0));
  aPoints.Append(gp_Pnt2d(7.0, 3.0));
  aPoints.Append(gp_Pnt2d(0.0, 3.0));
  aPoints.Append(gp_Pnt2d(0.0, 0.0));

  NCollection_Array1<gp_Pnt2d> aView = aPoints.ToArray1();
  ASSERT_EQ(aView.Lower(), 0);
  ASSERT_EQ(aView.Upper(), 4);
  EXPECT_TRUE(aView(0).IsEqual(aPoints[0], Precision::PConfusion()));
  EXPECT_TRUE(aView(3).IsEqual(aPoints[3], Precision::PConfusion()));

  CSLib_Class2d aClassifier(aPoints.ToArray1(), 1.e-7, 1.e-7, 0.0, 0.0, 7.0, 3.0);
  EXPECT_EQ(aClassifier.SiDans(gp_Pnt2d(2.0, 1.0)), CSLib_Class2d::Result_Inside);
  EXPECT_EQ(aClassifier.SiDans(gp_Pnt2d(8.0, 1.0)), CSLib_Class2d::Result_Outside);
}

TEST(BRepTopAdaptorFClass2dTest, PlanarHole_PerformAndRestrictionPreserveWireRoles)
{
  const TopoDS_Face aFace = makeFaceWithHole();
  ASSERT_FALSE(aFace.IsNull());
  BRepTopAdaptor_FClass2d aClassifier(aFace, Precision::PConfusion());

  const gp_Pnt2d aMaterialPoint(1.0, 1.0);
  const gp_Pnt2d aHolePoint(5.0, 4.0);
  const gp_Pnt2d anOutsidePoint(14.0, 4.0);
  EXPECT_EQ(aClassifier.Perform(aMaterialPoint), TopAbs_IN);
  EXPECT_EQ(aClassifier.Perform(aHolePoint), TopAbs_OUT);
  EXPECT_EQ(aClassifier.Perform(anOutsidePoint), TopAbs_OUT);
  EXPECT_EQ(aClassifier.TestOnRestriction(aMaterialPoint, 1.e-7), TopAbs_IN);
  EXPECT_EQ(aClassifier.TestOnRestriction(aHolePoint, 1.e-7), TopAbs_OUT);
  EXPECT_EQ(aClassifier.TestOnRestriction(anOutsidePoint, 1.e-7), TopAbs_OUT);
}

TEST(BRepTopAdaptorFClass2dTest, Boundary_PerformUsesRepeatableExactFallback)
{
  const TopoDS_Face aFace = makeFaceWithHole();
  ASSERT_FALSE(aFace.IsNull());
  BRepTopAdaptor_FClass2d aClassifier(aFace, Precision::PConfusion());
  const gp_Pnt2d          aBoundaryPoint(0.0, 3.0);

  BRepClass_FaceClassifier anExact(aFace, aBoundaryPoint, Precision::PConfusion());
  ASSERT_EQ(anExact.State(), TopAbs_ON);
  for (int anIteration = 0; anIteration < 20; ++anIteration)
  {
    EXPECT_EQ(aClassifier.Perform(aBoundaryPoint), anExact.State());
    EXPECT_EQ(aClassifier.TestOnRestriction(aBoundaryPoint, 1.e-6), TopAbs_ON);
  }
}

TEST(BRepTopAdaptorFClass2dTest, PeriodicCylinder_RecadresEquivalentParameters)
{
  occ::handle<Geom_CylindricalSurface> aSurface =
    new Geom_CylindricalSurface(gp_Ax3(gp::Origin(), gp::DZ()), 2.0);
  ASSERT_FALSE(aSurface.IsNull());
  BRepBuilderAPI_MakeFace aFaceBuilder(aSurface, 0.0, 2.0 * M_PI, 0.0, 5.0, Precision::Confusion());
  ASSERT_TRUE(aFaceBuilder.IsDone());
  BRepTopAdaptor_FClass2d aClassifier(aFaceBuilder.Face(), Precision::PConfusion());

  const gp_Pnt2d aCanonicalPoint(0.75, 2.0);
  const gp_Pnt2d aShiftedPoint(aCanonicalPoint.X() + 4.0 * M_PI, aCanonicalPoint.Y());
  EXPECT_EQ(aClassifier.Perform(aCanonicalPoint), TopAbs_IN);
  EXPECT_EQ(aClassifier.Perform(aShiftedPoint), aClassifier.Perform(aCanonicalPoint));
  EXPECT_EQ(aClassifier.TestOnRestriction(aShiftedPoint, 1.e-7),
            aClassifier.TestOnRestriction(aCanonicalPoint, 1.e-7));
}

TEST(BRepTopAdaptorFClass2dTest, DisconnectedWire_FallsBackToStableExactClassification)
{
  const TopoDS_Face aFace = makeFaceWithDisconnectedWire();
  ASSERT_FALSE(aFace.IsNull());
  const gp_Pnt2d           aPoint(5.0, 5.0);
  BRepClass_FaceClassifier anExact(aFace, aPoint, Precision::PConfusion());
  BRepTopAdaptor_FClass2d  aClassifier(aFace, Precision::PConfusion());

  const TopAbs_State anExpected = anExact.State();
  for (int anIteration = 0; anIteration < 10; ++anIteration)
  {
    EXPECT_EQ(aClassifier.Perform(aPoint, false), anExpected);
    EXPECT_EQ(aClassifier.TestOnRestriction(aPoint, Precision::PConfusion(), false), anExpected);
  }
}
