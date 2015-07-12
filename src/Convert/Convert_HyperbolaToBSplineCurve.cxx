// Copyright (c) 1995-1999 Matra Datavision
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

//JCV 16/10/91

#include <Convert_HyperbolaToBSplineCurve.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Trsf2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

static Standard_Integer TheDegree  = 2;
static Standard_Integer MaxNbKnots = 2;
static Standard_Integer MaxNbPoles = 3;


//=======================================================================
//function : Convert_HyperbolaToBSplineCurve
//purpose  : 
//=======================================================================

Convert_HyperbolaToBSplineCurve::Convert_HyperbolaToBSplineCurve 
  (const gp_Hypr2d&    H , 
   const Standard_Real U1,
   const Standard_Real U2 )

: Convert_ConicToBSplineCurve (MaxNbPoles, MaxNbKnots, TheDegree) 
{
  Standard_DomainError_Raise_if( Abs(U2 - U1) < Epsilon(0.),
				"Convert_ParabolaToBSplineCurve");

  Standard_Real UF = Min (U1, U2);
  Standard_Real UL = Max( U1, U2);
  
  nbPoles = 3;
  nbKnots = 2;
  isperiodic = Standard_False;
  knots->ChangeArray1()(1) = UF;  mults->ChangeArray1()(1) = 3;  
  knots->ChangeArray1()(2) = UL;  mults->ChangeArray1()(2) = 3;  
  
  // construction of hyperbola in the reference xOy.
  
  Standard_Real R = H.MajorRadius();
  Standard_Real r = H.MinorRadius();
  gp_Dir2d Ox = H.Axis().XDirection();
  gp_Dir2d Oy = H.Axis().YDirection();
  Standard_Real S = ( Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.) ?  1 : -1;
  
  // poles expressed in the reference mark
  // the 2nd pole is at the intersection of 2 tangents to the curve
  // at points P(UF), P(UL)
  // the weight of this pole is equal to : Cosh((UL-UF)/2)

  weights->ChangeArray1()(1) = 1.;
  weights->ChangeArray1()(2) = Cosh((UL-UF)/2);
  weights->ChangeArray1()(3) = 1.;

  Standard_Real delta = Sinh(UL-UF);
  Standard_Real x =     R * ( Sinh(UL) - Sinh(UF)) / delta;
  Standard_Real y = S * r * ( Cosh(UL) - Cosh(UF)) / delta;
  poles->ChangeArray1()(1) = gp_Pnt2d( R * Cosh(UF), S * r * Sinh(UF));
  poles->ChangeArray1()(2) = gp_Pnt2d( x, y);
  poles->ChangeArray1()(3) = gp_Pnt2d( R * Cosh(UL), S * r * Sinh(UL));

  // replace the bspline in the mark of the hyperbola
  gp_Trsf2d Trsf;
  Trsf.SetTransformation( H.Axis().XAxis(), gp::OX2d());
  poles->ChangeArray1()(1).Transform( Trsf);
  poles->ChangeArray1()(2).Transform( Trsf);
  poles->ChangeArray1()(3).Transform( Trsf);
}
