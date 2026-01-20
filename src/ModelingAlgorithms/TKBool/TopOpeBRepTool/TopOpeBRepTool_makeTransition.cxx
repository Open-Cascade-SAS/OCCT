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
#include <BRepAdaptor_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <TopoDS_Face.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_makeTransition.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define BEFORE (1)
#define AFTER (2)

#define isINifh1 (1)
#define isINifh2 (2)
#define isON2 (21)
#define isON2ifss (10)
#define isIN2ifss (11)
#define isOU2ifss (12)

#define INFFIRST (-1)
#define SUPLAST (-2)
#define ONFIRST (1)
#define ONLAST (2)

#define FORWARD (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING (5)

#define M_ON(sta) (sta == TopAbs_ON)
#define M_IN(sta) (sta == TopAbs_IN)
#define M_FORWARD(sta) (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)
#define M_UNKNOWN(sta) (sta == TopAbs_UNKNOWN)

static double FUN_tolang()
{
  return Precision::Angular() * 1.e6; //=1.e-6 NYITOLXPU
}

//=================================================================================================

TopOpeBRepTool_makeTransition::TopOpeBRepTool_makeTransition() {}

//=================================================================================================

bool TopOpeBRepTool_makeTransition::Initialize(const TopoDS_Edge&  E,
                                                           const double pbef,
                                                           const double paft,
                                                           const double parE,
                                                           const TopoDS_Face&  FS,
                                                           const gp_Pnt2d&     uv,
                                                           const double factor)
{
  bool isdge = BRep_Tool::Degenerated(E);
  if (isdge)
    return false;

  myE      = E;
  mypb     = pbef;
  mypa     = paft;
  mypE     = parE;
  myFS     = FS;
  myuv     = uv;
  myfactor = factor;
  hasES    = false;

  bool facko = (factor < 0.) || (factor > 1.);
  if (facko)
    return false;

  bool ok = TopOpeBRepTool_TOOL::EdgeONFace(mypE, myE, myuv, FS, isT2d);
  return ok;
}

//=================================================================================================

void TopOpeBRepTool_makeTransition::Setfactor(const double factor)
{
  myfactor = factor;
}

//=================================================================================================

double TopOpeBRepTool_makeTransition::Getfactor() const
{
  return myfactor;
}

//=================================================================================================

bool TopOpeBRepTool_makeTransition::IsT2d() const
{
  return isT2d;
}

//=================================================================================================

