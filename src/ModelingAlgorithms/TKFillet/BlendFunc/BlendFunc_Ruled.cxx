// Created on: 1993-12-02
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Blend_Point.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_Ruled.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>

BlendFunc_Ruled::BlendFunc_Ruled(const occ::handle<Adaptor3d_Surface>& S1,
                                 const occ::handle<Adaptor3d_Surface>& S2,
                                 const occ::handle<Adaptor3d_Curve>&   C)
    :

      surf1(S1),
      surf2(S2),
      curv(C),
      istangent(true),
      normtg(0.0),
      theD(0.0),
      distmin(RealLast())
{
}

int BlendFunc_Ruled::NbEquations() const
{
  return 4;
}

void BlendFunc_Ruled::Set(const double Param)
{
  curv->D2(Param, ptgui, d1gui, d2gui);
  normtg    = d1gui.Magnitude();
  nplan     = d1gui.Normalized();
  theD      = -(nplan.XYZ().Dot(ptgui.XYZ()));
  istangent = true;
}

void BlendFunc_Ruled::Set(const double, const double)
{
  throw Standard_NotImplemented("BlendFunc_Ruled::Set");
}

void BlendFunc_Ruled::GetTolerance(math_Vector& Tolerance, const double Tol) const
{
  Tolerance(1) = surf1->UResolution(Tol);
  Tolerance(2) = surf1->VResolution(Tol);
  Tolerance(3) = surf2->UResolution(Tol);
  Tolerance(4) = surf2->VResolution(Tol);
}

void BlendFunc_Ruled::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
{
  InfBound(1) = surf1->FirstUParameter();
  InfBound(2) = surf1->FirstVParameter();
  InfBound(3) = surf2->FirstUParameter();
  InfBound(4) = surf2->FirstVParameter();
  SupBound(1) = surf1->LastUParameter();
  SupBound(2) = surf1->LastVParameter();
  SupBound(3) = surf2->LastUParameter();
  SupBound(4) = surf2->LastVParameter();

  for (int i = 1; i <= 4; i++)
  {
    if (!Precision::IsInfinite(InfBound(i)) && !Precision::IsInfinite(SupBound(i)))
    {
      const double range = (SupBound(i) - InfBound(i));
      InfBound(i) -= range;
      SupBound(i) += range;
    }
  }
}

bool BlendFunc_Ruled::IsSolution(const math_Vector& Sol, const double Tol)
{
  math_Vector valsol(1, 4), secmember(1, 4);
  math_Matrix gradsol(1, 4, 1, 4);

  gp_Vec        dnplan, d1u1, d1v1, d1u2, d1v2, temp, ns, ncrossns;
  double norm, ndotns, grosterme;

  Values(Sol, valsol, gradsol);
  if (std::abs(valsol(1)) <= Tol && std::abs(valsol(2)) <= Tol && std::abs(valsol(3)) <= Tol
      && std::abs(valsol(4)) <= Tol)
  {

    // Calcul des tangentes

    surf1->D1(Sol(1), Sol(2), pts1, d1u1, d1v1);
    surf2->D1(Sol(3), Sol(4), pts2, d1u2, d1v2);
    dnplan.SetLinearForm(1. / normtg, d2gui, -1. / normtg * (nplan.Dot(d2gui)), nplan);

    secmember(1) = normtg - dnplan.Dot(gp_Vec(ptgui, pts1));
    secmember(2) = normtg - dnplan.Dot(gp_Vec(ptgui, pts2));

    ns       = d1u1.Crossed(d1v1);
    ncrossns = nplan.Crossed(ns);
    ndotns   = nplan.Dot(ns);
    norm     = ncrossns.Magnitude();

    // Derivee de nor1 par rapport au parametre sur la ligne guide
    grosterme = ncrossns.Dot(dnplan.Crossed(ns)) / norm / norm;
    temp.SetLinearForm((dnplan.Dot(ns) - grosterme * ndotns) / norm,
                       nplan,
                       ndotns / norm,
                       dnplan,
                       grosterme / norm,
                       ns);

    secmember(3) = -(temp.Dot(gp_Vec(pts1, pts2)));

    ns       = d1u2.Crossed(d1v2);
    ncrossns = nplan.Crossed(ns);
    ndotns   = nplan.Dot(ns);
    norm     = ncrossns.Magnitude();

    // Derivee de nor2 par rapport au parametre sur la ligne guide
    grosterme = ncrossns.Dot(dnplan.Crossed(ns)) / norm / norm;
    temp.SetLinearForm((dnplan.Dot(ns) - grosterme * ndotns) / norm,
                       nplan,
                       ndotns / norm,
                       dnplan,
                       grosterme / norm,
                       ns);

    secmember(4) = -(temp.Dot(gp_Vec(pts1, pts2)));

    math_Gauss Resol(gradsol);
    if (Resol.IsDone())
    {

      Resol.Solve(secmember);

      tg1.SetLinearForm(secmember(1), d1u1, secmember(2), d1v1);
      tg2.SetLinearForm(secmember(3), d1u2, secmember(4), d1v2);
      tg12d.SetCoord(secmember(1), secmember(2));
      tg22d.SetCoord(secmember(3), secmember(4));
      istangent = false;
    }
    else
    {
      istangent = true;
    }
    return true;
  }
  istangent = true;
  return false;
}

