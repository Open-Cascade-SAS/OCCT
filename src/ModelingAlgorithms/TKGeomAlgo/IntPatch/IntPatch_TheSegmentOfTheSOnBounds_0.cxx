// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
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

#include <IntPatch_TheSegmentOfTheSOnBounds.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Standard_DomainError.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <IntPatch_ThePathPointOfTheSOnBounds.hxx>

IntPatch_TheSegmentOfTheSOnBounds::IntPatch_TheSegmentOfTheSOnBounds()
    : hasfp(Standard_False),
      haslp(Standard_False)
{
}

void IntPatch_TheSegmentOfTheSOnBounds::SetLimitPoint(const IntPatch_ThePathPointOfTheSOnBounds& V,
                                                      const Standard_Boolean First)
{
  if (First)
  {
    hasfp = Standard_True;
    thefp = V;
  }
  else
  {
    haslp = Standard_True;
    thelp = V;
  }
}
