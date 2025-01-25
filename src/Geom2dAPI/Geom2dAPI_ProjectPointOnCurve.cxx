// Created on: 1994-03-23
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Extrema_ExtPC2d.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

Geom2dAPI_ProjectPointOnCurve::Geom2dAPI_ProjectPointOnCurve()
    : myIndex(-1)
{
  myIsDone = Standard_False;
}

//=================================================================================================

Geom2dAPI_ProjectPointOnCurve::Geom2dAPI_ProjectPointOnCurve(const gp_Pnt2d&             P,
                                                             const Handle(Geom2d_Curve)& Curve)
{
  Init(P, Curve);
}

//=================================================================================================

Geom2dAPI_ProjectPointOnCurve::Geom2dAPI_ProjectPointOnCurve(const gp_Pnt2d&             P,
                                                             const Handle(Geom2d_Curve)& Curve,
                                                             const Standard_Real         Umin,
                                                             const Standard_Real         Usup)
{
  Init(P, Curve, Umin, Usup);
}

//=================================================================================================

void Geom2dAPI_ProjectPointOnCurve::Init(const gp_Pnt2d& P, const Handle(Geom2d_Curve)& Curve)
{
  Init(P, Curve, Curve->FirstParameter(), Curve->LastParameter());
}

//=================================================================================================

void Geom2dAPI_ProjectPointOnCurve::Init(const gp_Pnt2d&             P,
                                         const Handle(Geom2d_Curve)& Curve,
                                         const Standard_Real         Umin,
                                         const Standard_Real         Usup)
{
  myC.Load(Curve, Umin, Usup);

  Extrema_ExtPC2d theExtPC2d(P, myC);

  myExtPC = theExtPC2d;

  myIsDone = myExtPC.IsDone() && (myExtPC.NbExt() > 0);

  // evaluate the lower distance and its index;

  if (myIsDone)
  {
    Standard_Real Dist2, Dist2Min = myExtPC.SquareDistance(1);
    myIndex = 1;

    for (Standard_Integer i = 2; i <= myExtPC.NbExt(); i++)
    {
      Dist2 = myExtPC.SquareDistance(i);
      if (Dist2 < Dist2Min)
      {
        Dist2Min = Dist2;
        myIndex  = i;
      }
    }
  }
}

//=================================================================================================

Standard_Integer Geom2dAPI_ProjectPointOnCurve::NbPoints() const
{
  if (myIsDone)
    return myExtPC.NbExt();
  else
    return 0;
}

//=================================================================================================

gp_Pnt2d Geom2dAPI_ProjectPointOnCurve::Point(const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbPoints(),
                               "Geom2dAPI_ProjectPointOnCurve::Point");
  return (myExtPC.Point(Index)).Value();
}

//=================================================================================================

Standard_Real Geom2dAPI_ProjectPointOnCurve::Parameter(const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbPoints(),
                               "Geom2dAPI_ProjectPointOnCurve::Parameter");
  return (myExtPC.Point(Index)).Parameter();
}

//=================================================================================================

void Geom2dAPI_ProjectPointOnCurve::Parameter(const Standard_Integer Index, Standard_Real& U) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbPoints(),
                               "Geom2dAPI_ProjectPointOnCurve::Parameter");
  U = (myExtPC.Point(Index)).Parameter();
}

//=================================================================================================

Standard_Real Geom2dAPI_ProjectPointOnCurve::Distance(const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbPoints(),
                               "Geom2dAPI_ProjectPointOnCurve::Distance");
  return sqrt(myExtPC.SquareDistance(Index));
}

//=================================================================================================

gp_Pnt2d Geom2dAPI_ProjectPointOnCurve::NearestPoint() const
{
  StdFail_NotDone_Raise_if(!myIsDone, "Geom2dAPI_ProjectPointOnCurve:NearestPoint");

  return (myExtPC.Point(myIndex)).Value();
}

//=================================================================================================

Geom2dAPI_ProjectPointOnCurve::operator Standard_Integer() const
{
  return NbPoints();
}

//=================================================================================================

Geom2dAPI_ProjectPointOnCurve::operator gp_Pnt2d() const
{
  return NearestPoint();
}

//=================================================================================================

Standard_Real Geom2dAPI_ProjectPointOnCurve::LowerDistanceParameter() const
{
  StdFail_NotDone_Raise_if(!myIsDone, "Geom2dAPI_ProjectPointOnCurve:LowerDistanceParameter");

  return (myExtPC.Point(myIndex)).Parameter();
}

//=================================================================================================

Standard_Real Geom2dAPI_ProjectPointOnCurve::LowerDistance() const
{
  StdFail_NotDone_Raise_if(!myIsDone, "Geom2dAPI_ProjectPointOnCurve:LowerDistance");

  return sqrt(myExtPC.SquareDistance(myIndex));
}

//=================================================================================================

Geom2dAPI_ProjectPointOnCurve::operator Standard_Real() const
{
  return LowerDistance();
}
