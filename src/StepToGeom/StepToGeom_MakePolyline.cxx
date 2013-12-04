// Created on: 1999-02-15
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
