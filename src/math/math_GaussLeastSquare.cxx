//static const char* sccsid = "@(#)math_GaussLeastSquare.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.

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


