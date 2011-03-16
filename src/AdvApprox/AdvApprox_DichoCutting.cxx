// File:	AdvApprox_DichoCutting.cxx
// Created:	Wed Jul  3 15:34:08 1996
// Author:	Joelle CHAUVET
//		<jct@sgi38>


#include <AdvApprox_DichoCutting.ixx>
#include <Precision.hxx>

 AdvApprox_DichoCutting::AdvApprox_DichoCutting()
{
}

Standard_Boolean AdvApprox_DichoCutting::Value(const Standard_Real a,
						const Standard_Real b,
						Standard_Real& cuttingvalue) const
{
//  longueur minimum d'un intervalle pour F(U,V) : EPS1=1.e-9 (cf.MEPS1)
  Standard_Real lgmin = 10*Precision::PConfusion();
  cuttingvalue = (a+b) / 2;
  return (Abs(b-a)>=2*lgmin);
}

