// Created on: 1994-02-09
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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
#include <BRepLProp_CLProps.hxx>
#include <BRepTools.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

//=================================================================================================

double TopOpeBRepTool_ShapeTool::Tolerance(const TopoDS_Shape& S)
{
  if (S.IsNull())
    return 0.;
  double tol = 0;
  switch (S.ShapeType())
  {
    case TopAbs_FACE:
      tol = BRep_Tool::Tolerance(TopoDS::Face(S));
      break;
    case TopAbs_EDGE:
      tol = BRep_Tool::Tolerance(TopoDS::Edge(S));
      break;
    case TopAbs_VERTEX:
      tol = BRep_Tool::Tolerance(TopoDS::Vertex(S));
      break;
    default:
      throw Standard_ProgramError("TopOpeBRepTool_ShapeTool : Shape has no tolerance");
      break;
  }
  return tol;
}

//=================================================================================================

gp_Pnt TopOpeBRepTool_ShapeTool::Pnt(const TopoDS_Shape& S)
{
  if (S.ShapeType() != TopAbs_VERTEX)
  {
    throw Standard_ProgramError("TopOpeBRepTool_ShapeTool::Pnt");
  }
  return BRep_Tool::Pnt(TopoDS::Vertex(S));
}

#include <Geom_OffsetCurve.hxx>
#include <Geom_TrimmedCurve.hxx>

//=================================================================================================

occ::handle<Geom_Curve> TopOpeBRepTool_ShapeTool::BASISCURVE(const occ::handle<Geom_Curve>& C)
{
  occ::handle<Standard_Type> T = C->DynamicType();
  if (T == STANDARD_TYPE(Geom_OffsetCurve))
    return BASISCURVE(occ::down_cast<Geom_OffsetCurve>(C)->BasisCurve());
  else if (T == STANDARD_TYPE(Geom_TrimmedCurve))
    return BASISCURVE(occ::down_cast<Geom_TrimmedCurve>(C)->BasisCurve());
  else
    return C;
}

occ::handle<Geom_Curve> TopOpeBRepTool_ShapeTool::BASISCURVE(const TopoDS_Edge& E)
{
  double                  f, l;
  occ::handle<Geom_Curve> C = BRep_Tool::Curve(E, f, l);
  if (C.IsNull())
    return C;
  return BASISCURVE(C);
}

#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>

//=================================================================================================

