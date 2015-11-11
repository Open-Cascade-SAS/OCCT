// Created on: 1999-02-11
// Created by: Xuan PHAM PHU
// Copyright (c) 1999-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_makeTransition.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define BEFORE (1)
#define AFTER  (2)

#define isINifh1   (1)
#define isINifh2   (2)
#define isON2     (21)
#define isON2ifss (10)
#define isIN2ifss (11)
#define isOU2ifss (12)

#define INFFIRST (-1)
#define SUPLAST (-2)
#define ONFIRST (1)
#define ONLAST  (2)

#define FORWARD  (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING  (5)

#define M_ON(sta)       (sta == TopAbs_ON)
#define M_IN(sta)       (sta == TopAbs_IN)
#define M_FORWARD(sta)  (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)
#define M_UNKNOWN(sta)  (sta == TopAbs_UNKNOWN)

static Standard_Real FUN_tolang()
{
  return Precision::Angular()*1.e6;//=1.e-6 NYITOLXPU
}

//=======================================================================
//function : TopOpeBRepTool_makeTransition
//purpose  : 
//=======================================================================

TopOpeBRepTool_makeTransition::TopOpeBRepTool_makeTransition()
{
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_makeTransition::Initialize
(const TopoDS_Edge& E,const Standard_Real pbef,const Standard_Real paft,const Standard_Real parE,
 const TopoDS_Face& FS, const gp_Pnt2d& uv,
 const Standard_Real factor)
{
  Standard_Boolean isdge = BRep_Tool::Degenerated(E);
  if (isdge) return Standard_False;

  myE = E;
  mypb = pbef; mypa = paft; mypE = parE;
  myFS = FS; myuv = uv;
  myfactor = factor;
  hasES = Standard_False;

  Standard_Boolean facko = (factor < 0.) || (factor > 1.);
  if (facko) return Standard_False;

  Standard_Boolean ok = TopOpeBRepTool_TOOL::EdgeONFace(mypE,myE,myuv,FS,isT2d);
  return ok;
}

//=======================================================================
//function : Setfactor
//purpose  : 
//=======================================================================

void TopOpeBRepTool_makeTransition::Setfactor(const Standard_Real factor)
{
  myfactor = factor;
}

//=======================================================================
//function : Getfactor
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_makeTransition::Getfactor() const 
{
  return myfactor;
}

//=======================================================================
//function : IsT2d
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_makeTransition::IsT2d() const
{
  return isT2d;
}



//=======================================================================
//function : SetRest
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_makeTransition::SetRest(const TopoDS_Edge& ES, const Standard_Real parES)
{
  Standard_Boolean isdge = BRep_Tool::Degenerated(ES);
  if (isdge) return Standard_False;

  hasES = Standard_True;
  myES = ES;
  mypES = parES;

  // nyi check <ES> is edge of <myFS>
  return Standard_True;
}

//=======================================================================
//function : HasRest
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_makeTransition::HasRest() const
{
  return hasES;
}

static Standard_Boolean FUN_nullcurv(const Standard_Real curv)
{
  Standard_Real tol = Precision::Confusion()*1.e+2;//NYITOLXPU
  return (curv < tol);
}

static Standard_Integer FUN_mkT2dquad(const Standard_Real curvC1, const Standard_Real curvC2)
{
  // !!! only for quadratic geometries :
  // prequesitory : C1 and C2 are ON given plane; they are tangent
  // at point pt, where curvatures are respectively curvC1,curvC2
  // stb = state of point on C1 before pt / C2
  // = sta = state of point on C1 after pt / C2

  Standard_Boolean nullc1  = FUN_nullcurv(curvC1);
  Standard_Boolean nullc2 = FUN_nullcurv(curvC2);
  if (nullc2 && nullc1) return isON2;
  if (nullc2) return isINifh1;//is IN if (dot=tg1(pt after).xx2 > 0)
  if (nullc1) return isINifh2;//is IN if (dot=tg2(pt after).xx2 < 0)  

  Standard_Boolean samec = (Abs(curvC2-curvC1) < 1.e-2); //NYITOLXPU kpartkoletge
  if (samec)           return isON2ifss;//is ON if curves are on same side/tg line
  if (curvC1 > curvC2) return isIN2ifss;//is IN if curves are on same side/tg line
  else                 return isOU2ifss;//is OU if curves are on same side/tg line
//  return 0;
}

static Standard_Boolean FUN_getnearpar(const TopoDS_Edge& e,const Standard_Real par,const Standard_Real f,const Standard_Real l,
			  const Standard_Real factor,const Standard_Integer sta, Standard_Real& nearpar)
{
  // hyp : f < par < l
  BRepAdaptor_Curve bc(e);
  Standard_Real tol1d = bc.Resolution( bc.Tolerance() ); 
  Standard_Boolean onf = (Abs(par-f) < tol1d);
  Standard_Boolean onl = (Abs(par-l) < tol1d);
  if (onf && (sta == BEFORE)) return Standard_False; 
  if (onl && (sta == AFTER))  return Standard_False; 
  //nearpar = (sta == BEFORE) ? ((1-factor)*par - factor*f) : ((1-factor)*par - factor*l); 
  nearpar = (sta == BEFORE) ? (par - factor*(l-f)) : (par + factor*(l-f)); 
  return Standard_True;
}

static Standard_Boolean FUN_tg(const TopoDS_Edge& e,const Standard_Real par,const Standard_Real f,const Standard_Real l,const Standard_Real factor,
		  gp_Dir& tg, Standard_Integer& st)
{
  st = BEFORE;
  for (Standard_Integer nite = 1; nite <= 2; nite++) {
    if (nite == 2) st = AFTER;    
    Standard_Real pn = 0.;
    Standard_Boolean mkp = FUN_getnearpar(e,par,f,l,factor,st,pn);
    if (!mkp) continue;
    gp_Vec tmp;
    Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(pn,e,tmp);
    if (!ok) continue;
    tg = gp_Dir(tmp); 
    return Standard_True;    
  }
  return Standard_False;  
}
static Standard_Boolean FUN_getsta(const Standard_Integer mkt,const gp_Dir& tga1,const gp_Dir& tga2,const gp_Dir& xx2,
		      TopAbs_State& sta)
{
  if      (mkt == isINifh1) {
    // curv(e1) > 0.
    //is IN if (dot=tg1(pt after).xx2 > 0)
    Standard_Real dot = tga1.Dot(xx2);
    sta =  (dot > 0) ? TopAbs_IN : TopAbs_OUT;
    return Standard_True;
  }
  else if (mkt == isINifh2) {
    // curv(e2) > 0.
    //is IN if (dot=tg2(pt after).xx2 < 0) 
    Standard_Real dot = tga2.Dot(xx2);
    sta =  (dot < 0) ? TopAbs_IN : TopAbs_OUT;
    return Standard_True;
  }
  else if (mkt == isON2ifss) {
    // curv(e1), curv(e2) > 0.
    //is ON if curves are on same side/tg line 
    Standard_Boolean ssided = (tga1.Dot(tga2) > 0);
    sta = ssided ? TopAbs_ON : TopAbs_IN;
    return Standard_True;
  }
  else if (mkt == isIN2ifss) {
    //stag = IN if curves are on same side/tg line  
    Standard_Real dot = tga1.Dot(xx2);
    sta = (dot < 0) ? TopAbs_OUT : TopAbs_IN;
    return Standard_True;
  }
  else if (mkt == isOU2ifss) {
    //stag = OU if curves are on same side/tg line 
    Standard_Real dot = tga2.Dot(xx2);
    sta = (dot < 0) ? TopAbs_IN : TopAbs_OUT;
    return Standard_True;   
  } 
  else {//mkt == isON2
    sta = TopAbs_ON; 
    return Standard_True;
  } 
} 
static Standard_Boolean FUN_mkT2dquad(const TopoDS_Edge& e1,const Standard_Real par1,const Standard_Real f1,const Standard_Real l1,
			 const TopoDS_Edge& e2,const Standard_Real par2,
			 const Standard_Integer mkt,const gp_Dir& xx2,const Standard_Real factor,
			 TopAbs_State& sta)
{
  sta = TopAbs_UNKNOWN;

  // !!! only for quadratic geometries :
  // stb = state of point on e1 before pt / e2
  // = sta = state of point on e1 after pt / e2

  gp_Dir tga1,tga2;
  Standard_Boolean mk1 = (mkt==isINifh1) || (mkt==isON2ifss) || (mkt==isIN2ifss);
  if (mk1) {
    Standard_Integer st1 = 0; gp_Dir tgnear1;
    Standard_Boolean ok = FUN_tg(e1,par1,f1,l1,factor,tgnear1,st1);
    if (!ok) return Standard_False;
    tga1 = (st1 == AFTER) ? tgnear1 : tgnear1.Reversed();    
  }
  Standard_Boolean mk2 = (mkt==isINifh2) || (mkt==isON2ifss) || (mkt==isOU2ifss);
  if (mk2) {
    Standard_Real f2,l2; FUN_tool_bounds(e2,f2,l2);
    Standard_Integer st2 = 0; gp_Dir tgnear2;
    Standard_Boolean ok = FUN_tg(e2,par2,f2,l2,factor,tgnear2,st2);
    if (!ok) return Standard_False;
    tga2 = (st2 == AFTER) ? tgnear2 : tgnear2.Reversed();
  }
  return (FUN_getsta(mkt,tga1,tga2,xx2, sta));
}

//=======================================================================
//function : MkT2donE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_makeTransition::MkT2donE(TopAbs_State& Stb,TopAbs_State& Sta) const
{
  if (!isT2d) return Standard_False;

  // E is IN 2d(FS), meets no restriction at given point :
  if (!hasES) {Stb = Sta = TopAbs_IN; return Standard_True;}
  
  // E is IN 2d(FS), meets restriction ES at given point :
  Standard_Integer oriESFS = TopOpeBRepTool_TOOL::OriinSor(myES,myFS, Standard_True);
  if (oriESFS == 0) return Standard_False;
  
  // ES is closing edge for FS, or ES is INTERNAL in FS : 
  if      (oriESFS == INTERNAL) {Stb = Sta = TopAbs_IN; return Standard_True;}
  else if (oriESFS == CLOSING)  {Stb = Sta = TopAbs_IN; return Standard_True;}  
  
  gp_Vec tmp; Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(mypE,myE,tmp);
  if (!ok) return Standard_False;
  gp_Dir tgE(tmp);     
  gp_Dir xxES; ok = TopOpeBRepTool_TOOL::XX(myuv,myFS, mypES,myES, xxES);
  if (!ok) return Standard_False;
  
  Standard_Real tola = FUN_tolang();
  Standard_Real dot = tgE.Dot(xxES);
  
  // E and ES are not tangent at interference point :
  Standard_Boolean tgts = (Abs(dot) < tola);
  if (!tgts) {
    Standard_Boolean dotpos = (dot > 0.);
    if (dotpos) {Stb = TopAbs_OUT; Sta = TopAbs_IN;}
    else        {Stb = TopAbs_IN; Sta = TopAbs_OUT;}
    return Standard_True;
  }
  
  // E and ES are tangent, curves are quadratic :transition is INTERNAL/EXTERNAL,
  // elsewhere                                  : transition is FOR/REV/INT/EXT
  // we then use curvatures to compute transition T :
  // xpu090299 PRO13455(E=e7, ES=e9, FS=f11)
  gp_Dir ntFS; ok = TopOpeBRepTool_TOOL::Nt(myuv,myFS,ntFS);
  if (!ok) return Standard_False;
  Standard_Real curvE;  ok = TopOpeBRepTool_TOOL::CurvE(myE,mypE,ntFS,curvE);
  if (!ok) return Standard_False;
  Standard_Real curvES; ok = TopOpeBRepTool_TOOL::CurvE(myES,mypES,ntFS,curvES);
  if (!ok) return Standard_False;  

  Standard_Boolean quadE  = TopOpeBRepTool_TOOL::IsQuad(myE);
  Standard_Boolean quadES = TopOpeBRepTool_TOOL::IsQuad(myES);
  if (quadE && quadES) { // should return INT/EXT
    TopAbs_State sta = TopAbs_UNKNOWN;
    Standard_Integer mkt = FUN_mkT2dquad(curvE,curvES);
    Standard_Boolean isOK  = FUN_mkT2dquad(myE,mypb,mypa,mypE, myES,mypES, mkt,xxES,myfactor,sta);
    if (isOK) {
      Stb = Sta = sta;
      return Standard_True;
    }
  }

  // !!!NYI: general case :
  // ----------------------
  // NYIKPART quadquad, only one state
  return Standard_False;
}

static Standard_Boolean FUN_getnearuv(const TopoDS_Face& f,const gp_Pnt2d& uv,
			 const Standard_Real factor,const Standard_Integer sta,const gp_Dir2d& duv, 
			 gp_Pnt2d& nearuv)
{
  BRepAdaptor_Surface bs(f);

  gp_Vec2d xuv = gp_Vec2d(duv).Multiplied(factor);
  if (sta == BEFORE) xuv.Reverse();
  nearuv = uv.Translated(xuv) ;

  Standard_Integer onu,onv; TopOpeBRepTool_TOOL::stuvF(uv,f, onu,onv);
  Standard_Boolean uok = (onu == 0);
  Standard_Boolean vok = (onv == 0);
  if (uok && vok) return Standard_True;

  Standard_Real nearu = nearuv.X(), nearv = nearuv.Y();

  if ((onu == INFFIRST)||(onu == SUPLAST)) {
    Standard_Boolean ucl = bs.IsUClosed();
    if (!ucl) return Standard_False;
    Standard_Real uper = bs.UPeriod();
    if (onu == INFFIRST) nearu += uper;
    else                 nearu -= uper;
  }

  if ((onv == INFFIRST)||(onv == SUPLAST)) {
    Standard_Boolean vcl = bs.IsVClosed();
    if (!vcl) return Standard_False;
    Standard_Real vper = bs.VPeriod();
    if (onv == INFFIRST) nearv += vper;
    else                 nearv -= vper;
  }
  nearuv = gp_Pnt2d(nearu,nearv);
  return Standard_True;
}//FUN_getnearuv
static Standard_Boolean FUN_tgef(const TopoDS_Face& f,
                                 const gp_Pnt2d& uv,
                                 const gp_Dir2d& duv,
                                 const Standard_Real factor,
//		                 const gp_Dir& tge,
		                 const gp_Dir& ,
		                 const gp_Dir& tg0,
		                 gp_Dir& tgef, 
		                 Standard_Integer& st)
{
  st = BEFORE;
  for (Standard_Integer nite = 1; nite <= 2; nite++) {
    if (nite == 2) st = AFTER;    
    gp_Pnt2d nearuv;
//    Standard_Real pn;
    Standard_Boolean mkp = FUN_getnearuv(f,uv,factor,st,duv, nearuv);
    if (!mkp) continue;
    gp_Dir nt; Standard_Boolean ok = TopOpeBRepTool_TOOL::Nt(nearuv,f,nt);
    if (!ok) return Standard_False;
    // recall : ntf^tge = tg0, (tgef(uv) = tge) 
    //          => near interference point, we assume nt^tgef(nearuv) = tg0 
    tgef = tg0.Crossed(nt);
    return Standard_True;    
  }
  return Standard_False;  
}//FUN_tgef
static Standard_Boolean FUN_mkT3dquad(const TopoDS_Edge& e,const Standard_Real pf,const Standard_Real pl,const Standard_Real par,
			 const TopoDS_Face& f,const gp_Pnt2d& uv,
			 const gp_Dir& tge,const gp_Dir& ntf,
			 const Standard_Integer mkt,const Standard_Real factor, TopAbs_State& sta)
     
{  
  // stb = state of point on e before pt / ef
  // = sta = state of point on e after pt / ef
  sta = TopAbs_UNKNOWN;
  gp_Dir xxef = ntf.Reversed();
  gp_Dir tg0 = ntf.Crossed(tge);

  gp_Dir tgae,tgaef;
  Standard_Boolean mke = (mkt==isINifh1) || (mkt==isON2ifss) || (mkt==isIN2ifss);
  if (mke) {
    Standard_Integer st = 0; gp_Dir tgnear; Standard_Boolean ok = FUN_tg(e,par,pf,pl,factor,tgnear,st);
    if (!ok) return Standard_False;
    tgae = (st == AFTER) ? tgnear : tgnear.Reversed();
  }
  Standard_Boolean mkef = (mkt==isINifh2) || (mkt==isON2ifss) || (mkt==isOU2ifss);
  if (mkef) { 
    // choice : tgdir(ef,uv) = tgdir(e,pare)
    Standard_Real fac3d = 0.01; //0.12345; not only planar faces
    gp_Dir2d duv; Standard_Boolean ok = TopOpeBRepTool_TOOL::Getduv(f,uv,tge,fac3d, duv);
    if (!ok) return Standard_False;
    gp_Dir tgnear; Standard_Integer st = 0; ok = FUN_tgef(f,uv,duv,factor, tge,tg0, tgnear,st);
    if (!ok) return Standard_False;
    tgaef = (st == AFTER) ? tgnear : tgnear.Reversed();
  }
  return (FUN_getsta(mkt,tgae,tgaef,xxef, sta));
}

static TopAbs_State FUN_stawithES(const gp_Dir& tgE,const gp_Dir& xxES,const Standard_Integer st)
{
  // prequesitory : FS and E are tangent at interference point
  // ---------------------------------------------------------
  // xxES : normal to ES oriented INSIDE 2d(FS)
  // tgE  : tangent to E at Pt
  // pt(st,E) (st=BEFORE,AFTER) has been found IN 3dFS()    
  
  TopAbs_State sta;
  Standard_Real prod = tgE.Dot(xxES);
  Standard_Real tola = FUN_tolang();
  if (Abs(prod) < tola) return TopAbs_UNKNOWN;
  Standard_Boolean positive = (prod > 0.);
  if (positive) sta = (st == BEFORE) ? TopAbs_OUT : TopAbs_IN; //T.Set(TopAbs_FORWARD);
  else          sta = (st == BEFORE) ? TopAbs_IN : TopAbs_OUT;//T.Set(TopAbs_REVERSED);
//  sta = (iP == BEFORE) ? T.Before() : T.After();
  return sta;
}//FUN_stawithES
static TopAbs_State FUN_stawithES(const gp_Dir& tgE,const gp_Dir& xxES,const Standard_Integer st,const TopAbs_State stt)
{
  TopAbs_State str = TopAbs_UNKNOWN;
  if (M_UNKNOWN(stt)) return str;

  TopAbs_State stES = FUN_stawithES(tgE,xxES,st);
  // we keep statx as IN or ON if xwithline is IN
  if (M_IN(stt) || M_ON(stt)) str = M_IN(stES) ? stt : TopAbs_OUT;
  return str;
}

static Standard_Boolean FUN_staproj(const TopoDS_Edge& e,const Standard_Real pf,const Standard_Real pl,const Standard_Real pe,
		       const Standard_Real factor, const Standard_Integer st, const TopoDS_Face& f, 
		       TopAbs_State& sta)
{
  Standard_Real par = 0.;
  Standard_Boolean ok = FUN_getnearpar(e,pe,pf,pl,factor,st, par);
  if (!ok) return Standard_False;
  gp_Pnt pt;
  ok = FUN_tool_value(par,e, pt);
  if (!ok) return Standard_False;
  gp_Pnt2d uv; ok = TopOpeBRepTool_TOOL::Getstp3dF(pt,f,uv,sta);
  return ok;
}

//=======================================================================
//function : MkT3dproj
//purpose  : can return in,out,on
//=======================================================================

Standard_Boolean TopOpeBRepTool_makeTransition::MkT3dproj(TopAbs_State& Stb,TopAbs_State& Sta) const
{
  Stb = Sta = TopAbs_UNKNOWN;
  Standard_Boolean okb = FUN_staproj(myE,mypb,mypa,mypE, myfactor,BEFORE,myFS, Stb);
  if (!okb) return Standard_False;
  Standard_Boolean oka = FUN_staproj(myE,mypb,mypa,mypE, myfactor,AFTER,myFS, Sta);
  if (!oka) return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : MkT3donE
//purpose  : 
//   <myE> is tangent to <myFS> at point P = Pt(<myuv>,<myFS>) = Pt(<mypE>,<myE>)
//   <tgE> = E's tangent at P,
//   <ntF> = <F>'s topological normal at P.

//   These define a plane Pln = (O = P, XY = (<ntF>,<tgE>)), 
//   the projection of <F> in Pln describes an bounding edge eF in 2dspace(Pln)

//   In thePlane :
//   P -> myuv
//   <ntF> -> 2d axis x
//   <tgE> -> 2d axis y
// ================================================================================

Standard_Boolean TopOpeBRepTool_makeTransition::MkT3onE(TopAbs_State& Stb,TopAbs_State& Sta) const
{
  if (isT2d) return Standard_False;
  gp_Vec tmp; Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(mypE,myE,tmp);
  if (!ok) return Standard_False;
  gp_Dir tgE(tmp); 
  gp_Dir ntFS; ok = TopOpeBRepTool_TOOL::Nt(myuv,myFS,ntFS);
  if (!ok) return Standard_False; 
  
  Standard_Real tola = FUN_tolang();
  Standard_Real dot = tgE.Dot(ntFS);
  
  if (Abs(dot) > tola) {
    Stb = (dot > 0) ? TopAbs_IN : TopAbs_OUT;
    Sta = (dot > 0) ? TopAbs_OUT : TopAbs_IN;
//    TopAbs_Orientation oE = (dot > 0) ? TopAbs_REVERSED : TopAbs_FORWARD;
//    T.Set(oE);
    return Standard_True;
  }
  
  //E is tangent to FS at interference point
  gp_Dir tg0 = ntFS.Crossed(tgE);
  Standard_Real curE; ok = TopOpeBRepTool_TOOL::CurvE(myE,mypE,tg0, curE);
  if (!ok) return Standard_False;
  Standard_Real curFS; Standard_Boolean direct; ok = TopOpeBRepTool_TOOL::CurvF(myFS,myuv,tg0, curFS,direct);
  if (!ok) return Standard_False;
    
  Standard_Boolean quadE   = TopOpeBRepTool_TOOL::IsQuad(myE);
  Standard_Boolean quadFS  = TopOpeBRepTool_TOOL::IsQuad(myFS);
  if (quadE && quadFS) { // should return INT/EXT
    Standard_Integer mkt = FUN_mkT2dquad(curE,curFS);    
    TopAbs_State sta = TopAbs_UNKNOWN;
    ok = FUN_mkT3dquad(myE,mypb,mypa,mypE, myFS,myuv, tgE,ntFS, mkt,myfactor,sta); 
    if (ok) {
      if (hasES) {
	gp_Dir xxES; Standard_Boolean isOK = TopOpeBRepTool_TOOL::XX(myuv,myFS, mypES,myES, xxES);
	if (!isOK) return Standard_False;
	Stb = FUN_stawithES(tgE,xxES,BEFORE,sta);
	Sta = FUN_stawithES(tgE,xxES,AFTER,sta);
      }
      else {
	Stb = Sta = sta; 
      }
      return Standard_True;
    }
  }
  
  // NYIGENERALCASE;
  // NYIKPART quadquad, only one state
  return Standard_False;
}

//=======================================================================
//function : MkTonE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_makeTransition::MkTonE(TopAbs_State& Stb,TopAbs_State& Sta) 
{
  Stb = Sta = TopAbs_UNKNOWN;
  if (isT2d) return (MkT2donE(Stb,Sta));

  Standard_Boolean ok = MkT3onE(Stb,Sta);
  if (!ok) ok = MkT3dproj(Stb, Sta);
//  if (!ok) return Standard_False;

  gp_Vec tmp; ok = TopOpeBRepTool_TOOL::TggeomE(mypE,myE,tmp);
  if (!ok) return Standard_False;
  gp_Dir tgE(tmp); 
  gp_Dir xxES; 
  if (hasES && ok) {ok = TopOpeBRepTool_TOOL::XX(myuv,myFS, mypES,myES, xxES);
		    if (!ok) return Standard_False;}
  
  Standard_Real delta = (1. - myfactor)/5.;  
  Standard_Boolean kob=Standard_False, koa=Standard_False;
  for (Standard_Integer nite = 1; nite <= 5; nite++) {
    kob = (Stb == TopAbs_ON)||(Stb == TopAbs_UNKNOWN);
    koa = (Sta == TopAbs_ON)||(Sta == TopAbs_UNKNOWN);
    if (!koa && !kob) return Standard_True;
    
    Standard_Boolean okb=Standard_True, oka=Standard_True;
    if (kob) {
      okb = FUN_staproj(myE,mypb,mypa,mypE, myfactor,BEFORE,myFS, Stb);
      if (okb && hasES) {TopAbs_State stb = Stb; Stb = FUN_stawithES(tgE,xxES,BEFORE,stb);}
    }
    if (koa) {
      oka = FUN_staproj(myE,mypb,mypa,mypE, myfactor,AFTER,myFS, Sta);
      if (oka && hasES) {TopAbs_State sta = Sta; Sta = FUN_stawithES(tgE,xxES,AFTER,sta);}
    }
    
    myfactor += delta;
  }//nite=1..5
  return Standard_False;
}

