// Created on: 1996-09-23
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _GeomConvert_CompCurveToBSplineCurve_HeaderFile
#define _GeomConvert_CompCurveToBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Convert_ParameterisationType.hxx>
#include <Standard_Integer.hxx>
class Geom_BSplineCurve;
class Geom_BoundedCurve;

//! Algorithm converts and concat several curve in an BSplineCurve
class GeomConvert_CompCurveToBSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initialize the algorithm
  //! - Parameterisation is used to convert
  Standard_EXPORT GeomConvert_CompCurveToBSplineCurve(
    const Convert_ParameterisationType Parameterisation = Convert_TgtThetaOver2);

  //! Initialize the algorithm with one curve
  //! - Parameterisation is used to convert
  Standard_EXPORT GeomConvert_CompCurveToBSplineCurve(
    const occ::handle<Geom_BoundedCurve>& BasisCurve,
    const Convert_ParameterisationType    Parameterisation = Convert_TgtThetaOver2);

  //! Append a curve in the BSpline Return False if the
  //! curve is not G0 with the BSplineCurve. Tolerance
  //! is used to check continuity and decrease
  //! Multiplicity at the common Knot until MinM
  //! if MinM = 0, the common Knot can be removed
  //!
  //! WithRatio defines whether the resulting curve should have a uniform
  //! parameterization. Setting WithRatio to false may greatly
  //! decrease the speed of algorithms like CPnts_AbscissaPoint::AdvPerform
  //! when applied to the resulting curve.
  Standard_EXPORT bool Add(const occ::handle<Geom_BoundedCurve>& NewCurve,
                           const double                          Tolerance,
                           const bool                            After     = false,
                           const bool                            WithRatio = true,
                           const int                             MinM      = 0);

  Standard_EXPORT occ::handle<Geom_BSplineCurve> BSplineCurve() const;

  //! Clear a result curve
  Standard_EXPORT void Clear();

private:
  //! Concat two BSplineCurves.
  Standard_EXPORT void Add(occ::handle<Geom_BSplineCurve>& FirstCurve,
                           occ::handle<Geom_BSplineCurve>& SecondCurve,
                           const bool                      After,
                           const bool                      WithRatio,
                           const int                       MinM);

  occ::handle<Geom_BSplineCurve> myCurve;
  double                         myTol;
  Convert_ParameterisationType   myType;
};

#endif // _GeomConvert_CompCurveToBSplineCurve_HeaderFile
