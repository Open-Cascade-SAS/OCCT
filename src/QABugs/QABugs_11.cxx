// Created on: 2002-03-20
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
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp.hxx>
#include <gp_Trsf.hxx>
#include <AIS_Trihedron.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <TopoDS_Solid.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <IGESToBRep_Reader.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <GCPnts_UniformDeflection.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <IGESToBRep.hxx>
#include <V3d_Viewer.hxx>
#include <BRepAdaptor_CompCurve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Overflow.hxx>
#include <Standard_Underflow.hxx>
#include <Standard_DivideByZero.hxx>
#include <OSD_SIGSEGV.hxx>
#include <OSD_Exception_ACCESS_VIOLATION.hxx>
#include <OSD_Exception_STACK_OVERFLOW.hxx>
#include <OSD.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_StepModelType.hxx>
#include <Interface_Static.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Standard_Failure.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dConvert_BSplineCurveToBezierCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <BRep_Tool.hxx>
#include <GeomProjLib.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_CopyLabel.hxx>
#include <NCollection_Vector.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <AIS_ColorScale.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <gp_GTrsf.hxx>
#include <Poly_Triangulation.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESEntity.hxx>
#include <V3d_View.hxx>
#include <BRepFeat_SplitShape.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

#if ! defined(_WIN32)
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
#else
Standard_EXPORT ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
#endif

static Standard_Integer  OCC128 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv)
{
  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] ;
    return 1;
  }

  Handle(Geom_Axis2Placement) aTrihedronAxis = new Geom_Axis2Placement(gp::XOY());

  gp_Trsf trsf1;
  trsf1.SetTranslation(gp_Vec(100, 100, 0));
  aTrihedronAxis->Transform(trsf1);
  Handle(AIS_Trihedron) myTrihedron = new AIS_Trihedron(aTrihedronAxis);
  myTrihedron->SetColor(Quantity_NOC_LIGHTSTEELBLUE4);
  myTrihedron->SetSize(100);
  myAISContext->Display(myTrihedron, Standard_True);

//  TopoDS_Shape shape1 = (TopoDS_Shape) BRepPrimAPI_MakeBox(50,50,50);
  TopoDS_Shape shape1 = BRepPrimAPI_MakeBox(50,50,50).Shape();
  Handle(AIS_Shape) AS = new AIS_Shape(shape1);
  AS->SetDisplayMode(1);
  Graphic3d_MaterialAspect mat(Graphic3d_NOM_PLASTIC);
  AS->SetMaterial(mat);
  AS->SetColor(Quantity_NOC_RED);
  myAISContext->Display (AS, Standard_False);

  gp_Trsf TouchTrsf;
  TouchTrsf.SetTranslation(gp_Vec(20, 20, 0));

  myAISContext->ResetLocation(AS);
  myAISContext->SetLocation(AS , TouchTrsf) ;
  myAISContext->Redisplay(AS, Standard_True);

 return 0;
}

  // Remove as bad version of QAAddOrRemoveSelected from QADraw
//static Standard_Integer OCC129 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
//{
//  if( argc != 3) {
//    di<<"Usage : " << argv[0] << " shape islocal\n";
//    return 1;
//  }
//  //get AIS_Shape:
//  Handle(AIS_InteractiveContext) anAISCtx = ViewerTest::GetAISContext();
//
// //   ViewerTest_DoubleMapOfInteractiveAndName& aMap =
// //                          ViewerTest::GetDataMapOfAIS ();
//  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
//
//  TCollection_AsciiString aName(argv[1]);
//  Handle(AIS_InteractiveObject) AISObj;
//
//  if(aMap.IsBound2(aName)) {
//    AISObj = aMap.Find2(aName);
//    if(AISObj.IsNull()){
//      di<<"No interactive object \n";
//      return 1;
//    }
//
//    Standard_Integer aNum = -1;
//
//    if(Draw::Atoi(argv[2])) {
//      aNum = anAISCtx->OpenLocalContext();
//    }
//
//    if(anAISCtx->HasOpenedContext()){
//      anAISCtx->InitSelected();
//      anAISCtx->AddOrRemoveSelected(AISObj);
//    }
//    else {
//      anAISCtx->InitCurrent();
//      anAISCtx->AddOrRemoveCurrentObject(AISObj);
//    }
//
//    if(aNum >= 0) {
// //      anAISCtx->CloseLocalContext(aNum);
//    }
//
//    return 0;
//  }
//  //select this shape:
//  else {
//    di<<"Use 'vdisplay' before";
//    return 1;
//  }
//}

static Standard_Integer OCC136 (Draw_Interpretor& di, Standard_Integer argc, const char ** /*argv*/)
{
  if(argc > 1){
    di<<"Usage: OCC136\n";
    return 1;
  }

  //create some primitives:
  // Two basic points:
  Standard_Real Size=100;
  gp_Pnt P0(0,0,0), P1(Size,Size,Size);
  //box
  TopoDS_Solid aBox = BRepPrimAPI_MakeBox(P0,P1);
  //sphere
  TopoDS_Solid aSphere = BRepPrimAPI_MakeSphere (Size*0.5);
  //cone
  gp_Ax2 anAx2(P1, gp_Dir(1,1,1));
  TopoDS_Solid aCone = BRepPrimAPI_MakeCone(anAx2, Size*0.7, Size*0.3, Size);
  //cylinder
  anAx2.SetLocation(gp_Pnt(Size,0,0));
  anAx2.SetDirection(gp_Dir(-1,-1,1));
  TopoDS_Solid aCyl = BRepPrimAPI_MakeCylinder(anAx2, Size*0.5, Size);

  Handle(AIS_InteractiveContext) anAISCtx = ViewerTest::GetAISContext();
  if(anAISCtx.IsNull()){
    di<<"Null interactive context. Use 'vinit' at first.\n";
    return 1;
  }

  anAISCtx->EraseAll (Standard_False);

  //load primitives to context
  Handle(AIS_InteractiveObject) aSh1 = new AIS_Shape(aBox);
  anAISCtx->Display (aSh1, Standard_False);

  Handle(AIS_InteractiveObject) aSh2 = new AIS_Shape(aSphere);
  anAISCtx->Display (aSh2, Standard_False);

  Handle(AIS_InteractiveObject) aSh3 = new AIS_Shape(aCone);
  anAISCtx->Display (aSh3, Standard_False);

  Handle(AIS_InteractiveObject) aSh4 = new AIS_Shape(aCyl);
  anAISCtx->Display (aSh4, Standard_False);

  //set selected
  anAISCtx->InitSelected();
  anAISCtx->AddOrRemoveSelected (aSh1, Standard_False);
  anAISCtx->AddOrRemoveSelected (aSh2, Standard_False);
  anAISCtx->AddOrRemoveSelected (aSh3, Standard_False);
  anAISCtx->AddOrRemoveSelected (aSh4, Standard_False);

  //remove all this objects from context
  anAISCtx->Remove (aSh1, Standard_False);
  anAISCtx->Remove (aSh2, Standard_False);
  anAISCtx->Remove (aSh3, Standard_False);
  anAISCtx->Remove (aSh4, Standard_False);

  anAISCtx->UpdateCurrentViewer();
  return 0;
}

static int BUC60610(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc < 2){
    printf("Usage: %s  iges_input [name]\n",argv[0]);
    return(1);
  }
  Standard_Character *Ch = NULL;

  if(argc > 2) {
    Ch = new Standard_Character[strlen(argv[2])+3];
  }
  IGESToBRep_Reader IR;
  IR.LoadFile (argv[1]);
  IR.Clear();
  IR.TransferRoots();
  TopoDS_Shape aTopShape = IR.OneShape();
  TopExp_Explorer ex(aTopShape, TopAbs_EDGE);
  Standard_Integer i=0;
  for( ; ex.More(); ex.Next()){
    const TopoDS_Edge &E = TopoDS::Edge(ex.Current());
    BRepAdaptor_Curve aCurve(E);
    GCPnts_UniformDeflection plin(aCurve, 0.1);
    di << "Num points = " << plin.NbPoints() << "\n";
    if(argc > 2) {
      i++;
      Sprintf(Ch,"%s_%i",argv[2],1);
      DBRep::Set(Ch,E);
    }
  }
  return (1);
}

//====================================================
//
// Following code is inserted from
// /dn03/KAS/dev/QAopt/src/QADraw/QADraw_TOPOLOGY.cxx
// ( 75455 Apr 16 18:59)
//
//====================================================

//OCC105
#include <BRepTools_WireExplorer.hxx>
#include <BRep_Tool.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <TopExp.hxx>

//
// usage : OCC105 shape
//
// comments:
//GCPnts_UniformAbscissa returns bad end point foe first edge. Its value is

//Value Pnt = -338.556216693211 -394.465571897208 0
//should be
//Value Pnt = -307.47165394 -340.18073533 0

static int OCC105(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2){
    di<<"Usage : OCC105 shape\n";
    return 1;
  }
//  TopoDS_Wire myTopoDSWire = TopoDS::Wire(DBRep::Get("aa.brep"));
  TopoDS_Wire myTopoDSWire = TopoDS::Wire(DBRep::Get(argv[1]));
  Standard_Real l = 0.5; //Draw::Atof(argv[2]);
  // Find the first vertex of the wire
  BRepTools_WireExplorer wire_exp(myTopoDSWire);
  TopoDS_Vertex vlast;
  {
    TopoDS_Vertex vw1, vw2;
    TopExp::Vertices(myTopoDSWire,vw1,vw2);
    TopoDS_Vertex ve1, ve2;
    TopoDS_Edge edge = TopoDS::Edge(wire_exp.Current());
    TopExp::Vertices(edge,ve1,ve2);
    if (vw1.IsSame(ve1) || vw1.IsSame(ve2))
      vlast = vw1;
    else {
//      assert(vw2.IsSame(ve1) || vw2.IsSame(ve2));
      vlast = vw2;
    }
  }
  for ( ; wire_exp.More(); wire_exp.Next())
    {
      di << "\n\n New Edge \n"   << "\n";
      Standard_Real newufirst, newulast;
      TopoDS_Edge edge = TopoDS::Edge(wire_exp.Current());
      Standard_Real ufirst, ulast;
      Handle(Geom_Curve) acurve;
      TopoDS_Vertex ve1, ve2;
      TopExp::Vertices(edge,ve1,ve2);
      if (ve1.IsSame(vlast))
	{
          acurve = BRep_Tool::Curve(edge, ufirst, ulast);
          newufirst = ufirst;
          newulast  = ulast;
          vlast = ve2;
	}
      else
	{
//          assert(ve2.IsSame(vlast));
//          assert ( wire_exp.Orientation( ) == TopAbs_REVERSED );
          acurve = BRep_Tool::Curve( edge, ufirst, ulast );
          newufirst = acurve->ReversedParameter( ufirst );
          newulast  = acurve->ReversedParameter( ulast );
          acurve = acurve->Reversed( );
          vlast = ve1;
	}

      GeomAdaptor_Curve   curve;
      GCPnts_UniformAbscissa  algo;
      curve.Load(acurve);
      algo.Initialize( curve, l, newufirst, newulast );
      if (!algo.IsDone())
        di << "Not Done!!!"   << "\n";
      for (Standard_Integer Index = 1; Index<=algo.NbPoints();Index++) {
        Standard_Real t = algo.Parameter(Index);
        gp_Pnt      pt3 = curve.Value(t);
        di << "Parameter t = " << t   << "\n";
        di << "Value Pnt = " << pt3.X()<<" " <<pt3.Y()<<" " << pt3.Z()  << "\n";
      }
    }
  return 0;

}

#include <TColStd_SequenceOfTransient.hxx>
#include <GeomFill_Pipe.hxx>
static int pipe_OCC9 (Draw_Interpretor& di,
		      Standard_Integer n, const char ** a)
{
  if (n < 6) {
    di << "Usage: " << a[0] << " result path cur1 cur2 radius [tolerance]\n";
    return 1;
  }

  TColStd_SequenceOfTransient aCurveSeq;
  Standard_Integer i;
  for (i=2 ; i<=4; i++) {
    Handle(Geom_Curve) aC = Handle(Geom_Curve)::DownCast( DrawTrSurf::Get(a[i]) );
    if (aC.IsNull()) {
      di << a[i] << " is not a curve\n";
      return 1;
    }
    aCurveSeq.Append(aC);
  }

  GeomFill_Pipe aPipe(Handle(Geom_Curve)::DownCast( aCurveSeq(1) ),
		      Handle(Geom_Curve)::DownCast( aCurveSeq(2) ),
		      Handle(Geom_Curve)::DownCast( aCurveSeq(3) ),
		      Draw::Atof (a[5]) );

  if (n == 7) {
    aPipe.Perform(Draw::Atof (a[6]), Standard_True);
  } else {
    aPipe.Perform(Standard_True/*, Standard_True*/);
  }

  if (!aPipe.IsDone()) {
    di << "GeomFill_Pipe cannot make a surface\n";
    return 1;
  }

  Handle(Geom_Surface) aSurf = aPipe.Surface();

  DrawTrSurf::Set(a[1], aSurf);
  return 0;
}

//======================================================================
// OCC125
// usage : OCC125 shell
//======================================================================
#include <ShapeFix_Shell.hxx>

Standard_Integer  OCC125(Draw_Interpretor& di ,
			 Standard_Integer n,
			 const char ** a)
{
  if (n!=2) {
    di<<" Use OCC125 shell";
    return 1;
  }

  TopoDS_Shape S = DBRep::Get(a[1]);

  if (S.IsNull()) {
    di<<" Null shape is not allowed";
    return 1;
  }

  TopAbs_ShapeEnum aT;
  aT=S.ShapeType();
  if (aT!=TopAbs_SHELL) {
    di<<" Shape Type must be SHELL";
    return 1;
  }

  const TopoDS_Shell& aShell = TopoDS::Shell(S);
  //
  Standard_Boolean isAccountMultiConex, bNonManifold, bResult;

  isAccountMultiConex=Standard_True;
  bNonManifold=Standard_False;

  Handle (ShapeFix_Shell) aFix = new ShapeFix_Shell(aShell);
  bResult=aFix->FixFaceOrientation(aShell, isAccountMultiConex, bNonManifold);

  di<<"bResult="<<(Standard_Integer)bResult;

  TopoDS_Shape aShape;
  aShape=aFix->Shape();

  TCollection_AsciiString aName(a[1]), aDef("_sh"), aRName;
  aRName=aName;
  aRName=aRName+aDef;
  DBRep::Set (aRName.ToCString(), aShape);
  di<<aRName.ToCString();
  //
  return 0;
}

#include <BRepLib_FindSurface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
Standard_Integer  OCC157(Draw_Interpretor& di,
			 Standard_Integer n,
			 const char ** a)
//static Standard_Integer findplanarsurface(Draw_Interpretor&, Standard_Integer n, const char ** a)
{
  if (n<3) {
    di << "bad number of arguments\n";
    return 1;
  }

  // try to read a shape:
  TopoDS_Shape inputShape=DBRep::Get(a[2]);
  if (inputShape.IsNull() || inputShape.ShapeType() != TopAbs_WIRE) {
    di << "Invalid input shape\n";
    return 1;
  }
  Standard_Real toler = Draw::Atof(a[3]);
  TopoDS_Wire aWire = TopoDS::Wire(inputShape);
  BRepLib_FindSurface FS(aWire, toler, Standard_True);
  if(FS.Found()) {
    di<<"OCC157: OK; Planar surface is found\n";
    Handle(Geom_Surface) aSurf = FS.Surface();
    BRepBuilderAPI_MakeFace aMakeFace(aSurf,aWire,Standard_True);
    if(aMakeFace.IsDone()) {
      TopoDS_Face aFace = aMakeFace.Face();
      DBRep::Set(a[1],aFace);
    }
  }
  else di<<"OCC157: ERROR; Planar surface is not found with toler = "<<toler <<"\n";
  return 0;

}

// #include <MyCommandsCMD.h>
#include <ShapeFix_Shape.hxx>
#include <BRepOffset_MakeOffset.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <BRepOffset_Mode.hxx>
#include <GeomAbs_JoinType.hxx>
#include <AIS_Shape.hxx>

#include <BRepTools.hxx>

Standard_Integer  OCC165(Draw_Interpretor& di ,
			 Standard_Integer n,
			 const char ** a)


//=======================================================================

// static int YOffset (Draw_Interpretor& di, Standard_Integer argc, const char ** argv);

// void MyOffsets_Commands(Draw_Interpretor& theCommands)
// {
// 	theCommands.Add("yoffset" , "yoffset" , __FILE__, YOffset, " Offset on Z Direction");
// }

//=======================================================================

// static int YOffset (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
 {
   if (n > 2)
     {
       di <<"Usage : " << a[0] << " [file]\n";
       return 1;
     }
	di.Eval ("axo");

#define _OFFSET_TELCO_
#ifdef _OFFSET_TELCO_

	Standard_CString file = a[1];

	BRep_Builder aBuilder;
	TopoDS_Shape theShape;
	//BRepTools::Read(theShape, Standard_CString("/dn02/users_SUN/inv/3/OCC165/2d_tr_line.brep"), aBuilder);
	BRepTools::Read(theShape, file, aBuilder);
	DBRep::Set("shape", theShape);

	TopoDS_Wire theWire = TopoDS::Wire(theShape);

	Standard_Real anOffset = 1.5;

#else

	Standard_Real xA = 0.0, xB = 200.0, xC = 200.0, xD = 0.0,
		yA = 0.0, yB = 0.0, yC = 200.0, yD = 200.0,
		zA = 0.0, zB = 0.0, zC = 0.0, zD = 0.0;

	BRepBuilderAPI_MakePolygon theSquare;
	TopoDS_Vertex theA = BRepBuilderAPI_MakeVertex(gp_Pnt(xA, yA, zA));
	theSquare.Add(theA);
	TopoDS_Vertex theB = BRepBuilderAPI_MakeVertex(gp_Pnt(xB, yB, zB));
	theSquare.Add(theB);
	TopoDS_Vertex theC = BRepBuilderAPI_MakeVertex(gp_Pnt(xC, yC, zC));
	theSquare.Add(theC);
	TopoDS_Vertex theD = BRepBuilderAPI_MakeVertex(gp_Pnt(xD, yD, zD));
	theSquare.Add(theD);

	theSquare.Close();
	TopoDS_Wire theWire = theSquare.Wire();

	Standard_Real anOffset = 10;


#endif /* _OFFSET_TELCO_ */


	TopoDS_Face theFace = BRepBuilderAPI_MakeFace(theWire).Face();
	DBRep::Set("face", theFace);


	Standard_Real anAlt = 0.;
	GeomAbs_JoinType theJoin = GeomAbs_Intersection;
//GeomAbs_Intersection; //GeomAbs_Arc;
	BRepOffsetAPI_MakeOffset aMakeOffset(theFace, theJoin);
	aMakeOffset.AddWire(theWire);

	aMakeOffset.Perform(anOffset, anAlt);

	TopoDS_Shape theOffsetShapePos = aMakeOffset.Shape();
	DBRep::Set("offset", theOffsetShapePos);
	return 0;
// 	return TCL_OK;
}

#include<BRepAlgoAPI_Cut.hxx>

#include<BRepPrimAPI_MakeHalfSpace.hxx>
#include<Geom_CartesianPoint.hxx>
#include<AIS_Point.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

static Standard_Integer OCC297 (Draw_Interpretor& di,Standard_Integer /*argc*/, const char ** argv )

{
  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if (myAISContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  
  gp_Pnt pt1_(250., 250., 0.);
  gp_Pnt pt2_(-250., 250., 0.);
  gp_Pnt pt3_(-250., -250., 0.);
  gp_Pnt pt4_(250., -250., 0.);
  BRepBuilderAPI_MakeEdge edg1_(pt1_, pt2_);
  BRepBuilderAPI_MakeEdge edg2_(pt2_, pt3_);
  BRepBuilderAPI_MakeEdge edg3_(pt3_, pt4_);
  BRepBuilderAPI_MakeEdge edg4_(pt4_, pt1_);

  BRepBuilderAPI_MakeWire wire_(edg1_, edg2_, edg3_, edg4_);
  BRepBuilderAPI_MakeFace face_(wire_);
  TopoDS_Face sh_ = face_.Face();

  int up = 1;

  gp_Pnt g_pnt;
  if (up)
    g_pnt = gp_Pnt(0, 0, -100);
  else
    g_pnt = gp_Pnt(0, 0, 100);

  myAISContext->EraseAll(Standard_False);
  Handle(Geom_CartesianPoint) GEOMPoint = new Geom_CartesianPoint(g_pnt);
  Handle(AIS_Point) AISPoint = new AIS_Point(GEOMPoint);
  myAISContext->Display(AISPoint, Standard_True);

  BRepPrimAPI_MakeHalfSpace half_(sh_, g_pnt);
  TopoDS_Solid sol1_ = half_.Solid();

  DBRep::Set("Face", sol1_);

  gp_Ax1 ax1_(gp_Pnt(0., 0., -100.), gp_Dir(0., 0., 1.));

  Standard_Real x = 0., y = 0., z = -80.;

  BRepPrimAPI_MakeBox box(gp_Pnt(x, y, z), gp_Pnt(x + 150, y + 200, z + 200));

  DBRep::Set("Box", box.Shape());

  return 0;

}

#include<GProp_GProps.hxx>
#include<BRepGProp.hxx>

static Standard_Integer OCC305 (Draw_Interpretor& di,Standard_Integer argc, const char ** argv )

{
  if (argc != 2)
  {
    di <<"Usage : " << argv[0] << " file\n";
    return 1;
  }
  Standard_CString file = argv[1];

  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

TopoDS_Shape sh;
BRep_Builder builder;
//BRepTools::Read(sh, "/dn02/users_SUN/inv/3/OCC305/testc.brep", builder);
BRepTools::Read(sh, file, builder);

TopoDS_Wire wire;
builder.MakeWire(wire);
TopoDS_Edge ed;
TopoDS_Vertex vt1, vt2;
TopExp_Explorer wex(sh, TopAbs_EDGE);
for(;wex.More();wex.Next())
{
    ed = TopoDS::Edge(wex.Current());
    TopExp::Vertices(ed, vt1, vt2);
    builder.UpdateVertex(vt1, 0.01);
    builder.UpdateVertex(vt2, 0.01);
    builder.UpdateEdge(ed, 0.01);
    builder.Add(wire, ed);


    GProp_GProps lprop;
    BRepGProp::LinearProperties(ed, lprop);
    printf("\n length = %f", lprop.Mass());
}
 DBRep::Set("Wire",wire);
// Handle(AIS_Shape) res = new AIS_Shape( wire );
// aContext->SetColor( res, Quantity_NOC_RED );
// aContext->Display( res );

// BRepOffsetAPI_MakeOffset off(wire, GeomAbs_Arc);
// off.Perform(0.5, 0);

// printf("\n IsDone = %d", off.IsDone());
// sh = off.Shape();
// res = new AIS_Shape( sh );
// aContext->SetColor( res, Quantity_NOC_GREEN );
// aContext->Display( res );

  return 0;

}

static Standard_Integer OCC166 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv)
{

  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0];
    return 1;
  }

  BRepPrimAPI_MakeBox aBox(gp_Pnt(0, 0, 0), 100, 100, 100);
  Handle(AIS_Shape) anAISBox = new AIS_Shape(aBox.Shape());
  myAISContext->SetAutoActivateSelection (Standard_False);
  myAISContext->Display(anAISBox, 1);

  TColStd_ListOfInteger anActivatedModes;
  myAISContext->ActivatedModes (anAISBox, anActivatedModes);
  if(anActivatedModes.Extent() != 1 || anActivatedModes.First() != -1 )
    return 1;

  return 0;
}

