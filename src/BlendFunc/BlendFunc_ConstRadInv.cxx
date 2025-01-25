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

#include <Adaptor2d_Curve2d.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_ConstRadInv.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>

#define Eps 1.e-15

BlendFunc_ConstRadInv::BlendFunc_ConstRadInv(const Handle(Adaptor3d_Surface)& S1,
                                             const Handle(Adaptor3d_Surface)& S2,
                                             const Handle(Adaptor3d_Curve)&   C)
    : surf1(S1),
      surf2(S2),
      curv(C),
      ray1(0.0),
      ray2(0.0),
      choix(0),
      first(Standard_False)
{
}

void BlendFunc_ConstRadInv::Set(const Standard_Real R, const Standard_Integer Choix)

{
  choix = Choix;
  switch (choix)
  {
    case 1:
    case 2: {
      ray1 = -R;
      ray2 = -R;
    }
    break;
    case 3:
    case 4: {
      ray1 = R;
      ray2 = -R;
    }
    break;
    case 5:
    case 6: {
      ray1 = R;
      ray2 = R;
    }
    break;
    case 7:
    case 8: {
      ray1 = -R;
      ray2 = R;
    }
    break;
    default:
      ray1 = ray2 = -R;
  }
}

void BlendFunc_ConstRadInv::Set(const Standard_Boolean OnFirst, const Handle(Adaptor2d_Curve2d)& C)
{
  first = OnFirst;
  csurf = C;
}

Standard_Integer BlendFunc_ConstRadInv::NbEquations() const
{
  return 4;
}

void BlendFunc_ConstRadInv::GetTolerance(math_Vector& Tolerance, const Standard_Real Tol) const
{
  Tolerance(1) = csurf->Resolution(Tol);
  Tolerance(2) = curv->Resolution(Tol);
  if (first)
  {
    Tolerance(3) = surf2->UResolution(Tol);
    Tolerance(4) = surf2->VResolution(Tol);
  }
  else
  {
    Tolerance(3) = surf1->UResolution(Tol);
    Tolerance(4) = surf1->VResolution(Tol);
  }
}

void BlendFunc_ConstRadInv::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
{
  InfBound(1) = csurf->FirstParameter();
  InfBound(2) = curv->FirstParameter();
  SupBound(1) = csurf->LastParameter();
  SupBound(2) = curv->LastParameter();

  if (first)
  {
    InfBound(3) = surf2->FirstUParameter();
    InfBound(4) = surf2->FirstVParameter();
    SupBound(3) = surf2->LastUParameter();
    SupBound(4) = surf2->LastVParameter();
    if (!Precision::IsInfinite(InfBound(3)) && !Precision::IsInfinite(SupBound(3)))
    {
      Standard_Real range = (SupBound(3) - InfBound(3));
      InfBound(3) -= range;
      SupBound(3) += range;
    }
    if (!Precision::IsInfinite(InfBound(4)) && !Precision::IsInfinite(SupBound(4)))
    {
      Standard_Real range = (SupBound(4) - InfBound(4));
      InfBound(4) -= range;
      SupBound(4) += range;
    }
  }
  else
  {
    InfBound(3) = surf1->FirstUParameter();
    InfBound(4) = surf1->FirstVParameter();
    SupBound(3) = surf1->LastUParameter();
    SupBound(4) = surf1->LastVParameter();
    if (!Precision::IsInfinite(InfBound(3)) && !Precision::IsInfinite(SupBound(3)))
    {
      Standard_Real range = (SupBound(3) - InfBound(3));
      InfBound(3) -= range;
      SupBound(3) += range;
    }
    if (!Precision::IsInfinite(InfBound(4)) && !Precision::IsInfinite(SupBound(4)))
    {
      Standard_Real range = (SupBound(4) - InfBound(4));
      InfBound(4) -= range;
      SupBound(4) += range;
    }
  }
}

