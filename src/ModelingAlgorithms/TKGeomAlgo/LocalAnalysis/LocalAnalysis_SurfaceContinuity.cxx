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
#include <GeomProp_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <StdFail_NotDone.hxx>

/*********************************************************************************/
/*********************************************************************************/
void LocalAnalysis_SurfaceContinuity::SurfC0(const gp_Pnt& theP1, const gp_Pnt& theP2)
{
  myContC0 = theP1.Distance(theP2);
}

/*********************************************************************************/

void LocalAnalysis_SurfaceContinuity::SurfC1(const gp_Vec& theD1U1,
                                             const gp_Vec& theD1V1,
                                             const gp_Vec& theD1U2,
                                             const gp_Vec& theD1V2)
{
  double norm1u = theD1U1.Magnitude();
  double norm2u = theD1U2.Magnitude();
  double norm1v = theD1V1.Magnitude();
  double norm2v = theD1V2.Magnitude();

  if ((norm1u > myepsnul) && (norm2u > myepsnul) && (norm1v > myepsnul) && (norm2v > myepsnul))
  {
    if (norm1u >= norm2u)
      myLambda1U = norm2u / norm1u;
    else
      myLambda1U = norm1u / norm2u;
    if (norm1v >= norm2v)
      myLambda1V = norm2v / norm1v;
    else
      myLambda1V = norm1v / norm2v;
    double angu = theD1U1.Angle(theD1U2);
    if (angu > M_PI / 2)
      myContC1U = M_PI - angu;
    else
      myContC1U = angu;
    double angv = theD1V1.Angle(theD1V2);
    if (angv > M_PI / 2)
      myContC1V = M_PI - angv;
    else
      myContC1V = angv;
  }
  else
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_NullFirstDerivative;
  }
}

/*********************************************************************************/

void LocalAnalysis_SurfaceContinuity::SurfC2(const gp_Vec& theD1U1,
                                             const gp_Vec& theD1V1,
                                             const gp_Vec& theD2U1,
                                             const gp_Vec& theD2V1,
                                             const gp_Vec& theD1U2,
                                             const gp_Vec& theD1V2,
                                             const gp_Vec& theD2U2,
                                             const gp_Vec& theD2V2)
{
  double norm11u = theD1U1.Magnitude();
  double norm12u = theD1U2.Magnitude();
  double norm21u = theD2U1.Magnitude();
  double norm22u = theD2U2.Magnitude();

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
      double ang = theD2U1.Angle(theD2U2);
      if (ang > M_PI / 2)
        myContC2U = M_PI - ang;
      else
        myContC2U = ang;
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

  double norm11v = theD1V1.Magnitude();
  double norm12v = theD1V2.Magnitude();
  double norm21v = theD2V1.Magnitude();
  double norm22v = theD2V2.Magnitude();

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
      double ang = theD2V1.Angle(theD2V2);
      if (ang > M_PI / 2)
        myContC2V = M_PI - ang;
      else
        myContC2V = ang;
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
void LocalAnalysis_SurfaceContinuity::SurfG1(const gp_Dir& theNorm1,
                                             bool          theNormDef1,
                                             const gp_Dir& theNorm2,
                                             bool          theNormDef2)
{
  if (theNormDef1 && theNormDef2)
  {
    double ang = theNorm1.Angle(theNorm2);
    if (ang > M_PI / 2)
      myContG1 = M_PI - ang;
    else
      myContG1 = ang;
  }
  else
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_NormalNotDefined;
  }
}

/*********************************************************************************/

