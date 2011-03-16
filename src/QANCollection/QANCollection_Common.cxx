// File:        QANCollection_Common.cxx
// Created:     Tue Apr 30 09:44:26 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>
//
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
