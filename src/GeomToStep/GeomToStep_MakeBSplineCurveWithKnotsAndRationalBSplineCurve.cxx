// Created on: 1993-06-16
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve.ixx>
#include <StdFail_NotDone.hxx>
#include <Geom_BSplineCurve.hxx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <GeomAbs_BSplKnotDistribution.hxx>
#include <StepGeom_KnotType.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une bspline_curve_with_knots_and_rational_bspline_curve de
// prostep a partir d' une BSplineCurve de Geom
//=============================================================================

GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve::
  GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve( const
    Handle(Geom_BSplineCurve)& BS )
								      
{
#define Array1OfPnt_gen TColgp_Array1OfPnt
#include <GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve_gen.pxx>
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
#include <GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve_gen.pxx>
#undef Array1OfPnt_gen
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve) &
      GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theBSplineCurveWithKnotsAndRationalBSplineCurve;
}
