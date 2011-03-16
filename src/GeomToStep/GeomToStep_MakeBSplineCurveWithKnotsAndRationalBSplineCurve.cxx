// File:	GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve.cxx
// Created:	Wed Jun 16 11:45:37 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

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
