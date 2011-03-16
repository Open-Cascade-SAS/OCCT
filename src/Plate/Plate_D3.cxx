// File:	Plate_D3.cxx
// Created:	Feb 17 1998
// Author:	Andre LIEUTIER
//		<ds@sgi63>


#include <Plate_D3.ixx>

Plate_D3::Plate_D3(const gp_XYZ& duuu, const gp_XYZ& duuv, const gp_XYZ& duvv, const gp_XYZ& dvvv)
:Duuu(duuu),Duuv(duuv),Duvv(duvv),Dvvv(dvvv)
{
}

Plate_D3::Plate_D3(const Plate_D3& ref)
:Duuu(ref.Duuu),Duuv(ref.Duuv),Duvv(ref.Duvv),Dvvv(ref.Dvvv)
{
}
