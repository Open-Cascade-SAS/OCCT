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

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <BRepPrimAPI_MakeSphere.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ShadingAspect.hxx>

//#include <AcisData_AcisModel.hxx>
#include <TopTools_HSequenceOfShape.hxx>

static Standard_Integer BUC60738 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv)
{
  
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  
  TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(-40,0,0),20).Shape();
  Handle(AIS_Shape) theAISShape = new AIS_Shape(theSphere);

  //display mode = Shading
  theAISShape->SetDisplayMode(1);

  //get the drawer
  Handle(Prs3d_Drawer) theDrawer = theAISShape->Attributes();
  Handle(Prs3d_ShadingAspect) theShadingAspect = theDrawer->ShadingAspect();
  Handle(Graphic3d_AspectFillArea3d) theAspectFillArea3d = theShadingAspect->Aspect();

  //allow to display the edges
  theAspectFillArea3d->SetEdgeOn();
  //set the style to Dash

  //but the style is not set to dash : it is always solid
  theAspectFillArea3d->SetEdgeLineType (Aspect_TOL_DASH); 
  theAspectFillArea3d->SetEdgeColor(Quantity_Color(Quantity_NOC_GREEN)); 
  theAspectFillArea3d->SetInteriorStyle(Aspect_IS_EMPTY);
  theShadingAspect->SetAspect(theAspectFillArea3d);
  theDrawer->SetShadingAspect(theShadingAspect);
  theAISShape->SetAttributes(theDrawer);

  aContext->Display (theAISShape, Standard_True);
  
  return 0;
}

void QABugs::Commands_4(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add("BUC60738","BUC60738",__FILE__,BUC60738,group);

  return;
}