//=================================================================================================

double BlendFunc_Ruled::GetMinimalDistance() const
{
  return distmin;
}

bool BlendFunc_Ruled::Value(const math_Vector& X, math_Vector& F)
{
  gp_Vec d1u1, d1v1, d1u2, d1v2;
  surf1->D1(X(1), X(2), pts1, d1u1, d1v1);
  surf2->D1(X(3), X(4), pts2, d1u2, d1v2);

  const gp_Vec temp(pts1, pts2);

  gp_Vec ns1 = d1u1.Crossed(d1v1);
  gp_Vec ns2 = d1u2.Crossed(d1v2);

  const double norm1 = nplan.Crossed(ns1).Magnitude();
  const double norm2 = nplan.Crossed(ns2).Magnitude();
  ns1.SetLinearForm(nplan.Dot(ns1) / norm1, nplan, -1. / norm1, ns1);
  ns2.SetLinearForm(nplan.Dot(ns2) / norm2, nplan, -1. / norm2, ns2);

  F(1) = (nplan.XYZ().Dot(pts1.XYZ())) + theD;
  F(2) = (nplan.XYZ().Dot(pts2.XYZ())) + theD;

  F(3) = temp.Dot(ns1);
  F(4) = temp.Dot(ns2);

  return true;
}

bool BlendFunc_Ruled::Derivatives(const math_Vector& X, math_Matrix& D)
{
  gp_Vec d1u1, d1v1, d1u2, d1v2;
  gp_Vec d2u1, d2v1, d2uv1, d2u2, d2v2, d2uv2;
  gp_Vec nor1, nor2, p1p2;
  gp_Vec ns1, ns2, ncrossns1, ncrossns2, resul, temp;

  double norm1, norm2, ndotns1, ndotns2, grosterme;

  surf1->D2(X(1), X(2), pts1, d1u1, d1v1, d2u1, d2v1, d2uv1);
  surf2->D2(X(3), X(4), pts2, d1u2, d1v2, d2u2, d2v2, d2uv2);

  D(1, 1) = nplan.Dot(d1u1);
  D(1, 2) = nplan.Dot(d1v1);
  D(1, 3) = 0.;
  D(1, 4) = 0.;

  D(2, 1) = 0.;
  D(2, 2) = 0.;
  D(2, 3) = nplan.Dot(d1u2);
  D(2, 4) = nplan.Dot(d1v2);

  ns1       = d1u1.Crossed(d1v1);
  ns2       = d1u2.Crossed(d1v2);
  ncrossns1 = nplan.Crossed(ns1);
  ncrossns2 = nplan.Crossed(ns2);
  norm1     = ncrossns1.Magnitude();
  norm2     = ncrossns2.Magnitude();

  ndotns1 = nplan.Dot(ns1);
  ndotns2 = nplan.Dot(ns2);

  nor1.SetLinearForm(ndotns1 / norm1, nplan, -1. / norm1, ns1);
  nor2.SetLinearForm(ndotns2 / norm2, nplan, -1. / norm2, ns2);

  p1p2 = gp_Vec(pts1, pts2);

  // Derivee de nor1 par rapport a u1
  temp      = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp)) / norm1 / norm1;
  resul.SetLinearForm(-(grosterme * ndotns1 - nplan.Dot(temp)) / norm1,
                      nplan,
                      grosterme / norm1,
                      ns1,
                      -1. / norm1,
                      temp);

  D(3, 1) = -(d1u1.Dot(nor1)) + p1p2.Dot(resul);

  // Derivee par rapport a v1
  temp      = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp)) / norm1 / norm1;
  resul.SetLinearForm(-(grosterme * ndotns1 - nplan.Dot(temp)) / norm1,
                      nplan,
                      grosterme / norm1,
                      ns1,
                      -1. / norm1,
                      temp);

  D(3, 2) = -(d1v1.Dot(nor1)) + p1p2.Dot(resul);

  D(3, 3) = d1u2.Dot(nor1);
  D(3, 4) = d1v2.Dot(nor1);

  D(4, 1) = -(d1u1.Dot(nor2));
  D(4, 2) = -(d1v1.Dot(nor2));

  // Derivee de nor2 par rapport a u2
  temp      = d2u2.Crossed(d1v2).Added(d1u2.Crossed(d2uv2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp)) / norm2 / norm2;
  resul.SetLinearForm(-(grosterme * ndotns2 - nplan.Dot(temp)) / norm2,
                      nplan,
                      grosterme / norm2,
                      ns2,
                      -1. / norm2,
                      temp);

  D(4, 3) = d1u2.Dot(nor2) + p1p2.Dot(resul);

  // Derivee par rapport a v2
  temp      = d2uv2.Crossed(d1v2).Added(d1u2.Crossed(d2v2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp)) / norm2 / norm2;
  resul.SetLinearForm(-(grosterme * ndotns2 - nplan.Dot(temp)) / norm2,
                      nplan,
                      grosterme / norm2,
                      ns2,
                      -1. / norm2,
                      temp);

  D(4, 4) = d1v2.Dot(nor2) + p1p2.Dot(resul);

  return true;
}

