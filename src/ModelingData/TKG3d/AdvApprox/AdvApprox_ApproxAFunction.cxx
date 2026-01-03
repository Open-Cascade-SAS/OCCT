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
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>

#ifdef OCCT_DEBUG

static bool AdvApprox_Debug = 0;

//=====================================================
static void MAPDBN(const int                    dimension,
                   const double                 Debut,
                   const double                 Fin,
                   AdvApprox_EvaluatorFunction& Evaluator,
                   const int                    Iordre)
// Objet : Controle par difference finis, des derives
// Warning : En mode Debug, uniquement
///===================================================
{
  int         derive, OrdreDer, ier, NDIMEN = dimension;
  double*     Ptr;
  double      h = 1.e-4 * (Fin - Debut + 1.e-3), eps = 1.e-3, t, ab[2];
  math_Vector V1(1, NDIMEN), V2(1, NDIMEN), Diff(1, NDIMEN), Der(1, NDIMEN);

  if (h < 100 * RealEpsilon())
  {
    h = 100 * RealEpsilon();
  }
  ab[0] = Debut;
  ab[1] = Fin;

  for (OrdreDer = 1, derive = 0; OrdreDer <= Iordre; OrdreDer++, derive++)
  {
    // Verif au debut
    Ptr = (double*)&V1.Value(1);
    t   = Debut + h;
    Evaluator(&NDIMEN, ab, &t, &derive, Ptr, &ier);

    Ptr = (double*)&V2.Value(1);
    t   = Debut;
    Evaluator(&NDIMEN, ab, &t, &derive, Ptr, &ier);

    Diff = (V1 - V2) / h;

    Ptr = (double*)&Der.Value(1);
    t   = Debut;
    Evaluator(&NDIMEN, ab, &t, &OrdreDer, Ptr, &ier);

    if ((Diff - Der).Norm() > eps * (Der.Norm() + 1))
    {
      std::cout << " Debug Ft au parametre t+ = " << t << std::endl;
      std::cout << " Positionement sur la derive " << OrdreDer << " : " << Der << std::endl;
      std::cout << " Erreur estime : " << (Der - Diff) << std::endl;
    }

    // Verif a la fin
    Ptr = (double*)&V1.Value(1);
    t   = Fin - h;
    Evaluator(&NDIMEN, ab, &t, &derive, Ptr, &ier);

    Ptr = (double*)&V2.Value(1);
    t   = Fin;
    Evaluator(&NDIMEN, ab, &t, &derive, Ptr, &ier);

    Diff = (V2 - V1) / h;

    Ptr = (double*)&Der.Value(1);
    t   = Fin;
    Evaluator(&NDIMEN, ab, &t, &OrdreDer, Ptr, &ier);

    if ((Diff - Der).Norm() > eps * (Der.Norm() + 1))
    {
      std::cout << " Debug Ft au parametre t- = " << t << std::endl;
      std::cout << " Positionement sur la derive " << OrdreDer << " : " << Der << std::endl;
      std::cout << " Erreur estime : " << (Der - Diff) << std::endl;
    }
  }
}
#endif

//===================================================================
static void PrepareConvert(const int                         NumCurves,
                           const int                         MaxDegree,
                           const int                         ContinuityOrder,
                           const int                         Num1DSS,
                           const int                         Num2DSS,
                           const int                         Num3DSS,
                           const NCollection_Array1<int>&    NumCoeffPerCurve,
                           NCollection_Array1<double>&       Coefficients,
                           const NCollection_Array2<double>& PolynomialIntervals,
                           const NCollection_Array1<double>& TrueIntervals,
                           const NCollection_Array1<double>& LocalTolerance,
                           NCollection_Array1<double>&       ErrorMax,
                           NCollection_Array1<int>&          Continuity)
// Pour determiner les continuites locales
//====================================================================

