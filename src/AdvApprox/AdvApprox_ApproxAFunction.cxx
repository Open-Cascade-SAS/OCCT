// Created on: 1995-05-29
// Created by: Xavier BENVENISTE
// Copyright (c) 1995-1999 Matra Datavision
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

// Modified     PMN 22/05/1996 : Pb de recopie des poles
// Modified     PMN 20/08/1996 : Introduction de la decoupe parametrable.
//                               (plus verif des derives en debug)
// Modified     PMN 20/08/1996 : Linearisation de F(t) => Les sous Espaces
//                               facilement approchable ont de petites erreurs.
// Modified     PMN 15/04/1997 : Gestion fine de la continuite aux lieux de decoupes 

#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_Cutting.hxx>
#include <AdvApprox_DichoCutting.hxx>
#include <AdvApprox_EvaluatorFunction.hxx>
#include <AdvApprox_SimpleApprox.hxx>
#include <BSplCLib.hxx>
#include <Convert_CompPolynomialToPoles.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Vector.hxx>
#include <PLib.hxx>
#include <PLib_JacobiPolynomial.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColgp_HArray2OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

#ifdef OCCT_DEBUG

static Standard_Boolean AdvApprox_Debug = 0;

//=====================================================
static void  MAPDBN(const Standard_Integer dimension, 
		    const Standard_Real Debut,
		    const Standard_Real Fin, 
		    AdvApprox_EvaluatorFunction& Evaluator,
		    const Standard_Integer Iordre)
// Objet : Controle par difference finis, des derives 
// Warning : En mode Debug, uniquement
///===================================================
{
 Standard_Integer derive, OrdreDer, ier, NDIMEN = dimension;
 Standard_Real* Ptr;
 Standard_Real h = 1.e-4*(Fin-Debut+1.e-3), eps = 1.e-3, t, ab[2];
 math_Vector V1(1,NDIMEN), V2(1,NDIMEN), Diff(1,NDIMEN), Der(1,NDIMEN);

 if (h < 100*RealEpsilon()) {h = 100*RealEpsilon();}
 ab[0] = Debut;
 ab[1] = Fin;

 for (OrdreDer=1, derive = 0; 
      OrdreDer <= Iordre;  OrdreDer++, derive++) {
  // Verif au debut
   Ptr = (Standard_Real*) &V1.Value(1);
   t = Debut+h;
   Evaluator(&NDIMEN, ab, &t, &derive, Ptr, &ier);

   Ptr = (Standard_Real*) &V2.Value(1);
   t = Debut;
   Evaluator(&NDIMEN, ab, &t, &derive, Ptr, &ier);

   Diff = (V1 - V2)/h;

   Ptr = (Standard_Real*) &Der.Value(1);
   t = Debut;
   Evaluator(&NDIMEN, ab, &t, &OrdreDer, Ptr, &ier); 

 
   if ( (Diff-Der).Norm() > eps * (Der.Norm()+1) ) {
     cout << " Debug Ft au parametre t+ = " << t << endl;
     cout << " Positionement sur la derive "<< OrdreDer 
          << " : " << Der << endl;
     cout << " Erreur estime : " << (Der-Diff) << endl;
   }

  // Verif a la fin
   Ptr = (Standard_Real*) &V1.Value(1);
   t = Fin-h;
   Evaluator(&NDIMEN, ab, &t, &derive, Ptr, &ier);

   Ptr = (Standard_Real*) &V2.Value(1);
   t = Fin;
   Evaluator(&NDIMEN, ab, &t, &derive, Ptr, &ier);

   Diff = (V2 - V1)/h;

   Ptr = (Standard_Real*) &Der.Value(1);
   t = Fin;
   Evaluator(&NDIMEN, ab, &t, &OrdreDer, Ptr, &ier); 

 
   if ( (Diff-Der).Norm() > eps * (Der.Norm()+1) ) {
     cout << " Debug Ft au parametre t- = " << t << endl;
     cout << " Positionement sur la derive "<< OrdreDer 
        << " : " << Der << endl;
     cout << " Erreur estime : " << (Der-Diff) << endl;
   }
 }
}
#endif

//===================================================================
static void PrepareConvert(const Standard_Integer NumCurves,
			   const Standard_Integer MaxDegree,
			   const Standard_Integer ContinuityOrder,  
			   const Standard_Integer Num1DSS,
			   const Standard_Integer Num2DSS,
			   const Standard_Integer Num3DSS,
			   const TColStd_Array1OfInteger& NumCoeffPerCurve,
			   TColStd_Array1OfReal& Coefficients,
			   const TColStd_Array2OfReal& PolynomialIntervals,
			   const TColStd_Array1OfReal& TrueIntervals,
			   const TColStd_Array1OfReal& LocalTolerance,
			   TColStd_Array1OfReal& ErrorMax,
			   TColStd_Array1OfInteger& Continuity)
// Pour determiner les continuites locales
//====================================================================
	     
