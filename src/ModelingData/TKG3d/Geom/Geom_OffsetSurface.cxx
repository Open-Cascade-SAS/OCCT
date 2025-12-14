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
// Modified     15/11/96 : JPI : ajout equivalent surface pour les surfaces canoniques et modif des
// methodes D0 D1, ... UIso,VIso Modified     18/11/96 : JPI : inversion de l'offsetValue dans
// UReverse et Vreverse

#include <AdvApprox_ApproxAFunction.hxx>
#include <CSLib.hxx>
#include <CSLib_NormalStatus.hxx>
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
#include "Geom_OsculatingSurface.pxx"
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
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLProp_SLProps.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfVec.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_OffsetSurface, Geom_Surface)

namespace
{

// tolerance for considering derivative to be null
const Standard_Real THE_D1_MAG_TOL = 1.e-9;

// If calculation of normal fails, try shifting the point towards the center
// of the parametric space of the surface, in the hope that derivatives
// are better defined there.
static Standard_Boolean shiftPoint(const Standard_Real         theUStart,
                                   const Standard_Real         theVStart,
                                   Standard_Real&              theU,
                                   Standard_Real&              theV,
                                   const Handle(Geom_Surface)& theSurf,
                                   const gp_Vec&               theD1U,
                                   const gp_Vec&               theD1V)
{
  Standard_Real aUMin, aUMax, aVMin, aVMax;
  theSurf->Bounds(aUMin, aUMax, aVMin, aVMax);
  Standard_Boolean isUPeriodic = theSurf->IsUPeriodic();
  Standard_Boolean isVPeriodic = theSurf->IsVPeriodic();

  Standard_Boolean isUSingular = (theD1U.SquareMagnitude() < THE_D1_MAG_TOL * THE_D1_MAG_TOL);
  Standard_Boolean isVSingular = (theD1V.SquareMagnitude() < THE_D1_MAG_TOL * THE_D1_MAG_TOL);

  Standard_Real aDirU =
    (isUPeriodic || (isUSingular && !isVSingular) ? 0. : 0.5 * (aUMin + aUMax) - theUStart);
  Standard_Real aDirV =
    (isVPeriodic || (isVSingular && !isUSingular) ? 0. : 0.5 * (aVMin + aVMax) - theVStart);
  Standard_Real aDist = std::sqrt(aDirU * aDirU + aDirV * aDirV);

  Standard_Real aDU   = theU - theUStart;
  Standard_Real aDV   = theV - theVStart;
  Standard_Real aStep = std::max(2. * std::sqrt(aDU * aDU + aDV * aDV), Precision::PConfusion());
  if (aStep >= aDist)
  {
    return Standard_False;
  }

  aStep /= aDist;
  theU += aDirU * aStep;
  theV += aDirV * aStep;

  return Standard_True;
}

// auxiliary function for computing derivatives at singular points
template <class SurfType>
static void computeDerivatives(Standard_Integer                   theMaxOrder,
                               Standard_Integer                   theMinOrder,
                               const Standard_Real                theU,
                               const Standard_Real                theV,
                               const SurfType&                    theBasisSurf,
                               const Standard_Integer             theNU,
                               const Standard_Integer             theNV,
                               const Standard_Boolean             theAlongU,
                               const Standard_Boolean             theAlongV,
                               const Handle(Geom_BSplineSurface)& theL,
                               TColgp_Array2OfVec&                theDerNUV,
                               TColgp_Array2OfVec&                theDerSurf)
{
  Standard_Integer i, j;
  gp_Pnt           P;
  gp_Vec           DL1U, DL1V, DL2U, DL2V, DL2UV, DL3U, DL3UUV, DL3UVV, DL3V;

  if (theAlongU || theAlongV)
  {
    theMaxOrder = 0;
    TColgp_Array2OfVec DerSurfL(0, theMaxOrder + theNU + 1, 0, theMaxOrder + theNV + 1);
    switch (theMinOrder)
    {
      case 1:
        theL->D1(theU, theV, P, DL1U, DL1V);
        DerSurfL.SetValue(1, 0, DL1U);
        DerSurfL.SetValue(0, 1, DL1V);
        break;
      case 2:
        theL->D2(theU, theV, P, DL1U, DL1V, DL2U, DL2V, DL2UV);
        DerSurfL.SetValue(1, 0, DL1U);
        DerSurfL.SetValue(0, 1, DL1V);
        DerSurfL.SetValue(1, 1, DL2UV);
        DerSurfL.SetValue(2, 0, DL2U);
        DerSurfL.SetValue(0, 2, DL2V);
        break;
      case 3:
        theL->D3(theU, theV, P, DL1U, DL1V, DL2U, DL2V, DL2UV, DL3U, DL3V, DL3UUV, DL3UVV);
        DerSurfL.SetValue(1, 0, DL1U);
        DerSurfL.SetValue(0, 1, DL1V);
        DerSurfL.SetValue(1, 1, DL2UV);
        DerSurfL.SetValue(2, 0, DL2U);
        DerSurfL.SetValue(0, 2, DL2V);
        DerSurfL.SetValue(3, 0, DL3U);
        DerSurfL.SetValue(2, 1, DL3UUV);
        DerSurfL.SetValue(1, 2, DL3UVV);
        DerSurfL.SetValue(0, 3, DL3V);
        break;
      default:
        break;
    }

    if (theNU <= theNV)
    {
      for (i = 0; i <= theMaxOrder + 1 + theNU; i++)
        for (j = i; j <= theMaxOrder + theNV + 1; j++)
          if (i + j > theMinOrder)
          {
            DerSurfL.SetValue(i, j, theL->DN(theU, theV, i, j));
            theDerSurf.SetValue(i, j, theBasisSurf->DN(theU, theV, i, j));
            if (i != j && j <= theNU + 1)
            {
              theDerSurf.SetValue(j, i, theBasisSurf->DN(theU, theV, j, i));
              DerSurfL.SetValue(j, i, theL->DN(theU, theV, j, i));
            }
          }
    }
    else
    {
      for (j = 0; j <= theMaxOrder + 1 + theNV; j++)
        for (i = j; i <= theMaxOrder + theNU + 1; i++)
          if (i + j > theMinOrder)
          {
            DerSurfL.SetValue(i, j, theL->DN(theU, theV, i, j));
            theDerSurf.SetValue(i, j, theBasisSurf->DN(theU, theV, i, j));
            if (i != j && i <= theNV + 1)
            {
              theDerSurf.SetValue(j, i, theBasisSurf->DN(theU, theV, j, i));
              DerSurfL.SetValue(j, i, theL->DN(theU, theV, j, i));
            }
          }
    }
    for (i = 0; i <= theMaxOrder + theNU; i++)
      for (j = 0; j <= theMaxOrder + theNV; j++)
      {
        if (theAlongU)
          theDerNUV.SetValue(i, j, CSLib::DNNUV(i, j, DerSurfL, theDerSurf));
        if (theAlongV)
          theDerNUV.SetValue(i, j, CSLib::DNNUV(i, j, theDerSurf, DerSurfL));
      }
  }
  else
  {
    for (i = 0; i <= theMaxOrder + theNU + 1; i++)
    {
      for (j = i; j <= theMaxOrder + theNV + 1; j++)
      {
        if (i + j > theMinOrder)
        {
          theDerSurf.SetValue(i, j, theBasisSurf->DN(theU, theV, i, j));
          if (i != j && j <= theDerSurf.UpperRow() && i <= theDerSurf.UpperCol())
          {
            theDerSurf.SetValue(j, i, theBasisSurf->DN(theU, theV, j, i));
          }
        }
      }
    }
    for (i = 0; i <= theMaxOrder + theNU; i++)
      for (j = 0; j <= theMaxOrder + theNV; j++)
        theDerNUV.SetValue(i, j, CSLib::DNNUV(i, j, theDerSurf));
  }
}

inline Standard_Boolean IsInfiniteCoord(const gp_Vec& theVec)
{
  return Precision::IsInfinite(theVec.X()) || Precision::IsInfinite(theVec.Y())
         || Precision::IsInfinite(theVec.Z());
}

inline void CheckInfinite(const gp_Vec& theVecU, const gp_Vec& theVecV)
{
  if (IsInfiniteCoord(theVecU) || IsInfiniteCoord(theVecV))
  {
    throw Standard_NumericError("Geom_OffsetSurface: Evaluation of infinite parameters");
  }
}

} // end of anonymous namespace

