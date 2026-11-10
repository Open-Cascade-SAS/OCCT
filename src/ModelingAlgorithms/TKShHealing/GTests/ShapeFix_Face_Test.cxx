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
#include <BRepBuilderAPI_MakeFace.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_Status.hxx>
#include <ShapeFix_Face.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepLib_MakeWire.hxx>
#include <Geom_ConicalSurface.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <gp_Ax3.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <Standard_Real.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Dir2d.hxx>

#include <gtest/gtest.h>

namespace
{
//! Creates a bounded planar face suitable for testing context replacements.
//! Returns a null face if construction fails.
TopoDS_Face makePlanarFace()
{
  BRepBuilderAPI_MakeFace aMakeFace(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                                    0.0,
                                    1.0,
                                    0.0,
                                    1.0);
  if (!aMakeFace.IsDone())
  {
    return TopoDS_Face();
  }
  return aMakeFace.Face();
}

// A closed edge belting a cone's full period (8 points sampled off the cone surface).
TopoDS_Face MakeFaceOnPeriodicConicalSingleWire(occ::handle<Geom_ConicalSurface>& theCone)
{
  gp_Ax3 anAxis(gp_Pnt(0.0, 4.0, -9.1), gp_Dir(0.0, 0.09, -1.0));
  theCone = new Geom_ConicalSurface(anAxis, 0.35, 0.0);

  const int                                anN  = 8;
  const double                             aV   = 0.5;
  occ::handle<NCollection_HArray1<gp_Pnt>> aPts = new NCollection_HArray1<gp_Pnt>(1, anN);
  for (int i = 1; i <= anN; i++)
  {
    const double anU = 2.0 * M_PI * (i - 1) / anN;
    aPts->SetValue(i, theCone->Value(anU, aV));
  }

  GeomAPI_Interpolate anInterp(aPts, true, 1e-6);
  anInterp.Perform();
  if (!anInterp.IsDone())
  {
    return TopoDS_Face();
  }

  BRepBuilderAPI_MakeEdge aMakeEdge(anInterp.Curve());
  if (!aMakeEdge.IsDone())
  {
    return TopoDS_Face();
  }

  BRepLib_MakeWire aMakeWire;
  aMakeWire.Add(aMakeEdge.Edge());
  if (!aMakeWire.IsDone() || !aMakeWire.Wire().Closed())
  {
    return TopoDS_Face();
  }

  BRepBuilderAPI_MakeFace aMakeFace(theCone, aMakeWire.Wire(), true);
  return aMakeFace.IsDone() ? aMakeFace.Face() : TopoDS_Face();
}
} // namespace

TEST(ShapeFix_FaceTest, Perform_FaceReplacedByCompound_PreservesReplacement)
{
  const TopoDS_Face aFace = makePlanarFace();
  ASSERT_FALSE(aFace.IsNull());

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  occ::handle<ShapeBuild_ReShape> aContext = new ShapeBuild_ReShape;
  aContext->Replace(aFace, aCompound);

  occ::handle<ShapeFix_Face> aFixer = new ShapeFix_Face(aFace);
  aFixer->SetContext(aContext);

  EXPECT_FALSE(aFixer->Perform());
  EXPECT_TRUE(aFixer->Status(ShapeExtend_OK));
  EXPECT_TRUE(aFixer->Result().IsSame(aCompound));
}

TEST(ShapeFix_FaceTest, Perform_RemovedFace_PreservesNullReplacement)
{
  const TopoDS_Face aFace = makePlanarFace();
  ASSERT_FALSE(aFace.IsNull());

  occ::handle<ShapeBuild_ReShape> aContext = new ShapeBuild_ReShape;
  aContext->Remove(aFace);

  occ::handle<ShapeFix_Face> aFixer = new ShapeFix_Face(aFace);
  aFixer->SetContext(aContext);

  EXPECT_FALSE(aFixer->Perform());
  EXPECT_TRUE(aFixer->Status(ShapeExtend_OK));
  EXPECT_TRUE(aFixer->Result().IsNull());
}

// No SetContext() call (the ordinary usage) -- used to SIGSEGV in FixPeriodicDegenerated().
TEST(ShapeFix_FaceTest, NoCrashOnPeriodicConicalSingleWire)
{
  occ::handle<Geom_ConicalSurface> aCone;
  TopoDS_Face                      aFace = MakeFaceOnPeriodicConicalSingleWire(aCone);
  ASSERT_FALSE(aFace.IsNull());

  ShapeFix_Face aFixer(aFace);
  aFixer.Perform();

  EXPECT_FALSE(aFixer.Face().IsNull());
}

// With a context supplied, the same input heals to a genuinely valid face.
TEST(ShapeFix_FaceTest, ValidFaceOnPeriodicConicalSingleWireWithContext)
{
  occ::handle<Geom_ConicalSurface> aCone;
  TopoDS_Face                      aFace = MakeFaceOnPeriodicConicalSingleWire(aCone);
  ASSERT_FALSE(aFace.IsNull());

  ShapeFix_Face aFixer(aFace);
  aFixer.SetContext(new ShapeBuild_ReShape);
  aFixer.Perform();

  const TopoDS_Face& aFixed = aFixer.Face();
  ASSERT_FALSE(aFixed.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aFixed).IsValid());
}

