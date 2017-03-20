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

#include <stdio.h>

#include <QABugs.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_EventManager.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Plane.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Pln.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAPI.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dGcc_Lin2d2Tan.hxx>
#include <Geom2d_Line.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Edge.hxx>
#include <Precision.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <Geom2dGcc_Lin2d2TanIter.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <BRep_Tool.hxx>
#include <gp_Circ.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>
#include <V3d_View.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_RadiusDimension.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Extrema_ExtPS.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2dGcc_Circ2d3Tan.hxx>
#include <Aspect_Window.hxx>

#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <StdSelect_ShapeTypeFilter.hxx>
#include <Prs3d_Projector.hxx>
#include <HLRAlgo_Projector.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Font_NameOfFont.hxx>

static Standard_Integer BUC60842 (Draw_Interpretor& di, Standard_Integer /*argc*/,const char ** /*argv*/)
{
  Standard_Character abuf[16];
  //Standard_Character * abuf = new Standard_Character[16];

  Standard_CString st = abuf;

  Handle(Geom_Circle) cir = new Geom_Circle (gp_Ax2 (gp_Pnt (823.687192, 502.366825, 478.960440), gp_Dir (0.173648, 0.984808, 0.000000), gp_Dir (-0.932169, 0.164367, -0.322560)), 50);
  Handle(Geom_Ellipse) ell = new Geom_Ellipse (gp_Ax2 (gp_Pnt (1262.224429, 425.040878, 363.609716), gp_Dir (0.173648, 0.984808, 0.000000), gp_Dir (-0.932169, 0.164367, -0.322560)), 150, 100);
  Handle(Geom_Plane) plne = new Geom_Plane (gp_Ax3 (gp_Ax2 (gp_Pnt (1262.224429, 425.040878, 363.609716), gp_Dir (0.173648, 0.984808, 0.000000), gp_Dir (-0.932169, 0.164367, -0.322560))));

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();

  gp_Pln pln = plne->Pln();
  Handle(Geom2d_Curve) curve2d = GeomAPI::To2d (ell, pln);
  Sprintf(abuf,"ell");
  DrawTrSurf::Set(st,curve2d);
  if(!aContext.IsNull()) {
    Handle(AIS_Shape) aisp = 
      new AIS_Shape (BRepBuilderAPI_MakeEdge(GeomAPI::To3d(curve2d, pln)).Edge());
    aContext->Display (aisp, Standard_False);
  }

  Handle(Geom2d_Curve) fromcurve2d = GeomAPI::To2d (cir, pln);
  Sprintf(abuf,"cil");
  DrawTrSurf::Set(st,fromcurve2d);
  if(!aContext.IsNull()) {
    Handle(AIS_Shape) aisp = 
      new AIS_Shape (BRepBuilderAPI_MakeEdge(GeomAPI::To3d(fromcurve2d, pln)).Edge());
    aContext->Display (aisp, Standard_False);
  }

  Geom2dAdaptor_Curve acur (curve2d), afromcur (fromcurve2d);
  Geom2dGcc_QualifiedCurve qcur (acur, GccEnt_outside);
  Geom2dGcc_QualifiedCurve qfromcur (afromcur, GccEnt_outside);
  Geom2dGcc_Lin2d2Tan lintan (qcur, qfromcur, 0.1, 0.0, 0.0);
  Standard_Integer i=0;
  for(i=0;i<lintan.NbSolutions();i++) {
    Sprintf(abuf,"lintan_%d",i);
    Handle(Geom2d_Line) glin = new Geom2d_Line(lintan.ThisSolution(i));
    DrawTrSurf::Set(st,glin);
    if(!aContext.IsNull()) {
      Handle(AIS_Shape) aisp = 
        new AIS_Shape (BRepBuilderAPI_MakeEdge(GeomAPI::To3d(glin, pln)).Edge());
      aContext->Display (aisp, Standard_False);
    }
  }
  aContext->UpdateCurrentViewer();
  di << " Is Done = \n" << (Standard_Integer) lintan.IsDone();
  return 0;
}

static Standard_Integer BUC60843 (Draw_Interpretor& di, Standard_Integer argc,const char ** argv)
{
  if (argc != 4 && argc != 5 && argc != 6 && argc != 7) {
    di << "Usage : " << argv[0] << "result_shape name_of_circle name_of_curve [ par1 [ tol ] ]\n";
    return 1;
  }

  Standard_Real par1 = 0.0, par2 = 0.0;
  Standard_Real tol  = Precision::Angular();
  if (argc >= 5)
    par1 = Draw::Atof(argv[4]);
  if (argc == 6)
    par2 = Draw::Atof(argv[5]);
  if (argc == 7)
    tol = Draw::Atof(argv[6]);
  Handle(Geom2d_Curve)  aCur2d1 = DrawTrSurf::GetCurve2d(argv[2]);
  Handle(Geom2d_Curve)  aCur2d2 = DrawTrSurf::GetCurve2d(argv[3]);
  if (aCur2d1.IsNull() || aCur2d2.IsNull()) {
    di << "Faulty : not curves\n";
    return 1;
  }
  Standard_Boolean c1IsCircle = Standard_False;
  Handle(Geom2d_Circle) aCir2d = Handle(Geom2d_Circle)::DownCast(aCur2d1);
  if (!aCir2d.IsNull()) {
    c1IsCircle = Standard_True;
    if (argc == 6) tol = Draw::Atof(argv[5]);
  }
  if(c1IsCircle) {
    Geom2dAdaptor_Curve acur(aCur2d2);
    Geom2dGcc_QCurve qcur(acur, GccEnt_unqualified);
    GccEnt_QualifiedCirc qfromcur(aCir2d->Circ2d(), GccEnt_unqualified);
    Geom2dGcc_Lin2d2TanIter lintan(qfromcur, qcur , par1, tol); 
    if (lintan.IsDone()) {
      gp_Lin2d lin = lintan.ThisSolution(); 
      Handle(Geom2d_Line) glin = new Geom2d_Line(lin); 
      DrawTrSurf::Set(argv[1], glin);
    } else {
      di << "Faulty : tangent line is not done.\n";
    }
  }
  else {
    Geom2dAdaptor_Curve acur1(aCur2d1);
    Geom2dAdaptor_Curve acur2(aCur2d2);
    Geom2dGcc_QCurve qcur1(acur1, GccEnt_unqualified);
    Geom2dGcc_QCurve qcur2(acur2, GccEnt_unqualified);
    Geom2dGcc_Lin2d2TanIter lintan(qcur1, qcur2 , par1, par2, tol); 
    if (lintan.IsDone()) {
      gp_Lin2d lin = lintan.ThisSolution(); 
      Handle(Geom2d_Line) glin = new Geom2d_Line(lin); 
      DrawTrSurf::Set(argv[1], glin);
    } else {
      di << "Faulty : tangent line is not done.\n";
    }
  }   
  return 0;
}

static Standard_Integer BUC60970 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc!=3)
  {
    di << "Usage : " << argv[0] << " shape result\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  TopoDS_Shape aResult = DBRep::Get(argv[1]);

  BRepBuilderAPI_MakeWire bRepSpineWire; 
  TopExp_Explorer exp_(aResult, TopAbs_WIRE); 

  Standard_Integer i = 0; 
  for(;exp_.More();exp_.Next()){     
    i++; 
    bRepSpineWire.Add(TopoDS::Wire(exp_.Current())); 
  } 

  //printf("total no of wires are ............. %d\n", i); 
  di << "total no of wires are ............. " << i << "\n"; 

  TopoDS_Wire spineWire = bRepSpineWire.Wire(); 
  aContext->Display (new AIS_Shape(spineWire), Standard_False);

  DBRep::Set("slineW",spineWire);

  TopExp_Explorer spineWireExp(spineWire, TopAbs_EDGE); 
  Standard_Real first, last; 
  Handle(Geom_Curve) curl_ = BRep_Tool::Curve(TopoDS::Edge(spineWireExp.Current()), first, last); 
  gp_Pnt firstPnt; 
  gp_Vec tanVec; 
  curl_->D1(first, firstPnt, tanVec); 
  tanVec.Normalize(); 
  gp_Dir tanDir(tanVec.X(), tanVec.Y(), tanVec.Z()); 
  gp_Ax2 gpAx2(firstPnt, tanDir); 
  gp_Circ gpCirc(gpAx2, 2.5); 
  BRepBuilderAPI_MakeWire aMWire(BRepBuilderAPI_MakeEdge(new Geom_Circle(gpCirc)).Edge());
  TopoDS_Wire topoWire(aMWire); 
  aContext->Display (new AIS_Shape(topoWire), Standard_False);

  DBRep::Set("topoW",topoWire);

  BRepOffsetAPI_MakePipeShell bRepPipe(spineWire);
  bRepPipe.Add(topoWire); 
  bRepPipe.Build(); 

  aContext->Display (new AIS_Shape(bRepPipe.Shape()), Standard_True);

  DBRep::Set(argv[2],bRepPipe.Shape());

  return 0;
}

