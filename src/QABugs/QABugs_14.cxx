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

#include <stdio.h>

#include <QABugs.hxx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <Geom2d_Line.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2d_Circle.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <gp_Lin.hxx>
#include <BRepFeat_SplitShape.hxx>
#include <DBRep_DrawableShape.hxx>
#include <BRep_Builder.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <Draw.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Precision.hxx>
#include <Geom_BSplineCurve.hxx>
#include <OSD_Path.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_WireData.hxx>
#include <TopoDS_Wire.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BRep_Tool.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <V3d_View.hxx>
#include <Standard_ErrorHandler.hxx>

static Standard_Integer BUC60897 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  Standard_Character abuf[16];

  Handle(Geom2d_Line) aLine = new Geom2d_Line(gp_Pnt2d(100, 0), gp_Dir2d(-1, 0));
  Sprintf(abuf,"line");
  Standard_CString st = abuf;
  DrawTrSurf::Set (st, aLine);

  TColgp_Array1OfPnt2d aPoints(1, 3);
  aPoints.SetValue(1, gp_Pnt2d(0, 0));
  aPoints.SetValue(2, gp_Pnt2d(50, 50));
  aPoints.SetValue(3, gp_Pnt2d(0, 100));
  Handle(Geom2d_BezierCurve) aCurve = new Geom2d_BezierCurve(aPoints);
  Sprintf(abuf,"curve");
  DrawTrSurf::Set (st, aCurve);

  Geom2dAdaptor_Curve aCLine(aLine);
  Geom2dAdaptor_Curve aCCurve(aCurve);
  Geom2dGcc_QualifiedCurve aQualifCurve1(aCLine, GccEnt_outside);
  Geom2dGcc_QualifiedCurve aQualifCurve2(aCCurve, GccEnt_outside);
  Geom2dGcc_Circ2d2TanRad aGccCirc2d(aQualifCurve1, aQualifCurve2, 10, 1e-7);
  if(!aGccCirc2d.IsDone())
  {
    di << "Faulty: can not create a circle.\n";
    return 1;
  }
  for(Standard_Integer i = 1; i <= aGccCirc2d.NbSolutions(); i++)
  {
    gp_Circ2d aCirc2d = aGccCirc2d.ThisSolution(i);
    di << "circle : X " << aCirc2d.Location().X() << " Y " << aCirc2d.Location().Y() << " R " << aCirc2d.Radius();
    Standard_Real aTmpR1, aTmpR2;
    gp_Pnt2d aPnt2d1, aPnt2d2;
    aGccCirc2d.Tangency1(i, aTmpR1, aTmpR2, aPnt2d1);
    aGccCirc2d.Tangency2(i, aTmpR1, aTmpR2, aPnt2d2);
    di << "\ntangency1 : X " << aPnt2d1.X() << " Y " << aPnt2d1.Y();
    di << "\ntangency2 : X " << aPnt2d2.X() << " Y " << aPnt2d2.Y() << "\n";
    
    Sprintf(abuf,"circle_%d",i);
    Handle(Geom2d_Curve) circ_res = new Geom2d_Circle(aCirc2d);
    DrawTrSurf::Set (st, circ_res);
  }

  di << "done\n";
  return 0;
}

static Standard_Integer BUC60889 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 10) {
    di << "Usage : " << argv[0] << " point_1 point_2 name_of_edge bndbox_X1 bndbox_Y1 bndbox_Z1 bndbox_X2 bndbox_Y2 bndbox_Z2\n";
    return 1;
  } else {
    gp_Pnt p1, p2;
    if (!(DrawTrSurf::GetPoint(argv[1], p1)) || !(DrawTrSurf::GetPoint(argv[2], p2)))
    {
      di << "Need two points to define a band\n";
      return 1;
    }
    TopoDS_Edge ed = TopoDS::Edge(DBRep::Get(argv[3]));
    if (ed.IsNull())
    {
      di << "Need an edge to define the band direction\n";
      return 1;
    }
    BRepAdaptor_Curve curve(ed);
    gp_Dir d = curve.Line().Direction();
    Bnd_Box bnd_box;
    bnd_box.Update(Draw::Atof(argv[4]), Draw::Atof(argv[5]), Draw::Atof(argv[6]), Draw::Atof(argv[7]), Draw::Atof(argv[8]), Draw::Atof(argv[9]));
    if(bnd_box.IsOut(p1, p2, d))
      di << "The band lies out of the box\n";
    else
      di << "The band intersects the box\n";
    
    return 0;
  }
}

