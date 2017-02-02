// Copyright (c) 1997-1999 Matra Datavision
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

// pmn 15/05/97 pas de Gauss avec des pivot trop petit. SVD fait mieux 
// l'affaire + limitation de la longeur du pas + qq comentaire issus d'EUCLID3
// pmn 10/06/97 refonte totale du traitement des bords + ajustement des init 
// et des tolerances pour brent...

//#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

//#endif
//math_FunctionSetRoot.cxx

#include <math_BrentMinimum.hxx>
#include <math_Function.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Gauss.hxx>
#include <math_GaussLeastSquare.hxx>
#include <math_IntegerVector.hxx>
#include <math_Matrix.hxx>
#include <math_SVD.hxx>
#include <Precision.hxx>
#include <Standard_DimensionError.hxx>
#include <StdFail_NotDone.hxx>

//===========================================================================
// - A partir d une solution de depart, recherche d une direction.( Newton la 
// plupart du temps, gradient si Newton echoue.
// - Recadrage au niveau des bornes avec recalcul de la direction si une
// inconnue a une valeur imposee.
// -Si On ne sort pas des bornes
//   Tant que (On ne progresse pas assez ou on ne change pas de direction) 
//    . Si (Progression encore possible) 
//        Si (On ne sort pas des bornes) 
//          On essaye de progresser dans cette meme direction.
//        Sinon
//          On diminue le pas d'avancement ou on change de direction.
//      Sinon 
//        Si on depasse le minimum
//          On fait une interpolation parabolique.
// - Si on a progresse sur F
//     On fait les tests d'arret
//   Sinon
//     On change de direction
//============================================================================
#define FSR_DEBUG(arg)
// Uncomment the following code to have debug output to cout 
//========================================================== 
//static Standard_Boolean mydebug = Standard_True;
//#undef FSR_DEBUG
//#define FSR_DEBUG(arg) {if (mydebug) { cout << arg << endl; }}
//===========================================================

class MyDirFunction : public math_Function
{

  math_Vector *P0;
  math_Vector *Dir;
  math_Vector *P;
  math_Vector *FV;
  math_FunctionSetWithDerivatives *F;

public :

  MyDirFunction(math_Vector& V1, 
    math_Vector& V2,
    math_Vector& V3,
    math_Vector& V4,
    math_FunctionSetWithDerivatives& f) ;

  void Initialize(const math_Vector& p0, const math_Vector& dir) const;
  //For hp :
  Standard_Boolean Value(const math_Vector& Sol, math_Vector& FF,
    math_Matrix& DF, math_Vector& GH, 
    Standard_Real& F2, Standard_Real& Gnr1);
  //     Standard_Boolean MyDirFunction::Value(const math_Vector& Sol, math_Vector& FF,
  //					   math_Matrix& DF, math_Vector& GH, 
  //					   Standard_Real& F2, Standard_Real& Gnr1);
  Standard_Boolean Value(const Standard_Real x, Standard_Real& fval) ;

};

MyDirFunction::MyDirFunction(math_Vector& V1, 
                             math_Vector& V2,
                             math_Vector& V3,
                             math_Vector& V4,
                             math_FunctionSetWithDerivatives& f) {

                               P0  = &V1;
                               Dir = &V2;
                               P   = &V3;
                               FV =  &V4;
                               F   = &f;
}

void MyDirFunction::Initialize(const math_Vector& p0, 
                               const math_Vector& dir)  const
{
  *P0 = p0;
  *Dir = dir;
}


Standard_Boolean MyDirFunction::Value(const Standard_Real x, 
                                      Standard_Real& fval) 
{
  Standard_Real p;
  for(Standard_Integer i = P->Lower(); i <= P->Upper(); i++) {
    p = Dir->Value(i);
    P->Value(i) = p * x + P0->Value(i);
  }
  if( F->Value(*P, *FV) )
  {

    Standard_Real aVal = 0.0;

    for(Standard_Integer i = FV->Lower(); i <= FV->Upper(); i++)
    {
      aVal = FV->Value(i);
      if(aVal <= -1.e+100) // Precision::HalfInfinite() later
        return Standard_False;
      else if(aVal >= 1.e+100) // Precision::HalfInfinite() later
        return Standard_False;
    }

    fval = 0.5 * (FV->Norm2());
    return Standard_True;
  }
  return Standard_False;
}

Standard_Boolean  MyDirFunction::Value(const math_Vector& Sol,
                                       math_Vector& FF,
                                       math_Matrix& DF,
                                       math_Vector& GH,
                                       Standard_Real& F2,
                                       Standard_Real& Gnr1)
{
  if( F->Values(Sol, FF, DF) ) {

    Standard_Real aVal = 0.;

    for(Standard_Integer i = FF.Lower(); i <= FF.Upper(); i++) {
      // modified by NIZHNY-MKK  Mon Oct  3 17:56:50 2005.BEGIN
      aVal = FF.Value(i);
      if(aVal < 0.) {
        if(aVal <= -1.e+100) // Precision::HalfInfinite() later
          //       if(Precision::IsInfinite(Abs(FF.Value(i)))) {
          //	F2 = Precision::Infinite();
          //	Gnr1 = Precision::Infinite();
          return Standard_False;
      }
      else if(aVal >= 1.e+100) // Precision::HalfInfinite() later
        return Standard_False;
      // modified by NIZHNY-MKK  Mon Oct  3 17:57:05 2005.END
    }


    F2 = 0.5 * (FF.Norm2());
    GH.TMultiply(DF, FF);
    for(Standard_Integer i = GH.Lower(); i <= GH.Upper(); i++) 
    {
      if(Precision::IsInfinite((GH.Value(i))))
      {
        return Standard_False;
      }
    }
    Gnr1 = GH.Norm2();
    return Standard_True;
  }
  return Standard_False;
}