static Standard_Integer  BUC60818(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc!=1)
  {
    di << "Usage : " << argv[0] << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) 
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

  // TRIHEDRON
  Handle(AIS_Trihedron) aTrihedron;
  Handle(Geom_Axis2Placement) aTrihedronAxis=new Geom_Axis2Placement(gp::XOY());
  aTrihedron=new AIS_Trihedron(aTrihedronAxis);
  myAISContext->Display (aTrihedron, Standard_True);

  Standard_DISABLE_DEPRECATION_WARNINGS
  myAISContext->OpenLocalContext(); 
  Standard_ENABLE_DEPRECATION_WARNINGS
  myAISContext->Load(aTrihedron,0); 

  myAISContext->SetAutomaticHilight(  Standard_False );

  Handle(V3d_View) myV3dView = ViewerTest::CurrentView();
  double Xv,Yv;
  myV3dView->Project(0,0,0,Xv,Yv);

  Standard_Integer Xp,Yp;
  myV3dView->Convert(Xv,Yv,Xp,Yp);

  myAISContext->MoveTo (Xp,Yp, myV3dView, Standard_False);
  myAISContext->MoveTo (Xp,Yp, myV3dView, Standard_True);

  if (myAISContext->HasDetected(  )) 
    di << "has detected shape : OK"   << "\n";
  else       di << "has detected shape : bugged - Faulty "   << "\n";

  myAISContext->SetAutomaticHilight(  Standard_True );

  return 0;
}

static Standard_Integer BUC60915_1(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc > 1) {
    di<<"Function don't has parameters\n";
    return 1;
  }

  if(ViewerTest::GetAISContext().IsNull())
  {
    di << "View was not created. Use 'vinit' command before "
       << argv[0] << "\n";
    return 1;
  }
  Handle(AIS_InteractiveContext) context = ViewerTest::GetAISContext();

  //The following dimesion code has problems regarding arrow_size. The desired effect is not produced.
  /***************************************/
  //First view
  /***************************************/
  gp_Pnt p1 = gp_Pnt(602.51,50.,0.);
  gp_Pnt p2 = gp_Pnt(602.51,200.,0.);
  gp_Pnt p3 = gp_Pnt(102.51,200.,0.);
  gp_Pnt p4 = gp_Pnt(102.51,170.,0.);
  gp_Pnt p5 = gp_Pnt(502.51,170.,0.);
  gp_Pnt p6 = gp_Pnt(502.51,80.,0.);
  gp_Pnt p7 = gp_Pnt(102.51,80.,0.);
  gp_Pnt p8 = gp_Pnt(102.51,50.,0.);
  TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(p1);
  TopoDS_Vertex V2 = BRepBuilderAPI_MakeVertex(p2);
  TopoDS_Vertex V3 = BRepBuilderAPI_MakeVertex(p3);
  TopoDS_Vertex V4 = BRepBuilderAPI_MakeVertex(p4);
  TopoDS_Vertex V5 = BRepBuilderAPI_MakeVertex(p5);
  TopoDS_Vertex V6 = BRepBuilderAPI_MakeVertex(p6);
  TopoDS_Vertex V7 = BRepBuilderAPI_MakeVertex(p7);
  TopoDS_Vertex V8 = BRepBuilderAPI_MakeVertex(p8);
  gp_Pnt plnpt(0, 0, 0);
  gp_Dir plndir(0, 0, 1);
  Handle(Geom_Plane) pln = new Geom_Plane(plnpt,plndir);
  Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
  anAspect->MakeArrows3d (Standard_True);
  anAspect->ArrowAspect()->SetAngle (M_PI/180.0 * 10.0);
  /***************************************/
  //dimension "L 502.51"
  /***************************************/
  Handle(AIS_LengthDimension) len = new AIS_LengthDimension(V2, V3, pln->Pln());
  anAspect->ArrowAspect()->SetLength (30.0);
  len->SetDimensionAspect (anAspect);
  context->Display (len, Standard_False);
  /***************************************/
  //dimension "L 90"
  /***************************************/
  Handle(AIS_LengthDimension) len1 = new AIS_LengthDimension(V7, V4, pln->Pln());
  len1->SetDimensionAspect (anAspect);
  len1->SetFlyout (30.0);
  anAspect->ArrowAspect()->SetLength (100.0);
  context->Display (len1, Standard_False);
  /***************************************/
  //dimension "L 150"
  /***************************************/
  Handle(AIS_LengthDimension) len2 = new AIS_LengthDimension(V1, V2, pln->Pln());
  len2->SetDimensionAspect (anAspect);
  context->Display (len2, Standard_False);
  /***************************************/
  //dimension "R 88.58"
  /***************************************/
  gp_Circ cir = gp_Circ(gp_Ax2(gp_Pnt(191.09, -88.58, 0), gp_Dir(0, 0, 1)), 88.58);
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(cir,gp_Pnt(191.09,0,0.),gp_Pnt(191.09,-177.16,0.) );
  Handle(AIS_RadiusDimension) dim1 = new AIS_RadiusDimension(E1);
  dim1->SetDimensionAspect (anAspect);
  context->Display (dim1, Standard_False);
  /***************************************/
  //dimension "R 43.80"
  /***************************************/
  gp_Circ cir1 = gp_Circ(gp_Ax2(gp_Pnt(191.09, -88.58, 0), gp_Dir(0, 0, 1)), 43.80);
  TopoDS_Edge E_cir1 = BRepBuilderAPI_MakeEdge(cir1);
  dim1 = new AIS_RadiusDimension(E_cir1);
  anAspect->ArrowAspect()->SetLength (60.0);
  dim1->SetDimensionAspect (anAspect);
  context->Display (dim1, Standard_False);
  /***************************************/
  //dimension "R 17.86"
  /***************************************/
  gp_Circ cir2 = gp_Circ(gp_Ax2(gp_Pnt(566.11, -88.58, 0), gp_Dir(0, 0, -1)), 17.86);
  TopoDS_Edge E_cir2 = BRepBuilderAPI_MakeEdge(cir2);
  dim1 = new AIS_RadiusDimension(E_cir2);
  anAspect->ArrowAspect()->SetLength (40.0);
  dim1->SetDimensionAspect (anAspect);
  context->Display (dim1, Standard_True);

  return 0;
}

static Standard_Integer  OCC138 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  BRepPrimAPI_MakeBox box1(gp_Pnt(0, 0, 0),  gp_Pnt(100, 100, 100));
  BRepPrimAPI_MakeBox box2(gp_Pnt(120, 120, 120),  gp_Pnt(300, 300,300));
  BRepPrimAPI_MakeBox box3(gp_Pnt(320, 320, 320),  gp_Pnt(500, 500,500));

  Handle(AIS_InteractiveObject) ais1 = new AIS_Shape(box1.Shape());
  Handle(AIS_InteractiveObject) ais2 = new AIS_Shape(box2.Shape());
  Handle(AIS_InteractiveObject) ais3 = new AIS_Shape(box3.Shape());

  aContext->Display (ais1, Standard_False);
  aContext->Display (ais2, Standard_False);
  aContext->Display (ais3, Standard_False);

  aContext->AddOrRemoveSelected (ais1, Standard_False);
  aContext->AddOrRemoveSelected (ais2, Standard_False);
  aContext->AddOrRemoveSelected (ais3, Standard_False);

  di << "\n No of currents = " << aContext->NbSelected();

  aContext->InitSelected();

  int count = 1;
  while(aContext->MoreSelected())
  {
    di << "\n count is = " << count++;
    Handle(AIS_InteractiveObject) ais = aContext->SelectedInteractive();
    aContext->AddOrRemoveSelected (ais, Standard_False);
    aContext->InitSelected();
  }

  aContext->UpdateCurrentViewer();

  return 0; 
}