#include <TDocStd_Document.hxx>
#include <DDocStd.hxx>
#include <PCDM_StoreStatus.hxx>
#include <TDocStd_Application.hxx>

static Standard_Integer OCC381_Save (Draw_Interpretor& di, Standard_Integer nb, const char ** a)
{
  if (nb != 2) {
    di << "Usage: " << a[0] << " Doc\n";
    return 1;
  }

  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(a[1],D)) return 1;

  Handle(TDocStd_Application) A = DDocStd::GetApplication();

  TCollection_ExtendedString theStatusMessage;
  if (!D->IsSaved()) {
    di << "this document has never been saved\n";
    return 0;
  }
  PCDM_StoreStatus theStatus = A->Save(D, theStatusMessage);
  if (theStatus != PCDM_SS_OK ) {
    switch ( theStatus ) {
      case PCDM_SS_DriverFailure: {
        di << "Error saving document: Could not store , no driver found to make it\n";
        break ;
      }
      case PCDM_SS_WriteFailure: {
        di << "Error saving document: Write access failure\n";
        break;
      }
      case PCDM_SS_Failure: {
        di << "Error saving document: Write failure\n" ;
        break;
      }
      case PCDM_SS_Doc_IsNull: {
        di << "Error saving document: No document to save\n";
        break ;
      }
      case PCDM_SS_No_Obj: {
        di << "Error saving document: No objects written\n";
        break;
      }
      case PCDM_SS_Info_Section_Error: {
        di << "Error saving document: Write info section failure\n" ;
        break;
      }
      default:
          break;
    }
    return 1;
  }
  return 0;
}

static Standard_Integer OCC381_SaveAs (Draw_Interpretor& di, Standard_Integer nb, const char ** a)
{
  if (nb != 3) {
    di << "Usage: " << a[0] << " Doc Path\n";
    return 1;
  }

  Handle(TDocStd_Document) D;
  if (!DDocStd::GetDocument(a[1],D)) return 1;

  TCollection_ExtendedString path (a[2]);
  Handle(TDocStd_Application) A = DDocStd::GetApplication();

  TCollection_ExtendedString theStatusMessage;
  PCDM_StoreStatus theStatus = A->SaveAs(D,path, theStatusMessage);
  if (theStatus != PCDM_SS_OK ) {
    switch ( theStatus ) {
      case PCDM_SS_DriverFailure: {
        di << "Error saving document: Could not store , no driver found to make it\n";
        break ;
      }
      case PCDM_SS_WriteFailure: {
        di << "Error saving document: Write access failure\n";
        break;
      }
      case PCDM_SS_Failure: {
        di << "Error saving document: Write failure\n" ;
        break;
      }
      case PCDM_SS_Doc_IsNull: {
        di << "Error saving document: No document to save\n";
        break ;
      }
      case PCDM_SS_No_Obj: {
        di << "Error saving document: No objects written\n";
        break;
      }
      case PCDM_SS_Info_Section_Error: {
        di << "Error saving document: Write info section failure\n" ;
        break;
      }
      default:
          break;
    }
    return 1;
  }

  return 0;
}

#include <BRepClass3d_SolidClassifier.hxx>

Standard_Integer OCC299bug (Draw_Interpretor& theDi,
                            Standard_Integer  theArgNb,
                            const char**      theArgVec)
{
  if (theArgNb < 3)
  {
    theDi << "Usage : " << theArgVec[0] << " Solid Point [Tolerance=1.e-7]\n";
    return -1;
  }

  TopoDS_Shape aS = DBRep::Get (theArgVec[1]);
  if (aS.IsNull())
  {
    theDi << " Null Shape is not allowed here\n";
    return 1;
  }
  else if (aS.ShapeType() != TopAbs_SOLID)
  {
    theDi << " Shape type must be SOLID\n";
    return 1;
  }

  gp_Pnt aP (8., 9., 10.);
  if (!DrawTrSurf::GetPoint (theArgVec[2], aP))
  {
    theDi << " Null Point is not allowed here\n";
    return 1;
  }
  const Standard_Real aTol = (theArgNb == 4) ? Draw::Atof (theArgVec[3]) : 1.e-7;

  BRepClass3d_SolidClassifier aSC (aS);
  aSC.Perform (aP, aTol);

  switch (aSC.State())
  {
    case TopAbs_IN:      theDi << "The point is IN shape\n";      return 0;
    case TopAbs_OUT:     theDi << "The point is OUT of shape\n";  return 0;
    case TopAbs_ON:      theDi << "The point is ON shape\n";      return 0;
    case TopAbs_UNKNOWN:
    default:             theDi << "The point is UNKNOWN shape\n"; return 0;
  }
}

#include <OSD_Process.hxx>
#include <OSD_Path.hxx>

static Standard_Integer OCC309bug (Draw_Interpretor& di, Standard_Integer nb, const char ** a)
{
  if (nb != 1) {
    di << "Usage: " << a[0] << "\n";
    return 1;
  }
  OSD_Process p;
  OSD_Path d = p.CurrentDirectory();
  TCollection_AsciiString s;
  d.SystemName(s);
  di << "*" <<  s.ToCString() << "*\n";
  d.UpTrek();
  d.SystemName(s);
  di <<  "*" <<  s.ToCString() <<  "*\n";
  return 0;
}

static Standard_Integer OCC310bug (Draw_Interpretor& di, Standard_Integer nb, const char ** a)
{
  if (nb != 1) {
    di << "Usage: " << a[0] << "\n";
    return 1;
  }
  OSD_Path p("/where/you/want/tmp/qwerty/tmp/");
  di << p.Trek().ToCString() << "\n";
  p.UpTrek();
  di << p.Trek().ToCString() << "\n";
  return 0;
}

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>

static Standard_Integer OCC277bug (Draw_Interpretor& di, Standard_Integer nb, const char ** a)
{
  if(nb != 1) {
    di << "Usage : " << a[0] << "\n";
    return 1;
  }

  BRepPrimAPI_MakeBox box1( 100, 100, 100 );
  BRepPrimAPI_MakeBox box2( gp_Pnt( 50, 50,50 ), 200, 200, 200 );

  TopoDS_Shape shape1 = box1.Shape();
  TopoDS_Shape shape2 = box2.Shape();

  TopoDS_Shape fuse,comm;
  di << "fuse = BRepAlgoAPI_Fuse( shape1, shape2 )\n";
  di << "comm = BRepAlgoAPI_Common( shape1, shape2 )\n";
  fuse = BRepAlgoAPI_Fuse(shape1, shape2).Shape();
  comm = BRepAlgoAPI_Common(shape1, shape2).Shape();

  return 0;
}

#include <ShapeAnalysis_Edge.hxx>

static Standard_Integer OCC333bug (Draw_Interpretor& di, Standard_Integer n, const char ** a)
{
  if( n < 3) {
    di<<"-1\n";
    di << "Usage: " << a[0] << " edge1 edge2 [toler domaindist]\n";
    return 1;
  }
  TopoDS_Shape Sh1 = DBRep::Get(a[1]);
  TopoDS_Shape Sh2 = DBRep::Get(a[2]);
  if(Sh1.IsNull() || Sh2.IsNull()) {
    di<<"-2\n";
    di<<"Invalid arguments\n";
    return 1;
  }
  TopoDS_Edge e1 = TopoDS::Edge(Sh1);
  TopoDS_Edge e2 = TopoDS::Edge(Sh2);
  if(e1.IsNull() || e2.IsNull()) {
    di<<"-3\n";
    di<<"Invalid type of arguments\n";
    return 1;
  }
  Standard_Real aTol = Precision::Confusion();
  Standard_Real aDistDomain = 0.0;
  Standard_Integer k = 3;
  if(k < n)
    aTol = Draw::Atof(a[k++]);
  if(k < n)
    aDistDomain = Draw::Atof(a[k++]);

  ShapeAnalysis_Edge sae;
  if(sae.CheckOverlapping(e1,e2,aTol,aDistDomain)) {
    if(aDistDomain ==0.0) {
      di<<"1\n";
      di<<"Edges is overlaping comletly\n";
    } else {
      di<<"2\n";
      di<<"Edges is overlaped\n";
      di<<"with tolerance = "<<aTol<<"\n";
      di<<"on segment length = "<<aDistDomain<<"\n";
    }
  } else {
    di<<"3\n";
    di<<"Edges is not overlaped\n";
  }
  return 0;
}


#include <DDocStd_DrawDocument.hxx>
#include <TDataStd_Name.hxx>
#include <Draw.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_LabelSequence.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <XCAFPrs_Driver.hxx>

//------------------------------------------------------------------------------------------
// name    : OCC363
// Purpose :
//------------------------------------------------------------------------------------------
static Standard_Integer OCC363 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  try
  {
    OCC_CATCH_SIGNALS
    // 1. Verufy amount of arguments
    if(argc < 3) { di <<"Error OCC363 : Use : OCC363 document filename\n"; return 1; }

    // 2. Retrieve DDocStd application
    Handle(TDocStd_Application) App = DDocStd::GetApplication();

    // 3. Open document
    TCollection_ExtendedString name(argv[2]);
    Handle(TDocStd_Document) Doc;
    if(App->Open(name, Doc) != PCDM_RS_OK) { di << "Error OCC363 : document was not opened successfully\n"; return 1;}
    Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(Doc);
    TDataStd_Name::Set(Doc->GetData()->Root(),argv[1]);
    Draw::Set(argv[1],DD);

    // 4. Create prsentations
    Handle(XCAFDoc_ShapeTool) shapes = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
    TDF_LabelSequence seq;
    shapes->GetFreeShapes ( seq );
    Handle(TPrsStd_AISPresentation) prs;
    for ( Standard_Integer i=1; i <= seq.Length(); i++ )
      if ( ! seq.Value(i).FindAttribute ( TPrsStd_AISPresentation::GetID(), prs ) )
        prs = TPrsStd_AISPresentation::Set(seq.Value(i),XCAFPrs_Driver::GetID());
  }
  catch(Standard_Failure) { di << "FAULTY OCC363 : Exception during reading document.\n";return 0;}

  di << "OCC363 OK\n";
  return 0;
}

// Must use OCC299
////======================================================================================
//// Function : OCC372
//// Purpose  :
////======================================================================================
//static Standard_Integer OCC372 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
//{
//  try
//  {
//    OCC_CATCH_SIGNALS
//    // 1. Verufy amount of arguments
//    if(argc < 2) {di << "OCC372 FAULTY. Use : OCC372 brep-file";return 0;}
//
//    // 2. Read solid
//    BRep_Builder B;
//    TopoDS_Shape Ref;
//    BRepTools::Read(Ref, argv[1], B);
//
//    // 3. Calculate location of aP3d in relation to the solid
//    gp_Pnt aP3d(6311.4862583184, -2841.3092756034, 16.461053497188);
//    BRepClass3d_SolidClassifier SC(Ref);
//    SC.Perform(aP3d, 1e-7);
//
//    // 4. Check returned state. The point must be inside the solid.
//    TopAbs_State aState=SC.State();
//    switch (aState)
//    {
//    case TopAbs_OUT:
//      di<<"OCC372 FAULTY. aState = TopAbs_OUT";
//      return 0;
//    case TopAbs_ON:
//      di<<"OCC372 FAULTY. aState = TopAbs_ON";
//     return 0;
//    case TopAbs_IN:
//      di<<"OCC372 OK. aState = TopAbs_IN" ;
//      return 0;
//    default:
//      di<<"OCC372 FAULTY. aState = UNKNOWN";
//      return 0;
//    }
//  }
//  catch (Standard_Failure) { di<<"OCC372 FAULTY. Exception raised"; }
//
//  return 0;
//}

#include <BRepTopAdaptor_FClass2d.hxx>

//======================================================================================
// Function : OCC377
// Purpose  :
//======================================================================================
static Standard_Integer OCC377 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  try
  {
    OCC_CATCH_SIGNALS
    // 1. Verify validity of arguments
    if ( argc < 1 ) {di << "Error OCC377. Use  OCC377 file x y precuv \n";return 0;}

    // 2. Initialize parameters
    gp_Pnt2d p2d;
    p2d.SetX ( Draw::Atof(argv[2]) );
    p2d.SetY ( Draw::Atof(argv[3]) );
    Standard_Real precuv = Draw::Atof (argv[4] );

    // 3. Read shape
    BRep_Builder B;
    TopoDS_Shape Shape;
    BRepTools::Read ( Shape, argv[1], B );

    // 4. Verify whether enrtry point is on wire and reversed ones (indeed results of veridying must be same)
    TopExp_Explorer exp;
    Standard_Integer i=1;
    for (exp.Init(Shape.Oriented(TopAbs_FORWARD),TopAbs_WIRE); exp.More(); exp.Next(), i++)
    {
      // 4.1. Verify whether enrtry point is on wire
      const TopoDS_Wire& wir = TopoDS::Wire(exp.Current());
      TopoDS_Face newFace = TopoDS::Face(Shape.EmptyCopied());

      TopAbs_Orientation orWire = wir.Orientation();
      newFace.Orientation(TopAbs_FORWARD);
      B.Add(newFace,wir);

      BRepTopAdaptor_FClass2d FClass2d1(newFace,precuv);
      TopAbs_State stat1 = FClass2d1.PerformInfinitePoint();
      //di << "Wire " << i << ": Infinite point is " <<
      //  ( stat1 == TopAbs_IN ? "IN" : stat1 == TopAbs_OUT ? "OUT" : stat1 == TopAbs_ON ? "ON" : "UNKNOWN" ) << "\n";

      TCollection_AsciiString TmpString;
      stat1 == TopAbs_IN ? TmpString.AssignCat("IN") : stat1 == TopAbs_OUT ? TmpString.AssignCat("OUT") : stat1 == TopAbs_ON ? TmpString.AssignCat("ON") : TmpString.AssignCat("UNKNOWN");
      di << "Wire " << i << ": Infinite point is " << TmpString.ToCString() << "\n";

      stat1 = FClass2d1.Perform(p2d);
      //di << "Wire " << i << ": point ( " << p2d.X() << ", " << p2d.Y() << " ) is " <<
      //  ( stat1 == TopAbs_IN ? "IN" : stat1 == TopAbs_OUT ? "OUT" : stat1 == TopAbs_ON ? "ON" : "UNKNOWN" ) << "\n";

      TmpString.Clear();
      stat1 == TopAbs_IN ? TmpString.AssignCat("IN") : stat1 == TopAbs_OUT ? TmpString.AssignCat("OUT") : stat1 == TopAbs_ON ? TmpString.AssignCat("ON") : TmpString.AssignCat("UNKNOWN");
      di << "Wire " << i << ": point ( " << p2d.X() << ", " << p2d.Y() << " ) is " << TmpString.ToCString() << "\n";

      // 4.2. Verify whether enrtry point is on reversed wire
      newFace = TopoDS::Face(Shape.EmptyCopied());
      newFace.Orientation(TopAbs_FORWARD);
      orWire = TopAbs::Reverse(orWire);
      B.Add(newFace,wir.Oriented(orWire));
      BRepTopAdaptor_FClass2d FClass2d2(newFace,precuv);
      TopAbs_State stat2 = FClass2d2.PerformInfinitePoint();
      //di << "Reversed Wire " << i << ": Infinite point is " <<
      //  ( stat2 == TopAbs_IN ? "IN" : stat2 == TopAbs_OUT ? "OUT" : stat2 == TopAbs_ON ? "ON" : "UNKNOWN" ) << "\n";

      TmpString.Clear();
      stat2 == TopAbs_IN ? TmpString.AssignCat("IN") : stat2 == TopAbs_OUT ? TmpString.AssignCat("OUT") : stat2 == TopAbs_ON ? TmpString.AssignCat("ON") : TmpString.AssignCat("UNKNOWN");
      di << "Reversed Wire " << i << ": Infinite point is " << TmpString.ToCString() << "\n";

      stat2 = FClass2d2.Perform(p2d);
      //di << "Reversed Wire " << i << ": point ( " << p2d.X() << ", " << p2d.Y() << " ) is " <<
      //  ( stat2 == TopAbs_IN ? "IN" : stat2 == TopAbs_OUT ? "OUT" : stat2 == TopAbs_ON ? "ON" : "UNKNOWN" ) << "\n";

      TmpString.Clear();
      stat2 == TopAbs_IN ? TmpString.AssignCat("IN") : stat2 == TopAbs_OUT ? TmpString.AssignCat("OUT") : stat2 == TopAbs_ON ? TmpString.AssignCat("ON") : TmpString.AssignCat("UNKNOWN");
      di << "Reversed Wire " << i << ": point ( " << p2d.X() << ", " << p2d.Y() << " ) is " << TmpString.ToCString() << "\n";

      // 4.3. Compare results (they must be same)
      if(stat1 ==stat2) di << "OCC377 OK\n";
      else {di << "OCC377 FAULTY\n"; return 0;}
    }
  }
  catch(Standard_Failure)
  {
    di << "OCC377 Exception";
  }

  return 0;
}

#include <ShapeUpgrade_ShapeDivideAngle.hxx>
#include <ShapeBuild_ReShape.hxx>

//=======================================================================
//function : OCC22
//purpose  :
//=======================================================================
static Standard_Integer OCC22 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  try
  {
    OCC_CATCH_SIGNALS
    // 1. Verify arguments of the command
    if (argc < 5) { di << "OCC22 FAULTY. Use : OCC22 Result Shape CompoundOfSubshapesToBeDivided ConsiderLocation"; return 0;}

    Standard_Boolean aConsiderLocation;
    if(strcmp(argv[4], "0")==0) aConsiderLocation = Standard_False;
    else aConsiderLocation = Standard_True;

    // 2. Iniitialize aShapeUpgrade
    ShapeUpgrade_ShapeDivideAngle aShapeUpgrade(M_PI/2.);
    // precision
    aShapeUpgrade.SetPrecision (Precision::Confusion());
    // tolerance
    aShapeUpgrade.SetMaxTolerance(0.1);
    // subshapes to be divided
    TopoDS_Shape aSubShapesToBeDivided = DBRep::Get(argv[3]);
    if(aSubShapesToBeDivided.IsNull()) {di << "OCC22 FAULTY. Compound of subshapes to be divided is not exist. Please, verify input values. \n";return 0;}
    aShapeUpgrade.Init(aSubShapesToBeDivided);
    // context
    Handle(ShapeBuild_ReShape) aReshape = new ShapeBuild_ReShape;
    aShapeUpgrade.SetContext(aReshape);
    if(aConsiderLocation) aReshape->ModeConsiderLocation() = Standard_True;

    // 3. Perform splitting
    if (aShapeUpgrade.Perform (Standard_False))         di << "Upgrade_SplitRevolution_Done \n";
    else if (aShapeUpgrade.Status (ShapeExtend_OK))     di << "Upgrade_SplitRevolution_OK \n";
    else if (aShapeUpgrade.Status (ShapeExtend_FAIL)) { di << "OCC22 FAULTY. Operation failed. Angle was not divided\n";return 0;}

    // 4. Perform rebuilding shape
    // 4.1. Retrieve Shape
    TopoDS_Shape anInitShape = DBRep::Get(argv[2]);
    if(anInitShape.IsNull()) { di << "OCC22 FAULTY. Initial shape is not exist. Please verify input values \n"; return 0;}
    // 4.2 Rebuid retrieved shape
    TopoDS_Shape aResultShape = aReshape->Apply(anInitShape);
    // 4.3. Create result Draw shape
    DBRep::Set(argv[1], aResultShape);
  }
  catch (Standard_Failure) {di << "OCC22 Exception \n" ;return 0;}

  return 0;
}


#include <ShapeProcess_OperLibrary.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <ShapeProcess.hxx>

#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

