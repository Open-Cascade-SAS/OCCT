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

#include <math_Jacobi.hxx>
#include <math_Matrix.hxx>
#include <math_NotSquare.hxx>
#include <math_Recipes.hxx>
#include <StdFail_NotDone.hxx>

math_Jacobi::math_Jacobi(const math_Matrix& A) : AA(1, A.RowNumber(), 
						      1, A.RowNumber()),
                                                EigenValues(1, A.RowNumber()),
                                                EigenVectors(1, A.RowNumber(),
						   1, A.RowNumber()) {

    math_NotSquare_Raise_if(A.RowNumber() != A.ColNumber(), " ");  

    AA = A;
    Standard_Integer Error = Jacobi(AA, EigenValues, EigenVectors, NbRotations);      
    if(!Error) {
      Done = Standard_True;
    }
    else {
      Done = Standard_False;
    }
  }

void math_Jacobi::Dump(Standard_OStream& o) const {

  o <<"math_Jacobi ";
   if (Done) {
     o << " Status = Done \n";
     o << " The eigenvalues vector is: " << EigenValues << endl;
   }
   else {
     o << "Status = not Done \n";
   }
}