bool BlendFunc_Ruled::Values(const math_Vector& X, math_Vector& F, math_Matrix& D)
{
  gp_Vec d1u1, d1v1, d1u2, d1v2;
  gp_Vec d2u1, d2v1, d2uv1, d2u2, d2v2, d2uv2;
  gp_Vec nor1, nor2, p1p2;
  gp_Vec ns1, ns2, ncrossns1, ncrossns2, resul, temp;

  double norm1, norm2, ndotns1, ndotns2, grosterme;

  surf1->D2(X(1), X(2), pts1, d1u1, d1v1, d2u1, d2v1, d2uv1);
  surf2->D2(X(3), X(4), pts2, d1u2, d1v2, d2u2, d2v2, d2uv2);

  p1p2 = gp_Vec(pts1, pts2);

  ns1       = d1u1.Crossed(d1v1);
  ns2       = d1u2.Crossed(d1v2);
  ncrossns1 = nplan.Crossed(ns1);
  ncrossns2 = nplan.Crossed(ns2);
  norm1     = ncrossns1.Magnitude();
  norm2     = ncrossns2.Magnitude();

  ndotns1 = nplan.Dot(ns1);
  ndotns2 = nplan.Dot(ns2);

  nor1.SetLinearForm(ndotns1 / norm1, nplan, -1. / norm1, ns1);
  nor2.SetLinearForm(ndotns2 / norm2, nplan, -1. / norm2, ns2);

  F(1) = (nplan.XYZ().Dot(pts1.XYZ())) + theD;
  F(2) = (nplan.XYZ().Dot(pts2.XYZ())) + theD;
  F(3) = p1p2.Dot(nor1);
  F(4) = p1p2.Dot(nor2);

  D(1, 1) = nplan.Dot(d1u1);
  D(1, 2) = nplan.Dot(d1v1);
  D(1, 3) = 0.;
  D(1, 4) = 0.;

  D(2, 1) = 0.;
  D(2, 2) = 0.;
  D(2, 3) = nplan.Dot(d1u2);
  D(2, 4) = nplan.Dot(d1v2);

  // Derivee de nor1 par rapport a u1
  temp      = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp)) / norm1 / norm1;
  resul.SetLinearForm(-(grosterme * ndotns1 - nplan.Dot(temp)) / norm1,
                      nplan,
                      grosterme / norm1,
                      ns1,
                      -1. / norm1,
                      temp);

  D(3, 1) = -(d1u1.Dot(nor1)) + p1p2.Dot(resul);

  // Derivee par rapport a v1
  temp      = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp)) / norm1 / norm1;
  resul.SetLinearForm(-(grosterme * ndotns1 - nplan.Dot(temp)) / norm1,
                      nplan,
                      grosterme / norm1,
                      ns1,
                      -1. / norm1,
                      temp);

  D(3, 2) = -(d1v1.Dot(nor1)) + p1p2.Dot(resul);

  D(3, 3) = d1u2.Dot(nor1);
  D(3, 4) = d1v2.Dot(nor1);

  D(4, 1) = -(d1u1.Dot(nor2));
  D(4, 2) = -(d1v1.Dot(nor2));

  // Derivee de nor2 par rapport a u2
  temp      = d2u2.Crossed(d1v2).Added(d1u2.Crossed(d2uv2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp)) / norm2 / norm2;
  resul.SetLinearForm(-(grosterme * ndotns2 - nplan.Dot(temp)) / norm2,
                      nplan,
                      grosterme / norm2,
                      ns2,
                      -1. / norm2,
                      temp);

  D(4, 3) = d1u2.Dot(nor2) + p1p2.Dot(resul);

  // Derivee par rapport a v2
  temp      = d2uv2.Crossed(d1v2).Added(d1u2.Crossed(d2v2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp)) / norm2 / norm2;
  resul.SetLinearForm(-(grosterme * ndotns2 - nplan.Dot(temp)) / norm2,
                      nplan,
                      grosterme / norm2,
                      ns2,
                      -1. / norm2,
                      temp);

  D(4, 4) = d1v2.Dot(nor2) + p1p2.Dot(resul);

  return true;
}

