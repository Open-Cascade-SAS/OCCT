// Created on: 1998-11-26
// Created by: Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepLProp_CLProps.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <GeomLProp_SLProps.hxx>
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_IndexedMapOfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_C2DF.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>

#include <algorithm>
#define M_FORWARD(sta)  (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

#define FORWARD  (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING  (5)

static Standard_Boolean FUN_nullprodv(const Standard_Real prodv)
{
//  Standard_Real tola = Precision::Angular()*1.e+1; // NYI
  Standard_Real tola = 1.e-6; // NYI NYI NYI : for case cto 012 I2
  return (Abs(prodv) < tola);
}

//modified by NIZNHY-PKV Fri Aug  4 11:22:57 2000 from

//=======================================================================
//function : CheckEdgeLength
//purpose  : 
//=======================================================================
static Standard_Boolean CheckEdgeLength (const TopoDS_Edge& E)
{
  BRepAdaptor_Curve BC(E);

  TopTools_IndexedMapOfShape aM;
  TopExp::MapShapes(E, TopAbs_VERTEX, aM);
  Standard_Integer i, anExtent, aN=10;
  Standard_Real ln=0., d, t, f, l, dt; 
  anExtent=aM.Extent();

  if (anExtent!=1) 
    return Standard_True;
    
  gp_Pnt p1, p2;
  f = BC.FirstParameter();
  l = BC.LastParameter();
  dt=(l-f)/aN;
  
  BC.D0(f, p1);
  for (i=1; i<=aN; i++) {
    t=f+i*dt;
    
    if (i==aN) 
      BC.D0(l, p2);
      else 
	BC.D0(t, p2);
    
    d=p1.Distance(p2);
    ln+=d;
    p1=p2;
  }
  
  return (ln > Precision::Confusion()); 
}

//modified by NIZNHY-PKV Fri Aug  4 11:23:07 2000 to

//=======================================================================
//function : OriinSor
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_TOOL::OriinSor(const TopoDS_Shape& sub, const TopoDS_Shape& S, const Standard_Boolean checkclo)
{
  if (checkclo) {
    Standard_Boolean Sclosed = Standard_False;
    if      (S.ShapeType() == TopAbs_EDGE) {
      if (sub.ShapeType() != TopAbs_VERTEX) return 0;
      
      TopoDS_Vertex vclo; Sclosed = TopOpeBRepTool_TOOL::ClosedE(TopoDS::Edge(S),vclo);
      if (Sclosed) 
	if (sub.IsSame(vclo)) return CLOSING;
    }
    else if (S.ShapeType() == TopAbs_FACE) {
      if (sub.ShapeType() != TopAbs_EDGE) return 0;
      
      Sclosed = ClosedS(TopoDS::Face(S));
      if (Sclosed) 
	if (IsClosingE(TopoDS::Edge(sub),TopoDS::Face(S))) return CLOSING;
    } 
  }

  TopExp_Explorer ex(S,sub.ShapeType());
  for(; ex.More(); ex.Next()) {
    const TopoDS_Shape& ssub = ex.Current();
    Standard_Boolean same = ssub.IsSame(sub);
    if (!same) continue;
    TopAbs_Orientation osub = ssub.Orientation();
    if      (M_FORWARD(osub))  return FORWARD;
    else if (M_REVERSED(osub)) return REVERSED;
    else if (M_INTERNAL(osub)) return INTERNAL;
    else if (M_EXTERNAL(osub)) return EXTERNAL;
  }
  return 0;
}

//=======================================================================
//function : OriinSorclosed
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_TOOL::OriinSorclosed(const TopoDS_Shape& sub, const TopoDS_Shape& S)
{
  if (S.ShapeType() == TopAbs_EDGE)
    {if (sub.ShapeType() != TopAbs_VERTEX) return 0;}
  else if (S.ShapeType() == TopAbs_FACE) 
    {if (sub.ShapeType() != TopAbs_EDGE) return 0;}
  TopoDS_Iterator it(S);
  for(; it.More(); it.Next()) {
    const TopoDS_Shape& ssub = it.Value();
    Standard_Boolean equal = ssub.IsEqual(sub);
    if (!equal) continue;
    TopAbs_Orientation osub = ssub.Orientation();
    if      (M_FORWARD(osub))  return FORWARD;
    else if (M_REVERSED(osub)) return REVERSED;
  }
  return 0;
}



//=======================================================================
//function : ClosedE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::ClosedE(const TopoDS_Edge& E, TopoDS_Vertex& vclo)
{
   // returns true if <E> has a closing vertex <vclosing>
//  return E.IsClosed();
  Standard_Boolean isdgE = BRep_Tool::Degenerated(E);
  if (isdgE) return Standard_False;

  TopoDS_Shape vv; vclo.Nullify();
  TopExp_Explorer ex(E,TopAbs_VERTEX);
  for (; ex.More(); ex.Next()) {
    const TopoDS_Shape& v = ex.Current();
    if (M_INTERNAL(v.Orientation())) continue;
    if (vv.IsNull()) vv = v;
    else if (v.IsSame(vv))
      {vclo = TopoDS::Vertex(vv); return Standard_True;}
  }
  return Standard_False; 
}

//=======================================================================
//function : ClosedS
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::ClosedS(const TopoDS_Face& F)
{
  Handle(Geom_Surface) S =TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  if (S.IsNull()) return Standard_False;
  Standard_Boolean uclosed = S->IsUClosed(); if (uclosed) uclosed = S->IsUPeriodic();
  Standard_Boolean vclosed = S->IsVClosed(); if (vclosed) vclosed = S->IsVPeriodic(); 
  return (uclosed || vclosed);
}

//=======================================================================
//function : IsClosingE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::IsClosingE(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  Standard_Integer nbocc = 0;
  TopExp_Explorer exp(F,TopAbs_EDGE);
  for (;exp.More();exp.Next()) 
    if (exp.Current().IsSame(E)) nbocc++;
  if (nbocc != 2) return Standard_False;
  return BRep_Tool::IsClosed(E,F);  
}

//=======================================================================
//function : IsClosingE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::IsClosingE(const TopoDS_Edge& E, const TopoDS_Shape& W, const TopoDS_Face& F)
{
  Standard_Integer nbocc = 0;
  TopExp_Explorer exp(W,TopAbs_EDGE);
  for (;exp.More();exp.Next()) 
    if (exp.Current().IsSame(E)) nbocc++;
  if (nbocc != 2) return Standard_False;
  return BRep_Tool::IsClosed(E,F);  
}

//=======================================================================
//function : Vertices
//purpose  : 
//=======================================================================

