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

CSLib_NormalPolyDef::CSLib_NormalPolyDef(const Standard_Integer k0, const TColStd_Array1OfReal& li)
    : myK0(k0),
      myTABli(0, k0)
{
  for (Standard_Integer i = 0; i <= k0; i++)
  {
    myTABli(i) = li(i);
  }
}

//=================================================================================================

Standard_Boolean CSLib_NormalPolyDef::Value(const Standard_Real X, Standard_Real& F)
{
  F                      = 0.0;
  const Standard_Real co = std::cos(X);
  const Standard_Real si = std::sin(X);

  if (std::abs(co) <= RealSmall() || std::abs(si) <= RealSmall())
  {
    return Standard_True;
  }
  for (Standard_Integer i = 0; i <= myK0; i++)
  {
    F += PLib::Bin(myK0, i) * std::pow(co, i) * std::pow(si, myK0 - i) * myTABli(i);
  }
  return Standard_True;
}

//=================================================================================================

Standard_Boolean CSLib_NormalPolyDef::Derivative(const Standard_Real X, Standard_Real& D)
{
  D                      = 0.0;
  const Standard_Real co = std::cos(X);
  const Standard_Real si = std::sin(X);
  if (std::abs(co) <= RealSmall() || std::abs(si) <= RealSmall())
  {
    return Standard_True;
  }
  for (Standard_Integer i = 0; i <= myK0; i++)
  {
    D += PLib::Bin(myK0, i) * std::pow(co, i - 1) * std::pow(si, myK0 - i - 1)
         * (myK0 * co * co - i) * myTABli(i);
  }
  return Standard_True;
}

//=================================================================================================

Standard_Boolean CSLib_NormalPolyDef::Values(const Standard_Real X,
                                             Standard_Real&      F,
                                             Standard_Real&      D)
{
  F                      = 0.0;
  D                      = 0.0;
  const Standard_Real co = std::cos(X);
  const Standard_Real si = std::sin(X);
  if (std::abs(co) <= RealSmall() || std::abs(si) <= RealSmall())
  {
    return Standard_True;
  }
  for (Standard_Integer i = 0; i <= myK0; i++)
  {
    const Standard_Real aBin   = PLib::Bin(myK0, i);
    const Standard_Real aCoeff = myTABli(i);
    F += aBin * std::pow(co, i) * std::pow(si, myK0 - i) * aCoeff;
    D += aBin * std::pow(co, i - 1) * std::pow(si, myK0 - i - 1) * (myK0 * co * co - i) * aCoeff;
  }
  return Standard_True;
}
