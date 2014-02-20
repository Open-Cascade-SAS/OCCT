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

#ifndef _TopOpeBRepTool_TOPOLOGY_HeaderFile
#define _TopOpeBRepTool_TOPOLOGY_HeaderFile

#include <TopOpeBRepTool_define.hxx>

#include <GeomAbs_CurveType.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec.hxx>
#include <Bnd_Box2d.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepAdaptor_Curve.hxx>
//#include <BRepAdaptor_Curve2d.hxx>

Standard_EXPORT void FUN_tool_tolUV(const TopoDS_Face& F,Standard_Real& tolu,Standard_Real& tolv);
Standard_EXPORT Standard_Boolean FUN_tool_direct(const TopoDS_Face& F,Standard_Boolean& direct);
//Standard_EXPORT Standard_Boolean FUN_tool_IsUViso(const TopoDS_Shape& E,const TopoDS_Shape& F,Standard_Boolean& isoU,Standard_Boolean& isoV,gp_Dir2d& d2d,gp_Pnt2d& o2d);
Standard_EXPORT Standard_Boolean FUN_tool_bounds(const TopoDS_Shape& F,Standard_Real& u1,Standard_Real& u2,Standard_Real& v1,Standard_Real& v2);
Standard_EXPORT Standard_Boolean FUN_tool_geombounds(const TopoDS_Face& F,
					Standard_Real& u1,Standard_Real& u2,Standard_Real& v1,Standard_Real& v2);
Standard_EXPORT Standard_Boolean FUN_tool_isobounds(const TopoDS_Shape& F,Standard_Real& u1,Standard_Real& u2,Standard_Real& v1,Standard_Real& v2);
Standard_EXPORT Standard_Boolean FUN_tool_outbounds(const TopoDS_Shape& Sh,Standard_Real& u1,Standard_Real& u2,Standard_Real& v1,Standard_Real& v2,Standard_Boolean& outbounds);

// ----------------------------------------------------------------------
//  project point <P> on geometries (curve <C>,surface <S>)
// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_PinC(const gp_Pnt& P,const BRepAdaptor_Curve& BAC,const Standard_Real pmin,const Standard_Real pmax,const Standard_Real tol);
Standard_EXPORT Standard_Boolean FUN_tool_PinC(const gp_Pnt& P,const BRepAdaptor_Curve& BAC,const Standard_Real tol);

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_value(const Standard_Real par,const TopoDS_Edge& E,gp_Pnt& P);
Standard_EXPORT Standard_Boolean FUN_tool_value(const gp_Pnt2d& UV,const TopoDS_Face& F,gp_Pnt& P);
Standard_EXPORT TopAbs_State FUN_tool_staPinE(const gp_Pnt& P,const TopoDS_Edge& E,const Standard_Real tol);
Standard_EXPORT TopAbs_State FUN_tool_staPinE(const gp_Pnt& P,const TopoDS_Edge& E);

// ----------------------------------------------------------------------
//  subshape's orientation :
//  - orientVinE : vertex orientation in edge 
//  - orientEinF : edge's orientation in face
//  - tool_orientEinFFORWARD : edge's orientation in face oriented FORWARD
//  - EboundF : true if vertex is oriented (FORWARD,REVERSED) in an edge
// ----------------------------------------------------------------------
Standard_EXPORT Standard_Integer FUN_tool_orientVinE(const TopoDS_Vertex& v,const TopoDS_Edge& e);
Standard_EXPORT Standard_Boolean FUN_tool_orientEinF(const TopoDS_Edge& E,const TopoDS_Face& F,TopAbs_Orientation& oriEinF);
Standard_EXPORT Standard_Boolean FUN_tool_orientEinFFORWARD(const TopoDS_Edge& E,const TopoDS_Face& F,TopAbs_Orientation& oriEinF);
Standard_EXPORT Standard_Boolean FUN_tool_EboundF(const TopoDS_Edge& E,const TopoDS_Face& F);

// ----------------------------------------------------------------------
//  derivatives :
// ----------------------------------------------------------------------
Standard_EXPORT gp_Vec FUN_tool_nggeomF(const gp_Pnt2d& p2d,const TopoDS_Face& F);
Standard_EXPORT Standard_Boolean FUN_tool_nggeomF(const Standard_Real& paronE,const TopoDS_Edge& E,const TopoDS_Face& F,gp_Vec& nggeomF);
Standard_EXPORT Standard_Boolean FUN_tool_nggeomF(const Standard_Real& paronE,const TopoDS_Edge& E,const TopoDS_Face& F,gp_Vec& nggeomF,const Standard_Real tol);
Standard_EXPORT Standard_Boolean FUN_tool_EtgF(const Standard_Real& paronE,const TopoDS_Edge& E,const gp_Pnt2d& p2d,const TopoDS_Face& F,const Standard_Real tola);
Standard_EXPORT Standard_Boolean FUN_tool_EtgOOE(const Standard_Real& paronE, const TopoDS_Edge& E,const Standard_Real& paronOOE,const TopoDS_Edge& OOE,const Standard_Real tola);

