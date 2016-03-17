// Created on: 1996-04-01
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

// 01-04-1996 : PMN Version originale

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif


#include <BSplCLib.hxx>
#include <FairCurve_BattenLaw.hxx>
#include <FairCurve_DistributionOfJerk.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

FairCurve_DistributionOfJerk::FairCurve_DistributionOfJerk(const Standard_Integer BSplOrder, 
							   const Handle(TColStd_HArray1OfReal)& FlatKnots,
							   const Handle(TColgp_HArray1OfPnt2d)& Poles,
							   const Standard_Integer DerivativeOrder,
							   const FairCurve_BattenLaw& Law,
							   const Standard_Integer NbValAux) :
                                  FairCurve_DistributionOfEnergy( BSplOrder,  
								  FlatKnots, 
								  Poles, 
								  DerivativeOrder,
								  NbValAux) , 
				   MyLaw (Law)								
{
}


Standard_Boolean FairCurve_DistributionOfJerk::Value(const math_Vector& TParam,
						           math_Vector& Jerk)
{
  Standard_Boolean Ok = Standard_True;
  Standard_Integer ier, ii, jj, kk;
  gp_XY CPrim  (0., 0.), CSecn (0., 0.), CTroi(0., 0.);
  Standard_Integer  LastGradientIndex, FirstNonZero, LastZero; 

  // (0.0) initialisations generales
  Jerk.Init(0.0);
  math_Matrix Base(1, 5, 1, MyBSplOrder ); // On shouhaite utiliser la derive troisieme
                                           // Dans EvalBsplineBasis C"' <=> DerivOrder = 4
                                           // et il faut ajouter 1 rang dans la matrice Base => 5 rangs
   
  ier  =  BSplCLib::EvalBsplineBasis(3,  MyBSplOrder, 
                                     MyFlatKnots->Array1(), TParam(TParam.Lower()),
                                     FirstNonZero, Base );
  if (ier != 0) return Standard_False;
  LastZero = FirstNonZero - 1;
  FirstNonZero = 2*LastZero+1;


  // (0.1) evaluation de CPrim et CScn
  for (ii= 1; ii<= MyBSplOrder; ii++) {
      CPrim += Base(2, ii) * MyPoles->Value(ii+LastZero).Coord();
      CSecn += Base(3, ii) * MyPoles->Value(ii+LastZero).Coord();
      CTroi += Base(4, ii) * MyPoles->Value(ii+LastZero).Coord();
      }

  // (1) Evaluation de la secousse locale = W*W
  Standard_Real NormeCPrim = CPrim.Modulus();
  Standard_Real InvNormeCPrim = 1 / NormeCPrim;
  Standard_Real InvNormeCPrim2 = InvNormeCPrim *InvNormeCPrim;
  Standard_Real ProduitC1C2 =  CPrim*CSecn; 
  Standard_Real DeriveNormeCPrim = ProduitC1C2 * InvNormeCPrim2;

  Standard_Real Hauteur, WVal, Mesure;
  Standard_Real NumRho = CPrim ^ CSecn;
  Standard_Real Numerateur = (CPrim ^ CTroi) - 3 * NumRho * DeriveNormeCPrim; 
  Standard_Real Denominateur = pow ( NormeCPrim, 2.5);

  Ok = MyLaw.Value (TParam(TParam.Lower()), Hauteur);
  if (!Ok) return Ok;

  Mesure =  pow(Hauteur, 3) / 12;
  WVal   =  Numerateur / Denominateur;
  Jerk(Jerk.Lower()) = Mesure * pow(WVal, 2);

  if (MyDerivativeOrder >= 1) {
  // (2) Evaluation du gradient de la secousse locale.

      math_Vector WGrad (1, 2*MyBSplOrder+MyNbValAux), 
                  NumGrad(1, 2*MyBSplOrder), 
                  NGrad1(1, 2*MyBSplOrder),
                  NGrad2(1, 2*MyBSplOrder),
                  GradNormeCPrim(1, 2*MyBSplOrder),
                  NGDNCPrim(1, 2*MyBSplOrder),
                  GradDeriveNormeCPrim(1, 2*MyBSplOrder),
                  GradNumRho(1, 2*MyBSplOrder),
                  NumduGrad(1, 2*MyBSplOrder);
      Standard_Real Facteur;
      Standard_Real XPrim = CPrim.X();
      Standard_Real YPrim = CPrim.Y();
      Standard_Real XSecn = CSecn.X();
      Standard_Real YSecn = CSecn.Y();
      Standard_Real XTroi = CTroi.X();
      Standard_Real YTroi = CTroi.Y();
      Standard_Real InvDenominateur = 1 / Denominateur;
      Standard_Real Aux, AuxBis;

      Facteur = 2 * Mesure * WVal;
      Aux = 2.5 * Numerateur * InvNormeCPrim;
      AuxBis = 2 * ProduitC1C2 * InvNormeCPrim;
      kk = Jerk.Lower() + FirstNonZero;
      

      jj = 1;
      for (ii=1; ii<=MyBSplOrder; ii++) {

   //     (2.1) Derivation en X
             GradNormeCPrim(jj) =  XPrim * Base(2, ii) * InvNormeCPrim;
             NGDNCPrim(jj) = (XPrim * Base(3, ii) + XSecn * Base(2, ii)) 
	                   -  AuxBis * GradNormeCPrim(jj);
             GradDeriveNormeCPrim(jj) =  NGDNCPrim(jj) * InvNormeCPrim2;
	     GradNumRho(jj) = YSecn * Base(2, ii) - YPrim * Base(3, ii);
	     NGrad1(jj) = YTroi * Base(2, ii) - YPrim * Base(4, ii);
             NGrad2(jj) =  - 3 * (  NumRho         * GradDeriveNormeCPrim(jj) 
                                  + GradNumRho(jj) * DeriveNormeCPrim);
             NumGrad(jj) =  NGrad1(jj) + NGrad2(jj);
             NumduGrad(jj) =  NumGrad(jj) - Aux * GradNormeCPrim(jj);
	     WGrad(jj)   = InvDenominateur * NumduGrad(jj);
             Jerk(kk) = Facteur *  WGrad(jj);

             jj +=1;

   //     (2.2) Derivation en Y                                                             

             GradNormeCPrim(jj) = YPrim * Base(2, ii) * InvNormeCPrim;
	     NGDNCPrim(jj) = (YPrim * Base(3, ii) + YSecn * Base(2, ii))  
                           -  AuxBis * GradNormeCPrim(jj);
             GradDeriveNormeCPrim(jj) =  NGDNCPrim(jj)  * InvNormeCPrim2;
             GradNumRho(jj) =  - XSecn * Base(2, ii) + XPrim * Base(3, ii);
	     NGrad1(jj) =  - XTroi * Base(2, ii) + XPrim * Base(4, ii);
             NGrad2(jj) =  - 3 * (  NumRho         * GradDeriveNormeCPrim(jj) 
                                 +  GradNumRho(jj) * DeriveNormeCPrim);
             NumGrad(jj) =  NGrad1(jj) + NGrad2(jj);
             NumduGrad(jj) =  NumGrad(jj) - Aux * GradNormeCPrim(jj);
	     WGrad(jj)   = InvDenominateur * NumduGrad(jj);
             Jerk(kk+1) = Facteur *  WGrad(jj);

             jj += 1;
             kk += 2;
      }
      if (MyNbValAux == 1) {
    //    (2.3) Gestion de la variable de glissement
             LastGradientIndex = Jerk.Lower() + 2*MyPoles->Length() + 1;
             WGrad( WGrad.Upper()) = 0.0; 
             Jerk(LastGradientIndex) = 0.0;       
       }

      else { LastGradientIndex = Jerk.Lower() + 2*MyPoles->Length(); }


      if (MyDerivativeOrder >= 2) { 
   
// (3) Evaluation du Hessien de la tension locale ---------------------- 


         Standard_Real FacteurX =  (1 - Pow(XPrim * InvNormeCPrim,2)) * InvNormeCPrim;
         Standard_Real FacteurY =  (1 - Pow(YPrim * InvNormeCPrim,2)) * InvNormeCPrim;
         Standard_Real FacteurXY = - (XPrim*InvNormeCPrim)
	                         * (YPrim*InvNormeCPrim) * InvNormeCPrim;
         Standard_Real FacteurW = WVal * InvNormeCPrim;

         Standard_Real Produit, Produit2, ProduitV, HNumRho, DSeconde, NSeconde, 
	               HDeriveNormeCPrim, Aux1, DeriveAuxBis;
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
             Produit2 = Base(2, II) *  Base(3, JJ) +  Base(3, II) *  Base(2, JJ);           
             ProduitV =   Base(2, II) *  Base(4, JJ)
                        - Base(4, II) *  Base(2, JJ);
             HNumRho  =   Base(2, II) *  Base(3, JJ)
                        - Base(3, II) *  Base(2, JJ);

	     // derivation en XiXj
             Aux1 =  InvNormeCPrim2 * GradNormeCPrim(jj-1);
             DeriveAuxBis = 2 * ( (XPrim * Base(3, JJ) + XSecn * Base(2, JJ))*InvNormeCPrim
			         - ProduitC1C2 * Aux1);
	     HDeriveNormeCPrim =  ( Produit2 - DeriveAuxBis*GradNormeCPrim(ii-1) 
                                  - AuxBis * ( Produit * InvNormeCPrim 
                                  - XPrim * Base(2, II)*Aux1) )*InvNormeCPrim2
                               - 2*NGDNCPrim(ii-1) * Aux1 *InvNormeCPrim;
             NSeconde = - 3 * (
		        GradNumRho(ii-1) * GradDeriveNormeCPrim(jj-1)
		      + NumRho * HDeriveNormeCPrim
                      + GradNumRho(jj-1) * GradDeriveNormeCPrim(ii-1) );

             DSeconde =  FacteurX * Produit;
             Aux = NormeCPrim * NSeconde
	         + NumGrad(ii-1)*GradNormeCPrim(jj-1)
                 - 2.5 * ( NumGrad(jj-1)*GradNormeCPrim(ii-1)
                         + DSeconde * Numerateur );
             VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(jj-1)*NumduGrad(ii-1));

	     Jerk(k1) = Facteur * ( WGrad(ii-1)*WGrad(jj-1)
                                  + FacteurW * VIntermed); 
             k1++;
 
	     // derivation en XiYj
             Aux1 =  InvNormeCPrim2 * GradNormeCPrim(jj);
             DeriveAuxBis = 2 * ( (YPrim * Base(3, JJ) + YSecn * Base(2, JJ))*InvNormeCPrim
			         - ProduitC1C2 * Aux1);
	     HDeriveNormeCPrim =  (- DeriveAuxBis*GradNormeCPrim(ii-1) 
                                   + AuxBis *  XPrim * Base(2, II) * Aux1) * InvNormeCPrim2
                               - 2*NGDNCPrim(ii-1)*Aux1*InvNormeCPrim;
             NSeconde = ProduitV 
                      - 3 * (
		        GradNumRho(ii-1) * GradDeriveNormeCPrim(jj)
		      + NumRho * HDeriveNormeCPrim
                      + GradNumRho(jj) * GradDeriveNormeCPrim(ii-1)
		      + HNumRho * DeriveNormeCPrim );
             DSeconde =  FacteurXY * Produit;	
             Aux = NormeCPrim * NSeconde
		 + NumGrad(ii-1)*GradNormeCPrim(jj)
                 - 2.5 * ( NumGrad(jj)*GradNormeCPrim(ii-1)
			 + DSeconde * Numerateur );
             VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(jj)*NumduGrad(ii-1));
	     Jerk(k1) = Facteur * ( WGrad(ii-1)*WGrad(jj)
                                     + FacteurW * VIntermed);
	     k1++;

	     // derivation en YiXj
             // DSeconde calcule ci-dessus
             Aux1 =  InvNormeCPrim2 * GradNormeCPrim(jj-1);
             DeriveAuxBis = 2 * ( (XPrim * Base(3, JJ) + XSecn * Base(2, JJ))*InvNormeCPrim
			         - ProduitC1C2 * Aux1);
	     HDeriveNormeCPrim =  (- DeriveAuxBis*GradNormeCPrim(ii) 
                                  + AuxBis * YPrim * Base(2, II)*Aux1) * InvNormeCPrim2
                               - 2*NGDNCPrim(ii)*Aux1 *InvNormeCPrim;
             NSeconde = - ProduitV 
                      - 3 * (
		        GradNumRho(ii) * GradDeriveNormeCPrim(jj-1)
		      + NumRho * HDeriveNormeCPrim
                      + GradNumRho(jj-1) * GradDeriveNormeCPrim(ii) 
                      - HNumRho * DeriveNormeCPrim);
             Aux = NormeCPrim * NSeconde
		   + NumGrad(ii)*GradNormeCPrim(jj-1)
                   - 2.5 * ( NumGrad(jj-1)*GradNormeCPrim(ii)
			 + DSeconde * Numerateur );
             VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(jj-1)*NumduGrad(ii));

	     Jerk(k2) = Facteur * ( WGrad(ii)*WGrad(jj-1)	
                                     + FacteurW * VIntermed);
             k2++;

	     // derivation en YiYj
             Aux1 =  InvNormeCPrim2 * GradNormeCPrim(jj);
             DeriveAuxBis = 2 * ( (YPrim * Base(3, JJ) + YSecn * Base(2, JJ))*InvNormeCPrim
			         - ProduitC1C2 * Aux1);
	     HDeriveNormeCPrim =  ( Produit2 - DeriveAuxBis*GradNormeCPrim(ii) 
                                  - AuxBis * ( Produit * InvNormeCPrim 
                                  - YPrim * Base(2, II)*Aux1) )*InvNormeCPrim2
                               - 2*NGDNCPrim(ii)*Aux1 *InvNormeCPrim;
             NSeconde = - 3 * (
		        GradNumRho(ii) * GradDeriveNormeCPrim(jj)
		      + NumRho * HDeriveNormeCPrim
                      + GradNumRho(jj) * GradDeriveNormeCPrim(ii) );

             DSeconde =  FacteurY * Produit;
             Aux = NSeconde * NormeCPrim
	         + NumGrad(ii)*GradNormeCPrim(jj)
                 - 2.5 * ( NumGrad(jj)*GradNormeCPrim(ii)
			 + DSeconde * Numerateur );
             VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(jj)*NumduGrad(ii));
	     Jerk(k2) = Facteur * ( WGrad(ii)*WGrad(jj)
                                     + FacteurW * VIntermed); 
	     k2++;
	     }

         // cas ou jj = ii : remplisage en triangle
             Produit =  pow (Base(2, II), 2);
             Produit2 = 2 * Base(2, II) *  Base(3, II); 
  //           ProduitV2 = Base         

  	     
	     // derivation en XiXi
             Aux1 =  InvNormeCPrim2 * GradNormeCPrim(ii-1);
             DeriveAuxBis = 2 * ( (XPrim * Base(3, II) + XSecn * Base(2, II))*InvNormeCPrim
			         - ProduitC1C2 * Aux1);
	     HDeriveNormeCPrim =  ( Produit2 - DeriveAuxBis*GradNormeCPrim(ii-1) 
                                  - AuxBis * ( Produit * InvNormeCPrim 
                                  - XPrim * Base(2, II)*Aux1) )*InvNormeCPrim2
                               - 2*NGDNCPrim(ii-1)*Aux1 *InvNormeCPrim;
             NSeconde = - 3 * (
                              2 * GradNumRho(ii-1) * GradDeriveNormeCPrim(ii-1)
			      + NumRho * HDeriveNormeCPrim );
             DSeconde = FacteurX * Produit;
             Aux =  NSeconde * NormeCPrim 
	          - 1.5 * NumGrad(ii-1)*GradNormeCPrim(ii-1)
	          - 2.5 * DSeconde * Numerateur;
             VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(ii-1)*NumduGrad(ii-1));
	     Jerk(k1) = Facteur * ( WGrad(ii-1)*WGrad(ii-1)
                                     + FacteurW * VIntermed );
	     // derivation en XiYi
	     Aux1 =  InvNormeCPrim2 * GradNormeCPrim(ii);
             DeriveAuxBis = 2 * ( (YPrim * Base(3, II) + YSecn * Base(2, II))*InvNormeCPrim
			         - ProduitC1C2 * Aux1);
	     HDeriveNormeCPrim =  ( - DeriveAuxBis * GradNormeCPrim(ii-1) 
                                    + AuxBis * XPrim * Base(2, II) * Aux1 ) * InvNormeCPrim2
                               - 2*NGDNCPrim(ii-1) * Aux1 *InvNormeCPrim;
             NSeconde = -3 * (
                                GradNumRho(ii-1) * GradDeriveNormeCPrim(ii)
			      + NumRho * HDeriveNormeCPrim 
	                      + GradNumRho(ii) * GradDeriveNormeCPrim(ii-1) );
             DSeconde = FacteurXY * Produit;
             Aux = NSeconde * NormeCPrim 
	         + NumGrad(ii)*GradNormeCPrim(ii-1)
                 - 2.5 * ( NumGrad(ii-1)*GradNormeCPrim(ii)
                         + DSeconde * Numerateur );
	     VIntermed = InvDenominateur 
                       * (Aux- 3.5*GradNormeCPrim(ii-1)*NumduGrad(ii));
	     Jerk(k2) = Facteur * ( WGrad(ii)*WGrad(ii-1)
                                     + FacteurW * VIntermed);
             k2++;

	     // derivation en YiYi
             // Aux1 et DeriveAuxBis calcules au pas precedent ...
	     HDeriveNormeCPrim =  ( Produit2 - DeriveAuxBis*GradNormeCPrim(ii) 
                                  - AuxBis * ( Produit * InvNormeCPrim 
                                  - YPrim * Base(2, II)*Aux1) )*InvNormeCPrim2
                               - 2*NGDNCPrim(ii)*Aux1 *InvNormeCPrim;
             NSeconde = - 3 * (
                              2 * GradNumRho(ii) * GradDeriveNormeCPrim(ii)
                              + NumRho * HDeriveNormeCPrim );
             DSeconde = FacteurY * Produit;
             Aux =  NSeconde * NormeCPrim
                 - 1.5 * NumGrad(ii)*GradNormeCPrim(ii) 
	         - 2.5 * DSeconde * Numerateur; 
	     VIntermed = InvDenominateur 
                       * (Aux - 3.5*GradNormeCPrim(ii)*NumduGrad(ii));
	     Jerk(k2) = Facteur * ( WGrad(ii)*WGrad(ii)
                                     + FacteurW * VIntermed);
	 }
       }     
  }

// sortie standard           
  return Ok;
}