void TopOpeBRepTool_TOOL::Vertices(const TopoDS_Edge& E, TopTools_Array1OfShape& Vces)
{
  // Returns vertices (F,R) if E is FORWARD
  //                  (R,V) if E is REVERSED
  TopAbs_Orientation oriE = E.Orientation();
  TopoDS_Vertex v1, v2; TopExp::Vertices(E,v1,v2);

  if (M_INTERNAL(oriE) || M_EXTERNAL(oriE)) 
    {Vces.ChangeValue(1)=v1;Vces.ChangeValue(2)=v2;}

  Standard_Real par1 = BRep_Tool::Parameter(v1,E);
  Standard_Real par2 = BRep_Tool::Parameter(v2,E);
#ifdef OCCT_DEBUG
//  if (par1>par2) cout<<"TopOpeBRepTool_TOOL::Vertices ERROR"<<endl;
#endif
  Standard_Integer ivparSMA = (par1<par2) ? FORWARD : REVERSED; 
  Standard_Integer ivparSUP = (par1<par2) ? REVERSED : FORWARD;
  if (M_REVERSED(oriE)) {
    ivparSMA = (ivparSMA == FORWARD) ? REVERSED : FORWARD;
    ivparSUP = (ivparSUP == REVERSED) ? FORWARD : REVERSED;
  }
  Vces.ChangeValue(ivparSMA) = v1;
  Vces.ChangeValue(ivparSUP) = v2;
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

TopoDS_Vertex TopOpeBRepTool_TOOL::Vertex(const Standard_Integer Iv, const TopoDS_Edge& E)
{  
  TopTools_Array1OfShape Vces(1,2); Vertices(E,Vces);
  TopoDS_Vertex V = TopoDS::Vertex(Vces(Iv));
  return V;  
}

//=======================================================================
//function : ParE
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_TOOL::ParE(const Standard_Integer Iv, const TopoDS_Edge& E)
{
  const TopoDS_Vertex& v = Vertex(Iv,E);
  return (BRep_Tool::Parameter(v,E));
}

//=======================================================================
//function : OnBoundary
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_TOOL::OnBoundary(const Standard_Real par, const TopoDS_Edge& e)
{
  BRepAdaptor_Curve bc(e);
  Standard_Boolean closed = bc.IsClosed();
  Standard_Real first = bc.FirstParameter();
  Standard_Real last = bc.LastParameter();
  Standard_Real tole = bc.Tolerance(); Standard_Real tolp = bc.Resolution(tole);
  
  Standard_Boolean onf = Abs(par-first)<tolp;
  Standard_Boolean onl = Abs(par-last)<tolp;
  Standard_Boolean onfl =  (onf || onl);
  if (onfl && closed) return CLOSING;
  if (onf) return FORWARD;
  if (onl) return REVERSED;
  if ((first < par)&&(par < last)) return INTERNAL;
  return EXTERNAL;
}



static void FUN_tool_sortVonE(TopTools_ListOfShape& lov, const TopoDS_Edge E)
{
  TopTools_DataMapOfIntegerShape mapiv;// mapiv.Find(iV) = V
  TColStd_IndexedMapOfReal mappar;     // mappar.FindIndex(parV) = iV
  
  for (TopTools_ListIteratorOfListOfShape itlove(lov); itlove.More(); itlove.Next()){
    const TopoDS_Vertex& v = TopoDS::Vertex(itlove.Value());
    Standard_Real par = BRep_Tool::Parameter(v,E);
    Standard_Integer iv = mappar.Add(par);
    mapiv.Bind(iv,v);
  }
  Standard_Integer nv = mapiv.Extent();
  NCollection_Array1<Standard_Real> tabpar(1,nv);
//  for (Standard_Integer i = 1; i <= nv; i++) {
  Standard_Integer i ;
  for ( i = 1; i <= nv; i++) {
    Standard_Real p = mappar.FindKey(i);
    tabpar.SetValue(i,p);
  }
  
  TopTools_ListOfShape newlov;
  std::sort (tabpar.begin(), tabpar.end());
  for (i = 1; i <= nv; i++) {
    Standard_Real par = tabpar.Value(i);
    Standard_Integer iv = mappar.FindIndex(par);
    const TopoDS_Shape& v = mapiv.Find(iv);
    newlov.Append(v);
  }
  lov.Clear(); lov.Append(newlov);
}

//=======================================================================
//function : SplitE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::SplitE(const TopoDS_Edge& Eanc, TopTools_ListOfShape& Splits)
{
  // prequesitory : <Eanc> is a valid edge.
  TopAbs_Orientation oEanc = Eanc.Orientation();
  TopoDS_Shape aLocalShape = Eanc.Oriented(TopAbs_FORWARD);
  TopoDS_Edge EFOR = TopoDS::Edge(aLocalShape);
//  TopoDS_Edge EFOR = TopoDS::Edge(Eanc.Oriented(TopAbs_FORWARD));
  TopTools_ListOfShape lov;
  TopExp_Explorer exv(EFOR,TopAbs_VERTEX);  
  for (;exv.More(); exv.Next()) {
    const TopoDS_Shape& v = exv.Current();
    lov.Append(v);
  }
  Standard_Integer nv = lov.Extent();
  if (nv <= 2) return Standard_False;

  ::FUN_tool_sortVonE(lov,EFOR);

  TopoDS_Vertex v0;
  TopTools_ListIteratorOfListOfShape itlov(lov);
  if (itlov.More()) {v0 = TopoDS::Vertex(itlov.Value()); itlov.Next();}
  else return Standard_False;

  for (; itlov.More(); itlov.Next()) {
    TopoDS_Vertex v = TopoDS::Vertex(itlov.Value());
    
    // prequesitory: par0 < par
    Standard_Real par0 = BRep_Tool::Parameter(v0, EFOR);
    Standard_Real par  = BRep_Tool::Parameter(v, EFOR);

    // here, ed has the same geometries than Ein, but with no subshapes.
    TopoDS_Edge ed; FUN_ds_CopyEdge(EFOR,ed);
    BRep_Builder BB;
    v0.Orientation(TopAbs_FORWARD); BB.Add(ed,v0); FUN_ds_Parameter(ed,v0,par0); 
    v.Orientation(TopAbs_REVERSED); BB.Add(ed,v);  FUN_ds_Parameter(ed,v,par); 
    Splits.Append(ed.Oriented(oEanc));
    v0 = v;
  }  
  return Standard_True;   
}




//=======================================================================
//function : UVF
//purpose  : 
//=======================================================================

gp_Pnt2d TopOpeBRepTool_TOOL::UVF(const Standard_Real par, const TopOpeBRepTool_C2DF& C2DF)
{
  Standard_Real f,l,tol; const Handle(Geom2d_Curve)& PC = C2DF.PC(f,l,tol);
  gp_Pnt2d UV; PC->D0(par,UV);
  return UV;
}

//=======================================================================
//function : ParISO
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::ParISO(const gp_Pnt2d& uv, const TopoDS_Edge& E, const TopoDS_Face& F, 
				Standard_Real& par)
{
  par = 1.e7;
  Standard_Boolean isou,isov; gp_Dir2d d2d; gp_Pnt2d o2d;
  Standard_Boolean uviso = TopOpeBRepTool_TOOL::UVISO(E,F, isou,isov, d2d,o2d);
  if (!uviso) return Standard_False;
  if (isou) {par = (uv.Y()-o2d.Y()); if (d2d.Y()<0) par = -par;}
  if (isov) {par = (uv.X()-o2d.X()); if (d2d.X()<0) par = -par;}
  return Standard_True;
}



//=======================================================================
//function : ParE2d
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::ParE2d(const gp_Pnt2d& p2d, const TopoDS_Edge& E, const TopoDS_Face& F, 
				Standard_Real& par, Standard_Real& dist)
{
  // Avoid projections if possible :
  BRepAdaptor_Curve2d BC2d(E,F);
  GeomAbs_CurveType CT = BC2d.GetType();
  const Handle(Geom2d_Curve)& C2d = BC2d.Curve();
  if (CT == GeomAbs_Line) {
    Standard_Boolean isoU,isoV; gp_Pnt2d Loc; gp_Dir2d dir2d;
    TopOpeBRepTool_TOOL::UVISO(C2d,isoU,isoV,dir2d,Loc);
    if (isoU) {par = p2d.Y()-Loc.Y();dist = Abs(p2d.X()-Loc.X());}
    if (isoV) {par = p2d.X()-Loc.X();dist = Abs(p2d.Y()-Loc.Y());}
    if (isoU || isoV) return Standard_True;
  }  

  Geom2dAPI_ProjectPointOnCurve proj(p2d,C2d);
  dist = p2d.Distance(proj.NearestPoint()); 
  par = proj.LowerDistanceParameter();
  return Standard_True;
}



//=======================================================================
//function : TgINSIDE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::TgINSIDE(const TopoDS_Vertex& v, const TopoDS_Edge& E,
				  gp_Vec& Tg, Standard_Integer& OvinE)
{
  TopoDS_Shape aLocalShape = E.Oriented(TopAbs_FORWARD);
  TopoDS_Edge EFOR = TopoDS::Edge(aLocalShape);
//  TopoDS_Edge EFOR = TopoDS::Edge(E.Oriented(TopAbs_FORWARD));
  Standard_Integer ovE = TopOpeBRepTool_TOOL::OriinSor(v,EFOR,Standard_True);
  if (ovE == 0) return Standard_False;
  OvinE = ovE;
  Standard_Integer iv = 0;
  if      (ovE == CLOSING)                      iv = FORWARD;
  else if ((ovE == FORWARD)||(ovE == REVERSED)) iv = ovE;    
  Standard_Real parE;
  if (iv == 0) parE = BRep_Tool::Parameter(v,E);
  else         parE = TopOpeBRepTool_TOOL::ParE(iv,EFOR);
  Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(parE,EFOR,Tg);
  if (!ok) return Standard_False;
  if (ovE == REVERSED) Tg.Reverse();
  return Standard_True;
}

