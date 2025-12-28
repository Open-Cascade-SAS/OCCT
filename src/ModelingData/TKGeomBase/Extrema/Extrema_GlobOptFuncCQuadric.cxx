// Copyright (c) 2020 OPEN CASCADE SAS
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
// commercial license or contractual agreement

#include <Extrema_GlobOptFuncCQuadric.hxx>

#include <gp_Pnt.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>

//=================================================================================================

void Extrema_GlobOptFuncCQuadric::value(double ct, double& F)
{
  double u, v;
  //
  gp_Pnt aCP = myC->Value(ct);
  switch (mySType)
  {
    case GeomAbs_Plane:
      ElSLib::Parameters(myPln, aCP, u, v);
      break;
    case GeomAbs_Cylinder:
      ElSLib::Parameters(myCylinder, aCP, u, v);
      break;
    case GeomAbs_Cone:
      ElSLib::Parameters(myCone, aCP, u, v);
      break;
    case GeomAbs_Sphere:
      ElSLib::Parameters(mySphere, aCP, u, v);
      break;
    case GeomAbs_Torus:
      ElSLib::Parameters(myTorus, aCP, u, v);
      break;
    default:
      F = Precision::Infinite();
      return;
  }
  //
  if (mySType != GeomAbs_Plane)
  {
    if (myUl > 2. * M_PI + Precision::PConfusion())
    {
      u += 2. * M_PI;
    }
  }
  if (mySType == GeomAbs_Torus)
  {
    if (myVl > 2. * M_PI + Precision::PConfusion())
    {
      v += 2. * M_PI;
    }
  }

  F = RealLast();
  if (u >= myUf && u <= myUl && v >= myVf && v <= myVl)
  {
    gp_Pnt aPS = myS->Value(u, v);
    F          = std::min(F, aCP.SquareDistance(aPS));
  }
  int i;
  for (i = 0; i < 4; ++i)
  {
    F = std::min(F, aCP.SquareDistance(myPTrim[i]));
  }
}

//=================================================================================================

bool Extrema_GlobOptFuncCQuadric::checkInputData(const math_Vector& X, double& ct)
{
  ct = X(X.Lower());

  if (ct < myTf || ct > myTl)
  {
    return false;
  }
  return true;
}

//=================================================================================================

Extrema_GlobOptFuncCQuadric::Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve*   C,
                                                         const Adaptor3d_Surface* S)
    : myC(C)
{
  myTf        = myC->FirstParameter();
  myTl        = myC->LastParameter();
  double anUf = S->FirstUParameter(), anUl = S->LastUParameter();
  double aVf = S->FirstVParameter(), aVl = S->LastVParameter();
  LoadQuad(S, anUf, anUl, aVf, aVl);
}

//=================================================================================================

Extrema_GlobOptFuncCQuadric::Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve* C)
    : myC(C)
{
  myTf = myC->FirstParameter();
  myTl = myC->LastParameter();
}

//=================================================================================================

Extrema_GlobOptFuncCQuadric::Extrema_GlobOptFuncCQuadric(const Adaptor3d_Curve* C,
                                                         const double           theTf,
                                                         const double           theTl)
    : myC(C),
      myTf(theTf),
      myTl(theTl)
{
}

//=================================================================================================

