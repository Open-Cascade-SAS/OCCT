// Created on: 1995-10-10
// Created by: Jacques GOUSSARD
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

#include <Convert_PolynomialCosAndSin.hxx>

#include <TColgp_Array1OfPnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>

#include <gp.hxx>
#include <Precision.hxx>
#include <PLib.hxx>
#include <BSplCLib.hxx>

#include <Standard_ConstructionError.hxx>

static Standard_Real Locate(const Standard_Real Angfin,
			    const TColgp_Array1OfPnt2d& TPoles,
			    const Standard_Real Umin,
			    const Standard_Real Umax)
{
  Standard_Real umin = Umin;
  Standard_Real umax = Umax;
  Standard_Real Ptol = Precision::Angular();
  Standard_Real Utol = Precision::PConfusion();
  while (Abs(umax-umin)>= Utol) {
    Standard_Real ptest = (umax+umin)/2.;
    gp_Pnt2d valP;
    BSplCLib::D0(ptest,TPoles,BSplCLib::NoWeights(),valP);
    Standard_Real theta = ATan2(valP.Y(),valP.X());
    if (theta < 0.) {
      theta +=2.*M_PI;
    }
    if (Abs(theta - Angfin) < Ptol) {
      return ptest;
    }
    if (theta < Angfin) {
      umin = ptest;
    }
    else if (theta > Angfin) {
      umax = ptest;
    }
  }
  return (umin+umax)/2.;
}


void BuildPolynomialCosAndSin
  (const Standard_Real UFirst,
   const Standard_Real ULast,
   const Standard_Integer num_poles,
   Handle(TColStd_HArray1OfReal)& CosNumeratorPtr,
   Handle(TColStd_HArray1OfReal)& SinNumeratorPtr,
   Handle(TColStd_HArray1OfReal)& DenominatorPtr)
{

  Standard_Real  Delta,
  locUFirst,
//  locULast,
//  temp_value,
  t_min,
  t_max,
  trim_min,
  trim_max,
  middle,
  Angle,
  PI2 = 2*M_PI ;
  Standard_Integer ii, degree = num_poles -1 ;
  locUFirst = UFirst ;

  // Return UFirst in [-2PI; 2PI]
  // to make rotations without risk
  while (locUFirst > PI2) {
    locUFirst -= PI2;
  }
  while (locUFirst < - PI2) {
    locUFirst += PI2;
  }

// Return to the arc [0, Delta]
  Delta = ULast - UFirst;
  middle =  0.5e0 * Delta ; 
  
  // coincide the required bisector of the angular sector with 
  // axis -Ox definition of the circle in Bezier of degree 7 so that 
  // parametre 1/2 of Bezier was exactly a point of the bissectrice 
  // of the required angular sector.
  //
  Angle = middle - M_PI ;
  //
  // Circle of radius 1. See Euclid
  //

  TColgp_Array1OfPnt2d TPoles(1,8),
  NewTPoles(1,8) ;
  TPoles(1).SetCoord(1.,0.);
  TPoles(2).SetCoord(1.,1.013854);
  TPoles(3).SetCoord(-0.199043,1.871905);
  TPoles(4).SetCoord(-1.937729,1.057323);
  TPoles(5).SetCoord(-1.937729,-1.057323);
  TPoles(6).SetCoord(-0.199043,-1.871905);
  TPoles(7).SetCoord(1.,-1.013854);
  TPoles(8).SetCoord(1.,0.);
  gp_Trsf2d T;
  T.SetRotation(gp::Origin2d(),Angle);
  for (ii=1; ii<=num_poles; ii++) {
    TPoles(ii).Transform(T);
  }


  t_min = 1.0e0 - (Delta * 1.3e0 / M_PI) ;
  t_min *= 0.5e0 ;
  t_min = Max(t_min,0.0e0) ;
  t_max = 1.0e0 + (Delta * 1.3e0 / M_PI) ;
  t_max *= 0.5e0 ;
  t_max = Min(t_max,1.0e0) ;
  trim_max = Locate(Delta,
		    TPoles,
		    t_min,
		    t_max);
  //
  // as Bezier is symmetric correspondingly to the bissector 
  // of the angular sector ...
  
  trim_min = 1.0e0 - trim_max ;
  //
  Standard_Real knot_array[2] ;
  Standard_Integer  mults_array[2] ; 
  knot_array[0] = 0.0e0 ;
  knot_array[1] = 1.0e0 ;
  mults_array[0] = degree + 1 ;
  mults_array[1] = degree + 1 ;
  
  TColStd_Array1OfReal  the_knots(knot_array[0],1,2),
  the_new_knots(knot_array[0],1,2);
  TColStd_Array1OfInteger the_mults(mults_array[0],1,2),
  the_new_mults(mults_array[0],1,2) ;
  
  BSplCLib::Trimming(degree,
		     Standard_False,
		     the_knots,
		     the_mults,
		     TPoles,
		     BSplCLib::NoWeights(),
		     trim_min,
		     trim_max,
		     the_new_knots,
		     the_new_mults,
		     NewTPoles,
		     BSplCLib::NoWeights());

  // readjustment is obviously redundant
  Standard_Real SinD = Sin(Delta), CosD = Cos(Delta);
  gp_Pnt2d Pdeb(1., 0.);
  gp_Pnt2d Pfin(CosD, SinD);

  Standard_Real dtg = NewTPoles(1).Distance(NewTPoles(2));
  NewTPoles(1) = Pdeb;
  gp_XY theXY(0.,dtg);
  Pdeb.ChangeCoord() += theXY;
  NewTPoles(2) = Pdeb;
  
  // readjustment to Euclid
  dtg = NewTPoles(num_poles).Distance(NewTPoles(num_poles-1));
  NewTPoles(num_poles) = Pfin;
  theXY.SetCoord(dtg*SinD,-dtg*CosD);
  Pfin.ChangeCoord() += theXY;
  NewTPoles(num_poles-1) = Pfin;

  // Rotation to return to the arc [LocUFirst, LocUFirst+Delta]
  T.SetRotation(gp::Origin2d(), locUFirst);
  for (ii=1; ii<=num_poles; ii++) {
    NewTPoles(ii).Transform(T);
  }  
  
  for (ii=1; ii<=num_poles; ii++) {
    CosNumeratorPtr->SetValue(ii,NewTPoles(ii).X());
    SinNumeratorPtr->SetValue(ii,NewTPoles(ii).Y());
    DenominatorPtr->SetValue(ii,1.);
  }
}

