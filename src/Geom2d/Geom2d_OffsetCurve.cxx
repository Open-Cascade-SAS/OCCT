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



//  modified by Edward AGAPOV (eap) Jan 28 2002 --- DN(), occ143(BUC60654)



#include <Geom2d_OffsetCurve.ixx>
#include <gp.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Geom2d_UndefinedDerivative.hxx>
#include <Geom2d_UndefinedValue.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_XY.hxx>

typedef Handle(Geom2d_OffsetCurve) Handle(OffsetCurve);
typedef Geom2d_OffsetCurve         OffsetCurve;
typedef Handle(Geom2d_Geometry)    Handle(Geometry);
typedef Handle(Geom2d_Curve)       Handle(Curve);
typedef Geom2d_Curve               Curve;
typedef gp_Dir2d  Dir2d;
typedef gp_Pnt2d  Pnt2d;
typedef gp_Vec2d  Vec2d;
typedef gp_Trsf2d Trsf2d;
typedef gp_XY     XY;



static const int MaxDegree = 9;
//ordre de derivation maximum pour la recherche de la premiere 
//derivee non nulle



//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom2d_Geometry) Geom2d_OffsetCurve::Copy () const 
{
  Handle(OffsetCurve) C;
  C = new OffsetCurve (basisCurve, offsetValue);
  return C;
}


//=======================================================================
//function : Geom2d_OffsetCurve
//purpose  : 
//=======================================================================

Geom2d_OffsetCurve::Geom2d_OffsetCurve (const Handle(Curve)& C,
					const Standard_Real Offset)  
: offsetValue (Offset) 
{
  if (C->DynamicType() == STANDARD_TYPE(Geom2d_OffsetCurve)) {
    Handle(OffsetCurve) OC = Handle(OffsetCurve)::DownCast(C->Copy());
    if ((OC->BasisCurve())->Continuity() == GeomAbs_C0)  
      Standard_ConstructionError::Raise();

    basisCurve = Handle(Curve)::DownCast((OC->BasisCurve())->Copy());
    offsetValue += OC->Offset();
  } else {
    if (C->Continuity() == GeomAbs_C0)  
      Standard_ConstructionError::Raise();

    basisCurve = Handle(Curve)::DownCast(C->Copy());
  }
}

//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::Reverse () 
{
  basisCurve->Reverse(); 
  offsetValue = -offsetValue;
}

//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_OffsetCurve::ReversedParameter( const Standard_Real U) const
{
  return basisCurve->ReversedParameter( U); 
}

//=======================================================================
//function : SetBasisCurve
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::SetBasisCurve (const Handle(Curve)& C) 
{
  if (C->Continuity() == GeomAbs_C0)  Standard_ConstructionError::Raise();
  basisCurve = Handle(Geom2d_Curve)::DownCast(C->Copy());
}

//=======================================================================
//function : SetOffsetValue
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::SetOffsetValue (const Standard_Real D) { offsetValue = D; }

