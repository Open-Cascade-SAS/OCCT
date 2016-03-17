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

// 30-01-1996 : PMN Version originale

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif


#include <BSplCLib.hxx>
#include <FairCurve_BattenLaw.hxx>
#include <FairCurve_DistributionOfTension.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

 FairCurve_DistributionOfTension::FairCurve_DistributionOfTension(const Standard_Integer BSplOrder,
								  const Handle(TColStd_HArray1OfReal)& FlatKnots, 
								  const Handle(TColgp_HArray1OfPnt2d)& Poles,
								  const Standard_Integer DerivativeOrder,
								  const Standard_Real LengthSliding, 
								  const FairCurve_BattenLaw& Law, 
								  const Standard_Integer NbValAux,
								  const Standard_Boolean Uniform ) :
                                   FairCurve_DistributionOfEnergy(BSplOrder,  
								  FlatKnots, 
								  Poles, 
								  DerivativeOrder,
								  NbValAux) ,
								  MyLengthSliding (LengthSliding),  
								  MyLaw (Law)
{
 if (Uniform) {MyLaw.Value(0.5, MyHeight);} // it used in MVC to avoid Parametrization Problemes
 else         {MyHeight = 0;}
}



Standard_Boolean FairCurve_DistributionOfTension::Value(const math_Vector& TParam, math_Vector& FTension)
{
  Standard_Boolean Ok = Standard_True;
  Standard_Integer ier, ii, jj, kk;
  gp_XY CPrim  (0., 0.);
  Standard_Integer  LastGradientIndex, FirstNonZero, LastZero; 

  // (0.0) initialisations generales
  FTension.Init(0.0);
  math_Matrix Base(1, 3, 1, MyBSplOrder ); // On shouhaite utiliser la derive premieres
                                           // Dans EvalBsplineBasis C' <=> DerivOrder = 2
                                           // et il faut ajouter 1 rang dans la matrice Base => 3 rang
   
  ier  =  BSplCLib::EvalBsplineBasis(1,  MyBSplOrder, 
                                    MyFlatKnots->Array1(), TParam(TParam.Lower()),
                                    FirstNonZero, Base );
  if (ier != 0) return Standard_False;
  LastZero = FirstNonZero - 1;
  FirstNonZero = 2*LastZero+1;

  // (0.1) evaluation de CPrim
  for (ii= 1; ii<= MyBSplOrder; ii++) {
      CPrim += Base(2, ii) * MyPoles->Value(ii+LastZero).Coord();
      }

  // (1) Evaluation de la tension locale --------------------------------
  Standard_Real NormeCPrim = CPrim.Modulus();
  Standard_Real Hauteur, Difference;

  if (MyHeight > 0) {Hauteur = MyHeight;} // it used in MVC to avoid Parametrization Problemes
  else { 
    Ok = MyLaw.Value (TParam(TParam.Lower()), Hauteur); 
    if (!Ok) return Ok;
  }
  Difference = NormeCPrim - MyLengthSliding;
 
  FTension(FTension.Lower()) = Hauteur * pow(Difference, 2) / MyLengthSliding ;

  if (MyDerivativeOrder >= 1) {
  // (2) Evaluation du gradient de la tension locale ----------------------
      math_Vector GradDifference (1, 2*MyBSplOrder+MyNbValAux);
      Standard_Real Xaux, Yaux, Facteur;

      Xaux = CPrim.X() / NormeCPrim;
      Yaux = CPrim.Y() / NormeCPrim;
      Facteur = 2 * Hauteur * Difference / MyLengthSliding;

      kk = FTension.Lower() + FirstNonZero;
      jj = 1;
      for (ii=1; ii<= MyBSplOrder; ii++) {
	     GradDifference(jj)   = Base(2, ii) * Xaux;
             FTension(kk) = Facteur *  GradDifference(jj);
             jj +=1;
	     GradDifference(jj) = Base(2, ii) * Yaux;
             FTension(kk+1) = Facteur *  GradDifference(jj);
             jj += 1;
             kk += 2;
      }
      if (MyNbValAux == 1) {
         LastGradientIndex = FTension.Lower() + 2*MyPoles->Length() + 1;
         GradDifference( GradDifference.Upper()) =  (1 - pow( NormeCPrim/MyLengthSliding, 2));
         FTension(LastGradientIndex) = Hauteur * GradDifference(GradDifference.Upper());        
       }

      else { LastGradientIndex = FTension.Lower() + 2*MyPoles->Length(); }


      if (MyDerivativeOrder >= 2) { 
   
// (3) Evaluation du Hessien de la tension locale ----------------------

         Standard_Real FacteurX =  Difference * (1-pow(Xaux,2)) / NormeCPrim;
         Standard_Real FacteurY =  Difference * (1-pow(Yaux,2)) / NormeCPrim;
         Standard_Real FacteurXY = - Difference * Xaux*Yaux / NormeCPrim;
         Standard_Real Produit;
         Standard_Integer k1, k2;
   
         Facteur = 2 * Hauteur / MyLengthSliding;

         kk = FirstNonZero;
         k2 = LastGradientIndex + (kk-1)*kk/2;

         for (ii=2; ii<= 2*MyBSplOrder; ii+=2) {
	   k1 = k2+FirstNonZero;
           k2 = k1+kk;
           kk += 2;              
           for (jj=2; jj< ii; jj+=2) {
             Produit =  Base(2, ii/2) *  Base(2, jj/2);

	     FTension(k1) = Facteur * ( GradDifference(ii-1)*GradDifference(jj-1)
                                      + FacteurX * Produit) ;  // derivation en XiXj
             k1++;
	     FTension(k1) = Facteur * ( GradDifference(ii)*GradDifference(jj-1)	
                                      + FacteurXY * Produit); // derivation en YiXj
	     k1++;
	     FTension(k2) = Facteur * ( GradDifference(ii-1)*GradDifference(jj)
                                    + FacteurXY * Produit); // derivation en XiYj
             k2++;
	     FTension(k2) = Facteur * ( GradDifference(ii)*GradDifference(jj)
                                      + FacteurY * Produit); // derivation en YiYj
	     k2++;
	     }
         // cas ou jj = ii : remplisage en triangle
             Produit =  pow (Base(2, ii/2), 2);

	     FTension(k1) = Facteur * ( GradDifference(ii-1)*GradDifference(ii-1)
                                      + FacteurX * Produit) ;  // derivation en XiXi
	     FTension(k2) = Facteur * ( GradDifference(ii)*GradDifference(ii-1)
                                      + FacteurXY * Produit); // derivation en XiYi
             k2++;
	     FTension(k2) = Facteur * ( GradDifference(ii)*GradDifference(ii)
                                      + FacteurY * Produit); // derivation en YiYi
	 }
         if (MyNbValAux == 1) {
           FacteurX = -2*CPrim.X()*Hauteur / pow (MyLengthSliding, 2);
           FacteurY = -2*CPrim.Y()*Hauteur / pow (MyLengthSliding, 2);

           ii = LastGradientIndex-FTension.Lower();
	   kk = LastGradientIndex + (ii-1)*ii/2 +  FirstNonZero;
	   for (ii=1; ii<= MyBSplOrder; ii++) {
             FTension(kk) = FacteurX * Base(2, ii);
             kk ++;
             FTension(kk) = FacteurY * Base(2, ii);
             kk ++;
	   }
           FTension(FTension.Upper()) = 2 * Hauteur * pow (NormeCPrim/MyLengthSliding, 2)
	                              / MyLengthSliding;
	 }
       }
    }
          
// sortie standard           
  return Ok;
}