{
  // Declaration
  bool isCi;
  int  icurve, idim, iordre, ii, Dimension = Num1DSS + 2 * Num2DSS + 3 * Num3DSS,
                                NbSpace = Num1DSS + Num2DSS + Num3DSS;
  double  diff, moy, facteur1, facteur2, normal1, normal2, eps;
  double *Res1, *Res2, *Val1, *Val2;
  double *Coef1, *Coef2;
  int     RealDegree = std::max(MaxDegree + 1, 2 * ContinuityOrder + 2);

  gp_Vec   V1, V2;
  gp_Vec2d v1, v2;

  NCollection_Array1<double> Result(1, 2 * (ContinuityOrder + 1) * Dimension);
  NCollection_Array1<double> Prec(1, NbSpace), Suivant(1, NbSpace);

  Res1 = (double*)&(Result.ChangeValue(1));
  Res2 = (double*)&(Result.ChangeValue((ContinuityOrder + 1) * Dimension + 1));

  // Init
  Continuity.Init(0);
  if (ContinuityOrder == 0)
    return;

  for (icurve = 1; icurve < NumCurves; icurve++)
  {
    // Init et positionement au noeud
    isCi = true;
    Coef1 =
      (double*)&(Coefficients.Value((icurve - 1) * Dimension * RealDegree + Coefficients.Lower()));
    Coef2    = Coef1 + Dimension * RealDegree;
    int Deg1 = NumCoeffPerCurve(NumCoeffPerCurve.Lower() + icurve - 1) - 1;
    PLib::EvalPolynomial(PolynomialIntervals(icurve, 2),
                         ContinuityOrder,
                         Deg1,
                         Dimension,
                         Coef1[0],
                         Res1[0]);
    int Deg2 = NumCoeffPerCurve(NumCoeffPerCurve.Lower() + icurve) - 1;
    PLib::EvalPolynomial(PolynomialIntervals(icurve + 1, 1),
                         ContinuityOrder,
                         Deg2,
                         Dimension,
                         Coef2[0],
                         Res2[0]);

    // Verif dans chaque sous espaces.
    for (iordre = 1; iordre <= ContinuityOrder && isCi; iordre++)
    {

      // fixing a bug PRO18577

      double Toler = 1.0e-5;

      double f1_dividend = PolynomialIntervals(icurve, 2) - PolynomialIntervals(icurve, 1);
      double f2_dividend = PolynomialIntervals(icurve + 1, 2) - PolynomialIntervals(icurve + 1, 1);
      double f1_divizor  = TrueIntervals(icurve + 1) - TrueIntervals(icurve);
      double f2_divizor  = TrueIntervals(icurve + 2) - TrueIntervals(icurve + 1);
      double fract1, fract2;

      if (std::abs(f1_divizor) < Toler) // this is to avoid divizion by zero
        // in this case fract1 =  5.14755758946803e-85
        facteur1 = 0.0;
      else
      {
        fract1   = f1_dividend / f1_divizor;
        facteur1 = std::pow(fract1, iordre);
      }
      if (std::abs(f2_divizor) < Toler)
        // in this case fract2 =  6.77193633669143e-313
        facteur2 = 0.0;
      else
      {
        fract2   = f2_dividend / f2_divizor;
        facteur2 = std::pow(fract2, iordre);
      }
      normal1 = std::pow(f1_divizor, iordre);
      normal2 = std::pow(f2_divizor, iordre);

      idim = 1;
      Val1 = Res1 + iordre * Dimension - 1;
      Val2 = Res2 + iordre * Dimension - 1;

      for (ii = 1; ii <= Num1DSS && isCi; ii++, idim++)
      {
        eps  = LocalTolerance(idim) * 0.01;
        diff = std::abs(Val1[ii] * facteur1 - Val2[ii] * facteur2);
        moy  = std::abs(Val1[ii] * facteur1 + Val2[ii] * facteur2);
        // Un premier controle sur la valeur relative
        if (diff > moy * 1.e-9)
        {
          Prec(idim)    = diff * normal1;
          Suivant(idim) = diff * normal2;
          // Et un second sur le majorant d'erreur engendree
          if (Prec(idim) > eps || Suivant(idim) > eps)
            isCi = false;
        }
        else
        {
          Prec(idim)    = 0;
          Suivant(idim) = 0;
        }
      }

      Val1 += Num1DSS;
      Val2 += Num1DSS;
      for (ii = 1; ii <= Num2DSS && isCi; ii++, idim++, Val1 += 2, Val2 += 2)
      {
        eps = LocalTolerance(idim) * 0.01;
        v1.SetCoord(Val1[1], Val1[2]);
        v2.SetCoord(Val2[1], Val2[2]);
        v1 *= facteur1;
        v2 *= facteur2;
        diff = std::abs(v1.X() - v2.X()) + std::abs(v1.Y() - v2.Y());
        moy  = std::abs(v1.X() + v2.X()) + std::abs(v1.Y() + v2.Y());
        if (diff > moy * 1.e-9)
        {
          Prec(idim)    = diff * normal1;
          Suivant(idim) = diff * normal2;
          if (Prec(idim) > eps || Suivant(idim) > eps)
            isCi = false;
        }
        else
        {
          Prec(idim)    = 0;
          Suivant(idim) = 0;
        }
      }

      for (ii = 1; ii <= Num3DSS && isCi; ii++, idim++, Val1 += 3, Val2 += 3)
      {
        eps = LocalTolerance(idim) * 0.01;
        V1.SetCoord(Val1[1], Val1[2], Val1[3]);
        V2.SetCoord(Val2[1], Val2[2], Val2[3]);
        V1 *= facteur1;
        V2 *= facteur2;
        diff = std::abs(V1.X() - V2.X()) + std::abs(V1.Y() - V2.Y()) + std::abs(V1.Z() - V2.Z());
        moy  = std::abs(V1.X() + V2.X()) + std::abs(V1.Y() + V2.Y()) + std::abs(V1.Z() + V2.Z());
        if (diff > moy * 1.e-9)
        {
          Prec(idim)    = diff * normal1;
          Suivant(idim) = diff * normal2;
          if (Prec(idim) > eps || Suivant(idim) > eps)
            isCi = false;
        }
        else
        {
          Prec(idim)    = 0;
          Suivant(idim) = 0;
        }
      }
      // Si c'est bon on update le tout
      if (isCi)
      {
        Continuity(icurve + 1) = iordre;
        int index              = (icurve - 1) * NbSpace + 1;
        for (ii = index, idim = 1; idim <= NbSpace; ii++, idim++)
        {
          ErrorMax(ii) += Prec(idim);
          ErrorMax(ii + NbSpace) += Suivant(idim);
        }
      }
    }
  }
}

