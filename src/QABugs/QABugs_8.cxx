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

static Standard_Integer BUC60753 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc!=3)
  {
    di << "Usage : " << argv[0] << " mode (0 <=mode<=5) ratio (0.0<=ration<=1.0)" <<"\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
           
  Standard_Real Alpha = M_PI/10.;
  Standard_Real CosAlpha = Cos (Alpha);
  Standard_Real SinAlpha = Sin (Alpha);
  Standard_Real MoinsCosAlpha = Cos (-Alpha);
  Standard_Real MoinsSinAlpha = Sin (-Alpha);
  TColStd_Array2OfReal TrsfI (0, 3, 0, 3);
  TColStd_Array2OfReal TrsfX (0, 3, 0, 3);
  TColStd_Array2OfReal TrsfY (0, 3, 0, 3);
  TColStd_Array2OfReal TrsfZ (0, 3, 0, 3);
  TColStd_Array2OfReal Trsfx (0, 3, 0, 3);
  TColStd_Array2OfReal Trsfy (0, 3, 0, 3);
  TColStd_Array2OfReal Trsfz (0, 3, 0, 3);
  Standard_Integer i,j;
  for (i=0; i<=3; i++)
    for (j=0; j<=3; j++)
      if (i == j) { 
	TrsfX.SetValue (i, j, 1.0);
	TrsfY.SetValue (i, j, 1.0);
	TrsfZ.SetValue (i, j, 1.0);
	Trsfx.SetValue (i, j, 1.0);
	Trsfy.SetValue (i, j, 1.0);
	Trsfz.SetValue (i, j, 1.0);
	TrsfI.SetValue (i, j, 1.0);
      } else { 
	TrsfX.SetValue (i, j, 0.0);
	TrsfY.SetValue (i, j, 0.0);
	TrsfZ.SetValue (i, j, 0.0);
	Trsfx.SetValue (i, j, 0.0);
	Trsfy.SetValue (i, j, 0.0);
	Trsfz.SetValue (i, j, 0.0);
	TrsfI.SetValue (i, j, 0.0);
      }
	
  // Rotation Alpha autour de l'axe X
  TrsfX.SetValue (1, 1, CosAlpha);
  TrsfX.SetValue (2, 2, CosAlpha);
  TrsfX.SetValue (1, 2, -SinAlpha);
  TrsfX.SetValue (2, 1, SinAlpha);
  
  // Rotation Alpha autour de l'axe Y
  TrsfY.SetValue (0, 0, CosAlpha);
  TrsfY.SetValue (2, 2, CosAlpha);
  TrsfY.SetValue (0, 2, SinAlpha);
  TrsfY.SetValue (2, 0, -SinAlpha);
  
  // Rotation Alpha autour de l'axe Z
  TrsfZ.SetValue (0, 0, CosAlpha);
  TrsfZ.SetValue (1, 1, CosAlpha);
  TrsfZ.SetValue (0, 1, -SinAlpha);
  TrsfZ.SetValue (1, 0, SinAlpha);
  
  // Rotation -Alpha autour de l'axe X
  Trsfx.SetValue (1, 1, MoinsCosAlpha);
  Trsfx.SetValue (2, 2, MoinsCosAlpha);
  Trsfx.SetValue (1, 2, -MoinsSinAlpha);
  Trsfx.SetValue (2, 1, MoinsSinAlpha);
  
  // Rotation -Alpha autour de l'axe Y
  Trsfy.SetValue (0, 0, MoinsCosAlpha);
  Trsfy.SetValue (2, 2, MoinsCosAlpha);
  Trsfy.SetValue (0, 2, MoinsSinAlpha);
	Trsfy.SetValue (2, 0, -MoinsSinAlpha);
  
  // Rotation -Alpha autour de l'axe Z
  Trsfz.SetValue (0, 0, MoinsCosAlpha);
  Trsfz.SetValue (1, 1, MoinsCosAlpha);
  Trsfz.SetValue (0, 1, -MoinsSinAlpha);
  Trsfz.SetValue (1, 0, MoinsSinAlpha);
  
  Handle(V3d_View) myV3dView = ViewerTest::CurrentView();
  
  myV3dView->SetAnimationMode(Standard_True,Standard_True);
  myAISContext-> SetDegenerateModel((Aspect_TypeOfDegenerateModel) atoi(argv[1]),atof(argv[2]));
  
//		  Timer.Reset ();
//		  Timer.Start ();
  myV3dView->SetAnimationModeOn();
  myV3dView->SetComputedMode ( Standard_False );
  for (i=0;i<40;i++) {
    myV3dView->View()->SetTransform (Trsfz);
    myV3dView->View()->Update ();
  }
  myV3dView->SetAnimationModeOff();
//		  Timer.Stop ();
//		  Timer.Show (cout);   

  return 0;
}

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
  Standard_Integer IntegerUpdateViewer = atoi(argv[1]);
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

  gp_Pnt aP1(atof(argv[2]), atof(argv[3]), atof(argv[4]));
  gp_Dir aD1(atof(argv[5]), atof(argv[6]), atof(argv[7]));
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
  char *group = "QABugs";

  theCommands.Add("BUC60753", "BUC60753 mode ratio", __FILE__, BUC60753, group);
  theCommands.Add("OCC162", "OCC162 name", __FILE__, OCC162, group);
  theCommands.Add("OCC172", "OCC172", __FILE__, OCC172, group);
  theCommands.Add("OCC204", "OCC204 updateviewer=0/1", __FILE__, OCC204, group);
  theCommands.Add("OCC1651", "OCC1651 Shape PntX PntY PntZ DirX DirY DirZ", __FILE__, OCC1651, group);

  return;
}
