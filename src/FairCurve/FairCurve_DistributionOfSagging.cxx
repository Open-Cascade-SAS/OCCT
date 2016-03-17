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

// 09-02-1996 : PMN Version originale

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif


#include <BSplCLib.hxx>
#include <FairCurve_BattenLaw.hxx>
#include <FairCurve_DistributionOfSagging.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

 FairCurve_DistributionOfSagging::FairCurve_DistributionOfSagging(const Standard_Integer BSplOrder,
								  const Handle(TColStd_HArray1OfReal)& FlatKnots, 
								  const Handle(TColgp_HArray1OfPnt2d)& Poles, 
								  const Standard_Integer DerivativeOrder, 
								  const FairCurve_BattenLaw& Law, 
								  const Standard_Integer NbValAux) :
                                   FairCurve_DistributionOfEnergy(BSplOrder,  
								  FlatKnots, 
								  Poles, 
								  DerivativeOrder,
								  NbValAux) , 
				   MyLaw (Law)								
{
}

Standard_Boolean FairCurve_DistributionOfSagging::Value(const math_Vector& TParam, math_Vector& Flexion)
{  
  Standard_Boolean Ok = Standard_True;
  Standard_Integer ier, ii, jj, kk;
  gp_XY CPrim  (0., 0.), CSecn (0., 0.);
  Standard_Integer  LastGradientIndex, FirstNonZero, LastZero; 

  // (0.0) initialisations generales
  Flexion.Init(0.0);
  math_Matrix Base(1, 4, 1, MyBSplOrder ); // On shouhaite utiliser la derive premieres
                                           // Dans EvalBsplineBasis C" <=> DerivOrder = 3
                                           // et il faut ajouter 1 rang dans la matrice Base => 4 rang
   
  ier  =  BSplCLib::EvalBsplineBasis(2,  MyBSplOrder, 
                                     MyFlatKnots->Array1(), TParam(TParam.Lower()),
                                     FirstNonZero, Base );
  if (ier != 0) return Standard_False;
  LastZero = FirstNonZero - 1;
  FirstNonZero = 2*LastZero+1;


  // (0.1) evaluation de CPrim et CScn
  for (ii= 1; ii<= MyBSplOrder; ii++) {
      CPrim += Base(2, ii) * MyPoles->Value(ii+LastZero).Coord();
      CSecn += Base(3, ii) * MyPoles->Value(ii+LastZero).Coord();
      }

  // (1) Evaluation de la flexion locale = W*W
  Standard_Real NormeCPrim = CPrim.Modulus();
  Standard_Real InvNormeCPrim = 1 / NormeCPrim;
  Standard_Real Hauteur, WVal, Mesure;
  Standard_Real Numerateur = CPrim ^ CSecn; 
  Standard_Real Denominateur = pow ( NormeCPrim, 2.5);

  Ok = MyLaw.Value (TParam(TParam.Lower()), Hauteur);
  if (!Ok) return Ok;

  Mesure =  pow(Hauteur, 3) / 12;
  WVal   =  Numerateur / Denominateur;
  Flexion(Flexion.Lower()) = Mesure * pow(WVal, 2);

  if (MyDerivativeOrder >= 1) {
  // (2) Evaluation du gradient de la flexion locale.

      math_Vector WGrad (1, 2*MyBSplOrder+MyNbValAux), 
                  NumGrad(1, 2*MyBSplOrder+MyNbValAux),
                  GradNormeCPrim(1, 2*MyBSplOrder+MyNbValAux),
                  NumduGrad(1, 2*MyBSplOrder+MyNbValAux);
      Standard_Real Facteur;
      Standard_Real XPrim = CPrim.X();
      Standard_Real YPrim = CPrim.Y();
      Standard_Real XSecn = CSecn.X();
      Standard_Real YSecn = CSecn.Y();
      Standard_Real InvDenominateur = 1 / Denominateur;
      Standard_Real Aux;

      Facteur = 2 * Mesure * WVal;
      Aux = 2.5 * Numerateur * InvNormeCPrim;
      kk = Flexion.Lower() + FirstNonZero;
      

      jj = 1;
      for (ii=1; ii<=MyBSplOrder; ii++) {

   //     (2.1) Derivation en X
             NumGrad(jj) = YSecn * Base(2, ii) - YPrim * Base(3, ii);
             GradNormeCPrim(jj) =  XPrim * Base(2, ii) * InvNormeCPrim;
             NumduGrad(jj) =  NumGrad(jj) - Aux * GradNormeCPrim(jj);
	     WGrad(jj)   = InvDenominateur * NumduGrad(jj);
             Flexion(kk) = Facteur *  WGrad(jj);
             jj +=1;

   //     (2.2) Derivation en Y
             NumGrad(jj) =  - XSecn * Base(2, ii) + XPrim * Base(3, ii);
             GradNormeCPrim(jj) = YPrim * Base(2, ii) * InvNormeCPrim;
             NumduGrad(jj) =  NumGrad(jj) - Aux * GradNormeCPrim(jj);
	     WGrad(jj)   = InvDenominateur * NumduGrad(jj);
             Flexion(kk+1) = Facteur *  WGrad(jj);
             jj += 1;
             kk += 2;
      }
      if (MyNbValAux == 1) {
    //    (2.3) Gestion de la variable de glissement
             LastGradientIndex = Flexion.Lower() + 2*MyPoles->Length() + 1;
             WGrad( WGrad.Upper()) = 0.0;        
       }

      else { LastGradientIndex = Flexion.Lower() + 2*MyPoles->Length(); }


      if (MyDerivativeOrder >= 2) { 
   
// (3) Evaluation du Hessien de la tension locale ----------------------

         Standard_Real FacteurX =  (1 - Pow(XPrim * InvNormeCPrim,2)) * InvNormeCPrim;
         Standard_Real FacteurY =  (1 - Pow(YPrim * InvNormeCPrim,2)) * InvNormeCPrim;
         Standard_Real FacteurXY = - (XPrim*InvNormeCPrim)
	                         * (YPrim*InvNormeCPrim) * InvNormeCPrim;
         Standard_Real FacteurW = WVal * InvNormeCPrim;

         Standard_Real Produit, DSeconde, NSeconde;
         Standard_Real VIntermed;
         Standard_Integer k1, k2, II, JJ;
   
         Facteur = 2 * Mesure;

         kk = FirstNonZero;
         k2 = LastGradientIndex + (kk-1)*kk/2;

         for (ii=2; ii<= 2*MyBSplOrder; ii+=2) {
           II = ii/2;
	   k1 = k2+FirstNonZero;
           k2 = k1+kk;
           kk += 2;              
           for (jj=2; jj< ii; jj+=2) {
             JJ = jj/2;
             Produit =  Base(2, II) *  Base(2, JJ);
             NSeconde =   Base(2, II) *  Base(3, JJ)
                        - Base(3, II) *  Base(2, JJ);

	     // derivation en XiXj
             DSeconde =  FacteurX * Produit;
             Aux = NumGrad(ii-1)*GradNormeCPrim(jj-1)
                 - 2.5 * ( NumGrad(jj-1)*GradNormeCPrim(ii-1)
                         + DSeconde * Numerateur );
             VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(jj-1)*NumduGrad(ii-1));

	     Flexion(k1) = Facteur * ( WGrad(ii-1)*WGrad(jj-1)
                                     + FacteurW * VIntermed); 
             k1++;
 
	     // derivation en XiYj
             DSeconde =  FacteurXY * Produit;	
             Aux = NormeCPrim * NSeconde
		 + NumGrad(ii-1)*GradNormeCPrim(jj)
                 - 2.5 * ( NumGrad(jj)*GradNormeCPrim(ii-1)
			 + DSeconde * Numerateur );
             VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(jj)*NumduGrad(ii-1));
	     Flexion(k1) = Facteur * ( WGrad(ii-1)*WGrad(jj)
                                     + FacteurW * VIntermed);
	     k1++;

	     // derivation en YiXj
             // DSeconde calcule ci-dessus
             Aux = - NormeCPrim * NSeconde
		   + NumGrad(ii)*GradNormeCPrim(jj-1)
                   - 2.5 * ( NumGrad(jj-1)*GradNormeCPrim(ii)
			 + DSeconde * Numerateur );
             VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(jj-1)*NumduGrad(ii));

	     Flexion(k2) = Facteur * ( WGrad(ii)*WGrad(jj-1)	
                                     + FacteurW * VIntermed);
             k2++;

	     // derivation en YiYj
             DSeconde =  FacteurY * Produit;
             Aux = NumGrad(ii)*GradNormeCPrim(jj)
                 - 2.5 * ( NumGrad(jj)*GradNormeCPrim(ii)
			 + DSeconde * Numerateur );
             VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(jj)*NumduGrad(ii));
	     Flexion(k2) = Facteur * ( WGrad(ii)*WGrad(jj)
                                     + FacteurW * VIntermed); 
	     k2++;
	     }

         // cas ou jj = ii : remplisage en triangle
             Produit =  pow (Base(2, II), 2);
  	     
	     // derivation en XiXi
             DSeconde = FacteurX * Produit;
             Aux =- 1.5 * NumGrad(ii-1)*GradNormeCPrim(ii-1)
	          - 2.5 * DSeconde * Numerateur;
             VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(ii-1)*NumduGrad(ii-1));
	     Flexion(k1) = Facteur * ( WGrad(ii-1)*WGrad(ii-1)
                                     + FacteurW * VIntermed );
	     // derivation en XiYi
             DSeconde = FacteurXY * Produit;
             Aux = NumGrad(ii-1)*GradNormeCPrim(ii)
                 - 2.5 * ( NumGrad(ii)*GradNormeCPrim(ii-1)
                         + DSeconde * Numerateur );
	     VIntermed = InvDenominateur 
                       * (Aux- 3.5*GradNormeCPrim(ii)*NumduGrad(ii-1));
	     Flexion(k2) = Facteur * ( WGrad(ii)*WGrad(ii-1)
                                     + FacteurW * VIntermed);
             k2++;

	     // derivation en YiYi
             DSeconde = FacteurY * Produit;
             Aux = - 1.5 * NumGrad(ii)*GradNormeCPrim(ii) 
	           - 2.5 * DSeconde * Numerateur; 
	     VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(ii)*NumduGrad(ii));
	     Flexion(k2) = Facteur * ( WGrad(ii)*WGrad(ii)
                                     + FacteurW * VIntermed);
	 }
       }     
  }

// sortie standard           
  return Ok;
  } 