// ----------------------------------------------------------------------
// oriented vectors :
// ----------------------------------------------------------------------
Standard_EXPORT gp_Vec FUN_tool_getgeomxx(const TopoDS_Face& Fi,const TopoDS_Edge& Ei,const Standard_Real parOnEi,const gp_Dir& ngFi);
Standard_EXPORT gp_Vec FUN_tool_getgeomxx(const TopoDS_Face& Fi,const TopoDS_Edge& Ei,const Standard_Real parOnEi);
Standard_EXPORT Standard_Boolean FUN_nearestISO(const TopoDS_Face& F,const Standard_Real xpar,const Standard_Boolean isoU,Standard_Real& xinf,Standard_Real& xsup);
Standard_EXPORT Standard_Boolean FUN_tool_getxx(const TopoDS_Face& Fi,const TopoDS_Edge& Ei,const Standard_Real parOnEi,const gp_Dir& ngFi,gp_Dir& XX);
Standard_EXPORT Standard_Boolean FUN_tool_getxx(const TopoDS_Face& Fi,const TopoDS_Edge& Ei, const Standard_Real parOnEi,gp_Dir& XX);
Standard_EXPORT Standard_Boolean FUN_tool_getdxx(const TopoDS_Face& F,const TopoDS_Edge& E,const Standard_Real parE,gp_Vec2d& XX);
Standard_EXPORT Standard_Boolean FUN_tool_EitangenttoFe(const gp_Dir& ngFe,const TopoDS_Edge& Ei,const Standard_Real parOnEi);

// ----------------------------------------------------------------------
// curve type,surface type :
// ----------------------------------------------------------------------
Standard_EXPORT GeomAbs_CurveType FUN_tool_typ(const TopoDS_Edge& E);
Standard_EXPORT Standard_Boolean FUN_tool_line(const TopoDS_Edge& E);
Standard_EXPORT Standard_Boolean FUN_tool_plane(const TopoDS_Shape& F);
Standard_EXPORT Standard_Boolean FUN_tool_cylinder(const TopoDS_Shape& F);
Standard_EXPORT Standard_Boolean FUN_tool_closedS(const TopoDS_Shape& F,Standard_Boolean& uclosed,Standard_Real& uperiod,Standard_Boolean& vclosed,Standard_Real& vperiod);
Standard_EXPORT Standard_Boolean FUN_tool_closedS(const TopoDS_Shape& F);
Standard_EXPORT Standard_Boolean FUN_tool_closedS(const TopoDS_Shape& F,Standard_Boolean& inU, Standard_Real& xmin, Standard_Real& xper);
Standard_EXPORT void FUN_tool_mkBnd2d(const TopoDS_Shape& W,const TopoDS_Shape& FF,Bnd_Box2d& B2d);

// ----------------------------------------------------------------------
//  closing topologies :
// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_IsClosingE(const TopoDS_Edge& E,const TopoDS_Shape& S,const TopoDS_Face& F);
Standard_EXPORT Standard_Boolean FUN_tool_ClosingE(const TopoDS_Edge& E,const TopoDS_Wire& W,const TopoDS_Face& F);
Standard_EXPORT Standard_Boolean FUN_tool_ClosedE(const TopoDS_Edge& E,TopoDS_Shape& vclosing);

// ----------------------------------------------------------------------
//  shared topologies :
// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_inS(const TopoDS_Shape& subshape,const TopoDS_Shape& shape);
Standard_EXPORT Standard_Boolean FUN_tool_Eshared(const TopoDS_Shape& v,const TopoDS_Shape& F1,const TopoDS_Shape& F2,TopoDS_Shape& Eshared);

