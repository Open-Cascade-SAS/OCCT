// Copyright (c) 1997-1999 Matra Datavision
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

// lpa le 20/08/91

//#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <math_Crout.ixx>
#include <math_NotSquare.hxx>
#include <StdFail_NotDone.hxx>
#include <math_Vector.hxx>

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