static const Standard_Real MyAngularToleranceForG1 = Precision::Angular();

//=================================================================================================

Handle(Geom_Geometry) Geom_OffsetSurface::Copy() const
{
  return new Geom_OffsetSurface(*this);
}

//=======================================================================
// function : Geom_OffsetSurface
// purpose  : Basis surface cannot be an Offset surface or trimmed from
//            offset surface.
//=======================================================================

Geom_OffsetSurface::Geom_OffsetSurface(const Geom_OffsetSurface& theOther)
    : basisSurf(Handle(Geom_Surface)::DownCast(theOther.basisSurf->Copy())),
      equivSurf(theOther.equivSurf.IsNull()
                  ? Handle(Geom_Surface)()
                  : Handle(Geom_Surface)::DownCast(theOther.equivSurf->Copy())),
      offsetValue(theOther.offsetValue),
      myOscSurf(theOther.myOscSurf ? std::make_unique<Geom_OsculatingSurface>(*theOther.myOscSurf)
                                   : nullptr),
      myBasisSurfContinuity(theOther.myBasisSurfContinuity)
{
  // Deep copy without validation - source surface is already validated
}

//=================================================================================================

Geom_OffsetSurface::~Geom_OffsetSurface() = default;

//=================================================================================================

Geom_OffsetSurface::Geom_OffsetSurface(const Handle(Geom_Surface)& theSurf,
                                       const Standard_Real         theOffset,
                                       const Standard_Boolean      isNotCheckC0)
    : offsetValue(theOffset)
{
  SetBasisSurface(theSurf, isNotCheckC0);
}

//=================================================================================================

void Geom_OffsetSurface::SetBasisSurface(const Handle(Geom_Surface)& S,
                                         const Standard_Boolean      isNotCheckC0)
{
  Standard_Real aUf, aUl, aVf, aVl;
  S->Bounds(aUf, aUl, aVf, aVl);

  Handle(Geom_Surface) aCheckingSurf = Handle(Geom_Surface)::DownCast(S->Copy());
  Standard_Boolean     isTrimmed     = Standard_False;

  while (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))
         || aCheckingSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
  {
    if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      Handle(Geom_RectangularTrimmedSurface) aTrimS =
        Handle(Geom_RectangularTrimmedSurface)::DownCast(aCheckingSurf);
      aCheckingSurf = aTrimS->BasisSurface();
      isTrimmed     = Standard_True;
    }

    if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    {
      Handle(Geom_OffsetSurface) aOS = Handle(Geom_OffsetSurface)::DownCast(aCheckingSurf);
      aCheckingSurf                  = aOS->BasisSurface();
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
      Handle(Geom_SurfaceOfRevolution) aRevSurf =
        Handle(Geom_SurfaceOfRevolution)::DownCast(aCheckingSurf);
      aCurve = aRevSurf->BasisCurve();
    }
    else if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
    {
      Handle(Geom_SurfaceOfLinearExtrusion) aLESurf =
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(aCheckingSurf);
      aCurve = aLESurf->BasisCurve();
    }

    if (!aCurve.IsNull())
    {
      while (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))
             || aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
      {
        if (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
        {
          Handle(Geom_TrimmedCurve) aTrimC = Handle(Geom_TrimmedCurve)::DownCast(aCurve);
          aCurve                           = aTrimC->BasisCurve();
        }

        if (aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
        {
          Handle(Geom_OffsetCurve) aOC = Handle(Geom_OffsetCurve)::DownCast(aCurve);
          aCurve                       = aOC->BasisCurve();
        }
      }
    }

    const Standard_Real aUIsoPar = (aUf + aUl) / 2.0, aVIsoPar = (aVf + aVl) / 2.0;
    Standard_Boolean    isUG1 = Standard_False, isVG1 = Standard_False;

    const Handle(Geom_Curve) aCurv1 = aCurve.IsNull() ? aCheckingSurf->UIso(aUIsoPar) : aCurve;
    const Handle(Geom_Curve) aCurv2 = aCheckingSurf->VIso(aVIsoPar);
    isUG1                           = !aCurv1->IsKind(STANDARD_TYPE(Geom_BSplineCurve));
    isVG1                           = !aCurv2->IsKind(STANDARD_TYPE(Geom_BSplineCurve));

    if (!isUG1)
    {
      Handle(Geom_BSplineCurve) aBC = Handle(Geom_BSplineCurve)::DownCast(aCurv1);
      isUG1                         = aBC->IsG1(aVf, aVl, MyAngularToleranceForG1);
    }
    //
    if (!isVG1)
    {
      Handle(Geom_BSplineCurve) aBC = Handle(Geom_BSplineCurve)::DownCast(aCurv2);
      isVG1                         = aBC->IsG1(aUf, aUl, MyAngularToleranceForG1);
    }
    //
    if (isUG1 && isVG1)
    {
      myBasisSurfContinuity = GeomAbs_G1;
      isC0                  = Standard_False;
    }

    // Raise exception if still C0
    if (isC0)
      throw Standard_ConstructionError("Offset with no C1 Surface");
  }

  if (isTrimmed)
  {
    basisSurf = new Geom_RectangularTrimmedSurface(aCheckingSurf, aUf, aUl, aVf, aVl);
  }
  else
  {
    basisSurf = aCheckingSurf;
  }

  equivSurf = Surface();

  if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface))
      || aCheckingSurf->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
  {
    // Tolerance en dur pour l'instant ,mais on devrait la proposer dans le constructeur
    // et la mettre en champ, on pourrait utiliser par exemple pour l'extraction d'iso
    // et aussi pour les singularite. Pour les surfaces osculatrices, on l'utilise pour
    // detecter si une iso est degeneree.
    constexpr Standard_Real Tol = Precision::Confusion(); // 0.0001;
    myOscSurf                   = std::make_unique<Geom_OsculatingSurface>(aCheckingSurf, Tol);
  }
}

//=================================================================================================

void Geom_OffsetSurface::SetOffsetValue(const Standard_Real D)
{
  offsetValue = D;
  equivSurf   = Surface();
}

//=================================================================================================

void Geom_OffsetSurface::UReverse()
{
  basisSurf->UReverse();
  offsetValue = -offsetValue;
  if (!equivSurf.IsNull())
    equivSurf->UReverse();
}

//=================================================================================================

Standard_Real Geom_OffsetSurface::UReversedParameter(const Standard_Real U) const
{
  return basisSurf->UReversedParameter(U);
}

//=================================================================================================

void Geom_OffsetSurface::VReverse()
{
  basisSurf->VReverse();
  offsetValue = -offsetValue;
  if (!equivSurf.IsNull())
    equivSurf->VReverse();
}

//=================================================================================================

Standard_Real Geom_OffsetSurface::VReversedParameter(const Standard_Real V) const
{
  return basisSurf->VReversedParameter(V);
}

//=================================================================================================

void Geom_OffsetSurface::Bounds(Standard_Real& U1,
                                Standard_Real& U2,
                                Standard_Real& V1,
                                Standard_Real& V2) const
{
  basisSurf->Bounds(U1, U2, V1, V2);
}

//=================================================================================================

GeomAbs_Shape Geom_OffsetSurface::Continuity() const
{
  switch (myBasisSurfContinuity)
  {
    case GeomAbs_C2:
      return GeomAbs_C1;
    case GeomAbs_C3:
      return GeomAbs_C2;
    case GeomAbs_CN:
      return GeomAbs_CN;
    default:
      break;
  }
  return GeomAbs_C0;
}

//=================================================================================================

