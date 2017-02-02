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

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_OffsetCurve,Geom2d_Curve)

static const Standard_Real MyAngularToleranceForG1 = Precision::Angular();


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom2d_Geometry) Geom2d_OffsetCurve::Copy () const 
{
  Handle(Geom2d_OffsetCurve) C;
  C = new Geom2d_OffsetCurve (basisCurve, offsetValue);
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
      throw Standard_ConstructionError("Offset on C0 curve");
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

  myEvaluator = new Geom2dEvaluator_OffsetCurve(basisCurve, offsetValue);
}

//=======================================================================
//function : SetOffsetValue
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::SetOffsetValue (const Standard_Real D)
{
  offsetValue = D;
  myEvaluator->SetOffsetValue(offsetValue);
}

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
                                   gp_Pnt2d&     theP) const
{
  myEvaluator->D0(theU, theP);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================
void Geom2d_OffsetCurve::D1 (const Standard_Real theU, gp_Pnt2d& theP, gp_Vec2d& theV1) const
{
  myEvaluator->D1(theU, theP, theV1);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D2 (const Standard_Real theU, 
                                   gp_Pnt2d& theP, 
                                   gp_Vec2d& theV1, gp_Vec2d& theV2) const 
{
  myEvaluator->D2(theU, theP, theV1, theV2);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom2d_OffsetCurve::D3 (const Standard_Real theU, 
                                   gp_Pnt2d& theP, 
                                   gp_Vec2d& theV1, gp_Vec2d& theV2, gp_Vec2d& theV3) const
{
  myEvaluator->D3(theU, theP, theV1, theV2, theV3);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec2d Geom2d_OffsetCurve::DN (const Standard_Real U, 
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
    throw Standard_NotImplemented("Exception: Derivative order is greater than 3. "
      "Cannot compute of derivative.");
  }
  
  return VN;
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

Standard_Real Geom2d_OffsetCurve::Offset () const
{ return offsetValue; }

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

void Geom2d_OffsetCurve::Transform (const gp_Trsf2d& T) 
{
  basisCurve->Transform (T);
  offsetValue *= Abs(T.ScaleFactor());

  myEvaluator->SetOffsetValue(offsetValue);
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