//=======================================================================
//function : TggeomE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::TggeomE(const Standard_Real par, const BRepAdaptor_Curve& BC, 
				 gp_Vec& Tg)
{
//#ifdef OCCT_DEBUG
//  GeomAbs_CurveType ct =
//#endif
//                         BC.GetType();
//#ifdef OCCT_DEBUG
//  Standard_Boolean apoles = (ct == GeomAbs_BezierCurve)||(ct == GeomAbs_BSplineCurve);
//#endif
  
  Standard_Real f = BC.FirstParameter(), l = BC.LastParameter();
  Standard_Real tolE = BC.Tolerance(); Standard_Real tolp = BC.Resolution(tolE);
  
  Standard_Boolean onf = Abs(f-par)<tolp; Standard_Boolean onl = Abs(l-par)<tolp; 
  Standard_Boolean inbounds = (f<par)&&(par<l);

  if ((!inbounds) && (!onf) && (!onl)) return Standard_False;
  Standard_Real thepar = par;

  gp_Pnt thepnt; BC.D1(thepar, thepnt, Tg);
  Tg.Normalize(); 
  return Standard_True;
  
}

//=======================================================================
//function : TggeomE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::TggeomE(const Standard_Real par, const TopoDS_Edge& E, gp_Vec& Tg)
{
  Standard_Boolean isdgE = BRep_Tool::Degenerated(E); 
  if (isdgE) return Standard_False;
  
  BRepAdaptor_Curve BC(E);
  //modified by NIZNHY-PKV Fri Aug  4 09:49:31 2000 f
  if (!CheckEdgeLength(E)) {
    return Standard_False;
  }
  //modified by NIZNHY-PKV Fri Aug  4 09:49:36 2000 t
  
  return (TopOpeBRepTool_TOOL::TggeomE(par,BC,Tg));
}

//=======================================================================
//function : Tg2d
//purpose  : 
//=======================================================================

gp_Vec2d TopOpeBRepTool_TOOL::Tg2d(const Standard_Integer iv, const TopoDS_Edge& E,
				   const TopOpeBRepTool_C2DF& C2DF)
{
  Standard_Real f,l,tol; const Handle(Geom2d_Curve)& PC = C2DF.PC(f,l,tol);
  Standard_Real par = TopOpeBRepTool_TOOL::ParE(iv,E);
  gp_Pnt2d UV; gp_Vec2d tg2d; PC->D1(par,UV,tg2d);
  gp_Dir2d d2d(tg2d);
  return d2d;
} 

//=======================================================================
//function : Tg2dApp
//purpose  : 
//=======================================================================

gp_Vec2d TopOpeBRepTool_TOOL::Tg2dApp(const Standard_Integer iv, const TopoDS_Edge& E,
				      const TopOpeBRepTool_C2DF& C2DF, 
				      const Standard_Real factor)
{
  Standard_Real f,l,tol; const Handle(Geom2d_Curve)& PC = C2DF.PC(f,l,tol);

  Standard_Integer iOOv  = (iv == 1) ? 2 : 1;
  Standard_Real par = TopOpeBRepTool_TOOL::ParE(iv,E);
  Standard_Real OOpar = TopOpeBRepTool_TOOL::ParE(iOOv,E);
  Standard_Real parE = (1-factor)*par + factor*OOpar;

  gp_Pnt2d UV; gp_Vec2d tg2d; PC->D1(parE,UV,tg2d);
  gp_Dir2d d2d(tg2d);

//modified by NIZHNY-MZV  Wed May 24 12:52:18 2000  
//  TopAbs_Orientation oE = E.Orientation();
//  if (M_REVERSED(oE)) d2d.Reverse();
//we remove this line because we want to know original tangent
  return d2d;
}

//=======================================================================
//function : tryTg2dApp
//purpose  : 
//=======================================================================

gp_Vec2d TopOpeBRepTool_TOOL::tryTg2dApp(const Standard_Integer iv, const TopoDS_Edge& E,
					 const TopOpeBRepTool_C2DF& C2DF, 
					 const Standard_Real factor)
{
  Standard_Real f,l,tol; const Handle(Geom2d_Curve)& PC = C2DF.PC(f,l,tol);
  Standard_Boolean isquad = FUN_tool_quad(PC);
  Standard_Boolean line   = FUN_tool_line(PC);
  if (!isquad || line) return TopOpeBRepTool_TOOL::Tg2d(iv,E,C2DF);
  return TopOpeBRepTool_TOOL::Tg2dApp(iv,E,C2DF,factor);
}

//=======================================================================
//function : OriEinF
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_TOOL::tryOriEinF(const Standard_Real par, const TopoDS_Edge& e, const TopoDS_Face& f)
{  
  // ------------------------------------------------------------
  // 1) <e> is a subshape of <f> 
  // 2) else, compute oriEinF, using <e>'s 2d rep on <f>
  //    PREQUESITORY : <e> must have a pcurve on <f>.
  // ------------------------------------------------------------
  Standard_Boolean checkclo = Standard_True; Standard_Integer oeinf = TopOpeBRepTool_TOOL::OriinSor(e,f,checkclo);
  if (oeinf != 0) return oeinf;

  Handle(Geom2d_Curve) pc; Standard_Real pf,pl,tol;
  Standard_Boolean hasold = FC2D_HasOldCurveOnSurface(e,f,pc);
  if (!hasold) return 0;
  pc = FC2D_EditableCurveOnSurface(e,f,pf,pl,tol);
  
  // n2d is such that (p2d,oop2d) is oriented INSIDE F
  gp_Pnt2d uv; gp_Vec2d tg2d; pc->D1(par,uv,tg2d);
  gp_Vec2d n2d(gp_Dir2d(-tg2d.Y(), tg2d.X()));

  Standard_Real delta = TopOpeBRepTool_TOOL::minDUV(f); delta *= 1.e-1; 
  gp_Pnt2d ouv = uv.Translated(delta*n2d);
  Standard_Boolean outuvbounds = TopOpeBRepTool_TOOL::outUVbounds(ouv,f); 
  oeinf = (outuvbounds) ? 2 : 1;
  return oeinf;
}