//--------------------------------------------------------------
static Standard_Boolean MinimizeDirection(const math_Vector&   P0,
                                          const math_Vector&   P1,
                                          const math_Vector&   P2,
                                          const Standard_Real  F1,
                                          math_Vector&         Delta,
                                          const math_Vector&   Tol,
                                          MyDirFunction& F)
                                          // Purpose : minimisation a partir de 3 points
                                          //-------------------------------------------------------
{
  // (1) Evaluation d'un tolerance parametrique 1D
  Standard_Real tol1d = 2.1 , invnorme, tsol;
  Standard_Real Eps = 1.e-16;
  Standard_Real ax, bx, cx;

  for (Standard_Integer ii =1; ii<=Tol.Length(); ii++) {
    invnorme = Abs(Delta(ii));
    if  (invnorme>Eps) tol1d = Min(tol1d, Tol(ii) / invnorme);
  }  
  if (tol1d > 1.9) return Standard_False; //Pas la peine de se fatiguer
  tol1d /= 3; 

  //JR/Hp :
  math_Vector PP0 = P0 ;
  math_Vector PP1 = P1 ;
  Delta = PP1 - PP0;
  //  Delta = P1 - P0;
  invnorme = Delta.Norm();
  if (invnorme <= Eps) return Standard_False;
  invnorme = ((Standard_Real) 1) / invnorme;

  F.Initialize(P1, Delta);

  // (2) On minimise
  FSR_DEBUG ("      minimisation dans la direction")
    ax = -1; bx = 0;
  cx = (P2-P1).Norm()*invnorme;
  if (cx < 1.e-2)
    return Standard_False;

  math_BrentMinimum Sol(tol1d, 100, tol1d);
  Sol.Perform(F, ax, bx, cx);

  if(Sol.IsDone()) {
    tsol = Sol.Location();
    if (Sol.Minimum() < F1) {
      Delta.Multiply(tsol);
      return Standard_True;
    }
  }
  return Standard_False;
}

//----------------------------------------------------------------------
static Standard_Boolean MinimizeDirection(const math_Vector&   P,
                                          math_Vector&   Dir,
                                          const Standard_Real& PValue,
                                          const Standard_Real& PDirValue,
                                          const math_Vector&   Gradient,
                                          const math_Vector&   DGradient,
                                          const math_Vector&   Tol,
                                          MyDirFunction& F)
                                          // Purpose: minimisation a partir de 2 points et une derives
                                          //----------------------------------------------------------------------

{
  if(Precision::IsInfinite(PValue) || Precision::IsInfinite(PDirValue))
  {
    return Standard_False;
  }
  // (0) Evaluation d'un tolerance parametrique 1D
  Standard_Boolean good = Standard_False;
  Standard_Real Eps = 1.e-20;
  Standard_Real tol1d = 1.1, Result = PValue, absdir;

  for (Standard_Integer ii =1; ii<=Tol.Length(); ii++) {
    absdir = Abs(Dir(ii));
    if (absdir >Eps) tol1d = Min(tol1d, Tol(ii) / absdir);
  }
  if (tol1d > 0.9) return Standard_False;

  // (1) On realise une premiere interpolation quadratique
  Standard_Real ax, bx, cx, df1, df2, Delta, tsol, fsol, tsolbis;
  FSR_DEBUG("     essai d interpolation");

  df1 = Gradient*Dir;
  df2 = DGradient*Dir;

  if (df1<-Eps && df2>Eps) { // cuvette
    tsol = - df1 / (df2 - df1);
  }
  else {
    cx = PValue;
    bx = df1;
    ax = PDirValue - (bx+cx);

    if (Abs(ax) <= Eps) { // cas lineaire
      if ((Abs(bx) >= Eps)) tsol = - cx/bx;
      else                  tsol = 0;
    }
    else { // cas quadratique
      Delta = bx*bx - 4*ax*cx;
      if (Delta > 1.e-9) {
        // il y a des racines, on prend la plus proche de 0
        Delta = Sqrt(Delta);
        tsol = -(bx + Delta);
        tsolbis = (Delta - bx);
        if (Abs(tsolbis) < Abs(tsol)) tsol = tsolbis;
        tsol /= 2*ax;
      }
      else {
        // pas ou peu de racine : on "extremise"
        tsol = -(0.5*bx)/ax;
      }
    }
  }

  if (Abs(tsol) >= 1) return Standard_False; //resultat sans interet

  F.Initialize(P, Dir);
  F.Value(tsol, fsol);

  if (fsol<PValue) { 
    good = Standard_True;
    Result = fsol;
    FSR_DEBUG("t= "<<tsol<<" F = " << fsol << " OldF = "<<PValue);
  }

  // (2) Si l'on a pas assez progresser on realise une recherche 
  //     en bonne et due forme, a partir des inits precedents
  if ((fsol > 0.2*PValue) && (tol1d < 0.5)) 
  {

    if (tsol <0) {
      ax = tsol; bx = 0.0; cx = 1.0;
    }
    else {
      ax = 0.0; bx = tsol; cx = 1.0;
    }
    FSR_DEBUG(" minimisation dans la direction");

    math_BrentMinimum Sol(tol1d, 100, tol1d);

    // Base invocation.
    Sol.Perform(F, ax, bx, cx);
    if(Sol.IsDone())
    {
      if (Sol.Minimum() <= Result)
      {
        tsol = Sol.Location();
        good = Standard_True;
        Result = Sol.Minimum();

        // Objective function changes too fast ->
        // perform additional computations.
        if (Gradient.Norm2() > 1.0 / Precision::SquareConfusion() &&
            tsol > ax && 
            tsol < cx) // Solution inside of (ax, cx) interval.
        {
          // First and second part invocation.
          Sol.Perform(F, ax, (ax + tsol) / 2.0, tsol);
          if(Sol.IsDone())
          {
            if (Sol.Minimum() <= Result)
            {
              tsol = Sol.Location();
              good = Standard_True;
              Result = Sol.Minimum();
            }
          }

          Sol.Perform(F, tsol, (cx + tsol) / 2.0, cx);
          if(Sol.IsDone())
          {
            if (Sol.Minimum() <= Result)
            {
              tsol = Sol.Location();
              good = Standard_True;
              Result = Sol.Minimum();
            }
          }
        }
      } // if (Sol.Minimum() <= Result)
    } // if(Sol.IsDone())
  }

  if (good)
  {
    // mise a jour du Delta
    Dir.Multiply(tsol);
  }
  return good;
}

