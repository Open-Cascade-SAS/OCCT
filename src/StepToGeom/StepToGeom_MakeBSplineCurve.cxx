// File:	StepToGeom_MakeBSplineCurve.cxx
// Created:	Thu Jul  1 17:35:13 1993
// Author:	Martine LANGLOIS

#include <StepToGeom_MakeBSplineCurve.ixx>
#include <gp_Pnt.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <Geom_CartesianPoint.hxx>

//=============================================================================
// Creation d' une BSplineCurve de Geom a partir d' une BSplineCurve de Step
//=============================================================================

#define Array1OfPnt_gen                  TColgp_Array1OfPnt
#define Pnt_gen                          gp_Pnt
#define Pnt_fonc                         Pnt
#define CartesianPoint_gen               Handle(Geom_CartesianPoint)
#define StepToGeom_MakeCartesianPoint_gen StepToGeom_MakeCartesianPoint
#define BSplineCurve_gen                 Geom_BSplineCurve
#define BSplineCurve_retour              Handle(Geom_BSplineCurve)
#define StepToGeom_MakeBSplineCurve_gen   StepToGeom_MakeBSplineCurve

Standard_Boolean StepToGeom_MakeBSplineCurve::Convert
    (const Handle(StepGeom_BSplineCurve)& SC,
     Handle(Geom_BSplineCurve)& CC)
  
#include <StepToGeom_MakeBSplineCurve.pxx>

#undef Array1OfPnt_gen
#undef Pnt_gen
#undef Pnt_fonc
#undef CartesianPoint_gen
#undef StepToGeom_MakeCartesianPoint_gen
#undef BSplineCurve_gen
#undef StepToGeom_MakeBSplineCurve_gen
#undef BSplineCurve_retour