//=======================================================================
//function : NgApp
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::NgApp(const Standard_Real par,const TopoDS_Edge& e, const TopoDS_Face& f,const Standard_Real tola,
			       gp_Dir& ngApp)
{
  // Give us an edge <e>, a face <f>, <e> has its geometry on <f>.
  //
  // P is the point of <par> on <e>
  // purpose : the compute of <neinsidef>, at a point P' on <F>, near P
  //           direction pp' is normal to <e>.
  // return false if the compute fails, or <neinsidef> is closed to <newneinsidef>
  //
  // PREQUESITORY : <e> must have a pcurve on <f>.
  // --------------

  Handle(Geom_Surface) S = TopOpeBRepTool_ShapeTool::BASISSURFACE(f);
  if (S.IsNull()) return Standard_False;  

  Standard_Boolean fplane = FUN_tool_plane(f);
  if (fplane) return Standard_False;

  // NYI : for bspline surfaces, use a evolutive parameter
  //       on curve to find out "significant" tangents
  Standard_Boolean fquad = FUN_tool_quad(f);
  if (!fquad) return Standard_False;
  // <pc> : 
  Handle(Geom2d_Curve) pc; Standard_Real pf,pl,tol;
  Standard_Boolean hasold = FC2D_HasOldCurveOnSurface(e,f,pc);
  if (!hasold) return Standard_False;
  pc = FC2D_EditableCurveOnSurface(e,f,pf,pl,tol);
  // <orieinf> : 
  TopoDS_Shape aLocalShape = f.Oriented(TopAbs_FORWARD);  
  Standard_Integer orieinf = TopOpeBRepTool_TOOL::tryOriEinF(par,e,TopoDS::Face(aLocalShape));
//  Standard_Integer orieinf = TopOpeBRepTool_TOOL::tryOriEinF(par,e,TopoDS::Face(f.Oriented(TopAbs_FORWARD)));
  if (orieinf == 0) return Standard_False;
  // <uv> : 
  gp_Pnt2d uv; Standard_Boolean ok = FUN_tool_paronEF(e,par,f,uv);
  if (!ok) return Standard_False;
  // <ng> : 
  gp_Dir ng = FUN_tool_ngS(uv,S);
  if (!ok) return Standard_False;
  
  // <n2dinsideS> :
  gp_Vec2d tg2d; pc->D1(par,uv,tg2d); 
  gp_Dir2d n2dinsideS = FUN_tool_nC2dINSIDES( gp_Dir2d(tg2d) );
  if (orieinf == 2)  n2dinsideS.Reverse();
  //<duv> : '
  Standard_Real eps = 0.45678; 
  gp_Vec2d duv = gp_Vec2d(n2dinsideS).Multiplied(eps);

  // cto009S4 : we need an iterative process to get other normal vector
  Standard_Integer nmax = 5; Standard_Boolean same = Standard_False; Standard_Real delta = 0.45678;
  for (Standard_Integer i=1; i<=nmax; i++) {

    gp_Pnt2d newuv = uv.Translated(duv);
    gp_Vec newng = FUN_tool_ngS(newuv,S);
    same = ng.IsEqual(newng,tola);
    Standard_Boolean okk = (newng.Magnitude() > tola);
    if (!same && okk) {ngApp = gp_Dir(newng); break;}
    delta *= 1.25; //  NYI
    duv = gp_Vec2d(n2dinsideS).Multiplied(delta);  

  }//i=1..nmax
  return !same;
}

//=======================================================================
//function : tryNgApp
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::tryNgApp(const Standard_Real par,const TopoDS_Edge& e, const TopoDS_Face& f,const Standard_Real tola,
				  gp_Dir& Ng)
{
  gp_Pnt2d uv; Standard_Boolean ok = FUN_tool_paronEF(e,par,f,uv);
  if (!ok) return Standard_False;
  gp_Dir ng( FUN_tool_nggeomF(uv,f) );  
#ifdef OCCT_DEBUG
  gp_Dir ngApp;
#endif
  ok = TopOpeBRepTool_TOOL::NgApp(par,e,f,tola,Ng);
  if (!ok) Ng = ng;
  return Standard_True;
}

//=======================================================================
//function : IsQuad
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::IsQuad(const TopoDS_Edge& E)
{
  BRepAdaptor_Curve bc(E);
  return ( FUN_quadCT(bc.GetType()) );
}


//=======================================================================
//function : IsQuad
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::IsQuad(const TopoDS_Face& F)
{
  Handle(Geom_Surface) S = TopOpeBRepTool_ShapeTool::BASISSURFACE(F);
  return ( FUN_tool_quad(S) );
}



//=======================================================================
//function : CurvE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::CurvE(const TopoDS_Edge& E,const Standard_Real par,const gp_Dir& tg0,
			       Standard_Real& curv)
{
  curv = 0.;
  BRepAdaptor_Curve BAC(E);
  GeomAbs_CurveType CT = BAC.GetType();
  Standard_Boolean line = (CT == GeomAbs_Line);
  Standard_Real tola = Precision::Angular()*1.e3;//NYITOLXPU
  if (line) {
    gp_Dir dir = BAC.Line().Direction();
    Standard_Real dot = dir.Dot(tg0);
    if (Abs(1-dot) < tola) return Standard_False;
    return Standard_True;
  }

  BRepLProp_CLProps clprops(BAC,par,2,Precision::Confusion());
  Standard_Boolean tgdef = clprops.IsTangentDefined();
  if (!tgdef) return Standard_False;
  curv = Abs(clprops.Curvature());

  Standard_Real tol = Precision::Confusion()*1.e+2;//NYITOLXPU
  Standard_Boolean nullcurv = (curv < tol);
  if (nullcurv) {curv = 0.; return Standard_True;}

  gp_Dir N; clprops.Normal(N);
  gp_Dir T; clprops.Tangent(T);
  gp_Dir axis = N^T;
  Standard_Real dot = Abs(axis.Dot(tg0));
  nullcurv = dot < tola;
  Standard_Boolean maxcurv  = Abs(1-dot) < tola;
  if (nullcurv) {
    curv = 0.;
    return Standard_True;
  }
  if (maxcurv) {
    return Standard_True;
  }
  return Standard_False; // nyi general case
}


// ================================================================================
//   In 3d space, give us a curve <C> and a surface <S>,
//   <C> is tangent to <S> at point P0 = <uv0> on <S> ,
//   <tgC> = C's tangent at P0,
//   <ngS> = <S>'s normal at P0.

//   These define a plane thePlane = (O = P0, XY = (<ngS>,<tgC>)), 
//   the projection of <S> in thePlane describes an apparent contour theContour.

//   In thePlane :
//   P0 -> p2d0 
//   <ngS> -> 2d axis x
//   <tgC> -> 2d axis y

//   <C> -> C2d (same curvature)
//   <S>'s contour -> theContour
//   - the half3dspace described by (<S>,<ngS>) -> the half2dspace described by (theContour,x)

//   if (<tgC>.<ngS> = 0.) : (X,Y) are normal vectors
//                           (x,y) are normal vectors
// ================================================================================
static Standard_Boolean FUN_analyticcS(const gp_Pnt2d& uv0, const Handle(Geom_Surface)& S, const gp_Dir& ngS,
			  const gp_Dir& tg0, 
			  Standard_Real& curv, Standard_Boolean& direct) // dummy if !analyticcontour
{
  curv = 0.; direct = Standard_True; 
  // purpose : Returns true if theContour is analytic, and
  //           then computes its curvature <curv>.
  Handle(Geom_Surface) su = TopOpeBRepTool_ShapeTool::BASISSURFACE(S);
  if (S.IsNull()) return Standard_True;
  GeomAdaptor_Surface GS(su);
  GeomAbs_SurfaceType ST = GS.GetType();
  Standard_Boolean plane = (ST == GeomAbs_Plane);
  Standard_Boolean cyl   = (ST == GeomAbs_Cylinder);
  Standard_Boolean cone  = (ST == GeomAbs_Cone);
  Standard_Boolean sphe  = (ST == GeomAbs_Sphere);
  Standard_Boolean torus  = (ST == GeomAbs_Torus);
  
  Standard_Boolean curvdone = Standard_False;
  if (plane) {curv = 0.; curvdone = Standard_True;}
  if (cyl || cone || torus){
    gp_Dir axis;
    if (cyl) {
      const gp_Cylinder& cycy = GS.Cylinder();
      axis = cycy.Axis().Direction();
      direct = cycy.Direct();
    }
    if (cone) {
      const gp_Cone& coco = GS.Cone();
      axis = coco.Axis().Direction();
      direct = coco.Direct();
    }
    if (torus) {
      const gp_Torus& toto = GS.Torus();
      axis = toto.Axis().Direction();
      direct = toto.Direct();
    }
    Standard_Real prod = axis.Dot(tg0);
    Standard_Boolean isMaxAcurv  = FUN_nullprodv(1-Abs(prod));
    Standard_Boolean nullcurv = FUN_nullprodv(prod);

    Standard_Real prod2 = ngS.Dot(tg0);
    if (cyl || cone) nullcurv = nullcurv || FUN_nullprodv(1-Abs(prod2));

    if (nullcurv) {curv = 0.; curvdone = Standard_True;}
    if (isMaxAcurv)  {
      GeomLProp_SLProps slprops(S,uv0.X(),uv0.Y(),2,Precision::Confusion());
      Standard_Boolean curdef = slprops.IsCurvatureDefined();
      if (curdef) {
	Standard_Real minAcurv = Abs(slprops.MinCurvature());
	Standard_Real maxAcurv = Abs(slprops.MaxCurvature()); 
	Standard_Boolean isAmax = (maxAcurv > minAcurv);
	curv = isAmax ? maxAcurv : minAcurv;
      }
      curvdone = Standard_True;
    }
  }
  if (sphe) {
    const gp_Sphere& spsp = GS.Sphere();    
    curv = 1./spsp.Radius(); curvdone = Standard_True;
    direct = spsp.Direct();
  }

  return curvdone;
}
//=======================================================================
//function : CurvF
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::CurvF(const TopoDS_Face& F,const gp_Pnt2d& uv,const gp_Dir& tg0,
			       Standard_Real& curv,Standard_Boolean& direct)
{
  curv = 0.;
  gp_Dir ngS = FUN_tool_nggeomF(uv,F);
  Handle(Geom_Surface) S = TopOpeBRepTool_ShapeTool::BASISSURFACE(F);  
  if (S.IsNull()) return Standard_False;
  // purpose : Computes theContour's curvature,
  //          returns false if the compute fails.
  
  Standard_Real tola = 1.e-6;//NYITOLXPU

  Standard_Boolean analyticcontour = FUN_analyticcS(uv,S,ngS,tg0,curv,direct);
  if (analyticcontour) return Standard_True;

  GeomLProp_SLProps slprops(S,uv.X(),uv.Y(),2,Precision::Confusion());
  Standard_Boolean curdef = slprops.IsCurvatureDefined();
  if (curdef) {
    gp_Dir npl = tg0;

    gp_Dir MaxD, MinD; slprops.CurvatureDirections(MaxD, MinD);
    Standard_Real mincurv = slprops.MinCurvature();
    Standard_Real maxcurv = slprops.MaxCurvature();
    
    gp_Vec Dmax=ngS^MaxD, Dmin=ngS^MinD; //xpu180898 : cto015G2
    Standard_Real dotmax = Dmax.Dot(npl);//MaxD.Dot(npl); -xpu180898
    Standard_Boolean iscurmax = Abs(1-dotmax)<tola;
    if (iscurmax) {direct = (maxcurv < 0.); curv = Abs(maxcurv);}
    Standard_Real dotmin = Dmin.Dot(npl);//MinD.Dot(npl); -xpu180898
    Standard_Boolean iscurmin = Abs(1-dotmin)<tola;
    if (iscurmin) {direct = (mincurv < 0.); curv = Abs(mincurv);}
    curdef = iscurmax || iscurmin;
    // -------------
    // NYI : !curdef
    // -------------
  }
  return curdef; 
}



