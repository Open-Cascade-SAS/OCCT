// Created on: 1991-06-25
// Created by: JCV
// Copyright (c) 1991-1999 Matra Datavision
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
#include <Precision.hxx>

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


//ordre de derivation maximum pour la recherche de la premiere 
//derivee non nulle
static const int maxDerivOrder = 3;
static const Standard_Real MinStep   = 1e-7;
static const Standard_Real MyAngularToleranceForG1 = Precision::Angular();

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
//purpose  : Basis curve cannot be an Offset curve or trimmed from
//            offset curve.
//=======================================================================

Geom2d_OffsetCurve::Geom2d_OffsetCurve (const Handle(Geom2d_Curve)& theCurve,
                                        const Standard_Real theOffset,
                                        const Standard_Boolean isTheNotCheckC0)  
: offsetValue (theOffset) 
{
  SetBasisCurve (theCurve, isTheNotCheckC0);
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

void Geom2d_OffsetCurve::SetBasisCurve (const Handle(Curve)& C,
                                        const Standard_Boolean isNotCheckC0) 
{
  const Standard_Real aUf = C->FirstParameter(),
                      aUl = C->LastParameter();
  Handle(Geom2d_Curve) aCheckingCurve = C;
  Standard_Boolean isTrimmed = Standard_False;

  while(aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)) ||
        aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
  {
    if (aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      Handle(Geom2d_TrimmedCurve) aTrimC = 
                Handle(Geom2d_TrimmedCurve)::DownCast(aCheckingCurve);
      aCheckingCurve = aTrimC->BasisCurve();
      isTrimmed = Standard_True;
    }

    if (aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
    {
      Handle(Geom2d_OffsetCurve) aOC = 
                Handle(Geom2d_OffsetCurve)::DownCast(aCheckingCurve);
      aCheckingCurve = aOC->BasisCurve();
      offsetValue += aOC->Offset();
    }
  }

  myBasisCurveContinuity = aCheckingCurve->Continuity();

  Standard_Boolean isC0 = !isNotCheckC0 &&
                          (myBasisCurveContinuity == GeomAbs_C0);

  // Basis curve must be at least C1
  if (isC0 && aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    Handle(Geom2d_BSplineCurve) aBC = Handle(Geom2d_BSplineCurve)::DownCast(aCheckingCurve);
    if(aBC->IsG1(aUf, aUl, MyAngularToleranceForG1))
    {
      //Checking if basis curve has more smooth (C1, G2 and above) is not done.
      //It can be done in case of need.
      myBasisCurveContinuity = GeomAbs_G1;
      isC0 = Standard_False;
    }

    // Raise exception if still C0
    if (isC0)
      Standard_ConstructionError::Raise("Offset on C0 curve");
  }
  //
  if(isTrimmed)
  {
    basisCurve = new Geom2d_TrimmedCurve(aCheckingCurve, aUf, aUl);
  } 
  else
  {
    basisCurve = aCheckingCurve;
  }

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
  switch (myBasisCurveContinuity) {
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

void Geom2d_OffsetCurve::D0 (const Standard_Real   theU,
			           Pnt2d& theP ) const 
  {
  const Standard_Real aTol = gp::Resolution();

  Vec2d vD1;

  basisCurve->D1 (theU, theP, vD1);
  Standard_Real Ndu = vD1.Magnitude();

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
    Vec2d V;
    
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
    
    Pnt2d P1, P2;
    basisCurve->D0(Min(theU, u),P1);
    basisCurve->D0(Max(theU, u),P2);
    
    Vec2d V1(P1,P2);
    Standard_Real aDirFactor = V.Dot(V1);
    
    if(aDirFactor < 0.0)
      vD1 = -V;
    else
      vD1 = V;

    Ndu = vD1.Magnitude();
    }//if(Ndu <= aTol)

  if (Ndu <= aTol)
    Geom2d_UndefinedValue::Raise("Exception: Undefined normal vector "
          "because tangent vector has zero-magnitude!");
  
  Standard_Real A = vD1.Y();
  Standard_Real B = - vD1.X();
  A = A * offsetValue/Ndu;
  B = B * offsetValue/Ndu;
  theP.SetCoord(theP.X() + A, theP.Y() + B);
  }

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================
void Geom2d_OffsetCurve::D1 (const Standard_Real theU, Pnt2d& P, Vec2d& theV1) const
  {
   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  const Standard_Real aTol = gp::Resolution();

  Vec2d V2;
  basisCurve->D2 (theU, P, theV1, V2);
  
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
    Vec2d V;
    
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
    
    Pnt2d P1, P2;
    basisCurve->D0(Min(theU, u),P1);
    basisCurve->D0(Max(theU, u),P2);
    
    Vec2d V1(P1,P2);
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
    }//if(theV1.Magnitude() <= aTol)

  XY Ndir  (theV1.Y(), -theV1.X());
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
    theV1.Add (Vec2d(DNdir));
  }
  else {
    // Same computation as IICURV in EUCLID-IS because the stability is
    // better
    DNdir.Multiply (offsetValue/R);
    DNdir.Subtract (Ndir.Multiplied (offsetValue*Dr/R3));        
    theV1.Add (Vec2d(DNdir));
  }

  D0(theU, P);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D2 (const Standard_Real theU, 
			           Pnt2d& P, 
			           Vec2d& theV1, Vec2d& V2) const 
{
   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
   //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  Vec2d V3;
  basisCurve->D3 (theU, P, theV1, V2, V3);

  const Standard_Real aTol = gp::Resolution();

  Standard_Boolean IsDirectionChange = Standard_False;

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
    Vec2d V;
    
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
    
    Pnt2d P1, P2;
    basisCurve->D0(Min(theU, u),P1);
    basisCurve->D0(Max(theU, u),P2);
    
    Vec2d V1(P1,P2);
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
    }//if(V1.Magnitude() <= aTol)

  XY Ndir (theV1.Y(), -theV1.X());
  XY DNdir (V2.Y(), -V2.X());
  XY D2Ndir (V3.Y(), -V3.X());
  Standard_Real R2  = Ndir.SquareModulus();
  Standard_Real R   = Sqrt (R2);
  Standard_Real R3  = R2 * R;
  Standard_Real R4  = R2 * R2;
  Standard_Real R5  = R3 * R2;
  Standard_Real Dr  = Ndir.Dot (DNdir);
  Standard_Real D2r = Ndir.Dot (D2Ndir) + DNdir.Dot (DNdir);
  if (R5 <= gp::Resolution())
    {
    //We try another computation but the stability is not very good
    //dixit ISG.
    if (R4 <= gp::Resolution())
      {
      Geom2d_UndefinedDerivative::Raise();
      }
    // V2 = P" (U) :
    Standard_Real R4 = R2 * R2;
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * Dr / R2));
    D2Ndir.Add (Ndir.Multiplied (((3.0 * Dr * Dr)/R4) - (D2r/R2)));
    D2Ndir.Multiply (offsetValue / R);

    if(IsDirectionChange)
      V2=-V2;

    V2.Add (Vec2d(D2Ndir));

    // V1 = P' (U) :
    DNdir.Multiply(R);
    DNdir.Subtract (Ndir.Multiplied (Dr/R));
    DNdir.Multiply (offsetValue/R2);
    theV1.Add (Vec2d(DNdir));
    }
  else
    {
    // Same computation as IICURV in EUCLID-IS because the stability is
    // better.
    // V2 = P" (U) :
    D2Ndir.Multiply (offsetValue/R);
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * offsetValue * Dr / R3));
    D2Ndir.Add (Ndir.Multiplied 
		     (offsetValue * (((3.0 * Dr * Dr) / R5) - (D2r / R3))));

    if(IsDirectionChange)
      V2=-V2;

    V2.Add (Vec2d(D2Ndir));

    // V1 = P' (U) 
    DNdir.Multiply (offsetValue/R);
    DNdir.Subtract (Ndir.Multiplied (offsetValue*Dr/R3));        
    theV1.Add (Vec2d(DNdir));
    }

  //P (U) :
  D0(theU, P);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D3 (const Standard_Real theU, 
                                   Pnt2d& P, 
                                   Vec2d& theV1, Vec2d& V2, Vec2d& V3) const {


   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
   //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

   //P"'(u) = p"'(u) + (Offset / R) * (D3Ndir - (3.0 * Dr/R**2 ) * D2Ndir -
   //         (3.0 * D2r / R2) * DNdir) + (3.0 * Dr * Dr / R4) * DNdir -
   //         (D3r/R2) * Ndir + (6.0 * Dr * Dr / R4) * Ndir +
   //         (6.0 * Dr * D2r / R4) * Ndir - (15.0 * Dr* Dr* Dr /R6) * Ndir

  const Standard_Real aTol = gp::Resolution();

  Standard_Boolean IsDirectionChange = Standard_False;

  basisCurve->D3 (theU, P, theV1, V2, V3);
  Vec2d V4 = basisCurve->DN (theU, 4);

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
    Vec2d V;
    
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
    
    Pnt2d P1, P2;
    basisCurve->D0(Min(theU, u),P1);
    basisCurve->D0(Max(theU, u),P2);
    
    Vec2d V1(P1,P2);
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

  XY Ndir   (theV1.Y(), -theV1.X());
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

  if (R7 <= gp::Resolution()) 
    {
    //We try another computation but the stability is not very good
    //dixit ISG.

    if (R6 <= gp::Resolution())
      Geom2d_UndefinedDerivative::Raise();

    // V3 = P"' (U) :
    D3Ndir.Subtract (D2Ndir.Multiplied (3.0 * offsetValue * Dr / R2));
    D3Ndir.Subtract (
      (DNdir.Multiplied ((3.0 * offsetValue) * ((D2r/R2) + (Dr*Dr)/R4))));
    D3Ndir.Add (Ndir.Multiplied (
      (offsetValue * (6.0*Dr*Dr/R4 + 6.0*Dr*D2r/R4 - 15.0*Dr*Dr*Dr/R6 - D3r))));
    D3Ndir.Multiply (offsetValue/R);

    if(IsDirectionChange)
      V3=-V3;

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
    theV1.Add (Vec2d(DNdir));
    }
  else
    {
    // Same computation as IICURV in EUCLID-IS because the stability is
    // better.
    // V3 = P"' (U) :
    D3Ndir.Multiply (offsetValue/R);
    D3Ndir.Subtract (D2Ndir.Multiplied (3.0 * offsetValue * Dr / R3));
    D3Ndir.Subtract (DNdir.Multiplied (
      ((3.0 * offsetValue) * ((D2r/R3) + (Dr*Dr)/R5))) );
    D3Ndir.Add (Ndir.Multiplied (
      (offsetValue * (6.0*Dr*Dr/R5 + 6.0*Dr*D2r/R5 - 15.0*Dr*Dr*Dr/R7 - D3r))));

    if(IsDirectionChange)
      V3=-V3;

    V3.Add (Vec2d(D3Ndir));

    // V2 = P" (U) :
    D2Ndir.Multiply (offsetValue/R);
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * offsetValue * Dr / R3));
    D2Ndir.Subtract (Ndir.Multiplied (
      offsetValue * (((3.0 * Dr * Dr) / R5) - (D2r / R3))));
    V2.Add (Vec2d(D2Ndir));
    // V1 = P' (U) :
    DNdir.Multiply (offsetValue/R);
    DNdir.Subtract (Ndir.Multiplied (offsetValue*Dr/R3));        
    theV1.Add (Vec2d(DNdir));
    }
  //P (U) :
  D0(theU, P);
  }

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec2d Geom2d_OffsetCurve::DN (const Standard_Real U, 
                              const Standard_Integer N) const 
{
Standard_RangeError_Raise_if (N < 1, "Exception: Geom2d_OffsetCurve::DN(). N<1.");

  gp_Vec2d VN, VBidon;
  gp_Pnt2d PBidon;
  switch (N) {
  case 1: D1( U, PBidon, VN); break;
  case 2: D2( U, PBidon, VBidon, VN); break;
  case 3: D3( U, PBidon, VBidon, VBidon, VN); break;
  default:
    Standard_NotImplemented::Raise("Exception: Derivative order is greater than 3. "
      "Cannot compute of derivative.");
  }
  
  return VN;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::Value (const Standard_Real theU, 
                                Pnt2d& theP, Pnt2d& thePbasis,
                                Vec2d& theV1basis ) const 
  {
  basisCurve->D1(theU, thePbasis, theV1basis);
  D0(theU,theP);
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
   while (V1.Magnitude() <= gp::Resolution() && Index <= maxDerivOrder) {
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
  while (V1.Magnitude() <= gp::Resolution() && Index <= maxDerivOrder) {
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

//=======================================================================
//function : GetBasisCurveContinuity
//purpose  : 
//=======================================================================
GeomAbs_Shape Geom2d_OffsetCurve::GetBasisCurveContinuity() const
{
  return myBasisCurveContinuity;
}
