// File:	StepToGeom_MakePolyline.cxx
// Created:	Mon Feb 15 15:05:36 1999
// Author:	Andrey BETENEV

#include <StepToGeom_MakePolyline.ixx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <Geom_CartesianPoint.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

//=======================================================================
//function : StepToGeom_MakePolyline
//purpose  : 
//=======================================================================

Standard_Boolean StepToGeom_MakePolyline::Convert (const Handle(StepGeom_Polyline)& SPL, Handle(Geom_BSplineCurve)& CC)
{
  if (SPL.IsNull())
    return Standard_False;

  const Standard_Integer nbp = SPL->NbPoints();
  if (nbp > 1)
  {
    TColgp_Array1OfPnt Poles ( 1, nbp );
    TColStd_Array1OfReal Knots ( 1, nbp );
    TColStd_Array1OfInteger Mults ( 1, nbp );

    Handle(Geom_CartesianPoint) P;
    for ( Standard_Integer i=1; i <= nbp; i++ )
    {
      if (StepToGeom_MakeCartesianPoint::Convert(SPL->PointsValue(i),P))
        Poles.SetValue ( i, P->Pnt() );
	  else
        return Standard_False;
      Knots.SetValue ( i, Standard_Real(i-1) );
      Mults.SetValue ( i, 1 );
    }
    Mults.SetValue ( 1, 2 );
    Mults.SetValue ( nbp, 2 );

    CC = new Geom_BSplineCurve ( Poles, Knots, Mults, 1 );
    return Standard_True;
  }
  return Standard_False;
}