static Standard_Integer BUC60852 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 8)
    di << "Usage : " << argv[0] << " name_of_edge bndbox_X1 bndbox_Y1 bndbox_Z1 bndbox_X2 bndbox_Y2 bndbox_Z2\n";
  else {
    TopoDS_Edge shape = TopoDS::Edge(DBRep::Get(argv[1]));
    if(shape.ShapeType() != TopAbs_EDGE)
      di << "shape must be an edge\n";
    else {
      BRepAdaptor_Curve curve(shape);
      gp_Lin lin = curve.Line();
      Bnd_Box bnd_box;
      bnd_box.Update(Draw::Atof(argv[2]), Draw::Atof(argv[3]), Draw::Atof(argv[4]), Draw::Atof(argv[5]), Draw::Atof(argv[6]), Draw::Atof(argv[7]));
      if(bnd_box.IsOut(lin))
	di << "Line that lies on edge does not intersect the box\n";
      else
	di << "Line that lies on edge intersects the box\n";
    }
  }
  return 0;
}

static Standard_Integer BUC60854 (Draw_Interpretor& /*di*/, Standard_Integer argc, const char ** argv)
{
  Standard_Integer newnarg;
  if (argc < 3) return 1;
  TopoDS_Shape S = DBRep::Get(argv[2]);
  BRepFeat_SplitShape Spls(S);
  Standard_Boolean pick = Standard_False;
  TopoDS_Shape EF;
  Standard_Real u,v;
  Standard_Integer i = 3;
  for ( newnarg=3; newnarg<argc; newnarg++) {
    if (argv[newnarg][0] == '@') {
      break;
    }
  }
  if (newnarg == 3 || 
      (newnarg !=argc && ((argc-newnarg)<=2 || (argc-newnarg)%2 != 1))) {
    return 1;
  }
  if (i<newnarg) {
    pick = (argv[i][0] == '.');
    EF = DBRep::Get(argv[i],TopAbs_FACE);
    if (EF.IsNull()) return 1;
  }
  while (i < newnarg) {
    if (pick) {
      DBRep_DrawableShape::LastPick(EF,u,v);
    }
    if (EF.ShapeType() == TopAbs_FACE) {
      i++;
      while (i < newnarg) {
	TopoDS_Shape W;
	Standard_Boolean rever = Standard_False;
	if (argv[i][0] == '-') {
	  if (argv[i][1] == '\0')
	    return 1;
	  pick = (argv[i][1] == '.');
	  const char* Temp = argv[i]+1;
	  W = DBRep::Get(Temp,TopAbs_SHAPE,Standard_False);
	  rever = Standard_True;
	}
	else {
	  pick = (argv[i][0] == '.');
	  W = DBRep::Get(argv[i],TopAbs_SHAPE,Standard_False);
	}
	if (W.IsNull()) {
	  return 1;
	}
	TopAbs_ShapeEnum wtyp = W.ShapeType();
	if (wtyp != TopAbs_WIRE && wtyp != TopAbs_EDGE && pick) {
	  Standard_Real aTempU, aTempV;
	  DBRep_DrawableShape::LastPick(W, aTempU, aTempV);
	  wtyp = W.ShapeType();
	}
	if (wtyp != TopAbs_WIRE && wtyp != TopAbs_EDGE) {
	  EF = DBRep::Get(argv[i]);
	  break;
	}
	else {
	  if (rever) {
	    W.Reverse();
	  }
	  if (wtyp == TopAbs_WIRE) {
	    Spls.Add(TopoDS::Wire(W),TopoDS::Face(EF));
	  }
	  else {
	    Spls.Add(TopoDS::Edge(W),TopoDS::Face(EF));
	  }
	}
	i++;
      }
    }
    else
      return 1;
  }
  for (i++; i<argc; i+=2) {
    TopoDS_Shape Ew,Es;
    TopoDS_Shape aLocalShape(DBRep::Get(argv[i],TopAbs_EDGE));
    Es = TopoDS::Edge(aLocalShape);
    if (Es.IsNull()) {
      return 1;
    }
    aLocalShape = DBRep::Get(argv[i+1],TopAbs_EDGE);
    Ew = TopoDS::Edge(aLocalShape);
    if (Ew.IsNull()) {
      return 1;
    }
    Spls.Add(TopoDS::Edge(Ew),TopoDS::Edge(Es));
  }
  Spls.Build();
  const TopTools_ListOfShape& aLeftPart = Spls.Left();
  BRep_Builder BB;
  TopoDS_Shape aShell;
  BB.MakeShell(TopoDS::Shell(aShell));
  TopTools_ListIteratorOfListOfShape anIter(aLeftPart);
  for(; anIter.More(); anIter.Next()) BB.Add(aShell, anIter.Value());
  aShell.Closed (BRep_Tool::IsClosed (aShell));
  DBRep::Set(argv[1],aShell);
  return 0;
}

