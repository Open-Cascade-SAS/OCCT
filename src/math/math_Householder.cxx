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

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif

#include <math_Householder.hxx>
#include <math_Matrix.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

// Cette classe decrit la methode de Householder qui transforme A en un
// produit de matrice orthogonale par une triangulaire superieure. Les seconds
// membres sont modifies dans le meme temps.
// Les references sur le cote sont celles de l'algorithme explique en page
// 90 du livre "Introduction a l'analyse numerique matricielle et a 
// l'optimisation." par P.G. CIARLET, edition MASSON. Les secondes 
// references sont celles du sous-programme HOUSEO d'Euclid.
// A la difference du sous-programme Houseo, la premiere colonne n'est pas 
// traitee separement. Les tests effectues ont montre que le code effectue
// specialement pour celle-ci etait plus long qu'une simple recopie. C'est
// donc cette solution de recopie initiale qui a ete retenue.
math_Householder::math_Householder(const math_Matrix& A, const math_Vector& B,
                                   const Standard_Real EPS):
                                   Sol(1, A.ColNumber(), 1, 1),
                                   Q(1, A.RowNumber(), 
                                     1, A.ColNumber()) {

  mylowerArow = A.LowerRow();
  mylowerAcol = A.LowerCol();
  myupperArow = A.UpperRow();
  myupperAcol = A.UpperCol();
  math_Matrix B1(1, B.Length(), 1, 1);
  B1.SetCol(1, B);
  Perform(A, B1, EPS);
}



math_Householder::math_Householder(const math_Matrix& A, const math_Matrix& B,
				   const Standard_Real EPS):
                                   Sol(1, A.ColNumber(), 
                                       1, B.ColNumber()),
                                   Q(1, A.RowNumber(), 
                                     A.LowerCol(), A.UpperCol()) {

  mylowerArow = A.LowerRow();
  mylowerAcol = A.LowerCol();
  myupperArow = A.UpperRow();
  myupperAcol = A.UpperCol();
  Perform(A, B, EPS);
}


math_Householder::math_Householder(const math_Matrix& A, const math_Matrix& B,
                                   const Standard_Integer lowerArow,
                                   const Standard_Integer upperArow,
                                   const Standard_Integer lowerAcol,
                                   const Standard_Integer upperAcol,
				   const Standard_Real EPS):
                                   Sol(1, upperAcol-lowerAcol+1, 
                                       1, B.ColNumber()),
                                   Q(1, upperArow-lowerArow+1, 
                                     1, upperAcol-lowerAcol+1) {
  mylowerArow = lowerArow;
  myupperArow = upperArow;
  mylowerAcol = lowerAcol;
  myupperAcol = upperAcol;

  Perform(A, B, EPS);
}


void math_Householder::Perform(const math_Matrix& A, const math_Matrix& B, 
                               const Standard_Real EPS) {

  Standard_Integer i, j, k, n, l, m;
  Standard_Real scale, f, g, h = 0., alfaii;
  Standard_Real qki;
  Standard_Real cj;
  n = Q.ColNumber();
  l = Q.RowNumber();
  m = B.ColNumber();
  math_Matrix B2(1, l, 1, m);

  Standard_Integer lbrow = B.LowerRow();
  for (i = 1; i <= l; i++) {
    for (j = 1; j <= n; j++) {
      Q(i, j) = A(i+mylowerArow-1, j+mylowerAcol-1);
    }
    for (j=1; j <= m; j++) {
      B2(i, j) = B(i+lbrow-1, j);
    }
  }

  Standard_DimensionError_Raise_if(l != B.RowNumber() || n > l, " ");

// Traitement de chaque colonne de A:
    for (i = 1; i <= n; i++) {
      h = scale = 0.0;
        for (k = i; k <= l; k++) {
          qki = Q(k, i);
	  h += qki*qki;                           // = ||a||*||a||     = EUAI
        }
        f = Q(i,i);                               // = a1              = AII
        g = f<1.e-15 ? Sqrt(h) : -Sqrt(h);
	if (fabs(g) <= EPS) {
	  Done = Standard_False;
	  return;
	}
        h -= f*g;                                 // = (v*v)/2         = C1
        alfaii = g-f;                             // = v               = ALFAII
        for (j =i+1; j <= n; j++) {
     	  scale = 0.0;
	  for (k = i; k <= l; k++) {
	   scale += Q(k,i)* Q(k,j);                //                   = SCAL
	  }
	  cj = (g*Q(i,j) - scale)/h;
	  Q(i,j) = Q(i,j) - alfaii*cj;
	  for(k= i+1; k <= l; k++) {
	    Q(k,j) = Q(k, j) + cj * Q(k,i);
	  }
        }

// Modification de B:

        for (j = 1; j <= m; j++) {
          scale= Q(i,i)*B2(i,j);
          for (k = i+1; k <= l; k++) {
	    scale += Q(k,i)*B2(k,j);
	  }
	  cj = (g*B2(i,j) - scale)/h;
	  B2(i,j) = B2(i,j) - cj*alfaii;
	  for (k = i+1; k <= l; k++) {
	    B2(k,j) = B2(k,j) + cj*Q(k,i);
	  }
        }
        Q(i,i) = g;
      }


  // Remontee:
  for (j = 1; j <= m; j++) {
    Sol(n,j) = B2(n,j)/Q(n,n);
    for (i = n -1; i >=1; i--) {
      scale= 0.0;
      for(k = i+1; k <= n; k++) {
	scale += Q(i,k) * Sol(k,j);
      }
      Sol(i,j) = (B2(i,j) - scale)/Q(i,i);
    }
  }
  Done = Standard_True;
}




void math_Householder::Dump(Standard_OStream& o) const {

  o <<"math_Householder ";
   if (Done) {
     o << " Status = Done \n";
   }
   else {
     o << "Status = not Done \n";
   }
}

