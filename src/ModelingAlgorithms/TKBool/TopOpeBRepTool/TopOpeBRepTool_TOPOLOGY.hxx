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

#include <GeomAbs_CurveType.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <TopoDS_Wire.hxx>
// #include <BRepAdaptor_Curve2d.hxx>

Standard_EXPORT void FUN_tool_tolUV(const TopoDS_Face& F, double& tolu, double& tolv);
Standard_EXPORT bool FUN_tool_direct(const TopoDS_Face& F, bool& direct);
// Standard_EXPORT bool FUN_tool_IsUViso(const TopoDS_Shape& E,const TopoDS_Shape&
// F,bool& isoU,bool& isoV,gp_Dir2d& d2d,gp_Pnt2d& o2d);
Standard_EXPORT bool FUN_tool_bounds(const TopoDS_Shape& F,
                                     double&             u1,
                                     double&             u2,
                                     double&             v1,
                                     double&             v2);
Standard_EXPORT bool FUN_tool_geombounds(const TopoDS_Face& F,
                                         double&            u1,
                                         double&            u2,
                                         double&            v1,
                                         double&            v2);
Standard_EXPORT bool FUN_tool_isobounds(const TopoDS_Shape& F,
                                        double&             u1,
                                        double&             u2,
                                        double&             v1,
                                        double&             v2);
Standard_EXPORT bool FUN_tool_outbounds(const TopoDS_Shape& Sh,
                                        double&             u1,
                                        double&             u2,
                                        double&             v1,
                                        double&             v2,
                                        bool&               outbounds);

// ----------------------------------------------------------------------
//  project point <P> on geometries (curve <C>,surface <S>)
// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_PinC(const gp_Pnt&            P,
                                   const BRepAdaptor_Curve& BAC,
                                   const double             pmin,
                                   const double             pmax,
                                   const double             tol);
Standard_EXPORT bool FUN_tool_PinC(const gp_Pnt& P, const BRepAdaptor_Curve& BAC, const double tol);

// ----------------------------------------------------------------------
Standard_EXPORT bool         FUN_tool_value(const double par, const TopoDS_Edge& E, gp_Pnt& P);
Standard_EXPORT bool         FUN_tool_value(const gp_Pnt2d& UV, const TopoDS_Face& F, gp_Pnt& P);
Standard_EXPORT TopAbs_State FUN_tool_staPinE(const gp_Pnt&      P,
                                              const TopoDS_Edge& E,
                                              const double       tol);
Standard_EXPORT TopAbs_State FUN_tool_staPinE(const gp_Pnt& P, const TopoDS_Edge& E);

// ----------------------------------------------------------------------
//  subshape's orientation :
//  - orientVinE : vertex orientation in edge
//  - orientEinF : edge's orientation in face
//  - tool_orientEinFFORWARD : edge's orientation in face oriented FORWARD
//  - EboundF : true if vertex is oriented (FORWARD,REVERSED) in an edge
// ----------------------------------------------------------------------
Standard_EXPORT int  FUN_tool_orientVinE(const TopoDS_Vertex& v, const TopoDS_Edge& e);
Standard_EXPORT bool FUN_tool_orientEinF(const TopoDS_Edge&  E,
                                         const TopoDS_Face&  F,
                                         TopAbs_Orientation& oriEinF);
Standard_EXPORT bool FUN_tool_orientEinFFORWARD(const TopoDS_Edge&  E,
                                                const TopoDS_Face&  F,
                                                TopAbs_Orientation& oriEinF);
Standard_EXPORT bool FUN_tool_EboundF(const TopoDS_Edge& E, const TopoDS_Face& F);

// ----------------------------------------------------------------------
//  derivatives :
// ----------------------------------------------------------------------
Standard_EXPORT gp_Vec FUN_tool_nggeomF(const gp_Pnt2d& p2d, const TopoDS_Face& F);
Standard_EXPORT bool   FUN_tool_nggeomF(const double&      paronE,
                                        const TopoDS_Edge& E,
                                        const TopoDS_Face& F,
                                        gp_Vec&            nggeomF);
Standard_EXPORT bool   FUN_tool_nggeomF(const double&      paronE,
                                        const TopoDS_Edge& E,
                                        const TopoDS_Face& F,
                                        gp_Vec&            nggeomF,
                                        const double       tol);
Standard_EXPORT bool   FUN_tool_EtgF(const double&      paronE,
                                     const TopoDS_Edge& E,
                                     const gp_Pnt2d&    p2d,
                                     const TopoDS_Face& F,
                                     const double       tola);