//=======================================================================
// function : ApproxFunction
//
// purpose  :  Approximation d' UNE fonction non polynomiale (dans l'espace de
//     dimension NDIMEN) par N courbes polynomiales, par la methode
//     des moindres carres. Le parametre de la fonction est conserve.
//
//     ARGUMENTS D'ENTREE :
// C     ------------------
// C     NDIMEN   : Dimension totale de l' espace (somme des dimensions
// C              des sous-espaces).
// C     NBSESP : Nombre de sous-espaces "independants".
// C     NDIMSE : Table des dimensions des sous-espaces.
// C     ABFONC : Bornes de l' intervalle (a,b) de definition de la
// C              fonction a approcher.
// C     FONCNP : Fonction externe de positionnement sur la fonction non
// C              polynomiale a approcher.
// C     IORDRE : Ordre de contrainte aux extremites de chaque courbe
// C              polynomiale cree :
// C              -1 = pas de contraintes,
// C               0 = contraintes de passage aux bornes (i.e. C0),
// C               1 = C0 + contraintes de derivees 1eres (i.e. C1),
// C               2 = C1 + contraintes de derivees 2ndes (i.e. C2).
// C     NBCRMX : Nombre maxi de courbes polynomiales a calculer.
// C     NCFLIM : Nombre LIMITE de coeff des "courbes" polynomiales
// C              d' approximation. Doit etre superieur ou egal a
// C              2*IORDRE + 2 et inferieur ou egal a 50 et a NCOFMX).
// C              Limite a 14 apres l'appel a VRIRAZ pour eviter le bruit.
// C     EPSAPR : Table des erreurs d' approximations souhaitees
// C              sous-espace par sous-espace.
// C     ICODEO  : Code d' init. des parametres de discretisation.
// C              (choix de NBPNT et de NDJAC).
// C              = 1 calcul rapide avec precision moyenne sur les coeff.
// C              = 2 calcul rapide avec meilleure precision "    "    ".
// C              = 3 calcul un peu plus lent avec bonne precision     ".
// C              = 4 calcul lent avec la meilleure precision possible ".
// C
// C
// C     ARGUMENTS DE SORTIE :
// C     -------------------
// C     NBCRBE : Nombre de courbes polynomiales creees.
// C     NCFTAB : Table des nombres de coeff. significatifs des NBCRBE
// C              "courbes" calculees.
// C     CRBTAB : Tableau des coeff. des "courbes" polynomiales calculees.
// C              Doit etre dimensionne a CRBTAB(NDIMEN,NCOFMX,NBCRMX).
// C     TABINT : Table des NBCRBE + 1 bornes des intervalles de decoupe de
// C              FONCNP.
// C     ERRMAX : Table des erreurs (sous-espace par sous espace)
// C              MAXIMALES commises dans l' approximation de FONCNP par
// C              par les NBCRBE courbes polynomiales.
// C     ERRMOY : Table des erreurs (sous-espace par sous espace)
// C              MOYENNES commises dans l' approximation de FONCNP par
// C              par les NBCRBE courbes polynomiales.
// C     IERCOD : Code d' erreur :
// C              -1 = ERRMAX > EPSAPR pour au moins un des sous-espace.
// C                   On a alors NCRBMX courbes calculees, certaines de
// C                   degre mathematique min(NCFLIM,14)-1 ou la precision
// C                   demandee n' est pas atteinte.
// C              -2 = Pb dans le mode DEBUG
// C               0 = Tout est ok.
// C               1 = Pb d' incoherence des entrees.
// C              10 = Pb de calcul de l' interpolation des contraintes.
// C              13 = Pb dans l' allocation dynamique.
// C              33 = Pb dans la recuperation des donnees du block data
// C                   des coeff. d' integration par la methode de GAUSS.
// C              >100 Pb dans l' evaluation de FONCNP, le code d' erreur
// C                   renvoye est egal au code d' erreur de FONCNP + 100.
// C
//
// -->  La i-eme courbe polynomiale creee correspond a l' approximation
// C     de FONCNP sur l' intervalle (TABINT(i),TABINT(i+1)). TABINT(i)
//      est une suite STRICTEMENT monotone avec TABINT(1)=ABFONC(1) et
//      TABINT(NBCRBE+1)=ABFONC(2).
//
// -->  Si IERCOD=-1, la precision demandee n' est pas atteinte (ERRMAX
// C     est superieur a EPSAPR sur au moins l' un des sous-espaces), mais
//      on donne le meilleur resultat possible pour min(NCFLIM,14),NBCRMX
//      et EPSAPR choisis par l' utilisateur Dans ce cas (ainsi que pour
// C     IERCOD=0), on a une solution.
// C
// C--> ATTENTION : Toute modification du calcul de NDJAC et NBPNT
// C                doit etre reportee dans le programme MAPNBP.
//
//    HISTORIQUE DES MODIFICATIONS   :
//    --------------------------------
//
//    20-08-1996 : PMN ; Creation d'apres la routine Fortran MAPFNC
//======================================================================
void AdvApprox_ApproxAFunction::Approximation(
  const int TotalDimension,
  // Dimension totale de l' espace
  // (somme des dimensions des sous-espaces).
  const int                      TotalNumSS,     // Nombre de sous-espaces "independants".
  const NCollection_Array1<int>& LocalDimension, // dimensions des sous-espaces.
  const double                   First,
  const double                   Last,
  // Intervalle (a,b) de definition de la fonction a approcher.
  AdvApprox_EvaluatorFunction& Evaluator,
  // Fonction externe de positionnement sur la fonction a approcher.
  const AdvApprox_Cutting& CutTool,
  // Maniere de decouper en 2 un intervalle.
  const int ContinuityOrder,
  // ordre de continutie a respecter (-1, 0, 1, 2)
  const int NumMaxCoeffs,
  // Nombre LIMITE de coeff des "courbes" polynomiales
  // d' approximation. Doit etre superieur ou egal a
  // Doit etre superieur ou egal a  2*ContinuityOrder + 2
  // Limite a 14 pour eviter le bruit.
  const int MaxSegments,
  // Nombre maxi de courbes polynomiales a calculer.
  const NCollection_Array1<double>& LocalTolerancesArray,
  // Tolerances d'approximation souhaitees sous-espace par sous-espace.
  const int code_precis,
  // Code d' init. des parametres de discretisation.
  // C              (choix de NBPNT et de NDJAC).
  // C              = 1 calcul rapide avec precision moyenne sur les coeff.
  // C              = 2 calcul rapide avec meilleure precision "    "    ".
  // C              = 3 calcul un peu plus lent avec bonne precision     ".
  // C              = 4 calcul lent avec la meilleure precision possible ".
  int&                        NumCurves,
  NCollection_Array1<int>&    NumCoeffPerCurveArray,
  NCollection_Array1<double>& CoefficientArray,
  NCollection_Array1<double>& IntervalsArray,
  NCollection_Array1<double>& ErrorMaxArray,
  NCollection_Array1<double>& AverageErrorArray,
  int&                        ErrorCode)
{
  //  double EpsPar =  Precision::Confusion();
  int NUPIL, TheDeg;
#ifdef OCCT_DEBUG
  int NDIMEN = TotalDimension;
#endif
  bool isCut = false;

  // Defintion des Tableaux C
  double* TABINT = (double*)&IntervalsArray(1);

  ErrorCode = 0;
  CoefficientArray.Init(0);

  //-------------------------- Verification des entrees ------------------

  if ((MaxSegments < 1) || (std::abs(Last - First) < 1.e-9))
  {
    ErrorCode = 1;
    return;
  }

  //--> La dimension totale doit etre la somme des dimensions des
  //    sous-espaces
  int IDIM = 0;
  for (int I = 1; I <= TotalNumSS; I++)
  {
    IDIM += LocalDimension(I);
  }
  if (IDIM != TotalDimension)
  {
    ErrorCode = 1;
    return;
  }
  GeomAbs_Shape Continuity = GeomAbs_C0;
  switch (ContinuityOrder)
  {
    case 0:
      Continuity = GeomAbs_C0;
      break;
    case 1:
      Continuity = GeomAbs_C1;
      break;
    case 2:
      Continuity = GeomAbs_C2;
      break;
    default:
      throw Standard_ConstructionError();
  }

  //--------------------- Choix du nombre de points ----------------------
  //---------- et du degre de lissage dans la base orthogonale -----------
  //-->  NDJAC est le degre de "travail" dans la base orthogonale.

  int NbGaussPoints, WorkDegree;

  PLib::JacobiParameters(Continuity, NumMaxCoeffs - 1, code_precis, NbGaussPoints, WorkDegree);
  //      NDJAC=WorkDegree;

  //------------------ Initialisation de la gestion des decoupes ---------

  TABINT[0] = First;
  TABINT[1] = Last;
  NUPIL     = 1;
  NumCurves = 0;

  // C**********************************************************************
  // C                       APPROXIMATION AVEC DECOUPES
  // C**********************************************************************
  PLib_JacobiPolynomial JacobiBase(WorkDegree, Continuity);
  // Portage HP le compilateur refuse le debranchement
  int                    IS;
  bool                   goto_fin_de_boucle;
  int                    MaxDegree = NumMaxCoeffs - 1;
  AdvApprox_SimpleApprox Approx(TotalDimension,
                                TotalNumSS,
                                Continuity,
                                WorkDegree,
                                NbGaussPoints,
                                JacobiBase,
                                Evaluator);
  while ((NUPIL - NumCurves) != 0)
  {
    //--> Lorsque l' on a atteint le haut de la pile, c' est fini !

    // Portage HP le compilateur refuse le debranchement
    goto_fin_de_boucle = false;

    //---- Calcul de la courbe d' approximation dans la base de Jacobi -----

    Approx.Perform(LocalDimension,
                   LocalTolerancesArray,
                   TABINT[NumCurves],
                   TABINT[NumCurves + 1],
                   MaxDegree);
    if (!Approx.IsDone())
    {
      ErrorCode = 1;
      return;
    }

    //---------- Calcul du degre de la courbe et de l' erreur max ----------

    NumCoeffPerCurveArray(NumCurves + 1) = 0;

    //    L'erreur doit etre satisfaite sur tous les sous-espaces sinon, on decoupe

    bool MaxMaxErr = true;
    for (IS = 1; IS <= TotalNumSS; IS++)
    {
      if (Approx.MaxError(IS) > LocalTolerancesArray(IS))
      {
        MaxMaxErr = false;
        break;
      }
    }

    if (MaxMaxErr)
    {
      NumCurves++;
      //            NumCoeffPerCurveArray(NumCurves) = Approx.Degree()+1;
    }
    else
    {
      //-> ...sinon on essai de decouper l' intervalle courant en 2...
      double TMIL;
      bool   Large;

      Large = CutTool.Value(TABINT[NumCurves], TABINT[NumCurves + 1], TMIL);

      if (NUPIL < MaxSegments && Large)
      {

        //	       if (IS!=1) {NumCurves--;}
        isCut         = true; // Ca y est, on le sait !
        double* IDEB  = TABINT + NumCurves + 1;
        double* IDEB1 = IDEB + 1;
        int     ILONG = NUPIL - NumCurves - 1;
        for (int iI = ILONG; iI >= 0; iI--)
        {
          IDEB1[iI] = IDEB[iI];
        }
        IDEB[0] = TMIL;
        NUPIL++;
        //--> ... et on recommence.
        // Portage HP le compilateur refuse le debranchement
        goto_fin_de_boucle = true;
        //	       break;
      }
      else
      {
        //--> Si la pile est pleine...
        // pour l'instant               ErrorCode=-1;
        NumCurves++;
        //               NumCoeffPerCurveArray(NumCurves) = Approx.Degree()+1;
      }
    }
    //         IDIM += NDSES;
    // Portage HP le compilateur refuse le debranchement
    if (goto_fin_de_boucle)
      continue;
    for (IS = 1; IS <= TotalNumSS; IS++)
    {
      ErrorMaxArray.SetValue(IS + (NumCurves - 1) * TotalNumSS, Approx.MaxError(IS));
      //---> ...et calcul de l' erreur moyenne.
      AverageErrorArray.SetValue(IS + (NumCurves - 1) * TotalNumSS, Approx.AverageError(IS));
    }

    occ::handle<NCollection_HArray1<double>> HJacCoeff = Approx.Coefficients();
    TheDeg                                             = Approx.Degree();
    if (isCut && (TheDeg < 2 * ContinuityOrder + 1))
      // Pour ne pas bruiter les derives aux bouts, et garder une continuite
      // correcte sur la BSpline resultat.
      TheDeg = 2 * ContinuityOrder + 1;

    NumCoeffPerCurveArray(NumCurves) = TheDeg + 1;
    NCollection_Array1<double> Coefficients(0, (TheDeg + 1) * TotalDimension - 1);
    JacobiBase.ToCoefficients(TotalDimension, TheDeg, HJacCoeff->Array1(), Coefficients);
    int i, j, f = (TheDeg + 1) * TotalDimension;
    for (i = 0, j = (NumCurves - 1) * TotalDimension * NumMaxCoeffs + 1; i < f; i++, j++)
    {
      CoefficientArray.SetValue(j, Coefficients.Value(i));
    }

#ifdef OCCT_DEBUG
    // Test des derives par difference finis
    int IORDRE = ContinuityOrder;

    if (IORDRE > 0 && AdvApprox_Debug)
    {
      MAPDBN(NDIMEN, TABINT[NumCurves - 1], TABINT[NumCurves], Evaluator, IORDRE);
    }
#endif
    // Portage HP le compilateur refuse le debranchement
    //     fin_de_boucle:
    //     {;}  fin de la boucle while
  }
  return;
}