bool TopOpeBRepTool_makeTransition::SetRest(const TopoDS_Edge&  ES,
                                                        const double parES)
{
  bool isdge = BRep_Tool::Degenerated(ES);
  if (isdge)
    return false;

  hasES = true;
  myES  = ES;
  mypES = parES;

  // nyi check <ES> is edge of <myFS>
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_makeTransition::HasRest() const
{
  return hasES;
}

static bool FUN_nullcurv(const double curv)
{
  double tol = Precision::Confusion() * 1.e+2; // NYITOLXPU
  return (curv < tol);
}

static int FUN_mkT2dquad(const double curvC1, const double curvC2)
{
  // !!! only for quadratic geometries :
  // prequesitory : C1 and C2 are ON given plane; they are tangent
  // at point pt, where curvatures are respectively curvC1,curvC2
  // stb = state of point on C1 before pt / C2
  // = sta = state of point on C1 after pt / C2

  bool nullc1 = FUN_nullcurv(curvC1);
  bool nullc2 = FUN_nullcurv(curvC2);
  if (nullc2 && nullc1)
    return isON2;
  if (nullc2)
    return isINifh1; // is IN if (dot=tg1(pt after).xx2 > 0)
  if (nullc1)
    return isINifh2; // is IN if (dot=tg2(pt after).xx2 < 0)

  bool samec = (std::abs(curvC2 - curvC1) < 1.e-2); // NYITOLXPU kpartkoletge
  if (samec)
    return isON2ifss; // is ON if curves are on same side/tg line
  if (curvC1 > curvC2)
    return isIN2ifss; // is IN if curves are on same side/tg line
  else
    return isOU2ifss; // is OU if curves are on same side/tg line
  //  return 0;
}

static bool FUN_getnearpar(const TopoDS_Edge&     e,
                                       const double    par,
                                       const double    f,
                                       const double    l,
                                       const double    factor,
                                       const int sta,
                                       double&         nearpar)
{
  // hyp : f < par < l
  BRepAdaptor_Curve bc(e);
  double     tol1d = bc.Resolution(bc.Tolerance());
  bool  onf   = (std::abs(par - f) < tol1d);
  bool  onl   = (std::abs(par - l) < tol1d);
  if (onf && (sta == BEFORE))
    return false;
  if (onl && (sta == AFTER))
    return false;
  // nearpar = (sta == BEFORE) ? ((1-factor)*par - factor*f) : ((1-factor)*par - factor*l);
  nearpar = (sta == BEFORE) ? (par - factor * (l - f)) : (par + factor * (l - f));
  return true;
}

static bool FUN_tg(const TopoDS_Edge&  e,
                               const double par,
                               const double f,
                               const double l,
                               const double factor,
                               gp_Dir&             tg,
                               int&   st)
{
  st = BEFORE;
  for (int nite = 1; nite <= 2; nite++)
  {
    if (nite == 2)
      st = AFTER;
    double    pn  = 0.;
    bool mkp = FUN_getnearpar(e, par, f, l, factor, st, pn);
    if (!mkp)
      continue;
    gp_Vec           tmp;
    bool ok = TopOpeBRepTool_TOOL::TggeomE(pn, e, tmp);
    if (!ok)
      continue;
    tg = gp_Dir(tmp);
    return true;
  }
  return false;
}

static bool FUN_getsta(const int mkt,
                                   const gp_Dir&          tga1,
                                   const gp_Dir&          tga2,
                                   const gp_Dir&          xx2,
                                   TopAbs_State&          sta)
{
  if (mkt == isINifh1)
  {
    // curv(e1) > 0.
    // is IN if (dot=tg1(pt after).xx2 > 0)
    double dot = tga1.Dot(xx2);
    sta               = (dot > 0) ? TopAbs_IN : TopAbs_OUT;
    return true;
  }
  else if (mkt == isINifh2)
  {
    // curv(e2) > 0.
    // is IN if (dot=tg2(pt after).xx2 < 0)
    double dot = tga2.Dot(xx2);
    sta               = (dot < 0) ? TopAbs_IN : TopAbs_OUT;
    return true;
  }
  else if (mkt == isON2ifss)
  {
    // curv(e1), curv(e2) > 0.
    // is ON if curves are on same side/tg line
    bool ssided = (tga1.Dot(tga2) > 0);
    sta                     = ssided ? TopAbs_ON : TopAbs_IN;
    return true;
  }
  else if (mkt == isIN2ifss)
  {
    // stag = IN if curves are on same side/tg line
    double dot = tga1.Dot(xx2);
    sta               = (dot < 0) ? TopAbs_OUT : TopAbs_IN;
    return true;
  }
  else if (mkt == isOU2ifss)
  {
    // stag = OU if curves are on same side/tg line
    double dot = tga2.Dot(xx2);
    sta               = (dot < 0) ? TopAbs_IN : TopAbs_OUT;
    return true;
  }
  else
  { // mkt == isON2
    sta = TopAbs_ON;
    return true;
  }
}

static bool FUN_mkT2dquad(const TopoDS_Edge&     e1,
                                      const double    par1,
                                      const double    f1,
                                      const double    l1,
                                      const TopoDS_Edge&     e2,
                                      const double    par2,
                                      const int mkt,
                                      const gp_Dir&          xx2,
                                      const double    factor,
                                      TopAbs_State&          sta)
{
  sta = TopAbs_UNKNOWN;

  // !!! only for quadratic geometries :
  // stb = state of point on e1 before pt / e2
  // = sta = state of point on e1 after pt / e2

  gp_Dir           tga1, tga2;
  bool mk1 = (mkt == isINifh1) || (mkt == isON2ifss) || (mkt == isIN2ifss);
  if (mk1)
  {
    int st1 = 0;
    gp_Dir           tgnear1;
    bool ok = FUN_tg(e1, par1, f1, l1, factor, tgnear1, st1);
    if (!ok)
      return false;
    tga1 = (st1 == AFTER) ? tgnear1 : tgnear1.Reversed();
  }
  bool mk2 = (mkt == isINifh2) || (mkt == isON2ifss) || (mkt == isOU2ifss);
  if (mk2)
  {
    double f2, l2;
    FUN_tool_bounds(e2, f2, l2);
    int st2 = 0;
    gp_Dir           tgnear2;
    bool ok = FUN_tg(e2, par2, f2, l2, factor, tgnear2, st2);
    if (!ok)
      return false;
    tga2 = (st2 == AFTER) ? tgnear2 : tgnear2.Reversed();
  }
  return (FUN_getsta(mkt, tga1, tga2, xx2, sta));
}

//=================================================================================================

bool TopOpeBRepTool_makeTransition::MkT2donE(TopAbs_State& Stb, TopAbs_State& Sta) const
{
  if (!isT2d)
    return false;

  // E is IN 2d(FS), meets no restriction at given point :
  if (!hasES)
  {
    Stb = Sta = TopAbs_IN;
    return true;
  }

  // E is IN 2d(FS), meets restriction ES at given point :
  int oriESFS = TopOpeBRepTool_TOOL::OriinSor(myES, myFS, true);
  if (oriESFS == 0)
    return false;

  // ES is closing edge for FS, or ES is INTERNAL in FS :
  if (oriESFS == INTERNAL)
  {
    Stb = Sta = TopAbs_IN;
    return true;
  }
  else if (oriESFS == CLOSING)
  {
    Stb = Sta = TopAbs_IN;
    return true;
  }

  gp_Vec           tmp;
  bool ok = TopOpeBRepTool_TOOL::TggeomE(mypE, myE, tmp);
  if (!ok)
    return false;
  gp_Dir tgE(tmp);
  gp_Dir xxES;
  ok = TopOpeBRepTool_TOOL::XX(myuv, myFS, mypES, myES, xxES);
  if (!ok)
    return false;

  double tola = FUN_tolang();
  double dot  = tgE.Dot(xxES);

  // E and ES are not tangent at interference point :
  bool tgts = (std::abs(dot) < tola);
  if (!tgts)
  {
    bool dotpos = (dot > 0.);
    if (dotpos)
    {
      Stb = TopAbs_OUT;
      Sta = TopAbs_IN;
    }
    else
    {
      Stb = TopAbs_IN;
      Sta = TopAbs_OUT;
    }
    return true;
  }

  // E and ES are tangent, curves are quadratic :transition is INTERNAL/EXTERNAL,
  // elsewhere                                  : transition is FOR/REV/INT/EXT
  // we then use curvatures to compute transition T :
  // xpu090299 PRO13455(E=e7, ES=e9, FS=f11)
  gp_Dir ntFS;
  ok = TopOpeBRepTool_TOOL::Nt(myuv, myFS, ntFS);
  if (!ok)
    return false;
  double curvE;
  ok = TopOpeBRepTool_TOOL::CurvE(myE, mypE, ntFS, curvE);
  if (!ok)
    return false;
  double curvES;
  ok = TopOpeBRepTool_TOOL::CurvE(myES, mypES, ntFS, curvES);
  if (!ok)
    return false;

  bool quadE  = TopOpeBRepTool_TOOL::IsQuad(myE);
  bool quadES = TopOpeBRepTool_TOOL::IsQuad(myES);
  if (quadE && quadES)
  { // should return INT/EXT
    TopAbs_State     sta = TopAbs_UNKNOWN;
    int mkt = FUN_mkT2dquad(curvE, curvES);
    bool isOK =
      FUN_mkT2dquad(myE, mypb, mypa, mypE, myES, mypES, mkt, xxES, myfactor, sta);
    if (isOK)
    {
      Stb = Sta = sta;
      return true;
    }
  }

  // !!!NYI: general case :
  // ----------------------
  // NYIKPART quadquad, only one state
  return false;
}

static bool FUN_getnearuv(const TopoDS_Face&     f,
                                      const gp_Pnt2d&        uv,
                                      const double    factor,
                                      const int sta,
                                      const gp_Dir2d&        duv,
                                      gp_Pnt2d&              nearuv)
{
  BRepAdaptor_Surface bs(f);

  gp_Vec2d xuv = gp_Vec2d(duv).Multiplied(factor);
  if (sta == BEFORE)
    xuv.Reverse();
  nearuv = uv.Translated(xuv);

  int onu, onv;
  TopOpeBRepTool_TOOL::stuvF(uv, f, onu, onv);
  bool uok = (onu == 0);
  bool vok = (onv == 0);
  if (uok && vok)
    return true;

  double nearu = nearuv.X(), nearv = nearuv.Y();

  if ((onu == INFFIRST) || (onu == SUPLAST))
  {
    bool ucl = bs.IsUClosed();
    if (!ucl)
      return false;
    double uper = bs.UPeriod();
    if (onu == INFFIRST)
      nearu += uper;
    else
      nearu -= uper;
  }

  if ((onv == INFFIRST) || (onv == SUPLAST))
  {
    bool vcl = bs.IsVClosed();
    if (!vcl)
      return false;
    double vper = bs.VPeriod();
    if (onv == INFFIRST)
      nearv += vper;
    else
      nearv -= vper;
  }
  nearuv = gp_Pnt2d(nearu, nearv);
  return true;
} // FUN_getnearuv

static bool FUN_tgef(const TopoDS_Face&  f,
                                 const gp_Pnt2d&     uv,
                                 const gp_Dir2d&     duv,
                                 const double factor,
                                 //		                 const gp_Dir& tge,
                                 const gp_Dir&,
                                 const gp_Dir&     tg0,
                                 gp_Dir&           tgef,
                                 int& st)
{
  st = BEFORE;
  for (int nite = 1; nite <= 2; nite++)
  {
    if (nite == 2)
      st = AFTER;
    gp_Pnt2d nearuv;
    //    double pn;
    bool mkp = FUN_getnearuv(f, uv, factor, st, duv, nearuv);
    if (!mkp)
      continue;
    gp_Dir           nt;
    bool ok = TopOpeBRepTool_TOOL::Nt(nearuv, f, nt);
    if (!ok)
      return false;
    // recall : ntf^tge = tg0, (tgef(uv) = tge)
    //          => near interference point, we assume nt^tgef(nearuv) = tg0
    tgef = tg0.Crossed(nt);
    return true;
  }
  return false;
} // FUN_tgef

static bool FUN_mkT3dquad(const TopoDS_Edge&     e,
                                      const double    pf,
                                      const double    pl,
                                      const double    par,
                                      const TopoDS_Face&     f,
                                      const gp_Pnt2d&        uv,
                                      const gp_Dir&          tge,
                                      const gp_Dir&          ntf,
                                      const int mkt,
                                      const double    factor,
                                      TopAbs_State&          sta)

{
  // stb = state of point on e before pt / ef
  // = sta = state of point on e after pt / ef
  sta         = TopAbs_UNKNOWN;
  gp_Dir xxef = ntf.Reversed();
  gp_Dir tg0  = ntf.Crossed(tge);

  gp_Dir           tgae, tgaef;
  bool mke = (mkt == isINifh1) || (mkt == isON2ifss) || (mkt == isIN2ifss);
  if (mke)
  {
    int st = 0;
    gp_Dir           tgnear;
    bool ok = FUN_tg(e, par, pf, pl, factor, tgnear, st);
    if (!ok)
      return false;
    tgae = (st == AFTER) ? tgnear : tgnear.Reversed();
  }
  bool mkef = (mkt == isINifh2) || (mkt == isON2ifss) || (mkt == isOU2ifss);
  if (mkef)
  {
    // choice : tgdir(ef,uv) = tgdir(e,pare)
    double    fac3d = 0.01; // 0.12345; not only planar faces
    gp_Dir2d         duv;
    bool ok = TopOpeBRepTool_TOOL::Getduv(f, uv, tge, fac3d, duv);
    if (!ok)
      return false;
    gp_Dir           tgnear;
    int st = 0;
    ok                  = FUN_tgef(f, uv, duv, factor, tge, tg0, tgnear, st);
    if (!ok)
      return false;
    tgaef = (st == AFTER) ? tgnear : tgnear.Reversed();
  }
  return (FUN_getsta(mkt, tgae, tgaef, xxef, sta));
}

static TopAbs_State FUN_stawithES(const gp_Dir& tgE, const gp_Dir& xxES, const int st)
{
  // prequesitory : FS and E are tangent at interference point
  // ---------------------------------------------------------
  // xxES : normal to ES oriented INSIDE 2d(FS)
  // tgE  : tangent to E at Pt
  // pt(st,E) (st=BEFORE,AFTER) has been found IN 3dFS()

  TopAbs_State  sta;
  double prod = tgE.Dot(xxES);
  double tola = FUN_tolang();
  if (std::abs(prod) < tola)
    return TopAbs_UNKNOWN;
  bool positive = (prod > 0.);
  if (positive)
    sta = (st == BEFORE) ? TopAbs_OUT : TopAbs_IN; // T.Set(TopAbs_FORWARD);
  else
    sta = (st == BEFORE) ? TopAbs_IN : TopAbs_OUT; // T.Set(TopAbs_REVERSED);
  //  sta = (iP == BEFORE) ? T.Before() : T.After();
  return sta;
} // FUN_stawithES

static TopAbs_State FUN_stawithES(const gp_Dir&          tgE,
                                  const gp_Dir&          xxES,
                                  const int st,
                                  const TopAbs_State     stt)
{
  TopAbs_State str = TopAbs_UNKNOWN;
  if (M_UNKNOWN(stt))
    return str;

  TopAbs_State stES = FUN_stawithES(tgE, xxES, st);
  // we keep statx as IN or ON if xwithline is IN
  if (M_IN(stt) || M_ON(stt))
    str = M_IN(stES) ? stt : TopAbs_OUT;
  return str;
}

static bool FUN_staproj(const TopoDS_Edge&     e,
                                    const double    pf,
                                    const double    pl,
                                    const double    pe,
                                    const double    factor,
                                    const int st,
                                    const TopoDS_Face&     f,
                                    TopAbs_State&          sta)
{
  double    par = 0.;
  bool ok  = FUN_getnearpar(e, pe, pf, pl, factor, st, par);
  if (!ok)
    return false;
  gp_Pnt pt;
  ok = FUN_tool_value(par, e, pt);
  if (!ok)
    return false;
  gp_Pnt2d uv;
  ok = TopOpeBRepTool_TOOL::Getstp3dF(pt, f, uv, sta);
  return ok;
}

//=======================================================================
// function : MkT3dproj
// purpose  : can return in,out,on
//=======================================================================

bool TopOpeBRepTool_makeTransition::MkT3dproj(TopAbs_State& Stb,
                                                          TopAbs_State& Sta) const
{
  Stb = Sta            = TopAbs_UNKNOWN;
  bool okb = FUN_staproj(myE, mypb, mypa, mypE, myfactor, BEFORE, myFS, Stb);
  if (!okb)
    return false;
  bool oka = FUN_staproj(myE, mypb, mypa, mypE, myfactor, AFTER, myFS, Sta);
  if (!oka)
    return false;
  return true;
}

//=======================================================================
// function : MkT3donE
// purpose  :
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

bool TopOpeBRepTool_makeTransition::MkT3onE(TopAbs_State& Stb, TopAbs_State& Sta) const
{
  if (isT2d)
    return false;
  gp_Vec           tmp;
  bool ok = TopOpeBRepTool_TOOL::TggeomE(mypE, myE, tmp);
  if (!ok)
    return false;
  gp_Dir tgE(tmp);
  gp_Dir ntFS;
  ok = TopOpeBRepTool_TOOL::Nt(myuv, myFS, ntFS);
  if (!ok)
    return false;

  double tola = FUN_tolang();
  double dot  = tgE.Dot(ntFS);

  if (std::abs(dot) > tola)
  {
    Stb = (dot > 0) ? TopAbs_IN : TopAbs_OUT;
    Sta = (dot > 0) ? TopAbs_OUT : TopAbs_IN;
    //    TopAbs_Orientation oE = (dot > 0) ? TopAbs_REVERSED : TopAbs_FORWARD;
    //    T.Set(oE);
    return true;
  }

  // E is tangent to FS at interference point
  gp_Dir        tg0 = ntFS.Crossed(tgE);
  double curE;
  ok = TopOpeBRepTool_TOOL::CurvE(myE, mypE, tg0, curE);
  if (!ok)
    return false;
  double    curFS;
  bool direct;
  ok = TopOpeBRepTool_TOOL::CurvF(myFS, myuv, tg0, curFS, direct);
  if (!ok)
    return false;

  bool quadE  = TopOpeBRepTool_TOOL::IsQuad(myE);
  bool quadFS = TopOpeBRepTool_TOOL::IsQuad(myFS);
  if (quadE && quadFS)
  { // should return INT/EXT
    int mkt = FUN_mkT2dquad(curE, curFS);
    TopAbs_State     sta = TopAbs_UNKNOWN;
    ok = FUN_mkT3dquad(myE, mypb, mypa, mypE, myFS, myuv, tgE, ntFS, mkt, myfactor, sta);
    if (ok)
    {
      if (hasES)
      {
        gp_Dir           xxES;
        bool isOK = TopOpeBRepTool_TOOL::XX(myuv, myFS, mypES, myES, xxES);
        if (!isOK)
          return false;
        Stb = FUN_stawithES(tgE, xxES, BEFORE, sta);
        Sta = FUN_stawithES(tgE, xxES, AFTER, sta);
      }
      else
      {
        Stb = Sta = sta;
      }
      return true;
    }
  }

  // NYIGENERALCASE;
  // NYIKPART quadquad, only one state
  return false;
}

//=================================================================================================

bool TopOpeBRepTool_makeTransition::MkTonE(TopAbs_State& Stb, TopAbs_State& Sta)
{
  Stb = Sta = TopAbs_UNKNOWN;
  if (isT2d)
    return (MkT2donE(Stb, Sta));

  bool ok = MkT3onE(Stb, Sta);
  if (!ok)
    ok = MkT3dproj(Stb, Sta);
  //  if (!ok) return false;

  gp_Vec tmp;
  ok = TopOpeBRepTool_TOOL::TggeomE(mypE, myE, tmp);
  if (!ok)
    return false;
  gp_Dir tgE(tmp);
  gp_Dir xxES;
  if (hasES && ok)
  {
    ok = TopOpeBRepTool_TOOL::XX(myuv, myFS, mypES, myES, xxES);
    if (!ok)
      return false;
  }

  double    delta = (1. - myfactor) / 5.;
  bool kob = false, koa = false;
  for (int nite = 1; nite <= 5; nite++)
  {
    kob = (Stb == TopAbs_ON) || (Stb == TopAbs_UNKNOWN);
    koa = (Sta == TopAbs_ON) || (Sta == TopAbs_UNKNOWN);
    if (!koa && !kob)
      return true;

    bool okb = true, oka = true;
    if (kob)
    {
      okb = FUN_staproj(myE, mypb, mypa, mypE, myfactor, BEFORE, myFS, Stb);
      if (okb && hasES)
      {
        TopAbs_State stb = Stb;
        Stb              = FUN_stawithES(tgE, xxES, BEFORE, stb);
      }
    }
    if (koa)
    {
      oka = FUN_staproj(myE, mypb, mypa, mypE, myfactor, AFTER, myFS, Sta);
      if (oka && hasES)
      {
        TopAbs_State sta = Sta;
        Sta              = FUN_stawithES(tgE, xxES, AFTER, sta);
      }
    }

    myfactor += delta;
  } // nite=1..5
  return false;
}
