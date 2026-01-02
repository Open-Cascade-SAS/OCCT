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
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <NCollection_IndexedMap.hxx>
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
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#include <algorithm>
#define M_FORWARD(sta) (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

#define FORWARD (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING (5)

static bool FUN_nullprodv(const double prodv)
{
  //  double tola = Precision::Angular()*1.e+1; // NYI
  double tola = 1.e-6; // NYI NYI NYI : for case cto 012 I2
  return (std::abs(prodv) < tola);
}

// modified by NIZNHY-PKV Fri Aug  4 11:22:57 2000 from

//=================================================================================================

static bool CheckEdgeLength(const TopoDS_Edge& E)
{
  BRepAdaptor_Curve BC(E);

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aM;
  TopExp::MapShapes(E, TopAbs_VERTEX, aM);
  int    i, anExtent, aN = 10;
  double ln = 0., d, t, f, l, dt;
  anExtent  = aM.Extent();

  if (anExtent != 1)
    return true;

  gp_Pnt p1, p2;
  f  = BC.FirstParameter();
  l  = BC.LastParameter();
  dt = (l - f) / aN;

  BC.D0(f, p1);
  for (i = 1; i <= aN; i++)
  {
    t = f + i * dt;

    if (i == aN)
      BC.D0(l, p2);
    else
      BC.D0(t, p2);

    d = p1.Distance(p2);
    ln += d;
    p1 = p2;
  }

  return (ln > Precision::Confusion());
}

// modified by NIZNHY-PKV Fri Aug  4 11:23:07 2000 to

//=================================================================================================

int TopOpeBRepTool_TOOL::OriinSor(const TopoDS_Shape& sub,
                                  const TopoDS_Shape& S,
                                  const bool          checkclo)
{
  if (checkclo)
  {
    bool Sclosed = false;
    if (S.ShapeType() == TopAbs_EDGE)
    {
      if (sub.ShapeType() != TopAbs_VERTEX)
        return 0;

      TopoDS_Vertex vclo;
      Sclosed = TopOpeBRepTool_TOOL::ClosedE(TopoDS::Edge(S), vclo);
      if (Sclosed)
        if (sub.IsSame(vclo))
          return CLOSING;
    }
    else if (S.ShapeType() == TopAbs_FACE)
    {
      if (sub.ShapeType() != TopAbs_EDGE)
        return 0;

      Sclosed = ClosedS(TopoDS::Face(S));
      if (Sclosed)
        if (IsClosingE(TopoDS::Edge(sub), TopoDS::Face(S)))
          return CLOSING;
    }
  }

  TopExp_Explorer ex(S, sub.ShapeType());
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Shape& ssub = ex.Current();
    bool                same = ssub.IsSame(sub);
    if (!same)
      continue;
    TopAbs_Orientation osub = ssub.Orientation();
    if (M_FORWARD(osub))
      return FORWARD;
    else if (M_REVERSED(osub))
      return REVERSED;
    else if (M_INTERNAL(osub))
      return INTERNAL;
    else if (M_EXTERNAL(osub))
      return EXTERNAL;
  }
  return 0;
}

//=================================================================================================

