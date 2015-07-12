// Created on: 1996-02-01
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

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif


#include <FairCurve_BattenLaw.hxx>
#include <FairCurve_EnergyOfBatten.hxx>
#include <math_GaussSetIntegration.hxx>
#include <math_IntegerVector.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>

//=======================================================================
FairCurve_EnergyOfBatten::FairCurve_EnergyOfBatten(const Standard_Integer BSplOrder, 
						   const Handle(TColStd_HArray1OfReal)& FlatKnots,
						   const Handle(TColgp_HArray1OfPnt2d)& Poles, 
						   const Standard_Integer ContrOrder1, 
						   const Standard_Integer ContrOrder2, 
						   const FairCurve_BattenLaw& Law, 
						   const Standard_Real LengthSliding, 
						   const Standard_Boolean FreeSliding, 
						   const Standard_Real Angle1, 
						   const Standard_Real Angle2)
//=======================================================================
                         : FairCurve_Energy( Poles, ContrOrder1,  ContrOrder2,
                                             FreeSliding,  Angle1, Angle2),
                           MyLengthSliding(LengthSliding),
                           OriginalSliding(LengthSliding),
                           MyBattenLaw(Law), 
                           MyTension(BSplOrder, FlatKnots, Poles, 1, LengthSliding,  Law, FreeSliding),
                           MySagging(BSplOrder, FlatKnots, Poles, 1, Law, FreeSliding)
{
}

//=======================================================================
Standard_Boolean FairCurve_EnergyOfBatten::Variable(math_Vector& X) const
//======================================================================= 
{
 Standard_Boolean Ok;
 Ok = FairCurve_Energy::Variable(X);
 if (MyWithAuxValue) { X(X.Upper()) = MyLengthSliding; }
 return Ok;
}  

//=======================================================================
void FairCurve_EnergyOfBatten::ComputePoles(const math_Vector& X)
//======================================================================= 
{
 FairCurve_Energy::ComputePoles(X);
 if (MyWithAuxValue) { MyLengthSliding = X(X.Upper()); }
}

//=======================================================================
Standard_Boolean FairCurve_EnergyOfBatten::Compute(const Standard_Integer DerivativeOrder, 
						   math_Vector& Result)
//=======================================================================
{
  math_Vector Debut(1, 1, 0.),  Fin(1, 1, 1.);
  math_IntegerVector MyOrder(1, 1, 24);
  Standard_Boolean Ok=Standard_False;

// Blindage contre les longueur de glissement trop exotique
  MyStatus = FairCurve_OK;
  if ( MyLengthSliding > 10*OriginalSliding ) {
    MyStatus =  FairCurve_InfiniteSliding;
    return Standard_False;
}

  if ( MyLengthSliding < OriginalSliding/100  ) {
    MyLengthSliding = OriginalSliding/100;
  }

// Mise a jour des objets sous-fonction
  MyTension.SetDerivativeOrder(DerivativeOrder);
  MyTension.SetLengthSliding(MyLengthSliding);
  MySagging.SetDerivativeOrder(DerivativeOrder);
  MyBattenLaw.SetSliding(MyLengthSliding);

//  Integrations
  
  // on decoupe afin d'avoir au moins 2 points d'integration par poles
  // 24 points de Gauss => 12 poles maximum.

  Standard_Integer NbInterv = (MyPoles->Length()-1) / 12 + 1, ii;
  Standard_Real Delta = 1./ NbInterv;
  Result.Init(0);

  for (ii=1; ii<=NbInterv; ii++) {
    Debut(1) = (ii-1)*Delta;
    Fin(1) = ii*Delta;
    
    math_GaussSetIntegration SumTension(MyTension, Debut, Fin, MyOrder); 
    Ok = SumTension.IsDone();
    if (!Ok) return Ok;

    math_GaussSetIntegration SumSagging(MySagging, Debut, Fin, MyOrder);
    Ok = SumSagging.IsDone();

    if (Ok) { Result += SumSagging.Value() + SumTension.Value(); }
  }

  return Ok;
}

