// Created on: 1996-09-09
// Created by: Herve LOUESSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomLProp_SLProps.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <StdFail_NotDone.hxx>

#include <algorithm>
#include <cmath>

namespace
{
//==================================================================================================

// Derivative order needed by the supported local continuity criteria.
int derivativeOrder(const GeomAbs_Shape theOrder)
{
  if (theOrder == GeomAbs_C0)
  {
    return 0;
  }
  return theOrder == GeomAbs_C1 || theOrder == GeomAbs_G1 ? 1 : 2;
}
} // namespace

/*********************************************************************************/
/*********************************************************************************/
void LocalAnalysis_SurfaceContinuity::SurfC0(const GeomLProp_SLProps& Surf1,
                                             const GeomLProp_SLProps& Surf2)
{
  myContC0 = (Surf1.Value()).Distance(Surf2.Value());
}

/*********************************************************************************/

void LocalAnalysis_SurfaceContinuity::SurfC1(GeomLProp_SLProps& Surf1, GeomLProp_SLProps& Surf2)
{
  gp_Vec V1u, V2u, V1v, V2v;
  double norm1u, norm2u, norm1v, norm2v, angu, angv;

  V1u = Surf1.D1U();
  V2u = Surf2.D1U();

  V1v = Surf1.D1V();
  V2v = Surf2.D1V();

  norm1u = V1u.Magnitude();
  norm2u = V2u.Magnitude();
  norm1v = V1v.Magnitude();
  norm2v = V2v.Magnitude();

  if ((norm1u > myepsnul) && (norm2u > myepsnul) && (norm1v > myepsnul) && (norm2v > myepsnul))
  {
    if (norm1u >= norm2u)
    {
      myLambda1U = norm2u / norm1u;
    }
    else
    {
      myLambda1U = norm1u / norm2u;
    }
    if (norm1v >= norm2v)
    {
      myLambda1V = norm2v / norm1v;
    }
    else
    {
      myLambda1V = norm1v / norm2v;
    }
    angu = V1u.Angle(V2u);
    if (angu > M_PI / 2)
    {
      myContC1U = M_PI - angu;
    }
    else
    {
      myContC1U = angu;
    }
    angv = V1v.Angle(V2v);
    if (angv > M_PI / 2)
    {
      myContC1V = M_PI - angv;
    }
    else
    {
      myContC1V = angv;
    }
  }
  else
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_NullFirstDerivative;
  }
}

/*********************************************************************************/

void LocalAnalysis_SurfaceContinuity::SurfC2(GeomLProp_SLProps& Surf1, GeomLProp_SLProps& Surf2)

{
  gp_Vec V11u, V12u, V21u, V22u, V11v, V12v, V21v, V22v;
  double norm11u, norm12u, norm21u, norm22u, norm11v, norm12v, norm21v, norm22v;
  double ang;
  V11u    = Surf1.D1U();
  V12u    = Surf2.D1U();
  V21u    = Surf1.D2U();
  V22u    = Surf2.D2U();
  norm11u = V11u.Magnitude();
  norm12u = V12u.Magnitude();
  norm21u = V21u.Magnitude();
  norm22u = V22u.Magnitude();

  if ((norm11u > myepsnul) && (norm12u > myepsnul))
  {
    if ((norm21u > myepsnul) && (norm22u > myepsnul))
    {
      if (norm11u >= norm12u)
      {
        myLambda1U = norm12u / norm11u;
        myLambda2U = norm22u / norm21u;
      }
      else
      {
        myLambda1U = norm11u / norm12u;
        myLambda2U = norm21u / norm22u;
      }
      ang = V21u.Angle(V22u);
      if (ang > M_PI / 2)
      {
        myContC2U = M_PI - ang;
      }
      else
      {
        myContC2U = ang;
      }
    }
    else
    {
      myIsDone      = false;
      myErrorStatus = LocalAnalysis_NullSecondDerivative;
    }
  }

  else
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_NullFirstDerivative;
  }

  V11v    = Surf1.D1V();
  V12v    = Surf2.D1V();
  V21v    = Surf1.D2V();
  V22v    = Surf2.D2V();
  norm11v = V11v.Magnitude();
  norm12v = V12v.Magnitude();
  norm21v = V21v.Magnitude();
  norm22v = V22v.Magnitude();

  if ((norm11v > myepsnul) && (norm12v > myepsnul))
  {
    if ((norm21v > myepsnul) && (norm22v > myepsnul))
    {
      if (norm11v >= norm12v)
      {
        myLambda1V = norm12v / norm11v;
        myLambda2V = norm22v / norm21v;
      }
      else
      {
        myLambda1V = norm11v / norm12v;
        myLambda2V = norm21v / norm22v;
      }
      ang = V21v.Angle(V22v);
      if (ang > M_PI / 2)
      {
        myContC2V = M_PI - ang;
      }
      else
      {
        myContC2V = ang;
      }
    }
    else
    {
      myIsDone      = false;
      myErrorStatus = LocalAnalysis_NullSecondDerivative;
    }
  }
  else
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_NullFirstDerivative;
  }
}

