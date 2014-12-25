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

#include <Geom2dAdaptor_Curve.ixx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <BSplCLib.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Precision.hxx>
#include <gp.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>
//#include <Geom2dConvert_BSplineCurveKnotSplitting.hxx>

#include <Standard_OutOfRange.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_NotImplemented.hxx>

#define myBspl (*((Handle(Geom2d_BSplineCurve)*)&myCurve))
#define PosTol Precision::PConfusion()/2

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
      Load((*((Handle(Geom2d_TrimmedCurve)*)&C))->BasisCurve(),
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
  else if (myCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))){
    GeomAbs_Shape S = 
      (*((Handle(Geom2d_OffsetCurve)*)&myCurve))->GetBasisCurveContinuity(); 
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
  else if (myCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))){
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
    Geom2dAdaptor_Curve C
      ((*((Handle(Geom2d_OffsetCurve)*)&myCurve))->BasisCurve());
    myNbIntervals = C.NbIntervals(BaseS);
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
  else if (myCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))){
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
    Geom2dAdaptor_Curve C
      ((*((Handle(Geom2d_OffsetCurve)*)&myCurve))->BasisCurve());
    myNbIntervals = C.NbIntervals(BaseS);
    C.Intervals(T, BaseS);
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
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt2d Geom2dAdaptor_Curve::Value(const Standard_Real U) const 
{
  if ( (myTypeCurve == GeomAbs_BSplineCurve)&&
      (U==myFirst || U==myLast) ) {
    Standard_Integer Ideb = 0, Ifin = 0;
    if (U==myFirst) {
      myBspl->LocateU(myFirst, PosTol, Ideb, Ifin);
      if (Ideb<1) Ideb=1;
      if (Ideb>=Ifin) Ifin = Ideb+1;
    }
    if (U==myLast) {
      myBspl->LocateU(myLast,  PosTol, Ideb, Ifin);
      if (Ifin>myBspl->NbKnots()) Ifin = myBspl->NbKnots();
      if (Ideb>=Ifin) Ideb = Ifin-1;
    }
    return myBspl->LocalValue(U, Ideb, Ifin);
  }
  else {
    return myCurve->Value( U);
  }
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom2dAdaptor_Curve::D0(const Standard_Real U, gp_Pnt2d& P) const
{
  if ( (myTypeCurve == GeomAbs_BSplineCurve)&&
      (U==myFirst || U==myLast) ) {
    Standard_Integer Ideb = 0, Ifin = 0;
    if (U==myFirst) {
      myBspl->LocateU(myFirst,  PosTol, Ideb, Ifin);
      if (Ideb<1) Ideb=1;
      if (Ideb>=Ifin) Ifin = Ideb+1;
    }
    if (U==myLast) {
      myBspl->LocateU(myLast,  PosTol, Ideb, Ifin);
      if (Ifin>myBspl->NbKnots()) Ifin = myBspl->NbKnots();
      if (Ideb>=Ifin) Ideb = Ifin-1;
    }
    myBspl->LocalD0( U, Ideb, Ifin, P);
  }
  else {
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
  if ( (myTypeCurve == GeomAbs_BSplineCurve)&&
      (U==myFirst || U==myLast) ) {
    Standard_Integer Ideb = 0, Ifin = 0;
    if (U==myFirst) {
      myBspl->LocateU(myFirst,  PosTol, Ideb, Ifin);
      if (Ideb<1) Ideb=1;
      if (Ideb>=Ifin) Ifin = Ideb+1;
    }
    if (U==myLast) {
      myBspl->LocateU(myLast,  PosTol, Ideb, Ifin);
      if (Ifin>myBspl->NbKnots()) Ifin = myBspl->NbKnots();
      if (Ideb>=Ifin) Ideb = Ifin-1;
    }
    myBspl->LocalD1( U, Ideb, Ifin, P, V); 
  } 
  else {
    myCurve->D1( U, P, V);
  }
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2dAdaptor_Curve::D2(const Standard_Real U, 
			     gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const 
{
  if ( (myTypeCurve == GeomAbs_BSplineCurve)&&
      (U==myFirst || U==myLast) ) {
    Standard_Integer Ideb = 0, Ifin = 0;
    if (U==myFirst) {
      myBspl->LocateU(myFirst,  PosTol, Ideb, Ifin);
      if (Ideb<1) Ideb=1;
      if (Ideb>=Ifin) Ifin = Ideb+1;
    }
    if (U==myLast) {
      myBspl->LocateU(myLast,  PosTol, Ideb, Ifin);
      if (Ifin>myBspl->NbKnots()) Ifin = myBspl->NbKnots();
      if (Ideb>=Ifin) Ideb = Ifin-1;
    }
    myBspl->LocalD2( U, Ideb, Ifin, P, V1, V2);
  }
  else {
    myCurve->D2( U, P, V1, V2);
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
  if ( (myTypeCurve == GeomAbs_BSplineCurve) &&
      (U==myFirst || U==myLast) ) {
    Standard_Integer Ideb = 0, Ifin = 0;
    if (U==myFirst) {
      myBspl->LocateU(myFirst,  PosTol, Ideb, Ifin);
      if (Ideb<1) Ideb=1;
      if (Ideb>=Ifin) Ifin = Ideb+1;
    }
    if (U==myLast) {
      myBspl->LocateU(myLast,  PosTol, Ideb, Ifin);
      if (Ifin>myBspl->NbKnots()) Ifin = myBspl->NbKnots();
      if (Ideb>=Ifin) Ideb = Ifin-1;
    }
    myBspl->LocalD3( U, Ideb, Ifin, P, V1, V2, V3);  
  }
  else {
    myCurve->D3( U, P, V1, V2, V3);
  }
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec2d Geom2dAdaptor_Curve::DN(const Standard_Real U, 
				 const Standard_Integer N) const 
{
  if ( (myTypeCurve == GeomAbs_BSplineCurve) &&
      (U==myFirst || U==myLast) ) {
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
  else {
    return myCurve->DN( U, N);
  }
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
    Standard_Real R = (*((Handle(Geom2d_Circle)*)&myCurve))->Circ2d().Radius();
    if ( R > Ruv/2.)
      return 2*ASin(Ruv/(2*R));
    else
      return 2*M_PI;
  }
  case GeomAbs_Ellipse: {
    return Ruv / (*((Handle(Geom2d_Ellipse)*)&myCurve))->MajorRadius();
  }
  case GeomAbs_BezierCurve: {
    Standard_Real res;
    (*((Handle(Geom2d_BezierCurve)*)&myCurve))->Resolution(Ruv,res);
    return res;
  }
  case GeomAbs_BSplineCurve: {
    Standard_Real res;
    (*((Handle(Geom2d_BSplineCurve)*)&myCurve))->Resolution(Ruv,res);
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
  return (*((Handle(Geom2d_Line)*)&myCurve))->Lin2d();
}

//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ2d  Geom2dAdaptor_Curve::Circle() const 
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Circle, "");
  return (*((Handle(Geom2d_Circle)*)&myCurve))->Circ2d();
}

//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips2d Geom2dAdaptor_Curve::Ellipse() const 
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Ellipse, "");
  return (*((Handle(Geom2d_Ellipse)*)&myCurve))->Elips2d();
}

//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr2d Geom2dAdaptor_Curve::Hyperbola() const 
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Hyperbola, "");
  return (*((Handle(Geom2d_Hyperbola)*)&myCurve))->Hypr2d();
}

//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

gp_Parab2d Geom2dAdaptor_Curve::Parabola() const 
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Parabola, "");
  return (*((Handle(Geom2d_Parabola)*)&myCurve))->Parab2d();
}

