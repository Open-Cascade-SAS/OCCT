// Created on: 1993-11-10
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

#ifndef _Geom2dHatch_Hatching_HeaderFile
#define _Geom2dHatch_Hatching_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <Standard_Boolean.hxx>
#include <HatchGen_PointsOnHatching.hxx>
#include <HatchGen_ErrorStatus.hxx>
#include <HatchGen_Domains.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>

class gp_Pnt2d;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class Geom2dHatch_Hatching 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dHatch_Hatching();
  
  //! Creates a hatching.
  Standard_EXPORT Geom2dHatch_Hatching(const Geom2dAdaptor_Curve& Curve);
  
  //! Returns the curve associated to the hatching.
  Standard_EXPORT const Geom2dAdaptor_Curve& Curve() const;
  
  //! Returns the curve associated to the hatching.
  Standard_EXPORT Geom2dAdaptor_Curve& ChangeCurve();
  
  //! Sets the flag about the trimming computations to the
  //! given value.
  Standard_EXPORT void TrimDone (const Standard_Boolean Flag);
  
  //! Returns the flag about the trimming computations.
  Standard_EXPORT Standard_Boolean TrimDone() const;
  
  //! Sets the flag about the trimming failure to the
  //! given value.
  Standard_EXPORT void TrimFailed (const Standard_Boolean Flag);
  
  //! Returns the flag about the trimming failure.
  Standard_EXPORT Standard_Boolean TrimFailed() const;
  
  //! Sets the flag about the domains computation to the
  //! given value.
  Standard_EXPORT void IsDone (const Standard_Boolean Flag);
  
  //! Returns the flag about the domains computation.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Sets the error status.
  Standard_EXPORT void Status (const HatchGen_ErrorStatus theStatus);
  
  //! Returns the error status.
  Standard_EXPORT HatchGen_ErrorStatus Status() const;
  
  //! Adds an intersection point to the hatching.
  Standard_EXPORT void AddPoint (const HatchGen_PointOnHatching& Point, const Standard_Real Confusion);
  
  //! Returns the number of intersection points
  //! of the hatching.
  Standard_EXPORT Standard_Integer NbPoints() const;
  
  //! Returns the Index-th intersection point of the
  //! hatching.
  //! The exception OutOfRange is raised if
  //! Index < 1 or Index > NbPoints.
  Standard_EXPORT const HatchGen_PointOnHatching& Point (const Standard_Integer Index) const;
  
  //! Returns the Index-th intersection point of the
  //! hatching.
  //! The exception OutOfRange is raised if
  //! Index < 1 or Index > NbPoints.
  Standard_EXPORT HatchGen_PointOnHatching& ChangePoint (const Standard_Integer Index);
  
  //! Removes the Index-th intersection point of the
  //! hatching.
  //! The exception OutOfRange is raised if
  //! Index < 1 or Index > NbPoints.
  Standard_EXPORT void RemPoint (const Standard_Integer Index);
  
  //! Removes all the intersection points of the hatching.
  Standard_EXPORT void ClrPoints();
  
  //! Adds a domain to the hatching.
  Standard_EXPORT void AddDomain (const HatchGen_Domain& Domain);
  
  //! Returns the number of domains of the hatching.
  Standard_EXPORT Standard_Integer NbDomains() const;
  
  //! Returns the Index-th domain of the hatching.
  //! The exception OutOfRange is raised if
  //! Index < 1 or Index > NbDomains.
  Standard_EXPORT const HatchGen_Domain& Domain (const Standard_Integer Index) const;
  
  //! Removes the Index-th domain of the hatching.
  //! The exception OutOfRange is raised if
  //! Index < 1 or Index > NbDomains.
  Standard_EXPORT void RemDomain (const Standard_Integer Index);
  
  //! Removes all the domains of the hatching.
  Standard_EXPORT void ClrDomains();
  
  //! Returns a point on the curve.
  //! This point will be used for the classification.
  Standard_EXPORT gp_Pnt2d ClassificationPoint() const;




protected:





private:



  Geom2dAdaptor_Curve myCurve;
  Standard_Boolean myTrimDone;
  Standard_Boolean myTrimFailed;
  HatchGen_PointsOnHatching myPoints;
  Standard_Boolean myIsDone;
  HatchGen_ErrorStatus myStatus;
  HatchGen_Domains myDomains;


};







#endif // _Geom2dHatch_Hatching_HeaderFile
