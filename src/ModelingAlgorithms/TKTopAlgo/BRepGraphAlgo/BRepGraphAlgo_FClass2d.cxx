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

#include <BRepGraphAlgo_FClass2d.hxx>

#include <BRepClass_FaceClassifier.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
#include <BRepGraphInc_WireExplorer.hxx>
#include <CSLib_Class2d.hxx>
#include <ElCLib.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pnt2d.hxx>

namespace
{
// Increments theValue by theIncrement towards theDirection, ensuring that the result
// differs from theValue even when floating-point precision is exhausted.
inline double safeIncrement(const double theValue,
                            const double theDirection,
                            const double theIncrement)
{
  const double aNextValue = theValue + theIncrement;
  return aNextValue == theValue ? std::nextafter(theValue, theDirection) : aNextValue;
}

//=================================================================================================

// Checks whether a 3D curve is degenerate between theStartParam and theEndParam by
// sampling points and testing distance to start.
bool isDegenerated(const occ::handle<Geom_Curve>& theCurve,
                   const TopLoc_Location&    theLoc,
                   const double              theStartParam,
                   const double              theEndParam)
{
  if (theCurve.IsNull())
    return true;

  const gp_Trsf aTrsf     = theLoc.IsIdentity() ? gp_Trsf() : theLoc.Transformation();
  const double  aStep     = (theEndParam - theStartParam) * 0.1;
  gp_Pnt        aStartPnt = theCurve->Value(theStartParam);
  if (!theLoc.IsIdentity())
    aStartPnt.Transform(aTrsf);

  for (double aParam = theStartParam; aParam < theEndParam;
       aParam        = safeIncrement(aParam, theEndParam, aStep))
  {
    gp_Pnt aPnt = theCurve->Value(aParam);
    if (!theLoc.IsIdentity())
      aPnt.Transform(aTrsf);
    if (aStartPnt.SquareDistance(aPnt) > Precision::Confusion())
      return false;
  }
  return true;
}

//=================================================================================================

// Updates max deflection (sagitta) from the last 3 points in the vector.
// theNbPnts is the 1-based count of points; the vector is 0-based.
void updateDeflection(const NCollection_Vector<gp_Pnt2d>& thePnts,
                      const int                           theNbPnts,
                      double&                             theFlecheU,
                      double&                             theFlecheV)
{
  if (theNbPnts < 3)
    return;
  const int aLast  = theNbPnts - 1; // 0-based index of last point
  const int aPrev  = theNbPnts - 2;
  const int aPPrev = theNbPnts - 3;
  if (thePnts(aPPrev).SquareDistance(thePnts(aLast)) == 0.0)
    return;
  gp_Lin2d       aLin(thePnts(aPPrev), gp_Dir2d(gp_Vec2d(thePnts(aPPrev), thePnts(aLast))));
  const double   aLinParam = ElCLib::Parameter(aLin, thePnts(aPrev));
  const gp_Pnt2d aProjPnt  = ElCLib::Value(aLinParam, aLin);
  const double   aDevU     = std::abs(aProjPnt.X() - thePnts(aPrev).X());
  const double   aDevV     = std::abs(aProjPnt.Y() - thePnts(aPrev).Y());
  if (aDevU > theFlecheU)
    theFlecheU = aDevU;
  if (aDevV > theFlecheV)
    theFlecheV = aDevV;
}

//=================================================================================================

// Computes signed area (shoelace formula) and perimeter from a 0-based point vector.
// Copies points into the 1-based aPClass array simultaneously.
void computeAreaPerimeter(const NCollection_Vector<gp_Pnt2d>& thePnts,
                          const int                           theNbPnts,
                          NCollection_Array1<gp_Pnt2d>&       thePClass,
                          double&                             theSquare,
                          double&                             thePer)
{
  theSquare = 0.0;
  thePer    = 0.0;

  if (theNbPnts < 3)
    return;

  int anIdxM1          = theNbPnts - 1;
  int anIdxM0          = 1;
  thePClass(anIdxM1)   = thePnts.Value(anIdxM1 - 1);
  thePClass(theNbPnts) = thePnts.Value(theNbPnts - 1);

  for (int anIdx = 1; anIdx < theNbPnts; anIdx++, anIdxM0++, anIdxM1++)
  {
    if (anIdxM1 >= theNbPnts)
      anIdxM1 = 1;
    thePClass(anIdx) = thePnts.Value(anIdx - 1);
    theSquare += (thePClass(anIdxM0).X() - thePClass(anIdxM1).X())
                 * (thePClass(anIdxM0).Y() + thePClass(anIdxM1).Y()) * 0.5;
    thePer += (thePClass(anIdxM0).XY() - thePClass(anIdxM1).XY()).Modulus();
  }
}
} // namespace

