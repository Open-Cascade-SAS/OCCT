// Created on: 1993-06-04
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
// 10/07/97 : PMN -> Pas de calcul de resolution dans Nb(Intervals) (PRO9248)
// 20/10/97 : JPI -> traitement des offset curves

#define No_Standard_RangeError
#define No_Standard_OutOfRange


#include <Adaptor2d_HCurve2d.hxx>
#include <BSplCLib.hxx>
#include <BSplCLib_Cache.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_UndefinedDerivative.hxx>
#include <Geom2d_UndefinedValue.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dEvaluator_OffsetCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>

//#include <Geom2dConvert_BSplineCurveKnotSplitting.hxx>
static const Standard_Real PosTol = Precision::PConfusion() / 2;


//=======================================================================
//function : LocalContinuity
//purpose  : Computes the Continuity of a BSplineCurve 
//           between the parameters U1 and U2
//           The continuity is C(d-m) 
//             with   d = degree, 
//                    m = max multiplicity of the Knots between U1 and U2
//=======================================================================

GeomAbs_Shape Geom2dAdaptor_Curve::LocalContinuity(const Standard_Real U1, 
						   const Standard_Real U2) const
{
       Standard_NoSuchObject_Raise_if(myTypeCurve!=GeomAbs_BSplineCurve," ");
       Standard_Integer Nb = myBSplineCurve->NbKnots();
       Standard_Integer Index1 = 0;
       Standard_Integer Index2 = 0;
       Standard_Real newFirst, newLast;
       TColStd_Array1OfReal    TK(1,Nb);
       TColStd_Array1OfInteger TM(1,Nb);
       myBSplineCurve->Knots(TK);
       myBSplineCurve->Multiplicities(TM);
       BSplCLib::LocateParameter(myBSplineCurve->Degree(),TK,TM,U1,myBSplineCurve->IsPeriodic(),
				 1,Nb,Index1,newFirst);
       BSplCLib::LocateParameter(myBSplineCurve->Degree(),TK,TM,U2,myBSplineCurve->IsPeriodic(),
				 1,Nb,Index2,newLast);
       if ( Abs(newFirst-TK(Index1+1))<Precision::PConfusion()) { 
	 if (Index1 < Nb)Index1++;
       }
       if ( Abs(newLast-TK(Index2))<Precision::PConfusion())
	 Index2--;
       Standard_Integer MultMax;
       // attention aux courbes peridiques.
       if ( myBSplineCurve->IsPeriodic() && Index1 == Nb )
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
//function : Geom2dAdaptor_Curve
//purpose  : 
//=======================================================================

Geom2dAdaptor_Curve::Geom2dAdaptor_Curve()
: myTypeCurve(GeomAbs_OtherCurve),
  myFirst    (0.0),
  myLast     (0.0)
{
}

//=======================================================================
//function : Geom2dAdaptor_Curve
//purpose  : 
//=======================================================================

Geom2dAdaptor_Curve::Geom2dAdaptor_Curve(const Handle(Geom2d_Curve)& theCrv)
: myTypeCurve(GeomAbs_OtherCurve),
  myFirst    (0.0),
  myLast     (0.0)
{
  Load(theCrv);
}

//=======================================================================
//function : Geom2dAdaptor_Curve
//purpose  : 
//=======================================================================

Geom2dAdaptor_Curve::Geom2dAdaptor_Curve(const Handle(Geom2d_Curve)& theCrv,
                                         const Standard_Real theUFirst,
                                         const Standard_Real theULast)
: myTypeCurve(GeomAbs_OtherCurve),
  myFirst    (theUFirst),
  myLast     (theULast)
{
  Load(theCrv, theUFirst, theULast);
}


//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Geom2dAdaptor_Curve::load(const Handle(Geom2d_Curve)& C,
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

    Handle(Standard_Type) TheType = C->DynamicType();
    if ( TheType == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
      Load(Handle(Geom2d_TrimmedCurve)::DownCast (C)->BasisCurve(),
	   UFirst,ULast);
    }
    else if ( TheType ==  STANDARD_TYPE(Geom2d_Circle)) {
      myTypeCurve = GeomAbs_Circle;
    }
    else if ( TheType ==STANDARD_TYPE(Geom2d_Line)) {
      myTypeCurve = GeomAbs_Line;
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_Ellipse)) {
      myTypeCurve = GeomAbs_Ellipse;
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_Parabola)) {
      myTypeCurve = GeomAbs_Parabola;
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_Hyperbola)) {
      myTypeCurve = GeomAbs_Hyperbola;
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_BezierCurve)) {
      myTypeCurve = GeomAbs_BezierCurve;
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_BSplineCurve)) {
      myTypeCurve = GeomAbs_BSplineCurve;
      myBSplineCurve = Handle(Geom2d_BSplineCurve)::DownCast(myCurve);
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_OffsetCurve))
    {
      myTypeCurve = GeomAbs_OffsetCurve;
      Handle(Geom2d_OffsetCurve) anOffsetCurve = Handle(Geom2d_OffsetCurve)::DownCast(myCurve);
      // Create nested adaptor for base curve
      Handle(Geom2d_Curve) aBaseCurve = anOffsetCurve->BasisCurve();
      Handle(Geom2dAdaptor_HCurve) aBaseAdaptor = new Geom2dAdaptor_HCurve(aBaseCurve);
      myNestedEvaluator = new Geom2dEvaluator_OffsetCurve(aBaseAdaptor, anOffsetCurve->Offset());
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

GeomAbs_Shape Geom2dAdaptor_Curve::Continuity() const 
{
  if (myTypeCurve == GeomAbs_BSplineCurve) {
    return LocalContinuity(myFirst, myLast);
  }
  else if (myTypeCurve == GeomAbs_OffsetCurve){
    GeomAbs_Shape S = 
      Handle(Geom2d_OffsetCurve)::DownCast (myCurve)->GetBasisCurveContinuity(); 
    switch(S){
    case GeomAbs_CN: return GeomAbs_CN;
    case GeomAbs_C3: return GeomAbs_C2;
    case GeomAbs_C2: return GeomAbs_C1;
    case GeomAbs_C1: return GeomAbs_C0;  
    case GeomAbs_G1: return GeomAbs_G1;
    case GeomAbs_G2: return GeomAbs_G2;

    default:
      throw Standard_NoSuchObject("Geom2dAdaptor_Curve::Continuity");
    }
  }

  else if (myTypeCurve == GeomAbs_OtherCurve) {
    throw Standard_NoSuchObject("Geom2dAdaptor_Curve::Continuity");
  }
  else {
    return GeomAbs_CN;
  }
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer Geom2dAdaptor_Curve::NbIntervals(const GeomAbs_Shape S) const
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
	  
	  Standard_Integer Nb = myBSplineCurve->NbKnots();
	  Standard_Integer Index1 = 0;
	  Standard_Integer Index2 = 0;
	  Standard_Real newFirst, newLast;
	  TColStd_Array1OfReal    TK(1,Nb);
	  TColStd_Array1OfInteger TM(1,Nb);
	  myBSplineCurve->Knots(TK);
	  myBSplineCurve->Multiplicities(TM);
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
    Geom2dAdaptor_Curve anAdaptor( Handle(Geom2d_OffsetCurve)::DownCast(myCurve)->BasisCurve() );
    myNbIntervals = anAdaptor.NbIntervals(BaseS);
  }

  return myNbIntervals;
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void Geom2dAdaptor_Curve::Intervals(TColStd_Array1OfReal& T,
				    const GeomAbs_Shape S   ) const 
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
	  
	  Standard_Integer Nb = myBSplineCurve->NbKnots();
	  Standard_Integer Index1 = 0;
	  Standard_Integer Index2 = 0;
	  Standard_Real newFirst, newLast;
	  TColStd_Array1OfReal    TK(1,Nb);
	  TColStd_Array1OfInteger TM(1,Nb);
	  myBSplineCurve->Knots(TK);
	  myBSplineCurve->Multiplicities(TM);
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
	  
	  Inter( 1) = Index1;
	  myNbIntervals = 1;
	  for ( Standard_Integer i=1; i<=NbInt; i++) {
	    if (Inter(i) > Index1 && Inter(i)<Index2 ) {
	      myNbIntervals++;
	      Inter(myNbIntervals) = Inter(i);
	    }
	  }
	  Inter(myNbIntervals+1) = Index2;
	  
	  Standard_Integer ii = T.Lower() - 1;
	  for (Standard_Integer I=1;I<=myNbIntervals+1;I++) {
	    T(ii + I) = TK(Inter(I));
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

    Geom2dAdaptor_Curve anAdaptor( Handle(Geom2d_OffsetCurve)::DownCast(myCurve)->BasisCurve() );
    myNbIntervals = anAdaptor.NbIntervals(BaseS);
    anAdaptor.Intervals(T, BaseS);
  }

  T( T.Lower() ) = myFirst;
  T( T.Lower() + myNbIntervals ) = myLast;
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Adaptor2d_HCurve2d) Geom2dAdaptor_Curve::Trim
(const Standard_Real First,
 const Standard_Real Last,
// const Standard_Real Tol) const 
 const Standard_Real ) const 
{
  Handle(Geom2dAdaptor_HCurve) HE = new Geom2dAdaptor_HCurve(myCurve,First,Last);
  return HE;
}


