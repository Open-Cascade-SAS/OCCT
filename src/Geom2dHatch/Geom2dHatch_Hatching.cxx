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


#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Hatching.hxx>
#include <gp_Pnt2d.hxx>
#include <HatchGen_Domain.hxx>
#include <HatchGen_PointOnElement.hxx>
#include <HatchGen_PointOnHatching.hxx>
#include <Standard_OutOfRange.hxx>

#define RAISE_IF_NOSUCHOBJECT 0

#include <Precision.hxx>
//=======================================================================
// Function : Geom2dHatch_Hatching
// Purpose  : Constructor.
//=======================================================================

Geom2dHatch_Hatching::Geom2dHatch_Hatching () {
}

//=======================================================================
// Function : Geom2dHatch_Hatching
// Purpose  : Constructor.
//=======================================================================

Geom2dHatch_Hatching::Geom2dHatch_Hatching (const Geom2dAdaptor_Curve& Curve) :
       myCurve      (Curve),
       myTrimDone   (Standard_False),
       myTrimFailed (Standard_False),
       myIsDone     (Standard_False),
       myStatus     (HatchGen_NoProblem)
{
}

//=======================================================================
// Function : Curve
// Purpose  : Returns the curve associated to the hatching.
//=======================================================================

const Geom2dAdaptor_Curve& Geom2dHatch_Hatching::Curve () const
{
  return myCurve ;
}
     
//=======================================================================
// Function : ChangeCurve
// Purpose  : Returns the curve associated to the hatching.
//=======================================================================

Geom2dAdaptor_Curve& Geom2dHatch_Hatching::ChangeCurve ()
{
  return myCurve ;
}
     
//=======================================================================
// Function : TrimDone
// Purpose  : Sets the flag about the trimmings computation to the given
//            value.
//=======================================================================

void Geom2dHatch_Hatching::TrimDone (const Standard_Boolean Flag)
{
  myTrimDone = Flag ;
}

//=======================================================================
// Function : TrimDone
// Purpose  : Returns the flag about the trimmings computation.
//=======================================================================

Standard_Boolean Geom2dHatch_Hatching::TrimDone () const
{
  return myTrimDone ;
}

//=======================================================================
// Function : TrimFailed
// Purpose  : Sets the flag about the trimmings failure to the given
//            value.
//=======================================================================

void Geom2dHatch_Hatching::TrimFailed (const Standard_Boolean Flag)
{
  myTrimFailed = Flag ;
  if (myTrimFailed) myStatus = HatchGen_TrimFailure ;
}

//=======================================================================
// Function : TrimFailed
// Purpose  : Returns the flag about the trimmings failure.
//=======================================================================

Standard_Boolean Geom2dHatch_Hatching::TrimFailed () const
{
  return myTrimFailed ;
}

//=======================================================================
// Function : IsDone
// Purpose  : Sets the flag about the domains computation to the given
//            value.
//=======================================================================

void Geom2dHatch_Hatching::IsDone (const Standard_Boolean Flag)
{
  myIsDone = Flag ;
}

//=======================================================================
// Function : IsDone
// Purpose  : Returns the flag about the domains computation.
//=======================================================================

Standard_Boolean Geom2dHatch_Hatching::IsDone () const
{
  return myIsDone ;
}

//=======================================================================
// Function : SetStatus
// Purpose  : Sets the error status.
//=======================================================================

void Geom2dHatch_Hatching::Status (const HatchGen_ErrorStatus theStatus)
{
  myStatus = theStatus;
}

//=======================================================================
// Function : Status
// Purpose  : Returns the error status.
//=======================================================================

HatchGen_ErrorStatus Geom2dHatch_Hatching::Status () const
{
  return myStatus ;
}

//=======================================================================
// Function : AddPoint
// Purpose  : Adds an intersection point to the hatching.
//=======================================================================

void Geom2dHatch_Hatching::AddPoint (const HatchGen_PointOnHatching& Point,
				     const Standard_Real Confusion)
{
  Standard_Integer NbPoints = myPoints.Length () ;
//for (Standard_Integer IPntH = 1 ; IPntH <= NbPoints ; IPntH++) {
  Standard_Integer IPntH;
  for (IPntH = 1 ; IPntH <= NbPoints ; IPntH++) {
    const HatchGen_PointOnHatching& PntH = myPoints.Value (IPntH) ;
    if (!PntH.IsLower (Point, Confusion)) break ;
  }
  if (IPntH > NbPoints) {
    myPoints.Append (Point) ;
  } else {
    HatchGen_PointOnHatching& PntH = myPoints.ChangeValue (IPntH) ;
    if (PntH.IsGreater (Point, Confusion)) {
      myPoints.InsertBefore (IPntH, Point) ;
    } else {
      for (Standard_Integer IPntE = 1 ; IPntE <= Point.NbPoints() ; IPntE++) {
	const HatchGen_PointOnElement& PntE = Point.Point (IPntE) ;
	PntH.AddPoint (PntE, Confusion) ;
      }
    }
  }
  if (myIsDone) ClrDomains() ;
}

