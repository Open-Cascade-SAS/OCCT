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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepLib_MakeWire.hxx>
#include <Geom_ConicalSurface.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_HArray1.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_Face.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

#include <gtest/gtest.h>

namespace
{
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
