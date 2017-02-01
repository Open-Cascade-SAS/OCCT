// Created on: 1991-08-09
// Created by: JCV
// Copyright (c) 1991-1999 Matra Datavision
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

// Modified RLE 9 Sep 1993
// pmn : modified 28-01-97  : fixed a mistake in LocateParameter (PRO6973)
// pmn : modified 4-11-96   : fixed a mistake in BuildKnots (PRO6124)
// pmn : modified 28-Jun-96 : fixed a mistake in AntiBoorScheme
// xab : modified 15-Jun-95 : fixed a mistake in IsRational
// xab : modified 15-Mar-95 : removed Epsilon comparison in IsRational
//                            added RationalDerivatives.
// xab : 30-Mar-95 : fixed coupling with lti in RationalDerivatives
// xab : 15-Mar-96 : fixed a typo in Eval with extrapolation
// jct : 15-Apr-97 : added TangExtendToConstraint
// jct : 24-Apr-97 : correction on computation of Tbord and NewFlatKnots
//                   in TangExtendToConstraint; Continuity can be equal to 0

#include <BSplCLib.hxx>
#include <ElCLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <NCollection_LocalArray.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>

typedef gp_Pnt Pnt;
typedef gp_Vec Vec;
typedef TColgp_Array1OfPnt Array1OfPnt;
typedef TColStd_Array1OfReal Array1OfReal;
typedef TColStd_Array1OfInteger Array1OfInteger;

//=======================================================================
//class : BSplCLib_LocalMatrix
//purpose: Auxiliary class optimizing creation of matrix buffer for
//         evaluation of bspline (using stack allocation for main matrix)
//=======================================================================

class BSplCLib_LocalMatrix : public math_Matrix 
{
public:
  BSplCLib_LocalMatrix (Standard_Integer DerivativeRequest, Standard_Integer Order)
    : math_Matrix (myBuffer, 1, DerivativeRequest + 1, 1, Order)
  {
    Standard_OutOfRange_Raise_if (DerivativeRequest > BSplCLib::MaxDegree() ||
        Order > BSplCLib::MaxDegree()+1 || BSplCLib::MaxDegree() > 25,
        "BSplCLib: bspline degree is greater than maximum supported");
  }

 private:
  // local buffer, to be sufficient for addressing by index [Degree+1][Degree+1]
  // (see math_Matrix implementation)
  Standard_Real myBuffer[27*27];
};

//=======================================================================
//function : Hunt
//purpose  : 
//=======================================================================

void BSplCLib::Hunt (const Array1OfReal& XX, 
		     const Standard_Real X,
		     Standard_Integer&   Ilc)
{
  // replaced by simple dichotomy (RLE)
  Ilc = XX.Lower();
  if (XX.Length() <= 1) return;
  const Standard_Real *px = &XX(Ilc);
  px -= Ilc;

  if (X < px[Ilc]) {
    Ilc--;
    return;
  }
  Standard_Integer Ihi = XX.Upper();
  if (X > px[Ihi]) {
    Ilc = Ihi + 1;
    return;
  }
  Standard_Integer Im;

  while (Ihi - Ilc != 1) {
    Im = (Ihi + Ilc) >> 1;
    if (X > px[Im]) Ilc = Im;
    else            Ihi = Im;
  }
}

//=======================================================================
//function : FirstUKnotIndex
//purpose  : 
//=======================================================================

Standard_Integer BSplCLib::FirstUKnotIndex (const Standard_Integer Degree,
				   const TColStd_Array1OfInteger& Mults)
{ 
  Standard_Integer Index = Mults.Lower();
  Standard_Integer SigmaMult = Mults(Index);

  while (SigmaMult <= Degree) {
    Index++;
    SigmaMult += Mults (Index);
  }
  return Index;
}

//=======================================================================
//function : LastUKnotIndex
//purpose  : 
//=======================================================================

Standard_Integer BSplCLib::LastUKnotIndex  (const Standard_Integer Degree,
				   const Array1OfInteger& Mults) 
{ 
   Standard_Integer Index = Mults.Upper();
   Standard_Integer SigmaMult = Mults(Index);

   while (SigmaMult <= Degree) {
     Index--;
     SigmaMult += Mults.Value (Index);
   }
   return Index;
}

//=======================================================================
//function : FlatIndex
//purpose  : 
//=======================================================================

Standard_Integer  BSplCLib::FlatIndex
  (const Standard_Integer Degree,
   const Standard_Integer Index,
   const TColStd_Array1OfInteger& Mults,
   const Standard_Boolean Periodic)
{
  Standard_Integer i, index = Index;
  const Standard_Integer MLower = Mults.Lower();
  const Standard_Integer *pmu = &Mults(MLower);
  pmu -= MLower;

  for (i = MLower + 1; i <= Index; i++)
    index += pmu[i] - 1;
  if ( Periodic)
    index += Degree;
  else
    index += pmu[MLower] - 1;
  return index;
}

//=======================================================================
//function : LocateParameter
//purpose  : Processing of nodes with multiplicities
//pmn  28-01-97 -> compute eventual of the period.
//=======================================================================

void BSplCLib::LocateParameter
(const Standard_Integer          , //Degree,
 const Array1OfReal&    Knots,
 const Array1OfInteger& , //Mults,
 const Standard_Real             U,
 const Standard_Boolean          IsPeriodic,
 const Standard_Integer          FromK1,
 const Standard_Integer          ToK2,
 Standard_Integer&               KnotIndex,
 Standard_Real&                  NewU)
{
  Standard_Real uf = 0, ul=1;
  if (IsPeriodic) {
    uf = Knots(Knots.Lower());
    ul = Knots(Knots.Upper());
  }
  BSplCLib::LocateParameter(Knots,U,IsPeriodic,FromK1,ToK2,
			    KnotIndex,NewU, uf, ul);
}

//=======================================================================
//function : LocateParameter
//purpose  : For plane nodes 
//   pmn  28-01-97 -> There is a need of the degre to calculate
//   the eventual period
//=======================================================================

void BSplCLib::LocateParameter
(const Standard_Integer          Degree,
 const Array1OfReal&    Knots,
 const Standard_Real             U,
 const Standard_Boolean          IsPeriodic,
 const Standard_Integer          FromK1,
 const Standard_Integer          ToK2,
 Standard_Integer&               KnotIndex,
 Standard_Real&                  NewU)
{ 
  if (IsPeriodic)
    BSplCLib::LocateParameter(Knots, U, IsPeriodic, FromK1, ToK2,
			      KnotIndex, NewU,
			      Knots(Knots.Lower() + Degree),
			      Knots(Knots.Upper() - Degree));
  else 
    BSplCLib::LocateParameter(Knots, U, IsPeriodic, FromK1, ToK2,
			      KnotIndex, NewU,
			      0.,
			      1.);
}

//=======================================================================
//function : LocateParameter
//purpose  : Effective computation
// pmn 28-01-97 : Add limits of the period as input argument,  
//                as it is imposible to produce them at this level.
//=======================================================================

void BSplCLib::LocateParameter 
(const TColStd_Array1OfReal& Knots,
 const Standard_Real         U,
 const Standard_Boolean      IsPeriodic,
 const Standard_Integer      FromK1,
 const Standard_Integer      ToK2,
 Standard_Integer&           KnotIndex,
 Standard_Real&              NewU,
 const Standard_Real         UFirst,
 const Standard_Real         ULast)
{
  /*
  Let Knots are distributed as follows (the array is sorted in ascending order):
    
      K1, K1,..., K1, K1, K2, K2,..., K2, K2,..., Kn, Kn,..., Kn
           M1 times             M2 times             Mn times

  NbKnots = sum(M1+M2+...+Mn)
  If U <= K1 then KnotIndex should be equal to M1.
  If U >= Kn then KnotIndex should be equal to NbKnots-Mn-1.
  If Ki <= U < K(i+1) then KnotIndex should be equal to sum (M1+M2+...+Mi).
  */

  Standard_Integer First,Last;
  if (FromK1 < ToK2) {
    First = FromK1;
    Last  = ToK2;
  }
  else {
    First = ToK2;
    Last  = FromK1;
  }
  Standard_Integer Last1 = Last - 1;
  NewU = U;
  if (IsPeriodic && (NewU < UFirst || NewU > ULast))
    NewU = ElCLib::InPeriod(NewU, UFirst, ULast);
  
  BSplCLib::Hunt (Knots, NewU, KnotIndex);
  
  Standard_Real val;
  const Standard_Integer  KLower = Knots.Lower(),
                          KUpper = Knots.Upper();

  const Standard_Real Eps = Epsilon(Min(Abs(Knots(KUpper)), Abs(U)));

  const Standard_Real *knots = &Knots(KLower);
  knots -= KLower;
  if ( KnotIndex < Knots.Upper()) {
    val = NewU - knots[KnotIndex + 1];
    if (val < 0) val = - val;
    // <= to be coherent with Segment where Eps corresponds to a bit of error.
    if (val <= Eps) KnotIndex++; 
  }
  if (KnotIndex < First) KnotIndex = First;
  if (KnotIndex > Last1) KnotIndex = Last1;
  
  if (KnotIndex != Last1) {
    Standard_Real K1 = knots[KnotIndex];
    Standard_Real K2 = knots[KnotIndex + 1];
    val = K2 - K1;
    if (val < 0) val = - val;

    while (val <= Eps) {
      KnotIndex++;

      if(KnotIndex >= Knots.Upper())
        break;

      K1 = K2;
      K2 = knots[KnotIndex + 1];
      val = K2 - K1;
      if (val < 0) val = - val;
    }
  }
}

//=======================================================================
//function : LocateParameter
//purpose  : the index is recomputed only if out of range
//pmn  28-01-97 -> eventual computation of the period.
//=======================================================================

void BSplCLib::LocateParameter 
(const Standard_Integer         Degree,
 const TColStd_Array1OfReal&    Knots,
 const TColStd_Array1OfInteger* Mults,
 const Standard_Real            U,
 const Standard_Boolean         Periodic,
 Standard_Integer&              KnotIndex,
 Standard_Real&                 NewU) 
{
  Standard_Integer first,last;
  if (Mults) {
    if (Periodic) {
      first = Knots.Lower();
      last  = Knots.Upper();
    }
    else {
      first = FirstUKnotIndex(Degree,*Mults);
      last  = LastUKnotIndex (Degree,*Mults);
    }
  }
  else {
    first = Knots.Lower() + Degree;
    last  = Knots.Upper() - Degree;
  }
  if ( KnotIndex < first || KnotIndex > last)
    BSplCLib::LocateParameter(Knots, U, Periodic, first, last,
			      KnotIndex, NewU, Knots(first), Knots(last));
  else
    NewU = U;
}

//=======================================================================
//function : MaxKnotMult
//purpose  : 
//=======================================================================

Standard_Integer BSplCLib::MaxKnotMult
(const Array1OfInteger& Mults,
 const Standard_Integer          FromK1,
 const Standard_Integer          ToK2)
{
  Standard_Integer MLower = Mults.Lower();
  const Standard_Integer *pmu = &Mults(MLower);
  pmu -= MLower;
  Standard_Integer MaxMult = pmu[FromK1];

  for (Standard_Integer i = FromK1; i <= ToK2; i++) {
    if (MaxMult < pmu[i]) MaxMult = pmu[i];
  }
  return MaxMult;
}

//=======================================================================
//function : MinKnotMult
//purpose  : 
//=======================================================================

Standard_Integer BSplCLib::MinKnotMult
(const Array1OfInteger& Mults,
 const Standard_Integer          FromK1,
 const Standard_Integer          ToK2)
{
  Standard_Integer MLower = Mults.Lower();
  const Standard_Integer *pmu = &Mults(MLower);
  pmu -= MLower;
  Standard_Integer MinMult = pmu[FromK1];

  for (Standard_Integer i = FromK1; i <= ToK2; i++) {
    if (MinMult > pmu[i]) MinMult = pmu[i];
  }
  return MinMult;
}

//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer BSplCLib::NbPoles(const Standard_Integer Degree,
				   const Standard_Boolean Periodic,
				   const TColStd_Array1OfInteger& Mults)
{
  Standard_Integer i,sigma = 0;
  Standard_Integer f = Mults.Lower();
  Standard_Integer l = Mults.Upper();
  const Standard_Integer * pmu = &Mults(f);
  pmu -= f;
  Standard_Integer Mf = pmu[f];
  Standard_Integer Ml = pmu[l];
  if (Mf <= 0) return 0;
  if (Ml <= 0) return 0;
  if (Periodic) {
    if (Mf > Degree) return 0;
    if (Ml > Degree) return 0;
    if (Mf != Ml   ) return 0;
    sigma = Mf;
  }
  else {
    Standard_Integer Deg1 = Degree + 1;
    if (Mf > Deg1) return 0;
    if (Ml > Deg1) return 0;
    sigma = Mf + Ml - Deg1;
  }
    
  for (i = f + 1; i < l; i++) {
    if (pmu[i] <= 0    ) return 0;
    if (pmu[i] > Degree) return 0;
    sigma += pmu[i];
  }
  return sigma;
}

//=======================================================================
//function : KnotSequenceLength
//purpose  : 
//=======================================================================

Standard_Integer BSplCLib::KnotSequenceLength
(const TColStd_Array1OfInteger& Mults,
 const Standard_Integer         Degree,
 const Standard_Boolean         Periodic)
{
  Standard_Integer i,l = 0;
  Standard_Integer MLower = Mults.Lower();
  Standard_Integer MUpper = Mults.Upper();
  const Standard_Integer * pmu = &Mults(MLower);
  pmu -= MLower;

  for (i = MLower; i <= MUpper; i++)
    l += pmu[i];
  if (Periodic) l += 2 * (Degree + 1 - pmu[MLower]);
  return l;
}

//=======================================================================
//function : KnotSequence
//purpose  : 
//=======================================================================

void BSplCLib::KnotSequence 
(const TColStd_Array1OfReal&    Knots,
 const TColStd_Array1OfInteger& Mults,
 TColStd_Array1OfReal&          KnotSeq,
 const Standard_Boolean         Periodic)
{
  BSplCLib::KnotSequence(Knots,Mults,0,Periodic,KnotSeq);
}

//=======================================================================
//function : KnotSequence
//purpose  : 
//=======================================================================

void BSplCLib::KnotSequence 
(const TColStd_Array1OfReal&    Knots,
 const TColStd_Array1OfInteger& Mults,
 const Standard_Integer         Degree,
 const Standard_Boolean         Periodic,
 TColStd_Array1OfReal&          KnotSeq)
{
  Standard_Real K;
  Standard_Integer Mult;
  Standard_Integer MLower = Mults.Lower();
  const Standard_Integer * pmu = &Mults(MLower);
  pmu -= MLower;
  Standard_Integer KLower = Knots.Lower();
  Standard_Integer KUpper = Knots.Upper();
  const Standard_Real * pkn = &Knots(KLower);
  pkn -= KLower;
  Standard_Integer M1 = Degree + 1 - pmu[MLower];  // for periodic
  Standard_Integer i,j,index = Periodic ? M1 + 1 : 1;

  for (i = KLower; i <= KUpper; i++) {
    Mult = pmu[i];
    K    = pkn[i];

    for (j = 1; j <= Mult; j++) { 
      KnotSeq (index) = K;   
      index++;
    }
  }
  if (Periodic) {
    Standard_Real period = pkn[KUpper] - pkn[KLower];
    Standard_Integer m;
    m = 1;
    j = KUpper - 1;

    for (i = M1; i >= 1; i--) {
      KnotSeq(i) = pkn[j] - period;
      m++;
      if (m > pmu[j]) {
	j--;
	m = 1;
      }
    }
    m = 1;
    j = KLower + 1;

    for (i = index; i <= KnotSeq.Upper(); i++) {
      KnotSeq(i) = pkn[j] + period;
      m++;
      if (m > pmu[j]) {
	j++;
	m = 1;
      }
    }
  }
}

//=======================================================================
//function : KnotsLength
//purpose  : 
//=======================================================================
 Standard_Integer BSplCLib::KnotsLength(const TColStd_Array1OfReal& SeqKnots,
//					const Standard_Boolean Periodic)
					const Standard_Boolean )
{
  Standard_Integer sizeMult = 1; 
  Standard_Real val = SeqKnots(1);
  for (Standard_Integer jj=2;
       jj<=SeqKnots.Length();jj++)
    {
      // test on strict equality on nodes
      if (SeqKnots(jj)!=val)
        {
          val = SeqKnots(jj);
          sizeMult++;
        }
    }
  return sizeMult;
}

//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================
void BSplCLib::Knots(const TColStd_Array1OfReal& SeqKnots, 
		     TColStd_Array1OfReal &knots,
		     TColStd_Array1OfInteger &mult,
//		     const Standard_Boolean Periodic)
		     const Standard_Boolean )
{
  Standard_Real val = SeqKnots(1);
  Standard_Integer kk=1;
  knots(kk) = val;
  mult(kk)  = 1;

  for (Standard_Integer jj=2;jj<=SeqKnots.Length();jj++)
    {
      // test on strict equality on nodes
      if (SeqKnots(jj)!=val)
        {
          val = SeqKnots(jj);
          kk++;
          knots(kk) = val;
          mult(kk)  = 1;
        }
      else
        {
          mult(kk)++;
        }
    }
}

//=======================================================================
//function : KnotForm
//purpose  : 
//=======================================================================

