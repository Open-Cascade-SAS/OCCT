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

#include <IntCurveSurface_ThePolygonOfHInter.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <Geom_Line.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

TEST(IntCurveSurface_ThePolygonOfHInter_Test, ClosedFlagReflectsStoredState)
{
  occ::handle<Geom_Line>          aLine = new Geom_Line(gp_Ax1(gp_Pnt(0.0, 0.0, 0.0),
                                                      gp_Dir(1.0, 0.0, 0.0)));
  occ::handle<GeomAdaptor_Curve>  anAdaptor = new GeomAdaptor_Curve(aLine);
  IntCurveSurface_ThePolygonOfHInter aPolygon(anAdaptor, 6);

  EXPECT_FALSE(aPolygon.Closed());
  aPolygon.Closed(true);
  EXPECT_TRUE(aPolygon.Closed());
}