//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer Geom2dAdaptor_Curve::Degree() const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
    return (*((Handle(Geom2d_BezierCurve)*)&myCurve))->Degree();
  else if (myTypeCurve == GeomAbs_BSplineCurve)
    return (*((Handle(Geom2d_BSplineCurve)*)&myCurve))->Degree();
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
    return (*((Handle(Geom2d_BSplineCurve)*)&myCurve))->IsRational();
  case GeomAbs_BezierCurve:
    return (*((Handle(Geom2d_BezierCurve)*)&myCurve))->IsRational();
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
    return (*((Handle(Geom2d_BezierCurve)*)&myCurve))->NbPoles();
  else if (myTypeCurve == GeomAbs_BSplineCurve)
    return (*((Handle(Geom2d_BSplineCurve)*)&myCurve))->NbPoles();
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
  return (*((Handle(Geom2d_BSplineCurve)*)&myCurve))->NbKnots();

}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom2d_BezierCurve) Geom2dAdaptor_Curve::Bezier() const 
{
  return *((Handle(Geom2d_BezierCurve)*)&myCurve);
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve) Geom2dAdaptor_Curve::BSpline() const 
{
  return *((Handle(Geom2d_BSplineCurve)*)&myCurve);
}

static Standard_Integer nbPoints(const Handle(Geom2d_Curve)& theCurve) 
{
 
  Standard_Integer nbs = 10;
  
  if(theCurve->IsKind(STANDARD_TYPE( Geom2d_Line)) )
    nbs = 2;
  else if(theCurve->IsKind(STANDARD_TYPE( Geom2d_BezierCurve))) 
  {
    nbs = 3 + (*((Handle(Geom2d_BezierCurve)*)&theCurve))->NbPoles();
  }
  else if(theCurve->IsKind(STANDARD_TYPE( Geom2d_BSplineCurve))) { 
    nbs =  (*((Handle(Geom2d_BSplineCurve)*)&theCurve))->NbKnots();
    nbs*= (*((Handle(Geom2d_BSplineCurve)*)&theCurve))->Degree();
    if(nbs < 2.0) nbs=2;
  }
  else if (theCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
  {
    Handle(Geom2d_Curve) aCurve = (*((Handle(Geom2d_OffsetCurve)*)&theCurve))->BasisCurve();
    return Max(nbs, nbPoints(aCurve));
  }

  else if (theCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    Handle(Geom2d_Curve) aCurve = (*((Handle(Geom2d_TrimmedCurve)*)&theCurve))->BasisCurve();
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
