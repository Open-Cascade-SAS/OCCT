// Created on: 1992-06-29
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

#include <stdio.h>

#include <Standard_Macro.hxx>

#include <iostream>

#include <iomanip>

#include <fstream>

#ifndef OCCT_DEBUG
  #define No_Standard_RangeError
  #define No_Standard_OutOfRange
#endif

//======================================================================
//== I n t e r s e c t i o n   C O N E           Q U A D R I Q U E
//==                           C Y L I N D R E   Q U A D R I Q U E
//======================================================================

#include <ElSLib.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>
#include <IntAna_IntQuadQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <math_TrigonometricFunctionRoots.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
// function : AddSpecialPoints
// purpose  : Sometimes the boundaries theTheta1 and theTheta2 are
//            computed with some inaccuracy. At that, some special points
//            (cone apex or sphere pole(s)), which are true intersection
//            points lie out of the domain [theTheta1, theTheta2] of the ALine.
//           This function corrects these boundaries to make them be included
//            in the domain of the ALine.
//           Parameters Theta1 and Theta2 must be initialized
//            before calling this function.
//=======================================================================
template <class gpSmth>
static void AddSpecialPoints(const IntAna_Quadric& theQuad,
                             const gpSmth&         theGpObj,
                             double&        theTheta1,
                             double&        theTheta2)
{
  const double             aPeriod = M_PI + M_PI;
  const NCollection_List<gp_Pnt>& aLSP    = theQuad.SpecialPoints();

  if (aLSP.IsEmpty())
    return;

  double aU = 0.0, aV = 0.0;
  double aMaxDelta = 0.0;
  for (NCollection_List<gp_Pnt>::Iterator anItr(aLSP); anItr.More(); anItr.Next())
  {
    const gp_Pnt& aPt = anItr.Value();
    ElSLib::Parameters(theGpObj, aPt, aU, aV);
    const gp_Pnt aPProj(ElSLib::Value(aU, aV, theGpObj));

    if (aPt.SquareDistance(aPProj) > Precision::SquareConfusion())
    {
      // aPt is not an intersection point
      continue;
    }

    double aDelta1 = std::min(aU - theTheta1, 0.0), aDelta2 = std::max(aU - theTheta2, 0.0);

    if (aDelta1 < -M_PI)
    {
      // Must be aDelta1 = std::min(aU - theTheta1 + aPeriod, 0.0).
      // But aU - theTheta1 + aPeriod >= 0 always.
      aDelta1 = 0.0;
    }

    if (aDelta2 > M_PI)
    {
      // Must be aDelta2 = std::max(aU - theTheta2 - aPeriod, 0.0).
      // But aU - theTheta2 - aPeriod <= 0 always.
      aDelta2 = 0.0;
    }

    const double aDelta = std::max(-aDelta1, aDelta2);
    aMaxDelta                  = std::max(aMaxDelta, aDelta);
  }

  if (aMaxDelta != 0.0)
  {
    theTheta1 -= aMaxDelta;
    theTheta2 += aMaxDelta;
    if ((theTheta2 - theTheta1) > aPeriod)
    {
      theTheta2 = theTheta1 + aPeriod;
    }
  }
}

//=======================================================================
// class : TrigonometricRoots
// purpose: Classe Interne (Donne des racines classees d un polynome trigo)
//======================================================================
class TrigonometricRoots
{

private:
  double    Roots[4];
  bool done;
  int NbRoots;
  bool infinite_roots;

public:
  TrigonometricRoots(const double CC,
                     const double SC,
                     const double C,
                     const double S,
                     const double Cte,
                     const double Binf,
                     const double Bsup);

  // IsDone
  bool IsDone() { return done; }

  // IsARoot
  bool IsARoot(double u)
  {
    int        i;
    constexpr double aEps  = RealEpsilon();
    double           PIpPI = M_PI + M_PI;
    //
    for (i = 0; i < NbRoots; ++i)
    {
      if (std::abs(u - Roots[i]) <= aEps)
      {
        return true;
      }
      if (std::abs(u - Roots[i] - PIpPI) <= aEps)
      {
        return true;
      }
    }
    return false;
  }

  // NbSolutions
  int NbSolutions()
  {
    if (!done)
    {
      throw StdFail_NotDone();
    }
    return NbRoots;
  }

  // InfiniteRoots
  bool InfiniteRoots()
  {
    if (!done)
    {
      throw StdFail_NotDone();
    }
    return infinite_roots;
  }

  // Value
  double Value(const int n)
  {
    if ((!done) || (n > NbRoots))
    {
      throw StdFail_NotDone();
    }
    return Roots[n - 1];
  }
};

//=================================================================================================

TrigonometricRoots::TrigonometricRoots(const double CC,
                                       const double SC,
                                       const double C,
                                       const double S,
                                       const double Cte,
                                       const double Binf,
                                       const double Bsup)
    : infinite_roots(false)
{
  int i, j, SvNbRoots;
  bool Triee;
  double    PIpPI = M_PI + M_PI;
  //
  done = false;
  //
  //-- F= AA*CN*CN+2*BB*CN*SN+CC*CN+DD*SN+EE;
  math_TrigonometricFunctionRoots MTFR(CC, SC, C, S, Cte, Binf, Bsup);
  if (!MTFR.IsDone())
  {
    return;
  }
  //
  done = true;
  if (MTFR.InfiniteRoots())
  {
    infinite_roots = true;
    return;
  }
  //
  NbRoots = MTFR.NbSolutions();
  //
  for (i = 0; i < NbRoots; ++i)
  {
    Roots[i] = MTFR.Value(i + 1);
    if (Roots[i] < 0.)
    {
      Roots[i] += PIpPI;
    }
    if (Roots[i] > PIpPI)
    {
      Roots[i] -= PIpPI;
    }
  }
  //
  //-- La recherche directe donne n importe quoi.
  SvNbRoots = NbRoots;
  for (i = 0; i < SvNbRoots; ++i)
  {
    double y;
    double co = cos(Roots[i]);
    double si = sin(Roots[i]);
    y                = co * (CC * co + (SC + SC) * si + C) + S * si + Cte;
    if (std::abs(y) > 1e-8)
    {
      done = false;
      return; //-- le 1er avril 98
    }
  }
  //
  do
  {
    Triee = true;
    for (i = 1, j = 0; i < SvNbRoots; ++i, ++j)
    {
      if (Roots[i] < Roots[j])
      {
        Triee           = false;
        double t = Roots[i];
        Roots[i]        = Roots[j];
        Roots[j]        = t;
      }
    }
  } while (!Triee);
  //
  infinite_roots = false;
  //
  if (!NbRoots)
  { //--!!!!! Detection du cas Pol = Cte ( 1e-50 ) !!!!
    if ((std::abs(CC) + std::abs(SC) + std::abs(C) + std::abs(S)) < 1e-10)
    {
      if (std::abs(Cte) < 1e-10)
      {
        infinite_roots = true;
      }
    }
  }
}