/*
void BuildHermitePolynomialCosAndSin
  (const Standard_Real UFirst,
   const Standard_Real ULast,
   const Standard_Integer num_poles,
   Handle(TColStd_HArray1OfReal)& CosNumeratorPtr,
   Handle(TColStd_HArray1OfReal)& SinNumeratorPtr,
   Handle(TColStd_HArray1OfReal)& DenominatorPtr)
{

  if (num_poles%2 != 0) {
    throw Standard_ConstructionError();
  }
  Standard_Integer ii;
  Standard_Integer ordre_deriv = num_poles/2;
  Standard_Real ang = ULast - UFirst;
  Standard_Real Cd = Cos(UFirst);
  Standard_Real Sd = Sin(UFirst);
  Standard_Real Cf = Cos(ULast);
  Standard_Real Sf = Sin(ULast);
  
  Standard_Integer Degree = num_poles-1;
  TColStd_Array1OfReal FlatKnots(1,2*num_poles);
  TColStd_Array1OfReal Parameters(1,num_poles);
  TColStd_Array1OfInteger ContactOrderArray(1,num_poles);
  TColgp_Array1OfPnt2d Poles(1,num_poles);
  TColgp_Array1OfPnt2d TPoles(1,num_poles);
 

  for (ii=1; ii<=num_poles; ii++) {
    FlatKnots(ii) = 0.;
    FlatKnots(ii+num_poles) = 1.;
  }

  Standard_Real coef = 1.;
  Standard_Real xd,yd,xf,yf; 

  for (ii=1; ii<=ordre_deriv; ii++) {
    Parameters(ii) = 0.;
    Parameters(ii+ordre_deriv) = 1.;

    ContactOrderArray(ii) = ContactOrderArray(num_poles-ii+1) = ii-1;

    switch ((ii-1)%4) {
    case 0:
      {
	xd = Cd*coef;
	yd = Sd*coef;
	xf = Cf*coef;
	yf = Sf*coef;
      }
      break;
    case 1:
      {
	xd = -Sd*coef;
	yd =  Cd*coef;
	xf = -Sf*coef;
	yf =  Cf*coef;
      }
      break;
    case 2:
      {
	xd = -Cd*coef;
	yd = -Sd*coef;
	xf = -Cf*coef;
	yf = -Sf*coef;
      }
      break;
    case 3:
      {
	xd =  Sd*coef;
	yd = -Cd*coef;
	xf =  Sf*coef;
	yf = -Cf*coef;
      }
      break;
    }

    Poles(ii).SetX(xd);
    Poles(ii).SetY(yd);
    Poles(num_poles-ii+1).SetX(xf);
    Poles(num_poles-ii+1).SetY(yf);

    coef *= ang;
  }

  Standard_Integer InversionPb;
  BSplCLib::Interpolate(Degree,FlatKnots,Parameters,
			ContactOrderArray,Poles,InversionPb);

  if (InversionPb !=0) {
    throw Standard_ConstructionError();
  }
  for (ii=1; ii<=num_poles; ii++) {
    CosNumeratorPtr->SetValue(ii,Poles(ii).X());
    SinNumeratorPtr->SetValue(ii,Poles(ii).Y());
    DenominatorPtr->SetValue(ii,1.);
  }

}
*/
