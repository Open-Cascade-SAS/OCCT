// Created on: 1997-11-24
// Created by: Xuan PHAM PHU
// Copyright (c) 1997-1999 Matra Datavision
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

#include <TopOpeBRepTool_PROJECT.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <Bnd_Box2d.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir2d.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>

#include <TopOpeBRepTool_GEOMETRY.hxx>

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_bounds(const TopoDS_Edge& E,Standard_Real& f,Standard_Real& l)
{
  BRepAdaptor_Curve BAC(E);
  f = BAC.FirstParameter();
  l = BAC.LastParameter();
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Integer FUN_tool_getindex(const Extrema_ExtPC& ponc)
{
  Standard_Real Dist2,Dist2Min = ponc.SquareDistance(1);
  Standard_Integer myIndex = 1,n = ponc.NbExt();
  
  for ( Standard_Integer i = 2; i <= n; i++) {
    Dist2 = ponc.SquareDistance(i);
    if ( Dist2 < Dist2Min) {
      Dist2Min = Dist2;
      myIndex = i;
    }
  }
  return myIndex;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Integer FUN_tool_getindex(const Extrema_ExtPC2d& ponc)
{
  Standard_Real Dist2,Dist2Min = ponc.SquareDistance(1);
  Standard_Integer myIndex = 1,n = ponc.NbExt();
  
  for ( Standard_Integer i = 2; i <= n; i++) {
    Dist2 = ponc.SquareDistance(i);
    if ( Dist2 < Dist2Min) {
      Dist2Min = Dist2;
      myIndex = i;
    }
  }
  return myIndex;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonC(const gp_Pnt& P,
				      const Standard_Real tole,const BRepAdaptor_Curve& BAC,
				      const Standard_Real pmin,const Standard_Real pmax,
				      Standard_Real& param,Standard_Real& dist)
{
  // <True> if projection succeeds,and sets <param> to parameter of <P> on <C>.
  Extrema_ExtPC ponc(P,BAC,pmin,pmax);
  Standard_Boolean ok = ponc.IsDone();
  Standard_Integer nbext = 0;
  if (ok) {nbext = ponc.NbExt(); ok = (nbext > 0);}
  if (!ok) {
    for (Standard_Integer i = 1; i <= 2; i++) {
      Standard_Real par = (i == 1) ? pmin : pmax;
      gp_Pnt pt = BAC.Value(par);
      Standard_Real d2 = pt.SquareDistance(P);
      Standard_Boolean onpt = (d2 < tole * tole);
      if (onpt) {
	param = par; 
	dist = sqrt (d2);
	return Standard_True;}
    }
    return Standard_False;
  }
  Standard_Integer i = FUN_tool_getindex(ponc);  
  param = ponc.Point(i).Parameter();
  dist = sqrt (ponc.SquareDistance(i));
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonC(const gp_Pnt& P,
				      const BRepAdaptor_Curve& BAC,
				      const Standard_Real pmin,const Standard_Real pmax,
				      Standard_Real& param,Standard_Real& dist)
{
  // <True> if projection succeeds,and sets <param> to parameter of <P> on <C>.
  Standard_Real tole = BAC.Tolerance();
  Standard_Boolean ok = FUN_tool_projPonC(P,tole,BAC,pmin,pmax,param,dist);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonC(const gp_Pnt& P,
				      const BRepAdaptor_Curve& BAC,
				      Standard_Real& param,Standard_Real& dist)
{
  // <True> if projection succeeds,and sets <param> to parameter of <P> on <C>.
  Standard_Real tole = BAC.Tolerance();
  Standard_Real pmin = BAC.FirstParameter();
  Standard_Real pmax = BAC.LastParameter();
  Standard_Boolean ok = FUN_tool_projPonC(P,tole,BAC,pmin,pmax,param,dist);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonC2D(const gp_Pnt& P,
					const Standard_Real tole,const BRepAdaptor_Curve2d& BAC2D,
					const Standard_Real pmin,const Standard_Real pmax,
					Standard_Real& param,Standard_Real& dist)
{
  // <True> if projection succeeds,and sets <param> to parameter of <P> on <C>.
  Standard_Boolean ok = Standard_False;

  gp_Pnt2d P2D;
  const TopoDS_Face& F = BAC2D.Face();
  ok = FUN_tool_projPonF(P,F,P2D,dist);
  if (!ok) return Standard_False;

  Extrema_ExtPC2d ponc2d(P2D,BAC2D,pmin,pmax);
  ok = ponc2d.IsDone();
  Standard_Integer nbext = ponc2d.NbExt();
  if (ok) ok = (nbext > 0);
  if (!ok) {
    for (Standard_Integer i = 1; i <= 2; i++) {
      Standard_Real par = (i == 1) ? pmin : pmax;
      gp_Pnt2d pt2d = BAC2D.Value(par);
      Standard_Real d2 = pt2d.SquareDistance(P2D);
      Standard_Boolean onpt = (d2 < tole * tole);
      if (onpt) {
	param = par; 
	dist = sqrt (d2);
	return Standard_True;}
    }
    return Standard_False;
  }

  Standard_Integer i = FUN_tool_getindex(ponc2d);  
  param = ponc2d.Point(i).Parameter();
  dist = sqrt (ponc2d.SquareDistance(i));
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonC2D(const gp_Pnt& P,
					const BRepAdaptor_Curve2d& BAC2D,
					const Standard_Real pmin,const Standard_Real pmax,
					Standard_Real& param,Standard_Real& dist)
{
  // <True> if projection succeeds,and sets <param> to parameter of <P> on <C>.
  Standard_Real tole = BRep_Tool::Tolerance(BAC2D.Edge());
  Standard_Boolean ok = FUN_tool_projPonC2D(P,tole,BAC2D,pmin,pmax,param,dist);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonC2D(const gp_Pnt& P,
					const BRepAdaptor_Curve2d& BAC2D,
					Standard_Real& param,Standard_Real& dist)
{
  // <True> if projection succeeds,and sets <param> to parameter of <P> on <C>.
  Standard_Real tole = BRep_Tool::Tolerance(BAC2D.Edge());
  Standard_Real pmin = BAC2D.FirstParameter();
  Standard_Real pmax = BAC2D.LastParameter();
  Standard_Boolean ok = FUN_tool_projPonC2D(P,tole,BAC2D,pmin,pmax,param,dist);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonS(const gp_Pnt& P,
                                                   const Handle(Geom_Surface)& S,
                                                   gp_Pnt2d& UV,Standard_Real& dist,
                                                   const Extrema_ExtFlag anExtFlag,
                                                   const Extrema_ExtAlgo anExtAlgo)
{ 
  Standard_Real UMin, UMax, VMin, VMax;
  GeomAPI_ProjectPointOnSurf PonS;
  //
  S->Bounds(UMin, UMax, VMin, VMax);
  PonS.Init(S, UMin, UMax, VMin, VMax, anExtAlgo);
  Extrema_ExtPS& anExtPS = const_cast<Extrema_ExtPS&>(PonS.Extrema());
  anExtPS.SetFlag(anExtFlag);
  //
  PonS.Perform(P);
  //
  if (!PonS.Extrema().IsDone()) return Standard_False;
  if (PonS.NbPoints() == 0) return Standard_False;
  dist = PonS.LowerDistance();
  Standard_Real U,V; PonS.LowerDistanceParameters(U,V); UV.SetCoord(U,V);
  return Standard_True;
}

// ----------------------------------------------------------------------
//  projecting point <P> on topologies (edge <E>,face <F>)
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonE(const gp_Pnt& P,const Standard_Real tole,const TopoDS_Edge& E,
				      Standard_Real& param,Standard_Real& dist)
{
  dist = 1.;
  BRepAdaptor_Curve BAC(E);
  Standard_Real first = BAC.FirstParameter();
  Standard_Real last = BAC.LastParameter();
  Standard_Boolean ok = FUN_tool_projPonC(P,tole,BAC,first,last,param,dist);
  if (!ok) return Standard_False;

  Standard_Real f,l; FUN_tool_bounds(E,f,l);
  Standard_Real tolp = Precision::Parametric(Precision::Confusion());
  Standard_Boolean onf = Abs(f-param) < tolp; if (onf) param = f;
  Standard_Boolean onl = Abs(l-param) < tolp; if (onl) param = l;
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonE(const gp_Pnt& P,const TopoDS_Edge& E,
				      Standard_Real& param,Standard_Real& dist)
{
  Standard_Real tole = BRep_Tool::Tolerance(E);
  Standard_Boolean ok = FUN_tool_projPonE(P,tole,E,param,dist);
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonboundedF(const gp_Pnt& P,const TopoDS_Face& F,
					     gp_Pnt2d& UV,Standard_Real& dist)
{
  dist = 1.;
  // ! projecting point on surf does not take into account the face's 
  // restriction
  BRepLib_MakeVertex mv(P); TopoDS_Vertex V = mv.Vertex();
  BRepExtrema_ExtPF PonF(V,F);
  if (!PonF.IsDone()) return Standard_False;
  Standard_Integer npt = PonF.NbExt();
  if (npt == 0) return Standard_False;

  // tri
  Standard_Real dmin2 = 1.e14; Standard_Integer imin = 0;
  for (Standard_Integer i=1; i <=npt; i++){
    Standard_Real d2 = PonF.SquareDistance(i);
    if (d2 < dmin2) {imin = i; dmin2 = d2;}
  }
  if (imin == 0) return Standard_False;
  
  Standard_Real u,v; PonF.Parameter(imin,u,v);
  dist = sqrt (dmin2);
  UV = gp_Pnt2d(u,v);
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonF(const gp_Pnt& P,const TopoDS_Face& F,
                                                   gp_Pnt2d& UV,Standard_Real& dist,
                                                   const Extrema_ExtFlag anExtFlag,
                                                   const Extrema_ExtAlgo anExtAlgo)
{
  dist = 1.;
  Handle(Geom_Surface) S = BRep_Tool::Surface(F);
  Standard_Boolean ok = FUN_tool_projPonS(P,S,UV,dist, anExtFlag, anExtAlgo);
  return ok;
}
