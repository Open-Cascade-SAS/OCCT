// Created on: 2002-04-30
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