/*********************************************************************************/
void LocalAnalysis_SurfaceContinuity::SurfG1(GeomLProp_SLProps& Surf1, GeomLProp_SLProps& Surf2)
{
  if (Surf1.IsNormalDefined() && Surf2.IsNormalDefined())
  {
    gp_Dir D1  = Surf1.Normal();
    gp_Dir D2  = Surf2.Normal();
    double ang = D1.Angle(D2);
    if (ang > M_PI / 2)
    {
      myContG1 = M_PI - ang;
    }
    else
    {
      myContG1 = ang;
    }
  }
  else
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_NormalNotDefined;
  }
}

/*********************************************************************************/

void LocalAnalysis_SurfaceContinuity::SurfG2(GeomLProp_SLProps& Surf1, GeomLProp_SLProps& Surf2)
{
  if (!Surf1.IsCurvatureDefined() || !Surf2.IsCurvatureDefined())
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_CurvatureNotDefined;
    return;
  }

  const gp_Dir& aNormal1   = Surf1.Normal();
  const gp_Dir& aNormal2   = Surf2.Normal();
  const double  aNormalDot = aNormal1.Dot(aNormal2);
  const double  aSign      = aNormalDot < 0.0 ? -1.0 : 1.0;
  const double  aMaxCurv1  = Surf1.MaxCurvature();
  const double  aMinCurv1  = Surf1.MinCurvature();
  const double  aMaxCurv2  = aSign * Surf2.MaxCurvature();
  const double  aMinCurv2  = aSign * Surf2.MinCurvature();
  myCurvatureScale         = std::max(std::max(std::abs(aMaxCurv1), std::abs(aMinCurv1)),
                              std::max(std::abs(aMaxCurv2), std::abs(aMinCurv2)));

  // An umbilic shape operator is a scalar multiple of the identity, so its
  // principal directions and the tangent-plane rotation are immaterial.
  if (aMaxCurv1 == aMinCurv1)
  {
    myGap = std::max(std::abs(aMaxCurv1 - aMaxCurv2), std::abs(aMaxCurv1 - aMinCurv2));
    return;
  }
  if (aMaxCurv2 == aMinCurv2)
  {
    myGap = std::max(std::abs(aMaxCurv1 - aMaxCurv2), std::abs(aMinCurv1 - aMaxCurv2));
    return;
  }

  gp_Dir aMax1, aMin1, aMax2, aMin2;
  Surf1.CurvatureDirections(aMax1, aMin1);
  Surf2.CurvatureDirections(aMax2, aMin2);

  // Transport a tangent t from n2 to n1 by their shortest rotation:
  // t' = t - (t.n1) / (1 + n1.n2) * (n1 + n2).
  // Aligning the normal signs makes the denominator >= 1, including opposite
  // parameterizations. No quaternion, matrix, or second transported axis is needed.
  const gp_XYZ aNormalSum = aNormal1.XYZ() + aSign * aNormal2.XYZ();
  const gp_XYZ aMax2Aligned =
    aMax2.XYZ() - (aMax2.Dot(aNormal1) / (1.0 + std::abs(aNormalDot))) * aNormalSum;
  const double aX = aMax1.XYZ().Dot(aMax2Aligned);
  const double aY = aMin1.XYZ().Dot(aMax2Aligned);
  // S2 = kMin2 * I + (kMax2 - kMin2) * t' * t'^T.
  const double aDelta = aMaxCurv2 - aMinCurv2;
  const double aD11   = aMaxCurv1 - aMinCurv2 - aDelta * aX * aX;
  const double aD22   = aMinCurv1 - aMinCurv2 - aDelta * aY * aY;
  const double aD12   = -aDelta * aX * aY;

  // Spectral radius of the symmetric operator difference.
  myGap = std::abs(0.5 * (aD11 + aD22)) + std::hypot(0.5 * (aD11 - aD22), aD12);
}