void BlendFunc_Ruled::Tangent(const double U1,
                              const double V1,
                              const double U2,
                              const double V2,
                              gp_Vec&             TgF,
                              gp_Vec&             TgL,
                              gp_Vec&             NmF,
                              gp_Vec&             NmL) const
{
  gp_Pnt bid;
  gp_Vec d1u, d1v;
  gp_Vec ns1;

  surf2->D1(U2, V2, bid, d1u, d1v);
  NmL = d1u.Crossed(d1v);

  surf1->D1(U1, V1, bid, d1u, d1v);
  NmF = ns1 = d1u.Crossed(d1v);

  TgF = TgL = gp_Vec(pts1, pts2);
}

const gp_Pnt& BlendFunc_Ruled::PointOnS1() const
{
  return pts1;
}

const gp_Pnt& BlendFunc_Ruled::PointOnS2() const
{
  return pts2;
}

bool BlendFunc_Ruled::IsTangencyPoint() const
{
  return istangent;
}

const gp_Vec& BlendFunc_Ruled::TangentOnS1() const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_Ruled::TangentOnS1");
  return tg1;
}

const gp_Vec& BlendFunc_Ruled::TangentOnS2() const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_Ruled::TangentOnS2");
  return tg2;
}

const gp_Vec2d& BlendFunc_Ruled::Tangent2dOnS1() const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_Ruled::Tangent2dOnS1");
  return tg12d;
}

const gp_Vec2d& BlendFunc_Ruled::Tangent2dOnS2() const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_Ruled::Tangent2dOnS2");
  return tg22d;
}

bool BlendFunc_Ruled::GetSection(const double Param,
                                             const double U1,
                                             const double V1,
                                             const double U2,
                                             const double V2,
                                             NCollection_Array1<gp_Pnt>& tabP,
                                             NCollection_Array1<gp_Vec>& tabV)

