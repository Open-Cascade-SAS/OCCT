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

#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_NotSquare.hxx>
#include <math_Recipes.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>

math_Gauss::math_Gauss(const math_Matrix& A, 
                           const Standard_Real MinPivot) 
                           : LU   (1, A.RowNumber(), 1, A.ColNumber()),
                             Index(1, A.RowNumber()) {

      math_NotSquare_Raise_if(A.RowNumber() != A.ColNumber(), " ");     
      LU = A;
      Standard_Integer Error = LU_Decompose(LU, 
                                  Index, 
                                  D,
                                  MinPivot);
      if(!Error) {
        Done = Standard_True;
      }
      else {
        Done = Standard_False;
      }
    }

    void  math_Gauss::Solve(const math_Vector& B, math_Vector& X) const{

       StdFail_NotDone_Raise_if(!Done, " ");

       X = B;
       LU_Solve(LU,
                Index,
                X);
    }

    void math_Gauss::Solve (math_Vector& X) const{

       StdFail_NotDone_Raise_if(!Done, " ");

       if(X.Length() != LU.RowNumber()) {
         throw Standard_DimensionError();
       }
       LU_Solve(LU,
                Index,
                X);
    }

    Standard_Real math_Gauss::Determinant() const{

       StdFail_NotDone_Raise_if(!Done, " ");

       Standard_Real Result = D;
       for(Standard_Integer J = 1; J <= LU.UpperRow(); J++) {
         Result *= LU(J,J);
       }
       return Result;
    }

    void math_Gauss::Invert(math_Matrix& Inv) const{

       StdFail_NotDone_Raise_if(!Done, " ");

       Standard_DimensionError_Raise_if((Inv.RowNumber() != LU.RowNumber()) ||
					(Inv.ColNumber() != LU.ColNumber()),
					" ");

       Standard_Integer LowerRow = Inv.LowerRow();
       Standard_Integer LowerCol = Inv.LowerCol();
       math_Vector Column(1, LU.UpperRow());

       Standard_Integer I, J;
       for(J = 1; J <= LU.UpperRow(); J++) {
         for(I = 1; I <= LU.UpperRow(); I++) {
           Column(I) = 0.0;
         }
         Column(J) = 1.0;
         LU_Solve(LU, Index, Column);
         for(I = 1; I <= LU.RowNumber(); I++) {
           Inv(I+LowerRow-1,J+LowerCol-1) = Column(I);
         }
       }

    }


void math_Gauss::Dump(Standard_OStream& o) const {
       o << "math_Gauss ";
       if(Done) {
         o<< " Status = Done \n";
	 o << " Determinant of A = " << D << endl;
       }
       else {
         o << " Status = not Done \n";
       }
    }




