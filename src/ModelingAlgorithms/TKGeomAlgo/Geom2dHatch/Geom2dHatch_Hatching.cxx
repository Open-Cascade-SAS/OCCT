// Created on: 1993-11-03
// Created by: Jean Marc LACHAUME
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Geom2dHatch_Hatching.hxx>
#include <gp_Pnt2d.hxx>
#include <HatchGen_Domain.hxx>
#include <HatchGen_PointOnElement.hxx>
#include <HatchGen_PointOnHatching.hxx>

#define RAISE_IF_NOSUCHOBJECT 0

#include <Precision.hxx>

//=================================================================================================

Geom2dHatch_Hatching::Geom2dHatch_Hatching()
    : myTrimDone(false),
      myTrimFailed(false),
      myIsDone(false),
      myStatus(HatchGen_NoProblem)
{
}

//=================================================================================================

Geom2dHatch_Hatching::Geom2dHatch_Hatching(const Geom2dAdaptor_Curve& Curve)
    : myCurve(Curve),
      myTrimDone(false),
      myTrimFailed(false),
      myIsDone(false),
      myStatus(HatchGen_NoProblem)
{
}

//=======================================================================
// Function : Curve
// Purpose  : Returns the curve associated to the hatching.
//=======================================================================

const Geom2dAdaptor_Curve& Geom2dHatch_Hatching::Curve() const
{
  return myCurve;
}

//=======================================================================
// Function : ChangeCurve
// Purpose  : Returns the curve associated to the hatching.
//=======================================================================

Geom2dAdaptor_Curve& Geom2dHatch_Hatching::ChangeCurve()
{
  return myCurve;
}

//=======================================================================
// Function : TrimDone
// Purpose  : Sets the flag about the trimmings computation to the given
//            value.
//=======================================================================

void Geom2dHatch_Hatching::TrimDone(const bool Flag)
{
  myTrimDone = Flag;
}

//=======================================================================
// Function : TrimDone
// Purpose  : Returns the flag about the trimmings computation.
//=======================================================================

bool Geom2dHatch_Hatching::TrimDone() const
{
  return myTrimDone;
}

//=======================================================================
// Function : TrimFailed
// Purpose  : Sets the flag about the trimmings failure to the given
//            value.
//=======================================================================

void Geom2dHatch_Hatching::TrimFailed(const bool Flag)
{
  myTrimFailed = Flag;
  if (myTrimFailed)
    myStatus = HatchGen_TrimFailure;
}

//=======================================================================
// Function : TrimFailed
// Purpose  : Returns the flag about the trimmings failure.
//=======================================================================

bool Geom2dHatch_Hatching::TrimFailed() const
{
  return myTrimFailed;
}

//=======================================================================
// Function : IsDone
// Purpose  : Sets the flag about the domains computation to the given
//            value.
//=======================================================================

void Geom2dHatch_Hatching::IsDone(const bool Flag)
{
  myIsDone = Flag;
}

//=======================================================================
// Function : IsDone
// Purpose  : Returns the flag about the domains computation.
//=======================================================================

bool Geom2dHatch_Hatching::IsDone() const
{
  return myIsDone;
}

//=======================================================================
// Function : SetStatus
// Purpose  : Sets the error status.
//=======================================================================

void Geom2dHatch_Hatching::Status(const HatchGen_ErrorStatus theStatus)
{
  myStatus = theStatus;
}

//=======================================================================
// Function : Status
// Purpose  : Returns the error status.
//=======================================================================

HatchGen_ErrorStatus Geom2dHatch_Hatching::Status() const
{
  return myStatus;
}

//=======================================================================
// Function : AddPoint
// Purpose  : Adds an intersection point to the hatching.
//=======================================================================

void Geom2dHatch_Hatching::AddPoint(const HatchGen_PointOnHatching& Point,
                                    const double             Confusion)
{
  int NbPoints = myPoints.Length();
  // for (int IPntH = 1 ; IPntH <= NbPoints ; IPntH++) {
  int IPntH;
  for (IPntH = 1; IPntH <= NbPoints; IPntH++)
  {
    const HatchGen_PointOnHatching& PntH = myPoints.Value(IPntH);
    if (!PntH.IsLower(Point, Confusion))
      break;
  }
  if (IPntH > NbPoints)
  {
    myPoints.Append(Point);
  }
  else
  {
    HatchGen_PointOnHatching& PntH = myPoints.ChangeValue(IPntH);
    if (PntH.IsGreater(Point, Confusion))
    {
      myPoints.InsertBefore(IPntH, Point);
    }
    else
    {
      for (int IPntE = 1; IPntE <= Point.NbPoints(); IPntE++)
      {
        const HatchGen_PointOnElement& PntE = Point.Point(IPntE);
        PntH.AddPoint(PntE, Confusion);
      }
    }
  }
  if (myIsDone)
    ClrDomains();
}

