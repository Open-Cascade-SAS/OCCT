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

#define No_Standard_RangeError
#define No_Standard_OutOfRange


#include <AppParCurves.hxx>
#include <BSplCLib.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <TColStd_Array1OfReal.hxx>

void AppParCurves::BernsteinMatrix(const Standard_Integer NbPoles, 
				   const math_Vector& U,
				   math_Matrix& A) {

  Standard_Integer i, j, id;
  Standard_Real u0, u1, y0, y1, xs;
  Standard_Integer first = U.Lower(), last = U.Upper();
  math_Vector B(1, NbPoles-1);


  for (i = first; i <= last; i++) {
    B(1) = 1;
    u0 = U(i);
    u1 = 1.-u0;
    
    for (id = 2; id <= NbPoles-1; id++) {
      y0 = B(1);
      y1 = u0*y0;
      B(1) = y0-y1;
      for (j = 2; j <= id-1; j++) {
	xs = y1;
	y0 = B(j);
	y1 = u0*y0;
	B(j) = y0-y1+xs;
      }
      B(id) = y1;
    }
    A(i, 1) = u1*B(1);
    A(i, NbPoles) = u0*B(NbPoles-1);
    for (j = 2; j <= NbPoles-1; j++) {
      A(i, j) = u1*B(j)+u0*B(j-1);
    }
  }
}


void AppParCurves::Bernstein(const Standard_Integer NbPoles, 
			     const math_Vector& U,
			     math_Matrix& A,
			     math_Matrix& DA) {
  
  Standard_Integer i, j, id, Ndeg = NbPoles-1;
  Standard_Real u0, u1, y0, y1, xs, bj, bj1;;
  Standard_Integer first = U.Lower(), last = U.Upper();
  math_Vector B(1, NbPoles-1);

  
  for (i = first; i <= last; i++) {
    B(1) = 1;
    u0 = U(i);
    u1 = 1.-u0;
    
    for (id = 2; id <= NbPoles-1; id++) {
      y0 = B(1);
      y1 = u0*y0;
      B(1) = y0-y1;
      for (j = 2; j <= id-1; j++) {
	xs = y1;
	y0 = B(j);
	y1 = u0*y0;
	B(j) = y0-y1+xs;
      }
      B(id) = y1;
    }
    DA(i, 1) = -Ndeg*B(1);
    DA(i, NbPoles) = Ndeg*B(NbPoles-1);
    A(i, 1) = u1*B(1);
    A(i, NbPoles) = u0*B(NbPoles-1);
    for (j = 2; j <= NbPoles-1; j++) {
      bj = B(j);  bj1 = B(j-1);
      DA(i,j) = Ndeg*(bj1-bj);
      A(i, j) = u1*bj+u0*bj1;
    }
  }
}


void AppParCurves::SecondDerivativeBernstein(const Standard_Real U,
					     math_Vector&        DDA) {
//  Standard_Real U1 = 1-U, Y0, Y1, Xs;
  Standard_Real Y0, Y1, Xs;
  Standard_Integer NbPoles = DDA.Length();
  Standard_Integer id, j, N4, deg = NbPoles-1;
  N4 = deg*(deg-1);
  math_Vector B(1, deg-1);
  B(1) = 1.;
  
  // Cas particulier si degre = 1:
  if (deg == 1) {
    DDA(1) = 0.0;
    DDA(2) = 0.0;
  }
  else if (deg == 2) {
    DDA(1) = 2.0;
    DDA(2) = -4.0;
    DDA(3) = 2.0;
  }
  else {

    for (id = 2; id <= deg-1; id++) {
      Y0 = B(1);
      Y1 = U*Y0;
      B(1) = Y0-Y1;
      for (j = 2; j <= id-1; j++) {
	Xs = Y1;
	Y0 = B(j);
	Y1 = U*Y0;
	B(j) = Y0 - Y1 +Xs;
      }
      B(id) = Y1;
    }
    
    DDA(1) = N4*B(1);
    DDA(2) = N4*(-2*B(1) + B(2));
    DDA(deg) = N4*(B(deg-2) - 2*B(deg-1));
    DDA(deg+1) = N4*B(deg-1);
    
    for(j = 2; j <= deg-2; j++) {
      DDA(j+1) = N4*(B(j-1)-2*B(j)+B(j+1));
    }
  }
}