//=======================================================================
//function : OCC24
//purpose  :
//=======================================================================
static Standard_Integer OCC24 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  try
  {
    OCC_CATCH_SIGNALS
    // 1. Verify amount of arguments of the command
    if (argc < 6) { di << "OCC24 FAULTY. Use : OCC22 Result Shape CompoundOfSubshapes ResourceFileName SequenceName"; return 0;}

    // 2. Retrieve parameters
    // initial shape
    TopoDS_Shape anInitShape = DBRep::Get(argv[2]);
    if(anInitShape.IsNull()) { di << "OCC24 FAULTY. Initial shape is not exist. Please verify input values \n"; return 0;}
    // compound of subshapes
    TopoDS_Shape aSubShapes = DBRep::Get(argv[3]);
    if(aSubShapes.IsNull()) {di << "OCC24 FAULTY. Compound of subshapes is not exist. Please, verify input values. \n";return 0;}
    // name of resource file
    const char* aResourceFile = argv[4];
    // name of sequence from resource file to be executed
    const char* aSequenceName = argv[5];

    // 3. Initialize ShapeContext and perform sequence of operation specified with resource file
    ShapeProcess_OperLibrary::Init();
    Handle(ShapeProcess_ShapeContext) aShapeContext = new ShapeProcess_ShapeContext (aSubShapes, aResourceFile);
    aShapeContext->SetDetalisation (TopAbs_EDGE);
    ShapeProcess::Perform (aShapeContext, aSequenceName);

    // 4. Rebuild initil shape in accordance with performed operation
    Handle(ShapeBuild_ReShape) aReshape = new ShapeBuild_ReShape;
    TopTools_DataMapIteratorOfDataMapOfShapeShape anIter (aShapeContext->Map());
    for (; anIter.More(); anIter.Next())
      aReshape->Replace(anIter.Key(), anIter.Value());
    TopoDS_Shape aResultShape = aReshape->Apply(anInitShape);

    // 5 Create resultant Draw shape
    DBRep::Set(argv[1], aResultShape);

  }
  catch (Standard_Failure) {di << "OCC24 Exception \n" ;return 0;}

  return 0;
}

//=======================================================================
//function : OCC369
//purpose  : Verify whether exception occurs during building mesh
//=======================================================================
static Standard_Integer OCC369(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  try
  {
    OCC_CATCH_SIGNALS
    // 1. Verify amount of arguments of the command
    if (argc < 2) { di << "OCC369 FAULTY. Use : OCC369 Shape \n"; return 0;}

    // 2. Retrieve shape
    TopoDS_Shape aShape = DBRep::Get(argv[1]);
    if(aShape.IsNull()) {di << "OCC369 FAULTY. Entry shape is NULL \n"; return 0;}

    // 3. Build mesh
    BRepMesh_FastDiscret::Parameters aMeshParams;
    aMeshParams.Relative = Standard_True;
    aMeshParams.Deflection = 0.2;
    aMeshParams.Angle = M_PI / 6;
    BRepMesh_IncrementalMesh aMesh(aShape, aMeshParams);

  }
  catch (Standard_Failure) {di << "OCC369 Exception \n" ;return 0;}

  di << "OCC369 OK \n";
  return 0;
}

#include <math_Vector.hxx>
#include <math_Matrix.hxx>
static Standard_Integer OCC524 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 9){
    di<<"Usage : " << argv[0] << " LowerVector UpperVector InitialValueVector LowerRowMatrix UpperRowMatrix LowerColMatrix UpperColMatrix InitialValueMatrix\n";
    return 1;
  }
  Standard_Integer LowerVector = Draw::Atoi(argv[1]);
  Standard_Integer UpperVector = Draw::Atoi(argv[2]);
  Standard_Real InitialValueVector = Draw::Atof(argv[3]);
  Standard_Integer LowerRowMatrix = Draw::Atoi(argv[4]);
  Standard_Integer UpperRowMatrix = Draw::Atoi(argv[5]);
  Standard_Integer LowerColMatrix = Draw::Atoi(argv[6]);
  Standard_Integer UpperColMatrix = Draw::Atoi(argv[7]);
  Standard_Real InitialValueMatrix = Draw::Atof(argv[8]);

  math_Vector Vector1(LowerVector, UpperVector);
  math_Vector Vector2(LowerVector, UpperVector);

  math_Vector Vector(LowerVector, UpperVector, InitialValueVector);
  math_Matrix Matrix(LowerRowMatrix, UpperRowMatrix, LowerColMatrix, UpperColMatrix, InitialValueMatrix);

  //Vector.Dump(cout);
  //cout<<endl;

  //Matrix.Dump(cout);
  //cout<<endl;

  Vector1.Multiply(Vector, Matrix);

  //Vector1.Dump(cout);
  Standard_SStream aSStream1;
  Vector1.Dump(aSStream1);
  di << aSStream1;
  di<<"\n";

  if (Matrix.RowNumber() > 1) {
    Matrix(Matrix.LowerRow() + 1, Matrix.LowerCol()) += 1.;
  }
  Vector2.TMultiply(Vector, Matrix);

  //Vector2.Dump(cout);
  Standard_SStream aSStream2;
  Vector2.Dump(aSStream2);
  di << aSStream2;
  di<<"\n";

  return 0;
}

#include <GeomPlate_BuildPlateSurface.hxx>
//=======================================================================
//function : OCC525
//purpose  :
//=======================================================================
static Standard_Integer OCC525(Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  GeomPlate_BuildPlateSurface aBuilder;
  aBuilder.Perform();

  if (aBuilder.IsDone())
  {
    di << "Error in OCC525. Null result is expected.\n";
  }
  else
  {
    di << "OCC525 OK \n";
  }

  return 0;
}

#include <BRepPrimAPI_MakeWedge.hxx>
#include <gce_MakeRotation.hxx>
#include <gce_MakeTranslation.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakeWedge.hxx>
//=======================================================================
//function :  OCC578
//purpose  :
//=======================================================================
static Standard_Integer OCC578 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 4) {
    di << "Usage : " << argv[0] << " shape1 shape2 shape3\n";
    return 1;
  }

  gp_Pnt P0(0,0,0.0);
  double xperiod = 1.0;
  double yperiod = 1.0;
  double sub_thick = 0.5;

  // mask_substrate
  //TopoDS_Shape substrate = BRepPrimAPI_MakeBox( P0, xperiod, yperiod, sub_thick );
  TopoDS_Shape substrate = BRepPrimAPI_MakeBox( P0, xperiod, yperiod, sub_thick ).Shape();

  // --------------------------------------------------------------------

  // wedge
  //TopoDS_Shape wedge1 = BRepPrimAPI_MakeWedge(0.5, 0.05, 0.5,
	//				      0.1,  0.1  , 0.4, 0.4 );
  TopoDS_Shape wedge1 = BRepPrimAPI_MakeWedge(0.5, 0.05, 0.5,
					      0.1,  0.1  , 0.4, 0.4 ).Shape();

  gp_Trsf rotate = gce_MakeRotation ( gp_Pnt(0.0,0.0,0.0),
				     gp_Dir(1.0,0.0,0.0),
				     1.570795 );

  gp_Trsf translate = gce_MakeTranslation(gp_Pnt( 0.0, -0.5, 0.0),
					  gp_Pnt( 0.25, 0.25, 0.5)
					  );

  rotate.PreMultiply( translate );

  TopoDS_Shape wedge1a = BRepBuilderAPI_Transform( wedge1, rotate );

  if (wedge1a.IsNull()) {
    di<<" Null shape1 is not allowed\n";
    return 1;
  }
  DBRep::Set(argv[1], wedge1a);

  // --------------------------------------------------------------------

  // wedge top
  //TopoDS_Shape wedge2 = BRepPrimAPI_MakeWedge(0.5, 0.3, 0.5,
	//				      0.1,  0.1  , 0.4, 0.4 );
  TopoDS_Shape wedge2 = BRepPrimAPI_MakeWedge(0.5, 0.3, 0.5,
					      0.1,  0.1  , 0.4, 0.4 ).Shape();

  gp_Trsf rotate2 = gce_MakeRotation ( gp_Pnt(0.0,0.0,0.0),
				      gp_Dir(1.0,0.0,0.0),
				      1.570795 * 3.0 );

  gp_Trsf translate2 = gce_MakeTranslation(gp_Pnt( 0.0, 0.0, 0.0),
					   gp_Pnt( 0.25, 0.25, 0.5)
					   );

  rotate2.PreMultiply( translate2 );

  TopoDS_Shape wedge2a = BRepBuilderAPI_Transform( wedge2, rotate2 );

  if (wedge2a.IsNull()) {
    di<<" Null shape2 is not allowed\n";
    return 1;
  }
  DBRep::Set(argv[2], wedge2a);


  // combine wedges
  di << "wedge_common = BRepAlgoAPI_Fuse(wedge1a , wedge2a)\n";
  TopoDS_Shape wedge_common = BRepAlgoAPI_Fuse(wedge1a , wedge2a).Shape();

  di << "sub_etch1 = BRepAlgoAPI_Cut(substrate, wedge_common)\n";
  TopoDS_Shape sub_etch1 = BRepAlgoAPI_Cut(substrate, wedge_common).Shape();
 
  if (sub_etch1.IsNull()) {
    di<<" Null shape3 is not allowed\n";
    return 1;
  }
  DBRep::Set(argv[3], sub_etch1);

  return 0;
}

#include <Standard_GUID.hxx>
//=======================================================================
//function :  OCC669
//purpose  :
//=======================================================================
static Standard_Integer OCC669 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 2){
    di<<"Usage : " << argv[0] << " GUID\n";
    return -1;
  }
  Standard_GUID guid(argv[1]);
  //guid.ShallowDump(cout);
  Standard_SStream aSStream;
  guid.ShallowDump(aSStream);
  di << aSStream;
  di<<"\n";
  return 0;
}

#include <XCAFDoc.hxx>
//=======================================================================
//function :  OCC738_ShapeRef
//purpose  :
//=======================================================================
static Standard_Integer OCC738_ShapeRef (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 1){
    di<<"Usage : " << argv[0] << "\n";
    return -1;
  }
  const Standard_GUID& guid = XCAFDoc::ShapeRefGUID ();
  //guid.ShallowDump(cout);
  Standard_SStream aSStream;
  guid.ShallowDump(aSStream);
  di << aSStream;
  return 0;
}

//=======================================================================
//function :  OCC738_Assembly
//purpose  : 
//=======================================================================
static Standard_Integer OCC738_Assembly (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 1){
    di<<"Usage : " << argv[0] << "\n";
    return -1;
  }
  const Standard_GUID& guid = XCAFDoc::AssemblyGUID ();
  //guid.ShallowDump(cout);
  Standard_SStream aSStream;
  guid.ShallowDump(aSStream);
  di << aSStream;
  return 0;
}

#if defined(DDataStd_def01)
#include <DDataStd_DrawPresentation.hxx>
//=======================================================================
//function :  OCC739_DrawPresentation
//purpose  : 
//=======================================================================
static Standard_Integer OCC739_DrawPresentation (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 1){
    di<<"Usage : " << argv[0] << "\n";
    return -1;
  }
  const Standard_GUID& guid = DDataStd_DrawPresentation::GetID() ;
  //guid.ShallowDump(cout);
  Standard_SStream aSStream;
  guid.ShallowDump(aSStream);
  di << aSStream;
  return 0;
}
#endif

//=======================================================================
//function :  OCC708
//purpose  : 
//=======================================================================
static Standard_Integer OCC708 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << argv[0] << "ERROR : use 'vinit' command before \n";
    return 1;
  }

  if ( argc != 2) {
    di << "ERROR : Usage : " << argv[0] << " shape ; Deactivate the current transformation\n";
    return 1;
  }
  
  Standard_Boolean updateviewer = Standard_True;

  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  
  TCollection_AsciiString aName(argv[1]);
  Handle(AIS_InteractiveObject) AISObj;

  if(!aMap.IsBound2(aName)) {
    di << "Use 'vdisplay' before\n";
    return 1;
  } else {
    AISObj = Handle(AIS_InteractiveObject)::DownCast(aMap.Find2(aName));
    if(AISObj.IsNull()){
      di << argv[1] << " : No interactive object\n";
      return 1;
    } 
    AISObj->ResetTransformation();

    aContext->Erase(AISObj, updateviewer);
    aContext->UpdateCurrentViewer();
    aContext->Display(AISObj, updateviewer);
    aContext->UpdateCurrentViewer();
  }
  return 0;
}

//=======================================================================
//function :  OCC670
//purpose  :
//=======================================================================
#include <TColStd_Array2OfInteger.hxx>
static Standard_Integer OCC670 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 1){
    di<<"Usage : " << argv[0] << "\n";
    return -1;
  }

  // check that exception initialized without message string can be safely handled and printed
  try {
    throw Standard_OutOfRange();
  }
  catch (Standard_Failure const& anException) {
    std::cout << "Caught successfully: ";
    std::cout << anException << endl;
  }
  return 0;
}

#include <GeomAPI_ProjectPointOnSurf.hxx>
//=======================================================================
//function :  OCC867
//purpose  : 
//=======================================================================
static Standard_Integer OCC867(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc!=7)  
  {
    di<<"Usage : " << argv[0] << " Point Surface Umin Usup Vmin Vsup \n";
    return 1;   
  }
  
  gp_Pnt aPoint3d;        
  DrawTrSurf::GetPoint(argv[1],aPoint3d);
  Handle (Geom_Surface) aSurface=DrawTrSurf::GetSurface(argv[2]);
  Standard_Real             Umin=Draw::Atof(argv[3]);
  Standard_Real             Usup=Draw::Atof(argv[4]);
  Standard_Real             Vmin=Draw::Atof(argv[5]);
  Standard_Real             Vsup=Draw::Atof(argv[6]);
 
  if (aSurface.IsNull()) {
    di << argv[2] << " Null surface \n" ;
    return 1;
  }
  
  GeomAPI_ProjectPointOnSurf PonSurf;
  PonSurf.Init(aSurface, Umin, Usup, Vmin, Vsup);
  PonSurf.Perform(aPoint3d);

  return 0; 
}

//=======================================================================
//function :  OCC909
//purpose  : 
//=======================================================================
static Standard_Integer OCC909 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc!=3)  
  {
    di<<"Usage : " << argv[0] << " wire face\n";
    return 1;   
  }
  
  TopoDS_Wire awire = TopoDS::Wire(DBRep::Get(argv[1])); //read the wire
  TopoDS_Face aface = TopoDS::Face(DBRep::Get(argv[2])); //read the face
  if (awire.IsNull() || aface.IsNull()) {
    di << "Null object\n";
    return 1;
  }

  Standard_Integer count = 0;
  TopExp_Explorer TE(awire, TopAbs_VERTEX);
  if ( TE.More()) {
    BRepTools_WireExplorer WE;
    for ( WE.Init(awire,aface); WE.More(); WE.Next()) {
      TopoDS_Edge E = WE.Current();
      count++;
    }
  }
  di << "Count = " << count << "\n";

  return 0; 
}

//=======================================================================
//function :  OCC921
//purpose  : 
//=======================================================================
static Standard_Integer OCC921 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2)
  {
    di <<"Usage : " << argv[0] << " face\n";
    return 1;
  }
  Standard_Real u1, u2, v1, v2;
  TopoDS_Face F = TopoDS::Face( DBRep::Get(argv[1]) ); //read the shape
  if (F.IsNull())
    return 1;
  BRepTools::UVBounds(F, u1, u2, v1, v2);
  di << "Bounds: " << u1 << "   " << u2 << "   " << v1 << "   " << v2 << "\n";
  return 0;
}

#include <Expr_NamedUnknown.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_Exponential.hxx>
//=======================================================================
//function :  OCC902
//purpose  : 
//=======================================================================
static Standard_Integer OCC902(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2)
  {
    di <<"Usage : " << argv[0] << " expression\n";
    return 1;
  }

 TCollection_AsciiString  myStr(argv[1]);

 Handle (Expr_NamedUnknown)      myNamed = new Expr_NamedUnknown(myStr);
 Handle (Expr_Exponential)       oldExpr = new Expr_Exponential(myNamed); 
 Handle (Expr_GeneralExpression) newExpr = oldExpr->Derivative(myNamed);

 
 TCollection_AsciiString  res        = newExpr->String();
 Standard_CString         resStr     = res.ToCString();
 TCollection_AsciiString  res_old    = oldExpr->String();
 Standard_CString         res_oldStr = res_old.ToCString();
 

 di << "X = " << argv[1] << "\n";
 di << "Y = " << res_oldStr << "\n";
 di << "Y' = " << resStr  << "\n";

 return 0;
}

#include <DDF.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TDF_Label.hxx>
#include <TPrsStd_AISPresentation.hxx>
//=======================================================================
//function : OCC1029_AISTransparency 
//purpose  : OCC1029_AISTransparency  (DOC,entry,[real])
//=======================================================================

static Standard_Integer OCC1029_AISTransparency (Draw_Interpretor& di,
					     Standard_Integer nb, 
					     const char ** arg) 
{
  if (nb >= 3 ) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      if( nb == 4 ) {
	prs->SetTransparency(Draw::Atof(arg[3]));
	TPrsStd_AISViewer::Update(L);
      }
      else {
         di << "Transparency = " << prs->Transparency() << "\n";
      }
      return 0;
    }
  }
  di << arg[0] << " : Error\n";
  return 1;
}

//=======================================================================
//function : OCC1030_AISColor 
//purpose  : OCC1030_AISColor (DOC,entry,[color])
//=======================================================================

static Standard_Integer OCC1030_AISColor (Draw_Interpretor& di,
				      Standard_Integer nb, 
				      const char ** arg) 
{
  if (nb >= 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      if( nb == 4 ) {
	prs->SetColor((Quantity_NameOfColor)Draw::Atoi(arg[3]));
	TPrsStd_AISViewer::Update(L);
      }
      else
         di << "Color = " << prs->Color() << "\n";
      return 0; 
    }
  }
  di << arg[0] << " : Error\n";
  return 1;
}

//=======================================================================
//function : OCC1031_AISMaterial
//purpose  : OCC1031_AISMaterial (DOC,entry,[material])
//=======================================================================

static Standard_Integer OCC1031_AISMaterial (Draw_Interpretor& di,
					 Standard_Integer nb,
					 const char ** arg)
{
  if (nb >= 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      if( nb == 4 ) {
	prs->SetMaterial((Graphic3d_NameOfMaterial)Draw::Atoi(arg[3]));
	TPrsStd_AISViewer::Update(L);
      }
      else {
         di << "Material = " << prs->Material() << "\n";
      }
      return 0;
    }
  }
  di << arg[0] << " : Error\n";
  return 1;
}

//=======================================================================
//function : OCC1032_AISWidth
//purpose  : OCC1032_AISWidth (DOC,entry,[width])
//=======================================================================

static Standard_Integer OCC1032_AISWidth (Draw_Interpretor& di,
				      Standard_Integer nb, 
				      const char ** arg) 
{
  if (nb >= 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      if( nb == 4 ) {
	prs->SetWidth(Draw::Atof(arg[3]));
	TPrsStd_AISViewer::Update(L);
      }
      else {
         di << "Width = " << prs->Width() << "\n";
      }
      return 0;
    }
  }
  di << arg[0] << " : Error\n";
  return 1;
}

//=======================================================================
//function : OCC1033_AISMode
//purpose  : OCC1033_AISMode (DOC,entry,[mode])
//=======================================================================

static Standard_Integer OCC1033_AISMode (Draw_Interpretor& di,
				     Standard_Integer nb, 
				     const char ** arg) 
{
  if (nb >= 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      if( nb == 4 ) {
	prs->SetMode(Draw::Atoi(arg[3]));
	TPrsStd_AISViewer::Update(L);
      }
      else {
         di << "Mode = " << prs->Mode() << "\n";
      }
      return 0;
    }
  }
  di << arg[0] << " : Error\n";
  return 1;
}

//=======================================================================
//function : OCC1034_AISSelectionMode
//purpose  : OCC1034_AISSelectionMode (DOC,entry,[selectionmode])
//=======================================================================

static Standard_Integer OCC1034_AISSelectionMode (Draw_Interpretor& di,
					      Standard_Integer nb, 
					      const char ** arg) 
{
  if (nb >= 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),arg[2],L)) return 1;  

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) return 1;  

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      if( nb == 4 ) {
	prs->SetSelectionMode(Draw::Atoi(arg[3]));
	TPrsStd_AISViewer::Update(L);
      }
      else {
         di << "SelectionMode = " << prs->SelectionMode() << "\n";
      }
      return 0;
    }
  }
  di << arg[0] << " : Error\n";
  return 1;
}

//=======================================================================
//function :  OCC1487
//purpose  :
//=======================================================================
static Standard_Integer OCC1487 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 5) {
    di << "Usage : " << argv[0] << " CylinderVariant(=1/2) cylinder1 cylinder2 cutshape\n";
    return 1;
  }

  Standard_Integer CaseNumber = Draw::Atoi(argv[1]);

  //BRepPrimAPI_MakeCylinder o_mc1 (gp_Ax2 (gp_Pnt(0,-50,140), gp_Dir(1,0,0)), 50,1000);
  gp_Dir myDir(1,0,0);
  gp_Pnt myPnt(0,-50,140);
  gp_Ax2 myAx2(myPnt, myDir);
  BRepPrimAPI_MakeCylinder o_mc1 (myAx2, 50,1000);

  TopoDS_Shape cyl1 = o_mc1.Shape();

  TopoDS_Shape cyl2;
  TopoDS_Shape o_cut_shape;
  if (CaseNumber == 1) {
    //BRepPrimAPI_MakeCylinder o_mc2 (gp_Ax2 (gp_Pnt(21.65064, -50.0, 127.5),gp_Dir(-sin(M_PI/3), 0.0, 0.5)), 5, 150);
    gp_Dir myDir_mc2(-sin(M_PI/3), 0.0, 0.5);
    gp_Pnt myPnt_mc2(21.65064, -50.0, 127.5);
    gp_Ax2 myAx2_mc2(myPnt_mc2, myDir_mc2);
    BRepPrimAPI_MakeCylinder o_mc2 (myAx2_mc2, 5, 150);

    cyl2 = o_mc2.Shape();
    di << "o_cut_shape = BRepAlgoAPI_Cut (o_mc1.Solid (), o_mc2.Solid ())\n";
    o_cut_shape = BRepAlgoAPI_Cut (o_mc1.Solid (), o_mc2.Solid ()).Shape();
  } else {
    //BRepPrimAPI_MakeCylinder o_mc2 (gp_Ax2 (gp_Pnt(978.34936, -50.0, 127.5),gp_Dir(sin(M_PI/3), 0.0, 0.5)), 5, 150);
    gp_Dir myDir_mc2(sin(M_PI/3), 0.0, 0.5);
    gp_Pnt myPnt_mc2(978.34936, -50.0, 127.5);
    gp_Ax2 myAx2_mc2(myPnt_mc2, myDir_mc2);
    BRepPrimAPI_MakeCylinder o_mc2 (myAx2_mc2, 5, 150);

    cyl2 = o_mc2.Shape();
    di << "o_cut_shape = BRepAlgoAPI_Cut (o_mc1.Solid (), o_mc2.Solid ())\n";
    o_cut_shape = BRepAlgoAPI_Cut (o_mc1.Solid (), o_mc2.Solid ()).Shape();
  }

  DBRep::Set(argv[2],cyl1);
  DBRep::Set(argv[3],cyl2);
  DBRep::Set(argv[4],o_cut_shape);

  return 0;
}

