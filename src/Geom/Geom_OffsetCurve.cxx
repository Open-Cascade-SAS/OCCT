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

// 24-Aug-95 : xab removed C1 and C2 test : appeller  D1 et D2 
//             avec discernement !
// 19-09-97  : JPI correction derivee seconde

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <Geom_UndefinedValue.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_OffsetCurve,Geom_Curve)

static const Standard_Real MyAngularToleranceForG1 = Precision::Angular();


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_OffsetCurve::Copy () const {

 Handle(Geom_OffsetCurve) C;
 C = new Geom_OffsetCurve (basisCurve, offsetValue, direction);
 return C;
}



//=======================================================================
//function : Geom_OffsetCurve
//purpose  : Basis curve cannot be an Offset curve or trimmed from
//            offset curve.
//=======================================================================

Geom_OffsetCurve::Geom_OffsetCurve (const Handle(Geom_Curve)& theCurve,
                                    const Standard_Real       theOffset,
                                    const gp_Dir&             theDir,
                                    const Standard_Boolean    isTheNotCheckC0)
 : direction(theDir), offsetValue(theOffset)
{
  SetBasisCurve (theCurve, isTheNotCheckC0);
}


//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::Reverse ()
{ 
  basisCurve->Reverse();
  offsetValue = -offsetValue;
  myEvaluator->SetOffsetValue(offsetValue);
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

void Geom_OffsetCurve::SetDirection (const gp_Dir& V)     
{
  direction = V;
  myEvaluator->SetOffsetDirection(direction);
}

//=======================================================================
//function : SetOffsetValue
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::SetOffsetValue (const Standard_Real D)   
{
  offsetValue = D;
  myEvaluator->SetOffsetValue(offsetValue);
}


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

void Geom_OffsetCurve::SetBasisCurve (const Handle(Geom_Curve)& C,
                                      const Standard_Boolean isNotCheckC0)
{
  const Standard_Real aUf = C->FirstParameter(),
                      aUl = C->LastParameter();
  Handle(Geom_Curve) aCheckingCurve =  Handle(Geom_Curve)::DownCast(C->Copy());
  Standard_Boolean isTrimmed = Standard_False;

  while(aCheckingCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)) ||
        aCheckingCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
  {
    if (aCheckingCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
    {
      Handle(Geom_TrimmedCurve) aTrimC = 
                Handle(Geom_TrimmedCurve)::DownCast(aCheckingCurve);
      aCheckingCurve = aTrimC->BasisCurve();
      isTrimmed = Standard_True;
    }

    if (aCheckingCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
    {
      Handle(Geom_OffsetCurve) aOC = 
            Handle(Geom_OffsetCurve)::DownCast(aCheckingCurve);
      aCheckingCurve = aOC->BasisCurve();
      Standard_Real PrevOff = aOC->Offset();
      gp_Vec V1(aOC->Direction());
      gp_Vec V2(direction);
      gp_Vec Vdir(PrevOff*V1 + offsetValue*V2);

      if (offsetValue >= 0.)
      {
        offsetValue = Vdir.Magnitude();
        direction.SetXYZ(Vdir.XYZ());
      }
      else
      {
        offsetValue = -Vdir.Magnitude();
        direction.SetXYZ((-Vdir).XYZ());
      }
    }
  }
  
  myBasisCurveContinuity = aCheckingCurve->Continuity();

  Standard_Boolean isC0 = !isNotCheckC0 &&
                          (myBasisCurveContinuity == GeomAbs_C0);

  // Basis curve must be at least C1
  if (isC0 && aCheckingCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    Handle(Geom_BSplineCurve) aBC = Handle(Geom_BSplineCurve)::DownCast(aCheckingCurve);
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
    basisCurve = new Geom_TrimmedCurve(aCheckingCurve, aUf, aUl);
  } 
  else
  {
    basisCurve = aCheckingCurve;
  }

  myEvaluator = new GeomEvaluator_OffsetCurve(basisCurve, offsetValue, direction);
}



//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_OffsetCurve::BasisCurve () const 
{ 
  return basisCurve;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_OffsetCurve::Continuity () const {

  GeomAbs_Shape OffsetShape=GeomAbs_C0;
  switch (myBasisCurveContinuity) {
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

void Geom_OffsetCurve::D0 (const Standard_Real U, gp_Pnt& P) const 
{
  myEvaluator->D0(U, P);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1) const 
{
  myEvaluator->D1(U, P, V1);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::D2 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const 
{
  myEvaluator->D2(U, P, V1, V2);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_OffsetCurve::D3 (const Standard_Real theU, gp_Pnt& theP,
                           gp_Vec& theV1, gp_Vec& theV2, gp_Vec& theV3) const
{
  myEvaluator->D3(theU, theP, theV1, theV2, theV3);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec Geom_OffsetCurve::DN (const Standard_Real U, const Standard_Integer N) const 
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
      throw Standard_NotImplemented("Exception: "
        "Derivative order is greater than 3. Cannot compute of derivative.");
  }
  
  return VN;
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetCurve::FirstParameter () const
{
   return basisCurve->FirstParameter();
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetCurve::LastParameter () const
{
   return basisCurve->LastParameter();
}


//=======================================================================
//function : Offset
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetCurve::Offset () const
{ return offsetValue; }


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

void Geom_OffsetCurve::Transform (const gp_Trsf& T)
{
  basisCurve->Transform (T);
  direction.Transform(T);
  offsetValue *= T.ScaleFactor();

  myEvaluator->SetOffsetValue(offsetValue);
  myEvaluator->SetOffsetDirection(direction);
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

//=======================================================================
//function : GetBasisCurveContinuity
//purpose  : 
//=======================================================================
GeomAbs_Shape Geom_OffsetCurve::GetBasisCurveContinuity() const
{
  return myBasisCurveContinuity;
}