void Extrema_GlobOptFuncCQuadric::LoadQuad(const Adaptor3d_Surface* S,
                                           const double             theUf,
                                           const double             theUl,
                                           const double             theVf,
                                           const double             theVl)
{
  myS  = S;
  myUf = theUf;
  myUl = theUl;
  myVf = theVf;
  myVl = theVl;
  //
  if (myS->IsUPeriodic())
  {
    constexpr double aTMax = 2. * M_PI + Precision::PConfusion();
    if (myUf > aTMax || myUf < -Precision::PConfusion() || std::abs(myUl - myUf) > aTMax)
    {
      ElCLib::AdjustPeriodic(0.,
                             2. * M_PI,
                             std::min(std::abs(myUl - myUf) / 2, Precision::PConfusion()),
                             myUf,
                             myUl);
    }
  }
  if (myS->IsVPeriodic())
  {
    constexpr double aTMax = 2. * M_PI + Precision::PConfusion();
    if (myVf > aTMax || myVf < -Precision::PConfusion() || std::abs(myVl - myVf) > aTMax)
    {
      ElCLib::AdjustPeriodic(0.,
                             2. * M_PI,
                             std::min(std::abs(myVl - myVf) / 2, Precision::PConfusion()),
                             myVf,
                             myVl);
    }
  }
  myPTrim[0] = myS->Value(myUf, myVf);
  myPTrim[1] = myS->Value(myUl, myVf);
  myPTrim[2] = myS->Value(myUl, myVl);
  myPTrim[3] = myS->Value(myUf, myVl);
  mySType    = S->GetType();
  switch (mySType)
  {
    case GeomAbs_Plane:
      myPln = myS->Plane();
      break;
    case GeomAbs_Cylinder:
      myCylinder = myS->Cylinder();
      break;
    case GeomAbs_Cone:
      myCone = myS->Cone();
      break;
    case GeomAbs_Sphere:
      mySphere = myS->Sphere();
      break;
    case GeomAbs_Torus:
      myTorus = myS->Torus();
      break;
    default:
      break;
  }
}

//=================================================================================================

int Extrema_GlobOptFuncCQuadric::NbVariables() const
{
  return 1;
}

//=================================================================================================

bool Extrema_GlobOptFuncCQuadric::Value(const math_Vector& X, double& F)
{
  double ct;
  if (!checkInputData(X, ct))
    return false;

  value(ct, F);
  if (Precision::IsInfinite(F))
  {
    return false;
  }
  return true;
}

//=================================================================================================

void Extrema_GlobOptFuncCQuadric::QuadricParameters(const math_Vector& theCT,
                                                    math_Vector&       theUV) const
{
  double u, v;
  //
  // Arrays of extremity points parameters correspond to array of corner
  // points  myPTrim[]
  double uext[4] = {myUf, myUl, myUl, myUf};
  double vext[4] = {myVf, myVf, myVl, myVl};
  gp_Pnt aCP     = myC->Value(theCT(1));
  switch (mySType)
  {
    case GeomAbs_Plane:
      ElSLib::Parameters(myPln, aCP, u, v);
      break;
    case GeomAbs_Cylinder:
      ElSLib::Parameters(myCylinder, aCP, u, v);
      break;
    case GeomAbs_Cone:
      ElSLib::Parameters(myCone, aCP, u, v);
      break;
    case GeomAbs_Sphere:
      ElSLib::Parameters(mySphere, aCP, u, v);
      break;
    case GeomAbs_Torus:
      ElSLib::Parameters(myTorus, aCP, u, v);
      break;
    default:
      theUV(1) = myUf;
      theUV(2) = myUl;
      return;
  }
  //
  if (mySType != GeomAbs_Plane)
  {
    if (myUl > 2. * M_PI + Precision::PConfusion())
    {
      u += 2. * M_PI;
    }
  }
  if (mySType == GeomAbs_Torus)
  {
    if (myVl > 2. * M_PI + Precision::PConfusion())
    {
      v += 2. * M_PI;
    }
  }

  double F = RealLast();
  if (u >= myUf && u <= myUl && v >= myVf && v <= myVl)
  {
    gp_Pnt aPS = myS->Value(u, v);
    F          = aCP.SquareDistance(aPS);
  }
  int i;
  for (i = 0; i < 4; ++i)
  {
    double Fi = aCP.SquareDistance(myPTrim[i]);
    if (Fi < F)
    {
      F = Fi;
      u = uext[i];
      v = vext[i];
    }
  }
  theUV(1) = u;
  theUV(2) = v;
}
