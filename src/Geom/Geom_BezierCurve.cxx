// Created on: 1993-03-09
// Created by: JCV
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

// Passage en classe persistante - 23/01/91
// Modif suite a la deuxieme revue de projet toolkit Geometry -23/01/91
// Infos :
// Actuellement pour les champs de la courbe le tableau des poles est 
// declare de 1 a NbPoles et le tableau des poids est declare de 1 a NbPoles

// Revised RLE  Aug 19 1993
// Suppressed Swaps, added Init, removed typedefs

#define No_Standard_OutOfRange
#define No_Standard_DimensionError


#include <Geom_BezierCurve.hxx>
#include <Geom_Geometry.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_BezierCurve,Geom_BoundedCurve)

//=======================================================================
//function : Rational
//purpose  : check rationality of an array of weights
//=======================================================================
static Standard_Boolean Rational(const TColStd_Array1OfReal& W)
{
  Standard_Integer i, n = W.Length();
  Standard_Boolean rat = Standard_False;
  for (i = 1; i < n; i++) {
    rat =  Abs(W(i) - W(i+1)) > gp::Resolution();
    if (rat) break;
  }
  return rat;
}

//=======================================================================
//function : Geom_BezierCurve
//purpose  : 
//=======================================================================

Geom_BezierCurve::Geom_BezierCurve(const TColgp_Array1OfPnt& Poles)
{
  Standard_Integer nbpoles = Poles.Length();
  if(nbpoles < 2 || nbpoles > (Geom_BezierCurve::MaxDegree() + 1))
    throw Standard_ConstructionError();
  //  copy the poles
  Handle(TColgp_HArray1OfPnt) npoles =
    new TColgp_HArray1OfPnt(1,nbpoles);
  
  npoles->ChangeArray1() = Poles;
  
  // Init non rational
  Init(npoles,
       Handle(TColStd_HArray1OfReal)());
}

//=======================================================================
//function : Geom_BezierCurve
//purpose  : 
//=======================================================================

Geom_BezierCurve::Geom_BezierCurve(const TColgp_Array1OfPnt&  Poles, 
                                   const TColStd_Array1OfReal& Weights)
{
  // copy the poles
  Standard_Integer nbpoles = Poles.Length();
  if(nbpoles < 2 || nbpoles > (Geom_BezierCurve::MaxDegree() + 1))
    throw Standard_ConstructionError();

  Handle(TColgp_HArray1OfPnt) npoles =
    new TColgp_HArray1OfPnt(1,nbpoles);

  npoles->ChangeArray1() = Poles;


  // check  the weights

  if (Weights.Length() != nbpoles)
    throw Standard_ConstructionError();

  Standard_Integer i;
  for (i = 1; i <= nbpoles; i++) {
    if (Weights(i) <= gp::Resolution()) {
      throw Standard_ConstructionError();
    }
  }
  
  // check really rational
  Standard_Boolean rat = Rational(Weights);

  // copy the weights
  Handle(TColStd_HArray1OfReal) nweights;
  if (rat) {
    nweights = new TColStd_HArray1OfReal(1,nbpoles);
    nweights->ChangeArray1() = Weights;
  }

  // Init
  Init(npoles,nweights);
}

//=======================================================================
//function : Increase
//purpose  : increase degree
//=======================================================================

void Geom_BezierCurve::Increase (const Standard_Integer Deg)
{
  if (Deg == Degree()) return;

  if(Deg < Degree() ||
     Deg > Geom_BezierCurve::MaxDegree()) 
    throw Standard_ConstructionError("Geom_BezierCurve::Increase");

  Handle(TColgp_HArray1OfPnt) npoles =
    new TColgp_HArray1OfPnt(1,Deg+1);

  Handle(TColStd_HArray1OfReal) nweights;

  TColStd_Array1OfReal bidknots(1,2); bidknots(1) = 0.; bidknots(2) = 1.;
  TColStd_Array1OfInteger bidmults(1,2); bidmults.Init(Degree() + 1);

  if (IsRational()) {
    nweights = new TColStd_HArray1OfReal(1,Deg+1);
    BSplCLib::IncreaseDegree(Degree(), Deg, 0,
			     poles->Array1(),
			     &weights->Array1(),
			     bidknots, bidmults,
			     npoles->ChangeArray1(),
			     &nweights->ChangeArray1(),
			     bidknots, bidmults);
  }
  else {
    BSplCLib::IncreaseDegree(Degree(), Deg, 0,
			     poles->Array1(),
			     BSplCLib::NoWeights(),
			     bidknots, bidmults,
			     npoles->ChangeArray1(),
			     BSplCLib::NoWeights(),
			     bidknots, bidmults);
  }

  Init(npoles,nweights);
}