void AppParCurves::SplineFunction(const Standard_Integer nbpoles,
				  const Standard_Integer deg,
				  const math_Vector&     Parameters,
				  const math_Vector&     flatknots,
				  math_Matrix&           A,
				  math_Matrix&           DA,
				  math_IntegerVector&    index)
{
//  Standard_Real U, NewU, co, diff, t1, t2;
  Standard_Real U, NewU;
//  gp_Pnt2d Pt, P0;
//  gp_Vec2d V1;
//  Standard_Integer i, j, k, iter, in, ik, deg1 = deg+1;
  Standard_Integer i, j, deg1 = deg+1;
//  Standard_Integer oldkindex, kindex, theindex, ttindex;
  Standard_Integer oldkindex, kindex, theindex;
  math_Vector locpoles(1 , deg1);
  math_Vector locdpoles(1 , deg1);
  Standard_Integer firstp = Parameters.Lower(), lastp = Parameters.Upper();

  TColStd_Array1OfReal Aflatknots(flatknots.Lower(), flatknots.Upper());
  for (i = flatknots.Lower(); i <= flatknots.Upper(); i++) {
    Aflatknots(i) = flatknots(i);
  }


  oldkindex = 1;
  
  Standard_Integer pp, qq;
  Standard_Real Saved, Inverse, LocalInverse, locqq, locdqq, val;

  for (i = firstp; i <= lastp; i++) {
    U = Parameters(i);
    NewU = U;
    kindex = oldkindex;
    BSplCLib::LocateParameter(deg, Aflatknots, U, Standard_False, 
			      deg1, nbpoles+1, kindex, NewU);
    
    oldkindex = kindex;
    
    // On stocke les index:
    index(i) = kindex - deg-1;
    
    locpoles(1) = 1.0;
    
    for (qq = 2; qq <= deg; qq++) {
      locpoles(qq) = 0.0;
      for (pp = 1; pp <= qq-1; pp++) {
	Inverse = 1.0 / (flatknots(kindex + pp)  - flatknots(kindex - qq + pp + 1)) ; 
        Saved = (U - flatknots(kindex - qq + pp + 1)) * Inverse * locpoles(pp);
        locpoles(pp) *= (flatknots(kindex + pp) - U) * Inverse ;
        locpoles(pp) += locpoles(qq) ;
        locpoles(qq) = Saved ;
      }
    }

    qq = deg+1;
    for (pp = 1; pp <= deg; pp++) {
      locdpoles(pp)= locpoles(pp);
    }

    locqq = 0.0;
    locdqq = 0.0;
    for (pp = 1; pp <= deg; pp++) {
      Inverse =  1.0 / (flatknots(kindex + pp)  - flatknots(kindex - qq + pp + 1)); 
      Saved = (U - flatknots(kindex - qq + pp + 1)) * Inverse * locpoles(pp);
      locpoles(pp) *= (flatknots(kindex + pp) - U) * Inverse;
      locpoles(pp) += locqq;
      locqq = Saved ;
      LocalInverse = (Standard_Real) (deg) * Inverse;
      Saved = LocalInverse * locdpoles(pp);
      locdpoles(pp) *= - LocalInverse ;
      locdpoles(pp) +=   locdqq;
      locdqq = Saved ;
    }

    locpoles(qq) = locqq;
    locdpoles(qq) = locdqq;

    for (j = 1; j <= deg1; j++) {
      val = locpoles(j);
      theindex = j+oldkindex-deg1;
      A(i, theindex) = val;
      DA(i, theindex) = locdpoles(j);
    }

    for (j = 1; j < oldkindex-deg; j++)
      A(i, j) = DA(i, j) = 0.0;
    for (j = oldkindex+1; j <= nbpoles; j++)
      A(i, j) = DA(i, j) = 0.0;
    
  }

}
