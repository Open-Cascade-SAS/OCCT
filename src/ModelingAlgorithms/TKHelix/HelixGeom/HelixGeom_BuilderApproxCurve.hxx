// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _HelixGeom_BuilderApproxCurve_HeaderFile
#define _HelixGeom_BuilderApproxCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_Sequence.hxx>

//! Base class for helix curve approximation algorithms.
//!
//! This abstract class provides common functionality for approximating
//! parametric helix curves using B-spline curves. It manages:
//! - Approximation tolerance and parameters
//! - Continuity requirements (C0, C1, C2)
//! - Maximum degree and number of segments
//! - Error and warning status reporting
//! - Result curve storage
//!
//! Derived classes must implement the Perform() method to execute
//! the specific approximation algorithm.
//!
//! @sa HelixGeom_BuilderHelixGen, HelixGeom_BuilderHelix, HelixGeom_BuilderHelixCoil
class HelixGeom_BuilderApproxCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Sets approximation parameters
  Standard_EXPORT void SetApproxParameters(const GeomAbs_Shape aCont,
                                           const int           aMaxDegree,
                                           const int           aMaxSeg);

  //! Gets approximation parameters
  Standard_EXPORT void ApproxParameters(GeomAbs_Shape& aCont, int& aMaxDegree, int& aMaxSeg) const;

  //! Sets approximation tolerance
  Standard_EXPORT void SetTolerance(const double aTolerance);

  //! Gets approximation tolerance
  Standard_EXPORT double Tolerance() const;

  //! Gets actual tolerance reached by approximation algorithm
  Standard_EXPORT double ToleranceReached() const;

  //! Gets sequence of BSpline curves representing helix coils
  Standard_EXPORT const NCollection_Sequence<occ::handle<Geom_Curve>>& Curves() const;

  //! Returns error status of algorithm
  Standard_EXPORT int ErrorStatus() const;

  //! Returns warning status of algorithm
  Standard_EXPORT int WarningStatus() const;

  //! Performs calculations.
  //! Must be redefined.
  Standard_EXPORT virtual void Perform() = 0;

protected:
  //! Sets default values of approximation parameters
  Standard_EXPORT HelixGeom_BuilderApproxCurve();
  Standard_EXPORT virtual ~HelixGeom_BuilderApproxCurve();

  int                                           myErrorStatus;
  int                                           myWarningStatus;
  double                                        myTolerance;
  GeomAbs_Shape                                 myCont;
  int                                           myMaxDegree;
  int                                           myMaxSeg;
  double                                        myTolReached;
  NCollection_Sequence<occ::handle<Geom_Curve>> myCurves;
};

#endif // _HelixGeom_BuilderApproxCurve_HeaderFile
