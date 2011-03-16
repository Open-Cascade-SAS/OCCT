// File:	Plate_D2.cxx
// Created:	Fri Oct 20 20:01:38 1995
// Author:	Andre LIEUTIER
//		<ds@sgi63>


#include <Plate_D2.ixx>

Plate_D2::Plate_D2(const gp_XYZ& duu, const gp_XYZ& duv, const gp_XYZ& dvv)
:Duu(duu),Duv(duv),Dvv(dvv)
{
}

Plate_D2::Plate_D2(const Plate_D2& ref)
:Duu(ref.Duu),Duv(ref.Duv),Dvv(ref.Dvv)
{
}
