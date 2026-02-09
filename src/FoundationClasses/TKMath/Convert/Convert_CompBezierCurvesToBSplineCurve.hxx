// Created on: 1993-10-20
// Created by: Bruno DUMORTIER
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

#ifndef _Convert_CompBezierCurvesToBSplineCurve_HeaderFile
#define _Convert_CompBezierCurvesToBSplineCurve_HeaderFile

#include <Convert_CompBezierCurvesToBSplineCurveBase.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//! An algorithm to convert a sequence of adjacent
//! non-rational Bezier curves into a BSpline curve.
//! A CompBezierCurvesToBSplineCurve object provides a framework for:
//! -   defining the sequence of adjacent non-rational Bezier
//! curves to be converted into a BSpline curve,
//! -   implementing the computation algorithm, and
//! -   consulting the results.
//! Warning
//! Do not attempt to convert rational Bezier curves using this type of algorithm.
class Convert_CompBezierCurvesToBSplineCurve
    : public Convert_CompBezierCurvesToBSplineCurveBase<gp_Pnt, gp_Vec>
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a framework for converting a sequence of
  //! adjacent non-rational Bezier curves into a BSpline curve.
  //! @param[in] theAngularTolerance angular tolerance in radians
  //!            for checking tangent parallelism at junction points
  Standard_EXPORT Convert_CompBezierCurvesToBSplineCurve(const double theAngularTolerance = 1.0e-4);
};

#endif // _Convert_CompBezierCurvesToBSplineCurve_HeaderFile
