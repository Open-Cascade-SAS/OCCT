//static const char* sccsid = "@(#)math_SVD.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.

//#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <math_SVD.ixx>
#include <math_Recipes.hxx>


#include <StdFail_NotDone.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_NotImplemented.hxx>


math_SVD::math_SVD (const math_Matrix& A) :
   U    (1, Max(A.RowNumber(),A.ColNumber()), 1, A.ColNumber()) ,
   V    (1, A.ColNumber(), 1, A.ColNumber()),
   Diag (1, A.ColNumber())
{
   U.Init(0.0);
   RowA = A.RowNumber() ;
   U.Set(1, A.RowNumber(), 1, A.ColNumber(), A) ;
   Standard_Integer Error = SVD_Decompose(U, Diag, V) ;
   Done = (!Error) ? Standard_True : Standard_False ;
}

void math_SVD::Solve (const math_Vector& B,
		      math_Vector& X,
		      const Standard_Real Eps) const
{
   StdFail_NotDone_Raise_if(!Done, " ");
   Standard_DimensionError_Raise_if((RowA != B.Length()) ||
				    (X.Length() != Diag.Length()), " ");

   math_Vector BB (1, U.RowNumber()) ;
   BB.Init(0.0);
   BB.Set (1, B.Length(), B) ;
   Standard_Real wmin = Eps * Diag(Diag.Max());
   for (Standard_Integer I = 1; I <= Diag.Upper(); I++)
   {
      if (Diag(I) < wmin) Diag(I) = 0.0 ;
   }
   SVD_Solve (U, Diag, V, BB, X) ;
}

void  math_SVD::PseudoInverse (math_Matrix& Result,
			       const Standard_Real Eps) const
{
   Standard_Integer i, j ;

   StdFail_NotDone_Raise_if(!Done, " ");

   Standard_Real wmin = Eps * Diag(Diag.Max());
   for (i=1; i<=Diag.Upper(); i++)
   {
      if (Diag(i) < wmin) Diag(i) = 0.0 ;
   }

   Standard_Integer ColA = Diag.Length () ;
   math_Vector VNorme (1, U.RowNumber()) ;
   math_Vector Column (1, ColA) ;

   for (j=1; j<=RowA; j++)
   {
      for (i=1; i<=VNorme.Upper(); i++) VNorme(i) = 0.0 ;
      VNorme(j) = 1.0 ;
      SVD_Solve (U, Diag, V, VNorme, Column) ;
      for (i=1; i<=ColA; i++) Result(i,j) = Column(i) ;
   }
}


void math_SVD::Dump(Standard_OStream& o) const {

  o << "math_SVD";
  if(Done) {
    o << " Status = Done \n";
  }
  else {
    o << " Status = not Done \n";
  }
}

