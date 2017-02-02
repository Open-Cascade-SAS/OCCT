// Created on: 1998-08-17
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Convert_ParameterisationType.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_EvolvedSection.hxx>
#include <gp_Pnt.hxx>
#include <Law_Function.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(GeomFill_EvolvedSection,GeomFill_SectionLaw)

#ifdef DRAW
#include <DrawTrSurf.hxx>
#include <Geom_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
static Standard_Integer NumSec = 0;
static Standard_Boolean Affich = 0;
#endif

GeomFill_EvolvedSection::GeomFill_EvolvedSection(const Handle(Geom_Curve)& C,
						 const Handle(Law_Function)& L)
{
  L->Bounds(First, Last);
  mySection = Handle(Geom_Curve)::DownCast(C->Copy());
  myLaw =  L->Trim(First, Last, 1.e-20);
  TLaw = myLaw;
  myCurve =  Handle(Geom_BSplineCurve)::DownCast(C);
  if (myCurve.IsNull()) {
    myCurve = GeomConvert::CurveToBSplineCurve(C, Convert_QuasiAngular);
    if (myCurve->IsPeriodic()) {
      Standard_Integer M = myCurve->Degree()/2+1;
      myCurve->RemoveKnot(1, M, Precision::Confusion());
    }
  }

#if DRAW
  if (Affich) {
    char name[256];
    sprintf(name,"UnifSect_%d",++NumSec);
    DrawTrSurf::Set(name, myCurve);
  }
#endif
}

//=======================================================
// Purpose :D0
//=======================================================
 Standard_Boolean GeomFill_EvolvedSection::D0(const Standard_Real U,
					      TColgp_Array1OfPnt& Poles,
					      TColStd_Array1OfReal& Weights) 
{
  Standard_Real val;
  Standard_Integer ii, L =  Poles.Length();
  val= TLaw->Value(U);
  myCurve->Poles(Poles);
  for (ii=1; ii<=L; ii++) {
    Poles(ii).ChangeCoord() *= val;
  }
  myCurve->Weights(Weights);

  return Standard_True;
}

//=======================================================
// Purpose :D1
//=======================================================
 Standard_Boolean GeomFill_EvolvedSection::D1(const Standard_Real U,
					      TColgp_Array1OfPnt& Poles,
					      TColgp_Array1OfVec& DPoles,
					      TColStd_Array1OfReal& Weights,
					      TColStd_Array1OfReal& DWeights) 
{
 Standard_Real val, dval;
 Standard_Integer ii, L =  Poles.Length();
 TLaw->D1(U, val, dval);

 myCurve->Poles(Poles);
 myCurve->Weights(Weights);
 for (ii=1; ii<=L; ii++) {
   DPoles(ii).SetXYZ(Poles(ii).XYZ());
   DPoles(ii) *= dval;
   Poles(ii).ChangeCoord() *= val;
 }
 DWeights.Init(0);

 return Standard_True;
}

//=======================================================
// Purpose :D2
//=======================================================
 Standard_Boolean GeomFill_EvolvedSection::D2(const Standard_Real U,
					      TColgp_Array1OfPnt& Poles,
					      TColgp_Array1OfVec& DPoles,
					      TColgp_Array1OfVec& D2Poles,
					      TColStd_Array1OfReal& Weights,
					      TColStd_Array1OfReal& DWeights,
					      TColStd_Array1OfReal& D2Weights) 
{ 
  Standard_Real val, dval, d2val;
  Standard_Integer ii, L =  Poles.Length();
  TLaw->D2(U, val, dval, d2val);
  myCurve->Poles(Poles);
  myCurve->Weights(Weights);

  for (ii=1; ii<=L; ii++) {
    DPoles(ii).SetXYZ(Poles(ii).XYZ());
    D2Poles(ii) = DPoles(ii);
    D2Poles(ii) *= d2val;
    DPoles(ii)  *= dval;
    Poles(ii).ChangeCoord() *= val;
  }  
 
  DWeights.Init(0);
  D2Weights.Init(0);

  return Standard_True;
}

//=======================================================
// Purpose :BSplineSurface()
//=======================================================
 Handle(Geom_BSplineSurface) GeomFill_EvolvedSection::BSplineSurface() const
{
/*  Standard_Integer ii, NbPoles = myCurve->NbPoles();
  TColgp_Array2OfPnt Poles( 1, NbPoles, 1, 2);
  TColStd_Array1OfReal UKnots(1,myCurve->NbKnots()), VKnots(1,2); 
  TColStd_Array1OfInteger UMults(1,myCurve->NbKnots()), VMults(1,2);
  
  for (ii=1; ii <= NbPoles; ii++) {
    Poles(ii, 1) =  Poles(ii, 2) = myCurve->Pole(ii);
  }

  myCurve->Knots(UKnots);
  VKnots(1) = First;
  VKnots(2) = Last;

  myCurve->Multiplicities(UMults);
  VMults.Init(2);


  Handle(Geom_BSplineSurface) BS = 
    new (Geom_BSplineSurface) ( Poles,
			       UKnots, VKnots,
			       UMults, VMults,
			       myCurve->Degree(), 1,
			       myCurve->IsPeriodic());*/
  Handle(Geom_BSplineSurface) BS;
  BS.Nullify();
  return BS;

}
//=======================================================
// Purpose :SectionShape
//=======================================================
 void GeomFill_EvolvedSection::SectionShape(Standard_Integer& NbPoles,
					    Standard_Integer& NbKnots,
					    Standard_Integer& Degree) const
{
   NbPoles = myCurve->NbPoles();
   NbKnots = myCurve->NbKnots();
   Degree  = myCurve->Degree();
}

 void GeomFill_EvolvedSection::Knots(TColStd_Array1OfReal& TKnots) const
{
 myCurve->Knots(TKnots);
}
//=======================================================
// Purpose :Mults
//=======================================================
 void GeomFill_EvolvedSection::Mults(TColStd_Array1OfInteger& TMults) const
{
   myCurve->Multiplicities(TMults);
}


