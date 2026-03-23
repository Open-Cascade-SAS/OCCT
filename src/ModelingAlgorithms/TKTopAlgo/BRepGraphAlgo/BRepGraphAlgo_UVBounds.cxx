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

#include <BRepGraphAlgo_UVBounds.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <ElCLib.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Precision.hxx>

namespace
{

//! GUID for UV bounds cache attribute.
static const Standard_GUID THE_UVBOUNDS_GUID("d4a2e8f1-6b73-4c9d-a5e2-8f1c3d7b9a04");

//! Internal attribute class storing cached UV bounds data.
class BRepGraphAlgo_UVBoundsAttribute : public BRepGraph_UserAttribute
{
public:
  DEFINE_STANDARD_RTTI_INLINE(BRepGraphAlgo_UVBoundsAttribute, BRepGraph_UserAttribute)
  BRepGraphAlgo_UVBoundsAttribute() = default;

  //! Return cached data if available (not dirty).
  //! @param[out] theData cached data (unchanged if dirty)
  //! @return true if cached data exists and is not dirty
  bool Get(BRepGraphAlgo_UVBounds::CachedData& theData) const
  {
    std::shared_lock<std::shared_mutex> aReadLock(myMutex);
    if (IsDirty())
    {
      return false;
    }
    theData = myData;
    return true;
  }

  //! Store new data and mark clean.
  void SetData(const BRepGraphAlgo_UVBounds::CachedData& theData)
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myMutex);
    myData = theData;
    MarkClean();
  }

private:
  BRepGraphAlgo_UVBounds::CachedData myData;
};

//! Check if a BSpline surface is geometrically U-periodic even when
//! IsUPeriodic() returns false. Ported from BRepTools::AddUVBounds.
//! @param[in] theSurface the surface to test (must be BSpline)
//! @param[in] theXmin    PCurve U minimum
//! @param[in] theXmax    PCurve U maximum
//! @param[in] theUmin    surface U minimum
//! @param[in] theUmax    surface U maximum
//! @param[in] theVmin    surface V minimum
//! @param[in] theVmax    surface V maximum
//! @return true if the surface is geometrically U-periodic
static bool isBSplinePseudoPeriodicU(const Handle(Geom_Surface)& theSurface,
                                     double                      theXmin,
                                     double                      theXmax,
                                     double                      theUmin,
                                     double                      theUmax,
                                     double                      theVmin,
                                     double                      theVmax)
{
  const GeomAdaptor_Surface aSurfAdaptor(theSurface);
  const GeomAdaptor_Surface aExtAdaptor(theSurface);
  if (aSurfAdaptor.GetType() != GeomAbs_BSplineSurface)
  {
    return false;
  }
  if (theXmin >= theUmin && theXmax <= theUmax)
  {
    return false;
  }

  constexpr double aTol2       = 100 * Precision::Confusion() * Precision::Confusion();
  bool             isUPeriodic = true;

  // Verify that the surface is U-closed.
  if (!aSurfAdaptor.IsUClosed())
  {
    const double aVStep = theVmax - theVmin;
    if (aVStep < Precision::Confusion())
    {
      return false;
    }
    for (double aV = theVmin; aV <= theVmax; aV += aVStep)
    {
      const gp_Pnt aP1 = aSurfAdaptor.EvalD0(theUmin, aV);
      const gp_Pnt aP2 = aSurfAdaptor.EvalD0(theUmax, aV);
      if (aP1.SquareDistance(aP2) > aTol2)
      {
        return false;
      }
    }
  }

  // Verify periodicity inside UV-bounds of the edge.
  double aV = (theVmin + theVmax) * 0.5;
  double aU[6];
  double aUpp[6];
  int    aNbPnt = 0;
  if (theXmin < theUmin)
  {
    aU[0]   = theXmin;
    aU[1]   = (theXmin + theUmin) * 0.5;
    aU[2]   = theUmin;
    aUpp[0] = aU[0] + theUmax - theUmin;
    aUpp[1] = aU[1] + theUmax - theUmin;
    aUpp[2] = aU[2] + theUmax - theUmin;
    aNbPnt += 3;
  }
  if (theXmax > theUmax)
  {
    aU[aNbPnt]       = theUmax;
    aU[aNbPnt + 1]   = (theXmax + theUmax) * 0.5;
    aU[aNbPnt + 2]   = theXmax;
    aUpp[aNbPnt]     = aU[aNbPnt] - theUmax + theUmin;
    aUpp[aNbPnt + 1] = aU[aNbPnt + 1] - theUmax + theUmin;
    aUpp[aNbPnt + 2] = aU[aNbPnt + 2] - theUmax + theUmin;
    aNbPnt += 3;
  }
  for (int anInd = 0; anInd < aNbPnt; anInd++)
  {
    const gp_Pnt aP1 = aExtAdaptor.EvalD0(aU[anInd], aV);
    const gp_Pnt aP2 = aSurfAdaptor.EvalD0(aUpp[anInd], aV);
    if (aP1.SquareDistance(aP2) > aTol2)
    {
      isUPeriodic = false;
      break;
    }
  }
  return isUPeriodic;
}

