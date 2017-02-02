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

// Modified     04/10/96 : JCT : derivee des surfaces offset utilisation de
//                               CSLib
// Modified     15/11/96 : JPI : ajout equivalent surface pour les surfaces canoniques et modif des methodes D0 D1, ... UIso,VIso
// Modified     18/11/96 : JPI : inversion de l'offsetValue dans UReverse et Vreverse

#include <AdvApprox_ApproxAFunction.hxx>
#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Convert_GridPolynomialToPoles.hxx>
#include <CSLib.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <Geom_UndefinedValue.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_IsoType.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomEvaluator_OffsetSurface.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfVec.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_OffsetSurface,Geom_Surface)

static const Standard_Real MyAngularToleranceForG1 = Precision::Angular();


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_OffsetSurface::Copy () const
{
  Handle(Geom_OffsetSurface) S(new Geom_OffsetSurface(basisSurf, offsetValue, Standard_True));
  return S;
}

//=======================================================================
//function : Geom_OffsetSurface
//purpose  : Basis surface cannot be an Offset surface or trimmed from
//            offset surface.
//=======================================================================

Geom_OffsetSurface::Geom_OffsetSurface (const Handle(Geom_Surface)& theSurf, 
  const Standard_Real theOffset,
  const Standard_Boolean isNotCheckC0) 
  : offsetValue (theOffset) 
{
  SetBasisSurface(theSurf, isNotCheckC0);
}

//=======================================================================
//function : SetBasisSurface
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::SetBasisSurface (const Handle(Geom_Surface)& S,
  const Standard_Boolean isNotCheckC0)
{
  Standard_Real aUf, aUl, aVf, aVl;
  S->Bounds(aUf, aUl, aVf, aVl);

  Handle(Geom_Surface) aCheckingSurf = Handle(Geom_Surface)::DownCast(S->Copy());
  Standard_Boolean isTrimmed = Standard_False;

  while(aCheckingSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)) ||
        aCheckingSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
  {
    if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      Handle(Geom_RectangularTrimmedSurface) aTrimS = 
        Handle(Geom_RectangularTrimmedSurface)::DownCast(aCheckingSurf);
      aCheckingSurf = aTrimS->BasisSurface();
      isTrimmed = Standard_True;
    }

    if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    {
      Handle(Geom_OffsetSurface) aOS = 
        Handle(Geom_OffsetSurface)::DownCast(aCheckingSurf);
      aCheckingSurf = aOS->BasisSurface();
      offsetValue += aOS->Offset();
    }
  }

  myBasisSurfContinuity = aCheckingSurf->Continuity();

  Standard_Boolean isC0 = !isNotCheckC0 && (myBasisSurfContinuity == GeomAbs_C0);

  // Basis surface must be at least C1
  if (isC0)
  {
    Handle(Geom_Curve) aCurve;

    if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    {
      Handle(Geom_SurfaceOfRevolution) aRevSurf = Handle(Geom_SurfaceOfRevolution)::DownCast(aCheckingSurf);
      aCurve = aRevSurf->BasisCurve();
    }
    else if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
    {
      Handle(Geom_SurfaceOfLinearExtrusion) aLESurf = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(aCheckingSurf);
      aCurve = aLESurf->BasisCurve();
    }

    if(!aCurve.IsNull())
    {
      while(aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)) ||
            aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
      {
        if (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
        {
          Handle(Geom_TrimmedCurve) aTrimC = 
            Handle(Geom_TrimmedCurve)::DownCast(aCurve);
          aCurve = aTrimC->BasisCurve();
        }

        if (aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
        {
          Handle(Geom_OffsetCurve) aOC = 
            Handle(Geom_OffsetCurve)::DownCast(aCurve);
          aCurve = aOC->BasisCurve();
        }
      }
    }

    const Standard_Real aUIsoPar = (aUf + aUl)/2.0, aVIsoPar = (aVf + aVl)/2.0;
    Standard_Boolean isUG1 = Standard_False, isVG1 = Standard_False;
 
    const Handle(Geom_Curve) aCurv1 = aCurve.IsNull() ? aCheckingSurf->UIso(aUIsoPar) : aCurve;
    const Handle(Geom_Curve) aCurv2 = aCheckingSurf->VIso(aVIsoPar);
    isUG1 = !aCurv1->IsKind(STANDARD_TYPE(Geom_BSplineCurve));
    isVG1 = !aCurv2->IsKind(STANDARD_TYPE(Geom_BSplineCurve));

    if(!isUG1)
    {
      Handle(Geom_BSplineCurve) aBC = Handle(Geom_BSplineCurve)::DownCast(aCurv1);
      isUG1 = aBC->IsG1(aVf, aVl, MyAngularToleranceForG1);
    }
    //
    if(!isVG1)
    {
      Handle(Geom_BSplineCurve) aBC = Handle(Geom_BSplineCurve)::DownCast(aCurv2);
      isVG1 = aBC->IsG1(aUf, aUl, MyAngularToleranceForG1);
    }
    //
    if(isUG1 && isVG1) 
    {
      myBasisSurfContinuity = GeomAbs_G1;
      isC0 = Standard_False;
    }

    // Raise exception if still C0
    if (isC0)
      throw Standard_ConstructionError("Offset with no C1 Surface");
  }

  if(isTrimmed)
  {
    basisSurf = 
      new Geom_RectangularTrimmedSurface(aCheckingSurf, aUf, aUl, aVf, aVl);
  }
  else
  {
    basisSurf = aCheckingSurf;
  }
  
  equivSurf = Surface();

  if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)) ||
      aCheckingSurf->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
  {
    // Tolerance en dur pour l'instant ,mais on devrait la proposer dans le constructeur
    // et la mettre en champ, on pourrait utiliser par exemple pour l'extraction d'iso 
    // et aussi pour les singularite. Pour les surfaces osculatrices, on l'utilise pour
    // detecter si une iso est degeneree.
    const Standard_Real Tol = Precision::Confusion(); //0.0001;
    myOscSurf = new Geom_OsculatingSurface(aCheckingSurf, Tol);
  }

  // Surface value calculator
  if (equivSurf.IsNull())
    myEvaluator = new GeomEvaluator_OffsetSurface(basisSurf, offsetValue, myOscSurf);
}

