// Created on: 1997-07-29
// Created by: Jerome LEMONIER
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

#include <Adaptor2d_Curve2d.hxx>
#include <BRepBlend_SurfCurvEvolRadInv.hxx>
#include <Law_Function.hxx>
#include <math_Matrix.hxx>

//=================================================================================================

BRepBlend_SurfCurvEvolRadInv::BRepBlend_SurfCurvEvolRadInv(const Handle(Adaptor3d_Surface)& S,
                                                           const Handle(Adaptor3d_Curve)&   C,
                                                           const Handle(Adaptor3d_Curve)&   Cg,
                                                           const Handle(Law_Function)&      Evol)
    : surf(S),
      curv(C),
      guide(Cg)
{
  tevol = Evol;
}

//=================================================================================================

void BRepBlend_SurfCurvEvolRadInv::Set(const Standard_Integer Choix)
{
  choix = Choix;
  switch (choix)
  {
    case 1:
    case 2:
      sg1 = -1;
      break;
    case 3:
    case 4:
      sg1 = 1;
      break;
    default:
      sg1 = -1;
      break;
  }
}

//=================================================================================================

Standard_Integer BRepBlend_SurfCurvEvolRadInv::NbEquations() const
{
  return 3;
}

//=================================================================================================

Standard_Boolean BRepBlend_SurfCurvEvolRadInv::Value(const math_Vector& X, math_Vector& F)
{
  gp_Pnt ptgui;
  gp_Vec d1gui(0., 0., 0.);
  guide->D1(X(1), ptgui, d1gui);
  ray          = sg1 * tevol->Value(X(1));
  gp_Vec nplan = d1gui.Normalized();
  //  Standard_Real theD = -(nplan.XYZ().Dot(ptgui.XYZ()));
  gp_XYZ        nplanXYZ(nplan.XYZ());
  gp_XYZ        ptguiXYZ(ptgui.XYZ());
  Standard_Real theD = nplanXYZ.Dot(ptguiXYZ);
  theD               = theD * (-1.);

  gp_Pnt ptcur    = curv->Value(X(2));
  F(1)            = nplan.XYZ().Dot(ptcur.XYZ()) + theD;
  gp_Pnt2d p2drst = rst->Value(X(3));
  gp_Pnt   pts;
  gp_Vec   du, dv;
  surf->D1(p2drst.X(), p2drst.Y(), pts, du, dv);
  F(2)                    = nplan.XYZ().Dot(pts.XYZ()) + theD;
  gp_Vec        ns        = du.Crossed(dv);
  Standard_Real norm      = nplan.Crossed(ns).Magnitude();
  Standard_Real unsurnorm = 1. / norm;
  ns.SetLinearForm(nplan.Dot(ns), nplan, -1., ns);
  ns.Multiply(unsurnorm);
  gp_Vec ref(ptcur, pts);

  ref.SetLinearForm(ray, ns, ref);
  F(3) = ref.SquareMagnitude() - ray * ray;
  return Standard_True;
}

//=================================================================================================