//=======================================================================
// function : AdvApprox_ApproxAFunction
// purpose  : Constructeur avec Decoupe Dichotomique
//=======================================================================

AdvApprox_ApproxAFunction::AdvApprox_ApproxAFunction(
  const int                                       Num1DSS,
  const int                                       Num2DSS,
  const int                                       Num3DSS,
  const occ::handle<NCollection_HArray1<double>>& OneDTol,
  const occ::handle<NCollection_HArray1<double>>& TwoDTol,
  const occ::handle<NCollection_HArray1<double>>& ThreeDTol,
  const double                                    First,
  const double                                    Last,
  const GeomAbs_Shape                             Continuity,
  const int                                       MaxDeg,
  const int                                       MaxSeg,
  const AdvApprox_EvaluatorFunction&              Func)
    : my1DTolerances(OneDTol),
      my2DTolerances(TwoDTol),
      my3DTolerances(ThreeDTol),
      myFirst(First),
      myLast(Last),
      myContinuity(Continuity),
      myMaxDegree(MaxDeg),
      myMaxSegments(MaxSeg),
      myDone(false),
      myHasResult(false),
      myEvaluator((void*)&Func)
{
  AdvApprox_DichoCutting Cut;
  Perform(Num1DSS, Num2DSS, Num3DSS, Cut);
}

