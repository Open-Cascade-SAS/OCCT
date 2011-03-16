#include <IntSurf_InteriorPoint.ixx>


IntSurf_InteriorPoint::IntSurf_InteriorPoint () {}


IntSurf_InteriorPoint::IntSurf_InteriorPoint (const gp_Pnt& P,
					      const Standard_Real U,
					      const Standard_Real V,
					      const gp_Vec& Direc,
					      const gp_Vec2d& Direc2d):
  point(P),paramu(U),paramv(V),direc(Direc),direc2d(Direc2d)

{}

void IntSurf_InteriorPoint::SetValue (const gp_Pnt& P,
				      const Standard_Real U,
				      const Standard_Real V,
				      const gp_Vec& Direc,
				      const gp_Vec2d& Direc2d) {

  point   = P;
  paramu  = U;
  paramv  = V;
  direc   = Direc;
  direc2d = Direc2d;
}