//=======================================================================
//function : UVISO
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::UVISO(const Handle(Geom2d_Curve)& PC,
			       Standard_Boolean& isoU, Standard_Boolean& isoV, gp_Dir2d& d2d, gp_Pnt2d& o2d)
{
  isoU = isoV = Standard_False;
  if (PC.IsNull()) return Standard_False;
  Handle(Geom2d_Curve) LLL = BASISCURVE2D(PC);
  Handle(Standard_Type) T2 = LLL->DynamicType();
  Standard_Boolean isline2d = (T2 == STANDARD_TYPE(Geom2d_Line));
  if (!isline2d) return Standard_False;

  Handle(Geom2d_Line) L = Handle(Geom2d_Line)::DownCast(LLL);
  d2d = L->Direction();
  isoU = (Abs(d2d.X()) < Precision::Parametric(Precision::Confusion()));
  isoV = (Abs(d2d.Y()) < Precision::Parametric(Precision::Confusion()));
  Standard_Boolean isoUV = isoU || isoV;
  if (!isoUV) return Standard_False;

  o2d = L->Location();
  return Standard_True;
}

Standard_Boolean TopOpeBRepTool_TOOL::UVISO(const TopoDS_Edge& E, const TopoDS_Face& F,
			       Standard_Boolean & isoU, Standard_Boolean& isoV, gp_Dir2d& d2d, gp_Pnt2d& o2d)
{
  //  Standard_Real f,l,tol; Handle(Geom2d_Curve) PC = FC2D_CurveOnSurface(E,F,f,l,tol);
  Handle(Geom2d_Curve) PC; Standard_Real f,l,tol;
  Standard_Boolean hasold = FC2D_HasOldCurveOnSurface(E,F,PC);
  PC = FC2D_EditableCurveOnSurface(E,F,f,l,tol);
  if (!hasold) FC2D_AddNewCurveOnSurface(PC,E,F,f,l,tol);
  
  Standard_Boolean iso = UVISO(PC,isoU,isoV,d2d,o2d);  
  return iso;
}

Standard_Boolean TopOpeBRepTool_TOOL::UVISO(const TopOpeBRepTool_C2DF& C2DF,
			       Standard_Boolean & isoU, Standard_Boolean& isoV, gp_Dir2d& d2d, gp_Pnt2d& o2d)
{
  Standard_Real f,l,tol; const Handle(Geom2d_Curve)& PC = C2DF.PC(f,l,tol);
//#ifdef OCCT_DEBUG
//  const iso = UVISO(PC,isoU,isoV,d2d,o2d);
//#else
  const Standard_Boolean iso = UVISO(PC,isoU,isoV,d2d,o2d);
//#endif
  return iso;
}


//=======================================================================
//function : IsonCLO
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::IsonCLO(const Handle(Geom2d_Curve)& PC,
				 const Standard_Boolean onU, const Standard_Real xfirst, const Standard_Real xperiod, const Standard_Real xtol)
{
  Standard_Boolean isou,isov; gp_Pnt2d o2d; gp_Dir2d d2d; 
  Standard_Boolean isouv = UVISO(PC,isou,isov,d2d,o2d);  
  if (!isouv) return Standard_False;
  Standard_Boolean onX = (onU && isou) || ((!onU) && isov);
  if (!onX) return Standard_False;
  Standard_Real dxx=0;
  if (onU) dxx = Abs(o2d.X()-xfirst);
  else     dxx = Abs(o2d.Y()-xfirst);

  Standard_Boolean onclo = (dxx < xtol);
  onclo = onclo || (Abs(xperiod-dxx) < xtol);
  return onclo;
}
Standard_Boolean TopOpeBRepTool_TOOL::IsonCLO(const TopOpeBRepTool_C2DF& C2DF,
				 const Standard_Boolean onU, const Standard_Real xfirst, const Standard_Real xperiod, const Standard_Real xtol)
{
  Standard_Real f,l,tol; const Handle(Geom2d_Curve)& PC = C2DF.PC(f,l,tol);
  Standard_Boolean onclo = IsonCLO(PC,onU,xfirst,xperiod,xtol);
  return onclo;
}

//=======================================================================
//function : TrslUV
//purpose  : 
//=======================================================================

void TopOpeBRepTool_TOOL::TrslUV(const gp_Vec2d& t2d, TopOpeBRepTool_C2DF& C2DF)
{
  Standard_Real f,l,tol; Handle(Geom2d_Curve) PC = C2DF.PC(f,l,tol);
  PC->Translate(t2d);
  C2DF.SetPC(PC,f,l,tol);
}

Standard_Boolean TopOpeBRepTool_TOOL::TrslUVModifE(const gp_Vec2d& t2d, const TopoDS_Face& F, TopoDS_Edge& E)
{
  Standard_Real f,l,tol; Handle(Geom2d_Curve) PC = FC2D_CurveOnSurface(E,F,f,l,tol);
//  Handle(Geom2d_Curve) PC; Standard_Real f,l,tol;

  if (PC.IsNull()) return Standard_False;
  PC->Translate(t2d);
//  Handle(Geom2d_Curve) toclear; BB.UpdateEdge(E,toclear,F,tole);
  BRep_Builder BB; BB.UpdateEdge(E,PC,F,tol);    
  return Standard_True;
}

//=======================================================================
//function : Matter
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_TOOL::Matter(const gp_Vec& d1, const gp_Vec& dR2, const gp_Vec& Ref)
{
  gp_Vec d2 = dR2.Reversed();

  Standard_Real tola = Precision::Angular();
  Standard_Real ang = d1.Angle(d2);
  Standard_Boolean equal = (ang < tola);
  if (equal) return 0.;
  Standard_Boolean oppo = ((M_PI-ang) < tola);
  if (oppo)  return M_PI;

  ang = d1.AngleWithRef(d2,Ref);
  if (ang < 0) ang = 2.*M_PI+ang;    
  return ang;
}

