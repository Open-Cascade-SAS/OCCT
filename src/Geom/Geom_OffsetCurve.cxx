// Created on: 1991-06-25
// Created by: JCV
// Copyright (c) 1991-1999 Matra Datavision
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


// 24-Aug-95 : xab removed C1 and C2 test : appeller  D1 et D2 
//             avec discernement !
// 19-09-97  : JPI correction derivee seconde


#include <Geom_OffsetCurve.ixx>


#include <gp.hxx>
#include <gp_XYZ.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Geometry.hxx>

#include <Geom_UndefinedDerivative.hxx>
#include <Geom_UndefinedValue.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_NotImplemented.hxx>

typedef Geom_OffsetCurve         OffsetCurve;
typedef Handle(Geom_OffsetCurve) Handle(OffsetCurve);
typedef Geom_Curve               Curve;
typedef Handle(Geom_Curve)       Handle(Curve);
typedef Handle(Geom_Geometry)    Handle(Geometry);

typedef gp_Dir  Dir;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;
typedef gp_XYZ  XYZ;





static const int MaxDegree = 9;
   //ordre de derivation maximum pour la recherche de la premiere 
   //derivee non nulle



//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_OffsetCurve::Copy () const {

 Handle(OffsetCurve) C;
 C = new OffsetCurve (basisCurve, offsetValue, direction);
 return C;
}



//=======================================================================
//function : Geom_OffsetCurve
//purpose  : 
//=======================================================================

Geom_OffsetCurve::Geom_OffsetCurve (const Handle(Curve)& C,
				    const Standard_Real           Offset, 
				    const Dir&           V      )
 : direction(V), offsetValue(Offset) {
  
  if (C->DynamicType() == STANDARD_TYPE(Geom_OffsetCurve)) {
    Handle(OffsetCurve) OC = Handle(OffsetCurve)::DownCast(C->Copy());
    if ((OC->BasisCurve())->Continuity() == GeomAbs_C0)  
      Standard_ConstructionError::Raise();

    basisCurve = Handle(Curve)::DownCast((OC->BasisCurve())->Copy());
    Standard_Real PrevOff = OC->Offset();
    gp_Vec V1(OC->Direction());
    gp_Vec V2(direction);
    gp_Vec Vdir(PrevOff*V1 + offsetValue*V2);

    if (Offset >= 0.) {
      offsetValue = Vdir.Magnitude();
      direction.SetXYZ(Vdir.XYZ());
    } else {
      offsetValue = -Vdir.Magnitude();
      direction.SetXYZ((-Vdir).XYZ());
    }
  } else {
    if (C->Continuity() == GeomAbs_C0) Standard_ConstructionError::Raise();
    basisCurve = Handle(Curve)::DownCast(C->Copy()); // DownCast: 10-03-93
  }
}


//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::Reverse ()
{ 
  basisCurve->Reverse();
  offsetValue = -offsetValue;
}


//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetCurve::ReversedParameter( const Standard_Real U) const 
{
  return basisCurve->ReversedParameter( U);
}

//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

const gp_Dir& Geom_OffsetCurve::Direction () const               
  { return direction; }

//=======================================================================
//function : SetDirection
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::SetDirection (const Dir& V)     
  { direction = V; }

//=======================================================================
//function : SetOffsetValue
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::SetOffsetValue (const Standard_Real D)   
  { offsetValue = D; }


//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetCurve::IsPeriodic () const
{
  return basisCurve->IsPeriodic();
}

//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetCurve::Period () const
{
  return basisCurve->Period();
}

//=======================================================================
//function : SetBasisCurve
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::SetBasisCurve (const Handle(Curve)& C) {

  if (C->Continuity() == GeomAbs_C0) Standard_ConstructionError::Raise();
  basisCurve = Handle(Curve)::DownCast(C->Copy()); // DownCast: 10-03-93
}



