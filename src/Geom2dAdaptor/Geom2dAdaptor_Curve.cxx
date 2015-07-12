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
#include <CSLib_Offset.hxx>
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
#define myBspl Handle(Geom2d_BSplineCurve)::DownCast (myCurve)
#define PosTol Precision::PConfusion()/2

static const int maxDerivOrder = 3;
static const Standard_Real MinStep   = 1e-7;

static gp_Vec2d dummyDerivative; // used as empty value for unused derivatives in AdjustDerivative

// Recalculate derivatives in the singular point
// Returns true is the direction of derivatives is changed
static Standard_Boolean AdjustDerivative(const Handle(Adaptor2d_HCurve2d)& theAdaptor, Standard_Integer theMaxDerivative,
                                         Standard_Real theU, gp_Vec2d& theD1, gp_Vec2d& theD2 = dummyDerivative,
                                         gp_Vec2d& theD3 = dummyDerivative, gp_Vec2d& theD4 = dummyDerivative);

//=======================================================================
//function : LocalContinuity
//purpose  : Computes the Continuity of a BSplineCurve 
//           between the parameters U1 and U2
//           The continuity is C(d-m) 
//             with   d = degree, 
//                    m = max multiplicity of the Knots between U1 and U2
//=======================================================================

GeomAbs_Shape Geom2dAdaptor_Curve::LocalContinuity(const Standard_Real U1, 
						   const Standard_Real U2) 
     const {

       Standard_NoSuchObject_Raise_if(myTypeCurve!=GeomAbs_BSplineCurve," ");
       Standard_Integer Nb = myBspl->NbKnots();
       Standard_Integer Index1 = 0;
       Standard_Integer Index2 = 0;
       Standard_Real newFirst, newLast;
       TColStd_Array1OfReal    TK(1,Nb);
       TColStd_Array1OfInteger TM(1,Nb);
       myBspl->Knots(TK);
       myBspl->Multiplicities(TM);
       BSplCLib::LocateParameter(myBspl->Degree(),TK,TM,U1,myBspl->IsPeriodic(),
				 1,Nb,Index1,newFirst);
       BSplCLib::LocateParameter(myBspl->Degree(),TK,TM,U2,myBspl->IsPeriodic(),
				 1,Nb,Index2,newLast);
       if ( Abs(newFirst-TK(Index1+1))<Precision::PConfusion()) { 
	 if (Index1 < Nb)Index1++;
       }
       if ( Abs(newLast-TK(Index2))<Precision::PConfusion())
	 Index2--;
       Standard_Integer MultMax;
       // attention aux courbes peridiques.
       if ( (myBspl->IsPeriodic()) && (Index1 == Nb) )
	 Index1 = 1;

       if ( Index2 - Index1 <= 0) {
	 MultMax = 100;  // CN entre 2 Noeuds consecutifs
       }
       else {
	 MultMax = TM(Index1+1);
	 for(Standard_Integer i = Index1+1;i<=Index2;i++) {
	   if ( TM(i)>MultMax) MultMax=TM(i);
	 }
	 MultMax = myBspl->Degree() - MultMax;
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

  if ( myCurve != C) {
    myCurve = C;

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
      // Create cache for B-spline
      myCurveCache = new BSplCLib_Cache(myBspl->Degree(), myBspl->IsPeriodic(), 
        myBspl->KnotSequence(), myBspl->Poles(), myBspl->Weights());
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_OffsetCurve))
    {
      myTypeCurve = GeomAbs_OtherCurve;
      // Create nested adaptor for base curve
      Handle(Geom2d_Curve) aBase = Handle(Geom2d_OffsetCurve)::DownCast(myCurve)->BasisCurve();
      myOffsetBaseCurveAdaptor = new Geom2dAdaptor_HCurve(aBase);
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
  else if (myCurve->DynamicType() == STANDARD_TYPE(Geom2d_OffsetCurve)){
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
      Standard_NoSuchObject::Raise("Geom2dAdaptor_Curve::Continuity");
    }
  }

  else if (myTypeCurve == GeomAbs_OtherCurve) {
    Standard_NoSuchObject::Raise("Geom2dAdaptor_Curve::Continuity");
  }
  else {
    return GeomAbs_CN;
  }

  // portage WNT
  return GeomAbs_CN;
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
    Standard_Integer FirstIndex = myBspl->FirstUKnotIndex();
    Standard_Integer LastIndex  = myBspl->LastUKnotIndex();
    TColStd_Array1OfInteger Inter (1, LastIndex-FirstIndex+1);
    if ( S > Continuity()) {
      Standard_Integer Cont;
      switch ( S) {
      case GeomAbs_G1:
      case GeomAbs_G2:
	Standard_DomainError::Raise("Geom2dAdaptor_Curve::NbIntervals");
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
	  else                       Cont = myBspl->Degree();
          Standard_Integer Degree = myBspl->Degree();
          Standard_Integer NbKnots = myBspl->NbKnots();
          TColStd_Array1OfInteger Mults (1, NbKnots);
          myBspl->Multiplicities (Mults);
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
	  
	  Standard_Integer Nb = myBspl->NbKnots();
	  Standard_Integer Index1 = 0;
	  Standard_Integer Index2 = 0;
	  Standard_Real newFirst, newLast;
	  TColStd_Array1OfReal    TK(1,Nb);
	  TColStd_Array1OfInteger TM(1,Nb);
	  myBspl->Knots(TK);
	  myBspl->Multiplicities(TM);
	  BSplCLib::LocateParameter(myBspl->Degree(),TK,TM,myFirst,
				    myBspl->IsPeriodic(),
				    1,Nb,Index1,newFirst);
	  BSplCLib::LocateParameter(myBspl->Degree(),TK,TM,myLast,
				    myBspl->IsPeriodic(),
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
  else if (myCurve->DynamicType() == STANDARD_TYPE(Geom2d_OffsetCurve)){
    GeomAbs_Shape BaseS=GeomAbs_C0;
    switch(S){
    case GeomAbs_G1:
    case GeomAbs_G2:
      Standard_DomainError::Raise("GeomAdaptor_Curve::NbIntervals");
      break;
    case GeomAbs_C0: BaseS = GeomAbs_C1; break;
    case GeomAbs_C1: BaseS = GeomAbs_C2; break;
    case GeomAbs_C2: BaseS = GeomAbs_C3; break;
    default: BaseS = GeomAbs_CN;
    }
    myNbIntervals = myOffsetBaseCurveAdaptor->NbIntervals(BaseS);
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
    Standard_Integer FirstIndex = myBspl->FirstUKnotIndex();
    Standard_Integer LastIndex  = myBspl->LastUKnotIndex();
    TColStd_Array1OfInteger Inter (1, LastIndex-FirstIndex+1);
    if ( S > Continuity()) {
      Standard_Integer Cont;
      switch ( S) {
      case GeomAbs_G1:
      case GeomAbs_G2:
	Standard_DomainError::Raise("Geom2dAdaptor_Curve::NbIntervals");
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
	  else                       Cont = myBspl->Degree();
          Standard_Integer Degree = myBspl->Degree();
          Standard_Integer NbKnots = myBspl->NbKnots();
          TColStd_Array1OfInteger Mults (1, NbKnots);
          myBspl->Multiplicities (Mults);
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
	  
	  Standard_Integer Nb = myBspl->NbKnots();
	  Standard_Integer Index1 = 0;
	  Standard_Integer Index2 = 0;
	  Standard_Real newFirst, newLast;
	  TColStd_Array1OfReal    TK(1,Nb);
	  TColStd_Array1OfInteger TM(1,Nb);
	  myBspl->Knots(TK);
	  myBspl->Multiplicities(TM);
	  BSplCLib::LocateParameter(myBspl->Degree(),TK,TM,myFirst,
				    myBspl->IsPeriodic(),
				    1,Nb,Index1,newFirst);
	  BSplCLib::LocateParameter(myBspl->Degree(),TK,TM,myLast,
				    myBspl->IsPeriodic(),
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
  else if (myCurve->DynamicType() == STANDARD_TYPE(Geom2d_OffsetCurve)){
    GeomAbs_Shape BaseS=GeomAbs_C0;
    switch(S){
    case GeomAbs_G1:
    case GeomAbs_G2:
      Standard_DomainError::Raise("GeomAdaptor_Curve::NbIntervals");
      break;
    case GeomAbs_C0: BaseS = GeomAbs_C1; break;
    case GeomAbs_C1: BaseS = GeomAbs_C2; break;
    case GeomAbs_C2: BaseS = GeomAbs_C3; break;
    default: BaseS = GeomAbs_CN;
    }
    myNbIntervals = myOffsetBaseCurveAdaptor->NbIntervals(BaseS);
    myOffsetBaseCurveAdaptor->Intervals(T, BaseS);
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
  if (myCurve->IsPeriodic())
    return IsClosed();
  else
    return Standard_False;
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
  myCurveCache->BuildCache(theParameter, myBspl->Degree(), 
    myBspl->IsPeriodic(), myBspl->KnotSequence(), 
    myBspl->Poles(), myBspl->Weights());
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt2d Geom2dAdaptor_Curve::Value(const Standard_Real U) const 
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
    return ValueBSpline(U);
  else if (myCurve->DynamicType() == STANDARD_TYPE(Geom2d_OffsetCurve))
    return ValueOffset(U);

  return myCurve->Value(U);
}

//=======================================================================
//function : ValueBSpline
//purpose  : Computes the point of parameter U on the B-spline curve
//=======================================================================
gp_Pnt2d Geom2dAdaptor_Curve::ValueBSpline(const Standard_Real theU) const
{
  if (theU == myFirst || theU == myLast)
  {
    Standard_Integer Ideb = 0, Ifin = 0;
    if (theU == myFirst)
    {
      myBspl->LocateU(myFirst, PosTol, Ideb, Ifin);
      if (Ideb<1) Ideb=1;
      if (Ideb>=Ifin) Ifin = Ideb+1;
    }
    if (theU == myLast)
    {
      myBspl->LocateU(myLast,  PosTol, Ideb, Ifin);
      if (Ifin>myBspl->NbKnots()) Ifin = myBspl->NbKnots();
      if (Ideb>=Ifin) Ideb = Ifin-1;
    }
    return myBspl->LocalValue(theU, Ideb, Ifin);
  }
  else if (!myCurveCache.IsNull()) // use cached B-spline data
  {
    if (!myCurveCache->IsCacheValid(theU))
      RebuildCache(theU);
    gp_Pnt2d aRes;
    myCurveCache->D0(theU, aRes);
    return aRes;
  }
  return myCurve->Value(theU);
}

//=======================================================================
//function : ValueOffset
//purpose  : Computes the point of parameter U on the offset curve
//=======================================================================
gp_Pnt2d Geom2dAdaptor_Curve::ValueOffset(const Standard_Real theU) const
{
  gp_Pnt2d aP;
  gp_Vec2d aD1;
  myOffsetBaseCurveAdaptor->D1(theU, aP, aD1);
  Standard_Boolean isDirectionChange = Standard_False;
  const Standard_Real aTol = gp::Resolution();
  if(aD1.SquareMagnitude() <= aTol)
    isDirectionChange = AdjustDerivative(myOffsetBaseCurveAdaptor, 1, theU, aD1);

  Standard_Real anOffset = Handle(Geom2d_OffsetCurve)::DownCast(myCurve)->Offset();
  CSLib_Offset::D0(aP, aD1, anOffset, isDirectionChange, aP);
  return aP;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom2dAdaptor_Curve::D0(const Standard_Real U, gp_Pnt2d& P) const
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    D0BSpline(U, P);
    return;
  }
  else if (myCurve->DynamicType() == STANDARD_TYPE(Geom2d_OffsetCurve))
  {
    D0Offset(U, P);
    return;
  }

  myCurve->D0(U, P);
}

//=======================================================================
//function : D0BSpline
//purpose  : Computes the point of parameter theU on the B-spline curve
//=======================================================================
void Geom2dAdaptor_Curve::D0BSpline(const Standard_Real theU, gp_Pnt2d& theP) const
{
  if (theU == myFirst || theU == myLast)
  {
    Standard_Integer Ideb = 0, Ifin = 0;
    if (theU == myFirst) {
      myBspl->LocateU(myFirst,  PosTol, Ideb, Ifin);
      if (Ideb<1) Ideb=1;
      if (Ideb>=Ifin) Ifin = Ideb+1;
    }
    if (theU == myLast) {
      myBspl->LocateU(myLast,  PosTol, Ideb, Ifin);
      if (Ifin>myBspl->NbKnots()) Ifin = myBspl->NbKnots();
      if (Ideb>=Ifin) Ideb = Ifin-1;
    }
    myBspl->LocalD0(theU, Ideb, Ifin, theP);
    return;
  }
  else if (!myCurveCache.IsNull()) // use cached B-spline data
  {
    if (!myCurveCache->IsCacheValid(theU))
      RebuildCache(theU);
    myCurveCache->D0(theU, theP);
    return;
  }
  myCurve->D0(theU, theP);
}

//=======================================================================
//function : D0Offset
//purpose  : Computes the point of parameter theU on the offset curve
//=======================================================================
void Geom2dAdaptor_Curve::D0Offset(const Standard_Real theU, gp_Pnt2d& theP) const
{
  theP = ValueOffset(theU);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom2dAdaptor_Curve::D1(const Standard_Real U, 
                             gp_Pnt2d& P, gp_Vec2d& V) const 
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    D1BSpline(U, P, V);
    return;
  }
  else if (myCurve->DynamicType() == STANDARD_TYPE(Geom2d_OffsetCurve))
  {
    D1Offset(U, P, V);
    return;
  }

  myCurve->D1(U, P, V);
}

//=======================================================================
//function : D1BSpline
//purpose  : Computes the point of parameter theU on the B-spline curve and its derivative
//=======================================================================
void Geom2dAdaptor_Curve::D1BSpline(const Standard_Real theU, gp_Pnt2d& theP, gp_Vec2d& theV) const
{
  if (theU == myFirst || theU == myLast)
  {
    Standard_Integer Ideb = 0, Ifin = 0;
    if (theU == myFirst) {
      myBspl->LocateU(myFirst,  PosTol, Ideb, Ifin);
      if (Ideb<1) Ideb=1;
      if (Ideb>=Ifin) Ifin = Ideb+1;
    }
    if (theU == myLast) {
      myBspl->LocateU(myLast,  PosTol, Ideb, Ifin);
      if (Ifin>myBspl->NbKnots()) Ifin = myBspl->NbKnots();
      if (Ideb>=Ifin) Ideb = Ifin-1;
    }
    myBspl->LocalD1(theU, Ideb, Ifin, theP, theV); 
    return;
  }
  else if (!myCurveCache.IsNull()) // use cached B-spline data
  {
    if (!myCurveCache->IsCacheValid(theU))
      RebuildCache(theU);
    myCurveCache->D1(theU, theP, theV);
    return;
  }
  myCurve->D1(theU, theP, theV);
}

//=======================================================================
//function : D1Offset
//purpose  : Computes the point of parameter theU on the offset curve and its derivative
//=======================================================================
void Geom2dAdaptor_Curve::D1Offset(const Standard_Real theU, gp_Pnt2d& theP, gp_Vec2d& theV) const
{
   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  gp_Vec2d V2;
  myOffsetBaseCurveAdaptor->D2 (theU, theP, theV, V2);

  Standard_Boolean IsDirectionChange = Standard_False;
  if(theV.SquareMagnitude() <= gp::Resolution())
    IsDirectionChange = AdjustDerivative(myOffsetBaseCurveAdaptor, 2, theU, theV, V2);

  Standard_Real anOffset = Handle(Geom2d_OffsetCurve)::DownCast(myCurve)->Offset();
  CSLib_Offset::D1(theP, theV, V2, anOffset, IsDirectionChange, theP, theV);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2dAdaptor_Curve::D2(const Standard_Real U, 
                             gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const 
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    D2BSpline(U, P, V1, V2);
    return;
  }
  else if (myCurve->DynamicType() == STANDARD_TYPE(Geom2d_OffsetCurve))
  {
    D2Offset(U, P, V1, V2);
    return;
  }

  myCurve->D2(U, P, V1, V2);
}

//=======================================================================
//function : D2BSpline
//purpose  : Computes the point of parameter theU on the B-spline curve and its first and second derivatives
//=======================================================================
void Geom2dAdaptor_Curve::D2BSpline(const Standard_Real theU, gp_Pnt2d& theP,
                                    gp_Vec2d& theV1, gp_Vec2d& theV2) const
{
  if (theU == myFirst || theU == myLast)
  {
    Standard_Integer Ideb = 0, Ifin = 0;
    if (theU == myFirst) {
      myBspl->LocateU(myFirst,  PosTol, Ideb, Ifin);
      if (Ideb<1) Ideb=1;
      if (Ideb>=Ifin) Ifin = Ideb+1;
    }
    if (theU == myLast) {
      myBspl->LocateU(myLast,  PosTol, Ideb, Ifin);
      if (Ifin>myBspl->NbKnots()) Ifin = myBspl->NbKnots();
      if (Ideb>=Ifin) Ideb = Ifin-1;
    }
    myBspl->LocalD2(theU, Ideb, Ifin, theP, theV1, theV2);
    return;
  }
  else if (!myCurveCache.IsNull()) // use cached B-spline data
  {
    if (!myCurveCache->IsCacheValid(theU))
      RebuildCache(theU);
    myCurveCache->D2(theU, theP, theV1, theV2);
    return;
  }
  myCurve->D2(theU, theP, theV1, theV2);
}
//=======================================================================
//function : D2Offset
//purpose  : Computes the point of parameter theU on the offset curve and its first and second derivatives
//=======================================================================
void Geom2dAdaptor_Curve::D2Offset(const Standard_Real theU, gp_Pnt2d& theP,
                                    gp_Vec2d& theV1, gp_Vec2d& theV2) const
{
   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
   //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  gp_Vec2d V3;
  myOffsetBaseCurveAdaptor->D3 (theU, theP, theV1, theV2, V3);

  Standard_Boolean IsDirectionChange = Standard_False;
  if(theV1.SquareMagnitude() <= gp::Resolution())
    IsDirectionChange = AdjustDerivative(myOffsetBaseCurveAdaptor, 3, theU, theV1, theV2, V3);

  Standard_Real anOffset = Handle(Geom2d_OffsetCurve)::DownCast(myCurve)->Offset();
  CSLib_Offset::D2(theP, theV1, theV2, V3, anOffset, IsDirectionChange, theP, theV1, theV2);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom2dAdaptor_Curve::D3(const Standard_Real U, 
                             gp_Pnt2d& P,  gp_Vec2d& V1, 
                             gp_Vec2d& V2, gp_Vec2d& V3) const 
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    D3BSpline(U, P, V1, V2, V3);
    return;
  }
  else if (myCurve->DynamicType() == STANDARD_TYPE(Geom2d_OffsetCurve))
  {
    D3Offset(U, P, V1, V2, V3);
    return;
  }

  myCurve->D3(U, P, V1, V2, V3);
}

//=======================================================================
//function : D3BSpline
//purpose  : Computes the point of parameter theU on the B-spline curve and its 1st - 3rd derivatives
//=======================================================================
void Geom2dAdaptor_Curve::D3BSpline(const Standard_Real theU, gp_Pnt2d& theP,
                                    gp_Vec2d& theV1, gp_Vec2d& theV2, gp_Vec2d& theV3) const
{
  if (theU == myFirst || theU == myLast)
  {
    Standard_Integer Ideb = 0, Ifin = 0;
    if (theU == myFirst) {
      myBspl->LocateU(myFirst,  PosTol, Ideb, Ifin);
      if (Ideb<1) Ideb=1;
      if (Ideb>=Ifin) Ifin = Ideb+1;
    }
    if (theU == myLast) {
      myBspl->LocateU(myLast,  PosTol, Ideb, Ifin);
      if (Ifin>myBspl->NbKnots()) Ifin = myBspl->NbKnots();
      if (Ideb>=Ifin) Ideb = Ifin-1;
    }
    myBspl->LocalD3(theU, Ideb, Ifin, theP, theV1, theV2, theV3);
    return;
  }
  else if (!myCurveCache.IsNull()) // use cached B-spline data
  {
    if (!myCurveCache->IsCacheValid(theU))
      RebuildCache(theU);
    myCurveCache->D3(theU, theP, theV1, theV2, theV3);
    return;
  }
  myCurve->D3(theU, theP, theV1, theV2, theV3);
}
//=======================================================================
//function : D3Offset
//purpose  : Computes the point of parameter theU on the offset curve and its 1st - 3rd derivatives
//=======================================================================
void Geom2dAdaptor_Curve::D3Offset(const Standard_Real theU, gp_Pnt2d& theP,
                                    gp_Vec2d& theV1, gp_Vec2d& theV2, gp_Vec2d& theV3) const
{
   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
   //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

   //P"'(u) = p"'(u) + (Offset / R) * (D3Ndir - (3.0 * Dr/R**2 ) * D2Ndir -
   //         (3.0 * D2r / R2) * DNdir) + (3.0 * Dr * Dr / R4) * DNdir -
   //         (D3r/R2) * Ndir + (6.0 * Dr * Dr / R4) * Ndir +
   //         (6.0 * Dr * D2r / R4) * Ndir - (15.0 * Dr* Dr* Dr /R6) * Ndir

  Standard_Boolean IsDirectionChange = Standard_False;

  myOffsetBaseCurveAdaptor->D3 (theU, theP, theV1, theV2, theV3);
  gp_Vec2d V4 = myOffsetBaseCurveAdaptor->DN (theU, 4);

  if(theV1.SquareMagnitude() <= gp::Resolution())
    IsDirectionChange = AdjustDerivative(myOffsetBaseCurveAdaptor, 4, theU, theV1, theV2, theV3, V4);

  Standard_Real anOffset = Handle(Geom2d_OffsetCurve)::DownCast(myCurve)->Offset();
  CSLib_Offset::D3(theP, theV1, theV2, theV3, V4, anOffset, IsDirectionChange,
                   theP, theV1, theV2, theV3);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec2d Geom2dAdaptor_Curve::DN(const Standard_Real U, 
                                 const Standard_Integer N) const 
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
    return DNBSpline(U, N);
  else if (myCurve->DynamicType() == STANDARD_TYPE(Geom2d_OffsetCurve))
    return DNOffset(U, N);

  return myCurve->DN(U, N);
}

gp_Vec2d Geom2dAdaptor_Curve::DNBSpline(const Standard_Real U, 
                                        const Standard_Integer N) const 
{
  if (U==myFirst || U==myLast)
  {
    Standard_Integer Ideb = 0, Ifin = 0;
    if (U==myFirst) {
      myBspl->LocateU(myFirst, PosTol, Ideb, Ifin);
      if (Ideb<1) Ideb=1;
      if (Ideb>=Ifin) Ifin = Ideb+1;
    }
    if (U==myLast) {
      myBspl->LocateU(myLast, PosTol, Ideb, Ifin);
      if (Ifin>myBspl->NbKnots()) Ifin = myBspl->NbKnots();
      if (Ideb>=Ifin) Ideb = Ifin-1;
    } 
    return myBspl->LocalDN( U, Ideb, Ifin, N);
  }

  return myCurve->DN( U, N);
}

gp_Vec2d Geom2dAdaptor_Curve::DNOffset(const Standard_Real    U,
                                       const Standard_Integer N) const
{
  gp_Pnt2d aPnt;
  gp_Vec2d aVec, aVN;

  switch (N)
  {
  case 1:
    D1Offset(U, aPnt, aVN);
    break;
  case 2:
    D2Offset(U, aPnt, aVec, aVN);
    break;
  case 3:
    D3Offset(U, aPnt, aVec, aVec, aVN);
    break;
  default:
    aVN = myCurve->DN(U, N);
  }
  return aVN;
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
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Line, "");
  return Handle(Geom2d_Line)::DownCast (myCurve)->Lin2d();
}

//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ2d  Geom2dAdaptor_Curve::Circle() const 
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Circle, "");
  return Handle(Geom2d_Circle)::DownCast (myCurve)->Circ2d();
}

//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips2d Geom2dAdaptor_Curve::Ellipse() const 
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Ellipse, "");
  return Handle(Geom2d_Ellipse)::DownCast (myCurve)->Elips2d();
}

