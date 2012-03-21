// Created on: 1996-01-22
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
