// Created on: 1994-10-25
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// modif : jlr branchement F(t) pour Edge/Face
//  Modified by skv - Wed Jun  9 17:16:26 2004 OCC5898
//  modified by Edward AGAPOV (eap) Fri Feb  8 2002 (bug occ67 == BUC61052)
//  ComputeData(), case where BRepBlend_Walking::Continu() can't get up to Target

#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <AppBlend_Approx.hxx>
#include <Approx_SweepFunction.hxx>
#include <Blend_CurvPointFuncInv.hxx>
#include <Blend_FuncInv.hxx>
#include <Blend_Function.hxx>
#include <Blend_Point.hxx>
#include <Blend_RstRstFunction.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <Blend_SurfPointFuncInv.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBlend_AppFunc.hxx>
#include <BRepBlend_AppFuncRst.hxx>
#include <BRepBlend_AppFuncRstRst.hxx>
#include <BRepBlend_AppSurf.hxx>
#include <BRepBlend_AppSurface.hxx>
#include <BRepBlend_ConstRad.hxx>
#include <BRepBlend_ConstRadInv.hxx>
#include <BRepBlend_CSWalking.hxx>
#include <BRepBlend_Extremity.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepBlend_PointOnRst.hxx>
#include <BRepBlend_RstRstLineBuilder.hxx>
#include <BRepBlend_SurfRstLineBuilder.hxx>
#include <BRepBlend_Walking.hxx>
#include <BRepTopAdaptor_HVertex.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <BSplCLib.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomLib.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopExp.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <stdio.h>

//#define DRAW 

#ifdef DRAW 
#include <Draw_Appli.hxx>
#include <Draw_Segment2D.hxx>
#include <Draw_Marker2D.hxx>
#include <Draw_Segment3D.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <BRepAdaptor_HSurface.hxx>
static Standard_Integer IndexOfConge = 0;
#endif

#ifdef OCCT_DEBUG
extern Standard_Boolean ChFi3d_GettraceDRAWFIL(); 
extern Standard_Boolean ChFi3d_GettraceDRAWWALK(); 
extern Standard_Boolean ChFi3d_GetcontextNOOPT();
extern void ChFi3d_SettraceDRAWFIL(const Standard_Boolean b); 
extern void ChFi3d_SettraceDRAWWALK(const Standard_Boolean b); 
extern void ChFi3d_SetcontextNOOPT(const Standard_Boolean b);
#endif

#ifdef DRAW 
static void drawline(const Handle(BRepBlend_Line)& lin,
		     const Standard_Boolean iscs)
{
  Handle(Draw_Marker3D) p3d;
  Handle(Draw_Marker2D) p2d;
  Handle(Draw_Segment3D) tg3d;
  Handle(Draw_Segment2D) tg2d;
  
  for(Standard_Integer i = 1; i <= lin->NbPoints(); i++){
    const Blend_Point& pt = lin->Point(i);
    gp_Pnt point = pt.PointOnS1();
    gp_Pnt extr = point.Translated(pt.TangentOnS1());
    p3d = new Draw_Marker3D(point,Draw_Square,Draw_rouge);
    dout<<p3d;
    tg3d = new Draw_Segment3D(point,extr,Draw_rouge);
    dout<<tg3d;
    point = pt.PointOnS2();
    extr = point.Translated(pt.TangentOnS2());
    p3d = new Draw_Marker3D(point,Draw_Plus,Draw_jaune);
    dout<<p3d;
    tg3d = new Draw_Segment3D(point,extr,Draw_jaune);
    dout<<tg3d;

    Standard_Real u,v;
    pt.ParametersOnS1(u,v);
    gp_Pnt2d point2d(u,v);
    gp_Pnt2d extr2d = point2d.Translated(pt.Tangent2dOnS1());
    p2d = new Draw_Marker2D(point2d,Draw_Square,Draw_rouge);
    dout<<p2d;
    tg2d = new Draw_Segment2D(point2d,extr2d,Draw_rouge);
    dout<<tg2d;
    pt.ParametersOnS2(u,v);
    point2d.SetCoord(u,v);
    extr2d = point2d.Translated(pt.Tangent2dOnS2());
    p2d = new Draw_Marker2D(point2d,Draw_Plus,Draw_jaune);
    dout<<p2d;
    tg2d = new Draw_Segment2D(point2d,extr2d,Draw_jaune);
    dout<<tg2d;
    dout.Flush();
  }
}
#endif
//=======================================================================
//function : SearchIndex
//purpose  : 
//           
//=======================================================================
static Standard_Integer SearchIndex(const Standard_Real Value,
				    Handle(BRepBlend_Line)& Lin)
{
  Standard_Integer NbPnt =  Lin->NbPoints(), Ind;

  for (Ind = 1;  
       (Ind < NbPnt) && (Lin->Point(Ind).Parameter() < Value); )
    Ind++;
  return Ind;
}


//=======================================================================
//function : IsObst
//purpose  : 
//           
//=======================================================================
static Standard_Integer nbedconnex(const TopTools_ListOfShape& L)
{
  Standard_Integer nb = 0, i = 0;
  TopTools_ListIteratorOfListOfShape It1(L);
  for(;It1.More();It1.Next(),i++){
    const TopoDS_Shape& curs = It1.Value();
    Standard_Boolean dejavu = 0;
    TopTools_ListIteratorOfListOfShape It2(L);
    for(Standard_Integer j = 0; j < i && It2.More(); j++, It2.Next()){
      if(curs.IsSame(It2.Value())){
	dejavu = 1;
	break;
      }
    }
    if(!dejavu) nb++;
  }
  return nb;
}

static Standard_Boolean IsVois(const TopoDS_Edge&     E,
			       const TopoDS_Vertex&   Vref,
			       const ChFiDS_Map&      VEMap,
			       TopTools_MapOfShape&   DONE,
			       const Standard_Integer prof,
			       const Standard_Integer profmax)
{
  if(prof > profmax) return Standard_False;
  if(DONE.Contains(E)) return Standard_False;
  TopoDS_Vertex V1,V2;
  TopExp::Vertices(E,V1,V2);
  if(Vref.IsSame(V1) || Vref.IsSame(V2)) return Standard_True;
  DONE.Add(E);
  const TopTools_ListOfShape& L1 = VEMap(V1);
  Standard_Integer i1 = nbedconnex(L1);
  TopTools_ListIteratorOfListOfShape It1(L1);
  for(;It1.More();It1.Next()){
    const TopoDS_Edge& curE = TopoDS::Edge(It1.Value());
    if(i1 <= 2){
      if(IsVois(curE,Vref,VEMap,DONE,prof,profmax)) return Standard_True;
    }
    else if(IsVois(curE,Vref,VEMap,DONE,prof+1,profmax)) return Standard_True;
  }
  const TopTools_ListOfShape& L2 = VEMap(V2);
#ifdef OCCT_DEBUG
//  Standard_Integer i2 = nbedconnex(L2);
#endif
  TopTools_ListIteratorOfListOfShape It2(L2);
  for(;It2.More();It2.Next()){
    const TopoDS_Edge& curE = TopoDS::Edge(It2.Value());
    if(i1 <= 2){
      if(IsVois(curE,Vref,VEMap,DONE,prof,profmax)) return Standard_True;
    }
    else if(IsVois(curE,Vref,VEMap,DONE,prof+1,profmax)) return Standard_True;
  }
  return Standard_False;
}

static Standard_Boolean IsObst(const ChFiDS_CommonPoint& CP,
			       const TopoDS_Vertex&      Vref,
			       const ChFiDS_Map&         VEMap)
{
  if(!CP.IsOnArc()) return Standard_False;
  const TopoDS_Edge& E = CP.Arc();
  TopTools_MapOfShape DONE;
  Standard_Integer prof = 4;
  return !IsVois(E,Vref,VEMap,DONE,0,prof);
}

//=======================================================================
//function : CompParam
//purpose  : 
//           
//=======================================================================

static void CompParam(Geom2dAdaptor_Curve  Carc,
		      Handle(Geom2d_Curve) Ctg,
		      Standard_Real&       parc,
		      Standard_Real&       ptg,
		      const Standard_Real  prefarc,
		      const Standard_Real  preftg)
{
  Standard_Boolean found = 0;
  //(1) It is checked if the provided parameters are good 
  //    if pcurves have the same parameters as the spine.
  gp_Pnt2d point = Carc.Value(prefarc);
  Standard_Real distini = point.Distance(Ctg->Value(preftg));
  if (distini <= Precision::PConfusion()) {
    parc =  prefarc;
    ptg = preftg;
    found = Standard_True;
  }
  else {
    //(2) Intersection
#ifdef OCCT_DEBUG
    cout<< "CompParam : bad intersection parameters"<<endl; 
#endif
    IntRes2d_IntersectionPoint int2d;
    Geom2dInt_GInter Intersection;
    Standard_Integer nbpt,nbseg;
    Intersection.Perform(Geom2dAdaptor_Curve(Ctg),Carc,
			 Precision::PIntersection(),
			 Precision::PIntersection());

    Standard_Real dist = Precision::Infinite(), p1, p2;
    if (Intersection.IsDone()){
      if (!Intersection.IsEmpty()){
	nbseg = Intersection.NbSegments();
	if ( nbseg > 0 ){ 
#ifdef OCCT_DEBUG
	  cout<< "segments of intersection on the restrictions"<<endl; 
#endif  
	}
	nbpt = Intersection.NbPoints();
	for (Standard_Integer i = 1; i <= nbpt; i++) {
	  int2d = Intersection.Point(i);
	  p1 = int2d.ParamOnFirst();
	  p2 = int2d.ParamOnSecond();
	  if(Abs(prefarc - p2) < dist){
	    ptg  = p1;
	    parc = p2;
	    dist = Abs(prefarc - p2);
	    found = 1;
	  }
	}
      }
    }
  }
  
  if(!found){
    // (3) Projection...
#ifdef OCCT_DEBUG
    cout<<"CompParam : failed intersection PC, projection is created."<<endl;
#endif
    parc = prefarc;
    Geom2dAPI_ProjectPointOnCurve projector(point,Ctg);

    if(projector.NbPoints() == 0){
      // This happens in some cases when there is a vertex 
      // at the end of spine...
      ptg = preftg; 
#ifdef OCCT_DEBUG
      cout<<"CompParam : failed proj p2d/c2d, the extremity is taken!" <<endl;
#endif
    }
    else  {
      // It is checked if everything was calculated correctly (EDC402 C2)
      if  (projector.LowerDistance() < distini) 
	ptg = projector.LowerDistanceParameter();
      else  ptg = preftg;
    }
  }
}

//=======================================================================
//function : CompBlendPoint
//purpose  : create BlendPoint corresponding to a tangency on Vertex
// pmn : 15/10/1997 : returns false, if there is no pcurve    
//=======================================================================

static Standard_Boolean CompBlendPoint(const TopoDS_Vertex& V,
				       const TopoDS_Edge& E,
				       const Standard_Real W,
				       const TopoDS_Face F1,
				       const TopoDS_Face F2,
				       Blend_Point& BP)
{    
  gp_Pnt2d P1, P2;
  gp_Pnt P3d;
  Standard_Real param, f, l;
  Handle(Geom2d_Curve) pc;
  
  P3d = BRep_Tool::Pnt(V);
  param = BRep_Tool::Parameter(V,E,F1);
  pc = BRep_Tool::CurveOnSurface(E,F1,f,l);
  if (pc.IsNull()) return Standard_False;
  P1 =  pc->Value(param); 
  param = BRep_Tool::Parameter(V,E,F2);
  pc = BRep_Tool::CurveOnSurface(E,F2,f,l);
  if (pc.IsNull()) return Standard_False;
  P2 =  pc->Value(param);
  BP.SetValue(P3d, P3d, W, P1.X(), P1.Y(),  P2.X(), P2.Y());
  return  Standard_True;
}

