// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_PointRepresentation.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Pln.hxx>

// All Is*() queries are called through the base class handle to avoid
// C++ name hiding by virtual overloads with parameters in derived classes.

TEST(BRep_PointRepresentation_Test, PointOnCurve_EnumChecks)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.));
  Handle(BRep_PointRepresentation) aRep = new BRep_PointOnCurve(0.5, aLine, TopLoc_Location());
  EXPECT_EQ(aRep->PointRepresentationKind(), BRep_PointRepKind::PointOnCurve)
    << "PointRepresentationKind should be PointOnCurve";
  EXPECT_TRUE(aRep->IsPointOnCurve()) << "IsPointOnCurve should be true";
  EXPECT_FALSE(aRep->IsPointOnSurface()) << "IsPointOnSurface should be false";
  EXPECT_FALSE(aRep->IsPointOnCurveOnSurface()) << "IsPointOnCurveOnSurface should be false";
}

TEST(BRep_PointRepresentation_Test, PointOnSurface_EnumChecks)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());
  Handle(BRep_PointRepresentation) aRep =
    new BRep_PointOnSurface(0.5, 0.5, aPlane, TopLoc_Location());
  EXPECT_EQ(aRep->PointRepresentationKind(), BRep_PointRepKind::PointOnSurface)
    << "PointRepresentationKind should be PointOnSurface";
  EXPECT_TRUE(aRep->IsPointOnSurface()) << "IsPointOnSurface should be true";
  EXPECT_FALSE(aRep->IsPointOnCurve()) << "IsPointOnCurve should be false";
  EXPECT_FALSE(aRep->IsPointOnCurveOnSurface()) << "IsPointOnCurveOnSurface should be false";
}

TEST(BRep_PointRepresentation_Test, PointOnCurveOnSurface_EnumChecks)
{
  Handle(Geom2d_Line) aLine2d = new Geom2d_Line(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.));
  Handle(Geom_Plane)  aPlane  = new Geom_Plane(gp_Pln());
  Handle(BRep_PointRepresentation) aRep =
    new BRep_PointOnCurveOnSurface(0.5, aLine2d, aPlane, TopLoc_Location());
  EXPECT_EQ(aRep->PointRepresentationKind(), BRep_PointRepKind::PointOnCurveOnSurface)
    << "PointRepresentationKind should be PointOnCurveOnSurface";
  EXPECT_TRUE(aRep->IsPointOnCurveOnSurface()) << "IsPointOnCurveOnSurface should be true";
  EXPECT_FALSE(aRep->IsPointOnCurve()) << "IsPointOnCurve should be false";
  EXPECT_FALSE(aRep->IsPointOnSurface()) << "IsPointOnSurface should be false";
}