//=======================================================================
// Function : NbPoints
// Purpose  : Returns the number of intersection points on the hatching.
//=======================================================================

Standard_Integer Geom2dHatch_Hatching::NbPoints () const
{
  return myPoints.Length () ;
}
     
//=======================================================================
// Function : Point
// Purpose  : Returns the Index-th intersection point on the hatching.
//=======================================================================

const HatchGen_PointOnHatching& Geom2dHatch_Hatching::Point (const Standard_Integer Index) const
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_Integer NbPoints = myPoints.Length () ;
  Standard_OutOfRange_Raise_if (Index < 1 || Index > NbPoints, "") ;
#endif
  const HatchGen_PointOnHatching& Point = myPoints.Value (Index) ;
  return Point ;
}
     
//=======================================================================
// Function : ChangePoint
// Purpose  : Returns the Index-th intersection point on the hatching.
//=======================================================================

HatchGen_PointOnHatching& Geom2dHatch_Hatching::ChangePoint (const Standard_Integer Index) 
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_Integer NbPoints = myPoints.Length () ;
  Standard_OutOfRange_Raise_if (Index < 1 || Index > NbPoints, "") ;
#endif
  HatchGen_PointOnHatching& Point = myPoints.ChangeValue (Index) ;
  return Point ;
}
     
//=======================================================================
// Function : RemPoint
// Purpose  : Removes the Index-th intersection point of the hatching.
//=======================================================================

void Geom2dHatch_Hatching::RemPoint (const Standard_Integer Index)
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_Integer NbPoints = myPoints.Length () ;
  Standard_OutOfRange_Raise_if (Index < 1 || Index > NbPoints, "") ;
#endif
  if (myIsDone) ClrDomains() ;
  myPoints.Remove (Index) ;
}

//=======================================================================
// Function : ClrPoints
// Purpose  : Removes all the intersection points of the hatching.
//=======================================================================

void Geom2dHatch_Hatching::ClrPoints ()
{
  if (myIsDone) ClrDomains() ;
  for (Standard_Integer IPntH = 1 ; IPntH <= myPoints.Length() ; IPntH++) {
    HatchGen_PointOnHatching& Point = myPoints.ChangeValue (IPntH) ;
    Point.ClrPoints() ;
  }
  myPoints.Clear () ;
  myTrimDone   = Standard_False ;
  myTrimFailed = Standard_False ;
}

//=======================================================================
// Function : AddDomain
// Purpose  : Adds a domain to the hatching.
//=======================================================================

void Geom2dHatch_Hatching::AddDomain (const HatchGen_Domain& Domain)
{
  myDomains.Append (Domain) ;
}

//=======================================================================
// Function : NbDomains
// Purpose  : Returns the number of domains on the hatching.
//=======================================================================

Standard_Integer Geom2dHatch_Hatching::NbDomains () const
{
  return myDomains.Length () ;
}
     
//=======================================================================
// Function : Domain
// Purpose  : Returns the Index-th domain on the hatching.
//=======================================================================

const HatchGen_Domain& Geom2dHatch_Hatching::Domain (const Standard_Integer Index) const
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_Integer NbDomains = myDomains.Length () ;
  Standard_OutOfRange_Raise_if (Index < 1 || Index > NbDomains, "") ;
#endif
  const HatchGen_Domain& Domain = myDomains.Value (Index) ;
  return Domain ;
}
     
//=======================================================================
// Function : RemDomain
// Purpose  : Removes the Index-th domain of the hatching.
//=======================================================================

void Geom2dHatch_Hatching::RemDomain (const Standard_Integer Index)
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_Integer NbDomains = myDomains.Length () ;
  Standard_OutOfRange_Raise_if (Index < 1 || Index > NbDomains, "") ;
#endif
  myDomains.Remove (Index) ;
}

//=======================================================================
// Function : ClrDomains
// Purpose  : Removes all the domains of the hatching.
//=======================================================================

void Geom2dHatch_Hatching::ClrDomains ()
{
  myDomains.Clear () ;
  myIsDone = Standard_False ;
}

//=======================================================================
// Function : ClassificationPoint
// Purpose  : returns a 2d point on the curve 
//=======================================================================
gp_Pnt2d Geom2dHatch_Hatching::ClassificationPoint () const { 
  Standard_Real t,a,b;
  a = myCurve.FirstParameter();
  b = myCurve.LastParameter();
  if(b >= Precision::Infinite()) { 
    if(a <= -Precision::Infinite()) { 
      t=0;
    }
    else { 
      t = a;
    }
  }
  else { 
    t = b; 
  }
  return(myCurve.Value(t));
}
    