BSplCLib_KnotDistribution BSplCLib::KnotForm
(const Array1OfReal& Knots,
 const Standard_Integer       FromK1,
 const Standard_Integer       ToK2)
{
   Standard_Real DU0,DU1,Ui,Uj,Eps0,val;
   BSplCLib_KnotDistribution  KForm = BSplCLib_Uniform;

   if (FromK1 + 1 > Knots.Upper())
   {
     return BSplCLib_Uniform;
   }

   Ui  = Knots(FromK1);
   if (Ui < 0) Ui = - Ui;
   Uj  = Knots(FromK1 + 1);
   if (Uj < 0) Uj = - Uj;
   DU0 = Uj - Ui;
   if (DU0 < 0) DU0 = - DU0;
   Eps0 = Epsilon (Ui) + Epsilon (Uj) + Epsilon (DU0);
   Standard_Integer i = FromK1 + 1;

   while (KForm != BSplCLib_NonUniform && i < ToK2) {
     Ui = Knots(i);
     if (Ui < 0) Ui = - Ui;
     i++;
     Uj = Knots(i);
     if (Uj < 0) Uj = - Uj;
     DU1 = Uj - Ui;
     if (DU1 < 0) DU1 = - DU1;
     val = DU1 - DU0;
     if (val < 0) val = -val;
     if (val > Eps0) KForm = BSplCLib_NonUniform;
     DU0 = DU1;
     Eps0 = Epsilon (Ui) + Epsilon (Uj) + Epsilon (DU0);
   }
   return KForm;
}

//=======================================================================
//function : MultForm
//purpose  : 
//=======================================================================

BSplCLib_MultDistribution BSplCLib::MultForm
(const Array1OfInteger& Mults,
 const Standard_Integer          FromK1,
 const Standard_Integer          ToK2)
{
  Standard_Integer First,Last;
  if (FromK1 < ToK2) {
    First = FromK1;
    Last  = ToK2;
  }
  else {
    First = ToK2;
    Last  = FromK1;
  }
  if (First + 1 > Mults.Upper())
  {
    return BSplCLib_Constant;
  }

  Standard_Integer FirstMult = Mults(First);
  BSplCLib_MultDistribution MForm = BSplCLib_Constant;
  Standard_Integer i    = First + 1;
  Standard_Integer Mult = Mults(i);
  
//  while (MForm != BSplCLib_NonUniform && i <= Last) { ???????????JR????????
  while (MForm != BSplCLib_NonConstant && i <= Last) {
    if (i == First + 1) {  
      if (Mult != FirstMult)      MForm = BSplCLib_QuasiConstant;
    }
    else if (i == Last)  {
      if (MForm == BSplCLib_QuasiConstant) {
        if (FirstMult != Mults(i))  MForm = BSplCLib_NonConstant;
      }
      else {
        if (Mult != Mults(i))       MForm = BSplCLib_NonConstant;
      }
    }
    else {
      if (Mult != Mults(i))         MForm = BSplCLib_NonConstant;
      Mult = Mults(i);
    }
    i++;
  }
  return MForm;
}

//=======================================================================
//function : KnotAnalysis
//purpose  : 
//=======================================================================

void BSplCLib::KnotAnalysis (const Standard_Integer         Degree,
                             const Standard_Boolean         Periodic,
                             const TColStd_Array1OfReal&    CKnots,
                             const TColStd_Array1OfInteger& CMults,
                             GeomAbs_BSplKnotDistribution&  KnotForm,
                             Standard_Integer&              MaxKnotMult)
{
  KnotForm = GeomAbs_NonUniform;

  BSplCLib_KnotDistribution KSet = 
    BSplCLib::KnotForm (CKnots, 1, CKnots.Length());
  

  if (KSet == BSplCLib_Uniform) {
    BSplCLib_MultDistribution MSet =
      BSplCLib::MultForm (CMults, 1, CMults.Length());
    switch (MSet) {
    case BSplCLib_NonConstant   :       
      break;
    case BSplCLib_Constant      : 
      if (CKnots.Length() == 2) {
        KnotForm = GeomAbs_PiecewiseBezier;
      }
      else {
        if (CMults (1) == 1)  KnotForm = GeomAbs_Uniform;   
      }
      break;
    case BSplCLib_QuasiConstant :   
      if (CMults (1) == Degree + 1) {
        Standard_Real M = CMults (2);
        if (M == Degree )   KnotForm = GeomAbs_PiecewiseBezier;
        else if  (M == 1)   KnotForm = GeomAbs_QuasiUniform;
      }
      break;
    }
  }

  Standard_Integer FirstKM = 
    Periodic ? CKnots.Lower() :  BSplCLib::FirstUKnotIndex (Degree,CMults);
  Standard_Integer LastKM =
    Periodic ? CKnots.Upper() :  BSplCLib::LastUKnotIndex (Degree,CMults);
  MaxKnotMult = 0;
  if (LastKM - FirstKM != 1) {
    Standard_Integer Multi;
    for (Standard_Integer i = FirstKM + 1; i < LastKM; i++) {
      Multi = CMults (i);
      MaxKnotMult = Max (MaxKnotMult, Multi);
    }
  }
}

//=======================================================================
//function : Reparametrize
//purpose  : 
//=======================================================================

void BSplCLib::Reparametrize
(const Standard_Real      U1,
 const Standard_Real      U2,
 Array1OfReal&   Knots)
{
  Standard_Integer Lower  = Knots.Lower();
  Standard_Integer Upper  = Knots.Upper();
  Standard_Real UFirst    = Min (U1, U2);
  Standard_Real ULast     = Max (U1, U2);
  Standard_Real NewLength = ULast - UFirst;
  BSplCLib_KnotDistribution KSet = BSplCLib::KnotForm (Knots, Lower, Upper);
  if (KSet == BSplCLib_Uniform) {
    Standard_Real DU = NewLength / (Upper - Lower);
    Knots (Lower) = UFirst;

    for (Standard_Integer i = Lower + 1; i <= Upper; i++) {
      Knots (i) = Knots (i-1) + DU;
    }
  }
  else {
    Standard_Real K2;
    Standard_Real Ratio;
    Standard_Real K1 = Knots (Lower);
    Standard_Real Length = Knots (Upper) - Knots (Lower);
    Knots (Lower) = UFirst;

    for (Standard_Integer i = Lower + 1; i <= Upper; i++) {
      K2 = Knots (i);
      Ratio = (K2 - K1) / Length;
      Knots (i) = Knots (i-1) + (NewLength * Ratio);

      //for CheckCurveData
      Standard_Real Eps = Epsilon( Abs(Knots(i-1)) );
      if (Knots(i) - Knots(i-1) <= Eps)
	Knots(i) = NextAfter (Knots(i-1) + Eps, RealLast());

      K1 = K2;
    }
  }
}

//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void  BSplCLib::Reverse(TColStd_Array1OfReal& Knots)
{
  Standard_Integer first = Knots.Lower();
  Standard_Integer last  = Knots.Upper();
  Standard_Real kfirst = Knots(first);
  Standard_Real klast = Knots(last);
  Standard_Real tfirst = kfirst;
  Standard_Real tlast  = klast;
  first++;
  last--;

  while (first <= last) {
    tfirst += klast - Knots(last);
    tlast  -= Knots(first) - kfirst;
    kfirst = Knots(first);
    klast  = Knots(last);
    Knots(first) = tfirst;
    Knots(last)  = tlast;
    first++;
    last--;
  }
}

//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void  BSplCLib::Reverse(TColStd_Array1OfInteger& Mults)
{
  Standard_Integer first = Mults.Lower();
  Standard_Integer last  = Mults.Upper();
  Standard_Integer temp;

  while (first < last) {
    temp = Mults(first);
    Mults(first) = Mults(last);
    Mults(last) = temp;
    first++;
    last--;
  }
}