{
  // Declaration
  Standard_Boolean isCi;
  Standard_Integer icurve, idim, iordre, ii, 
                   Dimension=Num1DSS + 2*Num2DSS + 3*Num3DSS,
                   NbSpace  = Num1DSS + Num2DSS + Num3DSS;
  Standard_Real diff, moy, facteur1,  facteur2, normal1, normal2, eps;
  Standard_Real *Res1, *Res2, *Val1, *Val2;
  Standard_Real *Coef1, *Coef2; 
  Standard_Integer RealDegree = Max(MaxDegree + 1, 2 * ContinuityOrder + 2);
  
  gp_Vec V1,V2;
  gp_Vec2d v1, v2;

  TColStd_Array1OfReal Result(1, 2*(ContinuityOrder+1)*Dimension);
  TColStd_Array1OfReal Prec(1, NbSpace), Suivant(1, NbSpace);
  

  Res1 = (Standard_Real*) &(Result.ChangeValue(1));
  Res2 = (Standard_Real*) &(Result.ChangeValue((ContinuityOrder+1)*Dimension+1));


  //Init
  Continuity.Init(0);
  if (ContinuityOrder ==0) return;

  for (icurve=1; icurve<NumCurves; icurve++) {
  // Init et positionement au noeud
    isCi = Standard_True;
    Coef1 = (Standard_Real*) &(Coefficients.Value( 
	     (icurve-1) * Dimension * RealDegree + Coefficients.Lower()));  
    Coef2 = Coef1 + Dimension * RealDegree;
    Standard_Integer Deg1 = NumCoeffPerCurve(NumCoeffPerCurve.Lower()+icurve-1) - 1;
    PLib::EvalPolynomial(PolynomialIntervals(icurve, 2),
			 ContinuityOrder,
			 Deg1,
			 Dimension,
			 Coef1[0],
			 Res1[0]);  
    Standard_Integer Deg2 = NumCoeffPerCurve(NumCoeffPerCurve.Lower()+icurve) - 1;
    PLib::EvalPolynomial(PolynomialIntervals(icurve+1, 1),
			 ContinuityOrder,
			 Deg2,
			 Dimension,
			 Coef2[0],
			 Res2[0]);
   
    // Verif dans chaque sous espaces.
    for  (iordre=1; iordre<=ContinuityOrder && isCi; iordre++) {

      // fixing a bug PRO18577 
      
      Standard_Real Toler = 1.0e-5;

      Standard_Real f1_dividend = PolynomialIntervals(icurve,2)-PolynomialIntervals(icurve,1); 
      Standard_Real f2_dividend = PolynomialIntervals(icurve+1,2)-PolynomialIntervals(icurve+1,1);
      Standard_Real f1_divizor = TrueIntervals(icurve+1)-TrueIntervals(icurve); 
      Standard_Real f2_divizor = TrueIntervals(icurve+2)-TrueIntervals(icurve+1);
      Standard_Real fract1, fract2;
      
      if( Abs(f1_divizor) < Toler ) 	 // this is to avoid divizion by zero 
	//in this case fract1 =  5.14755758946803e-85 
	facteur1 = 0.0;
      else{     fract1 =  f1_dividend/f1_divizor;
		facteur1 = Pow( fract1,  iordre);      
	      }
      if( Abs(f2_divizor) < Toler )   
	//in this case fract2 =  6.77193633669143e-313   
	facteur2 = 0.0;
      else{	fract2 =  f2_dividend/f2_divizor;
		facteur2 = Pow( fract2,  iordre);
	      }
      normal1 =  Pow(f1_divizor,  iordre);     
      normal2 =  Pow(f2_divizor,  iordre);

      
      idim = 1;
      Val1 = Res1+iordre*Dimension-1;
      Val2 = Res2+iordre*Dimension-1;
      
      for (ii=1;  ii<=Num1DSS && isCi; ii++, idim++) {
        eps = LocalTolerance(idim)*0.01;
	diff = Abs (Val1[ii]*facteur1 - Val2[ii]*facteur2);
	moy = Abs (Val1[ii]*facteur1 + Val2[ii]*facteur2);
        // Un premier controle sur la valeur relative
	if (diff > moy*1.e-9) {
	  Prec(idim) = diff*normal1;
	  Suivant(idim) = diff*normal2;
          // Et un second sur le majorant d'erreur engendree
	  if (Prec(idim)>eps  || 
	       Suivant(idim)>eps) isCi=Standard_False;
	}
	else {
	  Prec(idim) = 0;
	  Suivant(idim) = 0;
	}
      }
      
      Val1+=Num1DSS;
      Val2+=Num1DSS;
      for (ii=1;  ii<=Num2DSS && isCi; ii++, idim++,  Val1+=2,Val2+=2) {
	eps = LocalTolerance(idim)*0.01;
        v1.SetCoord(Val1[1],Val1[2]);
        v2.SetCoord(Val2[1],Val2[2]);
	v1 *= facteur1;
        v2 *= facteur2;
	diff = Abs(v1.X() - v2.X()) + Abs(v1.Y() - v2.Y());
        moy =  Abs(v1.X() + v2.X()) + Abs(v1.Y() + v2.Y());
	if (diff > moy*1.e-9) {
	  Prec(idim) = diff*normal1;
	  Suivant(idim) = diff*normal2;
	  if (Prec(idim)>eps  || 
	      Suivant(idim)>eps) isCi=Standard_False;
	}
	else {
	  Prec(idim) = 0;
	  Suivant(idim) = 0;
	}
      }

      for (ii=1;  ii<=Num3DSS && isCi; ii++, idim++,  Val1+=3,Val2+=3) {
	eps = LocalTolerance(idim)*0.01;
        V1.SetCoord(Val1[1], Val1[2], Val1[3]);
        V2.SetCoord(Val2[1], Val2[2], Val2[3]);
	V1 *= facteur1;
        V2 *= facteur2;
	diff = Abs(V1.X() - V2.X()) + Abs(V1.Y() - V2.Y()) + 
	       Abs(V1.Z() - V2.Z()); 
        moy = Abs(V1.X() + V2.X()) + Abs(V1.Y() + V2.Y()) + 
	      Abs(V1.Z() + V2.Z());
	if (diff > moy*1.e-9) {
	  Prec(idim) = diff*normal1;
	  Suivant(idim) = diff*normal2;
	  if (Prec(idim)>eps  || 
	      Suivant(idim)>eps) isCi=Standard_False;
	}
	else {
	  Prec(idim) = 0;
	  Suivant(idim) = 0;
	}
      }
      // Si c'est bon on update le tout
      if (isCi) {
	Continuity(icurve+1) = iordre;
        Standard_Integer index = (icurve-1)*NbSpace+1;
	for (ii=index, idim=1; idim<=NbSpace; ii++,idim++) {
	  ErrorMax(ii) += Prec(idim);
	  ErrorMax(ii+NbSpace) += Suivant(idim);
	}
      }
    } 
  }
}  


