// Created by: Julia GERASIMOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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
#include <BlendFunc_Chamfer.hxx>
#include <ElCLib.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>

//=================================================================================================

BlendFunc_GenChamfer::BlendFunc_GenChamfer(const occ::handle<Adaptor3d_Surface>& S1,
                                           const occ::handle<Adaptor3d_Surface>& S2,
                                           const occ::handle<Adaptor3d_Curve>&   CG)
    : surf1(S1),
      surf2(S2),
      curv(CG),
      choix(0),
      tol(0.0),
      distmin(RealLast())
{
}

//=================================================================================================

int BlendFunc_GenChamfer::NbEquations() const
{
  return 4;
}

//=================================================================================================

void BlendFunc_GenChamfer::Set(const double, const double) {}

//=================================================================================================

void BlendFunc_GenChamfer::GetTolerance(math_Vector& Tolerance, const double Tol) const
{
  Tolerance(1) = surf1->UResolution(Tol);
  Tolerance(2) = surf1->VResolution(Tol);
  Tolerance(3) = surf2->UResolution(Tol);
  Tolerance(4) = surf2->VResolution(Tol);
}

//=================================================================================================

void BlendFunc_GenChamfer::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
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

//=================================================================================================

double BlendFunc_GenChamfer::GetMinimalDistance() const
{
  return distmin;
}

//=================================================================================================

bool BlendFunc_GenChamfer::Values(const math_Vector& X, math_Vector& F, math_Matrix& D)
{
  bool val = Value(X, F);
  return (val && Derivatives(X, D));
}

//=================================================================================================

void BlendFunc_GenChamfer::Section(const double /*Param*/,
                                   const double U1,
                                   const double V1,
                                   const double U2,
                                   const double V2,
                                   double&      Pdeb,
                                   double&      Pfin,
                                   gp_Lin&      C)
{
  const gp_Pnt pts1 = surf1->Value(U1, V1);
  const gp_Pnt pts2 = surf2->Value(U2, V2);
  const gp_Dir dir(gp_Vec(pts1, pts2));

  C.SetLocation(pts1);
  C.SetDirection(dir);

  Pdeb = 0.;
  Pfin = ElCLib::Parameter(C, pts2);
}

//=================================================================================================

bool BlendFunc_GenChamfer::IsRational() const
{
  return false;
}

//=================================================================================================

void BlendFunc_GenChamfer::GetMinimalWeight(NCollection_Array1<double>& Weights) const
{
  Weights.Init(1);
}

//=================================================================================================

int BlendFunc_GenChamfer::NbIntervals(const GeomAbs_Shape S) const
{
  return curv->NbIntervals(BlendFunc::NextShape(S));
}

//=================================================================================================

void BlendFunc_GenChamfer::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  curv->Intervals(T, BlendFunc::NextShape(S));
}

//=================================================================================================

void BlendFunc_GenChamfer::GetShape(int& NbPoles, int& NbKnots, int& Degree, int& NbPoles2d)
{
  NbPoles   = 2;
  NbPoles2d = 2;
  NbKnots   = 2;
  Degree    = 1;
}

//=======================================================================
// function : GetTolerance
// purpose  : Determine les Tolerance a utiliser dans les approximations.
//=======================================================================
void BlendFunc_GenChamfer::GetTolerance(const double BoundTol,
                                        const double,
                                        const double,
                                        math_Vector& Tol3d,
                                        math_Vector&) const
{
  Tol3d.Init(BoundTol);
}

//=================================================================================================

void BlendFunc_GenChamfer::Knots(NCollection_Array1<double>& TKnots)
{
  TKnots(1) = 0.;
  TKnots(2) = 1.;
}

//=================================================================================================

void BlendFunc_GenChamfer::Mults(NCollection_Array1<int>& TMults)
{
  TMults(1) = 2;
  TMults(2) = 2;
}

//=================================================================================================

bool BlendFunc_GenChamfer::Section(const Blend_Point& /*P*/,
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

//=================================================================================================

bool BlendFunc_GenChamfer::Section(const Blend_Point&            P,
                                   NCollection_Array1<gp_Pnt>&   Poles,
                                   NCollection_Array1<gp_Vec>&   DPoles,
                                   NCollection_Array1<gp_Pnt2d>& Poles2d,
                                   NCollection_Array1<gp_Vec2d>& DPoles2d,
                                   NCollection_Array1<double>&   Weights,
                                   NCollection_Array1<double>&   DWeights)
{
  math_Vector sol(1, 4), valsol(1, 4), secmember(1, 4);
  math_Matrix gradsol(1, 4, 1, 4);

  double prm = P.Parameter();
  int    low = Poles.Lower();
  int    upp = Poles.Upper();
  bool   istgt;

  P.ParametersOnS1(sol(1), sol(2));
  P.ParametersOnS2(sol(3), sol(4));

  Set(prm);

  Values(sol, valsol, gradsol);
  IsSolution(sol, tol);

  istgt = IsTangencyPoint();

  Poles2d(Poles2d.Lower()).SetCoord(sol(1), sol(2));
  Poles2d(Poles2d.Upper()).SetCoord(sol(3), sol(4));
  if (!istgt)
  {
    DPoles2d(Poles2d.Lower()).SetCoord(Tangent2dOnS1().X(), Tangent2dOnS1().Y());
    DPoles2d(Poles2d.Upper()).SetCoord(Tangent2dOnS2().X(), Tangent2dOnS2().Y());
  }
  Poles(low)   = PointOnS1();
  Poles(upp)   = PointOnS2();
  Weights(low) = 1.0;
  Weights(upp) = 1.0;
  if (!istgt)
  {
    DPoles(low)   = TangentOnS1();
    DPoles(upp)   = TangentOnS2();
    DWeights(low) = 0.0;
    DWeights(upp) = 0.0;
  }

  return (!istgt);
}

//=================================================================================================

void BlendFunc_GenChamfer::Section(const Blend_Point&            P,
                                   NCollection_Array1<gp_Pnt>&   Poles,
                                   NCollection_Array1<gp_Pnt2d>& Poles2d,
                                   NCollection_Array1<double>&   Weights)
{
  double      u1, v1, u2, v2, prm = P.Parameter();
  int         low = Poles.Lower();
  int         upp = Poles.Upper();
  math_Vector X(1, 4), F(1, 4);

  P.ParametersOnS1(u1, v1);
  P.ParametersOnS2(u2, v2);
  X(1) = u1;
  X(2) = v1;
  X(3) = u2;
  X(4) = v2;
  Poles2d(Poles2d.Lower()).SetCoord(u1, v1);
  Poles2d(Poles2d.Upper()).SetCoord(u2, v2);

  Set(prm);
  Value(X, F);
  Poles(low)   = PointOnS1();
  Poles(upp)   = PointOnS2();
  Weights(low) = 1.0;
  Weights(upp) = 1.0;
}

void BlendFunc_GenChamfer::Resolution(const int    IC2d,
                                      const double Tol,
                                      double&      TolU,
                                      double&      TolV) const
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