LocalAnalysis_SurfaceContinuity::LocalAnalysis_SurfaceContinuity(const double EpsNul,
                                                                 const double EpsC0,
                                                                 const double EpsC1,
                                                                 const double EpsC2,
                                                                 const double EpsG1,
                                                                 const double Percent,
                                                                 const double Maxlen)
    : myContC0(0.0),
      myContC1U(0.0),
      myContC1V(0.0),
      myContC2U(0.0),
      myContC2V(0.0),
      myContG1(0.0),
      myLambda1U(0.0),
      myLambda2U(0.0),
      myLambda1V(0.0),
      myLambda2V(0.0),
      myCurvatureScale(0.0),
      myGap(0.0)
{
  myepsnul      = EpsNul;
  myepsC0       = EpsC0;
  myepsC1       = EpsC1;
  myepsC2       = EpsC2;
  myepsG1       = EpsG1;
  myperce       = Percent;
  mymaxlen      = Maxlen;
  myIsDone      = false;
  myTypeCont    = GeomAbs_C0;
  myErrorStatus = LocalAnalysis_InvalidInput;
}

void LocalAnalysis_SurfaceContinuity::ComputeAnalysis(GeomLProp_SLProps&  Surf1,
                                                      GeomLProp_SLProps&  Surf2,
                                                      const GeomAbs_Shape Order)
{
  myIsDone      = true;
  myErrorStatus = LocalAnalysis_NoError;
  myTypeCont    = Order;
  switch (Order)
  {
    case GeomAbs_C0: {
      SurfC0(Surf1, Surf2);
    }
    break;
    case GeomAbs_C1: {
      SurfC0(Surf1, Surf2);
      SurfC1(Surf1, Surf2);
    }
    break;
    case GeomAbs_C2: {
      SurfC0(Surf1, Surf2);
      SurfC1(Surf1, Surf2);
      SurfC2(Surf1, Surf2);
    }
    break;
    case GeomAbs_G1: {
      SurfC0(Surf1, Surf2);
      SurfG1(Surf1, Surf2);
    }
    break;
    case GeomAbs_G2: {
      SurfC0(Surf1, Surf2);
      SurfG1(Surf1, Surf2);
      if (myIsDone)
      {
        SurfG2(Surf1, Surf2);
      }
    }
    break;
    default: {
      myIsDone      = false;
      myErrorStatus = LocalAnalysis_InvalidInput;
    }
  }
}

/*********************************************************************************/

LocalAnalysis_SurfaceContinuity::LocalAnalysis_SurfaceContinuity(
  const occ::handle<Geom_Surface>& Surf1,
  const double                     u1,
  const double                     v1,
  const occ::handle<Geom_Surface>& Surf2,
  const double                     u2,
  const double                     v2,
  const GeomAbs_Shape              Ordre,
  const double                     EpsNul,
  const double                     EpsC0,
  const double                     EpsC1,
  const double                     EpsC2,
  const double                     EpsG1,
  const double                     Percent,
  const double                     Maxlen)
    : LocalAnalysis_SurfaceContinuity(EpsNul, EpsC0, EpsC1, EpsC2, EpsG1, Percent, Maxlen)
{
  if (Surf1.IsNull() || Surf2.IsNull())
  {
    return;
  }
  const int         aDerivativeOrder = derivativeOrder(Ordre);
  GeomLProp_SLProps aProps1(Surf1, u1, v1, aDerivativeOrder, myepsnul);
  GeomLProp_SLProps aProps2(Surf2, u2, v2, aDerivativeOrder, myepsnul);
  ComputeAnalysis(aProps1, aProps2, Ordre);
}

//=================================================================================================