//------------------------------------------------------
static void SearchDirection(const math_Matrix& DF,
                            const math_Vector& GH,
                            const math_Vector& FF,
                            Standard_Boolean ChangeDirection,
                            const math_Vector& InvLengthMax, 
                            math_Vector& Direction,
                            Standard_Real& Dy)

{
  Standard_Integer Ninc = DF.ColNumber(), Neq = DF.RowNumber();
  Standard_Real Eps = 1.e-32;
  if (!ChangeDirection) {
    if (Ninc == Neq) {
      for (Standard_Integer i = FF.Lower(); i <= FF.Upper(); i++) {
        Direction(i) = -FF(i);
      }
      math_Gauss Solut(DF, 1.e-9);
      if (Solut.IsDone()) Solut.Solve(Direction);
      else { // we have to "forget" singular directions.
        FSR_DEBUG(" Matrice singuliere : On prend SVD");
        math_SVD SolvebySVD(DF);
        if (SolvebySVD.IsDone()) SolvebySVD.Solve(-1*FF, Direction);
        else ChangeDirection = Standard_True;
      }
    }
    else if (Ninc > Neq) {
      math_SVD Solut(DF);
      if (Solut.IsDone()) Solut.Solve(-1*FF, Direction);
      else ChangeDirection = Standard_True;
    }
    else if (Ninc < Neq) {         // Calcul par GaussLeastSquare
      math_GaussLeastSquare Solut(DF);    
      if (Solut.IsDone()) Solut.Solve(-1*FF, Direction);
      else ChangeDirection = Standard_True;
    }
  }
  // Il vaut mieux interdire des directions trops longue
  // Afin de blinder les cas trop mal conditionne
  // PMN 12/05/97 Traitement des singularite dans les conges
  // Sur des surfaces periodiques

  Standard_Real ratio = Abs( Direction(Direction.Lower())
    *InvLengthMax(Direction.Lower()) );
  Standard_Integer i;
  for (i = Direction.Lower()+1; i<=Direction.Upper(); i++) {
    ratio = Max(ratio,  Abs( Direction(i)*InvLengthMax(i)) );
  }
  if (ratio > 1) {
    Direction /= ratio;
  }

  Dy = Direction*GH;
  if (Dy >= -Eps) { // newton "ne descend pas" on prend le gradient
    ChangeDirection = Standard_True;
  }
  if (ChangeDirection) { // On va faire un gradient !
    for (i = Direction.Lower(); i <= Direction.Upper(); i++) {
      Direction(i) = - GH(i);
    }
    Dy = - (GH.Norm2());
  }
}


//=====================================================================
static void SearchDirection(const math_Matrix& DF, 
                            const math_Vector& GH,
                            const math_Vector& FF,
                            const math_IntegerVector& Constraints,
                            //			    const math_Vector& X, // Le point d'init
                            const math_Vector& , // Le point d'init
                            Standard_Boolean ChangeDirection,
                            const math_Vector& InvLengthMax,
                            math_Vector& Direction,
                            Standard_Real& Dy)
                            //Purpose : Recherche une direction (et un pas si Newton Fonctionne) le long
                            //          d'une frontiere
                            //=====================================================================
{
  Standard_Integer Ninc = DF.ColNumber(), Neq = DF.RowNumber();
  Standard_Integer i, j, k, Cons = 0;

  // verification sur les bornes imposees:

  for (i = 1; i <= Ninc; i++) {
    if (Constraints(i) != 0) Cons++;
    // sinon le systeme a resoudre ne change pas.
  }

  if (Cons == 0) {
    SearchDirection(DF, GH, FF, ChangeDirection, InvLengthMax, 
      Direction, Dy);
  }
  else if (Cons == Ninc) { // il n'y a plus rien a faire...
    for(i = Direction.Lower(); i <= Direction.Upper(); i++) {
      Direction(i) = 0;
    }
    Dy = 0;
  }
  else { //(1) Cas general : On definit un sous probleme
    math_Matrix DF2(1, Neq, 1, Ninc-Cons);
    math_Vector MyGH(1, Ninc-Cons);
    math_Vector MyDirection(1, Ninc-Cons);
    math_Vector MyInvLengthMax(1, Ninc);

    for (k=1, i = 1; i <= Ninc; i++) {
      if (Constraints(i) == 0) { 
        MyGH(k) = GH(i);
        MyInvLengthMax(k) = InvLengthMax(i);
        MyDirection(k) = Direction(i);
        for (j = 1; j <= Neq; j++) {
          DF2(j, k) = DF(j, i);
        }
        k++; //on passe a l'inconnue suivante
      }
    }
    //(2) On le resoud
    SearchDirection(DF2, MyGH, FF, ChangeDirection, MyInvLengthMax, 
      MyDirection, Dy);

    // (3) On l'interprete...
    // Reconstruction de Direction:
    for (i = 1, k = 1; i <= Ninc; i++) {
      if (Constraints(i) == 0) {
        if (!ChangeDirection) {
          Direction(i) = MyDirection(k);
        }
        else Direction(i) = - GH(i);
        k++;
      }
      else {
        Direction(i) = 0.0;
      }
    }
  }
}



