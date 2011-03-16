// File:	Plate_PlaneConstraint.cxx
// Created:	Thu May  7 15:12:31 1998
// Author:	Andre LIEUTIER
//		<alr@sgi63>


#include <Plate_PlaneConstraint.ixx>
#include <Plate_PinpointConstraint.hxx>


Plate_PlaneConstraint::Plate_PlaneConstraint(const gp_XY& point2d,
					     const gp_Pln& pln,
					     const Standard_Integer iu,
					     const Standard_Integer iv)
:myLSC(1,1)
{
  gp_XYZ point = pln.Location().XYZ();
  myLSC.SetPPC(1,Plate_PinpointConstraint(point2d,point,iu,iv));
  gp_XYZ dir = pln.Axis().Direction().XYZ();
  dir.Normalize();
  myLSC.SetCoeff(1,1,dir);
}
