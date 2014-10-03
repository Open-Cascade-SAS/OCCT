// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <AndroidQt_TouchParameters.h>

// =======================================================================
// function : AndroidQt_TouchParameters
// purpose  :
// =======================================================================
AndroidQt_TouchParameters::AndroidQt_TouchParameters()
: myXStart (0.0),
  myXEnd   (0.0),
  myYStart (0.0),
  myYEnd   (0.0)
{
}

// =======================================================================
// function : AndroidQt_TouchParameters
// purpose  :
// =======================================================================
AndroidQt_TouchParameters::AndroidQt_TouchParameters (const double theX,
                                                      const double theY)
: myXStart (theX),
  myXEnd   (theX),
  myYStart (theY),
  myYEnd   (theY)
{
}

// =======================================================================
// function : X
// purpose  :
// =======================================================================
QPair<double, double> AndroidQt_TouchParameters::X() const
{
  return qMakePair(myXStart, myXEnd);
}

// =======================================================================
// function : DevX
// purpose  :
// =======================================================================
double AndroidQt_TouchParameters::DevX() const
{
  return myXEnd - myXStart;
}

// =======================================================================
// function : Y
// purpose  :
// =======================================================================
QPair<double, double> AndroidQt_TouchParameters::Y() const
{
  return qMakePair(myYStart, myYEnd);
}

// =======================================================================
// function : DevY
// purpose  :
// =======================================================================
double AndroidQt_TouchParameters::DevY() const
{
  return myYEnd - myYStart;
}

// =======================================================================
// function : SetStarts
// purpose  :
// =======================================================================
void AndroidQt_TouchParameters::SetStarts (const double theXStart,
                                           const double theYStart)
{
  myXStart = theXStart;
  myYStart = theYStart;
}

// =======================================================================
// function : SetEnds
// purpose  :
// =======================================================================
void AndroidQt_TouchParameters::SetEnds (const double theXEnd,
                                         const double theYEnd)
{
  myXEnd = theXEnd;
  myYEnd = theYEnd;
}

// =======================================================================
// function : ClearDev
// purpose  :
// =======================================================================
void AndroidQt_TouchParameters::ClearDev()
{
  myXStart = myXEnd;
  myYStart = myYEnd;
}
