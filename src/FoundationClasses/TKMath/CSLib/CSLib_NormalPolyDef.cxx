// Created on: 1996-08-23
// Created by: Benoit TANNIOU
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

#include <CSLib_NormalPolyDef.hxx>
#include <PLib.hxx>

#include <cmath>

//=================================================================================================

CSLib_NormalPolyDef::CSLib_NormalPolyDef(const int theK0, const TColStd_Array1OfReal& theLi)
    : myK0(theK0),
      myTABli(0, theK0)
{
  for (int i = 0; i <= theK0; ++i)
  {
    myTABli(i) = theLi(i);
  }
}

//=================================================================================================

bool CSLib_NormalPolyDef::Value(const double theX, double& theF)
{
  theF = 0.0;

  const double aCos = std::cos(theX);
  const double aSin = std::sin(theX);

  // At singular points (cos or sin near zero), return zero to avoid numerical instability.
  if (std::abs(aCos) <= RealSmall() || std::abs(aSin) <= RealSmall())
  {
    return true;
  }

  // Evaluate the polynomial: F(X) = Sum_{i=0}^{k0} C(k0,i) * cos^i(X) * sin^(k0-i)(X) * li(i)
  for (int i = 0; i <= myK0; ++i)
  {
    theF += PLib::Bin(myK0, i) * std::pow(aCos, i) * std::pow(aSin, myK0 - i) * myTABli(i);
  }

  return true;
}

//=================================================================================================

bool CSLib_NormalPolyDef::Derivative(const double theX, double& theD)
{
  theD = 0.0;

  const double aCos = std::cos(theX);
  const double aSin = std::sin(theX);

  // At singular points, return zero derivative.
  if (std::abs(aCos) <= RealSmall() || std::abs(aSin) <= RealSmall())
  {
    return true;
  }

  // Evaluate the derivative using the chain rule.
  // dF/dX = Sum_{i=0}^{k0} C(k0,i) * cos^(i-1)(X) * sin^(k0-i-1)(X) * (k0*cos^2(X) - i) * li(i)
  for (int i = 0; i <= myK0; ++i)
  {
    theD += PLib::Bin(myK0, i) * std::pow(aCos, i - 1) * std::pow(aSin, myK0 - i - 1)
            * (myK0 * aCos * aCos - i) * myTABli(i);
  }

  return true;
}

//=================================================================================================

bool CSLib_NormalPolyDef::Values(const double theX, double& theF, double& theD)
{
  theF = 0.0;
  theD = 0.0;

  const double aCos = std::cos(theX);
  const double aSin = std::sin(theX);

  // At singular points, return zeros.
  if (std::abs(aCos) <= RealSmall() || std::abs(aSin) <= RealSmall())
  {
    return true;
  }

  // Compute both function value and derivative in a single loop for efficiency.
  for (int i = 0; i <= myK0; ++i)
  {
    const double aBinCoeff = PLib::Bin(myK0, i);
    const double aLiCoeff  = myTABli(i);

    theF += aBinCoeff * std::pow(aCos, i) * std::pow(aSin, myK0 - i) * aLiCoeff;
    theD += aBinCoeff * std::pow(aCos, i - 1) * std::pow(aSin, myK0 - i - 1)
            * (myK0 * aCos * aCos - i) * aLiCoeff;
  }

  return true;
}