//! Check if a BSpline surface is geometrically V-periodic even when
//! IsVPeriodic() returns false. Ported from BRepTools::AddUVBounds.
//! @param[in] theSurface the surface to test (must be BSpline)
//! @param[in] theYmin    PCurve V minimum
//! @param[in] theYmax    PCurve V maximum
//! @param[in] theUmin    surface U minimum
//! @param[in] theUmax    surface U maximum
//! @param[in] theVmin    surface V minimum
//! @param[in] theVmax    surface V maximum
//! @return true if the surface is geometrically V-periodic
static bool isBSplinePseudoPeriodicV(const Handle(Geom_Surface)& theSurface,
                                     double                      theYmin,
                                     double                      theYmax,
                                     double                      theUmin,
                                     double                      theUmax,
                                     double                      theVmin,
                                     double                      theVmax)
{
  const GeomAdaptor_Surface aSurfAdaptor(theSurface);
  const GeomAdaptor_Surface aExtAdaptor(theSurface);
  if (aSurfAdaptor.GetType() != GeomAbs_BSplineSurface)
  {
    return false;
  }
  if (theYmin >= theVmin && theYmax <= theVmax)
  {
    return false;
  }

  constexpr double aTol2       = 100 * Precision::Confusion() * Precision::Confusion();
  bool             isVPeriodic = true;

  // Verify that the surface is V-closed.
  if (!aSurfAdaptor.IsVClosed())
  {
    const double aUStep = theUmax - theUmin;
    if (aUStep < Precision::Confusion())
    {
      return false;
    }
    for (double aU = theUmin; aU <= theUmax; aU += aUStep)
    {
      const gp_Pnt aP1 = aSurfAdaptor.EvalD0(aU, theVmin);
      const gp_Pnt aP2 = aSurfAdaptor.EvalD0(aU, theVmax);
      if (aP1.SquareDistance(aP2) > aTol2)
      {
        return false;
      }
    }
  }

  // Verify periodicity inside UV-bounds of the edge.
  double aU = (theUmin + theUmax) * 0.5;
  double aV[6];
  double aVpp[6];
  int    aNbPnt = 0;
  if (theYmin < theVmin)
  {
    aV[0]   = theYmin;
    aV[1]   = (theYmin + theVmin) * 0.5;
    aV[2]   = theVmin;
    aVpp[0] = aV[0] + theVmax - theVmin;
    aVpp[1] = aV[1] + theVmax - theVmin;
    aVpp[2] = aV[2] + theVmax - theVmin;
    aNbPnt += 3;
  }
  if (theYmax > theVmax)
  {
    aV[aNbPnt]       = theVmax;
    aV[aNbPnt + 1]   = (theYmax + theVmax) * 0.5;
    aV[aNbPnt + 2]   = theYmax;
    aVpp[aNbPnt]     = aV[aNbPnt] - theVmax + theVmin;
    aVpp[aNbPnt + 1] = aV[aNbPnt + 1] - theVmax + theVmin;
    aVpp[aNbPnt + 2] = aV[aNbPnt + 2] - theVmax + theVmin;
    aNbPnt += 3;
  }
  for (int anInd = 0; anInd < aNbPnt; anInd++)
  {
    const gp_Pnt aP1 = aExtAdaptor.EvalD0(aU, aV[anInd]);
    const gp_Pnt aP2 = aSurfAdaptor.EvalD0(aU, aVpp[anInd]);
    if (aP1.SquareDistance(aP2) > aTol2)
    {
      isVPeriodic = false;
      break;
    }
  }
  return isVPeriodic;
}

} // namespace

