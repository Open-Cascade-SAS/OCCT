// Created on: 2002-10-24
// Created by: Michael KUZMITCHEV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <QABugs.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gce_MakeCirc.hxx>
#include <gp_Circ.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS_Wire.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
//=======================================================================
//  OCC895
//=======================================================================
static Standard_Integer OCC895 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc < 2 || argc > 5)
  {
    di << "Usage : " << argv[0] << " result [angle [reverse [order]]]\n";
    return 1;
  }

  const Standard_Real rad = 1.0;
  const Standard_Real angle = (argc > 2)? Draw::Atof(argv[2]) : 0.0;
  const Standard_Integer reverse = (argc > 3)? Draw::Atoi(argv[3]) : 0;
  const Standard_Integer order = (argc > 4)? Draw::Atoi(argv[4]) : 0;

  // Make a wire from the first arc for ThruSections.
  //
  // This arc is rotated 5 degrees about the Z axis.
  // I don't know why, but if we don't rotate it,
  // the final shell is not twisted.
  gp_Pnt center1(0,10,0);
  gp_Ax2 axis1 = reverse?
    gp_Ax2(center1, gp::DY(), gp::DZ()) : gp_Ax2(center1, -gp::DY(), gp::DX());
  if (Abs(angle) > gp::Resolution())
    axis1.Rotate(gp_Ax1(center1, gp::DZ()), angle*M_PI/180.0);

  gce_MakeCirc makeCirc1(axis1, rad);
  if(!makeCirc1.IsDone()) return 1;
  gp_Circ circ1 = makeCirc1.Value();
  GC_MakeArcOfCircle makeArc1(circ1, 0, M_PI/2, Standard_True);
  if(!makeArc1.IsDone()) return 1;
  Handle(Geom_TrimmedCurve) arc1 = makeArc1.Value();

  // Create wire 1
  BRepBuilderAPI_MakeEdge makeEdge1(arc1, arc1->StartPoint(), arc1->EndPoint());
  if(!makeEdge1.IsDone()) return 1;
  TopoDS_Edge edge1 = makeEdge1.Edge();
  BRepBuilderAPI_MakeWire makeWire1;
  makeWire1.Add(edge1);
  if(!makeWire1.IsDone()) return 1;
  TopoDS_Wire wire1 = makeWire1.Wire();

  // Make a wire from the second arc for ThruSections.
  gp_Pnt center2(10,0,0);
  gp_Ax2 axis2(center2, -gp::DX(), gp::DZ());

  gce_MakeCirc makeCirc2(axis2, rad);
  if(!makeCirc2.IsDone()) return 1;
  gp_Circ circ2 = makeCirc2.Value();
  GC_MakeArcOfCircle makeArc2(circ2, 0, M_PI/2, Standard_True);
  if(!makeArc2.IsDone()) return 1;
  Handle(Geom_TrimmedCurve) arc2 = makeArc2.Value();

  // Create wire 2
  BRepBuilderAPI_MakeEdge makeEdge2(arc2, arc2->StartPoint(), arc2->EndPoint());
  if(!makeEdge2.IsDone()) return 1;
  TopoDS_Edge edge2 = makeEdge2.Edge();
  BRepBuilderAPI_MakeWire makeWire2;
  makeWire2.Add(edge2);
  if(!makeWire2.IsDone()) return 1;
  TopoDS_Wire wire2 = makeWire2.Wire();

  BRepOffsetAPI_ThruSections thruSect(Standard_False,Standard_True);
  if (order)
  {
    thruSect.AddWire(wire1);
    thruSect.AddWire(wire2);
  }
  else
  {
    thruSect.AddWire(wire2);
    thruSect.AddWire(wire1);
  }
  thruSect.Build();
  if(!thruSect.IsDone()) return 1;
  TopoDS_Shape myShape = thruSect.Shape();

  DBRep::Set(argv[1],myShape);

  return 0;
}

void QABugs::Commands_12(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add ("OCC895", "OCC895 result [angle [reverse [order]]]", __FILE__, OCC895, group);

  return;
}