#include<TopTools_ListIteratorOfListOfShape.hxx>
#include<BRepFilletAPI_MakeFillet.hxx>
//=======================================================================
//function :  OCC1077
//purpose  :
//=======================================================================
TopoDS_Shape OCC1077_boolbl(BRepAlgoAPI_BooleanOperation& aBoolenaOperation,const Standard_Real aRadius)
{
  Standard_Real t3d = 1.e-4;
  Standard_Real t2d = 1.e-5;
  Standard_Real ta  = 1.e-2;
  Standard_Real fl  = 1.e-3;
  Standard_Real tapp_angle = 1.e-2;
  GeomAbs_Shape blend_cont = GeomAbs_C1;

  TopoDS_Shape ShapeCut = aBoolenaOperation.Shape();

  TopTools_ListIteratorOfListOfShape its;

  TopoDS_Compound result;
  BRep_Builder B;
  B.MakeCompound(result);

  TopExp_Explorer ex;
  for (ex.Init(ShapeCut, TopAbs_SOLID); ex.More(); ex.Next())
    {
      const TopoDS_Shape& cutsol = ex.Current();

      BRepFilletAPI_MakeFillet fill(cutsol);
      fill.SetParams(ta, t3d, t2d, t3d, t2d, fl);
      fill.SetContinuity(blend_cont, tapp_angle);
      its = aBoolenaOperation.SectionEdges();
      while (its.More())
	{
	  TopoDS_Edge E = TopoDS::Edge(its.Value());
	  fill.Add(aRadius, E);
	  its.Next();
	}

      fill.Build();
      if (fill.IsDone())
	{
	  B.Add(result, fill.Shape());
	}
      else
	{
	  B.Add(result, cutsol);
	}
    }
  return result;
}

TopoDS_Shape OCC1077_cut_blend(const TopoDS_Shape& aShapeToCut, const TopoDS_Shape& aTool, const Standard_Real aRadius)
{
  //return OCC1077_boolbl(BRepAlgoAPI_Cut(aShapeToCut, aTool),aRadius);
  BRepAlgoAPI_Cut aCut(aShapeToCut, aTool);
  return OCC1077_boolbl(aCut,aRadius);
}

//TopoDS_Shape OCC1077_common_blend(const TopoDS_Shape& aShape1, const TopoDS_Shape& aShape2, const Standard_Real aRadius)
//{
//  return OCC1077_boolbl(BRepAlgoAPI_Common(aShape1, aShape2),aRadius);
//}

TopoDS_Shape OCC1077_Bug()
{
  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(gp_Pnt(-5, - 5, - 5), 10, 10, 10).Shape();
  TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(7).Shape();

  TopoDS_Shape theCommon = BRepAlgoAPI_Common(theBox,theSphere);
  TopoDS_Shape theCylinder1 = BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(0, 0, - 10),
							      gp_Dir(0, 0, 1)), 3, 20).Shape();
  TopoDS_Shape theCylinder2 = BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(-10, 0, 0),
							      gp_Dir(1, 0, 0)), 3, 20).Shape();
  TopoDS_Shape theCylinder3 = BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(0, - 10, 0),
							      gp_Dir(0, 1, 0)), 3, 20).Shape();
  TopoDS_Shape theTmp1 = OCC1077_cut_blend(theCommon,theCylinder1,0.7);
  Handle(ShapeFix_Shape) fixer = new ShapeFix_Shape(theTmp1);
  fixer->Perform();
  theTmp1 = fixer->Shape();
  TopoDS_Shape theTmp2 = OCC1077_cut_blend(theTmp1,theCylinder2,0.7);
  fixer->Init(theTmp2);
  fixer->Perform();
  theTmp2 = fixer->Shape();
  TopoDS_Shape theResult = OCC1077_cut_blend(theTmp2,theCylinder3,0.7);
  fixer->Init(theResult);
  fixer->Perform();
  theResult = fixer->Shape();
  return theResult;
}

static Standard_Integer OCC1077 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc < 1 || argc > 2) {
    di << "Usage : " << argv[0] << " result\n";
    return 1;
  }

  TopoDS_Shape S = OCC1077_Bug();
  DBRep::Set(argv[1],S);

  return 0;
}

//////////////////////////////////////////////////////////////
/*!
 * Compute uniform distribution of points using GCPnts_UniformAbscissa
 */
//////////////////////////////////////////////////////////////
static Standard_Integer OCC5739_UniAbs (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc < 4)
  {
    di << "Usage : " << argv[0] << " name shape step\n";
    return 1;
  }
  const char *name = argv[1];
  Adaptor3d_Curve *adapCurve=NULL;
  Handle(Geom_Curve) curve = DrawTrSurf::GetCurve(argv[2]);
  if (!curve.IsNull())
    adapCurve = new GeomAdaptor_Curve(curve);
  else
  {
    TopoDS_Shape wire = DBRep::Get(argv[2]);
    if (wire.IsNull() || wire.ShapeType() != TopAbs_WIRE)
    {
      di << argv[0] <<" Faulty : incorrect 1st parameter, curve or wire expected\n";
      return 1;
    }
    adapCurve = new BRepAdaptor_CompCurve(TopoDS::Wire(wire));
  }
  double step = Draw::Atof(argv[3]);
  GCPnts_UniformAbscissa aUni(*adapCurve, step);
  int res;
  if (!aUni.IsDone())
  {
    di << argv[0] <<" : fail\n";
    res = 1;
  }
  else
  {
    int i, np = aUni.NbPoints();
    for (i=0; i < np; i++)
    {
      double par = aUni.Parameter(i+1);
      gp_Pnt p = adapCurve->Value(par);
      char n[20], *pname=n;
      Sprintf(n,"%s_%d",name,i+1);
      DrawTrSurf::Set(pname,p);
      di<<pname<<" ";
    }
    res = 0;
  }
  delete adapCurve;
  return res;
}

static Standard_Integer OCC6046 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " nb_of_vectors size\n";
    return 1;
  }

  Standard_Integer nb = Draw::Atoi(argv[1]);
  Standard_Integer sz = Draw::Atoi(argv[2]);
  Standard_Real val = 10;
  math_Vector **pv = new math_Vector *[nb];

  di<<"creating "<<nb<<" vectors "<<sz<<" elements each...\n";
  Standard_Integer i;
  for (i=0; i < nb; i++) {
    pv[i] = new math_Vector (1, sz, val);
    if ((i % (nb/10)) == 0) {
      di<<" "<<i;
      //cout.flush();
      di<<"\n";
    }
  }
  di<<" done\n";
  di<<"deleting them ...\n";
  for (i=0; i < nb; i++) {
    delete pv[i];
    if ((i % (nb/10)) == 0) {
      di<<" "<<i;
      //cout.flush();
      di<<"\n";
    }
  }
  di<<" done\n";

  delete [] pv;

  return 0;
}

static Standard_Integer OCC5698 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " wire\n";
    return 1;
  }
  TopoDS_Shape shape = DBRep::Get(argv[1],TopAbs_WIRE);
  if (shape.IsNull())
    return 1;
  TopoDS_Wire wire = TopoDS::Wire(shape);
  // create curve parameterised by curvilinear distance
  BRepAdaptor_CompCurve curve(wire,Standard_True);
  Standard_Real length = curve.LastParameter();
  Standard_Real need_length = length/2;
  gp_Pnt pnt;
  curve.D0(need_length,pnt);
  // create check_curve parameterised in a general way
  BRepAdaptor_CompCurve check_curve(wire);
  Standard_Real check_par =
    GCPnts_AbscissaPoint(check_curve, need_length, 0).Parameter();
  gp_Pnt check_pnt;
  check_curve.D0(check_par,check_pnt);
  // check that points are coinsiding
  Standard_Real error_dist = pnt.Distance(check_pnt);
  if (error_dist > Precision::Confusion()) {
    //cout.precision(3);
    di<<"error_dist = "<<error_dist<<
      "  ( "<<error_dist/need_length*100<<" %)\n";
    return 0;
  }
  di<<"OK\n";
  return 0;
}

// stack overflow can be successfully handled only on 32-bit Windows
#if defined(_WIN32) && !defined(_WIN64)
static int StackOverflow (int i = -1)
{
  char arr[2000];
  memset (arr, 0, sizeof(arr));
  if (i < 0)
    StackOverflow(i-1);
  return i;
}
#endif

// this code does not work with optimize mode on Windows
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif
static Standard_Integer OCC6143 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 1)
    {
      cout << "Usage : " << argv[0] << "\n";
      return 1;
    }
  Standard_Boolean Succes;
  
  Succes = Standard_True;
  //OSD::SetSignal();

  {//==== Test Divide ByZero (Integer) ========================================
    try{
      OCC_CATCH_SIGNALS
      cout << "(Integer) Divide By Zero..." << endl;
      di << "(Integer) Divide By Zero...";
      //cout.flush();
      di << "\n";
      Standard_Integer res, a =4, b = 0 ;
      res = a / b;
      di << "Error: 4 / 0 = " << res << " - no exception is raised!\n";
      Succes = Standard_False;
    }
#if defined(SOLARIS) || defined(_WIN32)
    catch(Standard_DivideByZero)
#else
    catch(Standard_NumericError)
#endif
    {
      di << "Caught, OK\n";
    }
    catch(Standard_Failure const& anException) {
      di << " Caught (";
      di << anException.GetMessageString();
      di << ")... KO\n";
      Succes = Standard_False;
    }
    // this case tests if (...) supersedes (Standard_*),
    // the normal behaviour is not
    catch(...) {
      di<<" unknown exception... (But) Ok\n";
    }
  }

  {//==== Test Divide ByZero (Real) ===========================================
    try{
      OCC_CATCH_SIGNALS
      cout << "(Real) Divide By Zero..." << endl;
      di << "(Real) Divide By Zero...";
      //cout.flush();
      di << "\n";
      Standard_Real res, a= 4.0, b=0.0;
      res = a / b;
      di << "Error: 4.0 / 0.0 = " << res << " - no exception is raised!\n";
      Succes = Standard_False;
    }
    catch(Standard_DivideByZero) // Solaris, Windows w/o SSE2
    {
      di << "Caught, OK\n";
    }
    catch(Standard_NumericError) // Linux, Windows with SSE2
    {
      di << "Caught, OK\n";
    }
    catch(Standard_Failure const& anException) {
      //cout << " Caught (" << Standard_Failure::Caught() << ")... KO" << endl;
      di << " Caught (";
      di << anException.GetMessageString();
      di << ")... KO\n";
      Succes = Standard_False;
    }
  }

  {//==== Test Overflow (Integer) =============================================
    try{
      OCC_CATCH_SIGNALS
      cout << "(Integer) Overflow..." << endl;
      di << "(Integer) Overflow...";
      //cout.flush();
      di << "\n";
      Standard_Integer res, i=IntegerLast();
      res = i + 1;
      //++++ cout << " -- "<<res<<"="<<i<<"+1   Does not Caught... KO"<< endl;
      //++++ Succes = Standard_False;
      di << "Not caught: " << i << " + 1 = " << res << ", still OK\n";
    }
    catch(Standard_Overflow) {
      di << "Caught, OK\n";
    }
    catch(Standard_Failure const& anException) {
      //cout << " Caught (" << Standard_Failure::Caught() << ")... KO" << endl;
      di << " Caught (";
      di << anException.GetMessageString();
      di << ")... KO\n";
      Succes = Standard_False;
    }
  }

  {//==== Test Overflow (Real) ================================================ 
    try{
      OCC_CATCH_SIGNALS
      cout << "(Real) Overflow..." << endl;
      di << "(Real) Overflow...";
      //cout.flush();
      di << "\n";
      Standard_Real res, r=RealLast();
      res = r * r;
      
      (void)sin(1.); //this function tests FPU flags and raises signal (tested on LINUX).

      di << "Error: " << r << "*" << r << " = " << res << " - no exception is raised!\n";
      Succes = Standard_False;
    }
    catch(Standard_Overflow) // Solaris, Windows w/o SSE2
    {
      di << "Caught, OK\n";
    }
    catch(Standard_NumericError) // Linux, Windows with SSE2
    {
      di << "Caught, OK\n";
    }
    catch(Standard_Failure const& anException) {
      //cout << " Caught (" << Standard_Failure::Caught() << ")... KO" << endl;
      di << " Caught (";
      di << anException.GetMessageString();
      di << ")... KO\n";
      Succes = Standard_False;
    }
  }

  {//==== Test Underflow (Real) ===============================================
    try{
      OCC_CATCH_SIGNALS
      cout << "(Real) Underflow" << endl; // to have message in log even if process crashed
      di << "(Real) Underflow";
      //cout.flush();
      di << "\n";
      Standard_Real res, r=1.0e-308;
      res = r * r;
      //res = res + 1.;
      //++++ cout<<"-- "<<res<<"="<<r<<"*"<<r<<"   Does not Caught... KO"<<endl;
      //++++ Succes = Standard_False;
      di << "Not caught: " << r << "*" << r << " = " << res << ", still OK\n";
    }
    catch(Standard_Underflow) // could be on Solaris, Windows w/o SSE2
    {
      di << "Exception caught, KO\n";
      Succes = Standard_False;
    }
    catch(Standard_NumericError) // could be on Linux, Windows with SSE2
    {
      di << "Exception caught, KO\n";
      Succes = Standard_False;
    }
    catch(Standard_Failure const& anException) {
      //cout << " Caught (" << Standard_Failure::Caught() << ")... KO" << endl;
      di << " Caught (";
      di << anException.GetMessageString();
      di << ")... KO\n";
      Succes = Standard_False;
    }
  }

  {//==== Test Invalid Operation (Real) ===============================================
    try{
      OCC_CATCH_SIGNALS
      cout << "(Real) Invalid Operation..." << endl;
      di << "(Real) Invalid Operation...";
      //cout.flush();
      di << "\n";
      Standard_Real res, r=-1;
      res = sqrt(r);
      di << "Error: swrt(-1) = " << res << " - no exception is raised!\n";
      Succes = Standard_False;
    }
    catch(Standard_NumericError) {
      di << "Caught, OK\n";
    }
    catch(Standard_Failure const& anException) {
      //cout << " Caught (" << Standard_Failure::Caught() << ")... KO" << endl;
      di << " Caught (";
      di << anException.GetMessageString();
      di << ")... KO\n";
      Succes = Standard_False;
    }
  }

  {//==== Test Access Violation ===============================================
    try {
      OCC_CATCH_SIGNALS
      cout << "Segmentation Fault..." << endl;
      di << "Segmentation Fault...";
      //cout.flush();
      di << "\n";
      int* pint=NULL;
      *pint = 4;
      di << "Error: writing by NULL address - no exception is raised!\n";
      Succes = Standard_False;
    }
#ifdef _WIN32
    catch(OSD_Exception_ACCESS_VIOLATION)
#else
    catch(OSD_SIGSEGV)
#endif
    {
      di << "Caught, OK\n";
    } catch(Standard_Failure const& anException) {
      //cout << " Caught (" << Standard_Failure::Caught() << ")... KO" << endl;
      di << " Caught (";
      di << anException.GetMessageString();
      di << ")... KO\n";
      Succes = Standard_False;
    }
  }

#if defined(_WIN32) && !defined(_WIN64)
  {//==== Test Stack Overflow ===============================================
    try {
      OCC_CATCH_SIGNALS
      cout << "Stack Overflow..." << endl;
      di << "Stack Overflow...";
      //cout.flush();
      di << "\n";
      StackOverflow();
      di << "Error - no exception is raised!\n";
      Succes = Standard_False;
    }
    catch(OSD_Exception_STACK_OVERFLOW) {
      di << "Caught, OK\n";
    }
    catch(Standard_Failure const& anException) {
      //cout << " Caught (" << Standard_Failure::Caught() << ")... KO" << endl;
      di << " Caught (";
      di << anException.GetMessageString();
      di << ")... KO\n";
      Succes = Standard_False;
    }
  }
#endif

 if(Succes) {
   di << "TestExcept: Successfull completion\n";
 } else {
   di << "TestExcept: failure\n";
 }

  return 0;
}
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif

static TopoDS_Compound AddTestStructure(int nCount_)
{
  BRep_Builder B;
  int nCount=nCount_;
  TopoDS_Compound C;
  B.MakeCompound(C);
  BRepPrimAPI_MakeBox mkBox(1.0, 2.0, 3.0);
  for (int i=0; i<nCount; i++) {
    for (int j=0; j<nCount; j++) {
      gp_Trsf trsf;
      trsf.SetTranslationPart(gp_Vec(5.0*i, 05.0*j, 0.0));
      TopLoc_Location topLoc(trsf);
      TopoDS_Shape tempShape=mkBox.Shape().Located(topLoc);
      B.Add(C, tempShape);
    }
  }
  return C;
}

static Standard_Integer OCC7141 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2 && argc != 3)
  {
    std::cout << "Usage : " << argv[0] << " [nCount] path\n";
    return 1;
  }

  int nCount = (argc > 2 ? Draw::Atoi(argv[1]) : 10);
  TCollection_AsciiString aFilePath (argv[argc > 2 ? 2 : 1]);
  STEPCAFControl_Writer writer;
  Handle(TDocStd_Document) document;
  document = new TDocStd_Document("Pace Test-StepExporter-");
  Handle(XCAFDoc_ShapeTool) shapeTool;
  shapeTool = XCAFDoc_DocumentTool::ShapeTool(document->Main());
  shapeTool->AddShape(AddTestStructure(nCount), Standard_True);
  STEPControl_StepModelType mode = STEPControl_AsIs;
  if (!Interface_Static::SetIVal("write.step.assembly",1)) { //assembly mode
    di << "Failed to set assembly mode for step data\n\n";
    return 0;
  }
  try {
    OCC_CATCH_SIGNALS
    if( writer.Transfer(document, mode)) {
    	writer.Write(aFilePath.ToCString());
    }
  }
  catch(OSD_Exception_STACK_OVERFLOW) {
    di << "Failed : STACK OVERFLOW\n\n";
  }
  catch (Standard_Failure const& anException) {
    di << "Failed :\n\n";
    //cout << Standard_Failure::Caught() << endl;
    di << anException.GetMessageString();
  }
  di << argv[0] << " : Finish\n";
  
  return 0;
}

static Standard_Integer OCC7372 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 1)
    {
      di << "Usage : " << argv[0] << "\n";
      return 1;
    }
  
  // 1. Create an array of points
  Handle(TColgp_HArray1OfPnt2d) ap = new TColgp_HArray1OfPnt2d(1,5);
  ap->SetValue(1,gp_Pnt2d(100.0,0.0));
  ap->SetValue(2,gp_Pnt2d(100.0,100.0));
  ap->SetValue(3,gp_Pnt2d(0.0,100.0));
  ap->SetValue(4,gp_Pnt2d(0.0,0.0));
  ap->SetValue(5,gp_Pnt2d(50.0,-50.0));

  // 2. Create a periodic bspline through these 5 points
  Geom2dAPI_Interpolate intp(ap,Standard_True,1e-6);
  intp.Perform();
  Handle(Geom2d_BSplineCurve) bspline1 = intp.Curve();

  // 3. Increase degree of curve from 3 to 8
  bspline1->IncreaseDegree(8); // Increase degree to demonstrate the error
  Standard_CString CString1 = "BSplineCurve";
  DrawTrSurf::Set(CString1,bspline1);

  // 4. Convers BSpline curve to Bezier segments
  Geom2dConvert_BSplineCurveToBezierCurve bc(bspline1);

  // 5. Test the result of conversion
  TCollection_AsciiString aRName;
  for(Standard_Integer i = 1; i <= bc.NbArcs(); i++) {
    Handle(Geom2d_BezierCurve) arc = bc.Arc(i);
    aRName="segment_";
    aRName=aRName+TCollection_AsciiString(i);
    Standard_CString aRNameStr = aRName.ToCString();
    DrawTrSurf::Set(aRNameStr,arc);
    di << aRNameStr << " ";
  }

  return 0;
}

