// Created on: 1995-11-15
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Law_Interpolate_HeaderFile
#define _Law_Interpolate_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TColStd_Array1OfReal.hxx>
class Law_BSpline;

//! This  class   is used  to   interpolate a BsplineCurve
//! passing through    an  array of  points,   with   a C2
//! Continuity if tangency  is not requested at the point.
//! If tangency is  requested at the  point the continuity
//! will be C1.  If Perodicity is requested the curve will
//! be  closed  and the junction will  be  the first point
//! given. The curve will than be only C1
class Law_Interpolate
{
public:
  DEFINE_STANDARD_ALLOC

  //! Tolerance is to check if  the points are not too close
  //! to one an  other.  It is  also  used to check   if the
  //! tangent vector  is not too small.   There should be at
  //! least 2 points. If PeriodicFlag is True then the curve
  //! will be periodic be periodic
  Standard_EXPORT Law_Interpolate(const Handle(TColStd_HArray1OfReal)& Points,
                                  const Standard_Boolean               PeriodicFlag,
                                  const Standard_Real                  Tolerance);

  //! Tolerance is to check if  the points are not too close
  //! to one an  other.  It is  also  used to check   if the
  //! tangent vector  is not too small.   There should be at
  //! least 2 points. If PeriodicFlag is True then the curve
  //! will be periodic be periodic
  Standard_EXPORT Law_Interpolate(const Handle(TColStd_HArray1OfReal)& Points,
                                  const Handle(TColStd_HArray1OfReal)& Parameters,
                                  const Standard_Boolean               PeriodicFlag,
                                  const Standard_Real                  Tolerance);

  //! loads initial and final tangents if any.
  Standard_EXPORT void Load(const Standard_Real InitialTangent, const Standard_Real FinalTangent);

  //! loads the tangents. We should have as many tangents as
  //! they are points  in the array if TangentFlags.Value(i)
  //! is    Standard_True  use the tangent Tangents.Value(i)
  //! otherwise the tangent is not constrained.
  Standard_EXPORT void Load(const TColStd_Array1OfReal&             Tangents,
                            const Handle(TColStd_HArray1OfBoolean)& TangentFlags);

  //! Clears the tangents if any
  Standard_EXPORT void ClearTangents();

  //! Makes the interpolation
  Standard_EXPORT void Perform();

  Standard_EXPORT const Handle(Law_BSpline)& Curve() const;

  Standard_EXPORT Standard_Boolean IsDone() const;

protected:
private:
  //! Interpolates in a non periodic fashion.
  Standard_EXPORT void PerformNonPeriodic();

  //! Interpolates in a C1 periodic fashion.
  Standard_EXPORT void PerformPeriodic();

  Standard_Real                    myTolerance;
  Handle(TColStd_HArray1OfReal)    myPoints;
  Standard_Boolean                 myIsDone;
  Handle(Law_BSpline)              myCurve;
  Handle(TColStd_HArray1OfReal)    myTangents;
  Handle(TColStd_HArray1OfBoolean) myTangentFlags;
  Handle(TColStd_HArray1OfReal)    myParameters;
  Standard_Boolean                 myPeriodic;
  Standard_Boolean                 myTangentRequest;
};

#endif // _Law_Interpolate_HeaderFile
