// Created on: 1993-04-29
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

// 20/02/97 : PMN -> Positionement local sur BSpline (PRO6902)
// 10/07/97 : PMN -> Pas de calcul de resolution dans Nb(Intervals)(PRO9248)
// 20/10/97 : RBV -> traitement des offset curves

#define No_Standard_RangeError
#define No_Standard_OutOfRange

#include <GeomAdaptor_Curve.hxx>

#include <Adaptor3d_Curve.hxx>
#include <BSplCLib.hxx>
#include <BSplCLib_Cache.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomEvaluator_OffsetCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

//#include <GeomConvert_BSplineCurveKnotSplitting.hxx>
static const Standard_Real PosTol = Precision::PConfusion() / 2;

IMPLEMENT_STANDARD_RTTIEXT(GeomAdaptor_Curve, Adaptor3d_Curve)

static void DefinFPeriod(const Standard_Real theLower,
  const Standard_Real theUpper,
  const Standard_Real theEps,
  const Standard_Real thePeriod,
  Standard_Real &theCurFirst,
  Standard_Integer &theFPer);

static void DefinLPeriod(const Standard_Real theLower,
  const Standard_Real theUpper,
  const Standard_Real theEps,
  const Standard_Real thePeriod,
  Standard_Real &theCurLast,
  Standard_Integer &theLPer);

static Standard_Integer LocalNbIntervals(const TColStd_Array1OfReal& theTK,
  const TColStd_Array1OfInteger& theTM,
  const TColStd_Array1OfInteger& theInter,
  const Standard_Integer theCurDegree,
  const Standard_Integer theNb,
  const Standard_Integer theNbInt,
  const Standard_Real theFirst,
  const Standard_Real theLast,
  const Standard_Real theEps,
  const Standard_Boolean thePeriodicCur,
  Standard_Integer theNbIntervals,
  Standard_Real theLower = 0,
  Standard_Real thePeriod = 0,
  Standard_Integer theIndex1 = 0,
  Standard_Integer theIndex2 = 0);

static void WriteIntervals(const TColStd_Array1OfReal &theTK,
  const TColStd_Array1OfInteger &theInter,
  const Standard_Integer theNbInt,
  const Standard_Integer theIndex1,
  const Standard_Integer theIndex2,
  const Standard_Real theCurPeriod,
  const Standard_Boolean theFlagForFirst,
  TColStd_Array1OfReal &theT,
  TColStd_Array1OfInteger &theFinalIntervals,
  Standard_Integer &theNbIntervals,
  Standard_Integer &theCurInt);

static void SpreadInt(const TColStd_Array1OfReal &theTK,
  const TColStd_Array1OfInteger &theTM,
  const TColStd_Array1OfInteger &theInter,
  const Standard_Integer theCurDegree,
  const Standard_Integer theNb,
  const Standard_Integer theFPer,
  const Standard_Integer theLPer,
  const Standard_Integer theNbInt,
  const Standard_Real theLower,
  const Standard_Real theFirst,
  const Standard_Real theLast,
  const Standard_Real thePeriod,
  const Standard_Real theLastParam,
  const Standard_Real theEps,
  TColStd_Array1OfReal &theT,
  Standard_Integer &theNbIntervals);

//=======================================================================
//function : ShallowCopy
//purpose  : 
//=======================================================================

Handle(Adaptor3d_Curve) GeomAdaptor_Curve::ShallowCopy() const
{
  Handle(GeomAdaptor_Curve) aCopy = new GeomAdaptor_Curve();

  aCopy->myCurve           = myCurve;
  aCopy->myTypeCurve       = myTypeCurve;
  aCopy->myFirst           = myFirst;
  aCopy->myLast            = myLast;
  aCopy->myBSplineCurve    = myBSplineCurve;
  if (!myNestedEvaluator.IsNull())
  {
    aCopy->myNestedEvaluator = myNestedEvaluator->ShallowCopy();
  }

  return aCopy;
}

//=======================================================================
//function : LocalContinuity
//purpose  : Computes the Continuity of a BSplineCurve 
//           between the parameters U1 and U2
//           The continuity is C(d-m) 
//             with   d = degree, 
//                    m = max multiplicity of the Knots between U1 and U2
//=======================================================================

GeomAbs_Shape GeomAdaptor_Curve::LocalContinuity(const Standard_Real U1, 
						 const Standard_Real U2) 
     const 
{
  Standard_NoSuchObject_Raise_if(myTypeCurve!=GeomAbs_BSplineCurve," ");
  Standard_Integer Nb = myBSplineCurve->NbKnots();
  Standard_Integer Index1 = 0;
  Standard_Integer Index2 = 0;
  Standard_Real newFirst, newLast;
  const TColStd_Array1OfReal& TK = myBSplineCurve->Knots();
  const TColStd_Array1OfInteger& TM = myBSplineCurve->Multiplicities();
  BSplCLib::LocateParameter(myBSplineCurve->Degree(),TK,TM,U1,myBSplineCurve->IsPeriodic(),
			    1,Nb,Index1,newFirst);
  BSplCLib::LocateParameter(myBSplineCurve->Degree(),TK,TM,U2,myBSplineCurve->IsPeriodic(),
			    1,Nb,Index2,newLast);
  if ( Abs(newFirst-TK(Index1+1))<Precision::PConfusion()) { 
    if (Index1 < Nb) Index1++;
  }
  if ( Abs(newLast-TK(Index2))<Precision::PConfusion())
    Index2--;
  Standard_Integer MultMax;
  // attention aux courbes peridiques.
  if ( (myBSplineCurve->IsPeriodic()) && (Index1 == Nb) )
    Index1 = 1;

  if ((Index2 - Index1 <= 0) && (!myBSplineCurve->IsPeriodic())) {
    MultMax = 100;  // CN entre 2 Noeuds consecutifs
  }
  else {
    MultMax = TM(Index1+1);
    for(Standard_Integer i = Index1+1;i<=Index2;i++) {
      if ( TM(i)>MultMax) MultMax=TM(i);
    }
    MultMax = myBSplineCurve->Degree() - MultMax;
  }
  if ( MultMax <= 0) {
    return GeomAbs_C0;
  }
  else if ( MultMax == 1) {
    return GeomAbs_C1;
  } 
  else if ( MultMax == 2) {
    return GeomAbs_C2;
  }
  else if ( MultMax == 3) {
    return GeomAbs_C3;
  }
  else { 
    return GeomAbs_CN;
  }
}