//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom2dAdaptor_Curve::IsClosed() const 
{
  if (!Precision::IsPositiveInfinite(myLast) &&
      !Precision::IsNegativeInfinite(myFirst)) {
    gp_Pnt2d Pd = Value(myFirst);
    gp_Pnt2d Pf = Value(myLast);
    return ( Pd.Distance(Pf) <= Precision::Confusion());
  }
  else
    return Standard_False;
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom2dAdaptor_Curve::IsPeriodic() const 
{
  return myCurve->IsPeriodic();
}

//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real Geom2dAdaptor_Curve::Period() const 
{
  return myCurve->LastParameter() - myCurve->FirstParameter();
}

//=======================================================================
//function : RebuildCache
//purpose  : 
//=======================================================================
void Geom2dAdaptor_Curve::RebuildCache(const Standard_Real theParameter) const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
  {
    // Create cache for Bezier
    Handle(Geom2d_BezierCurve) aBezier = Handle(Geom2d_BezierCurve)::DownCast(myCurve);
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
Standard_Boolean Geom2dAdaptor_Curve::IsBoundary(const Standard_Real theU,
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

gp_Pnt2d Geom2dAdaptor_Curve::Value(const Standard_Real U) const 
{
  gp_Pnt2d aRes;
  D0(U, aRes);
  return aRes;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom2dAdaptor_Curve::D0(const Standard_Real U, gp_Pnt2d& P) const
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

void Geom2dAdaptor_Curve::D1(const Standard_Real U, 
                             gp_Pnt2d& P, gp_Vec2d& V) const 
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

void Geom2dAdaptor_Curve::D2(const Standard_Real U, 
                             gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const 
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

void Geom2dAdaptor_Curve::D3(const Standard_Real U, 
                             gp_Pnt2d& P,  gp_Vec2d& V1, 
                             gp_Vec2d& V2, gp_Vec2d& V3) const 
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

gp_Vec2d Geom2dAdaptor_Curve::DN(const Standard_Real U, 
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
      myBSplineCurve->LocalDN(U, aStart, aFinish, N);
    }
    else
      return myCurve->DN(U, N);
    break;
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

Standard_Real Geom2dAdaptor_Curve::Resolution(const Standard_Real Ruv) const {
  switch ( myTypeCurve) {
  case GeomAbs_Line :
    return Ruv;
  case GeomAbs_Circle: {
    Standard_Real R = Handle(Geom2d_Circle)::DownCast (myCurve)->Circ2d().Radius();
    if ( R > Ruv/2.)
      return 2*ASin(Ruv/(2*R));
    else
      return 2*M_PI;
  }
  case GeomAbs_Ellipse: {
    return Ruv / Handle(Geom2d_Ellipse)::DownCast (myCurve)->MajorRadius();
  }
  case GeomAbs_BezierCurve: {
    Standard_Real res;
    Handle(Geom2d_BezierCurve)::DownCast (myCurve)->Resolution(Ruv,res);
    return res;
  }
  case GeomAbs_BSplineCurve: {
    Standard_Real res;
    Handle(Geom2d_BSplineCurve)::DownCast (myCurve)->Resolution(Ruv,res);
    return res;
  }
  default:
    return Precision::Parametric(Ruv);
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

gp_Lin2d Geom2dAdaptor_Curve::Line() const 
{
  Standard_NoSuchObject_Raise_if (myTypeCurve != GeomAbs_Line,
                                  "Geom2dAdaptor_Curve::Line() - curve is not a Line");
  return Handle(Geom2d_Line)::DownCast (myCurve)->Lin2d();
}

//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ2d  Geom2dAdaptor_Curve::Circle() const 
{
  Standard_NoSuchObject_Raise_if (myTypeCurve != GeomAbs_Circle,
                                  "Geom2dAdaptor_Curve::Circle() - curve is not a Circle");
  return Handle(Geom2d_Circle)::DownCast (myCurve)->Circ2d();
}

//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips2d Geom2dAdaptor_Curve::Ellipse() const 
{
  Standard_NoSuchObject_Raise_if (myTypeCurve != GeomAbs_Ellipse,
                                  "Geom2dAdaptor_Curve::Ellipse() - curve is not an Ellipse");
  return Handle(Geom2d_Ellipse)::DownCast (myCurve)->Elips2d();
}

//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr2d Geom2dAdaptor_Curve::Hyperbola() const 
{
  Standard_NoSuchObject_Raise_if (myTypeCurve != GeomAbs_Hyperbola,
                                  "Geom2dAdaptor_Curve::Hyperbola() - curve is not a Hyperbola");
  return Handle(Geom2d_Hyperbola)::DownCast (myCurve)->Hypr2d();
}

//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

gp_Parab2d Geom2dAdaptor_Curve::Parabola() const 
{
  Standard_NoSuchObject_Raise_if (myTypeCurve != GeomAbs_Parabola,
                                  "Geom2dAdaptor_Curve::Parabola() - curve is not a Parabola");
  return Handle(Geom2d_Parabola)::DownCast (myCurve)->Parab2d();
}

//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer Geom2dAdaptor_Curve::Degree() const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
    return Handle(Geom2d_BezierCurve)::DownCast (myCurve)->Degree();
  else if (myTypeCurve == GeomAbs_BSplineCurve)
    return myBSplineCurve->Degree();
  else
    throw Standard_NoSuchObject();
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean Geom2dAdaptor_Curve::IsRational() const {
  switch( myTypeCurve) {
  case GeomAbs_BSplineCurve:
    return myBSplineCurve->IsRational();
  case GeomAbs_BezierCurve:
    return Handle(Geom2d_BezierCurve)::DownCast (myCurve)->IsRational();
  default:
    return Standard_False;
  }
}

//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer Geom2dAdaptor_Curve::NbPoles() const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
    return Handle(Geom2d_BezierCurve)::DownCast (myCurve)->NbPoles();
  else if (myTypeCurve == GeomAbs_BSplineCurve)
    return myBSplineCurve->NbPoles();
  else
    throw Standard_NoSuchObject();
}

//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer Geom2dAdaptor_Curve::NbKnots() const
{
  if ( myTypeCurve != GeomAbs_BSplineCurve)
    throw Standard_NoSuchObject("Geom2dAdaptor_Curve::NbKnots");
  return myBSplineCurve->NbKnots();
}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom2d_BezierCurve) Geom2dAdaptor_Curve::Bezier() const 
{
  return Handle(Geom2d_BezierCurve)::DownCast (myCurve);
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve) Geom2dAdaptor_Curve::BSpline() const 
{
  return myBSplineCurve;
}

static Standard_Integer nbPoints(const Handle(Geom2d_Curve)& theCurve) 
{
 
  Standard_Integer nbs = 20;
  
  if(theCurve->IsKind(STANDARD_TYPE( Geom2d_Line)) )
    nbs = 2;
  else if(theCurve->IsKind(STANDARD_TYPE( Geom2d_BezierCurve))) 
  {
    nbs = 3 + Handle(Geom2d_BezierCurve)::DownCast (theCurve)->NbPoles();
  }
  else if(theCurve->IsKind(STANDARD_TYPE( Geom2d_BSplineCurve))) { 
    nbs =  Handle(Geom2d_BSplineCurve)::DownCast (theCurve)->NbKnots();
    nbs*= Handle(Geom2d_BSplineCurve)::DownCast (theCurve)->Degree();
    if(nbs < 2.0) nbs=2;
  }
  else if (theCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
  {
    Handle(Geom2d_Curve) aCurve = Handle(Geom2d_OffsetCurve)::DownCast (theCurve)->BasisCurve();
    return Max(nbs, nbPoints(aCurve));
  }

  else if (theCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    Handle(Geom2d_Curve) aCurve = Handle(Geom2d_TrimmedCurve)::DownCast (theCurve)->BasisCurve();
    return Max(nbs, nbPoints(aCurve));
  }
  if(nbs>300)
    nbs = 300;
  return nbs;

}

Standard_Integer Geom2dAdaptor_Curve::NbSamples() const
{
  return  nbPoints(myCurve);
}