Standard_EXPORT bool   FUN_tool_EtgOOE(const double&      paronE,
                                       const TopoDS_Edge& E,
                                       const double&      paronOOE,
                                       const TopoDS_Edge& OOE,
                                       const double       tola);

// ----------------------------------------------------------------------
// oriented vectors :
// ----------------------------------------------------------------------
Standard_EXPORT gp_Vec FUN_tool_getgeomxx(const TopoDS_Face& Fi,
                                          const TopoDS_Edge& Ei,
                                          const double       parOnEi,
                                          const gp_Dir&      ngFi);
Standard_EXPORT gp_Vec FUN_tool_getgeomxx(const TopoDS_Face& Fi,
                                          const TopoDS_Edge& Ei,
                                          const double       parOnEi);
Standard_EXPORT bool   FUN_nearestISO(const TopoDS_Face& F,
                                      const double       xpar,
                                      const bool         isoU,
                                      double&            xinf,
                                      double&            xsup);
Standard_EXPORT bool   FUN_tool_getxx(const TopoDS_Face& Fi,
                                      const TopoDS_Edge& Ei,
                                      const double       parOnEi,
                                      const gp_Dir&      ngFi,
                                      gp_Dir&            XX);
Standard_EXPORT bool   FUN_tool_getxx(const TopoDS_Face& Fi,
                                      const TopoDS_Edge& Ei,
                                      const double       parOnEi,
                                      gp_Dir&            XX);
Standard_EXPORT bool   FUN_tool_getdxx(const TopoDS_Face& F,
                                       const TopoDS_Edge& E,
                                       const double       parE,
                                       gp_Vec2d&          XX);
Standard_EXPORT bool   FUN_tool_EitangenttoFe(const gp_Dir&      ngFe,
                                              const TopoDS_Edge& Ei,
                                              const double       parOnEi);

// ----------------------------------------------------------------------
// curve type,surface type :
// ----------------------------------------------------------------------
Standard_EXPORT GeomAbs_CurveType FUN_tool_typ(const TopoDS_Edge& E);
Standard_EXPORT bool              FUN_tool_line(const TopoDS_Edge& E);
Standard_EXPORT bool              FUN_tool_plane(const TopoDS_Shape& F);
Standard_EXPORT bool              FUN_tool_cylinder(const TopoDS_Shape& F);
Standard_EXPORT bool              FUN_tool_closedS(const TopoDS_Shape& F,
                                                   bool&               uclosed,
                                                   double&             uperiod,
                                                   bool&               vclosed,
                                                   double&             vperiod);
Standard_EXPORT bool              FUN_tool_closedS(const TopoDS_Shape& F);
Standard_EXPORT bool FUN_tool_closedS(const TopoDS_Shape& F, bool& inU, double& xmin, double& xper);
Standard_EXPORT void FUN_tool_mkBnd2d(const TopoDS_Shape& W,
                                      const TopoDS_Shape& FF,
                                      Bnd_Box2d&          B2d);

// ----------------------------------------------------------------------
//  closing topologies :
// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_IsClosingE(const TopoDS_Edge&  E,
                                         const TopoDS_Shape& S,
                                         const TopoDS_Face&  F);
Standard_EXPORT bool FUN_tool_ClosingE(const TopoDS_Edge& E,
                                       const TopoDS_Wire& W,
                                       const TopoDS_Face& F);

// ----------------------------------------------------------------------
//  shared topologies :
// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_inS(const TopoDS_Shape& subshape, const TopoDS_Shape& shape);
Standard_EXPORT bool FUN_tool_Eshared(const TopoDS_Shape& v,
                                      const TopoDS_Shape& F1,
                                      const TopoDS_Shape& F2,
                                      TopoDS_Shape&       Eshared);

Standard_EXPORT bool   FUN_tool_parVonE(const TopoDS_Vertex& v, const TopoDS_Edge& E, double& par);
Standard_EXPORT bool   FUN_tool_parE(const TopoDS_Edge& E0,
                                     const double&      par0,
                                     const TopoDS_Edge& E,
                                     double&            par,
                                     const double       tol);
Standard_EXPORT bool   FUN_tool_parE(const TopoDS_Edge& E0,
                                     const double&      par0,
                                     const TopoDS_Edge& E,
                                     double&            par);
Standard_EXPORT bool   FUN_tool_paronEF(const TopoDS_Edge& E,
                                        const double&      par,
                                        const TopoDS_Face& F,
                                        gp_Pnt2d&          UV,
                                        const double       tol);
