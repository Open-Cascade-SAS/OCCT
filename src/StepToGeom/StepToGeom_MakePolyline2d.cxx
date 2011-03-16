// File:	StepToGeom_MakePolyline2d.cxx
// Created:	Mon Feb 15 15:05:36 1999
// Author:	Andrey BETENEV

#include <StepToGeom_MakePolyline2d.ixx>
#include <StepToGeom_MakeCartesianPoint2d.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <Geom2d_CartesianPoint.hxx>

#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

//=======================================================================
//function : StepToGeom_MakePolyline2d
//purpose  : 
//=======================================================================

Standard_Boolean StepToGeom_MakePolyline2d::Convert (const Handle(StepGeom_Polyline)& SPL, Handle(Geom2d_BSplineCurve)& CC)
{
  if (SPL.IsNull())
    return Standard_False;

  const Standard_Integer nbp = SPL->NbPoints();
  if (nbp > 1)
  {
    TColgp_Array1OfPnt2d Poles ( 1, nbp );
    TColStd_Array1OfReal Knots ( 1, nbp );
    TColStd_Array1OfInteger Mults ( 1, nbp );

    Handle(Geom2d_CartesianPoint) P;
    for ( Standard_Integer i=1; i <= nbp; i++ )
    {
      if (StepToGeom_MakeCartesianPoint2d::Convert(SPL->PointsValue(i),P))
        Poles.SetValue ( i, P->Pnt2d() );
      else
        return Standard_False;
      Knots.SetValue ( i, Standard_Real(i-1) );
      Mults.SetValue ( i, 1 );
    }
    Mults.SetValue ( 1, 2 );
    Mults.SetValue ( nbp, 2 );

    CC = new Geom2d_BSplineCurve ( Poles, Knots, Mults, 1 );
    return Standard_True;
  }
  return Standard_False;
}