//=================================================================================================

void BRepGraphAlgo_UVBounds::Compute(const BRepGraph& theGraph, int theFaceIdx, CachedData& theData)
{
  theData = CachedData();

  const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Defs().Face(theFaceIdx);
  if (aFaceDef.Surface.IsNull())
  {
    return;
  }

  theData.IsValid              = true;
  theData.IsNaturalRestriction = aFaceDef.NaturalRestriction;
  theData.ComputeMethod        = Method::PCurve;

  GeomAdaptor_Surface aGAS(aFaceDef.Surface);

  theData.UMin = aGAS.FirstUParameter();
  theData.UMax = aGAS.LastUParameter();
  theData.VMin = aGAS.FirstVParameter();
  theData.VMax = aGAS.LastVParameter();

  if (theData.IsNaturalRestriction)
  {
    return;
  }

  // Check by comparing PCurves and surface boundaries (the 4-edge natural restriction check).
  const double theTol = aFaceDef.Tolerance;
  if (!aFaceDef.WireRefs.IsEmpty())
  {
    const bool     isUperiodic = aGAS.IsUPeriodic();
    const bool     isVperiodic = aGAS.IsVPeriodic();
    const double   aTolU       = std::max(aGAS.UResolution(theTol), Precision::PConfusion());
    const double   aTolV       = std::max(aGAS.VResolution(theTol), Precision::PConfusion());
    int            Nu = 0, Nv = 0, NbEdges = 0;
    const gp_Vec2d Du(1, 0), Dv(0, 1);
    bool           isAborted = false;

    // Collect all wire edges for the 4-edge check.
    auto checkWireEdges = [&](int theWireIdx) {
      if (isAborted)
      {
        return;
      }
      const BRepGraph_TopoNode::WireDef& aWireDef = theGraph.Defs().Wire(theWireIdx);
      for (int anIdx = 0; anIdx < aWireDef.EdgeRefs.Length() && !isAborted; ++anIdx)
      {
        NbEdges++;
        if (NbEdges > 4)
        {
          isAborted = true;
          return;
        }
        const BRepGraphInc::EdgeRef& aER = aWireDef.EdgeRefs.Value(anIdx);
        const BRepGraph_NodeId anEdgeDefId = BRepGraph_NodeId::Edge(aER.EdgeIdx);
        const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCurve =
          theGraph.Defs().FindPCurve(anEdgeDefId, aFaceDef.Id);
        if (aPCurve == nullptr || aPCurve->Curve2d.IsNull())
        {
          isAborted = true;
          return;
        }
        gp_Pnt2d     aP;
        gp_Vec2d     aV;
        const double aMidParam = (aPCurve->ParamFirst + aPCurve->ParamLast) / 2.;
        aPCurve->Curve2d->D1(aMidParam, aP, aV);
        const double aMagn = aV.SquareMagnitude();
        if (aMagn < gp::Resolution())
        {
          isAborted = true;
          return;
        }
        aV /= std::sqrt(aMagn);
        double u = aP.X(), v = aP.Y();
        if (isUperiodic)
        {
          ElCLib::InPeriod(u, theData.UMin, theData.UMax);
        }
        if (isVperiodic)
        {
          ElCLib::InPeriod(v, theData.VMin, theData.VMax);
        }
        if (std::abs(u - theData.UMin) <= aTolU || std::abs(u - theData.UMax) <= aTolU)
        {
          const double d = Dv * aV;
          if (1. - std::abs(d) <= Precision::PConfusion())
          {
            Nu++;
            if (Nu > 2)
            {
              isAborted = true;
              return;
            }
          }
          else
          {
            isAborted = true;
            return;
          }
        }
        else if (std::abs(v - theData.VMin) <= aTolV || std::abs(v - theData.VMax) <= aTolV)
        {
          const double d = Du * aV;
          if (1. - std::abs(d) <= Precision::PConfusion())
          {
            Nv++;
            if (Nv > 2)
            {
              isAborted = true;
              return;
            }
          }
          else
          {
            isAborted = true;
            return;
          }
        }
        else
        {
          isAborted = true;
          return;
        }
      }
    };

    for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length() && !isAborted; ++aWireRefIdx)
    {
      checkWireEdges(aFaceDef.WireRefs.Value(aWireRefIdx).WireIdx);
    }

    if (!isAborted && Nu == 2 && Nv == 2)
    {
      theData.IsNaturalRestriction = true;
      return;
    }
  }

  // Compute UV bounds from PCurves.
  const double aTolU  = std::max(aGAS.UResolution(theTol), Precision::PConfusion());
  const double aTolV  = std::max(aGAS.VResolution(theTol), Precision::PConfusion());
  const double aTolUV = std::max(aTolU, aTolV);

  Bnd_Box2d aBox;
  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
  {
    const BRepGraphInc::WireRef& aWR = aFaceDef.WireRefs.Value(aWireRefIdx);
    const BRepGraph_TopoNode::WireDef& aWireDef = theGraph.Defs().Wire(aWR.WireIdx);
    for (int anIdx = 0; anIdx < aWireDef.EdgeRefs.Length(); ++anIdx)
    {
      const BRepGraphInc::EdgeRef& aER = aWireDef.EdgeRefs.Value(anIdx);
      const BRepGraph_NodeId anEdgeDefId = BRepGraph_NodeId::Edge(aER.EdgeIdx);
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCurve =
        theGraph.Defs().FindPCurve(anEdgeDefId, aFaceDef.Id);
      if (aPCurve == nullptr || aPCurve->Curve2d.IsNull())
      {
        continue;
      }
      BndLib_Add2dCurve::AddOptimal(aPCurve->Curve2d,
                                    aPCurve->ParamFirst,
                                    aPCurve->ParamLast,
                                    aTolUV,
                                    aBox);
    }
  }

  if (!aBox.IsVoid())
  {
    aBox.Get(theData.UMin, theData.VMin, theData.UMax, theData.VMax);
  }

  // Clamp to surface bounds, handle periodicity.
  // Unwrap RectangularTrimmedSurface to get the basis surface for periodicity checks.
  Handle(Geom_Surface) aBasisSurf = aFaceDef.Surface;
  if (aBasisSurf->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    aBasisSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast(aBasisSurf)->BasisSurface();
  }

  double aSUmin, aSUmax, aSVmin, aSVmax;
  aFaceDef.Surface->Bounds(aSUmin, aSUmax, aSVmin, aSVmax);

  if (!aFaceDef.Surface->IsUPeriodic())
  {
    // BSpline pseudo-periodicity check: surface may be geometrically periodic
    // even though IsUPeriodic() returns false.
    if (!isBSplinePseudoPeriodicU(aBasisSurf,
                                  theData.UMin,
                                  theData.UMax,
                                  aSUmin,
                                  aSUmax,
                                  aSVmin,
                                  aSVmax))
    {
      theData.UMin = std::max(aSUmin, theData.UMin);
      theData.UMax = std::min(aSUmax, theData.UMax);
    }
  }
  else
  {
    if (theData.UMax - theData.UMin > aFaceDef.Surface->UPeriod())
    {
      const double delta = theData.UMax - theData.UMin - aFaceDef.Surface->UPeriod();
      theData.UMin += delta / 2.;
      theData.UMax -= delta / 2.;
    }
  }

  if (!aFaceDef.Surface->IsVPeriodic())
  {
    // BSpline pseudo-periodicity check for V direction.
    if (!isBSplinePseudoPeriodicV(aBasisSurf,
                                  theData.VMin,
                                  theData.VMax,
                                  aSUmin,
                                  aSUmax,
                                  aSVmin,
                                  aSVmax))
    {
      theData.VMin = std::max(aSVmin, theData.VMin);
      theData.VMax = std::min(aSVmax, theData.VMax);
    }
  }
  else
  {
    if (theData.VMax - theData.VMin > aFaceDef.Surface->VPeriod())
    {
      const double delta = theData.VMax - theData.VMin - aFaceDef.Surface->VPeriod();
      theData.VMin += delta / 2.;
      theData.VMax -= delta / 2.;
    }
  }
}