static Standard_Integer BUC60870 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer i1;
  if (argc != 5) {
    di << "Usage : " << argv[0] << " result name_of_shape_1 name_of_shape_2 dev\n";
    return 1;
  }
  const char *ns1 = (argv[2]), *ns2 = (argv[3]), *ns0 = (argv[1]);
  TopoDS_Shape S1(DBRep::Get(ns1)), S2(DBRep::Get(ns2))  ;
  Standard_Real dev =  Draw::Atof(argv[4]);
  BRepExtrema_DistShapeShape dst(S1 ,S2, dev );
  if (dst.IsDone()) {
    char named[100];
    Sprintf(named, "%s%s" ,ns0,"_val");
    char* tempd = named;
    Draw::Set(tempd,dst.Value());
    di << named << " ";
    for (i1=1; i1<= dst.NbSolution(); i1++) {
      gp_Pnt P1,P2;
      P1 = (dst.PointOnShape1(i1));
      P2 = (dst.PointOnShape2(i1));
      if (dst.Value()<=1.e-9) {
	TopoDS_Vertex V =BRepLib_MakeVertex(P1);
	char namev[100];
	if (i1==1) {
	  Sprintf(namev, "%s" ,ns0);
	} else {
	  Sprintf(namev, "%s%d" ,ns0,i1);
	}
	char* tempv = namev;
	DBRep::Set(tempv,V);
	di << namev << " ";
      } else {
	char name[100];
	TopoDS_Edge E = BRepLib_MakeEdge (P1, P2);
	if (i1==1) {
	  Sprintf(name,"%s",ns0);
	} else {
	  Sprintf(name,"%s%d",ns0,i1);
	}
	char* temp = name;
	DBRep::Set(temp,E);
	di << name << " " ;
      }
    }
  } else {
    di << "Faulty : found a problem\n";
  }
  return 0;
}

static Standard_Integer BUC60902 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  Handle(TColgp_HArray1OfPnt) aPnts = new TColgp_HArray1OfPnt(1,5);
  gp_Pnt aP(0., 0., 0.);
  for(Standard_Integer i = 1; i <= 5; i++) {
    aP.SetX((i-1)*1.57);
    aP.SetY(Sin((i-1)*1.57));
    aPnts->SetValue(i, aP);
  }
  GeomAPI_Interpolate anInterpolater(aPnts, Standard_False, Precision::Confusion());
  anInterpolater.Perform(); 
  if(!anInterpolater.IsDone()) {
    di << "Faulty : error in interpolation\n";
    return 1;
  }
  Handle(Geom_BSplineCurve) aCur = anInterpolater.Curve(); 
  gp_Vec aFirstTang, aLastTang;
  aCur->D1(aCur->FirstParameter(), aP, aFirstTang);
  aCur->D1(aCur->LastParameter(), aP, aLastTang);
  di << " Used Tang1 = " << aFirstTang.X() << " " << aFirstTang.Y() << " " << aFirstTang.Z() << "\n"; 
  di << " Used Tang2 = " << aLastTang.X() << " " << aLastTang.Y() << " " << aLastTang.Z() << "\n"; 
  GeomAPI_Interpolate anInterpolater1(aPnts, Standard_False, Precision::Confusion());
  anInterpolater1.Load(aFirstTang, aLastTang, Standard_False); 
  anInterpolater1.Perform(); 
  if(!anInterpolater1.IsDone()) {
    di << "Faulty : error in interpolation 1\n";
    return 1;
  }
  aCur = anInterpolater1.Curve();
  gp_Vec aFirstTang1, aLastTang1;
  aCur->D1(aCur->FirstParameter(), aP, aFirstTang1);
  aCur->D1(aCur->LastParameter(), aP, aLastTang1);
  di << " Tang1 after compute = " << aFirstTang1.X() << " " << aFirstTang1.Y() << " " << aFirstTang1.Z() << "\n"; 
  di << " Tang2 after compute = " << aLastTang1.X() << " " << aLastTang1.Y() << " " << aLastTang1.Z() << "\n"; 
  if(aFirstTang.IsEqual(aFirstTang1, Precision::Confusion(), Precision::Angular())) {
    di << "First tangent is OK\n";
  }
  else {
    di << "Faulty : first tangent is wrong\n";
  }
  if(aLastTang.IsEqual(aLastTang1, Precision::Confusion(), Precision::Angular())) {
    di << "Last tangent is OK\n";
  }
  else {
    di << "Faulty : last tangent is wrong\n";
  }
  return 0;
}

static Standard_Integer BUC60944 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc != 2) {
    di << "Usage : " << argv[0] << " path\n";
  }

  TCollection_AsciiString in(argv[1]);
  OSD_Path* aPath = new OSD_Path(in);
  TCollection_AsciiString out;
  aPath->SystemName(out);
  if(in == out) 
    di << "The convertion is right.\n";
  else
    di << "Faulty : The convertion is incorrect : " << out.ToCString() << "\n";
  di << out.ToCString() << "\n";
//  cout << aPath->Trek() << " !" << endl;
  return 0;
}

