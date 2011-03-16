// File:	GeomLib_LogSample.cxx
// Created:	Wed Sep 23 16:56:07 1998
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <GeomLib_LogSample.ixx>

#include <Standard_OutOfRange.hxx>


 GeomLib_LogSample::GeomLib_LogSample(const Standard_Real A,
				      const Standard_Real B,
				      const Standard_Integer N)
                   :math_FunctionSample(A, B, N)
{
  myF = A - 1;
  myexp = Log(B-A)/N;
}

Standard_Real GeomLib_LogSample::GetParameter(const Standard_Integer Index) const
{
  Standard_Integer n = NbPoints();
 

  if ((Index >= n) || (Index <= 1)) {
    Standard_Real a, b;
    Bounds(a, b);
    if (Index == 1) return a;
    else if (Index == n) return b;
    else Standard_OutOfRange::Raise("GeomLib_LogSample::GetParameter");
  }

  Standard_Real v = myF + Exp(myexp*Index);
  return v;
}
