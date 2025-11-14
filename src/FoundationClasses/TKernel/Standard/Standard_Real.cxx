// Copyright (c) 1998-1999 Matra Datavision
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

#include <float.h>
#include <Standard_Real.hxx>

//==================================================================================================

inline double apx_for_ACosApprox(const double theValue)
{
  return (-0.000007239283986332
          + theValue
              * (2.000291665285952400
                 + theValue
                     * (0.163910606547823220
                        + theValue
                            * (0.047654245891495528
                               - theValue
                                   * (0.005516443930088506 + 0.015098965761299077 * theValue)))))
         / sqrt(2 * theValue);
}

//==================================================================================================

double ACosApprox(const double theValue)
{
  if (theValue < 0.)
  {
    const double theX = 1. + theValue;
    return theX < RealSmall() ? 0. : M_PI - apx_for_ACosApprox(theX);
  }
  else
  {
    const double theX = 1. - theValue;
    return theX < RealSmall() ? 0. : apx_for_ACosApprox(theX);
  }
}