//=======================================================================
//function :  UpdateLine
//purpose  : Updates extremities after a partial invalidation   
//=======================================================================

static void UpdateLine(Handle(BRepBlend_Line)& Line, 
		       const Standard_Boolean isfirst)
{
  Standard_Real tguide, U, V;
  if (isfirst) {
   const Blend_Point& BP = Line->Point(1);
   tguide = BP.Parameter();
   if (Line->StartPointOnFirst().ParameterOnGuide() < tguide) {
     BRepBlend_Extremity BE;
     BP.ParametersOnS1(U, V);
     BE.SetValue(BP.PointOnS1(), U, V, Precision::Confusion());
     Line->SetStartPoints(BE, Line->StartPointOnSecond());
   }
   if (Line->StartPointOnSecond().ParameterOnGuide() < tguide) {
     BRepBlend_Extremity BE;
     BP.ParametersOnS2(U, V);
     BE.SetValue(BP.PointOnS2(), U, V, Precision::Confusion());
     Line->SetStartPoints(Line->StartPointOnFirst(), BE);
   }
 }
  else {
   const Blend_Point& BP = Line->Point(Line->NbPoints());
   tguide = BP.Parameter();
   if (Line->EndPointOnFirst().ParameterOnGuide() > tguide) {
     BRepBlend_Extremity BE;
     BP.ParametersOnS1(U, V);
     BE.SetValue(BP.PointOnS1(), U, V, Precision::Confusion());
     Line->SetEndPoints(BE, Line->EndPointOnSecond());
   }
   if (Line->EndPointOnSecond().ParameterOnGuide() > tguide) {
     BRepBlend_Extremity BE;
     BP.ParametersOnS2(U, V);
     BE.SetValue(BP.PointOnS2(), U, V, Precision::Confusion());
     Line->SetEndPoints(Line->EndPointOnFirst(), BE);
   }
  }
}

//=======================================================================
//function : CompleteData
//purpose  : Calculates curves and CommonPoints from the data
//           calculated by filling.
//=======================================================================

Standard_Boolean ChFi3d_Builder::CompleteData
(Handle(ChFiDS_SurfData)&        Data,
 const Handle(Geom_Surface)&     Surfcoin,
 const Handle(Adaptor3d_HSurface)& S1,
 const Handle(Geom2d_Curve)&     PC1,
 const Handle(Adaptor3d_HSurface)& S2,
 const Handle(Geom2d_Curve)&     PC2,
 const TopAbs_Orientation        Or,
 const Standard_Boolean          On1,
 const Standard_Boolean          Gd1,
 const Standard_Boolean          Gd2,
 const Standard_Boolean          Gf1,
 const Standard_Boolean          Gf2)
{
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  Data->ChangeSurf(DStr.AddSurface(TopOpeBRepDS_Surface(Surfcoin,tolesp)));
#ifdef DRAW
  ChFi3d_SettraceDRAWFIL(Standard_True);
  if (ChFi3d_GettraceDRAWFIL()) {
    IndexOfConge++;
//    char name[100];
    char* name = new char[100];
    sprintf(name,"%s_%d","Surf",IndexOfConge);
    DrawTrSurf::Set(name,Surfcoin);
  }
#endif
    
  Standard_Real UFirst,ULast,VFirst,VLast;
  Surfcoin->Bounds(UFirst,ULast,VFirst,VLast);
  if(!Gd1) Data->ChangeVertexFirstOnS1().SetPoint(Surfcoin->Value(UFirst,VFirst));
  if(!Gd2) Data->ChangeVertexFirstOnS2().SetPoint(Surfcoin->Value(UFirst,VLast));
  if(!Gf1) Data->ChangeVertexLastOnS1().SetPoint(Surfcoin->Value(ULast,VFirst));
  if(!Gf2) Data->ChangeVertexLastOnS2().SetPoint(Surfcoin->Value(ULast,VLast));

  //calculate curves side S1
  Handle(Geom_Curve) Crv3d1;
  if(!PC1.IsNull()) Crv3d1= Surfcoin->VIso(VFirst);
  gp_Pnt2d pd1(UFirst,VFirst), pf1(ULast,VFirst);
  gp_Lin2d lfil1(pd1,gp_Dir2d(gp_Vec2d(pd1,pf1)));
  Handle(Geom2d_Curve) PCurveOnSurf = new Geom2d_Line(lfil1);
  TopAbs_Orientation tra1 = TopAbs_FORWARD, orsurf = Or;
  Standard_Real x,y,w = 0.5*(UFirst+ULast);
  gp_Pnt p;
  gp_Vec du,dv;
  Handle(Geom2d_Curve) c2dtrim;
  Standard_Real tolreached = 1.e-5;
  if(!PC1.IsNull()){
    Handle(GeomAdaptor_HCurve) hcS1 = new GeomAdaptor_HCurve(Crv3d1);
    c2dtrim = new Geom2d_TrimmedCurve(PC1,UFirst,ULast);
    ChFi3d_SameParameter(hcS1,c2dtrim,S1,tolapp3d,tolreached);
    c2dtrim->Value(w).Coord(x,y);
    S1->D1(x,y,p,du,dv);
    gp_Vec nf = du.Crossed(dv);
    Surfcoin->D1(w,VFirst,p,du,dv);
    gp_Vec ns = du.Crossed(dv);
    if(nf.Dot(ns) > 0.)  tra1 = TopAbs_REVERSED;
    else if(On1) orsurf = TopAbs::Reverse(orsurf);
  }
  Standard_Integer Index1OfCurve = 
    DStr.AddCurve(TopOpeBRepDS_Curve(Crv3d1,tolreached));
  ChFiDS_FaceInterference& Fint1 = Data->ChangeInterferenceOnS1();
  Fint1.SetFirstParameter(UFirst);
  Fint1.SetLastParameter(ULast);
  Fint1.SetInterference(Index1OfCurve,tra1,c2dtrim,PCurveOnSurf);
  //calculate curves side S2
  Handle(Geom_Curve) Crv3d2;
  if(!PC2.IsNull()) Crv3d2 = Surfcoin->VIso(VLast);
  gp_Pnt2d pd2(UFirst,VLast), pf2(ULast,VLast);
  gp_Lin2d lfil2(pd2,gp_Dir2d(gp_Vec2d(pd2,pf2)));
  PCurveOnSurf = new Geom2d_Line(lfil2);
  TopAbs_Orientation tra2 = TopAbs_FORWARD;
  if(!PC2.IsNull()){
    Handle(GeomAdaptor_HCurve) hcS2 = new GeomAdaptor_HCurve(Crv3d2);
    c2dtrim = new Geom2d_TrimmedCurve(PC2,UFirst,ULast);
    ChFi3d_SameParameter(hcS2,c2dtrim,S2,tolapp3d,tolreached);
    c2dtrim->Value(w).Coord(x,y);
    S2->D1(x,y,p,du,dv);
    gp_Vec np = du.Crossed(dv);
    Surfcoin->D1(w,VLast,p,du,dv);
    gp_Vec ns = du.Crossed(dv);
    if(np.Dot(ns) < 0.) {
      tra2 = TopAbs_REVERSED;
      if(!On1) orsurf = TopAbs::Reverse(orsurf);
    }
  }
  Standard_Integer Index2OfCurve = 
    DStr.AddCurve(TopOpeBRepDS_Curve(Crv3d2,tolreached));
  ChFiDS_FaceInterference& Fint2 = Data->ChangeInterferenceOnS2();
  Fint2.SetFirstParameter(UFirst);
  Fint2.SetLastParameter(ULast);
  Fint2.SetInterference(Index2OfCurve,tra2,c2dtrim,PCurveOnSurf);
  Data->ChangeOrientation() = orsurf;
  return Standard_True;
}

//=======================================================================
//function : CompleteData
//purpose  : Calculates the surface of curves and eventually 
//           CommonPoints from the data calculated in ComputeData.
//
//  11/08/1996 : Use of F(t)
//
//=======================================================================

Standard_Boolean ChFi3d_Builder::CompleteData
(Handle(ChFiDS_SurfData)& Data,
 Blend_Function& Func,
 Handle(BRepBlend_Line)& lin,
 const Handle(Adaptor3d_HSurface)& S1,
 const Handle(Adaptor3d_HSurface)& S2,
 const TopAbs_Orientation Or1,
 const Standard_Boolean Gd1,
 const Standard_Boolean Gd2,
 const Standard_Boolean Gf1,
 const Standard_Boolean Gf2,
 const Standard_Boolean Reversed)
{
  Handle(BRepBlend_AppFunc) TheFunc 
    = new (BRepBlend_AppFunc)(lin, Func, tolapp3d, 1.e-5);

  Standard_Integer Degmax = 20, Segmax = 5000;
  BRepBlend_AppSurface approx (TheFunc, 
			       lin->Point(1).Parameter(),
			       lin->Point(lin->NbPoints()).Parameter(),
			       tolapp3d, 1.e-5, //tolapp2d, tolerance max
			       tolappangle, // Contact G1 
			       myConti, Degmax, Segmax);  
  if (!approx.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "Approximation non faite !!!" << endl;
#endif
    return Standard_False;
  }
#ifdef OCCT_DEBUG
  approx.Dump(cout);
#endif
  return StoreData( Data, approx, lin, S1, S2, Or1, Gd1, Gd2, Gf1, Gf2, Reversed);
} 


//=======================================================================
//function : CompleteData
//purpose  : New overload for functions surf/rst
// jlr le 28/07/97 branchement F(t)
//=======================================================================

Standard_Boolean ChFi3d_Builder::CompleteData
(Handle(ChFiDS_SurfData)&        Data,
 Blend_SurfRstFunction&          Func,
 Handle(BRepBlend_Line)&         lin,
 const Handle(Adaptor3d_HSurface)& S1,
 const Handle(Adaptor3d_HSurface)& S2,
 const TopAbs_Orientation        Or,
 const Standard_Boolean          Reversed)
{
  Handle(BRepBlend_AppFuncRst) TheFunc 
    = new (BRepBlend_AppFuncRst)(lin, Func, tolapp3d, 1.e-5);
  BRepBlend_AppSurface approx (TheFunc, 
			       lin->Point(1).Parameter(),
			       lin->Point(lin->NbPoints()).Parameter(),
			       tolapp3d, 1.e-5, //tolapp2d, tolerance max
			       tolappangle, // Contact G1 
			       myConti);  
 if (!approx.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "Approximation is not done!" << endl;
#endif  
    return Standard_False;
  }
#ifdef OCCT_DEBUG
  approx.Dump(cout);
#endif

  return StoreData(Data,approx,lin,S1,S2,Or,0,0,0,0,Reversed);
} 



//=======================================================================
//function : CompleteData
//purpose  : New overload for functions rst/rst
// jlr le 28/07/97 branchement F(t)
//=======================================================================