Standard_Boolean BuildWiresWithReshape
                (const Handle(ShapeBuild_ReShape)& theReshape,
                 const TopTools_ListOfShape &theListOfEdges,
                 TopTools_ListOfShape       &theListOfWires,
                 const Standard_Boolean      isFixConnectedMode,
                 const Standard_Boolean      isKeepLoopsMode,
                 const Standard_Real         theTolerance)
{
  TopTools_ListIteratorOfListOfShape anEdgeIter;
  Standard_Boolean                   isDone;
  TopoDS_Wire                        aWire;

  theListOfWires.Clear();
  Handle(ShapeExtend_WireData) aWireData  = new ShapeExtend_WireData;
  Handle(ShapeFix_Wire) aShFixWire = new ShapeFix_Wire;
  aShFixWire->SetContext (theReshape);

  Handle(ShapeAnalysis_Wire) aWireAnalyzer;
  ShapeAnalysis_WireOrder aWireOrder;

  aShFixWire->Load(aWireData);
  aShFixWire->SetPrecision(theTolerance);

  for(anEdgeIter.Initialize(theListOfEdges); anEdgeIter.More(); anEdgeIter.Next())
    aWireData->Add(TopoDS::Edge(anEdgeIter.Value()));

  aWireOrder.KeepLoopsMode() = isKeepLoopsMode;
  aWireAnalyzer = aShFixWire->Analyzer();
  aWireAnalyzer->CheckOrder(aWireOrder, Standard_True);

  aShFixWire->FixReorder(aWireOrder);
  isDone = !aShFixWire->StatusReorder(ShapeExtend_FAIL);
  if (!isDone)
    return Standard_False;

  if (isFixConnectedMode)
  {
    aShFixWire->ModifyTopologyMode() = Standard_True;
    aShFixWire->FixConnected(theTolerance);
  }

  aWire = aWireData->Wire();

//   if (aWire.Closed())
//   {
//     theListOfWires.Append(aWire);
//     return Standard_True;
//   }

  Standard_Integer i;
  BRep_Builder     aBuilder;
  TopoDS_Wire      aCurWire;
  TopoDS_Vertex    aVf;
  TopoDS_Vertex    aVl;
  TopoDS_Vertex    aVlast;
  Standard_Integer aNbEdges = aWireData->NbEdges();

  aBuilder.MakeWire(aCurWire);
  if (aNbEdges >= 1)
  {
    TopoDS_Edge anE = aWireData->Edge(1);
    TopExp::Vertices(anE, aVf, aVlast, Standard_True);
    aBuilder.Add(aCurWire, anE);
  }

  for(i = 2; i <= aNbEdges; i++)
  {
    TopoDS_Edge anE = aWireData->Edge(i);
    TopExp::Vertices(anE, aVf, aVl, Standard_True);
    if (aVf.IsSame(aVlast))
    {
      aBuilder.Add(aCurWire, anE);
      aVlast = aVl;
    }
    else
    {
      aVlast = aVl;
      TopExp::Vertices(aCurWire, aVf, aVl);
      if (aVf.IsSame(aVl))
        aCurWire.Closed(Standard_True);
      theListOfWires.Append(aCurWire);
      aBuilder.MakeWire(aCurWire);
      aBuilder.Add(aCurWire, anE);
    }
  }

  TopExp::Vertices(aCurWire, aVf, aVl);
  if (aVf.IsSame(aVl))
    aCurWire.Closed(Standard_True);
  theListOfWires.Append(aCurWire);

  return Standard_True;
}

Standard_Boolean BuildWires(const TopTools_ListOfShape &theListOfEdges,
                                     TopTools_ListOfShape       &theListOfWires,
                                     const Standard_Boolean      isFixConnectedMode = Standard_False,
                                     const Standard_Boolean      isKeepLoopsMode = Standard_True,
                                     const Standard_Real         theTolerance = Precision::Confusion())
{
  Handle(ShapeBuild_ReShape) aReshape = new ShapeBuild_ReShape;
  return BuildWiresWithReshape (aReshape, theListOfEdges, theListOfWires,
                                isFixConnectedMode, isKeepLoopsMode, theTolerance);
}

Standard_Boolean BuildBoundWires(const TopoDS_Shape   &theShell,
                                          TopTools_ListOfShape &theListOfWires)
{
  TopTools_IndexedDataMapOfShapeListOfShape
                               anEdgeFaceMap;
  Standard_Integer             i;
  Standard_Boolean             isBound;
  TopTools_ListOfShape         aBoundaryEdges;

  TopExp::MapShapesAndAncestors(theShell, TopAbs_EDGE, TopAbs_FACE, anEdgeFaceMap);

  isBound = Standard_False;
  for (i = 1; i <= anEdgeFaceMap.Extent(); i++)
  {
    const TopTools_ListOfShape &anAncestFaces = anEdgeFaceMap.FindFromIndex(i);
    if (anAncestFaces.Extent() == 1)
    {
      const TopoDS_Edge &anEdge = TopoDS::Edge(anEdgeFaceMap.FindKey(i));
      if (!BRep_Tool::Degenerated(anEdge))
      {
        aBoundaryEdges.Append(anEdge);
        isBound = Standard_True;
      }
    }
  }

  if (!isBound)
    return Standard_True;

  return BuildWires(aBoundaryEdges, theListOfWires);
}