//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr2d Geom2dAdaptor_Curve::Hyperbola() const 
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Hyperbola, "");
  return Handle(Geom2d_Hyperbola)::DownCast (myCurve)->Hypr2d();
}

//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

gp_Parab2d Geom2dAdaptor_Curve::Parabola() const 
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Parabola, "");
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
    return Handle(Geom2d_BSplineCurve)::DownCast (myCurve)->Degree();
  else
    Standard_NoSuchObject::Raise();
  // portage WNT 
  return 0;
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean Geom2dAdaptor_Curve::IsRational() const {
  switch( myTypeCurve) {
  case GeomAbs_BSplineCurve:
    return Handle(Geom2d_BSplineCurve)::DownCast (myCurve)->IsRational();
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
    return Handle(Geom2d_BSplineCurve)::DownCast (myCurve)->NbPoles();
  else
    Standard_NoSuchObject::Raise();
  // portage WNT 
  return 0;
}

//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer Geom2dAdaptor_Curve::NbKnots() const {
  if ( myTypeCurve != GeomAbs_BSplineCurve)
    Standard_NoSuchObject::Raise("Geom2dAdaptor_Curve::NbKnots");
  return Handle(Geom2d_BSplineCurve)::DownCast (myCurve)->NbKnots();

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
  return Handle(Geom2d_BSplineCurve)::DownCast (myCurve);
}

