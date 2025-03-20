// Created on: 2001-12-20
// Created by: Pavel TELKOV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _ShapeCustom_Curve2d_HeaderFile
#define _ShapeCustom_Curve2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_Array1OfPnt2d.hxx>
class Geom2d_Line;
class Geom2d_Curve;
class Geom2d_BSplineCurve;

//! Converts curve2d to analytical form with given
//! precision or simplify curve2d.
class ShapeCustom_Curve2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Check if poleses is in the plane with given precision
  //! Returns false if no.
  Standard_EXPORT static Standard_Boolean IsLinear(const TColgp_Array1OfPnt2d& thePoles,
                                                   const Standard_Real         theTolerance,
                                                   Standard_Real&              theDeviation);

  //! Try to convert BSpline2d or Bezier2d to line 2d
  //! only if it is linear. Recalculate first and last parameters.
  //! Returns line2d or null curve2d.
  Standard_EXPORT static Handle(Geom2d_Line) ConvertToLine2d(const Handle(Geom2d_Curve)& theCurve,
                                                             const Standard_Real         theFirstIn,
                                                             const Standard_Real         theLastIn,
                                                             const Standard_Real theTolerance,
                                                             Standard_Real&      theNewFirst,
                                                             Standard_Real&      theNewLast,
                                                             Standard_Real&      theDeviation);

  //! Try to remove knots from bspline where local derivatives are the same.
  //! Remove knots with given precision.
  //! Returns false if Bsplien was not modified
  Standard_EXPORT static Standard_Boolean SimplifyBSpline2d(
    Handle(Geom2d_BSplineCurve)& theBSpline2d,
    const Standard_Real          theTolerance);

protected:
private:
};

#endif // _ShapeCustom_Curve2d_HeaderFile
