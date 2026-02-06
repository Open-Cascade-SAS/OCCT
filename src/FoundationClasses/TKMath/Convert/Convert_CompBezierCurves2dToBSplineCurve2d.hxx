// Created on: 1993-11-09
// Created by: Modelistation
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

#ifndef _Convert_CompBezierCurves2dToBSplineCurve2d_HeaderFile
#define _Convert_CompBezierCurves2dToBSplineCurve2d_HeaderFile

#include <Convert_CompBezierCurvesToBSplineCurveBase.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

//! Converts a list of connecting Bezier Curves 2d to a
//! BSplineCurve 2d.
//! if possible, the continuity of the BSpline will be
//! increased to more than C0.
class Convert_CompBezierCurves2dToBSplineCurve2d
    : public Convert_CompBezierCurvesToBSplineCurveBase<gp_Pnt2d, gp_Vec2d>
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a framework for converting a sequence of
  //! adjacent non-rational Bezier curves into a BSpline curve.
  //! @param[in] theAngularTolerance angular tolerance in radians
  //!            for checking tangent parallelism at junction points
  Standard_EXPORT Convert_CompBezierCurves2dToBSplineCurve2d(
    const double theAngularTolerance = 1.0e-4);
};

#endif // _Convert_CompBezierCurves2dToBSplineCurve2d_HeaderFile