//=======================================================================
//function : Matter
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_TOOL::Matter(const gp_Vec2d& d1, const gp_Vec2d& dR2)
{
  gp_Vec v1 = gp_Vec(d1.X(),d1.Y(),0.);
  gp_Vec vR2 = gp_Vec(dR2.X(),dR2.Y(),0.);
  gp_Vec Ref(0.,0.,1.);

  Standard_Real ang = TopOpeBRepTool_TOOL::Matter(v1,vR2,Ref);
  return ang;
}

//=======================================================================
//function : Matter
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::Matter(const gp_Dir& xx1,const gp_Dir& nt1,
				const gp_Dir& xx2,const gp_Dir& nt2,
				const Standard_Real tola, Standard_Real& ang)
// purpose : the compute of MatterAng(f1,f2)
{
  // --------------------------------------------------
  // Give us a face f1 and one edge e of f1, pone=pnt(e,pare) 
  // We project the problem in a plane normal to e, at point pone 
  // ie we see the problem in space (x,y), with RONd (x,y,z), z tangent to e at pone.
  // RONd (x,y,z) = (xx1,nt1,x^y)
  // 
  // Make the analogy : 
  // f <-> Ef, e <-> Ve, 
  // In view (x,y), f1 is seen as an edge Ef, e is seen as a vertex Ve,
  // the matter delimited by f can be seen as the one delimited by Ef.
  // --------------------------------------------------

  // Sign( (v1^nt1).z ) describes Ve's orientation in Ef1
  // (v1^nt1).z > 0. => Ve is oriented REVERSED in Ef1.
  // - ori(Ve,Ef1) == REVERSED : the matter delimited by <f1> 
  //                              is (y<=0) in (x,y) 2d space -

  gp_Dir z1 = xx1^nt1;
  gp_Dir z2 = xx2^nt2;
  Standard_Real dot = z2.Dot(z1);
  Standard_Boolean oppo = (dot < 0.);
  if (!oppo) return Standard_False;

  // -nti points towards 3dmatter(fi)
  // => zi = xxi^nti gives the opposite sense for the compute of the matter angle
  z1.Reverse();
  ang = xx1.AngleWithRef(xx2,z1);
  if (Abs(ang) < tola) {ang = 0.; return Standard_True;}
  if (ang < 0) ang = 2.*M_PI+ang; 
  
  return Standard_True;
}

//=======================================================================
//function : Getduv
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::Getduv(const TopoDS_Face& f,const gp_Pnt2d& uv,const gp_Vec& dir,
				const Standard_Real factor, gp_Dir2d& duv)
{
  Standard_Boolean quad = TopOpeBRepTool_TOOL::IsQuad(f);
  if (!quad) return Standard_False;
  Bnd_Box bndf; BRepBndLib::AddClose(f,bndf);
  Standard_Real f1,f2,f3,l1,l2,l3; bndf.Get(f1,f2,f3,l1,l2,l3);
  gp_Vec d123(f1-l1, f2-l2, f3-l3);

  gp_Pnt p; FUN_tool_value(uv,f,p); p.Translate(dir.Multiplied(factor));
  Standard_Real d; gp_Pnt2d uvtr;
  FUN_tool_projPonF(p,f, uvtr,d);
  Standard_Real tolf = BRep_Tool::Tolerance(f); tolf *= 1.e2; //NYIXPUTOL
  if (d > tolf) return Standard_False;

  gp_Vec2d DUV( uv, uvtr );
  Handle(Geom_Surface) S = TopOpeBRepTool_ShapeTool::BASISSURFACE(f);
  if ((S->IsUPeriodic()) && (Abs(DUV.X()) > S->UPeriod()/2.))
    {
      Standard_Real U1 = uv.X(), U2 = uvtr.X(), period = S->UPeriod();
      ElCLib::AdjustPeriodic( 0., period, Precision::PConfusion(), U1, U2 );
      Standard_Real dx = U2-U1;
      if (dx > period/2.)
	dx -= period;
      DUV.SetX( dx );
    }
  if ((S->IsVPeriodic()) && (Abs(DUV.Y()) > S->VPeriod()/2.))
    {
      Standard_Real V1 = uv.Y(), V2 = uvtr.Y(), period = S->VPeriod();
      ElCLib::AdjustPeriodic( 0., period, Precision::PConfusion(), V1, V2 );
      Standard_Real dy = V2-V1;
      if (dy > period/2.)
	dy -= period;
      DUV.SetY( dy );
    }
  duv = gp_Dir2d( DUV );

  return Standard_True;
}



//=======================================================================
//function : uvApp
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::uvApp(const TopoDS_Face& f,const TopoDS_Edge& e,const Standard_Real pare,const Standard_Real eps,
			       gp_Pnt2d& uvapp)
{
  // uv : 
  Standard_Boolean ok = FUN_tool_paronEF(e,pare,f,uvapp);
  if (!ok) return Standard_False;
  gp_Vec2d dxx; ok = FUN_tool_getdxx(f,e,pare,dxx);
  if (!ok) return Standard_False;
  uvapp.Translate(dxx.Multiplied(eps));
  return Standard_True;
}

//=======================================================================
//function : XX
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::XX(const gp_Pnt2d& uv, const TopoDS_Face& f,
			    const Standard_Real par, const TopoDS_Edge& e, 
			    gp_Dir& XX)
{
  // ng(uv):
  gp_Vec ng = FUN_tool_nggeomF(uv,f);
  gp_Vec geomxx = FUN_tool_getgeomxx(f,e,par,ng); 

  Standard_Real tol = Precision::Confusion()*1.e2;//NYITOL
  Standard_Boolean nullng = (geomxx.Magnitude()<tol);
  if (nullng) return Standard_False;
 
  TopAbs_Orientation oef; Standard_Boolean ok = FUN_tool_orientEinFFORWARD(e,f,oef);
  if (!ok) return Standard_False;
  XX = gp_Dir(geomxx);
  if (M_REVERSED(oef)) XX.Reverse();
  return Standard_True;
}

//=======================================================================
//function : Nt
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::Nt(const gp_Pnt2d& uv, const TopoDS_Face& f,
			    gp_Dir& normt)
{
  gp_Vec nggeom; Standard_Boolean ok = TopOpeBRepTool_TOOL::NggeomF(uv,f,nggeom);
  if (!ok) return Standard_False;
  normt = gp_Dir(nggeom);
  if (M_REVERSED(f.Orientation())) normt.Reverse();
  return Standard_True;
}

//=======================================================================
//function : NggeomF
//purpose  : 
//=======================================================================

static Standard_Boolean FUN_ngF(const gp_Pnt2d& uv, const TopoDS_Face& F, gp_Vec& ngF)
{
  BRepAdaptor_Surface bs(F);
  Standard_Real tol3d = bs.Tolerance();
  Standard_Real tolu = bs.UResolution(tol3d);
  Standard_Real tolv = bs.VResolution(tol3d);
  
  // ###############################
  // nyi : all geometries are direct
  // ###############################
  gp_Pnt p; gp_Vec d1u,d1v; bs.D1(uv.X(),uv.Y(),p,d1u,d1v);  

  Standard_Real delta = TopOpeBRepTool_TOOL::minDUV(F); delta *= 1.e-1; 

  Standard_Real du = d1u.Magnitude();
  Standard_Real dv = d1v.Magnitude();
  Standard_Boolean kpart = (du < tolu) || (dv < tolv);
  if (kpart) { 
    GeomAbs_SurfaceType ST = bs.GetType();
    if (ST == GeomAbs_Cone) {
      Standard_Boolean nullx = (Abs(uv.X()) < tolu);
      Standard_Boolean apex = nullx && (Abs(uv.Y()) < tolv);
      if (apex) {
	const gp_Dir& axis = bs.Cone().Axis().Direction();
	gp_Vec ng(axis); ng.Reverse();
	ngF = ng; return Standard_True;
      }
      else if (du < tolu) {		
	Standard_Real x = uv.X(); 
	
	Standard_Real y = uv.Y(); 
	Standard_Real vf = bs.FirstVParameter();
	
	if (Abs(vf-y) < tolu) vf += delta;
	else                  vf -= delta;

	//modified by NIZHNY-MZV  Fri Nov 26 12:38:55 1999
	y = vf;
	bs.D1(x,y,p,d1u,d1v); 	
	gp_Vec ng = d1u^d1v;

	ngF = ng; return Standard_True;
      }
    }
    if (ST == GeomAbs_Sphere) {
      Standard_Real pisur2 = M_PI*.5;
      Standard_Real u = uv.X(),v = uv.Y();
      Standard_Boolean vpisur2 = (Abs(v-pisur2) < tolv);
      Standard_Boolean vmoinspisur2 = (Abs(v+pisur2) < tolv);
      Standard_Boolean apex = vpisur2 || vmoinspisur2;
      if (apex) {
	gp_Pnt center = bs.Sphere().Location();
	gp_Pnt value  = bs.Value(u,v); 
	gp_Vec ng(center,value); 
	ngF = ng; return Standard_True;
      }
    }
#ifdef OCCT_DEBUG
    cout<<"FUN_tool_nggeomF NYI"<<endl;
#endif
    return Standard_False;
  } //kpart

  gp_Dir udir(d1u);
  gp_Dir vdir(d1v);
  ngF = gp_Vec(gp_Dir(udir^vdir));
  return Standard_True;
}