//=======================================================================
//function : SetOffsetValue
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::SetOffsetValue (const Standard_Real D)
{
  offsetValue = D;
  equivSurf = Surface();
  if (equivSurf.IsNull())
  {
    if (myEvaluator.IsNull())
      myEvaluator = new GeomEvaluator_OffsetSurface(basisSurf, offsetValue, myOscSurf);
    else
      myEvaluator->SetOffsetValue(offsetValue);
  }
}

//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::UReverse ()
{
  basisSurf->UReverse();
  offsetValue = -offsetValue;
  if (!equivSurf.IsNull())
    equivSurf->UReverse();
  else
    myEvaluator->SetOffsetValue(offsetValue);
}

//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetSurface::UReversedParameter(const Standard_Real U) const
{
  return basisSurf->UReversedParameter(U);
}

//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::VReverse ()
{
  basisSurf->VReverse();
  offsetValue = -offsetValue;
  if (!equivSurf.IsNull())
    equivSurf->VReverse();
  else
    myEvaluator->SetOffsetValue(offsetValue);
}

//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetSurface::VReversedParameter(const Standard_Real V) const
{
  return basisSurf->VReversedParameter(V);
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::Bounds (Standard_Real& U1, Standard_Real& U2, 
                                 Standard_Real& V1, Standard_Real& V2) const
{
  basisSurf->Bounds (U1, U2 ,V1, V2);
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_OffsetSurface::Continuity () const
{
  switch (myBasisSurfContinuity) {
    case GeomAbs_C2 : return GeomAbs_C1;
    case GeomAbs_C3 : return GeomAbs_C2;
    case GeomAbs_CN : return GeomAbs_CN;
    default : break;
  }
  return GeomAbs_C0;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const
{
#ifdef CHECK  
  if (myBasisSurfContinuity == GeomAbs_C0)
    throw Geom_UndefinedValue();
#endif
  if (equivSurf.IsNull())
    myEvaluator->D0(U, V, P);
  else
    equivSurf->D0(U,V,P);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::D1 (const Standard_Real U, const Standard_Real V, 
  gp_Pnt& P, 
  gp_Vec& D1U, gp_Vec& D1V) const 
{
#ifdef CHECK  
  if (myBasisSurfContinuity == GeomAbs_C0 ||
      myBasisSurfContinuity == GeomAbs_C1)
    throw Geom_UndefinedDerivative();
#endif
  if (equivSurf.IsNull())
    myEvaluator->D1(U, V, P, D1U, D1V);
  else
    equivSurf->D1(U,V,P,D1U,D1V);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::D2 (const Standard_Real U, const Standard_Real V, 
  gp_Pnt& P, 
  gp_Vec& D1U, gp_Vec& D1V,
  gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const
{
#ifdef CHECK  
  if (myBasisSurfContinuity == GeomAbs_C0 ||
      myBasisSurfContinuity == GeomAbs_C1 ||
      myBasisSurfContinuity == GeomAbs_C2)
    throw Geom_UndefinedDerivative();
#endif
  if (equivSurf.IsNull())
    myEvaluator->D2(U, V, P, D1U, D1V, D2U, D2V, D2UV);
  else
    equivSurf->D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::D3 (const Standard_Real U, const Standard_Real V,
  gp_Pnt& P, 
  gp_Vec& D1U, gp_Vec& D1V, 
  gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV,
  gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const
{
#ifdef CHECK  
  if (!(basisSurf->IsCNu (4) && basisSurf->IsCNv (4))) { 
    throw Geom_UndefinedDerivative();
  }
#endif
  if (equivSurf.IsNull())
    myEvaluator->D3(U, V, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
  else
    equivSurf->D3(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec Geom_OffsetSurface::DN (const Standard_Real    U, const Standard_Real    V,
  const Standard_Integer Nu, const Standard_Integer Nv) const
{
  Standard_RangeError_Raise_if (Nu < 0 || Nv < 0 || Nu + Nv < 1, " ");
#ifdef CHECK  
  if (!(basisSurf->IsCNu (Nu) && basisSurf->IsCNv (Nv))) { 
    throw Geom_UndefinedDerivative();
  }
#endif  
  gp_Vec D(0,0,0);

  if (equivSurf.IsNull())
    D = myEvaluator->DN(U, V, Nu, Nv);
  else
    D = equivSurf->DN(U,V,Nu,Nv);
  return D; 
}


////*************************************************
////
////   EVALUATOR FOR THE ISO-CURVE APPROXIMATION
////
////*************************************************

class Geom_OffsetSurface_UIsoEvaluator : public AdvApprox_EvaluatorFunction
{
public:
  Geom_OffsetSurface_UIsoEvaluator (const Handle(Geom_Surface)& theSurface, const Standard_Real theU)
    : CurrentSurface(theSurface), IsoPar(theU) {}

  virtual void Evaluate (Standard_Integer *Dimension,
    Standard_Real     StartEnd[2],
    Standard_Real    *Parameter,
    Standard_Integer *DerivativeRequest,
    Standard_Real    *Result, // [Dimension]
    Standard_Integer *ErrorCode);

private:
  GeomAdaptor_Surface CurrentSurface;
  Standard_Real IsoPar;
};

void Geom_OffsetSurface_UIsoEvaluator::Evaluate(Standard_Integer *,/*Dimension*/
  Standard_Real     /*StartEnd*/[2],
  Standard_Real    *Parameter,
  Standard_Integer *DerivativeRequest,
  Standard_Real    *Result,
  Standard_Integer *ReturnCode) 
{ 
  gp_Pnt P;
  if (*DerivativeRequest == 0) {
    P = CurrentSurface.Value(IsoPar,*Parameter);
    Result[0] = P.X();
    Result[1] = P.Y();
    Result[2] = P.Z();
  }
  else {
    gp_Vec DU,DV;
    CurrentSurface.D1(IsoPar,*Parameter,P,DU,DV);
    Result[0] = DV.X();
    Result[1] = DV.Y();
    Result[2] = DV.Z();
  }
  *ReturnCode = 0;
}

class Geom_OffsetSurface_VIsoEvaluator : public AdvApprox_EvaluatorFunction
{
public:
  Geom_OffsetSurface_VIsoEvaluator (const Handle(Geom_Surface)& theSurface, const Standard_Real theV)
    : CurrentSurface(theSurface), IsoPar(theV) {}

  virtual void Evaluate (Standard_Integer *Dimension,
    Standard_Real     StartEnd[2],
    Standard_Real    *Parameter,
    Standard_Integer *DerivativeRequest,
    Standard_Real    *Result, // [Dimension]
    Standard_Integer *ErrorCode);

private:
  Handle(Geom_Surface) CurrentSurface;
  Standard_Real IsoPar;
};

void Geom_OffsetSurface_VIsoEvaluator::Evaluate(Standard_Integer *,/*Dimension*/
  Standard_Real     /*StartEnd*/[2],
  Standard_Real    *Parameter,
  Standard_Integer *DerivativeRequest,
  Standard_Real    *Result,
  Standard_Integer *ReturnCode) 
{ 
  gp_Pnt P;
  if (*DerivativeRequest == 0) {
    P = CurrentSurface->Value(*Parameter,IsoPar);
    Result[0] = P.X();
    Result[1] = P.Y();
    Result[2] = P.Z();
  }
  else {
    gp_Vec DU,DV;
    CurrentSurface->D1(*Parameter,IsoPar,P,DU,DV);
    Result[0] = DU.X();
    Result[1] = DU.Y();
    Result[2] = DU.Z();
  }
  *ReturnCode = 0;
}

//=======================================================================
//function : UIso
//purpose  : The Uiso or the VIso of an OffsetSurface can't be clearly 
//           exprimed as a curve from Geom. So, to extract the U or VIso
//           an Approximation is needed. This approx always will return a 
//           BSplineCurve from Geom.
//=======================================================================

Handle(Geom_Curve) Geom_OffsetSurface::UIso (const Standard_Real UU) const 
{
  if (equivSurf.IsNull()) {
    const Standard_Integer Num1 = 0, Num2 = 0, Num3 = 1;
    Handle(TColStd_HArray1OfReal) T1, T2, T3 = new TColStd_HArray1OfReal(1,Num3);
    T3->Init(Precision::Approximation());
    Standard_Real U1,U2,V1,V2;
    Bounds(U1,U2,V1,V2);
    const GeomAbs_Shape Cont = GeomAbs_C1;
    const Standard_Integer MaxSeg = 100, MaxDeg = 14;

    Handle(Geom_OffsetSurface) me (this);
    Geom_OffsetSurface_UIsoEvaluator ev (me, UU);
    AdvApprox_ApproxAFunction Approx(Num1, Num2, Num3, T1, T2, T3,
      V1, V2, Cont, MaxDeg, MaxSeg, ev);

    Standard_ConstructionError_Raise_if (!Approx.IsDone(), " Geom_OffsetSurface : UIso");

    const Standard_Integer NbPoles = Approx.NbPoles();

    TColgp_Array1OfPnt      Poles( 1, NbPoles);
    TColStd_Array1OfReal    Knots( 1, Approx.NbKnots());
    TColStd_Array1OfInteger Mults( 1, Approx.NbKnots());

    Approx.Poles(1, Poles);
    Knots = Approx.Knots()->Array1();
    Mults = Approx.Multiplicities()->Array1();

    Handle(Geom_BSplineCurve) C = 
      new Geom_BSplineCurve( Poles, Knots, Mults, Approx.Degree());
    return C;
  }
  else
    return equivSurf->UIso(UU);
}

//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_OffsetSurface::VIso (const Standard_Real VV) const 
{
  if (equivSurf.IsNull()) {
    const Standard_Integer Num1 = 0, Num2 = 0, Num3 = 1;
    Handle(TColStd_HArray1OfReal) T1, T2, T3 = new TColStd_HArray1OfReal(1,Num3);
    T3->Init(Precision::Approximation());
    Standard_Real U1,U2,V1,V2;
    Bounds(U1,U2,V1,V2);
    const GeomAbs_Shape Cont = GeomAbs_C1;
    const Standard_Integer MaxSeg = 100, MaxDeg = 14;

    Handle(Geom_OffsetSurface) me (this);
    Geom_OffsetSurface_VIsoEvaluator ev (me, VV);
    AdvApprox_ApproxAFunction Approx (Num1, Num2, Num3, T1, T2, T3,
      U1, U2, Cont, MaxDeg, MaxSeg, ev);

    Standard_ConstructionError_Raise_if (!Approx.IsDone(), " Geom_OffsetSurface : VIso");

    TColgp_Array1OfPnt      Poles( 1, Approx.NbPoles());
    TColStd_Array1OfReal    Knots( 1, Approx.NbKnots());
    TColStd_Array1OfInteger Mults( 1, Approx.NbKnots());

    Approx.Poles(1, Poles);
    Knots = Approx.Knots()->Array1();
    Mults = Approx.Multiplicities()->Array1();

    Handle(Geom_BSplineCurve) C = 
      new Geom_BSplineCurve( Poles, Knots, Mults, Approx.Degree());
    return C;
  }
  else
    return equivSurf->VIso(VV);
}

//=======================================================================
//function : IsCNu
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsCNu (const Standard_Integer N) const
{
  Standard_RangeError_Raise_if (N < 0, " ");
  return basisSurf->IsCNu (N+1);
}

//=======================================================================
//function : IsCNv
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsCNv (const Standard_Integer N) const
{
  Standard_RangeError_Raise_if (N < 0, " ");
  return basisSurf->IsCNv (N+1);
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsUPeriodic () const 
{
  return basisSurf->IsUPeriodic();
}

//=======================================================================
//function : UPeriod
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetSurface::UPeriod() const
{
  return basisSurf->UPeriod();
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsVPeriodic () const 
{
  return basisSurf->IsVPeriodic();
}

//=======================================================================
//function : VPeriod
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetSurface::VPeriod() const
{
  return basisSurf->VPeriod();
}

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsUClosed () const
{
  Standard_Boolean UClosed;
  Handle(Geom_Surface) SBasis = BasisSurface();

  if (SBasis->IsKind (STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) St = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(SBasis);

    Handle(Geom_Surface) S = St->BasisSurface();
    if (S->IsKind (STANDARD_TYPE(Geom_ElementarySurface))) {
      UClosed = SBasis->IsUClosed();
    }
    else if (S->IsKind (STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) { 
      Handle(Geom_SurfaceOfLinearExtrusion) Extru = 
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(S);

      Handle(Geom_Curve) C = Extru->BasisCurve();
      if (C->IsKind (STANDARD_TYPE(Geom_Circle)) || C->IsKind (STANDARD_TYPE(Geom_Ellipse))) {
        UClosed = SBasis->IsUClosed();
      }
      else { UClosed = Standard_False; }
    }
    else if (S->IsKind (STANDARD_TYPE(Geom_SurfaceOfRevolution))) { 
      UClosed = SBasis->IsUClosed();
    }
    else { UClosed = Standard_False; }
  }
  else {
    if (SBasis->IsKind (STANDARD_TYPE(Geom_ElementarySurface))) {
      UClosed = SBasis->IsUClosed();
    }
    else if (SBasis->IsKind (STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) { 
      Handle(Geom_SurfaceOfLinearExtrusion) Extru = 
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(SBasis);

      Handle(Geom_Curve) C = Extru->BasisCurve();
      UClosed = (C->IsKind(STANDARD_TYPE(Geom_Circle)) || C->IsKind(STANDARD_TYPE(Geom_Ellipse)));
    }
    else if (SBasis->IsKind (STANDARD_TYPE(Geom_SurfaceOfRevolution))) { 
      UClosed = Standard_True; 
    }
    else { UClosed = Standard_False; }
  }  
  return UClosed;
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsVClosed () const
{
  Standard_Boolean VClosed;
  Handle(Geom_Surface) SBasis = BasisSurface();

  if (SBasis->IsKind (STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) St = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(SBasis);

    Handle(Geom_Surface) S = St->BasisSurface();
    if (S->IsKind (STANDARD_TYPE(Geom_ElementarySurface))) {
      VClosed = SBasis->IsVClosed();
    }
    else { VClosed = Standard_False; }
  }
  else {
    if (SBasis->IsKind (STANDARD_TYPE(Geom_ElementarySurface))) {
      VClosed = SBasis->IsVClosed();
    }
    else { VClosed = Standard_False; }
  }
  return VClosed;
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::Transform (const gp_Trsf& T)
{
  basisSurf->Transform (T);
  offsetValue *= T.ScaleFactor();
  equivSurf.Nullify();
  if (myEvaluator.IsNull())
    myEvaluator = new GeomEvaluator_OffsetSurface(basisSurf, offsetValue, myOscSurf);
  else
    myEvaluator->SetOffsetValue(offsetValue);
}

//=======================================================================
//function : TransformParameters
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::TransformParameters(Standard_Real& U, Standard_Real& V,
  const gp_Trsf& T) const
{
  basisSurf->TransformParameters(U,V,T);
  if(!equivSurf.IsNull()) equivSurf->TransformParameters(U,V,T);
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

gp_GTrsf2d Geom_OffsetSurface::ParametricTransformation (const gp_Trsf& T) const
{
  return basisSurf->ParametricTransformation(T);
}

//=======================================================================
//function : Surface
//purpose  : Trouve si elle existe, une surface non offset, equivalente
//           a l'offset surface.
//=======================================================================

Handle(Geom_Surface) Geom_OffsetSurface::Surface() const 
{
  if (offsetValue == 0.0) return  basisSurf; // Cas direct 

  Standard_Real Tol = Precision::Confusion();
  Handle(Geom_Surface) Result, Base;
  Result.Nullify();
  Handle(Standard_Type) TheType = basisSurf->DynamicType();
  Standard_Boolean IsTrimmed;
  Standard_Real U1 = 0., V1 = 0., U2 = 0., V2 = 0.;

  // Preambule pour les surface trimmes
  if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_RectangularTrimmedSurface) S = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(basisSurf);
    Base = S->BasisSurface();
    TheType = Base->DynamicType();
    S->Bounds(U1,U2,V1,V2);  
    IsTrimmed = Standard_True;
  }
  else {
    IsTrimmed = Standard_False;
    Base = basisSurf;
  }

  // Traite les surfaces cannonique
  if (TheType == STANDARD_TYPE(Geom_Plane)) 
  {
    Handle(Geom_Plane) P =
      Handle(Geom_Plane)::DownCast(Base);
    gp_Vec T = P->Position().XDirection()^P->Position().YDirection();
    T *= offsetValue;
    Result = Handle(Geom_Plane)::DownCast(P->Translated(T));
  }
  else if (TheType == STANDARD_TYPE(Geom_CylindricalSurface)) 
  {
    Handle(Geom_CylindricalSurface) C =
      Handle(Geom_CylindricalSurface)::DownCast(Base);
    Standard_Real Radius = C->Radius();
    gp_Ax3 Axis = C->Position();
    if (Axis.Direct()) 
      Radius += offsetValue;
    else 
      Radius -= offsetValue;
    if ( Radius >= Tol ) {
      Result = new Geom_CylindricalSurface( Axis, Radius);
    }
    else if ( Radius <= -Tol ){
      Axis.Rotate(gp_Ax1(Axis.Location(),Axis.Direction()),M_PI);
      Result = new Geom_CylindricalSurface( Axis, Abs(Radius));
      Result->UReverse();
    }
    else 
    {
      // surface degeneree      
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ConicalSurface)) 
  {
    Handle(Geom_ConicalSurface) C =
      Handle(Geom_ConicalSurface)::DownCast(Base);
    gp_Ax3 anAxis = C->Position();
    Standard_Boolean isDirect = anAxis.Direct();
    Standard_Real anAlpha = C->SemiAngle();
    Standard_Real aRadius;
    if (isDirect)
    {
      aRadius = C->RefRadius() + offsetValue * Cos (anAlpha);
    }
    else
    {
      aRadius = C->RefRadius() - offsetValue * Cos (anAlpha);
    }
    if (aRadius >= 0.)
    {
      gp_Vec aZ (anAxis.Direction());
      if (isDirect)
      {
        aZ *= -offsetValue * Sin (anAlpha);
      }
      else
      {
        aZ *=  offsetValue * Sin (anAlpha);
      }
      anAxis.Translate (aZ);
      Result = new Geom_ConicalSurface (anAxis, anAlpha, aRadius);
    }
    else
    {
      // surface degeneree      
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_SphericalSurface)) {
    Handle(Geom_SphericalSurface) S = 
      Handle(Geom_SphericalSurface)::DownCast(Base);
    Standard_Real Radius = S->Radius();
    gp_Ax3 Axis = S->Position();
    if (Axis.Direct()) 
      Radius += offsetValue;
    else 
      Radius -= offsetValue;
    if ( Radius >= Tol) {
      Result = new Geom_SphericalSurface(Axis, Radius);
    }
    else if ( Radius <= -Tol ) {
      Axis.Rotate(gp_Ax1(Axis.Location(),Axis.Direction()),M_PI);
      Axis.ZReverse();
      Result = new Geom_SphericalSurface(Axis, -Radius);
      Result->UReverse();
    }
    else {
      //      surface degeneree
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface)) 

  {
    Handle(Geom_ToroidalSurface) 
      S = Handle(Geom_ToroidalSurface)::DownCast(Base);
    Standard_Real MajorRadius = S->MajorRadius();
    Standard_Real MinorRadius = S->MinorRadius();
    gp_Ax3 Axis = S->Position();
    if (MinorRadius <= MajorRadius) 
    {  
      if (Axis.Direct())
        MinorRadius += offsetValue;
      else 
        MinorRadius -= offsetValue;
      if (MinorRadius >= Tol) 
        Result = new Geom_ToroidalSurface(Axis,MajorRadius,MinorRadius);
      //      else if (MinorRadius <= -Tol) 
      //        Result->UReverse();
      else 
      {
        //	surface degeneree
      }
    }
  }

  // S'il le faut on trimme le resultat
  if (IsTrimmed && !Result.IsNull()) {
    Base = Result;
    Result = new Geom_RectangularTrimmedSurface (Base, U1, U2, V1,V2);
  }

  return Result;
}

//=======================================================================
//function : UOsculatingSurface
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::UOsculatingSurface(const Standard_Real U, const Standard_Real V,
  Standard_Boolean& t, Handle(Geom_BSplineSurface)& L) const
{
  return !myOscSurf.IsNull() && myOscSurf->UOscSurf(U,V,t,L);
}

//=======================================================================
//function : VOsculatingSurface
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::VOsculatingSurface(const Standard_Real U, const Standard_Real V,
  Standard_Boolean& t, Handle(Geom_BSplineSurface)& L) const
{
  return !myOscSurf.IsNull() && myOscSurf->VOscSurf(U, V, t, L);
}