int TopOpeBRepTool_TOOL::OriinSorclosed(const TopoDS_Shape& sub, const TopoDS_Shape& S)
{
  if (S.ShapeType() == TopAbs_EDGE)
  {
    if (sub.ShapeType() != TopAbs_VERTEX)
      return 0;
  }
  else if (S.ShapeType() == TopAbs_FACE)
  {
    if (sub.ShapeType() != TopAbs_EDGE)
      return 0;
  }
  TopoDS_Iterator it(S);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& ssub  = it.Value();
    bool                equal = ssub.IsEqual(sub);
    if (!equal)
      continue;
    TopAbs_Orientation osub = ssub.Orientation();
    if (M_FORWARD(osub))
      return FORWARD;
    else if (M_REVERSED(osub))
      return REVERSED;
  }
  return 0;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::ClosedE(const TopoDS_Edge& E, TopoDS_Vertex& vclo)
{
  // returns true if <E> has a closing vertex <vclosing>
  //  return E.IsClosed();
  bool isdgE = BRep_Tool::Degenerated(E);
  if (isdgE)
    return false;

  TopoDS_Shape vv;
  vclo.Nullify();
  TopExp_Explorer ex(E, TopAbs_VERTEX);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Shape& v = ex.Current();
    if (M_INTERNAL(v.Orientation()))
      continue;
    if (vv.IsNull())
      vv = v;
    else if (v.IsSame(vv))
    {
      vclo = TopoDS::Vertex(vv);
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::ClosedS(const TopoDS_Face& F)
{
  occ::handle<Geom_Surface> S = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  if (S.IsNull())
    return false;
  bool uclosed = S->IsUClosed();
  if (uclosed)
    uclosed = S->IsUPeriodic();
  bool vclosed = S->IsVClosed();
  if (vclosed)
    vclosed = S->IsVPeriodic();
  return (uclosed || vclosed);
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::IsClosingE(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  int             nbocc = 0;
  TopExp_Explorer exp(F, TopAbs_EDGE);
  for (; exp.More(); exp.Next())
    if (exp.Current().IsSame(E))
      nbocc++;
  if (nbocc != 2)
    return false;
  return BRep_Tool::IsClosed(E, F);
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::IsClosingE(const TopoDS_Edge&  E,
                                     const TopoDS_Shape& W,
                                     const TopoDS_Face&  F)
{
  int             nbocc = 0;
  TopExp_Explorer exp(W, TopAbs_EDGE);
  for (; exp.More(); exp.Next())
    if (exp.Current().IsSame(E))
      nbocc++;
  if (nbocc != 2)
    return false;
  return BRep_Tool::IsClosed(E, F);
}

//=================================================================================================

void TopOpeBRepTool_TOOL::Vertices(const TopoDS_Edge& E, NCollection_Array1<TopoDS_Shape>& Vces)
{
  // Returns vertices (F,R) if E is FORWARD
  //                  (R,V) if E is REVERSED
  TopAbs_Orientation oriE = E.Orientation();
  TopoDS_Vertex      v1, v2;
  TopExp::Vertices(E, v1, v2);

  if (M_INTERNAL(oriE) || M_EXTERNAL(oriE))
  {
    Vces.ChangeValue(1) = v1;
    Vces.ChangeValue(2) = v2;
  }

  double par1 = BRep_Tool::Parameter(v1, E);
  double par2 = BRep_Tool::Parameter(v2, E);
#ifdef OCCT_DEBUG
//  if (par1>par2) std::cout<<"TopOpeBRepTool_TOOL::Vertices ERROR"<<std::endl;
#endif
  int ivparSMA = (par1 < par2) ? FORWARD : REVERSED;
  int ivparSUP = (par1 < par2) ? REVERSED : FORWARD;
  if (M_REVERSED(oriE))
  {
    ivparSMA = (ivparSMA == FORWARD) ? REVERSED : FORWARD;
    ivparSUP = (ivparSUP == REVERSED) ? FORWARD : REVERSED;
  }
  Vces.ChangeValue(ivparSMA) = v1;
  Vces.ChangeValue(ivparSUP) = v2;
}

//=================================================================================================

TopoDS_Vertex TopOpeBRepTool_TOOL::Vertex(const int Iv, const TopoDS_Edge& E)
{
  NCollection_Array1<TopoDS_Shape> Vces(1, 2);
  Vertices(E, Vces);
  TopoDS_Vertex V = TopoDS::Vertex(Vces(Iv));
  return V;
}

//=================================================================================================

double TopOpeBRepTool_TOOL::ParE(const int Iv, const TopoDS_Edge& E)
{
  const TopoDS_Vertex& v = Vertex(Iv, E);
  return (BRep_Tool::Parameter(v, E));
}

//=================================================================================================

int TopOpeBRepTool_TOOL::OnBoundary(const double par, const TopoDS_Edge& e)
{
  BRepAdaptor_Curve bc(e);
  bool              closed = bc.IsClosed();
  double            first  = bc.FirstParameter();
  double            last   = bc.LastParameter();
  double            tole   = bc.Tolerance();
  double            tolp   = bc.Resolution(tole);

  bool onf  = std::abs(par - first) < tolp;
  bool onl  = std::abs(par - last) < tolp;
  bool onfl = (onf || onl);
  if (onfl && closed)
    return CLOSING;
  if (onf)
    return FORWARD;
  if (onl)
    return REVERSED;
  if ((first < par) && (par < last))
    return INTERNAL;
  return EXTERNAL;
}

static void FUN_tool_sortVonE(NCollection_List<TopoDS_Shape>& lov, const TopoDS_Edge& E)
{
  NCollection_DataMap<int, TopoDS_Shape> mapiv;  // mapiv.Find(iV) = V
  NCollection_IndexedMap<double>         mappar; // mappar.FindIndex(parV) = iV

  for (NCollection_List<TopoDS_Shape>::Iterator itlove(lov); itlove.More(); itlove.Next())
  {
    const TopoDS_Vertex& v   = TopoDS::Vertex(itlove.Value());
    double               par = BRep_Tool::Parameter(v, E);
    int                  iv  = mappar.Add(par);
    mapiv.Bind(iv, v);
  }
  int                        nv = mapiv.Extent();
  NCollection_Array1<double> tabpar(1, nv);
  //  for (int i = 1; i <= nv; i++) {
  int i;
  for (i = 1; i <= nv; i++)
  {
    double p = mappar.FindKey(i);
    tabpar.SetValue(i, p);
  }

  NCollection_List<TopoDS_Shape> newlov;
  std::sort(tabpar.begin(), tabpar.end());
  for (i = 1; i <= nv; i++)
  {
    double              par = tabpar.Value(i);
    int                 iv  = mappar.FindIndex(par);
    const TopoDS_Shape& v   = mapiv.Find(iv);
    newlov.Append(v);
  }
  lov.Clear();
  lov.Append(newlov);
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::SplitE(const TopoDS_Edge& Eanc, NCollection_List<TopoDS_Shape>& Splits)
{
  // prequesitory : <Eanc> is a valid edge.
  TopAbs_Orientation oEanc       = Eanc.Orientation();
  TopoDS_Shape       aLocalShape = Eanc.Oriented(TopAbs_FORWARD);
  TopoDS_Edge        EFOR        = TopoDS::Edge(aLocalShape);
  //  TopoDS_Edge EFOR = TopoDS::Edge(Eanc.Oriented(TopAbs_FORWARD));
  NCollection_List<TopoDS_Shape> lov;
  TopExp_Explorer                exv(EFOR, TopAbs_VERTEX);
  for (; exv.More(); exv.Next())
  {
    const TopoDS_Shape& v = exv.Current();
    lov.Append(v);
  }
  int nv = lov.Extent();
  if (nv <= 2)
    return false;

  ::FUN_tool_sortVonE(lov, EFOR);

  TopoDS_Vertex                            v0;
  NCollection_List<TopoDS_Shape>::Iterator itlov(lov);
  if (itlov.More())
  {
    v0 = TopoDS::Vertex(itlov.Value());
    itlov.Next();
  }
  else
    return false;

  for (; itlov.More(); itlov.Next())
  {
    TopoDS_Vertex v = TopoDS::Vertex(itlov.Value());

    // prequesitory: par0 < par
    double par0 = BRep_Tool::Parameter(v0, EFOR);
    double par  = BRep_Tool::Parameter(v, EFOR);

    // here, ed has the same geometries than Ein, but with no subshapes.
    TopoDS_Edge ed;
    FUN_ds_CopyEdge(EFOR, ed);
    BRep_Builder BB;
    v0.Orientation(TopAbs_FORWARD);
    BB.Add(ed, v0);
    FUN_ds_Parameter(ed, v0, par0);
    v.Orientation(TopAbs_REVERSED);
    BB.Add(ed, v);
    FUN_ds_Parameter(ed, v, par);
    Splits.Append(ed.Oriented(oEanc));
    v0 = v;
  }
  return true;
}

//=================================================================================================

gp_Pnt2d TopOpeBRepTool_TOOL::UVF(const double par, const TopOpeBRepTool_C2DF& C2DF)
{
  double                           f, l, tol;
  const occ::handle<Geom2d_Curve>& PC = C2DF.PC(f, l, tol);
  gp_Pnt2d                         UV;
  PC->D0(par, UV);
  return UV;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::ParISO(const gp_Pnt2d&    uv,
                                 const TopoDS_Edge& E,
                                 const TopoDS_Face& F,
                                 double&            par)
{
  par = 1.e7;
  bool     isou, isov;
  gp_Dir2d d2d;
  gp_Pnt2d o2d;
  bool     uviso = TopOpeBRepTool_TOOL::UVISO(E, F, isou, isov, d2d, o2d);
  if (!uviso)
    return false;
  if (isou)
  {
    par = (uv.Y() - o2d.Y());
    if (d2d.Y() < 0)
      par = -par;
  }
  if (isov)
  {
    par = (uv.X() - o2d.X());
    if (d2d.X() < 0)
      par = -par;
  }
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::ParE2d(const gp_Pnt2d&    p2d,
                                 const TopoDS_Edge& E,
                                 const TopoDS_Face& F,
                                 double&            par,
                                 double&            dist)
{
  // Avoid projections if possible :
  BRepAdaptor_Curve2d              BC2d(E, F);
  GeomAbs_CurveType                CT  = BC2d.GetType();
  const occ::handle<Geom2d_Curve>& C2d = BC2d.Curve();
  if (CT == GeomAbs_Line)
  {
    bool     isoU, isoV;
    gp_Pnt2d Loc;
    gp_Dir2d dir2d;
    TopOpeBRepTool_TOOL::UVISO(C2d, isoU, isoV, dir2d, Loc);
    if (isoU)
    {
      par  = p2d.Y() - Loc.Y();
      dist = std::abs(p2d.X() - Loc.X());
    }
    if (isoV)
    {
      par  = p2d.X() - Loc.X();
      dist = std::abs(p2d.Y() - Loc.Y());
    }
    if (isoU || isoV)
      return true;
  }

  Geom2dAPI_ProjectPointOnCurve proj(p2d, C2d);
  dist = p2d.Distance(proj.NearestPoint());
  par  = proj.LowerDistanceParameter();
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::TgINSIDE(const TopoDS_Vertex& v,
                                   const TopoDS_Edge&   E,
                                   gp_Vec&              Tg,
                                   int&                 OvinE)
{
  TopoDS_Shape aLocalShape = E.Oriented(TopAbs_FORWARD);
  TopoDS_Edge  EFOR        = TopoDS::Edge(aLocalShape);
  //  TopoDS_Edge EFOR = TopoDS::Edge(E.Oriented(TopAbs_FORWARD));
  int ovE = TopOpeBRepTool_TOOL::OriinSor(v, EFOR, true);
  if (ovE == 0)
    return false;
  OvinE  = ovE;
  int iv = 0;
  if (ovE == CLOSING)
    iv = FORWARD;
  else if ((ovE == FORWARD) || (ovE == REVERSED))
    iv = ovE;
  double parE;
  if (iv == 0)
    parE = BRep_Tool::Parameter(v, E);
  else
    parE = TopOpeBRepTool_TOOL::ParE(iv, EFOR);
  bool ok = TopOpeBRepTool_TOOL::TggeomE(parE, EFOR, Tg);
  if (!ok)
    return false;
  if (ovE == REVERSED)
    Tg.Reverse();
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::TggeomE(const double par, const BRepAdaptor_Curve& BC, gp_Vec& Tg)
{
  // #ifdef OCCT_DEBUG
  //   GeomAbs_CurveType ct =
  // #endif
  //                          BC.GetType();
  // #ifdef OCCT_DEBUG
  //   bool apoles = (ct == GeomAbs_BezierCurve)||(ct == GeomAbs_BSplineCurve);
  // #endif

  double f = BC.FirstParameter(), l = BC.LastParameter();
  double tolE = BC.Tolerance();
  double tolp = BC.Resolution(tolE);

  bool onf      = std::abs(f - par) < tolp;
  bool onl      = std::abs(l - par) < tolp;
  bool inbounds = (f < par) && (par < l);

  if ((!inbounds) && (!onf) && (!onl))
    return false;
  double thepar = par;

  gp_Pnt thepnt;
  BC.D1(thepar, thepnt, Tg);
  Tg.Normalize();
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::TggeomE(const double par, const TopoDS_Edge& E, gp_Vec& Tg)
{
  bool isdgE = BRep_Tool::Degenerated(E);
  if (isdgE)
    return false;

  BRepAdaptor_Curve BC(E);
  // modified by NIZNHY-PKV Fri Aug  4 09:49:31 2000 f
  if (!CheckEdgeLength(E))
  {
    return false;
  }
  // modified by NIZNHY-PKV Fri Aug  4 09:49:36 2000 t

  return (TopOpeBRepTool_TOOL::TggeomE(par, BC, Tg));
}

//=================================================================================================

gp_Vec2d TopOpeBRepTool_TOOL::Tg2d(const int                  iv,
                                   const TopoDS_Edge&         E,
                                   const TopOpeBRepTool_C2DF& C2DF)
{
  double                           f, l, tol;
  const occ::handle<Geom2d_Curve>& PC  = C2DF.PC(f, l, tol);
  double                           par = TopOpeBRepTool_TOOL::ParE(iv, E);
  gp_Pnt2d                         UV;
  gp_Vec2d                         tg2d;
  PC->D1(par, UV, tg2d);
  gp_Dir2d d2d(tg2d);
  return d2d;
}

//=================================================================================================

gp_Vec2d TopOpeBRepTool_TOOL::Tg2dApp(const int                  iv,
                                      const TopoDS_Edge&         E,
                                      const TopOpeBRepTool_C2DF& C2DF,
                                      const double               factor)
{
  double                           f, l, tol;
  const occ::handle<Geom2d_Curve>& PC = C2DF.PC(f, l, tol);

  int    iOOv  = (iv == 1) ? 2 : 1;
  double par   = TopOpeBRepTool_TOOL::ParE(iv, E);
  double OOpar = TopOpeBRepTool_TOOL::ParE(iOOv, E);
  double parE  = (1 - factor) * par + factor * OOpar;

  gp_Pnt2d UV;
  gp_Vec2d tg2d;
  PC->D1(parE, UV, tg2d);
  gp_Dir2d d2d(tg2d);

  // modified by NIZHNY-MZV  Wed May 24 12:52:18 2000
  //   TopAbs_Orientation oE = E.Orientation();
  //   if (M_REVERSED(oE)) d2d.Reverse();
  // we remove this line because we want to know original tangent
  return d2d;
}

//=================================================================================================

gp_Vec2d TopOpeBRepTool_TOOL::tryTg2dApp(const int                  iv,
                                         const TopoDS_Edge&         E,
                                         const TopOpeBRepTool_C2DF& C2DF,
                                         const double               factor)
{
  double                           f, l, tol;
  const occ::handle<Geom2d_Curve>& PC     = C2DF.PC(f, l, tol);
  bool                             isquad = FUN_tool_quad(PC);
  bool                             line   = FUN_tool_line(PC);
  if (!isquad || line)
    return TopOpeBRepTool_TOOL::Tg2d(iv, E, C2DF);
  return TopOpeBRepTool_TOOL::Tg2dApp(iv, E, C2DF, factor);
}

//=================================================================================================

int TopOpeBRepTool_TOOL::tryOriEinF(const double par, const TopoDS_Edge& e, const TopoDS_Face& f)
{
  // ------------------------------------------------------------
  // 1) <e> is a subshape of <f>
  // 2) else, compute oriEinF, using <e>'s 2d rep on <f>
  //    PREQUESITORY : <e> must have a pcurve on <f>.
  // ------------------------------------------------------------
  bool checkclo = true;
  int  oeinf    = TopOpeBRepTool_TOOL::OriinSor(e, f, checkclo);
  if (oeinf != 0)
    return oeinf;

  occ::handle<Geom2d_Curve> pc;
  double                    pf, pl, tol;
  bool                      hasold = FC2D_HasOldCurveOnSurface(e, f, pc);
  if (!hasold)
    return 0;
  pc = FC2D_EditableCurveOnSurface(e, f, pf, pl, tol);

  // n2d is such that (p2d,oop2d) is oriented INSIDE F
  gp_Pnt2d uv;
  gp_Vec2d tg2d;
  pc->D1(par, uv, tg2d);
  gp_Vec2d n2d(gp_Dir2d(-tg2d.Y(), tg2d.X()));

  double delta = TopOpeBRepTool_TOOL::minDUV(f);
  delta *= 1.e-1;
  gp_Pnt2d ouv         = uv.Translated(delta * n2d);
  bool     outuvbounds = TopOpeBRepTool_TOOL::outUVbounds(ouv, f);
  oeinf                = (outuvbounds) ? 2 : 1;
  return oeinf;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::NgApp(const double       par,
                                const TopoDS_Edge& e,
                                const TopoDS_Face& f,
                                const double       tola,
                                gp_Dir&            ngApp)
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

  occ::handle<Geom_Surface> S = TopOpeBRepTool_ShapeTool::BASISSURFACE(f);
  if (S.IsNull())
    return false;

  bool fplane = FUN_tool_plane(f);
  if (fplane)
    return false;

  // NYI : for bspline surfaces, use a evolutive parameter
  //       on curve to find out "significant" tangents
  bool fquad = FUN_tool_quad(f);
  if (!fquad)
    return false;
  // <pc> :
  occ::handle<Geom2d_Curve> pc;
  double                    pf, pl, tol;
  bool                      hasold = FC2D_HasOldCurveOnSurface(e, f, pc);
  if (!hasold)
    return false;
  pc = FC2D_EditableCurveOnSurface(e, f, pf, pl, tol);
  // <orieinf> :
  TopoDS_Shape aLocalShape = f.Oriented(TopAbs_FORWARD);
  int          orieinf     = TopOpeBRepTool_TOOL::tryOriEinF(par, e, TopoDS::Face(aLocalShape));
  //  int orieinf =
  //  TopOpeBRepTool_TOOL::tryOriEinF(par,e,TopoDS::Face(f.Oriented(TopAbs_FORWARD)));
  if (orieinf == 0)
    return false;
  // <uv> :
  gp_Pnt2d uv;
  bool     ok = FUN_tool_paronEF(e, par, f, uv);
  if (!ok)
    return false;
  // <ng> :
  gp_Dir ng = FUN_tool_ngS(uv, S);
  if (!ok)
    return false;

  // <n2dinsideS> :
  gp_Vec2d tg2d;
  pc->D1(par, uv, tg2d);
  gp_Dir2d n2dinsideS = FUN_tool_nC2dINSIDES(gp_Dir2d(tg2d));
  if (orieinf == 2)
    n2dinsideS.Reverse();
  //<duv> : '
  double   eps = 0.45678;
  gp_Vec2d duv = gp_Vec2d(n2dinsideS).Multiplied(eps);

  // cto009S4 : we need an iterative process to get other normal vector
  int    nmax  = 5;
  bool   same  = false;
  double delta = 0.45678;
  for (int i = 1; i <= nmax; i++)
  {

    gp_Pnt2d newuv = uv.Translated(duv);
    gp_Vec   newng = FUN_tool_ngS(newuv, S);
    same           = ng.IsEqual(newng, tola);
    bool okk       = (newng.Magnitude() > tola);
    if (!same && okk)
    {
      ngApp = gp_Dir(newng);
      break;
    }
    delta *= 1.25; //  NYI
    duv = gp_Vec2d(n2dinsideS).Multiplied(delta);

  } // i=1..nmax
  return !same;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::tryNgApp(const double       par,
                                   const TopoDS_Edge& e,
                                   const TopoDS_Face& f,
                                   const double       tola,
                                   gp_Dir&            Ng)
{
  gp_Pnt2d uv;
  bool     ok = FUN_tool_paronEF(e, par, f, uv);
  if (!ok)
    return false;
  gp_Dir ng(FUN_tool_nggeomF(uv, f));
#ifdef OCCT_DEBUG
  gp_Dir ngApp;
#endif
  ok = TopOpeBRepTool_TOOL::NgApp(par, e, f, tola, Ng);
  if (!ok)
    Ng = ng;
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::IsQuad(const TopoDS_Edge& E)
{
  BRepAdaptor_Curve bc(E);
  return (FUN_quadCT(bc.GetType()));
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::IsQuad(const TopoDS_Face& F)
{
  occ::handle<Geom_Surface> S = TopOpeBRepTool_ShapeTool::BASISSURFACE(F);
  return (FUN_tool_quad(S));
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::CurvE(const TopoDS_Edge& E,
                                const double       par,
                                const gp_Dir&      tg0,
                                double&            curv)
{
  curv = 0.;
  BRepAdaptor_Curve BAC(E);
  GeomAbs_CurveType CT   = BAC.GetType();
  bool              line = (CT == GeomAbs_Line);
  double            tola = Precision::Angular() * 1.e3; // NYITOLXPU
  if (line)
  {
    gp_Dir dir = BAC.Line().Direction();
    double dot = dir.Dot(tg0);
    return std::abs(1 - dot) >= tola;
  }

  BRepLProp_CLProps clprops(BAC, par, 2, Precision::Confusion());
  bool              tgdef = clprops.IsTangentDefined();
  if (!tgdef)
    return false;
  curv = std::abs(clprops.Curvature());

  double tol      = Precision::Confusion() * 1.e+2; // NYITOLXPU
  bool   nullcurv = (curv < tol);
  if (nullcurv)
  {
    curv = 0.;
    return true;
  }

  gp_Dir N;
  clprops.Normal(N);
  gp_Dir T;
  clprops.Tangent(T);
  gp_Dir axis  = N ^ T;
  double dot   = std::abs(axis.Dot(tg0));
  nullcurv     = dot < tola;
  bool maxcurv = std::abs(1 - dot) < tola;
  if (nullcurv)
  {
    curv = 0.;
    return true;
  }
  if (maxcurv)
  {
    return true;
  }
  return false; // nyi general case
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
static bool FUN_analyticcS(const gp_Pnt2d&                  uv0,
                           const occ::handle<Geom_Surface>& S,
                           const gp_Dir&                    ngS,
                           const gp_Dir&                    tg0,
                           double&                          curv,
                           bool&                            direct) // dummy if !analyticcontour
{
  curv   = 0.;
  direct = true;
  // purpose : Returns true if theContour is analytic, and
  //           then computes its curvature <curv>.
  occ::handle<Geom_Surface> su = TopOpeBRepTool_ShapeTool::BASISSURFACE(S);
  if (S.IsNull())
    return true;
  GeomAdaptor_Surface GS(su);
  GeomAbs_SurfaceType ST    = GS.GetType();
  bool                plane = (ST == GeomAbs_Plane);
  bool                cyl   = (ST == GeomAbs_Cylinder);
  bool                cone  = (ST == GeomAbs_Cone);
  bool                sphe  = (ST == GeomAbs_Sphere);
  bool                torus = (ST == GeomAbs_Torus);

  bool curvdone = false;
  if (plane)
  {
    curv     = 0.;
    curvdone = true;
  }
  if (cyl || cone || torus)
  {
    gp_Dir axis;
    if (cyl)
    {
      const gp_Cylinder& cycy = GS.Cylinder();
      axis                    = cycy.Axis().Direction();
      direct                  = cycy.Direct();
    }
    if (cone)
    {
      const gp_Cone& coco = GS.Cone();
      axis                = coco.Axis().Direction();
      direct              = coco.Direct();
    }
    if (torus)
    {
      const gp_Torus& toto = GS.Torus();
      axis                 = toto.Axis().Direction();
      direct               = toto.Direct();
    }
    double prod       = axis.Dot(tg0);
    bool   isMaxAcurv = FUN_nullprodv(1 - std::abs(prod));
    bool   nullcurv   = FUN_nullprodv(prod);

    double prod2 = ngS.Dot(tg0);
    if (cyl || cone)
      nullcurv = nullcurv || FUN_nullprodv(1 - std::abs(prod2));

    if (nullcurv)
    {
      curv     = 0.;
      curvdone = true;
    }
    if (isMaxAcurv)
    {
      GeomLProp_SLProps slprops(S, uv0.X(), uv0.Y(), 2, Precision::Confusion());
      bool              curdef = slprops.IsCurvatureDefined();
      if (curdef)
      {
        double minAcurv = std::abs(slprops.MinCurvature());
        double maxAcurv = std::abs(slprops.MaxCurvature());
        bool   isAmax   = (maxAcurv > minAcurv);
        curv            = isAmax ? maxAcurv : minAcurv;
      }
      curvdone = true;
    }
  }
  if (sphe)
  {
    const gp_Sphere& spsp = GS.Sphere();
    curv                  = 1. / spsp.Radius();
    curvdone              = true;
    direct                = spsp.Direct();
  }

  return curvdone;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::CurvF(const TopoDS_Face& F,
                                const gp_Pnt2d&    uv,
                                const gp_Dir&      tg0,
                                double&            curv,
                                bool&              direct)
{
  curv                          = 0.;
  gp_Dir                    ngS = FUN_tool_nggeomF(uv, F);
  occ::handle<Geom_Surface> S   = TopOpeBRepTool_ShapeTool::BASISSURFACE(F);
  if (S.IsNull())
    return false;
  // purpose : Computes theContour's curvature,
  //          returns false if the compute fails.

  double tola = 1.e-6; // NYITOLXPU

  bool analyticcontour = FUN_analyticcS(uv, S, ngS, tg0, curv, direct);
  if (analyticcontour)
    return true;

  GeomLProp_SLProps slprops(S, uv.X(), uv.Y(), 2, Precision::Confusion());
  bool              curdef = slprops.IsCurvatureDefined();
  if (curdef)
  {
    gp_Dir npl = tg0;

    gp_Dir MaxD, MinD;
    slprops.CurvatureDirections(MaxD, MinD);
    double mincurv = slprops.MinCurvature();
    double maxcurv = slprops.MaxCurvature();

    gp_Vec Dmax = ngS ^ MaxD, Dmin = ngS ^ MinD; // xpu180898 : cto015G2
    double dotmax   = Dmax.Dot(npl);             // MaxD.Dot(npl); -xpu180898
    bool   iscurmax = std::abs(1 - dotmax) < tola;
    if (iscurmax)
    {
      direct = (maxcurv < 0.);
      curv   = std::abs(maxcurv);
    }
    double dotmin   = Dmin.Dot(npl); // MinD.Dot(npl); -xpu180898
    bool   iscurmin = std::abs(1 - dotmin) < tola;
    if (iscurmin)
    {
      direct = (mincurv < 0.);
      curv   = std::abs(mincurv);
    }
    curdef = iscurmax || iscurmin;
    // -------------
    // NYI : !curdef
    // -------------
  }
  return curdef;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::UVISO(const occ::handle<Geom2d_Curve>& PC,
                                bool&                            isoU,
                                bool&                            isoV,
                                gp_Dir2d&                        d2d,
                                gp_Pnt2d&                        o2d)
{
  isoU = isoV = false;
  if (PC.IsNull())
    return false;
  occ::handle<Geom2d_Curve>  LLL      = BASISCURVE2D(PC);
  occ::handle<Standard_Type> T2       = LLL->DynamicType();
  bool                       isline2d = (T2 == STANDARD_TYPE(Geom2d_Line));
  if (!isline2d)
    return false;

  occ::handle<Geom2d_Line> L = occ::down_cast<Geom2d_Line>(LLL);
  d2d                        = L->Direction();
  isoU                       = (std::abs(d2d.X()) < Precision::Parametric(Precision::Confusion()));
  isoV                       = (std::abs(d2d.Y()) < Precision::Parametric(Precision::Confusion()));
  bool isoUV                 = isoU || isoV;
  if (!isoUV)
    return false;

  o2d = L->Location();
  return true;
}

bool TopOpeBRepTool_TOOL::UVISO(const TopoDS_Edge& E,
                                const TopoDS_Face& F,
                                bool&              isoU,
                                bool&              isoV,
                                gp_Dir2d&          d2d,
                                gp_Pnt2d&          o2d)
{
  //  double f,l,tol; occ::handle<Geom2d_Curve> PC = FC2D_CurveOnSurface(E,F,f,l,tol);
  occ::handle<Geom2d_Curve> PC;
  double                    f, l, tol;
  bool                      hasold = FC2D_HasOldCurveOnSurface(E, F, PC);
  PC                               = FC2D_EditableCurveOnSurface(E, F, f, l, tol);
  if (!hasold)
    FC2D_AddNewCurveOnSurface(PC, E, F, f, l, tol);

  bool iso = UVISO(PC, isoU, isoV, d2d, o2d);
  return iso;
}

bool TopOpeBRepTool_TOOL::UVISO(const TopOpeBRepTool_C2DF& C2DF,
                                bool&                      isoU,
                                bool&                      isoV,
                                gp_Dir2d&                  d2d,
                                gp_Pnt2d&                  o2d)
{
  double                           f, l, tol;
  const occ::handle<Geom2d_Curve>& PC = C2DF.PC(f, l, tol);
  // #ifdef OCCT_DEBUG
  //   const iso = UVISO(PC,isoU,isoV,d2d,o2d);
  // #else
  const bool iso = UVISO(PC, isoU, isoV, d2d, o2d);
  // #endif
  return iso;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::IsonCLO(const occ::handle<Geom2d_Curve>& PC,
                                  const bool                       onU,
                                  const double                     xfirst,
                                  const double                     xperiod,
                                  const double                     xtol)
{
  bool     isou, isov;
  gp_Pnt2d o2d;
  gp_Dir2d d2d;
  bool     isouv = UVISO(PC, isou, isov, d2d, o2d);
  if (!isouv)
    return false;
  bool onX = (onU && isou) || ((!onU) && isov);
  if (!onX)
    return false;
  double dxx = 0;
  if (onU)
    dxx = std::abs(o2d.X() - xfirst);
  else
    dxx = std::abs(o2d.Y() - xfirst);

  bool onclo = (dxx < xtol);
  onclo      = onclo || (std::abs(xperiod - dxx) < xtol);
  return onclo;
}

bool TopOpeBRepTool_TOOL::IsonCLO(const TopOpeBRepTool_C2DF& C2DF,
                                  const bool                 onU,
                                  const double               xfirst,
                                  const double               xperiod,
                                  const double               xtol)
{
  double                           f, l, tol;
  const occ::handle<Geom2d_Curve>& PC    = C2DF.PC(f, l, tol);
  bool                             onclo = IsonCLO(PC, onU, xfirst, xperiod, xtol);
  return onclo;
}

//=================================================================================================

void TopOpeBRepTool_TOOL::TrslUV(const gp_Vec2d& t2d, TopOpeBRepTool_C2DF& C2DF)
{
  double                    f, l, tol;
  occ::handle<Geom2d_Curve> PC = C2DF.PC(f, l, tol);
  PC->Translate(t2d);
  C2DF.SetPC(PC, f, l, tol);
}

bool TopOpeBRepTool_TOOL::TrslUVModifE(const gp_Vec2d& t2d, const TopoDS_Face& F, TopoDS_Edge& E)
{
  double                    f, l, tol;
  occ::handle<Geom2d_Curve> PC = FC2D_CurveOnSurface(E, F, f, l, tol);
  //  occ::handle<Geom2d_Curve> PC; double f,l,tol;

  if (PC.IsNull())
    return false;
  PC->Translate(t2d);
  //  occ::handle<Geom2d_Curve> toclear; BB.UpdateEdge(E,toclear,F,tole);
  BRep_Builder BB;
  BB.UpdateEdge(E, PC, F, tol);
  return true;
}

//=================================================================================================

double TopOpeBRepTool_TOOL::Matter(const gp_Vec& d1, const gp_Vec& dR2, const gp_Vec& Ref)
{
  gp_Vec d2 = dR2.Reversed();

  double tola  = Precision::Angular();
  double ang   = d1.Angle(d2);
  bool   equal = (ang < tola);
  if (equal)
    return 0.;
  bool oppo = ((M_PI - ang) < tola);
  if (oppo)
    return M_PI;

  ang = d1.AngleWithRef(d2, Ref);
  if (ang < 0)
    ang = 2. * M_PI + ang;
  return ang;
}

//=================================================================================================

double TopOpeBRepTool_TOOL::Matter(const gp_Vec2d& d1, const gp_Vec2d& dR2)
{
  gp_Vec v1  = gp_Vec(d1.X(), d1.Y(), 0.);
  gp_Vec vR2 = gp_Vec(dR2.X(), dR2.Y(), 0.);
  gp_Vec Ref(0., 0., 1.);

  double ang = TopOpeBRepTool_TOOL::Matter(v1, vR2, Ref);
  return ang;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::Matter(const gp_Dir& xx1,
                                 const gp_Dir& nt1,
                                 const gp_Dir& xx2,
                                 const gp_Dir& nt2,
                                 const double  tola,
                                 double&       ang)
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

  gp_Dir z1   = xx1 ^ nt1;
  gp_Dir z2   = xx2 ^ nt2;
  double dot  = z2.Dot(z1);
  bool   oppo = (dot < 0.);
  if (!oppo)
    return false;

  // -nti points towards 3dmatter(fi)
  // => zi = xxi^nti gives the opposite sense for the compute of the matter angle
  z1.Reverse();
  ang = xx1.AngleWithRef(xx2, z1);
  if (std::abs(ang) < tola)
  {
    ang = 0.;
    return true;
  }
  if (ang < 0)
    ang = 2. * M_PI + ang;

  return true;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::Getduv(const TopoDS_Face& f,
                                 const gp_Pnt2d&    uv,
                                 const gp_Vec&      dir,
                                 const double       factor,
                                 gp_Dir2d&          duv)
{
  bool quad = TopOpeBRepTool_TOOL::IsQuad(f);
  if (!quad)
    return false;
  Bnd_Box bndf;
  BRepBndLib::AddClose(f, bndf);
  double f1, f2, f3, l1, l2, l3;
  bndf.Get(f1, f2, f3, l1, l2, l3);
  gp_Vec d123(f1 - l1, f2 - l2, f3 - l3);

  gp_Pnt p;
  FUN_tool_value(uv, f, p);
  p.Translate(dir.Multiplied(factor));
  double   d;
  gp_Pnt2d uvtr;
  FUN_tool_projPonF(p, f, uvtr, d);
  double tolf = BRep_Tool::Tolerance(f);
  tolf *= 1.e2; // NYIXPUTOL
  if (d > tolf)
    return false;

  gp_Vec2d                  DUV(uv, uvtr);
  occ::handle<Geom_Surface> S = TopOpeBRepTool_ShapeTool::BASISSURFACE(f);
  if ((S->IsUPeriodic()) && (std::abs(DUV.X()) > S->UPeriod() / 2.))
  {
    double U1 = uv.X(), U2 = uvtr.X(), period = S->UPeriod();
    ElCLib::AdjustPeriodic(0., period, Precision::PConfusion(), U1, U2);
    double dx = U2 - U1;
    if (dx > period / 2.)
      dx -= period;
    DUV.SetX(dx);
  }
  if ((S->IsVPeriodic()) && (std::abs(DUV.Y()) > S->VPeriod() / 2.))
  {
    double V1 = uv.Y(), V2 = uvtr.Y(), period = S->VPeriod();
    ElCLib::AdjustPeriodic(0., period, Precision::PConfusion(), V1, V2);
    double dy = V2 - V1;
    if (dy > period / 2.)
      dy -= period;
    DUV.SetY(dy);
  }
  duv = gp_Dir2d(DUV);

  return true;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::uvApp(const TopoDS_Face& f,
                                const TopoDS_Edge& e,
                                const double       pare,
                                const double       eps,
                                gp_Pnt2d&          uvapp)
{
  // uv :
  bool ok = FUN_tool_paronEF(e, pare, f, uvapp);
  if (!ok)
    return false;
  gp_Vec2d dxx;
  ok = FUN_tool_getdxx(f, e, pare, dxx);
  if (!ok)
    return false;
  uvapp.Translate(dxx.Multiplied(eps));
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::XX(const gp_Pnt2d&    uv,
                             const TopoDS_Face& f,
                             const double       par,
                             const TopoDS_Edge& e,
                             gp_Dir&            XX)
{
  // ng(uv):
  gp_Vec ng     = FUN_tool_nggeomF(uv, f);
  gp_Vec geomxx = FUN_tool_getgeomxx(f, e, par, ng);

  double tol    = Precision::Confusion() * 1.e2; // NYITOL
  bool   nullng = (geomxx.Magnitude() < tol);
  if (nullng)
    return false;

  TopAbs_Orientation oef;
  bool               ok = FUN_tool_orientEinFFORWARD(e, f, oef);
  if (!ok)
    return false;
  XX = gp_Dir(geomxx);
  if (M_REVERSED(oef))
    XX.Reverse();
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::Nt(const gp_Pnt2d& uv, const TopoDS_Face& f, gp_Dir& normt)
{
  gp_Vec nggeom;
  bool   ok = TopOpeBRepTool_TOOL::NggeomF(uv, f, nggeom);
  if (!ok)
    return false;
  normt = gp_Dir(nggeom);
  if (M_REVERSED(f.Orientation()))
    normt.Reverse();
  return true;
}

//=================================================================================================

static bool FUN_ngF(const gp_Pnt2d& uv, const TopoDS_Face& F, gp_Vec& ngF)
{
  BRepAdaptor_Surface bs(F);
  double              tol3d = bs.Tolerance();
  double              tolu  = bs.UResolution(tol3d);
  double              tolv  = bs.VResolution(tol3d);

  // ###############################
  // nyi : all geometries are direct
  // ###############################
  gp_Pnt p;
  gp_Vec d1u, d1v;
  bs.D1(uv.X(), uv.Y(), p, d1u, d1v);

  double delta = TopOpeBRepTool_TOOL::minDUV(F);
  delta *= 1.e-1;

  double du    = d1u.Magnitude();
  double dv    = d1v.Magnitude();
  bool   kpart = (du < tolu) || (dv < tolv);
  if (kpart)
  {
    GeomAbs_SurfaceType ST = bs.GetType();
    if (ST == GeomAbs_Cone)
    {
      bool nullx = (std::abs(uv.X()) < tolu);
      bool apex  = nullx && (std::abs(uv.Y()) < tolv);
      if (apex)
      {
        const gp_Dir axis = bs.Cone().Axis().Direction();
        gp_Vec       ng(axis);
        ng.Reverse();
        ngF = ng;
        return true;
      }
      else if (du < tolu)
      {
        double x = uv.X();

        double y  = uv.Y();
        double vf = bs.FirstVParameter();

        if (std::abs(vf - y) < tolu)
          vf += delta;
        else
          vf -= delta;

        // modified by NIZHNY-MZV  Fri Nov 26 12:38:55 1999
        y = vf;
        bs.D1(x, y, p, d1u, d1v);
        gp_Vec ng = d1u ^ d1v;

        ngF = ng;
        return true;
      }
    }
    if (ST == GeomAbs_Sphere)
    {
      double pisur2 = M_PI * .5;
      double u = uv.X(), v = uv.Y();
      bool   vpisur2      = (std::abs(v - pisur2) < tolv);
      bool   vmoinspisur2 = (std::abs(v + pisur2) < tolv);
      bool   apex         = vpisur2 || vmoinspisur2;
      if (apex)
      {
        gp_Pnt center = bs.Sphere().Location();
        gp_Pnt value  = bs.Value(u, v);
        gp_Vec ng(center, value);
        ngF = ng;
        return true;
      }
    }
#ifdef OCCT_DEBUG
    std::cout << "FUN_tool_nggeomF NYI" << std::endl;
#endif
    return false;
  } // kpart

  gp_Dir udir(d1u);
  gp_Dir vdir(d1v);
  ngF = gp_Vec(gp_Dir(udir ^ vdir));
  return true;
}

bool TopOpeBRepTool_TOOL::NggeomF(const gp_Pnt2d& uv, const TopoDS_Face& f, gp_Vec& ng)
{
  return FUN_ngF(uv, f, ng);
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::Matter(const TopoDS_Face& f1,
                                 const TopoDS_Face& f2,
                                 const TopoDS_Edge& e,
                                 const double       par,
                                 const double       tola,
                                 double&            ang)
{
  gp_Dir xx1, xx2;
  gp_Dir nt1, nt2;

  double   tolf1 = BRep_Tool::Tolerance(f1) * 1.e2; // nyitolxpu
  gp_Pnt2d uv1;
  bool     ok1 = FUN_tool_paronEF(e, par, f1, uv1, tolf1);
  if (!ok1)
    return false;
  ok1 = TopOpeBRepTool_TOOL::Nt(uv1, f1, nt1);
  if (!ok1)
    return false;
  ok1 = TopOpeBRepTool_TOOL::XX(uv1, f1, par, e, xx1);
  if (!ok1)
    return false;

  double   tolf2 = BRep_Tool::Tolerance(f2) * 2.e2; // nyitolxpu
  gp_Pnt2d uv2;
  bool     ok2 = FUN_tool_paronEF(e, par, f2, uv2, tolf2);
  if (!ok2)
    return false;
  ok2 = TopOpeBRepTool_TOOL::Nt(uv2, f2, nt2);
  if (!ok2)
    return false;
  ok2 = TopOpeBRepTool_TOOL::XX(uv2, f2, par, e, xx2);
  if (!ok2)
    return false;

  return (TopOpeBRepTool_TOOL::Matter(xx1, nt1, xx2, nt2, tola, ang));
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::MatterKPtg(const TopoDS_Face& f1,
                                     const TopoDS_Face& f2,
                                     const TopoDS_Edge& e,
                                     double&            ang)
{
  double f, l;
  FUN_tool_bounds(e, f, l);
  double x    = 0.45678;
  double pare = (1 - x) * f + x * l;

  double eps = 0.123; // NYIXPU190199

  // double tola = Precision::Angular()*1.e3;

  gp_Pnt2d uv1;
  FUN_tool_paronEF(e, pare, f1, uv1);
  gp_Dir nt1;
  bool   ok1 = TopOpeBRepTool_TOOL::Nt(uv1, f1, nt1);
  if (!ok1)
    return false;
  gp_Pnt2d uvapp1;
  ok1 = TopOpeBRepTool_TOOL::uvApp(f1, e, pare, eps, uvapp1);
  if (!ok1)
    return false;
  gp_Pnt pf1;
  FUN_tool_value(uvapp1, f1, pf1);

  gp_Pnt2d uv2;
  double   d;
  bool     ok2 = FUN_tool_projPonF(pf1, f2, uv2, d);
  gp_Pnt   pf2;
  FUN_tool_value(uv2, f2, pf2);
  if (!ok2)
    return false;

  gp_Dir v12(gp_Vec(pf1, pf2));
  double dot = v12.Dot(nt1);
  ang        = (dot < 0.) ? 0. : 2. * M_PI;

  //  gp_Dir nt1; ok1 = TopOpeBRepTool_TOOL::Nt(uv1,f1,nt1);
  //  if (!ok1) return false;
  //  gp_Dir xx1; ok1 = TopOpeBRepTool_TOOL::XX(uv1,f1,pare,e,xx1);
  //  if (!ok1) return false;
  //  gp_Pnt2d uv2; bool ok2 = TopOpeBRepTool_TOOL::uvApp(f2,e,pare,eps,uv2);
  //  if (!ok2) return false;
  //  gp_Dir nt2; ok2 = TopOpeBRepTool_TOOL::Nt(uv2,f2,nt2);
  //  if (!ok2) return false;
  //  gp_Dir xx2; ok2 = TopOpeBRepTool_TOOL::XX(uv2,f2,pare,e,xx2);
  //  if (!ok2) return false;
  //  double angapp; bool ok = TopOpeBRepTool_TOOL::Matter(xx1,nt1,
  //  xx2,nt2,tola,angapp); if (!ok) return false; bool is0 = (std::abs(angapp)
  //  < std::abs(2.*M_PI-angapp)); ang = is0 ? 0. : 2.*M_PI;
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::Getstp3dF(const gp_Pnt&      p,
                                    const TopoDS_Face& f,
                                    gp_Pnt2d&          uv,
                                    TopAbs_State&      st)
// classification solide de <P> / <F>
{
  st           = TopAbs_UNKNOWN;
  double tol3d = BRep_Tool::Tolerance(f);
  // EXPENSIVE : calls an extrema
  double d;
  bool   ok = FUN_tool_projPonF(p, f, uv, d);
  if (!ok)
    return false;
  if (d < tol3d)
  {
    st = TopAbs_ON;
    return true;
  }

  gp_Pnt ppr;
  ok = FUN_tool_value(uv, f, ppr);
  if (!ok)
    return false;

  gp_Dir ntf;
  ok = TopOpeBRepTool_TOOL::Nt(uv, f, ntf);
  if (!ok)
    return false;

  gp_Dir dppr(gp_Vec(p, ppr));
  double dot   = dppr.Dot(ntf);
  bool   isOUT = (dot < 0.);
  st           = (isOUT ? TopAbs_OUT : TopAbs_IN);
  return true;
}

//=================================================================================================

void TopOpeBRepTool_TOOL::MkShell(const NCollection_List<TopoDS_Shape>& lF, TopoDS_Shape& She)
{
  BRep_Builder BB;
  BB.MakeShell(TopoDS::Shell(She));
  for (NCollection_List<TopoDS_Shape>::Iterator li(lF); li.More(); li.Next())
    BB.Add(She, li.Value());
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::Remove(NCollection_List<TopoDS_Shape>& loS, const TopoDS_Shape& toremove)
{
  NCollection_List<TopoDS_Shape>::Iterator it(loS);
  bool                                     found = false;
  while (it.More())
  {
    if (it.Value().IsEqual(toremove))
    {
      loS.Remove(it);
      found = true;
    }
    else
      it.Next();
  }
  return found;
}

//=================================================================================================

double TopOpeBRepTool_TOOL::minDUV(const TopoDS_Face& F)
{
  BRepAdaptor_Surface BS(F);
  double              delta = BS.LastUParameter() - BS.FirstUParameter();
  double              tmp   = BS.LastVParameter() - BS.FirstVParameter();
  delta                     = (tmp < delta) ? tmp : delta;
  return delta;
}

//=================================================================================================

#define INFFIRST (-1)
#define SUPLAST (-2)
#define ONFIRST (1)
#define ONLAST (2)

void TopOpeBRepTool_TOOL::stuvF(const gp_Pnt2d& uv, const TopoDS_Face& f, int& onU, int& onV)
{
  BRepAdaptor_Surface bs(f);
  onU = onV   = 0;
  double tolf = bs.Tolerance();
  double tolu = bs.UResolution(tolf), tolv = bs.VResolution(tolf);
  double u = uv.X(), v = uv.Y();
  double uf = bs.FirstUParameter(), ul = bs.LastUParameter(), vf = bs.FirstVParameter(),
         vl = bs.LastVParameter();
  bool onuf = (std::abs(uf - u) < tolu), onul = (std::abs(ul - u) < tolu);
  bool onvf = (std::abs(vf - v) < tolv), onvl = (std::abs(vl - v) < tolv);
  if (onuf)
    onU = ONFIRST;
  if (onul)
    onU = ONLAST;
  if (onvf)
    onV = ONFIRST;
  if (onvl)
    onV = ONLAST;
  if (u < (uf - tolu))
    onU = INFFIRST;
  if (u > (ul + tolu))
    onU = SUPLAST;
  if (v < (vf - tolv))
    onV = INFFIRST;
  if (v > (vl + tolv))
    onV = SUPLAST;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::outUVbounds(const gp_Pnt2d& uv, const TopoDS_Face& F)
{
  BRepAdaptor_Surface BS(F);
  bool outofboundU = (uv.X() > BS.LastUParameter()) || (uv.X() < BS.FirstUParameter());
  bool outofboundV = (uv.Y() > BS.LastVParameter()) || (uv.Y() < BS.FirstVParameter());
  return outofboundU || outofboundV;
}

//=================================================================================================

double TopOpeBRepTool_TOOL::TolUV(const TopoDS_Face& F, const double tol3d)
{
  BRepAdaptor_Surface bs(F);
  double              tol2d = bs.UResolution(tol3d);
  tol2d                     = std::max(tol2d, bs.VResolution(tol3d));
  return tol2d;
}

//=================================================================================================

double TopOpeBRepTool_TOOL::TolP(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  BRepAdaptor_Curve2d BC2d(E, F);
  return (BC2d.Resolution(BRep_Tool::Tolerance(E)));
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::WireToFace(
  const TopoDS_Face& Fref,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                  mapWlow,
  NCollection_List<TopoDS_Shape>& lFs)
{
  BRep_Builder BB;
  TopoDS_Shape aLocalShape = Fref.Oriented(TopAbs_FORWARD);
  TopoDS_Face  F           = TopoDS::Face(aLocalShape);
  //  TopoDS_Face F = TopoDS::Face(Fref.Oriented(TopAbs_FORWARD));
  bool toreverse = M_REVERSED(Fref.Orientation());
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itm(mapWlow);
  for (; itm.More(); itm.Next())
  {
    TopoDS_Shape       FF = F.EmptyCopied();
    const TopoDS_Wire& wi = TopoDS::Wire(itm.Key());
    BB.Add(FF, wi);
    NCollection_List<TopoDS_Shape>::Iterator itw(itm.Value());
    for (; itw.More(); itw.Next())
    {
      const TopoDS_Wire& wwi = TopoDS::Wire(itw.Value());
      BB.Add(FF, wwi);
    }
    if (toreverse)
      FF.Orientation(TopAbs_REVERSED);
    lFs.Append(FF);
  }
  return true;
}

//=================================================================================================

bool TopOpeBRepTool_TOOL::EdgeONFace(const double       par,
                                     const TopoDS_Edge& ed,
                                     const gp_Pnt2d&    uv,
                                     const TopoDS_Face& fa,
                                     bool&              isonfa)
{
  isonfa = false;
  // prequesitory : pnt(par,ed) = pnt(uv,f)
  bool dge = BRep_Tool::Degenerated(ed);
  if (dge)
  {
    isonfa = true;
    return true;
  }

  double tola = Precision::Angular() * 1.e2; // NYITOLXPU
  gp_Vec tge;
  bool   ok = TopOpeBRepTool_TOOL::TggeomE(par, ed, tge);
  if (!ok)
    return false;
  gp_Vec ngf      = FUN_tool_nggeomF(uv, fa);
  double aProdDot = tge.Dot(ngf);
  bool   etgf     = std::abs(aProdDot) < tola;
  if (!etgf)
    return true;

  BRepAdaptor_Surface bs(fa);
  GeomAbs_SurfaceType st       = bs.GetType();
  bool                plane    = (st == GeomAbs_Plane);
  bool                cylinder = (st == GeomAbs_Cylinder);

  BRepAdaptor_Curve bc(ed);
  GeomAbs_CurveType ct     = bc.GetType();
  bool              line   = (ct == GeomAbs_Line);
  bool              circle = (ct == GeomAbs_Circle);

  double tole   = bc.Tolerance();
  double tol1de = bc.Resolution(tole);
  double tolf   = bs.Tolerance();
  double tol3d  = std::max(tole, tolf) * 1.e2; // NYITOLXPU

  // NYIxpu100299 : for other analytic geometries
  if (plane && line)
  {
    isonfa = true;
    return true;
  }
  if (plane)
  {
    gp_Dir ne;
    bool   det = true;
    if (circle)
      ne = bc.Circle().Axis().Direction();
    else if (ct == GeomAbs_Ellipse)
      ne = bc.Ellipse().Axis().Direction();
    else if (ct == GeomAbs_Hyperbola)
      ne = bc.Hyperbola().Axis().Direction();
    else if (ct == GeomAbs_Parabola)
      ne = bc.Parabola().Axis().Direction();
    else
      det = false;
    if (det)
    {
      double prod = ne.Dot(ngf);
      isonfa      = (std::abs(1 - std::abs(prod)) < tola);
      return true;
    }
  } // plane
  else if (cylinder)
  {
    gp_Dir ne;
    bool   det = true;
    if (line)
      ne = tge;
    else if (circle)
      ne = bc.Circle().Axis().Direction();
    else
      det = false;
    gp_Dir axicy = bs.Cylinder().Axis().Direction();

    if (det)
    {
      double prod = ne.Dot(axicy);
      isonfa      = (std::abs(1 - std::abs(prod)) < tola);
      if (isonfa && circle)
      {
        double radci = bc.Circle().Radius();
        double radcy = bs.Cylinder().Radius();
        isonfa       = (std::abs(radci - radcy) < tol3d);
      }
      return true;
    }
  } // cylinder

  // !!!!!!!!!!!!!!!! NOT STILL OK !!!!!!!!!!!!!!
  // projecting point of <ed> on <fa>
  double x = 0.12345;
  double f, l;
  FUN_tool_bounds(ed, f, l);
  bool   onf  = (std::abs(par - f) < tol1de);
  double opar = onf ? ((1 - x) * f + x * l) : ((1 - x) * f + x * par);
  gp_Pnt opc  = bc.Value(opar);

  gp_Pnt2d ouv;
  ok = FUN_tool_parF(ed, opar, fa, ouv, tolf);
  if (!ok)
    return false;
  gp_Pnt ops = bs.Value(ouv.X(), ouv.Y());

  double dd = opc.Distance(ops);
  isonfa    = (dd < tol3d);
  return true;
}
