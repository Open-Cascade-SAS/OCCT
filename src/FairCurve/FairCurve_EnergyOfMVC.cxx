// Created on: 1996-04-01
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


#include <FairCurve_EnergyOfMVC.ixx>

#include <math_IntegerVector.hxx>
#include <math_GaussSetIntegration.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>

//=====================================================================================
FairCurve_EnergyOfMVC::FairCurve_EnergyOfMVC(const Standard_Integer BSplOrder,
					     const Handle(TColStd_HArray1OfReal)& FlatKnots,
					     const Handle(TColgp_HArray1OfPnt2d)& Poles,
					     const Standard_Integer ContrOrder1,
					     const Standard_Integer ContrOrder2,
					     const FairCurve_BattenLaw& Law,
					     const Standard_Real PhysicalRatio,
					     const Standard_Real LengthSliding,
					     const Standard_Boolean FreeSliding,
					     const Standard_Real Angle1,
					     const Standard_Real Angle2,
					     const Standard_Real Curvature1,
					     const Standard_Real Curvature2 )
//=====================================================================================
                         : FairCurve_Energy( Poles, ContrOrder1,  ContrOrder2,
				             FreeSliding,  Angle1, Angle2, 
					     BSplOrder-1, Curvature1, Curvature2),
                           MyLengthSliding(LengthSliding),
			   OriginalSliding(LengthSliding),
			   MyFreeSliding(FreeSliding),
                           MyBattenLaw(Law), 
                           MyPhysicalRatio(PhysicalRatio),
                           MyTension(BSplOrder, FlatKnots, Poles, 1, LengthSliding,  Law, FreeSliding, Standard_True),
                           MySagging(BSplOrder, FlatKnots, Poles, 1, Law, FreeSliding),
			   MyJerk(   BSplOrder, FlatKnots, Poles, 1, Law, FreeSliding)
{
  Standard_DomainError_Raise_if(PhysicalRatio < 0 || PhysicalRatio > 1, 
				"FairCurve_EnergyOfMVC: PhysicalRatio error" );
}


//=====================================================================================
void FairCurve_EnergyOfMVC::ComputePoles(const math_Vector& X)
//=====================================================================================
{
 FairCurve_Energy::ComputePoles(X);
 if (MyWithAuxValue) { MyLengthSliding = X(X.Upper()); }
}


//=====================================================================================
Standard_Boolean FairCurve_EnergyOfMVC::Variable(math_Vector& X) const 
//=====================================================================================
{
 Standard_Boolean Ok;
 Ok = FairCurve_Energy::Variable(X);
 if (MyWithAuxValue) { X(X.Upper()) = MyLengthSliding; }
 return Ok;
}


//=====================================================================================
Standard_Boolean FairCurve_EnergyOfMVC::Compute(const Standard_Integer DerivativeOrder,
						      math_Vector& Result)
//=====================================================================================
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
   if ( MyLengthSliding < OriginalSliding/100  )  {
     MyLengthSliding =  OriginalSliding/100;
   }

// Mise a jour des objets sous-fonction
  MyTension.SetDerivativeOrder(DerivativeOrder);
  MyTension.SetLengthSliding(MyLengthSliding);
  MySagging.SetDerivativeOrder(DerivativeOrder);
  MyJerk.SetDerivativeOrder(DerivativeOrder);
  MyBattenLaw.SetSliding(MyLengthSliding);

//  Integrations
  
  // on decoupe afin d'avoir au moins 2 points d'integration par poles
  // 24 points de Gauss => 12 poles maximum.

  Standard_Integer NbInterv = (MyPoles->Length()-1) / 12 + 1, ii;
  Standard_Real Delta = 1./ NbInterv;
  Result.Init(0);

  if (MyPhysicalRatio <= 1.e-12) {

 // Cas purement non physique --------------------------

    for (ii=1; ii<=NbInterv; ii++) {
      Debut(1) = (ii-1)*Delta;
      Fin(1) = ii*Delta;
    
      math_GaussSetIntegration SumTension(MyTension, Debut, Fin, MyOrder); 
      Ok = SumTension.IsDone();
      if (!Ok) return Ok;

      math_GaussSetIntegration SumJerk(MyJerk, Debut, Fin, MyOrder);
      Ok = SumJerk.IsDone();
      if (!Ok) return Ok;

      Result += SumJerk.Value() + SumTension.Value(); // Cas purement non physique
    }
  }
  else {
 // Cas mixte  --------------------------
    for (ii=1; ii<=NbInterv; ii++) {
      Debut(1) = (ii-1)*Delta;
      Fin(1) = ii*Delta;
    
      math_GaussSetIntegration SumTension(MyTension, Debut, Fin, MyOrder); 
      Ok = SumTension.IsDone();
      if (!Ok) return Ok;

      math_GaussSetIntegration SumSagging(MySagging, Debut, Fin, MyOrder);
      Ok = SumSagging.IsDone();
      if (!Ok) return Ok;

      math_GaussSetIntegration SumJerk(MyJerk, Debut, Fin, MyOrder);
      Ok = SumJerk.IsDone();
      if (!Ok) return Ok;

      Result += SumJerk.Value()    * (1-MyPhysicalRatio) 
	      + SumSagging.Value() *   MyPhysicalRatio
	      + SumTension.Value();
    }
  }
   
  return Ok;
}
