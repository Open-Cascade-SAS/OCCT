// Created on: 1996-01-22
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <FairCurve_DistributionOfEnergy.hxx>

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
