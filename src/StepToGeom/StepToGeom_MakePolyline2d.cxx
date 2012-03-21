// Created on: 1999-02-15
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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
