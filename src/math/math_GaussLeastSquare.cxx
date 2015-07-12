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

#include <math_GaussLeastSquare.hxx>
#include <math_Matrix.hxx>
#include <math_Recipes.hxx>
#include <Standard_DimensionError.hxx>
#include <StdFail_NotDone.hxx>

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