Standard_Boolean ChFi3d_Builder::CompleteData
(Handle(ChFiDS_SurfData)&        Data,
 Blend_RstRstFunction&           Func,
 Handle(BRepBlend_Line)&         lin,
 const Handle(Adaptor3d_HSurface)& S1,
 const Handle(Adaptor3d_HSurface)& S2,
 const TopAbs_Orientation        Or)
{
  Handle(BRepBlend_AppFuncRstRst) TheFunc 
    = new (BRepBlend_AppFuncRstRst)(lin, Func, tolapp3d, 1.e-5);
  BRepBlend_AppSurface approx (TheFunc, 
			       lin->Point(1).Parameter(),
			       lin->Point(lin->NbPoints()).Parameter(),
			       tolapp3d, 1.e-5, //tolapp2d, tolerance max
			       tolappangle, // Contact G1 
			       myConti);  
 if (!approx.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "Approximation non faite !!!" << endl;
#endif  
    return Standard_False;
  }
#ifdef OCCT_DEBUG
  approx.Dump(cout);
#endif

  return StoreData(Data,approx,lin,S1,S2,Or,0,0,0,0);
} 




//=======================================================================
//function : StoreData
//purpose  : Copy of an approximation result in SurfData.
//=======================================================================

Standard_Boolean ChFi3d_Builder::StoreData(Handle(ChFiDS_SurfData)& Data,
					   const AppBlend_Approx& approx,
					   const Handle(BRepBlend_Line)& lin,
					   const Handle(Adaptor3d_HSurface)& S1,
					   const Handle(Adaptor3d_HSurface)& S2,
					   const TopAbs_Orientation Or1,
					   const Standard_Boolean Gd1,
					   const Standard_Boolean Gd2,
					   const Standard_Boolean Gf1,
					   const Standard_Boolean Gf2,
					   const Standard_Boolean Reversed)
{
  // Small control tools.
  static Handle(GeomAdaptor_HCurve) checkcurve;
  if(checkcurve.IsNull()) checkcurve = new GeomAdaptor_HCurve();
  GeomAdaptor_Curve& chc = checkcurve->ChangeCurve();
  Standard_Real tolget3d, tolget2d, tolaux, tolC1,  tolcheck;
  Standard_Real  tolC2 = 0.;
  approx.TolReached(tolget3d, tolget2d);
  tolaux = approx.TolCurveOnSurf(1);
  tolC1 = tolget3d + tolaux;
  if(!S2.IsNull()) {
    tolaux = approx.TolCurveOnSurf(2);
    tolC2 = tolget3d + tolaux;
  }

  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  // By default parametric space is created using a square surface
  // to be able to parameterize in U by # R*teta // a revoir lbo 29/08/97
  const TColStd_Array1OfReal& ku = approx.SurfUKnots();
  const TColStd_Array1OfReal& kv = approx.SurfVKnots();
  Standard_Real larg = (kv(kv.Upper())-kv(kv.Lower()));
  TColStd_Array1OfReal& kku = *((TColStd_Array1OfReal*)((void*)&ku));
  BSplCLib::Reparametrize(0.,larg,kku);
  Handle(Geom_BSplineSurface) Surf = 
    new Geom_BSplineSurface(approx.SurfPoles(),approx.SurfWeights(),
			    kku,kv,
			    approx.SurfUMults(),approx.SurfVMults(),
			    approx.UDegree(),approx.VDegree());
// extension of the surface 

  Standard_Real length1,length2;
  length1=Data->FirstExtensionValue();
  length2=Data->LastExtensionValue();

  Handle(Geom_BoundedSurface) aBndSurf = Surf;
  if (length1 > Precision::Confusion())
    GeomLib::ExtendSurfByLength(aBndSurf,length1,1,Standard_False,Standard_False);
  if (length2 >  Precision::Confusion())
    GeomLib::ExtendSurfByLength(aBndSurf,length2,1,Standard_False,Standard_True);
  Surf = Handle(Geom_BSplineSurface)::DownCast (aBndSurf);

  //Correction of surface on extremities
  if (length1 <= Precision::Confusion())
  {
    gp_Pnt P11, P21;
    P11 = lin->StartPointOnFirst().Value();
    P21 = lin->StartPointOnSecond().Value();
    Surf->SetPole(1, 1, P11);
    Surf->SetPole(Surf->NbUPoles(), 1, P21);
  }
  if (length2 <= Precision::Confusion())
  {
    gp_Pnt P12, P22;
    P12 = lin->EndPointOnFirst().Value();
    P22 = lin->EndPointOnSecond().Value();
    Surf->SetPole(1, Surf->NbVPoles(), P12);
    Surf->SetPole(Surf->NbUPoles(), Surf->NbVPoles(), P22);
  }

  Data->ChangeSurf(DStr.AddSurface(TopOpeBRepDS_Surface(Surf,tolget3d)));

#ifdef DRAW
  ChFi3d_SettraceDRAWFIL(Standard_True);
  if (ChFi3d_GettraceDRAWFIL()) {
    IndexOfConge++;
//    char name[100];
    char* name=new char[100];
    sprintf(name,"%s_%d","Surf",IndexOfConge);
    DrawTrSurf::Set(name,Surf);
  }
#endif
  Standard_Real UFirst,ULast,VFirst,VLast,pppdeb,pppfin;
  Surf->Bounds(UFirst,ULast,VFirst,VLast);
  BRepAdaptor_Curve2d brc;
  BRepAdaptor_Curve CArc;
  Handle(BRepAdaptor_HSurface) 
    BS1 = Handle(BRepAdaptor_HSurface)::DownCast(S1);
  Handle(BRepAdaptor_HSurface) 
    BS2 = Handle(BRepAdaptor_HSurface)::DownCast(S2);
  Geom2dAPI_ProjectPointOnCurve projector;

  Standard_Real Uon1 = UFirst, Uon2 = ULast;
  Standard_Integer ion1 = 1, ion2 = 2;
  if(Reversed) { Uon1 = ULast; Uon2 = UFirst; ion1 = 2; ion2 = 1; }
  
  // The SurfData is filled in what concerns S1,
  Handle(Geom_Curve) Crv3d1 = Surf->UIso(Uon1);
  gp_Pnt2d pori1(Uon1,0.);
  gp_Lin2d lfil1(pori1,gp::DY2d());
  Handle(Geom2d_Curve) PCurveOnSurf = new Geom2d_Line(lfil1);
  Handle(Geom2d_Curve) PCurveOnFace;
  PCurveOnFace = new 
    Geom2d_BSplineCurve(approx.Curve2dPoles(ion1),approx.Curves2dKnots(),
			approx.Curves2dMults(),approx.Curves2dDegree());
  
  
   Standard_Real par1=PCurveOnFace->FirstParameter();
   Standard_Real par2= PCurveOnFace->LastParameter();
   chc.Load(Crv3d1,par1,par2);
   
 if(!ChFi3d_CheckSameParameter(checkcurve,PCurveOnFace,S1,tolC1,tolcheck)){
#ifdef OCCT_DEBUG
   cout<<"aaproximate tolerance under-valued : "<<tolC1<<" for "<<tolcheck<<endl;
#endif 
    tolC1 = tolcheck;
  }
  Standard_Integer Index1OfCurve = 
    DStr.AddCurve(TopOpeBRepDS_Curve(Crv3d1,tolC1));
  
  Standard_Real uarc,utg;
  if(Gd1){
    TopoDS_Face forwfac = BS1->ChangeSurface().Face();
    forwfac.Orientation(TopAbs_FORWARD);
    brc.Initialize(Data->VertexFirstOnS1().Arc(),forwfac);
    ChFiDS_CommonPoint& V = Data->ChangeVertexFirstOnS1();
    CArc.Initialize(V.Arc());
    CompParam(brc,PCurveOnFace,uarc,utg, V.ParameterOnArc(), V.Parameter());
    tolcheck = CArc.Value(uarc).Distance(V.Point());
    V.SetArc(tolC1+tolcheck,V.Arc(),uarc,V.TransitionOnArc());
    pppdeb = utg;
  }
  else pppdeb = VFirst;
  if(Gf1){
    TopoDS_Face forwfac = BS1->ChangeSurface().Face();
    forwfac.Orientation(TopAbs_FORWARD);
    ChFiDS_CommonPoint& V = Data->ChangeVertexLastOnS1();
    brc.Initialize(V.Arc(),forwfac);
    CArc.Initialize(V.Arc());
    CompParam(brc,PCurveOnFace,uarc,utg, V.ParameterOnArc(), V.Parameter());
    tolcheck = CArc.Value(uarc).Distance(V.Point());
    V.SetArc(tolC1+tolcheck,V.Arc(),uarc,V.TransitionOnArc());
    pppfin = utg;
  }
  else pppfin = VLast;
  ChFiDS_FaceInterference& Fint1 = Data->ChangeInterferenceOnS1();
  Fint1.SetFirstParameter(pppdeb);
  Fint1.SetLastParameter(pppfin);
  TopAbs_Orientation TraOn1;
  if(Reversed) TraOn1 = ChFi3d_TrsfTrans(lin->TransitionOnS2());
  else TraOn1 = ChFi3d_TrsfTrans(lin->TransitionOnS1());
  Fint1.SetInterference(Index1OfCurve,TraOn1,PCurveOnFace,PCurveOnSurf);
  
  // SurfData is filled in what concerns S2,
  Handle(Geom_Curve) Crv3d2 = Surf->UIso(Uon2);
  gp_Pnt2d pori2(Uon2,0.);
  gp_Lin2d lfil2(pori2,gp::DY2d());
  PCurveOnSurf = new Geom2d_Line(lfil2);
  if(!S2.IsNull()){
    PCurveOnFace = new Geom2d_BSplineCurve(approx.Curve2dPoles(ion2),
					   approx.Curves2dKnots(),
					   approx.Curves2dMults(),
					   approx.Curves2dDegree());
    chc.Load(Crv3d2,par1,par2);
   if(!ChFi3d_CheckSameParameter(checkcurve,PCurveOnFace,S2,tolC2,tolcheck)){
#ifdef OCCT_DEBUG
      cout<<"approximate tolerance under-evaluated : "<<tolC2<<" for "<<tolcheck<<endl;
#endif 
      tolC2 = tolcheck;
    }
  }
  Standard_Integer Index2OfCurve = 
    DStr.AddCurve(TopOpeBRepDS_Curve(Crv3d2,tolC2));
  if(Gd2){
    TopoDS_Face forwfac = BS2->ChangeSurface().Face();
    forwfac.Orientation(TopAbs_FORWARD);
    brc.Initialize(Data->VertexFirstOnS2().Arc(),forwfac);
    ChFiDS_CommonPoint& V = Data->ChangeVertexFirstOnS2();
    CArc.Initialize(V.Arc());
    CompParam(brc,PCurveOnFace,uarc,utg, V.ParameterOnArc(), V.Parameter());
    tolcheck = CArc.Value(uarc).Distance(V.Point());
    V.SetArc(tolC2+tolcheck,V.Arc(),uarc,V.TransitionOnArc());
    pppdeb = utg;
  }
  else pppdeb = VFirst;
  if(Gf2){
    TopoDS_Face forwfac = BS2->ChangeSurface().Face();
    forwfac.Orientation(TopAbs_FORWARD);
    brc.Initialize(Data->VertexLastOnS2().Arc(),forwfac);
    ChFiDS_CommonPoint& V = Data->ChangeVertexLastOnS2();
    CArc.Initialize(V.Arc());
    CompParam(brc,PCurveOnFace,uarc,utg, V.ParameterOnArc(), V.Parameter());
    tolcheck = CArc.Value(uarc).Distance(V.Point());
    V.SetArc(tolC2+tolcheck,V.Arc(),uarc,V.TransitionOnArc());
    pppfin = utg;
  }
  else pppfin = VLast;
  ChFiDS_FaceInterference& Fint2 = Data->ChangeInterferenceOnS2();
  Fint2.SetFirstParameter(pppdeb);
  Fint2.SetLastParameter(pppfin);
  if(!S2.IsNull()){
    TopAbs_Orientation TraOn2;
    if(Reversed) TraOn2 = ChFi3d_TrsfTrans(lin->TransitionOnS1());
    else TraOn2 = ChFi3d_TrsfTrans(lin->TransitionOnS2());
    Fint2.SetInterference(Index2OfCurve,TraOn2,PCurveOnFace,PCurveOnSurf);
  }
  else {
    Handle(Geom2d_Curve) bidpc;
    Fint2.SetInterference
      (Index2OfCurve,TopAbs_FORWARD,bidpc,PCurveOnSurf);
  }

  // the orientation of the fillet in relation to the faces is evaluated,

  Handle(Adaptor3d_HSurface) Sref = S1;
  PCurveOnFace = Fint1.PCurveOnFace();
  if(Reversed){ Sref = S2; PCurveOnFace = Fint2.PCurveOnFace(); }
  
//  Modified by skv - Wed Jun  9 17:16:26 2004 OCC5898 Begin
//   gp_Pnt2d PUV = PCurveOnFace->Value((VFirst+VLast)/2.);
//   gp_Pnt P;
//   gp_Vec Du1,Du2,Dv1,Dv2;
//   Sref->D1(PUV.X(),PUV.Y(),P,Du1,Dv1);
//   Du1.Cross(Dv1);
//   if (Or1 == TopAbs_REVERSED) Du1.Reverse();
//   Surf->D1(UFirst,(VFirst+VLast)/2.,P,Du2,Dv2);
//   Du2.Cross(Dv2);
//   if (Du1.Dot(Du2)>0) Data->ChangeOrientation() = TopAbs_FORWARD;
//   else Data->ChangeOrientation() = TopAbs_REVERSED;

  Standard_Real    aDelta = VLast - VFirst;
  Standard_Integer aDenom = 2;

  for(;;) {
    Standard_Real aDeltav = aDelta/aDenom;
    Standard_Real aParam  = VFirst + aDeltav;
    gp_Pnt2d      PUV     = PCurveOnFace->Value(aParam);
    gp_Pnt        P;
    gp_Vec        Du1,Du2,Dv1,Dv2;

    Sref->D1(PUV.X(),PUV.Y(),P,Du1,Dv1);
    Du1.Cross(Dv1);

    if (Or1 == TopAbs_REVERSED)
      Du1.Reverse();

    Surf->D1(UFirst, aParam, P, Du2, Dv2);
    Du2.Cross(Dv2);

    if (Du1.Magnitude() <= tolget3d ||
	Du2.Magnitude() <= tolget3d) {
      aDenom++;

      if (Abs(aDeltav) <= tolget2d)
	return Standard_False;

      continue;
    }

    if (Du1.Dot(Du2)>0)
      Data->ChangeOrientation() = TopAbs_FORWARD;
    else
      Data->ChangeOrientation() = TopAbs_REVERSED;

    break;
  }
//  Modified by skv - Wed Jun  9 17:16:26 2004 OCC5898 End
  
  if(!Gd1 && !S1.IsNull())
    ChFi3d_FilCommonPoint(lin->StartPointOnFirst(),lin->TransitionOnS1(),
			  Standard_True, Data->ChangeVertex(1,ion1),tolC1);
  if(!Gf1 && !S1.IsNull())
    ChFi3d_FilCommonPoint(lin->EndPointOnFirst(),lin->TransitionOnS1(),
			  Standard_False,Data->ChangeVertex(0,ion1),tolC1);
  if(!Gd2 && !S2.IsNull())
    ChFi3d_FilCommonPoint(lin->StartPointOnSecond(),lin->TransitionOnS2(),
			  Standard_True, Data->ChangeVertex(1,ion2),tolC2);
  if(!Gf2 && !S2.IsNull())
    ChFi3d_FilCommonPoint(lin->EndPointOnSecond(),lin->TransitionOnS2(),
			  Standard_False, Data->ChangeVertex(0,ion2),tolC2);
  // Parameters on ElSpine
  Standard_Integer nbp = lin->NbPoints();
  Data->FirstSpineParam(lin->Point(1).Parameter());
  Data->LastSpineParam(lin->Point(nbp).Parameter());
  return Standard_True;
}			 