static Standard_Integer OCC8169 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " edge1 edge2 plane\n";
    return 1;
  }
  TopoDS_Edge theEdge1 = TopoDS::Edge(DBRep::Get(argv[1],TopAbs_EDGE));
  if (theEdge1.IsNull()) {
    di << "Invalid input shape " << argv[1] << "\n";
    return 1;
  }
  TopoDS_Edge theEdge2 = TopoDS::Edge(DBRep::Get(argv[2],TopAbs_EDGE));
  if (theEdge2.IsNull()) {
    di << "Invalid input shape " << argv[2] << "\n";
    return 1;
  }
  TopoDS_Face theFace = TopoDS::Face(DBRep::Get(argv[3],TopAbs_FACE));
  if (theFace.IsNull()) {
    di << "Invalid input shape " << argv[3] << "\n";
    return 1;
  }

  Handle(Geom_Surface) thePlane = BRep_Tool::Surface(theFace);

  Standard_Real aConfusion = Precision::Confusion();
  Standard_Real aP1first, aP1last, aP2first, aP2last;

  Handle(Geom_Curve) aCurve1 = BRep_Tool::Curve(theEdge1, aP1first, aP1last);
  Handle(Geom_Curve) aCurve2 = BRep_Tool::Curve(theEdge2, aP2first, aP2last);
  Handle(Geom2d_Curve) aCurve2d1 = GeomProjLib::Curve2d(aCurve1, aP1first, aP1last, thePlane);
  Handle(Geom2d_Curve) aCurve2d2 = GeomProjLib::Curve2d(aCurve2, aP2first, aP2last, thePlane);

  Geom2dAPI_InterCurveCurve anInter(aCurve2d1, aCurve2d2, aConfusion);

  Standard_Integer NbPoints = anInter.NbPoints();

  di << "NbPoints = " << NbPoints << "\n" ;

  if (NbPoints > 0) {
    Standard_Integer i;
    for (i=1; i<=NbPoints; i++) {
      gp_Pnt2d aPi = anInter.Point(i);
      di << "Point.X(" << i << ") = " << aPi.X() << "   Point.Y(" << i << ") = " << aPi.Y() << "\n" ;
    }
  }

  Standard_Integer NbSegments = anInter.NbSegments();

  di << "\nNbSegments = " << NbSegments << "\n" ;

  if (NbSegments > 0) {
    IntRes2d_IntersectionSegment aSegment = anInter.Intersector().Segment(1);

    gp_Pnt2d aP1 = aCurve2d1->Value(aSegment.FirstPoint().ParamOnFirst());
    gp_Pnt2d aP2 = aCurve2d2->Value(aSegment.FirstPoint().ParamOnSecond());
  
    Standard_Real aDist = aP1.Distance(aP2);
  
    di << "aP1.X() = " << aP1.X() << "   aP1.Y() = " << aP1.Y() << "\n" ;
    di << "aP2.X() = " << aP2.X() << "   aP2.Y() = " << aP2.Y() << "\n" ;

    di << "Distance = " << aDist << "\n" ;

    di << "Confusion = " << aConfusion << "\n" ;

    if (aDist > aConfusion) {
      di << "\n" << argv[0] << " Faulty\n" ;
    } else {
      di << "\n" << argv[0] << " OK\n" ;
    }
  } else {
    di << "\n" << argv[0] << " OK\n" ;
  }

  return 0;
}
static Standard_Integer OCC10138 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " lower upper\n";
    return 1;
  }

  Standard_Integer LOWER = Draw::Atoi(argv[1]);
  Standard_Integer UPPER = Draw::Atoi(argv[2]);

  //! 0. Create an empty document with several test labels
  Handle(TDocStd_Document) doc = new TDocStd_Document("XmlOcaf");
  doc->SetUndoLimit(100);
  TDF_Label main_label = doc->Main();
  TDF_Label label1 = main_label.FindChild(1, Standard_True);
  TDF_Label label2 = main_label.FindChild(2, Standard_True);
  
  //! 1. Set/Get OCAF attribute
  doc->OpenCommand();
  TDataStd_RealArray::Set(label1, LOWER, UPPER);
  Handle(TDataStd_RealArray) array;
  if (label1.FindAttribute(TDataStd_RealArray::GetID(), array) && 
      array->Lower() == LOWER && array->Upper() == UPPER)
    cout<<"1: OK"<<endl;
  else
  {
    cout<<"1: Failed.."<<endl;
    return 1;
  }
  doc->CommitCommand();

  //! 2. Set/Get value
  doc->OpenCommand();
  Standard_Integer i;
  for (i = LOWER; i <= UPPER; i++)
    array->SetValue(i, i);
  for (i = LOWER; i <= UPPER; i++)
  {  
    if (array->Value(i) != i)
    {
      cout<<"2: Failed.."<<endl;
      return 2;
    }
  }
  cout<<"2: OK"<<endl;
  doc->CommitCommand();

  //! 3. Re-init the array
  doc->OpenCommand();
  array->Init(LOWER + 2, UPPER + 4);
  if (array->Lower() != LOWER + 2 && array->Upper() != UPPER + 4)
  {
    cout<<"3: Failed.."<<endl;
    return 3;
  }
  for (i = LOWER + 2; i <= UPPER + 4; i++)
    array->SetValue(i, i);
  for (i = LOWER + 2; i <= UPPER + 4; i++)
  {  
    if (array->Value(i) != i)
    {
      cout<<"3: Failed.."<<endl;
      return 3;
    }
  }
  cout<<"3: OK"<<endl;
  doc->CommitCommand();

  //! 4. Change array
  doc->OpenCommand();
  Handle(TColStd_HArray1OfReal) arr = new TColStd_HArray1OfReal(LOWER + 5, UPPER + 5);
  for (i = LOWER + 5; i <= UPPER + 5; i++)
    arr->SetValue(i, i);
  array->ChangeArray(arr);
  for (i = LOWER + 5; i <= UPPER + 5; i++)
  {  
    if (array->Value(i) != i)
    {
      cout<<"4: Failed.."<<endl;
      return 4;
    }
  }
  cout<<"4: OK"<<endl;
  doc->CommitCommand();

  //! 5. Copy the array
  doc->OpenCommand();
  TDF_CopyLabel copier(label1, label2);
  copier.Perform();
  if (!copier.IsDone())
  {
    cout<<"5: Failed.."<<endl;
    return 5;
  }
  Handle(TDataStd_RealArray) array2;
  if (!label2.FindAttribute(TDataStd_RealArray::GetID(), array2))
  {
    cout<<"5: Failed.."<<endl;
    return 5;
  }
  for (i = LOWER + 5; i <= UPPER + 5; i++)
  {  
    if (array->Value(i) != i)
    {
      cout<<"5: Failed.."<<endl;
      return 5;
    }
  }
  cout<<"5: OK"<<endl;
  doc->CommitCommand();

  //! 6. Undo/Redo
  //! 6.a: undoes the 5th action: the copied array should disappear
  doc->Undo();
  if (!label1.FindAttribute(TDataStd_RealArray::GetID(), array) ||
      label2.FindAttribute(TDataStd_RealArray::GetID(), array2))
  {
    cout<<"6.a: Failed.."<<endl;
    return 6;
  }
  //! 6.b: undoes the 4th action: the array should be changed to (lower+2,upper+4)
  doc->Undo();
  if (!label1.FindAttribute(TDataStd_RealArray::GetID(), array) || 
      array->Lower() != LOWER + 2 ||
      array->Upper() != UPPER + 4)
  {
    cout<<"6.b: Failed.."<<endl;
    return 6;
  }
  for (i = LOWER + 2; i <= UPPER + 4; i++)
  {
    if (array->Value(i) != i)
    {
      cout<<"6.b: Failed.."<<endl;
      return 6;
    }
  }
  //! 6.c: undoes the 3d action: the array should be changed to (lower,upper)
  doc->Undo();
  if (!label1.FindAttribute(TDataStd_RealArray::GetID(), array) || 
      array->Lower() != LOWER ||
      array->Upper() != UPPER)
  {
    cout<<"6.c: Failed.."<<endl;
    return 6;
  }
  for (i = LOWER; i <= UPPER; i++)
  {
    if (array->Value(i) != i)
    {
      cout<<"6.c: Failed.."<<endl;
      return 6;
    }
  }
  //! 6.d: undoes and redoes the 2nd action: no change is expected.
  doc->Undo();
  doc->Redo();
  if (!label1.FindAttribute(TDataStd_RealArray::GetID(), array) || 
      array->Lower() != LOWER ||
      array->Upper() != UPPER)
  {
    cout<<"6.d: Failed.."<<endl;
    return 6;
  }
  for (i = LOWER; i <= UPPER; i++)
  {
    if (array->Value(i) != i)
    {
      cout<<"6.d: Failed.."<<endl;
      return 6;
    }
  }
  cout<<"6: OK"<<endl;

  //! 7. Re-set the array
  doc->OpenCommand();
  array = TDataStd_RealArray::Set(label1, LOWER + 1, UPPER + 1);
  if (array->Lower() != LOWER + 1 && array->Upper() != UPPER + 1)
  {
    cout<<"7: Failed.."<<endl;
    return 7;
  }
  for (i = LOWER + 1; i <= UPPER + 1; i++)
    array->SetValue(i, i);
  for (i = LOWER + 1; i <= UPPER + 1; i++)
  {  
    if (array->Value(i) != i)
    {
      cout<<"7: Failed.."<<endl;
      return 7;
    }
  }
  cout<<"7: OK"<<endl;
  doc->CommitCommand();

  //! 8.Test of speed: set LOWER and UPPER equal to great integer number and 
  //! measure the time spent by this test.
  //! Good luck!

  return 0;
}

static Standard_Integer OCC7639 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Boolean IsEvenArgc =  Standard_True;
  if (argc % 2 == 0) {
    IsEvenArgc =  Standard_True;
  } else {
    IsEvenArgc =  Standard_False;
  }

  if (argc < 3 || IsEvenArgc)
    {
      di << "Usage : " << argv[0] << " index1 value1 ... [indexN valueN]\n";
      return 1;
    }

  Standard_Integer i, aValue, aPosition;
  NCollection_Vector<int> vec;
  for (i = 0; i < argc - 1; i++) {
    i++;
    aValue = Draw::Atoi(argv[i]);
    aPosition = Draw::Atoi(argv[i+1]);
    vec.SetValue(aValue, aPosition);
  }
  NCollection_Vector<int>::Iterator it(vec);
  Standard_Integer j;
  for (j = 0; it.More(); it.Next(), j++) {
    //di << it.Value() << "\n";                                               
    di << j << " " << it.Value() << "\n";                                               
  }

  return 0;
}

static Standard_Integer OCC8797 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }

  gp_Pnt point(0.0,0.0,0.0);

  TColgp_Array1OfPnt poles(0,6);
  poles(0)=point;

  point.SetCoord(1.0,1.0,0.0);
  poles(1)=point;

  point.SetCoord(2.0,1.0,0.0);
  poles(2)=point;

  point.SetCoord(3.0,0.0,0.0);
  poles(3)=point;

  point.SetCoord(4.0,1.0,0.0);
  poles(4)=point;

  point.SetCoord(5.0,1.0,0.0);
  poles(5)=point;

  point.SetCoord(6.0,0.0,0.0);
  poles(6)=point;

  TColStd_Array1OfReal knots(0,2);
  knots(0)=0.0;
  knots(1)=0.5;
  knots(2)=1.0;

  TColStd_Array1OfInteger multi(0,2);
  multi(0)=4;
  multi(1)=3;
  multi(2)=4;

  Handle(Geom_BSplineCurve) spline = new Geom_BSplineCurve(poles,knots,multi,3);

  //length!! 1.
  Standard_Real l_abcissa,l_gprop;
  GeomAdaptor_Curve adaptor_spline(spline);
  GCPnts_AbscissaPoint temp;
  l_abcissa=temp.Length(adaptor_spline);
  cout<<"Length Spline(abcissa_Pnt): "<<l_abcissa<<endl;

  //length!! 2.
  TopoDS_Edge edge = BRepBuilderAPI_MakeEdge (spline);
  GProp_GProps prop;
  BRepGProp::LinearProperties(edge,prop);
  l_gprop=prop.Mass();
  cout<<"Length Spline(GProp_GProps): "<<l_gprop<<endl;

  cout<<"Difference (abcissa_Pnt<->GProp_GProps): "<<l_gprop-l_abcissa<<endl;

  return 0;
}

static Standard_Integer OCC7068 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) AISContext = ViewerTest::GetAISContext();
  if(AISContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  // ObjectsInside
  AIS_ListOfInteractive ListOfIO_1;
  AISContext->ObjectsInside(ListOfIO_1);
  di<< "ObjectsInside = " << ListOfIO_1.Extent() <<"\n";
  if (!ListOfIO_1.IsEmpty() ) {
    AIS_ListIteratorOfListOfInteractive iter;
    for (iter.Initialize(ListOfIO_1); iter.More() ; iter.Next() ) {
      Handle(AIS_InteractiveObject) aIO=iter.Value();
      di<< GetMapOfAIS().Find1(aIO).ToCString() <<"\n";
    }
  }

  return 0;
}

static Standard_Integer OCC11457 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ((argc < 9) || (((argc-3) % 3) != 0))
  {
    di << "Usage : " << argv[0] << "polygon lastedge x1 y1 z1 x2 y2 z2 ...\n";
    return 1;
  }
  Standard_Integer i, j, np = (argc-3) / 3;
  BRepBuilderAPI_MakePolygon W;
  j = 3;
  for (i = 1; i <= np; i ++) {
    W.Add(gp_Pnt(Draw::Atof(argv[j]),Draw::Atof(argv[j+1]),Draw::Atof(argv[j+2])));
    j += 3;
  }
  W.Close();
  DBRep::Set(argv[1],W.Wire());
  DBRep::Set(argv[2],W.Edge());
  return 0;
}

static Standard_Integer OCC13963 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc < 5) {
    di << "Usage : " << argv[0] << " ratio origin_x origin_y origin_z\n";
    return 1;
  }
  gp_Ax2 aPln (gp_Pnt(0.,0.,0.),
               gp_Dir(1., -1., 0.));
  gp_GTrsf aTrf;
  aTrf.SetAffinity (aPln, Draw::Atof(argv[4]));
  gp_XYZ aOrigin (Draw::Atof(argv[1]),Draw::Atof(argv[2]),Draw::Atof(argv[3]));
  gp_XYZ aResult (aOrigin);
  aTrf.Transforms(aResult);
  char sbf[512];
  Sprintf(sbf, "( %8.3f %8.3f %8.3f ) => ( %8.3f %8.3f %8.3f )\n",
          aOrigin.X(), aOrigin.Y(), aOrigin.Z(),
          aResult.X(), aResult.Y(), aResult.Z());
  di<<sbf;
  return 0;
}

Standard_Integer OCC14376(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc < 2) {
    di << "Usage : " << argv[0] << " shape [deflection]\n";
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(argv[1]);

  if (aShape.IsNull()) {
    di<<" Null shape is not allowed";
    return 1;
  }

  Standard_Real aDeflection = 0.45110277533;
  if (argc > 2) {
    aDeflection = Draw::Atof(argv[2]);
  }
  di<<"deflection="<< aDeflection << "\n";

  BRepMesh_IncrementalMesh aIMesh(aShape, aDeflection, Standard_False, M_PI / 9.);
  TopLoc_Location aLocation;
  Handle(Poly_Triangulation) aTriang = BRep_Tool::Triangulation(TopoDS::Face(aShape), aLocation);

  if(aTriang.IsNull()) {
    di << argv[0] << " : Faulty\n" ;
  } else {
    di << argv[0] << " : OK\n" ;
    di<<"NbNodes="<< aTriang->NbNodes()<< "\n";
    di<<"NbTriangles="<< aTriang->NbTriangles()<< "\n";
  }
  return 0;
}

static Standard_Integer OCC15489 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 4) {
    di << "Usage : " << argv[0] << " A B C\n";
    return 1;
  }
  try
    {
      gp_Lin2d aLin2d (Draw::Atof(argv[1]),Draw::Atof(argv[2]),Draw::Atof(argv[3]));
      gp_Pnt2d anOrigin = aLin2d.Location();
      di << "X_0 = " << anOrigin.X() << "   Y_0 = " << anOrigin.Y() << "\n" ;
    }
  catch(Standard_ConstructionError)
    {
      di << argv[0] << " Exception: Sqrt(A*A + B*B) <= Resolution from gp\n";
    }
  return 0;
}

static Standard_Integer OCC15755 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 3) {
    di << "Usage : " << argv[0] << " file shape\n";
    return 1;
  }

  IGESControl_Reader aReader;
  aReader.ReadFile(argv[1]);
  aReader.SetReadVisible(Standard_True);
  aReader.TransferRoots();

  Handle(IGESData_IGESModel) model = aReader.IGESModel();
  if (model.IsNull()) {
    di << "model.IsNull()\n";
    return 1;
  }
  Standard_Integer nb = model->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ent = model->Entity(i);
    Handle(TCollection_HAsciiString) name;
    name = ent->NameValue();
    Standard_CString aStr = name->ToCString();
    di << "NameValue = " << aStr << "\n";
  }

  TopoDS_Shape shape = aReader.OneShape();
  DBRep::Set(argv[2],shape);
  return 0;
}

// For OCC16782 testing
#include <AppStd_Application.hxx>
#include <TDF_Tool.hxx>
#include <TColStd_HArray1OfInteger.hxx>
// Iterators
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <TDataStd_ListIteratorOfListOfByte.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
// Attributes
#include <TDataStd_Tick.hxx>
#include <TDataStd_IntegerList.hxx>
#include <TDataStd_RealList.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_NamedData.hxx>
#include <TDF_Reference.hxx>
//
Handle(AppStd_Application) app;
int TestSetGet(const Handle(TDocStd_Document)& doc)
{
  // TDataStd_Tick:
  // Set
  TDataStd_Tick::Set(doc->Main());
  // Get
  Handle(TDataStd_Tick) tick;
  if (!doc->Main().FindAttribute(TDataStd_Tick::GetID(), tick))
    return 1;
  // Forget
  doc->Main().ForgetAttribute(TDataStd_Tick::GetID());
  if (doc->Main().IsAttribute(TDataStd_Tick::GetID()))
    return 2;
  doc->Main().ResumeAttribute(tick);
  if (!doc->Main().IsAttribute(TDataStd_Tick::GetID()))
    return 3;
  // Forget
  doc->Main().ForgetAttribute(TDataStd_Tick::GetID());
  if (doc->Main().IsAttribute(TDataStd_Tick::GetID()))
    return 2;

  // TDataStd_IntegerList:
  // Set
  Handle(TDataStd_IntegerList) setintlist = TDataStd_IntegerList::Set(doc->Main());
  setintlist->Append(2);
  setintlist->Prepend(1);
  setintlist->InsertAfter(3, 2);
  setintlist->InsertBefore(0, 1);
  setintlist->Append(200);
  setintlist->Remove(0);
  setintlist->Remove(200);
  // Get
  Handle(TDataStd_IntegerList) getintlist;
  if (!doc->Main().FindAttribute(TDataStd_IntegerList::GetID(), getintlist))
    return 1;
  if (getintlist->First() != 1)
    return 2;
  if (getintlist->Last() != 3)
    return 3;
  const TColStd_ListOfInteger& intlist = getintlist->List();
  TColStd_ListIteratorOfListOfInteger itr_intlist(intlist);
  for (; itr_intlist.More(); itr_intlist.Next())
  {
    if (itr_intlist.Value() != 1 &&
	itr_intlist.Value() != 2 &&
	itr_intlist.Value() != 3)
    {
      return 4;
    }
  }
  getintlist->Clear();

  // TDataStd_RealList:
  // Set
  Handle(TDataStd_RealList) setdbllist = TDataStd_RealList::Set(doc->Main());
  setdbllist->Append(2.5);
  setdbllist->Prepend(1.5);
  setdbllist->InsertAfter(3.5, 2.5);
  setdbllist->InsertBefore(0.5, 1.5);
  setdbllist->Append(200.5);
  setdbllist->Remove(0.5);
  setdbllist->Remove(200.5);
  // Get
  Handle(TDataStd_RealList) getdbllist;
  if (!doc->Main().FindAttribute(TDataStd_RealList::GetID(), getdbllist))
    return 1;
  if (getdbllist->First() != 1.5)
    return 2;
  if (getdbllist->Last() != 3.5)
    return 3;
  const TColStd_ListOfReal& dbllist = getdbllist->List();
  TColStd_ListIteratorOfListOfReal itr_dbllist(dbllist);
  for (; itr_dbllist.More(); itr_dbllist.Next())
  {
    if (itr_dbllist.Value() != 1.5 &&
	itr_dbllist.Value() != 2.5 &&
	itr_dbllist.Value() != 3.5)
    {
      return 4;
    }
  }
  getdbllist->Clear();

  // TDataStd_ExtStringList:
  // Set
  Handle(TDataStd_ExtStringList) setstrlist = TDataStd_ExtStringList::Set(doc->Main());
  setstrlist->Append("Hello");
  setstrlist->Prepend("Guten Tag");
  setstrlist->InsertAfter("Bonjour", "Guten Tag");
  setstrlist->InsertBefore("Bonsoir", "Hello");
  setstrlist->Append("Good bye");
  setstrlist->Remove("Bonsoir");
  setstrlist->Remove("Good bye");
  // Get
  Handle(TDataStd_ExtStringList) getstrlist;
  if (!doc->Main().FindAttribute(TDataStd_ExtStringList::GetID(), getstrlist))
    return 1;
  if (getstrlist->First() != "Guten Tag")
    return 2;
  if (getstrlist->Last() != "Hello")
    return 3;
  const TDataStd_ListOfExtendedString& strlist = getstrlist->List();
  TDataStd_ListIteratorOfListOfExtendedString itr_strlist(strlist);
  for (; itr_strlist.More(); itr_strlist.Next())
  {
    if (itr_strlist.Value() != "Guten Tag" &&
	itr_strlist.Value() != "Bonjour" &&
	itr_strlist.Value() != "Hello")
    {
      return 4;
    }
  }
  getstrlist->Clear();

  // TDataStd_BooleanList:
  // Set
  Handle(TDataStd_BooleanList) setboollist = TDataStd_BooleanList::Set(doc->Main());
  setboollist->Append(Standard_True);
  setboollist->Prepend(Standard_False);
  // Get
  Handle(TDataStd_BooleanList) getboollist;
  if (!doc->Main().FindAttribute(TDataStd_BooleanList::GetID(), getboollist))
    return 1;
  if (getboollist->First() != Standard_False)
    return 2;
  if (getboollist->Last() != Standard_True)
    return 3;
  const TDataStd_ListOfByte& boollist = getboollist->List();
  for (TDataStd_ListIteratorOfListOfByte itr_boollist(boollist); itr_boollist.More(); itr_boollist.Next())
  {
    if (itr_boollist.Value() != 1
     && itr_boollist.Value() != 0)
    {
      return 4;
    }
  }
  getboollist->Clear();

  // TDataStd_ReferenceList:
  TDF_Label L1 = doc->Main().FindChild(100);
  TDF_Label L2 = doc->Main().FindChild(101);
  TDF_Label L3 = doc->Main().FindChild(102);
  TDF_Label L4 = doc->Main().FindChild(103);
  TDF_Label L5 = doc->Main().FindChild(104);
  // Set
  Handle(TDataStd_ReferenceList) setreflist = TDataStd_ReferenceList::Set(doc->Main());
  setreflist->Append(L1);
  setreflist->Prepend(L2);
  setreflist->InsertAfter(L3, L2);
  setreflist->InsertBefore(L4, L1);
  setreflist->Append(L5);
  setreflist->Remove(L4);
  setreflist->Remove(L5);
  // Get
  Handle(TDataStd_ReferenceList) getreflist;
  if (!doc->Main().FindAttribute(TDataStd_ReferenceList::GetID(), getreflist))
    return 1;
  if (getreflist->First() != L2)
    return 2;
  if (getreflist->Last() != L1)
    return 3;
  const TDF_LabelList& reflist = getreflist->List();
  TDF_ListIteratorOfLabelList itr_reflist(reflist);
  for (; itr_reflist.More(); itr_reflist.Next())
  {
    if (itr_reflist.Value() != L1 &&
	itr_reflist.Value() != L2 &&
	itr_reflist.Value() != L3)
    {
      return 4;
    }
  }
  getreflist->Clear();

  // TDataStd_BooleanArray:
  // Set
  Handle(TDataStd_BooleanArray) setboolarr = TDataStd_BooleanArray::Set(doc->Main(), 12, 16);
  setboolarr->SetValue(12, Standard_True);
  setboolarr->SetValue(13, Standard_False);
  setboolarr->SetValue(14, Standard_False);
  setboolarr->SetValue(15, Standard_False);
  setboolarr->SetValue(16, Standard_True);
  setboolarr->SetValue(14, Standard_True);
  // Get
  Handle(TDataStd_BooleanArray) getboolarr;
  if (!doc->Main().FindAttribute(TDataStd_BooleanArray::GetID(), getboolarr))
    return 1;
  if (getboolarr->Value(12) != Standard_True)
    return 2;
  if (getboolarr->Value(13) != Standard_False)
    return 2;
  if (getboolarr->Value(14) != Standard_True)
    return 2;
  if (getboolarr->Value(15) != Standard_False)
    return 2;
  if (getboolarr->Value(16) != Standard_True)
    return 2;

  // TDataStd_ReferenceArray:
  // Set
  Handle(TDataStd_ReferenceArray) setrefarr = TDataStd_ReferenceArray::Set(doc->Main(), 0, 4);
  setrefarr->SetValue(0, L1);
  setrefarr->SetValue(1, L2);
  setrefarr->SetValue(2, L3);
  setrefarr->SetValue(3, L4);
  setrefarr->SetValue(4, L5);
  // Get
  Handle(TDataStd_ReferenceArray) getrefarr;
  if (!doc->Main().FindAttribute(TDataStd_ReferenceArray::GetID(), getrefarr))
    return 1;
  if (getrefarr->Value(0) != L1)
    return 2;
  if (getrefarr->Value(1) != L2)
    return 2;
  if (getrefarr->Value(2) != L3)
    return 2;
  if (getrefarr->Value(3) != L4)
    return 2;
  if (getrefarr->Value(4) != L5)
    return 2;

  // TDataStd_ByteArray:
  // Set
  Handle(TDataStd_ByteArray) setbytearr = TDataStd_ByteArray::Set(doc->Main(), 12, 16);
  setbytearr->SetValue(12, 0);
  setbytearr->SetValue(13, 1);
  setbytearr->SetValue(14, 2);
  setbytearr->SetValue(15, 3);
  setbytearr->SetValue(16, 255);
  // Get
  Handle(TDataStd_ByteArray) getbytearr;
  if (!doc->Main().FindAttribute(TDataStd_ByteArray::GetID(), getbytearr))
    return 1;
  if (getbytearr->Value(12) != 0)
    return 2;
  if (getbytearr->Value(13) != 1)
    return 2;
  if (getbytearr->Value(14) != 2)
    return 2;
  if (getbytearr->Value(15) != 3)
    return 2;
  if (getbytearr->Value(16) != 255)
    return 2;
  
  // TDataStd_NamedData:
  // Set:
  Handle(TDataStd_NamedData) setnd = TDataStd_NamedData::Set(doc->Main());
  setnd->SetInteger("Integer1", 1);
  setnd->SetInteger("Integer2", 2);
  setnd->SetInteger("Integer3", 8);
  setnd->SetInteger("Integer3", 3);
  // Get:
  Handle(TDataStd_NamedData) getnd;
  if (!doc->Main().FindAttribute(TDataStd_NamedData::GetID(), getnd))
    return 1;
  if (!getnd->HasIntegers())
    return 2;
  if (!getnd->HasInteger("Integer1"))
    return 3;
  if (getnd->GetInteger("Integer2") != 2)
    return 4;
  if (getnd->GetInteger("Integer3") != 3)
    return 4;

  return 0;
}

