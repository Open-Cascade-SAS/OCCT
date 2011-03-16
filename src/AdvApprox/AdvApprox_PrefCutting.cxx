// File:	AdvApprox_PrefCutting.cxx
// Created:	Wed Jul  3 15:34:08 1996
// Author:	Joelle CHAUVET
//		<jct@sgi38>


#include <AdvApprox_PrefCutting.ixx>
#include <Precision.hxx>

AdvApprox_PrefCutting::AdvApprox_PrefCutting(const TColStd_Array1OfReal& CutPnts):
  myPntOfCutting(1, CutPnts.Length()) 
{
  myPntOfCutting = CutPnts;
}

Standard_Boolean AdvApprox_PrefCutting::Value(const Standard_Real a,
					      const Standard_Real b,
					      Standard_Real& cuttingvalue) const
{
//  longueur minimum d'un intervalle parametrique : PConfusion()
//                                    pour F(U,V) : EPS1=1.e-9 (cf.MMEPS1)
  Standard_Real lgmin = 10 * Precision::PConfusion();
  Standard_Integer i;
  Standard_Real cut, mil=(a+b)/2,
                dist = Abs((a-b)/2);
  cut = mil;
  for ( i=myPntOfCutting.Lower(); i<= myPntOfCutting.Upper(); i++) {
    if ((dist-lgmin) > Abs(mil-myPntOfCutting.Value(i))) {
      cut = myPntOfCutting.Value(i);
      dist = Abs(mil-myPntOfCutting.Value(i));
    }
  }
  cuttingvalue = cut;
  return (Abs(cut-a)>=lgmin && Abs(b-cut)>=lgmin);
}
