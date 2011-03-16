//static const char* sccsid = "@(#)math_FunctionSample.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.

//#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <math_FunctionSample.ixx>


#include <Standard_OutOfRange.hxx>


  math_FunctionSample::math_FunctionSample (const Standard_Real A,
					    const Standard_Real B,
					    const Standard_Integer N):
  a(A),b(B),n(N)
{
}

void math_FunctionSample::Bounds (Standard_Real& A, Standard_Real& B) const {

  A=a;
  B=b;
}


Standard_Integer math_FunctionSample::NbPoints () const {
  return n;
}


Standard_Real math_FunctionSample::GetParameter (const Standard_Integer Index) const {
  Standard_OutOfRange_Raise_if((Index <= 0)||(Index > n), " ");
  return ((n-Index)*a+(Index-1)*b)/(n-1);
}