//=======================================================================
//function : ApproxFunction
//
//purpose  :  Approximation d' UNE fonction non polynomiale (dans l'espace de
//     dimension NDIMEN) par N courbes polynomiales, par la methode
//     des moindres carres. Le parametre de la fonction est conserve.
//
//     ARGUMENTS D'ENTREE :
//C     ------------------
//C     NDIMEN   : Dimension totale de l' espace (somme des dimensions
//C              des sous-espaces).
//C     NBSESP : Nombre de sous-espaces "independants".
//C     NDIMSE : Table des dimensions des sous-espaces.
//C     ABFONC : Bornes de l' intervalle (a,b) de definition de la
//C              fonction a approcher.
//C     FONCNP : Fonction externe de positionnement sur la fonction non
//C              polynomiale a approcher.
//C     IORDRE : Ordre de contrainte aux extremites de chaque courbe
//C              polynomiale cree :
//C              -1 = pas de contraintes,
//C               0 = contraintes de passage aux bornes (i.e. C0),
//C               1 = C0 + contraintes de derivees 1eres (i.e. C1),
//C               2 = C1 + contraintes de derivees 2ndes (i.e. C2).
//C     NBCRMX : Nombre maxi de courbes polynomiales a calculer.
//C     NCFLIM : Nombre LIMITE de coeff des "courbes" polynomiales
//C              d' approximation. Doit etre superieur ou egal a
//C              2*IORDRE + 2 et inferieur ou egal a 50 et a NCOFMX).
//C              Limite a 14 apres l'appel a VRIRAZ pour eviter le bruit.
//C     EPSAPR : Table des erreurs d' approximations souhaitees
//C              sous-espace par sous-espace.
//C     ICODEO  : Code d' init. des parametres de discretisation.
//C              (choix de NBPNT et de NDJAC).
//C              = 1 calcul rapide avec precision moyenne sur les coeff.
//C              = 2 calcul rapide avec meilleure precision "    "    ".
//C              = 3 calcul un peu plus lent avec bonne precision     ".
//C              = 4 calcul lent avec la meilleure precision possible ".
//C
//C
//C     ARGUMENTS DE SORTIE :
//C     -------------------
//C     NBCRBE : Nombre de courbes polynomiales creees.
//C     NCFTAB : Table des nombres de coeff. significatifs des NBCRBE
//C              "courbes" calculees.
//C     CRBTAB : Tableau des coeff. des "courbes" polynomiales calculees.
//C              Doit etre dimensionne a CRBTAB(NDIMEN,NCOFMX,NBCRMX).
//C     TABINT : Table des NBCRBE + 1 bornes des intervalles de decoupe de
//C              FONCNP.
//C     ERRMAX : Table des erreurs (sous-espace par sous espace)
//C              MAXIMALES commises dans l' approximation de FONCNP par
//C              par les NBCRBE courbes polynomiales.
//C     ERRMOY : Table des erreurs (sous-espace par sous espace)
//C              MOYENNES commises dans l' approximation de FONCNP par
//C              par les NBCRBE courbes polynomiales.
//C     IERCOD : Code d' erreur :
//C              -1 = ERRMAX > EPSAPR pour au moins un des sous-espace.
//C                   On a alors NCRBMX courbes calculees, certaines de
//C                   degre mathematique min(NCFLIM,14)-1 ou la precision
//C                   demandee n' est pas atteinte.
//C              -2 = Pb dans le mode DEBUG
//C               0 = Tout est ok.
//C               1 = Pb d' incoherence des entrees.
//C              10 = Pb de calcul de l' interpolation des contraintes.
//C              13 = Pb dans l' allocation dynamique.
//C              33 = Pb dans la recuperation des donnees du block data
//C                   des coeff. d' integration par la methode de GAUSS.
//C              >100 Pb dans l' evaluation de FONCNP, le code d' erreur
//C                   renvoye est egal au code d' erreur de FONCNP + 100.
//C
//
// -->  La i-eme courbe polynomiale creee correspond a l' approximation
//C     de FONCNP sur l' intervalle (TABINT(i),TABINT(i+1)). TABINT(i)
//      est une suite STRICTEMENT monotone avec TABINT(1)=ABFONC(1) et
//      TABINT(NBCRBE+1)=ABFONC(2).
//
// -->  Si IERCOD=-1, la precision demandee n' est pas atteinte (ERRMAX
//C     est superieur a EPSAPR sur au moins l' un des sous-espaces), mais
//      on donne le meilleur resultat possible pour min(NCFLIM,14),NBCRMX
//      et EPSAPR choisis par l' utilisateur Dans ce cas (ainsi que pour
//C     IERCOD=0), on a une solution.
//C
//C--> ATTENTION : Toute modification du calcul de NDJAC et NBPNT
//C                doit etre reportee dans le programme MAPNBP.
//
//    HISTORIQUE DES MODIFICATIONS   :
//    --------------------------------
//    
//    20-08-1996 : PMN ; Creation d'apres la routine Fortran MAPFNC
//======================================================================
void AdvApprox_ApproxAFunction::Approximation(
	   const Standard_Integer TotalDimension,
	   // Dimension totale de l' espace 
           // (somme des dimensions des sous-espaces).
	   const Standard_Integer TotalNumSS,//Nombre de sous-espaces "independants".
	   const TColStd_Array1OfInteger& LocalDimension,//dimensions des sous-espaces.
      	   const Standard_Real First,
	   const Standard_Real Last, 
	   // Intervalle (a,b) de definition de la fonction a approcher.
	   AdvApprox_EvaluatorFunction& Evaluator,
	   // Fonction externe de positionnement sur la fonction a approcher.
	   const AdvApprox_Cutting& CutTool,
	   // Maniere de decouper en 2 un intervalle.
	   const Standard_Integer ContinuityOrder,
           // ordre de continutie a respecter (-1, 0, 1, 2) 
	   const Standard_Integer NumMaxCoeffs,
	   // Nombre LIMITE de coeff des "courbes" polynomiales
           // d' approximation. Doit etre superieur ou egal a
           // Doit etre superieur ou egal a  2*ContinuityOrder + 2
           // Limite a 14 pour eviter le bruit. 
	   const Standard_Integer MaxSegments, 
	   //Nombre maxi de courbes polynomiales a calculer.
	   const TColStd_Array1OfReal& LocalTolerancesArray,
           //Tolerances d'approximation souhaitees sous-espace par sous-espace.
	   const Standard_Integer code_precis, 
	   //Code d' init. des parametres de discretisation.
           //C              (choix de NBPNT et de NDJAC).
           //C              = 1 calcul rapide avec precision moyenne sur les coeff.
	   //C              = 2 calcul rapide avec meilleure precision "    "    ".
	   //C              = 3 calcul un peu plus lent avec bonne precision     ".
           //C              = 4 calcul lent avec la meilleure precision possible ".
			 		      Standard_Integer& NumCurves, 
					      TColStd_Array1OfInteger& NumCoeffPerCurveArray,
					      TColStd_Array1OfReal& CoefficientArray,
					      TColStd_Array1OfReal& IntervalsArray, 
					      TColStd_Array1OfReal& ErrorMaxArray, 
					      TColStd_Array1OfReal& AverageErrorArray, 
					      Standard_Integer& ErrorCode)
{
//  Standard_Real EpsPar =  Precision::Confusion();
  Standard_Integer IDIM, NUPIL,TheDeg;
#ifdef OCCT_DEBUG
  Standard_Integer NDIMEN = TotalDimension;
#endif
  Standard_Boolean isCut = Standard_False;

// Defintion des Tableaux C
  Standard_Real * TABINT = (Standard_Real*) &IntervalsArray(1);


  ErrorCode=0;
  CoefficientArray.Init(0);

//-------------------------- Verification des entrees ------------------

      if ((MaxSegments<1)|| (Abs(Last-First)<1.e-9))
	{ErrorCode=1;
	 return;}

//--> La dimension totale doit etre la somme des dimensions des
//    sous-espaces
      IDIM=0;
      for ( Standard_Integer I=1; I<=TotalNumSS; I++) {IDIM += LocalDimension(I);}
      if (IDIM != TotalDimension) 
	{ErrorCode=1;
	 return;}
      GeomAbs_Shape Continuity=GeomAbs_C0;
      switch (ContinuityOrder) {
      case 0:
         Continuity = GeomAbs_C0 ;
         break ;
      case 1: 
         Continuity = GeomAbs_C1 ;
         break ;
      case 2:
         Continuity = GeomAbs_C2 ;
         break ;
      default:
        throw Standard_ConstructionError();
  }

//--------------------- Choix du nombre de points ----------------------
//---------- et du degre de lissage dans la base orthogonale -----------
//-->  NDJAC est le degre de "travail" dans la base orthogonale.


      Standard_Integer NbGaussPoints, WorkDegree;

      PLib::JacobiParameters(Continuity, NumMaxCoeffs-1, code_precis, 
			     NbGaussPoints, WorkDegree);
//      NDJAC=WorkDegree;

//------------------ Initialisation de la gestion des decoupes ---------

      TABINT[0]=First;
      TABINT[1]=Last;
      NUPIL=1;
      NumCurves=0;

//C**********************************************************************
//C                       APPROXIMATION AVEC DECOUPES
//C**********************************************************************
  Handle(PLib_JacobiPolynomial) JacobiBase = new (PLib_JacobiPolynomial) (WorkDegree, Continuity);
//Portage HP le compilateur refuse le debranchement
  Standard_Integer IS ;
  Standard_Boolean goto_fin_de_boucle;
  Standard_Integer MaxDegree = NumMaxCoeffs-1;
  AdvApprox_SimpleApprox Approx (TotalDimension, TotalNumSS,  
                                 Continuity,
				 WorkDegree, NbGaussPoints,
				 JacobiBase, Evaluator);
  while ((NUPIL-NumCurves)!=0) {
//--> Lorsque l' on a atteint le haut de la pile, c' est fini !

//Portage HP le compilateur refuse le debranchement
	goto_fin_de_boucle=Standard_False;

//---- Calcul de la courbe d' approximation dans la base de Jacobi -----

      Approx.Perform ( LocalDimension, LocalTolerancesArray, 
		       TABINT[NumCurves], TABINT[NumCurves+1],
                       MaxDegree);
      if (!Approx.IsDone()) {
	ErrorCode = 1;
	return;
      }



//---------- Calcul du degre de la courbe et de l' erreur max ----------

      IDIM=0;
      NumCoeffPerCurveArray(NumCurves + 1)=0;

//    L'erreur doit etre satisfaite sur tous les sous-espaces sinon, on decoupe

      Standard_Boolean MaxMaxErr=Standard_True;
      for ( IS=1; IS<=TotalNumSS; IS++)
	{ if (Approx.MaxError(IS) > LocalTolerancesArray(IS)) 
             { MaxMaxErr = Standard_False;
               break;
             }
        }

       if (MaxMaxErr == Standard_True)
          {
            NumCurves++;
//            NumCoeffPerCurveArray(NumCurves) = Approx.Degree()+1;
	  }
       else 
          {
//-> ...sinon on essai de decouper l' intervalle courant en 2...
	   Standard_Real TMIL;
	   Standard_Boolean Large;

           Large = CutTool.Value(TABINT[NumCurves], TABINT[NumCurves+1], 
				   TMIL);
            
            if (NUPIL<MaxSegments && Large) {

//	       if (IS!=1) {NumCurves--;}
	       isCut = Standard_True; // Ca y est, on le sait !
	       Standard_Real *  IDEB =  TABINT+NumCurves+1;
	       Standard_Real *  IDEB1 = IDEB+1;
               Standard_Integer ILONG= NUPIL-NumCurves-1;
	       for (Standard_Integer iI=ILONG; iI>=0; iI--) {
		    IDEB1[iI] = IDEB[iI];
		  }
               IDEB[0] = TMIL;
               NUPIL++;
//--> ... et on recommence.
//Portage HP le compilateur refuse le debranchement
	       goto_fin_de_boucle=Standard_True;
//	       break;
	       }
            else {
//--> Si la pile est pleine...
// pour l'instant               ErrorCode=-1;
               NumCurves++;
//               NumCoeffPerCurveArray(NumCurves) = Approx.Degree()+1;
	     }
	 }
//         IDIM += NDSES;
//Portage HP le compilateur refuse le debranchement
	if (goto_fin_de_boucle) continue;
      for (IS=1; IS<=TotalNumSS; IS++) {
	 ErrorMaxArray.SetValue(IS+(NumCurves-1)*TotalNumSS,Approx. MaxError(IS));
//---> ...et calcul de l' erreur moyenne.
	 AverageErrorArray.SetValue(IS+(NumCurves-1)*TotalNumSS,Approx.AverageError(IS));
       }     

	Handle(TColStd_HArray1OfReal) HJacCoeff = Approx.Coefficients();
	TheDeg = Approx.Degree();
	if (isCut && (TheDeg < 2*ContinuityOrder+1) )
	  // Pour ne pas bruiter les derives aux bouts, et garder une continuite
	  // correcte sur la BSpline resultat.
	  TheDeg =  2*ContinuityOrder+1;

	NumCoeffPerCurveArray(NumCurves) = TheDeg+1;
	TColStd_Array1OfReal Coefficients(0,(TheDeg+1)*TotalDimension-1);
	JacobiBase->ToCoefficients (TotalDimension, TheDeg, 
				    HJacCoeff->Array1(), Coefficients);
	Standard_Integer i,j, f = (TheDeg+1)*TotalDimension;
	for (i=0,j=(NumCurves-1)*TotalDimension*NumMaxCoeffs+1;
	     i<f; i++, j++) {
	  CoefficientArray.SetValue(j, Coefficients.Value(i));
         }

#ifdef OCCT_DEBUG
 // Test des derives par difference finis
	Standard_Integer IORDRE = ContinuityOrder;
 
	if (IORDRE>0 && AdvApprox_Debug) {
	  MAPDBN(NDIMEN, TABINT[NumCurves-1], 
		 TABINT[NumCurves], Evaluator, IORDRE);
	}
#endif  
//Portage HP le compilateur refuse le debranchement
//    fin_de_boucle: 
//    {;}  fin de la boucle while   
    } 
  return;
}	
//=======================================================================
//function : AdvApprox_ApproxAFunction
//purpose  : Constructeur avec Decoupe Dichotomique
//=======================================================================

