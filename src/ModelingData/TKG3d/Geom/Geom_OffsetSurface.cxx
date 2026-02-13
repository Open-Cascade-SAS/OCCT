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
#include "Geom_OffsetSurfaceUtils.pxx"
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
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_OffsetSurface, Geom_Surface)

static const double MyAngularToleranceForG1 = Precision::Angular();

//=================================================================================================

occ::handle<Geom_Geometry> Geom_OffsetSurface::Copy() const
{
  return new Geom_OffsetSurface(*this);
}

//=======================================================================
// function : Geom_OffsetSurface
// purpose  : Basis surface cannot be an Offset surface or trimmed from
//            offset surface.
//=======================================================================

Geom_OffsetSurface::Geom_OffsetSurface(const Geom_OffsetSurface& theOther)
    : basisSurf(occ::down_cast<Geom_Surface>(theOther.basisSurf->Copy())),
      equivSurf(theOther.equivSurf.IsNull()
                  ? occ::handle<Geom_Surface>()
                  : occ::down_cast<Geom_Surface>(theOther.equivSurf->Copy())),
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

Geom_OffsetSurface::Geom_OffsetSurface(const occ::handle<Geom_Surface>& theSurf,
                                       const double                     theOffset,
                                       const bool                       isNotCheckC0)
    : offsetValue(theOffset)
{
  SetBasisSurface(theSurf, isNotCheckC0);
}

//=================================================================================================

