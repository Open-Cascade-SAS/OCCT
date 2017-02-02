// Created on: 1997-12-10
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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


#include <GeomFill_Fixed.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_Fixed,GeomFill_TrihedronLaw)

GeomFill_Fixed::GeomFill_Fixed(const gp_Vec& Tangent,
			       const gp_Vec& Normal)
{
  if (Tangent.IsParallel(Normal, 0.01) ) 
    throw Standard_ConstructionError(
      "GeomFill_Fixed : Two parallel vectors !");
  T = Tangent;
  T.Normalize();
  N = Normal;
  N.Normalize();
  B = T ^ N;
  B.Normalize();
}

Handle(GeomFill_TrihedronLaw) GeomFill_Fixed::Copy() const
{
 Handle(GeomFill_Fixed) copy = new (GeomFill_Fixed)(T, N);
 copy->SetCurve(myCurve);
 return copy;
} 

 Standard_Boolean GeomFill_Fixed::D0(const Standard_Real, 
						 gp_Vec& Tangent,
						 gp_Vec& Normal,
						 gp_Vec& BiNormal) 
{
  Tangent = T;
  Normal = N;
  BiNormal = B;

  return Standard_True; 
}

 Standard_Boolean GeomFill_Fixed::D1(const Standard_Real,
						 gp_Vec& Tangent,
						 gp_Vec& DTangent,
						 gp_Vec& Normal,
						 gp_Vec& DNormal,
						 gp_Vec& BiNormal,
						 gp_Vec& DBiNormal) 
{
  Tangent = T;
  Normal = N;
  BiNormal = B;

  gp_Vec V0(0,0,0);
  DTangent = DNormal = DBiNormal = V0;

  return Standard_True; 
}

 Standard_Boolean GeomFill_Fixed::D2(const Standard_Real,
						 gp_Vec& Tangent,
						 gp_Vec& DTangent,
						 gp_Vec& D2Tangent,
						 gp_Vec& Normal,
						 gp_Vec& DNormal,
						 gp_Vec& D2Normal,
						 gp_Vec& BiNormal,
						 gp_Vec& DBiNormal,
						 gp_Vec& D2BiNormal) 
{
  Tangent = T;
  Normal = N;
  BiNormal = B;

  gp_Vec V0(0,0,0);
  DTangent = D2Tangent = V0;
  DNormal = D2Normal = V0;
  DBiNormal = D2BiNormal = V0;

  return Standard_True; 
}

 Standard_Integer GeomFill_Fixed::NbIntervals(const GeomAbs_Shape) const
{
  return 1;
}

 void GeomFill_Fixed::Intervals(TColStd_Array1OfReal& theT,
					    const GeomAbs_Shape) const
{
  theT(theT.Lower()) = - Precision::Infinite();
  theT(theT.Upper()) =   Precision::Infinite();
}

 void GeomFill_Fixed::GetAverageLaw(gp_Vec& ATangent,
				    gp_Vec& ANormal,
				    gp_Vec& ABiNormal) 
{
   ATangent = T;
   ANormal = N;
   ABiNormal = B;
}

 Standard_Boolean GeomFill_Fixed::IsConstant() const
{
  return Standard_True;
}