AdvApprox_ApproxAFunction::
AdvApprox_ApproxAFunction(const Standard_Integer Num1DSS,
			  const Standard_Integer Num2DSS,
			  const Standard_Integer Num3DSS,
			  const Handle(TColStd_HArray1OfReal)& OneDTol, 
			  const Handle(TColStd_HArray1OfReal)& TwoDTol, 
			  const Handle(TColStd_HArray1OfReal)& ThreeDTol, 
			  const Standard_Real First, 
			  const Standard_Real Last,
			  const GeomAbs_Shape Continuity, 
			  const Standard_Integer MaxDeg, 
			  const Standard_Integer MaxSeg, 
			  const AdvApprox_EvaluatorFunction& Func) :
			  my1DTolerances(OneDTol),
			  my2DTolerances(TwoDTol),
			  my3DTolerances(ThreeDTol),
			  myFirst(First),
			  myLast(Last),
			  myContinuity(Continuity),
			  myMaxDegree(MaxDeg),
			  myMaxSegments(MaxSeg),
			  myDone(Standard_False),
			  myHasResult(Standard_False),
			  myEvaluator((Standard_Address)&Func) 
{
  AdvApprox_DichoCutting Cut;
  Perform(Num1DSS, Num2DSS, Num3DSS, Cut);
}