//====================================================
Standard_Boolean Bounds(const math_Vector&  InfBound,
                        const math_Vector&  SupBound,
                        const math_Vector&  Tol,
                        math_Vector&        Sol,
                        const math_Vector&  SolSave,
                        math_IntegerVector& Constraints,
                        math_Vector&        Delta,
                        Standard_Boolean&   theIsNewSol)
                        //
                        // Purpose: Trims an initial solution Sol to be within a domain defined by
                        //   InfBound and SupBound. Delta will contain a distance between final Sol and
                        //   SolSave.
                        //   IsNewSol returns False, if final Sol fully coincides with SolSave, i.e.
                        //   if SolSave already lied on a boundary and initial Sol was fully beyond it
                        //======================================================
{
  Standard_Boolean Out = Standard_False;
  Standard_Integer i, Ninc = Sol.Length();
  Standard_Real    monratio = 1;

  theIsNewSol = Standard_True;

  // Calcul du ratio de recadrage
  for (i = 1; i <= Ninc; i++) {
    Constraints(i) = 0;
    Delta(i) = Sol(i) - SolSave(i);
    if (InfBound(i) == SupBound(i)) {
      Constraints(i) = 1;
      Out = Standard_True; // Ok mais, cela devrait etre eviter
    }
    else if(Sol(i) < InfBound(i)) {
      Constraints(i) = 1;
      Out = Standard_True;
      // Delta(i) is negative
      if (-Delta(i) > Tol(i)) // Afin d'eviter des ratio nulles pour rien
        monratio = Min(monratio, (InfBound(i) - SolSave(i))/Delta(i) );
    }
    else if (Sol(i) > SupBound(i)) {
      Constraints(i) = 1;
      Out = Standard_True;
      // Delta(i) is positive
      if (Delta(i) > Tol(i))
        monratio = Min(monratio, (SupBound(i) - SolSave(i))/Delta(i) );
    }
  }

  if (Out){ // Troncature et derniers recadrage pour blinder (pb numeriques)
    if (monratio == 0.0) {
      theIsNewSol = Standard_False;
      Sol = SolSave;
      Delta.Init (0.0);
    } else {
      Delta *= monratio;
      Sol = SolSave+Delta;
      for (i = 1; i <= Ninc; i++) {
        if(Sol(i) < InfBound(i))  {
          Sol(i) = InfBound(i);
          Delta(i) = Sol(i) - SolSave(i);
        }
        else if (Sol(i) > SupBound(i)) {
          Sol(i) = SupBound(i);
          Delta(i) = Sol(i) - SolSave(i);
        }
      }
    }
  }
  return Out;
}




//=======================================================================
//function : math_FunctionSetRoot
//purpose  : Constructor
//=======================================================================
math_FunctionSetRoot::math_FunctionSetRoot(
  math_FunctionSetWithDerivatives& theFunction,
  const math_Vector&               theTolerance,
  const Standard_Integer           theNbIterations)

: Delta(1, theFunction.NbVariables()),
  Sol  (1, theFunction.NbVariables()),
  DF   (1, theFunction.NbEquations() , 1, theFunction.NbVariables()),
  Tol  (1, theFunction.NbVariables()),
  Done    (Standard_False),
  Kount   (0),
  State   (0),
  Itermax (theNbIterations),
  InfBound(1, theFunction.NbVariables(), RealFirst()),
  SupBound(1, theFunction.NbVariables(), RealLast ()),
  SolSave (1, theFunction.NbVariables()),
  GH      (1, theFunction.NbVariables()),
  DH      (1, theFunction.NbVariables()),
  DHSave  (1, theFunction.NbVariables()),
  FF      (1, theFunction.NbEquations()),
  PreviousSolution(1, theFunction.NbVariables()),
  Save    (0, theNbIterations),
  Constraints(1, theFunction.NbVariables()),
  Temp1   (1, theFunction.NbVariables()),
  Temp2   (1, theFunction.NbVariables()),
  Temp3   (1, theFunction.NbVariables()),
  Temp4   (1, theFunction.NbEquations()),
  myIsDivergent(Standard_False)
{
  SetTolerance(theTolerance);
}