void Geom_OffsetSurface::SetBasisSurface(const occ::handle<Geom_Surface>& S,
                                         const bool                       isNotCheckC0)
{
  double aUf, aUl, aVf, aVl;
  S->Bounds(aUf, aUl, aVf, aVl);

  occ::handle<Geom_Surface> aCheckingSurf = occ::down_cast<Geom_Surface>(S->Copy());
  bool                      isTrimmed     = false;

  while (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))
         || aCheckingSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
  {
    if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      occ::handle<Geom_RectangularTrimmedSurface> aTrimS =
        occ::down_cast<Geom_RectangularTrimmedSurface>(aCheckingSurf);
      aCheckingSurf = aTrimS->BasisSurface();
      isTrimmed     = true;
    }

    if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    {
      occ::handle<Geom_OffsetSurface> aOS = occ::down_cast<Geom_OffsetSurface>(aCheckingSurf);
      aCheckingSurf                       = aOS->BasisSurface();
      offsetValue += aOS->Offset();
    }
  }

  myBasisSurfContinuity = aCheckingSurf->Continuity();

  bool isC0 = !isNotCheckC0 && (myBasisSurfContinuity == GeomAbs_C0);

  // Basis surface must be at least C1
  if (isC0)
  {
    occ::handle<Geom_Curve> aCurve;

    if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    {
      occ::handle<Geom_SurfaceOfRevolution> aRevSurf =
        occ::down_cast<Geom_SurfaceOfRevolution>(aCheckingSurf);
      aCurve = aRevSurf->BasisCurve();
    }
    else if (aCheckingSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
    {
      occ::handle<Geom_SurfaceOfLinearExtrusion> aLESurf =
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(aCheckingSurf);
      aCurve = aLESurf->BasisCurve();
    }

    if (!aCurve.IsNull())
    {
      while (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))
             || aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
      {
        if (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
        {
          occ::handle<Geom_TrimmedCurve> aTrimC = occ::down_cast<Geom_TrimmedCurve>(aCurve);
          aCurve                                = aTrimC->BasisCurve();
        }

        if (aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
        {
          occ::handle<Geom_OffsetCurve> aOC = occ::down_cast<Geom_OffsetCurve>(aCurve);
          aCurve                            = aOC->BasisCurve();
        }
      }
    }

    const double aUIsoPar = (aUf + aUl) / 2.0, aVIsoPar = (aVf + aVl) / 2.0;
    bool         isUG1 = false, isVG1 = false;

    const occ::handle<Geom_Curve> aCurv1 = aCurve.IsNull() ? aCheckingSurf->UIso(aUIsoPar) : aCurve;
    const occ::handle<Geom_Curve> aCurv2 = aCheckingSurf->VIso(aVIsoPar);
    isUG1                                = !aCurv1->IsKind(STANDARD_TYPE(Geom_BSplineCurve));
    isVG1                                = !aCurv2->IsKind(STANDARD_TYPE(Geom_BSplineCurve));

    if (!isUG1)
    {
      occ::handle<Geom_BSplineCurve> aBC = occ::down_cast<Geom_BSplineCurve>(aCurv1);
      isUG1                              = aBC->IsG1(aVf, aVl, MyAngularToleranceForG1);
    }
    //
    if (!isVG1)
    {
      occ::handle<Geom_BSplineCurve> aBC = occ::down_cast<Geom_BSplineCurve>(aCurv2);
      isVG1                              = aBC->IsG1(aUf, aUl, MyAngularToleranceForG1);
    }
    //
    if (isUG1 && isVG1)
    {
      myBasisSurfContinuity = GeomAbs_G1;
      isC0                  = false;
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
    constexpr double Tol = Precision::Confusion(); // 0.0001;
    myOscSurf            = std::make_unique<Geom_OsculatingSurface>(aCheckingSurf, Tol);
  }
}

//=================================================================================================

void Geom_OffsetSurface::SetOffsetValue(const double D)
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

double Geom_OffsetSurface::UReversedParameter(const double U) const
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

double Geom_OffsetSurface::VReversedParameter(const double V) const
{
  return basisSurf->VReversedParameter(V);
}

//=================================================================================================

void Geom_OffsetSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
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

std::optional<gp_Pnt> Geom_OffsetSurface::EvalD0(const double U, const double V) const
{
#ifdef CHECK
  if (myBasisSurfContinuity == GeomAbs_C0)
  {
    return std::nullopt;
  }
#endif
  if (!equivSurf.IsNull())
  {
    return equivSurf->EvalD0(U, V);
  }

  gp_Pnt aP;
  if (!Geom_OffsetSurfaceUtils::EvaluateD0(U,
                                            V,
                                            basisSurf.get(),
                                            offsetValue,
                                            myOscSurf.get(),
                                            aP))
  {
    return std::nullopt;
  }
  return aP;
}

//=================================================================================================

std::optional<Geom_SurfD1> Geom_OffsetSurface::EvalD1(const double U, const double V) const
{
#ifdef CHECK
  if (myBasisSurfContinuity == GeomAbs_C0 || myBasisSurfContinuity == GeomAbs_C1)
  {
    return std::nullopt;
  }
#endif
  if (!equivSurf.IsNull())
  {
    return equivSurf->EvalD1(U, V);
  }

  Geom_SurfD1 aResult;
  if (!Geom_OffsetSurfaceUtils::EvaluateD1(U,
                                            V,
                                            basisSurf.get(),
                                            offsetValue,
                                            myOscSurf.get(),
                                            aResult.Point,
                                            aResult.D1U,
                                            aResult.D1V))
  {
    return std::nullopt;
  }
  return aResult;
}

//=================================================================================================

std::optional<Geom_SurfD2> Geom_OffsetSurface::EvalD2(const double U, const double V) const
{
#ifdef CHECK
  if (myBasisSurfContinuity == GeomAbs_C0 || myBasisSurfContinuity == GeomAbs_C1
      || myBasisSurfContinuity == GeomAbs_C2)
  {
    return std::nullopt;
  }
#endif
  if (!equivSurf.IsNull())
  {
    return equivSurf->EvalD2(U, V);
  }

  Geom_SurfD2 aResult;
  if (!Geom_OffsetSurfaceUtils::EvaluateD2(U,
                                            V,
                                            basisSurf.get(),
                                            offsetValue,
                                            myOscSurf.get(),
                                            aResult.Point,
                                            aResult.D1U,
                                            aResult.D1V,
                                            aResult.D2U,
                                            aResult.D2V,
                                            aResult.D2UV))
  {
    return std::nullopt;
  }
  return aResult;
}

//=================================================================================================

std::optional<Geom_SurfD3> Geom_OffsetSurface::EvalD3(const double U, const double V) const
{
#ifdef CHECK
  if (!(basisSurf->IsCNu(4) && basisSurf->IsCNv(4)))
  {
    return std::nullopt;
  }
#endif
  if (!equivSurf.IsNull())
  {
    return equivSurf->EvalD3(U, V);
  }

  Geom_SurfD3 aResult;
  if (!Geom_OffsetSurfaceUtils::EvaluateD3(U,
                                            V,
                                            basisSurf.get(),
                                            offsetValue,
                                            myOscSurf.get(),
                                            aResult.Point,
                                            aResult.D1U,
                                            aResult.D1V,
                                            aResult.D2U,
                                            aResult.D2V,
                                            aResult.D2UV,
                                            aResult.D3U,
                                            aResult.D3V,
                                            aResult.D3UUV,
                                            aResult.D3UVV))
  {
    return std::nullopt;
  }
  return aResult;
}

//=================================================================================================

std::optional<gp_Vec> Geom_OffsetSurface::EvalDN(const double U,
                                                 const double V,
                                                 const int    Nu,
                                                 const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
    return std::nullopt;
#ifdef CHECK
  if (!(basisSurf->IsCNu(Nu) && basisSurf->IsCNv(Nv)))
  {
    return std::nullopt;
  }
#endif
  if (!equivSurf.IsNull())
  {
    return equivSurf->EvalDN(U, V, Nu, Nv);
  }

  gp_Vec aResult;
  if (!Geom_OffsetSurfaceUtils::EvaluateDN(U,
                                            V,
                                            Nu,
                                            Nv,
                                            basisSurf.get(),
                                            offsetValue,
                                            myOscSurf.get(),
                                            aResult))
  {
    return std::nullopt;
  }
  return aResult;
}

////*************************************************
////
////   EVALUATOR FOR THE ISO-CURVE APPROXIMATION
////
////*************************************************

class Geom_OffsetSurface_UIsoEvaluator : public AdvApprox_EvaluatorFunction
{
public:
  Geom_OffsetSurface_UIsoEvaluator(const occ::handle<Geom_Surface>& theSurface, const double theU)
      : CurrentSurface(theSurface),
        IsoPar(theU)
  {
  }

  void Evaluate(int*    Dimension,
                double  StartEnd[2],
                double* Parameter,
                int*    DerivativeRequest,
                double* Result, // [Dimension]
                int*    ErrorCode) override;

private:
  GeomAdaptor_Surface CurrentSurface;
  double              IsoPar;
};

void Geom_OffsetSurface_UIsoEvaluator::Evaluate(int*, /*Dimension*/
                                                double /*StartEnd*/[2],
                                                double* Parameter,
                                                int*    DerivativeRequest,
                                                double* Result,
                                                int*    ReturnCode)
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
  Geom_OffsetSurface_VIsoEvaluator(const occ::handle<Geom_Surface>& theSurface, const double theV)
      : CurrentSurface(theSurface),
        IsoPar(theV)
  {
  }

  void Evaluate(int*    Dimension,
                double  StartEnd[2],
                double* Parameter,
                int*    DerivativeRequest,
                double* Result, // [Dimension]
                int*    ErrorCode) override;

private:
  occ::handle<Geom_Surface> CurrentSurface;
  double                    IsoPar;
};

void Geom_OffsetSurface_VIsoEvaluator::Evaluate(int*, /*Dimension*/
                                                double /*StartEnd*/[2],
                                                double* Parameter,
                                                int*    DerivativeRequest,
                                                double* Result,
                                                int*    ReturnCode)
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

occ::handle<Geom_Curve> Geom_OffsetSurface::UIso(const double UU) const
{
  if (equivSurf.IsNull())
  {
    GeomAdaptor_Surface aGAsurf(basisSurf);
    if (aGAsurf.GetType() == GeomAbs_SurfaceOfExtrusion)
    {
      occ::handle<Geom_Curve> aL = basisSurf->UIso(UU);
      GeomLProp_SLProps       aSurfProps(basisSurf, UU, 0., 2, Precision::Confusion());

      gp_Vec aDir;
      aDir = aSurfProps.Normal();
      aDir *= offsetValue;

      aL->Translate(aDir);
      return aL;
    }
    const int                                Num1 = 0, Num2 = 0, Num3 = 1;
    occ::handle<NCollection_HArray1<double>> T1, T2, T3 = new NCollection_HArray1<double>(1, Num3);
    T3->Init(Precision::Approximation());
    double U1, U2, V1, V2;
    Bounds(U1, U2, V1, V2);
    const GeomAbs_Shape Cont   = GeomAbs_C1;
    const int           MaxSeg = 100, MaxDeg = 14;

    occ::handle<Geom_OffsetSurface>  me(this);
    Geom_OffsetSurface_UIsoEvaluator ev(me, UU);
    AdvApprox_ApproxAFunction
      Approx(Num1, Num2, Num3, T1, T2, T3, V1, V2, Cont, MaxDeg, MaxSeg, ev);

    Standard_ConstructionError_Raise_if(!Approx.IsDone(), " Geom_OffsetSurface : UIso");

    const int NbPoles = Approx.NbPoles();

    NCollection_Array1<gp_Pnt> Poles(1, NbPoles);
    NCollection_Array1<double> Knots(1, Approx.NbKnots());
    NCollection_Array1<int>    Mults(1, Approx.NbKnots());

    Approx.Poles(1, Poles);
    Knots = Approx.Knots()->Array1();
    Mults = Approx.Multiplicities()->Array1();

    occ::handle<Geom_BSplineCurve> C = new Geom_BSplineCurve(Poles, Knots, Mults, Approx.Degree());
    return C;
  }
  else
    return equivSurf->UIso(UU);
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_OffsetSurface::VIso(const double VV) const
{
  if (equivSurf.IsNull())
  {
    const int                                Num1 = 0, Num2 = 0, Num3 = 1;
    occ::handle<NCollection_HArray1<double>> T1, T2, T3 = new NCollection_HArray1<double>(1, Num3);
    T3->Init(Precision::Approximation());
    double U1, U2, V1, V2;
    Bounds(U1, U2, V1, V2);
    const GeomAbs_Shape Cont   = GeomAbs_C1;
    const int           MaxSeg = 100, MaxDeg = 14;

    occ::handle<Geom_OffsetSurface>  me(this);
    Geom_OffsetSurface_VIsoEvaluator ev(me, VV);
    AdvApprox_ApproxAFunction
      Approx(Num1, Num2, Num3, T1, T2, T3, U1, U2, Cont, MaxDeg, MaxSeg, ev);

    Standard_ConstructionError_Raise_if(!Approx.IsDone(), " Geom_OffsetSurface : VIso");

    NCollection_Array1<gp_Pnt> Poles(1, Approx.NbPoles());
    NCollection_Array1<double> Knots(1, Approx.NbKnots());
    NCollection_Array1<int>    Mults(1, Approx.NbKnots());

    Approx.Poles(1, Poles);
    Knots = Approx.Knots()->Array1();
    Mults = Approx.Multiplicities()->Array1();

    occ::handle<Geom_BSplineCurve> C = new Geom_BSplineCurve(Poles, Knots, Mults, Approx.Degree());
    return C;
  }
  else
    return equivSurf->VIso(VV);
}

//=================================================================================================

bool Geom_OffsetSurface::IsCNu(const int N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");
  return basisSurf->IsCNu(N + 1);
}

//=================================================================================================

bool Geom_OffsetSurface::IsCNv(const int N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");
  return basisSurf->IsCNv(N + 1);
}

//=================================================================================================

bool Geom_OffsetSurface::IsUPeriodic() const
{
  return basisSurf->IsUPeriodic();
}

//=================================================================================================

double Geom_OffsetSurface::UPeriod() const
{
  return basisSurf->UPeriod();
}

//=================================================================================================

bool Geom_OffsetSurface::IsVPeriodic() const
{
  return basisSurf->IsVPeriodic();
}

//=================================================================================================

double Geom_OffsetSurface::VPeriod() const
{
  return basisSurf->VPeriod();
}

//=================================================================================================

bool Geom_OffsetSurface::IsUClosed() const
{
  bool                      UClosed;
  occ::handle<Geom_Surface> SBasis = BasisSurface();

  if (SBasis->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> St =
      occ::down_cast<Geom_RectangularTrimmedSurface>(SBasis);

    occ::handle<Geom_Surface> S = St->BasisSurface();
    if (S->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    {
      UClosed = SBasis->IsUClosed();
    }
    else if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
    {
      occ::handle<Geom_SurfaceOfLinearExtrusion> Extru =
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(S);

      occ::handle<Geom_Curve> C = Extru->BasisCurve();
      if (C->IsKind(STANDARD_TYPE(Geom_Circle)) || C->IsKind(STANDARD_TYPE(Geom_Ellipse)))
      {
        UClosed = SBasis->IsUClosed();
      }
      else
      {
        UClosed = false;
      }
    }
    else if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    {
      UClosed = SBasis->IsUClosed();
    }
    else
    {
      UClosed = false;
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
      occ::handle<Geom_SurfaceOfLinearExtrusion> Extru =
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(SBasis);

      occ::handle<Geom_Curve> C = Extru->BasisCurve();
      UClosed = (C->IsKind(STANDARD_TYPE(Geom_Circle)) || C->IsKind(STANDARD_TYPE(Geom_Ellipse)));
    }
    else if (SBasis->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    {
      UClosed = true;
    }
    else
    {
      UClosed = false;
    }
  }
  return UClosed;
}

//=================================================================================================

bool Geom_OffsetSurface::IsVClosed() const
{
  bool                      VClosed;
  occ::handle<Geom_Surface> SBasis = BasisSurface();

  if (SBasis->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> St =
      occ::down_cast<Geom_RectangularTrimmedSurface>(SBasis);

    occ::handle<Geom_Surface> S = St->BasisSurface();
    if (S->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    {
      VClosed = SBasis->IsVClosed();
    }
    else
    {
      VClosed = false;
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
      VClosed = false;
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

void Geom_OffsetSurface::TransformParameters(double& U, double& V, const gp_Trsf& T) const
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

//==================================================================================================

occ::handle<Geom_Surface> Geom_OffsetSurface::Surface() const
{
  // Returns an equivalent non-offset surface if it exists for this offset surface.
  if (offsetValue == 0.0)
    return basisSurf; // Direct case - no offset

  constexpr double          Tol = Precision::Confusion();
  occ::handle<Geom_Surface> Result, Base;
  Result.Nullify();
  occ::handle<Standard_Type> TheType = basisSurf->DynamicType();
  bool                       IsTrimmed;
  double                     U1 = 0., V1 = 0., U2 = 0., V2 = 0.;

  // Handle trimmed surfaces - extract the basis surface and bounds.
  if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    occ::handle<Geom_RectangularTrimmedSurface> S =
      occ::down_cast<Geom_RectangularTrimmedSurface>(basisSurf);
    Base    = S->BasisSurface();
    TheType = Base->DynamicType();
    S->Bounds(U1, U2, V1, V2);
    IsTrimmed = true;
  }
  else
  {
    IsTrimmed = false;
    Base      = basisSurf;
  }

  // Handle canonical surfaces - compute equivalent offset surface.
  // For direct orientation, offset is along outward normal; for indirect, it's reversed.
  if (TheType == STANDARD_TYPE(Geom_Plane))
  {
    occ::handle<Geom_Plane> P = occ::down_cast<Geom_Plane>(Base);
    // Plane normal is already available as Position().Direction().
    gp_Vec T(P->Position().Direction());
    T *= offsetValue;
    Result = occ::down_cast<Geom_Plane>(P->Translated(T));
  }
  else if (TheType == STANDARD_TYPE(Geom_CylindricalSurface))
  {
    occ::handle<Geom_CylindricalSurface> C      = occ::down_cast<Geom_CylindricalSurface>(Base);
    gp_Ax3                               Axis   = C->Position();
    const double                         aSign  = Axis.Direct() ? 1.0 : -1.0;
    const double                         Radius = C->Radius() + aSign * offsetValue;
    if (Radius >= Tol)
    {
      Result = new Geom_CylindricalSurface(Axis, Radius);
    }
    else if (Radius <= -Tol)
    {
      // Negative radius: flip X-axis to reverse normal orientation.
      Axis.XReverse();
      Result = new Geom_CylindricalSurface(Axis, -Radius);
    }
    // else: degenerate surface - radius is too small.
  }
  else if (TheType == STANDARD_TYPE(Geom_ConicalSurface))
  {
    occ::handle<Geom_ConicalSurface> C       = occ::down_cast<Geom_ConicalSurface>(Base);
    gp_Ax3                           anAxis  = C->Position();
    const double                     aSign   = anAxis.Direct() ? 1.0 : -1.0;
    const double                     anAlpha = C->SemiAngle();
    const double                     aCos    = std::cos(anAlpha);
    const double                     aSin    = std::sin(anAlpha);
    const double                     aRadius = C->RefRadius() + aSign * offsetValue * aCos;
    if (aRadius >= 0.)
    {
      // Translate apex along axis by offset component.
      gp_Vec aZ(anAxis.Direction());
      aZ *= -aSign * offsetValue * aSin;
      anAxis.Translate(aZ);
      Result = new Geom_ConicalSurface(anAxis, anAlpha, aRadius);
    }
    // else: degenerate surface - radius is negative.
  }
  else if (TheType == STANDARD_TYPE(Geom_SphericalSurface))
  {
    occ::handle<Geom_SphericalSurface> S      = occ::down_cast<Geom_SphericalSurface>(Base);
    gp_Ax3                             Axis   = S->Position();
    const double                       aSign  = Axis.Direct() ? 1.0 : -1.0;
    const double                       Radius = S->Radius() + aSign * offsetValue;
    if (Radius >= Tol)
    {
      Result = new Geom_SphericalSurface(Axis, Radius);
    }
    else if (Radius <= -Tol)
    {
      // Negative radius: flip both X and Z axes to reverse normal orientation.
      Axis.XReverse();
      Axis.ZReverse();
      Result = new Geom_SphericalSurface(Axis, -Radius);
    }
    // else: degenerate surface - radius is too small.
  }
  else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface))
  {
    occ::handle<Geom_ToroidalSurface> S           = occ::down_cast<Geom_ToroidalSurface>(Base);
    const double                      MajorRadius = S->MajorRadius();
    gp_Ax3                            Axis        = S->Position();
    const double                      aSign       = Axis.Direct() ? 1.0 : -1.0;
    const double                      MinorRadius = S->MinorRadius() + aSign * offsetValue;
    // Only handle non-self-intersecting torus (MinorRadius <= MajorRadius).
    if (MinorRadius >= Tol && MinorRadius <= MajorRadius)
    {
      Result = new Geom_ToroidalSurface(Axis, MajorRadius, MinorRadius);
    }
    else if (MinorRadius <= -Tol && -MinorRadius <= MajorRadius)
    {
      // Negative minor radius: flip X-axis to reverse normal orientation.
      Axis.XReverse();
      Result = new Geom_ToroidalSurface(Axis, MajorRadius, -MinorRadius);
    }
    // else: degenerate or self-intersecting torus - no equivalent surface.
  }

  // Trim the result if the basis surface was trimmed.
  if (IsTrimmed && !Result.IsNull())
  {
    Base   = Result;
    Result = new Geom_RectangularTrimmedSurface(Base, U1, U2, V1, V2);
  }

  return Result;
}

//=================================================================================================

bool Geom_OffsetSurface::UOsculatingSurface(const double                      U,
                                            const double                      V,
                                            bool&                             t,
                                            occ::handle<Geom_BSplineSurface>& L) const
{
  return myOscSurf && myOscSurf->UOsculatingSurface(U, V, t, L);
}

//=================================================================================================

bool Geom_OffsetSurface::VOsculatingSurface(const double                      U,
                                            const double                      V,
                                            bool&                             t,
                                            occ::handle<Geom_BSplineSurface>& L) const
{
  return myOscSurf && myOscSurf->VOsculatingSurface(U, V, t, L);
}

//=================================================================================================

void Geom_OffsetSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Surface)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, basisSurf.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, equivSurf.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, offsetValue)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBasisSurfContinuity)
}