Standard_Boolean BlendFunc_ConstRadInv::IsSolution(const math_Vector& Sol, const Standard_Real Tol)
{
  math_Vector valsol(1, 4);
  Value(Sol, valsol);
  if (Abs(valsol(1)) <= Tol
      && valsol(2) * valsol(2) + valsol(3) * valsol(3) + valsol(4) * valsol(4) <= Tol * Tol)
  {
    return Standard_True;
  }
  return Standard_False;
}

Standard_Boolean BlendFunc_ConstRadInv::Value(const math_Vector& X, math_Vector& F)
{
  gp_Pnt ptcur;
  gp_Vec d1cur;
  curv->D1(X(2), ptcur, d1cur);

  const gp_Vec        nplan = d1cur.Normalized();
  const Standard_Real theD  = -(nplan.XYZ().Dot(ptcur.XYZ()));

  const gp_Pnt2d pt2d(csurf->Value(X(1)));

  gp_Pnt pts1, pts2;
  gp_Vec d1u1, d1v1, d1u2, d1v2;
  if (first)
  {
    surf1->D1(pt2d.X(), pt2d.Y(), pts1, d1u1, d1v1);
    surf2->D1(X(3), X(4), pts2, d1u2, d1v2);
  }
  else
  {
    surf1->D1(X(3), X(4), pts1, d1u1, d1v1);
    surf2->D1(pt2d.X(), pt2d.Y(), pts2, d1u2, d1v2);
  }

  F(1) = (nplan.X() * (pts1.X() + pts2.X()) + nplan.Y() * (pts1.Y() + pts2.Y())
          + nplan.Z() * (pts1.Z() + pts2.Z()))
           / 2.
         + theD;

  gp_Vec ns1 = d1u1.Crossed(d1v1);
  if (ns1.Magnitude() < Eps)
  {
    if (first)
      BlendFunc::ComputeNormal(surf1, pt2d, ns1);
    else
    {
      gp_Pnt2d P(X(3), X(4));
      BlendFunc::ComputeNormal(surf1, P, ns1);
    }
  }

  gp_Vec ns2 = d1u2.Crossed(d1v2);
  if (ns2.Magnitude() < Eps)
  {
    if (!first)
      BlendFunc::ComputeNormal(surf2, pt2d, ns2);
    else
    {
      gp_Pnt2d P(X(3), X(4));
      BlendFunc::ComputeNormal(surf2, P, ns2);
    }
  }

  Standard_Real norm1 = nplan.Crossed(ns1).Magnitude();
  Standard_Real norm2 = nplan.Crossed(ns2).Magnitude();
  if (norm1 < Eps)
  {
    norm1 = 1;
  }
  if (norm2 < Eps)
  {
    norm2 = 1; // Unsatisfactory, but it is not necessary to stop
  }

  gp_Vec resul;
  ns1.SetLinearForm(nplan.Dot(ns1) / norm1, nplan, -1. / norm1, ns1);
  ns2.SetLinearForm(nplan.Dot(ns2) / norm2, nplan, -1. / norm2, ns2);
  resul.SetLinearForm(ray1, ns1, -1., pts2.XYZ(), -ray2, ns2, pts1.XYZ());
  F(2) = resul.X();
  F(3) = resul.Y();
  F(4) = resul.Z();

  return Standard_True;
}

