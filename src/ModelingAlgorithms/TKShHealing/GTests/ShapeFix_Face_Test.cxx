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
#include <ShapeFix_Face.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

#include <gtest/gtest.h>

// A single closed edge belting the full period of a conical surface, healed by a bare
// ShapeFix_Face (no SetContext call - the common usage). FixPeriodicDegenerated() used to
// null-deref at its final Context()->Replace() call.
TEST(ShapeFix_FaceTest, NoCrashOnPeriodicConicalSingleWire)
{
  TColgp_Array1OfPnt aPts(1, 8);
  const double       aRadius = 0.14;
  for (int i = 1; i <= 8; i++)
  {
    const double anAngle = 2.0 * M_PI * (i - 1) / 8;
    aPts.SetValue(i, gp_Pnt(aRadius * cos(anAngle), 4.0 + aRadius * sin(anAngle), -9.6));
  }

  occ::handle<TColgp_HArray1OfPnt> aHPts = new TColgp_HArray1OfPnt(1, aPts.Length());
  for (int i = aPts.Lower(); i <= aPts.Upper(); i++)
  {
    aHPts->SetValue(i, aPts.Value(i));
  }

  GeomAPI_Interpolate anInterp(aHPts, Standard_True, 1e-6);
  anInterp.Perform();
  ASSERT_TRUE(anInterp.IsDone());

  BRepBuilderAPI_MakeEdge aMakeEdge(anInterp.Curve());
  ASSERT_TRUE(aMakeEdge.IsDone());

  BRepLib_MakeWire aMakeWire;
  aMakeWire.Add(aMakeEdge.Edge());
  ASSERT_TRUE(aMakeWire.IsDone());
  const TopoDS_Wire& aWire = aMakeWire.Wire();
  ASSERT_TRUE(aWire.Closed());

  gp_Ax3                           anAxis(gp_Pnt(0.0, 4.0, -9.1), gp_Dir(0.0, 0.09, -1.0));
  occ::handle<Geom_ConicalSurface> aCone = new Geom_ConicalSurface(anAxis, 0.35, 0.0);

  BRepBuilderAPI_MakeFace aMakeFace(aCone, aWire, Standard_True);
  ASSERT_TRUE(aMakeFace.IsDone());

  ShapeFix_Face aFixer(aMakeFace.Face());
  aFixer.Perform(); // used to SIGSEGV: Context()->Replace() on a null Context()

  const TopoDS_Face& aFixed = aFixer.Face();
  ASSERT_FALSE(aFixed.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aFixed).IsValid());
}