//=======================================================================
//function : Reset
//purpose  :
//=======================================================================
void GeomAdaptor_Curve::Reset()
{
  myTypeCurve = GeomAbs_OtherCurve;
  myCurve.Nullify();
  myNestedEvaluator.Nullify();
  myBSplineCurve.Nullify();
  myCurveCache.Nullify();
  myFirst = myLast = 0.0;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void GeomAdaptor_Curve::load(const Handle(Geom_Curve)& C,
                             const Standard_Real UFirst,
                             const Standard_Real ULast)
{
  myFirst = UFirst;
  myLast  = ULast;
  myCurveCache.Nullify();

  if ( myCurve != C) {
    myCurve = C;
    myNestedEvaluator.Nullify();
    myBSplineCurve.Nullify();
    
    const Handle(Standard_Type)& TheType = C->DynamicType();
    if ( TheType == STANDARD_TYPE(Geom_TrimmedCurve)) {
      Load(Handle(Geom_TrimmedCurve)::DownCast (C)->BasisCurve(),UFirst,ULast);
    }
    else if ( TheType ==  STANDARD_TYPE(Geom_Circle)) {
      myTypeCurve = GeomAbs_Circle;
    }
    else if ( TheType ==STANDARD_TYPE(Geom_Line)) {
      myTypeCurve = GeomAbs_Line;
    }
    else if ( TheType == STANDARD_TYPE(Geom_Ellipse)) {
      myTypeCurve = GeomAbs_Ellipse;
    }
    else if ( TheType == STANDARD_TYPE(Geom_Parabola)) {
      myTypeCurve = GeomAbs_Parabola;
    }
    else if ( TheType == STANDARD_TYPE(Geom_Hyperbola)) {
      myTypeCurve = GeomAbs_Hyperbola;
    }
    else if ( TheType == STANDARD_TYPE(Geom_BezierCurve)) {
      myTypeCurve = GeomAbs_BezierCurve;
    }
    else if ( TheType == STANDARD_TYPE(Geom_BSplineCurve)) {
      myTypeCurve = GeomAbs_BSplineCurve;
      myBSplineCurve = Handle(Geom_BSplineCurve)::DownCast(myCurve);
    }
    else if ( TheType == STANDARD_TYPE(Geom_OffsetCurve)) {
      myTypeCurve = GeomAbs_OffsetCurve;
      Handle(Geom_OffsetCurve) anOffsetCurve = Handle(Geom_OffsetCurve)::DownCast(myCurve);
      // Create nested adaptor for base curve
      Handle(Geom_Curve) aBaseCurve = anOffsetCurve->BasisCurve();
      Handle(GeomAdaptor_Curve) aBaseAdaptor = new GeomAdaptor_Curve(aBaseCurve);
      myNestedEvaluator = new GeomEvaluator_OffsetCurve(
          aBaseAdaptor, anOffsetCurve->Offset(), anOffsetCurve->Direction());
    }
    else {
      myTypeCurve = GeomAbs_OtherCurve;
    }
  }
}

//    --
//    --     Global methods - Apply to the whole curve.
//    --     

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape GeomAdaptor_Curve::Continuity() const 
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
    return LocalContinuity(myFirst, myLast);

  if (myTypeCurve == GeomAbs_OffsetCurve)
  {
    const GeomAbs_Shape S =
      Handle(Geom_OffsetCurve)::DownCast (myCurve)->GetBasisCurveContinuity();
    switch(S)
    {
      case GeomAbs_CN: return GeomAbs_CN;
      case GeomAbs_C3: return GeomAbs_C2;
      case GeomAbs_C2: return GeomAbs_C1;
      case GeomAbs_C1: return GeomAbs_C0; 
      case GeomAbs_G1: return GeomAbs_G1;
      case GeomAbs_G2: return GeomAbs_G2;
      default:
        throw Standard_NoSuchObject("GeomAdaptor_Curve::Continuity");
    }
  }
  else if (myTypeCurve == GeomAbs_OtherCurve) {
    throw Standard_NoSuchObject("GeomAdaptor_Curve::Contunuity");
  }

  return GeomAbs_CN;
}

//=======================================================================
//function : DefinFPeriod
//purpose  :
//=======================================================================

void DefinFPeriod(const Standard_Real theLower,
  const Standard_Real theUpper,
  const Standard_Real theEps,
  const Standard_Real thePeriod,
  Standard_Real &theCurFirst,
  Standard_Integer &theFPer)
{
  if (theCurFirst >= theLower)
  {
    while (theCurFirst >= theUpper)
    {
      theCurFirst = theCurFirst - thePeriod;
      theFPer++;
    }
    if (Abs(theUpper - theCurFirst) <= theEps)
    {
      theFPer++;
      theCurFirst = theLower;
    }
  }
  else
  {
    while (theCurFirst < theLower)
    {
      theCurFirst = theCurFirst + thePeriod;
      if ((Abs(theLower - theCurFirst)) > theEps)
      {
        theFPer--;
      }
    }

    if (Abs(theUpper - theCurFirst) <= theEps)
    {
      theCurFirst = theLower;
    }
  }
}

//=======================================================================
//function : DefinLPeriod
//purpose  :
//=======================================================================

void DefinLPeriod(const Standard_Real theLower,
  const Standard_Real theUpper,
  const Standard_Real theEps,
  const Standard_Real thePeriod,
  Standard_Real &theCurLast,
  Standard_Integer &theLPer)
{
  if (theCurLast >= theLower)
  {
    if ((theCurLast >= theUpper) && (Abs(theCurLast - theUpper) <= theEps))
    {
      theCurLast = theUpper;
    }
    else
    {
      while (theCurLast >= theUpper)
      {
        theCurLast = theCurLast - thePeriod;
        theLPer++;
      }
      if (Abs(theUpper - theCurLast) <= theEps)
      {
        theCurLast = theLower;
      }
    }
  }
  else
  {
    while (theCurLast < theLower)
    {
      theCurLast = theCurLast + thePeriod;
      if (Abs(theLower - theCurLast) > theEps)
      {
        theLPer--;
      }
    }
    if ((theUpper - theCurLast) <= theEps)
    {
      theCurLast = theLower;
    }
  }
}

//=======================================================================
//function : LocalNbIntervals
//purpose  :
//=======================================================================