static int geom_get_2Dpt_from_3Dpt(const gp_Pnt& pnt3d, const gp_Pln& pln, gp_Pnt2d& pnt2d)
{ 
  int ret = 0; 
  Handle(Geom_Plane) gpln = new Geom_Plane(pln); 
  GeomAdaptor_Surface adsur(gpln); 
  Extrema_ExtPS extps(pnt3d, adsur, 0.001, 0.001); 
  if( extps.IsDone() ) 
  { 
    int index;
    for(index=1; index<= extps.NbExt(); index++ ) 
    { 
      Extrema_POnSurf pons = extps.Point(index); 
      Standard_Real U, V; 
      pons.Parameter(U, V); 
      pnt2d.SetCoord(U, V); 
      ret = 1; 
    } 
  } 
  return ret;
} 

static Standard_Integer OCC353 (Draw_Interpretor& di, Standard_Integer , const char ** )
{
  gp_Ax2 ax21( gp_Pnt(100,0,0), gp_Dir(0,0,1) );
  Handle(Geom_Circle) h_cir1 = new Geom_Circle( ax21, 25 );

  gp_Ax2 ax22( gp_Pnt(-100,0,0), gp_Dir(0,0,1) );
  Handle(Geom_Circle) h_cir2 = new Geom_Circle( ax22, 25 );

  gp_Pln refpln( gp_Pnt(0,0,0), gp_Dir(0,0,1) );
  Handle(Geom2d_Curve) cir2d1 = GeomAPI::To2d(h_cir1, refpln);
  Handle(Geom2d_Curve) cir2d2 = GeomAPI::To2d(h_cir2, refpln);

  Geom2dAdaptor_Curve adop1(cir2d1);
  Geom2dAdaptor_Curve adop2(cir2d2);

  Geom2dGcc_QualifiedCurve qcur1(adop1, GccEnt_enclosing);
  Geom2dGcc_QualifiedCurve qcur2(adop2, GccEnt_enclosing);

  Handle(Geom_CartesianPoint) h_carpt = new Geom_CartesianPoint(0,175,0);

  gp_Pnt pt3d = h_carpt->Pnt();
  gp_Pnt2d pt2d;
  geom_get_2Dpt_from_3Dpt(pt3d, refpln, pt2d);

  Standard_CString st;
  st = "cir2d1";
  DrawTrSurf::Set(st,cir2d1);
  st = "cir2d2";
  DrawTrSurf::Set(st,cir2d2);
  st = "pt2d";
  DrawTrSurf::Set(st,pt2d);

  Handle(Geom2d_CartesianPoint) pt = new Geom2d_CartesianPoint(pt2d);
  Geom2dGcc_Circ2d3Tan sol( qcur1, qcur2, pt, 0.001, 0.0, 0.0);

  int res = 0;
  Standard_Character buf[10];
  buf[0] = '\0';

  if( sol.IsDone() ) {
    res = sol.NbSolutions();
    for(Standard_Integer i=1;i<=res;i++) {
      Handle(Geom2d_Circle) aC = new Geom2d_Circle(sol.ThisSolution(i));
      Sprintf(buf,"Result_%d",i);
      st = buf;
      DrawTrSurf::Set(st,aC);
    }
  }
  else
    di << "\n Faulty: no solutions found ";
  //printf("\n Faulty: no solutions found ");

  //printf("\n Num of solutions are %d ", res );
  di << "\n Num of solutions are " << res << " ";

  return 0;
}

static Standard_Integer OCC280 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) 
  { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  if ( argc != 3) {
    di << "ERROR : Usage : " << argv[0] << " hlr=0/1 setsurfecedetail=0/1; set perspecrive view\n";
    return 1;
  }

  Standard_Integer HLR = Draw::Atoi(argv[1]);
  if (HLR != 0) {
    HLR = 1;
  }

  TCollection_AsciiString anOldName = ViewerTest::GetCurrentViewName();
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  aViewer->SetDefaultTypeOfView (V3d_PERSPECTIVE);
  Handle(Aspect_Window) asp = ViewerTest::CurrentView()->Window();
  Handle(V3d_View) aNewView = aViewer->CreateView();
  ViewerTest::CurrentView (aNewView);
  TCollection_AsciiString aNewName=anOldName + "_new";
  ViewerTest::InitViewName(aNewName,ViewerTest::CurrentView());
  aNewView->SetWindow (asp);
  if (!asp->IsMapped()) asp->Map();
  aNewView->Redraw();
  ViewerTest::RemoveView(anOldName,false);
  ViewerTest::UnsetEventManager();
  ViewerTest::SetEventManager (new ViewerTest_EventManager (aNewView, ViewerTest::GetAISContext()));

  if (HLR == 1)
  {
    di << "HLR\n";
    ViewerTest::CurrentView()->SetComputedMode (Standard_True);
  }

  return 0;
}

static Standard_Integer  OCC232 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  BRep_Builder builder;
  TopoDS_Compound comp;
  TopoDS_CompSolid cs1, cs2;

  builder.MakeCompound(comp);
  builder.MakeCompSolid(cs1);
  builder.MakeCompSolid(cs2);

  TopoDS_Shape sh1 = BRepPrimAPI_MakeBox(gp_Pnt(0, 0, 0), 100, 100, 100).Shape();
  TopoDS_Shape sh2 = BRepPrimAPI_MakeBox(gp_Pnt(100, 0, 0), 100, 100, 100).Shape();
  TopoDS_Shape sh3 = BRepPrimAPI_MakeBox(gp_Pnt(200, 0, 0), 100, 100, 100).Shape();
  builder.Add(cs1, sh1);
  builder.Add(cs1, sh2);
  builder.Add(cs1, sh3);

  TopoDS_Shape sh4 = BRepPrimAPI_MakeBox(gp_Pnt(0, 500, 0), 100, 100, 100).Shape();
  TopoDS_Shape sh5 = BRepPrimAPI_MakeBox(gp_Pnt(100, 500, 0), 100, 100, 100).Shape();
  TopoDS_Shape sh6 = BRepPrimAPI_MakeBox(gp_Pnt(200, 500, 0), 100, 100, 100).Shape();

  builder.Add(cs2, sh4);
  builder.Add(cs2, sh5);
  builder.Add(cs2, sh6);

  builder.Add(comp, cs1);
  builder.Add(comp, cs2);

  Handle(AIS_Shape) ais = new AIS_Shape(comp);
  aContext->Display (ais, Standard_False);

  TopExp_Explorer exp(comp,  TopAbs_COMPSOLID);
  while(exp.More())
  {
    //printf("\n TopAbs_COMPSOLID is  there \n");
    di << "\n TopAbs_COMPSOLID is  there \n";
    exp.Next();
  }

  Handle (StdSelect_ShapeTypeFilter) filt = new StdSelect_ShapeTypeFilter(TopAbs_COMPSOLID);
  aContext->AddFilter(filt);
  Standard_DISABLE_DEPRECATION_WARNINGS
  aContext->CloseAllContexts (Standard_False);
  aContext->OpenLocalContext(); 
  aContext->ActivateStandardMode(TopAbs_SOLID);
  Standard_ENABLE_DEPRECATION_WARNINGS

  aContext->UpdateCurrentViewer();

  return 0; 
}