//=======================================================================
//function : MaxDegree
//purpose  : 
//=======================================================================

Standard_Integer Geom_BezierCurve::MaxDegree () 
{ 
  return BSplCLib::MaxDegree(); 
}

//=======================================================================
//function : InsertPoleAfter
//purpose  : 
//=======================================================================

void Geom_BezierCurve::InsertPoleAfter
(const Standard_Integer Index,
 const gp_Pnt& P)
{
  InsertPoleAfter(Index,P,1.);
}

//=======================================================================
//function : InsertPoleAfter
//purpose  : 
//=======================================================================

void Geom_BezierCurve::InsertPoleAfter
(const Standard_Integer Index,
 const gp_Pnt& P,
 const Standard_Real Weight)
{
  Standard_Integer nbpoles = NbPoles();

  if(nbpoles >= Geom_BezierCurve::MaxDegree() ||
     Weight <= gp::Resolution())
    throw Standard_ConstructionError("Geom_BezierCurve::InsertPoleAfter");

  if(Index < 0 || Index > nbpoles)
    throw Standard_OutOfRange("Geom_BezierCurve::InsertPoleAfter");

  Standard_Integer i;

  // Insert the pole
  Handle(TColgp_HArray1OfPnt) npoles =
    new TColgp_HArray1OfPnt(1,nbpoles+1);
  
  TColgp_Array1OfPnt&        newpoles = npoles->ChangeArray1();
  const TColgp_Array1OfPnt& oldpoles  = poles->Array1();

  for (i = 1; i <= Index; i++)
    newpoles(i) = oldpoles(i);

  newpoles(Index+1) = P;

  for (i = Index+1; i <= nbpoles; i++)
    newpoles(i+1) = oldpoles(i);


  // Insert the weight
  Handle(TColStd_HArray1OfReal) nweights;
  Standard_Boolean rat = IsRational() || Abs(Weight-1.) > gp::Resolution();

  if (rat) {
    nweights = new TColStd_HArray1OfReal(1,nbpoles+1);
    TColStd_Array1OfReal& newweights = nweights->ChangeArray1();

    for (i = 1; i <= Index; i++)
      if (IsRational())
	newweights(i) = weights->Value(i);
      else
	newweights(i) = 1.;

    newweights(Index+1) = Weight;

    for (i = Index+1; i <= nbpoles; i++)
      if (IsRational())
	newweights(i+1) = weights->Value(i);
      else
	newweights(i+1) = 1.;
  }


  Init(npoles,nweights);
}

//=======================================================================
//function : InsertPoleBefore
//purpose  : 
//=======================================================================

void Geom_BezierCurve::InsertPoleBefore
(const Standard_Integer Index,
 const gp_Pnt& P)
{
  InsertPoleAfter(Index-1,P);
}

//=======================================================================
//function : InsertPoleBefore
//purpose  : 
//=======================================================================

void Geom_BezierCurve::InsertPoleBefore
(const Standard_Integer Index,
 const gp_Pnt& P,
 const Standard_Real Weight)
{
  InsertPoleAfter(Index-1,P,Weight);
}

//=======================================================================
//function : RemovePole
//purpose  : 
//=======================================================================