Standard_Boolean TopOpeBRepTool_TOOL::NggeomF(const gp_Pnt2d& uv, const TopoDS_Face& f,
				 gp_Vec& ng)
{
  return FUN_ngF(uv,f,ng);
}

//=======================================================================
//function : Matter
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::Matter(const TopoDS_Face& f1,const TopoDS_Face& f2,
				const TopoDS_Edge& e,const Standard_Real par,
				const Standard_Real tola, Standard_Real& ang)
{
  gp_Dir xx1,xx2;
  gp_Dir nt1,nt2;
  
  Standard_Real tolf1 = BRep_Tool::Tolerance(f1)*1.e2;//nyitolxpu
  gp_Pnt2d uv1; Standard_Boolean ok1 = FUN_tool_paronEF(e,par,f1,uv1,tolf1);
  if (!ok1) return Standard_False;
  ok1 = TopOpeBRepTool_TOOL::Nt(uv1,f1,nt1);
  if (!ok1) return Standard_False;
  ok1 = TopOpeBRepTool_TOOL::XX(uv1,f1,par,e,xx1);
  if (!ok1) return Standard_False;
  
  Standard_Real tolf2 = BRep_Tool::Tolerance(f2)*2.e2;//nyitolxpu
  gp_Pnt2d uv2; Standard_Boolean ok2 = FUN_tool_paronEF(e,par,f2,uv2,tolf2);
  if (!ok2) return Standard_False;
  ok2 = TopOpeBRepTool_TOOL::Nt(uv2,f2,nt2);
  if (!ok2) return Standard_False;
  ok2 = TopOpeBRepTool_TOOL::XX(uv2,f2,par,e,xx2);
  if (!ok2) return Standard_False;
  
  return (TopOpeBRepTool_TOOL::Matter(xx1,nt1,xx2,nt2,tola,ang));
}




//=======================================================================
//function : MatterKPtg
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::MatterKPtg(const TopoDS_Face& f1,const TopoDS_Face& f2,const TopoDS_Edge& e,
				    Standard_Real& ang)
{
  Standard_Real f,l; FUN_tool_bounds(e,f,l);
  Standard_Real x = 0.45678; Standard_Real pare = (1-x)*f+x*l;

  Standard_Real eps = 0.123; //NYIXPU190199

  //Standard_Real tola = Precision::Angular()*1.e3;

  gp_Pnt2d uv1; FUN_tool_paronEF(e,pare,f1,uv1);
  gp_Dir nt1; Standard_Boolean ok1 = TopOpeBRepTool_TOOL::Nt(uv1,f1,nt1);
  if (!ok1) return Standard_False;
  gp_Pnt2d uvapp1; ok1 = TopOpeBRepTool_TOOL::uvApp(f1,e,pare,eps,uvapp1);
  if (!ok1) return Standard_False;
  gp_Pnt pf1; FUN_tool_value(uvapp1,f1,pf1); 

  gp_Pnt2d uv2; Standard_Real d; Standard_Boolean ok2 = FUN_tool_projPonF(pf1,f2,uv2,d);
  gp_Pnt pf2; FUN_tool_value(uv2,f2,pf2); 
  if (!ok2) return Standard_False;

  gp_Dir v12(gp_Vec(pf1,pf2));
  Standard_Real dot = v12.Dot(nt1);
  ang = (dot < 0.) ? 0. : 2.*M_PI;

//  gp_Dir nt1; ok1 = TopOpeBRepTool_TOOL::Nt(uv1,f1,nt1);
//  if (!ok1) return Standard_False;
//  gp_Dir xx1; ok1 = TopOpeBRepTool_TOOL::XX(uv1,f1,pare,e,xx1);
//  if (!ok1) return Standard_False;    
//  gp_Pnt2d uv2; Standard_Boolean ok2 = TopOpeBRepTool_TOOL::uvApp(f2,e,pare,eps,uv2);
//  if (!ok2) return Standard_False;
//  gp_Dir nt2; ok2 = TopOpeBRepTool_TOOL::Nt(uv2,f2,nt2);
//  if (!ok2) return Standard_False;
//  gp_Dir xx2; ok2 = TopOpeBRepTool_TOOL::XX(uv2,f2,pare,e,xx2);
//  if (!ok2) return Standard_False;  
//  Standard_Real angapp; Standard_Boolean ok = TopOpeBRepTool_TOOL::Matter(xx1,nt1, xx2,nt2,tola,angapp);
//  if (!ok) return Standard_False;
//  Standard_Boolean is0 = (Abs(angapp) < Abs(2.*M_PI-angapp));
//  ang = is0 ? 0. : 2.*M_PI;
  return Standard_True;
}

//=======================================================================
//function : Getstp3dF
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::Getstp3dF(const gp_Pnt& p, const TopoDS_Face& f, gp_Pnt2d& uv, TopAbs_State& st)
// classification solide de <P> / <F>
{
  st = TopAbs_UNKNOWN;
  Standard_Real tol3d = BRep_Tool::Tolerance(f);
    // EXPENSIVE : calls an extrema
  Standard_Real d;  Standard_Boolean ok = FUN_tool_projPonF(p,f,uv,d);
  if (!ok) return Standard_False;
  if (d < tol3d) {st = TopAbs_ON; return Standard_True;}

  gp_Pnt ppr; ok = FUN_tool_value(uv,f,ppr);
  if (!ok) return Standard_False;

  gp_Dir ntf; ok = TopOpeBRepTool_TOOL::Nt(uv,f, ntf);
  if (!ok) return Standard_False;

  gp_Dir dppr(gp_Vec(p,ppr));
  Standard_Real dot = dppr.Dot(ntf);
  Standard_Boolean isOUT = (dot < 0.);
  st = (isOUT ? TopAbs_OUT : TopAbs_IN);
  return Standard_True;
}



//=======================================================================
//function : MkShell
//purpose  : 
//=======================================================================