namespace
{
//==================================================================================================

TopoDS_Wire makeNullAreaWire(const occ::handle<Geom_ToroidalSurface>& theSurface,
                             double                                   theFirst,
                             double                                   theHeight)
{
  const double                  aLast  = theFirst + 1.0;
  const occ::handle<Geom_Curve> aCurve = theSurface->VIso(0.0);
  BRepBuilderAPI_MakeEdge       aFirstBuilder(aCurve, theFirst, aLast);
  if (!aFirstBuilder.IsDone())
  {
    return TopoDS_Wire();
  }
  const TopoDS_Edge aFirstEdge = aFirstBuilder.Edge();
  TopoDS_Vertex     aFirstVertex, aLastVertex;
  TopExp::Vertices(aFirstEdge, aFirstVertex, aLastVertex);
  BRepBuilderAPI_MakeEdge aLastBuilder(aCurve, aFirstVertex, aLastVertex, theFirst, aLast);
  if (!aLastBuilder.IsDone())
  {
    return TopoDS_Wire();
  }
  const TopoDS_Edge            aLastEdge = aLastBuilder.Edge();
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  for (size_t anIndex = 0; anIndex < aPoles.Size(); ++anIndex)
  {
    aPoles.ChangeAt(anIndex) =
      gp_Pnt2d(theFirst + double(anIndex) / 3.0, anIndex == 1 || anIndex == 2 ? theHeight : 0.0);
  }
  const double                     aKnotValues[] = {theFirst, aLast};
  const int                        aMultValues[] = {4, 4};
  const NCollection_Array1<double> aKnots(aKnotValues[0], 1, 2);
  const NCollection_Array1<int>    aMults(aMultValues[0], 1, 2);
  BRep_Builder                     aBuilder;
  aBuilder.UpdateEdge(aFirstEdge,
                      new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3),
                      theSurface,
                      TopLoc_Location(),
                      Precision::Confusion());
  aBuilder.Range(aFirstEdge, theSurface, TopLoc_Location(), theFirst, aLast);
  aBuilder.UpdateEdge(aLastEdge,
                      new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)),
                      theSurface,
                      TopLoc_Location(),
                      Precision::Confusion());
  aBuilder.Range(aLastEdge, theSurface, TopLoc_Location(), theFirst, aLast);
  BRepBuilderAPI_MakeWire aWireBuilder(aFirstEdge, TopoDS::Edge(aLastEdge.Reversed()));
  return aWireBuilder.IsDone() ? aWireBuilder.Wire() : TopoDS_Wire();
}
} // namespace

TEST(ShapeFix_FaceTest, NaturalBoundsPreserveNullAreaTrimming)
{
  const occ::handle<Geom_ToroidalSurface> aSurface = new Geom_ToroidalSurface(gp_Ax3(), 10.0, 2.0);
  BRep_Builder                            aBuilder;
  TopoDS_Face                             aWholeFace;
  aBuilder.MakeFace(aWholeFace, aSurface, Precision::Confusion());
  ShapeFix_Face aWholeFixer(aWholeFace);
  EXPECT_TRUE(aWholeFixer.FixAddNaturalBound());
  EXPECT_TRUE(BRepCheck_Analyzer(aWholeFixer.Face()).IsValid());
  for (const double aHeight : {0.0, Epsilon(1.0)})
  {
    for (const bool isReversed : {false, true})
    {
      SCOPED_TRACE(aHeight);
      SCOPED_TRACE(isReversed);
      TopoDS_Wire aWire = makeNullAreaWire(aSurface, 0.0, aHeight);
      ASSERT_FALSE(aWire.IsNull());
      if (isReversed)
      {
        aWire.Reverse();
      }
      TopoDS_Face aFace;
      aBuilder.MakeFace(aFace, aSurface, Precision::Confusion());
      aBuilder.Add(aFace, aWire);
      ShapeFix_Face aFixer(aFace);
      EXPECT_FALSE(aFixer.FixAddNaturalBound());
      EXPECT_TRUE(aFixer.Face().IsSame(aFace));
    }
  }

  BRepBuilderAPI_MakeFace aPatchBuilder(aSurface, 0.0, 1.0, 0.0, 1.0, Precision::Confusion());
  ASSERT_TRUE(aPatchBuilder.IsDone());
  const TopoDS_Face aPatch = aPatchBuilder.Face();
  TopExp_Explorer   aWireExp(aPatch, TopAbs_WIRE);
  ASSERT_TRUE(aWireExp.More());
  const TopoDS_Wire aHoleWire = TopoDS::Wire(aWireExp.Current().Reversed());
  for (const bool hasNullWire : {false, true})
  {
    SCOPED_TRACE(hasNullWire);
    TopoDS_Face aFace;
    aBuilder.MakeFace(aFace, aSurface, Precision::Confusion());
    aBuilder.Add(aFace, aHoleWire);
    if (hasNullWire)
    {
      const TopoDS_Wire aWire = makeNullAreaWire(aSurface, 2.0, Epsilon(1.0));
      ASSERT_FALSE(aWire.IsNull());
      aBuilder.Add(aFace, aWire);
    }
    ASSERT_FALSE(ShapeAnalysis::IsOuterBound(aFace));
    ShapeFix_Face aFixer(aFace);
    EXPECT_TRUE(aFixer.FixAddNaturalBound());
    EXPECT_FALSE(aFixer.Face().IsSame(aFace));
  }
}