static Standard_Integer BUC60868 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 3) {
    di << "Usage : " <<argv[0] << " Result Shell\n";
    return 1;
  }

  TopoDS_Shape aShell = DBRep::Get(argv[2]);

  if (aShell.IsNull()) {
    di << "Faulty : The shape is NULL\n";
    return 1;
  }

  TopTools_ListOfShape               aListOfWires;
  BuildBoundWires(aShell, aListOfWires);

  TopoDS_Shape aRes;
  if (aListOfWires.IsEmpty())
    di << "no bound\n";
  else if (aListOfWires.Extent() == 1)
    aRes = aListOfWires.First();
  else {
    BRep_Builder aBld;
    aBld.MakeCompound (TopoDS::Compound(aRes));
    TopTools_ListIteratorOfListOfShape aWireIter (aListOfWires);
    for(; aWireIter.More(); aWireIter.Next())
      aBld.Add (aRes, aWireIter.Value());
  }

  DBRep::Set(argv[1], aRes);
  return 0;
}

static Standard_Integer BUC60924 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 5) {
    di << "Usage : " <<argv[0] << " curve X Y Z\n";
    return 1;
  }

  Handle(Geom_Curve) aCurve = DrawTrSurf::GetCurve(argv[1]);

  if (aCurve.IsNull()) {
    di << "Faulty : the curve is NULL.\n";
    return 1;
  }
  
  gp_XYZ aVec(Draw::Atof(argv[2]),Draw::Atof(argv[3]),Draw::Atof(argv[4]));
  
  Standard_Boolean isPlanar=Standard_False;
  isPlanar=ShapeAnalysis_Curve::IsPlanar(aCurve,aVec,1e-7);
  
  if(isPlanar)
    di << "The curve is planar !\n";
  else 
    di << "Faulty : the curve is not planar!\n";
  
  return 0;
}

static Standard_Integer  BUC60920(Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv)
{

  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    cerr << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

  di.Eval("box b 10 10 10");
  di.Eval("box w 20 20 20");
  di.Eval("explode w e");

  di.Eval(" vdisplay b");
  di.Eval("vsetdispmode 1");

  const char * Shname="w_11";
  TopoDS_Shape theShape =  DBRep::Get(Shname);

  Handle(AIS_Shape) anAISShape = new AIS_Shape( theShape ); 
  myAISContext->Display( anAISShape, Standard_True );
  
  Handle(V3d_View) myV3dView = ViewerTest::CurrentView();
  
  double Xv,Yv;
  myV3dView->Project(20,20,0,Xv,Yv);
//  cout<<Xv<<"\t"<<Yv<<endl;
  
  Standard_Integer Xp,Yp;
  myV3dView->Convert(Xv,Yv,Xp,Yp);
//  cout<<Xp<<"\t"<<Yp<<endl;

  myAISContext->MoveTo (Xp,Yp, myV3dView, Standard_True);

//   if (myAISContext->IsHilighted(anAISShape)) 
//              cout << "has hilighted shape : OK"   << endl;
//   else       cout << "has hilighted shape : bugged - Faulty "   << endl;
  
  return 0;
}

#include <LDOMParser.hxx>
static Standard_Integer  OCC983 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2) {
    di << "Usage : " << argv[0] << " file\n";
    return 1;
  }

  LDOMParser aParser;
  LDOM_Document myDOM;

  const char *File = (argv[1]);

  if(!aParser.parse(File)) {
    myDOM=aParser.getDocument();
    di<<"Document parsed\n";
  } else {
    di<<"Document not parsed\n";
    return 0;
  }

  LDOM_Element root = myDOM.getDocumentElement();

  if ( root.isNull() ) {
    di<<"Root element is null\n";
    return 0;
  }

  TCollection_AsciiString RootName = root.getTagName();
  di << "   RootName = " << RootName.ToCString() << "\n";
  LDOM_NodeList aChildList = root.GetAttributesList();
  for(Standard_Integer i=0,n=aChildList.getLength();i<n;i++) {
    LDOM_Node item = aChildList.item(i);
    TCollection_AsciiString itemName = item.getNodeName();
    TCollection_AsciiString itemValue = item.getNodeValue();
    di << "       AttributeName = " << itemName.ToCString() << "\n";
    di << "       AttributeValue = " << itemValue.ToCString() << "\n";
  }

//  LDOM_Element element;
//  for ( element = (const LDOM_Element&) root.getFirstChild();
//       !element.isNull();
//       element = (const LDOM_Element&) element.getNextSibling() ) {
  LDOM_Element element;
  LDOM_Node    node;
  for ( node = root.getFirstChild(), element = (const LDOM_Element&) node;
       !element.isNull();
	node = element.getNextSibling(), element = (const LDOM_Element&) node) {
    TCollection_AsciiString ElementName = element.getTagName();
    di << "   ElementName = " << ElementName.ToCString() << "\n";
    LDOM_NodeList aChildList2 = element.GetAttributesList();
    for(Standard_Integer i2=0,n2=aChildList2.getLength();i2<n2;i2++) {
      LDOM_Node item2 = aChildList2.item(i2);
      TCollection_AsciiString itemName2 = item2.getNodeName();
      TCollection_AsciiString itemValue2 = item2.getNodeValue();
      di << "       AttributeName = " << itemName2.ToCString() << "\n";
      di << "       AttributeValue = " << itemValue2.ToCString() << "\n";
    }
  }

  return 0;
}

