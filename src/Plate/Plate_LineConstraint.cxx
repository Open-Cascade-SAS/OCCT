// File:	Plate_LineConstraint.cxx
// Created:	Mon May 18 16:42:59 1998
// Author:	Andre LIEUTIER
//		<alr@sgi63>


#include <Plate_LineConstraint.ixx>

Plate_LineConstraint::Plate_LineConstraint(const gp_XY& point2d,
					     const gp_Lin& lin,
					     const Standard_Integer iu,
					     const Standard_Integer iv)
:myLSC(2,1)
{
  gp_XYZ point = lin.Location().XYZ();
  myLSC.SetPPC(1,Plate_PinpointConstraint(point2d,point,iu,iv));

  gp_XYZ dir = lin.Direction().XYZ();
  // one builds two directions orthogonal to dir
  gp_XYZ dX(1,0,0);
  gp_XYZ dY(0,1,0);

  gp_XYZ d1 = dX ^ dir;
  gp_XYZ d2 = dY ^ dir;
  if(d2.SquareModulus() > d1.SquareModulus()) d1 = d2;
  d1.Normalize();
  d2 = dir ^ d1;
  d2.Normalize();
  myLSC.SetCoeff(1,1,d1);
  myLSC.SetCoeff(2,1,d2);
}
