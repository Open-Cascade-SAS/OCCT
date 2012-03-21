// Created on: 1993-03-10
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <Geom_TrimmedCurve.ixx>

#include <gp.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Precision.hxx>
#include <ElCLib.hxx>


typedef Handle(Geom_TrimmedCurve) Handle(TrimmedCurve);
typedef Geom_TrimmedCurve         TrimmedCurve;
typedef Handle(Geom_Curve)        Handle(Curve);
typedef Handle(Geom_Geometry)     Handle(Geometry);
typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_TrimmedCurve::Copy () const {
 
  Handle(TrimmedCurve) Tc = new TrimmedCurve (basisCurve, uTrim1, uTrim2);
  return Tc;
}


//=======================================================================
//function : Geom_TrimmedCurve
//purpose  : 
//=======================================================================

Geom_TrimmedCurve::Geom_TrimmedCurve (const Handle(Geom_Curve)& C, 
				      const Standard_Real U1, 
				      const Standard_Real U2,
				      const Standard_Boolean Sense) :
       uTrim1 (U1),
       uTrim2 (U2) 
{
  // kill trimmed basis curves
  Handle(Geom_TrimmedCurve) T = Handle(Geom_TrimmedCurve)::DownCast(C);
  if (!T.IsNull())
    basisCurve = Handle(Curve)::DownCast(T->BasisCurve()->Copy());
  else
    basisCurve = Handle(Curve)::DownCast(C->Copy());

  SetTrim(U1,U2,Sense);
}

//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void Geom_TrimmedCurve::Reverse () 
{
  Standard_Real U1 = basisCurve->ReversedParameter(uTrim2);
  Standard_Real U2 = basisCurve->ReversedParameter(uTrim1);
  basisCurve->Reverse();
  SetTrim(U1,U2);
}


//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_TrimmedCurve::ReversedParameter
  (const Standard_Real U) const 
{
  return basisCurve->ReversedParameter(U);
}


//=======================================================================
//function : SetTrim
//purpose  : 
//=======================================================================

void Geom_TrimmedCurve::SetTrim (const Standard_Real U1, 
				 const Standard_Real U2, 
				 const Standard_Boolean Sense) 
{
   Standard_Boolean sameSense = Standard_True;
   if (U1 == U2) 
     Standard_ConstructionError::Raise("Geom_TrimmedCurve::U1 == U2");

   Standard_Real Udeb = basisCurve->FirstParameter();
   Standard_Real Ufin = basisCurve->LastParameter();

   if (basisCurve->IsPeriodic())  {
     sameSense = Sense;
      
     // set uTrim1 in the range Udeb , Ufin
     // set uTrim2 in the range uTrim1 , uTrim1 + Period()
     uTrim1 = U1;
     uTrim2 = U2;
     ElCLib::AdjustPeriodic(Udeb, Ufin, 
			    Min(Abs(uTrim2-uTrim1)/2,Precision::PConfusion()), 
			    uTrim1, uTrim2);
   }

   else {
     if (U1 < U2) {
       sameSense = Sense;
       uTrim1 = U1;
       uTrim2 = U2;
     }
     else {
       sameSense = !Sense;
       uTrim1 = U2;
       uTrim2 = U1;
     }

     if ((Udeb - uTrim1 > Precision::PConfusion()) ||
	 (uTrim2 - Ufin > Precision::PConfusion()))
      Standard_ConstructionError::Raise
	("Geom_TrimmedCurve::parameters out of range");
       

   }
   if (!sameSense) {
     Reverse();
   }
}


//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_TrimmedCurve::IsClosed () const
{
  return ( StartPoint().Distance(EndPoint()) <= gp::Resolution());
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_TrimmedCurve::IsPeriodic () const 
{
  //return basisCurve->IsPeriodic();
  return Standard_False;
}


//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real Geom_TrimmedCurve::Period() const
{
  return basisCurve->Period();
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_TrimmedCurve::Continuity () const { 

  return basisCurve->Continuity ();
}


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Curve) Geom_TrimmedCurve::BasisCurve () const { 

  return basisCurve;
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_TrimmedCurve::D0 (const Standard_Real U, Pnt& P) const {

    basisCurve->D0( U, P);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_TrimmedCurve::D1 (const Standard_Real U, Pnt& P, Vec& V1) const {

    basisCurve->D1 (U, P, V1);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_TrimmedCurve::D2 ( const Standard_Real U, 
			     Pnt& P, 
			     Vec& V1, Vec& V2) const {

  basisCurve->D2 (U, P, V1, V2);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_TrimmedCurve::D3 (const Standard_Real U, 
			    Pnt& P, 
			    Vec& V1, Vec& V2, Vec& V3) const {
 
  basisCurve->D3 (U, P, V1, V2, V3);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_TrimmedCurve::DN (const Standard_Real U, 
			   const Standard_Integer N) const 
{
   return basisCurve->DN (U, N);
}


//=======================================================================
//function : EndPoint
//purpose  : 
//=======================================================================

Pnt Geom_TrimmedCurve::EndPoint () const { 
 
  return basisCurve->Value (uTrim2);
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_TrimmedCurve::FirstParameter () const { 

  return uTrim1; 
}

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_TrimmedCurve::LastParameter () const {

  return uTrim2; 
}

//=======================================================================
//function : StartPoint
//purpose  : 
//=======================================================================

Pnt Geom_TrimmedCurve::StartPoint () const {

  return basisCurve->Value (uTrim1);
}


//=======================================================================
//function : IsCN
//purpose  : 
//=======================================================================

Standard_Boolean Geom_TrimmedCurve::IsCN (const Standard_Integer N) const {
  
  Standard_RangeError_Raise_if (N < 0, " ");
  return basisCurve->IsCN (N);
}

 
//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_TrimmedCurve::Transform (const Trsf& T) 
{
  basisCurve->Transform (T);
  Standard_Real U1 = basisCurve->TransformedParameter(uTrim1,T);
  Standard_Real U2 = basisCurve->TransformedParameter(uTrim2,T);
  SetTrim(U1,U2);
}


//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_TrimmedCurve::TransformedParameter(const Standard_Real U,
						      const gp_Trsf& T) const
{
  return basisCurve->TransformedParameter(U,T);
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

Standard_Real Geom_TrimmedCurve::ParametricTransformation(const gp_Trsf& T) 
const
{
  return basisCurve->ParametricTransformation(T);
}