static Standard_Integer  OCC138LC (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  BRepPrimAPI_MakeBox box1(gp_Pnt(0, 0, 0),  gp_Pnt(100, 100, 100));
  BRepPrimAPI_MakeBox box2(gp_Pnt(120, 120, 120),  gp_Pnt(300, 300,300));
  BRepPrimAPI_MakeBox box3(gp_Pnt(320, 320, 320),  gp_Pnt(500, 500,500));

  Handle(AIS_InteractiveObject) ais1 = new AIS_Shape(box1.Shape());
  Handle(AIS_InteractiveObject) ais2 = new AIS_Shape(box2.Shape());
  Handle(AIS_InteractiveObject) ais3 = new AIS_Shape(box3.Shape());

  aContext->Display (ais1, Standard_False);
  aContext->Display (ais2, Standard_False);
  aContext->Display (ais3, Standard_False);

  aContext->AddOrRemoveSelected (ais1, Standard_False);
  aContext->AddOrRemoveSelected (ais2, Standard_False);
  aContext->AddOrRemoveSelected (ais3, Standard_False);

  di << "\n No of selected = " << aContext->NbSelected();

  aContext->InitSelected();

  int count = 1;
  while(aContext->MoreSelected())
  {
    di << "\n count is = %d" << count++;
    Handle(AIS_InteractiveObject) ais = aContext->SelectedInteractive();
    aContext->AddOrRemoveSelected (ais, Standard_False);
    aContext->InitSelected();
  }

  aContext->UpdateCurrentViewer();

  return 0; 
}

