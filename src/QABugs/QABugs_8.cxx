// Created on: 2002-03-19
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

#include <TColStd_Array2OfReal.hxx>
#include <V3d_View.hxx>

#include <BRepOffsetAPI_Sewing.hxx>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <BRepPrimAPI_MakeBox.hxx>

static Standard_Integer  OCC162 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 2 ) {
    di << "Usage : " << argv[0] << " name\n";
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(argv[1]);
  if (aShape.IsNull()) return 0;

  Standard_Real tolValue = 0.0001;
  BRepOffsetAPI_Sewing sew(tolValue);
  sew.Add(aShape);
  sew.Perform();
  TopoDS_Shape aSewed = sew.SewedShape();
  
  return 0;	
}

static Standard_Integer  OCC172 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  
  AIS_ListOfInteractive aListOfIO;
  aContext->DisplayedObjects(aListOfIO);
  AIS_ListIteratorOfListOfInteractive It;
  for (It.Initialize(aListOfIO);It.More();It.Next())
    {
      aContext->AddOrRemoveSelected (It.Value(), Standard_False);
    }
  aContext->UpdateCurrentViewer();
  return 0;	
}

static Standard_Integer  OCC204 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 2 ) {
    di << "Usage : " << argv[0] << " updateviewer=0/1\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  Standard_Boolean UpdateViewer = Standard_True;
  Standard_Integer IntegerUpdateViewer = Draw::Atoi(argv[1]);
  if (IntegerUpdateViewer == 0) {
    UpdateViewer = Standard_False;
  }

  Standard_Integer deltaY = -500;
  BRepPrimAPI_MakeBox box1(gp_Pnt(0, 0 + deltaY, 0),  gp_Pnt(100, 100 + deltaY, 100));
  BRepPrimAPI_MakeBox box2(gp_Pnt(120, 120 + deltaY, 120),  gp_Pnt(300, 300 + deltaY,300));
  BRepPrimAPI_MakeBox box3(gp_Pnt(320, 320 + deltaY, 320),  gp_Pnt(500, 500 + deltaY,500));

  Handle(AIS_InteractiveObject) ais1 = new AIS_Shape(box1.Shape());
  Handle(AIS_InteractiveObject) ais2 = new AIS_Shape(box2.Shape());
  Handle(AIS_InteractiveObject) ais3 = new AIS_Shape(box3.Shape());

  aContext->Display (ais1, Standard_False);
  aContext->Display (ais2, Standard_False);
  aContext->Display (ais3, Standard_False);

  aContext->AddOrRemoveSelected (ais1, Standard_False);
  aContext->AddOrRemoveSelected (ais2, Standard_False);
  aContext->AddOrRemoveSelected (ais3, Standard_False);

  aContext->UpdateCurrentViewer();

  //printf("\n No of currents = %d", aContext->NbCurrents());

  aContext->InitSelected();
  
  //int count = 1;
  while(aContext->MoreSelected())
  {
    //printf("\n count is = %d",  count++);
    Handle(AIS_InteractiveObject) ais = aContext->SelectedInteractive();
    aContext->Remove(ais, UpdateViewer);
    aContext->InitSelected();
  }
  
  return 0;	
}

#include <gp_Lin.hxx>
#include <BRepClass3d_Intersector3d.hxx>
#include <TopoDS.hxx>
static Standard_Integer OCC1651 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 8 ) {
    di << "Usage : " << argv[0] << " Shape PntX PntY PntZ DirX DirY DirZ\n";
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(argv[1]);
  if (aShape.IsNull()) return 0;

  gp_Pnt aP1(Draw::Atof(argv[2]), Draw::Atof(argv[3]), Draw::Atof(argv[4]));
  gp_Dir aD1(Draw::Atof(argv[5]), Draw::Atof(argv[6]), Draw::Atof(argv[7]));
  gp_Lin aL1(aP1,aD1);
  BRepClass3d_Intersector3d aI1;
  aI1.Perform(aL1, -250, 1e-7, TopoDS::Face(aShape));
  if(aI1.IsDone() && aI1.HasAPoint()) {
    gp_Pnt aR1 = aI1.Pnt();
    di << aR1.X() << " " << aR1.Y() << " " << aR1.Z() << "\n";
  }

  return 0;
}

void QABugs::Commands_8(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add("OCC162", "OCC162 name", __FILE__, OCC162, group);
  theCommands.Add("OCC172", "OCC172", __FILE__, OCC172, group);
  theCommands.Add("OCC204", "OCC204 updateviewer=0/1", __FILE__, OCC204, group);
  theCommands.Add("OCC1651", "OCC1651 Shape PntX PntY PntZ DirX DirY DirZ", __FILE__, OCC1651, group);

  return;
}
