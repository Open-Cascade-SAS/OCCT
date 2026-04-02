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

#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Element.hxx>
#include <Geom2dHatch_Elements.hxx>
#include <TopAbs_Orientation.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

static Geom2dHatch_Element makeCircleElement()
{
  gp_Ax2d                    anAxis(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(anAxis, 1.0);
  Geom2dAdaptor_Curve        aCurve(aCircle);
  Geom2dHatch_Element        anElem(aCurve, TopAbs_FORWARD);
  return anElem;
}

// Regression test for bug #38: CurrentEdge in const context must work
// without void* const-cast hack. Verifies that edge traversal returns
// valid curve and orientation after Bind + InitWires/InitEdges.
TEST(Geom2dHatch_Elements, CurrentEdge_ReturnsValidData)
{
  Geom2dHatch_Elements anElements;
  anElements.Bind(1, makeCircleElement());

  anElements.InitWires();
  ASSERT_TRUE(anElements.MoreWires());

  anElements.InitEdges();
  ASSERT_TRUE(anElements.MoreEdges());

  Geom2dAdaptor_Curve aEdge;
  TopAbs_Orientation  anOr = TopAbs_INTERNAL;
  anElements.CurrentEdge(aEdge, anOr);

  EXPECT_EQ(anOr, TopAbs_FORWARD);
  // Circle has parameter range [0, 2*PI].
  EXPECT_NEAR(aEdge.FirstParameter(), 0.0, 1e-10);
  EXPECT_GT(aEdge.LastParameter(), 6.0);
}

// Bind/Find/IsBound work correctly.
TEST(Geom2dHatch_Elements, BindAndFind)
{
  Geom2dHatch_Elements anElements;
  EXPECT_FALSE(anElements.IsBound(1));

  anElements.Bind(1, makeCircleElement());
  EXPECT_TRUE(anElements.IsBound(1));

  const Geom2dHatch_Element& anElem = anElements.Find(1);
  EXPECT_EQ(anElem.Orientation(), TopAbs_FORWARD);
}

// Clear removes all elements.
TEST(Geom2dHatch_Elements, Clear_RemovesAll)
{
  Geom2dHatch_Elements anElements;
  anElements.Bind(1, makeCircleElement());
  anElements.Bind(2, makeCircleElement());
  EXPECT_TRUE(anElements.IsBound(1));

  anElements.Clear();
  EXPECT_FALSE(anElements.IsBound(1));
  EXPECT_FALSE(anElements.IsBound(2));
}