AdvApprox_ApproxAFunction::
AdvApprox_ApproxAFunction(const Standard_Integer Num1DSS,
			  const Standard_Integer Num2DSS,
			  const Standard_Integer Num3DSS,
			  const Handle(TColStd_HArray1OfReal)& OneDTol, 
			  const Handle(TColStd_HArray1OfReal)& TwoDTol, 
			  const Handle(TColStd_HArray1OfReal)& ThreeDTol, 
			  const Standard_Real First, 
			  const Standard_Real Last,
			  const GeomAbs_Shape Continuity, 
			  const Standard_Integer MaxDeg, 
			  const Standard_Integer MaxSeg, 
			  const AdvApprox_EvaluatorFunction& Func,
			  const AdvApprox_Cutting& CutTool) :
			  my1DTolerances(OneDTol),
			  my2DTolerances(TwoDTol),
			  my3DTolerances(ThreeDTol),
			  myFirst(First),
			  myLast(Last),
			  myContinuity(Continuity),
			  myMaxDegree(MaxDeg),
			  myMaxSegments(MaxSeg),
			  myDone(Standard_False),
			  myHasResult(Standard_False),
			  myEvaluator((Standard_Address)&Func) 
{
  Perform(Num1DSS, Num2DSS, Num3DSS, CutTool);
}