//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void  BSplCLib::Reverse(TColStd_Array1OfReal& Weights,
			const Standard_Integer L)
{
  Standard_Integer i, l = L;
  l = Weights.Lower()+(l-Weights.Lower())%(Weights.Upper()-Weights.Lower()+1);

  TColStd_Array1OfReal temp(0,Weights.Length()-1);

  for (i = Weights.Lower(); i <= l; i++)
    temp(l-i) = Weights(i);

  for (i = l+1; i <= Weights.Upper(); i++)
    temp(l-Weights.Lower()+Weights.Upper()-i+1) = Weights(i);

  for (i = Weights.Lower(); i <= Weights.Upper(); i++)
    Weights(i) = temp(i-Weights.Lower());
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean  BSplCLib::IsRational(const TColStd_Array1OfReal& Weights,
				       const Standard_Integer I1,
				       const Standard_Integer I2,
//				       const Standard_Real Epsi)
				       const Standard_Real )
{
  Standard_Integer i, f = Weights.Lower(), l = Weights.Length();
  Standard_Integer I3 = I2 - f;
  const Standard_Real * WG = &Weights(f);
  WG -= f;

  for (i = I1 - f; i < I3; i++) {
    if (WG[f + (i % l)] != WG[f + ((i + 1) % l)]) return Standard_True;
  }
  return Standard_False ;
}

//=======================================================================
//function : Eval
//purpose  : evaluate point and derivatives
//=======================================================================

void  BSplCLib::Eval(const Standard_Real U,
		     const Standard_Integer Degree,
		     Standard_Real& Knots, 
		     const Standard_Integer Dimension, 
		     Standard_Real& Poles)
{
  Standard_Integer step,i,Dms,Dm1,Dpi,Sti;
  Standard_Real X, Y, *poles, *knots = &Knots;
  Dm1 = Dms = Degree;
  Dm1--;
  Dms++;
  switch (Dimension) { 

  case 1 : {
    
    for (step = - 1; step < Dm1; step++) {
      Dms--;
      poles = &Poles;
      Dpi   = Dm1;
      Sti   = step;
      
      for (i = 0; i < Dms; i++) {
	Dpi++;
	Sti++;
	X = (knots[Dpi] - U) / (knots[Dpi] - knots[Sti]);
	Y = 1 - X;
	poles[0] *= X; poles[0] += Y * poles[1];
	poles += 1;
      }
    }
    break;
  }
  case 2 : {
    
    for (step = - 1; step < Dm1; step++) {
      Dms--;
      poles = &Poles;
      Dpi   = Dm1;
      Sti   = step;
      
      for (i = 0; i < Dms; i++) {
	Dpi++;
	Sti++;
	X = (knots[Dpi] - U) / (knots[Dpi] - knots[Sti]);
	Y = 1 - X;
	poles[0] *= X; poles[0] += Y * poles[2];
	poles[1] *= X; poles[1] += Y * poles[3];
	poles += 2;
      }
    }
    break;
  }
  case 3 : {
    
    for (step = - 1; step < Dm1; step++) {
      Dms--;
      poles = &Poles;
      Dpi   = Dm1;
      Sti   = step;
      
      for (i = 0; i < Dms; i++) {
	Dpi++;
	Sti++;
	X = (knots[Dpi] - U) / (knots[Dpi] - knots[Sti]);
	Y = 1 - X;
	poles[0] *= X; poles[0] += Y * poles[3];
	poles[1] *= X; poles[1] += Y * poles[4];
	poles[2] *= X; poles[2] += Y * poles[5];
	poles += 3;
      }
    }
    break;
  }
  case 4 : {
    
    for (step = - 1; step < Dm1; step++) {
      Dms--;
      poles = &Poles;
      Dpi   = Dm1;
      Sti   = step;
      
      for (i = 0; i < Dms; i++) {
	Dpi++;
	Sti++;
	X = (knots[Dpi] - U) / (knots[Dpi] - knots[Sti]);
	Y = 1 - X;
	poles[0] *= X; poles[0] += Y * poles[4];
	poles[1] *= X; poles[1] += Y * poles[5];
	poles[2] *= X; poles[2] += Y * poles[6];
	poles[3] *= X; poles[3] += Y * poles[7];
	poles += 4;
      }
    }
    break;
  }
    default : {
      Standard_Integer k;
      
      for (step = - 1; step < Dm1; step++) {
	Dms--;
	poles = &Poles;
	Dpi   = Dm1;
	Sti   = step;
	
	for (i = 0; i < Dms; i++) {
	  Dpi++;
	  Sti++;
	  X = (knots[Dpi] - U) / (knots[Dpi] - knots[Sti]);
	  Y = 1 - X;
	  
	  for (k = 0; k < Dimension; k++) {
	    poles[k] *= X;
	    poles[k] += Y * poles[k + Dimension];
	  }
	  poles += Dimension;
	}
      }
    }
  }
}

//=======================================================================
//function : BoorScheme
//purpose  : 
//=======================================================================

void  BSplCLib::BoorScheme(const Standard_Real U,
			   const Standard_Integer Degree,
			   Standard_Real& Knots, 
			   const Standard_Integer Dimension, 
			   Standard_Real& Poles, 
			   const Standard_Integer Depth, 
			   const Standard_Integer Length)
{
  //
  // Compute the values
  //
  //  P(i,j) (U).
  //
  // for i = 0 to Depth, 
  // j = 0 to Length - i
  //
  //  The Boor scheme is :
  //
  //  P(0,j) = Pole(j)
  //  P(i,j) = x * P(i-1,j) + (1-x) * P(i-1,j+1)
  //
  //    where x = (knot(i+j+Degree) - U) / (knot(i+j+Degree) - knot(i+j))
  //
  //
  //  The values are stored in the array Poles
  //  They are alternatively written if the odd and even positions.
  //
  //  The successives contents of the array are
  //   ***** means unitialised, l = Degree + Length
  //
  //  P(0,0) ****** P(0,1) ...... P(0,l-1) ******** P(0,l)
  //  P(0,0) P(1,0) P(0,1) ...... P(0,l-1) P(1,l-1) P(0,l)
  //  P(0,0) P(1,0) P(2,0) ...... P(2,l-1) P(1,l-1) P(0,l)
  //

  Standard_Integer i,k,step;
  Standard_Real *knots = &Knots;
  Standard_Real *pole, *firstpole = &Poles - 2 * Dimension;
  // the steps of recursion

  for (step = 0; step < Depth; step++) {
    firstpole += Dimension;
    pole = firstpole;
    // compute the new row of poles

    for (i = step; i < Length; i++) {
      pole += 2 * Dimension;
      // coefficient
      Standard_Real X = (knots[i+Degree-step] - U) 
	/ (knots[i+Degree-step] - knots[i]);
      Standard_Real Y = 1. - X;
      // Value
      // P(i,j) = X * P(i-1,j) + (1-X) * P(i-1,j+1)

      for (k = 0; k < Dimension; k++)
	pole[k] = X * pole[k - Dimension] + Y * pole[k + Dimension];
    }
  }
}

//=======================================================================
//function : AntiBoorScheme
//purpose  : 
//=======================================================================

Standard_Boolean  BSplCLib::AntiBoorScheme(const Standard_Real    U,
					   const Standard_Integer Degree,
					   Standard_Real&         Knots, 
					   const Standard_Integer Dimension, 
					   Standard_Real&         Poles, 
					   const Standard_Integer Depth, 
					   const Standard_Integer Length,
					   const Standard_Real    Tolerance)
{
  // do the Boor scheme reverted.

  Standard_Integer i,k,step, half_length;
  Standard_Real *knots = &Knots;
  Standard_Real z,X,Y,*pole, *firstpole = &Poles + (Depth-1) * Dimension;

  // Test the special case length = 1 
  // only verification of the central point

  if (Length == 1) {
    X = (knots[Degree] - U) / (knots[Degree] - knots[0]);
    Y = 1. - X;

    for (k = 0; k < Dimension; k++) {
      z = X * firstpole[k] + Y * firstpole[k+2*Dimension];
      if (Abs(z - firstpole[k+Dimension]) > Tolerance) 
	return Standard_False;
    }
    return Standard_True;
  }

  // General case
  // the steps of recursion

  for (step = Depth-1; step >= 0; step--) {
    firstpole -= Dimension;
    pole = firstpole;

    // first step from left to right

    for (i = step; i < Length-1; i++) {
      pole += 2 * Dimension;

      X = (knots[i+Degree-step] - U) / (knots[i+Degree-step] - knots[i]);
      Y = 1. - X;

      for (k = 0; k < Dimension; k++)
	pole[k+Dimension] = (pole[k] - X*pole[k-Dimension]) / Y;

    }

    // second step from right to left
    pole += 4* Dimension;
    half_length = (Length - 1 + step) / 2  ;
    //
    // only do half of the way from right to left 
    // otherwise it start degenerating because of 
    // overflows
    // 

    for (i = Length-1; i > half_length ; i--) {
      pole -= 2 * Dimension;

      // coefficient
      X = (knots[i+Degree-step] - U) / (knots[i+Degree-step] - knots[i]);
      Y = 1. - X;

      for (k = 0; k < Dimension; k++) {
	z = (pole[k] - Y * pole[k+Dimension]) / X;
	if (Abs(z-pole[k-Dimension]) > Tolerance) 
	  return Standard_False;
	pole[k-Dimension] += z;
	pole[k-Dimension] /= 2.;
      }
    }
  }
  return Standard_True;
}

//=======================================================================
//function : Derivative
//purpose  : 
//=======================================================================

void  BSplCLib::Derivative(const Standard_Integer Degree, 
			   Standard_Real& Knots, 
			   const Standard_Integer Dimension, 
			   const Standard_Integer Length, 
			   const Standard_Integer Order, 
			   Standard_Real& Poles)
{
  Standard_Integer i,k,step,span = Degree;
  Standard_Real *knot = &Knots;

  for (step = 1; step <= Order; step++) {
    Standard_Real* pole = &Poles;

    for (i = step; i < Length; i++) {
      Standard_Real coef = - span / (knot[i+span] - knot[i]);

      for (k = 0; k < Dimension; k++) {
	pole[k] -= pole[k+Dimension];
	pole[k] *= coef;
      }
      pole += Dimension;
    }
    span--;
  }
}

//=======================================================================
//function : Bohm
//purpose  : 
//=======================================================================

void  BSplCLib::Bohm(const Standard_Real U,
		     const Standard_Integer Degree,
		     const Standard_Integer N,
		     Standard_Real& Knots,
		     const Standard_Integer Dimension,
		     Standard_Real& Poles)
{
  // First phase independant of U, compute the poles of the derivatives
  Standard_Integer i,j,iDim,min,Dmi,DDmi,jDmi,Degm1;
  Standard_Real *knot = &Knots, *pole, coef, *tbis, *psav, *psDD, *psDDmDim;
  psav     = &Poles;
  if (N < Degree) min = N;
  else            min = Degree;
  Degm1 = Degree - 1;
  DDmi = (Degree << 1) + 1;
  switch (Dimension) { 
  case 1 : {
    psDD     = psav + Degree;
    psDDmDim = psDD - 1;
    
    for (i = 0; i < Degree; i++) {
      DDmi--;
      pole = psDD;
      tbis = psDDmDim;
      jDmi = DDmi;
      
      for (j = Degm1; j >= i; j--) {
	jDmi--;
	*pole -= *tbis;
  *pole = (knot[jDmi] == knot[j]) ? 0.0 :  *pole / (knot[jDmi] - knot[j]);
	pole--;
	tbis--;
      }
    }
    // Second phase, dependant of U
    iDim = - 1;
    
    for (i = 0; i < Degree; i++) {
      iDim += 1;
      pole  = psav + iDim;
      tbis  = pole + 1;
      coef  = U - knot[i];
      
      for (j = i; j >= 0; j--) {
	*pole += coef * (*tbis);
	pole--;
	tbis--;
      }
    }
    // multiply by the degrees
    coef = Degree;
    Dmi  = Degree;
    pole = psav + 1;
    
    for (i = 1; i <= min; i++) {
      *pole *= coef; pole++;
      Dmi--;
      coef  *= Dmi;
    }
    break;
  }
  case 2 : {
    psDD     = psav + (Degree << 1);
    psDDmDim = psDD - 2;
    
    for (i = 0; i < Degree; i++) {
      DDmi--;
      pole = psDD;
      tbis = psDDmDim;
      jDmi = DDmi;
      
      for (j = Degm1; j >= i; j--) {
	jDmi--;
	coef   = (knot[jDmi] == knot[j]) ? 0.0 : 1. / (knot[jDmi] - knot[j]);
	*pole -= *tbis; *pole *= coef; pole++; tbis++;
	*pole -= *tbis; *pole *= coef;
	pole  -= 3;
	tbis  -= 3;
      }
    }
    // Second phase, dependant of U
    iDim = - 2;
    
    for (i = 0; i < Degree; i++) {
      iDim += 2;
      pole  = psav + iDim;
      tbis  = pole + 2;
      coef  = U - knot[i];
      
      for (j = i; j >= 0; j--) {
	*pole += coef * (*tbis); pole++; tbis++;
	*pole += coef * (*tbis);
	pole  -= 3;
	tbis  -= 3;
      }
    }
    // multiply by the degrees
    coef = Degree;
    Dmi  = Degree;
    pole = psav + 2;
    
    for (i = 1; i <= min; i++) {
      *pole *= coef; pole++;
      *pole *= coef; pole++;
      Dmi--;
      coef  *= Dmi;
    }
    break;
  }
  case 3 : {
    psDD     = psav + (Degree << 1) + Degree;
    psDDmDim = psDD - 3;
    
    for (i = 0; i < Degree; i++) {
      DDmi--;
      pole = psDD;
      tbis = psDDmDim;
      jDmi = DDmi;
      
      for (j = Degm1; j >= i; j--) {
	jDmi--;
	coef   = (knot[jDmi] == knot[j]) ? 0.0 : 1. / (knot[jDmi] - knot[j]);
	*pole -= *tbis; *pole *= coef; pole++; tbis++;
	*pole -= *tbis; *pole *= coef; pole++; tbis++;
	*pole -= *tbis; *pole *= coef;
	pole  -= 5;
	tbis  -= 5;
      }
    }
    // Second phase, dependant of U
    iDim = - 3;
    
    for (i = 0; i < Degree; i++) {
      iDim += 3;
      pole  = psav + iDim;
      tbis  = pole + 3;
      coef  = U - knot[i];
      
      for (j = i; j >= 0; j--) {
	*pole += coef * (*tbis); pole++; tbis++;
	*pole += coef * (*tbis); pole++; tbis++;
	*pole += coef * (*tbis);
	pole  -= 5;
	tbis  -= 5;
      }
    }
    // multiply by the degrees
    coef = Degree;
    Dmi  = Degree;
    pole = psav + 3;
    
    for (i = 1; i <= min; i++) {
      *pole *= coef; pole++;
      *pole *= coef; pole++;
      *pole *= coef; pole++;
      Dmi--;
      coef  *= Dmi;
    }
    break;
  }
  case 4 : {
    psDD     = psav + (Degree << 2);
    psDDmDim = psDD - 4;
    
    for (i = 0; i < Degree; i++) {
      DDmi--;
      pole = psDD;
      tbis = psDDmDim;
      jDmi = DDmi;
      
      for (j = Degm1; j >= i; j--) {
	jDmi--;
	coef   = (knot[jDmi]  == knot[j]) ? 0.0 : 1. /(knot[jDmi] - knot[j]) ;
	*pole -= *tbis; *pole *= coef; pole++; tbis++;
	*pole -= *tbis; *pole *= coef; pole++; tbis++;
	*pole -= *tbis; *pole *= coef; pole++; tbis++;
	*pole -= *tbis; *pole *= coef;
	pole  -= 7;
	tbis  -= 7;
      }
    }
    // Second phase, dependant of U
    iDim = - 4;
    
    for (i = 0; i < Degree; i++) {
      iDim += 4;
      pole  = psav + iDim;
      tbis  = pole + 4;
      coef  = U - knot[i];
      
      for (j = i; j >= 0; j--) {
	*pole += coef * (*tbis); pole++; tbis++;
	*pole += coef * (*tbis); pole++; tbis++;
	*pole += coef * (*tbis); pole++; tbis++;
	*pole += coef * (*tbis);
	pole  -= 7;
	tbis  -= 7;
      }
    }
    // multiply by the degrees
    coef = Degree; 
    Dmi  = Degree;
    pole = psav + 4;
   
    for (i = 1; i <= min; i++) {
      *pole *= coef; pole++;
      *pole *= coef; pole++;
      *pole *= coef; pole++;
      *pole *= coef; pole++;
      Dmi--;
      coef  *= Dmi;
    }
    break;
  }
    default : {
      Standard_Integer k;
      Standard_Integer Dim2 = Dimension << 1;
      psDD     = psav + Degree * Dimension;
      psDDmDim = psDD - Dimension;
      
      for (i = 0; i < Degree; i++) {
	DDmi--;
	pole = psDD;
	tbis = psDDmDim;
	jDmi = DDmi;
	
	for (j = Degm1; j >= i; j--) {
	  jDmi--;
	  coef = (knot[jDmi] == knot[j]) ? 0.0 : 1. / (knot[jDmi] - knot[j]);
	  
	  for (k = 0; k < Dimension; k++) {
	    *pole -= *tbis; *pole *= coef; pole++; tbis++;
	  }
	  pole -= Dim2;
	  tbis -= Dim2;
	}
      }
      // Second phase, dependant of U
      iDim = - Dimension;
      
      for (i = 0; i < Degree; i++) {
	iDim += Dimension;
	pole  = psav + iDim;
	tbis  = pole + Dimension;
	coef  = U - knot[i];
	
	for (j = i; j >= 0; j--) {
	  
	  for (k = 0; k < Dimension; k++) {
	    *pole += coef * (*tbis); pole++; tbis++;
	  }
	  pole -= Dim2;
	  tbis -= Dim2;
	}
      }
      // multiply by the degrees
      coef = Degree;
      Dmi  = Degree;
      pole = psav + Dimension;
      
      for (i = 1; i <= min; i++) {
	
	for (k = 0; k < Dimension; k++) {
	  *pole *= coef; pole++;
	}
	Dmi--;
	coef *= Dmi;
      }
    }
  }
}

//=======================================================================
//function : BuildKnots
//purpose  : 
//=======================================================================

void BSplCLib::BuildKnots(const Standard_Integer         Degree,
			  const Standard_Integer         Index,
			  const Standard_Boolean         Periodic,
			  const TColStd_Array1OfReal&    Knots,
			  const TColStd_Array1OfInteger* Mults,
			  Standard_Real&                 LK)
{
  Standard_Integer KLower = Knots.Lower();
  const Standard_Real * pkn = &Knots(KLower);
  pkn -= KLower;
  Standard_Real *knot = &LK;
  if (Mults == NULL) {
    switch (Degree) {
    case 1 : {
      Standard_Integer j = Index    ;
      knot[0] = pkn[j]; j++;
      knot[1] = pkn[j];
      break;
    }
    case 2 : {
      Standard_Integer j = Index - 1;
      knot[0] = pkn[j]; j++;
      knot[1] = pkn[j]; j++;
      knot[2] = pkn[j]; j++;
      knot[3] = pkn[j];
      break;
    }
    case 3 : {
      Standard_Integer j = Index - 2;
      knot[0] = pkn[j]; j++;
      knot[1] = pkn[j]; j++;
      knot[2] = pkn[j]; j++;
      knot[3] = pkn[j]; j++;
      knot[4] = pkn[j]; j++;
      knot[5] = pkn[j];
      break;
    }
    case 4 : {
      Standard_Integer j = Index - 3;
      knot[0] = pkn[j]; j++;
      knot[1] = pkn[j]; j++;
      knot[2] = pkn[j]; j++;
      knot[3] = pkn[j]; j++;
      knot[4] = pkn[j]; j++;
      knot[5] = pkn[j]; j++;
      knot[6] = pkn[j]; j++;
      knot[7] = pkn[j];
      break;
    }
    case 5 : {
      Standard_Integer j = Index - 4;
      knot[0] = pkn[j]; j++;
      knot[1] = pkn[j]; j++;
      knot[2] = pkn[j]; j++;
      knot[3] = pkn[j]; j++;
      knot[4] = pkn[j]; j++;
      knot[5] = pkn[j]; j++;
      knot[6] = pkn[j]; j++;
      knot[7] = pkn[j]; j++;
      knot[8] = pkn[j]; j++;
      knot[9] = pkn[j];
      break;
    }
    case 6 : {
      Standard_Integer j = Index - 5;
      knot[ 0] = pkn[j]; j++;
      knot[ 1] = pkn[j]; j++;
      knot[ 2] = pkn[j]; j++;
      knot[ 3] = pkn[j]; j++;
      knot[ 4] = pkn[j]; j++;
      knot[ 5] = pkn[j]; j++;
      knot[ 6] = pkn[j]; j++;
      knot[ 7] = pkn[j]; j++;
      knot[ 8] = pkn[j]; j++;
      knot[ 9] = pkn[j]; j++;
      knot[10] = pkn[j]; j++;
      knot[11] = pkn[j];
      break;
    }
      default : {
	Standard_Integer i,j;
	Standard_Integer Deg2 = Degree << 1;
	j = Index - Degree;
	
	for (i = 0; i < Deg2; i++) {
	  j++;
	  knot[i] = pkn[j];
	}
      }
    }
  }
  else {
    Standard_Integer i;
    Standard_Integer Deg1 = Degree - 1;
    Standard_Integer KUpper = Knots.Upper();
    Standard_Integer MLower = Mults->Lower();
    Standard_Integer MUpper = Mults->Upper();
    const Standard_Integer * pmu = &(*Mults)(MLower);
    pmu -= MLower;
    Standard_Real dknot = 0;
    Standard_Integer ilow = Index    , mlow = 0;
    Standard_Integer iupp = Index + 1, mupp = 0;
    Standard_Real loffset = 0., uoffset = 0.;
    Standard_Boolean getlow = Standard_True, getupp = Standard_True;
    if (Periodic) {
      dknot = pkn[KUpper] - pkn[KLower];
      if (iupp > MUpper) {
	iupp = MLower + 1;
	uoffset = dknot;
      }
    }
    // Find the knots around Index

    for (i = 0; i < Degree; i++) {
      if (getlow) {
	mlow++;
	if (mlow > pmu[ilow]) {
	  mlow = 1;
	  ilow--;
	  getlow =  (ilow >= MLower);
	  if (Periodic && !getlow) {
	    ilow = MUpper - 1;
	    loffset = dknot;
	    getlow = Standard_True;
	  }
	}
	if (getlow)
	  knot[Deg1 - i] = pkn[ilow] - loffset;
      }
      if (getupp) {
	mupp++;
	if (mupp > pmu[iupp]) {
	  mupp = 1;
	  iupp++;
	  getupp = (iupp <= MUpper);
	  if (Periodic && !getupp) {
	    iupp = MLower + 1;
	    uoffset = dknot;
	    getupp = Standard_True;
	  }
	}
	if (getupp)
	  knot[Degree + i] = pkn[iupp] + uoffset;
      }
    }
  } 
}

//=======================================================================
//function : PoleIndex
//purpose  : 
//=======================================================================

Standard_Integer BSplCLib::PoleIndex(const Standard_Integer         Degree,
				     const Standard_Integer         Index,
				     const Standard_Boolean         Periodic,
				     const TColStd_Array1OfInteger& Mults)
{
  Standard_Integer i, pindex = 0;

  for (i = Mults.Lower(); i <= Index; i++)
    pindex += Mults(i);
  if (Periodic)
    pindex -= Mults(Mults.Lower());
  else
    pindex -= Degree + 1;

  return pindex;
}

//=======================================================================
//function : BuildBoor
//purpose  : builds the local array for boor
//=======================================================================

void  BSplCLib::BuildBoor(const Standard_Integer         Index,
			  const Standard_Integer         Length,
			  const Standard_Integer         Dimension,
			  const TColStd_Array1OfReal&    Poles,
			  Standard_Real&                 LP)
{
  Standard_Real *poles = &LP;
  Standard_Integer i,k, ip = Poles.Lower() + Index * Dimension;
  
  for (i = 0; i < Length+1; i++) {

    for (k = 0; k < Dimension; k++) {
      poles[k] = Poles(ip);
      ip++;
      if (ip > Poles.Upper()) ip = Poles.Lower();
    }
    poles += 2 * Dimension;
  }
}

//=======================================================================
//function : BoorIndex
//purpose  : 
//=======================================================================

Standard_Integer  BSplCLib::BoorIndex(const Standard_Integer Index,
				      const Standard_Integer Length,
				      const Standard_Integer Depth)
{
  if (Index <= Depth)  return Index;
  if (Index <= Length) return 2 * Index - Depth;
  return                      Length + Index - Depth;
}

//=======================================================================
//function : GetPole
//purpose  : 
//=======================================================================

void  BSplCLib::GetPole(const Standard_Integer         Index,
			const Standard_Integer         Length,
			const Standard_Integer         Depth,
			const Standard_Integer         Dimension,
			Standard_Real&                 LP,
			Standard_Integer&              Position,
			TColStd_Array1OfReal&          Pole)
{
  Standard_Integer k;
  Standard_Real* pole = &LP + BoorIndex(Index,Length,Depth) * Dimension;

  for (k = 0; k < Dimension; k++) {
    Pole(Position) = pole[k];
    Position++;
  }
  if (Position > Pole.Upper()) Position = Pole.Lower();
}

//=======================================================================
//function : PrepareInsertKnots
//purpose  : 
//=======================================================================

Standard_Boolean  BSplCLib::PrepareInsertKnots
(const Standard_Integer         Degree,
 const Standard_Boolean         Periodic, 
 const TColStd_Array1OfReal&    Knots,
 const TColStd_Array1OfInteger& Mults,
 const TColStd_Array1OfReal&    AddKnots,
 const TColStd_Array1OfInteger* AddMults,
 Standard_Integer&              NbPoles,
 Standard_Integer&              NbKnots, 
 const Standard_Real            Tolerance,
 const Standard_Boolean         Add)
{
  Standard_Boolean addflat = AddMults == NULL;
  
  Standard_Integer first,last;
  if (Periodic) {
    first = Knots.Lower();
    last  = Knots.Upper();
  }
  else {
    first = FirstUKnotIndex(Degree,Mults);
    last  = LastUKnotIndex(Degree,Mults);
  }
  Standard_Real adeltaK1 = Knots(first)-AddKnots(AddKnots.Lower());
   Standard_Real adeltaK2 = AddKnots(AddKnots.Upper())-Knots(last);
  if (adeltaK1 > Tolerance) return Standard_False;
  if (adeltaK2  > Tolerance) return Standard_False;
  
  Standard_Integer sigma = 0, mult, amult;
  NbKnots = 0;
  Standard_Integer  k  = Knots.Lower() - 1;
  Standard_Integer  ak = AddKnots.Lower();

  if(Periodic && AddKnots.Length() > 1)
  {
    //gka for case when segments was produced on full period only one knot
    //was added in the end of curve
    if(fabs(adeltaK1) <= gp::Resolution() && 
       fabs(adeltaK2) <= gp::Resolution())
      ak++;
  }
  
  Standard_Integer aLastKnotMult = Mults (Knots.Upper());
  Standard_Real au,oldau = AddKnots(ak),Eps;
  
  while (ak <= AddKnots.Upper()) {
    au = AddKnots(ak);
    if (au < oldau) return Standard_False;
    oldau = au;

    Eps = Max(Tolerance,Epsilon(au));
    
    while ((k < Knots.Upper()) && (Knots(k+1)  - au <= Eps)) {
      k++;
      NbKnots++;
      sigma += Mults(k);
    }

    if (addflat) amult = 1;
    else         amult = Max(0,(*AddMults)(ak));
    
    while ((ak < AddKnots.Upper()) &&
	   (Abs(au - AddKnots(ak+1)) <= Eps)) {
      ak++;
      if (Add) {
	if (addflat) amult++;
	else         amult += Max(0,(*AddMults)(ak));
      }
    }
    
    
    if (Abs(au - Knots(k)) <= Eps) {
      // identic to existing knot
      mult = Mults(k);
      if (Add) {
	if (mult + amult > Degree)
	  amult = Max(0,Degree - mult);
	sigma += amult;
	
      }
      else if (amult > mult) {
	if (amult > Degree) amult = Degree;
        if (k == Knots.Upper () && Periodic)
        {
          aLastKnotMult = Max (amult, mult);
          sigma += 2 * (aLastKnotMult - mult);
        }
        else
        {
	  sigma += amult - mult;
        }
      }
      /*
      // on periodic curves if this is the last knot
      // the multiplicity is added twice to account for the first knot
      if (k == Knots.Upper() && Periodic) {
	if (Add)
	  sigma += amult;
	else
	  sigma += amult - mult;
      }
      */
    }
    else {
      // not identic to existing knot
      if (amult > 0) {
	if (amult > Degree) amult = Degree;
	NbKnots++;
	sigma += amult;
      }
    }
    
    ak++;
  }
  
  // count the last knots
  while (k < Knots.Upper()) {
    k++;
    NbKnots++;
    sigma += Mults(k);
  }

  if (Periodic) {
    //for periodic B-Spline the requirement is that multiplicites of the first
    //and last knots must be equal (see Geom_BSplineCurve constructor for
    //instance);
    //respectively AddMults() must meet this requirement if AddKnots() contains
    //knot(s) coincident with first or last
    NbPoles = sigma - aLastKnotMult;
  }
  else {
    NbPoles = sigma - Degree - 1;
  }
 
  return Standard_True;
}

//=======================================================================
//function : Copy
//purpose  : copy reals from an array to an other
//        
//   NbValues are copied from OldPoles(OldFirst)
//                 to    NewPoles(NewFirst)
//
//   Periodicity is handled.
//   OldFirst and NewFirst are updated 
//   to the position after the last copied pole.
//
//=======================================================================

static void Copy(const Standard_Integer      NbPoles,
		 Standard_Integer&           OldFirst,
		 const TColStd_Array1OfReal& OldPoles,
		 Standard_Integer&           NewFirst,
		 TColStd_Array1OfReal&       NewPoles)
{
  // reset the index in the range for periodicity

  OldFirst = OldPoles.Lower() + 
    (OldFirst - OldPoles.Lower()) % (OldPoles.Upper() - OldPoles.Lower() + 1);

  NewFirst = NewPoles.Lower() + 
    (NewFirst - NewPoles.Lower()) % (NewPoles.Upper() - NewPoles.Lower() + 1);

  // copy
  Standard_Integer i;

  for (i = 1; i <= NbPoles; i++) {
    NewPoles(NewFirst) = OldPoles(OldFirst);
    OldFirst++;
    if (OldFirst > OldPoles.Upper()) OldFirst = OldPoles.Lower();
    NewFirst++;
    if (NewFirst > NewPoles.Upper()) NewFirst = NewPoles.Lower();
  }
}
		      
//=======================================================================
//function : InsertKnots
//purpose  : insert an array of knots and multiplicities
//=======================================================================

void BSplCLib::InsertKnots
(const Standard_Integer         Degree, 
 const Standard_Boolean         Periodic,
 const Standard_Integer         Dimension, 
 const TColStd_Array1OfReal&    Poles,  
 const TColStd_Array1OfReal&    Knots,    
 const TColStd_Array1OfInteger& Mults, 
 const TColStd_Array1OfReal&    AddKnots,    
 const TColStd_Array1OfInteger* AddMults, 
 TColStd_Array1OfReal&          NewPoles,
 TColStd_Array1OfReal&          NewKnots,    
 TColStd_Array1OfInteger&       NewMults, 
 const Standard_Real            Tolerance,
 const Standard_Boolean         Add)
{
  Standard_Boolean addflat  = AddMults == NULL;
  
  Standard_Integer i,k,mult,firstmult;
  Standard_Integer index,kn,curnk,curk;
  Standard_Integer p,np, curp, curnp, length, depth;
  Standard_Real u;
  Standard_Integer need;
  Standard_Real Eps;

  // -------------------
  // create local arrays
  // -------------------

  Standard_Real *knots = new Standard_Real[2*Degree];
  Standard_Real *poles = new Standard_Real[(2*Degree+1)*Dimension];
  
  //----------------------------
  // loop on the knots to insert
  //----------------------------
  
  curk   = Knots.Lower()-1;          // current position in Knots
  curnk  = NewKnots.Lower()-1;       // current position in NewKnots
  curp   = Poles.Lower();            // current position in Poles
  curnp  = NewPoles.Lower();         // current position in NewPoles

  // NewKnots, NewMults, NewPoles contains the current state of the curve

  // index is the first pole of the current curve for insertion schema

  if (Periodic) index = -Mults(Mults.Lower());
  else          index = -Degree-1;

  // on Periodic curves the first knot and the last knot are inserted later
  // (they are the same knot)
  firstmult = 0;  // multiplicity of the first-last knot for periodic
  

  // kn current knot to insert in AddKnots

  for (kn = AddKnots.Lower(); kn <= AddKnots.Upper(); kn++) {
    
    u = AddKnots(kn);
    Eps = Max(Tolerance,Epsilon(u));
    
    //-----------------------------------
    // find the position in the old knots
    // and copy to the new knots
    //-----------------------------------
    
    while (curk < Knots.Upper() && Knots(curk+1) - u <= Eps) {
      curk++; curnk++;
      NewKnots(curnk) = Knots(curk);
      index += NewMults(curnk) = Mults(curk);
    }
    
    //-----------------------------------
    // Slice the knots and the mults
    // to the current size of the new curve
    //-----------------------------------

    i = curnk + Knots.Upper() - curk;
    TColStd_Array1OfReal    nknots(NewKnots(NewKnots.Lower()),NewKnots.Lower(),i);
    TColStd_Array1OfInteger nmults(NewMults(NewMults.Lower()),NewMults.Lower(),i);

    //-----------------------------------
    // copy enough knots 
    // to compute the insertion schema
    //-----------------------------------

    k = curk;
    i = curnk;
    mult = 0;

    while (mult < Degree && k < Knots.Upper()) {
      k++; i++;
      nknots(i) = Knots(k);
      mult += nmults(i) = Mults(k);
    }

    // copy knots at the end for periodic curve
    if (Periodic) {
      mult = 0;
      k = Knots.Upper();
      i = nknots.Upper();

      while (mult < Degree && i > curnk) {
	nknots(i) = Knots(k);
	mult += nmults(i) = Mults(k);
	k--;
	i--;
      }
      nmults(nmults.Upper()) = nmults(nmults.Lower());
    }

  

    //------------------------------------
    // do the boor scheme on the new curve
    // to insert the new knot
    //------------------------------------
    
    Standard_Boolean sameknot = (Abs(u-NewKnots(curnk)) <= Eps);
    
    if (sameknot) length = Max(0,Degree - NewMults(curnk));
    else          length = Degree;
    
    if (addflat) depth = 1;
    else         depth = Min(Degree,(*AddMults)(kn));

    if (sameknot) {
      if (Add) {
	if ((NewMults(curnk) + depth) > Degree)
	  depth = Degree - NewMults(curnk);
      }
      else {
	depth = Max(0,depth-NewMults(curnk));
      }

      if (Periodic) {
	// on periodic curve the first and last knot are delayed to the end
	if (curk == Knots.Lower() || (curk == Knots.Upper())) {
          if (firstmult == 0) // do that only once
            firstmult += depth;
	  depth = 0;
	}
      }
    }
    if (depth <= 0) continue;
    
    BuildKnots(Degree,curnk,Periodic,nknots,&nmults,*knots);

    // copy the poles

    need   = NewPoles.Lower()+(index+length+1)*Dimension - curnp;
    need = Min(need,Poles.Upper() - curp + 1);

    p = curp;
    np = curnp;
    Copy(need,p,Poles,np,NewPoles);
    curp  += need;
    curnp += need;

    // slice the poles to the current number of poles in case of periodic
    TColStd_Array1OfReal npoles(NewPoles(NewPoles.Lower()),NewPoles.Lower(),curnp-1);

    BuildBoor(index,length,Dimension,npoles,*poles);
    BoorScheme(u,Degree,*knots,Dimension,*poles,depth,length);
    
    //-------------------
    // copy the new poles
    //-------------------

    curnp += depth * Dimension; // number of poles is increased by depth
    TColStd_Array1OfReal ThePoles(NewPoles(NewPoles.Lower()),NewPoles.Lower(),curnp-1);
    np = NewKnots.Lower()+(index+1)*Dimension;

    for (i = 1; i <= length + depth; i++)
      GetPole(i,length,depth,Dimension,*poles,np,ThePoles);
    
    //-------------------
    // insert the knot
    //-------------------

    index += depth;
    if (sameknot) {
      NewMults(curnk) += depth;
    }
    else {
      curnk++;
      NewKnots(curnk) = u;
      NewMults(curnk) = depth;
    }
  }
  
  //------------------------------
  // copy the last poles and knots
  //------------------------------
  
  Copy(Poles.Upper() - curp + 1,curp,Poles,curnp,NewPoles);
  
  while (curk < Knots.Upper()) {
    curk++;  curnk++;
    NewKnots(curnk) = Knots(curk);
    NewMults(curnk) = Mults(curk);
  }
  
  //------------------------------
  // process the first-last knot 
  // on periodic curves
  //------------------------------

  if (firstmult > 0) {
    curnk = NewKnots.Lower();
    if (NewMults(curnk) + firstmult > Degree) {
      firstmult = Degree - NewMults(curnk);
    }
    if (firstmult > 0) {

      length = Degree - NewMults(curnk);
      depth  = firstmult;

      BuildKnots(Degree,curnk,Periodic,NewKnots,&NewMults,*knots);
      TColStd_Array1OfReal npoles(NewPoles(NewPoles.Lower()),
				  NewPoles.Lower(),
				  NewPoles.Upper()-depth*Dimension);
      BuildBoor(0,length,Dimension,npoles,*poles);
      BoorScheme(NewKnots(curnk),Degree,*knots,Dimension,*poles,depth,length);
      
      //---------------------------
      // copy the new poles
      // but rotate them with depth
      //---------------------------
      
      np = NewPoles.Lower();

      for (i = depth; i < length + depth; i++)
	GetPole(i,length,depth,Dimension,*poles,np,NewPoles);

      np = NewPoles.Upper() - depth*Dimension + 1;

      for (i = 0; i < depth; i++)
	GetPole(i,length,depth,Dimension,*poles,np,NewPoles);
      
      NewMults(NewMults.Lower()) += depth;
      NewMults(NewMults.Upper()) += depth;
    }
  }
  // free local arrays
  delete [] knots;
  delete [] poles;
}

//=======================================================================
//function : RemoveKnot
//purpose  : 
//=======================================================================

Standard_Boolean BSplCLib::RemoveKnot 
(const Standard_Integer         Index,       
 const Standard_Integer         Mult,        
 const Standard_Integer         Degree,  
 const Standard_Boolean         Periodic,
 const Standard_Integer         Dimension,  
 const TColStd_Array1OfReal&    Poles,
 const TColStd_Array1OfReal&    Knots,  
 const TColStd_Array1OfInteger& Mults,
 TColStd_Array1OfReal&          NewPoles,
 TColStd_Array1OfReal&          NewKnots,  
 TColStd_Array1OfInteger&       NewMults,
 const Standard_Real            Tolerance)
{
  Standard_Integer index,i,j,k,p,np;

  Standard_Integer TheIndex = Index;

  // protection
  Standard_Integer first,last;
  if (Periodic) {
    first = Knots.Lower();
    last  = Knots.Upper();
  }
  else {
    first = BSplCLib::FirstUKnotIndex(Degree,Mults) + 1;
    last  = BSplCLib::LastUKnotIndex(Degree,Mults) - 1;
  }
  if (Index < first) return Standard_False;
  if (Index > last)  return Standard_False;

  if ( Periodic && (Index == first)) TheIndex = last;

  Standard_Integer depth  = Mults(TheIndex) - Mult;
  Standard_Integer length = Degree - Mult;

  // -------------------
  // create local arrays
  // -------------------

  Standard_Real *knots = new Standard_Real[4*Degree];
  Standard_Real *poles = new Standard_Real[(2*Degree+1)*Dimension];
  

  // ------------------------------------
  // build the knots for anti Boor Scheme
  // ------------------------------------

  // the new sequence of knots
  // is obtained from the knots at Index-1 and Index
  
  BSplCLib::BuildKnots(Degree,TheIndex-1,Periodic,Knots,&Mults,*knots);
  index = PoleIndex(Degree,TheIndex-1,Periodic,Mults);
  BSplCLib::BuildKnots(Degree,TheIndex,Periodic,Knots,&Mults,knots[2*Degree]);

  index += Mult;

  for (i = 0; i < Degree - Mult; i++)
    knots[i] = knots[i+Mult];

  for (i = Degree-Mult; i < 2*Degree; i++)
    knots[i] = knots[2*Degree+i];


  // ------------------------------------
  // build the poles for anti Boor Scheme
  // ------------------------------------

  p = Poles.Lower()+index * Dimension;

  for (i = 0; i <= length + depth; i++) {
    j = Dimension * BoorIndex(i,length,depth);

    for (k = 0; k < Dimension; k++) {
      poles[j+k] = Poles(p+k);
    }
    p += Dimension;
    if (p > Poles.Upper()) p = Poles.Lower();
  }


  // ----------------
  // Anti Boor Scheme
  // ----------------

  Standard_Boolean result = AntiBoorScheme(Knots(TheIndex),Degree,*knots,
					   Dimension,*poles,
					   depth,length,Tolerance);
  
  // ----------------
  // copy the results
  // ----------------

  if (result) {

    // poles

    p = Poles.Lower();
    np = NewPoles.Lower();
    
    // unmodified poles before
    Copy((index+1)*Dimension,p,Poles,np,NewPoles);
    
    
    // modified

    for (i = 1; i <= length; i++)
      BSplCLib::GetPole(i,length,0,Dimension,*poles,np,NewPoles);
    p += (length + depth) * Dimension ;
    
    // unmodified poles after
    if (p != Poles.Lower()) {
      i = Poles.Upper() - p + 1;
      Copy(i,p,Poles,np,NewPoles);
    }

    // knots and mults

    if (Mult > 0) {
      NewKnots = Knots;
      NewMults = Mults;
      NewMults(TheIndex) = Mult;
      if (Periodic) {
	if (TheIndex == first) NewMults(last)  = Mult;
	if (TheIndex == last)  NewMults(first) = Mult;
      }
    }
    else {
      if (!Periodic || (TheIndex != first && TheIndex != last)) {

	for (i = Knots.Lower(); i < TheIndex; i++) {
	  NewKnots(i) = Knots(i);
	  NewMults(i) = Mults(i);
	}

	for (i = TheIndex+1; i <= Knots.Upper(); i++) {
	  NewKnots(i-1) = Knots(i);
	  NewMults(i-1) = Mults(i);
	}
      }
      else {
	// The interesting case of a Periodic curve 
	// where the first and last knot is removed.
	
	for (i = first; i < last-1; i++) {
	  NewKnots(i) = Knots(i+1);
	  NewMults(i) = Mults(i+1);
	}
	NewKnots(last-1) = NewKnots(first) + Knots(last) - Knots(first);
	NewMults(last-1) = NewMults(first);
      }
    }
  }


  // free local arrays
  delete [] knots;
  delete [] poles;
  
  return result;
}

//=======================================================================
//function : IncreaseDegreeCountKnots
//purpose  : 
//=======================================================================

Standard_Integer  BSplCLib::IncreaseDegreeCountKnots
(const Standard_Integer Degree,
 const Standard_Integer NewDegree, 
 const Standard_Boolean Periodic, 
 const TColStd_Array1OfInteger& Mults)
{
  if (Periodic) return Mults.Length();
  Standard_Integer f = FirstUKnotIndex(Degree,Mults);
  Standard_Integer l = LastUKnotIndex(Degree,Mults);
  Standard_Integer m,i,removed = 0, step = NewDegree - Degree;
  
  i = Mults.Lower();
  m = Degree + (f - i + 1) * step + 1;

  while (m > NewDegree+1) {
    removed++;
    m -= Mults(i) + step;
    i++;
  }
  if (m < NewDegree+1) removed--;

  i = Mults.Upper();
  m = Degree + (i - l + 1) * step + 1;

  while (m > NewDegree+1) {
    removed++;
    m -= Mults(i) + step;
    i--;
  }
  if (m < NewDegree+1) removed--;

  return Mults.Length() - removed;
}

//=======================================================================
//function : IncreaseDegree
//purpose  : 
//=======================================================================

void BSplCLib::IncreaseDegree 
(const Standard_Integer         Degree,
 const Standard_Integer         NewDegree,
 const Standard_Boolean         Periodic,
 const Standard_Integer         Dimension,
 const TColStd_Array1OfReal&    Poles,
 const TColStd_Array1OfReal&    Knots,
 const TColStd_Array1OfInteger& Mults,
 TColStd_Array1OfReal&          NewPoles,
 TColStd_Array1OfReal&          NewKnots,
 TColStd_Array1OfInteger&       NewMults)
{ 
  // Degree elevation of a BSpline Curve

  // This algorithms loops on degree incrementation from Degree to NewDegree.
  // The variable curDeg is the current degree to increment.

  // Before degree incrementations a "working curve" is created.
  // It has the same knot, poles and multiplicities.

  // If the curve is periodic knots are added on the working curve before
  // and after the existing knots to make it a non-periodic curves. 
  // The poles are also copied.

  // The first and last multiplicity of the working curve are set to Degree+1,
  // null poles are  added if necessary.

  // Then the degree is incremented on the working curve.
  // The knots are unchanged but all multiplicities will be incremented.

  // Each degree incrementation is achieved by averaging curDeg+1 curves.

  // See : Degree elevation of B-spline curves
  //       Hartmut PRAUTZSCH
  //       CAGD 1 (1984)


  //-------------------------
  // create the working curve
  //-------------------------

  Standard_Integer i,k,f,l,m,pf,pl,firstknot;

  pf = 0; // number of null poles added at beginning
  pl = 0; // number of null poles added at end

  Standard_Integer nbwknots = Knots.Length();
  f = FirstUKnotIndex(Degree,Mults);
  l = LastUKnotIndex (Degree,Mults);

  if (Periodic) {
    // Periodic curves are transformed in non-periodic curves

    nbwknots += f - Mults.Lower();

    pf = -Degree - 1;

    for (i = Mults.Lower(); i <= f; i++)
      pf += Mults(i);

    nbwknots += Mults.Upper() - l;

    pl = -Degree - 1;

    for (i = l; i <= Mults.Upper(); i++)
      pl += Mults(i);
  }

  // copy the knots and multiplicities 
  TColStd_Array1OfReal    wknots(1,nbwknots);
  TColStd_Array1OfInteger wmults(1,nbwknots);
  if (!Periodic) {
    wknots  = Knots;
    wmults  = Mults;
  }
  else {
    // copy the knots for a periodic curve
    Standard_Real period = Knots(Knots.Upper()) - Knots(Knots.Lower());
    i = 0;

    for (k = l; k < Knots.Upper(); k++) {
      i++; 
      wknots(i) = Knots(k) - period;
      wmults(i) = Mults(k);
    }

    for (k = Knots.Lower(); k <= Knots.Upper(); k++) {
      i++; 
      wknots(i) = Knots(k);
      wmults(i) = Mults(k);
    }

    for (k = Knots.Lower()+1; k <= f; k++) {
      i++; 
      wknots(i) = Knots(k) + period;
      wmults(i) = Mults(k);
    }
  }

  // set the first and last mults to Degree+1
  // and add null poles

  pf += Degree + 1 - wmults(1);
  wmults(1) = Degree + 1;
  pl += Degree + 1 - wmults(nbwknots);
  wmults(nbwknots) = Degree + 1;

  //---------------------------
  // poles of the working curve
  //---------------------------

  Standard_Integer nbwpoles = 0;

  for (i = 1; i <= nbwknots; i++) nbwpoles += wmults(i);
  nbwpoles -= Degree + 1;

  // we provide space for degree elevation
  TColStd_Array1OfReal 
    wpoles(1,(nbwpoles + (nbwknots-1) * (NewDegree - Degree)) * Dimension);

  for (i = 1; i <= pf * Dimension; i++) 
    wpoles(i) = 0;

  k = Poles.Lower();

  for (i = pf * Dimension + 1; i <= (nbwpoles - pl) * Dimension; i++) {
    wpoles(i) = Poles(k);
    k++;
    if (k > Poles.Upper()) k = Poles.Lower();
  }

  for (i = (nbwpoles-pl)*Dimension+1; i <= nbwpoles*Dimension; i++)
    wpoles(i) = 0;
  
  
  //-----------------------------------------------------------
  // Declare the temporary arrays used in degree incrementation
  //-----------------------------------------------------------

  Standard_Integer nbwp = nbwpoles + (nbwknots-1) * (NewDegree - Degree);
  // Arrays for storing the temporary curves
  TColStd_Array1OfReal tempc1(1,nbwp * Dimension);
  TColStd_Array1OfReal tempc2(1,nbwp * Dimension);

  // Array for storing the knots to insert
  TColStd_Array1OfReal iknots(1,nbwknots);

  // Arrays for receiving the knots after insertion
  TColStd_Array1OfReal    nknots(1,nbwknots);


  
  //------------------------------
  // Loop on degree incrementation
  //------------------------------

  Standard_Integer step,curDeg;
  Standard_Integer nbp = nbwpoles;
  nbwp = nbp;

  for (curDeg = Degree; curDeg < NewDegree; curDeg++) {
    
    nbp  = nbwp;               // current number of poles
    nbwp = nbp + nbwknots - 1; // new number of poles

    // For the averaging
    TColStd_Array1OfReal nwpoles(1,nbwp * Dimension);
    nwpoles.Init(0.0e0) ;
  
    
    for (step = 0; step <= curDeg; step++) {
    
      // Compute the bspline of rank step.

      // if not the first time, decrement the multiplicities back
      if (step != 0) {
	for (i = 1; i <= nbwknots; i++)
	  wmults(i)--;
      }
    
      // Poles are the current poles 
      // but the poles congruent to step are duplicated.
      
      Standard_Integer offset = 0;

      for (i = 0; i < nbp; i++) {
	offset++;

	for (k = 0; k < Dimension; k++) {
	  tempc1((offset-1)*Dimension+k+1) = 
	    wpoles(NewPoles.Lower()+i*Dimension+k);
	}
	if (i % (curDeg+1) == step) {
	  offset++;

	  for (k = 0; k < Dimension; k++) {
	    tempc1((offset-1)*Dimension+k+1) = 
	      wpoles(NewPoles.Lower()+i*Dimension+k);
	  }
	}
      }
	
      // Knots multiplicities are increased
      // For each knot where the sum of multiplicities is congruent to step
      
      Standard_Integer stepmult = step+1;
      Standard_Integer nbknots = 0;
      Standard_Integer smult = 0;

      for (k = 1; k <= nbwknots; k++) {
	smult += wmults(k);
	if (smult  >= stepmult) {
	  // this knot is increased
	  stepmult += curDeg+1;
	  wmults(k)++;
	}
	else {
	  // this knot is inserted
	  nbknots++;
	  iknots(nbknots) = wknots(k);
	}
      }
      
      // the curve is obtained by inserting the knots
      // to raise the multiplicities

      // we build "slices" of the arrays to set the correct size
      if (nbknots > 0) {
	TColStd_Array1OfReal aknots(iknots(1),1,nbknots);
	TColStd_Array1OfReal curve (tempc1(1),1,offset * Dimension);
	TColStd_Array1OfReal ncurve(tempc2(1),1,nbwp   * Dimension);
//	InsertKnots(curDeg+1,Standard_False,Dimension,curve,wknots,wmults,
//		    aknots,NoMults(),ncurve,nknots,wmults,Epsilon(1.));

	InsertKnots(curDeg+1,Standard_False,Dimension,curve,wknots,wmults,
		    aknots,NoMults(),ncurve,nknots,wmults,0.0);
	
	// add to the average

	for (i = 1; i <= nbwp * Dimension; i++)
	  nwpoles(i) += ncurve(i);
      }
      else {
	// add to the average

	for (i = 1; i <= nbwp * Dimension; i++)
	  nwpoles(i) += tempc1(i);
      }
    }
    
    // The result is the average

    for (i = 1; i <= nbwp * Dimension; i++) {
      wpoles(i) = nwpoles(i) / (curDeg+1);
    }
  }
  
  //-----------------
  // Copy the results
  //-----------------

  // index in new knots of the first knot of the curve
  if (Periodic)
    firstknot = Mults.Upper() - l + 1;
  else 
    firstknot = f;
  
  // the new curve starts at index firstknot
  // so we must remove knots until the sum of multiplicities
  // from the first to the start is NewDegree+1

  // m is the current sum of multiplicities
  m = 0;

  for (k = 1; k <= firstknot; k++)
    m += wmults(k);

  // compute the new first knot (k), pf will be the index of the first pole
  k = 1;
  pf = 0;

  while (m > NewDegree+1) {
    k++;
    m  -= wmults(k);
    pf += wmults(k);
  }
  if (m < NewDegree+1) {
    k--;
    wmults(k) += m - NewDegree - 1;
    pf        += m - NewDegree - 1;
  }

  // on a periodic curve the knots start with firstknot
  if (Periodic)
    k = firstknot;

  // copy knots

  for (i = NewKnots.Lower(); i <= NewKnots.Upper(); i++) {
    NewKnots(i) = wknots(k);
    NewMults(i) = wmults(k);
    k++;
  }

  // copy poles
  pf *= Dimension;

  for (i = NewPoles.Lower(); i <= NewPoles.Upper(); i++) {
    pf++;
    NewPoles(i) = wpoles(pf);
  }
}

//=======================================================================
//function : PrepareUnperiodize
//purpose  : 
//=======================================================================

void  BSplCLib::PrepareUnperiodize
(const Standard_Integer         Degree, 
 const TColStd_Array1OfInteger& Mults, 
 Standard_Integer&        NbKnots, 
 Standard_Integer&        NbPoles)
{
  Standard_Integer i;
  // initialize NbKnots and NbPoles
  NbKnots = Mults.Length();
  NbPoles = - Degree - 1;

  for (i = Mults.Lower(); i <= Mults.Upper(); i++) 
    NbPoles += Mults(i);

  Standard_Integer sigma, k;
  // Add knots at the beginning of the curve to raise Multiplicities 
  // to Degre + 1;
  sigma = Mults(Mults.Lower());
  k = Mults.Upper() - 1;

  while ( sigma < Degree + 1) {
    sigma   += Mults(k);
    NbPoles += Mults(k);
    k--;
    NbKnots++;
  }
  // We must add exactly until Degree + 1 -> 
  //    Supress the excedent.
  if ( sigma > Degree + 1)
    NbPoles -= sigma - Degree - 1;

  // Add knots at the end of the curve to raise Multiplicities 
  // to Degre + 1;
  sigma = Mults(Mults.Upper());
  k = Mults.Lower() + 1;

  while ( sigma < Degree + 1) {
    sigma   += Mults(k);
    NbPoles += Mults(k);
    k++;
    NbKnots++;
  }
  // We must add exactly until Degree + 1 -> 
  //    Supress the excedent.
  if ( sigma > Degree + 1)
    NbPoles -= sigma - Degree - 1;
}

//=======================================================================
//function : Unperiodize
//purpose  : 
//=======================================================================

void  BSplCLib::Unperiodize
(const Standard_Integer         Degree,
 const Standard_Integer         , // Dimension,
 const TColStd_Array1OfInteger& Mults,
 const TColStd_Array1OfReal&    Knots,
 const TColStd_Array1OfReal&    Poles,
 TColStd_Array1OfInteger& NewMults,
 TColStd_Array1OfReal&    NewKnots,
 TColStd_Array1OfReal&    NewPoles)
{
  Standard_Integer sigma, k, index = 0;
  // evaluation of index : number of knots to insert before knot(1) to
  // raise sum of multiplicities to <Degree + 1>
  sigma = Mults(Mults.Lower());
  k = Mults.Upper() - 1;

  while ( sigma < Degree + 1) {
    sigma   += Mults(k);
    k--;
    index++;
  }

  Standard_Real    period = Knots(Knots.Upper()) - Knots(Knots.Lower());

  // set the 'interior' knots;

  for ( k = 1; k <= Knots.Length(); k++) {
    NewKnots ( k + index ) = Knots( k);
    NewMults ( k + index ) = Mults( k);
  }
  
  // set the 'starting' knots;

  for ( k = 1; k <= index; k++) {
    NewKnots( k) = NewKnots( k + Knots.Length() - 1) - period;
    NewMults( k) = NewMults( k + Knots.Length() - 1);
  }
  NewMults( 1) -= sigma - Degree -1;
  
  // set the 'ending' knots;
  sigma = NewMults( index + Knots.Length() );

  for ( k = Knots.Length() + index + 1; k <= NewKnots.Length(); k++) {
    NewKnots( k) = NewKnots( k - Knots.Length() + 1) + period;
    NewMults( k) = NewMults( k - Knots.Length() + 1);
    sigma += NewMults( k - Knots.Length() + 1);
  }
  NewMults(NewMults.Length()) -= sigma - Degree - 1;

  for ( k = 1 ; k <= NewPoles.Length(); k++) {
    NewPoles(k ) = Poles( (k-1) % Poles.Length() + 1);
  }
}

//=======================================================================
//function : PrepareTrimming
//purpose  : 
//=======================================================================

void BSplCLib::PrepareTrimming(const Standard_Integer         Degree,
			       const Standard_Boolean         Periodic,
			       const TColStd_Array1OfReal&    Knots,
			       const TColStd_Array1OfInteger& Mults,
			       const Standard_Real            U1,
			       const Standard_Real            U2,
			             Standard_Integer&        NbKnots,
			             Standard_Integer&        NbPoles)
{
  Standard_Integer i;
  Standard_Real NewU1, NewU2;
  Standard_Integer index1 = 0, index2 = 0;

  // Eval index1, index2 : position of U1 and U2 in the Array Knots
  // such as Knots(index1-1) <= U1 < Knots(index1)
  //         Knots(index2-1) <= U2 < Knots(index2)
  LocateParameter( Degree, Knots, Mults, U1, Periodic,
		   Knots.Lower(), Knots.Upper(), index1, NewU1);
  LocateParameter( Degree, Knots, Mults, U2, Periodic,
		   Knots.Lower(), Knots.Upper(), index2, NewU2);
  index1++;
  if ( Abs(Knots(index2) - U2) <= Epsilon( U1))
    index2--;

  // eval NbKnots:
  NbKnots = index2 - index1 + 3;

  // eval NbPoles:
  NbPoles = Degree + 1;

  for ( i = index1; i <= index2; i++) 
    NbPoles += Mults(i);
}

//=======================================================================
//function : Trimming
//purpose  : 
//=======================================================================

void BSplCLib::Trimming(const Standard_Integer         Degree,
			const Standard_Boolean         Periodic,
			const Standard_Integer         Dimension,
			const TColStd_Array1OfReal&    Knots,
			const TColStd_Array1OfInteger& Mults,
			const TColStd_Array1OfReal&    Poles,
			const Standard_Real            U1,
			const Standard_Real            U2,
			      TColStd_Array1OfReal&    NewKnots,
			      TColStd_Array1OfInteger& NewMults,
		  	      TColStd_Array1OfReal&    NewPoles)
{
  Standard_Integer i, nbpoles=0, nbknots=0;
  Standard_Real kk[2];
  Standard_Integer mm[2];
  TColStd_Array1OfReal    K( kk[0], 1, 2 );
  TColStd_Array1OfInteger M( mm[0], 1, 2 );

  K(1) = U1;  K(2) = U2;
  mm[0] = mm[1] = Degree;
  if (!PrepareInsertKnots( Degree, Periodic, Knots, Mults, K, &M, 
			  nbpoles, nbknots, Epsilon( U1), 0))
    throw Standard_OutOfRange();

  TColStd_Array1OfReal    TempPoles(1, nbpoles*Dimension);
  TColStd_Array1OfReal    TempKnots(1, nbknots);
  TColStd_Array1OfInteger TempMults(1, nbknots);

//
// do not allow the multiplicities to Add : they must be less than Degree
//
  InsertKnots(Degree, Periodic, Dimension, Poles, Knots, Mults,
	      K, &M, TempPoles, TempKnots, TempMults, Epsilon(U1),
	      Standard_False);

  // find in TempPoles the index of the pole corresponding to U1
  Standard_Integer Kindex = 0, Pindex;
  Standard_Real NewU1;
  LocateParameter( Degree, TempKnots, TempMults, U1, Periodic,
		   TempKnots.Lower(), TempKnots.Upper(), Kindex, NewU1);
  Pindex = PoleIndex ( Degree, Kindex, Periodic, TempMults);
  Pindex *= Dimension;

  for ( i = 1; i <= NewPoles.Length(); i++) NewPoles(i) = TempPoles(Pindex + i);

  for ( i = 1; i <= NewKnots.Length(); i++) {
    NewKnots(i) = TempKnots( Kindex+i-1);
    NewMults(i) = TempMults( Kindex+i-1);
  }
  NewMults(1) = Min(Degree, NewMults(1)) + 1 ;
  NewMults(NewMults.Length())= Min(Degree, NewMults(NewMults.Length())) + 1 ;
}

//=======================================================================
//function : Solves a LU factored Matrix 
//purpose  : 
//=======================================================================

Standard_Integer 
BSplCLib::SolveBandedSystem(const math_Matrix&  Matrix,
			    const Standard_Integer UpperBandWidth,
			    const Standard_Integer LowerBandWidth,
			    const Standard_Integer ArrayDimension,
			    Standard_Real&   Array) 
{
  Standard_Integer ii,
  jj,
  kk,
  MinIndex,
  MaxIndex,
  ReturnCode = 0 ;
  
  Standard_Real   *PolesArray = &Array ;
  Standard_Real   Inverse ;
  
  
  if (Matrix.LowerCol() != 1 || 
      Matrix.UpperCol() != UpperBandWidth + LowerBandWidth + 1) {
    ReturnCode = 1 ;
    goto FINISH ;
  }
  
  for (ii = Matrix.LowerRow() + 1; ii <=  Matrix.UpperRow() ; ii++) {
    MinIndex = (ii - LowerBandWidth >= Matrix.LowerRow() ?
                ii - LowerBandWidth : Matrix.LowerRow()) ;
    
    for ( jj = MinIndex  ; jj < ii  ; jj++) {
      
      for (kk = 0 ; kk < ArrayDimension ; kk++) {
	PolesArray[(ii-1) * ArrayDimension + kk] += 
	  PolesArray[(jj-1) * ArrayDimension + kk] * Matrix(ii, jj - ii + LowerBandWidth + 1) ;
      }
    }
  }
  
  for (ii = Matrix.UpperRow() ; ii >=  Matrix.LowerRow() ; ii--) {
    MaxIndex = (ii + UpperBandWidth <= Matrix.UpperRow() ? 
		ii + UpperBandWidth : Matrix.UpperRow()) ;
    
    for (jj = MaxIndex  ; jj > ii ; jj--) {
      
      for (kk = 0 ; kk < ArrayDimension ; kk++) {
	PolesArray[(ii-1)  * ArrayDimension + kk] -=
	  PolesArray[(jj - 1) * ArrayDimension + kk] * 
	    Matrix(ii, jj - ii + LowerBandWidth + 1) ;
      }
    }
    
    //fixing a bug PRO18577 to avoid divizion by zero
    
    Standard_Real divizor = Matrix(ii,LowerBandWidth + 1) ;
    Standard_Real Toler = 1.0e-16;
    if ( Abs(divizor) > Toler )
      Inverse = 1.0e0 / divizor ;
    else {
      Inverse = 1.0e0;
//      cout << "  BSplCLib::SolveBandedSystem() : zero determinant " << endl;
      ReturnCode = 1;
      goto FINISH;
    }
	
    for (kk = 0 ; kk < ArrayDimension ; kk++) {
      PolesArray[(ii-1)  * ArrayDimension + kk] *=  Inverse ; 
    }
  }
  FINISH :
    return (ReturnCode) ;
}

//=======================================================================
//function : Solves a LU factored Matrix 
//purpose  : 
//=======================================================================

Standard_Integer 
BSplCLib::SolveBandedSystem(const math_Matrix&  Matrix,
			    const Standard_Integer UpperBandWidth,
			    const Standard_Integer LowerBandWidth,
                            const Standard_Boolean HomogeneousFlag,
			    const Standard_Integer ArrayDimension,
			    Standard_Real&   Poles,
			    Standard_Real&   Weights) 
{
  Standard_Integer ii,
  kk,
  ErrorCode = 0,
  ReturnCode = 0 ;
  
  Standard_Real   Inverse,
  *PolesArray   = &Poles,
  *WeightsArray = &Weights ;
  
  if (Matrix.LowerCol() != 1 || 
      Matrix.UpperCol() != UpperBandWidth + LowerBandWidth + 1) {
    ReturnCode = 1 ;
    goto FINISH ;
  }
  if (HomogeneousFlag == Standard_False) {
    
    for (ii = 0 ; ii <  Matrix.UpperRow() - Matrix.LowerRow() + 1; ii++) {
      
      for (kk = 0 ; kk < ArrayDimension ; kk++) {
	PolesArray[ii * ArrayDimension + kk] *=
	  WeightsArray[ii] ;
      }
    }
  }
  ErrorCode = 
    BSplCLib::SolveBandedSystem(Matrix,
				UpperBandWidth,
				LowerBandWidth,
				ArrayDimension,
				Poles) ;
  if (ErrorCode != 0) {
    ReturnCode = 2 ;
    goto FINISH ;
  }
  ErrorCode = 
    BSplCLib::SolveBandedSystem(Matrix,
				UpperBandWidth,
				LowerBandWidth,
				1,
				Weights) ;
  if (ErrorCode != 0) {
    ReturnCode = 3 ;
    goto FINISH ;
  }
  if (HomogeneousFlag == Standard_False) {

    for (ii = 0  ; ii < Matrix.UpperRow() - Matrix.LowerRow() + 1 ; ii++) {
      Inverse = 1.0e0 / WeightsArray[ii] ;
      
      for (kk = 0  ; kk < ArrayDimension ; kk++) {
	PolesArray[ii * ArrayDimension + kk] *= Inverse ;
      }
    }
  }
  FINISH : return (ReturnCode) ;
}

//=======================================================================
//function : BuildSchoenbergPoints
//purpose  : 
//=======================================================================

void  BSplCLib::BuildSchoenbergPoints(const Standard_Integer         Degree,
				      const TColStd_Array1OfReal&    FlatKnots,
				      TColStd_Array1OfReal&          Parameters) 
{
  Standard_Integer ii,
  jj ;
  Standard_Real Inverse ;
  Inverse = 1.0e0 / (Standard_Real)Degree ;
  
  for (ii = Parameters.Lower() ;   ii <= Parameters.Upper() ; ii++) {
    Parameters(ii) = 0.0e0 ;
    
    for (jj = 1 ; jj <= Degree ; jj++) {
      Parameters(ii) += FlatKnots(jj + ii) ;
    } 
    Parameters(ii) *= Inverse ; 
  }
}

//=======================================================================
//function : Interpolate
//purpose  : 
//=======================================================================

void  BSplCLib::Interpolate(const Standard_Integer         Degree,
			    const TColStd_Array1OfReal&    FlatKnots,
			    const TColStd_Array1OfReal&    Parameters,
			    const TColStd_Array1OfInteger& ContactOrderArray,
			    const Standard_Integer         ArrayDimension,
			    Standard_Real&                 Poles,
			    Standard_Integer&              InversionProblem) 
{
  Standard_Integer ErrorCode,
  UpperBandWidth,
  LowerBandWidth ;
//  Standard_Real *PolesArray = &Poles ;
  math_Matrix InterpolationMatrix(1, Parameters.Length(),
				  1, 2 * Degree + 1) ;
  ErrorCode =
  BSplCLib::BuildBSpMatrix(Parameters,
                           ContactOrderArray,
                           FlatKnots,
                           Degree,
                           InterpolationMatrix,
                           UpperBandWidth,
                           LowerBandWidth) ;
  if(ErrorCode)
    throw Standard_OutOfRange("BSplCLib::Interpolate");

  ErrorCode =
  BSplCLib::FactorBandedMatrix(InterpolationMatrix,
                           UpperBandWidth,
                           LowerBandWidth,
                           InversionProblem) ;
  if(ErrorCode)
    throw Standard_OutOfRange("BSplCLib::Interpolate");

  ErrorCode  =
  BSplCLib::SolveBandedSystem(InterpolationMatrix,
                              UpperBandWidth,
                              LowerBandWidth,
			      ArrayDimension,
                              Poles) ;
  if(ErrorCode)
    throw Standard_OutOfRange("BSplCLib::Interpolate");
}

//=======================================================================
//function : Interpolate
//purpose  : 
//=======================================================================

void  BSplCLib::Interpolate(const Standard_Integer         Degree,
			    const TColStd_Array1OfReal&    FlatKnots,
			    const TColStd_Array1OfReal&    Parameters,
			    const TColStd_Array1OfInteger& ContactOrderArray,
			    const Standard_Integer         ArrayDimension,
			    Standard_Real&                 Poles,
			    Standard_Real&                 Weights,
			    Standard_Integer&              InversionProblem) 
{
  Standard_Integer ErrorCode,
  UpperBandWidth,
  LowerBandWidth ;

  math_Matrix InterpolationMatrix(1, Parameters.Length(),
				  1, 2 * Degree + 1) ;
  ErrorCode =
  BSplCLib::BuildBSpMatrix(Parameters,
                           ContactOrderArray,
                           FlatKnots,
                           Degree,
                           InterpolationMatrix,
                           UpperBandWidth,
                           LowerBandWidth) ;
  if(ErrorCode)
    throw Standard_OutOfRange("BSplCLib::Interpolate");

  ErrorCode =
  BSplCLib::FactorBandedMatrix(InterpolationMatrix,
                           UpperBandWidth,
                           LowerBandWidth,
                           InversionProblem) ;
  if(ErrorCode)
    throw Standard_OutOfRange("BSplCLib::Interpolate");

  ErrorCode  =
  BSplCLib::SolveBandedSystem(InterpolationMatrix,
                              UpperBandWidth,
                              LowerBandWidth,
			      Standard_False,
			      ArrayDimension,
                              Poles,
			      Weights) ;
  if(ErrorCode)
    throw Standard_OutOfRange("BSplCLib::Interpolate");
}

//=======================================================================
//function : Evaluates a Bspline function : uses the ExtrapMode 
//purpose  : the function is extrapolated using the Taylor expansion
//           of degree ExtrapMode[0] to the left and the Taylor
//           expansion of degree ExtrapMode[1] to the right 
//  this evaluates the numerator by multiplying by the weights
//  and evaluating it but does not call RationalDerivatives after 
//=======================================================================

void  BSplCLib::Eval
(const Standard_Real                   Parameter,
 const Standard_Boolean                PeriodicFlag,
 const Standard_Integer                DerivativeRequest,
 Standard_Integer&                     ExtrapMode,
 const Standard_Integer                Degree,
 const  TColStd_Array1OfReal&          FlatKnots, 
 const Standard_Integer                ArrayDimension,
 Standard_Real&                        Poles,
 Standard_Real&                        Weights,
 Standard_Real&                        PolesResults,
 Standard_Real&                        WeightsResults)
{
  Standard_Integer ii,
  jj,
  kk=0,
  Index,
  Index1,
  Index2,
  *ExtrapModeArray,
  Modulus,
  NewRequest,
  ExtrapolatingFlag[2],
  ErrorCode,
  Order = Degree + 1,
  FirstNonZeroBsplineIndex,
  LocalRequest = DerivativeRequest ;
  Standard_Real  *PResultArray,
  *WResultArray,
  *PolesArray,
  *WeightsArray,
  LocalParameter,
  Period,
  Inverse,
  Delta ;
  PolesArray = &Poles     ;
  WeightsArray = &Weights ;
  ExtrapModeArray = &ExtrapMode ;
  PResultArray = &PolesResults ;
  WResultArray = &WeightsResults ;
  LocalParameter = Parameter ;
  ExtrapolatingFlag[0] = 
    ExtrapolatingFlag[1] = 0 ;
  //
  // check if we are extrapolating to a degree which is smaller than
  // the degree of the Bspline
  //
  if (PeriodicFlag) {
    Period = FlatKnots(FlatKnots.Upper() - 1) - FlatKnots(2) ;

    while (LocalParameter > FlatKnots(FlatKnots.Upper() - 1)) {
      LocalParameter -= Period ;
    }
    
    while (LocalParameter < FlatKnots(2)) {
      LocalParameter +=  Period ;
    }
  }
  if (Parameter < FlatKnots(2) && 
      LocalRequest < ExtrapModeArray[0] &&
      ExtrapModeArray[0] < Degree) {
    LocalRequest = ExtrapModeArray[0] ;
    LocalParameter = FlatKnots(2) ;
    ExtrapolatingFlag[0] = 1 ;
  }
  if (Parameter > FlatKnots(FlatKnots.Upper()-1) &&
      LocalRequest < ExtrapModeArray[1]  &&
      ExtrapModeArray[1] < Degree) {
    LocalRequest = ExtrapModeArray[1] ;
    LocalParameter = FlatKnots(FlatKnots.Upper()-1) ;
    ExtrapolatingFlag[1] = 1 ;
  }
  Delta = Parameter - LocalParameter ;
  if (LocalRequest >= Order) {
    LocalRequest = Degree ;
  }
  if (PeriodicFlag) {
    Modulus = FlatKnots.Length() - Degree -1 ;
  }
  else {
    Modulus = FlatKnots.Length() - Degree ;
  }

  BSplCLib_LocalMatrix BsplineBasis (LocalRequest, Order);
  ErrorCode =
    BSplCLib::EvalBsplineBasis(LocalRequest,
			       Order,
			       FlatKnots,
			       LocalParameter,
			       FirstNonZeroBsplineIndex,
			       BsplineBasis) ;
  if (ErrorCode != 0) {
    goto FINISH ;
  }
  if (ExtrapolatingFlag[0] == 0 && ExtrapolatingFlag[1] == 0) {
    Index = 0 ;
    Index2 = 0 ;

    for (ii = 1 ; ii <= LocalRequest + 1 ; ii++) {
      Index1 = FirstNonZeroBsplineIndex ;

      for (kk = 0 ; kk < ArrayDimension ; kk++) {
	PResultArray[Index + kk] = 0.0e0 ;
      }
      WResultArray[Index] = 0.0e0 ;

      for (jj = 1  ; jj <= Order ; jj++) {
	
	for (kk = 0 ; kk < ArrayDimension ; kk++) {
	  PResultArray[Index + kk] += 
	    PolesArray[(Index1-1) * ArrayDimension + kk] 
	      * WeightsArray[Index1-1] * BsplineBasis(ii,jj) ;
	}
	WResultArray[Index2]  += WeightsArray[Index1-1] * BsplineBasis(ii,jj) ;
	
	Index1 = Index1 % Modulus ;
	Index1 += 1 ;
      }
      Index += ArrayDimension ;
      Index2 += 1 ;
    }
  }
  else {
    // 
    //  store Taylor expansion in LocalRealArray
    //
    NewRequest = DerivativeRequest ;
    if (NewRequest > Degree) {
      NewRequest = Degree ;
    }
    NCollection_LocalArray<Standard_Real> LocalRealArray((LocalRequest + 1)*ArrayDimension);
    Index = 0 ;
    Inverse = 1.0e0 ;

    for (ii = 1 ; ii <= LocalRequest + 1 ; ii++) {
      Index1 = FirstNonZeroBsplineIndex ;
      
      for (kk = 0 ; kk < ArrayDimension ; kk++) {
	LocalRealArray[Index + kk] = 0.0e0 ;
      }

      for (jj = 1  ; jj <= Order ; jj++) {

	for (kk = 0 ; kk < ArrayDimension ; kk++) {
	  LocalRealArray[Index + kk] += 
	    PolesArray[(Index1-1)*ArrayDimension + kk] * 
	      WeightsArray[Index1-1] * BsplineBasis(ii,jj) ;
	}
	Index1 = Index1 % Modulus ;
	Index1 += 1 ;
      }

      for (kk = 0 ; kk < ArrayDimension ; kk++) {
	LocalRealArray[Index + kk] *= Inverse ;
      }
      Index += ArrayDimension ;
      Inverse /= (Standard_Real) ii ;
    }
    PLib::EvalPolynomial(Delta,
			 NewRequest,
			 Degree,
			 ArrayDimension,
			 LocalRealArray[0],
			 PolesResults) ;
    Index = 0 ;
    Inverse = 1.0e0 ;

    for (ii = 1 ; ii <= LocalRequest + 1 ; ii++) {
      Index1 = FirstNonZeroBsplineIndex ;
      LocalRealArray[Index] = 0.0e0 ;

      for (jj = 1  ; jj <= Order ; jj++) {
	LocalRealArray[Index] += 
	  WeightsArray[Index1-1] * BsplineBasis(ii,jj) ;
	Index1 = Index1 % Modulus ;
	Index1 += 1 ;
      }
      LocalRealArray[Index + kk] *= Inverse ;
      Index += 1 ;
      Inverse /= (Standard_Real) ii ;
    }
    PLib::EvalPolynomial(Delta,
			 NewRequest,
			 Degree,
			 1,
			 LocalRealArray[0],
			 WeightsResults) ;
  }
  FINISH : ;
}

//=======================================================================
//function : Evaluates a Bspline function : uses the ExtrapMode 
//purpose  : the function is extrapolated using the Taylor expansion
//           of degree ExtrapMode[0] to the left and the Taylor
//           expansion of degree ExtrapMode[1] to the right 
// WARNING : the array Results is supposed to have at least 
// (DerivativeRequest + 1) * ArrayDimension slots and the 
// 
//=======================================================================

void  BSplCLib::Eval
(const Standard_Real                   Parameter,
 const Standard_Boolean                PeriodicFlag,
 const Standard_Integer                DerivativeRequest,
 Standard_Integer&                     ExtrapMode,
 const Standard_Integer                Degree,
 const  TColStd_Array1OfReal&          FlatKnots, 
 const Standard_Integer                ArrayDimension,
 Standard_Real&                        Poles,
 Standard_Real&                        Results) 
{
  Standard_Integer ii,
  jj,
  kk,
  Index,
  Index1,
  *ExtrapModeArray,
  Modulus,
  NewRequest,
  ExtrapolatingFlag[2],
  ErrorCode,
  Order = Degree + 1,
  FirstNonZeroBsplineIndex,
  LocalRequest = DerivativeRequest ;

  Standard_Real  *ResultArray,
  *PolesArray,
  LocalParameter,
  Period,
  Inverse,
  Delta ;
         
  PolesArray = &Poles ;
  ExtrapModeArray = &ExtrapMode ;
  ResultArray = &Results ;  
  LocalParameter = Parameter ;
  ExtrapolatingFlag[0] = 
    ExtrapolatingFlag[1] = 0 ;
  //
  // check if we are extrapolating to a degree which is smaller than
  // the degree of the Bspline
  //
  if (PeriodicFlag) {
    Period = FlatKnots(FlatKnots.Upper() - 1) - FlatKnots(2) ;

    while (LocalParameter > FlatKnots(FlatKnots.Upper() - 1)) {
      LocalParameter -= Period ;
    }

    while (LocalParameter < FlatKnots(2)) {
      LocalParameter +=  Period ;
    }
  }
  if (Parameter < FlatKnots(2) && 
      LocalRequest < ExtrapModeArray[0] &&
      ExtrapModeArray[0] < Degree) {
    LocalRequest = ExtrapModeArray[0] ;
    LocalParameter = FlatKnots(2) ;
    ExtrapolatingFlag[0] = 1 ;
  }
  if (Parameter > FlatKnots(FlatKnots.Upper()-1) &&
      LocalRequest < ExtrapModeArray[1]  &&
      ExtrapModeArray[1] < Degree) {
    LocalRequest = ExtrapModeArray[1] ;
    LocalParameter = FlatKnots(FlatKnots.Upper()-1) ;
    ExtrapolatingFlag[1] = 1 ;
  }
  Delta = Parameter - LocalParameter ;
  if (LocalRequest >= Order) {
    LocalRequest = Degree ;
  }
  
  if (PeriodicFlag) {
    Modulus = FlatKnots.Length() - Degree -1 ;
  }
  else {
    Modulus = FlatKnots.Length() - Degree ;
  }
  
  BSplCLib_LocalMatrix BsplineBasis (LocalRequest, Order);
  
  ErrorCode =
    BSplCLib::EvalBsplineBasis(LocalRequest,
			       Order,
			       FlatKnots,
			       LocalParameter,
			       FirstNonZeroBsplineIndex,
			       BsplineBasis);
  if (ErrorCode != 0) {
    goto FINISH ;
  }
  if (ExtrapolatingFlag[0] == 0 && ExtrapolatingFlag[1] == 0) {
    Index = 0 ;
    
    for (ii = 1 ; ii <= LocalRequest + 1 ; ii++) {
      Index1 = FirstNonZeroBsplineIndex ;
      
      for (kk = 0 ; kk < ArrayDimension ; kk++) {
	ResultArray[Index + kk] = 0.0e0 ;
      }

      for (jj = 1  ; jj <= Order ; jj++) {
	
	for (kk = 0 ; kk < ArrayDimension ; kk++) {
	  ResultArray[Index + kk] += 
	    PolesArray[(Index1-1) * ArrayDimension + kk] * BsplineBasis(ii,jj) ;
	}
	Index1 = Index1 % Modulus ;
	Index1 += 1 ;
      }
      Index += ArrayDimension ;
    }
  }
  else {
    // 
    //  store Taylor expansion in LocalRealArray
    //
    NewRequest = DerivativeRequest ;
    if (NewRequest > Degree) {
      NewRequest = Degree ;
    }
    NCollection_LocalArray<Standard_Real> LocalRealArray((LocalRequest + 1)*ArrayDimension);

    Index = 0 ;
    Inverse = 1.0e0 ;

    for (ii = 1 ; ii <= LocalRequest + 1 ; ii++) {
      Index1 = FirstNonZeroBsplineIndex ;
      
      for (kk = 0 ; kk < ArrayDimension ; kk++) {
	LocalRealArray[Index + kk] = 0.0e0 ;
      }

      for (jj = 1  ; jj <= Order ; jj++) {

	for (kk = 0 ; kk < ArrayDimension ; kk++) {
	  LocalRealArray[Index + kk] += 
	    PolesArray[(Index1-1)*ArrayDimension + kk] * BsplineBasis(ii,jj) ;
	}
	Index1 = Index1 % Modulus ;
	Index1 += 1 ;
      }

      for (kk = 0 ; kk < ArrayDimension ; kk++) {
	LocalRealArray[Index + kk] *= Inverse ;
      }
      Index += ArrayDimension ;
      Inverse /= (Standard_Real) ii ;
    }
    PLib::EvalPolynomial(Delta,
			 NewRequest,
			 Degree,
			 ArrayDimension,
			 LocalRealArray[0],
			 Results) ;
  }
  FINISH : ;
}

//=======================================================================
//function : TangExtendToConstraint 
//purpose  : Extends a Bspline function using the tangency map
// WARNING :  
//  
// 
//=======================================================================

void  BSplCLib::TangExtendToConstraint
(const  TColStd_Array1OfReal&          FlatKnots, 
 const Standard_Real                   C1Coefficient,
 const Standard_Integer                NumPoles,
 Standard_Real&                        Poles,
 const Standard_Integer                CDimension,
 const Standard_Integer                CDegree,
 const  TColStd_Array1OfReal&          ConstraintPoint, 
 const Standard_Integer                Continuity,
 const Standard_Boolean                After,
 Standard_Integer&                     NbPolesResult,
 Standard_Integer&                     NbKnotsResult,
 Standard_Real&                        KnotsResult, 
 Standard_Real&                        PolesResult) 
{
#ifdef OCCT_DEBUG
  if (CDegree<Continuity+1) {
    cout<<"The BSpline degree must be greater than the order of continuity"<<endl;
  }
#endif
  Standard_Real * Padr = &Poles ;
  Standard_Real * KRadr = &KnotsResult ;
  Standard_Real * PRadr = &PolesResult ;

////////////////////////////////////////////////////////////////////////
//
//    1. calculation of extension nD
//
////////////////////////////////////////////////////////////////////////

//  Hermite matrix
  Standard_Integer Csize = Continuity + 2;
  math_Matrix  MatCoefs(1,Csize, 1,Csize);
  if (After) {
    PLib::HermiteCoefficients(0, 1,           // Limits 
			      Continuity, 0,  // Orders of constraints
			      MatCoefs);
  }
  else {
    PLib::HermiteCoefficients(0, 1,           // Limits 
			      0, Continuity,  // Orders of constraints
			      MatCoefs);    
  }


//  position at the node of connection
  Standard_Real Tbord ;
  if (After) {
    Tbord = FlatKnots(FlatKnots.Upper()-CDegree);
  }
  else {
    Tbord = FlatKnots(FlatKnots.Lower()+CDegree);
  }
  Standard_Boolean periodic_flag = Standard_False ;
  Standard_Integer ipos, extrap_mode[2], derivative_request = Max(Continuity,1);
  extrap_mode[0] = extrap_mode[1] = CDegree;
  TColStd_Array1OfReal  EvalBS(1, CDimension * (derivative_request+1)) ; 
  Standard_Real * Eadr = (Standard_Real *) &EvalBS(1) ;
  BSplCLib::Eval(Tbord,periodic_flag,derivative_request,extrap_mode[0],
                  CDegree,FlatKnots,CDimension,Poles,*Eadr);

//  norm of the tangent at the node of connection
  math_Vector Tgte(1,CDimension);

  for (ipos=1;ipos<=CDimension;ipos++) {
    Tgte(ipos) = EvalBS(ipos+CDimension);
  }
  Standard_Real L1=Tgte.Norm();


//  matrix of constraints
  math_Matrix Contraintes(1,Csize,1,CDimension);
  if (After) {

    for (ipos=1;ipos<=CDimension;ipos++) {
      Contraintes(1,ipos) = EvalBS(ipos);
      Contraintes(2,ipos) = C1Coefficient * EvalBS(ipos+CDimension);
      if(Continuity >= 2) Contraintes(3,ipos) = EvalBS(ipos+2*CDimension) * Pow(C1Coefficient,2);
      if(Continuity >= 3) Contraintes(4,ipos) = EvalBS(ipos+3*CDimension) * Pow(C1Coefficient,3);
      Contraintes(Continuity+2,ipos) = ConstraintPoint(ipos);
    }
  }
  else {

    for (ipos=1;ipos<=CDimension;ipos++) {
      Contraintes(1,ipos) = ConstraintPoint(ipos);
      Contraintes(2,ipos) = EvalBS(ipos);
      if(Continuity >= 1) Contraintes(3,ipos) = C1Coefficient * EvalBS(ipos+CDimension);
      if(Continuity >= 2) Contraintes(4,ipos) = EvalBS(ipos+2*CDimension) * Pow(C1Coefficient,2);
      if(Continuity >= 3) Contraintes(5,ipos) = EvalBS(ipos+3*CDimension) * Pow(C1Coefficient,3);
    }
  }

//  calculate the coefficients of extension
  Standard_Integer ii, jj, kk;
  TColStd_Array1OfReal ExtraCoeffs(1,Csize*CDimension);
  ExtraCoeffs.Init(0.);

  for (ii=1; ii<=Csize; ii++) {

    for (jj=1; jj<=Csize; jj++) {

      for (kk=1; kk<=CDimension; kk++) {
        ExtraCoeffs(kk+(jj-1)*CDimension) += MatCoefs(ii,jj)*Contraintes(ii,kk);
      }
    }
  }

//  calculate the poles of extension
  TColStd_Array1OfReal ExtrapPoles(1,Csize*CDimension);
  Standard_Real * EPadr = &ExtrapPoles(1) ;
  PLib::CoefficientsPoles(CDimension,
                          ExtraCoeffs, PLib::NoWeights(),
                          ExtrapPoles, PLib::NoWeights());

//  calculate the nodes of extension with multiplicities
  TColStd_Array1OfReal ExtrapNoeuds(1,2);
  ExtrapNoeuds(1) = 0.;
  ExtrapNoeuds(2) = 1.;
  TColStd_Array1OfInteger ExtrapMults(1,2);
  ExtrapMults(1) = Csize;
  ExtrapMults(2) = Csize;

// flat nodes of extension
  TColStd_Array1OfReal FK2(1, Csize*2);
  BSplCLib::KnotSequence(ExtrapNoeuds,ExtrapMults,FK2);

//  norm of the tangent at the connection point 
  if (After) {
    BSplCLib::Eval(0.,periodic_flag,1,extrap_mode[0],
                  Csize-1,FK2,CDimension,*EPadr,*Eadr);
  }
  else {
    BSplCLib::Eval(1.,periodic_flag,1,extrap_mode[0],
                  Csize-1,FK2,CDimension,*EPadr,*Eadr);
  }

  for (ipos=1;ipos<=CDimension;ipos++) {
    Tgte(ipos) = EvalBS(ipos+CDimension);
  }
  Standard_Real L2 = Tgte.Norm();

//  harmonisation of degrees
  TColStd_Array1OfReal NewP2(1, (CDegree+1)*CDimension);
  TColStd_Array1OfReal NewK2(1, 2);
  TColStd_Array1OfInteger NewM2(1, 2);
  if (Csize-1<CDegree) {
    BSplCLib::IncreaseDegree(Csize-1,CDegree,Standard_False,CDimension,
                             ExtrapPoles,ExtrapNoeuds,ExtrapMults,
                             NewP2,NewK2,NewM2);
  }
  else {
    NewP2 = ExtrapPoles;
    NewK2 = ExtrapNoeuds;
    NewM2 = ExtrapMults;
  }

//  flat nodes of extension after harmonization of degrees
  TColStd_Array1OfReal NewFK2(1, (CDegree+1)*2);
  BSplCLib::KnotSequence(NewK2,NewM2,NewFK2);


////////////////////////////////////////////////////////////////////////
//
//    2.  concatenation C0
//
////////////////////////////////////////////////////////////////////////

//  ratio of reparametrization
  Standard_Real Ratio=1, Delta;
  if ( (L1 > Precision::Confusion()) && (L2 > Precision::Confusion()) ) {
    Ratio = L2 / L1;
  }
  if ( (Ratio < 1.e-5) || (Ratio > 1.e5) ) Ratio = 1;

  if (After) {
//    do not touch the first BSpline
    Delta = Ratio*NewFK2(NewFK2.Lower()) - FlatKnots(FlatKnots.Upper());
  }
  else {
//    do not touch the second BSpline
    Delta = Ratio*NewFK2(NewFK2.Upper()) - FlatKnots(FlatKnots.Lower());
  }

//  result of the concatenation
  Standard_Integer NbP1 = NumPoles, NbP2 = CDegree+1;
  Standard_Integer NbK1 = FlatKnots.Length(), NbK2 = 2*(CDegree+1);
  TColStd_Array1OfReal NewPoles (1, (NbP1+ NbP2-1)*CDimension);
  TColStd_Array1OfReal NewFlats (1, NbK1+NbK2-CDegree-2);

//  poles
  Standard_Integer indNP, indP, indEP;
  if (After) {

    for (ii=1;  ii<=NbP1+NbP2-1; ii++) {

      for (jj=1;  jj<=CDimension; jj++) {
	indNP = (ii-1)*CDimension+jj;
        indP = (ii-1)*CDimension+jj-1;
        indEP = (ii-NbP1)*CDimension+jj;
        if (ii<NbP1) NewPoles(indNP) =  Padr[indP];
        else NewPoles(indNP) = NewP2(indEP);
      }
    }
  }
  else {

    for (ii=1;  ii<=NbP1+NbP2-1; ii++) {

      for (jj=1;  jj<=CDimension; jj++) {
	indNP = (ii-1)*CDimension+jj;
        indEP = (ii-1)*CDimension+jj;
        indP = (ii-NbP2)*CDimension+jj-1;
        if (ii<NbP2) NewPoles(indNP) =  NewP2(indEP);
        else NewPoles(indNP) = Padr[indP];
      }
    }
  }

//  flat nodes 
  if (After) {
//    start with the nodes of the initial surface

    for (ii=1; ii<NbK1; ii++) {
      NewFlats(ii) = FlatKnots(FlatKnots.Lower()+ii-1);
    }
//    continue with the reparameterized nodes of the extension

    for (ii=1; ii<=NbK2-CDegree-1; ii++) {
      NewFlats(NbK1+ii-1) = Ratio*NewFK2(NewFK2.Lower()+ii+CDegree) - Delta;
    }
  }
  else {
//    start with the reparameterized nodes of the extension

    for (ii=1; ii<NbK2-CDegree; ii++) {
      NewFlats(ii) = Ratio*NewFK2(NewFK2.Lower()+ii-1) - Delta;
    }
//    continue with the nodes of the initial surface

    for (ii=2; ii<=NbK1; ii++) {
      NewFlats(NbK2+ii-CDegree-2) = FlatKnots(FlatKnots.Lower()+ii-1);
    }
  }


////////////////////////////////////////////////////////////////////////
//
//    3.  reduction of multiplicite at the node of connection
//
////////////////////////////////////////////////////////////////////////

//  number of separate nodes
  Standard_Integer KLength = 1;

  for (ii=2; ii<=NbK1+NbK2-CDegree-2;ii++) {
    if (NewFlats(ii) != NewFlats(ii-1)) KLength++;
  }

//  flat nodes --> nodes + multiplicities
  TColStd_Array1OfReal NewKnots (1, KLength);
  TColStd_Array1OfInteger NewMults (1, KLength);
  NewMults.Init(1);
  jj = 1;
  NewKnots(jj) = NewFlats(1);

  for (ii=2; ii<=NbK1+NbK2-CDegree-2;ii++) {
    if (NewFlats(ii) == NewFlats(ii-1)) NewMults(jj)++;
    else {
      jj++;
      NewKnots(jj) = NewFlats(ii);
    }
  }

//  reduction of multiplicity at the second or the last but one node
  Standard_Integer Index = 2, M = CDegree;
  if (After) Index = KLength-1;
  TColStd_Array1OfReal ResultPoles (1, (NbP1+ NbP2-1)*CDimension);
  TColStd_Array1OfReal ResultKnots (1, KLength);
  TColStd_Array1OfInteger ResultMults (1, KLength);
  Standard_Real Tol = 1.e-6;
  Standard_Boolean Ok = Standard_True;

  while ( (M>CDegree-Continuity) && Ok) {
    Ok = RemoveKnot(Index, M-1, CDegree, Standard_False, CDimension,
		    NewPoles, NewKnots, NewMults,
		    ResultPoles, ResultKnots, ResultMults, Tol);
    if (Ok) M--;
  }

  if (M == CDegree) {
//    number of poles of the concatenation
    NbPolesResult = NbP1 + NbP2 - 1;
//    the poles of the concatenation
    Standard_Integer PLength = NbPolesResult*CDimension;

    for (jj=1; jj<=PLength; jj++) {
      PRadr[jj-1] = NewPoles(jj);
    }
  
//    flat nodes of the concatenation
    Standard_Integer ideb = 0;

    for (jj=0; jj<NewKnots.Length(); jj++) {
      for (ii=0; ii<NewMults(jj+1); ii++) {
	KRadr[ideb+ii] = NewKnots(jj+1);
      }
      ideb += NewMults(jj+1);
    }
    NbKnotsResult = ideb;
  }

  else {
//    number of poles of the result
    NbPolesResult = NbP1 + NbP2 - 1 - CDegree + M;
//    the poles of the result
    Standard_Integer PLength = NbPolesResult*CDimension;

    for (jj=0; jj<PLength; jj++) {
      PRadr[jj] = ResultPoles(jj+1);
    }
  
//    flat nodes of the result
    Standard_Integer ideb = 0;

    for (jj=0; jj<ResultKnots.Length(); jj++) {
      for (ii=0; ii<ResultMults(jj+1); ii++) {
	KRadr[ideb+ii] = ResultKnots(jj+1);
      }
      ideb += ResultMults(jj+1);
    }
    NbKnotsResult = ideb;
  }
}

//=======================================================================
//function : Resolution
//purpose  : 
//                           d
//  Let C(t) = SUM      Ci Bi(t)  a Bspline curve of degree d  
//	      i = 1,n      
//  with nodes tj for j = 1,n+d+1 
//
//
//         '                    C1 - Ci-1   d-1
//  Then C (t) = SUM     d *  ---------  Bi (t) 
//   	          i = 2,n      ti+d - ti
//
//		            d-1
//  for the base of BSpline  Bi  (t) of degree d-1.
//
//  Consequently the upper bound of the norm of the derivative from C is :
//
//
//                        |  Ci - Ci-1  |
//          d *   Max     |  ---------  |
//	        i = 2,n |  ti+d - ti  |
//     
//					N(t) 
//  In the rational case set    C(t) = -----
//					D(t) 
//
//  
//  D(t) =  SUM    Di Bi(t) 
//	  i=1,n
//
//  N(t) =  SUM   Di * Ci Bi(t) 
//          i =1,n
//
//	    N'(t)  -    D'(t) C(t) 
//   C'(t) = -----------------------
//	             D(t)
//
//                                   
//   N'(t) - D'(t) C(t) = 
//	
//                     Di * (Ci - C(t)) - Di-1 * (Ci-1 - C(t))    d-1
//	SUM   d *   ---------------------------------------- * Bi  (t)  =
//        i=2,n                   ti+d   - ti
//
//    
//                   Di * (Ci - Cj) - Di-1 * (Ci-1 - Cj)                d-1
// SUM   SUM     d * -----------------------------------  * Betaj(t) * Bi  (t) 
//i=2,n j=1,n               ti+d  - ti  
//  
//
//
//                 Dj Bj(t) 
//    Betaj(t) =   --------
//	           D(t) 
//
//  Betaj(t) form a partition >= 0 of the entity with support
//  tj, tj+d+1. Consequently if Rj = {j-d, ....,  j+d+d+1} 
//  obtain an upper bound of the derivative of C by taking :
//
//
//
//
//
//    
//                         Di * (Ci - Cj) - Di-1 * (Ci-1 - Cj) 
//   Max   Max       d  *  -----------------------------------  
// j=1,n  i dans Rj                   ti+d  - ti  
//
//  --------------------------------------------------------
//
//               Min    Di
//              i =1,n
//  
//
//=======================================================================

void BSplCLib::Resolution(      Standard_Real&        Poles,
			  const Standard_Integer      ArrayDimension,
			  const Standard_Integer      NumPoles,
			  const TColStd_Array1OfReal* Weights,
			  const TColStd_Array1OfReal& FlatKnots,
			  const Standard_Integer      Degree,
			  const Standard_Real         Tolerance3D,
			  Standard_Real&              UTolerance) 
{
  Standard_Integer ii,num_poles,ii_index,jj_index,ii_inDim;
  Standard_Integer lower,upper,ii_minus,jj,ii_miDim;
  Standard_Integer Deg1 = Degree + 1;
  Standard_Integer Deg2 = (Degree << 1) + 1;
  Standard_Real value,factor,W,min_weights,inverse;
  Standard_Real pa_ii_inDim_0, pa_ii_inDim_1, pa_ii_inDim_2, pa_ii_inDim_3;
  Standard_Real pa_ii_miDim_0, pa_ii_miDim_1, pa_ii_miDim_2, pa_ii_miDim_3;
  Standard_Real wg_ii_index, wg_ii_minus;
  Standard_Real *PA,max_derivative;
  const Standard_Real * FK = &FlatKnots(FlatKnots.Lower());
  PA = &Poles;
  max_derivative = 0.0e0;
  num_poles = FlatKnots.Length() - Deg1;
  switch (ArrayDimension) {
  case 2 : {
    if (Weights != NULL) {
      const Standard_Real * WG = &(*Weights)(Weights->Lower());
      min_weights = WG[0];
      
      for (ii = 1 ; ii < NumPoles ; ii++) {
	W = WG[ii];
	if (W < min_weights) min_weights = W;
      }
      
      for (ii = 1 ; ii < num_poles ; ii++) {
	ii_index = ii % NumPoles;
	ii_inDim = ii_index << 1;
	ii_minus = (ii - 1) % NumPoles;
	ii_miDim = ii_minus << 1;
	pa_ii_inDim_0 = PA[ii_inDim]; ii_inDim++;
	pa_ii_inDim_1 = PA[ii_inDim];
	pa_ii_miDim_0 = PA[ii_miDim]; ii_miDim++;
	pa_ii_miDim_1 = PA[ii_miDim];
	wg_ii_index   = WG[ii_index];
	wg_ii_minus   = WG[ii_minus];
	inverse = FK[ii + Degree] - FK[ii];
	inverse = 1.0e0 / inverse;
	lower = ii - Deg1;
	if (lower < 0) lower = 0;
	upper = Deg2 + ii;
	if (upper > num_poles) upper = num_poles;
	
	for (jj = lower ; jj < upper ; jj++) {
	  jj_index = jj % NumPoles;
	  jj_index = jj_index << 1;
	  value = 0.0e0;
	  factor  = (((PA[jj_index] - pa_ii_inDim_0) * wg_ii_index) -
		     ((PA[jj_index] - pa_ii_miDim_0) * wg_ii_minus));
	  if (factor < 0) factor = - factor;
	  value += factor; jj_index++;
	  factor  = (((PA[jj_index] - pa_ii_inDim_1) * wg_ii_index) -
		     ((PA[jj_index] - pa_ii_miDim_1) * wg_ii_minus));
	  if (factor < 0) factor = - factor;
	  value += factor;
	  value *= inverse;
	  if (max_derivative < value) max_derivative = value;
	}
      }
      max_derivative /= min_weights;
    }
    else {
      
      for (ii = 1 ; ii < num_poles ; ii++) {
	ii_index = ii % NumPoles;
	ii_index = ii_index << 1;
	ii_minus = (ii - 1) % NumPoles;
	ii_minus = ii_minus << 1;
	inverse = FK[ii + Degree] - FK[ii];
	inverse = 1.0e0 / inverse;
	value = 0.0e0;
	factor = PA[ii_index] - PA[ii_minus];
	if (factor < 0) factor = - factor;
	value += factor; ii_index++; ii_minus++;
	factor = PA[ii_index] - PA[ii_minus];
	if (factor < 0) factor = - factor;
	value += factor;
	value *= inverse;
	if (max_derivative < value) max_derivative = value;
      }
    }
    break;
  }
  case 3 : {
    if (Weights != NULL) {
      const Standard_Real * WG = &(*Weights)(Weights->Lower());
      min_weights = WG[0];
      
      for (ii = 1 ; ii < NumPoles ; ii++) {
	W = WG[ii];
	if (W < min_weights) min_weights = W;
      }
      
      for (ii = 1 ; ii < num_poles ; ii++) {
	ii_index = ii % NumPoles;
	ii_inDim = (ii_index << 1) + ii_index;
	ii_minus = (ii - 1) % NumPoles;
	ii_miDim = (ii_minus << 1) + ii_minus;
	pa_ii_inDim_0 = PA[ii_inDim]; ii_inDim++;
	pa_ii_inDim_1 = PA[ii_inDim]; ii_inDim++;
	pa_ii_inDim_2 = PA[ii_inDim];
	pa_ii_miDim_0 = PA[ii_miDim]; ii_miDim++;
	pa_ii_miDim_1 = PA[ii_miDim]; ii_miDim++;
	pa_ii_miDim_2 = PA[ii_miDim];
	wg_ii_index   = WG[ii_index];
	wg_ii_minus   = WG[ii_minus];
	inverse = FK[ii + Degree] - FK[ii];
	inverse = 1.0e0 / inverse;
	lower = ii - Deg1;
	if (lower < 0) lower = 0;
	upper = Deg2 + ii;
	if (upper > num_poles) upper = num_poles;
	
	for (jj = lower ; jj < upper ; jj++) {
	  jj_index = jj % NumPoles;
	  jj_index = (jj_index << 1) + jj_index;
	  value = 0.0e0;
	  factor  = (((PA[jj_index] - pa_ii_inDim_0) * wg_ii_index) -
		     ((PA[jj_index] - pa_ii_miDim_0) * wg_ii_minus));
	  if (factor < 0) factor = - factor;
	  value += factor; jj_index++;
	  factor  = (((PA[jj_index] - pa_ii_inDim_1) * wg_ii_index) -
		     ((PA[jj_index] - pa_ii_miDim_1) * wg_ii_minus));
	  if (factor < 0) factor = - factor;
	  value += factor; jj_index++;
	  factor  = (((PA[jj_index] - pa_ii_inDim_2) * wg_ii_index) -
		     ((PA[jj_index] - pa_ii_miDim_2) * wg_ii_minus));
	  if (factor < 0) factor = - factor;
	  value += factor;
	  value *= inverse;
	  if (max_derivative < value) max_derivative = value;
	}
      }
      max_derivative /= min_weights;
    }
    else {
      
      for (ii = 1 ; ii < num_poles ; ii++) {
	ii_index = ii % NumPoles;
	ii_index = (ii_index << 1) + ii_index;
	ii_minus = (ii - 1) % NumPoles;
	ii_minus = (ii_minus << 1) + ii_minus;
	inverse = FK[ii + Degree] - FK[ii];
	inverse = 1.0e0 / inverse;
	value = 0.0e0;
	factor = PA[ii_index] - PA[ii_minus];
	if (factor < 0) factor = - factor;
	value += factor; ii_index++; ii_minus++;
	factor = PA[ii_index] - PA[ii_minus];
	if (factor < 0) factor = - factor;
	value += factor; ii_index++; ii_minus++;
	factor = PA[ii_index] - PA[ii_minus];
	if (factor < 0) factor = - factor;
	value += factor;
	value *= inverse;
	if (max_derivative < value) max_derivative = value;
      }
    }
    break;
  }
  case 4 : {
    if (Weights != NULL) {
      const Standard_Real * WG = &(*Weights)(Weights->Lower());
      min_weights = WG[0];
      
      for (ii = 1 ; ii < NumPoles ; ii++) {
	W = WG[ii];
	if (W < min_weights) min_weights = W;
      }
      
      for (ii = 1 ; ii < num_poles ; ii++) {
	ii_index = ii % NumPoles;
	ii_inDim = ii_index << 2;
	ii_minus = (ii - 1) % NumPoles;
	ii_miDim = ii_minus << 2;
	pa_ii_inDim_0 = PA[ii_inDim]; ii_inDim++;
	pa_ii_inDim_1 = PA[ii_inDim]; ii_inDim++;
	pa_ii_inDim_2 = PA[ii_inDim]; ii_inDim++;
	pa_ii_inDim_3 = PA[ii_inDim];
	pa_ii_miDim_0 = PA[ii_miDim]; ii_miDim++;
	pa_ii_miDim_1 = PA[ii_miDim]; ii_miDim++;
	pa_ii_miDim_2 = PA[ii_miDim]; ii_miDim++;
	pa_ii_miDim_3 = PA[ii_miDim];
	wg_ii_index   = WG[ii_index];
	wg_ii_minus   = WG[ii_minus];
	inverse = FK[ii + Degree] - FK[ii];
	inverse = 1.0e0 / inverse;
	lower = ii - Deg1;
	if (lower < 0) lower = 0;
	upper = Deg2 + ii;
	if (upper > num_poles) upper = num_poles;
	
	for (jj = lower ; jj < upper ; jj++) {
	  jj_index = jj % NumPoles;
	  jj_index = jj_index << 2;
	  value = 0.0e0;
	  factor  = (((PA[jj_index] - pa_ii_inDim_0) * wg_ii_index) -
		     ((PA[jj_index] - pa_ii_miDim_0) * wg_ii_minus));
	  if (factor < 0) factor = - factor;
	  value += factor; jj_index++;
	  factor  = (((PA[jj_index] - pa_ii_inDim_1) * wg_ii_index) -
		     ((PA[jj_index] - pa_ii_miDim_1) * wg_ii_minus));
	  if (factor < 0) factor = - factor;
	  value += factor; jj_index++;
	  factor  = (((PA[jj_index] - pa_ii_inDim_2) * wg_ii_index) -
		     ((PA[jj_index] - pa_ii_miDim_2) * wg_ii_minus));
	  if (factor < 0) factor = - factor;
	  value += factor; jj_index++;
	  factor  = (((PA[jj_index] - pa_ii_inDim_3) * wg_ii_index) -
		     ((PA[jj_index] - pa_ii_miDim_3) * wg_ii_minus));
	  if (factor < 0) factor = - factor;
	  value += factor;
	  value *= inverse;
	  if (max_derivative < value) max_derivative = value;
	}
      }
      max_derivative /= min_weights;
    }
    else {
      
      for (ii = 1 ; ii < num_poles ; ii++) {
	ii_index = ii % NumPoles;
	ii_index = ii_index << 2;
	ii_minus = (ii - 1) % NumPoles;
	ii_minus = ii_minus << 2;
	inverse = FK[ii + Degree] - FK[ii];
	inverse = 1.0e0 / inverse;
	value = 0.0e0;
	factor = PA[ii_index] - PA[ii_minus];
	if (factor < 0) factor = - factor;
	value += factor; ii_index++; ii_minus++;
	factor = PA[ii_index] - PA[ii_minus];
	if (factor < 0) factor = - factor;
	value += factor; ii_index++; ii_minus++;
	factor = PA[ii_index] - PA[ii_minus];
	if (factor < 0) factor = - factor;
	value += factor; ii_index++; ii_minus++;
	factor = PA[ii_index] - PA[ii_minus];
	if (factor < 0) factor = - factor;
	value += factor;
	value *= inverse;
	if (max_derivative < value) max_derivative = value;
      }
    }
    break;
  }
    default : {
      Standard_Integer kk;
      if (Weights != NULL) {
        const Standard_Real * WG = &(*Weights)(Weights->Lower());
	min_weights = WG[0];
	
	for (ii = 1 ; ii < NumPoles ; ii++) {
	  W = WG[ii];
	  if (W < min_weights) min_weights = W;
	}
	
	for (ii = 1 ; ii < num_poles ; ii++) {
	  ii_index  = ii % NumPoles;
	  ii_inDim  = ii_index * ArrayDimension;
	  ii_minus  = (ii - 1) % NumPoles;
	  ii_miDim  = ii_minus * ArrayDimension;
	  wg_ii_index   = WG[ii_index];
	  wg_ii_minus   = WG[ii_minus];
	  inverse = FK[ii + Degree] - FK[ii];
	  inverse = 1.0e0 / inverse;
	  lower = ii - Deg1;
	  if (lower < 0) lower = 0;
	  upper = Deg2 + ii;
	  if (upper > num_poles) upper = num_poles;
	  
	  for (jj = lower ; jj < upper ; jj++) {
	    jj_index = jj % NumPoles;
	    jj_index *= ArrayDimension;
	    value = 0.0e0;
	    
	    for (kk = 0 ; kk < ArrayDimension ; kk++) {
	      factor  = (((PA[jj_index + kk] - PA[ii_inDim + kk]) * wg_ii_index) -
			 ((PA[jj_index + kk] - PA[ii_miDim + kk]) * wg_ii_minus));
	      if (factor < 0) factor = - factor;
	      value += factor;
	    }
	    value *= inverse;
	    if (max_derivative < value) max_derivative = value;
	  }
	}
	max_derivative /= min_weights;
      }
      else {
	
	for (ii = 1 ; ii < num_poles ; ii++) {
	  ii_index  = ii % NumPoles;
	  ii_index *= ArrayDimension;
	  ii_minus  = (ii - 1) % NumPoles;
	  ii_minus *= ArrayDimension;
	  inverse = FK[ii + Degree] - FK[ii];
	  inverse = 1.0e0 / inverse;
	  value = 0.0e0;
	  
	  for (kk = 0 ; kk < ArrayDimension ; kk++) {
	    factor = PA[ii_index + kk] - PA[ii_minus + kk];
	    if (factor < 0) factor = - factor;
	    value += factor;
	  }
	  value *= inverse;
	  if (max_derivative < value) max_derivative = value;
	}
      }
    }
  }
  max_derivative *= Degree;
  if (max_derivative > RealSmall())
    UTolerance = Tolerance3D / max_derivative; 
  else
    UTolerance = Tolerance3D / RealSmall();
}

//=======================================================================
// function: FlatBezierKnots
// purpose :
//=======================================================================

// array of flat knots for bezier curve of maximum 25 degree
static const Standard_Real knots[52] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
const Standard_Real& BSplCLib::FlatBezierKnots (const Standard_Integer Degree)
{
  Standard_OutOfRange_Raise_if (Degree < 1 || Degree > MaxDegree() || MaxDegree() != 25,
    "Bezier curve degree greater than maximal supported");

  return knots[25-Degree];
}