{
  int NbPoint = tabP.Length();
  if (NbPoint != tabV.Length() || NbPoint < 2)
  {
    throw Standard_RangeError();
  }

  int i, lowp = tabP.Lower(), lowv = tabV.Lower();

  gp_Vec        dnplan, d1u1, d1v1, d1u2, d1v2, temp, ns, ncrossns;
  double norm, ndotns, grosterme, lambda;

  math_Vector sol(1, 4), valsol(1, 4), secmember(1, 4);
  math_Matrix gradsol(1, 4, 1, 4);

  curv->D2(Param, ptgui, d1gui, d2gui);
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  theD   = -(nplan.XYZ().Dot(ptgui.XYZ()));

  sol(1) = U1;
  sol(2) = V1;
  sol(3) = U2;
  sol(4) = V2;

  Values(sol, valsol, gradsol);

  surf1->D1(sol(1), sol(2), pts1, d1u1, d1v1);
  surf2->D1(sol(3), sol(4), pts2, d1u2, d1v2);
  dnplan.SetLinearForm(1. / normtg, d2gui, -1. / normtg * (nplan.Dot(d2gui)), nplan);

  secmember(1) = normtg - dnplan.Dot(gp_Vec(ptgui, pts1));
  secmember(2) = normtg - dnplan.Dot(gp_Vec(ptgui, pts2));

  ns       = d1u1.Crossed(d1v1);
  ncrossns = nplan.Crossed(ns);
  ndotns   = nplan.Dot(ns);
  norm     = ncrossns.Magnitude();

  // Derivee de nor1 par rapport au parametre sur la ligne guide
  grosterme = ncrossns.Dot(dnplan.Crossed(ns)) / norm / norm;
  temp.SetLinearForm((dnplan.Dot(ns) - grosterme * ndotns) / norm,
                     nplan,
                     ndotns / norm,
                     dnplan,
                     grosterme / norm,
                     ns);

  secmember(3) = -(temp.Dot(gp_Vec(pts1, pts2)));

  ns       = d1u2.Crossed(d1v2);
  ncrossns = nplan.Crossed(ns);
  ndotns   = nplan.Dot(ns);
  norm     = ncrossns.Magnitude();

  // Derivee de nor2 par rapport au parametre sur la ligne guide
  grosterme = ncrossns.Dot(dnplan.Crossed(ns)) / norm / norm;
  temp.SetLinearForm((dnplan.Dot(ns) - grosterme * ndotns) / norm,
                     nplan,
                     ndotns / norm,
                     dnplan,
                     grosterme / norm,
                     ns);

  secmember(4) = -(temp.Dot(gp_Vec(pts1, pts2)));

  math_Gauss Resol(gradsol);
  if (Resol.IsDone())
  {

    Resol.Solve(secmember);

    tg1.SetLinearForm(secmember(1), d1u1, secmember(2), d1v1);
    tg2.SetLinearForm(secmember(3), d1u2, secmember(4), d1v2);

    tabP(lowp)               = pts1;
    tabP(lowp + NbPoint - 1) = pts2;

    tabV(lowv)               = tg1;
    tabV(lowv + NbPoint - 1) = tg2;

    for (i = 2; i <= NbPoint - 1; i++)
    {

      lambda = (double)(i - 1) / (double)(NbPoint - 1);
      tabP(lowp + i - 1).SetXYZ((1. - lambda) * pts1.XYZ() + lambda * pts2.XYZ());
      tabV(lowv + i - 1).SetLinearForm(1. - lambda, tg1, lambda, tg2);
    }
    return true;
  }
  return false;
}

//=================================================================================================

bool BlendFunc_Ruled::IsRational() const
{
  return false;
}

//=================================================================================================

double BlendFunc_Ruled::GetSectionSize() const
{
  throw Standard_NotImplemented("BlendFunc_Ruled::GetSectionSize()");
}

//=================================================================================================

void BlendFunc_Ruled::GetMinimalWeight(NCollection_Array1<double>& Weigths) const
{
  Weigths.Init(1);
}

//=================================================================================================

int BlendFunc_Ruled::NbIntervals(const GeomAbs_Shape S) const
{
  return curv->NbIntervals(BlendFunc::NextShape(S));
}

//=================================================================================================

void BlendFunc_Ruled::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  curv->Intervals(T, BlendFunc::NextShape(S));
}

//=================================================================================================

void BlendFunc_Ruled::GetShape(int& NbPoles,
                               int& NbKnots,
                               int& Degree,
                               int& NbPoles2d)
{
  NbPoles   = 2;
  NbKnots   = 2;
  Degree    = 1;
  NbPoles2d = 2;
}

//=======================================================================
// function : GetTolerance
// purpose  : Determine les Tolerance a utiliser dans les approximations.
//=======================================================================
void BlendFunc_Ruled::GetTolerance(const double BoundTol,
                                   const double,
                                   const double,
                                   math_Vector& Tol3d,
                                   math_Vector&) const
{
  Tol3d.Init(BoundTol);
}

void BlendFunc_Ruled::Knots(NCollection_Array1<double>& TKnots)
{
  TKnots(TKnots.Lower()) = 0.;
  TKnots(TKnots.Upper()) = 1.;
}

