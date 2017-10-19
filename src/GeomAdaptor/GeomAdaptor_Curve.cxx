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


#include <Adaptor3d_HCurve.hxx>
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
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomEvaluator_OffsetCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>

//#include <GeomConvert_BSplineCurveKnotSplitting.hxx>
static const Standard_Real PosTol = Precision::PConfusion() / 2;


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

  if ( Index2 - Index1 <= 0) {
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
      Handle(GeomAdaptor_HCurve) aBaseAdaptor = new GeomAdaptor_HCurve(aBaseCurve);
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
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer GeomAdaptor_Curve::NbIntervals(const GeomAbs_Shape S) const
{
  Standard_Integer myNbIntervals = 1;
  Standard_Integer NbSplit;
  if (myTypeCurve == GeomAbs_BSplineCurve) {
    Standard_Integer FirstIndex = myBSplineCurve->FirstUKnotIndex();
    Standard_Integer LastIndex  = myBSplineCurve->LastUKnotIndex();
    TColStd_Array1OfInteger Inter (1, LastIndex-FirstIndex+1);
    if ( S > Continuity()) {
      Standard_Integer Cont;
      switch ( S) {
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
          if      ( S == GeomAbs_C1) Cont = 1;
          else if ( S == GeomAbs_C2) Cont = 2;
          else if ( S == GeomAbs_C3) Cont = 3;
          else                       Cont = myBSplineCurve->Degree();
          Standard_Integer Degree = myBSplineCurve->Degree();
          Standard_Integer NbKnots = myBSplineCurve->NbKnots();
          TColStd_Array1OfInteger Mults (1, NbKnots);
          myBSplineCurve->Multiplicities (Mults);
          NbSplit = 1;
          Standard_Integer Index   = FirstIndex;
          Inter (NbSplit) = Index;
          Index++;
          NbSplit++;
          while (Index < LastIndex) 
            {
	      if (Degree - Mults (Index) < Cont) 
		{
		  Inter (NbSplit) = Index;
		  NbSplit++;
		}
	      Index++;
	    }
          Inter (NbSplit) = Index;

          Standard_Integer NbInt = NbSplit-1;
          
          Standard_Integer Nb = myBSplineCurve->NbKnots();
          Standard_Integer Index1 = 0;
          Standard_Integer Index2 = 0;
          Standard_Real newFirst, newLast;
          const TColStd_Array1OfReal& TK = myBSplineCurve->Knots();
          const TColStd_Array1OfInteger& TM = myBSplineCurve->Multiplicities();
          BSplCLib::LocateParameter(myBSplineCurve->Degree(),TK,TM,myFirst,
                                    myBSplineCurve->IsPeriodic(),
                                    1,Nb,Index1,newFirst);
          BSplCLib::LocateParameter(myBSplineCurve->Degree(),TK,TM,myLast,
                                    myBSplineCurve->IsPeriodic(),
                                    1,Nb,Index2,newLast);

          // On decale eventuellement les indices  
          // On utilise une "petite" tolerance, la resolution ne doit 
          // servir que pour les tres longue courbes....(PRO9248)
          Standard_Real Eps = Min(Resolution(Precision::Confusion()),
                                  Precision::PConfusion()); 
          if ( Abs(newFirst-TK(Index1+1))< Eps) Index1++;
          if ( newLast-TK(Index2)> Eps) Index2++;
          
          myNbIntervals = 1;
          for ( Standard_Integer i=1; i<=NbInt; i++)
            if (Inter(i)>Index1 && Inter(i)<Index2) myNbIntervals++;
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
      Standard_Integer LastIndex  = myBSplineCurve->LastUKnotIndex();
      TColStd_Array1OfInteger Inter (1, LastIndex-FirstIndex+1);
      
      if ( S > Continuity()) {
	Standard_Integer Cont;
	switch ( S) {
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
	    if      ( S == GeomAbs_C1) Cont = 1;
	    else if ( S == GeomAbs_C2) Cont = 2;
	    else if ( S == GeomAbs_C3) Cont = 3;
	    else                       Cont = myBSplineCurve->Degree();
	    Standard_Integer Degree = myBSplineCurve->Degree();
	    Standard_Integer NbKnots = myBSplineCurve->NbKnots();
	    TColStd_Array1OfInteger Mults (1, NbKnots);
	    myBSplineCurve->Multiplicities (Mults);
	    NbSplit = 1;
	    Standard_Integer Index   = FirstIndex;
	    Inter (NbSplit) = Index;
	    Index++;
	    NbSplit++;
	    while (Index < LastIndex) 
	      {
		if (Degree - Mults (Index) < Cont) 
		  {
                    Inter (NbSplit) = Index;
                    NbSplit++;
		  }
		Index++;
	      }
            Inter (NbSplit) = Index;
	    Standard_Integer NbInt = NbSplit-1;
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
	    BSplCLib::LocateParameter(myBSplineCurve->Degree(),TK,TM,myFirst,
				      myBSplineCurve->IsPeriodic(),
				      1,Nb,Index1,newFirst);
	    BSplCLib::LocateParameter(myBSplineCurve->Degree(),TK,TM,myLast,
				      myBSplineCurve->IsPeriodic(),
				      1,Nb,Index2,newLast);
            FirstParam = newFirst;
            LastParam = newLast;
	    // On decale eventuellement les indices  
	    // On utilise une "petite" tolerance, la resolution ne doit 
	    // servir que pour les tres longue courbes....(PRO9248)
	    Standard_Real Eps = Min(Resolution(Precision::Confusion()),
				    Precision::PConfusion()); 
	    if ( Abs(newFirst-TK(Index1+1))< Eps) Index1++;
	    if ( newLast-TK(Index2)> Eps) Index2++;
	    
	    Inter( 1) = Index1;
	    myNbIntervals = 1;
	    for ( Standard_Integer i=1; i<=NbInt; i++) {
	      if (Inter(i) > Index1 && Inter(i)<Index2 ) {
		myNbIntervals++;
		Inter(myNbIntervals) = Inter(i);
	      }
	    }
	    Inter(myNbIntervals+1) = Index2;
	    
	    for (Standard_Integer I=1;I<=myNbIntervals+1;I++) {
	      T(I) = TK(Inter(I));
	    }
	  }
	  break;
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

Handle(Adaptor3d_HCurve) GeomAdaptor_Curve::Trim(const Standard_Real First,
                                                 const Standard_Real Last,
                                                 const Standard_Real /*Tol*/) const
{
  return Handle(GeomAdaptor_HCurve)(new GeomAdaptor_HCurve(myCurve,First,Last));
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
    myCurveCache->BuildCache(theParameter, aDeg, aBezier->IsPeriodic(), aFlatKnots,
        aBezier->Poles(), aBezier->Weights());
}
  else if (myTypeCurve == GeomAbs_BSplineCurve)
{
    // Create cache for B-spline
    if (myCurveCache.IsNull())
      myCurveCache = new BSplCLib_Cache(myBSplineCurve->Degree(), myBSplineCurve->IsPeriodic(),
        myBSplineCurve->KnotSequence(), myBSplineCurve->Poles(), myBSplineCurve->Weights());
    myCurveCache->BuildCache(theParameter, myBSplineCurve->Degree(),
        myBSplineCurve->IsPeriodic(), myBSplineCurve->KnotSequence(),
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