AdvApprox_ApproxAFunction::AdvApprox_ApproxAFunction(
  const int                                       Num1DSS,
  const int                                       Num2DSS,
  const int                                       Num3DSS,
  const occ::handle<NCollection_HArray1<double>>& OneDTol,
  const occ::handle<NCollection_HArray1<double>>& TwoDTol,
  const occ::handle<NCollection_HArray1<double>>& ThreeDTol,
  const double                                    First,
  const double                                    Last,
  const GeomAbs_Shape                             Continuity,
  const int                                       MaxDeg,
  const int                                       MaxSeg,
  const AdvApprox_EvaluatorFunction&              Func,
  const AdvApprox_Cutting&                        CutTool)
    : my1DTolerances(OneDTol),
      my2DTolerances(TwoDTol),
      my3DTolerances(ThreeDTol),
      myFirst(First),
      myLast(Last),
      myContinuity(Continuity),
      myMaxDegree(MaxDeg),
      myMaxSegments(MaxSeg),
      myDone(false),
      myHasResult(false),
      myEvaluator((void*)&Func)
{
  Perform(Num1DSS, Num2DSS, Num3DSS, CutTool);
}

//=======================================================================
// function : AdvApprox_ApproxAFunction::Perform
// purpose  : Make all the Work !!
//=======================================================================
void AdvApprox_ApproxAFunction::Perform(const int                Num1DSS,
                                        const int                Num2DSS,
                                        const int                Num3DSS,
                                        const AdvApprox_Cutting& CutTool)
{
  if (Num1DSS < 0 || Num2DSS < 0 || Num3DSS < 0 || Num1DSS + Num2DSS + Num3DSS <= 0
      || myLast < myFirst || myMaxDegree < 1 || myMaxSegments < 0)
    throw Standard_ConstructionError();
  if (myMaxDegree > 14)
  {
    myMaxDegree = 14;
  }
  //
  // Input
  //
  myNumSubSpaces[0]     = Num1DSS;
  myNumSubSpaces[1]     = Num2DSS;
  myNumSubSpaces[2]     = Num3DSS;
  int    TotalNumSS     = Num1DSS + Num2DSS + Num3DSS, ii, jj, kk, index, dim_index, local_index;
  int    TotalDimension = myNumSubSpaces[0] + 2 * myNumSubSpaces[1] + 3 * myNumSubSpaces[2];
  double error_value;

  int ContinuityOrder = 0;
  switch (myContinuity)
  {
    case GeomAbs_C0:
      ContinuityOrder = 0;
      break;
    case GeomAbs_C1:
      ContinuityOrder = 1;
      break;
    case GeomAbs_C2:
      ContinuityOrder = 2;
      break;
    default:
      throw Standard_ConstructionError();
  }
  double ApproxStartEnd[2];
  int    NumMaxCoeffs = std::max(myMaxDegree + 1, 2 * ContinuityOrder + 2);
  myMaxDegree         = NumMaxCoeffs - 1;
  int code_precis     = 1;
  //
  //  WARNING : the polynomial coefficients are the
  //  taylor expansion of the polynomial at 0.0e0 !
  //
  ApproxStartEnd[0] = -1.0e0;
  ApproxStartEnd[1] = 1.0e0;

  NCollection_Array1<int> LocalDimension(1, TotalNumSS);

  index = 1;
  NCollection_Array1<double> LocalTolerances(1, TotalNumSS);

  for (jj = 1; jj <= myNumSubSpaces[0]; jj++)
  {
    LocalTolerances.SetValue(index, my1DTolerances->Value(jj));
    LocalDimension.SetValue(index, 1);
    index += 1;
  }
  for (jj = 1; jj <= myNumSubSpaces[1]; jj++)
  {
    LocalTolerances.SetValue(index, my2DTolerances->Value(jj));
    LocalDimension.SetValue(index, 2);
    index += 1;
  }
  for (jj = 1; jj <= myNumSubSpaces[2]; jj++)
  {
    LocalTolerances.SetValue(index, my3DTolerances->Value(jj));
    LocalDimension.SetValue(index, 3);
    index += 1;
  }
  //
  // Output
  //

  int ErrorCode = 0, NumCurves, size = myMaxSegments * NumMaxCoeffs * TotalDimension;
  occ::handle<NCollection_HArray1<int>> NumCoeffPerCurvePtr =
    new NCollection_HArray1<int>(1, myMaxSegments);

  occ::handle<NCollection_HArray1<double>> LocalCoefficientsPtr =
    new NCollection_HArray1<double>(1, size);

  occ::handle<NCollection_HArray1<double>> IntervalsPtr =
    new NCollection_HArray1<double>(1, myMaxSegments + 1);

  NCollection_Array1<double> ErrorMax(1, myMaxSegments * TotalNumSS);

  NCollection_Array1<double> AverageError(1, myMaxSegments * TotalNumSS);

  Approximation(TotalDimension,
                TotalNumSS,
                LocalDimension,
                myFirst,
                myLast,
                *(AdvApprox_EvaluatorFunction*)myEvaluator,
                CutTool,
                ContinuityOrder,
                NumMaxCoeffs,
                myMaxSegments,
                LocalTolerances,
                code_precis,
                NumCurves,                            // Nombre de courbe en sortie
                NumCoeffPerCurvePtr->ChangeArray1(),  // Nbre de coeff par courbe
                LocalCoefficientsPtr->ChangeArray1(), // Les Coeffs solutions
                IntervalsPtr->ChangeArray1(),         // La Table des decoupes
                ErrorMax,                             // Majoration de l'erreur
                AverageError,                         // Erreur moyenne constatee
                ErrorCode);

  if (ErrorCode == 0 || ErrorCode == -1)
  {
    //
    // si tout est OK ou bien on a un resultat dont l une des erreurs max est
    // plus grande que la tolerance demandee

    NCollection_Array1<int>    TabContinuity(1, NumCurves);
    NCollection_Array2<double> PolynomialIntervalsPtr(1, NumCurves, 1, 2);
    for (ii = PolynomialIntervalsPtr.LowerRow(); ii <= PolynomialIntervalsPtr.UpperRow(); ii++)
    {
      // On force un degre 1 minimum (PRO5474)
      NumCoeffPerCurvePtr->ChangeValue(ii) = std::max(2, NumCoeffPerCurvePtr->Value(ii));
      PolynomialIntervalsPtr.SetValue(ii, 1, ApproxStartEnd[0]);
      PolynomialIntervalsPtr.SetValue(ii, 2, ApproxStartEnd[1]);
    }

    PrepareConvert(NumCurves,
                   myMaxDegree,
                   ContinuityOrder,
                   Num1DSS,
                   Num2DSS,
                   Num3DSS,
                   NumCoeffPerCurvePtr->Array1(),
                   LocalCoefficientsPtr->ChangeArray1(),
                   PolynomialIntervalsPtr,
                   IntervalsPtr->Array1(),
                   LocalTolerances,
                   ErrorMax,
                   TabContinuity);

    Convert_CompPolynomialToPoles AConverter(NumCurves,
                                             TotalDimension,
                                             myMaxDegree,
                                             TabContinuity,
                                             NumCoeffPerCurvePtr->Array1(),
                                             LocalCoefficientsPtr->Array1(),
                                             PolynomialIntervalsPtr,
                                             IntervalsPtr->Array1());

    if (AConverter.IsDone())
    {
      occ::handle<NCollection_HArray2<double>> PolesPtr;
      AConverter.Poles(PolesPtr);
      AConverter.Knots(myKnots);
      AConverter.Multiplicities(myMults);
      myDegree = AConverter.Degree();
      index    = 0;
      if (myNumSubSpaces[0] > 0)
      {
        my1DPoles = new NCollection_HArray2<double>(1, PolesPtr->ColLength(), 1, myNumSubSpaces[0]);
        my1DMaxError     = new NCollection_HArray1<double>(1, myNumSubSpaces[0]);
        my1DAverageError = new NCollection_HArray1<double>(1, myNumSubSpaces[0]);
        for (ii = 1; ii <= PolesPtr->ColLength(); ii++)
        {
          for (jj = 1; jj <= myNumSubSpaces[0]; jj++)
          {
            my1DPoles->SetValue(ii, jj, PolesPtr->Value(ii, jj));
          }
        }

        for (jj = 1; jj <= myNumSubSpaces[0]; jj++)
        {
          error_value = 0.0e0;
          for (ii = 1; ii <= NumCurves; ii++)
          {
            local_index = (ii - 1) * TotalNumSS;
            error_value = std::max(ErrorMax(local_index + jj), error_value);
          }
          my1DMaxError->SetValue(jj, error_value);
        }
        for (jj = 1; jj <= myNumSubSpaces[0]; jj++)
        {
          error_value = 0.0e0;
          for (ii = 1; ii <= NumCurves; ii++)
          {
            local_index = (ii - 1) * TotalNumSS;
            error_value += AverageError(local_index + jj);
          }
          error_value /= (double)NumCurves;
          my1DAverageError->SetValue(jj, error_value);
        }
        index += myNumSubSpaces[0];
      }

      dim_index = index; // Pour le cas ou il n'y a pas de 2D

      if (myNumSubSpaces[1] > 0)
      {
        gp_Pnt2d Point2d;
        my2DPoles =
          new NCollection_HArray2<gp_Pnt2d>(1, PolesPtr->ColLength(), 1, myNumSubSpaces[1]);
        my2DMaxError     = new NCollection_HArray1<double>(1, myNumSubSpaces[1]);
        my2DAverageError = new NCollection_HArray1<double>(1, myNumSubSpaces[1]);
        for (ii = 1; ii <= PolesPtr->ColLength(); ii++)
        {
          for (jj = 1; jj <= myNumSubSpaces[1]; jj++)
          {
            local_index = index + (jj - 1) * 2;
            for (kk = 1; kk <= 2; kk++)
            {
              Point2d.SetCoord(kk, PolesPtr->Value(ii, local_index + kk));
            }
            my2DPoles->SetValue(ii, jj, Point2d);
          }
        }

        for (jj = 1; jj <= myNumSubSpaces[1]; jj++)
        {
          error_value = 0.0e0;
          for (ii = 1; ii <= NumCurves; ii++)
          {
            local_index = (ii - 1) * TotalNumSS + index;
            error_value = std::max(ErrorMax(local_index + jj), error_value);
          }
          my2DMaxError->SetValue(jj, error_value);
        }
        for (jj = 1; jj <= myNumSubSpaces[1]; jj++)
        {
          error_value = 0.0e0;
          for (ii = 1; ii <= NumCurves; ii++)
          {
            local_index = (ii - 1) * TotalNumSS + index;
            error_value += AverageError(local_index + jj);
          }
          error_value /= (double)NumCurves;
          my2DAverageError->SetValue(jj, error_value);
        }
        index += myNumSubSpaces[1];
        // Pour les poles il faut doubler le decalage :
        dim_index = index + myNumSubSpaces[1];
      }

      if (myNumSubSpaces[2] > 0)
      {
        gp_Pnt Point;
        my3DPoles = new NCollection_HArray2<gp_Pnt>(1, PolesPtr->ColLength(), 1, myNumSubSpaces[2]);
        my3DMaxError     = new NCollection_HArray1<double>(1, myNumSubSpaces[2]);
        my3DAverageError = new NCollection_HArray1<double>(1, myNumSubSpaces[2]);
        for (ii = 1; ii <= PolesPtr->ColLength(); ii++)
        {
          for (jj = 1; jj <= myNumSubSpaces[2]; jj++)
          {
            local_index = dim_index + (jj - 1) * 3;
            for (kk = 1; kk <= 3; kk++)
            {
              Point.SetCoord(kk, PolesPtr->Value(ii, local_index + kk));
            }
            my3DPoles->SetValue(ii, jj, Point);
          }
        }

        for (jj = 1; jj <= myNumSubSpaces[2]; jj++)
        {
          error_value = 0.0e0;
          for (ii = 1; ii <= NumCurves; ii++)
          {
            local_index = (ii - 1) * TotalNumSS + index;
            error_value = std::max(ErrorMax(local_index + jj), error_value);
          }
          my3DMaxError->SetValue(jj, error_value);
        }
        for (jj = 1; jj <= myNumSubSpaces[2]; jj++)
        {
          error_value = 0.0e0;
          for (ii = 1; ii <= NumCurves; ii++)
          {
            local_index = (ii - 1) * TotalNumSS + index;
            error_value += AverageError(local_index + jj);
          }
          error_value /= (double)NumCurves;
          my3DAverageError->SetValue(jj, error_value);
        }
      }
      if (ErrorCode == 0)
      {
        myDone      = true;
        myHasResult = true;
      }
      else if (ErrorCode == -1)
      {
        myHasResult = true;
      }
    }
  }
}

