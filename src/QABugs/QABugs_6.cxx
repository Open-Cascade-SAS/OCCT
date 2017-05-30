// Created on: 2002-05-22
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

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <Graphic3d_AspectMarker3d.hxx>

static Standard_Integer OCC281bug (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) 
    { 
    cerr << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
    }
  if(argc < 4) {
    di << "Usage : " << argv[0] << " x y TypeOfMarker(0-12)\n";
    return 1;
  }

  Standard_Integer x,y,TypeOfMarker;
  x = Draw::Atoi(argv[1]);
  y = Draw::Atoi(argv[2]);
  TypeOfMarker = Draw::Atoi(argv[3]);
  if( x <= 0) {
    di << "Bad value x=" << x << "\n";
    return 1;
  }
  if( y <= 0) {
    di << "Bad value y=" << y << "\n";
    return 1;
  }
  if( TypeOfMarker < 0 || TypeOfMarker > 12) {
    di << "Bad value TypeOfMarker=" << TypeOfMarker << "\n";
    return 1;
  }
  Aspect_TypeOfMarker AspectTypeOfMarker( (Aspect_TypeOfMarker) TypeOfMarker);
  /*
        enumeration TypeOfMarker is     TOM_POINT,
                                        TOM_PLUS,
                                        TOM_STAR,
                                        TOM_O,
                                        TOM_X,
                                        TOM_O_POINT,
                                        TOM_O_PLUS,
                                        TOM_O_STAR,
                                        TOM_O_X,
                                        TOM_BALL,
                                        TOM_RING1,
                                        TOM_RING2,
                                        TOM_RING3,
                                        TOM_USERDEFINED 
        end TypeOfMarker;
        ---Purpose: Definition of types of markers
        --
        --          TOM_POINT   point   .
        --          TOM_PLUS    plus    +
        --          TOM_STAR    star    *
        --          TOM_O       circle  O
        --          TOM_X       cross   x
        --          TOM_O_POINT a point in a circle
        --          TOM_O_PLUS  a plus in a circle
        --          TOM_O_STAR  a star in a circle
        --          TOM_O_X     a cross in a circle
        --          TOM_BALL    a ball with 1 color and different saturations
        --          TOM_RING1   a large ring
        --          TOM_RING2   a medium ring
        --          TOM_RING3   a small ring
        --          TOM_USERDEFINED     defined by Users
        --
        ---Category: Enumerations
  */

  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  Handle(V3d_View) aView = ViewerTest::CurrentView();

  aViewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);
  Handle(Graphic3d_AspectMarker3d) GridAsp = new Graphic3d_AspectMarker3d(AspectTypeOfMarker, Quantity_NOC_BLUE1, 10.);
  aViewer->SetGridEcho(GridAsp);

  if (aViewer->IsActive()) {
    if (aViewer->GridEcho()) {
      Standard_Real X,Y,Z;
      aView->ConvertToGrid(x,y,X,Y,Z);
    } else {
      di << "NOT aViewer->GridEcho()\n";
      return 1;
    }
  } else {
    di << "NOT aViewer->IsActive()\n";
    return 1;
  }
  return 0;
}

void QABugs::Commands_6(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add ("OCC281", "OCC281 x y TypeOfMarker(0-12)", __FILE__, OCC281bug, group);

  return;
}
