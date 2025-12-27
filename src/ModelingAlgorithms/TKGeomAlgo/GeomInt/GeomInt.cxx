
// Created on:   25.08.14 17:59:59
// Created by:    jgv@VIVEX
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

#include <GeomInt.hxx>

//=================================================================================================

bool GeomInt::AdjustPeriodic(const double thePar,
                                         const double theParMin,
                                         const double theParMax,
                                         const double thePeriod,
                                         double&      theNewPar,
                                         double&      theOffset,
                                         const double theEps)
{
  bool bMin, bMax;
  //
  theOffset = 0.;
  theNewPar = thePar;
  bMin      = theParMin - thePar > theEps;
  bMax      = thePar - theParMax > theEps;
  //
  if (bMin || bMax)
  {
    double dp, aNbPer;
    //
    dp = (bMin) ? (theParMax - thePar) : (theParMin - thePar);
    modf(dp / thePeriod, &aNbPer);
    //
    theOffset = aNbPer * thePeriod;
    theNewPar += theOffset;
  }
  //
  return (theOffset > 0.);
}
