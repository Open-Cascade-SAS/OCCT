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

#include <Aspect_CircularGrid.hxx>

#include <Standard_NegativeValue.hxx>
#include <Standard_NullValue.hxx>
#include <Standard_NumericError.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Aspect_CircularGrid,Aspect_Grid)

Aspect_CircularGrid::Aspect_CircularGrid
     (const Standard_Real aRadiusStep,
      const Standard_Integer aDivisionNumber,
      const Standard_Real anXOrigin,
      const Standard_Real anYOrigin,
      const Standard_Real aRotationAngle)
:Aspect_Grid(anXOrigin,anYOrigin,aRotationAngle),myRadiusStep(aRadiusStep),
myDivisionNumber(aDivisionNumber) {
   }

void Aspect_CircularGrid::SetRadiusStep(const Standard_Real aRadiusStep) {
  Standard_NegativeValue_Raise_if(aRadiusStep < 0., "invalid radius step");
  Standard_NullValue_Raise_if(aRadiusStep == 0. , "invalid radius step");
  myRadiusStep= aRadiusStep;
  Init();
  UpdateDisplay();
}
void Aspect_CircularGrid::SetDivisionNumber(const Standard_Integer aNumber) {
  Standard_NegativeValue_Raise_if(aNumber < 0., "invalid division number");
  Standard_NullValue_Raise_if(aNumber == 0. , "invalid division number");
  myDivisionNumber = aNumber;
  Init();
  UpdateDisplay();
}
void Aspect_CircularGrid::SetGridValues
     (const Standard_Real theXOrigin,
      const Standard_Real theYOrigin,
      const Standard_Real theRadiusStep,
      const Standard_Integer theDivisionNumber,
      const Standard_Real theRotationAngle) {
  myXOrigin = theXOrigin;
  myYOrigin = theYOrigin;
  Standard_NegativeValue_Raise_if(theRadiusStep < 0., "invalid radius step");
  Standard_NullValue_Raise_if(theRadiusStep == 0. , "invalid radius step");
  myRadiusStep= theRadiusStep;
  Standard_NegativeValue_Raise_if(theDivisionNumber < 0., "invalid division number");
  Standard_NullValue_Raise_if(theDivisionNumber == 0. , "invalid division number");
  myDivisionNumber = theDivisionNumber;
  myRotationAngle = theRotationAngle;
  Init();
  UpdateDisplay();
}
void Aspect_CircularGrid::Compute(const Standard_Real X,
                                  const Standard_Real Y,
                                  Standard_Real& gridX,
                                  Standard_Real& gridY) const {

  Standard_Real xo = XOrigin();
  Standard_Real yo = YOrigin();
  Standard_Real d = Sqrt( (xo-X)*(xo-X) + (yo-Y)*(yo-Y) );
  Standard_Integer n = (Standard_Integer ) ( d/myRadiusStep + 0.5 ) ;
  Standard_Real radius = Standard_Real(n) * myRadiusStep;
  Standard_Real cosinus = (X-xo)/d;
  Standard_Real a = ACos(cosinus);
  Standard_Real ra = RotationAngle();
  if ( Y < yo ) a = 2 * M_PI - a;
  n = (Standard_Integer ) ((a-ra)/myAlpha + Sign(0.5, a-ra)) ;

  Standard_Real cs=0,sn=0;
  Standard_Boolean done = Standard_False;
  Standard_Integer nmax = 2*myDivisionNumber;
  Standard_Integer nquad,qmax;

  if( ra == 0. ) {
    nquad = 4; qmax = nmax/nquad;
    if( (n == 0) || (!(nmax % nquad) && !(n % qmax)) ) {
      Standard_Integer q = n/qmax;
      switch (q) {
	default:
        case 0:
          cs = 1.; sn = 0.;
          break;
        case 1:
          cs = 0.; sn = 1.;
          break;
        case 2:
          cs = -1.; sn = 0.;
          break;
        case 3:
          cs = 0.; sn = -1.;
          break;
      }
      done = Standard_True;
    } else {
      nquad = 2; qmax = nmax/nquad;
      if( !(nmax % nquad) && !(n % qmax) ) {
        Standard_Integer q = n/qmax;
        switch (q) {
	  default:
          case 0:
            cs = 1.; sn = 0.;
            break;
          case 1:
            cs = -1.; sn = 0.;
            break;
        }
        done = Standard_True;
      }
    }
  } 

  if( !done ) {
    Standard_Real ang = ra + Standard_Real(n)*myAlpha;
    cs = Cos(ang); sn = Sin(ang);
  }
  gridX = xo + cs * radius;
  gridY = yo + sn * radius;
}

Standard_Real Aspect_CircularGrid::RadiusStep() const {
  return myRadiusStep;
}

Standard_Integer Aspect_CircularGrid::DivisionNumber () const {
return myDivisionNumber;
}

void Aspect_CircularGrid::Init () {
  myAlpha = M_PI / Standard_Real(myDivisionNumber);
  myA1 = Cos(myAlpha); myB1=Sin(myAlpha);
}