//=================================================================================================

int BRepGraphAlgo_UVBounds::CacheKey()
{
  static const int THE_KEY = BRepGraph_UserAttribute::AllocateKey(THE_UVBOUNDS_GUID);
  return THE_KEY;
}

//=================================================================================================

//! Helper: retrieve existing UV attribute from a cache, or nullptr.
static Handle(BRepGraphAlgo_UVBoundsAttribute) findUVAttr(const BRepGraph_NodeCache& theCache, int theKey)
{
  Handle(BRepGraph_UserAttribute) anAttr = theCache.GetUserAttribute(theKey);
  if (anAttr.IsNull())
    return Handle(BRepGraphAlgo_UVBoundsAttribute)();
  return Handle(BRepGraphAlgo_UVBoundsAttribute)::DownCast(anAttr);
}

//! Helper: store CachedData into a cache — reuses existing attribute or creates a new one.
static void storeUVAttr(BRepGraph_NodeCache&                      theCache,
                        int                                       theKey,
                        const BRepGraphAlgo_UVBounds::CachedData& theData)
{
  Handle(BRepGraphAlgo_UVBoundsAttribute) anExisting = findUVAttr(theCache, theKey);
  if (!anExisting.IsNull())
  {
    anExisting->SetData(theData);
    return;
  }

  Handle(BRepGraphAlgo_UVBoundsAttribute) aNewAttr = new BRepGraphAlgo_UVBoundsAttribute();
  aNewAttr->SetData(theData);
  theCache.SetUserAttribute(theKey, aNewAttr);
}