int TestUndoRedo(const Handle(TDocStd_Document)& doc)
{
  // TDataStd_Tick:
  doc->OpenCommand();
  Handle(TDataStd_Tick) tick = TDataStd_Tick::Set(doc->Main());
  doc->CommitCommand();
  if (!doc->Main().IsAttribute(TDataStd_Tick::GetID()))
    return 1;
  doc->Undo();
  if (doc->Main().IsAttribute(TDataStd_Tick::GetID()))
    return 2;
  doc->Redo();
  if (!doc->Main().IsAttribute(TDataStd_Tick::GetID()))
    return 3;

  // TDataStd_IntegerList:
  doc->OpenCommand();
  Handle(TDataStd_IntegerList) intlist = TDataStd_IntegerList::Set(doc->Main());
  intlist->Append(2);
  intlist->Prepend(1);
  intlist->InsertBefore(0, 1);
  intlist->InsertAfter(3, 2);
  doc->CommitCommand();
  if (!doc->Main().IsAttribute(TDataStd_IntegerList::GetID()))
    return 1;
  doc->Undo();
  if (!intlist->IsEmpty())
    return 2;
  doc->Redo();
  if (!intlist->Extent())
    return 3;
  if (intlist->First() != 0)
    return 4;
  if (intlist->Last() != 3)
    return 5;
  intlist->Clear();

  // TDataStd_RealList:
  doc->OpenCommand();
  Handle(TDataStd_RealList) dbllist = TDataStd_RealList::Set(doc->Main());
  dbllist->Append(2.5);
  dbllist->Prepend(1.5);
  dbllist->InsertBefore(0.5, 1.5);
  dbllist->InsertAfter(3.5, 2.5);
  doc->CommitCommand();
  if (!doc->Main().IsAttribute(TDataStd_RealList::GetID()))
    return 1;
  doc->Undo();
  if (!dbllist->IsEmpty())
    return 2;
  doc->Redo();
  if (!dbllist->Extent())
    return 3;
  if (dbllist->First() != 0.5)
    return 4;
  if (dbllist->Last() != 3.5)
    return 5;
  dbllist->Clear();

  // TDataStd_ExtStringList:
  doc->OpenCommand();
  Handle(TDataStd_ExtStringList) strlist = TDataStd_ExtStringList::Set(doc->Main());
  strlist->Append("Hello");
  strlist->Prepend("Guten Tag");
  strlist->InsertAfter("Bonjour", "Guten Tag");
  strlist->InsertBefore("Bonsoir", "Hello");
  doc->CommitCommand();
  if (!doc->Main().IsAttribute(TDataStd_ExtStringList::GetID()))
    return 1;
  doc->Undo();
  if (!strlist->IsEmpty())
    return 2;
  doc->Redo();
  if (!strlist->Extent())
    return 3;
  if (strlist->First() != "Guten Tag")
    return 4;
  if (strlist->Last() != "Hello")
    return 5;
  strlist->Clear();

  // TDataStd_BooleanList:
  doc->OpenCommand();
  Handle(TDataStd_BooleanList) boollist = TDataStd_BooleanList::Set(doc->Main());
  boollist->Append(Standard_True);
  boollist->Prepend(Standard_False);
  doc->CommitCommand();
  if (!doc->Main().IsAttribute(TDataStd_BooleanList::GetID()))
    return 1;
  doc->Undo();
  if (!boollist->IsEmpty())
    return 2;
  doc->Redo();
  if (!boollist->Extent())
    return 3;
  if (boollist->First() != Standard_False)
    return 4;
  if (boollist->Last() != Standard_True)
    return 5;
  boollist->Clear();

  // TDataStd_ReferenceList:
  TDF_Label L1 = doc->Main().FindChild(100);
  TDF_Label L2 = doc->Main().FindChild(101);
  TDF_Label L3 = doc->Main().FindChild(102);
  TDF_Label L4 = doc->Main().FindChild(103);
  doc->OpenCommand();
  Handle(TDataStd_ReferenceList) reflist = TDataStd_ReferenceList::Set(doc->Main());
  reflist->Append(L1);
  reflist->Prepend(L2);
  reflist->InsertBefore(L3, L1);
  reflist->InsertAfter(L4, L2);
  doc->CommitCommand();
  if (!doc->Main().IsAttribute(TDataStd_ReferenceList::GetID()))
    return 1;
  doc->Undo();
  if (!reflist->IsEmpty())
    return 2;
  doc->Redo();
  if (!reflist->Extent())
    return 3;
  if (reflist->First() != L2)
    return 4;
  if (reflist->Last() != L1)
    return 5;
  reflist->Clear();

  // TDataStd_BooleanArray:
  doc->OpenCommand();
  Handle(TDataStd_BooleanArray) boolarr = TDataStd_BooleanArray::Set(doc->Main(), 23, 25);
  boolarr->SetValue(23, Standard_True);
  boolarr->SetValue(25, Standard_True);
  doc->CommitCommand();
  doc->OpenCommand();
  boolarr = TDataStd_BooleanArray::Set(doc->Main(), 230, 250);
  boolarr->SetValue(230, Standard_True);
  boolarr->SetValue(250, Standard_True);  
  doc->CommitCommand();
  doc->Undo();
  if (boolarr->Value(23) != Standard_True)
    return 2;
  if (boolarr->Value(24) != Standard_False)
    return 2;
  if (boolarr->Value(25) != Standard_True)
    return 2;
  doc->Redo();
  if (boolarr->Value(230) != Standard_True)
    return 3;
  if (boolarr->Value(240) != Standard_False)
    return 3;
  if (boolarr->Value(250) != Standard_True)
    return 3;

  // TDataStd_ReferenceArray:
  doc->OpenCommand();
  Handle(TDataStd_ReferenceArray) refarr = TDataStd_ReferenceArray::Set(doc->Main(), 5, 8);
  refarr->SetValue(5, L1);
  refarr->SetValue(6, L2);
  refarr->SetValue(7, L3);
  refarr->SetValue(8, L4);
  doc->CommitCommand();
  if (!doc->Main().IsAttribute(TDataStd_ReferenceArray::GetID()))
    return 1;
  doc->Undo();
  doc->Redo();
  if (refarr->Value(5) != L1)
    return 4;
  if (refarr->Value(6) != L2)
    return 4;
  if (refarr->Value(7) != L3)
    return 4;
  if (refarr->Value(8) != L4)
    return 4;

  // TDataStd_ByteArray:
  doc->OpenCommand();
  Handle(TDataStd_ByteArray) bytearr = TDataStd_ByteArray::Set(doc->Main(), 23, 25);
  bytearr->SetValue(23, 23);
  bytearr->SetValue(25, 25);
  doc->CommitCommand();
  doc->OpenCommand();
  bytearr = TDataStd_ByteArray::Set(doc->Main(), 230, 250);
  bytearr->SetValue(230, 230);
  bytearr->SetValue(250, 250);  
  doc->CommitCommand();
  doc->Undo();
  if (bytearr->Value(23) != 23)
    return 2;
  if (bytearr->Value(25) != 25)
    return 2;
  doc->Redo();
  if (bytearr->Value(230) != 230)
    return 3;
  if (bytearr->Value(250) != 250)
    return 3;

  // TDataStd_NamedData:
  doc->OpenCommand();
  Handle(TDataStd_NamedData) nd = TDataStd_NamedData::Set(doc->Main());
  nd->SetByte("b14", 12);
  nd->SetByte("b17", 18);
  nd->SetByte("b14", 14);
  nd->SetByte("b17", 17);
  doc->CommitCommand();
  doc->OpenCommand();
  nd = TDataStd_NamedData::Set(doc->Main());
  nd->SetReal("r14", 14);
  nd->SetReal("r17", 17);
  nd->SetReal("r14", 14.4);
  nd->SetReal("r17", 17.7);
  doc->CommitCommand();
  doc->Undo();
  if (nd->HasStrings())
    return 1;
  if (nd->HasReals())
    return 1;
  if (nd->HasReal("r17"))
    return 2;
  if (!nd->HasBytes())
    return 3;
  if (nd->GetByte("b14") != 14)
    return 4;
  if (nd->GetByte("b17") != 17)
    return 4;
  if (nd->HasByte("b18"))
    return 5;
  doc->Redo();
  if (!nd->HasBytes())
    return 1;
  if (!nd->HasReals())
    return 1;
  if (nd->GetByte("b14") != 14)
    return 2;
  if (nd->GetReal("r14") != 14.4)
    return 2;
  if (nd->GetReal("r17") != 17.7)
    return 2;

  return 0;
}

int TestCopyPaste(const Handle(TDocStd_Document)& doc)
{
  TDF_Label L1 = doc->Main().FindChild(1);
  TDF_Label L2 = doc->Main().FindChild(2);
  TDF_CopyLabel copier(L1, L2);

  // TDataStd_Tick:
  TDataStd_Tick::Set(L1);
  copier.Perform();
  if (!copier.IsDone())
    return 1;
  if (!L2.IsAttribute(TDataStd_Tick::GetID()))
    return 2;

  // TDataStd_IntegerList:
  Handle(TDataStd_IntegerList) intlist = TDataStd_IntegerList::Set(L1);
  intlist->Append(1);
  intlist->InsertAfter(2, 1);
  copier.Perform();
  if (!copier.IsDone())
    return 1;
  intlist->Clear();
  intlist.Nullify();
  if (!L2.FindAttribute(TDataStd_IntegerList::GetID(), intlist))
    return 2;
  if (intlist->First() != 1)
    return 3;
  if (intlist->Last() != 2)
    return 4;
  intlist->Clear();

  // TDataStd_RealList:
  Handle(TDataStd_RealList) dbllist = TDataStd_RealList::Set(L1);
  dbllist->Append(1.5);
  dbllist->InsertAfter(2.5, 1.5);
  copier.Perform();
  if (!copier.IsDone())
    return 1;
  dbllist->Clear();
  dbllist.Nullify();
  if (!L2.FindAttribute(TDataStd_RealList::GetID(), dbllist))
    return 2;
  if (dbllist->First() != 1.5)
    return 3;
  if (dbllist->Last() != 2.5)
    return 4;
  dbllist->Clear();

  // TDataStd_ExtStringList:
  Handle(TDataStd_ExtStringList) strlist = TDataStd_ExtStringList::Set(L1);
  strlist->Append("Open CASCADE");
  strlist->InsertAfter(" - is the best set of libraries!", "Open CASCADE");
  copier.Perform();
  if (!copier.IsDone())
    return 1;
  strlist->Clear();
  strlist.Nullify();
  if (!L2.FindAttribute(TDataStd_ExtStringList::GetID(), strlist))
    return 2;
  if (strlist->First() != "Open CASCADE")
    return 3;
  if (strlist->Last() != " - is the best set of libraries!")
    return 4;
  strlist->Clear();

  // TDataStd_BooleanList:
  Handle(TDataStd_BooleanList) boollist = TDataStd_BooleanList::Set(L1);
  boollist->Append(Standard_True);
  boollist->Prepend(Standard_False);
  copier.Perform();
  if (!copier.IsDone())
    return 1;
  boollist->Clear();
  boollist.Nullify();
  if (!L2.FindAttribute(TDataStd_BooleanList::GetID(), boollist))
    return 2;
  if (boollist->First() != Standard_False)
    return 3;
  if (boollist->Last() != Standard_True)
    return 4;
  boollist->Clear();

  // TDataStd_ReferenceList:
  TDF_Label L100 = doc->Main().FindChild(100);
  TDF_Label L101 = doc->Main().FindChild(101);
  Handle(TDataStd_ReferenceList) reflist = TDataStd_ReferenceList::Set(L1);
  reflist->Append(L100);
  reflist->InsertAfter(L101, L100);
  copier.Perform();
  if (!copier.IsDone())
    return 1;
  reflist->Clear();
  reflist.Nullify();
  if (!L2.FindAttribute(TDataStd_ReferenceList::GetID(), reflist))
    return 2;
  if (reflist->First() != L100)
    return 3;
  if (reflist->Last() != L101)
    return 4;
  reflist->Clear();

  // TDataStd_BooleanArray:
  Handle(TDataStd_BooleanArray) boolarr = TDataStd_BooleanArray::Set(L1, 4, 6);
  boolarr->SetValue(4, Standard_True);
  boolarr->SetValue(6, Standard_True);
  copier.Perform();
  if (!copier.IsDone())
    return 1;
  boolarr.Nullify();
  if (!L2.FindAttribute(TDataStd_BooleanArray::GetID(), boolarr))
    return 2;
  if (boolarr->Value(4) != Standard_True)
    return 3;
  if (boolarr->Value(5) != Standard_False)
    return 3;
  if (boolarr->Value(6) != Standard_True)
    return 3;

  // TDataStd_ReferenceArray:
  Handle(TDataStd_ReferenceArray) refarr = TDataStd_ReferenceArray::Set(L1, 3, 4);
  refarr->SetValue(3, L100);
  refarr->SetValue(4, L101);
  copier.Perform();
  if (!copier.IsDone())
    return 1;
  refarr.Nullify();
  if (!L2.FindAttribute(TDataStd_ReferenceArray::GetID(), refarr))
    return 2;
  if (refarr->Value(3) != L100)
    return 3;
  if (refarr->Value(4) != L101)
    return 3;

  // TDataStd_ByteArray:
  Handle(TDataStd_ByteArray) bytearr = TDataStd_ByteArray::Set(L1, 4, 6);
  bytearr->SetValue(4, 40);
  bytearr->SetValue(6, 60);
  copier.Perform();
  if (!copier.IsDone())
    return 1;
  bytearr.Nullify();
  if (!L2.FindAttribute(TDataStd_ByteArray::GetID(), bytearr))
    return 2;
  if (bytearr->Value(4) != 40)
    return 3;
  if (bytearr->Value(6) != 60)
    return 3;

  // TDataStd_NamedData:
  Handle(TDataStd_NamedData) nd = TDataStd_NamedData::Set(L1);
  nd->SetInteger("Integer1", 11);
  nd->SetReal("Real1", 11.1);
  nd->SetString("String1", "11.11111111");
  nd->SetByte("Byte1", 111);
  Handle(TColStd_HArray1OfInteger) ints_arr = new TColStd_HArray1OfInteger(4, 5);
  ints_arr->SetValue(4, 4);
  ints_arr->SetValue(5, 5);
  nd->SetArrayOfIntegers("Integers1", ints_arr);
  copier.Perform();
  if (!copier.IsDone())
    return 1;
  nd.Nullify();
  if (!L2.FindAttribute(TDataStd_NamedData::GetID(), nd))
    return 2;
  if (!nd->HasIntegers())
    return 3;
  if (!nd->HasReals())
    return 3;
  if (!nd->HasStrings())
    return 3;
  if (!nd->HasBytes())
    return 3;
  if (!nd->HasArraysOfIntegers())
    return 3;
  if (nd->HasArraysOfReals())
    return 3;
  if (!nd->HasInteger("Integer1"))
    return 4;
  if (nd->GetInteger("Integer1") != 11)
    return 4;
  if (!nd->HasReal("Real1"))
    return 4;
  if (nd->GetReal("Real1") != 11.1)
    return 4;
  if (!nd->HasString("String1"))
    return 4;
  if (nd->GetString("String1") != "11.11111111")
    return 4;
  if (!nd->HasByte("Byte1"))
    return 4;
  if (nd->GetByte("Byte1") != 111)
    return 4;
  if (!nd->HasArrayOfIntegers("Integers1"))
    return 4;
  const Handle(TColStd_HArray1OfInteger)& ints_arr_out = nd->GetArrayOfIntegers("Integers1");
  if (ints_arr_out.IsNull())
    return 4;
  if (ints_arr_out->Value(5) != 5)
    return 4;

  return 0;
}