//=======================================================================
//function : ComputeData
//purpose  : Head of the path edge/face for the bypass of obstacle.
//=======================================================================

Standard_Boolean ChFi3d_Builder::ComputeData
(Handle(ChFiDS_SurfData)&         Data,
 const Handle(ChFiDS_HElSpine)&   HGuide,
 Handle(BRepBlend_Line)&          Lin,
 const Handle(Adaptor3d_HSurface)&  S1,
 const Handle(Adaptor3d_TopolTool)& I1,
 const Handle(Adaptor3d_HSurface)&  S2,
 const Handle(Adaptor2d_HCurve2d)&  PC2,
 const Handle(Adaptor3d_TopolTool)& I2,
 Standard_Boolean&                Decroch,
 Blend_SurfRstFunction&           Func,
 Blend_FuncInv&                   FInv,
 Blend_SurfPointFuncInv&          FInvP,
 Blend_SurfCurvFuncInv&           FInvC,
 const Standard_Real              PFirst,
 const Standard_Real              MaxStep,
 const Standard_Real              Fleche,
 const Standard_Real              TolGuide,
 Standard_Real&                   First,
 Standard_Real&                   Last,
 const math_Vector&               Soldep,
 const Standard_Boolean           Inside,
 const Standard_Boolean           Appro,
 const Standard_Boolean           Forward,
 const Standard_Boolean           RecP,
 const Standard_Boolean           RecS,
 const Standard_Boolean           RecRst)
{
  BRepBlend_SurfRstLineBuilder TheWalk(S1,I1,S2,PC2,I2);
  
  Data->FirstExtensionValue(0);
  Data->LastExtensionValue(0); 

  Standard_Boolean reverse = (!Forward || Inside);
  Standard_Real SpFirst = HGuide->FirstParameter();
  Standard_Real SpLast =  HGuide->LastParameter();
  Standard_Real Target = SpLast;
  if(reverse) Target = SpFirst;
  Standard_Real Targetsov = Target;
  
  Standard_Real MS = MaxStep;
  Standard_Integer again = 0;
  Standard_Integer nbptmin = 3; //jlr
  Standard_Integer Nbpnt = 1;
  // the initial solution is reframed if necessary.
  math_Vector ParSol(1,3);
  Standard_Real NewFirst = PFirst;
  if(RecP || RecS || RecRst){
    if(!TheWalk.PerformFirstSection(Func,FInv,FInvP,FInvC,PFirst,Target,Soldep,
				    tolesp,TolGuide,RecRst,RecP,RecS,
				    NewFirst,ParSol)){
#ifdef OCCT_DEBUG
      cout<<"ChFi3d_Builder::ComputeData : calculation fail first section"<<endl;
#endif
      return Standard_False;
    }
  }
  else {
    ParSol = Soldep;
  }

  while (again < 2){
    TheWalk.Perform (Func,FInv,FInvP,FInvC,NewFirst,Last,
		     MS,TolGuide,ParSol,tolesp,Fleche,Appro);

    if (!TheWalk.IsDone()) {
#ifdef OCCT_DEBUG
      cout << "Path not created" << endl;
#endif  
      return Standard_False;
    }
  
    if (reverse) {
      if (!TheWalk.Complete(Func,FInv,FInvP,FInvC,SpLast)) {
#ifdef OCCT_DEBUG
	cout << "Not completed" << endl;
#endif
      }
    }  
  

    Lin = TheWalk.Line();
    Nbpnt = Lin->NbPoints();
    if (Nbpnt <= 1 && again == 0)  {
      again++;
#ifdef OCCT_DEBUG
      cout <<"one point of the path MS/50 is attempted."<<endl;
#endif  
      MS = MS/50.; Target = Targetsov;
    }
    else if (Nbpnt<=nbptmin && again == 0)  {
      again++;
#ifdef OCCT_DEBUG
      cout <<"Number of points is too small, the step is reduced"<<endl;
#endif  
      Standard_Real u1 = Lin->Point(1).Parameter();
      Standard_Real u2 = Lin->Point(Nbpnt).Parameter();
      MS = (u2-u1)/(nbptmin+1.0);
//      cout << " MS : " << MS << " u1 : " << u1 << " u2 : " << u2 << " nbptmin : " << nbptmin << endl;
      Target = Targetsov;
    }
    else if(Nbpnt<=nbptmin){
#ifdef OCCT_DEBUG
      cout <<"Number of points is still too small, quit"<<endl;
#endif  
      return Standard_False;
    }
    else {
      again = 2;
    }
  }
#ifdef DRAW
  ChFi3d_SettraceDRAWWALK(Standard_True);
  if(ChFi3d_GettraceDRAWWALK()) drawline(Lin,Standard_True);
#endif  
  if(Forward) Decroch = TheWalk.DecrochEnd();
  else Decroch = TheWalk.DecrochStart();
  Last = Lin->Point(Nbpnt).Parameter();
  First = Lin->Point(1).Parameter();
  return Standard_True;
}


//=======================================================================
//function : ComputeData
//purpose  : Heading of the path edge/edge for the bypass of obstacle.
//=======================================================================