void Geom_OffsetSurface::D0(const Standard_Real U, const Standard_Real V, gp_Pnt& P) const
{
#ifdef CHECK
  if (myBasisSurfContinuity == GeomAbs_C0)
    throw Geom_UndefinedValue();
#endif
  if (!equivSurf.IsNull())
  {
    equivSurf->D0(U, V, P);
    return;
  }

  Standard_Real aU = U, aV = V;
  for (;;)
  {
    gp_Vec aD1U, aD1V;
    basisSurf->D1(aU, aV, P, aD1U, aD1V);
    CheckInfinite(aD1U, aD1V);

    try
    {
      calculateD0(aU, aV, P, aD1U, aD1V);
      break;
    }
    catch (Geom_UndefinedValue&)
    {
      // if failed at parametric boundary, try taking derivative at shifted point
      if (!shiftPoint(U, V, aU, aV, basisSurf, aD1U, aD1V))
      {
        throw;
      }
    }
  }
}

//=================================================================================================

void Geom_OffsetSurface::D1(const Standard_Real U,
                            const Standard_Real V,
                            gp_Pnt&             P,
                            gp_Vec&             D1U,
                            gp_Vec&             D1V) const
{
#ifdef CHECK
  if (myBasisSurfContinuity == GeomAbs_C0 || myBasisSurfContinuity == GeomAbs_C1)
    throw Geom_UndefinedDerivative();
#endif
  if (!equivSurf.IsNull())
  {
    equivSurf->D1(U, V, P, D1U, D1V);
    return;
  }

  Standard_Real aU = U, aV = V;
  for (;;)
  {
    gp_Vec aD2U, aD2V, aD2UV;
    basisSurf->D2(aU, aV, P, D1U, D1V, aD2U, aD2V, aD2UV);
    CheckInfinite(D1U, D1V);

    try
    {
      calculateD1(aU, aV, P, D1U, D1V, aD2U, aD2V, aD2UV);
      break;
    }
    catch (Geom_UndefinedValue&)
    {
      // if failed at parametric boundary, try taking derivative at shifted point
      if (!shiftPoint(U, V, aU, aV, basisSurf, D1U, D1V))
      {
        throw;
      }
    }
  }
}

//=================================================================================================

void Geom_OffsetSurface::D2(const Standard_Real U,
                            const Standard_Real V,
                            gp_Pnt&             P,
                            gp_Vec&             D1U,
                            gp_Vec&             D1V,
                            gp_Vec&             D2U,
                            gp_Vec&             D2V,
                            gp_Vec&             D2UV) const
{
#ifdef CHECK
  if (myBasisSurfContinuity == GeomAbs_C0 || myBasisSurfContinuity == GeomAbs_C1
      || myBasisSurfContinuity == GeomAbs_C2)
    throw Geom_UndefinedDerivative();
#endif
  if (!equivSurf.IsNull())
  {
    equivSurf->D2(U, V, P, D1U, D1V, D2U, D2V, D2UV);
    return;
  }

  Standard_Real aU = U, aV = V;
  for (;;)
  {
    gp_Vec aD3U, aD3V, aD3UUV, aD3UVV;
    basisSurf->D3(aU, aV, P, D1U, D1V, D2U, D2V, D2UV, aD3U, aD3V, aD3UUV, aD3UVV);
    CheckInfinite(D1U, D1V);

    try
    {
      calculateD2(aU, aV, P, D1U, D1V, D2U, D2V, D2UV, aD3U, aD3V, aD3UUV, aD3UVV);
      break;
    }
    catch (Geom_UndefinedValue&)
    {
      // if failed at parametric boundary, try taking derivative at shifted point
      if (!shiftPoint(U, V, aU, aV, basisSurf, D1U, D1V))
      {
        throw;
      }
    }
  }
}

//=================================================================================================

