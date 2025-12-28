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

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class Law_BSpline;

//! This class is used to interpolate a BsplineCurve
//! passing through an array of points, with a C2
//! Continuity if tangency is not requested at the point.
//! If tangency is requested at the point the continuity
//! will be C1. If Perodicity is requested the curve will
//! be closed and the junction will be the first point
//! given. The curve will than be only C1
class Law_Interpolate
{
public:
  DEFINE_STANDARD_ALLOC

  //! Tolerance is to check if the points are not too close
  //! to one an other. It is also used to check if the
  //! tangent vector is not too small. There should be at
  //! least 2 points. If PeriodicFlag is True then the curve
  //! will be periodic be periodic
  Standard_EXPORT Law_Interpolate(const occ::handle<NCollection_HArray1<double>>& Points,
                                  const bool                                      PeriodicFlag,
                                  const double                                    Tolerance);

  //! Tolerance is to check if the points are not too close
  //! to one an other. It is also used to check if the
  //! tangent vector is not too small. There should be at
  //! least 2 points. If PeriodicFlag is True then the curve
  //! will be periodic be periodic
  Standard_EXPORT Law_Interpolate(const occ::handle<NCollection_HArray1<double>>& Points,
                                  const occ::handle<NCollection_HArray1<double>>& Parameters,
                                  const bool                                      PeriodicFlag,
                                  const double                                    Tolerance);

  //! loads initial and final tangents if any.
  Standard_EXPORT void Load(const double InitialTangent, const double FinalTangent);

  //! loads the tangents. We should have as many tangents as
  //! they are points in the array if TangentFlags.Value(i)
  //! is true use the tangent Tangents.Value(i)
  //! otherwise the tangent is not constrained.
  Standard_EXPORT void Load(const NCollection_Array1<double>&             Tangents,
                            const occ::handle<NCollection_HArray1<bool>>& TangentFlags);

  //! Clears the tangents if any
  Standard_EXPORT void ClearTangents();

  //! Makes the interpolation
  Standard_EXPORT void Perform();

  Standard_EXPORT const occ::handle<Law_BSpline>& Curve() const;

  Standard_EXPORT bool IsDone() const;

private:
  //! Interpolates in a non periodic fashion.
  Standard_EXPORT void PerformNonPeriodic();

  //! Interpolates in a C1 periodic fashion.
  Standard_EXPORT void PerformPeriodic();

  double                                   myTolerance;
  occ::handle<NCollection_HArray1<double>> myPoints;
  bool                                     myIsDone;
  occ::handle<Law_BSpline>                 myCurve;
  occ::handle<NCollection_HArray1<double>> myTangents;
  occ::handle<NCollection_HArray1<bool>>   myTangentFlags;
  occ::handle<NCollection_HArray1<double>> myParameters;
  bool                                     myPeriodic;
  bool                                     myTangentRequest;
};

#endif // _Law_Interpolate_HeaderFile