Standard_Integer LocalNbIntervals(const TColStd_Array1OfReal& theTK,
  const TColStd_Array1OfInteger& theTM,
  const TColStd_Array1OfInteger& theInter,
  const Standard_Integer theCurDegree,
  const Standard_Integer theNb,
  const Standard_Integer theNbInt,
  const Standard_Real theFirst,
  const Standard_Real theLast,
  const Standard_Real theEps,
  const Standard_Boolean thePeriodicCur,
  Standard_Integer theNbIntervals,
  Standard_Real theLower,
  Standard_Real thePeriod,
  Standard_Integer theIndex1,
  Standard_Integer theIndex2)
{
  Standard_Real aNewFirst = theFirst;
  Standard_Real aNewLast = theLast;
  if (theIndex1 == 0)
  {
    BSplCLib::LocateParameter(theCurDegree, theTK, theTM, theFirst,
      thePeriodicCur, 1, theNb, theIndex1, aNewFirst);
  }
  if (theIndex2 == 0)
  {
    BSplCLib::LocateParameter(theCurDegree, theTK, theTM, theLast,
      thePeriodicCur, 1, theNb, theIndex2, aNewLast);
  }
  // Protection against theFirst = UFirst - eps, which located as ULast - eps
  if (thePeriodicCur && ((aNewLast - aNewFirst) < Precision::PConfusion()))
  {
    if (Abs(aNewLast - theLower) < Precision::PConfusion())
    {
      aNewLast += thePeriod;
    }
    else
    {
      aNewFirst -= thePeriod;
    }
  }

  if (Abs(aNewFirst - theTK(theIndex1 + 1)) < theEps)
  {
    theIndex1++;
  }
  if ((aNewLast - theTK(theIndex2)) > theEps)
  {
    theIndex2++;
  }
  for (Standard_Integer i = 1; i <= theNbInt; i++)
  {
    if (theInter(i) > theIndex1 && theInter(i) < theIndex2) theNbIntervals++;
  }
  return theNbIntervals;
}


//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Curve::NbIntervals(const GeomAbs_Shape S) const
{
  Standard_Integer myNbIntervals = 1;
  Standard_Integer NbSplit;
  if (myTypeCurve == GeomAbs_BSplineCurve) {
    Standard_Integer FirstIndex = myBSplineCurve->FirstUKnotIndex();
    Standard_Integer LastIndex = myBSplineCurve->LastUKnotIndex();
    TColStd_Array1OfInteger Inter(1, LastIndex - FirstIndex + 1);
    Standard_Boolean aContPer = (S >= Continuity()) && myBSplineCurve->IsPeriodic();
    Standard_Boolean aContNotPer = (S > Continuity()) && !myBSplineCurve->IsPeriodic();

    if(aContPer || aContNotPer) {
      Standard_Integer Cont;
      switch (S) {
      case GeomAbs_G1:
      case GeomAbs_G2:
        throw Standard_DomainError("GeomAdaptor_Curve::NbIntervals");
        break;
      case GeomAbs_C0:
        myNbIntervals = 1;
        break;
      case GeomAbs_C1:
      case GeomAbs_C2:
      case GeomAbs_C3:
      case GeomAbs_CN:
      {
        if (S == GeomAbs_C1) Cont = 1;
        else if (S == GeomAbs_C2) Cont = 2;
        else if (S == GeomAbs_C3) Cont = 3;
        else                       Cont = myBSplineCurve->Degree();
        Standard_Integer Degree = myBSplineCurve->Degree();
        Standard_Integer NbKnots = myBSplineCurve->NbKnots();
        TColStd_Array1OfInteger Mults(1, NbKnots);
        myBSplineCurve->Multiplicities(Mults);
        NbSplit = 1;
        Standard_Integer Index = FirstIndex;
        Inter(NbSplit) = Index;
        Index++;
        NbSplit++;
        while (Index < LastIndex)
        {
          if (Degree - Mults(Index) < Cont)
          {
            Inter(NbSplit) = Index;
            NbSplit++;
          }
          Index++;
        }
        Inter(NbSplit) = Index;

        Standard_Integer NbInt = NbSplit - 1;

        Standard_Integer Nb = myBSplineCurve->NbKnots();
        Standard_Integer Index1 = 0;
        Standard_Integer Index2 = 0;
        const TColStd_Array1OfReal& TK = myBSplineCurve->Knots();
        const TColStd_Array1OfInteger& TM = myBSplineCurve->Multiplicities();
        Standard_Real Eps = Min(Resolution(Precision::Confusion()),
          Precision::PConfusion());

        myNbIntervals = 1;

        if (!myBSplineCurve->IsPeriodic())
        {
          myNbIntervals = LocalNbIntervals(TK, TM, Inter, Degree, Nb, NbInt,
            myFirst, myLast, Eps, Standard_False, myNbIntervals);
        }
        else
        {
          Standard_Real aCurFirst = myFirst;
          Standard_Real aCurLast = myLast;
          Standard_Real aLower = myBSplineCurve->FirstParameter();
          Standard_Real anUpper = myBSplineCurve->LastParameter();

          if ((Abs(aCurFirst - aLower) < Eps) && (aCurFirst < aLower))
          {
            aCurFirst = aLower;
          }
          if ((Abs(aCurLast - anUpper) < Eps) && (aCurLast < anUpper))
          {
            aCurLast = anUpper;
          }

          Standard_Real aPeriod = myBSplineCurve->Period();
          Standard_Integer aLPer = 1; Standard_Integer aFPer = 1;
          if ((Abs(aLower - myFirst) < Eps) && (aCurFirst < aLower))
          {
            aCurFirst = aLower;
          }
          else
          {
            DefinFPeriod(aLower, anUpper,
              Eps, aPeriod, aCurFirst, aFPer);
          }
          DefinLPeriod(aLower, anUpper,
            Eps, aPeriod, aCurLast, aLPer);

          Standard_Real aNewFirst;
          Standard_Real aNewLast;
          BSplCLib::LocateParameter(myBSplineCurve->Degree(), TK, TM, myFirst,
            Standard_True, 1, Nb, Index1, aNewFirst);
          BSplCLib::LocateParameter(myBSplineCurve->Degree(), TK, TM, myLast,
            Standard_True, 1, Nb, Index2, aNewLast);
          if ((aNewFirst == myFirst && aNewLast == myLast) && (aFPer != aLPer))
          {
            myNbIntervals = LocalNbIntervals(TK, TM, Inter, Degree, Nb, NbInt,
              myFirst, myLast, Eps, Standard_True, myNbIntervals, aLower, aPeriod);
          }
          else
          {
            Standard_Integer aSumPer = Abs(aLPer - aFPer);
            
            Standard_Real aFirst = 0;
            if (aLower < 0 && anUpper == 0)
            {
              if (Abs(aCurLast) < Eps)
              {
                aCurLast = 0;
              }
              aFirst = aLower;
            }

            if (aSumPer <= 1)
            {
              if ((Abs(myFirst - TK(Nb) - aPeriod * (aFPer - 1)) <= Eps) && (myLast < (TK(Nb) + aPeriod * (aLPer - 1))))
              {
                myNbIntervals = LocalNbIntervals(TK, TM, Inter, Degree, Nb, NbInt,
                  myFirst, myLast, Eps, Standard_True, myNbIntervals, aLower, aPeriod);
                return myNbIntervals;
              }
              if ((Abs(myFirst - aLower) < Eps) && (Abs(myLast - anUpper) < Eps))
              {
                myNbIntervals = LocalNbIntervals(TK, TM, Inter, Degree, Nb, NbInt,
                  myFirst, myLast, Eps, Standard_True, myNbIntervals, aLower, aPeriod);
                return myNbIntervals;
              }
            }

            if (aSumPer != 0)
            {
              Standard_Integer aFInt = 0;
              Standard_Integer aLInt = 0;
              Standard_Integer aPInt = NbInt;

              if ((aCurFirst != aPeriod) || ((aCurFirst != anUpper) && (Abs(myFirst) < Eps)))
              {
                aFInt = 1;
              }
              if ((aCurLast != aLower) && (aCurLast != anUpper))
              {
                aLInt = 1;
              }

              aFInt = LocalNbIntervals(TK, TM, Inter, Degree, Nb, NbInt,
                aCurFirst, anUpper, Eps, Standard_True, aFInt, aLower, aPeriod);

              if (aCurLast == anUpper)
              {
                aLInt = NbInt;
              }
              else
              {
                if (Abs(aCurLast - aFirst) > Eps)
                {
                  aLInt = LocalNbIntervals(TK, TM, Inter, Degree, Nb, NbInt,
                    aFirst, aCurLast, Eps, Standard_True, aLInt, aLower, aPeriod, 1);
                }
                else
                {
                  aLInt = LocalNbIntervals(TK, TM, Inter, Degree, Nb, NbInt,
                    aFirst, aCurLast, Eps, Standard_True, aLInt, aLower, aPeriod);
                }
              }

              myNbIntervals = aFInt + aLInt + aPInt * (aSumPer - 1);
            }
            else
            {
              myNbIntervals = LocalNbIntervals(TK, TM, Inter, Degree, Nb, NbInt,
                aCurFirst, aCurLast, Eps, Standard_True, myNbIntervals, aLower, aPeriod);
            }
          }
        }
      }
      break;
      }
    }
  }
  
  else if (myTypeCurve == GeomAbs_OffsetCurve) {
    GeomAbs_Shape BaseS=GeomAbs_C0;
    switch(S){
    case GeomAbs_G1:
    case GeomAbs_G2:
      throw Standard_DomainError("GeomAdaptor_Curve::NbIntervals");
      break;
    case GeomAbs_C0: BaseS = GeomAbs_C1; break;
    case GeomAbs_C1: BaseS = GeomAbs_C2; break;
    case GeomAbs_C2: BaseS = GeomAbs_C3; break;
    default: BaseS = GeomAbs_CN;
    }
    GeomAdaptor_Curve C
      (Handle(Geom_OffsetCurve)::DownCast (myCurve)->BasisCurve());
    // akm 05/04/02 (OCC278)  If our curve is trimmed we must recalculate 
    //                    the number of intervals obtained from the basis to
    //              vvv   reflect parameter bounds
    Standard_Integer iNbBasisInt = C.NbIntervals(BaseS), iInt;
    if (iNbBasisInt>1)
    {
      TColStd_Array1OfReal rdfInter(1,1+iNbBasisInt);
      C.Intervals(rdfInter,BaseS);
      for (iInt=1; iInt<=iNbBasisInt; iInt++)
	if (rdfInter(iInt)>myFirst && rdfInter(iInt)<myLast)
	  myNbIntervals++;
    }
    // akm 05/04/02 ^^^
  }
  return myNbIntervals;
}

