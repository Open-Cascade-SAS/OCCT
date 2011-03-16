#ifndef math_GaussPoints_HeaderFile
#define math_GaussPoints_HeaderFile
#include <math_Vector.hxx>
#include <Standard_Real.hxx>


extern const Standard_Real Point[157];
extern const Standard_Real Weight[157];

math_Vector GPoints(const Standard_Integer Index);

math_Vector GWeights(const Standard_Integer Index);

#endif