//=======================================================================
//function : AdvApprox_ApproxAFunction::Perform
//purpose  : Make all the Work !!
//=======================================================================
void AdvApprox_ApproxAFunction::Perform(const Standard_Integer Num1DSS,
					const Standard_Integer Num2DSS,
					const Standard_Integer Num3DSS, 
					const AdvApprox_Cutting& CutTool)
{
  if (Num1DSS < 0 ||
      Num2DSS < 0 ||
      Num3DSS < 0 ||
      Num1DSS + Num2DSS + Num3DSS <= 0 ||
      myLast < myFirst ||
      myMaxDegree < 1  ||
      myMaxSegments < 0)
    throw Standard_ConstructionError();
  if (myMaxDegree > 14) { 
      myMaxDegree = 14 ;
  }
  //
  // Input 
  // 
  myNumSubSpaces[0] = Num1DSS ;
  myNumSubSpaces[1] = Num2DSS ;
  myNumSubSpaces[2] = Num3DSS ;
  Standard_Integer  TotalNumSS  =
    Num1DSS + Num2DSS + Num3DSS,
  ii,
  jj,
  kk,
  index,
  dim_index,
  local_index;
  Standard_Integer  TotalDimension =
    myNumSubSpaces[0] + 2 * myNumSubSpaces[1] + 3 * myNumSubSpaces[2] ;
  Standard_Real  error_value ;

  Standard_Integer ContinuityOrder=0 ;
  switch (myContinuity) {
  case GeomAbs_C0:
    ContinuityOrder = 0 ;
    break ;
  case GeomAbs_C1: 
    ContinuityOrder = 1 ;
    break ;
  case GeomAbs_C2:
    ContinuityOrder = 2 ;
    break ;
  default:
    throw Standard_ConstructionError();
  }
  Standard_Real ApproxStartEnd[2] ;
  Standard_Integer NumMaxCoeffs = Max(myMaxDegree + 1, 2 * ContinuityOrder + 2);
  myMaxDegree = NumMaxCoeffs - 1;
  Standard_Integer code_precis = 1 ;
  //
  //  WARNING : the polynomial coefficients are the 
  //  taylor expansion of the polynomial at 0.0e0 !
  //
  ApproxStartEnd[0] =  -1.0e0 ;
  ApproxStartEnd[1] =  1.0e0 ;
  
  TColStd_Array1OfInteger LocalDimension(1,TotalNumSS) ;

  
  index = 1 ;
  TColStd_Array1OfReal LocalTolerances(1,TotalNumSS) ;
  
  for(jj = 1; jj <= myNumSubSpaces[0] ; jj++) {
    LocalTolerances.SetValue(index,my1DTolerances->Value(jj)) ;
    LocalDimension.SetValue(index,1) ;
    index += 1 ;
  }
  for(jj = 1 ; jj <= myNumSubSpaces[1] ; jj++) {
    LocalTolerances.SetValue(index,my2DTolerances->Value(jj)) ;
    LocalDimension.SetValue(index,2) ;
    index += 1 ;
  }
  for(jj = 1; jj <= myNumSubSpaces[2] ; jj++) {
    LocalTolerances.SetValue(index,my3DTolerances->Value(jj)) ;
    LocalDimension.SetValue(index,3) ;
    index += 1 ;
  }
  //
  // Ouput
  //
  
  Standard_Integer ErrorCode = 0,
  NumCurves,
  size = 
    myMaxSegments * NumMaxCoeffs * TotalDimension ;
  Handle(TColStd_HArray1OfInteger)  NumCoeffPerCurvePtr =
    new TColStd_HArray1OfInteger (1,myMaxSegments) ;

  Handle(TColStd_HArray1OfReal)  LocalCoefficientsPtr =
    new TColStd_HArray1OfReal(1,size) ;

  Handle(TColStd_HArray1OfReal)  IntervalsPtr = 
    new TColStd_HArray1OfReal (1,myMaxSegments+1) ;

  TColStd_Array1OfReal ErrorMax(1,myMaxSegments * TotalNumSS) ;

  TColStd_Array1OfReal AverageError(1,myMaxSegments * TotalNumSS) ;
  
  Approximation ( TotalDimension,
                  TotalNumSS,   LocalDimension,
                  myFirst,      myLast,
                  *(AdvApprox_EvaluatorFunction*)myEvaluator, 
                  CutTool, ContinuityOrder, NumMaxCoeffs,
                  myMaxSegments, LocalTolerances, code_precis,
                  NumCurves,                            // Nombre de courbe en sortie
                  NumCoeffPerCurvePtr->ChangeArray1(),  // Nbre de coeff par courbe
                  LocalCoefficientsPtr->ChangeArray1(), // Les Coeffs solutions
                  IntervalsPtr->ChangeArray1(),         // La Table des decoupes
                  ErrorMax,                             // Majoration de l'erreur
                  AverageError,                         // Erreur moyenne constatee
                  ErrorCode) ;
  
  if (ErrorCode == 0 || ErrorCode == -1)    {
    //
    // si tout est OK ou bien on a un resultat dont l une des erreurs max est
    // plus grande que la tolerance demandee

    TColStd_Array1OfInteger TabContinuity(1, NumCurves); 
    TColStd_Array2OfReal PolynomialIntervalsPtr (1,NumCurves,1,2);
    for (ii = PolynomialIntervalsPtr.LowerRow() ;
	 ii <= PolynomialIntervalsPtr.UpperRow() ;
	 ii++) {
      // On force un degre 1 minimum (PRO5474)
      NumCoeffPerCurvePtr->ChangeValue(ii) = 
	   Max(2, NumCoeffPerCurvePtr->Value(ii));
      PolynomialIntervalsPtr.SetValue(ii,1,ApproxStartEnd[0]) ;
      PolynomialIntervalsPtr.SetValue(ii,2,ApproxStartEnd[1]) ;
    }

    PrepareConvert(NumCurves, myMaxDegree, ContinuityOrder,  
		   Num1DSS, Num2DSS, Num3DSS,
		   NumCoeffPerCurvePtr->Array1(), 
		   LocalCoefficientsPtr->ChangeArray1(),
		   PolynomialIntervalsPtr, IntervalsPtr->Array1(),
                   LocalTolerances, ErrorMax,
		   TabContinuity);
           
    Convert_CompPolynomialToPoles
      AConverter(NumCurves,
		 TotalDimension,
		 myMaxDegree,
		 TabContinuity, 
		 NumCoeffPerCurvePtr->Array1(),
		 LocalCoefficientsPtr->Array1(),
		 PolynomialIntervalsPtr,
		 IntervalsPtr->Array1());

    if (AConverter.IsDone()) {
      Handle(TColStd_HArray2OfReal) PolesPtr ;
      AConverter.Poles(PolesPtr) ;
      AConverter.Knots(myKnots) ;
      AConverter.Multiplicities(myMults) ;
      myDegree = AConverter.Degree() ;
      index = 0 ;
      if (myNumSubSpaces[0] > 0) {
	my1DPoles = new TColStd_HArray2OfReal(1,PolesPtr->ColLength(),
					      1,myNumSubSpaces[0]) ;
	my1DMaxError = new TColStd_HArray1OfReal(1,myNumSubSpaces[0]) ;
	my1DAverageError = new TColStd_HArray1OfReal(1,myNumSubSpaces[0]) ;
	for (ii = 1 ; ii <= PolesPtr->ColLength() ; ii++) {
	  for (jj = 1 ; jj <= myNumSubSpaces[0] ; jj++) {
	    my1DPoles->SetValue(ii,jj, PolesPtr->Value(ii,jj)) ;
	  }
	}
	
	for (jj = 1 ; jj <= myNumSubSpaces[0] ; jj++) {
	  error_value = 0.0e0 ;
	  for (ii = 1 ; ii <= NumCurves ; ii++) {      
	    local_index = (ii - 1) * TotalNumSS ;
	    error_value = Max(ErrorMax(local_index + jj),error_value)  ;
	    
	  }
	  my1DMaxError->SetValue(jj, error_value) ;
	}
	for (jj = 1 ; jj <= myNumSubSpaces[0] ; jj++) {
	  error_value = 0.0e0 ;
	  for (ii = 1 ; ii <= NumCurves ; ii++) {      
	    local_index = (ii - 1) * TotalNumSS ;
	    error_value += AverageError(local_index + jj)  ;
	    
	  }
	  error_value /= (Standard_Real) NumCurves ;
	  my1DAverageError->SetValue(jj, error_value) ;
	}
	index += myNumSubSpaces[0] ;
      }

      dim_index = index; //Pour le cas ou il n'y a pas de 2D

      if (myNumSubSpaces[1] > 0) {
	gp_Pnt2d Point2d ;
	my2DPoles = new TColgp_HArray2OfPnt2d(1,PolesPtr->ColLength(),
					      1,myNumSubSpaces[1]) ;
	my2DMaxError = new TColStd_HArray1OfReal(1,myNumSubSpaces[1]) ;
	my2DAverageError = new TColStd_HArray1OfReal(1,myNumSubSpaces[1]) ;
	for (ii = 1 ; ii <= PolesPtr->ColLength() ; ii++) {
	  for (jj = 1 ; jj <= myNumSubSpaces[1] ; jj++) {
	    local_index = index + (jj-1) * 2 ;
	    for (kk = 1; kk <= 2 ; kk++) {
	      Point2d.SetCoord(kk, 
			       PolesPtr->Value(ii,local_index + kk)) ;
	    }
	    my2DPoles->SetValue(ii,jj, Point2d) ;
	  }
	}
	
	for (jj = 1 ; jj <= myNumSubSpaces[1] ; jj++) {
	  error_value = 0.0e0 ;
	  for (ii = 1 ; ii <= NumCurves ; ii++) {      
	    local_index = (ii - 1) * TotalNumSS + index ;
	    error_value = Max(ErrorMax(local_index + jj),error_value)  ;
	    
	  }
	  my2DMaxError->SetValue(jj, error_value) ;
	}
	for (jj = 1 ; jj <= myNumSubSpaces[1] ; jj++) {
	  error_value = 0.0e0 ;
	  for (ii = 1 ; ii <= NumCurves ; ii++) {      
	    local_index = (ii - 1) * TotalNumSS + index ;
	    error_value += AverageError(local_index + jj)  ;
	    
	  }
	  error_value /= (Standard_Real) NumCurves ;
	  my2DAverageError->SetValue(jj, error_value) ;
	}
	index += myNumSubSpaces[1] ;
	// Pour les poles il faut doubler le decalage :
	dim_index = index + myNumSubSpaces[1];
      }

      if (myNumSubSpaces[2] > 0) {
	gp_Pnt Point ;
	my3DPoles = new TColgp_HArray2OfPnt(1,PolesPtr->ColLength(),
					    1,myNumSubSpaces[2]) ;
	my3DMaxError = new TColStd_HArray1OfReal(1,myNumSubSpaces[2]) ;
	my3DAverageError = new TColStd_HArray1OfReal(1,myNumSubSpaces[2]) ;
	for (ii = 1 ; ii <= PolesPtr->ColLength() ; ii++) {
	  for (jj = 1 ; jj <= myNumSubSpaces[2] ; jj++) {
	    local_index = dim_index + (jj-1) * 3 ;
	    for (kk = 1; kk <= 3 ; kk++) {
	      Point.SetCoord(kk, 
			     PolesPtr->Value(ii,local_index + kk)) ;
	    }
	    my3DPoles->SetValue(ii,jj,Point) ;
	    
	  }
	}
	
	for (jj = 1 ; jj <= myNumSubSpaces[2] ; jj++) {
	  error_value = 0.0e0 ;
	  for (ii = 1 ; ii <= NumCurves ; ii++) {      
	    local_index = (ii - 1) * TotalNumSS + index ;
	    error_value = Max(ErrorMax(local_index + jj),error_value)  ;
	    
	  }
	  my3DMaxError->SetValue(jj, error_value) ;
	}
	for (jj = 1 ; jj <= myNumSubSpaces[2] ; jj++) {
	  error_value = 0.0e0 ;
	  for (ii = 1 ; ii <= NumCurves ; ii++) {      
	    local_index = (ii - 1) * TotalNumSS + index ;
	    error_value += AverageError(local_index + jj)  ;
	    
	  }
	  error_value /= (Standard_Real) NumCurves ;
	  my3DAverageError->SetValue(jj, error_value) ;
	}
      }
      if (ErrorCode == 0) {
	myDone = Standard_True ;
	myHasResult = Standard_True ;
      } 
      else if (ErrorCode == -1) {
	myHasResult = Standard_True ;
      }
      
    }
  }
}


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void AdvApprox_ApproxAFunction::Poles(const Standard_Integer Index,
				      TColgp_Array1OfPnt&   P) const 
{
  Standard_Integer ii ;
  for (ii = P.Lower() ; ii <= P.Upper() ; ii++) {
    P.SetValue(ii,my3DPoles->Value(ii,Index)) ;
  }
}


