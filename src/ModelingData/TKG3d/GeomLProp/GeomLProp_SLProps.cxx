// Created on: 1992-03-26
// Created by: Herve LEGRAND
// Copyright (c) 1992-1999 Matra Datavision
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

#include <GeomLProp_SLProps.hxx>

#include <Geom_Surface.hxx>
#include <GeomProp.hxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Standard_OutOfRange.hxx>

static const double MinStep = 1.0e-7;

static bool IsTangentDefined(GeomLProp_SLProps& SProp,
                             const int          cn,
                             const double       linTol,
                             const int          Derivative,
                             int&               Order,
                             LProp_Status&      theStatus)
{
  double Tol = linTol * linTol;
  gp_Vec V[2];
  Order = 0;

  while (Order < 3)
  {
    Order++;
    if (cn >= Order)
    {
      switch (Order)
      {
        case 1:
          V[0] = SProp.D1U();
          V[1] = SProp.D1V();
          break;
        case 2:
          V[0] = SProp.D2U();
          V[1] = SProp.D2V();
          break;
      }

      if (V[Derivative].SquareMagnitude() > Tol)
      {
        theStatus = LProp_Defined;
        return true;
      }
    }
    else
    {
      theStatus = LProp_Undefined;
      return false;
    }
  }

  return false;
}

//=================================================================================================

GeomLProp_SLProps::GeomLProp_SLProps(const occ::handle<Geom_Surface>& S,
                                     const double                     U,
                                     const double                     V,
                                     const int                        N,
                                     const double                     Resolution)
    : mySurf(S),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "GeomLProp_SLProps::GeomLProp_SLProps()");

  SetParameters(U, V);
}

//=================================================================================================

GeomLProp_SLProps::GeomLProp_SLProps(const occ::handle<Geom_Surface>& S,
                                     const int                        N,
                                     const double                     Resolution)
    : mySurf(S),
      myU(RealLast()),
      myV(RealLast()),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myUTangentStatus(LProp_Undecided),
      myVTangentStatus(LProp_Undecided),
      myNormalStatus(LProp_Undecided),
      myCurvatureStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "GeomLProp_SLProps::GeomLProp_SLProps()");
}

//=================================================================================================

GeomLProp_SLProps::GeomLProp_SLProps(const int N, const double Resolution)
    : myU(RealLast()),
      myV(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myUTangentStatus(LProp_Undecided),
      myVTangentStatus(LProp_Undecided),
      myNormalStatus(LProp_Undecided),
      myCurvatureStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "GeomLProp_SLProps::GeomLProp_SLProps() bad level");
}

//=================================================================================================

void GeomLProp_SLProps::SetSurface(const occ::handle<Geom_Surface>& S)
{
  mySurf = S;
  myCN   = 4;
}

//=================================================================================================

void GeomLProp_SLProps::SetParameters(const double U, const double V)
{
  myU = U;
  myV = V;
  switch (myDerOrder)
  {
    case 0:
      myPnt = mySurf->Value(myU, myV);
      break;
    case 1:
      mySurf->D1(myU, myV, myPnt, myD1u, myD1v);
      break;
    case 2:
      mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
      break;
  }

  myUTangentStatus  = LProp_Undecided;
  myVTangentStatus  = LProp_Undecided;
  myNormalStatus    = LProp_Undecided;
  myCurvatureStatus = LProp_Undecided;
}

//=================================================================================================

const gp_Pnt& GeomLProp_SLProps::Value() const
{
  return myPnt;
}

//=================================================================================================

const gp_Vec& GeomLProp_SLProps::D1U()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    mySurf->D1(myU, myV, myPnt, myD1u, myD1v);
  }

  return myD1u;
}

//=================================================================================================

const gp_Vec& GeomLProp_SLProps::D1V()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    mySurf->D1(myU, myV, myPnt, myD1u, myD1v);
  }

  return myD1v;
}

//=================================================================================================

const gp_Vec& GeomLProp_SLProps::D2U()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  return myD2u;
}

//=================================================================================================

const gp_Vec& GeomLProp_SLProps::D2V()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  return myD2v;
}

//=================================================================================================

const gp_Vec& GeomLProp_SLProps::DUV()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  return myDuv;
}

//=================================================================================================

bool GeomLProp_SLProps::IsTangentUDefined()
{
  if (myUTangentStatus == LProp_Undefined)
    return false;
  else if (myUTangentStatus >= LProp_Defined)
    return true;

  return ::IsTangentDefined(*this,
                            myCN,
                            myLinTol,
                            0,
                            mySignificantFirstDerivativeOrderU,
                            myUTangentStatus);
}

//=================================================================================================

void GeomLProp_SLProps::TangentU(gp_Dir& D)
{
  if (!IsTangentUDefined())
    throw LProp_NotDefined();

  if (mySignificantFirstDerivativeOrderU == 1)
    D = gp_Dir(myD1u);
  else
  {
    const double DivisionFactor = 1.e-3;
    double       anUfirst, anVfirst, anUlast, anVlast;
    mySurf->Bounds(anUfirst, anUlast, anVfirst, anVlast);
    double du;
    if ((anUlast >= RealLast()) || (anUfirst <= RealFirst()))
      du = 0.0;
    else
      du = anUlast - anUfirst;

    const double aDeltaU = std::max(du * DivisionFactor, MinStep);

    gp_Vec V = myD2u;

    double u;

    if (myU - anUfirst < aDeltaU)
      u = myU + aDeltaU;
    else
      u = myU - aDeltaU;

    gp_Pnt P1 = mySurf->Value(std::min(myU, u), myV);
    gp_Pnt P2 = mySurf->Value(std::max(myU, u), myV);

    gp_Vec V1(P1, P2);
    double aDirFactor = V.Dot(V1);

    if (aDirFactor < 0.0)
      V = -V;

    D = gp_Dir(V);
  }
}

