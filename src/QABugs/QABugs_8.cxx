// Created on: 2002-03-19
// Created by: QA Admin
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
#include <Visual3d_View.hxx>

#include <BRepOffsetAPI_Sewing.hxx>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <BRepPrimAPI_MakeBox.hxx>

static Standard_Integer  OCC162 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 2 ) {
    di << "Usage : " << argv[0] << " name" << "\n";
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
      aContext->AddOrRemoveCurrentObject(It.Value());
    }
  
  return 0;	
}

static Standard_Integer  OCC204 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 2 ) {
    di << "Usage : " << argv[0] << " updateviewer=0/1" << "\n";
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

  Handle_AIS_Shape ais1 = new AIS_Shape(box1.Shape());
  Handle_AIS_Shape ais2 = new AIS_Shape(box2.Shape());
  Handle_AIS_Shape ais3 = new AIS_Shape(box3.Shape());

  aContext->Display(ais1);
  aContext->Display(ais2);
  aContext->Display(ais3);

  aContext->AddOrRemoveCurrentObject(ais1);
  aContext->AddOrRemoveCurrentObject(ais2);
  aContext->AddOrRemoveCurrentObject(ais3);

  //printf("\n No of currents = %d", aContext->NbCurrents());

  aContext->InitCurrent();
  
  //int count = 1;
  while(aContext->MoreCurrent())
  {
    //printf("\n count is = %d",  count++);
    Handle_AIS_InteractiveObject ais = aContext->Current();
    aContext->Remove(ais, UpdateViewer);
    aContext->InitCurrent();
  }
  
  return 0;	
}

#include <gp_Lin.hxx>
#include <BRepClass3d_Intersector3d.hxx>
#include <TopoDS.hxx>
static Standard_Integer OCC1651 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 8 ) {
    di << "Usage : " << argv[0] << " Shape PntX PntY PntZ DirX DirY DirZ" << "\n";
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