Standard_Boolean BlendFunc_ConstRadInv::Derivatives(const math_Vector& X, math_Matrix& D)
{
  gp_Vec        d1u1, d1v1, d1u2, d1v2;
  gp_Vec        d2u1, d2v1, d2uv1, d2u2, d2v2, d2uv2;
  gp_Vec        d1cur, d2cur;
  gp_Vec        ns1, ns2, nplan, dnplan, ncrossns1, ncrossns2, resul1, resul2, temp;
  gp_Pnt        pts1, pts2, ptcur;
  gp_Pnt2d      p2d;
  gp_Vec2d      v2d;
  Standard_Real norm1, norm2, ndotns1, ndotns2, normtgcur;
  Standard_Real grosterme, theD;

  curv->D2(X(2), ptcur, d1cur, d2cur);
  normtgcur = d1cur.Magnitude();
  nplan     = d1cur.Normalized();
  theD      = -(nplan.XYZ().Dot(ptcur.XYZ()));

  dnplan.SetLinearForm(theD, nplan, d2cur);
  dnplan /= normtgcur;

  csurf->D1(X(1), p2d, v2d);
  if (first)
  {
    surf1->D2(p2d.X(), p2d.Y(), pts1, d1u1, d1v1, d2u1, d2v1, d2uv1);
    surf2->D2(X(3), X(4), pts2, d1u2, d1v2, d2u2, d2v2, d2uv2);
    temp.SetLinearForm(v2d.X(), d1u1, v2d.Y(), d1v1);
    D(1, 1) = nplan.Dot(temp) / 2.;
    temp.SetXYZ(0.5 * (pts1.XYZ() + pts2.XYZ()) - ptcur.XYZ());
    D(1, 2) = dnplan.Dot(temp) - normtgcur;
    D(1, 3) = nplan.Dot(d1u2) / 2.;
    D(1, 4) = nplan.Dot(d1v2) / 2.;
  }
  else
  {
    surf1->D2(X(3), X(4), pts1, d1u1, d1v1, d2u1, d2v1, d2uv1);
    surf2->D2(p2d.X(), p2d.Y(), pts2, d1u2, d1v2, d2u2, d2v2, d2uv2);
    temp.SetLinearForm(v2d.X(), d1u2, v2d.Y(), d1v2);
    D(1, 1) = nplan.Dot(temp) / 2.;
    temp.SetXYZ(0.5 * (pts1.XYZ() + pts2.XYZ()) - ptcur.XYZ());
    D(1, 2) = dnplan.Dot(temp) - normtgcur;
    D(1, 3) = nplan.Dot(d1u1) / 2.;
    D(1, 4) = nplan.Dot(d1v1) / 2.;
  }

  ns1 = d1u1.Crossed(d1v1);
  if (ns1.Magnitude() < Eps)
  {
    if (first)
      BlendFunc::ComputeNormal(surf1, p2d, ns1);
    else
    {
      gp_Pnt2d P(X(3), X(4));
      BlendFunc::ComputeNormal(surf1, P, ns1);
    }
  }

  ns2 = d1u2.Crossed(d1v2);
  if (ns2.Magnitude() < Eps)
  {
    if (!first)
      BlendFunc::ComputeNormal(surf2, p2d, ns2);
    else
    {
      gp_Pnt2d P(X(3), X(4));
      BlendFunc::ComputeNormal(surf2, P, ns2);
    }
  }

  ncrossns1 = nplan.Crossed(ns1);
  ncrossns2 = nplan.Crossed(ns2);
  norm1     = ncrossns1.Magnitude();
  norm2     = ncrossns2.Magnitude();
  if (norm1 < Eps)
  {
    norm1 = 1; // Unsatisfactory, but it is not necessary to stop
#ifdef OCCT_DEBUG
    std::cout << " ConstRadInv : Surface singuliere " << std::endl;
#endif
  }
  if (norm2 < Eps)
  {
    norm2 = 1; // Unsatisfactory, but it is not necessary to stop
#ifdef OCCT_DEBUG
    std::cout << " ConstRadInv : Surface singuliere " << std::endl;
#endif
  }

  ndotns1 = nplan.Dot(ns1);
  ndotns2 = nplan.Dot(ns2);

  // Derived compared to u1

  temp      = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp)) / norm1 / norm1;
  resul1.SetLinearForm(-ray1 / norm1 * (grosterme * ndotns1 - nplan.Dot(temp)),
                       nplan,
                       ray1 * grosterme / norm1,
                       ns1,
                       -ray1 / norm1,
                       temp,
                       d1u1);

  // Derived compared to v1

  temp      = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp)) / norm1 / norm1;
  resul2.SetLinearForm(-ray1 / norm1 * (grosterme * ndotns1 - nplan.Dot(temp)),
                       nplan,
                       ray1 * grosterme / norm1,
                       ns1,
                       -ray1 / norm1,
                       temp,
                       d1v1);

  if (first)
  {
    D(2, 1) = v2d.X() * resul1.X() + v2d.Y() * resul2.X();
    D(3, 1) = v2d.X() * resul1.Y() + v2d.Y() * resul2.Y();
    D(4, 1) = v2d.X() * resul1.Z() + v2d.Y() * resul2.Z();
  }
  else
  {
    D(2, 3) = resul1.X();
    D(3, 3) = resul1.Y();
    D(4, 3) = resul1.Z();

    D(2, 4) = resul2.X();
    D(3, 4) = resul2.Y();
    D(4, 4) = resul2.Z();
  }

  // derived compared to w (parameter on guideline)
  // It is assumed that the radius is constant

  grosterme = ncrossns1.Dot(dnplan.Crossed(ns1)) / norm1 / norm1;
  resul1.SetLinearForm(-ray1 / norm1 * (grosterme * ndotns1 - dnplan.Dot(ns1)),
                       nplan,
                       ray1 * ndotns1 / norm1,
                       dnplan,
                       ray1 * grosterme / norm1,
                       ns1);

  grosterme = ncrossns2.Dot(dnplan.Crossed(ns2)) / norm2 / norm2;
  resul2.SetLinearForm(ray2 / norm2 * (grosterme * ndotns2 - dnplan.Dot(ns2)),
                       nplan,
                       -ray2 * ndotns2 / norm2,
                       dnplan,
                       -ray2 * grosterme / norm2,
                       ns2);

  D(2, 2) = resul1.X() + resul2.X();
  D(3, 2) = resul1.Y() + resul2.Y();
  D(4, 2) = resul1.Z() + resul2.Z();

  // Derived compared to u2
  temp      = d2u2.Crossed(d1v2).Added(d1u2.Crossed(d2uv2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp)) / norm2 / norm2;
  resul1.SetLinearForm(ray2 / norm2 * (grosterme * ndotns2 - nplan.Dot(temp)),
                       nplan,
                       -ray2 * grosterme / norm2,
                       ns2,
                       ray2 / norm2,
                       temp);
  resul1.Subtract(d1u2);

  // Derived compared to v2
  temp      = d2uv2.Crossed(d1v2).Added(d1u2.Crossed(d2v2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp)) / norm2 / norm2;
  resul2.SetLinearForm(ray2 / norm2 * (grosterme * ndotns2 - nplan.Dot(temp)),
                       nplan,
                       -ray2 * grosterme / norm2,
                       ns2,
                       ray2 / norm2,
                       temp);
  resul2.Subtract(d1v2);

  if (!first)
  {
    D(2, 1) = v2d.X() * resul1.X() + v2d.Y() * resul2.X();
    D(3, 1) = v2d.X() * resul1.Y() + v2d.Y() * resul2.Y();
    D(4, 1) = v2d.X() * resul1.Z() + v2d.Y() * resul2.Z();
  }
  else
  {
    D(2, 3) = resul1.X();
    D(3, 3) = resul1.Y();
    D(4, 3) = resul1.Z();

    D(2, 4) = resul2.X();
    D(3, 4) = resul2.Y();
    D(4, 4) = resul2.Z();
  }

  return Standard_True;
}