//=======================================================================
//function : math_FunctionSetRoot
//purpose  : Constructor
//=======================================================================
math_FunctionSetRoot::math_FunctionSetRoot(math_FunctionSetWithDerivatives& theFunction,
                                           const Standard_Integer           theNbIterations)

: Delta(1, theFunction.NbVariables()),
  Sol  (1, theFunction.NbVariables()),
  DF   (1, theFunction.NbEquations() , 1, theFunction.NbVariables()),
  Tol  (1, theFunction.NbVariables()),
  Done    (Standard_False),
  Kount   (0),
  State   (0),
  Itermax (theNbIterations),
  InfBound(1, theFunction.NbVariables(), RealFirst()),
  SupBound(1, theFunction.NbVariables(), RealLast ()),
  SolSave (1, theFunction.NbVariables()),
  GH      (1, theFunction.NbVariables()),
  DH      (1, theFunction.NbVariables()),
  DHSave  (1, theFunction.NbVariables()),
  FF      (1, theFunction.NbEquations()),
  PreviousSolution(1, theFunction.NbVariables()),
  Save    (0, theNbIterations),
  Constraints(1, theFunction.NbVariables()),
  Temp1   (1, theFunction.NbVariables()),
  Temp2   (1, theFunction.NbVariables()),
  Temp3   (1, theFunction.NbVariables()),
  Temp4   (1, theFunction.NbEquations()),
  myIsDivergent(Standard_False)
{
}

//=======================================================================
//function : ~math_FunctionSetRoot
//purpose  : Destructor
//=======================================================================
math_FunctionSetRoot::~math_FunctionSetRoot()
{
}