//=======================================================================
// class    : MyTrigonometricFunction
// purpose  :
//  Classe interne : Donne Value et Derivative sur un polynome
//                   trigonometrique
//======================================================================
class MyTrigonometricFunction
{

private:
  double CC, SS, SC, S, C, Cte;

public:
  //
  MyTrigonometricFunction(const double xCC,
                          const double xSS,
                          const double xSC,
                          const double xC,
                          const double xS,
                          const double xCte)
  {
    CC  = xCC;
    SS  = xSS;
    SC  = xSC;
    S   = xS;
    C   = xC;
    Cte = xCte;
  }

  double Value(const double& U)
  {
    double sinus, cosinus, aRet;
    //
    sinus   = sin(U);
    cosinus = cos(U);
    aRet    = CC * cosinus * cosinus + SS * sinus * sinus
           + 2.0 * (sinus * (SC * cosinus + S) + cosinus * C) + Cte;
    //
    return aRet;
  }

  //
  double Derivative(const double& U)
  {
    double sinus, cosinus;
    //
    sinus   = sin(U);
    cosinus = cos(U);
    //
    return (2.0
            * ((sinus * cosinus) * (SS - CC) + S * cosinus - C * sinus
               + SC * (cosinus * cosinus - sinus * sinus)));
  }
};

//////////
//=======================================================================
// function : IntAna_IntQuadQuad::IntAna_IntQuadQuad
// purpose  : C o n s t r u c t e u r    v i d e
//=======================================================================
IntAna_IntQuadQuad::IntAna_IntQuadQuad(void)
{
  done                   = false;
  identical              = false;
  NbCurves               = 0;
  Nbpoints               = 0;
  myNbMaxCurves          = 12;
  myEpsilon              = 0.00000001;
  myEpsilonCoeffPolyNull = 0.00000001;
  memset(nextcurve, 0, sizeof(nextcurve));
  memset(previouscurve, 0, sizeof(previouscurve));
}

//=======================================================================
// function : IntAna_IntQuadQuad::IntAna_IntQuadQuad
// purpose  : I n t e r s e c t i o n   C y l i n d r e   Q u a d r i q u e
//=======================================================================
IntAna_IntQuadQuad::IntAna_IntQuadQuad(const gp_Cylinder&    Cyl,
                                       const IntAna_Quadric& Quad,
                                       const double   Tol)
{
  myNbMaxCurves          = 12;
  myEpsilon              = 0.00000001;
  myEpsilonCoeffPolyNull = 0.00000001;
  Perform(Cyl, Quad, Tol);
}