//=======================================================================
//function : WriteIntervals
//purpose  :
//=======================================================================

static void WriteIntervals(const TColStd_Array1OfReal &theTK,
  const TColStd_Array1OfInteger &theInter,
  const Standard_Integer theNbInt,
  const Standard_Integer theIndex1,
  const Standard_Integer theIndex2,
  const Standard_Real theCurPeriod,
  const Standard_Boolean theFlagForFirst,
  TColStd_Array1OfReal &theT,
  TColStd_Array1OfInteger &theFinalIntervals,
  Standard_Integer &theNbIntervals,
  Standard_Integer &theCurInt)
{
  if (theFlagForFirst)
  {
    for (Standard_Integer anId = 1; anId <= theNbInt; anId++)
    {
      if (theInter(anId) > theIndex1 && theInter(anId) <= theIndex2)
      {
        theNbIntervals++;
        theFinalIntervals(theNbIntervals) = theInter(anId);
      }
    }
  }
  else
  {
    for (Standard_Integer anId = 1; anId <= theNbInt; anId++)
    {
      if (theInter(anId) > theIndex1 && theInter(anId) < theIndex2)
      {
        theNbIntervals++;
        theFinalIntervals(theNbIntervals) = theInter(anId);
      }
    }
  }
 
  theFinalIntervals(theNbIntervals + 1) = theIndex2;

  for (Standard_Integer anId = theCurInt; anId <= theNbIntervals + 1; anId++)
  {
    theT(anId) = theTK(theFinalIntervals(anId)) + theCurPeriod;
    theCurInt++;
  }
}

//=======================================================================
//function : SpreadInt
//purpose  :
//=======================================================================