#include <BRepBndLib.hxx>
#include <Draw.hxx>
//=======================================================================
//function : OCC566
//purpose  : 
//=======================================================================
static Standard_Integer OCC566(Draw_Interpretor& di,Standard_Integer n,const char ** a)
{
  if (n < 2) {
    di<<"Usage: " << a[0] << " shape [ xmin ymin zmin xmax ymax zmax]\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 1;
  Bnd_Box B;
  BRepBndLib::AddClose(S,B);
  Standard_Real axmin,aymin,azmin,axmax,aymax,azmax;
  B.Get(axmin,aymin,azmin,axmax,aymax,azmax);
  di << axmin<<" "<< aymin<<" "<< azmin<<" "<< axmax<<" "<< aymax<<" "<< azmax;
  if (n >= 8) {
    Draw::Set(a[2],axmin) ;
    Draw::Set(a[3],aymin) ;
    Draw::Set(a[4],azmin) ;
    Draw::Set(a[5],axmax) ;
    Draw::Set(a[6],aymax) ;
    Draw::Set(a[7],azmax) ;
  }
  return 0;
}

#include <BRepFilletAPI_MakeFillet.hxx>
//=======================================================================
//function : OCC570
//purpose  : 
//=======================================================================
static Standard_Integer OCC570 (Draw_Interpretor& di, Standard_Integer argc,const char ** argv)
{
  if (argc < 2) {
    di<<"Usage: " << argv[0] << " result\n";
    return 1;
  }

  BRepPrimAPI_MakeBox mkBox(100.,100.,100.);
  TopoDS_Shape aBox = mkBox.Shape();

  TopExp_Explorer aExp;
  aExp.Init(aBox,TopAbs_WIRE);
  if (aExp.More())
  {
    TopoDS_Shape aWire = aExp.Current();

    aExp.Init(aWire,TopAbs_EDGE);
    TopoDS_Edge e1 = TopoDS::Edge(aExp.Current()); aExp.Next();
    TopoDS_Edge e2 = TopoDS::Edge(aExp.Current()); aExp.Next();
    TopoDS_Edge e3 = TopoDS::Edge(aExp.Current()); aExp.Next();
    TopoDS_Edge e4 = TopoDS::Edge(aExp.Current());

    try
    {
      OCC_CATCH_SIGNALS
        BRepFilletAPI_MakeFillet mkFillet(aBox);
      mkFillet.SetContinuity(GeomAbs_C1,.001);

      // Setup variable fillet data
      TColgp_Array1OfPnt2d t_pnt(1,4);
      t_pnt.SetValue(1,gp_Pnt2d(0.0,5.0));
      t_pnt.SetValue(2,gp_Pnt2d(0.3,15.0));
      t_pnt.SetValue(3,gp_Pnt2d(0.7,15.0));
      t_pnt.SetValue(4,gp_Pnt2d(1.0,5.0));

      // HERE:
      // It is impossible to build fillet if at least one edge
      // with variable radius is added!!! If all are constant, everything is ok.
      mkFillet.Add(t_pnt,e1);
      mkFillet.Add(5.0,e2);
      mkFillet.Add(t_pnt,e3);
      mkFillet.Add(5.0,e4);

      mkFillet.Build();
      TopoDS_Shape aFinalShape = mkFillet.Shape();

      DBRep::Set(argv[1],aFinalShape);
    }
    catch(Standard_Failure)
    {
      di << argv[0] << ": Exception in fillet\n";
      return 2;
    }
  }

  return 0;
}

#include <Law_Interpol.hxx>

static Standard_Real t3d = 1.e-4;
static Standard_Real t2d = 1.e-5;
static Standard_Real ta  = 1.e-2;
static Standard_Real fl  = 1.e-3;
static Standard_Real tapp_angle = 1.e-2;
static GeomAbs_Shape blend_cont = GeomAbs_C1;

static BRepFilletAPI_MakeFillet* Rake = 0;
static char name[100];

static void printtolblend(Draw_Interpretor& di)
{
  di<<"tolerance ang : "<<ta<<"\n";
  di<<"tolerance 3d  : "<<t3d<<"\n";
  di<<"tolerance 2d  : "<<t2d<<"\n";
  di<<"fleche        : "<<fl<<"\n";

  di<<"tolblend "<<ta<<" "<<t3d<<" "<<t2d<<" "<<fl<<"\n";
}

static Standard_Integer MKEVOL(Draw_Interpretor& di, 
                               Standard_Integer narg, 
                               const char ** a)
{
  if(Rake != 0) {delete Rake; Rake = 0;}
  printtolblend(di);
  if (narg < 3) return 1;
  TopoDS_Shape V = DBRep::Get(a[2]);
  Rake = new BRepFilletAPI_MakeFillet(V);
  Rake->SetParams(ta,t3d,t2d,t3d,t2d,fl);
  Rake->SetContinuity(blend_cont, tapp_angle);
  if (narg == 4) {
    ChFi3d_FilletShape FSh = ChFi3d_Rational;
    if (!strcasecmp(a[3], "Q")) {
      FSh = ChFi3d_QuasiAngular;
    }
    else if (!strcasecmp(a[3], "P")) {
      FSh = ChFi3d_Polynomial;
    }
    Rake->SetFilletShape(FSh);
  }
  strcpy(name, a[1]);
  return 0;
}

static Standard_Integer UPDATEVOL(Draw_Interpretor& di, 
                                  Standard_Integer narg, 
                                  const char ** a)
{
  if(Rake == 0){
    di << "MakeFillet non initialise\n";
    return 1 ;
  }
  if(narg%2 != 0 || narg < 4) return 1;
  TColgp_Array1OfPnt2d uandr(1,(narg/2)-1);
  Standard_Real Rad, Par;
  TopoDS_Shape aLocalEdge(DBRep::Get(a[1],TopAbs_EDGE));
  TopoDS_Edge E = TopoDS::Edge(aLocalEdge);
  for (Standard_Integer ii = 1; ii <= (narg/2)-1; ii++){
    Par = Draw::Atof(a[2*ii]);
    Rad = Draw::Atof(a[2*ii + 1]);
    uandr.ChangeValue(ii).SetCoord(Par,Rad);
  }
  //HELPDESK: Add law creation
  const Standard_Boolean aLawMode = !strcmp(a[0],"OCC570evollaw");
  if (aLawMode)
  {
    di<<"INFO: Using law to define variable fillet\n";
    Handle(Law_Interpol) law = new Law_Interpol;
    law->Set(uandr);
    Rake->Add(law,E);
  }
  else
    Rake->Add(uandr,E);

  return 0;
}

static Standard_Integer BUILDEVOL(Draw_Interpretor& di,
                                  Standard_Integer, 
                                  const char **)
{
  if(Rake == 0){
    di << "MakeFillet non initialise\n";
    return 1 ;
  }
  Rake->Build();
  if(Rake->IsDone()){
    TopoDS_Shape result = Rake->Shape();
    DBRep::Set(name,result);
    if(Rake != 0) {delete Rake; Rake = 0;}
    return 0;
  }
  if(Rake != 0) {delete Rake; Rake = 0;}
  return 1;
}

#include <TColGeom_SequenceOfCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomFill_NSections.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
//=======================================================================
//function : OCC606
//purpose  : 
//=======================================================================
static Standard_Integer OCC606 ( Draw_Interpretor& di, Standard_Integer n, const char ** a )
{
  if (n != 3 && n != 4)
  {
    di << "Usage : " << a[0] << " result shape [-t]\n";
    return 0;
  }

  Standard_Boolean TrimMode = (n == 4);
  if (TrimMode) di<<"INFO: Using trimmed curves...\n";
  else di<<"INFO: Using non trimmed curves...\n";

  TopoDS_Shape S = DBRep::Get(a[2]);

  TopExp_Explorer t_exp(S,TopAbs_EDGE);
  TColGeom_SequenceOfCurve n_curves1;
  TColStd_SequenceOfReal np;
  Standard_Real param = 5.0;

  for (;t_exp.More();t_exp.Next())
  {
    Standard_Real f,l;
    Handle(Geom_Curve) h_cur = BRep_Tool::Curve(TopoDS::Edge(t_exp.Current()),f,l);
    if (!h_cur.IsNull())
    {
      if (TrimMode) h_cur = new Geom_TrimmedCurve(h_cur,f,l);

      n_curves1.Append(h_cur);
      np.Append(param);
      param += 1.0;
    }
  }

  if (n_curves1.Length()>1 && !np.IsEmpty())
  {
    try
    {
      OCC_CATCH_SIGNALS
        GeomFill_NSections b_surface1(n_curves1, np);
      b_surface1.ComputeSurface();
      Handle(Geom_BSplineSurface) result_surf1 = b_surface1.BSplineSurface();
      if (!result_surf1.IsNull())
      {
        BRepBuilderAPI_MakeFace b_face1(result_surf1, Precision::Confusion());
        TopoDS_Face bsp_face1 = b_face1.Face();
        DBRep::Set(a[1],bsp_face1);
      }
    }
    catch(Standard_Failure)
    {
      di<<"ERROR: Exception in GeomFill_NSections\n";
    }
  }


  return 0;
}

//=======================================================================
//function : OCC813
//purpose  : 
//=======================================================================
static Standard_Integer OCC813 (Draw_Interpretor& di, Standard_Integer argc,const char ** argv)
{
  if (argc < 3)
  {
    di << "Usage : " << argv[0] << " U V\n";
    return 1;
  }

  Standard_CString str;
  Standard_Real U = Draw::Atof(argv[1]);
  Standard_Real V = Draw::Atof(argv[2]);

  //Between ellipse and point:

  Handle(Geom_Ellipse) ell = new Geom_Ellipse(gp_Ax2(gp_Pnt(1262.224429,425.040878,363.609716),
    gp_Dir(0.173648,0.984808,0.000000),
    gp_Dir(-0.932169,0.164367,-0.322560)), 150, 100);
  Handle(Geom_Plane) plne = new Geom_Plane(gp_Ax3(gp_Ax2(gp_Pnt(1262.224429,425.040878,363.609716),
    gp_Dir(0.173648,0.984808,0.000000),
    gp_Dir(-0.932169,0.164367,-0.322560))) );

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();

  gp_Pnt2d pt2d(U,V);
  gp_Pln pln = plne->Pln();

  str = "OCC813_pnt"; DrawTrSurf::Set(str,pt2d);

  Handle(Geom2d_Curve) curve2d = GeomAPI::To2d(ell,pln);
  Geom2dAdaptor_Curve acur(curve2d);
  Geom2dGcc_QualifiedCurve qcur(acur, GccEnt_outside);

  str = "OCC813_ell"; DrawTrSurf::Set(str,curve2d);
  if(!aContext.IsNull()) {
    Handle(AIS_Shape) aisp = 
      new AIS_Shape (BRepBuilderAPI_MakeEdge(GeomAPI::To3d(curve2d, pln)).Edge());
    aContext->Display (aisp, Standard_False);
  }

  //This does not give any solutions.
  Geom2dGcc_Lin2d2Tan lintan(qcur, pt2d, 0.1);
  di<<"OCC813 nb of solutions = "<<lintan.NbSolutions()<<"\n";

  Standard_Character abuf[16];
  Standard_CString st = abuf;

  Standard_Integer i;
  for(i=1;i<=lintan.NbSolutions();i++) {
    Sprintf(abuf,"lintan_%d",i);
    Handle(Geom2d_Line) glin = new Geom2d_Line(lintan.ThisSolution(i));
    DrawTrSurf::Set(st,glin);
    if(!aContext.IsNull()) {
      Handle(AIS_Shape) aisp = 
        new AIS_Shape (BRepBuilderAPI_MakeEdge(GeomAPI::To3d(glin, pln)).Edge());
      aContext->Display (aisp, Standard_False);
    }
  }

  if (!aContext.IsNull())
  {
    aContext->UpdateCurrentViewer();
  }

  return 0;
}

//=======================================================================
//function : OCC814
//purpose  : 
//=======================================================================
static Standard_Integer OCC814 (Draw_Interpretor& di, Standard_Integer argc,const char ** argv)
{
  if (argc > 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }

  Standard_CString str;

  //Between Ellipse and Circle:

  Handle(Geom_Circle) cir = new Geom_Circle(gp_Ax2(gp_Pnt(823.687192,502.366825,478.960440),
    gp_Dir(0.173648,0.984808,0.000000),
    gp_Dir(-0.932169,0.164367,-0.322560)), 50);
  Handle(Geom_Ellipse) ell = new Geom_Ellipse(gp_Ax2(gp_Pnt(1262.224429,425.040878,363.609716),
    gp_Dir(0.173648,0.984808,0.000000),
    gp_Dir(-0.932169,0.164367,-0.322560)), 150, 100);
  Handle(Geom_Plane) plne = new Geom_Plane(gp_Ax3(gp_Ax2(gp_Pnt(1262.224429,425.040878,363.609716),
    gp_Dir(0.173648,0.984808,0.000000),
    gp_Dir(-0.932169,0.164367,-0.322560))) );

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();

  gp_Pln pln = plne->Pln();
  Handle(Geom2d_Curve) curve2d = GeomAPI::To2d(ell, pln);
  Handle(Geom2d_Curve) fromcurve2d = GeomAPI::To2d(cir, pln);

  str = "OCC814_cir"; DrawTrSurf::Set(str,curve2d);
  str = "OCC814_ell"; DrawTrSurf::Set(str,fromcurve2d);
  if(!aContext.IsNull()) {
    Handle(AIS_Shape) aisp = 
      new AIS_Shape (BRepBuilderAPI_MakeEdge(GeomAPI::To3d(curve2d, pln)).Edge());
    aContext->Display (aisp, Standard_False);
  }
  if(!aContext.IsNull()) {
    Handle(AIS_Shape) aisp = 
      new AIS_Shape (BRepBuilderAPI_MakeEdge(GeomAPI::To3d(fromcurve2d, pln)).Edge());
    aContext->Display (aisp, Standard_False);
  }

  Geom2dAdaptor_Curve acur(curve2d), afromcur(fromcurve2d);

  Geom2dGcc_QualifiedCurve qcur(acur, GccEnt_outside) ;
  Geom2dGcc_QualifiedCurve qfromcur(afromcur, GccEnt_outside) ;

  //This does not give any solutions.
  Geom2dGcc_Lin2d2Tan lintan(qcur, qfromcur, 0.1);
  di<<"OCC814 nb of solutions = "<<lintan.NbSolutions()<<"\n";

  Standard_Character abuf[16];
  Standard_CString st = abuf;

  Standard_Integer i;
  for(i=1;i<=lintan.NbSolutions();i++) {
    Sprintf(abuf,"lintan_%d",i);
    Handle(Geom2d_Line) glin = new Geom2d_Line(lintan.ThisSolution(i));
    DrawTrSurf::Set(st,glin);
    if(!aContext.IsNull()) {
      Handle(AIS_Shape) aisp = 
        new AIS_Shape (BRepBuilderAPI_MakeEdge(GeomAPI::To3d(glin, pln)).Edge());
      aContext->Display (aisp, Standard_False);
    }
  }

  if (!aContext.IsNull())
  {
    aContext->UpdateCurrentViewer();
  }

  return 0;
}

#include <ShapeAnalysis_Wire.hxx>
#include <IntRes2d_SequenceOfIntersectionPoint.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <ShapeFix_Wire.hxx>
//=======================================================================
//function : OCC884
//purpose  : 
//=======================================================================
static Standard_Integer OCC884 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc < 3 || argc > 5)
  {
    di << "Usage : " << argv[0] << " result shape [toler [maxtoler]]\n";
    return 1;
  }

  TopoDS_Shape shape = DBRep::Get(argv[2]); //read the shape

  TopExp_Explorer exp(shape, TopAbs_EDGE);
  if (!exp.More())
  {
    di<<"Error: no edge found\n";
    return 1;
  }

  BRep_Builder builder;
  TopoDS_Wire wire;
  builder.MakeWire(wire);
  builder.Add(wire, TopoDS::Edge(exp.Current()));

  // HelpDesk: Create planar face if possible
  TopoDS_Face face = BRepBuilderAPI_MakeFace(wire,Standard_True);

  Handle(ShapeAnalysis_Wire) advWA = new ShapeAnalysis_Wire;
  advWA->Load(wire);
  advWA->SetFace(face);
  advWA->SetPrecision(0.001);

  IntRes2d_SequenceOfIntersectionPoint points2d;
  TColgp_SequenceOfPnt points3d;

  di << "Info: CheckSelfIntersectingEdge = " << (Standard_Integer) advWA->CheckSelfIntersectingEdge(1,points2d,points3d) << "\n";

  ShapeExtend_Status status = ShapeExtend_FAIL1;
  if (advWA->StatusSelfIntersection(status))
    di << "Info: No P Curve found in the edge\n";

  status = ShapeExtend_FAIL2; 
  if (advWA->StatusSelfIntersection(status))
    di << "Info: No Vertices found in the edge\n";

  status = ShapeExtend_DONE1; 
  if (advWA->StatusSelfIntersection(status))
    di << "Info: Self-intersection found in the edge\n";

  Standard_Integer i, num = points2d.Length();
  di << "Info: No. of self-intersection points : " << num << "\n";

  char str[80];
  Standard_CString aName = str;
  for (i = 1; i <= num; ++i)
  { 
    gp_Pnt pt = points3d(i); 
    di << "Info: Intersecting pt : (" << pt.X() << ", " << pt.Y() << ", " << pt.Z() << ")\n";
    Sprintf(str,"p_%d",i);
    DrawTrSurf::Set(aName,pt);
  }

  Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
  sfw->Load(wire);
  sfw->SetFace(face);

  if (argc > 3) sfw->SetPrecision(Draw::Atof(argv[3])/*0.1*/);
  if (argc > 4) sfw->SetMaxTolerance(Draw::Atof(argv[4]));
  di << "Info: Precision is set to " << sfw->Precision() << "\n";
  di << "Info: MaxTolerance is set to " << sfw->MaxTolerance() << "\n";

  sfw->ModifyTopologyMode() = 1;
  sfw->ModifyGeometryMode() = 1; 
  sfw->FixSelfIntersectingEdgeMode() = 1;
  //printf("Info: FixSelfIntersection = %d\n",sfw->FixSelfIntersection());
  di << "Info: FixSelfIntersection = " << (Standard_Integer) sfw->FixSelfIntersection() << "\n";

  DBRep::Set(argv[1],sfw->Wire());

  status = ShapeExtend_OK;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_OK : No intersection found\n";
  //printf("Info: ShapeExtend_OK : No intersection found\n");

  status = ShapeExtend_FAIL1;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_FAIL1 : analysis failed (edge has no pcurve, or no vertices etc.)\n";

  status = ShapeExtend_FAIL2;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_FAIL2 : self-intersection was found, but not fixed because of limit of increasing tolerance (MaxTolerance)\n";

  status = ShapeExtend_FAIL3;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_FAIL3 : intercestion of non adjacent edges found, but not fixed because of limit of increasing tolerance (MaxTolerance)\n";

  status = ShapeExtend_DONE1;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE1 : tolerance of vertex was increased to fix self-intersection\n";

  status = ShapeExtend_DONE2;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE2 : vertex was moved to fix self-intersection\n";

  status = ShapeExtend_DONE3;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE3 : some edges were removed because of intersection\n";

  status = ShapeExtend_DONE4;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE4 : pcurve(s) was(were) modified\n";

  status = ShapeExtend_DONE5;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE5 : non adjacent intersection fixed by increasing tolerance of vertex(vertices)\n";

  status = ShapeExtend_DONE6;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE6 : tolerance of edge was increased to hide intersection\n";

  status = ShapeExtend_DONE7;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE7 : range of some edges was decreased to avoid intersection\n";

  return 0;
}

