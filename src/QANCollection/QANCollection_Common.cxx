// Created on: 2002-04-30
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Purpose:     To test all methods of all NCollection classes

#include <QANCollection_Common.hxx>
#include <stdio.h>

void PrintItem(const gp_Pnt& thePnt)
{
  printf ("   (%5.1f %5.1f %5.1f)\n", thePnt.X(), thePnt.Y(), thePnt.Z());
}

void PrintItem(const Standard_Real theDbl)
{
  printf ("   (%5.1f)\n", theDbl);
}

void Random (Standard_Real& theValue)
{
  static Standard_Real dfV=0.14159265358979323846;
  dfV *= 37.;
  dfV -= Floor(dfV);
  theValue = dfV;
  //theValue=drand48();
}

void Random (Standard_Integer& theValue,
             const Standard_Integer theMax)
{
  Standard_Real dfR;
  Random(dfR);
  theValue = RealToInt(theMax*dfR);
}

void Random (gp_Pnt& thePnt)
{
  // thePnt.SetCoord(drand48(),drand48(),drand48());
  Standard_Real dfX, dfY, dfZ;
  Random(dfX);
  Random(dfY);
  Random(dfZ);
  thePnt.SetCoord(dfX,dfY,dfZ);
}