Standard_EXPORT bool   FUN_tool_paronEF(const TopoDS_Edge& E,
                                        const double&      par,
                                        const TopoDS_Face& F,
                                        gp_Pnt2d&          UV);
Standard_EXPORT bool   FUN_tool_parF(const TopoDS_Edge& E,
                                     const double&      par,
                                     const TopoDS_Face& F,
                                     gp_Pnt2d&          UV,
                                     const double       tol);
Standard_EXPORT bool   FUN_tool_parF(const TopoDS_Edge& E,
                                     const double&      par,
                                     const TopoDS_Face& F,
                                     gp_Pnt2d&          UV);
Standard_EXPORT gp_Dir FUN_tool_dirC(const double par, const BRepAdaptor_Curve& BAC);
Standard_EXPORT gp_Vec FUN_tool_tggeomE(const double paronE, const TopoDS_Edge& E);
Standard_EXPORT bool   FUN_tool_line(const BRepAdaptor_Curve& BAC);
Standard_EXPORT bool   FUN_tool_quad(const TopoDS_Edge& E);
Standard_EXPORT bool   FUN_tool_quad(const BRepAdaptor_Curve& BAC);
Standard_EXPORT bool   FUN_tool_quad(const TopoDS_Face& F);
Standard_EXPORT bool   FUN_tool_findPinBAC(const BRepAdaptor_Curve& BAC, gp_Pnt& P, double& par);
Standard_EXPORT bool   FUN_tool_findparinBAC(const BRepAdaptor_Curve& BAC, double& par);
Standard_EXPORT bool   FUN_tool_findparinE(const TopoDS_Shape& E, double& par);
Standard_EXPORT bool   FUN_tool_findPinE(const TopoDS_Shape& E, gp_Pnt& P, double& par);
Standard_EXPORT bool   FUN_tool_maxtol(const TopoDS_Shape&     S,
                                       const TopAbs_ShapeEnum& typ,
                                       double&                 tol);
Standard_EXPORT double FUN_tool_maxtol(const TopoDS_Shape& S);
Standard_EXPORT int    FUN_tool_nbshapes(const TopoDS_Shape& S, const TopAbs_ShapeEnum& typ);
Standard_EXPORT void   FUN_tool_shapes(const TopoDS_Shape&             S,
                                       const TopAbs_ShapeEnum&         typ,
                                       NCollection_List<TopoDS_Shape>& ltyp);
Standard_EXPORT int    FUN_tool_comparebndkole(const TopoDS_Shape& sh1, const TopoDS_Shape& sh2);
Standard_EXPORT bool   FUN_tool_SameOri(const TopoDS_Edge& E1, const TopoDS_Edge& E2);
Standard_EXPORT bool   FUN_tool_haspc(const TopoDS_Edge& E, const TopoDS_Face& F);
Standard_EXPORT bool   FUN_tool_pcurveonF(const TopoDS_Face& F, TopoDS_Edge& E);
Standard_EXPORT bool   FUN_tool_pcurveonF(const TopoDS_Face&               fF,
                                          TopoDS_Edge&                     faultyE,
                                          const occ::handle<Geom2d_Curve>& C2d,
                                          TopoDS_Face&                     newf);

// ----------------------------------------------------------------------
//  shared geometry :
// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_curvesSO(const TopoDS_Edge& E1,
                                       const double       p1,
                                       const TopoDS_Edge& E2,
                                       const double       p2,
                                       bool&              so);
Standard_EXPORT bool FUN_tool_curvesSO(const TopoDS_Edge& E1,
                                       const double       p1,
                                       const TopoDS_Edge& E2,
                                       bool&              so);
Standard_EXPORT bool FUN_tool_curvesSO(const TopoDS_Edge& E1, const TopoDS_Edge& E2, bool& so);
Standard_EXPORT bool FUN_tool_findAncestor(const NCollection_List<TopoDS_Shape>& lF,
                                           const TopoDS_Edge&                    E,
                                           TopoDS_Face&                          Fanc);

// ----------------------------------------------------------------------
//  new topologies :
// ----------------------------------------------------------------------
Standard_EXPORT void FUN_ds_CopyEdge(const TopoDS_Shape& Ein, TopoDS_Shape& Eou);
Standard_EXPORT void FUN_ds_Parameter(const TopoDS_Shape& E, const TopoDS_Shape& V, const double P);
Standard_EXPORT bool FUN_tool_MakeWire(const NCollection_List<TopoDS_Shape>& loE,
                                       TopoDS_Wire&                          newW);

#endif