Standard_Boolean BlendFunc_ConstRadInv::Values(const math_Vector& X, math_Vector& F, math_Matrix& D)
{
  gp_Vec        d1u1, d1v1, d1u2, d1v2, d1cur;
  gp_Vec        d2u1, d2v1, d2uv1, d2u2, d2v2, d2uv2, d2cur;
  gp_Vec        ns1, ns2, nplan, dnplan, ncrossns1, ncrossns2, resul1, resul2, temp;
  gp_Pnt        ptcur, pts1, pts2;
  gp_Pnt2d      p2d;
  gp_Vec2d      v2d;
  Standard_Real norm1, norm2, ndotns1, ndotns2, normtgcur;
  Standard_Real grosterme, theD;

  curv->D2(X(2), ptcur, d1cur, d2cur);
  normtgcur = d1cur.Magnitude();
  nplan     = d1cur.Normalized();
  theD      = -(nplan.XYZ().Dot(ptcur.XYZ()));
  dnplan.SetLinearForm(-nplan.Dot(d2cur), nplan, d2cur);
  dnplan /= normtgcur;

  csurf->D1(X(1), p2d, v2d);

  if (first)
  {
    surf1->D2(p2d.X(), p2d.Y(), pts1, d1u1, d1v1, d2u1, d2v1, d2uv1);
    surf2->D2(X(3), X(4), pts2, d1u2, d1v2, d2u2, d2v2, d2uv2);

    temp.SetLinearForm(v2d.X(), d1u1, v2d.Y(), d1v1);
    D(1, 1) = nplan.Dot(temp) / 2.;
    temp.SetXYZ(0.5 * (pts1.XYZ() + pts2.XYZ()) - ptcur.XYZ());
    D(1, 2) = dnplan.Dot(temp) - normtgcur;
    D(1, 3) = nplan.Dot(d1u2) / 2.;
    D(1, 4) = nplan.Dot(d1v2) / 2.;
  }
  else
  {
    surf1->D2(X(3), X(4), pts1, d1u1, d1v1, d2u1, d2v1, d2uv1);
    surf2->D2(p2d.X(), p2d.Y(), pts2, d1u2, d1v2, d2u2, d2v2, d2uv2);

    temp.SetLinearForm(v2d.X(), d1u2, v2d.Y(), d1v2);
    D(1, 1) = nplan.Dot(temp) / 2.;
    temp.SetXYZ(0.5 * (pts1.XYZ() + pts2.XYZ()) - ptcur.XYZ());
    D(1, 2) = dnplan.Dot(temp) - normtgcur;
    D(1, 3) = nplan.Dot(d1u1) / 2.;
    D(1, 4) = nplan.Dot(d1v1) / 2.;
  }

  F(1) = (nplan.X() * (pts1.X() + pts2.X()) + nplan.Y() * (pts1.Y() + pts2.Y())
          + nplan.Z() * (pts1.Z() + pts2.Z()))
           / 2.
         + theD;

  ns1 = d1u1.Crossed(d1v1);
  if (ns1.Magnitude() < Eps)
  {
    if (first)
      BlendFunc::ComputeNormal(surf1, p2d, ns1);
    else
    {
      gp_Pnt2d P(X(3), X(4));
      BlendFunc::ComputeNormal(surf1, P, ns1);
    }
  }

  ns2 = d1u2.Crossed(d1v2);
  if (ns2.Magnitude() < Eps)
  {
    if (!first)
      BlendFunc::ComputeNormal(surf2, p2d, ns2);
    else
    {
      gp_Pnt2d P(X(3), X(4));
      BlendFunc::ComputeNormal(surf2, P, ns2);
    }
  }

  ncrossns1 = nplan.Crossed(ns1);
  ncrossns2 = nplan.Crossed(ns2);
  norm1     = ncrossns1.Magnitude();
  norm2     = ncrossns2.Magnitude();
  if (norm1 < Eps)
  {
    norm1 = 1; // Unsatisfactory, but it is not necessary to stop
#ifdef OCCT_DEBUG
    std::cout << " ConstRadInv : Surface singuliere " << std::endl;
#endif
  }
  if (norm2 < Eps)
  {
    norm2 = 1; // Unsatisfactory, but it is not necessary to stop
#ifdef OCCT_DEBUG
    std::cout << " ConstRadInv : Surface singuliere " << std::endl;
#endif
  }

  ndotns1 = nplan.Dot(ns1);
  ndotns2 = nplan.Dot(ns2);

  temp.SetLinearForm(ndotns1 / norm1, nplan, -1. / norm1, ns1);
  resul1.SetLinearForm(ray1, temp, gp_Vec(pts2, pts1));
  temp.SetLinearForm(ndotns2 / norm2, nplan, -1. / norm2, ns2);
  resul1.Subtract(ray2 * temp);

  F(2) = resul1.X();
  F(3) = resul1.Y();
  F(4) = resul1.Z();

  // Derived compared to u1

  temp      = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp)) / norm1 / norm1;
  resul1.SetLinearForm(-ray1 / norm1 * (grosterme * ndotns1 - nplan.Dot(temp)),
                       nplan,
                       ray1 * grosterme / norm1,
                       ns1,
                       -ray1 / norm1,
                       temp,
                       d1u1);

  // Derived compared to v1

  temp      = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp)) / norm1 / norm1;
  resul2.SetLinearForm(-ray1 / norm1 * (grosterme * ndotns1 - nplan.Dot(temp)),
                       nplan,
                       ray1 * grosterme / norm1,
                       ns1,
                       -ray1 / norm1,
                       temp,
                       d1v1);

  if (first)
  {
    D(2, 1) = v2d.X() * resul1.X() + v2d.Y() * resul2.X();
    D(3, 1) = v2d.X() * resul1.Y() + v2d.Y() * resul2.Y();
    D(4, 1) = v2d.X() * resul1.Z() + v2d.Y() * resul2.Z();
  }
  else
  {
    D(2, 3) = resul1.X();
    D(3, 3) = resul1.Y();
    D(4, 3) = resul1.Z();

    D(2, 4) = resul2.X();
    D(3, 4) = resul2.Y();
    D(4, 4) = resul2.Z();
  }

  // derived compared to w (parameter on guideline)
  // It is assumed that the raduis is constant

  grosterme = ncrossns1.Dot(dnplan.Crossed(ns1)) / norm1 / norm1;
  resul1.SetLinearForm(-ray1 / norm1 * (grosterme * ndotns1 - dnplan.Dot(ns1)),
                       nplan,
                       ray1 * ndotns1 / norm1,
                       dnplan,
                       ray1 * grosterme / norm1,
                       ns1);

  grosterme = ncrossns2.Dot(dnplan.Crossed(ns2)) / norm2 / norm2;
  resul2.SetLinearForm(ray2 / norm2 * (grosterme * ndotns2 - dnplan.Dot(ns2)),
                       nplan,
                       -ray2 * ndotns2 / norm2,
                       dnplan,
                       -ray2 * grosterme / norm2,
                       ns2);

  D(2, 2) = resul1.X() + resul2.X();
  D(3, 2) = resul1.Y() + resul2.Y();
  D(4, 2) = resul1.Z() + resul2.Z();

  // Derived compared to u2
  temp      = d2u2.Crossed(d1v2).Added(d1u2.Crossed(d2uv2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp)) / norm2 / norm2;
  resul1.SetLinearForm(ray2 / norm2 * (grosterme * ndotns2 - nplan.Dot(temp)),
                       nplan,
                       -ray2 * grosterme / norm2,
                       ns2,
                       ray2 / norm2,
                       temp);
  resul1.Subtract(d1u2);

  // Derived compared to v2
  temp      = d2uv2.Crossed(d1v2).Added(d1u2.Crossed(d2v2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp)) / norm2 / norm2;
  resul2.SetLinearForm(ray2 / norm2 * (grosterme * ndotns2 - nplan.Dot(temp)),
                       nplan,
                       -ray2 * grosterme / norm2,
                       ns2,
                       ray2 / norm2,
                       temp);
  resul2.Subtract(d1v2);

  if (!first)
  {
    D(2, 1) = v2d.X() * resul1.X() + v2d.Y() * resul2.X();
    D(3, 1) = v2d.X() * resul1.Y() + v2d.Y() * resul2.Y();
    D(4, 1) = v2d.X() * resul1.Z() + v2d.Y() * resul2.Z();
  }
  else
  {
    D(2, 3) = resul1.X();
    D(3, 3) = resul1.Y();
    D(4, 3) = resul1.Z();

    D(2, 4) = resul2.X();
    D(3, 4) = resul2.Y();
    D(4, 4) = resul2.Z();
  }
  return Standard_True;
}