void SpreadInt(const TColStd_Array1OfReal &theTK,
  const TColStd_Array1OfInteger &theTM,
  const TColStd_Array1OfInteger &theInter,
  const Standard_Integer theCurDegree,
  const Standard_Integer theNb,
  const Standard_Integer theFPer,
  const Standard_Integer theLPer,
  const Standard_Integer theNbInt,
  const Standard_Real theLower,
  const Standard_Real theFirst,
  const Standard_Real theLast,
  const Standard_Real thePeriod,
  const Standard_Real theLastParam,
  const Standard_Real theEps,
  TColStd_Array1OfReal &theT,
  Standard_Integer &theNbIntervals)
{
  Standard_Integer anIndex1 = 0;
  Standard_Integer anIndex2 = 0;
  Standard_Real aNewFirst, aNewLast;
  Standard_Integer anUpper;
  BSplCLib::LocateParameter(theCurDegree, theTK, theTM, theFirst,
    Standard_True, 1, theNb, anIndex1, aNewFirst);
  BSplCLib::LocateParameter(theCurDegree, theTK, theTM, theLastParam,
    Standard_True, 1, theNb, anIndex2, aNewLast);

  if (Abs(aNewFirst - theTK(anIndex1 + 1)) < theEps)
  {
    anIndex1++;
  }
  if ((aNewLast - theTK(anIndex2)) > theEps)
  {
    anIndex2++;
  }
  theNbIntervals = 1;

  if (anIndex1 == theNb)
  {
    anIndex1 = 1;
  }

  // Count the max number of boundaries of intervals
  if (Abs(theLPer - theFPer) > 1)
  {
    anUpper = theNb - anIndex1 + anIndex2 + (theLPer - theFPer - 1) * theNb + 1;
  }
  else
  {
    anUpper = theNb - anIndex1 + anIndex2 + 1;
  }

  if (theLPer == theFPer)
  {
    anUpper = theInter.Upper();
  }
  TColStd_Array1OfInteger aFinalIntervals(1, anUpper);
  aFinalIntervals(1) = anIndex1;

  // If first and last are in the same period
  if ((Abs(theLPer - theFPer) == 0))
  {
    Standard_Integer aCurInt = 1;
    Standard_Real aCurPeriod = theFPer * thePeriod;

    if (theFirst == aNewFirst && theLast == aNewLast)
    {
      aCurPeriod = 0;
    }
    WriteIntervals(theTK, theInter, theNbInt, anIndex1,
      anIndex2, aCurPeriod, Standard_False, theT, aFinalIntervals, theNbIntervals, aCurInt);
    return;
  }

  // If the first and the last are in neighboring periods
  if (Abs(theLPer - theFPer) == 1)
  {
    Standard_Integer aCurInt = 1;

    if (Abs(theLastParam - theLower) < theEps)
    {
      WriteIntervals(theTK, theInter, theNbInt, anIndex1,
        theNb, theFPer * thePeriod, Standard_True, theT, aFinalIntervals, theNbIntervals, aCurInt);
      return;
    }
    else
    {
      // For period with first
      WriteIntervals(theTK, theInter, theNbInt, anIndex1,
        theNb, theFPer * thePeriod, Standard_True, theT, aFinalIntervals, theNbIntervals, aCurInt);
      // For period with last
      theNbIntervals++;
      WriteIntervals(theTK, theInter, theNbInt, 1,
        anIndex2, theLPer * thePeriod, Standard_False, theT, aFinalIntervals, theNbIntervals, aCurInt);
      return;
    }
  }
  // If the first and the last are far apart
  if (Abs(theLPer - theFPer) > 1)
  {
    Standard_Integer aCurInt = 1;

    if (Abs(theLastParam - theLower) < theEps)
    {
      WriteIntervals(theTK, theInter, theNbInt, anIndex1,
        theNb, theFPer * thePeriod, Standard_True, theT, aFinalIntervals, theNbIntervals, aCurInt);

      Standard_Integer aNbPer = Abs(theLPer - theFPer);
      Standard_Integer aCurPer = theFPer + 1;

      while (aNbPer > 1)
      {
        theNbIntervals++;
        WriteIntervals(theTK, theInter, theNbInt, 1,
          theNb, aCurPer * thePeriod, Standard_True, theT, aFinalIntervals, theNbIntervals, aCurInt);
        
        aNbPer--;
        aCurPer++;
      }
      return;
    }
    else
    {
      // For period with first
      WriteIntervals(theTK, theInter, theNbInt, anIndex1,
        theNb, theFPer * thePeriod, Standard_True, theT, aFinalIntervals, theNbIntervals, aCurInt);
      
      Standard_Integer aNbPer = Abs(theLPer - theFPer);
      Standard_Integer aCurPer = theFPer + 1;
      while (aNbPer > 1)
      {
        theNbIntervals++;
        WriteIntervals(theTK, theInter, theNbInt, 1,
          theNb, aCurPer * thePeriod, Standard_True, theT, aFinalIntervals, theNbIntervals, aCurInt);

        aNbPer--;
        aCurPer++;
      }
      // For period with last
      theNbIntervals++;
      WriteIntervals(theTK, theInter, theNbInt, 1,
        anIndex2, theLPer * thePeriod, Standard_False, theT, aFinalIntervals, theNbIntervals, aCurInt);
      return;
    }
  }
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void GeomAdaptor_Curve::Intervals(TColStd_Array1OfReal& T,
                                  const GeomAbs_Shape S   ) const
{
  Standard_Integer myNbIntervals = 1;
  Standard_Integer NbSplit;
  Standard_Real FirstParam = myFirst, LastParam = myLast;

  if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    Standard_Integer FirstIndex = myBSplineCurve->FirstUKnotIndex();
    Standard_Integer LastIndex = myBSplineCurve->LastUKnotIndex();
    TColStd_Array1OfInteger Inter(1, LastIndex - FirstIndex + 1);
    Standard_Boolean aContPer = (S >= Continuity()) && myBSplineCurve->IsPeriodic();
    Standard_Boolean aContNotPer = (S > Continuity()) && !myBSplineCurve->IsPeriodic();

    if (aContPer || aContNotPer) {
      Standard_Integer Cont;
      switch (S) {
      case GeomAbs_G1:
      case GeomAbs_G2:
        throw Standard_DomainError("Geom2dAdaptor_Curve::NbIntervals");
        break;
      case GeomAbs_C0:
        myNbIntervals = 1;
        break;
      case GeomAbs_C1:
      case GeomAbs_C2:
      case GeomAbs_C3:
      case GeomAbs_CN:
      {
        if (S == GeomAbs_C1) Cont = 1;
        else if (S == GeomAbs_C2) Cont = 2;
        else if (S == GeomAbs_C3) Cont = 3;
        else                       Cont = myBSplineCurve->Degree();
        Standard_Integer Degree = myBSplineCurve->Degree();
        Standard_Integer NbKnots = myBSplineCurve->NbKnots();
        TColStd_Array1OfInteger Mults(1, NbKnots);
        myBSplineCurve->Multiplicities(Mults);
        NbSplit = 1;
        Standard_Integer Index = FirstIndex;
        Inter(NbSplit) = Index;
        Index++;
        NbSplit++;
        while (Index < LastIndex)
        {
          if (Degree - Mults(Index) < Cont)
          {
            Inter(NbSplit) = Index;
            NbSplit++;
          }
          Index++;
        }
        Inter(NbSplit) = Index;
        Standard_Integer NbInt = NbSplit - 1;
        //        GeomConvert_BSplineCurveKnotSplitting Convector(myBspl, Cont);
        //        Standard_Integer NbInt = Convector.NbSplits()-1;
        //        TColStd_Array1OfInteger Inter(1,NbInt+1);
        //        Convector.Splitting( Inter);

        Standard_Integer Nb = myBSplineCurve->NbKnots();
        Standard_Integer Index1 = 0;
        Standard_Integer Index2 = 0;
        Standard_Real newFirst, newLast;
        const TColStd_Array1OfReal& TK = myBSplineCurve->Knots();
        const TColStd_Array1OfInteger& TM = myBSplineCurve->Multiplicities();
        Standard_Real Eps = Min(Resolution(Precision::Confusion()),
          Precision::PConfusion());

        if (!myBSplineCurve->IsPeriodic() || ((Abs(myFirst - myBSplineCurve->FirstParameter()) < Eps) &&
          (Abs(myLast - myBSplineCurve->LastParameter()) < Eps)))
        {
          BSplCLib::LocateParameter(myBSplineCurve->Degree(), TK, TM, myFirst,
            myBSplineCurve->IsPeriodic(),
            1, Nb, Index1, newFirst);
          BSplCLib::LocateParameter(myBSplineCurve->Degree(), TK, TM, myLast,
            myBSplineCurve->IsPeriodic(),
            1, Nb, Index2, newLast);
          FirstParam = newFirst;
          LastParam = newLast;
          // Protection against myFirst = UFirst - eps, which located as ULast - eps
          if (myBSplineCurve->IsPeriodic() && (LastParam - FirstParam) < Precision::PConfusion())
          {
            if (Abs(LastParam - myBSplineCurve->FirstParameter()) < Precision::PConfusion())
              LastParam += myBSplineCurve->Period();
            else
              FirstParam -= myBSplineCurve->Period();
          }
          // On decale eventuellement les indices  
          // On utilise une "petite" tolerance, la resolution ne doit 
          // servir que pour les tres longue courbes....(PRO9248)

          if (Abs(FirstParam - TK(Index1 + 1)) < Eps) Index1++;
          if (LastParam - TK(Index2) > Eps) Index2++;

          myNbIntervals = 1;

          TColStd_Array1OfInteger aFinalIntervals(1, Inter.Upper());
          aFinalIntervals(1) = Index1;
          for (Standard_Integer i = 1; i <= NbInt; i++) {
            if (Inter(i) > Index1 && Inter(i) < Index2) {
              myNbIntervals++;
              aFinalIntervals(myNbIntervals) = Inter(i);
            }
          }
          aFinalIntervals(myNbIntervals + 1) = Index2;

          for (Standard_Integer I = 1; I <= myNbIntervals + 1; I++) {
            T(I) = TK(aFinalIntervals(I));
          }
        }
        else
        {
          Standard_Real aFirst = myFirst;
          Standard_Real aLast = myLast;

          Standard_Real aCurFirst = aFirst;
          Standard_Real aCurLast = aLast;

          Standard_Real aPeriod = myBSplineCurve->Period();
          Standard_Real aLower = myBSplineCurve->FirstParameter();
          Standard_Real anUpper = myBSplineCurve->LastParameter();

          Standard_Integer aLPer = 0; Standard_Integer aFPer = 0;

          if (Abs(myFirst - aLower) <= Eps)
          {
            aCurFirst = aLower;
            aFirst = aCurFirst;
          }

          if (Abs(myLast - anUpper) <= Eps)
          {
            aCurLast = anUpper;
            aLast = aCurLast;
          }

          if ((Abs(aLower - myFirst) < Eps) && (aCurFirst < aLower))
          {
            aCurFirst = aLower;
          }
          else
          {
            DefinFPeriod(aLower, anUpper,
              Eps, aPeriod, aCurFirst, aFPer);
          }
          DefinLPeriod(aLower, anUpper,
            Eps, aPeriod, aCurLast, aLPer);

          if (myFirst == aLower)
          {
            aFPer = 0;
          }

          SpreadInt(TK, TM, Inter, myBSplineCurve->Degree(), Nb, aFPer, aLPer, NbInt, aLower, myFirst, myLast, aPeriod,
            aCurLast, Eps, T, myNbIntervals);

          T(T.Lower()) = aFirst;
          T(T.Lower() + myNbIntervals) = aLast;
          return;
        }
      }
      T(T.Lower()) = myFirst;
      T(T.Lower() + myNbIntervals) = myLast;
      return;
      }
    }
  }

  else if (myTypeCurve == GeomAbs_OffsetCurve){
    GeomAbs_Shape BaseS=GeomAbs_C0;
    switch(S){
    case GeomAbs_G1:
    case GeomAbs_G2:
      throw Standard_DomainError("GeomAdaptor_Curve::NbIntervals");
      break;
    case GeomAbs_C0: BaseS = GeomAbs_C1; break;
    case GeomAbs_C1: BaseS = GeomAbs_C2; break;
    case GeomAbs_C2: BaseS = GeomAbs_C3; break;
    default: BaseS = GeomAbs_CN;
    }
    GeomAdaptor_Curve C
      (Handle(Geom_OffsetCurve)::DownCast (myCurve)->BasisCurve());
    // akm 05/04/02 (OCC278)  If our curve is trimmed we must recalculate 
    //                    the array of intervals obtained from the basis to
    //              vvv   reflect parameter bounds
    Standard_Integer iNbBasisInt = C.NbIntervals(BaseS), iInt;
    if (iNbBasisInt>1)
    {
      TColStd_Array1OfReal rdfInter(1,1+iNbBasisInt);
      C.Intervals(rdfInter,BaseS);
      for (iInt=1; iInt<=iNbBasisInt; iInt++)
	if (rdfInter(iInt)>myFirst && rdfInter(iInt)<myLast)
	  T(++myNbIntervals)=rdfInter(iInt);
    }
    // old - myNbIntervals = C.NbIntervals(BaseS);
    // old - C.Intervals(T, BaseS);
    // akm 05/04/02 ^^^
  }
  
  T( T.Lower() ) = FirstParam;
  T( T.Lower() + myNbIntervals ) = LastParam;
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_Curve) GeomAdaptor_Curve::Trim(const Standard_Real First,
                                                 const Standard_Real Last,
                                                 const Standard_Real /*Tol*/) const
{
  return Handle(GeomAdaptor_Curve)(new GeomAdaptor_Curve(myCurve,First,Last));
}


