// Created on: 2002-03-18
// Created by: QA Admin
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

#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GC_MakeTrimmedCone.hxx>

static int BUC60857(Draw_Interpretor& di, int /*argc*/, const char** argv)
{
  double R1 = 8, R2 = 16;
  gp_Pnt P1(0, 0, 20), P2(0, 0, 45);

  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "Use vinit command before " << argv[0] << "\n";
    return 1;
  }

  occ::handle<Geom_RectangularTrimmedSurface> S = GC_MakeTrimmedCone(P1, P2, R1, R2).Value();
  TopoDS_Shape myshape = BRepBuilderAPI_MakeFace(S, Precision::Confusion()).Shape();
  DBRep::Set("BUC60857_BLUE", myshape);
  occ::handle<AIS_Shape> ais1 = new AIS_Shape(myshape);
  aContext->Display(ais1, false);
  aContext->SetColor(ais1, Quantity_NOC_BLUE1, false);

  occ::handle<Geom_RectangularTrimmedSurface> S2 = GC_MakeTrimmedCone(P1, P2, R1, 0).Value();
  TopoDS_Shape myshape2 = BRepBuilderAPI_MakeFace(S2, Precision::Confusion()).Shape();
  DBRep::Set("BUC60857_RED", myshape2);
  occ::handle<AIS_Shape> ais2 = new AIS_Shape(myshape2);
  aContext->Display(ais2, false);
  aContext->SetColor(ais2, Quantity_NOC_RED, false);

  occ::handle<Geom_RectangularTrimmedSurface> S3 = GC_MakeTrimmedCone(P1, P2, R2, R1).Value();
  TopoDS_Shape myshape3 = BRepBuilderAPI_MakeFace(S3, Precision::Confusion()).Shape();
  DBRep::Set("BUC60857_GREEN", myshape3);
  occ::handle<AIS_Shape> ais3 = new AIS_Shape(myshape3);
  aContext->Display(ais3, false);
  aContext->SetColor(ais3, Quantity_NOC_GREEN, true);

  return 0;
}

#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <gp_Elips2d.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Circle.hxx>

void QABugs::Commands_9(Draw_Interpretor& theCommands)
{
  const char* group = "QABugs";

  theCommands.Add("BUC60857", "BUC60857", __FILE__, BUC60857, group);

  return;
}