//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer AdvApprox_ApproxAFunction::NbPoles()  const 
{ if (myDone || myHasResult) return BSplCLib::NbPoles(myDegree,
						      Standard_False,
						      myMults->Array1()) ;
  return 0 ; }

//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void AdvApprox_ApproxAFunction::Poles2d(const Standard_Integer Index,
					TColgp_Array1OfPnt2d&   P) const 
{
  Standard_Integer ii ;
  for (ii = P.Lower() ; ii <= P.Upper() ; ii++) {
    P.SetValue(ii,my2DPoles->Value(ii,Index)) ;
  }
}
//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void AdvApprox_ApproxAFunction::Poles1d(const Standard_Integer Index,
					TColStd_Array1OfReal&   P) const 
{
  Standard_Integer ii ;
  for (ii = P.Lower() ; ii <= P.Upper() ; ii++) {
    P.SetValue(ii,my1DPoles->Value(ii,Index)) ;
  }
}
//=======================================================================
//function : MaxError
//purpose  : 
//=======================================================================
Handle(TColStd_HArray1OfReal)
     AdvApprox_ApproxAFunction::MaxError(const Standard_Integer D) const 
     
{
  Handle(TColStd_HArray1OfReal) EPtr ;
  if (D <= 0 ||
      D > 3) {
    
    throw Standard_OutOfRange() ;
  }
  switch (D) {
  case 1:
    EPtr = my1DMaxError ;
    break ;
  case 2:
    EPtr = my2DMaxError ;
    break ;
  case 3:
    EPtr = my3DMaxError ;
    break ;
  }
  return EPtr ;
}
//=======================================================================
//function : MaxError
//purpose  : 
//=======================================================================
Standard_Real AdvApprox_ApproxAFunction::MaxError(const Standard_Integer D,
						  const Standard_Integer Index) const 
{
  Handle(TColStd_HArray1OfReal) EPtr =
    MaxError(D) ;
  
  return EPtr->Value(Index) ;
}  