void Geom_BezierCurve::RemovePole
(const Standard_Integer Index)
{
  Standard_Integer nbpoles = NbPoles();

  if(nbpoles <= 2)
    throw Standard_ConstructionError("Geom_BezierCurve::RemovePole");

  if(Index < 1 || Index > nbpoles)
    throw Standard_OutOfRange("Geom_BezierCurve::RemovePole");

  Standard_Integer i;

  // Remove the pole
  Handle(TColgp_HArray1OfPnt) npoles =
    new TColgp_HArray1OfPnt(1,nbpoles-1);
  
  TColgp_Array1OfPnt&        newpoles = npoles->ChangeArray1();
  const TColgp_Array1OfPnt& oldpoles  = poles->Array1();

  for (i = 1; i < Index; i++)
    newpoles(i) = oldpoles(i);

  for (i = Index+1; i <= nbpoles; i++)
    newpoles(i-1) = oldpoles(i);


  // Remove the weight
  Handle(TColStd_HArray1OfReal) nweights;

  if (IsRational()) {
    nweights = new TColStd_HArray1OfReal(1,nbpoles-1);
    TColStd_Array1OfReal&       newweights = nweights->ChangeArray1();
    const TColStd_Array1OfReal& oldweights = weights->Array1();

    for (i = 1; i < Index; i++)
      newweights(i) = oldweights(i);

    for (i = Index+1; i <= nbpoles; i++)
      newweights(i-1) = oldweights(i);
  }

  Init(npoles,nweights);
}

//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void Geom_BezierCurve::Reverse ()
{
  gp_Pnt P;
  Standard_Integer i, nbpoles = NbPoles();
  TColgp_Array1OfPnt & cpoles = poles->ChangeArray1();
  
  // reverse poles
  for (i = 1; i <= nbpoles / 2; i++) {
    P = cpoles(i);
    cpoles(i) = cpoles(nbpoles-i+1);
    cpoles(nbpoles-i+1) = P;
  }

  // reverse weights
  if (IsRational()) {
    TColStd_Array1OfReal & cweights = weights->ChangeArray1();
    Standard_Real w;
    for (i = 1; i <= nbpoles / 2; i++) {
      w = cweights(i);
      cweights(i) = cweights(nbpoles-i+1);
      cweights(nbpoles-i+1) = w;
    }
  }
}

//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_BezierCurve::ReversedParameter(const Standard_Real U) const 
{
  return ( 1. - U);
}

//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

void Geom_BezierCurve::Segment(const Standard_Real U1, const Standard_Real U2)
{
  closed =  (Abs(Value(U1).Distance (Value(U2))) <= Precision::Confusion());

  TColStd_Array1OfReal bidflatknots(BSplCLib::FlatBezierKnots(Degree()), 1, 2 * (Degree() + 1));
  TColgp_HArray1OfPnt coeffs(1, poles->Size());
  if (IsRational()) {
    TColStd_Array1OfReal wcoeffs(1, poles->Size());
    BSplCLib::BuildCache(0.0, 1.0, 0, Degree(), bidflatknots,
        poles->Array1(), &weights->Array1(), coeffs, &wcoeffs);
    PLib::Trimming(U1, U2, coeffs, &wcoeffs);
    PLib::CoefficientsPoles(coeffs, &wcoeffs, poles->ChangeArray1(), &weights->ChangeArray1());
  }
  else {
    BSplCLib::BuildCache(0.0, 1.0, 0, Degree(), bidflatknots,
        poles->Array1(), BSplCLib::NoWeights(), coeffs, BSplCLib::NoWeights());
    PLib::Trimming(U1, U2, coeffs, PLib::NoWeights());
    PLib::CoefficientsPoles(coeffs, PLib::NoWeights(), poles->ChangeArray1(), PLib::NoWeights());
  }
}

//=======================================================================
//function : SetPole
//purpose  : 
//=======================================================================

void Geom_BezierCurve::SetPole (const Standard_Integer Index,
				const gp_Pnt& P)
{
  if(Index < 1 || Index > NbPoles())
    throw Standard_OutOfRange("Geom_BezierCurve::SetPole");
  
  TColgp_Array1OfPnt& cpoles = poles->ChangeArray1();
  cpoles(Index) = P;
  
  if (Index == 1 || Index == cpoles.Length()) {
    closed = (cpoles(1).Distance(cpoles(NbPoles())) <= Precision::Confusion());
  }
}