#include <Aspect_FillMethod.hxx>
//=======================================================================
//function : OCC1188
//purpose  : 
//=======================================================================
static Standard_Integer OCC1188 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc < 2 || argc > 3)
  {
    di << "Usage : " << argv[0] << " imagefile [filltype] : Load image as background\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) AISContext = ViewerTest::GetAISContext();
  if(AISContext.IsNull()) 
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  Aspect_FillMethod aFillType = Aspect_FM_CENTERED;
  if (argc == 3)
  {
    const char* szType = argv[2];
    if      (strcmp(szType, "NONE"    ) == 0) aFillType = Aspect_FM_NONE;
    else if (strcmp(szType, "CENTERED") == 0) aFillType = Aspect_FM_CENTERED;
    else if (strcmp(szType, "TILED"   ) == 0) aFillType = Aspect_FM_TILED;
    else if (strcmp(szType, "STRETCH" ) == 0) aFillType = Aspect_FM_STRETCH;
    else
    {
      di << "Wrong fill type : " << szType << "\n";
      di << "Must be one of CENTERED, TILED, STRETCH, NONE\n";
      return 1;
    }
  }

  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  V3dView->SetBackgroundImage(argv[1], aFillType, Standard_True);

  return 0;
}

#include <Graphic3d_MaterialAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ShadingAspect.hxx>
//=======================================================================
//function : OCC1174_1
//purpose  : 
//=======================================================================
static Standard_Integer OCC1174_1 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " shape\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if(anAISContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(argv[1]);

  Handle(AIS_Shape) anAisIO = new AIS_Shape(aShape);

  Quantity_Color aColF(0.0, 0.4, 0.0, Quantity_TOC_RGB);
  Quantity_Color aColB(0.0, 0.0, 0.6, Quantity_TOC_RGB);

  Handle(Prs3d_Drawer) aDrawer = anAisIO->Attributes();
  Handle(Prs3d_ShadingAspect) aShadingAspect = aDrawer->ShadingAspect();

  Graphic3d_MaterialAspect aFront = aShadingAspect->Material(Aspect_TOFM_FRONT_SIDE);
  aFront.SetAmbientColor(aColF);
  aFront.SetDiffuseColor(aColF);
  aFront.SetSpecularColor(aColF);
  aFront.SetEmissiveColor(aColF);
  aFront.SetTransparency(0.0);
  aShadingAspect->SetMaterial(aFront,Aspect_TOFM_FRONT_SIDE);

  Graphic3d_MaterialAspect aBack = aShadingAspect->Material(Aspect_TOFM_BACK_SIDE);
  aBack.SetAmbientColor(aColB);
  aBack.SetDiffuseColor(aColB);
  aBack.SetSpecularColor(aColB);
  aBack.SetEmissiveColor(aColB);
  aBack.SetTransparency(0.0);
  aShadingAspect->SetMaterial(aBack,Aspect_TOFM_BACK_SIDE);

  aDrawer->SetShadingAspect (aShadingAspect);

  anAISContext->Display (anAisIO, 1, 0, Standard_True);

  Standard_Real r, g, b; 
  aShadingAspect->Color(Aspect_TOFM_FRONT_SIDE).Values(r,g,b, Quantity_TOC_RGB);
  di << "Info: color on front side (" << r << "," << g << "," << b << ")\n";
  aShadingAspect->Color(Aspect_TOFM_BACK_SIDE).Values(r,g,b, Quantity_TOC_RGB);
  di << "Info: color on back side (" << r << "," << g << "," << b << ")\n";

  return 0;
}

