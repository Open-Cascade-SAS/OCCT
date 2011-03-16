//static const char* sccsid = "@(#)math_Jacobi.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.

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