//=================================================================================================

BRepGraphAlgo_FClass2d::BRepGraphAlgo_FClass2d(const BRepGraph& theGraph,
                                               const int        theFaceDefIdx,
                                               const double     theTolUV)
    : myTolUV(theTolUV),
      myGraph(&theGraph),
      myFaceDefIdx(theFaceDefIdx)
{
  const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Defs().Face(theFaceDefIdx);

  // Surface type and periodicity.
  // Geometry is stored at identity; location offsets are absorbed into usages.
  if (aFaceDef.SurfaceRepIdx < 0)
    return;
  const occ::handle<Geom_Surface>& aFaceSurf =
    theGraph.Defs().SurfaceRep(aFaceDef.SurfaceRepIdx).Surface;
  GeomAdaptor_TransformedSurface aSurfAdaptor(aFaceSurf, gp_Trsf());

  myIsUPer  = aSurfAdaptor.IsUPeriodic();
  myIsVPer  = aSurfAdaptor.IsVPeriodic();
  myUPeriod = myIsUPer ? aSurfAdaptor.UPeriod() : 0.0;
  myVPeriod = myIsVPer ? aSurfAdaptor.VPeriod() : 0.0;

  // Get wires via incidence refs.
  if (aFaceDef.WireRefs.IsEmpty())
    return;

  // Collect wire indices: outer wire first, then inner wires.
  const int aNbWires = aFaceDef.WireRefs.Length();
  NCollection_Array1<int> aWireDefIndices(0, aNbWires - 1);
  int aWireFillIdx = 0;
  // Put outer wire first.
  for (int i = 0; i < aFaceDef.WireRefs.Length(); ++i)
  {
    if (aFaceDef.WireRefs.Value(i).IsOuter)
    {
      aWireDefIndices(aWireFillIdx++) = aFaceDef.WireRefs.Value(i).WireIdx;
      break;
    }
  }
  // Then inner wires.
  for (int i = 0; i < aFaceDef.WireRefs.Length(); ++i)
  {
    if (!aFaceDef.WireRefs.Value(i).IsOuter)
      aWireDefIndices(aWireFillIdx++) = aFaceDef.WireRefs.Value(i).WireIdx;
  }

  myUmin = myVmin = 0.0;
  myUmax = myVmax = 0.0;

  constexpr double             THE_EPS     = 1.e-10;
  bool                         anIsBadWire = false;
  NCollection_Vector<gp_Pnt2d> aPnt2dVec;

  for (int aWireIdx = 0; aWireIdx < aNbWires && !anIsBadWire; ++aWireIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWireDef =
      theGraph.Defs().Wire(aWireDefIndices(aWireIdx));
    int aNbPnts = 0;
    aPnt2dVec.Clear();
    int    aFirstPoint     = 1;
    double aFlecheU        = 0.0;
    double aFlecheV        = 0.0;
    bool   aWireIsNotEmpty = false;

    // Build connection-ordered edge sequence using WireExplorer.
    auto edgeLookup = [&theGraph](int theIdx) -> const BRepGraphInc::EdgeEntity& {
      return theGraph.Defs().Edge(theIdx);
    };
    auto coedgeLookup = [&theGraph](int theIdx) -> const BRepGraphInc::CoEdgeEntity& {
      return theGraph.Defs().CoEdge(theIdx);
    };
    BRepGraphInc_WireExplorer aWireExp(aWireDef.CoEdgeRefs, coedgeLookup, edgeLookup);
    const NCollection_Vector<BRepGraphInc::CoEdgeRef>& anOrderedRefs = aWireExp.OrderedRefs();

    const int aNbEdgeRefs = anOrderedRefs.Length();
    const int aNbE        = aNbEdgeRefs;

    // Edge count mismatch detection.
    const int aNbEdgeMismatch = 0; // EdgeRefs is the single source of truth now.

    gp_Pnt aOldPnt3d(0, 0, 0);
    bool   anOldPnt3dInit = false;

    // Iterate coedge refs in connection order.
    for (int anEdgeIdx = 0; anEdgeIdx < aNbEdgeRefs; ++anEdgeIdx)
    {
      const BRepGraphInc::CoEdgeRef& aCoEdgeRef = anOrderedRefs.Value(anEdgeIdx);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdgeDef =
        theGraph.Defs().CoEdge(aCoEdgeRef.CoEdgeIdx);
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
        theGraph.Defs().Edge(aCoEdgeDef.EdgeIdx);
      const TopAbs_Orientation anOri = aCoEdgeDef.Sense;

      if (anOri != TopAbs_FORWARD && anOri != TopAbs_REVERSED)
        continue;

      // Resolve 3D curve from rep index.
      const occ::handle<Geom_Curve> anEdgeCurve3d =
        anEdgeDef.Curve3DRepIdx >= 0
          ? theGraph.Defs().Curve3DRep(anEdgeDef.Curve3DRepIdx).Curve
          : occ::handle<Geom_Curve>();

      // PCurve data from CoEdge.
      if (aCoEdgeDef.Curve2DRepIdx < 0)
        return;
      const occ::handle<Geom2d_Curve>& aCurve2d =
        theGraph.Defs().Curve2DRep(aCoEdgeDef.Curve2DRepIdx).Curve;

      double aParamFirst = aCoEdgeDef.ParamFirst;
      double aParamLast  = aCoEdgeDef.ParamLast;

      if (std::abs(aParamLast - aParamFirst) < 1.e-9)
        continue;

      // Edge degeneration check.
      bool anIsDegenerated = false;
      if (anEdgeDef.IsDegenerate)
        anIsDegenerated = true;

      if (aCoEdgeDef.SeamPairIdx >= 0)
        anIsDegenerated = true;

      if (!anEdgeDef.StartVertexDefId().IsValid() || !anEdgeDef.EndVertexDefId().IsValid())
        anIsDegenerated = true;

      // Check 3D curve degeneration if not already flagged.
      if (!anIsDegenerated && anEdgeDef.Curve3DRepIdx >= 0)
      {
        const occ::handle<Geom_Curve>& aEdgeCurve3d =
          theGraph.Defs().Curve3DRep(anEdgeDef.Curve3DRepIdx).Curve;
        anIsDegenerated = isDegenerated(aEdgeCurve3d,
                                        TopLoc_Location(),
                                        anEdgeDef.ParamFirst,
                                        anEdgeDef.ParamLast);
      }

      // Discretize pcurve into 2D points.
      Geom2dAdaptor_Curve aCurveAdaptor2D(aCurve2d, aParamFirst, aParamLast);

      int aNbSamples = Geom2dInt_Geom2dCurveTool::NbSamples(aCurveAdaptor2D);
      if (aNbSamples > 2)
        aNbSamples *= 4;
      double aDu = (aParamLast - aParamFirst) / static_cast<double>(aNbSamples - 1);
      double aU  = 0.0;
      if (anOri == TopAbs_FORWARD)
        aU = aParamFirst;
      else
      {
        aU  = aParamLast;
        aDu = -aDu;
      }

      if (aFirstPoint == 2)
        aU += aDu;
      const int aPrevNbPnts = aNbPnts;
      for (int aSampIdx = aFirstPoint; aSampIdx <= aNbSamples; aSampIdx++)
      {
        gp_Pnt2d aP2d = aCurveAdaptor2D.Value(aU);
        if (aP2d.X() < myUmin)
          myUmin = aP2d.X();
        if (aP2d.X() > myUmax)
          myUmax = aP2d.X();
        if (aP2d.Y() < myVmin)
          myVmin = aP2d.Y();
        if (aP2d.Y() > myVmax)
          myVmax = aP2d.Y();

        double aDist3dOld = 1e+20;
        gp_Pnt aP3d;
        if (!anIsDegenerated && !anEdgeCurve3d.IsNull())
        {
          aP3d = anEdgeCurve3d->Value(aU);
          if (aNbPnts > 1 && anOldPnt3dInit)
            aDist3dOld = aP3d.Distance(aOldPnt3d);
        }
        bool anIsRealCurve3d = true;
        if (aDist3dOld < Precision::Confusion())
        {
          if (!anIsDegenerated && !anEdgeCurve3d.IsNull())
          {
            gp_Pnt aMidP3d = anEdgeCurve3d->Value(aU - aDu / 2.0);
            if (aP3d.Distance(aMidP3d) < Precision::Confusion())
              anIsRealCurve3d = false;
          }
        }
        if (anIsRealCurve3d)
        {
          if (!anIsDegenerated)
          {
            aOldPnt3d      = aP3d;
            anOldPnt3dInit = true;
          }
          aNbPnts++;
          aPnt2dVec.Append(aP2d);
        }

        aU += aDu;
        if (aNbPnts > (aPrevNbPnts + 4))
          updateDeflection(aPnt2dVec, aNbPnts, aFlecheU, aFlecheV);
      }
      if (aFirstPoint == 1)
        aFirstPoint = 2;
      aWireIsNotEmpty = true;
    }

    if (aNbEdgeMismatch != 0)
    {
      // Mismatch between ordered edges and usage edges — bad wire.
      NCollection_Array1<gp_Pnt2d> aPClass(1, 2);
      aPClass.Init(gp_Pnt2d(0., 0.));
      myWireClassifiers.Append(
        CSLib_Class2d(aPClass, aFlecheU, aFlecheV, myUmin, myVmin, myUmax, myVmax));
      anIsBadWire = true;
      myWireOrients.Append(WireOrient::Invalid);
    }
    else if (aWireIsNotEmpty)
    {
      NCollection_Array1<gp_Pnt2d> aPClass(1, aNbPnts);
      double                       aSquare = 0.0;

      if (aNbPnts > 3)
      {
        double aPer = 0.0;
        computeAreaPerimeter(aPnt2dVec, aNbPnts, aPClass, aSquare, aPer);

        // Polygon quality and re-discretization.
        double anExpThick = std::max(2.0 * std::abs(aSquare) / aPer, 1e-7);
        double aDefl      = std::max(aFlecheU, aFlecheV);
        double aDiscrDefl = std::min(aDefl * 0.1, anExpThick * 10.0);
        while (aDefl > anExpThick && aDiscrDefl > 1e-7)
        {
          aFirstPoint = 1;
          aPnt2dVec.Clear();
          aFlecheU = 0.0;
          aFlecheV = 0.0;

          for (int anEdgeIdx = 0; anEdgeIdx < aNbEdgeRefs; ++anEdgeIdx)
          {
            const BRepGraphInc::CoEdgeRef& aCoEdgeRef2 = anOrderedRefs.Value(anEdgeIdx);
            const BRepGraph_TopoNode::CoEdgeDef& aCoEdgeDef2 =
              theGraph.Defs().CoEdge(aCoEdgeRef2.CoEdgeIdx);
            const TopAbs_Orientation anOri = aCoEdgeDef2.Sense;

            if (anOri != TopAbs_FORWARD && anOri != TopAbs_REVERSED)
              continue;

            if (aCoEdgeDef2.Curve2DRepIdx < 0)
              continue;

            const occ::handle<Geom2d_Curve>& aCurve2d2 =
              theGraph.Defs().Curve2DRep(aCoEdgeDef2.Curve2DRepIdx).Curve;
            double aParamFirst = aCoEdgeDef2.ParamFirst;
            double aParamLast  = aCoEdgeDef2.ParamLast;
            if (std::abs(aParamLast - aParamFirst) < 1.e-9)
              continue;

            Geom2dAdaptor_Curve           aC(aCurve2d2, aParamFirst, aParamLast);
            GCPnts_QuasiUniformDeflection aDiscr(aC, aDiscrDefl);
            if (!aDiscr.IsDone())
              break;

            const int aNbDiscrPts = aDiscr.NbPoints();
            int       aStep = 1, aI = 1, anIEnd = aNbDiscrPts + 1;
            if (anOri == TopAbs_REVERSED)
            {
              aStep  = -1;
              aI     = aNbDiscrPts;
              anIEnd = 0;
            }
            if (aFirstPoint == 2)
              aI += aStep;
            for (; aI != anIEnd; aI += aStep)
            {
              gp_Pnt2d aP2d = aC.Value(aDiscr.Parameter(aI));
              aPnt2dVec.Append(aP2d);
            }
            if (aNbDiscrPts > 2)
              updateDeflection(aPnt2dVec, aPnt2dVec.Length(), aFlecheU, aFlecheV);
            aFirstPoint = 2;
          }
          aNbPnts = aPnt2dVec.Length();
          aPClass.Resize(1, aNbPnts, false);
          computeAreaPerimeter(aPnt2dVec, aNbPnts, aPClass, aSquare, aPer);

          anExpThick = std::max(2.0 * std::abs(aSquare) / aPer, 1e-7);
          aDefl      = std::max(aFlecheU, aFlecheV);
          aDiscrDefl = std::min(aDiscrDefl * 0.1, anExpThick * 10.0);
        }

        // Determine wire orientation.
        if (aNbE == 1 && aFlecheU < THE_EPS && aFlecheV < THE_EPS && std::abs(aSquare) < THE_EPS)
        {
          myWireOrients.Append(WireOrient::Outer);
        }
        else
        {
          myWireOrients.Append((aSquare < 0.0) ? WireOrient::Outer : WireOrient::Inner);
        }

        if (aFlecheU < myTolUV)
          aFlecheU = myTolUV;
        if (aFlecheV < myTolUV)
          aFlecheV = myTolUV;
        myWireClassifiers.Append(
          CSLib_Class2d(aPClass, aFlecheU, aFlecheV, myUmin, myVmin, myUmax, myVmax));
      }
      else
      {
        anIsBadWire = true;
        myWireOrients.Append(WireOrient::Invalid);
        const int aBadLen = std::max(2, aPnt2dVec.Length());
        NCollection_Array1<gp_Pnt2d> aBadPClass(1, aBadLen);
        if (aPnt2dVec.Length() >= 1)
          aBadPClass(1) = aPnt2dVec(0);
        if (aPnt2dVec.Length() >= 2)
          aBadPClass(2) = aPnt2dVec(1);
        myWireClassifiers.Append(
          CSLib_Class2d(aBadPClass, aFlecheU, aFlecheV, myUmin, myVmin, myUmax, myVmax));
      }
    }
  }

  const int aNbTabClass = myWireClassifiers.Length();
  if (aNbTabClass > 0)
  {
    // If an error was detected on a wire, set first orientation to Invalid.
    if (anIsBadWire)
      myWireOrients(0) = WireOrient::Invalid;

    // Periodic ranges.
    const GeomAbs_SurfaceType aSurfType = aSurfAdaptor.GetType();
    if (aSurfType == GeomAbs_Cone || aSurfType == GeomAbs_Cylinder || aSurfType == GeomAbs_Torus
        || aSurfType == GeomAbs_Sphere || aSurfType == GeomAbs_SurfaceOfRevolution)
    {
      myIsUPer  = true;
      myUPeriod = 2.0 * M_PI;
    }

    if (aSurfType == GeomAbs_Torus)
    {
      myIsVPer  = true;
      myVPeriod = 2.0 * M_PI;
    }
  }
}