int TestOpenSave(TCollection_ExtendedString aFile1,
		 TCollection_ExtendedString aFile2,
		 TCollection_ExtendedString aFile3)
{
  // Std
  Handle(TDocStd_Document) doc_std, doc_std_open;
  app->NewDocument("BinOcaf", doc_std);
  // TDataStd_Tick:
  TDataStd_Tick::Set(doc_std->Main());
  // TDataStd_IntegerList:
  Handle(TDataStd_IntegerList) intlist = TDataStd_IntegerList::Set(doc_std->Main());
  intlist->Append(1);
  intlist->Append(5);
  // TDataStd_RealList:
  Handle(TDataStd_RealList) dbllist = TDataStd_RealList::Set(doc_std->Main());
  dbllist->Append(1.5);
  dbllist->Append(5.5);
  // TDataStd_ExtStringList:
  Handle(TDataStd_ExtStringList) strlist = TDataStd_ExtStringList::Set(doc_std->Main());
  strlist->Append("Auf");
  strlist->Append("Wiedersehen");
  // TDataStd_BooleanList:
  Handle(TDataStd_BooleanList) boollist = TDataStd_BooleanList::Set(doc_std->Main());
  boollist->Append(Standard_False);
  boollist->Append(Standard_True);
  // TDataStd_ReferenceList:
  TCollection_AsciiString entry1, entry2, entry_first, entry_last;
  TDF_Label Lstd1 = doc_std->Main().FindChild(100);
  TDF_Tool::Entry(Lstd1, entry1);
  TDF_Label Lstd2 = doc_std->Main().FindChild(101);
  TDF_Tool::Entry(Lstd2, entry2);
  Handle(TDataStd_ReferenceList) reflist = TDataStd_ReferenceList::Set(doc_std->Main());
  reflist->Append(Lstd1);
  reflist->Append(Lstd2);
  // TDataStd_BooleanArray:
  Handle(TDataStd_BooleanArray) boolarr = TDataStd_BooleanArray::Set(doc_std->Main(), 15, 18);
  boolarr->SetValue(15, Standard_False);
  boolarr->SetValue(16, Standard_True);
  boolarr->SetValue(17, Standard_True);
  boolarr->SetValue(18, Standard_True);
  // TDataStd_ReferenceArray:
  Handle(TDataStd_ReferenceArray) refarr = TDataStd_ReferenceArray::Set(doc_std->Main(), 45, 46);
  refarr->SetValue(45, Lstd1);
  refarr->SetValue(46, Lstd2);
  // TDataStd_ByteArray:
  Handle(TDataStd_ByteArray) bytearr = TDataStd_ByteArray::Set(doc_std->Main(), 15, 18);
  bytearr->SetValue(15, 150);
  bytearr->SetValue(16, 160);
  bytearr->SetValue(17, 170);
  bytearr->SetValue(18, 180);
  // TDataStd_NamedData:
  Handle(TDataStd_NamedData) nameddata = TDataStd_NamedData::Set(doc_std->Main());
  // TDF_Reference:
  TDF_Label Lstd3 = doc_std->Main().FindChild(103);
  Handle(TDF_Reference) ref = TDF_Reference::Set(doc_std->Main(), Lstd3);
  // 
  // Save
  //if (app->SaveAs(doc_std, "W:\\doc.std") != PCDM_SS_OK)
  if (app->SaveAs(doc_std, aFile1) != PCDM_SS_OK)
    return 1;
  intlist.Nullify();
  dbllist.Nullify();
  strlist.Nullify();
  boollist.Nullify();
  reflist.Nullify();
  boolarr.Nullify();
  ref.Nullify();
  app->Close(doc_std);
  doc_std.Nullify();
  //if (app->Open("W:\\doc.std", doc_std_open) != PCDM_RS_OK)
  if (app->Open(aFile1, doc_std_open) != PCDM_RS_OK)
    return 2;
  if (!doc_std_open->Main().IsAttribute(TDataStd_Tick::GetID()))
    return 3;
  if (!doc_std_open->Main().FindAttribute(TDataStd_IntegerList::GetID(), intlist))
    return 4;
  if (intlist->First() != 1)
    return 5;
  if (intlist->Last() != 5)
    return 6;
  if (!doc_std_open->Main().FindAttribute(TDataStd_RealList::GetID(), dbllist))
    return 4;
  if (dbllist->First() != 1.5)
    return 5;
  if (dbllist->Last() != 5.5)
    return 6;
  if (!doc_std_open->Main().FindAttribute(TDataStd_ExtStringList::GetID(), strlist))
    return 4;
  if (strlist->First() != "Auf")
    return 5;
  if (strlist->Last() != "Wiedersehen")
    return 6;
  if (!doc_std_open->Main().FindAttribute(TDataStd_BooleanList::GetID(), boollist))
    return 4;
  if (boollist->First() != Standard_False)
    return 5;
  if (boollist->Last() != Standard_True)
    return 6;
  if (!doc_std_open->Main().FindAttribute(TDataStd_ReferenceList::GetID(), reflist))
    return 4;
  TDF_Tool::Entry(reflist->First(), entry_first);
  if (entry1 != entry_first)
    return 5;
  TDF_Tool::Entry(reflist->Last(), entry_last);
  if (entry2 != entry_last)
    return 6;
  if (!doc_std_open->Main().FindAttribute(TDataStd_BooleanArray::GetID(), boolarr))
    return 4;
  if (boolarr->Value(15) != Standard_False)
    return 5;
  if (boolarr->Value(16) != Standard_True)
    return 5;
  if (boolarr->Value(17) != Standard_True)
    return 5;
  if (boolarr->Value(18) != Standard_True)
    return 5;
  if (!doc_std_open->Main().FindAttribute(TDataStd_ReferenceArray::GetID(), refarr))
    return 4;
  TDF_Tool::Entry(refarr->Value(45), entry_first);
  if (entry1 != entry_first)
    return 5;
  TDF_Tool::Entry(refarr->Value(46), entry_last);
  if (entry2 != entry_last)
    return 6;
  if (!doc_std_open->Main().FindAttribute(TDataStd_ByteArray::GetID(), bytearr))
    return 4;
  if (bytearr->Value(15) != 150)
    return 5;
  if (bytearr->Value(16) != 160)
    return 5;
  if (bytearr->Value(17) != 170)
    return 5;
  if (bytearr->Value(18) != 180)
    return 5;
  if (!doc_std_open->Main().FindAttribute(TDF_Reference::GetID(), ref))
    return 4;
  if (ref->Get().IsNull())
    return 5;
  if (ref->Get().Tag() != 103)
      return 5;

  // Xml
  Handle(TDocStd_Document) doc_xml, doc_xml_open;
  app->NewDocument("XmlOcaf", doc_xml);
  // TDataStd_Tick:
  TDataStd_Tick::Set(doc_xml->Main());
  // TDataStd_IntegerList:
  intlist = TDataStd_IntegerList::Set(doc_xml->Main());
  intlist->Append(1);
  intlist->Append(5);
  // TDataStd_RealList:
  dbllist = TDataStd_RealList::Set(doc_xml->Main());
  dbllist->Append(1.5);
  dbllist->Append(5.5);
  // TDataStd_ExtStringList:
  strlist = TDataStd_ExtStringList::Set(doc_xml->Main());
  strlist->Append("Guten ");
  strlist->Append("Tag");
  // TDataStd_BooleanList:
  boollist = TDataStd_BooleanList::Set(doc_xml->Main());
  boollist->Append(Standard_False);
  boollist->Append(Standard_True);
  // TDataStd_ReferenceList:
  TDF_Label Lxml1 = doc_xml->Main().FindChild(100);
  TDF_Tool::Entry(Lxml1, entry1);
  TDF_Label Lxml2 = doc_xml->Main().FindChild(101);
  TDF_Tool::Entry(Lxml2, entry2);
  reflist = TDataStd_ReferenceList::Set(doc_xml->Main());
  reflist->Append(Lxml1);
  reflist->Append(Lxml2);
  // TDataStd_BooleanArray:
  boolarr = TDataStd_BooleanArray::Set(doc_xml->Main(), 15, 24);
  boolarr->SetValue(15, Standard_False);
  boolarr->SetValue(16, Standard_True);
  boolarr->SetValue(17, Standard_True);
  boolarr->SetValue(18, Standard_True);
  boolarr->SetValue(19, Standard_True);
  boolarr->SetValue(20, Standard_True);
  boolarr->SetValue(21, Standard_False);
  boolarr->SetValue(22, Standard_True);
  boolarr->SetValue(23, Standard_True);
  boolarr->SetValue(24, Standard_True);
  // TDataStd_ReferenceArray:
  refarr = TDataStd_ReferenceArray::Set(doc_xml->Main(), 444, 445);
  refarr->SetValue(444, Lxml1);
  refarr->SetValue(445, Lxml2);
  // TDataStd_ByteArray:
  bytearr = TDataStd_ByteArray::Set(doc_xml->Main(), 15, 24);
  bytearr->SetValue(15, 0);
  bytearr->SetValue(16, 10);
  bytearr->SetValue(17, 100);
  bytearr->SetValue(18, 200);
  bytearr->SetValue(19, 250);
  bytearr->SetValue(20, 251);
  bytearr->SetValue(21, 252);
  bytearr->SetValue(22, 253);
  bytearr->SetValue(23, 254);
  bytearr->SetValue(24, 255);
  // TDF_Reference:
  Lstd3 = doc_xml->Main().FindChild(103);
  ref = TDF_Reference::Set(doc_xml->Main(), Lstd3);
  // 
  // Save
  //if (app->SaveAs(doc_xml, "W:\\doc.xml") != PCDM_SS_OK)
  if (app->SaveAs(doc_xml, aFile2) != PCDM_SS_OK)
    return 1;
  intlist.Nullify();
  ref.Nullify();
  app->Close(doc_xml);
  doc_xml.Nullify();
  //if (app->Open("W:\\doc.xml", doc_xml_open) != PCDM_RS_OK)
  if (app->Open(aFile2, doc_xml_open) != PCDM_RS_OK)
    return 2;
  if (!doc_xml_open->Main().IsAttribute(TDataStd_Tick::GetID()))
    return 3;
  if (!doc_xml_open->Main().FindAttribute(TDataStd_IntegerList::GetID(), intlist))
    return 4;
  if (intlist->First() != 1)
    return 5;
  if (intlist->Last() != 5)
    return 6;
  if (!doc_xml_open->Main().FindAttribute(TDataStd_RealList::GetID(), dbllist))
    return 4;
  if (dbllist->First() != 1.5)
    return 5;
  if (dbllist->Last() != 5.5)
    return 6;
  if (!doc_xml_open->Main().FindAttribute(TDataStd_ExtStringList::GetID(), strlist))
    return 4;
  if (strlist->First() != "Guten ")
    return 5;
  if (strlist->Last() != "Tag")
    return 6;
  if (!doc_xml_open->Main().FindAttribute(TDataStd_BooleanList::GetID(), boollist))
    return 4;
  if (boollist->First() != Standard_False)
    return 5;
  if (boollist->Last() != Standard_True)
    return 6;
  if (!doc_xml_open->Main().FindAttribute(TDataStd_ReferenceList::GetID(), reflist))
    return 4;
  TDF_Tool::Entry(reflist->First(), entry_first);
  if (entry1 != entry_first)
    return 5;
  TDF_Tool::Entry(reflist->Last(), entry_last);
  if (entry2 != entry_last)
    return 6;
  if (!doc_xml_open->Main().FindAttribute(TDataStd_BooleanArray::GetID(), boolarr))
    return 4;
  if (boolarr->Value(15) != Standard_False)
    return 5;
  if (boolarr->Value(16) != Standard_True)
    return 5;
  if (boolarr->Value(17) != Standard_True)
    return 5;
  if (boolarr->Value(18) != Standard_True)
    return 5;
  if (boolarr->Value(19) != Standard_True)
    return 5;
  if (boolarr->Value(20) != Standard_True)
    return 5;
  if (boolarr->Value(21) != Standard_False)
    return 5;
  if (boolarr->Value(22) != Standard_True)
    return 5;
  if (boolarr->Value(23) != Standard_True)
    return 5;
  if (boolarr->Value(24) != Standard_True)
    return 5;
  if (!doc_xml_open->Main().FindAttribute(TDataStd_ReferenceArray::GetID(), refarr))
    return 4;
  TDF_Tool::Entry(refarr->Value(444), entry_first);
  if (entry1 != entry_first)
    return 5;
  TDF_Tool::Entry(refarr->Value(445), entry_last);
  if (entry2 != entry_last)
    return 6;
  if (!doc_xml_open->Main().FindAttribute(TDataStd_ByteArray::GetID(), bytearr))
    return 4;
  if (bytearr->Value(15) != 0)
    return 5;
  if (bytearr->Value(16) != 10)
    return 5;
  if (bytearr->Value(17) != 100)
    return 5;
  if (bytearr->Value(18) != 200)
    return 5;
  if (bytearr->Value(19) != 250)
    return 5;
  if (bytearr->Value(20) != 251)
    return 5;
  if (bytearr->Value(21) != 252)
    return 5;
  if (bytearr->Value(22) != 253)
    return 5;
  if (bytearr->Value(23) != 254)
    return 5;
  if (bytearr->Value(24) != 255)
    return 5;
  if (!doc_xml_open->Main().FindAttribute(TDF_Reference::GetID(), ref))
    return 4;
  if (ref->Get().IsNull())
    return 5;
  if (ref->Get().Tag() != 103)
      return 5;

  // Bin
  Handle(TDocStd_Document) doc_bin, doc_bin_open;
  app->NewDocument("BinOcaf", doc_bin);
  // TDataStd_Tick:
  TDataStd_Tick::Set(doc_bin->Main());
  // TDataStd_IntegerList:
  intlist = TDataStd_IntegerList::Set(doc_bin->Main());
  intlist->Append(1);
  intlist->Append(5);
  // TDataStd_RealList:
  dbllist = TDataStd_RealList::Set(doc_bin->Main());
  dbllist->Append(1.5);
  dbllist->Append(5.5);
  // TDataStd_ExtStringList:
  strlist = TDataStd_ExtStringList::Set(doc_bin->Main());
  strlist->Append("Bonjour");
  strlist->Append("Bonsoir");
  // TDataStd_BooleanList:
  boollist = TDataStd_BooleanList::Set(doc_bin->Main());
  boollist->Append(Standard_False);
  boollist->Append(Standard_True);
  // TDataStd_ReferenceList:
  TDF_Label Lbin1 = doc_bin->Main().FindChild(100);
  TDF_Tool::Entry(Lbin1, entry1);
  TDF_Label Lbin2 = doc_bin->Main().FindChild(101);
  TDF_Tool::Entry(Lbin2, entry2);
  reflist = TDataStd_ReferenceList::Set(doc_bin->Main());
  reflist->Append(Lbin1);
  reflist->Append(Lbin2);
  // TDataStd_BooleanArray:
  boolarr = TDataStd_BooleanArray::Set(doc_bin->Main(), 15, 24);
  boolarr->SetValue(15, Standard_False);
  boolarr->SetValue(16, Standard_True);
  boolarr->SetValue(17, Standard_True);
  boolarr->SetValue(18, Standard_True);
  boolarr->SetValue(19, Standard_True);
  boolarr->SetValue(20, Standard_True);
  boolarr->SetValue(21, Standard_False);
  boolarr->SetValue(22, Standard_True);
  boolarr->SetValue(23, Standard_True);
  boolarr->SetValue(24, Standard_True);
  // TDataStd_ReferenceArray:
  refarr = TDataStd_ReferenceArray::Set(doc_bin->Main(), 0, 1);
  refarr->SetValue(0, Lbin1);
  refarr->SetValue(1, Lbin2);
  // TDataStd_ByteArray:
  bytearr = TDataStd_ByteArray::Set(doc_bin->Main(), 15, 16);
  bytearr->SetValue(15, 0);
  bytearr->SetValue(16, 255);
  // TDataStd_NamedData:
  nameddata = TDataStd_NamedData::Set(doc_bin->Main());
  nameddata->SetByte("A", 12);
  nameddata->SetByte("B", 234);
  // TDF_Reference:
  Lstd3 = doc_bin->Main().FindChild(103);
  ref = TDF_Reference::Set(doc_bin->Main(), Lstd3);
  // 
  // Save
  //if (app->SaveAs(doc_bin, "W:\\doc.cbf") != PCDM_SS_OK)
  if (app->SaveAs(doc_bin, aFile3) != PCDM_SS_OK)
    return 1;
  intlist.Nullify();
  ref.Nullify();
  app->Close(doc_bin);
  doc_bin.Nullify();
  //if (app->Open("W:\\doc.cbf", doc_bin_open) != PCDM_RS_OK)
  if (app->Open(aFile3, doc_bin_open) != PCDM_RS_OK)
    return 2;
  if (!doc_bin_open->Main().IsAttribute(TDataStd_Tick::GetID()))
    return 3;
  if (!doc_bin_open->Main().FindAttribute(TDataStd_IntegerList::GetID(), intlist))
    return 4;
  if (intlist->First() != 1)
    return 5;
  if (intlist->Last() != 5)
    return 6;
  if (!doc_bin_open->Main().FindAttribute(TDataStd_RealList::GetID(), dbllist))
    return 4;
  if (dbllist->First() != 1.5)
    return 5;
  if (dbllist->Last() != 5.5)
    return 6;
  if (!doc_bin_open->Main().FindAttribute(TDataStd_ExtStringList::GetID(), strlist))
    return 4;
  if (strlist->First() != "Bonjour")
    return 5;
  if (strlist->Last() != "Bonsoir")
    return 6;
  if (!doc_bin_open->Main().FindAttribute(TDataStd_BooleanList::GetID(), boollist))
    return 4;
  if (boollist->First() != Standard_False)
    return 5;
  if (boollist->Last() != Standard_True)
    return 6;
  if (!doc_bin_open->Main().FindAttribute(TDataStd_ReferenceList::GetID(), reflist))
    return 4;
  TDF_Tool::Entry(reflist->First(), entry_first);
  if (entry1 != entry_first)
    return 5;
  TDF_Tool::Entry(reflist->Last(), entry_last);
  if (entry2 != entry_last)
    return 6;
  if (!doc_bin_open->Main().FindAttribute(TDataStd_BooleanArray::GetID(), boolarr))
    return 4;
  if (boolarr->Value(15) != Standard_False)
    return 5;
  if (boolarr->Value(16) != Standard_True)
    return 5;
  if (boolarr->Value(17) != Standard_True)
    return 5;
  if (boolarr->Value(18) != Standard_True)
    return 5;
  if (boolarr->Value(19) != Standard_True)
    return 5;
  if (boolarr->Value(20) != Standard_True)
    return 5;
  if (boolarr->Value(21) != Standard_False)
    return 5;
  if (boolarr->Value(22) != Standard_True)
    return 5;
  if (boolarr->Value(23) != Standard_True)
    return 5;
  if (boolarr->Value(24) != Standard_True)
    return 5;
  if (!doc_bin_open->Main().FindAttribute(TDataStd_ReferenceArray::GetID(), refarr))
    return 4;
  TDF_Tool::Entry(refarr->Value(0), entry_first);
  if (entry1 != entry_first)
    return 5;
  TDF_Tool::Entry(refarr->Value(1), entry_last);
  if (entry2 != entry_last)
    return 6;
  if (!doc_bin_open->Main().FindAttribute(TDataStd_ByteArray::GetID(), bytearr))
    return 4;
  if (bytearr->Value(15) != 0)
    return 5;
  if (bytearr->Value(16) != 255)
    return 5;
  if (!doc_bin_open->Main().FindAttribute(TDataStd_NamedData::GetID(), nameddata))
    return 4;
  if (nameddata->GetByte("A") != 12)
    return 5;
  if (nameddata->GetByte("B") != 234)
    return 5;
  if (!doc_bin_open->Main().FindAttribute(TDF_Reference::GetID(), ref))
    return 4;
  if (ref->Get().IsNull())
    return 5;
  if (ref->Get().Tag() != 103)
      return 5;

  return 0;
}
// For OCC16782 testing

static Standard_Integer OCC16782 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " file.std file.xml file.cbf\n";
    return 1;
  }
  TCollection_ExtendedString aFile1(argv[1]);
  TCollection_ExtendedString aFile2(argv[2]);
  TCollection_ExtendedString aFile3(argv[3]);

  if (app.IsNull())
    app = new AppStd_Application();

  int good = 0;
  
  Handle(TDocStd_Document) doc;
  app->NewDocument("BinOcaf", doc);
  doc->SetUndoLimit(10);

  di <<"\nTestSetGet start\n";
  good += TestSetGet(doc);
  di <<"TestSetGet finish\n";
  di <<"Status = " << good << "\n";

  di <<"\nTestUndoRedo start\n";
  good += TestUndoRedo(doc);
  di <<"TestUndoRedo finish\n";
  di <<"Status = " << good << "\n";

  di <<"\nTestCopyPaste start\n";
  good += TestCopyPaste(doc);
  di <<"TestCopyPaste finish\n";
  di <<"Status = " << good << "\n";

  di <<"\nTestOpenSave start\n";
  good += TestOpenSave(aFile1, aFile2, aFile3);
  di <<"TestOpenSave finish\n";
  di <<"Status = " << good << "\n";

  if (!good)
    di <<"\nThe " << argv[0] << " test is passed well, OK\n";
  else
    di <<"\nThe " << argv[0] << " test failed, Faulty\n";

  return 0;
}

static Standard_Integer OCC12584 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << argv[0] << " ERROR : use 'vinit' command before \n";
    return -1;
  }

  if (argc > 2)
  {
    di << "Usage : " << argv[0] << " [mode = 0/1/2]\n";
    return 1;
  }
  Standard_Integer mode = 0;
  if (argc == 2)
  {
    mode = Draw::Atoi(argv[1]);
  }
  if (mode > 2 || mode < 0)
  {
    di << "Usage : " << argv[0] << " [mode = 0/1/2]\n";
    return 1;
  }
  Handle(V3d_View) V = ViewerTest::CurrentView();
  static Handle(AIS_ColorScale) aCS;
  if (aCS.IsNull())
  {
    aCS = new AIS_ColorScale();
  }
  if (aCS->ZLayer() != Graphic3d_ZLayerId_TopOSD)
  {
    aCS->SetZLayer (Graphic3d_ZLayerId_TopOSD);
  }
  if (aCS->TransformPersistence().IsNull()
   || aCS->TransformPersistence()->Mode() != Graphic3d_TMF_2d)
  {
    aContext->SetTransformPersistence (aCS, new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER));
  }
  Standard_Integer aWinWidth, aWinHeight;
  V->Window()->Size (aWinWidth, aWinHeight);
  aCS->SetSize (aWinWidth, aWinHeight);
  if ( !V.IsNull() ) {
    if (mode == 0) {
      aContext->Display (aCS, Standard_True);
    }
    if (mode == 1) {
      aContext->Erase (aCS, Standard_False);
      V->UpdateLights();
      V->Update();
    }
    if (mode == 2) {
      Standard_Boolean IsDisplayed = aContext->IsDisplayed (aCS);
      if (IsDisplayed)
	di <<"ColorScaleIsDisplayed = 1\n";
      else
	di <<"ColorScaleIsDisplayed = 0\n";
    }
  }
  return 0;
}

#include <Interface_Macros.hxx>
#include <IGESControl_Controller.hxx>
#include <XSDRAW.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <XSControl_WorkSession.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Message_ProgressSentry.hxx>
#include <XSControl_TransferReader.hxx>

#include <Geom_Plane.hxx>
static Standard_Integer OCC20766 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 6)
  {
    di << "Usage : " << argv[0] << " plane a b c d\n";
    return 1;
  }

  Standard_Real A = Draw::Atof(argv[2]);
  Standard_Real B = Draw::Atof(argv[3]);
  Standard_Real C = Draw::Atof(argv[4]);
  Standard_Real D = Draw::Atof(argv[5]);

  Handle(Geom_Geometry) result;

  Handle(Geom_Plane) aPlane = new Geom_Plane(A, B, C, D);
  result = aPlane;

  DrawTrSurf::Set(argv[1],result);
  return 0;
}

