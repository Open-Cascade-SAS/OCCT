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

//#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <math_GaussLeastSquare.ixx>
#include <math_Recipes.hxx>

#include <StdFail_NotDone.hxx> 
#include <Standard_DimensionError.hxx>


math_GaussLeastSquare::math_GaussLeastSquare (const math_Matrix& A,
		       		      const Standard_Real MinPivot) :
                                      LU(1, A.ColNumber(),
					 1, A.ColNumber()),
                                      A2(1, A.ColNumber(),
					 1, A.RowNumber()),
                                      Index(1, A.ColNumber()) {
  A2 = A.Transposed();					
  LU.Multiply(A2, A);

  Standard_Integer Error = LU_Decompose(LU, Index, D, MinPivot);
  Done = (!Error) ? Standard_True : Standard_False;

}

void math_GaussLeastSquare::Solve(const math_Vector& B, math_Vector& X) const{
  StdFail_NotDone_Raise_if(!Done, " ");
  Standard_DimensionError_Raise_if((B.Length() != A2.ColNumber()) ||
				   (X.Length() != A2.RowNumber()), " ");

  X.Multiply(A2, B);

  LU_Solve(LU, Index, X);

  return;
}


void math_GaussLeastSquare::Dump(Standard_OStream& o) const {

  o <<"math_GaussLeastSquare ";
   if (Done) {
     o << " Status = Done \n";
   }
   else {
     o << "Status = not Done \n";
   }
}