//=======================================================================
// function : Perform
// purpose  : I n t e r s e c t i o n   C y l i n d r e   Q u a d r i q u e
//=======================================================================
void IntAna_IntQuadQuad::Perform(const gp_Cylinder&    Cyl,
                                 const IntAna_Quadric& Quad,
                                 const double)
{
  done      = true;
  identical = false;
  NbCurves  = 0;
  Nbpoints  = 0;
  //
  bool UN_SEUL_Z_PAR_THETA, DEUX_Z_PAR_THETA, Z_POSITIF, Z_INDIFFERENT, Z_NEGATIF;
  //
  UN_SEUL_Z_PAR_THETA = false;
  DEUX_Z_PAR_THETA    = true;
  Z_POSITIF           = true;
  Z_INDIFFERENT       = true;
  Z_NEGATIF           = false;
  //
  double Qxx, Qyy, Qzz, Qxy, Qxz, Qyz, Qx, Qy, Qz, Q1, aRealEpsilon, RCyl, R2;
  double PIpPI = M_PI + M_PI;
  //
  for (int raz = 0; raz < myNbMaxCurves; raz++)
  {
    previouscurve[raz] = nextcurve[raz] = 0;
  }
  //
  RCyl         = Cyl.Radius();
  aRealEpsilon = RealEpsilon();
  //----------------------------------------------------------------------
  //-- Coefficients de la quadrique :
  //--      2       2       2
  //-- Qxx x + Qyy y + Qzz z + 2 ( Qxy x y + Qxz x z + Qyz y z )
  //-- + 2 ( Qx x + Qy y + Qz z ) + Q1
  //----------------------------------------------------------------------
  Quad.Coefficients(Qxx, Qyy, Qzz, Qxy, Qxz, Qyz, Qx, Qy, Qz, Q1);

  //----------------------------------------------------------------------
  //-- Ecriture des Coefficients de la Quadrique dans le repere liee
  //-- au Cylindre
  //--                 Cyl.Position() -> Ax2
  //----------------------------------------------------------------------
  Quad.NewCoefficients(Qxx, Qyy, Qzz, Qxy, Qxz, Qyz, Qx, Qy, Qz, Q1, Cyl.Position());

  //----------------------------------------------------------------------
  //-- Parametrage du Cylindre Cyl :
  //--     X = Rcyl * std::cos(Theta)
  //--     Y = Rcyl * std::sin(Theta)
  //--     Z = Z
  //----------------------------------------------------------------------
  //-- Donne une Equation de la forme :
  //--   F(std::sin(Theta),std::cos(Theta),ZCyl) = 0
  //--   (Equation du second degre en ZCyl)
  //--    ZCyl**2  CoeffZ2(Theta) + ZCyl CoeffZ1(Theta) + CoeffZ0(Theta)
  //----------------------------------------------------------------------
  //-- CoeffZ0 = Q1 + 2*Qx*RCyl*Cos[Theta] + Qxx*RCyl^2*Cos[Theta]^2
  //--           2*RCyl*Sin[Theta]* ( Qy + Qxy*RCyl*Cos[Theta]);
  //--           Qyy*RCyl^2*Sin[Theta]^2;
  //-- CoeffZ1 =2.0 * ( Qz + RCyl*(Qxz*Cos[Theta] + Sin[Theta]*Qyz)) ;
  //-- CoeffZ2 =  Qzz;
  //-- !!!! Attention , si on norme sur Qzz pour detecter le cas 1er degre
  //----------------------------------------------------------------------
  //-- On Cherche Les Racines en Theta du discriminant de cette equation :
  //-- DIS(Theta) = C_1 + C_SS std::sin()**2 + C_CC std::cos()**2 + 2 C_SC std::sin() std::cos()
  //--                  + 2 C_S  std::sin()    + 2 C_C std::cos()
  //--
  //-- Si Qzz = 0   Alors  On Resout Z=Fct(Theta)  sur le domaine de Theta
  //----------------------------------------------------------------------

  if (std::abs(Qzz) < myEpsilonCoeffPolyNull)
  {
    done = false; //-- le 12 mars 98
    return;
  }
  else
  { // #0
    //----------------------------------------------------------------------
    //--- Cas  ou  F(Z,Theta) est du second degre en Z                  ----
    //----------------------------------------------------------------------
    R2 = RCyl * RCyl;

    double C_1  = Qz * Qz - Qzz * Q1;
    double C_SS = R2 * (Qyz * Qyz - Qyy * Qzz);
    double C_CC = R2 * (Qxz * Qxz - Qxx * Qzz);
    double C_S  = RCyl * (Qyz * Qz - Qy * Qzz);
    double C_C  = RCyl * (Qxz * Qz - Qx * Qzz);
    double C_SC = R2 * (Qxz * Qyz - Qxy * Qzz);
    //
    MyTrigonometricFunction MTF(C_CC, C_SS, C_SC, C_C, C_S, C_1);
    TrigonometricRoots      PolDIS(C_CC - C_SS, C_SC, C_C + C_C, C_S + C_S, C_1 + C_SS, 0., PIpPI);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (PolDIS.IsDone() == false)
    {
      done = false;
      return;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (PolDIS.InfiniteRoots())
    {
      TheCurve[0].SetCylinderQuadValues(Cyl,
                                        Qxx,
                                        Qyy,
                                        Qzz,
                                        Qxy,
                                        Qxz,
                                        Qyz,
                                        Qx,
                                        Qy,
                                        Qz,
                                        Q1,
                                        myEpsilon,
                                        0.,
                                        PIpPI,
                                        UN_SEUL_Z_PAR_THETA,
                                        Z_POSITIF);
      TheCurve[1].SetCylinderQuadValues(Cyl,
                                        Qxx,
                                        Qyy,
                                        Qzz,
                                        Qxy,
                                        Qxz,
                                        Qyz,
                                        Qx,
                                        Qy,
                                        Qz,
                                        Q1,
                                        myEpsilon,
                                        0.,
                                        PIpPI,
                                        UN_SEUL_Z_PAR_THETA,
                                        Z_NEGATIF);
      NbCurves = 2;
    }

    else
    { // #1
      //---------------------------------------------------------------
      //-- La Recherche des Zero de DIS a reussi
      //---------------------------------------------------------------
      int nbsolDIS = PolDIS.NbSolutions();
      if (nbsolDIS == 0)
      {
        //--------------------------------------------------------------
        //-- Le Discriminant a un signe constant :
        //--
        //-- Si Positif  ---> 2 Courbes
        //-- Sinon       ---> Pas de solution
        //--------------------------------------------------------------
        if (MTF.Value(M_PI) >= -aRealEpsilon)
        {

          TheCurve[0].SetCylinderQuadValues(Cyl,
                                            Qxx,
                                            Qyy,
                                            Qzz,
                                            Qxy,
                                            Qxz,
                                            Qyz,
                                            Qx,
                                            Qy,
                                            Qz,
                                            Q1,
                                            myEpsilon,
                                            0.0,
                                            PIpPI,
                                            UN_SEUL_Z_PAR_THETA,
                                            Z_POSITIF);
          TheCurve[1].SetCylinderQuadValues(Cyl,
                                            Qxx,
                                            Qyy,
                                            Qzz,
                                            Qxy,
                                            Qxz,
                                            Qyz,
                                            Qx,
                                            Qy,
                                            Qz,
                                            Q1,
                                            myEpsilon,
                                            0.0,
                                            PIpPI,
                                            UN_SEUL_Z_PAR_THETA,
                                            Z_NEGATIF);

          NbCurves = 2;
        }
        else
        {
          //------------------------------------------------------------
          //-- Le Discriminant est toujours Negatif
          //------------------------------------------------------------
          NbCurves = 0;
        }
      }
      else
      { // #2
        //------------------------------------------------------------
        //-- Le Discriminant a des racines
        //-- (Le Discriminant est une fonction periodique donc ... )
        //------------------------------------------------------------
        if (nbsolDIS == 1)
        {
          //------------------------------------------------------------
          //-- Point de Tangence pour ce Theta Solution
          //-- Si Signe de Discriminant >=0 pour tout Theta
          //--     Alors
          //--       Courbe Solution pour la partie Positive
          //--       entre les 2 racines ( Ici Tout le domaine )
          //-- Sinon Seulement un point Tangent
          //------------------------------------------------------------
          if (MTF.Value(PolDIS.Value(1) + M_PI) >= -aRealEpsilon)
          {
            //------------------------------------------------------------
            //-- On a Un Point de Tangence + Une Courbe Solution
            //------------------------------------------------------------
            TheCurve[0].SetCylinderQuadValues(Cyl,
                                              Qxx,
                                              Qyy,
                                              Qzz,
                                              Qxy,
                                              Qxz,
                                              Qyz,
                                              Qx,
                                              Qy,
                                              Qz,
                                              Q1,
                                              myEpsilon,
                                              0.0,
                                              PIpPI,
                                              UN_SEUL_Z_PAR_THETA,
                                              Z_POSITIF);
            TheCurve[1].SetCylinderQuadValues(Cyl,
                                              Qxx,
                                              Qyy,
                                              Qzz,
                                              Qxy,
                                              Qxz,
                                              Qyz,
                                              Qx,
                                              Qy,
                                              Qz,
                                              Q1,
                                              myEpsilon,
                                              0.0,
                                              PIpPI,
                                              UN_SEUL_Z_PAR_THETA,
                                              Z_NEGATIF);

            NbCurves = 2;
          }
          else
          {
            //------------------------------------------------------------
            //-- On a simplement un Point de tangence
            //------------------------------------------------------------
            //--double Theta = PolDIS(1);
            //--double SecPar= -0.5 * MTFZ1.Value(Theta) / MTFZ2.Value(Theta);
            //--Thepoints[Nbpoints] =
            // ElSLib::CylinderValue(Theta,SecPar,Cyl.Position(),Cyl.Radius());
            //--Nbpoints++;
            NbCurves = 0;
          }
        }
        else
        { // #3
          //------------------------------------------------------------
          //-- On detecte   :  Les racines double
          //--              :  Les Zones Positives [Modulo 2 PI]
          //-- Les Courbes solutions seront obtenues pour les valeurs
          //-- de Theta ou Discriminant(Theta) > 0  (>=0? en limite)
          //-- Par la resolution de l equation implicite avec Theta fixe
          //------------------------------------------------------------
          //-- Si tout est solution, Alors on est sur une iso
          //-- Ce cas devrait etre traite en amont
          //------------------------------------------------------------
          //-- On construit la fonction permettant connaissant un Theta
          //-- de calculer les Z+ et Z- Correspondants.
          //-------------------------------------------------------------

          //-------------------------------------------------------------
          //-- Calcul des Intervalles ou Discriminant >=0
          //-- On a au plus nbsol intervalles ( en fait 2 )
          //--  (1,2) (2,3) .. (nbsol,1+2PI)
          //-------------------------------------------------------------
          int i;
          double    Theta1, Theta2, Theta3, autrepar, qwet;
          bool UnPtTg = false;
          //
          NbCurves = 0;
          if (nbsolDIS == 2)
          {
            for (i = 1; i <= nbsolDIS; i++)
            {
              Theta1 = PolDIS.Value(i);
              Theta2 = (i < nbsolDIS) ? PolDIS.Value(i + 1) : (PolDIS.Value(1) + PIpPI);
              //----------------------------------------------------------------
              //-- On detecte les racines doubles
              //-- Si il n y a que 2 racines alors on prend tout l interval
              //----------------------------------------------------------------
              if (std::abs(Theta2 - Theta1) <= aRealEpsilon)
              {
                UnPtTg   = true;
                autrepar = Theta1 - 0.1;
                if (autrepar < 0.)
                {
                  autrepar = Theta1 + 0.1;
                }
                //
                qwet = MTF.Value(autrepar);
                if (qwet >= 0.)
                {
                  double aParam = Theta1 + PIpPI;
                  AddSpecialPoints(Quad, Cyl, Theta1, aParam);
                  TheCurve[NbCurves].SetCylinderQuadValues(Cyl,
                                                           Qxx,
                                                           Qyy,
                                                           Qzz,
                                                           Qxy,
                                                           Qxz,
                                                           Qyz,
                                                           Qx,
                                                           Qy,
                                                           Qz,
                                                           Q1,
                                                           myEpsilon,
                                                           Theta1,
                                                           aParam,
                                                           UN_SEUL_Z_PAR_THETA,
                                                           Z_POSITIF);
                  NbCurves++;
                  TheCurve[NbCurves].SetCylinderQuadValues(Cyl,
                                                           Qxx,
                                                           Qyy,
                                                           Qzz,
                                                           Qxy,
                                                           Qxz,
                                                           Qyz,
                                                           Qx,
                                                           Qy,
                                                           Qz,
                                                           Q1,
                                                           myEpsilon,
                                                           Theta1,
                                                           aParam,
                                                           UN_SEUL_Z_PAR_THETA,
                                                           Z_NEGATIF);
                  NbCurves++;
                }
              }
            }
          }

          for (i = 1; UnPtTg == (false) && (i <= nbsolDIS); i++)
          {
            Theta1 = PolDIS.Value(i);
            Theta2 = (i < nbsolDIS) ? PolDIS.Value(i + 1) : (PolDIS.Value(1) + PIpPI);
            //----------------------------------------------------------------
            //-- On detecte les racines doubles
            //-- Si il n y a que 2 racines alors on prend tout l interval
            //----------------------------------------------------------------
            if (std::abs(Theta2 - Theta1) <= 1e-12)
            {
              //-- std::cout<<"\n####### Un Point de Tangence en Theta = "<<Theta1<<std::endl;
              //-- i++;
            }
            else
            { //-- On evite les pbs numeriques (Tout est du meme signe entre les racines)
              qwet = MTF.Value(0.5 * (Theta1 + Theta2)) + MTF.Value(0.4 * Theta1 + 0.6 * Theta2)
                     + MTF.Value(0.6 * Theta1 + 0.4 * Theta2);
              if (qwet >= 0.)
              {
                //------------------------------------------------------------
                //-- On est positif a partir de Theta1
                //-- L intervalle Theta1,Theta2 est retenu
                //------------------------------------------------------------

                //-- le 8 octobre 1997 :
                //-- PB: Racine en 0    pi/2 pi/2  et 2pi
                //-- On cree 2 courbes : 0    -> pi/2    2zpartheta
                //--                     pi/2 -> 2pi     2zpartheta
                //--
                //-- la courbe 0 pi/2   passe par le double pt de tangence pi/2
                //-- il faut donc couper cette courbe en 2
                //--
                Theta3 = ((i + 1) < nbsolDIS) ? PolDIS.Value(i + 2) : (PolDIS.Value(1) + PIpPI);
                // ft
                if ((Theta3 - Theta2) < 5.e-8)
                {
                  //
                  AddSpecialPoints(Quad, Cyl, Theta1, Theta2);
                  TheCurve[NbCurves].SetCylinderQuadValues(Cyl,
                                                           Qxx,
                                                           Qyy,
                                                           Qzz,
                                                           Qxy,
                                                           Qxz,
                                                           Qyz,
                                                           Qx,
                                                           Qy,
                                                           Qz,
                                                           Q1,
                                                           myEpsilon,
                                                           Theta1,
                                                           Theta2,
                                                           UN_SEUL_Z_PAR_THETA,
                                                           Z_POSITIF);
                  NbCurves++;
                  TheCurve[NbCurves].SetCylinderQuadValues(Cyl,
                                                           Qxx,
                                                           Qyy,
                                                           Qzz,
                                                           Qxy,
                                                           Qxz,
                                                           Qyz,
                                                           Qx,
                                                           Qy,
                                                           Qz,
                                                           Q1,
                                                           myEpsilon,
                                                           Theta1,
                                                           Theta2,
                                                           UN_SEUL_Z_PAR_THETA,
                                                           Z_NEGATIF);
                  NbCurves++;
                }
                else
                {
                  AddSpecialPoints(Quad, Cyl, Theta1, Theta2);
                  TheCurve[NbCurves].SetCylinderQuadValues(Cyl,
                                                           Qxx,
                                                           Qyy,
                                                           Qzz,
                                                           Qxy,
                                                           Qxz,
                                                           Qyz,
                                                           Qx,
                                                           Qy,
                                                           Qz,
                                                           Q1,
                                                           myEpsilon,
                                                           Theta1,
                                                           Theta2,
                                                           DEUX_Z_PAR_THETA,
                                                           Z_INDIFFERENT);
                  NbCurves++;
                }
              } // if(qwet >= 0.)
            } // else {  //-- On evite les pbs numerique ...
          } // for(i=1; UnPtTg == (false) && (i<=nbsolDIS) ; i++) {
        } // else { // #3
      } // else { //#2
    } // else { //#1

  } // else { //#0
}

//=================================================================================================

IntAna_IntQuadQuad::IntAna_IntQuadQuad(const gp_Cone&        Cone,
                                       const IntAna_Quadric& Quad,
                                       const double   Tol)
{
  myNbMaxCurves          = 12;
  myEpsilon              = 0.00000001;
  myEpsilonCoeffPolyNull = 0.00000001;
  Perform(Cone, Quad, Tol);
}

//=================================================================================================

void IntAna_IntQuadQuad::Perform(const gp_Cone&        Cone,
                                 const IntAna_Quadric& Quad,
                                 const double)
{
  //
  bool UN_SEUL_Z_PAR_THETA, Z_POSITIF, Z_NEGATIF;
  //
  UN_SEUL_Z_PAR_THETA = false;
  Z_POSITIF           = true;
  Z_NEGATIF           = false;
  //
  int i;
  double    Qxx, Qyy, Qzz, Qxy, Qxz, Qyz, Qx, Qy, Qz, Q1;
  double    Theta1, Theta2, TgAngle;
  double    PIpPI = M_PI + M_PI;
  //
  done      = true;
  identical = false;
  NbCurves  = 0;
  Nbpoints  = 0;
  //
  for (i = 0; i < myNbMaxCurves; ++i)
  {
    previouscurve[i] = 0;
    nextcurve[i]     = 0;
  }
  //
  Quad.Coefficients(Qxx, Qyy, Qzz, Qxy, Qxz, Qyz, Qx, Qy, Qz, Q1);
  //
  gp_Ax3 tAx3(Cone.Position());
  tAx3.SetLocation(Cone.Apex());
  Quad.NewCoefficients(Qxx, Qyy, Qzz, Qxy, Qxz, Qyz, Qx, Qy, Qz, Q1, tAx3);
  //
  TgAngle = 1. / (std::tan(Cone.SemiAngle()));
  //
  // The parametrization of the Cone
  //
  // x= z*tan(beta)*cos(t)
  // y= z*tan(beta)*sin(t)
  // z=z
  //
  // The intersection curves are defined by the equation
  //
  //                        2
  //          f(z,t)= A(t)*z + B(t)*z + C(t)=0
  //
  //
  // 1. Try to solve A(t)=0 -> PolZ2
  //
  int nbsol, nbsol1, nbsolZ2;
  double    Z2CC, Z2SS, Z2Cte, Z2SC, Z2C, Z2S;
  double    Z1CC, Z1SS, Z1Cte, Z1SC, Z1C, Z1S;
  double    C_1, C_SS, C_CC, C_S, C_C, C_SC;
  //
  Z2CC  = Qxx;
  Z2SS  = Qyy;
  Z2Cte = Qzz * TgAngle * TgAngle;
  Z2SC  = Qxy;
  Z2C   = (TgAngle)*Qxz;
  Z2S   = (TgAngle)*Qyz;
  //
  TrigonometricRoots PolZ2(Z2CC - Z2SS, Z2SC, Z2C + Z2C, Z2S + Z2S, Z2Cte + Z2SS, 0., PIpPI);
  if (!PolZ2.IsDone())
  {
    done = !done;
    return;
  }
  //
  // MyTrigonometricFunction MTF2(Z2CC,Z2SS,Z2SC,Z2C,Z2S,Z2Cte);
  nbsolZ2 = PolZ2.NbSolutions();
  //
  // 2. Try to solve B(t)=0  -> PolZ1
  //
  Z1Cte = 2. * (TgAngle)*Qz;
  Z1SS  = 0.;
  Z1CC  = 0.;
  Z1S   = Qy;
  Z1C   = Qx;
  Z1SC  = 0.;
  //
  TrigonometricRoots PolZ1(Z1CC - Z1SS, Z1SC, Z1C + Z1C, Z1S + Z1S, Z1Cte + Z1SS, 0., PIpPI);
  if (!PolZ1.IsDone())
  {
    done = !done;
    return;
  }
  MyTrigonometricFunction MTFZ1(Z1CC, Z1SS, Z1SC, Z1C, Z1S, Z1Cte);
  //
  nbsol1 = PolZ1.NbSolutions();
  if (PolZ2.InfiniteRoots())
  { // i.e A(t)=0 for any t
    if (!PolZ1.InfiniteRoots())
    {
      if (!nbsol1)
      {
        //-- B(t)*z + C(t) = 0    avec C(t) != 0
        TheCurve[0].SetConeQuadValues(Cone,
                                      Qxx,
                                      Qyy,
                                      Qzz,
                                      Qxy,
                                      Qxz,
                                      Qyz,
                                      Qx,
                                      Qy,
                                      Qz,
                                      Q1,
                                      myEpsilon,
                                      0.,
                                      PIpPI,
                                      UN_SEUL_Z_PAR_THETA,
                                      Z_POSITIF);
        NbCurves = 1;
      }
    }
    else
    {
      if (std::abs(Q1) <= myEpsilon)
      {
        done = !done;
        return;
      }
      else
      {
        //-- Pas de Solutions
      }
    }
    return;
  }
  //
  // else { //#5
  //
  //                2
  //-- f(z,t)=A(t)*z + B(t)*z + C(t)=0   avec A n est pas toujours nul
  //
  //                                              2
  // 3. Try to explore s.c. Discriminant:  D(t)=B(t)-4*A(t)*C(t) => Pol
  //
  // The function D(t) is just a formula that has sense for quadratic
  // equation above.
  // For cases when A(t)=0 (say at t=ti, t=tj. etc) the equation
  // will be
  //
  //     f(z,t)=B(t)*z + C(t)=0, where B(t)!=0,
  //
  // and the D(t) is nonsense for it.
  //
  C_1  = TgAngle * TgAngle * (Qz * Qz - Qzz * Q1);
  C_SS = Qy * Qy - Qyy * Q1;
  C_CC = Qx * Qx - Qxx * Q1;
  C_S  = TgAngle * (Qy * Qz - Qyz * Q1);
  C_C  = TgAngle * (Qx * Qz - Qxz * Q1);
  C_SC = Qx * Qy - Qxy * Q1;
  //
  TrigonometricRoots Pol(C_CC - C_SS, C_SC, C_C + C_C, C_S + C_S, C_1 + C_SS, 0., PIpPI);
  if (!Pol.IsDone())
  {
    done = !done;
    return;
  }
  //
  nbsol = Pol.NbSolutions();
  MyTrigonometricFunction MTF(C_CC, C_SS, C_SC, C_C, C_S, C_1);
  //
  if (Pol.InfiniteRoots())
  {
    //                             2
    //         f(z,t)= (z(t)-zo(t)) = 0       Discriminant(t)=0 pour tout t
    //
    TheCurve[0].SetConeQuadValues(Cone,
                                  Qxx,
                                  Qyy,
                                  Qzz,
                                  Qxy,
                                  Qxz,
                                  Qyz,
                                  Qx,
                                  Qy,
                                  Qz,
                                  Q1,
                                  myEpsilon,
                                  0.,
                                  PIpPI,
                                  UN_SEUL_Z_PAR_THETA,
                                  Z_POSITIF);
    TheCurve[1].SetConeQuadValues(Cone,
                                  Qxx,
                                  Qyy,
                                  Qzz,
                                  Qxy,
                                  Qxz,
                                  Qyz,
                                  Qx,
                                  Qy,
                                  Qz,
                                  Q1,
                                  myEpsilon,
                                  0.,
                                  PIpPI,
                                  UN_SEUL_Z_PAR_THETA,
                                  Z_NEGATIF);
    NbCurves = 2;
    return;
  }
  // else {//#4
  //                      2
  //         f(z,t)=A(t)*z + B(t)*z + C(t)      Discriminant(t) != 0
  //
  if (!nbsol && (MTF.Value(M_PI) < 0.))
  {
    //-- Discriminant signe constant negatif
    return;
  }
  // else {//# 3
  //
  //-- On Traite le cas : Discriminant(t) > 0 pour tout t en simulant 1
  //    racine double en 0
  bool DiscriminantConstantPositif = false;
  if (!nbsol)
  {
    nbsol                       = 1;
    DiscriminantConstantPositif = true;
  }
  //----------------------------------------------------------------------
  //-- Le discriminant admet au moins une racine ( -> Point de Tangence )
  //-- ou est constant positif.
  //----------------------------------------------------------------------
  for (i = 1; i <= nbsol; ++i)
  {
    if (DiscriminantConstantPositif)
    {
      Theta1 = 0.;
      Theta2 = PIpPI - myEpsilon;
    }
    else
    {
      Theta1 = Pol.Value(i);
      Theta2 = (i < nbsol) ? Pol.Value(i + 1) : (Pol.Value(1) + PIpPI);
    }
    //
    if (std::abs(Theta2 - Theta1) <= myEpsilon)
    {
      done = false;
      return; // !!! pkv
    }
    // else {// #2
    double qwet = MTF.Value(0.5 * (Theta1 + Theta2)) + MTF.Value(0.4 * Theta1 + 0.6 * Theta2)
                         + MTF.Value(0.6 * Theta1 + 0.4 * Theta2);
    if (qwet < 0.)
    {
      continue;
    }
    //---------------------------------------------------------------------
    //-- On a des Solutions entre Theta1 et Theta 2
    //---------------------------------------------------------------------

    //---------------------------------------------------------------------
    //-- On Subdivise si necessaire Theta1-->Theta2
    //-- en Theta1-->t1   t1--->t2   ...  tn--->Theta2
    //--  ou t1,t2,...tn sont des racines de A(t)
    //--
    //-- Seule la courbe Z_NEGATIF est affectee
    //----------------------------------------------------------------------
    bool RacinesdePolZ2DansTheta1Theta2;
    int i2;
    double    r;
    //
    // nbsolZ2=PolZ2.NbSolutions();
    RacinesdePolZ2DansTheta1Theta2 = false;
    for (i2 = 1; i2 <= nbsolZ2 && !RacinesdePolZ2DansTheta1Theta2; ++i2)
    {
      r = PolZ2.Value(i2);
      if (r > Theta1 && r < Theta2)
      {
        RacinesdePolZ2DansTheta1Theta2 = true;
      }
      else
      {
        r += PIpPI;
        if (r > Theta1 && r < Theta2)
        {
          RacinesdePolZ2DansTheta1Theta2 = true;
        }
      }
    }
    //
    if (!RacinesdePolZ2DansTheta1Theta2)
    {
      //--------------------------------------------------------------------
      //-- Pas de Branches Infinies
      TheCurve[NbCurves].SetConeQuadValues(Cone,
                                           Qxx,
                                           Qyy,
                                           Qzz,
                                           Qxy,
                                           Qxz,
                                           Qyz,
                                           Qx,
                                           Qy,
                                           Qz,
                                           Q1,
                                           myEpsilon,
                                           Theta1,
                                           Theta2,
                                           UN_SEUL_Z_PAR_THETA,
                                           Z_POSITIF);
      NbCurves++;
      TheCurve[NbCurves].SetConeQuadValues(Cone,
                                           Qxx,
                                           Qyy,
                                           Qzz,
                                           Qxy,
                                           Qxz,
                                           Qyz,
                                           Qx,
                                           Qy,
                                           Qz,
                                           Q1,
                                           myEpsilon,
                                           Theta1,
                                           Theta2,
                                           UN_SEUL_Z_PAR_THETA,
                                           Z_NEGATIF);
      NbCurves++;
    }

    else
    { // #1
      bool NoChanges;
      double    NewMin, NewMax, to;
      //
      NewMin    = Theta1;
      NewMax    = Theta2;
      NoChanges = true;
      //
      for (i2 = 1; i2 <= (nbsolZ2 + nbsolZ2); ++i2)
      {
        if (i2 > nbsolZ2)
        {
          to = PolZ2.Value(i2 - nbsolZ2) + PIpPI;
        }
        else
        {
          to = PolZ2.Value(i2);
        }
        //
        // to is value of the parameter t where A(t)=0, i.e.
        // f(z,to)=B(to)*z + C(to)=0, B(to)!=0.
        //
        // z=-C(to)/B(to) is one and only
        // solution in spite of the fact that D(t)>0 for any value of t.
        //
        if (to < NewMax && to > NewMin)
        {
          //-----------------------------------------------------------------
          //-- On coupe au moins une fois le domaine Theta1 Theta2
          //-----------------------------------------------------------------
          NoChanges = false;
          TheCurve[NbCurves].SetConeQuadValues(Cone,
                                               Qxx,
                                               Qyy,
                                               Qzz,
                                               Qxy,
                                               Qxz,
                                               Qyz,
                                               Qx,
                                               Qy,
                                               Qz,
                                               Q1,
                                               myEpsilon,
                                               NewMin,
                                               to,
                                               UN_SEUL_Z_PAR_THETA,
                                               Z_NEGATIF);
          //
          NbCurves++;
          TheCurve[NbCurves].SetConeQuadValues(Cone,
                                               Qxx,
                                               Qyy,
                                               Qzz,
                                               Qxy,
                                               Qxz,
                                               Qyz,
                                               Qx,
                                               Qy,
                                               Qz,
                                               Q1,
                                               myEpsilon,
                                               NewMin,
                                               to,
                                               UN_SEUL_Z_PAR_THETA,
                                               Z_POSITIF);
          //------------------------------------------------------------
          //-- A == 0
          //-- Si B < 0    Alors Infini sur   Z+
          //-- Sinon             Infini sur   Z-
          //------------------------------------------------------------
          if (PolZ2.IsARoot(NewMin))
          {
            if (MTFZ1.Value(NewMin) < 0.)
            {
              TheCurve[NbCurves].SetIsFirstOpen(true);
            }
            else
            {
              TheCurve[NbCurves - 1].SetIsFirstOpen(true);
            }
          }
          if (MTFZ1.Value(to) < 0.)
          {
            TheCurve[NbCurves].SetIsLastOpen(true);
          }
          else
          {
            TheCurve[NbCurves - 1].SetIsLastOpen(true);
          }
          //------------------------------------------------------------
          NbCurves++;
          NewMin = to;
        } // if(to<NewMax && to>NewMin)
      } // for(i2=1; i2<= (nbsolZ2+nbsolZ2) ; ++i2)
      //
      if (NoChanges)
      {
        TheCurve[NbCurves].SetConeQuadValues(Cone,
                                             Qxx,
                                             Qyy,
                                             Qzz,
                                             Qxy,
                                             Qxz,
                                             Qyz,
                                             Qx,
                                             Qy,
                                             Qz,
                                             Q1,
                                             myEpsilon,
                                             Theta1,
                                             Theta2,
                                             UN_SEUL_Z_PAR_THETA,
                                             Z_NEGATIF);
        NbCurves++;
        TheCurve[NbCurves].SetConeQuadValues(Cone,
                                             Qxx,
                                             Qyy,
                                             Qzz,
                                             Qxy,
                                             Qxz,
                                             Qyz,
                                             Qx,
                                             Qy,
                                             Qz,
                                             Q1,
                                             myEpsilon,
                                             Theta1,
                                             Theta2,
                                             UN_SEUL_Z_PAR_THETA,
                                             Z_POSITIF);
        //------------------------------------------------------------
        //-- A == 0
        //-- Si B < 0    Alors Infini sur   Z+
        //-- Sinon             Infini sur   Z-
        //------------------------------------------------------------
        if (PolZ2.IsARoot(Theta1))
        {
          if (MTFZ1.Value(Theta1) < 0.)
          {
            TheCurve[NbCurves].SetIsFirstOpen(true);
          }
          else
          {
            TheCurve[NbCurves - 1].SetIsFirstOpen(true);
          }
        }
        if (PolZ2.IsARoot(Theta2))
        {
          if (MTFZ1.Value(Theta2) < 0.)
          {
            TheCurve[NbCurves].SetIsLastOpen(true);
          }
          else
          {
            TheCurve[NbCurves - 1].SetIsLastOpen(true);
          }
        }
        //------------------------------------------------------------
        NbCurves++;
      } // if(NoChanges) {
      else
      { // #0
        TheCurve[NbCurves].SetConeQuadValues(Cone,
                                             Qxx,
                                             Qyy,
                                             Qzz,
                                             Qxy,
                                             Qxz,
                                             Qyz,
                                             Qx,
                                             Qy,
                                             Qz,
                                             Q1,
                                             myEpsilon,
                                             NewMin,
                                             Theta2,
                                             UN_SEUL_Z_PAR_THETA,
                                             Z_NEGATIF);
        NbCurves++;
        TheCurve[NbCurves].SetConeQuadValues(Cone,
                                             Qxx,
                                             Qyy,
                                             Qzz,
                                             Qxy,
                                             Qxz,
                                             Qyz,
                                             Qx,
                                             Qy,
                                             Qz,
                                             Q1,
                                             myEpsilon,
                                             NewMin,
                                             Theta2,
                                             UN_SEUL_Z_PAR_THETA,
                                             Z_POSITIF);
        //------------------------------------------------------------
        //-- A == 0
        //-- Si B < 0    Alors Infini sur   Z+
        //-- Sinon             Infini sur   Z-
        //------------------------------------------------------------
        if (PolZ2.IsARoot(NewMin))
        {
          if (MTFZ1.Value(NewMin) < 0.)
          {
            TheCurve[NbCurves].SetIsFirstOpen(true);
          }
          else
          {
            TheCurve[NbCurves - 1].SetIsFirstOpen(true);
          }
        }
        if (PolZ2.IsARoot(Theta2))
        {
          if (MTFZ1.Value(Theta2) < 0.)
          {
            TheCurve[NbCurves].SetIsLastOpen(true);
          }
          else
          {
            TheCurve[NbCurves - 1].SetIsLastOpen(true);
          }
        }
        //------------------------------------------------------------

        NbCurves++;
      } // else {// #0
    } // else { //#1
  } // for(i=1; i<=nbsol ; i++) {
  //}//else { //#5
  InternalSetNextAndPrevious();
}

//=======================================================================
// function :InternalSetNextAndPrevious
// purpose  :
//-- Raccordement des courbes bout a bout
//--    - Utilisation du champ : IsFirstOpen
//--    -                        IsLastOpen
//-- Pas de verification si ces champs retournent Vrai.
//--
//--
//-- 1: Test de      Fin(C1)    = Debut(C2)   ->N(C1) et P(C2)
//-- 2:              Debut(C1)  = Fin(C2)     ->P(C1) et N(C2)
//=======================================================================
void IntAna_IntQuadQuad::InternalSetNextAndPrevious()
{
  bool NotLastOpenC2, NotFirstOpenC2;
  int c1, c2;
  double    aEps, aEPSILON_DISTANCE;
  double    DInfC1, DSupC1, DInfC2, DSupC2;
  //
  aEps              = 0.0000001;
  aEPSILON_DISTANCE = 0.0000000001;
  //
  for (c1 = 0; c1 < NbCurves; c1++)
  {
    nextcurve[c1]     = 0;
    previouscurve[c1] = 0;
  }
  //
  for (c1 = 0; c1 < NbCurves; c1++)
  {
    TheCurve[c1].Domain(DInfC1, DSupC1);

    for (c2 = 0; (c2 < NbCurves) && (c2 != c1); c2++)
    {

      NotLastOpenC2  = !TheCurve[c2].IsLastOpen();
      NotFirstOpenC2 = !TheCurve[c2].IsFirstOpen();
      TheCurve[c2].Domain(DInfC2, DSupC2);
      if (TheCurve[c1].IsFirstOpen() == false)
      {
        if (NotLastOpenC2)
        {
          if (std::abs(DInfC1 - DSupC2) <= aEps
              && (TheCurve[c1].Value(DInfC1).Distance(TheCurve[c2].Value(DSupC2))
                  < aEPSILON_DISTANCE))
          {
            previouscurve[c1] = c2 + 1;
            nextcurve[c2]     = c1 + 1;
          }
        }
        if (NotFirstOpenC2)
        {
          if (std::abs(DInfC1 - DInfC2) <= aEps
              && (TheCurve[c1].Value(DInfC1).Distance(TheCurve[c2].Value(DInfC2))
                  < aEPSILON_DISTANCE))
          {
            previouscurve[c1] = -(c2 + 1);
            previouscurve[c2] = -(c1 + 1);
          }
        }
      }
      if (TheCurve[c1].IsLastOpen() == false)
      {
        if (NotLastOpenC2)
        {
          if (std::abs(DSupC1 - DSupC2) <= aEps
              && (TheCurve[c1].Value(DSupC1).Distance(TheCurve[c2].Value(DSupC2))
                  < aEPSILON_DISTANCE))
          {

            nextcurve[c1] = -(c2 + 1);
            nextcurve[c2] = -(c1 + 1);
          }
        }
        if (NotFirstOpenC2)
        {
          if (std::abs(DSupC1 - DInfC2) <= aEps
              && (TheCurve[c1].Value(DSupC1).Distance(TheCurve[c2].Value(DInfC2))
                  < aEPSILON_DISTANCE))
          {
            nextcurve[c1]     = c2 + 1;
            previouscurve[c2] = c1 + 1;
          }
        }
      }
    }
  }
}

//=================================================================================================

bool IntAna_IntQuadQuad::HasPreviousCurve(const int I) const
{
  if (!done)
  {
    throw StdFail_NotDone("IntQuadQuad Not done");
  }
  if (identical)
  {
    throw Standard_DomainError("IntQuadQuad identical");
  }
  if ((I > NbCurves) || (I <= 0))
  {
    throw Standard_OutOfRange("Incorrect Curve Number 'HasPrevious Curve'");
  }
  if (previouscurve[I - 1])
  {
    return true;
  }
  return false;
}

//=================================================================================================

bool IntAna_IntQuadQuad::HasNextCurve(const int I) const
{
  if (!done)
  {
    throw StdFail_NotDone("IntQuadQuad Not done");
  }
  if (identical)
  {
    throw Standard_DomainError("IntQuadQuad identical");
  }
  if ((I > NbCurves) || (I <= 0))
  {
    throw Standard_OutOfRange("Incorrect Curve Number 'HasNextCurve'");
  }
  if (nextcurve[I - 1])
  {
    return true;
  }
  return (false);
}

//=================================================================================================

int IntAna_IntQuadQuad::PreviousCurve(const int I,
                                                   bool&      theOpposite) const
{
  if (HasPreviousCurve(I))
  {
    if (previouscurve[I - 1] > 0)
    {
      theOpposite = false;
      return (previouscurve[I - 1]);
    }
    else
    {
      theOpposite = true;
      return (-previouscurve[I - 1]);
    }
  }
  else
  {
    throw Standard_DomainError("Incorrect Curve Number 'PreviousCurve'");
  }
}

//=================================================================================================

int IntAna_IntQuadQuad::NextCurve(const int I,
                                               bool&      theOpposite) const
{
  if (HasNextCurve(I))
  {
    theOpposite = nextcurve[I - 1] < 0;
    return std::abs(nextcurve[I - 1]);
  }
  else
  {
    throw Standard_DomainError("Incorrect Curve Number 'NextCurve'");
  }
}

//=================================================================================================

const IntAna_Curve& IntAna_IntQuadQuad::Curve(const int i) const
{
  if (!done)
  {
    throw StdFail_NotDone("IntQuadQuad Not done");
  }
  if (identical)
  {
    throw Standard_DomainError("IntQuadQuad identical");
  }
  if ((i <= 0) || (i > NbCurves))
  {
    throw Standard_OutOfRange("Incorrect Curve Number");
  }
  return TheCurve[i - 1];
}

//=================================================================================================

const gp_Pnt& IntAna_IntQuadQuad::Point(const int i) const
{
  if (!done)
  {
    throw StdFail_NotDone("IntQuadQuad Not done");
  }
  if (identical)
  {
    throw Standard_DomainError("IntQuadQuad identical");
  }
  if ((i <= 0) || (i > Nbpoints))
  {
    throw Standard_OutOfRange("Incorrect Point Number");
  }
  return Thepoints[i - 1];
}

//=================================================================================================

void IntAna_IntQuadQuad::Parameters(const int, // i,
                                    double&,
                                    double&) const
{
  std::cout << "IntAna_IntQuadQuad::Parameters(...) is not yet implemented" << std::endl;
}

/*********************************************************************************

Mathematica Pour Cone Quadrique
In[6]:= y[r_,t_]:=r*Sin[t]

In[7]:= x[r_,t_]:=r*Cos[t]

In[8]:= z[r_,t_]:=r*d

In[14]:= Quad[x_,y_,z_]:=Qxx x x + Qyy y y + Qzz z z + 2 (Qxy x y + Qxz x z + Qyz y z + Qx x + Qy y
+ Qz z ) + Q1

In[15]:= Quad[x[r,t],y[r,t],z[r,t]]

               2      2        2       2        2       2
Out[15]= Q1 + d  Qzz r  + Qxx r  Cos[t]  + Qyy r  Sin[t]  +

                                      2
>    2 (d Qz r + Qx r Cos[t] + d Qxz r  Cos[t] + Qy r Sin[t] +

               2               2
>       d Qyz r  Sin[t] + Qxy r  Cos[t] Sin[t])

In[16]:= QQ=%

In[17]:= Collect[QQ,r]
Collect[QQ,r]

Out[17]= Q1 + r (2 d Qz + 2 Qx Cos[t] + 2 Qy Sin[t]) +

      2   2                                  2
>    r  (d  Qzz + 2 d Qxz Cos[t] + Qxx Cos[t]  + 2 d Qyz Sin[t] +

                                        2
>       2 Qxy Cos[t] Sin[t] + Qyy Sin[t] )
********************************************************************************/

//**********************************************************************
//***                   C O N E   - Q U A D R I Q U E                ***
//***   2    2                                  2                    ***
//***  R  ( d  Qzz + 2 d Qxz Cos[t] + Qxx Cos[t]  + 2 d Qyz Sin[t] + ***
//***                                                                ***
//***       2 Qxy Cos[t] Sin[t] + Qyy Sin[t] )                       ***
//***                                                                ***
//*** + R  ( 2 d Qz + 2 Qx Cos[t] + 2 Qy Sin[t] )                    ***
//***                                                                ***
//*** + Q1                                                           ***
//**********************************************************************
// FortranForm= ( DIS = QQ1 QQ1 - 4 QQ0 QQ2  ) / 4
//   -  d**2*Qz**2 - d**2*Qzz*Q1 + (Qx**2 - Qxx*Q1)*std::cos(t)**2 +
//   -   (2*d*Qy*Qz - 2*d*Qyz*Q1)*Sin(t) + (Qy**2 - Qyy*Q1)*Sin(t)**2 +
//   -   std::cos(t)*(2*d*Qx*Qz - 2*d*Qxz*Q1 + (2*Qx*Qy - 2*Qxy*Q1)*Sin(t))
//**********************************************************************
// modified by NIZNHY-PKV Fri Dec  2 10:56:03 2005f
/*
static
  void DumpCurve(const int aIndex,
         IntAna_Curve& aC);
//=================================================================================================

void DumpCurve(const int aIndex,
           IntAna_Curve& aC)
{
  bool bIsOpen, bIsConstant, bIsFirstOpen, bIsLastOpen;
  int i, aNb;
  double aT1, aT2, aT, dT;
  gp_Pnt aP;
  //
  aC.Domain(aT1, aT2);
  bIsOpen=aC.IsOpen();
  bIsConstant=aC.IsConstant();
  bIsFirstOpen=aC.IsFirstOpen();
  bIsLastOpen=aC.IsLastOpen();
  //
  printf("\n");
  printf(" * IntAna_Curve #%d*\n", aIndex);
  printf(" Domain: [ %lf, %lf ]\n", aT1, aT2);
  printf(" IsOpen=%d\n", bIsOpen);
  printf(" IsConstant=%d\n", bIsConstant);
  printf(" IsFirstOpen =%d\n", bIsFirstOpen);
  printf(" IsLastOpen =%d\n", bIsLastOpen);
  //
  aNb=11;
  dT=(aT2-aT1)/(aNb-1);
  for (i=0; i<aNb; ++i) {
    aT=aT1+i*dT;
    if (i==(aNb-1)){
      aT=aT2;
    }
    //
    aP=aC.Value(aT);
    printf("point p%d_%d %lf %lf %lf\n",
       aIndex, i, aP.X(), aP.Y(), aP.Z());
  }
  printf(" ---- end of curve ----\n");
}
*/
// modified by NIZNHY-PKV Fri Dec  2 10:42:16 2005t