//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Curve) Geom_OffsetCurve::BasisCurve () const 
{ 
  return basisCurve;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_OffsetCurve::Continuity () const {

  GeomAbs_Shape OffsetShape=GeomAbs_C0;
  switch (basisCurve->Continuity()) {
    case GeomAbs_C0 : OffsetShape = GeomAbs_C0;       break;
    case GeomAbs_C1 : OffsetShape = GeomAbs_C0;       break;
    case GeomAbs_C2 : OffsetShape = GeomAbs_C1;       break;
    case GeomAbs_C3 : OffsetShape = GeomAbs_C2;       break;
    case GeomAbs_CN : OffsetShape = GeomAbs_CN;       break;
    case GeomAbs_G1 : OffsetShape = GeomAbs_G1;       break;
    case GeomAbs_G2 : OffsetShape = GeomAbs_G2;       break;
  }
  return OffsetShape;
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::D0 (const Standard_Real U, Pnt& P) const 
{
  gp_Pnt PBasis;
  gp_Vec VBasis;
  D0(U,P,PBasis,VBasis);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::D1 (const Standard_Real U, Pnt& P, Vec& V1) const 
{
  gp_Pnt PBasis;
  gp_Vec V1Basis,V2Basis;
  D1(U,P,PBasis,V1,V1Basis,V2Basis);
}



//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::D2 (const Standard_Real U, Pnt& P, Vec& V1, Vec& V2) const 
{
  gp_Pnt PBasis;
  gp_Vec V1Basis,V2Basis,V3Basis;
  D2(U,P,PBasis,V1,V2,V1Basis,V2Basis,V3Basis);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::D3 (const Standard_Real U, Pnt& P, Vec& V1, Vec& V2, Vec& V3) 
const {


   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ V|| and Ndir = P' ^ direction (local normal direction)

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
   //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

   //P"'(u) = p"'(u) + (Offset / R) * (D3Ndir - (3.0 * Dr/R**2) * D2Ndir -
   //         (3.0 * D2r / R2) * DNdir + (3.0 * Dr * Dr / R4) * DNdir -
   //         (D3r/R2) * Ndir + (6.0 * Dr * Dr / R4) * Ndir +
   //         (6.0 * Dr * D2r / R4) * Ndir - (15.0 * Dr* Dr* Dr /R6) * Ndir



  basisCurve->D3 (U, P, V1, V2, V3);
  Vec V4 = basisCurve->DN (U, 4);
  Standard_Integer Index = 2;
  while (V1.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
    V1 = basisCurve->DN (U, Index);
    Index++;
  }
  if (Index != 2) {
    V2 = basisCurve->DN (U, Index);
    V3 = basisCurve->DN (U, Index + 1);
    V4 = basisCurve->DN (U, Index + 2);
  }
  XYZ OffsetDir = direction.XYZ();
  XYZ Ndir      = (V1.XYZ()).Crossed (OffsetDir);
  XYZ DNdir     = (V2.XYZ()).Crossed (OffsetDir);
  XYZ D2Ndir    = (V3.XYZ()).Crossed (OffsetDir);
  XYZ D3Ndir    = (V4.XYZ()).Crossed (OffsetDir);
  Standard_Real R2  = Ndir.SquareModulus();
  Standard_Real R   = Sqrt (R2);
  Standard_Real R3  = R2 * R;
  Standard_Real R4  = R2 * R2;
  Standard_Real R5  = R3 * R2;
  Standard_Real R6  = R3 * R3;
  Standard_Real R7  = R5 * R2;
  Standard_Real Dr  = Ndir.Dot (DNdir);
  Standard_Real D2r = Ndir.Dot (D2Ndir) + DNdir.Dot (DNdir);
  Standard_Real D3r = Ndir.Dot (D3Ndir) + 3.0 * DNdir.Dot (D2Ndir);
  if (R7 <= gp::Resolution()) {
    if (R6 <= gp::Resolution())  Geom_UndefinedDerivative::Raise();
    // V3 = P"' (U) :
    D3Ndir.Subtract (D2Ndir.Multiplied (3.0 * Dr / R2));
    D3Ndir.Subtract (DNdir.Multiplied (3.0 * ((D2r/R2) + (Dr*Dr/R4))));
    D3Ndir.Add (Ndir.Multiplied (6.0*Dr*Dr/R4 + 6.0*Dr*D2r/R4 -
                                 15.0*Dr*Dr*Dr/R6 - D3r));
    D3Ndir.Multiply (offsetValue/R);
    V3.Add (Vec(D3Ndir));
    // V2 = P" (U) :
    Standard_Real R4 = R2 * R2;
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * Dr / R2));
    D2Ndir.Subtract (Ndir.Multiplied ((3.0 * Dr * Dr / R4) - (D2r / R2)));
    D2Ndir.Multiply (offsetValue / R);
    V2.Add (Vec(D2Ndir));
    // V1 = P' (U) :
    DNdir.Multiply(R);
    DNdir.Subtract (Ndir.Multiplied (Dr/R));
    DNdir.Multiply (offsetValue/R2);
    V1.Add (Vec(DNdir));
  }
  else {
    // V3 = P"' (U) :
    D3Ndir.Divide (R);
    D3Ndir.Subtract (D2Ndir.Multiplied (3.0 * Dr / R3));
    D3Ndir.Subtract (DNdir.Multiplied ((3.0 * ((D2r/R3) + (Dr*Dr)/R5))));
    D3Ndir.Add (Ndir.Multiplied (6.0*Dr*Dr/R5 + 6.0*Dr*D2r/R5 - 
                15.0*Dr*Dr*Dr/R7 - D3r));
    D3Ndir.Multiply (offsetValue);
    V3.Add (Vec(D3Ndir));
    // V2 = P" (U) :
    D2Ndir.Divide (R);
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * Dr / R3));
    D2Ndir.Subtract (Ndir.Multiplied ((3.0 * Dr * Dr / R5) - (D2r / R3)));
    D2Ndir.Multiply (offsetValue);
    V2.Add (Vec(D2Ndir));
    // V1 = P' (U) :
    DNdir.Multiply (offsetValue/R);
    DNdir.Subtract (Ndir.Multiplied (offsetValue*Dr/R3));        
    V1.Add (Vec(DNdir));
  }
  //P (U) :
  Ndir.Multiply (offsetValue/R);
  Ndir.Add (P.XYZ());
  P.SetXYZ (Ndir);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_OffsetCurve::DN (const Standard_Real U, const Standard_Integer N) const {


  if (N < 1)                  Standard_RangeError::Raise();
  XYZ OffsetDir = direction.XYZ();
  Pnt P;
  Vec V1, V2, dummy;
  if (N == 1) {
    basisCurve->D2 (U, P, V1, V2);
    Standard_Integer Index = 2;
    while (V1.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
      V1 = basisCurve->DN (U, Index);
      Index++;
    }
    if (Index != 2)  V2 = basisCurve->DN (U, Index);
    XYZ Ndir  = (V1.XYZ()).Crossed (OffsetDir);
    XYZ DNdir = (V2.XYZ()).Crossed (OffsetDir);
    Standard_Real R2 = Ndir.SquareModulus();
    Standard_Real R  = Sqrt (R2);
    Standard_Real R3 = R * R2;
    Standard_Real Dr = Ndir.Dot (DNdir);
    if (R3 <= gp::Resolution()) {
      if (R2 <= gp::Resolution())  Geom_UndefinedDerivative::Raise();
      Ndir.Multiply (Dr/R);
      DNdir.Multiply(R);
      DNdir.Subtract (Ndir);
      DNdir.Multiply (offsetValue/R2);
      V1.Add (Vec(DNdir));
    }
    else {
      Ndir.Multiply (offsetValue * Dr / R3);
      DNdir.Multiply (offsetValue/R);
      DNdir.Subtract (Ndir);        
      V1.Add (Vec(DNdir));
    }
    dummy = V1;
  }
  else if (N == 2) {
    Vec V3;
    basisCurve->D3 (U, P, V1, V2, V3);
    Standard_Integer Index = 2;
    while (V1.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
      V1 = basisCurve->DN (U, Index);
      Index++;
    }
    if (Index != 2) {
      V2 = basisCurve->DN (U, Index);
      V3 = basisCurve->DN (U, Index + 1);
    }
    XYZ Ndir   = (V1.XYZ()).Crossed (OffsetDir);
    XYZ DNdir  = (V2.XYZ()).Crossed (OffsetDir);
    XYZ D2Ndir = (V3.XYZ()).Crossed (OffsetDir);
    Standard_Real R2  = Ndir.SquareModulus();
    Standard_Real R   = Sqrt (R2);
    Standard_Real R3  = R2 * R;   Standard_Real R4 = R2 * R2;  Standard_Real R5 = R3 * R2;
    Standard_Real Dr  = Ndir.Dot (DNdir);
    Standard_Real D2r = Ndir.Dot (D2Ndir) + DNdir.Dot (DNdir);
    if (R5 <= gp::Resolution()) {
      if (R4 <= gp::Resolution())  Geom_UndefinedDerivative::Raise();
      Ndir.Multiply ((3.0 * Dr * Dr / R4) - (D2r/R2));
      DNdir.Multiply (2.0 * Dr / R2);
      D2Ndir.Subtract (DNdir);
      D2Ndir.Subtract (Ndir);
      D2Ndir.Multiply (offsetValue / R);
      V2.Add (Vec(D2Ndir));
    }
    else {
      Ndir.Multiply ((3.0 * Dr * Dr / R4) - (D2r / R2));
      DNdir.Multiply (2.0 * Dr / R2);
      D2Ndir.Divide (R);
      D2Ndir.Subtract (DNdir);
      D2Ndir.Subtract (Ndir);
      D2Ndir.Multiply (offsetValue);
      V2.Add (Vec(D2Ndir));
    }
    dummy = V2;
  }
  else if (N == 3) {
    Vec V3;
    basisCurve->D3 (U, P, V1, V2, V3);
    Vec V4 = basisCurve->DN (U, 4);
    Standard_Integer Index = 2;
    while (V1.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
      V1 = basisCurve->DN (U, Index);
      Index++;
    }
    if (Index != 2) {
      V2 = basisCurve->DN (U, Index);
      V3 = basisCurve->DN (U, Index + 1);
      V4 = basisCurve->DN (U, Index + 2);
    }
    XYZ Ndir = (V1.XYZ()).Crossed (OffsetDir);
    XYZ DNdir = (V2.XYZ()).Crossed (OffsetDir);
    XYZ D2Ndir = (V3.XYZ()).Crossed (OffsetDir);
    XYZ D3Ndir = (V4.XYZ()).Crossed (OffsetDir);
    Standard_Real R2  = Ndir.SquareModulus();
    Standard_Real R   = Sqrt (R2);  Standard_Real R3 = R2 * R;  Standard_Real R4 = R2 * R2;
    Standard_Real R5  = R3 * R2;  Standard_Real R6 = R3 * R3;  Standard_Real R7 = R5 * R2;
    Standard_Real Dr  = Ndir.Dot (DNdir);
    Standard_Real D2r = Ndir.Dot (D2Ndir) + DNdir.Dot (DNdir);
    Standard_Real D3r = Ndir.Dot (D3Ndir) + 3.0 * DNdir.Dot (D2Ndir);
    if (R7 <= gp::Resolution()) {
      if (R6 <= gp::Resolution()) Geom_UndefinedDerivative::Raise();
      D2Ndir.Multiply (3.0 * Dr / R2);
      DNdir.Multiply (3.0 * ((D2r/R2) + (Dr*Dr)/R4));
      Ndir.Multiply (6.0*Dr*Dr/R4 + 6.0*Dr*D2r/R4 - 15.0*Dr*Dr*Dr/R6 - D3r);
      D3Ndir.Subtract (D2Ndir);
      D3Ndir.Subtract (DNdir);
      D3Ndir.Add (Ndir);
      D3Ndir.Multiply (offsetValue/R);
      V3.Add (Vec(D3Ndir));
    }
    else {
      D2Ndir.Multiply (3.0 * Dr / R3);
      DNdir.Multiplied (3.0 * ((D2r/R3) + (Dr*Dr/R5)));
      Ndir.Multiply (6.0*Dr*Dr/R5 + 6.0*Dr*D2r/R5 - 15.0*Dr*Dr*Dr/R7 - D3r);
      D3Ndir.Divide (R);
      D3Ndir.Subtract (D2Ndir);
      D3Ndir.Subtract (DNdir);
      D3Ndir.Add (Ndir);
      D3Ndir.Multiply (offsetValue);
      V3.Add (Vec(D3Ndir));
    }
    dummy = V3;
  }
  else { Standard_NotImplemented::Raise();  }

  return dummy;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void  Geom_OffsetCurve::D0(const Standard_Real U,
                           gp_Pnt& P,
                           gp_Pnt& Pbasis,
                           gp_Vec& V1basis)const 
{

  basisCurve->D1 (U, Pbasis, V1basis);
  Standard_Integer Index = 2;
  while (V1basis.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
    V1basis = basisCurve->DN (U, Index);
    Index++;
  }
  XYZ Ndir = (V1basis.XYZ()).Crossed (direction.XYZ());
  Standard_Real R = Ndir.Modulus();
  if (R <= gp::Resolution())  Geom_UndefinedValue::Raise();
  Ndir.Multiply (offsetValue/R);
  Ndir.Add (Pbasis.XYZ());
  P.SetXYZ(Ndir);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::D1 ( const Standard_Real U, 
                            Pnt& P , Pnt& PBasis ,
                            Vec& V1, Vec& V1basis, Vec& V2basis) const {

   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ V|| and Ndir = P' ^ direction (local normal direction)

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  basisCurve->D2 (U, PBasis, V1basis, V2basis);
  V1 = V1basis;
  Vec V2 = V2basis;
  Standard_Integer Index = 2;
  while (V1.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
    V1 = basisCurve->DN (U, Index);
    Index++;
  }
  if (Index != 2)  V2 = basisCurve->DN (U, Index);
  XYZ OffsetDir = direction.XYZ();
  XYZ Ndir  = (V1.XYZ()).Crossed (OffsetDir);
  XYZ DNdir = (V2.XYZ()).Crossed (OffsetDir);
  Standard_Real R2 = Ndir.SquareModulus();
  Standard_Real R  = Sqrt (R2);
  Standard_Real R3 = R * R2;
  Standard_Real Dr = Ndir.Dot (DNdir);
  if (R3 <= gp::Resolution()) {
    //We try another computation but the stability is not very good.
    if (R2 <= gp::Resolution()) Geom_UndefinedDerivative::Raise();
    DNdir.Multiply(R);
    DNdir.Subtract (Ndir.Multiplied (Dr/R));
    DNdir.Multiply (offsetValue/R2);
    V1.Add (Vec(DNdir));
  }
  else {
    // Same computation as IICURV in EUCLID-IS because the stability is
    // better
    DNdir.Multiply (offsetValue/R);
    DNdir.Subtract (Ndir.Multiplied (offsetValue * Dr/R3));        
    V1.Add (Vec(DNdir));
  }
  Ndir.Multiply (offsetValue/R);
  Ndir.Add (PBasis.XYZ());
  P.SetXYZ (Ndir);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::D2 (const Standard_Real U, 
                           Pnt& P      , Pnt& PBasis ,
                           Vec& V1     , Vec& V2     , 
                           Vec& V1basis, Vec& V2basis, Vec& V3basis) const {

   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ V|| and Ndir = P' ^ direction (local normal direction)

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
   //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  basisCurve->D3 (U, PBasis, V1basis, V2basis, V3basis);
  Standard_Integer Index = 2;
  V1     = V1basis;
  V2     = V2basis;
  Vec V3 = V3basis;
  while (V1.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
    V1 = basisCurve->DN (U, Index);
    Index++;
  }
  if (Index != 2) {
    V2 = basisCurve->DN (U, Index);
    V3 = basisCurve->DN (U, Index + 1);
  }
  XYZ OffsetDir = direction.XYZ();
  XYZ Ndir   = (V1.XYZ()).Crossed (OffsetDir);
  XYZ DNdir  = (V2.XYZ()).Crossed (OffsetDir);
  XYZ D2Ndir = (V3.XYZ()).Crossed (OffsetDir);
  Standard_Real R2    = Ndir.SquareModulus();
  Standard_Real R     = Sqrt (R2);
  Standard_Real R3    = R2 * R;
  Standard_Real R4    = R2 * R2;
  Standard_Real R5    = R3 * R2;
  Standard_Real Dr    = Ndir.Dot (DNdir);
  Standard_Real D2r   = Ndir.Dot (D2Ndir) + DNdir.Dot (DNdir);
  if (R5 <= gp::Resolution()) {
    //We try another computation but the stability is not very good
    //dixit ISG.
    if (R4 <= gp::Resolution())  Geom_UndefinedDerivative::Raise();
    // V2 = P" (U) :
    Standard_Real R4 = R2 * R2;
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * Dr / R2));
    D2Ndir.Add (Ndir.Multiplied (((3.0 * Dr * Dr)/R4) - (D2r/R2)));
    D2Ndir.Multiply (offsetValue / R);
    V2.Add (Vec(D2Ndir));
    // V1 = P' (U) :
    DNdir.Multiply(R);
    DNdir.Subtract (Ndir.Multiplied (Dr/R));
    DNdir.Multiply (offsetValue/R2);
    V1.Add (Vec(DNdir));
  }
  else {
    // Same computation as IICURV in EUCLID-IS because the stability is
    // better.
    // V2 = P" (U) :
    D2Ndir.Multiply (offsetValue/R);
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * offsetValue * Dr / R3));
    D2Ndir.Add (Ndir.Multiplied (
                     offsetValue * (((3.0 * Dr * Dr) / R5) - (D2r / R3))
                                     )
                    );
    V2.Add (Vec(D2Ndir));
    // V1 = P' (U) :
    DNdir.Multiply (offsetValue/R);
    DNdir.Subtract (Ndir.Multiplied (offsetValue*Dr/R3));        
    V1.Add (Vec(DNdir));
  }
  //P (U) :
  Ndir.Multiply (offsetValue/R);
  Ndir.Add (PBasis.XYZ());
  P.SetXYZ (Ndir);
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetCurve::FirstParameter () const {

   return basisCurve->FirstParameter();
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetCurve::LastParameter () const {

   return basisCurve->LastParameter();
}


//=======================================================================
//function : Offset
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetCurve::Offset () const { return offsetValue; }

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::Value (
const Standard_Real U, Pnt& P, Pnt& PBasis,  Vec& V1basis) const {

  if (basisCurve->Continuity() == GeomAbs_C0)  Geom_UndefinedValue::Raise();
  basisCurve->D1 (U, PBasis, V1basis);
  Standard_Integer Index = 2;
  while (V1basis.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
    V1basis = basisCurve->DN (U, Index);
    Index++;
  }
  XYZ Ndir = (V1basis.XYZ()).Crossed (direction.XYZ());
  Standard_Real R = Ndir.Modulus();
  if (R <= gp::Resolution())  Geom_UndefinedValue::Raise();
  Ndir.Multiply (offsetValue/R);
  Ndir.Add (PBasis.XYZ());
  P.SetXYZ (Ndir);
}


//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetCurve::IsClosed () const 
{ 
  gp_Pnt PF,PL;
  D0(FirstParameter(),PF);
  D0(LastParameter(),PL);
  return ( PF.Distance(PL) <= gp::Resolution());
}



//=======================================================================
//function : IsCN
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetCurve::IsCN (const Standard_Integer N) const {

   Standard_RangeError_Raise_if (N < 0, " ");
   return basisCurve->IsCN (N + 1);
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::Transform (const Trsf& T) {

  basisCurve->Transform (T);
  direction.Transform(T);
  offsetValue *= T.ScaleFactor();
}

//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetCurve::TransformedParameter(const Standard_Real U,
						     const gp_Trsf& T) const
{
  return basisCurve->TransformedParameter(U,T);
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetCurve::ParametricTransformation(const gp_Trsf& T)
const
{
  return basisCurve->ParametricTransformation(T);
}
