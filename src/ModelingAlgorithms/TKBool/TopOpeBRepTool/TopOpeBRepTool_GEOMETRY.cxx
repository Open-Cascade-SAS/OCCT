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

#include <TopOpeBRepTool_GEOMETRY.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Vec.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <Precision.hxx>

// ----------------------------------------------------------------------
Standard_EXPORT occ::handle<Geom2d_Curve> BASISCURVE2D(const occ::handle<Geom2d_Curve>& C)
{
  occ::handle<Standard_Type> T = C->DynamicType();
  if (T == STANDARD_TYPE(Geom2d_OffsetCurve))
    return ::BASISCURVE2D(occ::down_cast<Geom2d_OffsetCurve>(C)->BasisCurve());
  else if (T == STANDARD_TYPE(Geom2d_TrimmedCurve))
    return ::BASISCURVE2D(occ::down_cast<Geom2d_TrimmedCurve>(C)->BasisCurve());
  else
    return C;
}

/*// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_IsUViso(const occ::handle<Geom2d_Curve>& PC,
                     bool& isoU,bool& isoV,
                     gp_Dir2d& d2d,gp_Pnt2d& o2d)
{
  isoU = isoV = false;
  if (PC.IsNull()) return false;
  occ::handle<Geom2d_Curve> LLL = BASISCURVE2D(PC);
  occ::handle<Standard_Type> T2 = LLL->DynamicType();
  bool isline2d = (T2 == STANDARD_TYPE(Geom2d_Line));
  if (!isline2d) return false;

  occ::handle<Geom2d_Line> L = occ::down_cast<Geom2d_Line>(LLL);
  d2d = L->Direction();
  isoU = (std::abs(d2d.X()) < Precision::Parametric(Precision::Confusion()));
  isoV = (std::abs(d2d.Y()) < Precision::Parametric(Precision::Confusion()));
  bool isoUV = isoU || isoV;
  if (!isoUV) return false;

  o2d = L->Location();
  return true;
}*/