//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_Curve::IsClosed() const
{
  if (!Precision::IsPositiveInfinite(myLast) &&
      !Precision::IsNegativeInfinite(myFirst))
  {
    const gp_Pnt Pd = Value(myFirst);
    const gp_Pnt Pf = Value(myLast);
    return (Pd.Distance(Pf) <= Precision::Confusion());
  }
  return Standard_False;
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_Curve::IsPeriodic() const 
{
  return myCurve->IsPeriodic();
}

//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_Curve::Period() const 
{
  return myCurve->LastParameter() - myCurve->FirstParameter();
}

//=======================================================================
//function : RebuildCache
//purpose  : 
//=======================================================================
void GeomAdaptor_Curve::RebuildCache(const Standard_Real theParameter) const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
  {
    // Create cache for Bezier
    Handle(Geom_BezierCurve) aBezier = Handle(Geom_BezierCurve)::DownCast(myCurve);
    Standard_Integer aDeg = aBezier->Degree();
    TColStd_Array1OfReal aFlatKnots(BSplCLib::FlatBezierKnots(aDeg), 1, 2 * (aDeg + 1));
    if (myCurveCache.IsNull())
      myCurveCache = new BSplCLib_Cache(aDeg, aBezier->IsPeriodic(), aFlatKnots,
        aBezier->Poles(), aBezier->Weights());
    myCurveCache->BuildCache (theParameter, aFlatKnots, aBezier->Poles(), aBezier->Weights());
}
  else if (myTypeCurve == GeomAbs_BSplineCurve)
{
    // Create cache for B-spline
    if (myCurveCache.IsNull())
      myCurveCache = new BSplCLib_Cache(myBSplineCurve->Degree(), myBSplineCurve->IsPeriodic(),
        myBSplineCurve->KnotSequence(), myBSplineCurve->Poles(), myBSplineCurve->Weights());
    myCurveCache->BuildCache (theParameter, myBSplineCurve->KnotSequence(),
                              myBSplineCurve->Poles(), myBSplineCurve->Weights());
}
}

