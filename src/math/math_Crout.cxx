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

// lpa le 20/08/91

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif

#include <math_Crout.hxx>
#include <math_Matrix.hxx>
#include <math_NotSquare.hxx>
#include <math_Vector.hxx>
#include <Standard_DimensionError.hxx>
#include <StdFail_NotDone.hxx>

math_Crout::math_Crout(const math_Matrix& A, const Standard_Real MinPivot):
                       InvA(1, A.RowNumber(), 1, A.ColNumber()) 
{
  Standard_Integer i,j,k;
  Standard_Integer Nctl = A.RowNumber();
  Standard_Integer lowr = A.LowerRow(), lowc = A.LowerCol();
  Standard_Real scale;

  math_Matrix L(1, Nctl, 1, Nctl);
  math_Vector Diag(1, Nctl);


  
  math_NotSquare_Raise_if(Nctl != A.ColNumber(), " ");
  
  Det = 1;
  for (i =1; i <= Nctl; i++) {
    for (j = 1; j <= i -1; j++) {
      scale = 0.0;
      for (k = 1; k <= j-1; k++) {
	scale += L(i,k)*L(j,k)*Diag(k);
      }
      L(i,j) = (A(i+lowr-1,j+lowc-1)-scale)/Diag(j);
    }
    scale = 0.0;
    for (k = 1; k <= i-1; k++) {
      scale += L(i,k)*L(i,k)*Diag(k);
    }
    Diag(i) = A(i+lowr-1,i+lowc-1)-scale;
    Det *= Diag(i);
    if (Abs(Diag(i)) <= MinPivot) {
      Done = Standard_False;
      return;
    }
    L(i,i) = 1.0;
  }

// Calcul de l inverse de L:
//==========================

  L(1,1) = 1./L(1,1);
  for (i = 2; i <= Nctl; i++) {
    for (k = 1; k <= i-1; k++) {
      scale = 0.0;
      for (j = k; j <= i-1; j++) {
	scale += L(i,j)*L(j,k);
      }
      L(i,k) = -scale/L(i,i);
    }
    L(i,i) = 1./L(i,i);
  }

// Calcul de l inverse de Mat:
//============================

  for (j = 1; j <= Nctl; j++) {
    scale = L(j,j)*L(j,j)/Diag(j);
    for (k = j+1; k <= Nctl; k++) {
      scale += L(k,j) *L(k,j)/Diag(k);
    }
    InvA(j,j) = scale;
    for (i = j+1; i <= Nctl; i++) {
      scale = L(i,j) *L(i,i)/Diag(i);
      for (k = i+1; k <= Nctl; k++) {
	scale += L(k,j)*L(k,i)/Diag(k);
      }
      InvA(i,j) = scale;
    }
  }
  Done = Standard_True;
}



void math_Crout::Solve(const math_Vector& B, math_Vector& X) const 
{
  StdFail_NotDone_Raise_if(!Done, " ");
  Standard_DimensionError_Raise_if((B.Length() != InvA.RowNumber()) ||
				   (X.Length() != B.Length()), " ");
  
  Standard_Integer n = InvA.RowNumber();
  Standard_Integer lowb = B.Lower(), lowx = X.Lower();
  Standard_Integer i, j;

  for (i = 1; i <= n; i++) {
    X(i+lowx-1) = InvA(i, 1)*B(1+lowb-1);
    for ( j = 2; j <= i; j++) {
      X(i+lowx-1) += InvA(i, j)*B(j+lowb-1);
    }
    for (j = i+1; j <= n; j++) {
      X(i+lowx-1) += InvA(j,i)*B(j+lowb-1);
    }
  }
}

void math_Crout::Dump(Standard_OStream& o) const 
{
  o << "math_Crout ";
  if(Done) {
    o << " Status = Done \n";
  }
  else {
    o << " Status = not Done \n";
  }
}