Standard_Boolean ChFi3d_Builder::ComputeData
(Handle(ChFiDS_SurfData)&         Data,
 const Handle(ChFiDS_HElSpine)&   HGuide,
 Handle(BRepBlend_Line)&          Lin,
 const Handle(Adaptor3d_HSurface)&  S1,
 const Handle(Adaptor2d_HCurve2d)&  PC1,
 const Handle(Adaptor3d_TopolTool)& I1,
 Standard_Boolean&                Decroch1,
 const Handle(Adaptor3d_HSurface)&  S2,
 const Handle(Adaptor2d_HCurve2d)&  PC2,
 const Handle(Adaptor3d_TopolTool)& I2,
 Standard_Boolean&                Decroch2,
 Blend_RstRstFunction&            Func,
 Blend_SurfCurvFuncInv&           FInv1,
 Blend_CurvPointFuncInv&          FInvP1,
 Blend_SurfCurvFuncInv&           FInv2,
 Blend_CurvPointFuncInv&          FInvP2,
 const Standard_Real              PFirst,
 const Standard_Real              MaxStep,
 const Standard_Real              Fleche,
 const Standard_Real              TolGuide,
 Standard_Real&                   First,
 Standard_Real&                   Last,
 const math_Vector&               Soldep,
 const Standard_Boolean           Inside,
 const Standard_Boolean           Appro,
 const Standard_Boolean           Forward,
 const Standard_Boolean           RecP1,
 const Standard_Boolean           RecRst1,
 const Standard_Boolean           RecP2,
 const Standard_Boolean           RecRst2)
{
  BRepBlend_RstRstLineBuilder TheWalk(S1, PC1, I1, S2, PC2, I2);
  
  Data->FirstExtensionValue(0);
  Data->LastExtensionValue(0); 

  Standard_Boolean reverse = (!Forward || Inside);
  Standard_Real SpFirst = HGuide->FirstParameter();
  Standard_Real SpLast =  HGuide->LastParameter();
  Standard_Real Target = SpLast;
  if(reverse) Target = SpFirst;
  Standard_Real Targetsov = Target;
  
  Standard_Real MS = MaxStep;
  Standard_Integer again = 0;
  Standard_Integer nbptmin = 3; //jlr
  Standard_Integer Nbpnt = 0;
  // the initial solution is reframed if necessary.
  math_Vector ParSol(1,2);
  Standard_Real NewFirst = PFirst;
  if (RecP1 || RecRst1 || RecP2 || RecRst2) {
    if (!TheWalk.PerformFirstSection(Func, FInv1, FInvP1, FInv2, FInvP2, PFirst, Target, Soldep,
	  			     tolesp, TolGuide, RecRst1, RecP1, RecRst2, RecP2,
				     NewFirst, ParSol)){
#ifdef OCCT_DEBUG
      cout<<"ChFi3d_Builder::ComputeData : fail calculation first section"<<endl;
#endif
      return Standard_False;
    }
  }
  else {
    ParSol = Soldep;
  }

  while (again < 2){
    TheWalk.Perform (Func, FInv1, FInvP1, FInv2, FInvP2, NewFirst, Last,
		     MS, TolGuide, ParSol, tolesp, Fleche, Appro);

    if (!TheWalk.IsDone()) {
#ifdef OCCT_DEBUG
      cout << "Path not done" << endl;
#endif  
      return Standard_False;
    }
  
    if (reverse) {
      if (!TheWalk.Complete(Func, FInv1, FInvP1, FInv2, FInvP2, SpLast)) {
#ifdef OCCT_DEBUG
	cout << "Not completed" << endl;
#endif
      }
    }  
  

    Lin = TheWalk.Line();
    Nbpnt = Lin->NbPoints();
    if (Nbpnt <= 1 && again == 0)  {
      again++;
#ifdef OCCT_DEBUG
      cout <<"one point of path MS/50 is attempted."<<endl;
#endif  
      MS = MS/50.; Target = Targetsov;
    }
    else if (Nbpnt<=nbptmin && again == 0)  {
      again++;
#ifdef OCCT_DEBUG
      cout <<"Number of points is too small, the step is reduced"<<endl;
#endif  
      Standard_Real u1 = Lin->Point(1).Parameter();
      Standard_Real u2 = Lin->Point(Nbpnt).Parameter();
      MS = (u2-u1)/(nbptmin+1);
      Target = Targetsov;
    }
    else if(Nbpnt<=nbptmin){
#ifdef OCCT_DEBUG
      cout <<"Number of points is still too small, quit"<<endl;
#endif  
      return Standard_False;
    }
    else {
      again = 2;
    }
  }
#ifdef DRAW
  ChFi3d_SettraceDRAWWALK(Standard_True);
  if(ChFi3d_GettraceDRAWWALK()) drawline(Lin,Standard_True);
#endif  
  if (Forward) {
    Decroch1 = TheWalk.Decroch1End();
    Decroch2 = TheWalk.Decroch2End();
  }
  else {
    Decroch1 = TheWalk.Decroch1Start();
    Decroch2 = TheWalk.Decroch2Start();  
  }
  Last  = Lin->Point(Nbpnt).Parameter();
  First = Lin->Point(1).Parameter();
  return Standard_True;
}


//=======================================================================
//function : SimulData
//purpose  : Heading of the path edge/face for the bypass of obstacle in simulation mode.
//=======================================================================

Standard_Boolean ChFi3d_Builder::SimulData
(Handle(ChFiDS_SurfData)&         /*Data*/,
 const Handle(ChFiDS_HElSpine)&   HGuide,
 Handle(BRepBlend_Line)&          Lin,
 const Handle(Adaptor3d_HSurface)&  S1,
 const Handle(Adaptor3d_TopolTool)& I1,
 const Handle(Adaptor3d_HSurface)&  S2,
 const Handle(Adaptor2d_HCurve2d)&  PC2,
 const Handle(Adaptor3d_TopolTool)& I2,
 Standard_Boolean&                Decroch,
 Blend_SurfRstFunction&           Func,
 Blend_FuncInv&                   FInv,
 Blend_SurfPointFuncInv&          FInvP,
 Blend_SurfCurvFuncInv&           FInvC,
 const Standard_Real              PFirst,
 const Standard_Real              MaxStep,
 const Standard_Real              Fleche,
 const Standard_Real              TolGuide,
 Standard_Real&                   First,
 Standard_Real&                   Last,
 const math_Vector&               Soldep,
 const Standard_Integer           NbSecMin,
 const Standard_Boolean           Inside,
 const Standard_Boolean           Appro,
 const Standard_Boolean           Forward,
 const Standard_Boolean           RecP,
 const Standard_Boolean           RecS,
 const Standard_Boolean           RecRst)
{
  BRepBlend_SurfRstLineBuilder TheWalk(S1,I1,S2,PC2,I2);

  Standard_Boolean reverse = (!Forward || Inside);
  Standard_Real SpFirst = HGuide->FirstParameter();
  Standard_Real SpLast =  HGuide->LastParameter();
  Standard_Real Target = SpLast;
  if(reverse) Target = SpFirst;
  Standard_Real Targetsov = Target;
  
  Standard_Real MS = MaxStep;
  Standard_Integer again = 0;
  Standard_Integer Nbpnt = 0; 
  // the starting solution is reframed if needed.
  math_Vector ParSol(1,3);
  Standard_Real NewFirst = PFirst;
  if(RecP || RecS || RecRst){
    if(!TheWalk.PerformFirstSection(Func,FInv,FInvP,FInvC,PFirst,Target,Soldep,
				    tolesp,TolGuide,RecRst,RecP,RecS,
				    NewFirst,ParSol)){
#ifdef OCCT_DEBUG

      cout<<"ChFi3d_Builder::SimulData : fail calculate first section"<<endl;
#endif
      return Standard_False;
    }
  }
  else {
    ParSol = Soldep;
  }

  while (again < 2){
    TheWalk.Perform (Func,FInv,FInvP,FInvC,NewFirst,Last,
		     MS,TolGuide,ParSol,tolesp,Fleche,Appro);
    if (!TheWalk.IsDone()) {
#ifdef OCCT_DEBUG
      cout << "Path not done" << endl;
#endif
      return Standard_False;
    }
    if (reverse) {
      if (!TheWalk.Complete(Func,FInv,FInvP,FInvC,SpLast)) {
#ifdef OCCT_DEBUG
	cout << "Not completed" << endl;
#endif
      }
    }  
    Lin = TheWalk.Line();
    Nbpnt = Lin->NbPoints();
    if (Nbpnt <= 1 && again == 0)  {
      again++;
#ifdef OCCT_DEBUG
      cout <<"one point of path MS/50 is attempted."<<endl;
#endif
      MS = MS/50.; Target = Targetsov;
    }
    else if (Nbpnt <= NbSecMin && again == 0)  {
      again++;
#ifdef OCCT_DEBUG
      cout <<"Number of points is too small, the step is reduced"<<endl;
#endif
      Standard_Real u1 = Lin->Point(1).Parameter();
      Standard_Real u2 = Lin->Point(Nbpnt).Parameter();
      MS = (u2-u1)/(NbSecMin+1);
      Target = Targetsov;
    }
    else if(Nbpnt<=NbSecMin){
#ifdef OCCT_DEBUG
      cout <<"Number of points is still too small, quit"<<endl;
#endif
      return Standard_False;
    }
    else {
      again = 2;
    }
  }
#ifdef DRAW
  ChFi3d_SettraceDRAWWALK(Standard_True);
  if(ChFi3d_GettraceDRAWWALK()) drawline(Lin,Standard_True);
#endif  
  if(Forward) Decroch = TheWalk.DecrochEnd();
  else Decroch = TheWalk.DecrochStart();
  Last = Lin->Point(Nbpnt).Parameter();
  First = Lin->Point(1).Parameter();
  return Standard_True;
}


//=======================================================================
//function : SimulData
//purpose  : Heading of path edge/edge for the bypass
//           of obstacle in simulation mode.
//=======================================================================

Standard_Boolean ChFi3d_Builder::SimulData
(Handle(ChFiDS_SurfData)&         /*Data*/,
 const Handle(ChFiDS_HElSpine)&   HGuide,
 Handle(BRepBlend_Line)&          Lin,
 const Handle(Adaptor3d_HSurface)&  S1,
 const Handle(Adaptor2d_HCurve2d)&  PC1,
 const Handle(Adaptor3d_TopolTool)& I1,
 Standard_Boolean&                Decroch1,
 const Handle(Adaptor3d_HSurface)&  S2,
 const Handle(Adaptor2d_HCurve2d)&  PC2,
 const Handle(Adaptor3d_TopolTool)& I2,
 Standard_Boolean&                Decroch2,
 Blend_RstRstFunction&            Func,
 Blend_SurfCurvFuncInv&           FInv1,
 Blend_CurvPointFuncInv&          FInvP1,
 Blend_SurfCurvFuncInv&           FInv2,
 Blend_CurvPointFuncInv&          FInvP2,
 const Standard_Real              PFirst,
 const Standard_Real              MaxStep,
 const Standard_Real              Fleche,
 const Standard_Real              TolGuide,
 Standard_Real&                   First,
 Standard_Real&                   Last,
 const math_Vector&               Soldep,
 const Standard_Integer           NbSecMin,
 const Standard_Boolean           Inside,
 const Standard_Boolean           Appro,
 const Standard_Boolean           Forward,
 const Standard_Boolean           RecP1,
 const Standard_Boolean           RecRst1,
 const Standard_Boolean           RecP2,
 const Standard_Boolean           RecRst2)
{
  BRepBlend_RstRstLineBuilder TheWalk(S1, PC1, I1, S2, PC2, I2);

  Standard_Boolean reverse = (!Forward || Inside);
  Standard_Real SpFirst = HGuide->FirstParameter();
  Standard_Real SpLast =  HGuide->LastParameter();
  Standard_Real Target = SpLast;
  if(reverse) Target = SpFirst;
  Standard_Real Targetsov = Target;
  
  Standard_Real MS = MaxStep;
  Standard_Integer again = 0;
  Standard_Integer Nbpnt = 0; 
  // The initial solution is reframed if necessary.
  math_Vector ParSol(1,2);
  Standard_Real NewFirst = PFirst;
  if (RecP1 || RecRst1 || RecP2 || RecRst2) {
    if(!TheWalk.PerformFirstSection(Func, FInv1, FInvP1, FInv2, FInvP2, PFirst, Target, Soldep,
	  			    tolesp, TolGuide, RecRst1, RecP1, RecRst2, RecP2,
				    NewFirst,ParSol)){
#ifdef OCCT_DEBUG

      cout<<"ChFi3d_Builder::SimulData : calculation fail first section"<<endl;
#endif
      return Standard_False;
    }
  }
  else {
    ParSol = Soldep;
  }

  while (again < 2){
    TheWalk.Perform (Func, FInv1, FInvP1, FInv2, FInvP2, NewFirst, Last,
		     MS, TolGuide, ParSol, tolesp, Fleche, Appro);
    if (!TheWalk.IsDone()) {
#ifdef OCCT_DEBUG
      cout << "Path not created" << endl;
#endif
      return Standard_False;
    }
    if (reverse) {
      if (!TheWalk.Complete(Func, FInv1, FInvP1, FInv2, FInvP2, SpLast)) {
#ifdef OCCT_DEBUG
	cout << "Not completed" << endl;
#endif
      }
    }  
    Lin = TheWalk.Line();
    Nbpnt = Lin->NbPoints();
    if (Nbpnt <= 1 && again == 0)  {
      again++;
#ifdef OCCT_DEBUG
      cout <<"only one point of path MS/50 is attempted."<<endl;
#endif
      MS = MS/50.; Target = Targetsov;
    }
    else if (Nbpnt <= NbSecMin && again == 0)  {
      again++;
#ifdef OCCT_DEBUG
      cout <<"Number of points is too small, the step is reduced"<<endl;
#endif
      Standard_Real u1 = Lin->Point(1).Parameter();
      Standard_Real u2 = Lin->Point(Nbpnt).Parameter();
      MS = (u2-u1)/(NbSecMin+1);
      Target = Targetsov;
    }
    else if(Nbpnt<=NbSecMin){
#ifdef OCCT_DEBUG
      cout <<"Number of points is still too small, quit"<<endl;
#endif
      return Standard_False;
    }
    else {
      again = 2;
    }
  }
#ifdef DRAW
  if(ChFi3d_GettraceDRAWWALK()) drawline(Lin,Standard_True);
#endif
  if (Forward) {
    Decroch1 = TheWalk.Decroch1End();
    Decroch2 = TheWalk.Decroch2End();
  }
  else {
    Decroch1 = TheWalk.Decroch1Start();
    Decroch2 = TheWalk.Decroch2Start();  
  }  

  Last = Lin->Point(Nbpnt).Parameter();
  First = Lin->Point(1).Parameter();
  return Standard_True;
}