//=================================================================================================

TopAbs_State BRepGraphAlgo_FClass2d::PerformInfinitePoint() const
{
  if (myUmax == 0.0 && myVmax == 0.0 && myUmin == 0.0 && myVmin == 0.0
      && myWireClassifiers.IsEmpty())
  {
    return TopAbs_IN;
  }
  const gp_Pnt2d aPnt(myUmin - (myUmax - myUmin), myVmin - (myVmax - myVmin));
  return Perform(aPnt, false);
}

//=================================================================================================

TopAbs_State BRepGraphAlgo_FClass2d::Perform(const gp_Pnt2d& thePntUV,
                                             const bool      theRecadreOnPeriodic) const
{
  const double aTol = (myTolUV > 4.0) ? 4.0 : myTolUV;
  return classifyInternal(thePntUV, aTol, theRecadreOnPeriodic, false);
}

//=================================================================================================

TopAbs_State BRepGraphAlgo_FClass2d::TestOnRestriction(const gp_Pnt2d& thePntUV,
                                                       const double    theTol,
                                                       const bool      theRecadreOnPeriodic) const
{
  return classifyInternal(thePntUV, theTol, theRecadreOnPeriodic, true);
}

//=================================================================================================

TopAbs_State BRepGraphAlgo_FClass2d::classifyInternal(const gp_Pnt2d& thePntUV,
                                                      const double    theTol,
                                                      const bool      theRecadreOnPeriodic,
                                                      const bool      theUseOnMode) const
{
  const int aNbTabClass = myWireClassifiers.Length();
  if (aNbTabClass == 0)
    return TopAbs_IN;

  double aU  = thePntUV.X();
  double aV  = thePntUV.Y();
  double aUU = aU, aVV = aV;

  TopAbs_State aStatus    = TopAbs_UNKNOWN;
  bool         anURecadre = false, aVRecadre = false;

  if (theRecadreOnPeriodic)
  {
    if (myIsUPer)
    {
      if (aUU < myUmin)
        while (aUU < myUmin)
          aUU += myUPeriod;
      else
      {
        while (aUU >= myUmin)
          aUU -= myUPeriod;
        aUU += myUPeriod;
      }
    }
    if (myIsVPer)
    {
      if (aVV < myVmin)
        while (aVV < myVmin)
          aVV += myVPeriod;
      else
      {
        while (aVV >= myVmin)
          aVV -= myVPeriod;
        aVV += myVPeriod;
      }
    }
  }

  for (;;)
  {
    int      aDedans = 1;
    gp_Pnt2d aPuv(aU, aV);

    if (myWireOrients(0) != WireOrient::Invalid)
    {
      for (int aWIdx = 0; aWIdx < aNbTabClass; aWIdx++)
      {
        const CSLib_Class2d::Result aCur = theUseOnMode
                                             ? myWireClassifiers(aWIdx).SiDans_OnMode(aPuv, theTol)
                                             : myWireClassifiers(aWIdx).SiDans(aPuv);
        if (aCur == CSLib_Class2d::Result_Inside)
        {
          if (myWireOrients(aWIdx) == WireOrient::Inner)
          {
            aDedans = -1;
            break;
          }
        }
        else if (aCur == CSLib_Class2d::Result_Outside)
        {
          if (myWireOrients(aWIdx) == WireOrient::Outer)
          {
            aDedans = -1;
            break;
          }
        }
        else
        {
          aDedans = 0;
          break;
        }
      }
      if (aDedans == 0)
      {
        if (theUseOnMode)
        {
          aStatus = TopAbs_ON;
        }
        else
        {
          // Uncertain — fallback to BRepClass_FaceClassifier via face reconstruction.
          const TopoDS_Shape aReconstructed = myGraph->Shapes().ReconstructFace(myFaceDefIdx);
          if (!aReconstructed.IsNull())
          {
            BRepClass_FaceClassifier aClassifier;
            aClassifier.Perform(TopoDS::Face(aReconstructed), aPuv, theTol);
            aStatus = aClassifier.State();
          }
          else
          {
            aStatus = TopAbs_ON;
          }
        }
      }
      else if (aDedans == 1)
        aStatus = TopAbs_IN;
      else
        aStatus = TopAbs_OUT;
    }
    else
    {
      // Bad wire — fallback to BRepClass_FaceClassifier.
      const TopoDS_Shape aReconstructed = myGraph->Shapes().ReconstructFace(myFaceDefIdx);
      if (!aReconstructed.IsNull())
      {
        BRepClass_FaceClassifier aClassifier;
        aClassifier.Perform(TopoDS::Face(aReconstructed), aPuv, theTol);
        aStatus = aClassifier.State();
      }
      else
      {
        aStatus = TopAbs_IN;
      }
    }

    if (!theRecadreOnPeriodic || (!myIsUPer && !myIsVPer))
      return aStatus;
    if (aStatus == TopAbs_IN || aStatus == TopAbs_ON)
      return aStatus;

    if (!anURecadre)
    {
      aU         = aUU;
      anURecadre = true;
    }
    else if (myIsUPer)
      aU += myUPeriod;
    if (aU > myUmax || !myIsUPer)
    {
      if (!aVRecadre)
      {
        aV        = aVV;
        aVRecadre = true;
      }
      else if (myIsVPer)
        aV += myVPeriod;

      aU = aUU;

      if (aV > myVmax || !myIsVPer)
        return aStatus;
    }
  }
}