static Standard_Integer OCC20627 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc!=2)
    {
      di << "Usage : " << argv[0] << " MaxNbr\n";
      return -1;
    }
  Standard_Integer aMaxNbr = Draw::Atoi(argv[1]);

  for (Standard_Integer i=0;i<aMaxNbr;i++)
    {
      BRepBuilderAPI_MakePolygon w(gp_Pnt(0,0,0),gp_Pnt(0,100,0),gp_Pnt(20,100,0),gp_Pnt(20,0,0));
      w.Close();
      TopoDS_Wire wireShape( w.Wire());
      BRepBuilderAPI_MakeFace faceBuilder(wireShape);
      TopoDS_Face f( faceBuilder.Face());
      BRepMesh_IncrementalMesh im(f,1);
      BRepTools::Clean(f);
    }
  return 0;
}

#include <Graphic3d_Vector.hxx>
Standard_Integer OCC22762 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
    if (argc!=7)
	{
	    di << "Wrong number of arguments\n";
	    return -1;
	}
    Standard_Real X1_Pnt = Draw::Atof(argv[1]);
    Standard_Real Y1_Pnt = Draw::Atof(argv[2]);
    Standard_Real Z1_Pnt = Draw::Atof(argv[3]);
    Standard_Real X2_Pnt = Draw::Atof(argv[4]);
    Standard_Real Y2_Pnt = Draw::Atof(argv[5]);
    Standard_Real Z2_Pnt = Draw::Atof(argv[6]);
    
    Graphic3d_Vector AV1(X1_Pnt, Y1_Pnt, Z1_Pnt);
    Graphic3d_Vector AV2(X2_Pnt, Y2_Pnt, Z2_Pnt);
     
    di << "Result is: " << (Graphic3d_Vector::IsParallel(AV1, AV2) ? "true" : "false") << "\n" ;
    return 0;
}
    
    


#include <IntCurvesFace_ShapeIntersector.hxx>
#include <gp_Lin.hxx>
Standard_Integer OCC17424 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc!=9)
    {
      di << "Usage : " << argv[0] << " shape X_Pnt Y_Pnt Z_Pnt X_Dir Y_Dir Z_Dir PInf\n";
      return -1;
    }

  TopoDS_Shape shape = DBRep::Get(argv[1]);

  if (shape.IsNull()) {
    di<<" Null shape is not allowed";
    return 1;
  }

  Standard_Real X_Pnt = Draw::Atof(argv[2]);
  Standard_Real Y_Pnt = Draw::Atof(argv[3]);
  Standard_Real Z_Pnt = Draw::Atof(argv[4]);

  Standard_Real X_Dir = Draw::Atof(argv[5]);
  Standard_Real Y_Dir = Draw::Atof(argv[6]);
  Standard_Real Z_Dir = Draw::Atof(argv[7]);

  Standard_Real PInf  = Draw::Atof(argv[8]);

  IntCurvesFace_ShapeIntersector intersector;
  intersector.Load(shape, Precision::Intersection());

  gp_Pnt origin(X_Pnt, Y_Pnt, Z_Pnt);
  gp_Dir dir(X_Dir, Y_Dir, Z_Dir);
  gp_Lin ray(origin, dir);

  Standard_Real PSup = RealLast();
  intersector.PerformNearest(ray, PInf, PSup);
  if (intersector.NbPnt() != 0)
    {
      di << argv[0] << " status = 0 \n";
      Standard_Real w = intersector.WParameter(1);
      di << "w = " << w << "\n";
    } else {
      di << argv[0] << " status = -1 \n";
    }
  return 0;
}

Standard_Integer OCC22301 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }

  // Create mask 1111: extent == 4
  TColStd_PackedMapOfInteger aFullMask;
  for (Standard_Integer i = 0; i < 4; i++)
    aFullMask.Add(i);
  
  // Create mask 1100: extent == 2
  TColStd_PackedMapOfInteger aPartMask;
  for (Standard_Integer i = 0; i < 2; i++)
    aPartMask.Add(i);
  
  di << "aFullMask = 1111\n";
  di << "aPartMask = 1100\n";
  
  Standard_Boolean isAffected;
  
  isAffected = aFullMask.Intersect(aPartMask); // true; extent == 2 (OK)
  di << "First time: aFullMask.Intersect(aPartMask), isAffected = " << (Standard_Integer)isAffected << "\n";
  isAffected = aFullMask.Intersect(aPartMask); // true; extent == 0 (?)
  di << "Second time: aFullMask.Intersect(aPartMask), isAffected = " << (Standard_Integer)isAffected << "\n";
  isAffected = aFullMask.Subtract(aPartMask); // false (?)
  di << "After two intersections: aFullMask.Subtract(aPartMask), isAffected = " << (Standard_Integer)isAffected << "\n";

  return 0;
}

#include <NCollection_DataMap.hxx>
Standard_Integer OCC22744 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
	
  if (argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  
  TCollection_ExtendedString anExtString;
  
  Standard_ExtCharacter aNonAsciiChar = 0x0f00;
  anExtString.Insert(1, aNonAsciiChar);

  di << "Is ASCII: " << ( anExtString.IsAscii() ? "true : Error" : "false : OK" ) << "\n";
  NCollection_DataMap<TCollection_ExtendedString, Standard_Integer> aMap;
  aMap.Bind(anExtString, 0);
  
  return 0;

}

Standard_Integer OCC22558 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
    if (argc != 10) {
	di << "Wrong number of arguments" << argv[0] << "\n";
	return 1;
    }
    
    Standard_Real X_vec = Draw::Atof(argv[1]);
    Standard_Real Y_vec = Draw::Atof(argv[2]);
    Standard_Real Z_vec = Draw::Atof(argv[3]);
    
    Standard_Real X_dir = Draw::Atof(argv[4]);
    Standard_Real Y_dir = Draw::Atof(argv[5]);
    Standard_Real Z_dir = Draw::Atof(argv[6]);
    
    Standard_Real X_pnt = Draw::Atof(argv[7]);
    Standard_Real Y_pnt = Draw::Atof(argv[8]);
    Standard_Real Z_pnt = Draw::Atof(argv[9]);
    
    gp_Dir toSym(X_vec, Y_vec, Z_vec);
    gp_Dir dir(X_dir, Y_dir, Z_dir);
    gp_Pnt loc(X_pnt, Y_pnt, Z_pnt);
    gp_Ax2 symObj(loc,dir);
    toSym.Mirror(symObj);
    
    di << "The result " << toSym.X() << " " << toSym.Y() << " " << toSym.Z() << "\n"; 
    return 0;
}
    

Standard_Integer OCC22736 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
	
  if (argc != 9) {
    di << "Usage : " << argv[0] << " X_mirrorFirstPoint Y_mirrorFirstPoint X_mirrorSecondPoint Y_mirrorSecondPoint X_p1 Y_p1 X_p2 Y_p2\n";
    return 1;
  }

  Standard_Real X_mirrorFirstPoint = Draw::Atof(argv[1]);
  Standard_Real Y_mirrorFirstPoint = Draw::Atof(argv[2]);
  Standard_Real X_mirrorSecondPoint = Draw::Atof(argv[3]);
  Standard_Real Y_mirrorSecondPoint = Draw::Atof(argv[4]);
  Standard_Real X_p1 = Draw::Atof(argv[5]);
  Standard_Real Y_p1 = Draw::Atof(argv[6]);
  Standard_Real X_p2 = Draw::Atof(argv[7]);
  Standard_Real Y_p2 = Draw::Atof(argv[8]);
  
  gp_Trsf2d identityTransformation;

  gp_Pnt2d mirrorFirstPoint(X_mirrorFirstPoint,Y_mirrorFirstPoint);
  gp_Pnt2d mirrorSecondPoint(X_mirrorSecondPoint,Y_mirrorSecondPoint);
  gp_Ax2d  mirrorAxis(mirrorFirstPoint,gp_Vec2d(mirrorFirstPoint,mirrorSecondPoint));

  gp_Pnt2d p1(X_p1,Y_p1);
  gp_Pnt2d p2(X_p2,Y_p2);

  gp_Trsf2d M1;
  M1.SetMirror(mirrorAxis);
  gp_Trsf2d M2;
  M2.SetMirror(mirrorAxis);
  gp_Trsf2d Tcomp;
  Tcomp = M2.Multiplied(M1);

  Standard_Real aTol = Precision::Confusion();
  Standard_Integer aStatus = 0;

  //After applying two times the same mirror the point is located on the same location OK
  gp_Pnt2d p1MirrorM1   = p1.Transformed(M1);
  if ( Abs(p2.X() - p1MirrorM1.X()) > aTol )
    aStatus = 2;
  if ( Abs(p2.Y() - p1MirrorM1.Y()) > aTol )
    aStatus = 3;

  gp_Pnt2d p1MirrorM1M2 = p1MirrorM1.Transformed(M2);
  if ( Abs(p1.X() - p1MirrorM1M2.X()) > aTol )
    aStatus = 4;
  if ( Abs(p1.Y() - p1MirrorM1M2.Y()) > aTol )
    aStatus = 5;

  //If we apply the composed transformation of the same two mirrors to a point the result is //not located on the initial position.-->>ERROR
  gp_Pnt2d p1MirrorComp = p1.Transformed(Tcomp);
  if ( Abs(p1.X() - p1MirrorComp.X()) > aTol )
    aStatus = 6;
  if ( Abs(p1.Y() - p1MirrorComp.Y()) > aTol )
    aStatus = 7;

  di << "Status = " << aStatus << "\n";
  return 0;
}

Standard_Integer OCC23429(Draw_Interpretor& /*di*/,
                          Standard_Integer narg, const char** a)
{
  if (narg < 4) return 1;
  
  TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull()) return 1;
  
  BRepFeat_SplitShape Spls(aShape);
  Spls.SetCheckInterior(Standard_False);

  TopoDS_Shape aTool = DBRep::Get(a[3]);

  BRepAlgoAPI_Section Builder(aShape, aTool, Standard_False);
  Builder.ComputePCurveOn1(Standard_True);
  if (narg == 5)
    Builder.Approximation(Standard_True); 
  Builder.Build();
  TopoDS_Shape aSection = Builder.Shape();

  TopExp_Explorer ExpSec(aSection, TopAbs_EDGE);
  for (; ExpSec.More(); ExpSec.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge(ExpSec.Current());
    Handle(Geom2d_Curve) thePCurve;
    Handle(Geom_Surface) theSurface;
    TopLoc_Location theLoc;
    Standard_Real fpar, lpar;
    BRep_Tool::CurveOnSurface(anEdge, thePCurve, theSurface, theLoc, fpar, lpar);
    TopoDS_Face aFace;
    TopExp_Explorer ExpShape(aShape, TopAbs_FACE);
    for (; ExpShape.More(); ExpShape.Next())
    {
      aFace = TopoDS::Face(ExpShape.Current());
      TopLoc_Location aLoc;
      Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace, aLoc);
      if (aSurface == theSurface && aLoc == theLoc)
        break;
    }
    Spls.Add(anEdge, aFace);
  }

  TopoDS_Shape Result = Spls.Shape();
  DBRep::Set(a[1], Result);

  return 0;
}

#include <ExprIntrp_GenExp.hxx>
Standard_Integer CR23403 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
	
  if (argc != 2) {
    di << "Usage : " << argv[0] << " string\n";
    return 1;
  }

  Standard_CString aString = argv[1];
  Handle(ExprIntrp_GenExp) myExpr = ExprIntrp_GenExp::Create();
  try {
    OCC_CATCH_SIGNALS
    myExpr->Process( aString );
  }
  catch(Standard_Failure const& anException) {
    di << "Exception : " << anException.GetMessageString() << "\n";
  }

  return 0;
}

Standard_Integer OCC28478 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{	
  Standard_Integer nbOuter = (argc > 1 ? Draw::Atoi(argv[1]) : 3);
  Standard_Integer nbInner = (argc > 2 ? Draw::Atoi(argv[2]) : 2);

  // test behavior of progress indicator when using nested scopes with names set by Sentry objects
  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator (di, 1);
  aProgress->SetTextMode (Standard_True);

  // Outer cycle
  Message_ProgressSentry anOuter (aProgress, "Outer", 0, nbOuter, 1);
  for (int i = 0; i < nbOuter && anOuter.More(); i++, anOuter.Next())
  {
    // Inner cycle
    Message_ProgressSentry anInner (aProgress, "Inner", 0, nbInner, 1);
    for (int j = 0; j < nbInner && anInner.More(); j++, anInner.Next())
    {
      // Cycle body
    }
  }

  return 0;
}

void QABugs::Commands_11(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add("OCC128", "OCC128", __FILE__, OCC128, group);

  // Remove as bad version of QAAddOrRemoveSelected from QADraw
  //theCommands.Add("OCC129", "OCC129 shape islocal", __FILE__, OCC129, group);

  theCommands.Add("OCC136", "OCC136", __FILE__, OCC136, group);
  theCommands.Add("BUC60610","BUC60610 iges_input [name]",__FILE__,BUC60610,group);

//====================================================
//
// Following commands are inserted from 
// /dn03/KAS/dev/QAopt/src/QADraw/QADraw_TOPOLOGY.cxx
// ( 75455 Apr 16 18:59)
//
//====================================================

  theCommands.Add("OCC105","OCC105 shape",__FILE__,OCC105,group); 
  theCommands.Add("OCC9"," result path cur1 cur2 radius [tolerance]:\t test GeomFill_Pipe", __FILE__, pipe_OCC9,group);

  theCommands.Add("OCC125","OCC125 shell", __FILE__, OCC125,group);

  theCommands.Add("OCC157","findplanarsurface Result wire Tol",__FILE__,OCC157,group);
  //theCommands.Add("OCC165","OCC165",__FILE__,OCC165,group);
  theCommands.Add("OCC165","OCC165 file",__FILE__,OCC165,group);
  theCommands.Add("OCC297","OCC297",__FILE__,OCC297,group);
  //theCommands.Add("OCC305","OCC305",__FILE__,OCC305,group);
  theCommands.Add("OCC305","OCC305 file",__FILE__,OCC305,group);

  // New commands:
  theCommands.Add("OCC166", "OCC166", __FILE__, OCC166, group);
  theCommands.Add("OCC381_Save", "OCC381_Save Doc", __FILE__, OCC381_Save, group);
  theCommands.Add("OCC381_SaveAs", "OCC381_SaveAs Doc Path", __FILE__, OCC381_SaveAs, group);

  theCommands.Add("OCC299","OCC299 Solid Point [Tolerance=1.e-7]", __FILE__, OCC299bug, group);
  theCommands.Add("OCC309","OCC309", __FILE__, OCC309bug, group);
  theCommands.Add("OCC310","OCC310", __FILE__, OCC310bug, group);

  //theCommands.Add("OCC277","OCC277", __FILE__, OCC277bug, group);
  theCommands.Add("OCC277","OCC277", __FILE__, OCC277bug, group);

  theCommands.Add("OCC333","OCC333 edge1 edge2 [toler domaindist]; Check overlapping edges", __FILE__, OCC333bug, group);

  theCommands.Add("OCC363", "OCC363 document filename ", __FILE__, OCC363, group);
  // Must use OCC299
  //theCommands.Add("OCC372", "OCC372", __FILE__, OCC372, group);
  theCommands.Add("OCC377", "OCC377", __FILE__, OCC377, group);
  theCommands.Add("OCC22", "OCC22 Result Shape CompoundOfSubshapesToBeDivided ConsiderLocation", __FILE__, OCC22, group);
  theCommands.Add("OCC24", "OCC24 Result Shape CompoundOfSubshapes ResourceFileName", __FILE__, OCC24, group);
  theCommands.Add("OCC369", "OCC369 Shape", __FILE__, OCC369, group);
  theCommands.Add("OCC524", "OCC524 LowerVector UpperVector InitialValueVector LowerRowMatrix UpperRowMatrix LowerColMatrix UpperColMatrix InitialValueMatrix", __FILE__, OCC524, group);
  theCommands.Add("OCC525", "OCC525", __FILE__, OCC525, group);
  //theCommands.Add("OCC578", "OCC578 shape1 shape2 shape3", __FILE__, OCC578, group);
  theCommands.Add("OCC578", "OCC578 shape1 shape2 shape3", __FILE__, OCC578, group);
  theCommands.Add("OCC669", "OCC669 GUID", __FILE__, OCC669, group);
  theCommands.Add("OCC738_ShapeRef", "OCC738_ShapeRef", __FILE__, OCC738_ShapeRef, group);
  theCommands.Add("OCC738_Assembly", "OCC738_Assembly", __FILE__, OCC738_Assembly, group);
  theCommands.Add("OCC708", "OCC708 shape ; Deactivate the current transformation", __FILE__, OCC708, group);
  theCommands.Add("OCC670", "OCC670", __FILE__, OCC670, group);
  theCommands.Add("OCC867", "OCC867 Point Surface Umin Usup Vmin Vsup", __FILE__, OCC867, group);
  theCommands.Add("OCC909", "OCC909 wire face", __FILE__, OCC909, group);
  theCommands.Add("OCC921", "OCC921 face", __FILE__, OCC921, group);
  theCommands.Add("OCC902", "OCC902 expression", __FILE__, OCC902, group);

  theCommands.Add ("OCC1029_AISTransparency","OCC1029_AISTransparency (DOC, entry, [real])",__FILE__, OCC1029_AISTransparency, group);
  theCommands.Add ("OCC1030_AISColor", "OCC1030_AISColor (DOC, entry, [color])", __FILE__, OCC1030_AISColor, group);
  theCommands.Add ("OCC1031_AISMaterial", "OCC1031_AISMaterial (DOC, entry, [material])", __FILE__, OCC1031_AISMaterial, group); 
  theCommands.Add ("OCC1032_AISWidth", "OCC1032_AISWidth (DOC, entry, [width])", __FILE__, OCC1032_AISWidth, group); 
  theCommands.Add ("OCC1033_AISMode", "OCC1033_AISMode (DOC, entry, [mode])", __FILE__, OCC1033_AISMode, group); 
  theCommands.Add ("OCC1034_AISSelectionMode", "OCC1034_AISSelectionMode (DOC, entry, [selectionmode])", __FILE__, OCC1034_AISSelectionMode, group); 

  //theCommands.Add("OCC1487", "OCC1487 CylinderVariant(=1/2) cylinder1 cylinder2 cutshape", __FILE__, OCC1487, group);
  theCommands.Add("OCC1487", "OCC1487 CylinderVariant(=1/2) cylinder1 cylinder2 cutshape", __FILE__, OCC1487, group);

  theCommands.Add("OCC1077", "OCC1077 result", __FILE__, OCC1077, group);
  theCommands.Add("OCC5739", "OCC5739 name shape step", __FILE__, OCC5739_UniAbs, group);
  theCommands.Add("OCC6046", "OCC6046 nb_of_vectors size", __FILE__, OCC6046, group);
  theCommands.Add("OCC5698", "OCC5698 wire", __FILE__, OCC5698, group);
  theCommands.Add("OCC6143", "OCC6143", __FILE__, OCC6143, group);
  theCommands.Add("OCC7141", "OCC7141 [nCount] aPath", __FILE__, OCC7141, group);
  theCommands.Add("OCC7372", "OCC7372", __FILE__, OCC7372, group);
  theCommands.Add("OCC8169", "OCC8169 edge1 edge2 plane", __FILE__, OCC8169, group);
  theCommands.Add("OCC10138", "OCC10138 lower upper", __FILE__, OCC10138, group);
  theCommands.Add("OCC7639", "OCC7639 index1 value1 ... [indexN valueN]", __FILE__, OCC7639, group);
  theCommands.Add("OCC8797", "OCC8797", __FILE__, OCC8797, group);
  theCommands.Add("OCC7068", "OCC7068", __FILE__, OCC7068, group);
  theCommands.Add("OCC11457", "OCC11457 polygon lastedge x1 y1 z1 x2 y2 z2 ...", __FILE__, OCC11457, group);
  theCommands.Add("OCC13963", "OCC13963 ratio origin_x origin_y origin_z", __FILE__, OCC13963, group);
  theCommands.Add("OCC14376", "OCC14376 shape [deflection]", __FILE__, OCC14376, group);
  theCommands.Add("OCC15489", "OCC15489 A B C", __FILE__, OCC15489, group);
  theCommands.Add("OCC15755", "OCC15755 file shape", __FILE__, OCC15755, group);
  theCommands.Add("OCC16782", "OCC16782 file.std file.xml file.cbf", __FILE__, OCC16782, group);
  theCommands.Add("OCC12584", "OCC12584 [mode = 0/1/2]", __FILE__, OCC12584, group);
  theCommands.Add("OCC20766", "OCC20766 plane a b c d", __FILE__, OCC20766, group);
  theCommands.Add("OCC20627", "OCC20627", __FILE__, OCC20627, group);
  theCommands.Add("OCC17424", "OCC17424  shape X_Pnt Y_Pnt Z_Pnt X_Dir Y_Dir Z_Dir PInf", __FILE__, OCC17424, group);
  theCommands.Add("OCC22301", "OCC22301", __FILE__, OCC22301, group);
  theCommands.Add("OCC22736", "OCC22736 X_mirrorFirstPoint Y_mirrorFirstPoint X_mirrorSecondPoint Y_mirrorSecondPoint X_p1 Y_p1 X_p2 Y_p2", __FILE__, OCC22736, group);
  theCommands.Add("OCC22744", "OCC22744", __FILE__, OCC22744, group);
  theCommands.Add("OCC22762", "OCC22762 x1 y1 z1 x2 y2 z3", __FILE__, OCC22762, group);
  theCommands.Add("OCC22558", "OCC22558 x_vec y_vec z_vec x_dir y_dir z_dit x_pnt y_pnt z_pnt", __FILE__, OCC22558, group);
  theCommands.Add("CR23403", "CR23403 string", __FILE__, CR23403, group);
  theCommands.Add("OCC23429", "OCC23429 res shape tool [appr]", __FILE__, OCC23429, group);
  theCommands.Add("OCC28478", "OCC28478 [nb_outer=3 [nb_inner=2]: test progress indicator on nested cycles", __FILE__, OCC28478, group);
  return;
}
