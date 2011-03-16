#include <Extrema_POnSurf.ixx>

Extrema_POnSurf::Extrema_POnSurf () {}

Extrema_POnSurf::Extrema_POnSurf ( const Standard_Real U, const Standard_Real V, const gp_Pnt& P) 
{
  myU = U;
  myV = V;
  myP = P;
}

