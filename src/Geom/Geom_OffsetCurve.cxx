// Created on: 1991-06-25
// Created by: JCV
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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




//ordre de derivation maximum pour la recherche de la premiere 
//derivee non nulle
static const int maxDerivOrder = 3;
static const Standard_Real MinStep   = 1e-7;




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
 : direction(V), offsetValue(Offset)
{
  if (C->DynamicType() == STANDARD_TYPE(Geom_OffsetCurve)) {
    Handle(OffsetCurve) OC = Handle(OffsetCurve)::DownCast(C);
    SetBasisCurve (OC->BasisCurve());

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
  }
  else {
    SetBasisCurve(C);
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

void Geom_OffsetCurve::SetBasisCurve (const Handle(Curve)& C)
{
  Handle(Curve) aBasisCurve = Handle(Curve)::DownCast(C->Copy());

  // Basis curve must be at least C1
  if (aBasisCurve->Continuity() == GeomAbs_C0)
  {
    // For B-splines it is sometimes possible to increase continuity by removing 
    // unnecessarily duplicated knots
    if (aBasisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
    {
      Handle(Geom_BSplineCurve) aBCurve = Handle(Geom_BSplineCurve)::DownCast(aBasisCurve);
      Standard_Integer degree = aBCurve->Degree();
      Standard_Real Toler = Precision::Confusion();
      Standard_Integer start = aBCurve->IsPeriodic() ? 1 :  aBCurve->FirstUKnotIndex(),
                       finish = aBCurve->IsPeriodic() ? aBCurve->NbKnots() :  aBCurve->LastUKnotIndex();
      for (Standard_Integer i = start; i <= finish; i++)
      {
        Standard_Integer mult = aBCurve->Multiplicity(i);
        if ( mult == degree )
          aBCurve->RemoveKnot(i,degree - 1, Toler);
      }
    }

    // Raise exception if still C0
    if (aBasisCurve->Continuity() == GeomAbs_C0)
      Standard_ConstructionError::Raise("Offset on C0 curve");
  }

  basisCurve = aBasisCurve;
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

void Geom_OffsetCurve::D3 (const Standard_Real theU, Pnt& P, Vec& theV1, Vec& V2, Vec& V3) 
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

  const Standard_Real aTol = gp::Resolution();

  Standard_Boolean IsDirectionChange = Standard_False;

  basisCurve->D3 (theU, P, theV1, V2, V3);
  Vec V4 = basisCurve->DN (theU, 4);
  if(theV1.Magnitude() <= aTol)
    {
    const Standard_Real anUinfium   = basisCurve->FirstParameter();
    const Standard_Real anUsupremum = basisCurve->LastParameter();

    const Standard_Real DivisionFactor = 1.e-3;
    Standard_Real du;
    if((anUsupremum >= RealLast()) || (anUinfium <= RealFirst())) 
      du = 0.0;
    else
      du = anUsupremum-anUinfium;
      
    const Standard_Real aDelta = Max(du*DivisionFactor,MinStep);
//Derivative is approximated by Taylor-series
    
    Standard_Integer anIndex = 1; //Derivative order
    Vec V;
    
    do
      {
      V =  basisCurve->DN(theU,++anIndex);
      }
    while((V.Magnitude() <= aTol) && anIndex < maxDerivOrder);
    
    Standard_Real u;
    
    if(theU-anUinfium < aDelta)
      u = theU+aDelta;
    else
      u = theU-aDelta;
    
    Pnt P1, P2;
    basisCurve->D0(Min(theU, u),P1);
    basisCurve->D0(Max(theU, u),P2);
    
    Vec V1(P1,P2);
    Standard_Real aDirFactor = V.Dot(V1);
    
    if(aDirFactor < 0.0)
      {
      theV1 = -V;
      V2 = -basisCurve->DN (theU, anIndex + 1);
      V3 = -basisCurve->DN (theU, anIndex + 2);
      V4 = -basisCurve->DN (theU, anIndex + 3);

      IsDirectionChange = Standard_True;
      }
    else
      {
      theV1 = V;
      V2 = basisCurve->DN (theU, anIndex + 1);
      V3 = basisCurve->DN (theU, anIndex + 2);
      V4 = basisCurve->DN (theU, anIndex + 3);
      }
    }//if(V1.Magnitude() <= aTol)


  XYZ OffsetDir = direction.XYZ();
  XYZ Ndir      = (theV1.XYZ()).Crossed (OffsetDir);
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

    if(IsDirectionChange)
      V3=-V3;

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
    theV1.Add (Vec(DNdir));
  }
  else {
    // V3 = P"' (U) :
    D3Ndir.Divide (R);
    D3Ndir.Subtract (D2Ndir.Multiplied (3.0 * Dr / R3));
    D3Ndir.Subtract (DNdir.Multiplied ((3.0 * ((D2r/R3) + (Dr*Dr)/R5))));
    D3Ndir.Add (Ndir.Multiplied (6.0*Dr*Dr/R5 + 6.0*Dr*D2r/R5 - 
                15.0*Dr*Dr*Dr/R7 - D3r));
    D3Ndir.Multiply (offsetValue);
    
    if(IsDirectionChange)
      V3=-V3;

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
    theV1.Add (Vec(DNdir));
  }
  //P (U) :
  D0(theU,P);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_OffsetCurve::DN (const Standard_Real U, const Standard_Integer N) const 
  {
  Standard_RangeError_Raise_if (N < 1, "Exception: "
                              "Geom_OffsetCurve::DN(...). N<1.");

  gp_Vec VN, Vtemp;
  gp_Pnt Ptemp;
  switch (N)
    {
    case 1:
      D1( U, Ptemp, VN);
      break;
    case 2:
      D2( U, Ptemp, Vtemp, VN);
      break;
    case 3:
      D3( U, Ptemp, Vtemp, Vtemp, VN);
      break;
    default:
      Standard_NotImplemented::Raise("Exception: "
        "Derivative order is greater than 3. Cannot compute of derivative.");
  }
  
  return VN;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void  Geom_OffsetCurve::D0(const Standard_Real theU, gp_Pnt& theP,
                           gp_Pnt& thePbasis, gp_Vec& theV1basis)const 
  {
  const Standard_Real aTol = gp::Resolution();

  basisCurve->D1 (theU, thePbasis, theV1basis);
  Standard_Real Ndu = theV1basis.Magnitude();
  
  if(Ndu <= aTol)
    {
    const Standard_Real anUinfium   = basisCurve->FirstParameter();
    const Standard_Real anUsupremum = basisCurve->LastParameter();

    const Standard_Real DivisionFactor = 1.e-3;
    Standard_Real du;
    if((anUsupremum >= RealLast()) || (anUinfium <= RealFirst())) 
      du = 0.0;
    else
      du = anUsupremum-anUinfium;
      
    const Standard_Real aDelta = Max(du*DivisionFactor,MinStep);
//Derivative is approximated by Taylor-series
    
    Standard_Integer anIndex = 1; //Derivative order
    gp_Vec V;
    
    do
      {
      V =  basisCurve->DN(theU,++anIndex);
      Ndu = V.Magnitude();
      }
    while((Ndu <= aTol) && anIndex < maxDerivOrder);
    
    Standard_Real u;
    
    if(theU-anUinfium < aDelta)
      u = theU+aDelta;
    else
      u = theU-aDelta;
    
    gp_Pnt P1, P2;
    basisCurve->D0(Min(theU, u),P1);
    basisCurve->D0(Max(theU, u),P2);
    
    gp_Vec V1(P1,P2);
    Standard_Real aDirFactor = V.Dot(V1);
    
    if(aDirFactor < 0.0)
      theV1basis = -V;
    else
      theV1basis = V;

    Ndu = theV1basis.Magnitude();
    }//if(Ndu <= aTol)
  
  XYZ Ndir = (theV1basis.XYZ()).Crossed (direction.XYZ());
  Standard_Real R = Ndir.Modulus();
  if (R <= gp::Resolution())
    Geom_UndefinedValue::Raise("Exception: Undefined normal vector "
          "because tangent vector has zero-magnitude!");

  Ndir.Multiply (offsetValue/R);
  Ndir.Add (thePbasis.XYZ());
  theP.SetXYZ(Ndir);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::D1 ( const Standard_Real theU, 
                            Pnt& P , Pnt& PBasis ,
                            Vec& theV1, Vec& V1basis, Vec& V2basis) const {

   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ V|| and Ndir = P' ^ direction (local normal direction)

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  const Standard_Real aTol = gp::Resolution();

  basisCurve->D2 (theU, PBasis, V1basis, V2basis);
  theV1 = V1basis;
  Vec V2 = V2basis;

  if(theV1.Magnitude() <= aTol)
    {
    const Standard_Real anUinfium   = basisCurve->FirstParameter();
    const Standard_Real anUsupremum = basisCurve->LastParameter();

    const Standard_Real DivisionFactor = 1.e-3;
    Standard_Real du;
    if((anUsupremum >= RealLast()) || (anUinfium <= RealFirst())) 
      du = 0.0;
    else
      du = anUsupremum-anUinfium;
      
    const Standard_Real aDelta = Max(du*DivisionFactor,MinStep);
//Derivative is approximated by Taylor-series
    
    Standard_Integer anIndex = 1; //Derivative order
    Vec V;
    
    do
      {
      V =  basisCurve->DN(theU,++anIndex);
      }
    while((V.Magnitude() <= aTol) && anIndex < maxDerivOrder);
    
    Standard_Real u;
    
    if(theU-anUinfium < aDelta)
      u = theU+aDelta;
    else
      u = theU-aDelta;
    
    Pnt P1, P2;
    basisCurve->D0(Min(theU, u),P1);
    basisCurve->D0(Max(theU, u),P2);
    
    Vec V1(P1,P2);
    Standard_Real aDirFactor = V.Dot(V1);
    
    if(aDirFactor < 0.0)
      {
      theV1 = -V;
      V2 = - basisCurve->DN (theU, anIndex+1);
      }
    else
      {
      theV1 = V;
      V2 = basisCurve->DN (theU, anIndex+1);
      }
    
    V2basis = V2;
    V1basis = theV1;
    }//if(theV1.Magnitude() <= aTol)

  XYZ OffsetDir = direction.XYZ();
  XYZ Ndir  = (theV1.XYZ()).Crossed (OffsetDir);
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
    theV1.Add (Vec(DNdir));
  }
  else {
    // Same computation as IICURV in EUCLID-IS because the stability is
    // better
    DNdir.Multiply (offsetValue/R);
    DNdir.Subtract (Ndir.Multiplied (offsetValue * Dr/R3));        
    theV1.Add (Vec(DNdir));
  }
  D0(theU,P);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::D2 (const Standard_Real theU, 
                           Pnt& P      , Pnt& PBasis ,
                           Vec& theV1     , Vec& V2     , 
                           Vec& V1basis, Vec& V2basis, Vec& V3basis) const {

   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ V|| and Ndir = P' ^ direction (local normal direction)

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
   //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  const Standard_Real aTol = gp::Resolution();

  Standard_Boolean IsDirectionChange = Standard_False;

  basisCurve->D3 (theU, PBasis, V1basis, V2basis, V3basis);

  theV1  = V1basis;
  V2     = V2basis;
  Vec V3 = V3basis;
  
  if(theV1.Magnitude() <= aTol)
    {
    const Standard_Real anUinfium   = basisCurve->FirstParameter();
    const Standard_Real anUsupremum = basisCurve->LastParameter();

    const Standard_Real DivisionFactor = 1.e-3;
    Standard_Real du;
    if((anUsupremum >= RealLast()) || (anUinfium <= RealFirst())) 
      du = 0.0;
    else
      du = anUsupremum-anUinfium;
      
    const Standard_Real aDelta = Max(du*DivisionFactor,MinStep);
//Derivative is approximated by Taylor-series
    
    Standard_Integer anIndex = 1; //Derivative order
    Vec V;
    
    do
      {
      V =  basisCurve->DN(theU,++anIndex);
      }
    while((V.Magnitude() <= aTol) && anIndex < maxDerivOrder);
    
    Standard_Real u;
    
    if(theU-anUinfium < aDelta)
      u = theU+aDelta;
    else
      u = theU-aDelta;
    
    Pnt P1, P2;
    basisCurve->D0(Min(theU, u),P1);
    basisCurve->D0(Max(theU, u),P2);
    
    Vec V1(P1,P2);
    Standard_Real aDirFactor = V.Dot(V1);
    
    if(aDirFactor < 0.0)
      {
      theV1 = -V;
      V2 = -basisCurve->DN (theU, anIndex+1);
      V3 = -basisCurve->DN (theU, anIndex + 2);

      IsDirectionChange = Standard_True;
      }
    else
      {
      theV1 = V;
      V2 = basisCurve->DN (theU, anIndex+1);
      V3 = basisCurve->DN (theU, anIndex + 2);
      }
    
    V2basis = V2;
    V1basis = theV1;
    }//if(V1.Magnitude() <= aTol)
  
  XYZ OffsetDir = direction.XYZ();
  XYZ Ndir   = (theV1.XYZ()).Crossed (OffsetDir);
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
    
    if(IsDirectionChange)
      V2=-V2;

    V2.Add (Vec(D2Ndir));
        
    // V1 = P' (U) :
    DNdir.Multiply(R);
    DNdir.Subtract (Ndir.Multiplied (Dr/R));
    DNdir.Multiply (offsetValue/R2);
    theV1.Add (Vec(DNdir));
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
    
    if(IsDirectionChange)
      V2=-V2;

    V2.Add (Vec(D2Ndir));
    
    // V1 = P' (U) :
    DNdir.Multiply (offsetValue/R);
    DNdir.Subtract (Ndir.Multiplied (offsetValue*Dr/R3));        
    theV1.Add (Vec(DNdir));
  }
  //P (U) :
  D0(theU,P);
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

void Geom_OffsetCurve::Value (const Standard_Real theU, Pnt& theP, 
                              Pnt& thePbasis,  Vec& theV1basis) const 
  {
  if (basisCurve->Continuity() == GeomAbs_C0)
    Geom_UndefinedValue::Raise("Exception: Basis curve is C0 continuity!");

  basisCurve->D1(theU, thePbasis, theV1basis);
  D0(theU,theP);
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
