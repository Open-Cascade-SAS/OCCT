// File:	StepGeom_SurfaceCurveAndBoundedCurve.cxx
// Created:	Mon Feb 15 10:37:53 1999
// Author:	Andrey BETENEV
//		<abv@doomox.nnov.matra-dtv.fr>


#include <StepGeom_SurfaceCurveAndBoundedCurve.ixx>


StepGeom_SurfaceCurveAndBoundedCurve::StepGeom_SurfaceCurveAndBoundedCurve () 
{
}
     
Handle(StepGeom_BoundedCurve) &StepGeom_SurfaceCurveAndBoundedCurve::BoundedCurve ()
{
  return myBoundedCurve;
}
