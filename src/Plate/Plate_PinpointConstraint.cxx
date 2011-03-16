// File:	Plate_PinpointConstraint.cxx
// Created:	Fri Oct 20 19:39:17 1995
// Author:	Andre LIEUTIER
//		<ds@sgi63>


#include <Plate_PinpointConstraint.ixx>

Plate_PinpointConstraint::Plate_PinpointConstraint()
{
  pnt2d = gp_XY(0,0);
  value = gp_XYZ(0,0,0);
  idu = 0;
  idv = 0;
}


Plate_PinpointConstraint::Plate_PinpointConstraint(const gp_XY& point2d, const gp_XYZ& ImposedValue,
						   const Standard_Integer iu, const Standard_Integer iv)
{
  pnt2d = point2d;
  value = ImposedValue;
  idu = iu;
  idv = iv;
}