//=================================================================================================

bool GeomLProp_SLProps::IsTangentVDefined()
{
  if (myVTangentStatus == LProp_Undefined)
    return false;
  else if (myVTangentStatus >= LProp_Defined)
    return true;

  return ::IsTangentDefined(*this,
                            myCN,
                            myLinTol,
                            1,
                            mySignificantFirstDerivativeOrderV,
                            myVTangentStatus);
}

//=================================================================================================

void GeomLProp_SLProps::TangentV(gp_Dir& D)
{
  if (!IsTangentVDefined())
    throw LProp_NotDefined();

  if (mySignificantFirstDerivativeOrderV == 1)
    D = gp_Dir(myD1v);
  else
  {
    const double DivisionFactor = 1.e-3;
    double       anUfirst, anVfirst, anUlast, anVlast;
    mySurf->Bounds(anUfirst, anUlast, anVfirst, anVlast);

    double dv;
    if ((anVlast >= RealLast()) || (anVfirst <= RealFirst()))
      dv = 0.0;
    else
      dv = anVlast - anVfirst;

    const double aDeltaV = std::max(dv * DivisionFactor, MinStep);

    gp_Vec V = myD2v;

    double v;

    if (myV - anVfirst < aDeltaV)
      v = myV + aDeltaV;
    else
      v = myV - aDeltaV;

    gp_Pnt P1 = mySurf->Value(myU, std::min(myV, v));
    gp_Pnt P2 = mySurf->Value(myU, std::max(myV, v));

    gp_Vec V1(P1, P2);
    double aDirFactor = V.Dot(V1);

    if (aDirFactor < 0.0)
      V = -V;

    D = gp_Dir(V);
  }
}

//=================================================================================================

bool GeomLProp_SLProps::IsNormalDefined()
{
  if (myNormalStatus == LProp_Undefined)
    return false;
  else if (myNormalStatus >= LProp_Defined)
    return true;

  GeomProp::SurfaceNormalResult aResult = GeomProp::ComputeSurfaceNormal(myD1u, myD1v, myLinTol);
  if (aResult.IsDefined)
  {
    myNormal       = aResult.Direction;
    myNormalStatus = LProp_Computed;
    return true;
  }

  myNormalStatus = LProp_Undefined;
  return false;
}

//=================================================================================================

const gp_Dir& GeomLProp_SLProps::Normal()
{
  if (!IsNormalDefined())
  {
    throw LProp_NotDefined();
  }

  return myNormal;
}

//=================================================================================================

bool GeomLProp_SLProps::IsCurvatureDefined()
{
  if (myCurvatureStatus == LProp_Undefined)
    return false;
  else if (myCurvatureStatus >= LProp_Defined)
    return true;

  if (myCN < 2)
  {
    myCurvatureStatus = LProp_Undefined;
    return false;
  }

  if (!IsNormalDefined())
  {
    myCurvatureStatus = LProp_Undefined;
    return false;
  }

  if (!IsTangentUDefined() || !IsTangentVDefined())
  {
    myCurvatureStatus = LProp_Undefined;
    return false;
  }

  // Ensure second derivatives are computed.
  if (myDerOrder < 2)
    this->D2U();

  // Use pre-computed normal to avoid redundant normal computation.
  GeomProp::SurfaceCurvatureResult aCurvResult =
    GeomProp::ComputeSurfaceCurvatures(myNormal, myD1u, myD1v, myD2u, myD2v, myDuv, myLinTol);

  if (!aCurvResult.IsDefined)
  {
    myCurvatureStatus = LProp_Undefined;
    return false;
  }

  myMinCurv    = aCurvResult.MinCurvature;
  myMaxCurv    = aCurvResult.MaxCurvature;
  myDirMinCurv = aCurvResult.MinDirection;
  myDirMaxCurv = aCurvResult.MaxDirection;
  myMeanCurv   = aCurvResult.MeanCurvature;
  myGausCurv   = aCurvResult.GaussianCurvature;

  myCurvatureStatus = LProp_Computed;
  return true;
}

//=================================================================================================

bool GeomLProp_SLProps::IsUmbilic()
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();

  return std::abs(myMaxCurv - myMinCurv) < std::abs(Epsilon(myMaxCurv));
}

//=================================================================================================

double GeomLProp_SLProps::MaxCurvature()
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();

  return myMaxCurv;
}

//=================================================================================================

double GeomLProp_SLProps::MinCurvature()
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();

  return myMinCurv;
}

//=================================================================================================

void GeomLProp_SLProps::CurvatureDirections(gp_Dir& Max, gp_Dir& Min)
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();

  Max = myDirMaxCurv;
  Min = myDirMinCurv;
}

//=================================================================================================

double GeomLProp_SLProps::MeanCurvature()
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();

  return myMeanCurv;
}

//=================================================================================================

double GeomLProp_SLProps::GaussianCurvature()
{
  if (!IsCurvatureDefined())
    throw LProp_NotDefined();

  return myGausCurv;
}