static Standard_Integer  OCC984 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2) {
    di << "Usage : " << argv[0] << " file\n";
    return 1;
  }

  LDOMParser aParser;
  LDOM_Document myDOM;

  //Standard_Character  *File = new Standard_Character [100];
  //Sprintf(File,"%s",argv[1]);
  const char *File = (argv[1]);

  if(!aParser.parse(File)) {
    myDOM=aParser.getDocument();
    di<<"Document parsed\n";
  } else {
    di<<"Document not parsed\n";
  }

  return 0;
}

static Standard_Integer OCC1786 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if( argc != 2)
  {
    di << "Usage : " << argv[0] << " AutoHilight=0/1\n";
    return 1;
  }
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) 
  {
    cerr << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  Standard_Integer AutoHilightInteger =  Draw::Atoi(argv[1]);
  Standard_Boolean AutoHilightBoolean = Standard_False;
  if (AutoHilightInteger != 0) {
     AutoHilightBoolean = Standard_True;
  }
  aContext->SetAutomaticHilight(AutoHilightBoolean);
  return 0;
}

//#include <math.h>
// See QAOCC.cxx OCC6143
//static Standard_Integer OCC1723 (Draw_Interpretor& /*di*/, Standard_Integer argc, const char ** argv)
//{
//  if( argc != 1)
//  {
//    cout << "Usage : " << argv[0] << endl;
//    return 1;
//  }
//
//  Standard_Boolean isBad = Standard_False, isCaught;
//
//  // Case 1
//  isCaught = Standard_False;
//  {
//    try {
//      OCC_CATCH_SIGNALS
//      Standard_Integer a = 1;
//      Standard_Integer b = 0;
//      Standard_Integer c = a / b;
//    }
//    catch ( Standard_Failure ) {
//      isCaught = Standard_True;
//      cout << "OCC1723 Case 1 : OK" << endl;
//    }
//  }
//  isBad = isBad || !isCaught;
//
//  // Case 2
//  isCaught = Standard_False;
//  {
//    try {
//      OCC_CATCH_SIGNALS
//      Standard_Real d = -1.0;
//      Standard_Real e = sqrt(d);
//    }
//    catch ( Standard_Failure ) {
//      isCaught = Standard_True;
//      cout << "OCC1723 Case 2 : OK" << endl;
//    }
//  }
//  isBad = isBad || !isCaught;
//
//  // Case 3
//  isCaught = Standard_False;
//  {
//    try {
//      OCC_CATCH_SIGNALS
//      Standard_Real f = 1.0e-200;
//      Standard_Real g = 1.0e-200;
//      Standard_Real h = f * g;
//    }
//    catch ( Standard_Failure ) {
//      isCaught = Standard_True;
//      cout << "OCC1723 Case 3 : OK" << endl;
//    }
//  }
//  // MSV: underflow is not caught
//  //isBad = isBad || !isCaught;
//
//  // Case 4
//  isCaught = Standard_False;
//  {
//    try {
//      OCC_CATCH_SIGNALS
//      Standard_Real i = 1.0e+200;
//      Standard_Real j = 1.0e+200;
//      Standard_Real k = i * j;
//    }
//    catch ( Standard_Failure ) {
//      isCaught = Standard_True;
//      cout << "OCC1723 Case 4 : OK" << endl;
//    }
//  }
//  isBad = isBad || !isCaught;
//
//  if (isBad) {
//    cout << "OCC1723 : Error" << endl;
//  } else {
//    cout << "OCC1723 : OK" << endl;
//  }
//
//  return 0;
//}

#include <locale.h>
static Standard_Integer OCC1919_get (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if( argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  const TCollection_AsciiString anOldNumLocale =
    (Standard_CString) setlocale (LC_NUMERIC, NULL);
  di << "LC_NUMERIC = " << anOldNumLocale.ToCString() << "\n";
  return 0;
}
static Standard_Integer OCC1919_set (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if( argc != 2)
  {
    di << "Usage : " << argv[0] << " LC_NUMERIC\n";
    return 1;
  }
  TCollection_AsciiString aNumLocale(argv[1]);
  setlocale(LC_ALL, "") ;
  setlocale(LC_NUMERIC, aNumLocale.ToCString()) ;
  return 0;
}
#include <DDF.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Real.hxx>
#include <NCollection_BaseMap.hxx>
static Standard_Integer OCC1919_real (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc == 4) {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(argv[1],DF)) return 1;
    TDF_Label L;
    DDF::AddLabel(DF, argv[2], L);

    //TDataStd_Real::Set(L,Draw::Atof(arg[3]));
    TCollection_AsciiString AsciiStringReal(argv[3]);
    if (!AsciiStringReal.IsRealValue()) return 1;
    Standard_Real aReal = AsciiStringReal.RealValue();
    di << "aReal = " << aReal << "\n";

    TDataStd_Real::Set(L,aReal);
    return 0;
  }
  return 1;
}