//=======================================================================
//function : ComputeData
//purpose  : Construction of elementary fillet by path.
//
//=======================================================================

Standard_Boolean ChFi3d_Builder::ComputeData
(Handle(ChFiDS_SurfData)& Data,
 const Handle(ChFiDS_HElSpine)& HGuide,
 const Handle(ChFiDS_Spine)& Spine,
 Handle(BRepBlend_Line)& Lin,
 const Handle(Adaptor3d_HSurface)& S1,
 const Handle(Adaptor3d_TopolTool)& I1,
 const Handle(Adaptor3d_HSurface)& S2,
 const Handle(Adaptor3d_TopolTool)& I2,
 Blend_Function& Func,
 Blend_FuncInv& FInv,
 const Standard_Real PFirst,
 const Standard_Real MaxStep,
 const Standard_Real Fleche,
 const Standard_Real tolguide,
 Standard_Real& First,
 Standard_Real& Last,
 const Standard_Boolean Inside,
 const Standard_Boolean Appro,
 const Standard_Boolean Forward,
 const math_Vector& Soldep,
 Standard_Integer& intf,
 Standard_Integer& intl,
 Standard_Boolean& Gd1,
 Standard_Boolean& Gd2,
 Standard_Boolean& Gf1,
 Standard_Boolean& Gf2,
 const Standard_Boolean RecOnS1,
 const Standard_Boolean RecOnS2)
{
  //The extrensions are created in case of output of two domains
  //directly and not by path ( too hasardous ).
  Data->FirstExtensionValue(0);
  Data-> LastExtensionValue(0);

  //The eventual faces are restored to test the jump of edge.
  TopoDS_Face F1, F2;
  Handle(BRepAdaptor_HSurface) HS = Handle(BRepAdaptor_HSurface)::DownCast(S1); 
  if(!HS.IsNull()) F1 = HS->ChangeSurface().Face();
  HS = Handle(BRepAdaptor_HSurface)::DownCast(S2); 
  if(!HS.IsNull()) F2 = HS->ChangeSurface().Face();
  
  // Path framing variables
  Standard_Real TolGuide=tolguide, TolEsp = tolesp;
  Standard_Integer nbptmin = 4;

  BRepBlend_Walking TheWalk(S1,S2,I1,I2,HGuide);

  //Start of removal, 2D path controls 
  //that qui s'accomodent mal des surfaces a parametrages non homogenes
  //en u et en v are extinguished.
  TheWalk.Check2d(0);
  
  Standard_Real MS = MaxStep;
  Standard_Integer Nbpnt;
  Standard_Real SpFirst = HGuide->FirstParameter();
  Standard_Real SpLast =  HGuide->LastParameter();

  // When the start point is inside, the path goes first to the left  
  // to determine the Last for the periodicals.
  Standard_Boolean reverse = (!Forward || Inside);
  Standard_Real Target;
  if(reverse){
    Target = SpFirst;
    if(!intf) Target = Last;
  }
  else{
    Target = SpLast + Abs(SpLast);
    if(!intl) Target = Last;
  }

  // In case if the singularity is pre-determined,
  // the path is indicated.
  if (!Spine.IsNull()){
    if (Spine->IsTangencyExtremity(Standard_True)) {
      TopoDS_Vertex V = Spine->FirstVertex();
      TopoDS_Edge E = Spine->Edges(1);
      Standard_Real param =  Spine->FirstParameter();
      Blend_Point BP;
      if (CompBlendPoint(V, E, param, F1, F2, BP)) {
	math_Vector vec(1,4);
	BP.ParametersOnS1(vec(1),vec(2));
	BP.ParametersOnS2(vec(3),vec(4));
	Func.Set(param);
	if (Func.IsSolution(vec, tolesp)) {
	  TheWalk.AddSingularPoint(BP);
	}
      }
    }
    if (Spine->IsTangencyExtremity(Standard_False)) {
      TopoDS_Vertex V = Spine->LastVertex();
      TopoDS_Edge E = Spine->Edges( Spine->NbEdges()); 
      Standard_Real param =  Spine->LastParameter();
      Blend_Point BP;
      if (CompBlendPoint(V, E, param, F1, F2, BP)) {
	math_Vector vec(1,4);
	BP.ParametersOnS1(vec(1),vec(2));
	BP.ParametersOnS2(vec(3),vec(4));
	Func.Set(param);
	if (Func.IsSolution(vec, tolesp)) {
	  TheWalk.AddSingularPoint(BP);
	}
      }
    }
  }

  //The starting solution is reframed if necessary.
  //**********************************************//
  math_Vector ParSol(1,4);
  Standard_Real NewFirst = PFirst;
  if(RecOnS1 || RecOnS2){
    if(!TheWalk.PerformFirstSection(Func,FInv,PFirst,Target,Soldep,
				    tolesp,TolGuide,RecOnS1,RecOnS2,
				    NewFirst,ParSol)){
#ifdef OCCT_DEBUG
      cout<<"ChFi3d_Builder::ComputeData : calculation fail first section"<<endl;
#endif
      return Standard_False;
    }
  }
  else {
    ParSol = Soldep;
  }

  //First the valid part is calculate, without caring for the extensions.
  //******************************************************************//
  Standard_Integer again = 0;
  Standard_Boolean tchernobyl = 0;
  Standard_Real u1sov = 0., u2sov = 0.;
  TopoDS_Face bif;
  //Max step is relevant, but too great, the vector is required to detect
  //the twists.
  if( (Abs(Last-First) <= MS * 5.) &&
      (Abs(Last-First) >= 0.01*Abs(NewFirst-Target)) ){ 
    MS = Abs(Last-First)*0.2; 
  }

  while(again < 3){
    //Path. 
    if(!again && (MS < 5*TolGuide)) MS = 5*TolGuide;
    else {
      if (5*TolGuide > MS) TolGuide = MS/5;
      if (5*TolEsp > MS) TolEsp = MS/5;
    }
    TheWalk.Perform(Func,FInv,NewFirst,Target,MS,TolGuide,
		    ParSol,TolEsp,Fleche,Appro);
    if (!TheWalk.IsDone()) {
#ifdef OCCT_DEBUG
      cout << "Path is not created" << endl;
#endif
      return Standard_False;
    }
    Lin = TheWalk.Line();
    if(HGuide->IsPeriodic() && Inside) {
      SpFirst = Lin->Point(1).Parameter();
      SpLast  = SpFirst + HGuide->Period();
      HGuide->ChangeCurve().FirstParameter(SpFirst);
      HGuide->ChangeCurve().LastParameter (SpLast );
      HGuide->ChangeCurve().SetOrigin(SpFirst);
    }
    Standard_Boolean complmnt = Standard_True;
    if (Inside)  complmnt = TheWalk.Complete(Func,FInv,SpLast);
    if(!complmnt){
#ifdef OCCT_DEBUG
      cout << "Not completed" << endl;
#endif
      return Standard_False;
    }
    
    //The result is controlled using two criterions :
    //- if there is enough points,
    //- if one has gone far enough.
    Nbpnt = Lin->NbPoints();
    if (Nbpnt == 0){
#ifdef OCCT_DEBUG
      cout <<"0 point of path, quit."<<endl;
#endif
      return Standard_False;
    }
    Standard_Real fpointpar = Lin->Point(1).Parameter();
    Standard_Real lpointpar = Lin->Point(Nbpnt).Parameter();
    
    Standard_Real factor =  1./(nbptmin + 1);
    Standard_Boolean okdeb = (Forward && !Inside);  
    Standard_Boolean okfin = (!Forward && !Inside);
    if(!okdeb){
      Standard_Integer narc1 = Lin->StartPointOnFirst().NbPointOnRst();
      Standard_Integer narc2 = Lin->StartPointOnSecond().NbPointOnRst();
      okdeb = (narc1 > 0 || narc2 > 0 || (fpointpar-First) < 10*TolGuide); 
    }
    if(!okfin){
      Standard_Integer narc1 = Lin->EndPointOnFirst().NbPointOnRst();
      Standard_Integer narc2 = Lin->EndPointOnSecond().NbPointOnRst();
      okfin = (narc1 > 0 || narc2 > 0 || (Last-lpointpar) < 10*TolGuide);
    }
    if(!okdeb || !okfin || Nbpnt == 1){
      //It drags, the controls are extended, it is  expected to evaluate a
      //satisfactory maximum step. If it already done, quit.
      if(tchernobyl){
#ifdef OCCT_DEBUG
	cout <<"If it drags without control, quit."<<endl;
#endif
	return Standard_False;
      }
      tchernobyl = Standard_True;
      TheWalk.Check(0);
      if (Nbpnt == 1){
#ifdef OCCT_DEBUG
	cout <<"only one point of path MS/100 is attempted"<<endl;
	cout <<"and the controls are extended."<<endl;
#endif
	MS *= 0.01;
      }
      else{
#ifdef OCCT_DEBUG
	cout <<"It drags, the controls are extended."<<endl;
#endif
	MS = (lpointpar-fpointpar)/Nbpnt; //EvalStep(Lin);
      }
    }
    else if (Nbpnt < nbptmin){
      if(again == 0){
#ifdef OCCT_DEBUG
	cout <<"Number of points is too small, the step is reduced"<<endl;
#endif
	u1sov = fpointpar;
	u2sov = lpointpar;
	MS = (lpointpar - fpointpar) * factor;
      }
      else if(again == 1){
	if(Abs(fpointpar-u1sov)>=TolGuide || 
	   Abs(lpointpar-u2sov)>=TolGuide){
#ifdef OCCT_DEBUG
	  cout <<"Number of points is still too small, the step is reduced"<<endl;
#endif  
	  MS = (lpointpar - fpointpar) * factor;
	}
	else{
#ifdef OCCT_DEBUG
	  cout <<"Number of points is still too small, quit"<<endl;
#endif  
	  return Standard_False;
	}
      }
      again++;
    }
    else {
      again = 3;
    }
  }

  if(TheWalk.TwistOnS1()){
    Data->TwistOnS1(Standard_True);
#ifdef OCCT_DEBUG
    cout<<"Path completed, but TWIST on S1"<<endl;
#endif
  }
  if(TheWalk.TwistOnS2()){
    Data->TwistOnS2(Standard_True);
#ifdef OCCT_DEBUG
    cout<<"Parh completed, but TWIST on S2"<<endl;
#endif
  }


  //Here there is a more or less presentable result 
  //however it covers a the minimum zone.
  //The extensions are targeted.
  //*****************************//

  Gd1 = Gd2 = Gf1 = Gf2 = Standard_False;
  
  Standard_Boolean unseulsuffitdeb = (intf >= 2);
  Standard_Boolean unseulsuffitfin = (intl >= 2);
  Standard_Boolean noproldeb = (intf >= 3);
  Standard_Boolean noprolfin = (intl >= 3);

  Standard_Real Rab = 0.03*(SpLast-SpFirst);

  Standard_Boolean debarc1 = 0, debarc2 = 0;
  Standard_Boolean debcas1 = 0, debcas2 = 0;
  Standard_Boolean debobst1 = 0, debobst2 = 0;

  Standard_Boolean finarc1 = 0, finarc2 = 0;
  Standard_Boolean fincas1 = 0, fincas2 = 0;
  Standard_Boolean finobst1 = 0, finobst2 = 0;

  Standard_Integer narc1, narc2;

  Standard_Boolean backwContinueFailed = Standard_False; // eap
  if(reverse && intf) {
    narc1 = Lin->StartPointOnFirst().NbPointOnRst();
    narc2 = Lin->StartPointOnSecond().NbPointOnRst();
    if(narc1 != 0) {
      ChFi3d_FilCommonPoint(Lin->StartPointOnFirst(),Lin->TransitionOnS1(),
			    Standard_True, Data->ChangeVertexFirstOnS1(),tolesp);
      debarc1 = Standard_True;
      if(!SearchFace(Spine,Data->VertexFirstOnS1(),F1,bif)){
	//It is checked if there is not an obstacle.
	debcas1 = Standard_True;
	if(!Spine.IsNull()){
	  if(Spine->IsPeriodic()){
	    debobst1 = 1;
	  }
	  else{
	    debobst1 = IsObst(Data->VertexFirstOnS1(),
			      Spine->FirstVertex(),myVEMap);
	  }
	}
      }
    }
    if(narc2 != 0){
      ChFi3d_FilCommonPoint(Lin->StartPointOnSecond(),Lin->TransitionOnS2(),
			    Standard_True, Data->ChangeVertexFirstOnS2(),tolesp);
      debarc2 = Standard_True;
      if(!SearchFace(Spine,Data->VertexFirstOnS2(),F2,bif)){
	//It is checked if it is not an obstacle.
	debcas2 = Standard_True;
	if(!Spine.IsNull()){
	  if(Spine->IsPeriodic()){
	    debobst2 = 1;
	  }
	  else{
	    debobst2 = IsObst(Data->VertexFirstOnS2(),
			      Spine->FirstVertex(),myVEMap);
	  }
	}
      }
    }
    Standard_Boolean oncontinue = !noproldeb && (narc1 != 0 || narc2 != 0);
    if(debobst1 || debobst2) oncontinue = Standard_False;
    else if(debcas1 && debcas2) oncontinue = Standard_False;
    else if((!debcas1 && debarc1) || (!debcas2 && debarc2)) oncontinue = Standard_False;

    if(oncontinue) {
      TheWalk.ClassificationOnS1(!debarc1);
      TheWalk.ClassificationOnS2(!debarc2);
      TheWalk.Check2d(Standard_True); // It should be strict (PMN)
      TheWalk.Continu(Func,FInv,Target);
      TheWalk.ClassificationOnS1(Standard_True);
      TheWalk.ClassificationOnS2(Standard_True);
      TheWalk.Check2d(Standard_False);
      narc1 = Lin->StartPointOnFirst().NbPointOnRst();
      narc2 = Lin->StartPointOnSecond().NbPointOnRst();
//  modified by eap Fri Feb  8 11:43:48 2002 ___BEGIN___
      if(!debarc1) {
	if (narc1 == 0)
	  backwContinueFailed = Lin->StartPointOnFirst().ParameterOnGuide() > Target;
	else {
	  ChFi3d_FilCommonPoint(Lin->StartPointOnFirst(),Lin->TransitionOnS1(),
				Standard_True, Data->ChangeVertexFirstOnS1(),tolesp);
	  debarc1 = Standard_True;
	  if(!SearchFace(Spine,Data->VertexFirstOnS1(),F1,bif)){
	    //It is checked if it is not an obstacle.
	    debcas1 = Standard_True;
// 	    if(!Spine.IsNull()) {
// 	      if(Spine->IsPeriodic()){
// 	        debobst1 = 1;
// 	      }
// 	      else{
// 		debobst1 = IsObst(Data->VertexFirstOnS1(),
// 				  Spine->FirstVertex(),myVEMap);
// 	      }
// 	    }
	  }
	}
      }
      if(!debarc2) {
	if (narc2 == 0)
	  backwContinueFailed = Lin->StartPointOnSecond().ParameterOnGuide() > Target;
	else {
	  ChFi3d_FilCommonPoint(Lin->StartPointOnSecond(),Lin->TransitionOnS2(),
                                Standard_True, Data->ChangeVertexFirstOnS2(),tolesp);
          debarc2 = Standard_True;
          if(!SearchFace(Spine,Data->VertexFirstOnS2(),F2,bif)){
	    //It is checked if it is not an obstacle.
            debcas2 = Standard_True;
//             if(!Spine.IsNull()){
//               if(Spine->IsPeriodic()){
//                 debobst2 = 1;
//               }
//               else{
//                 debobst2 = IsObst(Data->VertexFirstOnS2(),
//                                   Spine->FirstVertex(),myVEMap);
//               }
//             }
          }
        }
      }
      if (backwContinueFailed) {
	// if we leave backwContinueFailed as is, we will stop in this direction
	// but we are to continue if there are no more faces on the side with arc
	// check this condition
	const ChFiDS_CommonPoint& aCP
	  = debarc1 ? Data->VertexFirstOnS1() : Data->VertexFirstOnS2();
	if (aCP.IsOnArc() && bif.IsNull())
	  backwContinueFailed = Standard_False;
      }
    }
  }
  Standard_Boolean forwContinueFailed = Standard_False;
//  modified by eap Fri Feb  8 11:44:11 2002 ___END___
  if(Forward && intl) {
    Target = SpLast;
    narc1 = Lin->EndPointOnFirst().NbPointOnRst();
    narc2 = Lin->EndPointOnSecond().NbPointOnRst();
    if(narc1 != 0){
      ChFi3d_FilCommonPoint(Lin->EndPointOnFirst(),Lin->TransitionOnS1(),
			    Standard_False, Data->ChangeVertexLastOnS1(),tolesp);
      finarc1 = Standard_True;
      if(!SearchFace(Spine,Data->VertexLastOnS1(),F1,bif)){
             //It is checked if it is not an obstacle.
	fincas1 = Standard_True;
	if(!Spine.IsNull()){
	  finobst1 = IsObst(Data->VertexLastOnS1(),
			    Spine->LastVertex(),myVEMap);
	}
      }
    }
    if(narc2 != 0){
      ChFi3d_FilCommonPoint(Lin->EndPointOnSecond(),Lin->TransitionOnS2(),
			    Standard_False, Data->ChangeVertexLastOnS2(),tolesp);
      finarc2 = Standard_True;
      if(!SearchFace(Spine,Data->VertexLastOnS2(),F2,bif)){
	 //It is checked if it is not an obstacle.
	fincas2 = Standard_True;
	if(!Spine.IsNull()){
	  finobst2 = IsObst(Data->VertexLastOnS2(),
			    Spine->LastVertex(),myVEMap);
	}
      }
    }
    Standard_Boolean oncontinue = !noprolfin && (narc1 != 0 || narc2 != 0);
    if(finobst1 || finobst2) oncontinue = Standard_False;
    else if(fincas1 && fincas2) oncontinue = Standard_False;
    else if((!fincas1 && finarc1) || (!fincas2 && finarc2)) oncontinue = Standard_False;
    
    if(oncontinue){
      TheWalk.ClassificationOnS1(!finarc1);
      TheWalk.ClassificationOnS2(!finarc2);
      TheWalk.Check2d(Standard_True); // It should be strict (PMN)
      TheWalk.Continu(Func,FInv,Target);
      TheWalk.ClassificationOnS1(Standard_True);
      TheWalk.ClassificationOnS2(Standard_True);
      TheWalk.Check2d(Standard_False);
      narc1 = Lin->EndPointOnFirst().NbPointOnRst();
      narc2 = Lin->EndPointOnSecond().NbPointOnRst();
//  modified by eap Fri Feb  8 11:44:57 2002 ___BEGIN___
      if(!finarc1) {
	if (narc1 == 0) 
	  forwContinueFailed = Lin->EndPointOnFirst().ParameterOnGuide() < Target;
	else {
	  ChFi3d_FilCommonPoint(Lin->EndPointOnFirst(),Lin->TransitionOnS1(),
				Standard_False, Data->ChangeVertexLastOnS1(),tolesp);
	  finarc1 = Standard_True;
	  if(!SearchFace(Spine,Data->VertexLastOnS1(),F1,bif)){
             //It is checked if it is not an obstacle.
	    fincas1 = Standard_True;
// 	    if(!Spine.IsNull()){
// 	      finobst1 = IsObst(Data->VertexLastOnS1(),
// 				Spine->LastVertex(),myVEMap);
// 	    }
	  }
	}
      }
      if(!finarc2) {
	if (narc2 == 0)
	  forwContinueFailed = Lin->EndPointOnSecond().ParameterOnGuide() < Target;
	else {
	  ChFi3d_FilCommonPoint(Lin->EndPointOnSecond(),Lin->TransitionOnS2(),
				Standard_False, Data->ChangeVertexLastOnS2(),tolesp);
	  finarc2 = Standard_True;
	  if(!SearchFace(Spine,Data->VertexLastOnS2(),F2,bif)){
	    //On regarde si ce n'est pas un obstacle.
	    fincas2 = Standard_True;
// 	    if(!Spine.IsNull()){
// 	      finobst2 = IsObst(Data->VertexLastOnS2(),
// 				Spine->LastVertex(),myVEMap);
// 	    }
	  }
	}
      }
      if (forwContinueFailed) {
	// if we leave forwContinueFailed as is, we will stop in this direction
	// but we are to continue if there are no more faces on the side with arc
	// check this condition
	const ChFiDS_CommonPoint& aCP
	  = finarc1 ? Data->VertexLastOnS1() : Data->VertexLastOnS2();
	if (aCP.IsOnArc() && bif.IsNull())
	  forwContinueFailed = Standard_False;
      }
//  modified by eap Fri Feb  8 11:45:10 2002 ___END___
    }
  }
  Nbpnt = Lin->NbPoints();
#ifdef DRAW
  if(ChFi3d_GettraceDRAWWALK()) drawline(Lin,Standard_False);
#endif  
  First = Lin->Point(1).Parameter();
  Last  = Lin->Point(Nbpnt).Parameter();

  // ============= INVALIDATION EVENTUELLE =============
  // ------ Preparation des prolongement par plan tangent -----
  if(reverse && intf){
    Gd1 = debcas1/* && !debobst1*/; // skv(occ67)
    Gd2 = debcas2/* && !debobst2*/; // skv(occ67)
    if ((debarc1^debarc2) && !unseulsuffitdeb && (First!=SpFirst)) {
      // Case of incomplete path, of course this ends badly : 
      // the result is truncated instead of exit.
      Standard_Real sortie;
      Standard_Integer ind;
      if (debarc1)  sortie = Data->VertexFirstOnS1().Parameter();
      else  sortie = Data->VertexFirstOnS2().Parameter();
      if (sortie - First > tolesp) {
	ind = SearchIndex(sortie, Lin);
	if (Lin->Point(ind).Parameter() == sortie) ind--;
	if (ind >= 1) {
	  Lin->Remove(1, ind);
	  UpdateLine(Lin, Standard_True);
	}
	Nbpnt = Lin->NbPoints();
	First = Lin->Point(1).Parameter();
      }
    }
    else if ((intf>=5) && !debarc1 && !debarc2 && (First!=SpFirst)) {
      Standard_Real sortie = (2*First+Last)/3;
      Standard_Integer ind;
      if (sortie - First > tolesp) {
	ind = SearchIndex(sortie, Lin);
	if (Lin->Point(ind).Parameter() == sortie) ind--;
        if (Nbpnt-ind < 3) ind = Nbpnt -3;
	if (ind >= 1) {
	  Lin->Remove(1, ind);
	  UpdateLine(Lin, Standard_True);
	}
	Nbpnt = Lin->NbPoints();
	First = Lin->Point(1).Parameter();
      }
    }
    if(Gd1 && Gd2){
      Target = Min((Lin->Point(1).Parameter() - Rab),First);
      Target = Max(Target,SpFirst);
      Data->FirstExtensionValue(Abs(Lin->Point(1).Parameter()-Target));
    }
    if (intf && !unseulsuffitdeb) intf = (Gd1 && Gd2)//;
      || backwContinueFailed; // eap
    else if (intf && unseulsuffitdeb && (intf<5)) {
      intf = (Gd1 || Gd2);
      // It is checked if there is no new face.
      if (intf && 
	  ((!debcas1 && debarc1) || (!debcas2 && debarc2)) ) intf = 0;  
    }
    else if (intf < 5) intf = 0;
  }

  if(Forward && intl){
    Gf1 = fincas1/* && !finobst1*/; // skv(occ67)
    Gf2 = fincas2/* && !finobst2*/; // skv(occ67)
    if ((finarc1 ^finarc2) && !unseulsuffitfin && (Last!=SpLast)) {
      // Case of incomplete path, of course, this ends badly : 
      // the result is truncated instead of exit.
      Standard_Real sortie;
      Standard_Integer ind;
      if (finarc1)  sortie = Data->VertexLastOnS1().Parameter();
      else  sortie = Data->VertexLastOnS2().Parameter();
      if (Last - sortie > tolesp) {
	ind = SearchIndex(sortie, Lin);
        if (Lin->Point(ind).Parameter() == sortie) ind++;
	if (ind<= Nbpnt) {
	  Lin->Remove(ind, Nbpnt);
	  UpdateLine(Lin, Standard_False);
	}
	Nbpnt = Lin->NbPoints();
	Last = Lin->Point(Nbpnt).Parameter();
      }
    }
    else if ((intl>=5) && !finarc1 && !finarc2 && (Last!=SpLast) ) {
      // The same in case when the entire "Lin" is an extension
      Standard_Real sortie = (First+2*Last)/3;
      Standard_Integer ind;
      if (Last - sortie > tolesp) {
	ind = SearchIndex(sortie, Lin);
	if (Lin->Point(ind).Parameter() == sortie) ind++;
        if (ind < 3) ind = 3;
	if (ind <= Nbpnt) {
	  Lin->Remove(ind, Nbpnt);
	  UpdateLine(Lin, Standard_False);
	}
	Nbpnt = Lin->NbPoints();
	Last = Lin->Point(Nbpnt).Parameter();
      }
    }
    if(Gf1 && Gf2) {
      Target = Max((Lin->Point(Nbpnt).Parameter() + Rab),Last);
      Target = Min(Target,SpLast);
      Data->LastExtensionValue(Abs(Target-Lin->Point(Nbpnt).Parameter()));
    }    

    if (intl && !unseulsuffitfin) intl = (Gf1 && Gf2)//;
      || forwContinueFailed;  // eap
    else if (intl && unseulsuffitfin && (intl<5)) {
      intl = (Gf1 || Gf2);// It is checked if there is no new face.
      if (intl && 
	  ((!fincas1 && finarc1) || (!fincas2 && finarc2)) ) intl = 0;  
    }
    else if (intl <5) intl = 0;
  }
  return Standard_True;
}

