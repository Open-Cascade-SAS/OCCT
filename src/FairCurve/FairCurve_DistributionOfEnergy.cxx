// File:	FairCurve_DistributionOfEnergy.cxx
// Created:	Mon Jan 22 15:11:20 1996
// Author:	Philippe MANGIN

#include <FairCurve_DistributionOfEnergy.ixx>

Standard_Integer FairCurve_DistributionOfEnergy::NbVariables() const 
{
 return MyNbVar;
}

Standard_Integer FairCurve_DistributionOfEnergy::NbEquations() const 
{
 return MyNbEqua;
}

 FairCurve_DistributionOfEnergy::FairCurve_DistributionOfEnergy(const Standard_Integer BSplOrder, 
								const Handle(TColStd_HArray1OfReal)& FlatKnots, 
								const Handle(TColgp_HArray1OfPnt2d)& Poles, 
								const Standard_Integer DerivativeOrder,
								const Standard_Integer NbValAux)
                               : MyBSplOrder (BSplOrder), 
				 MyFlatKnots (FlatKnots), 
                                 MyPoles (Poles), 
				 MyDerivativeOrder (DerivativeOrder),
				 MyNbValAux (NbValAux)
{
  MyNbVar = 1;
  SetDerivativeOrder( DerivativeOrder);

}

void FairCurve_DistributionOfEnergy::SetDerivativeOrder(const Standard_Integer DerivativeOrder)
{
  MyNbEqua = 1;
  if (DerivativeOrder>= 1) MyNbEqua +=  2*MyPoles->Length() + MyNbValAux;
  if (DerivativeOrder>= 2) MyNbEqua = MyNbEqua + (MyNbEqua-1)*MyNbEqua / 2;
  MyDerivativeOrder = DerivativeOrder; 
}