#include <TDataStd_UAttribute.hxx>
static Standard_Integer OCC2932_SetIDUAttribute (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if( argc != 5)
  {
    di << "Usage : " << argv[0] << " (DF, entry, oldLocalID, newLocalID)\n";
    return 1;
  }
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(argv[1],DF)) return 1; 
  TDF_Label label;
  if( !DDF::FindLabel(DF, argv[2], label) ) {
    di << "No label for entry " << argv[2] << "\n";
    return 0;
  }
  Standard_GUID old_guid(argv[3]);  //"00000000-0000-0000-2222-000000000000");
  Standard_GUID new_guid(argv[4]);  //"00000000-0000-0000-2222-000000000001");

  Handle(TDataStd_UAttribute) UA;    
  if( !label.FindAttribute(old_guid, UA) ) {
    di << "No UAttribute Attribute on label " << argv[2] << "\n";
    return 0;
  }
  Handle(TDataStd_UAttribute) anotherUA;    
  if( label.FindAttribute(new_guid, anotherUA) ) {
    di << "There is this UAttribute Attribute on label " << argv[2] << "\n";
    return 0;
  }
  UA->SetID(new_guid);
  return 0;  
}

static Standard_Integer OCC2932_SetTag (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if( argc != 4)
  {
    di << "Usage : " << argv[0] << " (DF, entry, Tag)\n";
    return 1;
  }
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(argv[1],DF)) return 1;
  TDF_Label L;
  DDF::AddLabel(DF, argv[2], L);
  Standard_Integer Tag = Draw::Atoi(argv[3]);
  Handle(TDF_TagSource) A = TDF_TagSource::Set(L);
  A->Set(Tag);
  return 0;
}

#include <TDataStd_Current.hxx>
static Standard_Integer OCC2932_SetCurrent (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if( argc != 3)
  {
    di << "Usage : " << argv[0] << " (DF, entry)\n";
    return 1;
  }
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(argv[1],DF)) return 1;
  TDF_Label L;
  DDF::AddLabel(DF, argv[2], L);
  TDataStd_Current::Set(L);  
  return 0;
}

#include <TDataStd_Expression.hxx>
static Standard_Integer OCC2932_SetExpression (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if( argc != 4)
  {
    di << "Usage : " << argv[0] << " (DF, entry, Expression)\n";
    return 1;
  }
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(argv[1],DF)) return 1;
  TDF_Label L;
  DDF::AddLabel(DF, argv[2], L);
  TCollection_ExtendedString Expression(argv[3]);
  Handle(TDataStd_Expression) A = TDataStd_Expression::Set(L);
  A->SetExpression(Expression);
  return 0;
}

#include <TDataStd_Relation.hxx>
static Standard_Integer OCC2932_SetRelation (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if( argc != 4)
  {
    di << "Usage : " << argv[0] << " (DF, entry, Relation)\n";
    return 1;
  }
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(argv[1],DF)) return 1;
  TDF_Label L;
  DDF::AddLabel(DF, argv[2], L);
  TCollection_ExtendedString Relation(argv[3]);
  Handle(TDataStd_Relation) A = TDataStd_Relation::Set(L);
  A->SetRelation(Relation);
  return 0;
}

static Standard_Integer OCC3277 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if( argc != 2)
  {
    di << "Usage : " << argv[0] << " string\n";
    return 1;
  }
  TCollection_ExtendedString ExtendedString;
  TCollection_ExtendedString InputString(argv[1]);
  ExtendedString.Cat(InputString);
  //ExtendedString.Print(cout);
  Standard_SStream aSStream;
  ExtendedString.Print(aSStream);
  di << aSStream;
  return 0;
}

static Standard_Integer OCC6794 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc > 2)
    {
      di << "Usage: " << argv[0] << " [nb]\n";
      return 1;
    }

  char* max = ::getenv( "MMGT_THRESHOLD" );
  
  Standard_Integer aNb = 1;
  if ( max )
    aNb += Draw::Atoi( max );
  else
    aNb += 40000;

  if ( argc > 1 )
    aNb = Draw::Atoi( argv[1] );

  di << "Use nb = " << aNb << "\n";

  const char* c = "a";
  {
    TCollection_AsciiString anAscii;
    for ( int i = 1; i <= aNb; i++ ) {
      anAscii += TCollection_AsciiString( c );
    }
    Standard_Integer aLength = anAscii.Length();
    di << "aLength = " << aLength << "\n";
  }
  return 0;
}

