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

#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Bnd_Box2d.hxx>
#include <Bnd_Box.hxx>
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
#include <TopoDS_Wire.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <ElCLib.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_2d.hxx>

#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

//---------------------------------------------
#define  M_FORWARD(ori) (ori == TopAbs_FORWARD) 
#define M_REVERSED(ori) (ori == TopAbs_REVERSED) 
#define M_INTERNAL(ori) (ori == TopAbs_INTERNAL) 
#define M_EXTERNAL(ori) (ori == TopAbs_EXTERNAL) 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_tolUV(const TopoDS_Face& F,Standard_Real& tolu,Standard_Real& tolv)
{
  Standard_Real tolF = BRep_Tool::Tolerance(TopoDS::Face(F));
  BRepAdaptor_Surface BS(TopoDS::Face(F));
  tolu = BS.UResolution(tolF);
  tolv = BS.VResolution(tolF); 
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_direct(const TopoDS_Face& F,Standard_Boolean& direct)
{
  BRepAdaptor_Surface BS(TopoDS::Face(F));
  GeomAbs_SurfaceType ST = BS.GetType();
  Standard_Boolean plane = (ST == GeomAbs_Plane);
  Standard_Boolean cyl   = (ST == GeomAbs_Cylinder);
  Standard_Boolean cone  = (ST == GeomAbs_Cone);
  Standard_Boolean sphe  = (ST == GeomAbs_Sphere);
  Standard_Boolean torus  = (ST == GeomAbs_Torus);
  if (plane) {const gp_Pln& plpl = BS.Plane(); direct = plpl.Direct();}
  if (cyl)   {const gp_Cylinder& cycy = BS.Cylinder(); direct = cycy.Direct();}
  if (cone)  {const gp_Cone& coco = BS.Cone(); direct = coco.Direct();}
  if (sphe)  {const gp_Sphere& spsp = BS.Sphere(); direct = spsp.Direct();}
  if (torus) {const gp_Torus& toto = BS.Torus(); direct = toto.Direct();}  
  Standard_Boolean ok = plane || cyl || cone || sphe || torus;
  return ok;
}

/*// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_IsUViso(const TopoDS_Shape& E,const TopoDS_Shape& F,
				     Standard_Boolean& isoU,Standard_Boolean& isoV,
				     gp_Dir2d& d2d,gp_Pnt2d& o2d)
{
  isoU = isoV = Standard_False; Standard_Real f,l,tol; 
  Handle(Geom2d_Curve) PC = 
    FC2D_CurveOnSurface(TopoDS::Edge(E),TopoDS::Face(F),f,l,tol);
  if (PC.IsNull()) return Standard_False;
  Standard_Boolean iso = FUN_tool_IsUViso(PC,isoU,isoV,d2d,o2d);
  return iso;
}*/

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_geombounds(const TopoDS_Face& F,
					Standard_Real& u1,Standard_Real& u2,Standard_Real& v1,Standard_Real& v2)
{
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F);
  if (S.IsNull()) return Standard_False;
  S->Bounds(u1,u2,v1,v2);
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_bounds(const TopoDS_Shape& Sh,
				                 Standard_Real& u1,
				                 Standard_Real& u2,
				                 Standard_Real& v1,
//				                 Standard_Real& v2)
				                 Standard_Real& )
{
  Bnd_Box2d B2d;
  const TopoDS_Face& F = TopoDS::Face(Sh);
  TopExp_Explorer ex(F,TopAbs_WIRE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Wire W = TopoDS::Wire(ex.Current());
    Bnd_Box2d newB2d; FUN_tool_mkBnd2d(W,F,newB2d);
    B2d.Add(newB2d);
  }
  B2d.Get(u1,v1,u2,v1);
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_isobounds(const TopoDS_Shape& Sh,
				       Standard_Real& u1,Standard_Real& u2,Standard_Real& v1,Standard_Real& v2)
{
  const TopoDS_Face& F = TopoDS::Face(Sh);
  u1 = v1 = 1.e7;  u2 = v2 = -1.e7;

  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F);
  if (S.IsNull()) return Standard_False;

  Standard_Boolean uclosed,vclosed; Standard_Real uperiod,vperiod; 

//  Standard_Boolean uvclosed =

  FUN_tool_closedS(F,uclosed,uperiod,vclosed,vperiod);

//  Standard_Real uf,ul,vf,vl; S->Bounds(uf,ul,vf,vl);

//  if (!uvclosed) {
//    u1 = uf; v1 = vf; u2 = ul; v2 = vl;
//    return Standard_True;
//  }
  TopExp_Explorer ex(F,TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    Standard_Real f,l,tol; Handle(Geom2d_Curve) PC = FC2D_CurveOnSurface(E,F,f,l,tol);
    if (PC.IsNull()) return Standard_False;
    
    Standard_Boolean isou,isov; gp_Pnt2d o2d; gp_Dir2d d2d;
    Standard_Boolean isouv = TopOpeBRepTool_TOOL::UVISO(PC,isou,isov,d2d,o2d);

    if (isouv) {
      gp_Pnt2d p2df = PC->Value(f); gp_Pnt2d p2dl = PC->Value(l);
      u1 = Min(p2df.X(),u1); u2 = Max(p2df.X(),u2); v1 = Min(p2df.Y(),v1); v2 = Max(p2df.Y(),v2); 
      u1 = Min(p2dl.X(),u1); u2 = Max(p2dl.X(),u2); v1 = Min(p2dl.Y(),v1); v2 = Max(p2dl.Y(),v2); 
    }
    if (!isouv) return Standard_False;
    // ====================
  }
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_outbounds(const TopoDS_Shape& Sh,
				       Standard_Real& u1,Standard_Real& u2,Standard_Real& v1,Standard_Real& v2,Standard_Boolean& outbounds)
{
  Handle(Geom_Surface) S = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(Sh));
  if (S.IsNull()) return Standard_False;
  Standard_Real uf,ul,vf,vl; S->Bounds(uf,ul,vf,vl);

  outbounds = Standard_False;
  Standard_Boolean ok = FUN_tool_bounds(Sh,u1,u2,v1,v2);
  if (!ok) return Standard_False;

  const TopoDS_Face& F = TopoDS::Face(Sh);
  Standard_Boolean uclosed,vclosed; Standard_Real uperiod,vperiod; 
  FUN_tool_closedS(F,uclosed,uperiod,vclosed,vperiod);
  Standard_Real tolp = 1.e-6;
  
  if (uclosed) {
    Standard_Real dd = u2-u1;
    if (dd > uperiod + tolp) {u1 = uf; v1 = vf; u2 = ul; v2 = vl; outbounds=Standard_True;}
  }
  if (vclosed) {
    Standard_Real dd = v2-v1;
    if (dd > vperiod + tolp) {u1 = uf; v1 = vf; u2 = ul; v2 = vl; outbounds=Standard_True;}
  }
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_PinC(const gp_Pnt& P,
				  const BRepAdaptor_Curve& BAC,
				  const Standard_Real pmin,const Standard_Real pmax,
				  const Standard_Real tol)
{
  // returns true if <P> is on <C> under a given tolerance <tol>
  Standard_Boolean PinC = Standard_False; 
  Extrema_ExtPC ponc(P,BAC,pmin,pmax);
  Standard_Boolean ok = ponc.IsDone();
  if ( ok ) {
    if ( ponc.NbExt() ) {
      Standard_Integer i = FUN_tool_getindex(ponc);
      Standard_Real d2 = ponc.SquareDistance(i);
      PinC = (d2 <= tol * tol);
    }
  }
  return PinC;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_PinC(const gp_Pnt& P,
				  const BRepAdaptor_Curve& BAC,
				  const Standard_Real tol)
{
  // returns true if <P> is on <C> under a given tolerance <tol>
  Standard_Boolean PinC = Standard_False;
  Standard_Real pmin = BAC.FirstParameter();
  Standard_Real pmax = BAC.LastParameter();
  PinC = FUN_tool_PinC(P,BAC,pmin,pmax,tol);
  return PinC;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_value(const Standard_Real par,const TopoDS_Edge& E,gp_Pnt& P)
{
  BRepAdaptor_Curve BAC(E);
  Standard_Real f = BAC.FirstParameter();
  Standard_Real l = BAC.LastParameter();
  Standard_Boolean ok = (f <= par) && (par <= l);
  if (!ok) return Standard_False;
  P = BAC.Value(par);
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_value(const gp_Pnt2d& UV,const TopoDS_Face& F,gp_Pnt& P)
{
  BRepAdaptor_Surface BS(F);
  P = BS.Value(UV.X(),UV.Y());
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT TopAbs_State FUN_tool_staPinE(const gp_Pnt& P,const TopoDS_Edge& E,const Standard_Real tol)
{
  // project point on curve,
  // if projection fails,returns UNKNOWN.
  // finds a point <pnear> on edge <E> / d(<pnear>,<P>) < tol
  //   => returns IN
  // else,returns OUT.
  TopAbs_State sta = TopAbs_UNKNOWN;
  BRepAdaptor_Curve BAC(E);
  Extrema_ExtPC ProjonBAC(P,BAC);
  if (ProjonBAC.IsDone() && ProjonBAC.NbExt()>0) {
    Standard_Integer i = FUN_tool_getindex(ProjonBAC);
    gp_Pnt Pnear = ProjonBAC.Point(i).Value();
    Standard_Real d = Pnear.Distance(P);
    sta = (d < tol)? TopAbs_IN: TopAbs_OUT;
  }
  return sta;
}
// ----------------------------------------------------------------------
Standard_EXPORT TopAbs_State FUN_tool_staPinE(const gp_Pnt& P,const TopoDS_Edge& E)
{
//  Standard_Real tol = Precision::Confusion()*10.;
  Standard_Real tol3d = BRep_Tool::Tolerance(E)*1.e2;//KKKKK a revoir xpu(CTS21118,f14ou,GI13)
  TopAbs_State sta = FUN_tool_staPinE(P,E,tol3d);
  return sta;
}

// ----------------------------------------------------------------------
//  subshape's orientation :
//    - vertex orientation in edge 
//    - edge's orientation in face.Oriented(FORWARD)
// ----------------------------------------------------------------------

#define FIRST   (1)
#define LAST    (2)
#define CLOSING (3)

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Integer FUN_tool_orientVinE(const TopoDS_Vertex& v,const TopoDS_Edge& e)
{
  Standard_Integer result = 0;
  TopoDS_Vertex vf,vl; TopExp::Vertices(e,vf,vl);
  Standard_Boolean visf = v.IsSame(vf);
  Standard_Boolean visl = v.IsSame(vl);
  if (visf) result = FIRST;
  if (visl) result = LAST;
  if (visf && visl) result = CLOSING;
  return result;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_orientEinF(const TopoDS_Edge& E,const TopoDS_Face& F,TopAbs_Orientation& oriEinF)
{
  oriEinF = TopAbs_FORWARD;
  TopExp_Explorer e(F,TopAbs_EDGE);
  for (;e.More();e.Next()) {
    const TopoDS_Shape& EF = e.Current(); 
    if (EF.IsSame(E)) { 
      oriEinF=EF.Orientation();
      break;
    }
  }
  Standard_Boolean ok=e.More();
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_orientEinFFORWARD(const TopoDS_Edge& E,const TopoDS_Face& F,TopAbs_Orientation& oriEinF)
{
  // <oriEinF> : dummy for closing edge <E> of <F>.
  // returns false if <E> is not bound of <F>
  // else,<oriEinF> = orientation of the edge in <F> oriented FORWARD.
  TopoDS_Shape aLocalShape = F.Oriented(TopAbs_FORWARD);
  TopoDS_Face FF = TopoDS::Face(aLocalShape);
//  TopoDS_Face FF = TopoDS::Face(F.Oriented(TopAbs_FORWARD));
  oriEinF = TopAbs_EXTERNAL;
  TopAbs_Orientation reso; Standard_Boolean ok = ::FUN_tool_orientEinF(E,FF,reso);
  if (ok) oriEinF = reso;
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_EboundF(const TopoDS_Edge& E,const TopoDS_Face& F)
{
  TopAbs_Orientation ori; Standard_Boolean ok = FUN_tool_orientEinFFORWARD(E,F,ori);
  if (!ok) return Standard_False;
  Standard_Boolean closingE = BRep_Tool::IsClosed(E,F);
  if (closingE) return Standard_True;
  Standard_Boolean notbound = (ori == TopAbs_INTERNAL) || (ori == TopAbs_EXTERNAL);
  return notbound;
}

// ----------------------------------------------------------------------
Standard_EXPORT gp_Vec FUN_tool_nggeomF(const gp_Pnt2d& p2d,const TopoDS_Face& F)
{
  Handle(Geom_Surface) S = BRep_Tool::Surface(F);
  gp_Vec ngF(FUN_tool_ngS(p2d,S));
  return ngF;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_nggeomF(const Standard_Real& paronE,const TopoDS_Edge& E,const TopoDS_Face& F,
				     gp_Vec& nggeomF,const Standard_Real tol)
{
  // <p2d> :
  Standard_Real f,l; gp_Pnt2d p2d;
  Standard_Boolean project = Standard_True;
  TopAbs_Orientation oef; 
  Standard_Boolean edonfa = FUN_tool_orientEinFFORWARD(E,F,oef);
  if (edonfa) {
    Standard_Real ttol; 
    Handle(Geom2d_Curve) c2d = FC2D_CurveOnSurface(E,F,f,l,ttol);
    project = c2d.IsNull();
    if (!project) 
      p2d = c2d->Value(paronE);
  } 
  if (project) {
    BRepAdaptor_Curve BC(E);
    gp_Pnt p3d = BC.Value(paronE);
    Standard_Real d;  Standard_Boolean ok = FUN_tool_projPonF(p3d,F,p2d,d);

    //modified by NIZHNY-MZV  Wed Dec  1 13:55:08 1999
    //if !ok try to compute new pcurve 
    if(!ok) {
      Standard_Real ttol; 
      Handle(Geom2d_Curve) c2d = FC2D_CurveOnSurface(E,F,f,l,ttol);
      ok = !c2d.IsNull();
      if (ok) 
	p2d = c2d->Value(paronE);
    }
    if (!ok) return Standard_False;
    //modified by NIZHNY-MZV  Wed Dec  1 13:56:14 1999
    //xpu010698
    gp_Pnt p3duv; FUN_tool_value(p2d,F,p3duv);
    Standard_Real dd = p3duv.Distance(p3d);
    if (dd > tol) return Standard_False;
    //xpu010698
  }  
  nggeomF = FUN_tool_nggeomF(p2d,F);
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_nggeomF(const Standard_Real& paronE,const TopoDS_Edge& E,const TopoDS_Face& F,
				     gp_Vec& nggeomF)
{
  Standard_Real tol3d = BRep_Tool::Tolerance(F);
  tol3d *= 1.e2;
  Standard_Boolean ok = FUN_tool_nggeomF(paronE,E,F,nggeomF,tol3d);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_EtgF(const Standard_Real& paronE,const TopoDS_Edge& E,
				  const gp_Pnt2d& p2d,const TopoDS_Face& F,
				  const Standard_Real tola)
{
  gp_Vec tgE; Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(paronE,E,tgE);
  if (!ok) return Standard_False;//NYIRAISE

  gp_Vec ngF = FUN_tool_nggeomF(p2d,F);
  Standard_Real prod = tgE.Dot(ngF);
  Standard_Boolean tgt = Abs(prod) < tola;
  return tgt;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_EtgOOE(const Standard_Real& paronE,const TopoDS_Edge& E,
				    const Standard_Real& paronOOE,const TopoDS_Edge& OOE,
				    const Standard_Real  tola)
{
  gp_Vec tgOOE; Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(paronOOE,OOE,tgOOE);
  if (!ok) return Standard_False;//NYIRAISE
  gp_Vec tgE;       ok = TopOpeBRepTool_TOOL::TggeomE(paronE,E,tgE);
  if (!ok) return Standard_False;//NYIRAISE
  Standard_Real prod = tgOOE.Dot(tgE);
  Standard_Boolean tg = (Abs(1- Abs(prod)) < tola);
  return tg;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_nearestISO(const TopoDS_Face& F,const Standard_Real xpar,const Standard_Boolean isoU,Standard_Real& xinf,Standard_Real& xsup)
{  
  // IMPORTANT : xinf=xf,xsup=xl are INITIALIZED with first and last x 
  //             parameters of F (x = u,v )
  // purpose : finding greater xinf : xinf <= xpar
  //                   smaller xsup :        xpar <=xsup
  Standard_Real tol2d = 1.e-6;
  Standard_Real df = xpar-xinf; Standard_Boolean onf = (Abs(df)<tol2d);
  Standard_Real dl = xpar-xsup; Standard_Boolean onl = (Abs(dl)<tol2d);
  if (onf || onl) return Standard_True;

  Standard_Boolean isoV = !isoU;
  TopExp_Explorer ex(F,TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    Standard_Boolean isou,isov; gp_Pnt2d o2d; gp_Dir2d d2d;
    Standard_Boolean isouv = TopOpeBRepTool_TOOL::UVISO(E,F,isou,isov,d2d,o2d);
    if (!isouv) return Standard_False;
    
    Standard_Boolean compare = (isoU && isou) || (isoV && isov);
    if (!compare) return Standard_False;
    Standard_Real xvalue = isou ? o2d.X() : o2d.Y(); 

    if (xvalue < xpar) 
      if (xinf < xvalue) xinf = xvalue;
    if (xpar < xvalue)
      if (xvalue < xsup) xsup = xvalue;
  }
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_EitangenttoFe(const gp_Dir& ngFe,const TopoDS_Edge& Ei,
					   const Standard_Real parOnEi)
{
  // returns true if <Ei> is tangent to Fe at point
  // p3d of param <parOnEi>,<ngFe> is normal to Fe at p3d.    
  gp_Vec tgEi; Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(parOnEi,Ei,tgEi);  
  if (!ok) return Standard_False;//NYIRAISE

  Standard_Real prod = ngFe.Dot(tgEi);
  Standard_Real tol = Precision::Parametric(Precision::Confusion());
  Standard_Boolean tangent = (Abs(prod) <= tol);
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
Standard_EXPORT Standard_Boolean FUN_tool_line(const TopoDS_Edge& E)
{
  BRepAdaptor_Curve BC(E);
  Standard_Boolean line = (BC.GetType() == GeomAbs_Line);
  return line;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_plane(const TopoDS_Shape& F)
{
  Handle(Geom_Surface) S = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  GeomAdaptor_Surface GS(S);
  return (GS.GetType() == GeomAbs_Plane);
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_cylinder(const TopoDS_Shape& F)
{
  Handle(Geom_Surface) S = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  GeomAdaptor_Surface GS(S);
  return (GS.GetType() == GeomAbs_Cylinder);
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_closedS(const TopoDS_Shape& F,
				     Standard_Boolean& uclosed,Standard_Real& uperiod,
				     Standard_Boolean& vclosed,Standard_Real& vperiod)
{
//  const Handle(Geom_Surface)& S = BRep_Tool::Surface(TopoDS::Face(F));
  Handle(Geom_Surface) S =TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  if (S.IsNull()) return Standard_False;
  Standard_Boolean closed = FUN_tool_closed(S,uclosed,uperiod,vclosed,vperiod);
  return closed;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_closedS(const TopoDS_Shape& F)
{
  Standard_Boolean uclosed=Standard_False,vclosed=Standard_False; Standard_Real uperiod=0.,vperiod=0.;
  Standard_Boolean closed = FUN_tool_closedS(F,uclosed,uperiod,vclosed,vperiod);
  return closed;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_closedS(const TopoDS_Shape& F, 
				     Standard_Boolean& inU, Standard_Real& xmin, Standard_Real& xper)
{
  Standard_Boolean uclosed,vclosed; Standard_Real uperiod,vperiod; 
  Handle(Geom_Surface) S =TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  if (S.IsNull()) return Standard_False;
  Standard_Boolean closed = FUN_tool_closed(S,uclosed,uperiod,vclosed,vperiod);
  if (!closed) return Standard_False;
  Standard_Real u1,u2,v1,v2; S->Bounds(u1,u2,v1,v2);

  inU = uclosed;
  xper = inU ? uperiod : vperiod;
  xmin = inU ? u1 : v1;
  return Standard_False ;
}
// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_mkBnd2d(const TopoDS_Shape& W,const TopoDS_Shape& FF,Bnd_Box2d& B2d)
{
  // greater <B> with <W>'s UV representation on <F>
  Standard_Real tol = 1.e-8;
  Bnd_Box2d newB2d;
  TopExp_Explorer ex;
  for (ex.Init(W,TopAbs_EDGE); ex.More(); ex.Next()) {
//  for (TopExp_Explorer ex(W,TopAbs_EDGE); ex.More(); ex.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    const TopoDS_Face& F = TopoDS::Face(FF);
    Standard_Real f,l,tolpc; Handle(Geom2d_Curve) pc; 
    Standard_Boolean haspc = FC2D_HasCurveOnSurface(E,F);
    if (!haspc) { 
      Standard_Real tolE = BRep_Tool::Tolerance(E); 
      pc = FC2D_CurveOnSurface(E,F,f,l,tolpc);
      Standard_Real newtol = Max(tolE,tolpc);
      BRep_Builder BB; BB.UpdateEdge(E,pc,F,newtol); 
    } 
    BRepAdaptor_Curve2d BC2d(E,F); 
    BndLib_Add2dCurve::Add(BC2d,tol,newB2d);
  } //ex(W,EDGE)

  FUN_tool_UpdateBnd2d(B2d,newB2d);
}

// ----------------------------------------------------------------------
//  closing topologies :
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_IsClosingE(const TopoDS_Edge& E,const TopoDS_Shape& S,const TopoDS_Face& F)
{   
  Standard_Integer nbocc = 0;
  TopExp_Explorer exp;
  for (exp.Init(S,TopAbs_EDGE);exp.More();exp.Next()) 
//  for (TopExp_Explorer exp(S,TopAbs_EDGE);exp.More();exp.Next()) 
    if (exp.Current().IsSame(E)) nbocc++;
  if (nbocc != 2) return Standard_False;
  return BRep_Tool::IsClosed(E,F);
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_ClosingE(const TopoDS_Edge& E,const TopoDS_Wire& W,const TopoDS_Face& F)
{
  Standard_Boolean clo = FUN_tool_IsClosingE(E,W,F); 
  return clo;
}

/*// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_ClosedE(const TopoDS_Edge& E,TopoDS_Shape& vclosing)
{
  // returns true if <E> has a closing vertex <vclosing>
//  return E.IsClosed();
  Standard_Boolean isdgE = BRep_Tool::Degenerated(E);
  if (isdgE) return Standard_False;

  TopoDS_Shape vv; vclosing.Nullify();
  TopExp_Explorer ex(E,TopAbs_VERTEX);
  for (; ex.More(); ex.Next()) {
    const TopoDS_Shape& v = ex.Current();
    if (M_INTERNAL(v.Orientation())) continue;
    if (vv.IsNull()) vv = v;
    else if (v.IsSame(vv))
      {vclosing = vv; return Standard_True;}
  }
  return Standard_False;
}*/

// ----------------------------------------------------------------------
//  shared topologies :
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_inS(const TopoDS_Shape& subshape,const TopoDS_Shape& shape)
{
  TopAbs_ShapeEnum sstyp = subshape.ShapeType();
  TopTools_IndexedMapOfShape M; TopExp::MapShapes(shape,sstyp,M);
  Standard_Boolean isbound = M.Contains(subshape);
  return isbound;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_Eshared(const TopoDS_Shape& v,const TopoDS_Shape& F1,const TopoDS_Shape& F2,
				     TopoDS_Shape& Eshared)
//purpose : finds out <Eshared> shared by <F1> and <F2>,
//          with <V> bound of <Eshared>
{
  TopTools_ListOfShape e1s;
  TopExp_Explorer ex(F1,TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Shape& e1 = ex.Current();
    Standard_Boolean e1ok = Standard_False;
    TopExp_Explorer exv(e1,TopAbs_VERTEX);
    for (; exv.More(); exv.Next())
      if (exv.Current().IsSame(v)) {e1ok = Standard_True; break;}
    if (e1ok) e1s.Append(e1);
  }
  ex.Init(F2,TopAbs_EDGE);  
  for (; ex.More(); ex.Next()){
    const TopoDS_Shape& e2 = ex.Current();
    TopTools_ListIteratorOfListOfShape it1(e1s);
    for (; it1.More(); it1.Next())
      if (it1.Value().IsSame(e2)) {Eshared = e2; return Standard_True;}
  }
  return Standard_False;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_parVonE(const TopoDS_Vertex& v,const TopoDS_Edge& E,Standard_Real& par) 
{
  Standard_Real tol = Precision::Confusion();
  Standard_Boolean isVofE = Standard_False;;
  TopExp_Explorer ex;
  for (ex.Init(E,TopAbs_VERTEX); ex.More(); ex.Next()) {
//  for (TopExp_Explorer ex(E,TopAbs_VERTEX); ex.More(); ex.Next()) {
    isVofE = ex.Current().IsSame(v);
    if (isVofE) {
      par = BRep_Tool::Parameter(TopoDS::Vertex(ex.Current()),E);
      break; 
    }
  }
  if (!isVofE) { 
    gp_Pnt pt = BRep_Tool::Pnt(v);
    // <v> can share same domain with a vertex of <E>
    for (ex.Init(E,TopAbs_VERTEX); ex.More(); ex.Next()) {
      const TopoDS_Vertex& vex = TopoDS::Vertex(ex.Current());
      gp_Pnt ptex = BRep_Tool::Pnt(vex);
      if (ptex.IsEqual(pt,tol)) {
	par = BRep_Tool::Parameter(vex,E);
	return Standard_True;
      }
    }

//    Standard_Real f,l; 
    BRepAdaptor_Curve BAC(E);
    Extrema_ExtPC pro(pt,BAC);
    Standard_Boolean done = pro.IsDone() && (pro.NbExt() >0);
    if (!done) return Standard_False;
    Standard_Integer i = FUN_tool_getindex(pro);
    par = pro.Point(i).Parameter();
  }
  return Standard_True;
} 

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_parE(const TopoDS_Edge& E0,const Standard_Real& par0,
				  const TopoDS_Edge& E,Standard_Real& par,const Standard_Real tol)
{
  gp_Pnt P; Standard_Boolean ok = FUN_tool_value(par0,E0,P);
  if (!ok) return Standard_False;

  Standard_Real dist; ok = FUN_tool_projPonE(P,E,par,dist);
  if (!ok) return Standard_False;

  ok = (dist < tol);
  return ok;
}
// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_parE(const TopoDS_Edge& E0,const Standard_Real& par0,
				  const TopoDS_Edge& E,Standard_Real& par)
// ? <par> :  P -> <par0> on <E0>,<par> on <E>
// prequesitory : point(par0 ,E0) is IN 1d(E)
{
  Standard_Real tol3d = BRep_Tool::Tolerance(E)*1.e2;//KKKKK a revoir xpu(CTS21118,f14ou,GI13)
  Standard_Boolean ok = FUN_tool_parE(E0,par0,E,par,tol3d);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_parF(const TopoDS_Edge& E,const Standard_Real& par,
				  const TopoDS_Face& F,gp_Pnt2d& UV, const Standard_Real tol3d)
// ? <UV> : P -> <par> on <E>,<UV> on <F>
{
  gp_Pnt P; Standard_Boolean ok = FUN_tool_value(par,E,P);
  if (!ok) return Standard_False;
 
  Standard_Real dist; ok = FUN_tool_projPonF(P,F,UV,dist);
  if (!ok) return Standard_False;

  ok = (dist < tol3d);
  return ok;
}
Standard_EXPORT Standard_Boolean FUN_tool_parF(const TopoDS_Edge& E,const Standard_Real& par,
				  const TopoDS_Face& F,gp_Pnt2d& UV)
{  
  Standard_Real tol3d = BRep_Tool::Tolerance(F)*1.e2; //KK xpu
  Standard_Boolean ok = FUN_tool_parF(E,par,F,UV,tol3d);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_paronEF(const TopoDS_Edge& E,const Standard_Real& par,
				     const TopoDS_Face& F,gp_Pnt2d& UV, const Standard_Real tol3d)
// <E> is on <F> ? <UV> : P -> <par> on <E>,<UV> on <F>
{
  Standard_Real f,l,tol; Handle(Geom2d_Curve) PC = FC2D_CurveOnSurface(E,F,f,l,tol);
  if (PC.IsNull()) {
    Standard_Boolean ok = FUN_tool_parF(E,par,F,UV,tol3d);
    return ok;
  }  
  Standard_Boolean ok = (f <= par) && (par <= l);
  if (!ok) return Standard_False;
  UV = PC->Value(par);
  return Standard_True;
}
Standard_EXPORT Standard_Boolean FUN_tool_paronEF(const TopoDS_Edge& E,const Standard_Real& par,
				     const TopoDS_Face& F,gp_Pnt2d& UV)
{
  Standard_Real tol3d = BRep_Tool::Tolerance(F)*1.e2; // KKxpu
  Standard_Boolean ok = FUN_tool_paronEF(E,par,F,UV,tol3d);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT gp_Dir FUN_tool_dirC(const Standard_Real par,const BRepAdaptor_Curve& BAC)
{
  gp_Pnt p; gp_Vec tgE; BAC.D1(par,p,tgE); 
  gp_Dir dirC(tgE);
  return dirC;
}

// ----------------------------------------------------------------------
Standard_EXPORT gp_Vec FUN_tool_tggeomE(const Standard_Real paronE,const TopoDS_Edge& E)
{
  Standard_Boolean isdgE = BRep_Tool::Degenerated(E); 
  if (isdgE) return gp_Vec(0,0,0);
  gp_Vec dirE(FUN_tool_dirC(paronE,E)); 
  return dirE;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_line(const BRepAdaptor_Curve& BAC)
{
  Standard_Boolean line = (BAC.GetType() == GeomAbs_Line);
  return line;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_quad(const TopoDS_Edge& E)
{
  BRepAdaptor_Curve BC(E);
  GeomAbs_CurveType CT = BC.GetType();
  Standard_Boolean quad = FUN_quadCT(CT);
  return quad;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_quad(const BRepAdaptor_Curve& BAC)
{
  GeomAbs_CurveType CT = BAC.GetType();
  Standard_Boolean isquad = Standard_False;
  if (CT == GeomAbs_Line) isquad = Standard_True;
  if (CT == GeomAbs_Circle) isquad = Standard_True;
  if (CT == GeomAbs_Ellipse) isquad = Standard_True;
  if (CT == GeomAbs_Hyperbola) isquad = Standard_True;
  if (CT == GeomAbs_Parabola) isquad = Standard_True;
  return isquad;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_quad(const TopoDS_Face& F)
{
  Handle(Geom_Surface) S = TopOpeBRepTool_ShapeTool::BASISSURFACE(F);
  Standard_Boolean quad = FUN_tool_quad(S);
  return quad;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_findPinBAC(const BRepAdaptor_Curve& BAC,gp_Pnt& P,Standard_Real& par)
{
  FUN_tool_findparinBAC(BAC,par);
  BAC.D0(par,P);
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_findparinBAC(const BRepAdaptor_Curve& BAC,Standard_Real& par)
{
  Standard_Real fE = BAC.FirstParameter(),lE = BAC.LastParameter();
  Standard_Real t = 0.34567237; par = (1-t)*fE + t*lE;
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_findparinE(const TopoDS_Shape& E,Standard_Real& par)
{ 
  BRepAdaptor_Curve BAC(TopoDS::Edge(E)); 
  Standard_Boolean r = FUN_tool_findparinBAC(BAC,par);
  return r;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_findPinE(const TopoDS_Shape& E,gp_Pnt& P,Standard_Real& par)
{ 
  BRepAdaptor_Curve BAC(TopoDS::Edge(E)); 
  Standard_Boolean r = FUN_tool_findPinBAC(BAC,P,par);
  return r;
}

// ----------------------------------------------------------------------
//  tolerances :
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_maxtol(const TopoDS_Shape& S,const TopAbs_ShapeEnum& typ,Standard_Real& maxtol)
// purpose : returns maxtol of <S>'s shapes of type <typ> 
{
  Standard_Boolean face   = (typ == TopAbs_FACE);
  Standard_Boolean edge   = (typ == TopAbs_EDGE);
  Standard_Boolean vertex = (typ == TopAbs_VERTEX);
  TopExp_Explorer ex(S,typ);
  Standard_Boolean hasshatyp = ex.More();
  for (; ex.More(); ex.Next()){
    const TopoDS_Shape& ss = ex.Current();
    Standard_Real tolss = 0.;
    if (face)   tolss =  BRep_Tool::Tolerance(TopoDS::Face(ss));
    if (edge)   tolss =  BRep_Tool::Tolerance(TopoDS::Edge(ss));
    if (vertex) tolss =  BRep_Tool::Tolerance(TopoDS::Vertex(ss));
    if (tolss > maxtol) maxtol = tolss;
  }
  return hasshatyp;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Real FUN_tool_maxtol(const TopoDS_Shape& S)
// purpose : returns maxtol between <S>'s shapes.
{
  Standard_Real maxtol = 0.;
  FUN_tool_maxtol(S,TopAbs_FACE,maxtol);
  FUN_tool_maxtol(S,TopAbs_EDGE,maxtol);
  FUN_tool_maxtol(S,TopAbs_VERTEX,maxtol);
  return maxtol;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Integer FUN_tool_nbshapes(const TopoDS_Shape& S,const TopAbs_ShapeEnum& typ)
{
  TopExp_Explorer ex(S,typ);
  Standard_Integer i = 0;
  for (; ex.More(); ex.Next()) i++;
  return i; 
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_shapes(const TopoDS_Shape& S,const TopAbs_ShapeEnum& typ,
				     TopTools_ListOfShape& ltyp)
{
  TopExp_Explorer ex(S,typ);
  for (; ex.More(); ex.Next()) ltyp.Append(ex.Current());
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Integer FUN_tool_comparebndkole(const TopoDS_Shape& sh1,const TopoDS_Shape& sh2)  
// purpose: comparing bounding boxes of <sh1> and <sh2>,
//          returns k =1,2 if shi is contained in shk
//          else returns 0
{
  Bnd_Box bnd1; BRepBndLib::Add(sh1,bnd1); bnd1.SetGap(0.);
  Bnd_Box bnd2; BRepBndLib::Add(sh2,bnd2); bnd2.SetGap(0.);

  if(bnd1.IsOut(bnd2)) return 0;
  TColStd_Array1OfReal xyz1(1,6),xyz2(1,6);
  bnd1.Get(xyz1(1),xyz1(2),xyz1(3),xyz1(4),xyz1(5),xyz1(6)); 
  bnd2.Get(xyz2(1),xyz2(2),xyz2(3),xyz2(4),xyz2(5),xyz2(6)); 
  Standard_Real tol = Precision::Confusion();
  
  Standard_Integer neq,n2sup; neq=n2sup=0;
//  for (Standard_Integer i = 1; i<=3; i++) {
  Standard_Integer i ;
  for ( i = 1; i<=3; i++) {
    Standard_Real d = xyz2(i)-xyz1(i);
    Standard_Boolean eq = (Abs(d) < tol);
    if (eq) {neq++; continue;}
    if (d<0.) n2sup++;
  }
  for (i = 4; i<=6; i++) {
    Standard_Real d = xyz2(i)-xyz1(i);
    Standard_Boolean eq = (Abs(d) < tol);
    if (eq) {neq++; continue;}
    if (d>0.) n2sup++;
  }  
  if (n2sup + neq != 6) return 0;
  if (neq == 6) return 0;

  Standard_Integer ires = (n2sup > 0)? 2: 1;
  return ires;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_SameOri(const TopoDS_Edge& E1,const TopoDS_Edge& E2)
//prequesitory : 1- <E1> and <E2> share same domain,
//               2- C3d<E1> contains C3d<E2>
{
  Standard_Real f,l; FUN_tool_bounds(E2,f,l);  
  Standard_Real x = 0.345;
  Standard_Real mid = x*f + (1-x)*l;
  gp_Pnt Pmid; FUN_tool_value(mid,E2,Pmid);
  gp_Vec tmp; Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(mid,E2,tmp);
  if (!ok) return Standard_False;
  gp_Dir tgE2(tmp);
  TopAbs_Orientation oriE2 = E2.Orientation();
  if (M_REVERSED(oriE2)) tgE2.Reverse();

  Standard_Real pE1,dist; ok = FUN_tool_projPonE(Pmid,E1,pE1,dist);
  Standard_Real tol1 = BRep_Tool::Tolerance(E1); 
  Standard_Real tol2 = BRep_Tool::Tolerance(E2); 
  Standard_Real tol  = Max(tol1,tol2)* 10.;
  if (dist > tol) return Standard_False;
  if (!ok) return Standard_False;
  ok = TopOpeBRepTool_TOOL::TggeomE(pE1,E1,tmp);
  if (!ok) return Standard_False;
  gp_Dir tgE1(tmp);
  TopAbs_Orientation oriE1 = E1.Orientation();
  if (M_REVERSED(oriE1)) tgE1.Reverse();
  
  Standard_Boolean sameori = (tgE2.Dot(tgE1) > 0.);
  return sameori;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_haspc(const TopoDS_Edge& E,const TopoDS_Face& F)
{
  Standard_Real f,l,tol; Handle(Geom2d_Curve) C2d =  FC2D_CurveOnSurface(E,F,f,l,tol);
  Standard_Boolean null = C2d.IsNull();
  return !null;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_pcurveonF(const TopoDS_Face& F,TopoDS_Edge& E)
{
  Standard_Real f,l; Handle(Geom_Curve) C3d = BRep_Tool::Curve(E,f,l);
  if (C3d.IsNull()) return Standard_False;   
  Standard_Real tolReached2d;
  Handle(Geom2d_Curve) C2d =
    TopOpeBRepTool_CurveTool::MakePCurveOnFace(F,C3d,tolReached2d,f,l);
  if (C2d.IsNull()) return Standard_False;

  Standard_Real tolE = BRep_Tool::Tolerance(E);
  BRep_Builder BB;
  BB.UpdateEdge(E,C2d,F,tolE);
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_pcurveonF(const TopoDS_Face& fF,TopoDS_Edge& faultyE,
				       const Handle(Geom2d_Curve)& C2d,
				       TopoDS_Face& newf)
{
  BRep_Builder BB;
  TopExp_Explorer exw(fF,TopAbs_WIRE);
  TopTools_ListOfShape low;
  Standard_Boolean hasnewf = Standard_False;
  for (; exw.More(); exw.Next()){
    const TopoDS_Shape& w = exw.Current();
    
    TopTools_ListOfShape loe; Standard_Boolean hasneww = Standard_False;
    TopExp_Explorer exe(w,TopAbs_EDGE);
    for (; exe.More(); exe.Next()){
      const TopoDS_Edge& e = TopoDS::Edge(exe.Current());
      Standard_Boolean equal = e.IsEqual(faultyE);
      if (!equal) {loe.Append(e); continue;}
      
      Standard_Real tole = BRep_Tool::Tolerance(e);
      TopoDS_Vertex vf,vl; TopExp::Vertices(e,vf,vl);

      TopoDS_Edge newe = faultyE;
//      TopoDS_Edge newe; FUN_ds_CopyEdge(e,newe); newe.Orientation(TopAbs_FORWARD);
//      vf.Orientation(TopAbs_FORWARD);  BB.Add(newe,vf); FUN_ds_Parameter(newe,vf,parf); 
//      vl.Orientation(TopAbs_REVERSED); BB.Add(newe,vl); FUN_ds_Parameter(newe,vl,parl);
      BB.UpdateEdge(newe,C2d,fF,tole);
      newe.Orientation(e.Orientation());
      loe.Append(newe);  
      hasneww = Standard_True;
      hasnewf = Standard_True;
    }
    if (hasneww) {
      TopoDS_Wire neww; Standard_Boolean ok = FUN_tool_MakeWire(loe,neww);
      if (!ok) return Standard_False;
      low.Append(neww);
    }
    else low.Append(w);
  } // exw
  if (hasnewf) {
    TopoDS_Shape aLocalShape = fF.EmptyCopied();
    newf = TopoDS::Face(aLocalShape);
//    newf = TopoDS::Face(fF.EmptyCopied()); 
    for (TopTools_ListIteratorOfListOfShape itw(low); itw.More(); itw.Next()){
      const TopoDS_Shape w = itw.Value();
      BB.Add(newf,w);
    }
    return Standard_True;
  }
  return Standard_False;
}

// ----------------------------------------------------------------------
//  shared geometry :
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_curvesSO(const TopoDS_Edge& E1,const Standard_Real p1,const TopoDS_Edge& E2,const Standard_Real p2,
				      Standard_Boolean& so)
{
  BRepAdaptor_Curve BAC1(E1);
  BRepAdaptor_Curve BAC2(E2);
  gp_Vec tg1; Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(p1,E1,tg1);
  if (!ok) return Standard_False;//NYIRAISE
  gp_Vec tg2;     ok = TopOpeBRepTool_TOOL::TggeomE(p2,E2,tg2);
  if (!ok) return Standard_False;//NYIRAISE
  Standard_Real tola = Precision::Angular()*1.e3;
  Standard_Boolean oppo = tg1.IsOpposite(tg2,tola);
  Standard_Boolean samo = tg1.IsParallel(tg2,tola);
  if      (oppo) so = Standard_False;
  else if (samo) so = Standard_True;
  else return Standard_False;
  return Standard_True;
}
Standard_EXPORT Standard_Boolean FUN_tool_curvesSO(const TopoDS_Edge& E1,const Standard_Real p1,const TopoDS_Edge& E2,Standard_Boolean& so)
{
  // prequesitory : P3d(E1,p1) is IN 1d(E2)
  Standard_Real p2 = 0.; Standard_Boolean ok = FUN_tool_parE(E1,p1,E2,p2);
  if (!ok) return Standard_False;
  ok = FUN_tool_curvesSO(E1,p1,E2,p2,so);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_curvesSO(const TopoDS_Edge& E1,const TopoDS_Edge& E2,Standard_Boolean& so)
{
  // prequesitory : E1 is IN 1d(E2)
  TopoDS_Vertex vf1,vl1; TopExp::Vertices(E1,vf1,vl1);
  Standard_Boolean closed1 = vf1.IsSame(vl1);
  TopoDS_Vertex vf2,vl2; TopExp::Vertices(E2,vf2,vl2);
  Standard_Boolean closed2 = vf2.IsSame(vl2);
  Standard_Boolean project = Standard_False;
  if (closed1 || closed2) project = Standard_True;
  else {
    if      (vf1.IsSame(vf2)) so = Standard_True;
    else if (vl1.IsSame(vl2)) so = Standard_True;
    else if (vf1.IsSame(vl2)) so = Standard_False;
    else if (vl1.IsSame(vf2)) so = Standard_False;
    else project = Standard_True;      
  }
  if (project) {
    Standard_Real f,l; FUN_tool_bounds(E1,f,l);
    Standard_Real x = 0.45678; Standard_Real p1 = (1-x)*l + x*f;
    Standard_Boolean ok = FUN_tool_curvesSO(E1,p1,E2,so);
    return ok;
  }
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_findAncestor(const TopTools_ListOfShape& lF,const TopoDS_Edge& E,TopoDS_Face& Fanc)
{
  TopTools_ListIteratorOfListOfShape it(lF);
  for (; it.More(); it.Next()){
    const TopoDS_Face& F = TopoDS::Face(it.Value());
    TopAbs_Orientation dummy; Standard_Boolean found = FUN_tool_orientEinF(E,F,dummy);
    if (found) {Fanc = F; return Standard_True;}
  }
  return Standard_False;
} 

// ----------------------------------------------------------------------
//  new topologies : 
// ----------------------------------------------------------------------

// FUN_ds_* methods are methods of TopOpeBRepDS_BuildTool
// that cannot be called (cyclic dependencies)

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_CopyEdge(const TopoDS_Shape& Ein,TopoDS_Shape& Eou)
{
  Standard_Real f,l;
  TopoDS_Edge E1 = TopoDS::Edge(Ein); 
  BRep_Tool::Range(E1,f,l);
  Eou = Ein.EmptyCopied();
  TopoDS_Edge E2 = TopoDS::Edge(Eou); 
  BRep_Builder BB;
  BB.Range(E2,f,l);
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_Parameter(const TopoDS_Shape& E,const TopoDS_Shape& V,const Standard_Real P)
{
  BRep_Builder BB;
  const TopoDS_Edge&   e = TopoDS::Edge(E);
  const TopoDS_Vertex& v = TopoDS::Vertex(V);
  Standard_Real p = P;
  TopLoc_Location loc; Standard_Real f,l;
  Handle(Geom_Curve) C = BRep_Tool::Curve(e,loc,f,l);
  if ( !C.IsNull() && C->IsPeriodic()) {
    Standard_Real per = C->Period();

    TopAbs_Orientation oV=TopAbs_FORWARD;

    TopExp_Explorer exV(e,TopAbs_VERTEX);
    for (; exV.More(); exV.Next()) {
      const TopoDS_Vertex& vofe = TopoDS::Vertex(exV.Current());
      if ( vofe.IsSame(v) ) {
	oV = vofe.Orientation();
	break;
      }
    }
    if ( exV.More() ) {
      if ( oV == TopAbs_REVERSED ) {
	if ( p < f ) {
	  Standard_Real pp = ElCLib::InPeriod(p,f,f+per);
	  p = pp;
	}
      }
    }
  }
  BB.UpdateVertex(v,p,e,0); 
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_MakeWire(const TopTools_ListOfShape& loE,TopoDS_Wire& newW)
{
  newW.Nullify();
  BRep_Builder BB; 
  BB.MakeWire(newW);
  TopTools_ListIteratorOfListOfShape itloE(loE);
  for (; itloE.More(); itloE.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(itloE.Value());
    BB.Add(newW,E);
  }
  return Standard_True;
}