void Geom_OffsetSurface::D3(const Standard_Real U,
                            const Standard_Real V,
                            gp_Pnt&             P,
                            gp_Vec&             D1U,
                            gp_Vec&             D1V,
                            gp_Vec&             D2U,
                            gp_Vec&             D2V,
                            gp_Vec&             D2UV,
                            gp_Vec&             D3U,
                            gp_Vec&             D3V,
                            gp_Vec&             D3UUV,
                            gp_Vec&             D3UVV) const
{
#ifdef CHECK
  if (!(basisSurf->IsCNu(4) && basisSurf->IsCNv(4)))
  {
    throw Geom_UndefinedDerivative();
  }
#endif
  if (!equivSurf.IsNull())
  {
    equivSurf->D3(U, V, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
    return;
  }

  Standard_Real aU = U, aV = V;
  for (;;)
  {
    basisSurf->D3(aU, aV, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
    CheckInfinite(D1U, D1V);

    try
    {
      calculateD3(aU, aV, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
      break;
    }
    catch (Geom_UndefinedValue&)
    {
      // if failed at parametric boundary, try taking derivative at shifted point
      if (!shiftPoint(U, V, aU, aV, basisSurf, D1U, D1V))
      {
        throw;
      }
    }
  }
}

//=================================================================================================

gp_Vec Geom_OffsetSurface::DN(const Standard_Real    U,
                              const Standard_Real    V,
                              const Standard_Integer Nu,
                              const Standard_Integer Nv) const
{
  Standard_RangeError_Raise_if(Nu < 0 || Nv < 0 || Nu + Nv < 1, " ");
#ifdef CHECK
  if (!(basisSurf->IsCNu(Nu) && basisSurf->IsCNv(Nv)))
  {
    throw Geom_UndefinedDerivative();
  }
#endif
  if (!equivSurf.IsNull())
  {
    return equivSurf->DN(U, V, Nu, Nv);
  }

  Standard_Real aU = U, aV = V;
  for (;;)
  {
    gp_Pnt aP;
    gp_Vec aD1U, aD1V;
    basisSurf->D1(aU, aV, aP, aD1U, aD1V);
    CheckInfinite(aD1U, aD1V);

    try
    {
      return calculateDN(aU, aV, Nu, Nv, aD1U, aD1V);
    }
    catch (Geom_UndefinedValue&)
    {
      // if failed at parametric boundary, try taking derivative at shifted point
      if (!shiftPoint(U, V, aU, aV, basisSurf, aD1U, aD1V))
      {
        throw;
      }
    }
  }
}

////*************************************************
////
////   EVALUATOR FOR THE ISO-CURVE APPROXIMATION
////
////*************************************************

class Geom_OffsetSurface_UIsoEvaluator : public AdvApprox_EvaluatorFunction
{
public:
  Geom_OffsetSurface_UIsoEvaluator(const Handle(Geom_Surface)& theSurface, const Standard_Real theU)
      : CurrentSurface(theSurface),
        IsoPar(theU)
  {
  }

  virtual void Evaluate(Standard_Integer* Dimension,
                        Standard_Real     StartEnd[2],
                        Standard_Real*    Parameter,
                        Standard_Integer* DerivativeRequest,
                        Standard_Real*    Result, // [Dimension]
                        Standard_Integer* ErrorCode);

private:
  GeomAdaptor_Surface CurrentSurface;
  Standard_Real       IsoPar;
};

void Geom_OffsetSurface_UIsoEvaluator::Evaluate(Standard_Integer*, /*Dimension*/
                                                Standard_Real /*StartEnd*/[2],
                                                Standard_Real*    Parameter,
                                                Standard_Integer* DerivativeRequest,
                                                Standard_Real*    Result,
                                                Standard_Integer* ReturnCode)
{
  gp_Pnt P;
  if (*DerivativeRequest == 0)
  {
    P         = CurrentSurface.Value(IsoPar, *Parameter);
    Result[0] = P.X();
    Result[1] = P.Y();
    Result[2] = P.Z();
  }
  else
  {
    gp_Vec DU, DV;
    CurrentSurface.D1(IsoPar, *Parameter, P, DU, DV);
    Result[0] = DV.X();
    Result[1] = DV.Y();
    Result[2] = DV.Z();
  }
  *ReturnCode = 0;
}

class Geom_OffsetSurface_VIsoEvaluator : public AdvApprox_EvaluatorFunction
{
public:
  Geom_OffsetSurface_VIsoEvaluator(const Handle(Geom_Surface)& theSurface, const Standard_Real theV)
      : CurrentSurface(theSurface),
        IsoPar(theV)
  {
  }

  virtual void Evaluate(Standard_Integer* Dimension,
                        Standard_Real     StartEnd[2],
                        Standard_Real*    Parameter,
                        Standard_Integer* DerivativeRequest,
                        Standard_Real*    Result, // [Dimension]
                        Standard_Integer* ErrorCode);

private:
  Handle(Geom_Surface) CurrentSurface;
  Standard_Real        IsoPar;
};

void Geom_OffsetSurface_VIsoEvaluator::Evaluate(Standard_Integer*, /*Dimension*/
                                                Standard_Real /*StartEnd*/[2],
                                                Standard_Real*    Parameter,
                                                Standard_Integer* DerivativeRequest,
                                                Standard_Real*    Result,
                                                Standard_Integer* ReturnCode)
{
  gp_Pnt P;
  if (*DerivativeRequest == 0)
  {
    P         = CurrentSurface->Value(*Parameter, IsoPar);
    Result[0] = P.X();
    Result[1] = P.Y();
    Result[2] = P.Z();
  }
  else
  {
    gp_Vec DU, DV;
    CurrentSurface->D1(*Parameter, IsoPar, P, DU, DV);
    Result[0] = DU.X();
    Result[1] = DU.Y();
    Result[2] = DU.Z();
  }
  *ReturnCode = 0;
}

//=======================================================================
// function : UIso
// purpose  : The Uiso or the VIso of an OffsetSurface can't be clearly
//           exprimed as a curve from Geom (except some particular cases).
//           So, to extract the U or VIso an Approximation is needed.
//           This approx always will return a BSplineCurve from Geom.
//=======================================================================

Handle(Geom_Curve) Geom_OffsetSurface::UIso(const Standard_Real UU) const
{
  if (equivSurf.IsNull())
  {
    GeomAdaptor_Surface aGAsurf(basisSurf);
    if (aGAsurf.GetType() == GeomAbs_SurfaceOfExtrusion)
    {
      Handle(Geom_Curve) aL = basisSurf->UIso(UU);
      GeomLProp_SLProps  aSurfProps(basisSurf, UU, 0., 2, Precision::Confusion());

      gp_Vec aDir;
      aDir = aSurfProps.Normal();
      aDir *= offsetValue;

      aL->Translate(aDir);
      return aL;
    }
    const Standard_Integer        Num1 = 0, Num2 = 0, Num3 = 1;
    Handle(TColStd_HArray1OfReal) T1, T2, T3               = new TColStd_HArray1OfReal(1, Num3);
    T3->Init(Precision::Approximation());
    Standard_Real U1, U2, V1, V2;
    Bounds(U1, U2, V1, V2);
    const GeomAbs_Shape    Cont   = GeomAbs_C1;
    const Standard_Integer MaxSeg = 100, MaxDeg = 14;

    Handle(Geom_OffsetSurface)       me(this);
    Geom_OffsetSurface_UIsoEvaluator ev(me, UU);
    AdvApprox_ApproxAFunction
      Approx(Num1, Num2, Num3, T1, T2, T3, V1, V2, Cont, MaxDeg, MaxSeg, ev);

    Standard_ConstructionError_Raise_if(!Approx.IsDone(), " Geom_OffsetSurface : UIso");

    const Standard_Integer NbPoles = Approx.NbPoles();

    TColgp_Array1OfPnt      Poles(1, NbPoles);
    TColStd_Array1OfReal    Knots(1, Approx.NbKnots());
    TColStd_Array1OfInteger Mults(1, Approx.NbKnots());

    Approx.Poles(1, Poles);
    Knots = Approx.Knots()->Array1();
    Mults = Approx.Multiplicities()->Array1();

    Handle(Geom_BSplineCurve) C = new Geom_BSplineCurve(Poles, Knots, Mults, Approx.Degree());
    return C;
  }
  else
    return equivSurf->UIso(UU);
}

//=================================================================================================

Handle(Geom_Curve) Geom_OffsetSurface::VIso(const Standard_Real VV) const
{
  if (equivSurf.IsNull())
  {
    const Standard_Integer        Num1 = 0, Num2 = 0, Num3 = 1;
    Handle(TColStd_HArray1OfReal) T1, T2, T3               = new TColStd_HArray1OfReal(1, Num3);
    T3->Init(Precision::Approximation());
    Standard_Real U1, U2, V1, V2;
    Bounds(U1, U2, V1, V2);
    const GeomAbs_Shape    Cont   = GeomAbs_C1;
    const Standard_Integer MaxSeg = 100, MaxDeg = 14;

    Handle(Geom_OffsetSurface)       me(this);
    Geom_OffsetSurface_VIsoEvaluator ev(me, VV);
    AdvApprox_ApproxAFunction
      Approx(Num1, Num2, Num3, T1, T2, T3, U1, U2, Cont, MaxDeg, MaxSeg, ev);

    Standard_ConstructionError_Raise_if(!Approx.IsDone(), " Geom_OffsetSurface : VIso");

    TColgp_Array1OfPnt      Poles(1, Approx.NbPoles());
    TColStd_Array1OfReal    Knots(1, Approx.NbKnots());
    TColStd_Array1OfInteger Mults(1, Approx.NbKnots());

    Approx.Poles(1, Poles);
    Knots = Approx.Knots()->Array1();
    Mults = Approx.Multiplicities()->Array1();

    Handle(Geom_BSplineCurve) C = new Geom_BSplineCurve(Poles, Knots, Mults, Approx.Degree());
    return C;
  }
  else
    return equivSurf->VIso(VV);
}

//=================================================================================================

Standard_Boolean Geom_OffsetSurface::IsCNu(const Standard_Integer N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");
  return basisSurf->IsCNu(N + 1);
}

//=================================================================================================

Standard_Boolean Geom_OffsetSurface::IsCNv(const Standard_Integer N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");
  return basisSurf->IsCNv(N + 1);
}

//=================================================================================================

Standard_Boolean Geom_OffsetSurface::IsUPeriodic() const
{
  return basisSurf->IsUPeriodic();
}

//=================================================================================================

Standard_Real Geom_OffsetSurface::UPeriod() const
{
  return basisSurf->UPeriod();
}

//=================================================================================================

Standard_Boolean Geom_OffsetSurface::IsVPeriodic() const
{
  return basisSurf->IsVPeriodic();
}

//=================================================================================================

Standard_Real Geom_OffsetSurface::VPeriod() const
{
  return basisSurf->VPeriod();
}

//=================================================================================================

Standard_Boolean Geom_OffsetSurface::IsUClosed() const
{
  Standard_Boolean     UClosed;
  Handle(Geom_Surface) SBasis = BasisSurface();

  if (SBasis->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    Handle(Geom_RectangularTrimmedSurface) St =
      Handle(Geom_RectangularTrimmedSurface)::DownCast(SBasis);

    Handle(Geom_Surface) S = St->BasisSurface();
    if (S->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    {
      UClosed = SBasis->IsUClosed();
    }
    else if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
    {
      Handle(Geom_SurfaceOfLinearExtrusion) Extru =
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(S);

      Handle(Geom_Curve) C = Extru->BasisCurve();
      if (C->IsKind(STANDARD_TYPE(Geom_Circle)) || C->IsKind(STANDARD_TYPE(Geom_Ellipse)))
      {
        UClosed = SBasis->IsUClosed();
      }
      else
      {
        UClosed = Standard_False;
      }
    }
    else if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    {
      UClosed = SBasis->IsUClosed();
    }
    else
    {
      UClosed = Standard_False;
    }
  }
  else
  {
    if (SBasis->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    {
      UClosed = SBasis->IsUClosed();
    }
    else if (SBasis->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
    {
      Handle(Geom_SurfaceOfLinearExtrusion) Extru =
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(SBasis);

      Handle(Geom_Curve) C = Extru->BasisCurve();
      UClosed = (C->IsKind(STANDARD_TYPE(Geom_Circle)) || C->IsKind(STANDARD_TYPE(Geom_Ellipse)));
    }
    else if (SBasis->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    {
      UClosed = Standard_True;
    }
    else
    {
      UClosed = Standard_False;
    }
  }
  return UClosed;
}

//=================================================================================================

Standard_Boolean Geom_OffsetSurface::IsVClosed() const
{
  Standard_Boolean     VClosed;
  Handle(Geom_Surface) SBasis = BasisSurface();

  if (SBasis->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    Handle(Geom_RectangularTrimmedSurface) St =
      Handle(Geom_RectangularTrimmedSurface)::DownCast(SBasis);

    Handle(Geom_Surface) S = St->BasisSurface();
    if (S->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    {
      VClosed = SBasis->IsVClosed();
    }
    else
    {
      VClosed = Standard_False;
    }
  }
  else
  {
    if (SBasis->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    {
      VClosed = SBasis->IsVClosed();
    }
    else
    {
      VClosed = Standard_False;
    }
  }
  return VClosed;
}

//=================================================================================================

void Geom_OffsetSurface::Transform(const gp_Trsf& T)
{
  basisSurf->Transform(T);
  offsetValue *= T.ScaleFactor();
  equivSurf.Nullify();
}

//=================================================================================================

void Geom_OffsetSurface::TransformParameters(Standard_Real& U,
                                             Standard_Real& V,
                                             const gp_Trsf& T) const
{
  basisSurf->TransformParameters(U, V, T);
  if (!equivSurf.IsNull())
    equivSurf->TransformParameters(U, V, T);
}

//=================================================================================================

gp_GTrsf2d Geom_OffsetSurface::ParametricTransformation(const gp_Trsf& T) const
{
  return basisSurf->ParametricTransformation(T);
}

//=======================================================================
// function : Surface
// purpose  : Trouve si elle existe, une surface non offset, equivalente
//           a l'offset surface.
//=======================================================================

Handle(Geom_Surface) Geom_OffsetSurface::Surface() const
{
  if (offsetValue == 0.0)
    return basisSurf; // Cas direct

  constexpr Standard_Real Tol = Precision::Confusion();
  Handle(Geom_Surface)    Result, Base;
  Result.Nullify();
  Handle(Standard_Type) TheType = basisSurf->DynamicType();
  Standard_Boolean      IsTrimmed;
  Standard_Real         U1 = 0., V1 = 0., U2 = 0., V2 = 0.;

  // Preambule pour les surface trimmes
  if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    Handle(Geom_RectangularTrimmedSurface) S =
      Handle(Geom_RectangularTrimmedSurface)::DownCast(basisSurf);
    Base    = S->BasisSurface();
    TheType = Base->DynamicType();
    S->Bounds(U1, U2, V1, V2);
    IsTrimmed = Standard_True;
  }
  else
  {
    IsTrimmed = Standard_False;
    Base      = basisSurf;
  }

  // Traite les surfaces cannonique
  if (TheType == STANDARD_TYPE(Geom_Plane))
  {
    Handle(Geom_Plane) P = Handle(Geom_Plane)::DownCast(Base);
    gp_Vec             T = P->Position().XDirection() ^ P->Position().YDirection();
    T *= offsetValue;
    Result = Handle(Geom_Plane)::DownCast(P->Translated(T));
  }
  else if (TheType == STANDARD_TYPE(Geom_CylindricalSurface))
  {
    Handle(Geom_CylindricalSurface) C      = Handle(Geom_CylindricalSurface)::DownCast(Base);
    Standard_Real                   Radius = C->Radius();
    gp_Ax3                          Axis   = C->Position();
    if (Axis.Direct())
      Radius += offsetValue;
    else
      Radius -= offsetValue;
    if (Radius >= Tol)
    {
      Result = new Geom_CylindricalSurface(Axis, Radius);
    }
    else if (Radius <= -Tol)
    {
      Axis.Rotate(gp_Ax1(Axis.Location(), Axis.Direction()), M_PI);
      Result = new Geom_CylindricalSurface(Axis, std::abs(Radius));
      Result->UReverse();
    }
    else
    {
      // surface degeneree
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ConicalSurface))
  {
    Handle(Geom_ConicalSurface) C        = Handle(Geom_ConicalSurface)::DownCast(Base);
    gp_Ax3                      anAxis   = C->Position();
    Standard_Boolean            isDirect = anAxis.Direct();
    Standard_Real               anAlpha  = C->SemiAngle();
    Standard_Real               aRadius;
    if (isDirect)
    {
      aRadius = C->RefRadius() + offsetValue * std::cos(anAlpha);
    }
    else
    {
      aRadius = C->RefRadius() - offsetValue * std::cos(anAlpha);
    }
    if (aRadius >= 0.)
    {
      gp_Vec aZ(anAxis.Direction());
      if (isDirect)
      {
        aZ *= -offsetValue * std::sin(anAlpha);
      }
      else
      {
        aZ *= offsetValue * std::sin(anAlpha);
      }
      anAxis.Translate(aZ);
      Result = new Geom_ConicalSurface(anAxis, anAlpha, aRadius);
    }
    else
    {
      // surface degeneree
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_SphericalSurface))
  {
    Handle(Geom_SphericalSurface) S      = Handle(Geom_SphericalSurface)::DownCast(Base);
    Standard_Real                 Radius = S->Radius();
    gp_Ax3                        Axis   = S->Position();
    if (Axis.Direct())
      Radius += offsetValue;
    else
      Radius -= offsetValue;
    if (Radius >= Tol)
    {
      Result = new Geom_SphericalSurface(Axis, Radius);
    }
    else if (Radius <= -Tol)
    {
      Axis.Rotate(gp_Ax1(Axis.Location(), Axis.Direction()), M_PI);
      Axis.ZReverse();
      Result = new Geom_SphericalSurface(Axis, -Radius);
      Result->UReverse();
    }
    else
    {
      //      surface degeneree
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface))

  {
    Handle(Geom_ToroidalSurface) S           = Handle(Geom_ToroidalSurface)::DownCast(Base);
    Standard_Real                MajorRadius = S->MajorRadius();
    Standard_Real                MinorRadius = S->MinorRadius();
    gp_Ax3                       Axis        = S->Position();
    if (MinorRadius <= MajorRadius)
    {
      if (Axis.Direct())
        MinorRadius += offsetValue;
      else
        MinorRadius -= offsetValue;
      if (MinorRadius >= Tol)
        Result = new Geom_ToroidalSurface(Axis, MajorRadius, MinorRadius);
      //      else if (MinorRadius <= -Tol)
      //        Result->UReverse();
      else
      {
        //	surface degeneree
      }
    }
  }

  // S'il le faut on trimme le resultat
  if (IsTrimmed && !Result.IsNull())
  {
    Base   = Result;
    Result = new Geom_RectangularTrimmedSurface(Base, U1, U2, V1, V2);
  }

  return Result;
}

//=================================================================================================

Standard_Boolean Geom_OffsetSurface::UOsculatingSurface(const Standard_Real          U,
                                                        const Standard_Real          V,
                                                        Standard_Boolean&            t,
                                                        Handle(Geom_BSplineSurface)& L) const
{
  return myOscSurf && myOscSurf->UOscSurf(U, V, t, L);
}

//=================================================================================================

Standard_Boolean Geom_OffsetSurface::VOsculatingSurface(const Standard_Real          U,
                                                        const Standard_Real          V,
                                                        Standard_Boolean&            t,
                                                        Handle(Geom_BSplineSurface)& L) const
{
  return myOscSurf && myOscSurf->VOscSurf(U, V, t, L);
}

//=================================================================================================

void Geom_OffsetSurface::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Surface)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, basisSurf.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, equivSurf.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, offsetValue)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBasisSurfContinuity)
}

//=================================================================================================

void Geom_OffsetSurface::calculateD0(const Standard_Real theU,
                                     const Standard_Real theV,
                                     gp_Pnt&             theValue,
                                     const gp_Vec&       theD1U,
                                     const gp_Vec&       theD1V) const
{
  // Normalize derivatives before normal calculation because it gives more stable result.
  // There will be normalized only derivatives greater than 1.0 to avoid differences in last
  // significant digit
  gp_Vec        aD1U(theD1U);
  gp_Vec        aD1V(theD1V);
  Standard_Real aD1UNorm2 = aD1U.SquareMagnitude();
  Standard_Real aD1VNorm2 = aD1V.SquareMagnitude();
  if (aD1UNorm2 > 1.0)
    aD1U /= std::sqrt(aD1UNorm2);
  if (aD1VNorm2 > 1.0)
    aD1V /= std::sqrt(aD1VNorm2);

  gp_Vec aNorm = aD1U.Crossed(aD1V);
  if (aNorm.SquareMagnitude() > THE_D1_MAG_TOL * THE_D1_MAG_TOL)
  {
    // Non singular case. Simple computations.
    aNorm.Normalize();
    theValue.SetXYZ(theValue.XYZ() + offsetValue * aNorm.XYZ());
  }
  else
  {
    const Standard_Integer MaxOrder = 3;

    Handle(Geom_BSplineSurface) L;
    Standard_Boolean            isOpposite = Standard_False;
    Standard_Boolean            AlongU     = Standard_False;
    Standard_Boolean            AlongV     = Standard_False;
    if (myOscSurf)
    {
      AlongU = myOscSurf->UOscSurf(theU, theV, isOpposite, L);
      AlongV = myOscSurf->VOscSurf(theU, theV, isOpposite, L);
    }
    const Standard_Real aSign = ((AlongV || AlongU) && isOpposite) ? -1. : 1.;

    TColgp_Array2OfVec DerNUV(0, MaxOrder, 0, MaxOrder);
    TColgp_Array2OfVec DerSurf(0, MaxOrder + 1, 0, MaxOrder + 1);
    Standard_Integer   OrderU, OrderV;
    Standard_Real      Umin = 0, Umax = 0, Vmin = 0, Vmax = 0;
    Bounds(Umin, Umax, Vmin, Vmax);

    DerSurf.SetValue(1, 0, theD1U);
    DerSurf.SetValue(0, 1, theD1V);
    computeDerivatives(MaxOrder,
                       1,
                       theU,
                       theV,
                       basisSurf,
                       0,
                       0,
                       AlongU,
                       AlongV,
                       L,
                       DerNUV,
                       DerSurf);

    gp_Dir             Normal;
    CSLib_NormalStatus NStatus = CSLib_Singular;
    CSLib::Normal(MaxOrder,
                  DerNUV,
                  THE_D1_MAG_TOL,
                  theU,
                  theV,
                  Umin,
                  Umax,
                  Vmin,
                  Vmax,
                  NStatus,
                  Normal,
                  OrderU,
                  OrderV);
    if (NStatus == CSLib_InfinityOfSolutions)
    {
      // Replace zero derivative and try to calculate normal
      gp_Vec aNewDU = theD1U;
      gp_Vec aNewDV = theD1V;
      if (replaceDerivative(theU, theV, aNewDU, aNewDV, THE_D1_MAG_TOL * THE_D1_MAG_TOL))
        CSLib::Normal(aNewDU, aNewDV, THE_D1_MAG_TOL, NStatus, Normal);
    }

    if (NStatus != CSLib_Defined)
      throw Geom_UndefinedValue("Geom_OffsetSurface::calculateD0(): Unable to calculate normal");

    theValue.SetXYZ(theValue.XYZ() + offsetValue * aSign * Normal.XYZ());
  }
}

//=================================================================================================

void Geom_OffsetSurface::calculateD1(const Standard_Real theU,
                                     const Standard_Real theV,
                                     gp_Pnt&             theValue,
                                     gp_Vec&             theD1U,
                                     gp_Vec&             theD1V,
                                     const gp_Vec&       theD2U,
                                     const gp_Vec&       theD2V,
                                     const gp_Vec&       theD2UV) const
{
  // Check offset side.
  Handle(Geom_BSplineSurface) L;
  Standard_Boolean            isOpposite = Standard_False;
  Standard_Boolean            AlongU     = Standard_False;
  Standard_Boolean            AlongV     = Standard_False;

  // Normalize derivatives before normal calculation because it gives more stable result.
  // There will be normalized only derivatives greater than 1.0 to avoid differences in last
  // significant digit
  gp_Vec        aD1U(theD1U);
  gp_Vec        aD1V(theD1V);
  Standard_Real aD1UNorm2 = aD1U.SquareMagnitude();
  Standard_Real aD1VNorm2 = aD1V.SquareMagnitude();
  if (aD1UNorm2 > 1.0)
    aD1U /= std::sqrt(aD1UNorm2);
  if (aD1VNorm2 > 1.0)
    aD1V /= std::sqrt(aD1VNorm2);

  Standard_Boolean isSingular = Standard_False;
  Standard_Integer MaxOrder   = 0;
  gp_Vec           aNorm      = aD1U.Crossed(aD1V);
  if (aNorm.SquareMagnitude() <= THE_D1_MAG_TOL * THE_D1_MAG_TOL)
  {
    if (myOscSurf)
    {
      AlongU = myOscSurf->UOscSurf(theU, theV, isOpposite, L);
      AlongV = myOscSurf->VOscSurf(theU, theV, isOpposite, L);
    }

    MaxOrder   = 3;
    isSingular = Standard_True;
  }

  const Standard_Real aSign = ((AlongV || AlongU) && isOpposite) ? -1. : 1.;

  if (!isSingular)
  {
    // AlongU or AlongV leads to more complex D1 computation
    // Try to compute D0 and D1 much simpler
    aNorm.Normalize();
    theValue.SetXYZ(theValue.XYZ() + offsetValue * aSign * aNorm.XYZ());

    gp_Vec        aN0(aNorm.XYZ()), aN1U, aN1V;
    Standard_Real aScale = (theD1U ^ theD1V).Dot(aN0);
    aN1U.SetX(theD2U.Y() * theD1V.Z() + theD1U.Y() * theD2UV.Z() - theD2U.Z() * theD1V.Y()
              - theD1U.Z() * theD2UV.Y());
    aN1U.SetY((theD2U.X() * theD1V.Z() + theD1U.X() * theD2UV.Z() - theD2U.Z() * theD1V.X()
               - theD1U.Z() * theD2UV.X())
              * -1.0);
    aN1U.SetZ(theD2U.X() * theD1V.Y() + theD1U.X() * theD2UV.Y() - theD2U.Y() * theD1V.X()
              - theD1U.Y() * theD2UV.X());
    Standard_Real aScaleU = aN1U.Dot(aN0);
    aN1U.Subtract(aScaleU * aN0);
    aN1U /= aScale;

    aN1V.SetX(theD2UV.Y() * theD1V.Z() + theD2V.Z() * theD1U.Y() - theD2UV.Z() * theD1V.Y()
              - theD2V.Y() * theD1U.Z());
    aN1V.SetY((theD2UV.X() * theD1V.Z() + theD2V.Z() * theD1U.X() - theD2UV.Z() * theD1V.X()
               - theD2V.X() * theD1U.Z())
              * -1.0);
    aN1V.SetZ(theD2UV.X() * theD1V.Y() + theD2V.Y() * theD1U.X() - theD2UV.Y() * theD1V.X()
              - theD2V.X() * theD1U.Y());
    Standard_Real aScaleV = aN1V.Dot(aN0);
    aN1V.Subtract(aScaleV * aN0);
    aN1V /= aScale;

    theD1U += offsetValue * aSign * aN1U;
    theD1V += offsetValue * aSign * aN1V;

    return;
  }

  Standard_Integer   OrderU, OrderV;
  TColgp_Array2OfVec DerNUV(0, MaxOrder + 1, 0, MaxOrder + 1);
  TColgp_Array2OfVec DerSurf(0, MaxOrder + 2, 0, MaxOrder + 2);
  Standard_Real      Umin = 0, Umax = 0, Vmin = 0, Vmax = 0;
  Bounds(Umin, Umax, Vmin, Vmax);

  DerSurf.SetValue(1, 0, theD1U);
  DerSurf.SetValue(0, 1, theD1V);
  DerSurf.SetValue(1, 1, theD2UV);
  DerSurf.SetValue(2, 0, theD2U);
  DerSurf.SetValue(0, 2, theD2V);
  computeDerivatives(MaxOrder, 2, theU, theV, basisSurf, 1, 1, AlongU, AlongV, L, DerNUV, DerSurf);

  gp_Dir             Normal;
  CSLib_NormalStatus NStatus;
  CSLib::Normal(MaxOrder,
                DerNUV,
                THE_D1_MAG_TOL,
                theU,
                theV,
                Umin,
                Umax,
                Vmin,
                Vmax,
                NStatus,
                Normal,
                OrderU,
                OrderV);
  if (NStatus == CSLib_InfinityOfSolutions)
  {
    gp_Vec aNewDU = theD1U;
    gp_Vec aNewDV = theD1V;
    // Replace zero derivative and try to calculate normal
    if (replaceDerivative(theU, theV, aNewDU, aNewDV, THE_D1_MAG_TOL * THE_D1_MAG_TOL))
    {
      DerSurf.SetValue(1, 0, aNewDU);
      DerSurf.SetValue(0, 1, aNewDV);
      computeDerivatives(MaxOrder,
                         2,
                         theU,
                         theV,
                         basisSurf,
                         1,
                         1,
                         AlongU,
                         AlongV,
                         L,
                         DerNUV,
                         DerSurf);
      CSLib::Normal(MaxOrder,
                    DerNUV,
                    THE_D1_MAG_TOL,
                    theU,
                    theV,
                    Umin,
                    Umax,
                    Vmin,
                    Vmax,
                    NStatus,
                    Normal,
                    OrderU,
                    OrderV);
    }
  }

  if (NStatus != CSLib_Defined)
    throw Geom_UndefinedValue("Geom_OffsetSurface::calculateD1(): Unable to calculate normal");

  theValue.SetXYZ(theValue.XYZ() + offsetValue * aSign * Normal.XYZ());

  theD1U = DerSurf(1, 0) + offsetValue * aSign * CSLib::DNNormal(1, 0, DerNUV, OrderU, OrderV);
  theD1V = DerSurf(0, 1) + offsetValue * aSign * CSLib::DNNormal(0, 1, DerNUV, OrderU, OrderV);
}

//=================================================================================================

void Geom_OffsetSurface::calculateD2(const Standard_Real theU,
                                     const Standard_Real theV,
                                     gp_Pnt&             theValue,
                                     gp_Vec&             theD1U,
                                     gp_Vec&             theD1V,
                                     gp_Vec&             theD2U,
                                     gp_Vec&             theD2V,
                                     gp_Vec&             theD2UV,
                                     const gp_Vec&       theD3U,
                                     const gp_Vec&       theD3V,
                                     const gp_Vec&       theD3UUV,
                                     const gp_Vec&       theD3UVV) const
{
  gp_Dir             Normal;
  CSLib_NormalStatus NStatus;
  CSLib::Normal(theD1U, theD1V, THE_D1_MAG_TOL, NStatus, Normal);

  const Standard_Integer MaxOrder = (NStatus == CSLib_Defined) ? 0 : 3;
  Standard_Integer       OrderU, OrderV;
  TColgp_Array2OfVec     DerNUV(0, MaxOrder + 2, 0, MaxOrder + 2);
  TColgp_Array2OfVec     DerSurf(0, MaxOrder + 3, 0, MaxOrder + 3);

  Standard_Real Umin = 0, Umax = 0, Vmin = 0, Vmax = 0;
  Bounds(Umin, Umax, Vmin, Vmax);

  DerSurf.SetValue(1, 0, theD1U);
  DerSurf.SetValue(0, 1, theD1V);
  DerSurf.SetValue(1, 1, theD2UV);
  DerSurf.SetValue(2, 0, theD2U);
  DerSurf.SetValue(0, 2, theD2V);
  DerSurf.SetValue(3, 0, theD3U);
  DerSurf.SetValue(2, 1, theD3UUV);
  DerSurf.SetValue(1, 2, theD3UVV);
  DerSurf.SetValue(0, 3, theD3V);

  Handle(Geom_BSplineSurface) L;
  Standard_Boolean            isOpposite = Standard_False;
  Standard_Boolean            AlongU     = Standard_False;
  Standard_Boolean            AlongV     = Standard_False;
  if ((NStatus != CSLib_Defined) && myOscSurf)
  {
    AlongU = myOscSurf->UOscSurf(theU, theV, isOpposite, L);
    AlongV = myOscSurf->VOscSurf(theU, theV, isOpposite, L);
  }
  const Standard_Real aSign = ((AlongV || AlongU) && isOpposite) ? -1. : 1.;

  computeDerivatives(MaxOrder, 3, theU, theV, basisSurf, 2, 2, AlongU, AlongV, L, DerNUV, DerSurf);

  CSLib::Normal(MaxOrder,
                DerNUV,
                THE_D1_MAG_TOL,
                theU,
                theV,
                Umin,
                Umax,
                Vmin,
                Vmax,
                NStatus,
                Normal,
                OrderU,
                OrderV);
  if (NStatus != CSLib_Defined)
    throw Geom_UndefinedValue("Geom_OffsetSurface::calculateD2(): Unable to calculate normal");

  theValue.SetXYZ(theValue.XYZ() + offsetValue * aSign * Normal.XYZ());

  theD1U = DerSurf(1, 0) + offsetValue * aSign * CSLib::DNNormal(1, 0, DerNUV, OrderU, OrderV);
  theD1V = DerSurf(0, 1) + offsetValue * aSign * CSLib::DNNormal(0, 1, DerNUV, OrderU, OrderV);

  theD2U  = basisSurf->DN(theU, theV, 2, 0);
  theD2V  = basisSurf->DN(theU, theV, 0, 2);
  theD2UV = basisSurf->DN(theU, theV, 1, 1);

  theD2U += aSign * offsetValue * CSLib::DNNormal(2, 0, DerNUV, OrderU, OrderV);
  theD2V += aSign * offsetValue * CSLib::DNNormal(0, 2, DerNUV, OrderU, OrderV);
  theD2UV += aSign * offsetValue * CSLib::DNNormal(1, 1, DerNUV, OrderU, OrderV);
}

//=================================================================================================

void Geom_OffsetSurface::calculateD3(const Standard_Real theU,
                                     const Standard_Real theV,
                                     gp_Pnt&             theValue,
                                     gp_Vec&             theD1U,
                                     gp_Vec&             theD1V,
                                     gp_Vec&             theD2U,
                                     gp_Vec&             theD2V,
                                     gp_Vec&             theD2UV,
                                     gp_Vec&             theD3U,
                                     gp_Vec&             theD3V,
                                     gp_Vec&             theD3UUV,
                                     gp_Vec&             theD3UVV) const
{
  gp_Dir             Normal;
  CSLib_NormalStatus NStatus;
  CSLib::Normal(theD1U, theD1V, THE_D1_MAG_TOL, NStatus, Normal);
  const Standard_Integer MaxOrder = (NStatus == CSLib_Defined) ? 0 : 3;
  Standard_Integer       OrderU, OrderV;
  TColgp_Array2OfVec     DerNUV(0, MaxOrder + 3, 0, MaxOrder + 3);
  TColgp_Array2OfVec     DerSurf(0, MaxOrder + 4, 0, MaxOrder + 4);
  Standard_Real          Umin = 0, Umax = 0, Vmin = 0, Vmax = 0;
  Bounds(Umin, Umax, Vmin, Vmax);

  DerSurf.SetValue(1, 0, theD1U);
  DerSurf.SetValue(0, 1, theD1V);
  DerSurf.SetValue(1, 1, theD2UV);
  DerSurf.SetValue(2, 0, theD2U);
  DerSurf.SetValue(0, 2, theD2V);
  DerSurf.SetValue(3, 0, theD3U);
  DerSurf.SetValue(2, 1, theD3UUV);
  DerSurf.SetValue(1, 2, theD3UVV);
  DerSurf.SetValue(0, 3, theD3V);

  Handle(Geom_BSplineSurface) L;
  Standard_Boolean            isOpposite = Standard_False;
  Standard_Boolean            AlongU     = Standard_False;
  Standard_Boolean            AlongV     = Standard_False;
  if ((NStatus != CSLib_Defined) && myOscSurf)
  {
    AlongU = myOscSurf->UOscSurf(theU, theV, isOpposite, L);
    AlongV = myOscSurf->VOscSurf(theU, theV, isOpposite, L);
  }
  const Standard_Real aSign = ((AlongV || AlongU) && isOpposite) ? -1. : 1.;

  computeDerivatives(MaxOrder, 3, theU, theV, basisSurf, 3, 3, AlongU, AlongV, L, DerNUV, DerSurf);

  CSLib::Normal(MaxOrder,
                DerNUV,
                THE_D1_MAG_TOL,
                theU,
                theV,
                Umin,
                Umax,
                Vmin,
                Vmax,
                NStatus,
                Normal,
                OrderU,
                OrderV);
  if (NStatus != CSLib_Defined)
    throw Geom_UndefinedValue("Geom_OffsetSurface::calculateD3(): Unable to calculate normal");

  theValue.SetXYZ(theValue.XYZ() + offsetValue * aSign * Normal.XYZ());

  theD1U = DerSurf(1, 0) + offsetValue * aSign * CSLib::DNNormal(1, 0, DerNUV, OrderU, OrderV);
  theD1V = DerSurf(0, 1) + offsetValue * aSign * CSLib::DNNormal(0, 1, DerNUV, OrderU, OrderV);

  theD2U   = basisSurf->DN(theU, theV, 2, 0);
  theD2V   = basisSurf->DN(theU, theV, 0, 2);
  theD2UV  = basisSurf->DN(theU, theV, 1, 1);
  theD3U   = basisSurf->DN(theU, theV, 3, 0);
  theD3V   = basisSurf->DN(theU, theV, 0, 3);
  theD3UUV = basisSurf->DN(theU, theV, 2, 1);
  theD3UVV = basisSurf->DN(theU, theV, 1, 2);

  theD2U += aSign * offsetValue * CSLib::DNNormal(2, 0, DerNUV, OrderU, OrderV);
  theD2V += aSign * offsetValue * CSLib::DNNormal(0, 2, DerNUV, OrderU, OrderV);
  theD2UV += aSign * offsetValue * CSLib::DNNormal(1, 1, DerNUV, OrderU, OrderV);
  theD3U += aSign * offsetValue * CSLib::DNNormal(3, 0, DerNUV, OrderU, OrderV);
  theD3V += aSign * offsetValue * CSLib::DNNormal(0, 3, DerNUV, OrderU, OrderV);
  theD3UUV += aSign * offsetValue * CSLib::DNNormal(2, 1, DerNUV, OrderU, OrderV);
  theD3UVV += aSign * offsetValue * CSLib::DNNormal(1, 2, DerNUV, OrderU, OrderV);
}

//=================================================================================================

gp_Vec Geom_OffsetSurface::calculateDN(const Standard_Real    theU,
                                       const Standard_Real    theV,
                                       const Standard_Integer theNu,
                                       const Standard_Integer theNv,
                                       const gp_Vec&          theD1U,
                                       const gp_Vec&          theD1V) const
{
  gp_Dir             Normal;
  CSLib_NormalStatus NStatus;
  CSLib::Normal(theD1U, theD1V, THE_D1_MAG_TOL, NStatus, Normal);
  const Standard_Integer MaxOrder = (NStatus == CSLib_Defined) ? 0 : 3;
  Standard_Integer       OrderU, OrderV;
  TColgp_Array2OfVec     DerNUV(0, MaxOrder + theNu, 0, MaxOrder + theNv);
  TColgp_Array2OfVec     DerSurf(0, MaxOrder + theNu + 1, 0, MaxOrder + theNv + 1);

  Standard_Real Umin = 0, Umax = 0, Vmin = 0, Vmax = 0;
  Bounds(Umin, Umax, Vmin, Vmax);

  DerSurf.SetValue(1, 0, theD1U);
  DerSurf.SetValue(0, 1, theD1V);

  Handle(Geom_BSplineSurface) L;
  //   Is there any osculatingsurface along U or V;
  Standard_Boolean isOpposite = Standard_False;
  Standard_Boolean AlongU     = Standard_False;
  Standard_Boolean AlongV     = Standard_False;
  if ((NStatus != CSLib_Defined) && myOscSurf)
  {
    AlongU = myOscSurf->UOscSurf(theU, theV, isOpposite, L);
    AlongV = myOscSurf->VOscSurf(theU, theV, isOpposite, L);
  }
  const Standard_Real aSign = ((AlongV || AlongU) && isOpposite) ? -1. : 1.;

  computeDerivatives(MaxOrder,
                     1,
                     theU,
                     theV,
                     basisSurf,
                     theNu,
                     theNv,
                     AlongU,
                     AlongV,
                     L,
                     DerNUV,
                     DerSurf);

  CSLib::Normal(MaxOrder,
                DerNUV,
                THE_D1_MAG_TOL,
                theU,
                theV,
                Umin,
                Umax,
                Vmin,
                Vmax,
                NStatus,
                Normal,
                OrderU,
                OrderV);
  if (NStatus != CSLib_Defined)
    throw Geom_UndefinedValue("Geom_OffsetSurface::calculateDN(): Unable to calculate normal");

  gp_Vec D = basisSurf->DN(theU, theV, theNu, theNv);
  D += aSign * offsetValue * CSLib::DNNormal(theNu, theNv, DerNUV, OrderU, OrderV);
  return D;
}

//=================================================================================================

Standard_Boolean Geom_OffsetSurface::replaceDerivative(const Standard_Real theU,
                                                       const Standard_Real theV,
                                                       gp_Vec&             theDU,
                                                       gp_Vec&             theDV,
                                                       const Standard_Real theSquareTol) const
{
  Standard_Boolean isReplaceDU = theDU.SquareMagnitude() < theSquareTol;
  Standard_Boolean isReplaceDV = theDV.SquareMagnitude() < theSquareTol;
  Standard_Boolean isReplaced  = Standard_False;
  if (isReplaceDU ^ isReplaceDV)
  {
    Standard_Real aU    = theU;
    Standard_Real aV    = theV;
    Standard_Real aUMin = 0, aUMax = 0, aVMin = 0, aVMax = 0;
    Bounds(aUMin, aUMax, aVMin, aVMax);

    // Calculate step along non-zero derivative
    Standard_Real aStep;
    if (isReplaceDV)
    {
      aStep = Precision::Confusion() * theDU.Magnitude();
      if (aStep > aUMax - aUMin)
        aStep = (aUMax - aUMin) / 100.;
    }
    else
    {
      aStep = Precision::Confusion() * theDV.Magnitude();
      if (aStep > aVMax - aVMin)
        aStep = (aVMax - aVMin) / 100.;
    }

    gp_Pnt aP;
    gp_Vec aDU, aDV;
    // Step away from current parametric coordinates and calculate derivatives once again.
    // Replace zero derivative by the obtained.
    for (Standard_Real aStepSign = -1.0; aStepSign <= 1.0 && !isReplaced; aStepSign += 2.0)
    {
      if (isReplaceDV)
      {
        aU = theU + aStepSign * aStep;
        if (aU < aUMin || aU > aUMax)
          continue;
      }
      else
      {
        aV = theV + aStepSign * aStep;
        if (aV < aVMin || aV > aVMax)
          continue;
      }

      basisSurf->D1(aU, aV, aP, aDU, aDV);

      if (isReplaceDU && aDU.SquareMagnitude() > theSquareTol)
      {
        theDU      = aDU;
        isReplaced = Standard_True;
      }
      if (isReplaceDV && aDV.SquareMagnitude() > theSquareTol)
      {
        theDV      = aDV;
        isReplaced = Standard_True;
      }
    }
  }
  return isReplaced;
}