//=======================================================================
//function : AverageError
//purpose  : 
//=======================================================================
Handle(TColStd_HArray1OfReal)
     AdvApprox_ApproxAFunction::AverageError(const Standard_Integer D) const 
     
{
  Handle(TColStd_HArray1OfReal) EPtr ;
  if (D <= 0 ||
      D > 3) {
    
    throw Standard_OutOfRange() ;
  }
  switch (D) {
  case 1:
    EPtr = my1DAverageError ;
    break ;
  case 2:
    EPtr = my2DAverageError ;
    break ;
  case 3:
    EPtr = my3DAverageError ;
    break ;
  }
  return EPtr ;
}
//=======================================================================
//function : AverageError
//purpose  : 
//=======================================================================

Standard_Real  AdvApprox_ApproxAFunction::AverageError(
						       const Standard_Integer D,
						       const Standard_Integer Index) const 
						       
{
  Handle(TColStd_HArray1OfReal) EPtr =
    AverageError(D) ;
  return EPtr->Value(Index) ;
}

void  AdvApprox_ApproxAFunction::Dump(Standard_OStream& o) const 
{
  Standard_Integer ii;
  o << "Dump of ApproxAFunction" << endl;
  if (myNumSubSpaces[0] > 0) {
    o << "Error(s) 1d = " << endl;
    for (ii=1; ii <= myNumSubSpaces[0]; ii++) {
      o << "   " << MaxError(1, ii) << endl;
    }
  }

  if (myNumSubSpaces[1] > 0) {
    o << "Error(s) 2d = " << endl;
    for (ii=1; ii <= myNumSubSpaces[1]; ii++) {
      o << "   " << MaxError(2, ii) << endl;
    }
  }

  if (myNumSubSpaces[2] > 0) {
    o << "Error(s) 3d = " << endl;
    for (ii=1; ii <= myNumSubSpaces[2]; ii++) {
      o << "   " << MaxError(3, ii) << endl;
    }
  }
}