//=======================================================================
// Function : NbPoints
// Purpose  : Returns the number of intersection points on the hatching.
//=======================================================================

int Geom2dHatch_Hatching::NbPoints() const
{
  return myPoints.Length();
}

//=======================================================================
// Function : Point
// Purpose  : Returns the Index-th intersection point on the hatching.
//=======================================================================

const HatchGen_PointOnHatching& Geom2dHatch_Hatching::Point(const int Index) const
{
#if RAISE_IF_NOSUCHOBJECT
  int NbPoints = myPoints.Length();
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbPoints, "");
#endif
  const HatchGen_PointOnHatching& Point = myPoints.Value(Index);
  return Point;
}

//=======================================================================
// Function : ChangePoint
// Purpose  : Returns the Index-th intersection point on the hatching.
//=======================================================================

HatchGen_PointOnHatching& Geom2dHatch_Hatching::ChangePoint(const int Index)
{
#if RAISE_IF_NOSUCHOBJECT
  int NbPoints = myPoints.Length();
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbPoints, "");
#endif
  HatchGen_PointOnHatching& Point = myPoints.ChangeValue(Index);
  return Point;
}

//=======================================================================
// Function : RemPoint
// Purpose  : Removes the Index-th intersection point of the hatching.
//=======================================================================

void Geom2dHatch_Hatching::RemPoint(const int Index)
{
#if RAISE_IF_NOSUCHOBJECT
  int NbPoints = myPoints.Length();
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbPoints, "");
#endif
  if (myIsDone)
    ClrDomains();
  myPoints.Remove(Index);
}

//=======================================================================
// Function : ClrPoints
// Purpose  : Removes all the intersection points of the hatching.
//=======================================================================

void Geom2dHatch_Hatching::ClrPoints()
{
  if (myIsDone)
    ClrDomains();
  for (int IPntH = 1; IPntH <= myPoints.Length(); IPntH++)
  {
    HatchGen_PointOnHatching& Point = myPoints.ChangeValue(IPntH);
    Point.ClrPoints();
  }
  myPoints.Clear();
  myTrimDone   = false;
  myTrimFailed = false;
}

//=======================================================================
// Function : AddDomain
// Purpose  : Adds a domain to the hatching.
//=======================================================================

void Geom2dHatch_Hatching::AddDomain(const HatchGen_Domain& Domain)
{
  myDomains.Append(Domain);
}

//=======================================================================
// Function : NbDomains
// Purpose  : Returns the number of domains on the hatching.
//=======================================================================

int Geom2dHatch_Hatching::NbDomains() const
{
  return myDomains.Length();
}

//=======================================================================
// Function : Domain
// Purpose  : Returns the Index-th domain on the hatching.
//=======================================================================

const HatchGen_Domain& Geom2dHatch_Hatching::Domain(const int Index) const
{
#if RAISE_IF_NOSUCHOBJECT
  int NbDomains = myDomains.Length();
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbDomains, "");
#endif
  const HatchGen_Domain& Domain = myDomains.Value(Index);
  return Domain;
}

//=======================================================================
// Function : RemDomain
// Purpose  : Removes the Index-th domain of the hatching.
//=======================================================================

void Geom2dHatch_Hatching::RemDomain(const int Index)
{
#if RAISE_IF_NOSUCHOBJECT
  int NbDomains = myDomains.Length();
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbDomains, "");
#endif
  myDomains.Remove(Index);
}

//=======================================================================
// Function : ClrDomains
// Purpose  : Removes all the domains of the hatching.
//=======================================================================

void Geom2dHatch_Hatching::ClrDomains()
{
  myDomains.Clear();
  myIsDone = false;
}

//=======================================================================
// Function : ClassificationPoint
// Purpose  : returns a 2d point on the curve
//=======================================================================
gp_Pnt2d Geom2dHatch_Hatching::ClassificationPoint() const
{
  double t, a, b;
  a = myCurve.FirstParameter();
  b = myCurve.LastParameter();
  if (b >= Precision::Infinite())
  {
    if (a <= -Precision::Infinite())
    {
      t = 0;
    }
    else
    {
      t = a;
    }
  }
  else
  {
    t = b;
  }
  return (myCurve.Value(t));
}