Standard_EXPORT Standard_Boolean FUN_tool_parVonE(const TopoDS_Vertex& v,const TopoDS_Edge& E,Standard_Real& par);
Standard_EXPORT Standard_Boolean FUN_tool_parE(const TopoDS_Edge& E0,const Standard_Real& par0,const TopoDS_Edge& E,Standard_Real& par,const Standard_Real tol);
Standard_EXPORT Standard_Boolean FUN_tool_parE(const TopoDS_Edge& E0,const Standard_Real& par0,const TopoDS_Edge& E,Standard_Real& par);
Standard_EXPORT Standard_Boolean FUN_tool_paronEF(const TopoDS_Edge& E,const Standard_Real& par,const TopoDS_Face& F,gp_Pnt2d& UV,const Standard_Real tol);
Standard_EXPORT Standard_Boolean FUN_tool_paronEF(const TopoDS_Edge& E,const Standard_Real& par,const TopoDS_Face& F,gp_Pnt2d& UV);
Standard_EXPORT Standard_Boolean FUN_tool_parF(const TopoDS_Edge& E,const Standard_Real& par,const TopoDS_Face& F,gp_Pnt2d& UV,const Standard_Real tol);
Standard_EXPORT Standard_Boolean FUN_tool_parF(const TopoDS_Edge& E,const Standard_Real& par,const TopoDS_Face& F,gp_Pnt2d& UV);
Standard_EXPORT gp_Dir FUN_tool_dirC(const Standard_Real par,const BRepAdaptor_Curve& BAC);
Standard_EXPORT gp_Vec FUN_tool_tggeomE(const Standard_Real paronE,const TopoDS_Edge& E);
Standard_EXPORT Standard_Boolean FUN_tool_line(const BRepAdaptor_Curve& BAC);
Standard_EXPORT Standard_Boolean FUN_tool_quad(const TopoDS_Edge& E);
Standard_EXPORT Standard_Boolean FUN_tool_quad(const BRepAdaptor_Curve& BAC);
Standard_EXPORT Standard_Boolean FUN_tool_quad(const TopoDS_Face& F);
Standard_EXPORT Standard_Boolean FUN_tool_findPinBAC(const BRepAdaptor_Curve& BAC,gp_Pnt& P,Standard_Real& par);
Standard_EXPORT Standard_Boolean FUN_tool_findparinBAC(const BRepAdaptor_Curve& BAC,Standard_Real& par);
Standard_EXPORT Standard_Boolean FUN_tool_findparinE(const TopoDS_Shape& E,Standard_Real& par);
Standard_EXPORT Standard_Boolean FUN_tool_findPinE(const TopoDS_Shape& E,gp_Pnt& P,Standard_Real& par);
Standard_EXPORT Standard_Boolean FUN_tool_maxtol(const TopoDS_Shape& S,const TopAbs_ShapeEnum& typ,Standard_Real& tol);
Standard_EXPORT Standard_Real FUN_tool_maxtol(const TopoDS_Shape& S);
Standard_EXPORT Standard_Integer FUN_tool_nbshapes(const TopoDS_Shape& S,const TopAbs_ShapeEnum& typ);
Standard_EXPORT void FUN_tool_shapes(const TopoDS_Shape& S,const TopAbs_ShapeEnum& typ,TopTools_ListOfShape& ltyp);
Standard_EXPORT Standard_Integer FUN_tool_comparebndkole(const TopoDS_Shape& sh1,const TopoDS_Shape& sh2);
Standard_EXPORT Standard_Boolean FUN_tool_SameOri(const TopoDS_Edge& E1,const TopoDS_Edge& E2);
Standard_EXPORT Standard_Boolean FUN_tool_haspc(const TopoDS_Edge& E,const TopoDS_Face& F);
Standard_EXPORT Standard_Boolean FUN_tool_pcurveonF(const TopoDS_Face& F,TopoDS_Edge& E);
Standard_EXPORT Standard_Boolean FUN_tool_pcurveonF(const TopoDS_Face& fF,TopoDS_Edge& faultyE,const Handle(Geom2d_Curve)& C2d,TopoDS_Face& newf);

// ----------------------------------------------------------------------
//  shared geometry :
// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_curvesSO(const TopoDS_Edge& E1,const Standard_Real p1,const TopoDS_Edge& E2,const Standard_Real p2,Standard_Boolean& so);
Standard_EXPORT Standard_Boolean FUN_tool_curvesSO(const TopoDS_Edge& E1,const Standard_Real p1,const TopoDS_Edge& E2,Standard_Boolean& so);
Standard_EXPORT Standard_Boolean FUN_tool_curvesSO(const TopoDS_Edge& E1,const TopoDS_Edge& E2,Standard_Boolean& so);
Standard_EXPORT Standard_Boolean FUN_tool_findAncestor(const TopTools_ListOfShape& lF,const TopoDS_Edge& E,TopoDS_Face& Fanc);

// ----------------------------------------------------------------------
//  new topologies : 
// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_CopyEdge(const TopoDS_Shape& Ein,TopoDS_Shape& Eou);
Standard_EXPORT void FUN_ds_Parameter(const TopoDS_Shape& E,const TopoDS_Shape& V,const Standard_Real P);
Standard_EXPORT Standard_Boolean FUN_tool_MakeWire(const TopTools_ListOfShape& loE,TopoDS_Wire& newW);


Standard_EXPORT Standard_Boolean FUN_tool_getEclo(const TopoDS_Face& F, const Standard_Boolean UISO, TopoDS_Edge& Eclo);
#endif