LocalAnalysis_SurfaceContinuity::LocalAnalysis_SurfaceContinuity(
  const occ::handle<Geom2d_Curve>& curv1,
  const occ::handle<Geom2d_Curve>& curv2,
  const double                     U,
  const occ::handle<Geom_Surface>& Surf1,
  const occ::handle<Geom_Surface>& Surf2,
  const GeomAbs_Shape              Ordre,
  const double                     EpsNul,
  const double                     EpsC0,
  const double                     EpsC1,
  const double                     EpsC2,
  const double                     EpsG1,
  const double                     Percent,
  const double                     Maxlen)
    : LocalAnalysis_SurfaceContinuity(EpsNul, EpsC0, EpsC1, EpsC2, EpsG1, Percent, Maxlen)
{
  if (curv1.IsNull() || curv2.IsNull() || Surf1.IsNull() || Surf2.IsNull()
      || U < curv1->FirstParameter() || U > curv1->LastParameter() || U < curv2->FirstParameter()
      || U > curv2->LastParameter())
  {
    return;
  }
  const gp_Pnt2d    aUV1             = curv1->EvalD0(U);
  const gp_Pnt2d    aUV2             = curv2->EvalD0(U);
  const int         aDerivativeOrder = derivativeOrder(Ordre);
  GeomLProp_SLProps aProps1(Surf1, aUV1.X(), aUV1.Y(), aDerivativeOrder, myepsnul);
  GeomLProp_SLProps aProps2(Surf2, aUV2.X(), aUV2.Y(), aDerivativeOrder, myepsnul);
  ComputeAnalysis(aProps1, aProps2, Ordre);
}

//=================================================================================================

bool LocalAnalysis_SurfaceContinuity::IsC0() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return myContC0 <= myepsC0;
}

/*********************************************************************************/

bool LocalAnalysis_SurfaceContinuity::IsC1() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return IsC0() && (myContC1U <= myepsC1) && (myContC1V <= myepsC1);
}

/*********************************************************************************/

bool LocalAnalysis_SurfaceContinuity::IsC2() const
{
  double eps1u, eps1v, eps2u, eps2v;

  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  if (IsC1())
  {
    eps1u = 0.5 * myepsC1 * myepsC1 * myLambda1U;
    eps1v = 0.5 * myepsC1 * myepsC1 * myLambda1V;
    eps2u = 0.5 * myepsC2 * myepsC2 * myLambda2U;
    eps2v = 0.5 * myepsC2 * myepsC2 * myLambda2V;
    if ((myContC2U < myepsC2) && (myContC2V < myepsC2))
    {
      if (std::abs(myLambda1U * myLambda1U - myLambda2U) <= (eps1u * eps1u + eps2u))
      {
        if (std::abs(myLambda1V * myLambda1V - myLambda2V) <= (eps1v * eps1v + eps2v))
        {
          return true;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

/*********************************************************************************/

bool LocalAnalysis_SurfaceContinuity::IsG1() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return IsC0() && (myContG1 <= myepsG1);
}

/*********************************************************************************/

bool LocalAnalysis_SurfaceContinuity::IsG2() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  // Scale the relative tolerance by the largest absolute principal curvature
  // of either surface. The positional tolerance supplies an absolute floor
  // near flat regions: 8 * EpsC0 / Maxlen^2.
  const double aNullCurvature = 8.0 * myepsC0 / (mymaxlen * mymaxlen);
  return IsG1() && myGap <= std::max(aNullCurvature, myperce * myCurvatureScale);
}

/*********************************************************************************/

GeomAbs_Shape LocalAnalysis_SurfaceContinuity::ContinuityStatus() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myTypeCont);
}

/*********************************************************************************/

double LocalAnalysis_SurfaceContinuity::C0Value() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myContC0);
}

/*********************************************************************************/

double LocalAnalysis_SurfaceContinuity::C1UAngle() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myContC1U);
}

/*********************************************************************************/

double LocalAnalysis_SurfaceContinuity::C1VAngle() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myContC1V);
}

/*********************************************************************************/

double LocalAnalysis_SurfaceContinuity::C2UAngle() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myContC2U);
}

/*********************************************************************************/

double LocalAnalysis_SurfaceContinuity::C2VAngle() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myContC2V);
}

/*********************************************************************************/

double LocalAnalysis_SurfaceContinuity::G1Angle() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myContG1);
}

/*********************************************************************************/

double LocalAnalysis_SurfaceContinuity::C1URatio() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myLambda1U);
}

/*********************************************************************************/

double LocalAnalysis_SurfaceContinuity::C2URatio() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myLambda2U);
}

/*********************************************************************************/

double LocalAnalysis_SurfaceContinuity::C1VRatio() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myLambda1V);
}

/*********************************************************************************/

double LocalAnalysis_SurfaceContinuity::C2VRatio() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myLambda2V);
}

/*********************************************************************************/

double LocalAnalysis_SurfaceContinuity::G2CurvatureGap() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  return (myGap);
}

/*********************************************************************************/

bool LocalAnalysis_SurfaceContinuity::IsDone() const
{
  return (myIsDone);
}

/*********************************************************************************/
LocalAnalysis_StatusErrorType LocalAnalysis_SurfaceContinuity::StatusError() const
{
  return myErrorStatus;
}