Standard_Boolean BRepBlend_SurfCurvEvolRadInv::Derivatives(const math_Vector& X, math_Matrix& D)
{
  gp_Pnt ptgui;
  gp_Vec d1gui, d2gui;
  guide->D2(X(1), ptgui, d1gui, d2gui);
  Standard_Real normd1gui      = d1gui.Magnitude(), dray;
  Standard_Real unsurnormd1gui = 1. / normd1gui;
  tevol->D1(X(1), ray, dray);
  ray          = sg1 * ray;
  dray         = sg1 * dray;
  gp_Vec nplan = d1gui.Multiplied(unsurnormd1gui);
  gp_Vec dnplan;
  dnplan.SetLinearForm(-nplan.Dot(d2gui), nplan, d2gui);
  dnplan.Multiply(unsurnormd1gui);
  Standard_Real dtheD = -nplan.XYZ().Dot(d1gui.XYZ()) - dnplan.XYZ().Dot(ptgui.XYZ());
  gp_Pnt        ptcur;
  gp_Vec        d1cur;
  curv->D1(X(2), ptcur, d1cur);
  D(1, 1) = dnplan.XYZ().Dot(ptcur.XYZ()) + dtheD;
  D(1, 2) = nplan.XYZ().Dot(d1cur.XYZ());
  D(1, 3) = 0.;

  gp_Pnt2d p2drst;
  gp_Vec2d d1rst;
  rst->D1(X(3), p2drst, d1rst);
  gp_Pnt pts;
  gp_Vec d1u, d1v, d2u, d2v, duv;
  surf->D2(p2drst.X(), p2drst.Y(), pts, d1u, d1v, d2u, d2v, duv);
  D(2, 1) = dnplan.XYZ().Dot(pts.XYZ()) + dtheD;
  D(2, 2) = 0.;
  gp_Vec dwrstpts;
  dwrstpts.SetLinearForm(d1rst.X(), d1u, d1rst.Y(), d1v);
  D(2, 3) = nplan.XYZ().Dot(dwrstpts.XYZ());

  gp_Vec nsurf   = d1u.Crossed(d1v);
  gp_Vec dunsurf = d2u.Crossed(d1v).Added(d1u.Crossed(duv));
  gp_Vec dvnsurf = d1u.Crossed(d2v).Added(duv.Crossed(d1v));
  gp_Vec dwrstnsurf;
  dwrstnsurf.SetLinearForm(d1rst.X(), dunsurf, d1rst.Y(), dvnsurf);

  gp_Vec nplancrosnsurf      = nplan.Crossed(nsurf);
  gp_Vec dwguinplancrosnsurf = dnplan.Crossed(nsurf);
  gp_Vec dwrstnplancrosnsurf = nplan.Crossed(dwrstnsurf);

  Standard_Real norm2       = nplancrosnsurf.SquareMagnitude();
  Standard_Real norm        = sqrt(norm2);
  Standard_Real unsurnorm   = 1. / norm;
  Standard_Real raysurnorm  = ray * unsurnorm;
  Standard_Real unsurnorm2  = unsurnorm * unsurnorm;
  Standard_Real raysurnorm2 = ray * unsurnorm2;
  Standard_Real dwguinorm   = unsurnorm * nplancrosnsurf.Dot(dwguinplancrosnsurf);
  Standard_Real dwrstnorm   = unsurnorm * nplancrosnsurf.Dot(dwrstnplancrosnsurf);

  Standard_Real nplandotnsurf      = nplan.Dot(nsurf);
  Standard_Real dwguinplandotnsurf = dnplan.Dot(nsurf);
  Standard_Real dwrstnplandotnsurf = nplan.Dot(dwrstnsurf);

  gp_Vec temp, dwguitemp, dwrsttemp;
  temp.SetLinearForm(nplandotnsurf, nplan, -1., nsurf);
  dwguitemp.SetLinearForm(nplandotnsurf, dnplan, dwguinplandotnsurf, nplan);
  dwrsttemp.SetLinearForm(dwrstnplandotnsurf, nplan, -1., dwrstnsurf);

  gp_Vec corde(ptcur, pts);
  gp_Vec ref, dwguiref, dwrstref;
  ref.SetLinearForm(raysurnorm, temp, corde);
  dwguiref.SetLinearForm(raysurnorm, dwguitemp, -raysurnorm2 * dwguinorm, temp);
  dwguiref.SetLinearForm(1., dwguiref, dray * unsurnorm, temp);
  dwrstref.SetLinearForm(raysurnorm, dwrsttemp, -raysurnorm2 * dwrstnorm, temp, dwrstpts);

  ref.Add(ref);
  D(3, 1) = ref.Dot(dwguiref) - 2. * dray * ray;
  D(3, 2) = -ref.Dot(d1cur);
  D(3, 3) = ref.Dot(dwrstref);

  return Standard_True;
}

//=================================================================================================