//=======================================================================
//function : OCC1174_2
//purpose  : 
//=======================================================================
static Standard_Integer OCC1174_2 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " shape\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) AISContext = ViewerTest::GetAISContext();
  if(AISContext.IsNull()) 
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  TopoDS_Shape sh = DBRep::Get(argv[1]);

  Handle(AIS_Shape) ais = new AIS_Shape(sh); 
  AISContext->Display (ais, 1, 0, Standard_False);
  AISContext->SetMaterial (ais, Graphic3d_NOM_SHINY_PLASTIC, Standard_False);

  Quantity_Color colf(0.0, 0.4, 0.0, Quantity_TOC_RGB); 
  Quantity_Color colb(0.0, 0.0, 0.6, Quantity_TOC_RGB); 
  Handle(Prs3d_ShadingAspect) sa = ais->Attributes()->ShadingAspect(); 

  Graphic3d_MaterialAspect front = sa->Material(Aspect_TOFM_FRONT_SIDE); 
  front.SetAmbientColor(colf); 
  front.SetDiffuseColor(colf); 
  front.SetSpecularColor(colf); 
  front.SetEmissiveColor(colf); 
  front.SetTransparency (0.4f);
  sa->SetMaterial(front,Aspect_TOFM_FRONT_SIDE); 

  Graphic3d_MaterialAspect back = sa->Material(Aspect_TOFM_BACK_SIDE); 
  back.SetAmbientColor(colb); 
  back.SetDiffuseColor(colb); 
  back.SetSpecularColor(colb); 
  back.SetEmissiveColor(colb); 
  back.SetTransparency (0.2f);
  sa->SetMaterial(back,Aspect_TOFM_BACK_SIDE); 

  AISContext->Redisplay (ais, 1, 0);

  return 0;
}

#include <TopoDS_Solid.hxx>
#include <BRepFeat_MakeDPrism.hxx>
//=======================================================================
//function : OCCN1
//purpose  : FEATURE OPERATION (BOSS and SLOT)
//=======================================================================
static Standard_Integer OCCN1 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " angle fuse(1 for boss / 0 for slot) length\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) 
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  Standard_Real    angle  = Draw::Atof(argv[1]);
  Standard_Integer fuse   = Draw::Atoi(argv[2]);
  Standard_Real    length = Draw::Atof(argv[3]);

  BRepBuilderAPI_MakeEdge edge1(gp_Pnt(0, 0, 0), gp_Pnt(50, 0, 0));
  BRepBuilderAPI_MakeEdge edge2(gp_Pnt(50, 0, 0), gp_Pnt(50, 50, 0));
  BRepBuilderAPI_MakeEdge edge3(gp_Pnt(50, 50, 0), gp_Pnt(0, 50, 0));
  BRepBuilderAPI_MakeEdge edge4(gp_Pnt(0, 50, 0), gp_Pnt(0, 0, 0));
  TopoDS_Edge ted1 = edge1.Edge();
  TopoDS_Edge ted2 = edge2.Edge();
  TopoDS_Edge ted3 = edge3.Edge();
  TopoDS_Edge ted4 = edge4.Edge();

  BRepBuilderAPI_MakeWire wire(ted1, ted2, ted3, ted4);
  TopoDS_Wire twire = wire.Wire();

  BRepBuilderAPI_MakeFace face(twire);
  TopoDS_Face tface = face.Face();
  ////////Handle(AIS_Shape) face_ais = new AIS_Shape( tface );
  ////////aContext->Display(face_ais);

  TopoDS_Solid box;
  BRepPrimAPI_MakeBox block(gp_Pnt(-50, -50, -150), 150, 150, 100);
  box = block.Solid();

  TopoDS_Face top_face;
  top_face = block.TopFace();

  BRepFeat_MakeDPrism slot(box, tface, top_face, angle, fuse,
    Standard_True);

  slot.Perform(length);
  TopoDS_Shape S2 = slot.Shape();

  DBRep::Set("OCCN1_tface",tface);
  DBRep::Set("OCCN1_slot",S2);

  return 0;
}

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepAlgoAPI_Section.hxx>
//=======================================================================
//function : OCCN2
//purpose  : BOOLEAN OPERATION
//=======================================================================
static Standard_Integer OCCN2 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc > 2) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) 
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  BRepPrimAPI_MakeCylinder cylinder(50,200);
  TopoDS_Shape cylinder_sh = cylinder.Shape();

  BRepPrimAPI_MakeSphere sphere(gp_Pnt(60,0,100),50);
  TopoDS_Shape sphere_sh = sphere.Shape();

  di << "BRepAlgoAPI_Section section(cylinder_sh, sphere_sh)\n";
  BRepAlgoAPI_Section section(cylinder_sh, sphere_sh);
  if (!section.IsDone()){
    di << "Error performing intersection: not done.\n";
  }
  const TopoDS_Shape& shape = section.Shape();

  DBRep::Set("OCCN2_cylinder",cylinder_sh);
  DBRep::Set("OCCN2_sphere",sphere_sh);
  DBRep::Set("OCCN2_section",shape);

  return 0;
}

#include <TColgp_Array1OfPnt.hxx>
#include <Geom_BezierCurve.hxx>

static Standard_Integer OCC2569 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) 
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if(argc != 3) {
    di << "Usage : " << argv[0] << " nbpoles result\n";
    return 1;
  }

  int poles=Draw::Atoi(argv[1]); 

  TColgp_Array1OfPnt arr(1, poles); 
  for(int i=1; i<=poles; i++) 
    arr.SetValue(i, gp_Pnt(i+10, i*2+20, i*3+45)); 

  Handle(Geom_BezierCurve) bez = new Geom_BezierCurve(arr); 
  if(bez.IsNull()) {
    di << "\n The curve is not created.\n";
  } else {
    di << "\n Degree = " << bez->Degree() << "\n";   
  }
  TopoDS_Edge sh = BRepBuilderAPI_MakeEdge(bez).Edge(); 
  Handle(AIS_Shape) ais = new AIS_Shape(sh); 
  aContext->Display (ais, Standard_True);
  DrawTrSurf::Set(argv[2],bez);
  return 0;
}

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>