//=======================================================================
//function : SetPole
//purpose  : 
//=======================================================================

void Geom_BezierCurve::SetPole(const Standard_Integer Index,
			       const gp_Pnt& P,
			       const Standard_Real Weight)
{
  SetPole(Index,P);
  SetWeight(Index,Weight);
}

//=======================================================================
//function : SetWeight
//purpose  : 
//=======================================================================

void Geom_BezierCurve::SetWeight(const Standard_Integer Index,
				 const Standard_Real Weight)
{
  Standard_Integer nbpoles = NbPoles();
  
  if(Index < 1 || Index > nbpoles)
    throw Standard_OutOfRange("Geom_BezierCurve::SetWeight");
  if(Weight <= gp::Resolution ())
    throw Standard_ConstructionError("Geom_BezierCurve::SetWeight");
  
  // compute new rationality
  Standard_Boolean wasrat = IsRational();
  if (!wasrat) {
    // a weight of 1. does not turn to rational
    if (Abs(Weight - 1.) <= gp::Resolution()) return;
    
    // set weights of 1.
    weights = new TColStd_HArray1OfReal(1,nbpoles);
    weights->Init(1.);
  }
  
  TColStd_Array1OfReal & cweights = weights->ChangeArray1();
  cweights(Index) = Weight;
  
  // is it turning into non rational
  if (wasrat && !Rational(cweights))
    weights.Nullify();
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierCurve::IsClosed () const 
{
  return closed; 
}

//=======================================================================
//function : IsCN
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierCurve::IsCN (const Standard_Integer ) const 
{
  return Standard_True; 
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierCurve::IsPeriodic () const 
{
  return Standard_False; 
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BezierCurve::IsRational () const 
{  
  return !weights.IsNull(); 
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_BezierCurve::Continuity () const 
{ 
  return GeomAbs_CN; 
}

//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer Geom_BezierCurve::Degree () const 
{
  return poles->Length()-1;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_BezierCurve::D0 (const Standard_Real U, gp_Pnt& P ) const
{
  BSplCLib::D0(U, Poles(), Weights(), P);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_BezierCurve::D1(const Standard_Real U, gp_Pnt& P, gp_Vec& V1) const
{
  BSplCLib::D1(U, Poles(), Weights(), P, V1);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_BezierCurve::D2 (const Standard_Real U,
			   gp_Pnt& P,
			   gp_Vec& V1,
			   gp_Vec& V2) const
{
  BSplCLib::D2(U, Poles(), Weights(), P, V1, V2);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_BezierCurve::D3 (const Standard_Real U,
			   gp_Pnt& P,
			   gp_Vec& V1,
			   gp_Vec& V2,
			   gp_Vec& V3) const
{
  BSplCLib::D3(U, Poles(), Weights(), P, V1, V2, V3);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec Geom_BezierCurve::DN (const Standard_Real U,
			     const Standard_Integer N) const
{
  if(N < 1) 
    throw Standard_RangeError("Geom_BezierCurve::DN");
  gp_Vec V;

  TColStd_Array1OfReal bidknots(1,2); bidknots(1) = 0.; bidknots(2) = 1.;
  TColStd_Array1OfInteger bidmults(1,2); bidmults.Init(Degree() + 1);

  if (IsRational())
//    BSplCLib::DN(U,N,0,Degree(),0.,
    BSplCLib::DN(U,N,0,Degree(),Standard_False,
		 poles->Array1(),
		 &weights->Array1(),
		 bidknots,&bidmults,V);
  else 
//    BSplCLib::DN(U,N,0,Degree(),0.,
    BSplCLib::DN(U,N,0,Degree(),Standard_False,
		 poles->Array1(),
		 BSplCLib::NoWeights(),
		 bidknots,&bidmults,V);
  return V;
}

//=======================================================================
//function : StartPoint
//purpose  : 
//=======================================================================

gp_Pnt Geom_BezierCurve::StartPoint () const
{
  return poles->Value(1);
}

//=======================================================================
//function : EndPoint
//purpose  : 
//=======================================================================

gp_Pnt Geom_BezierCurve::EndPoint () const
{
  return poles->Value (poles->Upper());
}

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_BezierCurve::FirstParameter () const 
{
  return 0.0; 
}

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_BezierCurve::LastParameter () const 
{
  return 1.0; 
}

//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer Geom_BezierCurve::NbPoles () const 
{
  return poles->Length(); 
}

//=======================================================================
//function : Pole
//purpose  : 
//=======================================================================

const gp_Pnt& Geom_BezierCurve::Pole (const Standard_Integer Index) const
{
  if(Index < 1 || Index > poles->Length()) 
    throw Standard_OutOfRange("Geom_BezierCurve::Pole");
  return poles->Value(Index);
}

//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void Geom_BezierCurve::Poles (TColgp_Array1OfPnt& P) const
{
  if(P.Length() != poles->Length()) 
    throw Standard_DimensionError("Geom_BezierCurve::Poles");
  P = poles->Array1();
}


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

const TColgp_Array1OfPnt& Geom_BezierCurve::Poles() const
{
  return poles->Array1();
}

//=======================================================================
//function : Weight
//purpose  : 
//=======================================================================

Standard_Real Geom_BezierCurve::Weight
(const Standard_Integer Index) const
{
  if(Index < 1 || Index > poles->Length()) 
    throw Standard_OutOfRange("Geom_BezierCurve::Weight");
  if (IsRational())
    return weights->Value(Index);
  else
    return 1.;
}

//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void Geom_BezierCurve::Weights
(TColStd_Array1OfReal& W) const
{

  Standard_Integer nbpoles = NbPoles();
  if(W.Length() != nbpoles)
    throw Standard_DimensionError("Geom_BezierCurve::Weights");
  if (IsRational())
    W = weights->Array1();
  else {
    Standard_Integer i;
    for (i = 1; i <= nbpoles; i++)
      W(i) = 1.;
  }
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_BezierCurve::Transform (const gp_Trsf& T)
{
  Standard_Integer nbpoles = NbPoles();
  TColgp_Array1OfPnt & cpoles = poles->ChangeArray1();

  for (Standard_Integer i = 1; i <= nbpoles; i++) 
    cpoles (i).Transform(T);
}

//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

void Geom_BezierCurve::Resolution(const Standard_Real Tolerance3D,
				  Standard_Real &     UTolerance) 
{
  if(!maxderivinvok){
    TColStd_Array1OfReal bidflatknots(BSplCLib::FlatBezierKnots(Degree()),
                                      1, 2*(Degree()+1));
    
    if (IsRational()) {  
      BSplCLib::Resolution(poles->Array1(),
			   &weights->Array1(),
			   poles->Length(),
			   bidflatknots,
			   Degree(),
			   1.,
			   maxderivinv) ;
    }
    else {
      BSplCLib::Resolution(poles->Array1(),
			   BSplCLib::NoWeights(),
			   poles->Length(),
			   bidflatknots,
			   Degree(),
			   1.,
			   maxderivinv) ;
    }
    maxderivinvok = 1;
  }
  UTolerance = Tolerance3D * maxderivinv;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_BezierCurve::Copy() const {

  Handle(Geom_BezierCurve) C;
  if (IsRational())
    C = new Geom_BezierCurve (poles->Array1(),weights->Array1());
  else
    C = new Geom_BezierCurve (poles->Array1());
  return C;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom_BezierCurve::Init
(const Handle(TColgp_HArray1OfPnt)&   Poles, 
 const Handle(TColStd_HArray1OfReal)& Weights)
{
  Standard_Integer nbpoles = Poles->Length();
  // closed ?
  const TColgp_Array1OfPnt&   cpoles   = Poles->Array1();
  closed = cpoles(1).Distance(cpoles(nbpoles)) <= Precision::Confusion(); 

  // rational
  rational = !Weights.IsNull();

  // set fields
  poles   = Poles;

  if (rational)
    weights = Weights;
  else
    weights.Nullify();
}