// ----------------------------------------------------------------------
Standard_EXPORT gp_Dir FUN_tool_dirC(const double par, const occ::handle<Geom_Curve>& C)
{
  gp_Pnt p;
  gp_Vec tgE;
  C->D1(par, p, tgE);
  gp_Dir dirC(tgE);
  return dirC;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_onapex(const gp_Pnt2d& p2d, const occ::handle<Geom_Surface>& S)
{
  bool    isapex = false;
  GeomAdaptor_Surface GS(S);
  double       tol   = Precision::Confusion();
  GeomAbs_SurfaceType ST    = GS.GetType();
  double       toluv = 1.e-8;
  if (ST == GeomAbs_Cone)
  {
    gp_Cone       co   = GS.Cone();
    gp_Pnt        apex = co.Apex();
    gp_Pnt        pnt  = GS.Value(p2d.X(), p2d.Y());
    double dist = pnt.Distance(apex);
    isapex             = (dist < tol);
  }
  if (ST == GeomAbs_Sphere)
  {
    double    pisur2       = M_PI * .5;
    double    v            = p2d.Y();
    bool vpisur2      = (std::abs(v - pisur2) < toluv);
    bool vmoinspisur2 = (std::abs(v + pisur2) < toluv);
    isapex                        = vpisur2 || vmoinspisur2;
  }
  return isapex;
}

// ----------------------------------------------------------------------
Standard_EXPORT gp_Dir FUN_tool_ngS(const gp_Pnt2d& p2d, const occ::handle<Geom_Surface>& S)
{
  // ###############################
  // nyi : all geometries are direct
  // ###############################
  gp_Pnt p;
  gp_Vec d1u, d1v;
  S->D1(p2d.X(), p2d.Y(), p, d1u, d1v);

  double    du    = d1u.Magnitude();
  double    dv    = d1v.Magnitude();
  double    tol   = Precision::Confusion();
  bool kpart = (du < tol) || (dv < tol);
  if (kpart)
  {
    GeomAdaptor_Surface GS(S);
    GeomAbs_SurfaceType ST    = GS.GetType();
    double       toluv = 1.e-8;
    if (ST == GeomAbs_Cone)
    {
      bool nullx = (std::abs(p2d.X()) < toluv);
      bool apex  = nullx && (std::abs(p2d.Y()) < toluv);
      if (apex)
      {
        gp_Dir axis = GS.Cone().Axis().Direction();
        gp_Vec ng(axis);
        ng.Reverse();
        return ng;
      }
      else if (du < tol)
      {
        double    vf   = GS.FirstVParameter();
        bool onvf = std::abs(p2d.Y() - vf) < toluv;

        double x = p2d.X();
        double y = p2d.Y();
        // NYIXPU : devrait plutot etre fait sur les faces & TopOpeBRepTool_TOOL::minDUV...
        if (onvf)
          y += 1.;
        else
          y -= 1.;
        S->D1(x, y, p, d1u, d1v);
        gp_Vec ng = d1u ^ d1v;
        return ng;
      }
    }
    if (ST == GeomAbs_Sphere)
    {
      //      double deuxpi = 2*M_PI;
      double pisur2 = M_PI * .5;
      double u = p2d.X(), v = p2d.Y();
      //      bool u0  =(std::abs(u) < toluv);
      //      bool u2pi=(std::abs(u-deuxpi) < toluv);
      //      bool apex = u0 || u2pi;
      bool vpisur2      = (std::abs(v - pisur2) < toluv);
      bool vmoinspisur2 = (std::abs(v + pisur2) < toluv);
      bool apex         = vpisur2 || vmoinspisur2;
      if (apex)
      {
        gp_Pnt center = GS.Sphere().Location();
        gp_Pnt value  = GS.Value(u, v);
        gp_Vec ng(center, value);
        //	ng.Reverse();
        return ng;
      }
    }
#ifdef OCCT_DEBUG
    std::cout << "FUN_tool_nggeomF NYI" << std::endl;
#endif
    return gp_Dir(gp_Dir::D::Z);
  }

  gp_Dir udir(d1u);
  gp_Dir vdir(d1v);
  gp_Dir ngS(udir ^ vdir);
  return ngS;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_line(const occ::handle<Geom_Curve>& C3d)
{
  occ::handle<Geom_Curve> C = TopOpeBRepTool_ShapeTool::BASISCURVE(C3d);
  GeomAdaptor_Curve  GC(C);
  bool   line = (GC.GetType() == GeomAbs_Line);
  return line;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_quadCT(const GeomAbs_CurveType& CT)
{
  bool isquad = false;
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
Standard_EXPORT bool FUN_tool_quad(const occ::handle<Geom_Curve>& C3d)
{
  occ::handle<Geom_Curve> C = TopOpeBRepTool_ShapeTool::BASISCURVE(C3d);
  if (C.IsNull())
    return false;
  GeomAdaptor_Curve GC(C);
  GeomAbs_CurveType CT   = GC.GetType();
  bool  quad = FUN_quadCT(CT);
  return quad;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_quad(const occ::handle<Geom2d_Curve>& pc)
{
  occ::handle<Geom2d_Curve> pcb = BASISCURVE2D(pc); // NYI TopOpeBRepTool_ShapeTool
  if (pcb.IsNull())
    return false;
  Geom2dAdaptor_Curve GC2d(pcb);
  GeomAbs_CurveType   typ    = GC2d.GetType();
  bool    isquad = false;
  if (typ == GeomAbs_Line)
    isquad = true;
  if (typ == GeomAbs_Circle)
    isquad = true;
  if (typ == GeomAbs_Ellipse)
    isquad = true;
  if (typ == GeomAbs_Hyperbola)
    isquad = true;
  if (typ == GeomAbs_Parabola)
    isquad = true;
  return isquad;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_line(const occ::handle<Geom2d_Curve>& pc)
{
  occ::handle<Geom2d_Curve> pcb = BASISCURVE2D(pc); // NYI TopOpeBRepTool_ShapeTool
  if (pcb.IsNull())
    return false;
  Geom2dAdaptor_Curve GC2d(pcb);
  GeomAbs_CurveType   typ = GC2d.GetType();

  if (typ == GeomAbs_Line)
    return true;

  return false;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_quad(const occ::handle<Geom_Surface>& S)
{
  if (S.IsNull())
    return false;
  GeomAdaptor_Surface GAS(S);
  GeomAbs_SurfaceType typ    = GAS.GetType();
  bool    isquad = false;
  if (typ == GeomAbs_Plane)
    isquad = true;
  if (typ == GeomAbs_Cylinder)
    isquad = true;
  if (typ == GeomAbs_Cone)
    isquad = true;
  if (typ == GeomAbs_Sphere)
    isquad = true;
  if (typ == GeomAbs_Torus)
    isquad = true;
  return isquad;
}

// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_closed(const occ::handle<Geom_Surface>& S,
                                                 bool&           uclosed,
                                                 double&              uperiod,
                                                 bool&           vclosed,
                                                 double&              vperiod)
{
  uperiod = vperiod = 0.;
  if (S.IsNull())
    return false;
  uclosed = S->IsUClosed();
  if (uclosed)
    uclosed = S->IsUPeriodic(); // xpu261098 (BUC60382)
  if (uclosed)
    uperiod = S->UPeriod();
  vclosed = S->IsVClosed();
  if (vclosed)
    vclosed = S->IsVPeriodic();
  if (vclosed)
    vperiod = S->VPeriod();
  bool closed = uclosed || vclosed;
  return closed;
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_UpdateBnd2d(Bnd_Box2d& B2d, const Bnd_Box2d& newB2d)
{
  //  B2d.SetVoid(); -> DOESN'T EMPTY THE  BOX
  B2d = newB2d;
}