static Standard_Integer OCC1642 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 10) {
    di<<"Usage: " << argv[0] << " FinalWare FinalFace InitWare InitFace shape FixReorder FixDegenerated FixConnected FixSelfIntersection\n";
    return 1;
  }

  TopoDS_Shape shape = DBRep::Get(argv[5]);

  TopExp_Explorer exp(shape, TopAbs_WIRE);
  TopoDS_Shape wire = exp.Current();

  Handle(ShapeAnalysis_Wire) advWA = new ShapeAnalysis_Wire;
  advWA->Load(TopoDS::Wire(wire));

  DBRep::Set(argv[3],wire);

  TopoDS_Face face =
    BRepBuilderAPI_MakeFace(TopoDS::Wire(wire),Standard_True);

  DBRep::Set(argv[4],face);

  advWA->SetFace(face);
  Standard_Real precision_to_ana = 0.0001;
  advWA->SetPrecision(precision_to_ana);

  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(wire, TopAbs_EDGE, M);

  int j= 1;
  for(j= 1; j<=M.Extent(); ++j) {
    Standard_Integer num=1;
    IntRes2d_SequenceOfIntersectionPoint points2d;
    TColgp_SequenceOfPnt points3d;

    cout << "\n j =" << j << ",  CheckSelfIntersectingEdge = " <<
      advWA->CheckSelfIntersectingEdge(j, points2d, points3d);


    ShapeExtend_Status status = ShapeExtend_FAIL1;
    if(advWA->StatusSelfIntersection(status))

      status = ShapeExtend_FAIL2;
    if(advWA->StatusSelfIntersection(status))
      di << "\n No Vertices found in the edge";

    status = ShapeExtend_DONE1;
    if(advWA->StatusSelfIntersection(status))
      di << "\n Self-intersection found in the edge";

    num = points2d.Length();
    di << "\n No. of self-intersecting edges : " << num;

    for(int i=1; i<=num; ++i) {
      gp_Pnt pt = points3d(i);
      di << "\n Intersecting pt : (" << pt.X() << ", " << pt.Y()<< ", " << pt.Z() << ")";
    }

  }

  Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
  sfw->Load(TopoDS::Wire(wire));
  sfw->SetFace(face);
  sfw->SetPrecision(Precision::Confusion());

  sfw->FixReorderMode() = 1;
  sfw->ClosedWireMode() = 1;
  sfw->FixDegeneratedMode() = 1;
  sfw->FixConnectedMode() = 1;
  sfw->ModifyTopologyMode() = 1;
  sfw->ModifyGeometryMode() = 1;
  sfw->FixSelfIntersectionMode() = 1;
  sfw->FixSelfIntersectingEdgeMode() = 1;
  sfw->FixIntersectingEdgesMode() = 1;
  sfw->FixNonAdjacentIntersectingEdgesMode() = 1;
  sfw->FixEdgeCurvesMode() = 1;

  sfw->ModifyRemoveLoopMode() = 1;
  sfw->SetMaxTolerance(1.0);

  di << "\n FixReorder = " << (Standard_Integer) sfw->FixReorder();
  di << "\n FixDegenerated = " << (Standard_Integer) sfw->FixDegenerated();
  di << "\n FixConnected = " << (Standard_Integer) sfw->FixConnected();
  di << "\n";
  di << "\n FixSelfIntersection = " << (Standard_Integer) sfw->FixSelfIntersection();
  di << "\n";

  Draw::Set(argv[6],sfw->FixReorder()) ;
  Draw::Set(argv[7],sfw->FixDegenerated()) ;
  Draw::Set(argv[8],sfw->FixConnected()) ;
  Draw::Set(argv[9],sfw->FixSelfIntersection()) ;

  ShapeExtend_Status status = ShapeExtend_OK;
  if(sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_OK : No intersection found";

  status = ShapeExtend_FAIL1;
  if(sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_FAIL1 : analysis failed (edge has no pcurve,or no vertices etc.)";

  status = ShapeExtend_FAIL2;
  if(sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_FAIL2 : self-intersection was found, but not fixed because of limit of increasing tolerance (MaxTolerance)";

  status = ShapeExtend_FAIL3;
  if(sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_FAIL3 : intercestion of non adjacent edges found, but not fixed because of limit of increasing tolerance (MaxTolerance)";

  status = ShapeExtend_DONE1;
  if(sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE1 : tolerance of vertex was increased to fix self-intersection";

  status = ShapeExtend_DONE2;
  if(sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE2 : vertex was moved to fix self-intersection";

  status = ShapeExtend_DONE3;
  if(sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE3 : some edges were removed because of intersection";

  status = ShapeExtend_DONE4;
  if(sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE4 : pcurve(s) was(were) modified";

  status = ShapeExtend_DONE5;
  if(sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE5 : non adjacent intersection fixed by increasing tolerance of vertex(vertices)";

  status = ShapeExtend_DONE6;
  if(sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE6 : tolerance of edge was increased to hide intersection";

  status = ShapeExtend_DONE7;
  if(sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE7 : range of some edges was decreased to avoid intersection";

  TopoDS_Wire finalwire = sfw->Wire();

  DBRep::Set(argv[1],finalwire);


  advWA->Load(TopoDS::Wire(finalwire));

  TopoDS_Face fface =
    BRepBuilderAPI_MakeFace(TopoDS::Wire(finalwire),Standard_True);

  DBRep::Set(argv[2],fface);

  advWA->SetFace(fface);

  TopTools_IndexedMapOfShape fM;
  TopExp::MapShapes(finalwire, TopAbs_EDGE, fM);

  for(j= 1; j<=fM.Extent(); ++j) {
    Standard_Integer num=1;
    IntRes2d_SequenceOfIntersectionPoint points2d;
    TColgp_SequenceOfPnt points3d;

    di << "\n j =" << j << ",  CheckSelfIntersectingEdge = " <<
      (Standard_Integer) advWA->CheckSelfIntersectingEdge(j, points2d, points3d);


    status = ShapeExtend_FAIL1;
    if(advWA->StatusSelfIntersection(status))

      status = ShapeExtend_FAIL2;
    if(advWA->StatusSelfIntersection(status))
      di << "\n No Vertices found in the edge";

    status = ShapeExtend_DONE1;
    if(advWA->StatusSelfIntersection(status))
      di << "\n Self-intersection found in the edge";

    num = points2d.Length();
    di << "\n No. of self-intersecting edges : " << num;

    for(int i=1; i<=num; ++i) {
      gp_Pnt pt = points3d(i);
      di << "\n Intersecting pt : (" << pt.X() << ", " << pt.Y() << ", " << pt.Z() << ")";
    }

  }

  di << "\n";

  return 0;
}

void QABugs::Commands_17(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add ("BUC60842", "BUC60842", __FILE__, BUC60842, group);
  theCommands.Add ("BUC60843", "BUC60843 result_shape name_of_circle name_of_curve [ par1 [ tol ] ]", __FILE__, BUC60843, group);
  theCommands.Add ("BUC60970", "BUC60970 shape result", __FILE__, BUC60970, group);
  theCommands.Add ("BUC60818", "BUC60818", __FILE__, BUC60818, group);
  theCommands.Add ("BUC60915", "BUC60915", __FILE__, BUC60915_1, group);
  theCommands.Add ("OCC138", "OCC138", __FILE__, OCC138, group);
  theCommands.Add ("OCC353","OCC353",__FILE__,OCC353,group);
  theCommands.Add ("OCC280","OCC280 hlr=0/1 setsurfecedetail=0/1; set perspecrive view",__FILE__,OCC280,group);
  theCommands.Add ("OCC232", "OCC232", __FILE__, OCC232 , group);
  theCommands.Add ("OCC138LC", "OCC138LC", __FILE__, OCC138LC, group);
  theCommands.Add ("OCC566", "OCC566 shape [ xmin ymin zmin xmax ymax zmax] ; print bounding box", __FILE__, OCC566, group);
  theCommands.Add ("OCC570", "OCC570 result", __FILE__, OCC570, group);

  theCommands.Add("OCC570mkevol",
    "OCC570mkevol result object (then use updatevol) [R/Q/P]; mkevol",
    __FILE__, MKEVOL,group);
  theCommands.Add("OCC570updatevol",
    "OCC570updatevol edge u1 rad1 u2 rad2 ...; updatevol",
    __FILE__, UPDATEVOL,group);
  theCommands.Add("OCC570updatevollaw",
    "OCC570updatevollaw edge u1 rad1 u2 rad2 ...; updatevollaw",
    __FILE__, UPDATEVOL,group);
  theCommands.Add("OCC570buildevol",
    "OCC570buildevol; end of the evol fillet computation",
    __FILE__, BUILDEVOL,group);

  theCommands.Add ("OCC606", "OCC606 result shape [-t]", __FILE__, OCC606, group);

  theCommands.Add ("OCC813", "OCC813 U V", __FILE__, OCC813, group);
  theCommands.Add ("OCC814", "OCC814", __FILE__, OCC814, group);
  theCommands.Add ("OCC884", "OCC884 result shape [toler [maxtoler]]", __FILE__, OCC884, group);
  theCommands.Add ("OCC1188", "OCC1188 imagefile [filltype] : Load image as background", __FILE__, OCC1188, group);

  theCommands.Add ("OCC1174_1", "OCC1174_1 shape", __FILE__, OCC1174_1, group);
  theCommands.Add ("OCC1174_2", "OCC1174_2 shape", __FILE__, OCC1174_2, group);

  theCommands.Add ("OCCN1", "OCCN1 angle fuse(1 for boss / 0 for slot) length", __FILE__, OCCN1, group);
  theCommands.Add ("OCCN2", "OCCN2", __FILE__, OCCN2, group);

  theCommands.Add ("OCC2569", "OCC2569 nbpoles result", __FILE__, OCC2569, group);

  theCommands.Add ("OCC1642", "OCC1642 FinalWare FinalFace InitWare InitFace shape FixReorder FixDegenerated FixConnected FixSelfIntersection", __FILE__, OCC1642, group);

  return;
}