void TopOpeBRepTool_TOOL::MkShell(const TopTools_ListOfShape& lF, TopoDS_Shape& She)
{
  BRep_Builder BB; BB.MakeShell(TopoDS::Shell(She));
  for (TopTools_ListIteratorOfListOfShape li(lF); li.More(); li.Next()) BB.Add(She,li.Value());
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::Remove(TopTools_ListOfShape& loS, const TopoDS_Shape& toremove)
{
  TopTools_ListIteratorOfListOfShape it(loS);
  Standard_Boolean found = Standard_False;
  while (it.More()) {
    if (it.Value().IsEqual(toremove)) {loS.Remove(it);found = Standard_True;}
    else                              it.Next();
  }
  return found;
}

//=======================================================================
//function : minDUV
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_TOOL::minDUV(const TopoDS_Face& F)
{
  BRepAdaptor_Surface BS(F);
  Standard_Real delta = BS.LastUParameter() - BS.FirstUParameter();
  Standard_Real tmp = BS.LastVParameter() - BS.FirstVParameter();
  delta = (tmp < delta) ? tmp : delta;  
  return delta;  
}


//=======================================================================
//function : stuvF
//purpose  : 
//=======================================================================
#define INFFIRST (-1)
#define SUPLAST (-2)
#define ONFIRST (1)
#define ONLAST  (2)
void TopOpeBRepTool_TOOL::stuvF(const gp_Pnt2d& uv,const TopoDS_Face& f,  Standard_Integer& onU,Standard_Integer& onV)
{
  BRepAdaptor_Surface bs(f);
  onU = onV = 0;
  Standard_Real tolf = bs.Tolerance();
  Standard_Real tolu = bs.UResolution(tolf), tolv = bs.VResolution(tolf); 
  Standard_Real u=uv.X(),v = uv.Y();
  Standard_Real uf=bs.FirstUParameter(),ul=bs.LastUParameter(),vf=bs.FirstVParameter(),vl=bs.LastVParameter();
  Standard_Boolean onuf = (Abs(uf-u)<tolu), onul = (Abs(ul-u)<tolu);
  Standard_Boolean onvf = (Abs(vf-v)<tolv), onvl = (Abs(vl-v)<tolv);
  if (onuf) onU = ONFIRST;
  if (onul) onU = ONLAST;
  if (onvf) onV = ONFIRST;
  if (onvl) onV = ONLAST;
  if (u < (uf-tolu)) onU = INFFIRST;
  if (u > (ul+tolu)) onU = SUPLAST;
  if (v < (vf-tolv)) onV = INFFIRST;
  if (v > (vl+tolv)) onV = SUPLAST;
}

//=======================================================================
//function : outUVbounds
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::outUVbounds(const gp_Pnt2d& uv, const TopoDS_Face& F)
{
  BRepAdaptor_Surface BS(F);
  Standard_Boolean outofboundU = (uv.X() > BS.LastUParameter())||(uv.X() < BS.FirstUParameter());
  Standard_Boolean outofboundV = (uv.Y() > BS.LastVParameter())||(uv.Y() < BS.FirstVParameter());  
  return outofboundU || outofboundV;
}

//=======================================================================
//function : TolUV
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_TOOL::TolUV(const TopoDS_Face& F, const Standard_Real tol3d)
{ 
  BRepAdaptor_Surface bs(F);
  Standard_Real tol2d = bs.UResolution(tol3d);
  tol2d = Max(tol2d,bs.VResolution(tol3d));
  return tol2d;
}

//=======================================================================
//function : TolP
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_TOOL::TolP(const TopoDS_Edge& E, const TopoDS_Face& F) 
{
  BRepAdaptor_Curve2d BC2d(E,F);
  return ( BC2d.Resolution(BRep_Tool::Tolerance(E)) );
}

//=======================================================================
//function : WireToFace
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::WireToFace(const TopoDS_Face& Fref, const TopTools_DataMapOfShapeListOfShape& mapWlow,
				    TopTools_ListOfShape& lFs)
{
  BRep_Builder BB;
  TopoDS_Shape aLocalShape = Fref.Oriented(TopAbs_FORWARD);
  TopoDS_Face F = TopoDS::Face(aLocalShape);
//  TopoDS_Face F = TopoDS::Face(Fref.Oriented(TopAbs_FORWARD));
  Standard_Boolean toreverse = M_REVERSED(Fref.Orientation());
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(mapWlow);
  for (; itm.More(); itm.Next()) {
    TopoDS_Shape FF = F.EmptyCopied();
    const TopoDS_Wire& wi = TopoDS::Wire(itm.Key());  
    BB.Add(FF,wi);
    TopTools_ListIteratorOfListOfShape itw(itm.Value());
    for (; itw.More(); itw.Next()) {
      const TopoDS_Wire& wwi = TopoDS::Wire(itw.Value());
      BB.Add(FF,wwi);
    }
    if (toreverse) FF.Orientation(TopAbs_REVERSED);
    lFs.Append(FF);
  }
  return Standard_True;
}

//=======================================================================
//function : EdgeONFace
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_TOOL::EdgeONFace(const Standard_Real par,const TopoDS_Edge& ed,
				    const gp_Pnt2d& uv,const TopoDS_Face& fa,
				    Standard_Boolean& isonfa)
{
  isonfa = Standard_False;
  // prequesitory : pnt(par,ed) = pnt(uv,f)
  Standard_Boolean dge = BRep_Tool::Degenerated(ed);
  if (dge) {
    isonfa = Standard_True;
    return Standard_True;
  }

  Standard_Real tola = Precision::Angular()*1.e2;//NYITOLXPU
  gp_Vec tge; Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(par,ed,tge);  
  if (!ok) return Standard_False;
  gp_Vec ngf = FUN_tool_nggeomF(uv,fa);
  Standard_Real aProdDot = tge.Dot(ngf);
  Standard_Boolean etgf = Abs(aProdDot) < tola;
  if (!etgf) return Standard_True;

  BRepAdaptor_Surface bs(fa);
  GeomAbs_SurfaceType st = bs.GetType();
  Standard_Boolean plane = (st == GeomAbs_Plane);
  Standard_Boolean cylinder = (st == GeomAbs_Cylinder);
  
  BRepAdaptor_Curve bc(ed);
  GeomAbs_CurveType ct = bc.GetType();
  Standard_Boolean line = (ct == GeomAbs_Line);
  Standard_Boolean circle = (ct == GeomAbs_Circle);

  Standard_Real tole = bc.Tolerance(); Standard_Real tol1de = bc.Resolution(tole);
  Standard_Real tolf = bs.Tolerance();
  Standard_Real tol3d = Max(tole,tolf)*1.e2;//NYITOLXPU

  // NYIxpu100299 : for other analytic geometries
  if (plane && line) {isonfa = Standard_True; return Standard_True;}
  if       (plane) {
    gp_Dir ne;
    Standard_Boolean det = Standard_True;
    if      (circle)  ne = bc.Circle().Axis().Direction();
    else if (ct == GeomAbs_Ellipse) ne = bc.Ellipse().Axis().Direction();
    else if (ct == GeomAbs_Hyperbola)    ne = bc.Hyperbola().Axis().Direction();
    else if (ct == GeomAbs_Parabola)ne = bc.Parabola().Axis().Direction();
    else                            det = Standard_False;
    if (det) {
      Standard_Real prod = ne.Dot(ngf);
      isonfa = ( Abs(1-Abs(prod)) < tola );
      return Standard_True;
    }
  }//plane
  else if (cylinder) {
    gp_Dir ne; Standard_Boolean det = Standard_True;
    if      (line)  ne = tge;
    else if (circle)ne = bc.Circle().Axis().Direction();
    else            det = Standard_False;
    gp_Dir axicy = bs.Cylinder().Axis().Direction();

    if (det) {
      Standard_Real prod = ne.Dot(axicy);
      isonfa = ( Abs(1-Abs(prod)) < tola );
      if (isonfa && circle) {
	Standard_Real radci = bc.Circle().Radius();
	Standard_Real radcy = bs.Cylinder().Radius();
	isonfa = ( Abs(radci-radcy)<tol3d );
      }
      return Standard_True;
    }
  }//cylinder

  // !!!!!!!!!!!!!!!! NOT STILL OK !!!!!!!!!!!!!!
  // projecting point of <ed> on <fa>
  Standard_Real x = 0.12345;
  Standard_Real f,l; FUN_tool_bounds(ed,f,l);
  Standard_Boolean onf = ( Abs(par-f)<tol1de );
  Standard_Real opar = onf ?  ((1-x)*f+x*l) : ((1-x)*f+x*par);
  gp_Pnt opc = bc.Value(opar);
  
  gp_Pnt2d ouv; ok = FUN_tool_parF(ed,opar,fa,ouv,tolf);
  if (!ok) return Standard_False;  
  gp_Pnt ops = bs.Value(ouv.X(),ouv.Y());
  
  Standard_Real dd = opc.Distance(ops);
  isonfa = (dd < tol3d);
  return Standard_True;
}
