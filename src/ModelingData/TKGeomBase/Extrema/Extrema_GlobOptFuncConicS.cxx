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

#include <Extrema_GlobOptFuncConicS.hxx>

#include <gp_Pnt.hxx>
#include <ElCLib.hxx>

// F(u, v) = Conic.SquareDistance(myS(u, v))

//=================================================================================================

void Extrema_GlobOptFuncConicS::value(double su, double sv, double& F)
{
  double ct;
  gp_Pnt        aPS = myS->Value(su, sv);
  switch (myCType)
  {
    case GeomAbs_Line:
      ct = ElCLib::Parameter(myLin, aPS);
      break;
    case GeomAbs_Circle:
      ct = ElCLib::Parameter(myCirc, aPS);
      break;
    case GeomAbs_Ellipse:
      ct = ElCLib::Parameter(myElips, aPS);
      break;
    case GeomAbs_Hyperbola:
      ct = ElCLib::Parameter(myHypr, aPS);
      break;
    case GeomAbs_Parabola:
      ct = ElCLib::Parameter(myParab, aPS);
      break;
    default:
      F = Precision::Infinite();
      return;
  }
  //
  if (myCType == GeomAbs_Circle || myCType == GeomAbs_Ellipse)
  {
    if (myTl > 2. * M_PI + Precision::PConfusion())
    {
      ct += 2. * M_PI;
    }
  }
  F = RealLast();
  if (ct >= myTf && ct <= myTl)
  {
    gp_Pnt aPC = myC->Value(ct);
    F          = std::min(F, aPS.SquareDistance(aPC));
  }
  F = std::min(F, aPS.SquareDistance(myCPf));
  F = std::min(F, aPS.SquareDistance(myCPl));
}

//=================================================================================================

bool Extrema_GlobOptFuncConicS::checkInputData(const math_Vector& X,
                                                           double&     su,
                                                           double&     sv)
{
  int aStartIndex = X.Lower();
  su                           = X(aStartIndex);
  sv                           = X(aStartIndex + 1);

  if (su < myUf || su > myUl || sv < myVf || sv > myVl)
  {
    return false;
  }
  return true;
}

//=================================================================================================

Extrema_GlobOptFuncConicS::Extrema_GlobOptFuncConicS(const Adaptor3d_Surface* S,
                                                     const double      theUf,
                                                     const double      theUl,
                                                     const double      theVf,
                                                     const double      theVl)
    : myS(S),
      myUf(theUf),
      myUl(theUl),
      myVf(theVf),
      myVl(theVl)
{
}

//=================================================================================================

Extrema_GlobOptFuncConicS::Extrema_GlobOptFuncConicS(const Adaptor3d_Surface* S)
    : myS(S),
      myUf(S->FirstUParameter()),
      myUl(S->LastUParameter()),
      myVf(S->FirstVParameter()),
      myVl(S->LastVParameter())
{
}

//=================================================================================================

Extrema_GlobOptFuncConicS::Extrema_GlobOptFuncConicS(const Adaptor3d_Curve*   C,
                                                     const Adaptor3d_Surface* S)
    : myS(S),
      myUf(S->FirstUParameter()),
      myUl(S->LastUParameter()),
      myVf(S->FirstVParameter()),
      myVl(S->LastVParameter())
{
  double aCTf = C->FirstParameter();
  double aCTl = C->LastParameter();
  LoadConic(C, aCTf, aCTl);
}

//=================================================================================================

void Extrema_GlobOptFuncConicS::LoadConic(const Adaptor3d_Curve* C,
                                          const double    theTf,
                                          const double    theTl)
{
  myC  = C;
  myTf = theTf;
  myTl = theTl;
  if (myC->IsPeriodic())
  {
    constexpr double aTMax = 2. * M_PI + Precision::PConfusion();
    if (myTf > aTMax || myTf < -Precision::PConfusion() || std::abs(myTl - myTf) > aTMax)
    {
      ElCLib::AdjustPeriodic(0.,
                             2. * M_PI,
                             std::min(std::abs(myTl - myTf) / 2, Precision::PConfusion()),
                             myTf,
                             myTl);
    }
  }
  myCPf   = myC->Value(myTf);
  myCPl   = myC->Value(myTl);
  myCType = myC->GetType();
  switch (myCType)
  {
    case GeomAbs_Line:
      myLin = myC->Line();
      break;
    case GeomAbs_Circle:
      myCirc = myC->Circle();
      break;
    case GeomAbs_Ellipse:
      myElips = myC->Ellipse();
      break;
    case GeomAbs_Hyperbola:
      myHypr = myC->Hyperbola();
      break;
    case GeomAbs_Parabola:
      myParab = myC->Parabola();
      break;
    default:
      break;
  }
}

//=================================================================================================

int Extrema_GlobOptFuncConicS::NbVariables() const
{
  return 2;
}

//=================================================================================================

bool Extrema_GlobOptFuncConicS::Value(const math_Vector& X, double& F)
{
  double su, sv;
  if (!checkInputData(X, su, sv))
    return false;

  value(su, sv, F);
  if (Precision::IsInfinite(F))
  {
    return false;
  }
  return true;
}

//=================================================================================================

double Extrema_GlobOptFuncConicS::ConicParameter(const math_Vector& theUV) const
{
  double ct;
  gp_Pnt        aPS = myS->Value(theUV(1), theUV(2));
  switch (myCType)
  {
    case GeomAbs_Line:
      ct = ElCLib::Parameter(myLin, aPS);
      break;
    case GeomAbs_Circle:
      ct = ElCLib::Parameter(myCirc, aPS);
      break;
    case GeomAbs_Ellipse:
      ct = ElCLib::Parameter(myElips, aPS);
      break;
    case GeomAbs_Hyperbola:
      ct = ElCLib::Parameter(myHypr, aPS);
      break;
    case GeomAbs_Parabola:
      ct = ElCLib::Parameter(myParab, aPS);
      break;
    default:
      ct = myTf;
      return ct;
  }
  //
  if (myCType == GeomAbs_Circle || myCType == GeomAbs_Ellipse)
  {
    if (myTl > 2. * M_PI + Precision::PConfusion())
    {
      ct += 2. * M_PI;
    }
  }
  double F = RealLast();
  if (ct >= myTf && ct <= myTl)
  {
    gp_Pnt aPC = myC->Value(ct);
    F          = std::min(F, aPS.SquareDistance(aPC));
  }
  double Fext = aPS.SquareDistance(myCPf);
  if (Fext < F)
  {
    F  = Fext;
    ct = myTf;
  }
  Fext = aPS.SquareDistance(myCPl);
  if (Fext < F)
  {
    F  = Fext;
    ct = myTl;
  }
  return ct;
}