void BlendFunc_Ruled::Mults(NCollection_Array1<int>& TMults)
{
  TMults(TMults.Lower()) = TMults(TMults.Upper()) = 2;
}

bool BlendFunc_Ruled::Section(const Blend_Point& /*P*/,
                                          NCollection_Array1<gp_Pnt>& /*Poles*/,
                                          NCollection_Array1<gp_Vec>& /*DPoles*/,
                                          NCollection_Array1<gp_Vec>& /*D2Poles*/,
                                          NCollection_Array1<gp_Pnt2d>& /*Poles2d*/,
                                          NCollection_Array1<gp_Vec2d>& /*DPoles2d*/,
                                          NCollection_Array1<gp_Vec2d>& /*D2Poles2d*/,
                                          NCollection_Array1<double>& /*Weights*/,
                                          NCollection_Array1<double>& /*DWeights*/,
                                          NCollection_Array1<double>& /*D2Weights*/)
{
  return false;
}

bool BlendFunc_Ruled::Section(const Blend_Point&    P,
                                          NCollection_Array1<gp_Pnt>&   Poles,
                                          NCollection_Array1<gp_Vec>&   DPoles,
                                          NCollection_Array1<gp_Pnt2d>& Poles2d,
                                          NCollection_Array1<gp_Vec2d>& DPoles2d,
                                          NCollection_Array1<double>& Weights,
                                          NCollection_Array1<double>& DWeights)
{
  int lowp = Poles.Lower(), lowp2d = Poles2d.Lower();
  double    u, v;

  Poles(lowp)     = P.PointOnS1();
  Poles(lowp + 1) = P.PointOnS2();

  P.ParametersOnS1(u, v);
  Poles2d(lowp2d).SetCoord(u, v);
  P.ParametersOnS2(u, v);
  Poles2d(lowp2d + 1).SetCoord(u, v);

  Weights(lowp)     = 1.;
  Weights(lowp + 1) = 1.;

  if (!P.IsTangencyPoint())
  {

    DPoles(lowp)     = P.TangentOnS1();
    DPoles(lowp + 1) = P.TangentOnS2();

    DPoles2d(lowp2d)     = P.Tangent2dOnS1();
    DPoles2d(lowp2d + 1) = P.Tangent2dOnS2();

    DWeights(lowp)     = 0.;
    DWeights(lowp + 1) = 0.;

    return true;
  }

  return false;
}

void BlendFunc_Ruled::Section(const Blend_Point&    P,
                              NCollection_Array1<gp_Pnt>&   Poles,
                              NCollection_Array1<gp_Pnt2d>& Poles2d,
                              NCollection_Array1<double>& Weights)
{
  int lowp = Poles.Lower(), lowp2d = Poles2d.Lower();
  double    u, v;

  Poles(lowp)     = P.PointOnS1();
  Poles(lowp + 1) = P.PointOnS2();

  P.ParametersOnS1(u, v);
  Poles2d(lowp2d).SetCoord(u, v);
  P.ParametersOnS2(u, v);
  Poles2d(lowp2d + 1).SetCoord(u, v);

  Weights(lowp)     = 1.;
  Weights(lowp + 1) = 1.;
}

gp_Ax1 BlendFunc_Ruled::AxeRot(const double Prm)
{
  gp_Ax1 axrot;
  gp_Vec dirax, dnplan;
  gp_Pnt oriax;

  curv->D2(Prm, ptgui, d1gui, d2gui);

  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  dnplan.SetLinearForm(1. / normtg, d2gui, -1. / normtg * (nplan.Dot(d2gui)), nplan);

  dirax = nplan.Crossed(dnplan);
  axrot.SetDirection(dirax);
  oriax.SetXYZ(ptgui.XYZ() + (normtg / dnplan.Magnitude()) * dnplan.Normalized().XYZ());
  axrot.SetLocation(oriax);
  return axrot;
}

void BlendFunc_Ruled::Resolution(const int IC2d,
                                 const double    Tol,
                                 double&         TolU,
                                 double&         TolV) const
{
  if (IC2d == 1)
  {
    TolU = surf1->UResolution(Tol);
    TolV = surf1->VResolution(Tol);
  }
  else
  {
    TolU = surf2->UResolution(Tol);
    TolV = surf2->VResolution(Tol);
  }
}