//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Curve) Geom2d_OffsetCurve::BasisCurve () const 
{ 
  return basisCurve;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom2d_OffsetCurve::Continuity () const 
{
  GeomAbs_Shape OffsetShape=GeomAbs_C0;
  switch (basisCurve->Continuity()) {
     case GeomAbs_C0 : OffsetShape = GeomAbs_C0;   break;
     case GeomAbs_C1 : OffsetShape = GeomAbs_C0;   break;
     case GeomAbs_C2 : OffsetShape = GeomAbs_C1;   break;
     case GeomAbs_C3 : OffsetShape = GeomAbs_C2;   break;
     case GeomAbs_CN : OffsetShape = GeomAbs_CN;   break;
     case GeomAbs_G1 : OffsetShape = GeomAbs_G1;   break;
     case GeomAbs_G2 : OffsetShape = GeomAbs_G2;   break;
  }
  return OffsetShape;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D0 (const Standard_Real   U,
			           Pnt2d& P ) const 
{
  Vec2d V1;

  basisCurve->D1 (U, P, V1);
  Standard_Integer Index = 2;
  while (V1.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
    V1 = basisCurve->DN (U, Index);
    Index++;
  }
  Standard_Real A = V1.Y();
  Standard_Real B = - V1.X();
  Standard_Real R = Sqrt(A*A + B * B);
  if (R <= gp::Resolution())  Geom2d_UndefinedValue::Raise();
  A = A * offsetValue/R;
  B = B * offsetValue/R;
  P.SetCoord(P.X() + A, P.Y() + B);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D1 (const Standard_Real U, Pnt2d& P, Vec2d& V1) const {

   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  Vec2d V2;
  basisCurve->D2 (U, P, V1, V2);
  Standard_Integer Index = 2;
  while (V1.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
    V1 = basisCurve->DN (U, Index);
    Index++;
  }
  if (Index != 2) { V2 = basisCurve->DN (U, Index); }
  XY Ndir  (V1.Y(), -V1.X());
  XY DNdir (V2.Y(), -V2.X());
  Standard_Real R2 = Ndir.SquareModulus();
  Standard_Real R  = Sqrt (R2);
  Standard_Real R3 = R * R2;
  Standard_Real Dr = Ndir.Dot (DNdir);
  if (R3 <= gp::Resolution()) {
    //We try another computation but the stability is not very good.
    if (R2 <= gp::Resolution()) Geom2d_UndefinedDerivative::Raise();
    DNdir.Multiply(R);
    DNdir.Subtract (Ndir.Multiplied (Dr/R));
    DNdir.Multiply (offsetValue/R2);
    V1.Add (Vec2d(DNdir));
  }
  else {
    // Same computation as IICURV in EUCLID-IS because the stability is
    // better
    DNdir.Multiply (offsetValue/R);
    DNdir.Subtract (Ndir.Multiplied (offsetValue*Dr/R3));        
    V1.Add (Vec2d(DNdir));
  }
  Ndir.Multiply (offsetValue/R);
  Ndir.Add (P.XY());
  P.SetXY (Ndir);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D2 (const Standard_Real U, 
			           Pnt2d& P, 
			           Vec2d& V1, Vec2d& V2) const 
{
   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
   //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  Vec2d V3;
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
  XY Ndir (V1.Y(), -V1.X());
  XY DNdir (V2.Y(), -V2.X());
  XY D2Ndir (V3.Y(), -V3.X());
  Standard_Real R2  = Ndir.SquareModulus();
  Standard_Real R   = Sqrt (R2);
  Standard_Real R3  = R2 * R;
  Standard_Real R4  = R2 * R2;
  Standard_Real R5  = R3 * R2;
  Standard_Real Dr  = Ndir.Dot (DNdir);
  Standard_Real D2r = Ndir.Dot (D2Ndir) + DNdir.Dot (DNdir);
  if (R5 <= gp::Resolution()) {
    //We try another computation but the stability is not very good
    //dixit ISG.
    if (R4 <= gp::Resolution()) { Geom2d_UndefinedDerivative::Raise(); }
    // V2 = P" (U) :
     Standard_Real R4 = R2 * R2;
     D2Ndir.Subtract (DNdir.Multiplied (2.0 * Dr / R2));
     D2Ndir.Add (Ndir.Multiplied (((3.0 * Dr * Dr)/R4) - (D2r/R2)));
     D2Ndir.Multiply (offsetValue / R);
     V2.Add (Vec2d(D2Ndir));
     // V1 = P' (U) :
     DNdir.Multiply(R);
     DNdir.Subtract (Ndir.Multiplied (Dr/R));
     DNdir.Multiply (offsetValue/R2);
     V1.Add (Vec2d(DNdir));
   }
   else {
     // Same computation as IICURV in EUCLID-IS because the stability is
     // better.
     // V2 = P" (U) :
    D2Ndir.Multiply (offsetValue/R);
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * offsetValue * Dr / R3));
    D2Ndir.Add (Ndir.Multiplied 
		     (offsetValue * (((3.0 * Dr * Dr) / R5) - (D2r / R3))));
    V2.Add (Vec2d(D2Ndir));
    // V1 = P' (U) 
     DNdir.Multiply (offsetValue/R);
     DNdir.Subtract (Ndir.Multiplied (offsetValue*Dr/R3));        
     V1.Add (Vec2d(DNdir));
   }
   //P (U) :
   Ndir.Multiply (offsetValue/R);
   Ndir.Add (P.XY());
   P.SetXY (Ndir);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D3 (const Standard_Real U, 
                                   Pnt2d& P, 
                                   Vec2d& V1, Vec2d& V2, Vec2d& V3) const {


   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
   //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

   //P"'(u) = p"'(u) + (Offset / R) * (D3Ndir - (3.0 * Dr/R**2 ) * D2Ndir -
   //         (3.0 * D2r / R2) * DNdir) + (3.0 * Dr * Dr / R4) * DNdir -
   //         (D3r/R2) * Ndir + (6.0 * Dr * Dr / R4) * Ndir +
   //         (6.0 * Dr * D2r / R4) * Ndir - (15.0 * Dr* Dr* Dr /R6) * Ndir



   basisCurve->D3 (U, P, V1, V2, V3);
   Vec2d V4 = basisCurve->DN (U, 4);
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
   XY Ndir   (V1.Y(), -V1.X());
   XY DNdir  (V2.Y(), -V2.X());
   XY D2Ndir (V3.Y(), -V3.X());
   XY D3Ndir (V4.Y(), -V4.X());
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
     //We try another computation but the stability is not very good
     //dixit ISG.
     if (R6 <= gp::Resolution()) Geom2d_UndefinedDerivative::Raise();
     // V3 = P"' (U) :
     D3Ndir.Subtract (D2Ndir.Multiplied (3.0 * offsetValue * Dr / R2));
     D3Ndir.Subtract (
     (DNdir.Multiplied ((3.0 * offsetValue) * ((D2r/R2) + (Dr*Dr)/R4))));
     D3Ndir.Add (Ndir.Multiplied (
     (offsetValue * (6.0*Dr*Dr/R4 + 6.0*Dr*D2r/R4 - 15.0*Dr*Dr*Dr/R6 - D3r))
     ));
     D3Ndir.Multiply (offsetValue/R);
     V3.Add (Vec2d(D3Ndir));
     // V2 = P" (U) :
     Standard_Real R4 = R2 * R2;
     D2Ndir.Subtract (DNdir.Multiplied (2.0 * Dr / R2));
     D2Ndir.Subtract (Ndir.Multiplied (((3.0 * Dr * Dr)/R4) - (D2r/R2)));
     D2Ndir.Multiply (offsetValue / R);
     V2.Add (Vec2d(D2Ndir));
     // V1 = P' (U) :
     DNdir.Multiply(R);
     DNdir.Subtract (Ndir.Multiplied (Dr/R));
     DNdir.Multiply (offsetValue/R2);
     V1.Add (Vec2d(DNdir));
   }
   else {
     // Same computation as IICURV in EUCLID-IS because the stability is
     // better.
     // V3 = P"' (U) :
      D3Ndir.Multiply (offsetValue/R);
     D3Ndir.Subtract (D2Ndir.Multiplied (3.0 * offsetValue * Dr / R3));
     D3Ndir.Subtract (DNdir.Multiplied (
     ((3.0 * offsetValue) * ((D2r/R3) + (Dr*Dr)/R5))) );
     D3Ndir.Add (Ndir.Multiplied (
     (offsetValue * (6.0*Dr*Dr/R5 + 6.0*Dr*D2r/R5 - 15.0*Dr*Dr*Dr/R7 - D3r))
     ));
     V3.Add (Vec2d(D3Ndir));
     // V2 = P" (U) :
     D2Ndir.Multiply (offsetValue/R);
     D2Ndir.Subtract (DNdir.Multiplied (2.0 * offsetValue * Dr / R3));
     D2Ndir.Subtract (Ndir.Multiplied (
                      offsetValue * (((3.0 * Dr * Dr) / R5) - (D2r / R3))
                                      )
                     );
     V2.Add (Vec2d(D2Ndir));
     // V1 = P' (U) :
     DNdir.Multiply (offsetValue/R);
     DNdir.Subtract (Ndir.Multiplied (offsetValue*Dr/R3));        
     V1.Add (Vec2d(DNdir));
   }
   //P (U) :
   Ndir.Multiply (offsetValue/R);
   Ndir.Add (P.XY());
   P.SetXY (Ndir);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec2d Geom2d_OffsetCurve::DN (const Standard_Real U, 
                              const Standard_Integer N) const 
{
  Standard_RangeError_Raise_if (N < 1, "Geom2d_OffsetCurve::DN()");

  gp_Vec2d VN, VBidon;
  gp_Pnt2d PBidon;
  switch (N) {
  case 1: D1( U, PBidon, VN); break;
  case 2: D2( U, PBidon, VBidon, VN); break;
  case 3: D3( U, PBidon, VBidon, VBidon, VN); break;
  default:
    Standard_NotImplemented::Raise();
  }
  
  return VN;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::Value (const Standard_Real U, 
				Pnt2d& P, Pnt2d& Pbasis,
				Vec2d& V1basis ) const 
{

  basisCurve->D1 (U, Pbasis, V1basis);
  Standard_Integer Index = 2;
  while (V1basis.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
    V1basis = basisCurve->DN (U, Index);
    Index++;
  }
  Standard_Real A = V1basis.Y();
  Standard_Real B = - V1basis.X();
  Standard_Real R = Sqrt(A*A + B * B);
  if (R <= gp::Resolution())  Geom2d_UndefinedValue::Raise();
  A = A * offsetValue/R;
  B = B * offsetValue/R;
  P.SetCoord (A + Pbasis.X(), B + Pbasis.Y());
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D1 (const Standard_Real U, 
			     Pnt2d& P, Pnt2d& Pbasis,
			     Vec2d& V1, Vec2d& V1basis, 
			     Vec2d& V2basis ) const 
{
   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   basisCurve->D2 (U, Pbasis, V1basis, V2basis);
   V1 = V1basis;
   Vec2d V2 = V2basis;
   Standard_Integer Index = 2;
   while (V1.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
     V1 = basisCurve->DN (U, Index);
     Index++;
   }
   if (Index != 2) {
     V2 = basisCurve->DN (U, Index);
   }
   XY Ndir (V1.Y(), -V1.X());
   XY DNdir (V2.Y(), -V2.X());
   Standard_Real R2 = Ndir.SquareModulus();
   Standard_Real R = Sqrt (R2);
   Standard_Real R3 = R * R2;
   Standard_Real Dr = Ndir.Dot (DNdir);
   if (R3 <= gp::Resolution()) {
      //We try another computation but the stability is not very good.
      if (R2 <= gp::Resolution()) { Geom2d_UndefinedDerivative::Raise(); }
      DNdir.Multiply(R);
      DNdir.Subtract (Ndir.Multiplied (Dr/R));
      DNdir.Multiply (offsetValue / R2);
      V1.Add (Vec2d(DNdir));
   }
   else {
      // Same computation as IICURV in EUCLID-IS because the stability is
      // better
      DNdir.Multiply (offsetValue/R);
      DNdir.Subtract (Ndir.Multiplied (offsetValue*Dr/R3));        
      V1.Add (Vec2d(DNdir));
   }
   Ndir.Multiply (offsetValue/R);
   Ndir.Add (Pbasis.XY());
   P.SetXY (Ndir);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D2 (const Standard_Real U, 
			     Pnt2d& P, Pnt2d& Pbasis,
			     Vec2d& V1, Vec2d& V2, 
			     Vec2d& V1basis, Vec2d& V2basis,
			     Vec2d& V3basis ) const 
{
   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
   //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  basisCurve->D3 (U, Pbasis, V1basis, V2basis, V3basis);
  Standard_Integer Index = 2;
  V1 = V1basis;
  V2 = V2basis;
  Vec2d V3 = V3basis;
  while (V1.Magnitude() <= gp::Resolution() && Index <= MaxDegree) {
    V1 = basisCurve->DN (U, Index);
    Index++;
  }
  if (Index != 2) {
    V2 = basisCurve->DN (U, Index);
    V3 = basisCurve->DN (U, Index + 1);
  }
  XY Ndir (V1.Y(), -V1.X());
  XY DNdir (V2.Y(), -V2.X());
  XY D2Ndir (V3.Y(), -V3.X());
  Standard_Real R2  = Ndir.SquareModulus();
  Standard_Real R   = Sqrt (R2);
  Standard_Real R3  = R2 * R;
  Standard_Real R4  = R2 * R2;
  Standard_Real R5  = R3 * R2;
  Standard_Real Dr  = Ndir.Dot (DNdir);
  Standard_Real D2r = Ndir.Dot (D2Ndir) + DNdir.Dot (DNdir);
  if (R5 <= gp::Resolution()) {
     //We try another computation but the stability is not very good
     //dixit ISG.
     if (R4 <= gp::Resolution()) { Geom2d_UndefinedDerivative::Raise(); }
     // V2 = P" (U) :
     Standard_Real R4 = R2 * R2;
     D2Ndir.Subtract (DNdir.Multiplied (2.0 * Dr / R2));
     D2Ndir.Subtract (Ndir.Multiplied (((3.0 * Dr * Dr)/R4) - (D2r/R2)));
     D2Ndir.Multiply (offsetValue / R);
     V2.Add (Vec2d(D2Ndir));
     // V1 = P' (U) :
     DNdir.Multiply(R);
     DNdir.Subtract (Ndir.Multiplied (Dr/R));
     DNdir.Multiply (offsetValue/R2);
     V1.Add (Vec2d(DNdir));
  }
  else {
     // Same computation as IICURV in EUCLID-IS because the stability is
     // better.
     // V2 = P" (U) :
     D2Ndir.Multiply (offsetValue/R);
     D2Ndir.Subtract (DNdir.Multiplied (2.0 * offsetValue * Dr / R3));
     D2Ndir.Subtract (Ndir.Multiplied (
                      offsetValue * (((3.0 * Dr * Dr) / R5) - (D2r / R3))
                                      )
                     );
     V2.Add (Vec2d(D2Ndir));
     // V1 = P' (U) :
     DNdir.Multiply (offsetValue/R);
     DNdir.Subtract (Ndir.Multiplied (offsetValue*Dr/R3));        
     V1.Add (Vec2d(DNdir));
  }
  //P (U) :
  Ndir.Multiply (offsetValue/R);
  Ndir.Add (Pbasis.XY());
  P.SetXY (Ndir);
}

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_OffsetCurve::FirstParameter () const 
{
  return basisCurve->FirstParameter();
}

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_OffsetCurve::LastParameter () const 
{
  return basisCurve->LastParameter();
}


//=======================================================================
//function : Offset
//purpose  : 
//=======================================================================

Standard_Real Geom2d_OffsetCurve::Offset () const { return offsetValue; }

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_OffsetCurve::IsClosed () const 
{ 
  gp_Pnt2d PF, PL;
  D0(FirstParameter(),PF);
  D0(LastParameter(),PL);
  return ( PF.Distance(PL) <= gp::Resolution());
}

//=======================================================================
//function : IsCN
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_OffsetCurve::IsCN (const Standard_Integer N) const 
{
  Standard_RangeError_Raise_if (N < 0, " " );
  return basisCurve->IsCN (N + 1);
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_OffsetCurve::IsPeriodic () const 
{ 
  return basisCurve->IsPeriodic();
}

//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real Geom2d_OffsetCurve::Period() const
{
  return basisCurve->Period();
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::Transform (const Trsf2d& T) 
{
  basisCurve->Transform (T);
  offsetValue *= Abs(T.ScaleFactor());
}


//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_OffsetCurve::TransformedParameter(const Standard_Real U,
							const gp_Trsf2d& T) const
{
  return basisCurve->TransformedParameter(U,T);
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

Standard_Real Geom2d_OffsetCurve::ParametricTransformation(const gp_Trsf2d& T) const
{
  return basisCurve->ParametricTransformation(T);
}