void LocalAnalysis_SurfaceContinuity::SurfG2(const GeomProp::SurfaceCurvatureResult& theCurv1,
                                             const GeomProp::SurfaceCurvatureResult& theCurv2)
{
  if (theCurv1.IsDefined && theCurv2.IsDefined)
  {
    double x1, x2, y1, y2, z1, z2;
    theCurv1.MinDirection.Coord(x1, y1, z1);
    theCurv1.MaxDirection.Coord(x2, y2, z2);
    gp_Dir MCD1((std::abs(x1) + std::abs(x2)) / 2,
                (std::abs(y1) + std::abs(y2)) / 2,
                (std::abs(z1) + std::abs(z2)) / 2);
    theCurv2.MinDirection.Coord(x1, y1, z1);
    theCurv2.MaxDirection.Coord(x2, y2, z2);
    gp_Dir MCD2((std::abs(x1) + std::abs(x2)) / 2,
                (std::abs(y1) + std::abs(y2)) / 2,
                (std::abs(z1) + std::abs(z2)) / 2);

    myAlpha       = MCD1.Angle(MCD2);
    double RMIN1  = theCurv1.MinCurvature;
    double RMAX1  = theCurv1.MaxCurvature;
    double RMIN2  = theCurv2.MinCurvature;
    double RMAX2  = theCurv2.MaxCurvature;
    myETA1        = (RMIN1 + RMAX1) / 2;
    myETA2        = (RMIN2 + RMAX2) / 2;
    myETA         = (myETA1 + myETA2) / 2;
    myZETA1       = (RMAX1 - RMIN1) / 2;
    myZETA2       = (RMAX2 - RMIN2) / 2;
    myZETA        = (myZETA1 + myZETA2) / 2;
    double DETA   = (myETA1 - myETA2) / 2;
    double DZETA  = (myZETA1 - myZETA2) / 2;
    myGap = std::abs(DETA)
            + sqrt(DZETA * DZETA * std::cos(myAlpha) * std::cos(myAlpha)
                   + myZETA * myZETA * std::sin(myAlpha) * std::sin(myAlpha));
  }
  else
  {
    myIsDone      = false;
    myErrorStatus = LocalAnalysis_CurvatureNotDefined;
  }
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
      myETA1(0.0),
      myETA2(0.0),
      myETA(0.0),
      myZETA1(0.0),
      myZETA2(0.0),
      myZETA(0.0),
      myAlpha(0.0),
      myGap(0.0)
{
  myepsnul = EpsNul;
  myepsC0  = EpsC0;
  myepsC1  = EpsC1;
  myepsC2  = EpsC2;
  myepsG1  = EpsG1;
  myperce  = Percent;
  mymaxlen = Maxlen;
  myIsDone = true;
}

void LocalAnalysis_SurfaceContinuity::ComputeAnalysis(const occ::handle<Geom_Surface>& theSurf1,
                                                      double                           theU1,
                                                      double                           theV1,
                                                      const occ::handle<Geom_Surface>& theSurf2,
                                                      double                           theU2,
                                                      double                           theV2,
                                                      const GeomAbs_Shape              Order)
{
  myTypeCont = Order;
  switch (Order)
  {
    case GeomAbs_C0: {
      gp_Pnt P1, P2;
      theSurf1->D0(theU1, theV1, P1);
      theSurf2->D0(theU2, theV2, P2);
      SurfC0(P1, P2);
    }
    break;
    case GeomAbs_C1: {
      gp_Pnt P1, P2;
      gp_Vec D1U1, D1V1, D1U2, D1V2;
      theSurf1->D1(theU1, theV1, P1, D1U1, D1V1);
      theSurf2->D1(theU2, theV2, P2, D1U2, D1V2);
      SurfC0(P1, P2);
      SurfC1(D1U1, D1V1, D1U2, D1V2);
    }
    break;
    case GeomAbs_C2: {
      gp_Pnt P1, P2;
      gp_Vec D1U1, D1V1, D2U1, D2V1, D2UV1;
      gp_Vec D1U2, D1V2, D2U2, D2V2, D2UV2;
      theSurf1->D2(theU1, theV1, P1, D1U1, D1V1, D2U1, D2V1, D2UV1);
      theSurf2->D2(theU2, theV2, P2, D1U2, D1V2, D2U2, D2V2, D2UV2);
      SurfC0(P1, P2);
      SurfC1(D1U1, D1V1, D1U2, D1V2);
      SurfC2(D1U1, D1V1, D2U1, D2V1, D1U2, D1V2, D2U2, D2V2);
    }
    break;
    case GeomAbs_G1: {
      gp_Pnt P1, P2;
      theSurf1->D0(theU1, theV1, P1);
      theSurf2->D0(theU2, theV2, P2);
      SurfC0(P1, P2);
      GeomProp_Surface aProp1(theSurf1);
      GeomProp_Surface aProp2(theSurf2);
      GeomProp::SurfaceNormalResult aNorm1 = aProp1.Normal(theU1, theV1, myepsnul);
      GeomProp::SurfaceNormalResult aNorm2 = aProp2.Normal(theU2, theV2, myepsnul);
      SurfG1(aNorm1.Direction, aNorm1.IsDefined, aNorm2.Direction, aNorm2.IsDefined);
    }
    break;
    case GeomAbs_G2: {
      gp_Pnt P1, P2;
      theSurf1->D0(theU1, theV1, P1);
      theSurf2->D0(theU2, theV2, P2);
      SurfC0(P1, P2);
      GeomProp_Surface aProp1(theSurf1);
      GeomProp_Surface aProp2(theSurf2);
      GeomProp::SurfaceNormalResult aNorm1 = aProp1.Normal(theU1, theV1, myepsnul);
      GeomProp::SurfaceNormalResult aNorm2 = aProp2.Normal(theU2, theV2, myepsnul);
      SurfG1(aNorm1.Direction, aNorm1.IsDefined, aNorm2.Direction, aNorm2.IsDefined);
      GeomProp::SurfaceCurvatureResult aCurv1 = aProp1.Curvatures(theU1, theV1, myepsnul);
      GeomProp::SurfaceCurvatureResult aCurv2 = aProp2.Curvatures(theU2, theV2, myepsnul);
      SurfG2(aCurv1, aCurv2);
    }
    break;
    default: {
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
      myETA1(0.0),
      myETA2(0.0),
      myETA(0.0),
      myZETA1(0.0),
      myZETA2(0.0),
      myZETA(0.0),
      myAlpha(0.0),
      myGap(0.0)
{
  myTypeCont = Ordre;
  myepsnul   = EpsNul;
  myepsC0    = EpsC0;
  myepsC1    = EpsC1;
  myepsC2    = EpsC2;
  myepsG1    = EpsG1;
  myperce    = Percent;
  mymaxlen   = Maxlen;
  myIsDone   = true;
  ComputeAnalysis(Surf1, u1, v1, Surf2, u2, v2, Ordre);
}

/*********************************************************************************/

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
      myETA1(0.0),
      myETA2(0.0),
      myETA(0.0),
      myZETA1(0.0),
      myZETA2(0.0),
      myZETA(0.0),
      myAlpha(0.0),
      myGap(0.0)
{
  double pard1, parf1, pard2, parf2, u1, v1, u2, v2;

  myTypeCont = Ordre;
  myepsnul   = EpsNul;
  myepsC0    = EpsC0;
  myepsC1    = EpsC1;
  myepsC2    = EpsC2;
  myepsG1    = EpsG1;
  myperce    = Percent;
  mymaxlen   = Maxlen;
  myIsDone   = true;

  pard1 = curv1->FirstParameter();
  pard2 = curv2->FirstParameter();
  parf1 = curv1->LastParameter();
  parf2 = curv2->LastParameter();

  if ((U > parf1) || (U < pard1) || (U > parf2) || (U < pard2))
    myIsDone = false;
  else
  {
    gp_Pnt2d pt1 = curv1->Value(U);
    gp_Pnt2d pt2 = curv2->Value(U);

    pt1.Coord(u1, v1);
    pt2.Coord(u2, v2);
    ComputeAnalysis(Surf1, u1, v1, Surf2, u2, v2, Ordre);
  }
}

/*********************************************************************************/

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
        if (std::abs(myLambda1V * myLambda1V - myLambda2V) <= (eps1v * eps1v + eps2v))
          return true;
        else
          return false;
      else
        return false;
    }
    else
      return false;
  }
  else
    return false;
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
  double EPSNL;
  int    itype;

  if (!myIsDone)
  {
    throw StdFail_NotDone();
  }
  itype = 0;
  EPSNL = 8 * myepsC0 / (mymaxlen * mymaxlen);
  if (IsG1())
  {
    if ((std::abs(myETA) < EPSNL) && (std::abs(myZETA) < EPSNL))
      return true;
    if ((std::abs(myZETA1) < EPSNL) && (std::abs(myZETA2) < EPSNL))
      itype = 1;
    else if ((std::abs(myETA1) < EPSNL) && (std::abs(myETA2) < EPSNL))
      itype = 1;
    else if ((std::abs(std::abs(myZETA) - std::abs(myETA))) < EPSNL)
      itype = 1;
    else if ((myETA1 < myZETA1) && (myETA2 < myZETA2))
      itype = 1;
    else if ((myETA1 > myZETA1) && (myETA2 > myZETA2))
      itype = 1;
    if (itype == 1)
    {

      if ((myETA >= (2 * myZETA)) && (myGap <= (myperce * (myETA - myZETA))))
        return true;
      if ((myZETA >= myETA) && (myGap <= (myperce * myZETA)))
        return true;
      return (myZETA <= myETA) && (myETA <= (2 * myZETA)) && (myGap <= (myperce * myETA));
    }
    else
      return false;
  }
  else
    return false;
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