Standard_Boolean BRepBlend_SurfCurvEvolRadInv::Values(const math_Vector& X,
                                                      math_Vector&       F,
                                                      math_Matrix&       D)
{
  gp_Pnt ptgui;
  gp_Vec d1gui(0., 0., 0.), d2gui(0., 0., 0.);
  guide->D2(X(1), ptgui, d1gui, d2gui);
  Standard_Real dray;
  tevol->D1(X(1), ray, dray);
  ray                          = sg1 * ray;
  dray                         = sg1 * dray;
  Standard_Real normd1gui      = d1gui.Magnitude();
  Standard_Real unsurnormd1gui = 1. / normd1gui;
  gp_Vec        nplan          = d1gui.Multiplied(unsurnormd1gui);
  //  Standard_Real theD = -(nplan.XYZ().Dot(ptgui.XYZ()));
  gp_XYZ        nplanXYZ(nplan.XYZ());
  gp_XYZ        ptcurXYZ(ptgui.XYZ());
  Standard_Real theD = nplanXYZ.Dot(ptcurXYZ);
  theD               = theD * (-1.);

  gp_Vec dnplan;
  dnplan.SetLinearForm(-nplan.Dot(d2gui), nplan, d2gui);
  dnplan.Multiply(unsurnormd1gui);
  Standard_Real dtheD = -nplan.XYZ().Dot(d1gui.XYZ()) - dnplan.XYZ().Dot(ptgui.XYZ());
  gp_Pnt        ptcur;
  gp_Vec        d1cur;
  curv->D1(X(2), ptcur, d1cur);
  F(1)    = nplan.XYZ().Dot(ptcur.XYZ()) + theD;
  D(1, 1) = dnplan.XYZ().Dot(ptcur.XYZ()) + dtheD;
  D(1, 2) = nplan.XYZ().Dot(d1cur.XYZ());
  D(1, 3) = 0.;

  gp_Pnt2d p2drst;
  gp_Vec2d d1rst;
  rst->D1(X(3), p2drst, d1rst);
  gp_Pnt pts;
  gp_Vec d1u, d1v, d2u, d2v, duv;
  surf->D2(p2drst.X(), p2drst.Y(), pts, d1u, d1v, d2u, d2v, duv);
  F(2)    = nplan.XYZ().Dot(pts.XYZ()) + theD;
  D(2, 1) = dnplan.XYZ().Dot(pts.XYZ()) + dtheD;
  D(2, 2) = 0.;
  gp_Vec dwrstpts;
  dwrstpts.SetLinearForm(d1rst.X(), d1u, d1rst.Y(), d1v);
  D(2, 3) = nplan.XYZ().Dot(dwrstpts.XYZ());

  gp_Vec nsurf   = d1u.Crossed(d1v);
  gp_Vec dunsurf = d2u.Crossed(d1v).Added(d1u.Crossed(duv));
  gp_Vec dvnsurf = d1u.Crossed(d2v).Added(duv.Crossed(d1v));
  gp_Vec dwrstnsurf;
  dwrstnsurf.SetLinearForm(d1rst.X(), dunsurf, d1rst.Y(), dvnsurf);

  gp_Vec nplancrosnsurf      = nplan.Crossed(nsurf);
  gp_Vec dwguinplancrosnsurf = dnplan.Crossed(nsurf);
  gp_Vec dwrstnplancrosnsurf = nplan.Crossed(dwrstnsurf);

  Standard_Real norm2       = nplancrosnsurf.SquareMagnitude();
  Standard_Real norm        = sqrt(norm2);
  Standard_Real unsurnorm   = 1. / norm;
  Standard_Real raysurnorm  = ray * unsurnorm;
  Standard_Real unsurnorm2  = unsurnorm * unsurnorm;
  Standard_Real raysurnorm2 = ray * unsurnorm2;
  Standard_Real dwguinorm   = unsurnorm * nplancrosnsurf.Dot(dwguinplancrosnsurf);
  Standard_Real dwrstnorm   = unsurnorm * nplancrosnsurf.Dot(dwrstnplancrosnsurf);

  Standard_Real nplandotnsurf      = nplan.Dot(nsurf);
  Standard_Real dwguinplandotnsurf = dnplan.Dot(nsurf);
  Standard_Real dwrstnplandotnsurf = nplan.Dot(dwrstnsurf);

  gp_Vec temp, dwguitemp, dwrsttemp;
  temp.SetLinearForm(nplandotnsurf, nplan, -1., nsurf);
  dwguitemp.SetLinearForm(nplandotnsurf, dnplan, dwguinplandotnsurf, nplan);
  dwrsttemp.SetLinearForm(dwrstnplandotnsurf, nplan, -1., dwrstnsurf);

  gp_Vec corde(ptcur, pts);
  gp_Vec ref, dwguiref, dwrstref;
  ref.SetLinearForm(raysurnorm, temp, corde);
  F(3) = ref.SquareMagnitude() - ray * ray;
  dwguiref.SetLinearForm(raysurnorm, dwguitemp, -raysurnorm2 * dwguinorm, temp);
  dwguiref.SetLinearForm(1., dwguiref, dray * unsurnorm, temp);
  dwrstref.SetLinearForm(raysurnorm, dwrsttemp, -raysurnorm2 * dwrstnorm, temp, dwrstpts);

  ref.Add(ref);
  D(3, 1) = ref.Dot(dwguiref) - 2. * dray * ray;
  D(3, 2) = -ref.Dot(d1cur);
  D(3, 3) = ref.Dot(dwrstref);
  return Standard_True;
}

//=================================================================================================

void BRepBlend_SurfCurvEvolRadInv::Set(const Handle(Adaptor2d_Curve2d)& Rst)
{
  rst = Rst;
}

//=================================================================================================

void BRepBlend_SurfCurvEvolRadInv::GetTolerance(math_Vector&        Tolerance,
                                                const Standard_Real Tol) const
{
  Tolerance(1) = guide->Resolution(Tol);
  Tolerance(2) = curv->Resolution(Tol);
  Standard_Real ru, rv;
  ru           = surf->UResolution(Tol);
  rv           = surf->VResolution(Tol);
  Tolerance(3) = rst->Resolution(Min(ru, rv));
}

//=================================================================================================

void BRepBlend_SurfCurvEvolRadInv::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
{
  InfBound(1) = guide->FirstParameter();
  SupBound(1) = guide->LastParameter();
  InfBound(2) = curv->FirstParameter();
  SupBound(2) = curv->LastParameter();
  InfBound(3) = rst->FirstParameter();
  SupBound(3) = rst->LastParameter();
}

//=================================================================================================

Standard_Boolean BRepBlend_SurfCurvEvolRadInv::IsSolution(const math_Vector&  Sol,
                                                          const Standard_Real Tol)
{
  math_Vector valsol(1, 3);
  Value(Sol, valsol);
  if (Abs(valsol(1)) <= Tol && Abs(valsol(2)) <= Tol && Abs(valsol(3)) <= 2 * Tol * Abs(ray))
  {
    return Standard_True;
  }
  return Standard_False;
}
