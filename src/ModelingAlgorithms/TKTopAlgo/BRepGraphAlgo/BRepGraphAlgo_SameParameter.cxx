// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BRepGraphAlgo_SameParameter.hxx>

#include <Approx_CurvilinearParameter.hxx>
#include <Approx_SameParameter.hxx>
#include <BRepCheck.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_MutView.hxx>
#include <BSplCLib.hxx>
#include <ExtremaPC_Curve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>
#include <GeomLib.hxx>
#include <NCollection_Array1.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <cmath>

namespace
{

//! Compute max deviation between a 3D curve and a 2D curve evaluated on a surface.
//! Replicates the static ComputeTol() from BRepLib.cxx.
double computeTol(const Handle(Adaptor3d_Curve)&   theC3d,
                  const Handle(Adaptor2d_Curve2d)& theC2d,
                  const Handle(Adaptor3d_Surface)& theSurf,
                  int                              theNbP)
{
  NCollection_Array1<double> aDist(1, theNbP + 10);
  aDist.Init(-1.0);

  const double aUf         = theSurf->FirstUParameter();
  const double aUl         = theSurf->LastUParameter();
  const double aVf         = theSurf->FirstVParameter();
  const double aVl         = theSurf->LastVParameter();
  const double aDu         = 0.01 * (aUl - aUf);
  const double aDv         = 0.01 * (aVl - aVf);
  const bool   isUPeriodic = theSurf->IsUPeriodic();
  const bool   isVPeriodic = theSurf->IsVPeriodic();
  const double aDSdu       = 1.0 / theSurf->UResolution(1.0);
  const double aDSdv       = 1.0 / theSurf->VResolution(1.0);
  double       aD2         = 0.0;
  const double aFirst      = theC3d->FirstParameter();
  const double aLast       = theC3d->LastParameter();
  double       aDapp       = -1.0;

  for (int i = 0; i <= theNbP; ++i)
  {
    const double   aT    = static_cast<double>(i) / static_cast<double>(theNbP);
    const double   aU    = aFirst * (1.0 - aT) + aLast * aT;
    const gp_Pnt   aPc3d = theC3d->EvalD0(aU);
    const gp_Pnt2d aPuv  = theC2d->EvalD0(aU);

    if (!isUPeriodic)
    {
      if (aPuv.X() < aUf - aDu)
      {
        aDapp = std::max(aDapp, aDSdu * (aUf - aPuv.X()));
        continue;
      }
      else if (aPuv.X() > aUl + aDu)
      {
        aDapp = std::max(aDapp, aDSdu * (aPuv.X() - aUl));
        continue;
      }
    }
    if (!isVPeriodic)
    {
      if (aPuv.Y() < aVf - aDv)
      {
        aDapp = std::max(aDapp, aDSdv * (aVf - aPuv.Y()));
        continue;
      }
      else if (aPuv.Y() > aVl + aDv)
      {
        aDapp = std::max(aDapp, aDSdv * (aPuv.Y() - aVl));
        continue;
      }
    }

    const gp_Pnt aPcons = theSurf->Value(aPuv.X(), aPuv.Y());
    if (Precision::IsInfinite(aPcons.X()) || Precision::IsInfinite(aPcons.Y())
        || Precision::IsInfinite(aPcons.Z()))
    {
      aD2 = Precision::Infinite();
      break;
    }

    const double aTemp = aPc3d.SquareDistance(aPcons);
    aDist(i + 1)       = aTemp;
    aD2                = std::max(aD2, aTemp);
  }

  if (Precision::IsInfinite(aD2))
  {
    return aD2;
  }

  aD2 = std::sqrt(aD2);
  if (aDapp > aD2)
  {
    return aDapp;
  }

  bool   isAna  = false;
  double aD2alt = 0.0;
  int    aN1    = 0;
  int    aN2    = 0;
  int    aN3    = 0;

  for (int i = 1; i <= theNbP + 10; ++i)
  {
    if (aDist(i) > 0.0)
    {
      if (aDist(i) < 1.0)
        ++aN1;
      else
        ++aN2;
    }
  }

  if (aN1 > aN2 && aN2 != 0)
  {
    aN3 = 100 * aN2 / (aN1 + aN2);
  }
  if (aN3 < 10 && aN3 != 0)
  {
    isAna = true;
    for (int i = 1; i <= theNbP + 10; ++i)
    {
      if (aDist(i) > 0.0 && aDist(i) < 1.0)
      {
        aD2alt = std::max(aD2alt, aDist(i));
      }
    }
  }

  aD2 = (!isAna) ? 1.5 * aD2 : 1.5 * std::sqrt(aD2alt);
  aD2 = std::max(aD2, 1.0e-7);
  return aD2;
}

//! Evaluate tolerance by projecting sampled PCurve-on-Surface points back onto the 3D curve.
//! Replicates the static EvalTol() from BRepLib.cxx, using ExtremaPC_Curve and adaptors.
bool evalTol(const Geom2dAdaptor_Curve& thePCAdaptor,
             const GeomAdaptor_Surface& theSurfAdaptor,
             const GeomAdaptor_Curve&   theGAC,
             double                     theTol,
             double&                    theTolBail)
{
  int             aOk = 0;
  const double    aF  = theGAC.FirstParameter();
  const double    aL  = theGAC.LastParameter();
  ExtremaPC_Curve aProjector(theGAC, aF, aL);
  theTolBail = theTol;
  for (int i = 1; i <= 5; ++i)
  {
    const double   aT     = static_cast<double>(i) / 6.0;
    const double   aParam = (1.0 - aT) * aF + aT * aL;
    const gp_Pnt2d aUV    = thePCAdaptor.EvalD0(aParam);
    const gp_Pnt   aP     = theSurfAdaptor.EvalD0(aUV.X(), aUV.Y());

    const ExtremaPC::Result& aRes =
      aProjector.Perform(aP, Precision::Confusion(), ExtremaPC::SearchMode::Min);
    if (aRes.IsDone() && aRes.NbExt() > 0)
    {
      const double aDist2 = aRes[aRes.MinIndex()].SquareDistance;
      if (aDist2 > theTolBail * theTolBail)
      {
        theTolBail = std::sqrt(aDist2);
      }
      ++aOk;
    }
  }
  return (aOk > 2);
}

constexpr int    THE_NCONTROL  = 22;     //!< Number of control points for ComputeTol sampling
constexpr double THE_BIG_ERROR = 1.0e10; //!< Threshold above which error is considered degenerate
constexpr double THE_MIN_POLE_DIST_RATIO =
  0.1;                                        //!< Fraction of min pole distance for C0→C1 tolerance
constexpr double THE_APPROX_MIN_TOL = 1.0e-3; //!< Minimum tolerance for Approx_CurvilinearParameter
constexpr double THE_ANA_BSP_TOL    = 1.0e-7; //!< Tight tolerance for analytical+BSpline case
constexpr double THE_KNOT_RATIO_CRIT =
  10.0; //!< Critical knot spacing ratio for "bad" BSpline detection
constexpr int THE_MAX_APPROX_DEGREE   = 14; //!< Max degree for CurvilinearParameter approximation
constexpr int THE_MAX_APPROX_SEGMENTS = 10; //!< Max segments for CurvilinearParameter approximation

} // anonymous namespace