static Standard_Integer nbPoints(const Handle(Geom2d_Curve)& theCurve) 
{
 
  Standard_Integer nbs = 10;
  
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


// ============= Auxiliary functions ===================
Standard_Boolean AdjustDerivative(const Handle(Adaptor2d_HCurve2d)& theAdaptor, Standard_Integer theMaxDerivative,
                                  Standard_Real theU, gp_Vec2d& theD1, gp_Vec2d& theD2,
                                  gp_Vec2d& theD3, gp_Vec2d& theD4)
{
  static const Standard_Real aTol = gp::Resolution();

  Standard_Boolean IsDirectionChange = Standard_False;
  const Standard_Real anUinfium   = theAdaptor->FirstParameter();
  const Standard_Real anUsupremum = theAdaptor->LastParameter();

  const Standard_Real DivisionFactor = 1.e-3;
  Standard_Real du;
  if((anUsupremum >= RealLast()) || (anUinfium <= RealFirst())) 
    du = 0.0;
  else
    du = anUsupremum - anUinfium;

  const Standard_Real aDelta = Max(du * DivisionFactor, MinStep);

  //Derivative is approximated by Taylor-series
  Standard_Integer anIndex = 1; //Derivative order
  gp_Vec2d V;

  do
  {
    V =  theAdaptor->DN(theU, ++anIndex);
  }
  while((V.Magnitude() <= aTol) && anIndex < maxDerivOrder);

  Standard_Real u;

  if(theU-anUinfium < aDelta)
    u = theU+aDelta;
  else
    u = theU-aDelta;

  gp_Pnt2d P1, P2;
  theAdaptor->D0(Min(theU, u),P1);
  theAdaptor->D0(Max(theU, u),P2);

  gp_Vec2d V1(P1, P2);
  IsDirectionChange = V.Dot(V1) < 0.0;
  Standard_Real aSign = IsDirectionChange ? -1.0 : 1.0;

  theD1 = V * aSign;
  gp_Vec2d* aDeriv[3] = {&theD2, &theD3, &theD4};
  for (Standard_Integer i = 1; i < theMaxDerivative; i++)
    *(aDeriv[i-1]) = theAdaptor->DN(theU, anIndex + i) * aSign;

  return IsDirectionChange;
}