bool BRepGraphAlgo_UVBounds::GetCached(const BRepGraph& theGraph,
                                       BRepGraph_NodeId theNode,
                                       CachedData&      theData)
{
  if (theNode.NodeKind != BRepGraph_NodeId::Kind::Face || !theNode.IsValid())
  {
    return false;
  }

  const BRepGraph_TopoNode::BaseDef* aDef = theGraph.TopoDef(theNode);
  if (aDef == nullptr)
  {
    return false;
  }

  Handle(BRepGraphAlgo_UVBoundsAttribute) aUVAttr = findUVAttr(aDef->Cache, CacheKey());
  return !aUVAttr.IsNull() && aUVAttr->Get(theData);
}

//=================================================================================================

BRepGraphAlgo_UVBounds::CachedData BRepGraphAlgo_UVBounds::AddCached(BRepGraph&       theGraph,
                                                                     BRepGraph_NodeId theNode)
{
  if (theNode.NodeKind != BRepGraph_NodeId::Kind::Face || !theNode.IsValid())
  {
    return CachedData();
  }

  const int            aKey   = CacheKey();
  BRepGraph_NodeCache* aCache = theGraph.mutableCache(theNode);
  if (aCache == nullptr)
  {
    return CachedData();
  }

  // Try cache hit.
  Handle(BRepGraphAlgo_UVBoundsAttribute) anExisting = findUVAttr(*aCache, aKey);
  if (!anExisting.IsNull())
  {
    CachedData aData;
    if (anExisting->Get(aData))
    {
      return aData;
    }
  }

  // Compute and cache only if valid.
  CachedData aData;
  Compute(theGraph, theNode.Index, aData);
  if (aData.IsValid)
  {
    storeUVAttr(*aCache, aKey, aData);
  }

  return aData;
}

//=================================================================================================

void BRepGraphAlgo_UVBounds::SetCached(BRepGraph&        theGraph,
                                       BRepGraph_NodeId  theNode,
                                       const CachedData& theData)
{
  if (theNode.NodeKind != BRepGraph_NodeId::Kind::Face || !theNode.IsValid())
  {
    return;
  }

  BRepGraph_NodeCache* aCache = theGraph.mutableCache(theNode);
  if (aCache == nullptr)
  {
    return;
  }

  storeUVAttr(*aCache, CacheKey(), theData);
}

//=================================================================================================

void BRepGraphAlgo_UVBounds::InvalidateCached(BRepGraph& theGraph, BRepGraph_NodeId theNode)
{
  if (theNode.NodeKind != BRepGraph_NodeId::Kind::Face || !theNode.IsValid())
  {
    return;
  }

  BRepGraph_NodeCache* aCache = theGraph.mutableCache(theNode);
  if (aCache == nullptr)
  {
    return;
  }
  aCache->InvalidateUserAttribute(CacheKey());
}
