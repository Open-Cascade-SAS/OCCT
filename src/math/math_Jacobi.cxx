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

#include <math_Jacobi.ixx>

#include <math_Recipes.hxx>
#include <math_NotSquare.hxx>

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