//=======================================================
// Purpose :IsRational
//=======================================================
 Standard_Boolean GeomFill_EvolvedSection::IsRational() const
{
  return myCurve->IsRational();
}

//=======================================================
// Purpose :IsUPeriodic
//=======================================================
 Standard_Boolean GeomFill_EvolvedSection::IsUPeriodic() const
{
  return myCurve->IsPeriodic();
}

//=======================================================
// Purpose :IsVPeriodic
//=======================================================
 Standard_Boolean GeomFill_EvolvedSection::IsVPeriodic() const
{
  return 
    (Abs(myLaw->Value(First) - myLaw->Value(Last)) < Precision::Confusion());
}

//=======================================================
// Purpose :NbIntervals
//=======================================================
 Standard_Integer GeomFill_EvolvedSection::NbIntervals(const GeomAbs_Shape S) const
{
  return myLaw->NbIntervals(S) ;
}


//=======================================================
// Purpose :Intervals
//=======================================================
 void GeomFill_EvolvedSection::Intervals(TColStd_Array1OfReal& T,
					 const GeomAbs_Shape S) const
{
  myLaw->Intervals(T, S) ; 
}


//=======================================================
// Purpose : SetInterval
//=======================================================
 void GeomFill_EvolvedSection::SetInterval(const Standard_Real F,
					   const Standard_Real L) 
{
 TLaw = myLaw->Trim(F, L, Precision::PConfusion());
}

//=======================================================
// Purpose : GetInterval
//=======================================================
 void GeomFill_EvolvedSection::GetInterval(Standard_Real& F,
					   Standard_Real& L) const
{
  TLaw->Bounds(F, L); 
}

//=======================================================
// Purpose : GetDomain
//=======================================================
 void GeomFill_EvolvedSection::GetDomain(Standard_Real& F,
					 Standard_Real& L) const
{
  F = First;
  L = Last;
}

//=======================================================
// Purpose : GetTolerance
//=======================================================
 void GeomFill_EvolvedSection::GetTolerance(const Standard_Real BoundTol,
					    const Standard_Real SurfTol,
//					    const Standard_Real AngleTol,
					    const Standard_Real ,
					    TColStd_Array1OfReal& Tol3d) const
{
  Tol3d.Init(SurfTol);
  if (BoundTol<SurfTol) {
    Tol3d(Tol3d.Lower()) = BoundTol;
    Tol3d(Tol3d.Upper()) = BoundTol;
  }
}

//=======================================================
// Purpose :
//=======================================================
 gp_Pnt GeomFill_EvolvedSection::BarycentreOfSurf() const
{
  Standard_Real U = mySection->FirstParameter(), Delta, b;
  Standard_Integer ii;
  gp_Pnt P, Bary;
  
  Delta = ( myCurve->LastParameter() - U ) / 20;
  Bary.SetCoord(0., 0., 0.);
  for (ii=0 ; ii <=20; ii++, U+=Delta) {
    P = myCurve->Value(U);
    Bary.ChangeCoord() += P.XYZ();
  } 

  U = First;
  Delta = ( Last - First) / 20;
  for (ii=0, b=0.0; ii <=20; ii++, U+=Delta) {
    b += myLaw->Value(U);
  }  
  Bary.ChangeCoord() *= b/(21*21);
  return Bary;
}

 Standard_Real GeomFill_EvolvedSection::MaximalSection() const
{
  Standard_Real L, val, max, U, Delta;
  Standard_Integer ii;
  GeomAdaptor_Curve AC (mySection);
  L = GCPnts_AbscissaPoint::Length(AC);

  Delta = ( Last - First) / 20;
  for (ii=0, max=0.0, U = First; ii <=20; ii++, U+=Delta) {
    val = myLaw->Value(U);
    if (val>max) max = val;
  }
  return L*max;
}

void GeomFill_EvolvedSection::GetMinimalWeight(TColStd_Array1OfReal& Weights) const
{
  if (myCurve->IsRational()) {
    myCurve->Weights(Weights);
  }
  else {
    Weights.Init(1);
  }
}

 Standard_Boolean GeomFill_EvolvedSection::IsConstant(Standard_Real& Error) const
{
//  Standard_Real isconst = Standard_False;
  Standard_Boolean isconst = Standard_False;
  Error = 0.;
  return isconst;
}

 Handle(Geom_Curve) GeomFill_EvolvedSection::ConstantSection() const
{
  Standard_Real Err, scale;
  if (!IsConstant(Err)) throw StdFail_NotDone("The Law is not Constant!");
  gp_Trsf T;
  gp_Pnt P(0, 0, 0);
  scale = myLaw->Value(First) +  
          myLaw->Value((First+Last)/2) +
          myLaw->Value(Last);
  T.SetScale(P, scale/3);
  
  Handle(Geom_Curve) C;
  C = Handle(Geom_Curve)::DownCast( mySection->Copy());
  C->Transform(T);
  return C;
}



