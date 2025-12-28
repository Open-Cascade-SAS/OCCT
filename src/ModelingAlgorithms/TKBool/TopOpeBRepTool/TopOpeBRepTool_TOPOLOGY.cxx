// Created on: 1998-10-06
// Created by: Jean Yves LEBEY
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

#include <Geom_TrimmedCurve.hxx>
#include <BRepBndLib.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <ElCLib.hxx>
#include <Precision.hxx>
#include <NCollection_Array1.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_2d.hxx>

#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

//---------------------------------------------
#define M_FORWARD(ori) (ori == TopAbs_FORWARD)
#define M_REVERSED(ori) (ori == TopAbs_REVERSED)
#define M_INTERNAL(ori) (ori == TopAbs_INTERNAL)
#define M_EXTERNAL(ori) (ori == TopAbs_EXTERNAL)

// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_tolUV(const TopoDS_Face& F, double& tolu, double& tolv)
{
  double              tolF = BRep_Tool::Tolerance(TopoDS::Face(F));
  BRepAdaptor_Surface BS(TopoDS::Face(F));
  tolu = BS.UResolution(tolF);
  tolv = BS.VResolution(tolF);
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_direct(const TopoDS_Face& F, bool& direct)
{
  BRepAdaptor_Surface BS(TopoDS::Face(F));
  GeomAbs_SurfaceType ST    = BS.GetType();
  bool                plane = (ST == GeomAbs_Plane);
  bool                cyl   = (ST == GeomAbs_Cylinder);
  bool                cone  = (ST == GeomAbs_Cone);
  bool                sphe  = (ST == GeomAbs_Sphere);
  bool                torus = (ST == GeomAbs_Torus);
  if (plane)
  {
    const gp_Pln& plpl = BS.Plane();
    direct             = plpl.Direct();
  }
  if (cyl)
  {
    const gp_Cylinder& cycy = BS.Cylinder();
    direct                  = cycy.Direct();
  }
  if (cone)
  {
    const gp_Cone& coco = BS.Cone();
    direct              = coco.Direct();
  }
  if (sphe)
  {
    const gp_Sphere& spsp = BS.Sphere();
    direct                = spsp.Direct();
  }
  if (torus)
  {
    const gp_Torus& toto = BS.Torus();
    direct               = toto.Direct();
  }
  bool ok = plane || cyl || cone || sphe || torus;
  return ok;
}

/*// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_IsUViso(const TopoDS_Shape& E,const TopoDS_Shape& F,
                     bool& isoU,bool& isoV,
                     gp_Dir2d& d2d,gp_Pnt2d& o2d)
{
  isoU = isoV = false; double f,l,tol;
  occ::handle<Geom2d_Curve> PC =
    FC2D_CurveOnSurface(TopoDS::Edge(E),TopoDS::Face(F),f,l,tol);
  if (PC.IsNull()) return false;
  bool iso = FUN_tool_IsUViso(PC,isoU,isoV,d2d,o2d);
  return iso;
}*/

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_geombounds(const TopoDS_Face& F,
                                         double&            u1,
                                         double&            u2,
                                         double&            v1,
                                         double&            v2)
{
  const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(F);
  if (S.IsNull())
    return false;
  S->Bounds(u1, u2, v1, v2);
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_bounds(const TopoDS_Shape& Sh,
                                     double&             u1,
                                     double&             u2,
                                     double&             v1,
                                     //				                 double& v2)
                                     double&)
{
  Bnd_Box2d          B2d;
  const TopoDS_Face& F = TopoDS::Face(Sh);
  TopExp_Explorer    ex(F, TopAbs_WIRE);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Wire W = TopoDS::Wire(ex.Current());
    Bnd_Box2d         newB2d;
    FUN_tool_mkBnd2d(W, F, newB2d);
    B2d.Add(newB2d);
  }
  B2d.Get(u1, v1, u2, v1);
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_isobounds(const TopoDS_Shape& Sh,
                                        double&             u1,
                                        double&             u2,
                                        double&             v1,
                                        double&             v2)
{
  const TopoDS_Face& F = TopoDS::Face(Sh);
  u1 = v1 = 1.e7;
  u2 = v2 = -1.e7;

  const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(F);
  if (S.IsNull())
    return false;

  bool   uclosed, vclosed;
  double uperiod, vperiod;

  //  bool uvclosed =

  FUN_tool_closedS(F, uclosed, uperiod, vclosed, vperiod);

  //  double uf,ul,vf,vl; S->Bounds(uf,ul,vf,vl);

  //  if (!uvclosed) {
  //    u1 = uf; v1 = vf; u2 = ul; v2 = vl;
  //    return true;
  //  }
  TopExp_Explorer ex(F, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Edge&        E = TopoDS::Edge(ex.Current());
    double                    f, l, tol;
    occ::handle<Geom2d_Curve> PC = FC2D_CurveOnSurface(E, F, f, l, tol);
    if (PC.IsNull())
      return false;

    bool     isou, isov;
    gp_Pnt2d o2d;
    gp_Dir2d d2d;
    bool     isouv = TopOpeBRepTool_TOOL::UVISO(PC, isou, isov, d2d, o2d);

    if (isouv)
    {
      gp_Pnt2d p2df = PC->Value(f);
      gp_Pnt2d p2dl = PC->Value(l);
      u1            = std::min(p2df.X(), u1);
      u2            = std::max(p2df.X(), u2);
      v1            = std::min(p2df.Y(), v1);
      v2            = std::max(p2df.Y(), v2);
      u1            = std::min(p2dl.X(), u1);
      u2            = std::max(p2dl.X(), u2);
      v1            = std::min(p2dl.Y(), v1);
      v2            = std::max(p2dl.Y(), v2);
    }
    if (!isouv)
      return false;
    // ====================
  }
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_outbounds(const TopoDS_Shape& Sh,
                                        double&             u1,
                                        double&             u2,
                                        double&             v1,
                                        double&             v2,
                                        bool&               outbounds)
{
  occ::handle<Geom_Surface> S = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(Sh));
  if (S.IsNull())
    return false;
  double uf, ul, vf, vl;
  S->Bounds(uf, ul, vf, vl);

  outbounds = false;
  bool ok   = FUN_tool_bounds(Sh, u1, u2, v1, v2);
  if (!ok)
    return false;

  const TopoDS_Face& F = TopoDS::Face(Sh);
  bool               uclosed, vclosed;
  double             uperiod, vperiod;
  FUN_tool_closedS(F, uclosed, uperiod, vclosed, vperiod);
  double tolp = 1.e-6;

  if (uclosed)
  {
    double dd = u2 - u1;
    if (dd > uperiod + tolp)
    {
      u1        = uf;
      v1        = vf;
      u2        = ul;
      v2        = vl;
      outbounds = true;
    }
  }
  if (vclosed)
  {
    double dd = v2 - v1;
    if (dd > vperiod + tolp)
    {
      u1        = uf;
      v1        = vf;
      u2        = ul;
      v2        = vl;
      outbounds = true;
    }
  }
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_PinC(const gp_Pnt&            P,
                                   const BRepAdaptor_Curve& BAC,
                                   const double             pmin,
                                   const double             pmax,
                                   const double             tol)
{
  // returns true if <P> is on <C> under a given tolerance <tol>
  bool          PinC = false;
  Extrema_ExtPC ponc(P, BAC, pmin, pmax);
  bool          ok = ponc.IsDone();
  if (ok)
  {
    if (ponc.NbExt())
    {
      int    i  = FUN_tool_getindex(ponc);
      double d2 = ponc.SquareDistance(i);
      PinC      = (d2 <= tol * tol);
    }
  }
  return PinC;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_PinC(const gp_Pnt& P, const BRepAdaptor_Curve& BAC, const double tol)
{
  // returns true if <P> is on <C> under a given tolerance <tol>
  bool   PinC = false;
  double pmin = BAC.FirstParameter();
  double pmax = BAC.LastParameter();
  PinC        = FUN_tool_PinC(P, BAC, pmin, pmax, tol);
  return PinC;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_value(const double par, const TopoDS_Edge& E, gp_Pnt& P)
{
  BRepAdaptor_Curve BAC(E);
  double            f  = BAC.FirstParameter();
  double            l  = BAC.LastParameter();
  bool              ok = (f <= par) && (par <= l);
  if (!ok)
    return false;
  P = BAC.Value(par);
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_value(const gp_Pnt2d& UV, const TopoDS_Face& F, gp_Pnt& P)
{
  BRepAdaptor_Surface BS(F);
  P = BS.Value(UV.X(), UV.Y());
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT TopAbs_State FUN_tool_staPinE(const gp_Pnt&      P,
                                              const TopoDS_Edge& E,
                                              const double       tol)
{
  // project point on curve,
  // if projection fails,returns UNKNOWN.
  // finds a point <pnear> on edge <E> / d(<pnear>,<P>) < tol
  //   => returns IN
  // else,returns OUT.
  TopAbs_State      sta = TopAbs_UNKNOWN;
  BRepAdaptor_Curve BAC(E);
  Extrema_ExtPC     ProjonBAC(P, BAC);
  if (ProjonBAC.IsDone() && ProjonBAC.NbExt() > 0)
  {
    int    i     = FUN_tool_getindex(ProjonBAC);
    gp_Pnt Pnear = ProjonBAC.Point(i).Value();
    double d     = Pnear.Distance(P);
    sta          = (d < tol) ? TopAbs_IN : TopAbs_OUT;
  }
  return sta;
}

// ----------------------------------------------------------------------
Standard_EXPORT TopAbs_State FUN_tool_staPinE(const gp_Pnt& P, const TopoDS_Edge& E)
{
  //  double tol = Precision::Confusion()*10.;
  double       tol3d = BRep_Tool::Tolerance(E) * 1.e2; // KKKKK a revoir xpu(CTS21118,f14ou,GI13)
  TopAbs_State sta   = FUN_tool_staPinE(P, E, tol3d);
  return sta;
}

// ----------------------------------------------------------------------
//  subshape's orientation :
//    - vertex orientation in edge
//    - edge's orientation in face.Oriented(FORWARD)
// ----------------------------------------------------------------------

#define FIRST (1)
#define LAST (2)
#define CLOSING (3)

// ----------------------------------------------------------------------
Standard_EXPORT int FUN_tool_orientVinE(const TopoDS_Vertex& v, const TopoDS_Edge& e)
{
  int           result = 0;
  TopoDS_Vertex vf, vl;
  TopExp::Vertices(e, vf, vl);
  bool visf = v.IsSame(vf);
  bool visl = v.IsSame(vl);
  if (visf)
    result = FIRST;
  if (visl)
    result = LAST;
  if (visf && visl)
    result = CLOSING;
  return result;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_orientEinF(const TopoDS_Edge&  E,
                                         const TopoDS_Face&  F,
                                         TopAbs_Orientation& oriEinF)
{
  oriEinF = TopAbs_FORWARD;
  TopExp_Explorer e(F, TopAbs_EDGE);
  for (; e.More(); e.Next())
  {
    const TopoDS_Shape& EF = e.Current();
    if (EF.IsSame(E))
    {
      oriEinF = EF.Orientation();
      break;
    }
  }
  bool ok = e.More();
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_orientEinFFORWARD(const TopoDS_Edge&  E,
                                                const TopoDS_Face&  F,
                                                TopAbs_Orientation& oriEinF)
{
  // <oriEinF> : dummy for closing edge <E> of <F>.
  // returns false if <E> is not bound of <F>
  // else,<oriEinF> = orientation of the edge in <F> oriented FORWARD.
  TopoDS_Shape aLocalShape = F.Oriented(TopAbs_FORWARD);
  TopoDS_Face  FF          = TopoDS::Face(aLocalShape);
  //  TopoDS_Face FF = TopoDS::Face(F.Oriented(TopAbs_FORWARD));
  oriEinF = TopAbs_EXTERNAL;
  TopAbs_Orientation reso;
  bool               ok = ::FUN_tool_orientEinF(E, FF, reso);
  if (ok)
    oriEinF = reso;
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_EboundF(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  TopAbs_Orientation ori;
  bool               ok = FUN_tool_orientEinFFORWARD(E, F, ori);
  if (!ok)
    return false;
  bool closingE = BRep_Tool::IsClosed(E, F);
  if (closingE)
    return true;
  bool notbound = (ori == TopAbs_INTERNAL) || (ori == TopAbs_EXTERNAL);
  return notbound;
}

// ----------------------------------------------------------------------
Standard_EXPORT gp_Vec FUN_tool_nggeomF(const gp_Pnt2d& p2d, const TopoDS_Face& F)
{
  occ::handle<Geom_Surface> S = BRep_Tool::Surface(F);
  gp_Vec                    ngF(FUN_tool_ngS(p2d, S));
  return ngF;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_nggeomF(const double&      paronE,
                                      const TopoDS_Edge& E,
                                      const TopoDS_Face& F,
                                      gp_Vec&            nggeomF,
                                      const double       tol)
{
  // <p2d> :
  double             f, l;
  gp_Pnt2d           p2d;
  bool               project = true;
  TopAbs_Orientation oef;
  bool               edonfa = FUN_tool_orientEinFFORWARD(E, F, oef);
  if (edonfa)
  {
    double                    ttol;
    occ::handle<Geom2d_Curve> c2d = FC2D_CurveOnSurface(E, F, f, l, ttol);
    project                       = c2d.IsNull();
    if (!project)
      p2d = c2d->Value(paronE);
  }
  if (project)
  {
    BRepAdaptor_Curve BC(E);
    gp_Pnt            p3d = BC.Value(paronE);
    double            d;
    bool              ok = FUN_tool_projPonF(p3d, F, p2d, d);

    // modified by NIZHNY-MZV  Wed Dec  1 13:55:08 1999
    // if !ok try to compute new pcurve
    if (!ok)
    {
      double                    ttol;
      occ::handle<Geom2d_Curve> c2d = FC2D_CurveOnSurface(E, F, f, l, ttol);
      ok                            = !c2d.IsNull();
      if (ok)
        p2d = c2d->Value(paronE);
    }
    if (!ok)
      return false;
    // modified by NIZHNY-MZV  Wed Dec  1 13:56:14 1999
    // xpu010698
    gp_Pnt p3duv;
    FUN_tool_value(p2d, F, p3duv);
    double dd = p3duv.Distance(p3d);
    if (dd > tol)
      return false;
    // xpu010698
  }
  nggeomF = FUN_tool_nggeomF(p2d, F);
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_nggeomF(const double&      paronE,
                                      const TopoDS_Edge& E,
                                      const TopoDS_Face& F,
                                      gp_Vec&            nggeomF)
{
  double tol3d = BRep_Tool::Tolerance(F);
  tol3d *= 1.e2;
  bool ok = FUN_tool_nggeomF(paronE, E, F, nggeomF, tol3d);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_EtgF(const double&      paronE,
                                   const TopoDS_Edge& E,
                                   const gp_Pnt2d&    p2d,
                                   const TopoDS_Face& F,
                                   const double       tola)
{
  gp_Vec tgE;
  bool   ok = TopOpeBRepTool_TOOL::TggeomE(paronE, E, tgE);
  if (!ok)
    return false; // NYIRAISE

  gp_Vec ngF  = FUN_tool_nggeomF(p2d, F);
  double prod = tgE.Dot(ngF);
  bool   tgt  = std::abs(prod) < tola;
  return tgt;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_EtgOOE(const double&      paronE,
                                     const TopoDS_Edge& E,
                                     const double&      paronOOE,
                                     const TopoDS_Edge& OOE,
                                     const double       tola)
{
  gp_Vec tgOOE;
  bool   ok = TopOpeBRepTool_TOOL::TggeomE(paronOOE, OOE, tgOOE);
  if (!ok)
    return false; // NYIRAISE
  gp_Vec tgE;
  ok = TopOpeBRepTool_TOOL::TggeomE(paronE, E, tgE);
  if (!ok)
    return false; // NYIRAISE
  double prod = tgOOE.Dot(tgE);
  bool   tg   = (std::abs(1 - std::abs(prod)) < tola);
  return tg;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_nearestISO(const TopoDS_Face& F,
                                    const double       xpar,
                                    const bool         isoU,
                                    double&            xinf,
                                    double&            xsup)
{
  // IMPORTANT : xinf=xf,xsup=xl are INITIALIZED with first and last x
  //             parameters of F (x = u,v )
  // purpose : finding greater xinf : xinf <= xpar
  //                   smaller xsup :        xpar <=xsup
  double tol2d = 1.e-6;
  double df    = xpar - xinf;
  bool   onf   = (std::abs(df) < tol2d);
  double dl    = xpar - xsup;
  bool   onl   = (std::abs(dl) < tol2d);
  if (onf || onl)
    return true;

  bool            isoV = !isoU;
  TopExp_Explorer ex(F, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    bool               isou, isov;
    gp_Pnt2d           o2d;
    gp_Dir2d           d2d;
    bool               isouv = TopOpeBRepTool_TOOL::UVISO(E, F, isou, isov, d2d, o2d);
    if (!isouv)
      return false;

    bool compare = (isoU && isou) || (isoV && isov);
    if (!compare)
      return false;
    double xvalue = isou ? o2d.X() : o2d.Y();

    if (xvalue < xpar)
      if (xinf < xvalue)
        xinf = xvalue;
    if (xpar < xvalue)
      if (xvalue < xsup)
        xsup = xvalue;
  }
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_EitangenttoFe(const gp_Dir&      ngFe,
                                            const TopoDS_Edge& Ei,
                                            const double       parOnEi)
{
  // returns true if <Ei> is tangent to Fe at point
  // p3d of param <parOnEi>,<ngFe> is normal to Fe at p3d.
  gp_Vec tgEi;
  bool   ok = TopOpeBRepTool_TOOL::TggeomE(parOnEi, Ei, tgEi);
  if (!ok)
    return false; // NYIRAISE

  double prod    = ngFe.Dot(tgEi);
  double tol     = Precision::Parametric(Precision::Confusion());
  bool   tangent = (std::abs(prod) <= tol);
  return tangent;
}

// ----------------------------------------------------------------------
Standard_EXPORT GeomAbs_CurveType FUN_tool_typ(const TopoDS_Edge& E)
{
  BRepAdaptor_Curve BC(E);
  GeomAbs_CurveType typ = BC.GetType();
  return typ;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_line(const TopoDS_Edge& E)
{
  BRepAdaptor_Curve BC(E);
  bool              line = (BC.GetType() == GeomAbs_Line);
  return line;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_plane(const TopoDS_Shape& F)
{
  occ::handle<Geom_Surface> S = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  GeomAdaptor_Surface       GS(S);
  return (GS.GetType() == GeomAbs_Plane);
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_cylinder(const TopoDS_Shape& F)
{
  occ::handle<Geom_Surface> S = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  GeomAdaptor_Surface       GS(S);
  return (GS.GetType() == GeomAbs_Cylinder);
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_closedS(const TopoDS_Shape& F,
                                      bool&               uclosed,
                                      double&             uperiod,
                                      bool&               vclosed,
                                      double&             vperiod)
{
  //  const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(TopoDS::Face(F));
  occ::handle<Geom_Surface> S = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  if (S.IsNull())
    return false;
  bool closed = FUN_tool_closed(S, uclosed, uperiod, vclosed, vperiod);
  return closed;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_closedS(const TopoDS_Shape& F)
{
  bool   uclosed = false, vclosed = false;
  double uperiod = 0., vperiod = 0.;
  bool   closed = FUN_tool_closedS(F, uclosed, uperiod, vclosed, vperiod);
  return closed;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_closedS(const TopoDS_Shape& F, bool& inU, double& xmin, double& xper)
{
  bool                      uclosed, vclosed;
  double                    uperiod, vperiod;
  occ::handle<Geom_Surface> S = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  if (S.IsNull())
    return false;
  bool closed = FUN_tool_closed(S, uclosed, uperiod, vclosed, vperiod);
  if (!closed)
    return false;
  double u1, u2, v1, v2;
  S->Bounds(u1, u2, v1, v2);

  inU  = uclosed;
  xper = inU ? uperiod : vperiod;
  xmin = inU ? u1 : v1;
  return false;
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_mkBnd2d(const TopoDS_Shape& W, const TopoDS_Shape& FF, Bnd_Box2d& B2d)
{
  // greater <B> with <W>'s UV representation on <F>
  double          tol = 1.e-8;
  Bnd_Box2d       newB2d;
  TopExp_Explorer ex;
  for (ex.Init(W, TopAbs_EDGE); ex.More(); ex.Next())
  {
    //  for (TopExp_Explorer ex(W,TopAbs_EDGE); ex.More(); ex.Next()) {
    const TopoDS_Edge&        E = TopoDS::Edge(ex.Current());
    const TopoDS_Face&        F = TopoDS::Face(FF);
    double                    f, l, tolpc;
    occ::handle<Geom2d_Curve> pc;
    bool                      haspc = FC2D_HasCurveOnSurface(E, F);
    if (!haspc)
    {
      double tolE         = BRep_Tool::Tolerance(E);
      pc                  = FC2D_CurveOnSurface(E, F, f, l, tolpc);
      double       newtol = std::max(tolE, tolpc);
      BRep_Builder BB;
      BB.UpdateEdge(E, pc, F, newtol);
    }
    BRepAdaptor_Curve2d BC2d(E, F);
    BndLib_Add2dCurve::Add(BC2d, tol, newB2d);
  } // ex(W,EDGE)

  FUN_tool_UpdateBnd2d(B2d, newB2d);
}

//=================================================================================================

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_IsClosingE(const TopoDS_Edge&  E,
                                         const TopoDS_Shape& S,
                                         const TopoDS_Face&  F)
{
  int             nbocc = 0;
  TopExp_Explorer exp;
  for (exp.Init(S, TopAbs_EDGE); exp.More(); exp.Next())
    //  for (TopExp_Explorer exp(S,TopAbs_EDGE);exp.More();exp.Next())
    if (exp.Current().IsSame(E))
      nbocc++;
  if (nbocc != 2)
    return false;
  return BRep_Tool::IsClosed(E, F);
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_ClosingE(const TopoDS_Edge& E,
                                       const TopoDS_Wire& W,
                                       const TopoDS_Face& F)
{
  bool clo = FUN_tool_IsClosingE(E, W, F);
  return clo;
}

/*// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_ClosedE(const TopoDS_Edge& E,TopoDS_Shape& vclosing)
{
  // returns true if <E> has a closing vertex <vclosing>
//  return E.IsClosed();
  bool isdgE = BRep_Tool::Degenerated(E);
  if (isdgE) return false;

  TopoDS_Shape vv; vclosing.Nullify();
  TopExp_Explorer ex(E,TopAbs_VERTEX);
  for (; ex.More(); ex.Next()) {
    const TopoDS_Shape& v = ex.Current();
    if (M_INTERNAL(v.Orientation())) continue;
    if (vv.IsNull()) vv = v;
    else if (v.IsSame(vv))
      {vclosing = vv; return true;}
  }
  return false;
}*/

//=================================================================================================

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_inS(const TopoDS_Shape& subshape, const TopoDS_Shape& shape)
{
  TopAbs_ShapeEnum                                              sstyp = subshape.ShapeType();
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> M;
  TopExp::MapShapes(shape, sstyp, M);
  bool isbound = M.Contains(subshape);
  return isbound;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_Eshared(const TopoDS_Shape& v,
                                      const TopoDS_Shape& F1,
                                      const TopoDS_Shape& F2,
                                      TopoDS_Shape&       Eshared)
// purpose : finds out <Eshared> shared by <F1> and <F2>,
//           with <V> bound of <Eshared>
{
  NCollection_List<TopoDS_Shape> e1s;
  TopExp_Explorer                ex(F1, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Shape& e1   = ex.Current();
    bool                e1ok = false;
    TopExp_Explorer     exv(e1, TopAbs_VERTEX);
    for (; exv.More(); exv.Next())
      if (exv.Current().IsSame(v))
      {
        e1ok = true;
        break;
      }
    if (e1ok)
      e1s.Append(e1);
  }
  ex.Init(F2, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Shape&                      e2 = ex.Current();
    NCollection_List<TopoDS_Shape>::Iterator it1(e1s);
    for (; it1.More(); it1.Next())
      if (it1.Value().IsSame(e2))
      {
        Eshared = e2;
        return true;
      }
  }
  return false;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_parVonE(const TopoDS_Vertex& v, const TopoDS_Edge& E, double& par)
{
  double          tol    = Precision::Confusion();
  bool            isVofE = false;
  TopExp_Explorer ex;
  for (ex.Init(E, TopAbs_VERTEX); ex.More(); ex.Next())
  {
    //  for (TopExp_Explorer ex(E,TopAbs_VERTEX); ex.More(); ex.Next()) {
    isVofE = ex.Current().IsSame(v);
    if (isVofE)
    {
      par = BRep_Tool::Parameter(TopoDS::Vertex(ex.Current()), E);
      break;
    }
  }
  if (!isVofE)
  {
    gp_Pnt pt = BRep_Tool::Pnt(v);
    // <v> can share same domain with a vertex of <E>
    for (ex.Init(E, TopAbs_VERTEX); ex.More(); ex.Next())
    {
      const TopoDS_Vertex& vex  = TopoDS::Vertex(ex.Current());
      gp_Pnt               ptex = BRep_Tool::Pnt(vex);
      if (ptex.IsEqual(pt, tol))
      {
        par = BRep_Tool::Parameter(vex, E);
        return true;
      }
    }

    //    double f,l;
    BRepAdaptor_Curve BAC(E);
    Extrema_ExtPC     pro(pt, BAC);
    bool              done = pro.IsDone() && (pro.NbExt() > 0);
    if (!done)
      return false;
    int i = FUN_tool_getindex(pro);
    par   = pro.Point(i).Parameter();
  }
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_parE(const TopoDS_Edge& E0,
                                   const double&      par0,
                                   const TopoDS_Edge& E,
                                   double&            par,
                                   const double       tol)
{
  gp_Pnt P;
  bool   ok = FUN_tool_value(par0, E0, P);
  if (!ok)
    return false;

  double dist;
  ok = FUN_tool_projPonE(P, E, par, dist);
  if (!ok)
    return false;

  ok = (dist < tol);
  return ok;
}
// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_parE(const TopoDS_Edge& E0,
                                   const double&      par0,
                                   const TopoDS_Edge& E,
                                   double&            par)
// ? <par> :  P -> <par0> on <E0>,<par> on <E>
// prequesitory : point(par0 ,E0) is IN 1d(E)
{
  double tol3d = BRep_Tool::Tolerance(E) * 1.e2; // KKKKK a revoir xpu(CTS21118,f14ou,GI13)
  bool   ok    = FUN_tool_parE(E0, par0, E, par, tol3d);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_parF(const TopoDS_Edge& E,
                                   const double&      par,
                                   const TopoDS_Face& F,
                                   gp_Pnt2d&          UV,
                                   const double       tol3d)
// ? <UV> : P -> <par> on <E>,<UV> on <F>
{
  gp_Pnt P;
  bool   ok = FUN_tool_value(par, E, P);
  if (!ok)
    return false;

  double dist;
  ok = FUN_tool_projPonF(P, F, UV, dist);
  if (!ok)
    return false;

  ok = (dist < tol3d);
  return ok;
}

Standard_EXPORT bool FUN_tool_parF(const TopoDS_Edge& E,
                                   const double&      par,
                                   const TopoDS_Face& F,
                                   gp_Pnt2d&          UV)
{
  double tol3d = BRep_Tool::Tolerance(F) * 1.e2; // KK xpu
  bool   ok    = FUN_tool_parF(E, par, F, UV, tol3d);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_paronEF(const TopoDS_Edge& E,
                                      const double&      par,
                                      const TopoDS_Face& F,
                                      gp_Pnt2d&          UV,
                                      const double       tol3d)
// <E> is on <F> ? <UV> : P -> <par> on <E>,<UV> on <F>
{
  double                    f, l, tol;
  occ::handle<Geom2d_Curve> PC = FC2D_CurveOnSurface(E, F, f, l, tol);
  if (PC.IsNull())
  {
    bool ok = FUN_tool_parF(E, par, F, UV, tol3d);
    return ok;
  }
  bool ok = (f <= par) && (par <= l);
  if (!ok)
    return false;
  UV = PC->Value(par);
  return true;
}

Standard_EXPORT bool FUN_tool_paronEF(const TopoDS_Edge& E,
                                      const double&      par,
                                      const TopoDS_Face& F,
                                      gp_Pnt2d&          UV)
{
  double tol3d = BRep_Tool::Tolerance(F) * 1.e2; // KKxpu
  bool   ok    = FUN_tool_paronEF(E, par, F, UV, tol3d);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT gp_Dir FUN_tool_dirC(const double par, const BRepAdaptor_Curve& BAC)
{
  gp_Pnt p;
  gp_Vec tgE;
  BAC.D1(par, p, tgE);
  gp_Dir dirC(tgE);
  return dirC;
}

// ----------------------------------------------------------------------
Standard_EXPORT gp_Vec FUN_tool_tggeomE(const double paronE, const TopoDS_Edge& E)
{
  bool isdgE = BRep_Tool::Degenerated(E);
  if (isdgE)
    return gp_Vec(0, 0, 0);
  gp_Vec dirE(FUN_tool_dirC(paronE, E));
  return dirE;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_line(const BRepAdaptor_Curve& BAC)
{
  bool line = (BAC.GetType() == GeomAbs_Line);
  return line;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_quad(const TopoDS_Edge& E)
{
  BRepAdaptor_Curve BC(E);
  GeomAbs_CurveType CT   = BC.GetType();
  bool              quad = FUN_quadCT(CT);
  return quad;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_quad(const BRepAdaptor_Curve& BAC)
{
  GeomAbs_CurveType CT     = BAC.GetType();
  bool              isquad = false;
  if (CT == GeomAbs_Line)
    isquad = true;
  if (CT == GeomAbs_Circle)
    isquad = true;
  if (CT == GeomAbs_Ellipse)
    isquad = true;
  if (CT == GeomAbs_Hyperbola)
    isquad = true;
  if (CT == GeomAbs_Parabola)
    isquad = true;
  return isquad;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_quad(const TopoDS_Face& F)
{
  occ::handle<Geom_Surface> S    = TopOpeBRepTool_ShapeTool::BASISSURFACE(F);
  bool                      quad = FUN_tool_quad(S);
  return quad;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_findPinBAC(const BRepAdaptor_Curve& BAC, gp_Pnt& P, double& par)
{
  FUN_tool_findparinBAC(BAC, par);
  BAC.D0(par, P);
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_findparinBAC(const BRepAdaptor_Curve& BAC, double& par)
{
  double fE = BAC.FirstParameter(), lE = BAC.LastParameter();
  double t = 0.34567237;
  par      = (1 - t) * fE + t * lE;
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_findparinE(const TopoDS_Shape& E, double& par)
{
  BRepAdaptor_Curve BAC(TopoDS::Edge(E));
  bool              r = FUN_tool_findparinBAC(BAC, par);
  return r;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_findPinE(const TopoDS_Shape& E, gp_Pnt& P, double& par)
{
  BRepAdaptor_Curve BAC(TopoDS::Edge(E));
  bool              r = FUN_tool_findPinBAC(BAC, P, par);
  return r;
}

//=================================================================================================

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_maxtol(const TopoDS_Shape&     S,
                                     const TopAbs_ShapeEnum& typ,
                                     double&                 maxtol)
// purpose : returns maxtol of <S>'s shapes of type <typ>
{
  bool            face   = (typ == TopAbs_FACE);
  bool            edge   = (typ == TopAbs_EDGE);
  bool            vertex = (typ == TopAbs_VERTEX);
  TopExp_Explorer ex(S, typ);
  bool            hasshatyp = ex.More();
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Shape& ss    = ex.Current();
    double              tolss = 0.;
    if (face)
      tolss = BRep_Tool::Tolerance(TopoDS::Face(ss));
    if (edge)
      tolss = BRep_Tool::Tolerance(TopoDS::Edge(ss));
    if (vertex)
      tolss = BRep_Tool::Tolerance(TopoDS::Vertex(ss));
    if (tolss > maxtol)
      maxtol = tolss;
  }
  return hasshatyp;
}

// ----------------------------------------------------------------------
Standard_EXPORT double FUN_tool_maxtol(const TopoDS_Shape& S)
// purpose : returns maxtol between <S>'s shapes.
{
  double maxtol = 0.;
  FUN_tool_maxtol(S, TopAbs_FACE, maxtol);
  FUN_tool_maxtol(S, TopAbs_EDGE, maxtol);
  FUN_tool_maxtol(S, TopAbs_VERTEX, maxtol);
  return maxtol;
}

// ----------------------------------------------------------------------
Standard_EXPORT int FUN_tool_nbshapes(const TopoDS_Shape& S, const TopAbs_ShapeEnum& typ)
{
  TopExp_Explorer ex(S, typ);
  int             i = 0;
  for (; ex.More(); ex.Next())
    i++;
  return i;
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_shapes(const TopoDS_Shape&             S,
                                     const TopAbs_ShapeEnum&         typ,
                                     NCollection_List<TopoDS_Shape>& ltyp)
{
  TopExp_Explorer ex(S, typ);
  for (; ex.More(); ex.Next())
    ltyp.Append(ex.Current());
}

// ----------------------------------------------------------------------
Standard_EXPORT int FUN_tool_comparebndkole(const TopoDS_Shape& sh1, const TopoDS_Shape& sh2)
// purpose: comparing bounding boxes of <sh1> and <sh2>,
//          returns k =1,2 if shi is contained in shk
//          else returns 0
{
  Bnd_Box bnd1;
  BRepBndLib::Add(sh1, bnd1);
  bnd1.SetGap(0.);
  Bnd_Box bnd2;
  BRepBndLib::Add(sh2, bnd2);
  bnd2.SetGap(0.);

  if (bnd1.IsOut(bnd2))
    return 0;
  NCollection_Array1<double> xyz1(1, 6), xyz2(1, 6);
  bnd1.Get(xyz1(1), xyz1(2), xyz1(3), xyz1(4), xyz1(5), xyz1(6));
  bnd2.Get(xyz2(1), xyz2(2), xyz2(3), xyz2(4), xyz2(5), xyz2(6));
  double tol = Precision::Confusion();

  int neq, n2sup;
  neq = n2sup = 0;
  //  for (int i = 1; i<=3; i++) {
  int i;
  for (i = 1; i <= 3; i++)
  {
    double d  = xyz2(i) - xyz1(i);
    bool   eq = (std::abs(d) < tol);
    if (eq)
    {
      neq++;
      continue;
    }
    if (d < 0.)
      n2sup++;
  }
  for (i = 4; i <= 6; i++)
  {
    double d  = xyz2(i) - xyz1(i);
    bool   eq = (std::abs(d) < tol);
    if (eq)
    {
      neq++;
      continue;
    }
    if (d > 0.)
      n2sup++;
  }
  if (n2sup + neq != 6)
    return 0;
  if (neq == 6)
    return 0;

  int ires = (n2sup > 0) ? 2 : 1;
  return ires;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_SameOri(const TopoDS_Edge& E1, const TopoDS_Edge& E2)
// prequesitory : 1- <E1> and <E2> share same domain,
//                2- C3d<E1> contains C3d<E2>
{
  double f, l;
  FUN_tool_bounds(E2, f, l);
  double x   = 0.345;
  double mid = x * f + (1 - x) * l;
  gp_Pnt Pmid;
  FUN_tool_value(mid, E2, Pmid);
  gp_Vec tmp;
  bool   ok = TopOpeBRepTool_TOOL::TggeomE(mid, E2, tmp);
  if (!ok)
    return false;
  gp_Dir             tgE2(tmp);
  TopAbs_Orientation oriE2 = E2.Orientation();
  if (M_REVERSED(oriE2))
    tgE2.Reverse();

  double pE1, dist;
  ok          = FUN_tool_projPonE(Pmid, E1, pE1, dist);
  double tol1 = BRep_Tool::Tolerance(E1);
  double tol2 = BRep_Tool::Tolerance(E2);
  double tol  = std::max(tol1, tol2) * 10.;
  if (dist > tol)
    return false;
  if (!ok)
    return false;
  ok = TopOpeBRepTool_TOOL::TggeomE(pE1, E1, tmp);
  if (!ok)
    return false;
  gp_Dir             tgE1(tmp);
  TopAbs_Orientation oriE1 = E1.Orientation();
  if (M_REVERSED(oriE1))
    tgE1.Reverse();

  bool sameori = (tgE2.Dot(tgE1) > 0.);
  return sameori;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_haspc(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  double                    f, l, tol;
  occ::handle<Geom2d_Curve> C2d  = FC2D_CurveOnSurface(E, F, f, l, tol);
  bool                      null = C2d.IsNull();
  return !null;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_pcurveonF(const TopoDS_Face& F, TopoDS_Edge& E)
{
  double                  f, l;
  occ::handle<Geom_Curve> C3d = BRep_Tool::Curve(E, f, l);
  if (C3d.IsNull())
    return false;
  double                    tolReached2d;
  occ::handle<Geom2d_Curve> C2d =
    TopOpeBRepTool_CurveTool::MakePCurveOnFace(F, C3d, tolReached2d, f, l);
  if (C2d.IsNull())
    return false;

  double       tolE = BRep_Tool::Tolerance(E);
  BRep_Builder BB;
  BB.UpdateEdge(E, C2d, F, tolE);
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_pcurveonF(const TopoDS_Face&               fF,
                                        TopoDS_Edge&                     faultyE,
                                        const occ::handle<Geom2d_Curve>& C2d,
                                        TopoDS_Face&                     newf)
{
  BRep_Builder                   BB;
  TopExp_Explorer                exw(fF, TopAbs_WIRE);
  NCollection_List<TopoDS_Shape> low;
  bool                           hasnewf = false;
  for (; exw.More(); exw.Next())
  {
    const TopoDS_Shape& w = exw.Current();

    NCollection_List<TopoDS_Shape> loe;
    bool                           hasneww = false;
    TopExp_Explorer                exe(w, TopAbs_EDGE);
    for (; exe.More(); exe.Next())
    {
      const TopoDS_Edge& e     = TopoDS::Edge(exe.Current());
      bool               equal = e.IsEqual(faultyE);
      if (!equal)
      {
        loe.Append(e);
        continue;
      }

      double        tole = BRep_Tool::Tolerance(e);
      TopoDS_Vertex vf, vl;
      TopExp::Vertices(e, vf, vl);

      TopoDS_Edge newe = faultyE;
      //      TopoDS_Edge newe; FUN_ds_CopyEdge(e,newe); newe.Orientation(TopAbs_FORWARD);
      //      vf.Orientation(TopAbs_FORWARD);  BB.Add(newe,vf); FUN_ds_Parameter(newe,vf,parf);
      //      vl.Orientation(TopAbs_REVERSED); BB.Add(newe,vl); FUN_ds_Parameter(newe,vl,parl);
      BB.UpdateEdge(newe, C2d, fF, tole);
      newe.Orientation(e.Orientation());
      loe.Append(newe);
      hasneww = true;
      hasnewf = true;
    }
    if (hasneww)
    {
      TopoDS_Wire neww;
      bool        ok = FUN_tool_MakeWire(loe, neww);
      if (!ok)
        return false;
      low.Append(neww);
    }
    else
      low.Append(w);
  } // exw
  if (hasnewf)
  {
    TopoDS_Shape aLocalShape = fF.EmptyCopied();
    newf                     = TopoDS::Face(aLocalShape);
    //    newf = TopoDS::Face(fF.EmptyCopied());
    for (NCollection_List<TopoDS_Shape>::Iterator itw(low); itw.More(); itw.Next())
    {
      const TopoDS_Shape w = itw.Value();
      BB.Add(newf, w);
    }
    return true;
  }
  return false;
}

//=================================================================================================

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_curvesSO(const TopoDS_Edge& E1,
                                       const double       p1,
                                       const TopoDS_Edge& E2,
                                       const double       p2,
                                       bool&              so)
{
  BRepAdaptor_Curve BAC1(E1);
  BRepAdaptor_Curve BAC2(E2);
  gp_Vec            tg1;
  bool              ok = TopOpeBRepTool_TOOL::TggeomE(p1, E1, tg1);
  if (!ok)
    return false; // NYIRAISE
  gp_Vec tg2;
  ok = TopOpeBRepTool_TOOL::TggeomE(p2, E2, tg2);
  if (!ok)
    return false; // NYIRAISE
  double tola = Precision::Angular() * 1.e3;
  bool   oppo = tg1.IsOpposite(tg2, tola);
  bool   samo = tg1.IsParallel(tg2, tola);
  if (oppo)
    so = false;
  else if (samo)
    so = true;
  else
    return false;
  return true;
}

Standard_EXPORT bool FUN_tool_curvesSO(const TopoDS_Edge& E1,
                                       const double       p1,
                                       const TopoDS_Edge& E2,
                                       bool&              so)
{
  // prequesitory : P3d(E1,p1) is IN 1d(E2)
  double p2 = 0.;
  bool   ok = FUN_tool_parE(E1, p1, E2, p2);
  if (!ok)
    return false;
  ok = FUN_tool_curvesSO(E1, p1, E2, p2, so);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_curvesSO(const TopoDS_Edge& E1, const TopoDS_Edge& E2, bool& so)
{
  // prequesitory : E1 is IN 1d(E2)
  TopoDS_Vertex vf1, vl1;
  TopExp::Vertices(E1, vf1, vl1);
  bool          closed1 = vf1.IsSame(vl1);
  TopoDS_Vertex vf2, vl2;
  TopExp::Vertices(E2, vf2, vl2);
  bool closed2 = vf2.IsSame(vl2);
  bool project = false;
  if (closed1 || closed2)
    project = true;
  else
  {
    if (vf1.IsSame(vf2))
      so = true;
    else if (vl1.IsSame(vl2))
      so = true;
    else if (vf1.IsSame(vl2))
      so = false;
    else if (vl1.IsSame(vf2))
      so = false;
    else
      project = true;
  }
  if (project)
  {
    double f, l;
    FUN_tool_bounds(E1, f, l);
    double x  = 0.45678;
    double p1 = (1 - x) * l + x * f;
    bool   ok = FUN_tool_curvesSO(E1, p1, E2, so);
    return ok;
  }
  return true;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_findAncestor(const NCollection_List<TopoDS_Shape>& lF,
                                           const TopoDS_Edge&                    E,
                                           TopoDS_Face&                          Fanc)
{
  NCollection_List<TopoDS_Shape>::Iterator it(lF);
  for (; it.More(); it.Next())
  {
    const TopoDS_Face& F = TopoDS::Face(it.Value());
    TopAbs_Orientation dummy;
    bool               found = FUN_tool_orientEinF(E, F, dummy);
    if (found)
    {
      Fanc = F;
      return true;
    }
  }
  return false;
}

//=================================================================================================

// FUN_ds_* methods are methods of TopOpeBRepDS_BuildTool
// that cannot be called (cyclic dependencies)

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_CopyEdge(const TopoDS_Shape& Ein, TopoDS_Shape& Eou)
{
  double      f, l;
  TopoDS_Edge E1 = TopoDS::Edge(Ein);
  BRep_Tool::Range(E1, f, l);
  Eou             = Ein.EmptyCopied();
  TopoDS_Edge  E2 = TopoDS::Edge(Eou);
  BRep_Builder BB;
  BB.Range(E2, f, l);
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_Parameter(const TopoDS_Shape& E, const TopoDS_Shape& V, const double P)
{
  BRep_Builder            BB;
  const TopoDS_Edge&      e = TopoDS::Edge(E);
  const TopoDS_Vertex&    v = TopoDS::Vertex(V);
  double                  p = P;
  TopLoc_Location         loc;
  double                  f, l;
  occ::handle<Geom_Curve> C = BRep_Tool::Curve(e, loc, f, l);
  if (!C.IsNull() && C->IsPeriodic())
  {
    double per = C->Period();

    TopAbs_Orientation oV = TopAbs_FORWARD;

    TopExp_Explorer exV(e, TopAbs_VERTEX);
    for (; exV.More(); exV.Next())
    {
      const TopoDS_Vertex& vofe = TopoDS::Vertex(exV.Current());
      if (vofe.IsSame(v))
      {
        oV = vofe.Orientation();
        break;
      }
    }
    if (exV.More())
    {
      if (oV == TopAbs_REVERSED)
      {
        if (p < f)
        {
          double pp = ElCLib::InPeriod(p, f, f + per);
          p         = pp;
        }
      }
    }
  }
  BB.UpdateVertex(v, p, e, 0);
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_MakeWire(const NCollection_List<TopoDS_Shape>& loE, TopoDS_Wire& newW)
{
  newW.Nullify();
  BRep_Builder BB;
  BB.MakeWire(newW);
  NCollection_List<TopoDS_Shape>::Iterator itloE(loE);
  for (; itloE.More(); itloE.Next())
  {
    const TopoDS_Edge& E = TopoDS::Edge(itloE.Value());
    BB.Add(newW, E);
  }
  return true;
}