//=======================================================================
//function : SimulData
//purpose  : 
//=======================================================================

Standard_Boolean ChFi3d_Builder::SimulData
(Handle(ChFiDS_SurfData)& /*Data*/,
 const Handle(ChFiDS_HElSpine)& HGuide,
 Handle(BRepBlend_Line)& Lin,
 const Handle(Adaptor3d_HSurface)& S1,
 const Handle(Adaptor3d_TopolTool)& I1,
 const Handle(Adaptor3d_HSurface)& S2,
 const Handle(Adaptor3d_TopolTool)& I2,
 Blend_Function& Func,
 Blend_FuncInv& FInv,
 const Standard_Real PFirst,
 const Standard_Real MaxStep,
 const Standard_Real Fleche,
 const Standard_Real tolguide,
 Standard_Real& First,
 Standard_Real& Last,
 const Standard_Boolean Inside,
 const Standard_Boolean Appro,
 const Standard_Boolean Forward,
 const math_Vector& Soldep,
 const Standard_Integer NbSecMin,
 const Standard_Boolean RecOnS1,
 const Standard_Boolean RecOnS2)
{
  BRepBlend_Walking TheWalk(S1,S2,I1,I2,HGuide);
  TheWalk.Check2d(Standard_False);
  
  Standard_Real MS = MaxStep;
  Standard_Real TolGuide=tolguide, TolEsp = tolesp;
  Standard_Integer Nbpnt = 0;
  Standard_Real SpFirst = HGuide->FirstParameter();
  Standard_Real SpLast =  HGuide->LastParameter();
  Standard_Boolean reverse = (!Forward || Inside);
  Standard_Real Target;
  if(reverse){
    Target = SpFirst;
  }
  else{
    Target = SpLast;
  }

  Standard_Real Targetsov = Target;
  Standard_Real u1sov = 0., u2sov = 0.; 
  // on recadre la solution de depart a la demande.
  math_Vector ParSol(1,4);
  Standard_Real NewFirst = PFirst;
  if(RecOnS1 || RecOnS2){
    if(!TheWalk.PerformFirstSection(Func,FInv,PFirst,Target,Soldep,
				    tolesp,TolGuide,RecOnS1,RecOnS2,
				    NewFirst,ParSol)){
#ifdef OCCT_DEBUG
      cout<<"ChFi3d_Builder::SimulData : calculation fail first section"<<endl;
#endif
      return Standard_False;
    }
  }
  else {
    ParSol = Soldep;
  }
  Standard_Integer again = 0;
  while(again < 3){
     // When the start point is inside, the path goes first to the left  
     // to determine the Last for the periodicals.
    if(!again && (MS < 5*TolGuide)) MS = 5*TolGuide;
    else  {
      if (5*TolGuide > MS) TolGuide = MS/5;
      if (5*TolEsp > MS) TolEsp = MS/5;
    }
      
    TheWalk.Perform(Func,FInv,NewFirst,Target,MS,TolGuide,
		    ParSol,TolEsp,Fleche,Appro);
    
    if (!TheWalk.IsDone()) {
#ifdef OCCT_DEBUG
      cout << "Path not created" << endl;
#endif
      return Standard_False;
    }
    Lin = TheWalk.Line();
    if(reverse){
      if(HGuide->IsPeriodic()) {
	SpFirst = Lin->Point(1).Parameter();
	SpLast  = SpFirst + HGuide->Period();
	HGuide->ChangeCurve().FirstParameter(SpFirst);
	HGuide->ChangeCurve().LastParameter (SpLast );
      }
      Standard_Boolean complmnt = Standard_True;
      if (Inside)  complmnt = TheWalk.Complete(Func,FInv,SpLast);
      if(!complmnt){
#ifdef OCCT_DEBUG
	cout << "Not completed" << endl;
#endif
	return Standard_False;
      }
    }
    Nbpnt = Lin->NbPoints();
    Standard_Real factor =  1./(NbSecMin + 1);
    if (Nbpnt == 0){
#ifdef OCCT_DEBUG
      cout <<"0 point of path, quit."<<endl;
#endif
      return Standard_False;
    }
    else if (Nbpnt == 1 && again == 0)  {
      again++;
#ifdef OCCT_DEBUG
      cout <<"only one point of path, MS/100 is attempted."<<endl;
#endif
      MS *= 0.01; Target = Targetsov;
      u1sov = u2sov = Lin->Point(1).Parameter();
    }
    else if (Nbpnt< NbSecMin && again == 0)  {
      again++;
#ifdef OCCT_DEBUG
      cout <<"Number of points is too small, the step is reduced"<<endl;
#endif
      Standard_Real u1 = u1sov = Lin->Point(1).Parameter();
      Standard_Real u2 = u2sov = Lin->Point(Nbpnt).Parameter();
      MS = (u2-u1)*factor;
      Target = Targetsov;
    }
    else if (Nbpnt < NbSecMin && again == 1)  {
      Standard_Real u1 = Lin->Point(1).Parameter();
      Standard_Real u2 = Lin->Point(Nbpnt).Parameter();
      if(Abs(u1-u1sov)>=TolGuide || Abs(u2-u2sov)>=TolGuide){
	again++;
#ifdef OCCT_DEBUG
	cout <<"Number of points is still too small, the step is reduced"<<endl;
#endif
	MS /= 100;
	Target = Targetsov;
      }
      else{
#ifdef OCCT_DEBUG
	cout <<"Number of points is still too small, quit"<<endl;
#endif
	return Standard_False;
      }
    }
    else if(Nbpnt < NbSecMin){
#ifdef OCCT_DEBUG
      cout <<"Number of points is still too small, quit"<<endl;
#endif
      return Standard_False;
    }
    else {
      again = 3;
    }
  }
#ifdef DRAW
  if(ChFi3d_GettraceDRAWWALK()) drawline(Lin,Standard_False);
#endif  
  First = Lin->Point(1).Parameter();
  Last  = Lin->Point(Nbpnt).Parameter();
  return Standard_True;
}