//=================================================================================================

bool BRepGraphAlgo_SameParameter::Enforce(BRepGraph&       theGraph,
                                          BRepGraph_NodeId theEdgeId,
                                          double           theTolerance)
{
  const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(theEdgeId.Index);
  if (anEdge.Curve3d.IsNull() || anEdge.PCurves.IsEmpty())
  {
    theGraph.Mut().EdgeDef(theEdgeId.Index).SameParameter = true;
    return true;
  }

  // Build 3D curve adaptor from graph, handle periodicity/trimming like BRepLib.
  Handle(Geom_Curve) aC3d = anEdge.Curve3d;
  double             aF3d = anEdge.ParamFirst;
  double             aL3d = anEdge.ParamLast;

  bool isTrimmedPeriodical = false;
  if (aC3d->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
  {
    Handle(Geom_Curve) aBasis = Handle(Geom_TrimmedCurve)::DownCast(aC3d)->BasisCurve();
    isTrimmedPeriodical       = aBasis->IsPeriodic();
  }

  if (!aC3d->IsPeriodic() && !isTrimmedPeriodical)
  {
    const double aUdeb = aC3d->FirstParameter();
    const double aUfin = aC3d->LastParameter();
    if (aUdeb > aF3d)
      aF3d = aUdeb;
    if (aL3d > aUfin)
      aL3d = aUfin;
  }

  // Apply edge location transform if any (graph CurveAdaptor handles this internally,
  // but for Approx_SameParameter we need explicit Geom_Curve + GeomAdaptor_Curve).
  GeomAdaptor_TransformedCurve aTransCurve = theGraph.Defs().CurveAdaptor(theEdgeId);
  const gp_Trsf&               aCrvTrsf    = aTransCurve.Trsf();

  // Build handles for Approx_SameParameter.
  Handle(GeomAdaptor_Curve)   aHC    = new GeomAdaptor_Curve();
  Handle(Geom2dAdaptor_Curve) aHC2d  = new Geom2dAdaptor_Curve();
  Handle(GeomAdaptor_Surface) aHS    = new GeomAdaptor_Surface();
  GeomAdaptor_Curve&          aGAC   = *aHC;
  Geom2dAdaptor_Curve&        aGAC2d = *aHC2d;
  GeomAdaptor_Surface&        aGAS   = *aHS;

  // Load 3D curve (apply transform if non-identity).
  Handle(Geom_Curve) aC3dTransformed = aC3d;
  if (aCrvTrsf.Form() != gp_Identity)
  {
    aC3dTransformed = Handle(Geom_Curve)::DownCast(aC3d->Transformed(aCrvTrsf));
  }
  aGAC.Load(aC3dTransformed, aF3d, aL3d);

  const double aPrecC3d    = BRepCheck::PrecCurve(aGAC);
  const double anEdgeTol   = anEdge.Tolerance;
  const bool   isSameRange = anEdge.SameRange;

  // Pre-classify 3D curve type for analytic fast path (Opt 4).
  const GeomAbs_CurveType aCrvType = aGAC.GetType();

  bool   isAllSameP = true;
  double aMaxDist   = 0.0;
  bool   hasYaPCu   = false;

  for (int aPCIdx = 0; aPCIdx < anEdge.PCurves.Length(); ++aPCIdx)
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPC = anEdge.PCurves.Value(aPCIdx);
    if (aPC.Curve2d.IsNull())
    {
      continue;
    }

    const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(aPC.FaceDefId.Index);
    if (aFace.Surface.IsNull())
    {
      continue;
    }

    hasYaPCu = true;

    // Load surface (apply face transform if non-identity).
    GeomAdaptor_TransformedSurface aTransSurf = theGraph.Defs().SurfaceAdaptor(aPC.FaceDefId);
    Handle(Geom_Surface)           aSurf      = aFace.Surface;
    const gp_Trsf&                 aSrfTrsf   = aTransSurf.Trsf();
    if (aSrfTrsf.Form() != gp_Identity)
    {
      aSurf = Handle(Geom_Surface)::DownCast(aSurf->Transformed(aSrfTrsf));
    }
    aGAS.Load(aSurf);

    Handle(Geom2d_Curve) aCurPC    = aPC.Curve2d;
    bool                 aUpdatePC = false;

    // SameRange check.
    const double aTolSameRange = std::max(aGAC.Resolution(theTolerance), Precision::PConfusion());
    if (!isSameRange)
    {
      Handle(Geom2d_Curve) aNewPC;
      GeomLib::SameRange(aTolSameRange,
                         aPC.Curve2d,
                         aPC.ParamFirst,
                         aPC.ParamLast,
                         aF3d,
                         aL3d,
                         aNewPC);
      if (!aNewPC.IsNull() && aNewPC != aPC.Curve2d)
      {
        aCurPC    = aNewPC;
        aUpdatePC = true;
      }
    }

    bool isGoodPC = true;
    aGAC2d.Load(aCurPC, aF3d, aL3d);

    // Analytic fast path: line-on-plane, circle-on-plane, and line-on-cylinder
    // have naturally matching parameterizations. Validate with a cheap 3-point
    // sample before skipping the full computeTol + Approx_SameParameter.
    {
      const GeomAbs_SurfaceType aSrfType = aGAS.GetType();
      if ((aCrvType == GeomAbs_Line
           && (aSrfType == GeomAbs_Plane || aSrfType == GeomAbs_Cylinder))
          || (aCrvType == GeomAbs_Circle && aSrfType == GeomAbs_Plane))
      {
        // Quick validation: sample at uniform intervals and check deviation.
        constexpr int    THE_NB_ANALYTIC_SAMPLES = 5;
        constexpr double THE_ANALYTIC_STEP = 1.0 / (THE_NB_ANALYTIC_SAMPLES - 1);
        bool isAnalyticOk  = true;
        double aMaxAnalErr = 0.0;
        for (int iSmp = 0; iSmp < THE_NB_ANALYTIC_SAMPLES; ++iSmp)
        {
          const double aT   = aF3d + (aL3d - aF3d) * (static_cast<double>(iSmp) * THE_ANALYTIC_STEP);
          const gp_Pnt aP3d = aGAC.EvalD0(aT);
          const gp_Pnt2d aUV = aGAC2d.EvalD0(aT);
          const gp_Pnt aPSurf = aGAS.EvalD0(aUV.X(), aUV.Y());
          const double aDev  = aP3d.Distance(aPSurf);
          aMaxAnalErr = std::max(aMaxAnalErr, aDev);
          if (aDev > theTolerance)
          {
            isAnalyticOk = false;
            break;
          }
        }
        if (isAnalyticOk)
        {
          aMaxDist = std::max(aMaxDist, std::max(aMaxAnalErr, Precision::Confusion()));
          if (aUpdatePC)
          {
            theGraph.Mut().EdgeDef(theEdgeId.Index).PCurves.ChangeValue(aPCIdx).Curve2d = aCurPC;
          }
          continue;
        }
        // Validation failed — fall through to full computeTol path.
      }
    }

    double anError = computeTol(aHC, aHC2d, aHS, THE_NCONTROL);

    if (anError > THE_BIG_ERROR)
    {
      aMaxDist = anError;
      break;
    }

    bool isANA = false;
    bool isBSP = false;

    // C0 BSpline PCurve → C1 conversion.
    if (aGAC2d.GetType() == GeomAbs_BSplineCurve && aGAC2d.Continuity() == GeomAbs_C0)
    {
      const double aUResol    = aGAS.UResolution(theTolerance);
      const double aVResol    = aGAS.VResolution(theTolerance);
      double       aTolConf2d = std::min(aUResol, aVResol);
      aTolConf2d              = std::max(aTolConf2d, Precision::PConfusion());

      Handle(Geom2d_BSplineCurve) aBs2d         = aGAC2d.BSpline();
      Handle(Geom2d_BSplineCurve) aBs2dSov      = aBs2d;
      const double                aFC0          = aBs2d->FirstParameter();
      const double                aLC0          = aBs2d->LastParameter();
      bool                        isRepar       = true;
      const gp_Pnt2d              anOriginPoint = aGAC2d.EvalD0(aFC0);
      try
      {
        OCC_CATCH_SIGNALS
        Geom2dConvert::C0BSplineToC1BSplineCurve(aBs2d, aTolConf2d);
      }
      catch (const Standard_Failure&)
      {
        aBs2d = aBs2dSov;
      }
      isBSP = true;

      if (aBs2d->IsPeriodic())
      {
        Geom2dAdaptor_Curve aBs2dAdaptor(aBs2d);
        const gp_Pnt2d      aNewOriginPt = aBs2dAdaptor.EvalD0(aBs2d->FirstParameter());
        if (std::abs(anOriginPoint.X() - aNewOriginPt.X()) > Precision::PConfusion()
            || std::abs(anOriginPoint.Y() - aNewOriginPt.Y()) > Precision::PConfusion())
        {
          for (int anIndex = 1; anIndex <= aBs2d->NbKnots(); ++anIndex)
          {
            const gp_Pnt2d aKnotPt = aBs2dAdaptor.EvalD0(aBs2d->Knot(anIndex));
            if (std::abs(anOriginPoint.X() - aKnotPt.X()) <= Precision::PConfusion()
                && std::abs(anOriginPoint.Y() - aKnotPt.Y()) <= Precision::PConfusion())
            {
              aBs2d->SetOrigin(anIndex);
              break;
            }
          }
        }
      }

      if (aBs2d->Continuity() == GeomAbs_C0)
      {
        double aTolBail;
        if (evalTol(aGAC2d, aGAS, aGAC, theTolerance, aTolBail))
        {
          aBs2d                   = aBs2dSov;
          const double aUResBail  = aGAS.UResolution(aTolBail);
          const double aVResBail  = aGAS.VResolution(aTolBail);
          double       aTol2dBail = std::min(aUResBail, aVResBail);

          const gp_Pnt2d anOriginPt2 = aBs2d->EvalD0(aBs2d->FirstParameter());

          const int                           aNbP   = aBs2d->NbPoles();
          const NCollection_Array1<gp_Pnt2d>& aPoles = aBs2d->Poles();
          gp_Pnt2d                            aP     = aPoles(1);
          double                              aD     = Precision::Infinite();
          for (int anIp = 2; anIp <= aNbP; ++anIp)
          {
            const gp_Pnt2d& aP1 = aPoles(anIp);
            aD                  = std::min(aD, aP.SquareDistance(aP1));
            aP                  = aP1;
          }
          aD = std::sqrt(aD) * THE_MIN_POLE_DIST_RATIO;

          aTol2dBail = std::max(std::min(aTol2dBail, aD), aTolConf2d);
          try
          {
            OCC_CATCH_SIGNALS
            Geom2dConvert::C0BSplineToC1BSplineCurve(aBs2d, aTol2dBail);
          }
          catch (const Standard_Failure&)
          {
            aBs2d   = aBs2dSov;
            isRepar = false;
          }

          if (aBs2d->IsPeriodic())
          {
            Geom2dAdaptor_Curve aBs2dAdaptor2(aBs2d);
            const gp_Pnt2d      aNewOriginPt = aBs2dAdaptor2.EvalD0(aBs2d->FirstParameter());
            if (std::abs(anOriginPt2.X() - aNewOriginPt.X()) > Precision::PConfusion()
                || std::abs(anOriginPt2.Y() - aNewOriginPt.Y()) > Precision::PConfusion())
            {
              for (int anIndex = 1; anIndex <= aBs2d->NbKnots(); ++anIndex)
              {
                const gp_Pnt2d aKnotPt = aBs2dAdaptor2.EvalD0(aBs2d->Knot(anIndex));
                if (std::abs(anOriginPt2.X() - aKnotPt.X()) <= Precision::PConfusion()
                    && std::abs(anOriginPt2.Y() - aKnotPt.Y()) <= Precision::PConfusion())
                {
                  aBs2d->SetOrigin(anIndex);
                  break;
                }
              }
            }
          }

          if (aBs2d->Continuity() == GeomAbs_C0)
          {
            isGoodPC = true;
            aBs2d    = aBs2dSov;
            isRepar  = false;
          }
        }
        else
        {
          isGoodPC = false;
        }
      }

      if (isGoodPC)
      {
        if (isRepar)
        {
          NCollection_Array1<double> aKnots(aBs2d->Knots());
          BSplCLib::Reparametrize(aFC0, aLC0, aKnots);
          aBs2d->SetKnots(aKnots);
          aGAC2d.Load(aBs2d, aF3d, aL3d);
          aCurPC                  = aBs2d;
          const bool aUpdatePCSov = aUpdatePC;
          aUpdatePC               = true;

          const double anError1 = computeTol(aHC, aHC2d, aHS, THE_NCONTROL);
          if (anError1 > anError)
          {
            aBs2d = aBs2dSov;
            aGAC2d.Load(aBs2d, aF3d, aL3d);
            aCurPC    = aBs2d;
            aUpdatePC = aUpdatePCSov;
            isANA     = true;
          }
          else
          {
            anError = anError1;
          }
        }

        // Check if the BSpline is "bad" (knot ratio issue).
        const GeomAbs_Shape aCont = aBs2d->Continuity();
        bool                isBad = false;

        if (aCont > GeomAbs_C0 && anError > std::max(THE_APPROX_MIN_TOL, theTolerance))
        {
          const int                         aNbKnots   = aBs2d->NbKnots();
          const NCollection_Array1<double>& aKnots     = aBs2d->Knots();
          constexpr double                  aCritRatio = THE_KNOT_RATIO_CRIT;
          double                            aDtPrev    = aKnots(2) - aKnots(1);
          double                            aDtMin     = aDtPrev;
          double                            aDtCur     = 0.0;

          for (int j = 2; j < aNbKnots; ++j)
          {
            aDtCur = aKnots(j + 1) - aKnots(j);
            aDtMin = std::min(aDtMin, aDtCur);
            if (!isBad)
            {
              const double aDtRatio = (aDtCur > aDtPrev) ? (aDtCur / aDtPrev) : (aDtPrev / aDtCur);
              if (aDtRatio > aCritRatio)
              {
                isBad = true;
              }
            }
            aDtPrev = aDtCur;
          }
          if (isBad)
          {
            aBs2d->Resolution(std::max(THE_APPROX_MIN_TOL, theTolerance), aDtCur);
            if (aDtMin < aDtCur)
            {
              isBad = false;
            }
          }
        }

        if (isBad)
        {
          GeomAbs_Shape aContApprox = aBs2d->Continuity();
          if (aContApprox > GeomAbs_C2)
          {
            aContApprox = GeomAbs_C2;
          }
          int aMaxDeg = aBs2d->Degree();
          if (aMaxDeg == 1)
          {
            aMaxDeg = THE_MAX_APPROX_DEGREE;
          }
          Approx_CurvilinearParameter anAppCurPar(aHC2d,
                                                  aHS,
                                                  std::max(THE_APPROX_MIN_TOL, theTolerance),
                                                  aContApprox,
                                                  aMaxDeg,
                                                  THE_MAX_APPROX_SEGMENTS);
          if (anAppCurPar.IsDone() || anAppCurPar.HasResult())
          {
            aBs2d = anAppCurPar.Curve2d1();
            aGAC2d.Load(aBs2d, aF3d, aL3d);
            aCurPC = aBs2d;

            if (std::abs(aBs2d->FirstParameter() - aFC0) > aTolSameRange
                || std::abs(aBs2d->LastParameter() - aLC0) > aTolSameRange)
            {
              NCollection_Array1<double> aKnots(aBs2d->Knots());
              BSplCLib::Reparametrize(aFC0, aLC0, aKnots);
              aBs2d->SetKnots(aKnots);
              aGAC2d.Load(aBs2d, aF3d, aL3d);
              aCurPC = aBs2d;
            }
          }
        }
      }
    }

    if (isGoodPC)
    {
      const double                     aTol     = (isANA && isBSP) ? THE_ANA_BSP_TOL : theTolerance;
      const Handle(Adaptor3d_Curve)&   aHCurv   = aHC;
      const Handle(Adaptor2d_Curve2d)& aHCurv2d = aHC2d;
      Approx_SameParameter             aSameP(aHCurv, aHCurv2d, aHS, aTol);

      if (aSameP.IsSameParameter())
      {
        aMaxDist = std::max(aMaxDist, aSameP.TolReached());
        if (aUpdatePC)
        {
          theGraph.Mut().EdgeDef(theEdgeId.Index).PCurves.ChangeValue(aPCIdx).Curve2d = aCurPC;
        }
      }
      else if (aSameP.IsDone())
      {
        const double aTolReached = aSameP.TolReached();
        if (aTolReached <= anError)
        {
          aCurPC    = aSameP.Curve2d();
          aUpdatePC = true;
          aMaxDist  = std::max(aMaxDist, aTolReached);
        }
        else
        {
          aMaxDist = std::max(aMaxDist, anError);
        }
        if (aUpdatePC)
        {
          theGraph.Mut().EdgeDef(theEdgeId.Index).PCurves.ChangeValue(aPCIdx).Curve2d = aCurPC;
        }
      }
      else
      {
        // Approx_SameParameter failed — try SameRange fallback.
        Handle(Geom2d_Curve) aFallbackPC;
        GeomLib::SameRange(aTolSameRange,
                           aPC.Curve2d,
                           aPC.ParamFirst,
                           aPC.ParamLast,
                           aF3d,
                           aL3d,
                           aFallbackPC);
        if (!aFallbackPC.IsNull())
        {
          theGraph.Mut().EdgeDef(theEdgeId.Index).PCurves.ChangeValue(aPCIdx).Curve2d = aFallbackPC;
        }
        isAllSameP = false;
      }
    }
    else
    {
      isAllSameP = false;
    }

    // OCC5898: if not same-parameter, check if precision covers the error.
    if (!isAllSameP)
    {
      const double aPrecSurf = BRepCheck::PrecSurface(aHS);
      const double aCurTol   = anEdgeTol + std::max(aPrecC3d, aPrecSurf);
      if (aCurTol >= anError)
      {
        aMaxDist   = std::max(aMaxDist, anEdgeTol);
        isAllSameP = true;
      }
    }
  }

  BRepGraph_TopoNode::EdgeDef& anEdgeMut = theGraph.Mut().EdgeDef(theEdgeId.Index);
  anEdgeMut.SameRange                    = true;

  if (isAllSameP)
  {
    if (hasYaPCu)
    {
      aMaxDist            = std::max(aMaxDist, Precision::Confusion());
      anEdgeMut.Tolerance = aMaxDist;
    }
    anEdgeMut.SameParameter = true;
  }

  return isAllSameP;
}

//=================================================================================================

void BRepGraphAlgo_SameParameter::Perform(BRepGraph&                         theGraph,
                                          const NCollection_IndexedMap<int>& theEdgeIndices,
                                          double                             theTolerance,
                                          bool                               theParallel)
{
  const int aNbEdges = theEdgeIndices.Extent();
  if (aNbEdges == 0)
  {
    return;
  }

  // Copy indices to array for indexed parallel access.
  NCollection_Array1<int> anIndices(0, aNbEdges - 1);
  for (int anIdx = 1; anIdx <= aNbEdges; ++anIdx)
  {
    anIndices.SetValue(anIdx - 1, theEdgeIndices.FindKey(anIdx));
  }

  theGraph.BeginDeferredInvalidation();
  OSD_Parallel::For(
    0,
    aNbEdges,
    [&](int theIdx) {
      const int              anEdgeIdx = anIndices.Value(theIdx);
      const BRepGraph_NodeId anEdgeId(BRepGraph_NodeId::Kind::Edge, anEdgeIdx);
      Enforce(theGraph, anEdgeId, theTolerance);
    },
    !theParallel);
  theGraph.EndDeferredInvalidation();
}
