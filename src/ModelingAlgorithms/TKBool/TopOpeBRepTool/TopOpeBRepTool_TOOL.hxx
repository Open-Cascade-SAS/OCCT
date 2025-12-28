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

#ifndef _TopOpeBRepTool_TOOL_HeaderFile
#define _TopOpeBRepTool_TOOL_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Vertex;
class TopoDS_Face;
class gp_Pnt2d;
class TopOpeBRepTool_C2DF;
class gp_Vec;
class gp_Dir2d;
class BRepAdaptor_Curve;
class gp_Vec2d;
class gp_Dir;
class Geom2d_Curve;
class gp_Pnt;

class TopOpeBRepTool_TOOL
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static int OriinSor(
    const TopoDS_Shape&    sub,
    const TopoDS_Shape&    S,
    const bool checkclo = false);

  Standard_EXPORT static int OriinSorclosed(const TopoDS_Shape& sub,
                                                         const TopoDS_Shape& S);

  Standard_EXPORT static bool ClosedE(const TopoDS_Edge& E, TopoDS_Vertex& vclo);

  Standard_EXPORT static bool ClosedS(const TopoDS_Face& F);

  Standard_EXPORT static bool IsClosingE(const TopoDS_Edge& E, const TopoDS_Face& F);

  Standard_EXPORT static bool IsClosingE(const TopoDS_Edge&  E,
                                                     const TopoDS_Shape& W,
                                                     const TopoDS_Face&  F);

  Standard_EXPORT static void Vertices(const TopoDS_Edge& E, NCollection_Array1<TopoDS_Shape>& Vces);

  Standard_EXPORT static TopoDS_Vertex Vertex(const int Iv, const TopoDS_Edge& E);

  Standard_EXPORT static double ParE(const int Iv, const TopoDS_Edge& E);

  Standard_EXPORT static int OnBoundary(const double par, const TopoDS_Edge& E);

  Standard_EXPORT static gp_Pnt2d UVF(const double par, const TopOpeBRepTool_C2DF& C2DF);

  Standard_EXPORT static bool ParISO(const gp_Pnt2d&    p2d,
                                                 const TopoDS_Edge& e,
                                                 const TopoDS_Face& f,
                                                 double&     pare);

  Standard_EXPORT static bool ParE2d(const gp_Pnt2d&    p2d,
                                                 const TopoDS_Edge& e,
                                                 const TopoDS_Face& f,
                                                 double&     par,
                                                 double&     dist);

  Standard_EXPORT static bool Getduv(const TopoDS_Face&  f,
                                                 const gp_Pnt2d&     uv,
                                                 const gp_Vec&       dir,
                                                 const double factor,
                                                 gp_Dir2d&           duv);

  Standard_EXPORT static bool uvApp(const TopoDS_Face&  f,
                                                const TopoDS_Edge&  e,
                                                const double par,
                                                const double eps,
                                                gp_Pnt2d&           uvapp);

  Standard_EXPORT static double TolUV(const TopoDS_Face& F, const double tol3d);

  Standard_EXPORT static double TolP(const TopoDS_Edge& E, const TopoDS_Face& F);

  Standard_EXPORT static double minDUV(const TopoDS_Face& F);

  Standard_EXPORT static bool outUVbounds(const gp_Pnt2d& uv, const TopoDS_Face& F);

  Standard_EXPORT static void stuvF(const gp_Pnt2d&    uv,
                                    const TopoDS_Face& F,
                                    int&  onU,
                                    int&  onV);

  Standard_EXPORT static bool TggeomE(const double      par,
                                                  const BRepAdaptor_Curve& BC,
                                                  gp_Vec&                  Tg);

  Standard_EXPORT static bool TggeomE(const double par,
                                                  const TopoDS_Edge&  E,
                                                  gp_Vec&             Tg);

  Standard_EXPORT static bool TgINSIDE(const TopoDS_Vertex& v,
                                                   const TopoDS_Edge&   E,
                                                   gp_Vec&              Tg,
                                                   int&    OvinE);

  Standard_EXPORT static gp_Vec2d Tg2d(const int     iv,
                                       const TopoDS_Edge&         E,
                                       const TopOpeBRepTool_C2DF& C2DF);

  Standard_EXPORT static gp_Vec2d Tg2dApp(const int     iv,
                                          const TopoDS_Edge&         E,
                                          const TopOpeBRepTool_C2DF& C2DF,
                                          const double        factor);

  Standard_EXPORT static gp_Vec2d tryTg2dApp(const int     iv,
                                             const TopoDS_Edge&         E,
                                             const TopOpeBRepTool_C2DF& C2DF,
                                             const double        factor);

  Standard_EXPORT static bool XX(const gp_Pnt2d&     uv,
                                             const TopoDS_Face&  f,
                                             const double par,
                                             const TopoDS_Edge&  e,
                                             gp_Dir&             xx);

  Standard_EXPORT static bool Nt(const gp_Pnt2d&    uv,
                                             const TopoDS_Face& f,
                                             gp_Dir&            normt);

  Standard_EXPORT static bool NggeomF(const gp_Pnt2d&    uv,
                                                  const TopoDS_Face& F,
                                                  gp_Vec&            ng);

  Standard_EXPORT static bool NgApp(const double par,
                                                const TopoDS_Edge&  E,
                                                const TopoDS_Face&  F,
                                                const double tola,
                                                gp_Dir&             ngApp);

  Standard_EXPORT static bool tryNgApp(const double par,
                                                   const TopoDS_Edge&  E,
                                                   const TopoDS_Face&  F,
                                                   const double tola,
                                                   gp_Dir&             ng);

  Standard_EXPORT static int tryOriEinF(const double par,
                                                     const TopoDS_Edge&  E,
                                                     const TopoDS_Face&  F);

  Standard_EXPORT static bool IsQuad(const TopoDS_Edge& E);

  Standard_EXPORT static bool IsQuad(const TopoDS_Face& F);

  Standard_EXPORT static bool CurvE(const TopoDS_Edge&  E,
                                                const double par,
                                                const gp_Dir&       tg0,
                                                double&      Curv);

  Standard_EXPORT static bool CurvF(const TopoDS_Face& F,
                                                const gp_Pnt2d&    uv,
                                                const gp_Dir&      tg0,
                                                double&     Curv,
                                                bool&  direct);

  Standard_EXPORT static bool UVISO(const occ::handle<Geom2d_Curve>& PC,
                                                bool&           isou,
                                                bool&           isov,
                                                gp_Dir2d&                   d2d,
                                                gp_Pnt2d&                   o2d);

  Standard_EXPORT static bool UVISO(const TopOpeBRepTool_C2DF& C2DF,
                                                bool&          isou,
                                                bool&          isov,
                                                gp_Dir2d&                  d2d,
                                                gp_Pnt2d&                  o2d);

  Standard_EXPORT static bool UVISO(const TopoDS_Edge& E,
                                                const TopoDS_Face& F,
                                                bool&  isou,
                                                bool&  isov,
                                                gp_Dir2d&          d2d,
                                                gp_Pnt2d&          o2d);

  Standard_EXPORT static bool IsonCLO(const occ::handle<Geom2d_Curve>& PC,
                                                  const bool      onU,
                                                  const double         xfirst,
                                                  const double         xperiod,
                                                  const double         xtol);

  Standard_EXPORT static bool IsonCLO(const TopOpeBRepTool_C2DF& C2DF,
                                                  const bool     onU,
                                                  const double        xfirst,
                                                  const double        xperiod,
                                                  const double        xtol);

  Standard_EXPORT static void TrslUV(const gp_Vec2d& t2d, TopOpeBRepTool_C2DF& C2DF);

  Standard_EXPORT static bool TrslUVModifE(const gp_Vec2d&    t2d,
                                                       const TopoDS_Face& F,
                                                       TopoDS_Edge&       E);

  Standard_EXPORT static double Matter(const gp_Vec& d1,
                                              const gp_Vec& d2,
                                              const gp_Vec& ref);

  Standard_EXPORT static double Matter(const gp_Vec2d& d1, const gp_Vec2d& d2);

  Standard_EXPORT static bool Matter(const gp_Dir&       xx1,
                                                 const gp_Dir&       nt1,
                                                 const gp_Dir&       xx2,
                                                 const gp_Dir&       nt2,
                                                 const double tola,
                                                 double&      Ang);

  Standard_EXPORT static bool Matter(const TopoDS_Face&  f1,
                                                 const TopoDS_Face&  f2,
                                                 const TopoDS_Edge&  e,
                                                 const double pare,
                                                 const double tola,
                                                 double&      Ang);

  Standard_EXPORT static bool MatterKPtg(const TopoDS_Face& f1,
                                                     const TopoDS_Face& f2,
                                                     const TopoDS_Edge& e,
                                                     double&     Ang);

  Standard_EXPORT static bool Getstp3dF(const gp_Pnt&      p,
                                                    const TopoDS_Face& f,
                                                    gp_Pnt2d&          uv,
                                                    TopAbs_State&      st);

  Standard_EXPORT static bool SplitE(const TopoDS_Edge&    Eanc,
                                                 NCollection_List<TopoDS_Shape>& Splits);

  Standard_EXPORT static void MkShell(const NCollection_List<TopoDS_Shape>& lF, TopoDS_Shape& She);

  Standard_EXPORT static bool Remove(NCollection_List<TopoDS_Shape>& loS,
                                                 const TopoDS_Shape&   toremove);

  Standard_EXPORT static bool WireToFace(
    const TopoDS_Face&                        Fref,
    const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& mapWlow,
    NCollection_List<TopoDS_Shape>&                     lFs);

  Standard_EXPORT static bool EdgeONFace(const double par,
                                                     const TopoDS_Edge&  ed,
                                                     const gp_Pnt2d&     uv,
                                                     const TopoDS_Face&  fa,
                                                     bool&   isonfa);

};

#endif // _TopOpeBRepTool_TOOL_HeaderFile
