// Created on: 1993-06-16
// Created by: Martine LANGLOIS
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


#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAbs_BSplKnotDistribution.hxx>
#include <GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepGeom_KnotType.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=============================================================================
// Creation d' une bspline_curve_with_knots_and_rational_bspline_curve de
// prostep a partir d' une BSplineCurve de Geom
//=============================================================================
GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve::
  GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve( const
    Handle(Geom_BSplineCurve)& BS )
								      
{
#define Array1OfPnt_gen TColgp_Array1OfPnt
#include "GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve_gen.pxx"
#undef Array1OfPnt_gen
}
//=============================================================================
// Creation d' une bspline_curve_with_knots_and_rational_bspline_curve de
// prostep a partir d' une BSplineCurve de Geom2d
//=============================================================================

GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve::
  GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve( const
    Handle(Geom2d_BSplineCurve)& BS )
								      
{
#define Array1OfPnt_gen TColgp_Array1OfPnt2d
#include "GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve_gen.pxx"
#undef Array1OfPnt_gen
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve) &
      GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve::Value() const
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve::Value() - no result");
  return theBSplineCurveWithKnotsAndRationalBSplineCurve;
}
