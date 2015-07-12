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

#include <CSLib_Offset.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_UndefinedDerivative.hxx>
#include <Geom2d_UndefinedValue.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

typedef Geom2d_OffsetCurve         OffsetCurve;
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

static gp_Vec2d dummyDerivative; // used as empty value for unused derivatives in AdjustDerivative

// Recalculate derivatives in the singular point
// Returns true if the direction of derivatives is changed
static Standard_Boolean AdjustDerivative(const Handle(Geom2d_Curve)& theCurve, Standard_Integer theMaxDerivative,
                                         Standard_Real theU, gp_Vec2d& theD1, gp_Vec2d& theD2 = dummyDerivative,
                                         gp_Vec2d& theD3 = dummyDerivative, gp_Vec2d& theD4 = dummyDerivative);

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom2d_Geometry) Geom2d_OffsetCurve::Copy () const 
{
  Handle(Geom2d_OffsetCurve) C;
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

void Geom2d_OffsetCurve::SetBasisCurve (const Handle(Geom2d_Curve)& C,
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

Handle(Geom2d_Curve) Geom2d_OffsetCurve::BasisCurve () const 
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

void Geom2d_OffsetCurve::D0 (const Standard_Real theU,
                                   Pnt2d&        theP) const
  {
  Vec2d vD1;
  basisCurve->D1 (theU, theP, vD1);

  Standard_Boolean IsDirectionChange = Standard_False;
  if(vD1.SquareMagnitude() <= gp::Resolution())
    IsDirectionChange = AdjustDerivative(basisCurve, 1, theU, vD1);

  CSLib_Offset::D0(theP, vD1, offsetValue, IsDirectionChange, theP);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================
void Geom2d_OffsetCurve::D1 (const Standard_Real theU, Pnt2d& theP, Vec2d& theV1) const
{
   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  Vec2d V2;
  basisCurve->D2 (theU, theP, theV1, V2);

  Standard_Boolean IsDirectionChange = Standard_False;
  if(theV1.SquareMagnitude() <= gp::Resolution())
    IsDirectionChange = AdjustDerivative(basisCurve, 2, theU, theV1, V2);

  CSLib_Offset::D1(theP, theV1, V2, offsetValue, IsDirectionChange, theP, theV1);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D2 (const Standard_Real theU, 
                                   Pnt2d& theP, 
                                   Vec2d& theV1, Vec2d& theV2) const 
{
   // P(u) = p(u) + Offset * Ndir / R
   // with R = || p' ^ Z|| and Ndir = P' ^ Z

   // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

   // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
   //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  Vec2d V3;
  basisCurve->D3 (theU, theP, theV1, theV2, V3);

  Standard_Boolean IsDirectionChange = Standard_False;
  if(theV1.SquareMagnitude() <= gp::Resolution())
    IsDirectionChange = AdjustDerivative(basisCurve, 3, theU, theV1, theV2, V3);

  CSLib_Offset::D2(theP, theV1, theV2, V3, offsetValue, IsDirectionChange, theP, theV1, theV2);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D3 (const Standard_Real theU, 
                                   Pnt2d& theP, 
                                   Vec2d& theV1, Vec2d& theV2, Vec2d& theV3) const
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

  basisCurve->D3 (theU, theP, theV1, theV2, theV3);
  Vec2d V4 = basisCurve->DN (theU, 4);

  Standard_Boolean IsDirectionChange = Standard_False;
  if(theV1.SquareMagnitude() <= gp::Resolution())
    IsDirectionChange = AdjustDerivative(basisCurve, 4, theU, theV1, theV2, theV3, V4);

  CSLib_Offset::D3(theP, theV1, theV2, theV3, V4, offsetValue, IsDirectionChange,
                   theP, theV1, theV2, theV3);
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

   CSLib_Offset::D1(P, V1, V2, offsetValue, Standard_False, P, V1);
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

  CSLib_Offset::D2(P, V1, V2, V3, offsetValue, Standard_False, P, V1, V2);
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


// ============= Auxiliary functions ===================
Standard_Boolean AdjustDerivative(const Handle(Geom2d_Curve)& theCurve, Standard_Integer theMaxDerivative,
                                  Standard_Real theU, gp_Vec2d& theD1, gp_Vec2d& theD2,
                                  gp_Vec2d& theD3, gp_Vec2d& theD4)
{
  static const Standard_Real aTol = gp::Resolution();

  Standard_Boolean IsDirectionChange = Standard_False;
  const Standard_Real anUinfium   = theCurve->FirstParameter();
  const Standard_Real anUsupremum = theCurve->LastParameter();

  const Standard_Real DivisionFactor = 1.e-3;
  Standard_Real du;
  if((anUsupremum >= RealLast()) || (anUinfium <= RealFirst())) 
    du = 0.0;
  else
    du = anUsupremum - anUinfium;

  const Standard_Real aDelta = Max(du * DivisionFactor, MinStep);

  //Derivative is approximated by Taylor-series
  Standard_Integer anIndex = 1; //Derivative order
  Vec2d V;

  do
  {
    V =  theCurve->DN(theU, ++anIndex);
  }
  while((V.SquareMagnitude() <= aTol) && anIndex < maxDerivOrder);

  Standard_Real u;

  if(theU-anUinfium < aDelta)
    u = theU+aDelta;
  else
    u = theU-aDelta;

  Pnt2d P1, P2;
  theCurve->D0(Min(theU, u),P1);
  theCurve->D0(Max(theU, u),P2);

  Vec2d V1(P1,P2);
  IsDirectionChange = V.Dot(V1) < 0.0;
  Standard_Real aSign = IsDirectionChange ? -1.0 : 1.0;

  theD1 = V * aSign;
  gp_Vec2d* aDeriv[3] = {&theD2, &theD3, &theD4};
  for (Standard_Integer i = 1; i < theMaxDerivative; i++)
    *(aDeriv[i-1]) = theCurve->DN(theU, anIndex + i) * aSign;

  return IsDirectionChange;
}