static Standard_Integer OCC16485 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc > 1)
    {
      di << "Usage: " << argv[0] << "\n";
      return 1;
    }

  // Create points with X co-ordinate from varying from 0. to 1000.
  // anc compute cumulative bounding box by adding boxes for all the 
  // points, enlarged on tolerance
  
  Standard_Real tol = 1e-3;
  int nbstep = 1000;
  Bnd_Box Box;
  for ( int i=0; i <= nbstep; i++ )
  {
    gp_Pnt p(i, 0., 0.);
    Bnd_Box B;
    B.Add(p);
    B.Enlarge(tol);
    B.Add ( Box ); 
    Box = B; // in this case XMin of Box will grow each time
  }
  
  Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
  Box.Get (xmin, ymin, zmin, xmax, ymax, zmax);
  //cout.precision(16);
  //cout << "Resulting dimensions: Xmin = " << xmin << " , Xmax = " << xmax << " , Tolerance = " << tol << endl;
  di << "Resulting dimensions: Xmin = " << xmin << " , Xmax = " << xmax << " , Tolerance = " << tol << "\n";
  if ( Abs ( xmin + tol ) > 1e-10 )
      di << "TEST FAILED: Xmin must be equal to -1e3!\n";
  else
      di << "TEST OK\n";
      //cout << "TEST FAILED: Xmin must be equal to -1e3!" << endl;
      //cout << "TEST OK" << endl;
      //di << "TEST FAILED: Xmin must be equal to -1e3!\n";
      //di << "TEST OK\n";
  return 0;
}
//Resulting dimensions: Xmin = -0.001 , Xmax = 1000.001 , Tolerance = 0.001
//TEST OK

void QABugs::Commands_14(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add ("BUC60897", "BUC60897", __FILE__, BUC60897, group);
  theCommands.Add ("BUC60889", "BUC60889 point_1 point_2 name_of_edge bndbox_X1 bndbox_Y1 bndbox_Z1 bndbox_X2 bndbox_Y2 bndbox_Z2", __FILE__, BUC60889, group);
  theCommands.Add ("BUC60852", "BUC60852 name_of_edge bndbox_X1 bndbox_Y1 bndbox_Z1 bndbox_X2 bndbox_Y2 bndbox_Z2", __FILE__, BUC60852, group);
  theCommands.Add ("BUC60854", "BUC60854 result_shape name_of_shape name_of_face name_of_wire/name_of_edge [ name_of_wire/name_of_edge ... ] [ name_of_face name_of_wire/name_of_edge [ name_of_wire/name_of_edge ... ] ... ] [ @ edge_on_shape edge_on_wire [ edge_on_shape edge_on_wire ... ] ] ", __FILE__, BUC60854, group);
  theCommands.Add ("BUC60870", "BUC60870 result name_of_shape_1 name_of_shape_2 dev", __FILE__, BUC60870, group);
  theCommands.Add ("BUC60902", "BUC60902", __FILE__, BUC60902, group);
  theCommands.Add ("BUC60944", "BUC60944 path", __FILE__, BUC60944, group);
  theCommands.Add ("BUC60868", "BUC60868 Result Shell", __FILE__, BUC60868, group);
  theCommands.Add ("BUC60924", "BUC60924 curve X Y Z", __FILE__, BUC60924, group);
  theCommands.Add ("BUC60920", "BUC60920", __FILE__, BUC60920, group);
  theCommands.Add ("OCC983", "OCC983 file", __FILE__, OCC983, group);
  theCommands.Add ("OCC984", "OCC984 file", __FILE__, OCC984, group);

  theCommands.Add ("OCC1786", "OCC1786 AutoHilight=0/1", __FILE__, OCC1786, group);

//  theCommands.Add ("OCC1723", "OCC1723", __FILE__, OCC1723, group);

  theCommands.Add ("OCC1919_get", "OCC1919_get", __FILE__, OCC1919_get, group);

  theCommands.Add ("OCC1919_set", "OCC1919_set LC_NUMERIC", __FILE__, OCC1919_set, group);

  theCommands.Add ("OCC1919_real", "OCC1919_real (DF, entry, value)", __FILE__, OCC1919_real, group);

  theCommands.Add ("OCC2932_SetIDUAttribute", "OCC2932_SetIDUAttribute (DF, entry, oldLocalID, newLocalID)", __FILE__, OCC2932_SetIDUAttribute, group);

  theCommands.Add ("OCC2932_SetTag", "OCC2932_SetTag (DF, entry, Tag)", __FILE__, OCC2932_SetTag, group);

  theCommands.Add ("OCC2932_SetCurrent", "OCC2932_SetCurrent (DF, entry)", __FILE__, OCC2932_SetCurrent, group);

  theCommands.Add ("OCC2932_SetExpression", "OCC2932_SetExpression (DF, entry, Expression)", __FILE__, OCC2932_SetExpression, group);

  theCommands.Add ("OCC2932_SetRelation", "OCC2932_SetRelation (DF, entry, Relation)", __FILE__, OCC2932_SetRelation, group);

  theCommands.Add ("OCC3277", "OCC3277 string", __FILE__, OCC3277, group);

  theCommands.Add ("OCC6794", "OCC6794 [nb]", __FILE__, OCC6794, group);

  theCommands.Add ("OCC16485", "OCC16485", __FILE__, OCC16485, group);

  return;
}