//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================
void math_FunctionSetRoot::SetTolerance(const math_Vector& theTolerance)
{
  for (Standard_Integer i = 1; i <= Tol.Length(); ++i)
    Tol(i) = theTolerance(i);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void math_FunctionSetRoot::Perform(math_FunctionSetWithDerivatives& theFunction,
                                   const math_Vector&               theStartingPoint,
                                   const Standard_Boolean           theStopOnDivergent)
{
  Perform(theFunction, theStartingPoint, InfBound, SupBound, theStopOnDivergent);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void math_FunctionSetRoot::Perform(math_FunctionSetWithDerivatives& F,
                                   const math_Vector& StartingPoint,
                                   const math_Vector& theInfBound,
                                   const math_Vector& theSupBound,
                                   Standard_Boolean theStopOnDivergent)
{
  Standard_Integer Ninc = F.NbVariables(), Neq = F.NbEquations();

  if ((Neq <= 0)                      ||
    (StartingPoint.Length()!= Ninc) ||
    (theInfBound.Length() != Ninc)     ||
    (theSupBound.Length() != Ninc))  { throw Standard_DimensionError(); }

  Standard_Integer i;
  Standard_Boolean ChangeDirection = Standard_False, Sort = Standard_False, isNewSol = Standard_False;
  Standard_Boolean Good, Verif;
  Standard_Boolean Stop;
  const Standard_Real EpsSqrt = 1.e-16, Eps = 1.e-32, Eps2 = 1.e-64, Progres = 0.005;
  Standard_Real F2, PreviousMinimum, Dy, OldF;
  Standard_Real Ambda, Ambda2, Gnr1, Oldgr;
  math_Vector InvLengthMax(1, Ninc); // Pour bloquer les pas a 1/4 du domaine
  math_IntegerVector aConstraints(1, Ninc); // Pour savoir sur quels bord on se trouve
  for (i = 1; i <= Ninc ; i++) {
    const Standard_Real aSupBound  = Min (theSupBound(i),  Precision::Infinite());
    const Standard_Real anInfBound = Max (theInfBound(i), -Precision::Infinite());
    InvLengthMax(i) = 1. / Max((aSupBound - anInfBound)/4, 1.e-9);
  }

  MyDirFunction F_Dir(Temp1, Temp2, Temp3, Temp4, F);
  Standard_Integer  DescenteIter;

  Done = Standard_False;
  Sol = StartingPoint;
  Kount = 0;

  //
  myIsDivergent = Standard_False;
  for (i = 1; i <= Ninc; i++)
  {
    myIsDivergent |= (Sol(i) < theInfBound(i)) | (theSupBound(i) < Sol(i));
  }
  if (theStopOnDivergent & myIsDivergent)
  {
    return;
  }
  // Verification de la validite des inconnues par rapport aux bornes.
  // Recentrage sur les bornes si pas valide.
  for ( i = 1; i <= Ninc; i++) {
    if (Sol(i) <= theInfBound(i)) Sol(i) = theInfBound(i);
    else if (Sol(i) > theSupBound(i)) Sol(i) = theSupBound(i);
  }

  // Calcul de la premiere valeur de F et de son gradient
  if(!F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1)) {
    Done = Standard_False;
    if (!theStopOnDivergent || !myIsDivergent)
    {
      State = F.GetStateNumber();
    }
    return;
  }
  Ambda2 = Gnr1;
  // Le rang 0 de Save ne doit servir q'au test accelarteur en fin de boucle
  // s'il on est dejas sur la solution, il faut leurer ce test pour eviter
  // de faire une seconde iteration...
  Save(0) = Max (F2, EpsSqrt);
  Standard_Real aTol_Func = Epsilon(F2);
  FSR_DEBUG("=== Mode Debug de Function Set Root" << endl);
  FSR_DEBUG("    F2 Initial = " << F2);

  if ((F2 <= Eps) || (Gnr1 <= Eps2)) {
    Done = Standard_False;
    if (!theStopOnDivergent || !myIsDivergent)
    {
      Done = Standard_True;
      State = F.GetStateNumber();
    }
    return;
  }

  for (Kount = 1; Kount <= Itermax; Kount++) {
    PreviousMinimum = F2;
    Oldgr = Gnr1;
    PreviousSolution = Sol;
    SolSave = Sol;

    SearchDirection(DF, GH, FF, ChangeDirection, InvLengthMax, DH, Dy);
    if (Abs(Dy) <= Eps) {
      Done = Standard_False;
      if (!theStopOnDivergent || !myIsDivergent)
      {
        Done = Standard_True;
        ////modified by jgv, 31.08.2011////
        F.Value(Sol, FF); //update F before GetStateNumber
        ///////////////////////////////////
        State = F.GetStateNumber();
      }
      return;
    }
    if (ChangeDirection) {
      Ambda = Ambda2 / Sqrt(Abs(Dy));
      if (Ambda > 1.0) Ambda = 1.0;
    }
    else {
      Ambda = 1.0;
      Ambda2 = 0.5*Ambda/DH.Norm();
    }

    for( i = Sol.Lower(); i <= Sol.Upper(); i++) { 
      Sol(i) = Sol(i) + Ambda * DH(i);
    }
    //
    for (i = 1; i <= Ninc; i++)
    {
      myIsDivergent |= (Sol(i) < theInfBound(i)) | (theSupBound(i) < Sol(i));
    }
    if (theStopOnDivergent & myIsDivergent)
    {
      return;
    }
    //
    Sort = Bounds(theInfBound, theSupBound, Tol, Sol, SolSave,
      aConstraints, Delta, isNewSol);


    DHSave = GH;
    if (isNewSol) {
      //      F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1);
      if(!F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1)) {
        Done = Standard_False;
        if (!theStopOnDivergent || !myIsDivergent)
        {
          State = F.GetStateNumber();
        }
        return;
      }
    }

    FSR_DEBUG("Kount         = " << Kount);
    FSR_DEBUG("Le premier F2 = " << F2);
    FSR_DEBUG("Direction     = " << ChangeDirection);

    if ((F2 <= Eps) || (Gnr1 <= Eps2)) {
      Done = Standard_False;
      if (!theStopOnDivergent || !myIsDivergent)
      {
        Done = Standard_True;
        ////modified by jgv, 31.08.2011////
        F.Value(Sol, FF); //update F before GetStateNumber
        ///////////////////////////////////
        State = F.GetStateNumber();
      }
      return;
    }

    if (Sort || (F2/PreviousMinimum > Progres)) {
      Dy = GH*DH;
      OldF = PreviousMinimum;
      Stop = Standard_False;
      Good = Standard_False;
      DescenteIter = 0;
      Standard_Boolean Sortbis;

      // -------------------------------------------------
      // Traitement standard sans traitement des bords
      // -------------------------------------------------
      if (!Sort) { // si l'on est pas sortie on essaye de progresser en avant
        while((F2/PreviousMinimum > Progres) && !Stop) {
          if (F2 < OldF && (Dy < 0.0)) {
            // On essaye de progresser dans cette direction.
            FSR_DEBUG(" iteration de descente = " << DescenteIter);
            DescenteIter++;
            SolSave = Sol;
            OldF = F2;
            for( i = Sol.Lower(); i <= Sol.Upper(); i++) {
              Sol(i) = Sol(i) + Ambda * DH(i);
            }
            //
            for (i = 1; i <= Ninc; i++)
            {
              myIsDivergent |= (Sol(i) < theInfBound(i)) | (theSupBound(i) < Sol(i));
            }
            if (theStopOnDivergent & myIsDivergent)
            {
              return;
            }
            //
            Stop = Bounds(theInfBound, theSupBound, Tol, Sol, SolSave,
              aConstraints, Delta, isNewSol);
            FSR_DEBUG(" Augmentation de lambda");
            Ambda *= 1.7;
          }
          else {
            if ((F2 >= OldF)||(F2 >= PreviousMinimum)) {
              Good = Standard_False;
              if (DescenteIter == 0) { 
                // C'est le premier pas qui flanche, on fait une interpolation.
                // et on minimise si necessaire.
                DescenteIter++;
                Good = MinimizeDirection(SolSave, Delta, OldF, F2, DHSave, GH,
                  Tol, F_Dir);
              }
              else if (ChangeDirection || (DescenteIter>1) 
                || (OldF>PreviousMinimum) ) {
                  // La progression a ete utile, on minimise...
                  DescenteIter++;
                  Good = MinimizeDirection(PreviousSolution, SolSave, Sol, 
                    OldF, Delta, Tol, F_Dir);
              }
              if (!Good) {
                Sol = SolSave;
                F2 = OldF;
              }
              else {
                Sol = SolSave+Delta;
                //
                for (i = 1; i <= Ninc; i++)
                {
                  myIsDivergent |= (Sol(i) < theInfBound(i)) | (theSupBound(i) < Sol(i));
                }
                if (theStopOnDivergent & myIsDivergent)
                {
                  return;
                }
                //
                Sort = Bounds(theInfBound, theSupBound, Tol, Sol, SolSave,
                  aConstraints, Delta, isNewSol);
              }
              Sort = Standard_False; // On a rejete le point sur la frontiere
            }
            Stop = Standard_True; // et on sort dans tous les cas...
          }
          DHSave = GH;
          if (isNewSol) {
            //            F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1);
            if(!F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1)) {
              Done = Standard_False;
              if (!theStopOnDivergent || !myIsDivergent)
              {
                State = F.GetStateNumber();
              }
              return;
            }
          }
          Dy = GH*DH;
          if (Abs(Dy) <= Eps) {
            if (F2 > OldF)
              Sol = SolSave;
            Done = Standard_False;
            if (!theStopOnDivergent || !myIsDivergent)
            {
              Done = Standard_True;
              ////modified by jgv, 31.08.2011////
              F.Value(Sol, FF); //update F before GetStateNumber
              ///////////////////////////////////
              State = F.GetStateNumber();
            }
            return;
          }
          if (DescenteIter >= 100) {
            Stop = Standard_True;
          }
        }
        FSR_DEBUG("--- Sortie du Traitement Standard");
        FSR_DEBUG("    DescenteIter = "<<DescenteIter << " F2 = " << F2);
      }
      // ------------------------------------
      //  on passe au traitement des bords
      // ------------------------------------
      if (Sort) {
        Stop = (F2 > 1.001*OldF); // Pour ne pas progresser sur le bord
        Sortbis = Sort;
        DescenteIter = 0;
        while (Sortbis && ((F2<OldF)|| (DescenteIter == 0))
          && (!Stop)) {
            DescenteIter++;
            // On essaye de progresser sur le bord
            SolSave = Sol;
            OldF = F2;
            SearchDirection(DF, GH, FF,  aConstraints, Sol,
              ChangeDirection, InvLengthMax, DH, Dy);
            FSR_DEBUG(" Conditional Direction = " << ChangeDirection);
            if (Dy<-Eps) { //Pour eviter des calculs inutiles et des /0...
              if (ChangeDirection) {

                // 	      Ambda = Ambda2 / Sqrt(Abs(Dy));
                Ambda = Ambda2 / Sqrt(-Dy);
                if (Ambda > 1.0) Ambda = 1.0;
              }
              else {
                Ambda = 1.0;
                Ambda2 = 0.5*Ambda/DH.Norm();
              }

              for( i = Sol.Lower(); i <= Sol.Upper(); i++) { 
                Sol(i) = Sol(i) + Ambda * DH(i);
              }
              //
              for (i = 1; i <= Ninc; i++)
              {
                myIsDivergent |= (Sol(i) < theInfBound(i)) | (theSupBound(i) < Sol(i));
              }
              if (theStopOnDivergent & myIsDivergent)
              {
                return;
              }
              //
              Sortbis = Bounds(theInfBound, theSupBound, Tol, Sol, SolSave,
                aConstraints, Delta, isNewSol);

              DHSave = GH;
              if (isNewSol) {
                //              F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1);
                if(!F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1)) {
                  Done = Standard_False;
                  if (!theStopOnDivergent || !myIsDivergent)
                  {
                    State = F.GetStateNumber();
                  }
                  return;
                }
              }
              Ambda2 = Gnr1;
              FSR_DEBUG("---  Iteration au bords : " << DescenteIter);
              FSR_DEBUG("---  F2 = " << F2);
            }
            else {
              Stop = Standard_True;
            }

            while((F2/PreviousMinimum > Progres) && (F2<OldF) && (!Stop) ) {
              DescenteIter++;
              FSR_DEBUG("--- Iteration de descente conditionnel = " << DescenteIter);
              if (F2 < OldF && Dy < 0.0) {
                // On essaye de progresser dans cette direction.
                SolSave = Sol;
                OldF = F2;
                for( i = Sol.Lower(); i <= Sol.Upper(); i++) {
                  Sol(i) = Sol(i) + Ambda * DH(i);
                }
                //
                for (i = 1; i <= Ninc; i++)
                {
                  myIsDivergent |= (Sol(i) < theInfBound(i)) | (theSupBound(i) < Sol(i));
                }
                if (theStopOnDivergent & myIsDivergent)
                {
                  return;
                }
                //
                Sortbis = Bounds(theInfBound, theSupBound, Tol, Sol, SolSave,
                  aConstraints, Delta, isNewSol);
              }
              DHSave = GH;
              if (isNewSol) {
                //              F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1);
                if(!F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1)) {
                  Done = Standard_False;
                  if (!theStopOnDivergent || !myIsDivergent)
                  {
                    State = F.GetStateNumber();
                  }
                  return;
                }
              }
              Ambda2 = Gnr1;
              Dy = GH*DH;
              Stop = ((Dy >=0) || (DescenteIter >= 10) || Sortbis);
            }
            Stop = ((Dy >=0) || (DescenteIter >= 10));
        }
        if (((F2/PreviousMinimum > Progres) &&
          (F2>=OldF))||(F2>=PreviousMinimum)) {
            // On minimise par Brent
            DescenteIter++;
            Good = MinimizeDirection(SolSave, Delta, OldF, F2,  
              DHSave, GH, Tol, F_Dir);
            if (!Good) {
              Sol = SolSave;
              Sort = Standard_False;
            }
            else {
              Sol = SolSave + Delta;
              //
              for (i = 1; i <= Ninc; i++)
              {
                myIsDivergent |= (Sol(i) < theInfBound(i)) | (theSupBound(i) < Sol(i));
              }
              if (theStopOnDivergent & myIsDivergent)
              {
                return;
              }
              //
              Sort = Bounds(theInfBound, theSupBound, Tol, Sol, SolSave,
                aConstraints, Delta, isNewSol);
              if (isNewSol) {
                //              F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1);
                if(!F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1)) {
                  Done = Standard_False;
                  if (!theStopOnDivergent || !myIsDivergent)
                  {
                    State = F.GetStateNumber();
                  }
                  return;
                }
              }
            }
            Dy = GH*DH;
        }	
        FSR_DEBUG("--- Sortie du Traitement des Bords");
        FSR_DEBUG("--- DescenteIter = "<<DescenteIter << " F2 = " << F2);
      }
    }

    // ---------------------------------------------
    //  on passe aux tests d'ARRET
    // ---------------------------------------------
    Save(Kount) = F2; 
    // Est ce la solution ?
    if (ChangeDirection) Verif = Standard_True;
    // Gradient : Il faut eviter de boucler
    else {
      Verif = Standard_False;
      if (Kount > 1) { // Pour accelerer les cas quasi-quadratique
        if (Save(Kount-1)<1.e-4*Save(Kount-2)) Verif = Standard_True;
      }
      else Verif = (F2 < 1.e-6*Save(0)); //Pour les cas dejas solutions
    }
    if (Verif) {
      for(i = Delta.Lower(); i <= Delta.Upper(); i++) {
        Delta(i) = PreviousSolution(i) - Sol(i);
      }

      if (IsSolutionReached(F)) {
        if (PreviousMinimum < F2) {
          Sol = SolSave;
        }
        Done = Standard_False;
        if (!theStopOnDivergent || !myIsDivergent)
        {
          Done = Standard_True;
          ////modified by jgv, 31.08.2011////
          F.Value(Sol, FF); //update F before GetStateNumber
          ///////////////////////////////////
          State = F.GetStateNumber();
        }
        return;
      }
    }
    //fin du test solution

    // Analyse de la progression...
    //comparison of current minimum and previous minimum
    if ((F2 - PreviousMinimum) <= aTol_Func){ 
      if (Kount > 5) {
        // L'historique est il bon ?
        if (F2 >= 0.95*Save(Kount - 5)) {
          if (!ChangeDirection) ChangeDirection = Standard_True;
          else 
          {
            Done = Standard_False;
            if (!theStopOnDivergent || !myIsDivergent)
            {
              Done = Standard_True;
              State = F.GetStateNumber();
            }
            return; //  si un gain inf a 5% on sort
          }
        }
        else ChangeDirection = Standard_False; //Si oui on recommence
      }
      else  ChangeDirection = Standard_False; //Pas d'historique on continue
      // Si le gradient ne diminue pas suffisemment par newton on essaie
      // le gradient sauf si f diminue (aussi bizarre que cela puisse 
      // paraitre avec NEWTON le gradient de f peut augmenter alors que f 
      // diminue: dans ce cas il faut garder NEWTON)
      if ((Gnr1 > 0.9*Oldgr) && 
        (F2 > 0.5*PreviousMinimum)) {
          ChangeDirection = Standard_True;
      }

      // Si l'on ne decide pas de changer de strategie, on verifie,
      // si ce n'est dejas fait     
      if ((!ChangeDirection) && (!Verif)) {
        for(i = Delta.Lower(); i <= Delta.Upper(); i++) {
          Delta(i) = PreviousSolution(i) - Sol(i);
        }
        if (IsSolutionReached(F)) {
          Done = Standard_False;
          if (!theStopOnDivergent || !myIsDivergent)
          {
            Done = Standard_True;
            ////modified by jgv, 31.08.2011////
            F.Value(Sol, FF); //update F before GetStateNumber
            ///////////////////////////////////
            State = F.GetStateNumber();
          }
          return;
        }
      }
    } 
    else { // Cas de regression
      if (!ChangeDirection) { // On passe au gradient
        ChangeDirection = Standard_True;
        Sol = PreviousSolution;
        //	F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1);
        if(!F_Dir.Value(Sol, FF, DF, GH, F2, Gnr1)) {
          Done = Standard_False;
          if (!theStopOnDivergent || !myIsDivergent)
          {
            State = F.GetStateNumber();
          }
          return;
        }
      }
      else 
      {

        if (!theStopOnDivergent || !myIsDivergent)
        {
          State = F.GetStateNumber();
        }
        return; // y a plus d'issues
      }
    }
  }
  if (!theStopOnDivergent || !myIsDivergent)
  {
    State = F.GetStateNumber();
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void math_FunctionSetRoot::Dump(Standard_OStream& o) const
{
  o << " math_FunctionSetRoot";
  if (Done) {
    o << " Status = Done\n";
    o << " Location value = " << Sol << "\n";
    o << " Number of iterations = " << Kount << "\n";
  }
  else {
    o << "Status = Not Done\n";
  }
}

//=======================================================================
//function : Root
//purpose  : 
//=======================================================================
void math_FunctionSetRoot::Root(math_Vector& Root) const
{
  StdFail_NotDone_Raise_if(!Done, " ");
  Standard_DimensionError_Raise_if(Root.Length() != Sol.Length(), " ");
  Root = Sol;
}

//=======================================================================
//function : FunctionSetErrors
//purpose  : 
//=======================================================================
void math_FunctionSetRoot::FunctionSetErrors(math_Vector& Err) const
{
  StdFail_NotDone_Raise_if(!Done, " ");
  Standard_DimensionError_Raise_if(Err.Length() != Sol.Length(), " ");
  Err = Delta;
}
