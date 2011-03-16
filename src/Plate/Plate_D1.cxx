// File:	Plate_D1.cxx
// Created:	Fri Oct 20 20:00:09 1995
// Author:	Andre LIEUTIER
//		<ds@sgi63>


#include <Plate_D1.ixx>
 
Plate_D1::Plate_D1(const gp_XYZ& du, const gp_XYZ& dv)
:Du(du),Dv(dv)
{
}


Plate_D1::Plate_D1(const Plate_D1& ref)
:Du(ref.Du),Dv(ref.Dv)
{
}
