// Created on: 1998-03-23
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

#include <gp_Vec2d.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Geom_Surface.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef OCCT_DEBUG
void debc2dnull(void) {}
#endif

Standard_EXPORT occ::handle<Geom2d_Curve> MakePCurve(const ProjLib_ProjectedCurve& PC);

// ------------------------------------------------------------------------------------
Standard_EXPORT bool FC2D_HasC3D(const TopoDS_Edge& E)
{
  TopLoc_Location         loc;
  double                  f3d, l3d;
  occ::handle<Geom_Curve> C3D = BRep_Tool::Curve(E, loc, f3d, l3d);
  bool                    b   = (!C3D.IsNull());
  return b;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT bool FC2D_HasCurveOnSurface(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  occ::handle<Geom2d_Curve> C2D;
  return FC2D_HasOldCurveOnSurface(E, F, C2D);
}

// ------------------------------------------------------------------------------------
Standard_EXPORT bool FC2D_HasOldCurveOnSurface(const TopoDS_Edge&         E,
                                               const TopoDS_Face&         F,
                                               occ::handle<Geom2d_Curve>& C2D,
                                               double&                    f2d,
                                               double&                    l2d,
                                               double&                    tol)
{
  bool hasold = false;
  tol         = BRep_Tool::Tolerance(E);
  C2D         = BRep_Tool::CurveOnSurface(E, F, f2d, l2d);
  hasold      = (!C2D.IsNull());
  return hasold;
}

Standard_EXPORT bool FC2D_HasOldCurveOnSurface(const TopoDS_Edge&         E,
                                               const TopoDS_Face&         F,
                                               occ::handle<Geom2d_Curve>& C2D)
{
  double f2d, l2d, tol;
  bool   hasold = FC2D_HasOldCurveOnSurface(E, F, C2D, f2d, l2d, tol);
  return hasold;
}

// ------------------------------------------------------------------------------------
static occ::handle<Geom2d_Curve> FC2D_make2d(const TopoDS_Edge& E,
                                             const TopoDS_Face& F,
                                             double&            f2d,
                                             double&            l2d,
                                             double&            tol,
                                             const bool         trim3d = false);

static occ::handle<Geom2d_Curve> FC2D_make2d(const TopoDS_Edge& E,
                                             const TopoDS_Face& F,
                                             double&            f2d,
                                             double&            l2d,
                                             double&            tol,
                                             const bool         trim3d)
{
  occ::handle<Geom2d_Curve> C2D = BRep_Tool::CurveOnSurface(E, F, f2d, l2d);
  if (!C2D.IsNull())
  {
    return C2D;
  }

  // pas de 2D
  double                  f3d, l3d;
  TopLoc_Location         eloc;
  occ::handle<Geom_Curve> C1     = BRep_Tool::Curve(E, eloc, f3d, l3d);
  bool                    hasC3D = (!C1.IsNull());

  if (hasC3D)
  {
    bool                    elocid = eloc.IsIdentity();
    occ::handle<Geom_Curve> C2;
    if (elocid)
    {
      C2 = C1;
    }
    else
    {
      C2 = occ::down_cast<Geom_Curve>(C1->Transformed(eloc.Transformation()));
    }
    double f = 0., l = 0.;
    if (trim3d)
    {
      f = f3d;
      l = l3d;
    }
    C2D = TopOpeBRepTool_CurveTool::MakePCurveOnFace(F, C2, tol, f, l);
    f2d = f3d;
    l2d = l3d;
    return C2D;
  }

  return C2D;
} // make2d1

// ------------------------------------------------------------------------------------
// modified by NIZHNY-MZV  Mon Oct  4 10:37:36 1999
Standard_EXPORT occ::handle<Geom2d_Curve> FC2D_MakeCurveOnSurface(const TopoDS_Edge& E,
                                                                  const TopoDS_Face& F,
                                                                  double&            f,
                                                                  double&            l,
                                                                  double&            tol,
                                                                  const bool         trim3d)
{

  occ::handle<Geom2d_Curve> C2D = FC2D_make2d(E, F, f, l, tol, trim3d);
  return C2D;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT occ::handle<Geom2d_Curve> FC2D_CurveOnSurface(const TopoDS_Edge& E,
                                                              const TopoDS_Face& F,
                                                              double&            f,
                                                              double&            l,
                                                              double&            tol,
                                                              const bool         trim3d)
{
  occ::handle<Geom2d_Curve> C2D;
  bool                      hasold = FC2D_HasOldCurveOnSurface(E, F, C2D, f, l, tol);
  if (hasold)
  {
    return C2D;
  }
  C2D = FC2D_MakeCurveOnSurface(E, F, f, l, tol, trim3d);
  return C2D;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT occ::handle<Geom2d_Curve> FC2D_EditableCurveOnSurface(const TopoDS_Edge& E,
                                                                      const TopoDS_Face& F,
                                                                      double&            f,
                                                                      double&            l,
                                                                      double&            tol,
                                                                      const bool         trim3d)
{
  bool hasold = false;
  {
    occ::handle<Geom2d_Curve> C2D;
    hasold = FC2D_HasOldCurveOnSurface(E, F, C2D, f, l, tol);
    if (hasold)
    {
      occ::handle<Geom2d_Curve> copC2D = occ::down_cast<Geom2d_Curve>(C2D->Copy());
      return copC2D;
    }
  }
  occ::handle<Geom2d_Curve> makC2D = FC2D_MakeCurveOnSurface(E, F, f, l, tol, trim3d);
  return makC2D;
}

// ------------------------------------------------------------------------------------
static void FC2D_translate(const occ::handle<Geom2d_Curve>& C2D,
                           //                           const TopoDS_Edge& E,
                           const TopoDS_Edge&,
                           const TopoDS_Face& F,
                           const TopoDS_Edge& EF)
{
  TopLoc_Location                  sloc;
  const occ::handle<Geom_Surface>& S1      = BRep_Tool::Surface(F, sloc);
  bool                             isperio = S1->IsUPeriodic() || S1->IsVPeriodic();
  gp_Dir2d                         d2d;
  gp_Pnt2d                         O2d;
  bool                             isuiso, isviso;
  bool uviso  = TopOpeBRepTool_TOOL::UVISO(C2D, isuiso, isviso, d2d, O2d);
  bool EFnull = EF.IsNull();

  if (isperio && uviso && !EFnull)
  {
    // C2D prend comme origine dans F l'origine de la pcurve de EF dans F
    TopoDS_Face FFOR = F;
    FFOR.Orientation(TopAbs_FORWARD);
    gp_Pnt2d p1, p2;
    BRep_Tool::UVPoints(EF, FFOR, p1, p2);
    double pEF    = isuiso ? p1.X() : p1.Y();
    double pC2D   = isuiso ? O2d.X() : O2d.Y();
    double factor = pEF - pC2D;
    bool   b      = (std::abs(factor) > 1.e-6);
    if (b)
    {
      gp_Vec2d transl(1., 0.);
      if (isviso)
      {
        transl = gp_Vec2d(0., 1.);
      }
      transl.Multiply(factor);
      C2D->Translate(transl);
    }
  }
}

// ------------------------------------------------------------------------------------
static occ::handle<Geom2d_Curve> FC2D_make2d(const TopoDS_Edge& E,
                                             const TopoDS_Face& F,
                                             const TopoDS_Edge& EF,
                                             double&            f2d,
                                             double&            l2d,
                                             double&            tol,
                                             const bool         trim3d = false);

static occ::handle<Geom2d_Curve> FC2D_make2d(const TopoDS_Edge& E,
                                             const TopoDS_Face& F,
                                             const TopoDS_Edge& EF,
                                             double&            f2d,
                                             double&            l2d,
                                             double&            tol,
                                             const bool         trim3d)
{
  occ::handle<Geom2d_Curve> C2D = BRep_Tool::CurveOnSurface(E, F, f2d, l2d);
  if (!C2D.IsNull())
  {
    return C2D;
  }

  // pas de 2D
  double                  f3d, l3d;
  TopLoc_Location         eloc;
  occ::handle<Geom_Curve> C1     = BRep_Tool::Curve(E, eloc, f3d, l3d);
  bool                    hasC3D = (!C1.IsNull());

  if (hasC3D)
  {
    bool                    elocid = eloc.IsIdentity();
    occ::handle<Geom_Curve> C2;
    if (elocid)
    {
      C2 = C1;
    }
    else
    {
      C2 = occ::down_cast<Geom_Curve>(C1->Transformed(eloc.Transformation()));
    }
    double f = 0., l = 0.;
    if (trim3d)
    {
      f = f3d;
      l = l3d;
    }
    C2D = TopOpeBRepTool_CurveTool::MakePCurveOnFace(F, C2, tol, f, l);
    f2d = f3d;
    l2d = l3d;
    FC2D_translate(C2D, E, F, EF);
    return C2D;
  }

  return C2D;
} // make2d2

// ------------------------------------------------------------------------------------
Standard_EXPORT occ::handle<Geom2d_Curve> FC2D_CurveOnSurface(const TopoDS_Edge& E,
                                                              const TopoDS_Face& F,
                                                              const TopoDS_Edge& EF,
                                                              double&            f2d,
                                                              double&            l2d,
                                                              double&            tol,
                                                              const bool         trim3d)
{
  occ::handle<Geom2d_Curve> C2D;

  bool hasold = FC2D_HasOldCurveOnSurface(E, F, C2D, f2d, l2d, tol);
  if (hasold)
  {
    return C2D;
  }

  C2D = FC2D_make2d(E, F, EF, f2d, l2d, tol, trim3d);
  return C2D;
}