//=======================================================================
//function : IsBoundary
//purpose  : 
//=======================================================================
Standard_Boolean GeomAdaptor_Curve::IsBoundary(const Standard_Real theU,
                                               Standard_Integer& theSpanStart,
                                               Standard_Integer& theSpanFinish) const
{
  if (!myBSplineCurve.IsNull() && (theU == myFirst || theU == myLast))
  {
    if (theU == myFirst)
    {
      myBSplineCurve->LocateU(myFirst, PosTol, theSpanStart, theSpanFinish);
      if (theSpanStart < 1)
        theSpanStart = 1;
      if (theSpanStart >= theSpanFinish)
        theSpanFinish = theSpanStart + 1;
    }
    else if (theU == myLast)
    {
      myBSplineCurve->LocateU(myLast, PosTol, theSpanStart, theSpanFinish);
      if (theSpanFinish > myBSplineCurve->NbKnots())
        theSpanFinish = myBSplineCurve->NbKnots();
      if (theSpanStart >= theSpanFinish)
        theSpanStart = theSpanFinish - 1;
    }
    return Standard_True;
  }
  return Standard_False;
  }

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt GeomAdaptor_Curve::Value(const Standard_Real U) const
{
  gp_Pnt aValue;
  D0(U, aValue);
  return aValue;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void GeomAdaptor_Curve::D0(const Standard_Real U, gp_Pnt& P) const
{
  switch (myTypeCurve)
{
  case GeomAbs_BezierCurve:
  case GeomAbs_BSplineCurve:
  {
    Standard_Integer aStart = 0, aFinish = 0;
    if (IsBoundary(U, aStart, aFinish))
    {
      myBSplineCurve->LocalD0(U, aStart, aFinish, P);
    }
    else
  {
      // use cached data
      if (myCurveCache.IsNull() || !myCurveCache->IsCacheValid(U))
        RebuildCache(U);
      myCurveCache->D0(U, P);
  }
    break;
}

  case GeomAbs_OffsetCurve:
    myNestedEvaluator->D0(U, P);
    break;

  default:
    myCurve->D0(U, P);
}
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void GeomAdaptor_Curve::D1(const Standard_Real U, gp_Pnt& P, gp_Vec& V) const 
{
  switch (myTypeCurve)
{
  case GeomAbs_BezierCurve:
  case GeomAbs_BSplineCurve:
  {
    Standard_Integer aStart = 0, aFinish = 0;
    if (IsBoundary(U, aStart, aFinish))
    {
      myBSplineCurve->LocalD1(U, aStart, aFinish, P, V);
    }
    else
  {
      // use cached data
      if (myCurveCache.IsNull() || !myCurveCache->IsCacheValid(U))
        RebuildCache(U);
      myCurveCache->D1(U, P, V);
  }
    break;
}

  case GeomAbs_OffsetCurve:
    myNestedEvaluator->D1(U, P, V);
    break;

  default:
    myCurve->D1(U, P, V);
}
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void GeomAdaptor_Curve::D2(const Standard_Real U, 
                           gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const 
{
  switch (myTypeCurve)
{
  case GeomAbs_BezierCurve:
  case GeomAbs_BSplineCurve:
  {
    Standard_Integer aStart = 0, aFinish = 0;
    if (IsBoundary(U, aStart, aFinish))
    {
      myBSplineCurve->LocalD2(U, aStart, aFinish, P, V1, V2);
    }
    else
  {
      // use cached data
      if (myCurveCache.IsNull() || !myCurveCache->IsCacheValid(U))
        RebuildCache(U);
      myCurveCache->D2(U, P, V1, V2);
  }
    break;
}

  case GeomAbs_OffsetCurve:
    myNestedEvaluator->D2(U, P, V1, V2);
    break;

  default:
    myCurve->D2(U, P, V1, V2);
}
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void GeomAdaptor_Curve::D3(const Standard_Real U, 
                           gp_Pnt& P, gp_Vec& V1, 
                           gp_Vec& V2, gp_Vec& V3) const 
{
  switch (myTypeCurve)
{
  case GeomAbs_BezierCurve:
  case GeomAbs_BSplineCurve:
  {
    Standard_Integer aStart = 0, aFinish = 0;
    if (IsBoundary(U, aStart, aFinish))
    {
      myBSplineCurve->LocalD3(U, aStart, aFinish, P, V1, V2, V3);
    }
    else
  {
      // use cached data
      if (myCurveCache.IsNull() || !myCurveCache->IsCacheValid(U))
        RebuildCache(U);
      myCurveCache->D3(U, P, V1, V2, V3);
  }
    break;
}

  case GeomAbs_OffsetCurve:
    myNestedEvaluator->D3(U, P, V1, V2, V3);
    break;

  default:
    myCurve->D3(U, P, V1, V2, V3);
}
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec GeomAdaptor_Curve::DN(const Standard_Real    U, 
                             const Standard_Integer N) const 
{
  switch (myTypeCurve)
{
  case GeomAbs_BezierCurve:
  case GeomAbs_BSplineCurve:
  {
    Standard_Integer aStart = 0, aFinish = 0;
    if (IsBoundary(U, aStart, aFinish))
    {
      return myBSplineCurve->LocalDN(U, aStart, aFinish, N);
    }
    else
      return myCurve->DN (U, N);
  }

  case GeomAbs_OffsetCurve:
    return myNestedEvaluator->DN(U, N);
    break;

  default: // to eliminate gcc warning
    break;
  }
  return myCurve->DN(U, N);
}

//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

Standard_Real GeomAdaptor_Curve::Resolution(const Standard_Real R3D) const
{
  switch ( myTypeCurve) {
  case GeomAbs_Line :
    return R3D;
  case GeomAbs_Circle: {
    Standard_Real R = Handle(Geom_Circle)::DownCast (myCurve)->Circ().Radius();
    if ( R > R3D/2. )
      return 2*ASin(R3D/(2*R));
    else
      return 2*M_PI;
  }
  case GeomAbs_Ellipse: {
    return R3D / Handle(Geom_Ellipse)::DownCast (myCurve)->MajorRadius();
  }
  case GeomAbs_BezierCurve: {
    Standard_Real res;
    Handle(Geom_BezierCurve)::DownCast (myCurve)->Resolution(R3D,res);
    return res;
  }
  case GeomAbs_BSplineCurve: {
    Standard_Real res;
    myBSplineCurve->Resolution(R3D,res);
    return res;
  }
  default:
    return Precision::Parametric(R3D);
  }  
}


//    --
//    --     The following methods must  be called when GetType returned
//    --     the corresponding type.
//    --     

//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

gp_Lin GeomAdaptor_Curve::Line() const 
{
  Standard_NoSuchObject_Raise_if (myTypeCurve != GeomAbs_Line,
                                  "GeomAdaptor_Curve::Line() - curve is not a Line");
  return Handle(Geom_Line)::DownCast (myCurve)->Lin();  
}

//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ  GeomAdaptor_Curve::Circle() const 
{
  Standard_NoSuchObject_Raise_if (myTypeCurve != GeomAbs_Circle,
                                  "GeomAdaptor_Curve::Circle() - curve is not a Circle");
  return Handle(Geom_Circle)::DownCast (myCurve)->Circ();
}

//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips GeomAdaptor_Curve::Ellipse() const 
{
  Standard_NoSuchObject_Raise_if (myTypeCurve != GeomAbs_Ellipse,
                                  "GeomAdaptor_Curve::Ellipse() - curve is not an Ellipse");
  return Handle(Geom_Ellipse)::DownCast (myCurve)->Elips();
}

//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr GeomAdaptor_Curve::Hyperbola() const 
{
  Standard_NoSuchObject_Raise_if (myTypeCurve != GeomAbs_Hyperbola,
                                  "GeomAdaptor_Curve::Hyperbola() - curve is not a Hyperbola");
  return Handle(Geom_Hyperbola)::DownCast (myCurve)->Hypr();  
}

//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

gp_Parab GeomAdaptor_Curve::Parabola() const 
{
  Standard_NoSuchObject_Raise_if (myTypeCurve != GeomAbs_Parabola,
                                  "GeomAdaptor_Curve::Parabola() - curve is not a Parabola");
  return Handle(Geom_Parabola)::DownCast (myCurve)->Parab();
}

//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Curve::Degree() const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
    return Handle(Geom_BezierCurve)::DownCast (myCurve)->Degree();
  else if (myTypeCurve == GeomAbs_BSplineCurve)
    return myBSplineCurve->Degree();
  else
    throw Standard_NoSuchObject();
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean GeomAdaptor_Curve::IsRational() const {
  switch( myTypeCurve) {
  case GeomAbs_BSplineCurve:
    return myBSplineCurve->IsRational();
  case GeomAbs_BezierCurve:
    return Handle(Geom_BezierCurve)::DownCast (myCurve)->IsRational();
  default:
    return Standard_False;
  }
}

//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Curve::NbPoles() const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
    return Handle(Geom_BezierCurve)::DownCast (myCurve)->NbPoles();
  else if (myTypeCurve == GeomAbs_BSplineCurve)
    return myBSplineCurve->NbPoles();
  else
    throw Standard_NoSuchObject();
}

//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Curve::NbKnots() const
{
  if ( myTypeCurve != GeomAbs_BSplineCurve)
    throw Standard_NoSuchObject("GeomAdaptor_Curve::NbKnots");
  return myBSplineCurve->NbKnots();
}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierCurve) GeomAdaptor_Curve::Bezier() const 
{
 if ( myTypeCurve != GeomAbs_BezierCurve)
    throw Standard_NoSuchObject("GeomAdaptor_Curve::Bezier");
  return Handle(Geom_BezierCurve)::DownCast (myCurve);
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) GeomAdaptor_Curve::BSpline() const 
{
 if ( myTypeCurve != GeomAbs_BSplineCurve)
    throw Standard_NoSuchObject("GeomAdaptor_Curve::BSpline");

  return myBSplineCurve;
}

//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Geom_OffsetCurve) GeomAdaptor_Curve::OffsetCurve() const
{
  if ( myTypeCurve != GeomAbs_OffsetCurve)
    throw Standard_NoSuchObject("GeomAdaptor_Curve::OffsetCurve");
  return Handle(Geom_OffsetCurve)::DownCast(myCurve);
}