//=================================================================================================

void AdvApprox_ApproxAFunction::Poles(const int Index, NCollection_Array1<gp_Pnt>& P) const
{
  int ii;
  for (ii = P.Lower(); ii <= P.Upper(); ii++)
  {
    P.SetValue(ii, my3DPoles->Value(ii, Index));
  }
}

//=================================================================================================

int AdvApprox_ApproxAFunction::NbPoles() const
{
  if (myDone || myHasResult)
    return BSplCLib::NbPoles(myDegree, false, myMults->Array1());
  return 0;
}

//=================================================================================================

void AdvApprox_ApproxAFunction::Poles2d(const int Index, NCollection_Array1<gp_Pnt2d>& P) const
{
  int ii;
  for (ii = P.Lower(); ii <= P.Upper(); ii++)
  {
    P.SetValue(ii, my2DPoles->Value(ii, Index));
  }
}

//=================================================================================================

void AdvApprox_ApproxAFunction::Poles1d(const int Index, NCollection_Array1<double>& P) const
{
  int ii;
  for (ii = P.Lower(); ii <= P.Upper(); ii++)
  {
    P.SetValue(ii, my1DPoles->Value(ii, Index));
  }
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> AdvApprox_ApproxAFunction::MaxError(const int D) const

{
  occ::handle<NCollection_HArray1<double>> EPtr;
  if (D <= 0 || D > 3)
  {

    throw Standard_OutOfRange();
  }
  switch (D)
  {
    case 1:
      EPtr = my1DMaxError;
      break;
    case 2:
      EPtr = my2DMaxError;
      break;
    case 3:
      EPtr = my3DMaxError;
      break;
  }
  return EPtr;
}

//=================================================================================================

double AdvApprox_ApproxAFunction::MaxError(const int D, const int Index) const
{
  occ::handle<NCollection_HArray1<double>> EPtr = MaxError(D);

  return EPtr->Value(Index);
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> AdvApprox_ApproxAFunction::AverageError(const int D) const

{
  occ::handle<NCollection_HArray1<double>> EPtr;
  if (D <= 0 || D > 3)
  {

    throw Standard_OutOfRange();
  }
  switch (D)
  {
    case 1:
      EPtr = my1DAverageError;
      break;
    case 2:
      EPtr = my2DAverageError;
      break;
    case 3:
      EPtr = my3DAverageError;
      break;
  }
  return EPtr;
}

//=================================================================================================

double AdvApprox_ApproxAFunction::AverageError(const int D, const int Index) const

{
  occ::handle<NCollection_HArray1<double>> EPtr = AverageError(D);
  return EPtr->Value(Index);
}

void AdvApprox_ApproxAFunction::Dump(Standard_OStream& o) const
{
  int ii;
  o << "Dump of ApproxAFunction" << std::endl;
  if (myNumSubSpaces[0] > 0)
  {
    o << "Error(s) 1d = " << std::endl;
    for (ii = 1; ii <= myNumSubSpaces[0]; ii++)
    {
      o << "   " << MaxError(1, ii) << std::endl;
    }
  }

  if (myNumSubSpaces[1] > 0)
  {
    o << "Error(s) 2d = " << std::endl;
    for (ii = 1; ii <= myNumSubSpaces[1]; ii++)
    {
      o << "   " << MaxError(2, ii) << std::endl;
    }
  }

  if (myNumSubSpaces[2] > 0)
  {
    o << "Error(s) 3d = " << std::endl;
    for (ii = 1; ii <= myNumSubSpaces[2]; ii++)
    {
      o << "   " << MaxError(3, ii) << std::endl;
    }
  }
}