occ::handle<Geom_Surface> TopOpeBRepTool_ShapeTool::BASISSURFACE(const occ::handle<Geom_Surface>& S)
{
  occ::handle<Standard_Type> T = S->DynamicType();
  if (T == STANDARD_TYPE(Geom_OffsetSurface))
    return BASISSURFACE(occ::down_cast<Geom_OffsetSurface>(S)->BasisSurface());
  else if (T == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    return BASISSURFACE(occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface());
  else
    return S;
}

occ::handle<Geom_Surface> TopOpeBRepTool_ShapeTool::BASISSURFACE(const TopoDS_Face& F)
{
  TopLoc_Location           L;
  occ::handle<Geom_Surface> S = BRep_Tool::Surface(F, L);
  return BASISSURFACE(S);
}

//=================================================================================================

void TopOpeBRepTool_ShapeTool::UVBOUNDS(const occ::handle<Geom_Surface>& S,
                                        bool&                            UPeriodic,
                                        bool&                            VPeriodic,
                                        double&                          Umin,
                                        double&                          Umax,
                                        double&                          Vmin,
                                        double&                          Vmax)
{
  const occ::handle<Geom_Surface> BS = BASISSURFACE(S);
  occ::handle<Standard_Type>      T  = BS->DynamicType();

  if (T == STANDARD_TYPE(Geom_SurfaceOfRevolution))
  {
    occ::handle<Geom_SurfaceOfRevolution> SR = occ::down_cast<Geom_SurfaceOfRevolution>(BS);
    occ::handle<Geom_Curve>               C  = BASISCURVE(SR->BasisCurve());
    if (C->IsPeriodic())
    {
      UPeriodic = false;
      VPeriodic = true;
      Vmin      = C->FirstParameter();
      Vmax      = C->LastParameter();
    }
  }
  else if (T == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
  {
    occ::handle<Geom_SurfaceOfLinearExtrusion> SE =
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(BS);
    occ::handle<Geom_Curve> C = BASISCURVE(SE->BasisCurve());
    if (C->IsPeriodic())
    {
      UPeriodic = true;
      Umin      = C->FirstParameter();
      Umax      = C->LastParameter();
      VPeriodic = false;
    }
  }
  else
  {
    UPeriodic = BS->IsUPeriodic();
    VPeriodic = BS->IsVPeriodic();
    BS->Bounds(Umin, Umax, Vmin, Vmax);
  }
}

void TopOpeBRepTool_ShapeTool::UVBOUNDS(const TopoDS_Face& F,
                                        bool&              UPeriodic,
                                        bool&              VPeriodic,
                                        double&            Umin,
                                        double&            Umax,
                                        double&            Vmin,
                                        double&            Vmax)
{
  TopLoc_Location           L;
  occ::handle<Geom_Surface> S = BRep_Tool::Surface(F, L);
  UVBOUNDS(S, UPeriodic, VPeriodic, Umin, Umax, Vmin, Vmax);
}

//=================================================================================================

void TopOpeBRepTool_ShapeTool::AdjustOnPeriodic(const TopoDS_Shape& F, double& u, double& v)
{
  TopoDS_Face                     FF = TopoDS::Face(F);
  TopLoc_Location                 Loc;
  const occ::handle<Geom_Surface> Surf = BRep_Tool::Surface(FF, Loc);

  //  double Ufirst,Ulast,Vfirst,Vlast;
  bool isUperio, isVperio;
  isUperio = Surf->IsUPeriodic();
  isVperio = Surf->IsVPeriodic();

  // exit if surface supporting F is not periodic on U or V
  if (!isUperio && !isVperio)
    return;

  double UFfirst, UFlast, VFfirst, VFlast;
  BRepTools::UVBounds(FF, UFfirst, UFlast, VFfirst, VFlast);

  double tol = Precision::PConfusion();

  if (isUperio)
  {
    double Uperiod = Surf->UPeriod();

    //    double ubid = UFfirst;

    //    ElCLib::AdjustPeriodic(UFfirst,UFfirst + Uperiod,tol,ubid,u);
    if (std::abs(u - UFfirst - Uperiod) > tol)
      u = ElCLib::InPeriod(u, UFfirst, UFfirst + Uperiod);
  }
  if (isVperio)
  {
    double Vperiod = Surf->VPeriod();

    //    double vbid = VFfirst;

    //    ElCLib::AdjustPeriodic(VFfirst,VFfirst + Vperiod,tol,vbid,v);
    if (std::abs(v - VFfirst - Vperiod) > tol)
      v = ElCLib::InPeriod(v, VFfirst, VFfirst + Vperiod);
  }
}

//=================================================================================================

bool TopOpeBRepTool_ShapeTool::Closed(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  const TopoDS_Edge& E          = TopoDS::Edge(S1);
  const TopoDS_Face& F          = TopoDS::Face(S2);
  bool               brepclosed = BRep_Tool::IsClosed(E, F);
  if (brepclosed)
  {
    int n = 0;
    for (TopExp_Explorer x(F, TopAbs_EDGE); x.More(); x.Next())
      if (x.Current().IsSame(E))
        n++;
    if (n < 2)
      return false;
    else
      return true;
  }
  return false;
}

#ifdef OCCT_DEBUG
extern bool TopOpeBRepTool_GettraceVC();
extern bool TopOpeBRepTool_GettraceNYI();
#endif

inline bool PARINBOUNDS(const double par, const double first, const double last, const double tol)
{
  bool b = (((first + tol) <= par) && (par <= (last - tol)));
  return b;
}

inline bool PARONBOUND(const double par, const double bound, const double tol)
{
  bool b = (((bound - tol) <= par) && (par <= (bound + tol)));
  return b;
}

double ADJUST(const double par, const double first, const double last, const double tol)
{
  double period = last - first, periopar = par;

  if (PARINBOUNDS(par, first, last, tol))
  {
    periopar = par + period;
  }
  else if (PARONBOUND(par, first, tol))
  {
    periopar = par + period;
  }
  else if (PARONBOUND(par, last, tol))
  {
    periopar = par - period;
  }
  return periopar;
}

//=================================================================================================

double TopOpeBRepTool_ShapeTool::PeriodizeParameter(const double        par,
                                                    const TopoDS_Shape& EE,
                                                    const TopoDS_Shape& FF)
{
  double periopar = par;
  if (!TopOpeBRepTool_ShapeTool::Closed(EE, FF))
    return periopar;

  TopoDS_Edge E = TopoDS::Edge(EE);
  TopoDS_Face F = TopoDS::Face(FF);

  TopLoc_Location                 Loc;
  const occ::handle<Geom_Surface> Surf     = BRep_Tool::Surface(F, Loc);
  bool                            isUperio = Surf->IsUPeriodic();
  bool                            isVperio = Surf->IsVPeriodic();
  if (!isUperio && !isVperio)
    return periopar;

  double Ufirst, Ulast, Vfirst, Vlast;
  Surf->Bounds(Ufirst, Ulast, Vfirst, Vlast);

  double                          first, last, tolpc;
  const occ::handle<Geom2d_Curve> PC = FC2D_CurveOnSurface(E, F, first, last, tolpc);
  if (PC.IsNull())
    throw Standard_ProgramError("ShapeTool::PeriodizeParameter : no 2d curve");

  occ::handle<Standard_Type> TheType = PC->DynamicType();
  if (TheType == STANDARD_TYPE(Geom2d_Line))
  {

    occ::handle<Geom2d_Line> HL(occ::down_cast<Geom2d_Line>(PC));
    const gp_Dir2d&          D = HL->Direction();

    double tol  = Precision::Angular();
    bool   isoU = false, isoV = false;
    if (D.IsParallel(gp_Dir2d(gp_Dir2d::D::Y), tol))
      isoU = true;
    else if (D.IsParallel(gp_Dir2d(gp_Dir2d::D::X), tol))
      isoV = true;
    if (isoU)
    {
      periopar = ADJUST(par, Ufirst, Ulast, tol);
    }
    else if (isoV)
    {
      periopar = ADJUST(par, Vfirst, Vlast, tol);
    }

#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceVC())
    {
      std::cout << "TopOpeBRepTool_ShapeTool PC on edge is ";
      if (isoU)
        std::cout << "isoU f,l " << Ufirst << " " << Ulast << std::endl;
      else if (isoV)
        std::cout << "isoV f,l " << Vfirst << " " << Vlast << std::endl;
      else
        std::cout << "not isoU, not isoV" << std::endl;
      std::cout << "par = " << par << " --> " << periopar << std::endl;
    }
#endif
  }
  // NYI : BSpline ...

  return periopar;
}

//=================================================================================================

bool TopOpeBRepTool_ShapeTool::ShapesSameOriented(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  bool so = true;

  bool sam = S1.IsSame(S2);
  if (sam)
  {
    const TopAbs_Orientation o1 = S1.Orientation();
    const TopAbs_Orientation o2 = S2.Orientation();
    if ((o1 == TopAbs_FORWARD || o1 == TopAbs_REVERSED)
        && (o2 == TopAbs_FORWARD || o2 == TopAbs_REVERSED))
    {
      so = (o1 == o2);
      return so;
    }
  }

  TopAbs_ShapeEnum t1 = S1.ShapeType(), t2 = S2.ShapeType();
  if ((t1 == TopAbs_SOLID) && (t2 == TopAbs_SOLID))
  {
    so = true;
  }
  else if ((t1 == TopAbs_FACE) && (t2 == TopAbs_FACE))
  {
    so = FacesSameOriented(S1, S2);
  }
  else if ((t1 == TopAbs_EDGE) && (t2 == TopAbs_EDGE))
  {
    so = EdgesSameOriented(S1, S2);
  }
  else if ((t1 == TopAbs_VERTEX) && (t2 == TopAbs_VERTEX))
  {
    TopAbs_Orientation o1 = S1.Orientation();
    TopAbs_Orientation o2 = S2.Orientation();
    if (o1 == TopAbs_EXTERNAL || o1 == TopAbs_INTERNAL || o2 == TopAbs_EXTERNAL
        || o2 == TopAbs_INTERNAL)
      so = true;
    else
      so = (o1 == o2);
  }

  return so;
}

//=================================================================================================

bool TopOpeBRepTool_ShapeTool::SurfacesSameOriented(const BRepAdaptor_Surface& S1,
                                                    const BRepAdaptor_Surface& Sref)
{
  const BRepAdaptor_Surface& S2  = Sref;
  GeomAbs_SurfaceType        ST1 = S1.GetType();
  GeomAbs_SurfaceType        ST2 = S2.GetType();

  bool so = true;

  if (ST1 == GeomAbs_Plane && ST2 == GeomAbs_Plane)
  {

    double u1 = S1.FirstUParameter();
    double v1 = S1.FirstVParameter();
    gp_Pnt p1;
    gp_Vec d1u, d1v;
    S1.D1(u1, v1, p1, d1u, d1v);
    gp_Vec n1 = d1u.Crossed(d1v);

    double u2 = S2.FirstUParameter();
    double v2 = S2.FirstVParameter();
    gp_Pnt p2;
    gp_Vec d2u, d2v;
    S2.D1(u2, v2, p2, d2u, d2v);
    gp_Vec n2 = d2u.Crossed(d2v);

    double d = n1.Dot(n2);
    so       = (d > 0.);
  }
  else if (ST1 == GeomAbs_Cylinder && ST2 == GeomAbs_Cylinder)
  {

    // On peut projeter n'importe quel point.
    // prenons donc l'origine
    double u1 = 0.;
    double v1 = 0.;
    gp_Pnt p1;
    gp_Vec d1u, d1v;
    S1.D1(u1, v1, p1, d1u, d1v);
    gp_Vec n1 = d1u.Crossed(d1v);

    occ::handle<Geom_Surface> HS2 = S2.Surface().Surface();
    HS2                           = occ::down_cast<Geom_Surface>(HS2->Transformed(S2.Trsf()));
    gp_Pnt2d p22d;
    double   dp2;
    bool     ok = FUN_tool_projPonS(p1, HS2, p22d, dp2);
    if (!ok)
      return so; // NYI : raise

    double u2 = p22d.X();
    double v2 = p22d.Y();
    gp_Pnt p2;
    gp_Vec d2u, d2v;
    S2.D1(u2, v2, p2, d2u, d2v);
    gp_Vec n2 = d2u.Crossed(d2v);

    double d = n1.Dot(n2);
    so       = (d > 0.);
  }
  else
  {
    // prendre u1,v1 et projeter sur 2 pour calcul des normales
    // au meme point 3d.
#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceNYI())
    {
      std::cout << "TopOpeBRepTool_ShapeTool::SurfacesSameOriented surfaces non traitees : NYI";
      std::cout << std::endl;
    }
#endif
  }

  return so;
}

//=================================================================================================

bool TopOpeBRepTool_ShapeTool::FacesSameOriented(const TopoDS_Shape& S1, const TopoDS_Shape& Sref)
{
  const TopoDS_Shape& S2 = Sref;
  const TopoDS_Face&  F1 = TopoDS::Face(S1);
  const TopoDS_Face&  F2 = TopoDS::Face(S2);
  TopAbs_Orientation  o1 = F1.Orientation();
  TopAbs_Orientation  o2 = F2.Orientation();
  if (o1 == TopAbs_EXTERNAL || o1 == TopAbs_INTERNAL || o2 == TopAbs_EXTERNAL
      || o2 == TopAbs_INTERNAL)
  {
    return true;
  }

  bool                computerestriction = false;
  BRepAdaptor_Surface BAS1(F1, computerestriction);
  BRepAdaptor_Surface BAS2(F2, computerestriction);
  bool                so = F1.IsSame(F2) || SurfacesSameOriented(BAS1, BAS2);
  bool                b  = so;
  if (o1 != o2)
    b = !so;
  return b;
}

//=================================================================================================

bool TopOpeBRepTool_ShapeTool::CurvesSameOriented(const BRepAdaptor_Curve& C1,
                                                  const BRepAdaptor_Curve& Cref)
{
  const BRepAdaptor_Curve& C2  = Cref;
  GeomAbs_CurveType        CT1 = C1.GetType();
  GeomAbs_CurveType        CT2 = C2.GetType();
  bool                     so  = true;

  if (CT1 == GeomAbs_Line && CT2 == GeomAbs_Line)
  {
    double p1 = C1.FirstParameter();
    gp_Dir t1, n1;
    double c1;
    EdgeData(C1, p1, t1, n1, c1);
    double p2 = C2.FirstParameter();
    gp_Dir t2, n2;
    double c2;
    EdgeData(C2, p2, t2, n2, c2);
    double d = t1.Dot(t2);
    so       = (d > 0.);
  }
  else
  {
    // prendre p1 et projeter sur 2 pour calcul des normales
    // au meme point 3d.
#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceNYI())
    {
      std::cout << "TopOpeBRepTool_ShapeTool::CurvesSameOriented non lineaires : NYI";
      std::cout << std::endl;
    }
#endif
  }

  return so;
}

//=================================================================================================

bool TopOpeBRepTool_ShapeTool::EdgesSameOriented(const TopoDS_Shape& S1, const TopoDS_Shape& Sref)
{
  const TopoDS_Shape& S2 = Sref;
  const TopoDS_Edge&  E1 = TopoDS::Edge(S1);
  const TopoDS_Edge&  E2 = TopoDS::Edge(S2);
  TopAbs_Orientation  o1 = E1.Orientation();
  TopAbs_Orientation  o2 = E2.Orientation();
  if (o1 == TopAbs_EXTERNAL || o1 == TopAbs_INTERNAL || o2 == TopAbs_EXTERNAL
      || o2 == TopAbs_INTERNAL)
  {
    return true;
  }
  BRepAdaptor_Curve BAC1(E1);
  BRepAdaptor_Curve BAC2(E2);
  bool              so = CurvesSameOriented(BAC1, BAC2);
  bool              b  = so;
  if (o1 != o2)
    b = !so;
  return b;
}

//=================================================================================================

double TopOpeBRepTool_ShapeTool::EdgeData(const BRepAdaptor_Curve& BAC,
                                          const double             P,
                                          gp_Dir&                  T,
                                          gp_Dir&                  N,
                                          double&                  C)

{
  double tol = Precision::Angular();

  BRepLProp_CLProps BL(BAC, P, 2, tol);
  BL.Tangent(T);
  C = BL.Curvature();

  // xpu150399 cto900R4
  const double     tol1 = Epsilon(0.);
  constexpr double tol2 = RealLast();
  double           tolm = std::max(tol, std::max(tol1, tol2));

  if (std::abs(C) > tolm)
    BL.Normal(N);
  return tol;
}

//=================================================================================================

double TopOpeBRepTool_ShapeTool::EdgeData(const TopoDS_Shape& E,
                                          const double        P,
                                          gp_Dir&             T,
                                          gp_Dir&             N,
                                          double&             C)
{
  BRepAdaptor_Curve BAC(TopoDS::Edge(E));
  double            d = EdgeData(BAC, P, T, N, C);
  return d;
}

//=================================================================================================

double TopOpeBRepTool_ShapeTool::Resolution3dU(const occ::handle<Geom_Surface>& SU,
                                               const double                     Tol2d)
{
  GeomAdaptor_Surface GAS(SU);
  double              r3dunit = 0.00001; // petite valeur (1.0 -> RangeError sur un tore)
  double              ru      = GAS.UResolution(r3dunit);
  double              r3du    = r3dunit * (Tol2d / ru);
  return r3du;
}

//=================================================================================================

double TopOpeBRepTool_ShapeTool::Resolution3dV(const occ::handle<Geom_Surface>& SU,
                                               const double                     Tol2d)
{
  GeomAdaptor_Surface GAS(SU);
  double              r3dunit = 0.00001; // petite valeur (1.0 -> RangeError sur un tore)
  double              rv      = GAS.VResolution(r3dunit);
  double              r3dv    = r3dunit * (Tol2d / rv);
  return r3dv;
}

//=================================================================================================

double TopOpeBRepTool_ShapeTool::Resolution3d(const occ::handle<Geom_Surface>& SU,
                                              const double                     Tol2d)
{
  double ru = Resolution3dU(SU, Tol2d);
  double rv = Resolution3dV(SU, Tol2d);
  double r  = std::max(ru, rv);
  return r;
}

//=================================================================================================

double TopOpeBRepTool_ShapeTool::Resolution3d(const TopoDS_Face& F, const double Tol2d)
{
  TopLoc_Location                  L;
  const occ::handle<Geom_Surface>& SU = BRep_Tool::Surface(F, L);
  double                           r  = Resolution3d(SU, Tol2d);
  return r;
}
