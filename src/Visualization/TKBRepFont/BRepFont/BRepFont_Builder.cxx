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

#include <BRepFont_Builder.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <GC_MakeSegment2d.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomLib.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <array>
#include <cmath>
#include <optional>
#include <utility>

namespace
{
//=================================================================================================

bool isFiniteScaled(const gp_XY& thePoint, const double theScale) noexcept
{
  const double aX = thePoint.X() * theScale;
  const double aY = thePoint.Y() * theScale;
  return std::isfinite(aX) && std::isfinite(aY) && !Precision::IsInfinite(aX)
         && !Precision::IsInfinite(aY);
}

//=================================================================================================

occ::handle<Geom2d_Curve> makeCurve2d(const BRepFont_PlanarRegion::Curve& theCurve,
                                      const double                        theScale)
{
  if (!std::isfinite(theScale) || !(theScale > 0.0) || !isFiniteScaled(theCurve.Start(), theScale)
      || !isFiniteScaled(theCurve.End(), theScale)
      || (theCurve.Type() != Font_GlyphOutline::Segment::Kind::Line
          && !isFiniteScaled(theCurve.Control1(), theScale))
      || (theCurve.Type() == Font_GlyphOutline::Segment::Kind::CubicBezier
          && !isFiniteScaled(theCurve.Control2(), theScale)))
  {
    return {};
  }

  if (theCurve.Type() == Font_GlyphOutline::Segment::Kind::Line)
  {
    const gp_Pnt2d   aFirst(theCurve.Start().X() * theScale, theCurve.Start().Y() * theScale);
    const gp_Pnt2d   aLast(theCurve.End().X() * theScale, theCurve.End().Y() * theScale);
    GC_MakeSegment2d aMaker(aFirst, aLast);
    return aMaker.IsDone() ? aMaker.Value() : occ::handle<Geom2d_Curve>();
  }

  const int aNbPoles = theCurve.Type() == Font_GlyphOutline::Segment::Kind::QuadraticBezier ? 3 : 4;
  std::array<gp_Pnt2d, 4> aPoleStorage;
  aPoleStorage[0].SetCoord(theCurve.Start().X() * theScale, theCurve.Start().Y() * theScale);
  aPoleStorage[1].SetCoord(theCurve.Control1().X() * theScale, theCurve.Control1().Y() * theScale);
  if (aNbPoles == 3)
  {
    aPoleStorage[2].SetCoord(theCurve.End().X() * theScale, theCurve.End().Y() * theScale);
  }
  else
  {
    aPoleStorage[2].SetCoord(theCurve.Control2().X() * theScale,
                             theCurve.Control2().Y() * theScale);
    aPoleStorage[3].SetCoord(theCurve.End().X() * theScale, theCurve.End().Y() * theScale);
  }
  const NCollection_Array1<gp_Pnt2d> aPoles(aPoleStorage.data(), static_cast<size_t>(aNbPoles));
  return new Geom2d_BezierCurve(aPoles);
}

//=================================================================================================

double curveParameter(const occ::handle<Geom2d_Curve>& theCurve,
                      const double                     theNormalizedParameter) noexcept
{
  return theCurve->FirstParameter()
         + (theCurve->LastParameter() - theCurve->FirstParameter()) * theNormalizedParameter;
}

//=================================================================================================

std::optional<TopoDS_Wire> buildWire(
  const BRepFont_PlanarRegion&                               theRegion,
  const uint32_t                                             theLoop,
  const double                                               theTolerance,
  const occ::handle<Geom_Surface>&                           theSurface,
  const NCollection_LinearVector<TopoDS_Vertex>&             theVertices,
  const NCollection_LinearVector<occ::handle<Geom2d_Curve>>& theCurves2d,
  const NCollection_LinearVector<occ::handle<Geom_Curve>>&   theCurves3d,
  const bool                                                 theToConcatenateContours)
{
  if (theLoop >= theRegion.Loops().Size())
  {
    return std::nullopt;
  }

  const BRepFont_PlanarRegion::Loop& aLoop = theRegion.Loops()[theLoop];
  if (aLoop.NbUses() == 0
      || static_cast<uint64_t>(aLoop.FirstUse()) + aLoop.NbUses() > theRegion.Uses().Size())
  {
    return std::nullopt;
  }
  BRepBuilderAPI_MakeWire               aWireMaker;
  BRep_Builder                          aBuilder;
  Geom2dConvert_CompCurveToBSplineCurve aCompositeMaker;

  for (uint32_t anOffset = 0; anOffset < aLoop.NbUses(); ++anOffset)
  {
    const uint32_t                    aUseIndex    = aLoop.FirstUse() + anOffset;
    const BRepFont_PlanarRegion::Use& aUse         = theRegion.Uses()[aUseIndex];
    const uint32_t                    aCurveIndex  = aUse.CurveIndex().Value();
    const uint32_t                    aFirstVertex = aUse.FirstVertex().Value();
    const uint32_t                    aLastVertex  = aUse.LastVertex().Value();
    if (aCurveIndex >= theCurves2d.Size() || aFirstVertex >= theRegion.Vertices().Size()
        || aLastVertex >= theRegion.Vertices().Size())
    {
      return std::nullopt;
    }

    const occ::handle<Geom2d_Curve>& aCurve2d = theCurves2d[aCurveIndex];
    if (aCurve2d.IsNull() || !(aCurve2d->LastParameter() > aCurve2d->FirstParameter())
        || (!theToConcatenateContours
            && (aCurveIndex >= theCurves3d.Size() || theCurves3d[aCurveIndex].IsNull())))
    {
      return std::nullopt;
    }
    if (aUse.FirstParameter() < 0.0 || aUse.LastParameter() > 1.0
        || !(aUse.LastParameter() > aUse.FirstParameter()))
    {
      return std::nullopt;
    }
    const double aFirstParameter = curveParameter(aCurve2d, aUse.FirstParameter());
    const double aLastParameter  = curveParameter(aCurve2d, aUse.LastParameter());
    if (theToConcatenateContours)
    {
      const occ::handle<Geom2d_TrimmedCurve> aUseCurve =
        new Geom2d_TrimmedCurve(aCurve2d, aFirstParameter, aLastParameter, !aUse.IsReversed());
      if (!aCompositeMaker.Add(aUseCurve, theTolerance, true))
      {
        return std::nullopt;
      }
      continue;
    }

    if (aFirstVertex >= theVertices.Size() || aLastVertex >= theVertices.Size())
    {
      return std::nullopt;
    }

    const TopoDS_Vertex& anEdgeFirst =
      aUse.IsReversed() ? theVertices[aLastVertex] : theVertices[aFirstVertex];
    const TopoDS_Vertex& anEdgeLast =
      aUse.IsReversed() ? theVertices[aFirstVertex] : theVertices[aLastVertex];
    BRepLib_MakeEdge anEdgeMaker(theCurves3d[aCurveIndex],
                                 anEdgeFirst,
                                 anEdgeLast,
                                 aFirstParameter,
                                 aLastParameter);
    if (!anEdgeMaker.IsDone())
    {
      return std::nullopt;
    }
    TopoDS_Edge anEdge = anEdgeMaker.Edge();
    aBuilder.UpdateEdge(anEdge, aCurve2d, theSurface, TopLoc_Location(), theTolerance);
    if (aUse.IsReversed())
    {
      anEdge.Reverse();
    }
    aWireMaker.Add(anEdge);
    if (!aWireMaker.IsDone())
    {
      return std::nullopt;
    }
  }
  if (theToConcatenateContours)
  {
    const occ::handle<Geom2d_BSplineCurve> aCurve2d = aCompositeMaker.BSplineCurve();
    if (aCurve2d.IsNull())
    {
      return std::nullopt;
    }
    const occ::handle<Geom_Curve> aCurve3d = GeomLib::To3d(gp::XOY(), aCurve2d);
    if (aCurve3d.IsNull())
    {
      return std::nullopt;
    }
    BRepLib_MakeEdge anEdgeMaker(aCurve3d);
    if (!anEdgeMaker.IsDone())
    {
      return std::nullopt;
    }
    TopoDS_Edge anEdge = anEdgeMaker.Edge();
    aBuilder.UpdateEdge(anEdge, aCurve2d, theSurface, TopLoc_Location(), theTolerance);
    aWireMaker.Add(anEdge);
    if (!aWireMaker.IsDone())
    {
      return std::nullopt;
    }
  }
  return aWireMaker.Wire();
}
} // namespace

//=================================================================================================

TopoDS_Shape BRepFont_Builder::BuildGlyph(const BRepFont_PlanarRegion& theRegion,
                                          const GlyphOptions&          theOptions)
{
  const bool aToConcatenateContours = theOptions.ToConcatenateContours;
  if (!std::isfinite(theRegion.UnitsPerEm()) || !(theRegion.UnitsPerEm() > 0.0)
      || !std::isfinite(theOptions.Size) || !(theOptions.Size > 0.0)
      || !std::isfinite(theOptions.Tolerance) || !(theOptions.Tolerance > 0.0))
  {
    return {};
  }
  if (theRegion.IsEmpty())
  {
    return {};
  }

  const double aScale = theOptions.Size / theRegion.UnitsPerEm();
  if (!std::isfinite(aScale))
  {
    return {};
  }
  const occ::handle<Geom_Surface>         aSurface = new Geom_Plane(gp_Pln(gp::XOY()));
  NCollection_LinearVector<TopoDS_Vertex> aVertices(
    aToConcatenateContours ? 0 : theRegion.Vertices().Size());
  BRep_Builder aBuilder;
  for (const gp_XY& aPoint : theRegion.Vertices())
  {
    const double aX = aPoint.X() * aScale;
    const double aY = aPoint.Y() * aScale;
    if (!std::isfinite(aX) || !std::isfinite(aY) || Precision::IsInfinite(aX)
        || Precision::IsInfinite(aY))
    {
      return {};
    }
    if (!aToConcatenateContours)
    {
      TopoDS_Vertex aVertex;
      aBuilder.MakeVertex(aVertex, gp_Pnt(aX, aY, 0.0), theOptions.Tolerance);
      aVertices.Append(aVertex);
    }
  }
  NCollection_LinearVector<TopoDS_Wire>               aWires(theRegion.Loops().Size());
  NCollection_LinearVector<occ::handle<Geom2d_Curve>> aCurves2d(theRegion.Curves().Size());
  NCollection_LinearVector<occ::handle<Geom_Curve>>   aCurves3d(
    aToConcatenateContours ? 0 : theRegion.Curves().Size());
  for (const BRepFont_PlanarRegion::Curve& aCurve : theRegion.Curves())
  {
    const occ::handle<Geom2d_Curve> aCurve2d = makeCurve2d(aCurve, aScale);
    if (aCurve2d.IsNull() || !(aCurve2d->LastParameter() > aCurve2d->FirstParameter()))
    {
      return {};
    }
    aCurves2d.Append(aCurve2d);
    if (!aToConcatenateContours)
    {
      const occ::handle<Geom_Curve> aCurve3d = GeomLib::To3d(gp::XOY(), aCurve2d);
      if (aCurve3d.IsNull())
      {
        return {};
      }
      aCurves3d.Append(aCurve3d);
    }
  }
  for (uint32_t aLoopIndex = 0; aLoopIndex < theRegion.Loops().Size(); ++aLoopIndex)
  {
    std::optional<TopoDS_Wire> aWire = buildWire(theRegion,
                                                 aLoopIndex,
                                                 theOptions.Tolerance,
                                                 aSurface,
                                                 aVertices,
                                                 aCurves2d,
                                                 aCurves3d,
                                                 aToConcatenateContours);
    if (!aWire.has_value())
    {
      return {};
    }
    aWires.Append(std::move(*aWire));
  }

  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  if (theRegion.IsSingleStroke())
  {
    for (const TopoDS_Wire& aWire : aWires)
    {
      aBuilder.Add(aCompound, aWire);
    }
  }
  else
  {
    for (const BRepFont_PlanarRegion::Region& aRegion : theRegion.Regions())
    {
      const uint32_t anOuterIndex = aRegion.OuterLoop().Value();
      if (anOuterIndex >= aWires.Size())
      {
        return {};
      }
      TopoDS_Wire anOuterWire = aWires[anOuterIndex];
      if (theRegion.Loops()[anOuterIndex].SignedArea() < 0.0)
      {
        anOuterWire.Reverse();
      }
      TopoDS_Face aFace;
      aBuilder.MakeFace(aFace, aSurface, theOptions.Tolerance);
      aBuilder.Add(aFace, anOuterWire);
      if (static_cast<uint64_t>(aRegion.FirstHole()) + aRegion.NbHoles()
          > theRegion.HoleLoops().Size())
      {
        return {};
      }
      for (uint32_t aHoleOffset = 0; aHoleOffset < aRegion.NbHoles(); ++aHoleOffset)
      {
        const BRepFont_PlanarRegion::LoopId& aHoleId =
          theRegion.HoleLoops()[aRegion.FirstHole() + aHoleOffset];
        if (aHoleId.Value() >= aWires.Size())
        {
          return {};
        }
        const uint32_t aHoleIndex = aHoleId.Value();
        TopoDS_Wire    aHoleWire  = aWires[aHoleIndex];
        if (theRegion.Loops()[aHoleIndex].SignedArea() > 0.0)
        {
          aHoleWire.Reverse();
        }
        aBuilder.Add(aFace, aHoleWire);
      }
      aBuilder.Add(aCompound, aFace);
    }
  }

  if (aCompound.NbChildren() == 0)
  {
    return {};
  }
  if (aCompound.NbChildren() == 1)
  {
    return TopoDS_Iterator(aCompound).Value();
  }
  else
  {
    return aCompound;
  }
}

namespace
{
//=================================================================================================

std::optional<int> arrayLength(const size_t theValue)
{
  const int aLength = static_cast<int>(theValue);
  return aLength >= 0 && static_cast<size_t>(aLength) == theValue ? std::optional<int>(aLength)
                                                                  : std::nullopt;
}

//=================================================================================================

bool isStructurallyValid(const BRepFont_PlanarRegion& theRegion)
{
  for (const BRepFont_PlanarRegion::Use& aUse : theRegion.Uses())
  {
    if (aUse.CurveIndex().Value() >= theRegion.Curves().Size()
        || aUse.FirstVertex().Value() >= theRegion.Vertices().Size()
        || aUse.LastVertex().Value() >= theRegion.Vertices().Size() || aUse.FirstParameter() < 0.0
        || aUse.LastParameter() > 1.0 || !(aUse.LastParameter() > aUse.FirstParameter()))
    {
      return false;
    }
  }
  for (const BRepFont_PlanarRegion::Loop& aLoop : theRegion.Loops())
  {
    if (aLoop.NbUses() == 0 || !arrayLength(aLoop.NbUses()).has_value()
        || static_cast<uint64_t>(aLoop.FirstUse()) + aLoop.NbUses() > theRegion.Uses().Size())
    {
      return false;
    }
  }
  if (theRegion.IsSingleStroke())
  {
    return !theRegion.Loops().IsEmpty() && arrayLength(theRegion.Loops().Size()).has_value();
  }
  if (theRegion.Regions().IsEmpty() || !arrayLength(theRegion.Regions().Size()).has_value())
  {
    return false;
  }
  for (const BRepFont_PlanarRegion::Region& aRegion : theRegion.Regions())
  {
    if (aRegion.OuterLoop().Value() >= theRegion.Loops().Size()
        || !arrayLength(aRegion.NbHoles()).has_value()
        || static_cast<uint64_t>(aRegion.FirstHole()) + aRegion.NbHoles()
             > theRegion.HoleLoops().Size())
    {
      return false;
    }
    for (uint32_t aHoleOffset = 0; aHoleOffset < aRegion.NbHoles(); ++aHoleOffset)
    {
      if (theRegion.HoleLoops()[aRegion.FirstHole() + aHoleOffset].Value()
          >= theRegion.Loops().Size())
      {
        return false;
      }
    }
  }
  return true;
}
} // namespace

//=================================================================================================

BRepGraph_NodeId BRepFont_Builder::AddGlyph(BRepGraph&                   theGraph,
                                            const BRepFont_PlanarRegion& theRegion,
                                            const GlyphOptions&          theOptions)
{
  if (!std::isfinite(theRegion.UnitsPerEm()) || !(theRegion.UnitsPerEm() > 0.0)
      || !std::isfinite(theOptions.Size) || !(theOptions.Size > 0.0)
      || !std::isfinite(theOptions.Tolerance) || !(theOptions.Tolerance > 0.0))
  {
    return {};
  }
  if (theRegion.IsEmpty() || !isStructurallyValid(theRegion))
  {
    return {};
  }

  const double aScale = theOptions.Size / theRegion.UnitsPerEm();
  if (!std::isfinite(aScale))
  {
    return {};
  }
  NCollection_LinearVector<gp_Pnt> aPoints(theRegion.Vertices().Size());
  for (const gp_XY& aVertex : theRegion.Vertices())
  {
    const double aX = aVertex.X() * aScale;
    const double aY = aVertex.Y() * aScale;
    if (!std::isfinite(aX) || !std::isfinite(aY) || Precision::IsInfinite(aX)
        || Precision::IsInfinite(aY))
    {
      return {};
    }
    aPoints.Append(gp_Pnt(aX, aY, 0.0));
  }

  NCollection_LinearVector<occ::handle<Geom2d_Curve>> aCurves2d(theRegion.Curves().Size());
  NCollection_LinearVector<occ::handle<Geom_Curve>>   aCurves3d(theRegion.Curves().Size());
  for (const BRepFont_PlanarRegion::Curve& aCurve : theRegion.Curves())
  {
    const occ::handle<Geom2d_Curve> aCurve2d = makeCurve2d(aCurve, aScale);
    if (aCurve2d.IsNull() || !(aCurve2d->LastParameter() > aCurve2d->FirstParameter()))
    {
      return {};
    }
    const occ::handle<Geom_Curve> aCurve3d = GeomLib::To3d(gp::XOY(), aCurve2d);
    if (aCurve3d.IsNull())
    {
      return {};
    }
    aCurves2d.Append(aCurve2d);
    aCurves3d.Append(aCurve3d);
  }

  BRepGraph::EditorView&                       anEditor = theGraph.Editor();
  const occ::handle<Geom_Surface>              aSurface = new Geom_Plane(gp_Pln(gp::XOY()));
  NCollection_LinearVector<BRepGraph_VertexId> aVertices(aPoints.Size());
  for (const gp_Pnt& aPoint : aPoints)
  {
    const BRepGraph_VertexId aVertexId = anEditor.Vertices().Add(aPoint, theOptions.Tolerance);
    if (!aVertexId.IsValid())
    {
      return {};
    }
    aVertices.Append(aVertexId);
  }

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdges(theRegion.Uses().Size());
  for (uint32_t aUseIndex = 0; aUseIndex < theRegion.Uses().Size(); ++aUseIndex)
  {
    const BRepFont_PlanarRegion::Use& aUse         = theRegion.Uses()[aUseIndex];
    const uint32_t                    aCurveIndex  = aUse.CurveIndex().Value();
    const uint32_t                    aFirstVertex = aUse.FirstVertex().Value();
    const uint32_t                    aLastVertex  = aUse.LastVertex().Value();
    if (aCurveIndex >= aCurves2d.Size() || aCurveIndex >= aCurves3d.Size()
        || aFirstVertex >= aVertices.Size() || aLastVertex >= aVertices.Size())
    {
      return {};
    }

    const occ::handle<Geom2d_Curve>& aCurve2d = aCurves2d[aCurveIndex];
    const occ::handle<Geom_Curve>&   aCurve3d = aCurves3d[aCurveIndex];
    if (aCurve2d.IsNull() || aCurve3d.IsNull())
    {
      return {};
    }
    if (aUse.FirstParameter() < 0.0 || aUse.LastParameter() > 1.0
        || !(aUse.LastParameter() > aUse.FirstParameter()))
    {
      return {};
    }
    const double             aFirstParameter = curveParameter(aCurve2d, aUse.FirstParameter());
    const double             aLastParameter  = curveParameter(aCurve2d, aUse.LastParameter());
    const BRepGraph_VertexId anEdgeFirst =
      aUse.IsReversed() ? aVertices[aLastVertex] : aVertices[aFirstVertex];
    const BRepGraph_VertexId anEdgeLast =
      aUse.IsReversed() ? aVertices[aFirstVertex] : aVertices[aLastVertex];
    const BRepGraph_EdgeId anEdge = anEditor.Edges().Add(anEdgeFirst,
                                                         anEdgeLast,
                                                         aCurve3d,
                                                         aFirstParameter,
                                                         aLastParameter,
                                                         theOptions.Tolerance);
    if (!anEdge.IsValid())
    {
      return {};
    }
    const BRepGraph_CoEdgeId aCoEdge =
      anEditor.CoEdges().Add(anEdge, aUse.IsReversed() ? TopAbs_REVERSED : TopAbs_FORWARD);
    if (!aCoEdge.IsValid())
    {
      return {};
    }
    aCoEdges.Append(aCoEdge);
  }

  NCollection_LinearVector<BRepGraph_WireId> aWires(theRegion.Loops().Size());
  for (const BRepFont_PlanarRegion::Loop& aLoop : theRegion.Loops())
  {
    const std::optional<int> aNbUses = arrayLength(aLoop.NbUses());
    if (aLoop.NbUses() == 0
        || static_cast<uint64_t>(aLoop.FirstUse()) + aLoop.NbUses() > aCoEdges.Size())
    {
      return {};
    }
    if (!aNbUses.has_value())
    {
      return {};
    }
    NCollection_Array1<BRepGraph_CoEdgeId> aWireCoEdges(1, *aNbUses);
    for (uint32_t anOffset = 0; anOffset < aLoop.NbUses(); ++anOffset)
    {
      aWireCoEdges.SetValue(static_cast<int>(anOffset + 1), aCoEdges[aLoop.FirstUse() + anOffset]);
    }
    const BRepGraph_WireId aWire = anEditor.Wires().Add(aWireCoEdges);
    if (!aWire.IsValid())
    {
      return {};
    }
    aWires.Append(aWire);
  }

  NCollection_LinearVector<BRepGraph_NodeId> aRoots;
  if (theRegion.IsSingleStroke())
  {
    aRoots.Reserve(aWires.Size());
    for (const BRepGraph_WireId aWire : aWires)
    {
      aRoots.Append(BRepGraph_NodeId(aWire));
    }
  }
  else
  {
    aRoots.Reserve(theRegion.Regions().Size());
    for (const BRepFont_PlanarRegion::Region& aRegion : theRegion.Regions())
    {
      if (aRegion.OuterLoop().Value() >= aWires.Size())
      {
        return {};
      }
      const uint32_t anOuterLoopIndex = aRegion.OuterLoop().Value();
      if (theRegion.Loops()[anOuterLoopIndex].SignedArea() < 0.0)
      {
        anEditor.Wires().Reverse(aWires[anOuterLoopIndex]);
      }
      if (static_cast<uint64_t>(aRegion.FirstHole()) + aRegion.NbHoles()
          > theRegion.HoleLoops().Size())
      {
        return {};
      }
      NCollection_Array1<BRepGraph_WireId> aHoleWires;
      if (aRegion.NbHoles() > 0)
      {
        const std::optional<int> aNbHoles = arrayLength(aRegion.NbHoles());
        if (!aNbHoles.has_value())
        {
          return {};
        }
        aHoleWires.Resize(1, *aNbHoles, false);
        for (uint32_t aHoleOffset = 0; aHoleOffset < aRegion.NbHoles(); ++aHoleOffset)
        {
          const BRepFont_PlanarRegion::LoopId& aHole =
            theRegion.HoleLoops()[aRegion.FirstHole() + aHoleOffset];
          if (aHole.Value() >= aWires.Size())
          {
            return {};
          }
          const uint32_t aHoleLoopIndex = aHole.Value();
          if (theRegion.Loops()[aHoleLoopIndex].SignedArea() > 0.0)
          {
            anEditor.Wires().Reverse(aWires[aHoleLoopIndex]);
          }
          aHoleWires.SetValue(static_cast<int>(aHoleOffset + 1), aWires[aHoleLoopIndex]);
        }
      }
      const BRepGraph_FaceId aFace =
        anEditor.Faces().Add(aSurface, aWires[anOuterLoopIndex], aHoleWires, theOptions.Tolerance);
      if (!aFace.IsValid())
      {
        return {};
      }
      aRoots.Append(BRepGraph_NodeId(aFace));

      const BRepFont_PlanarRegion::Loop& anOuterLoop = theRegion.Loops()[anOuterLoopIndex];
      for (uint32_t anOffset = 0; anOffset < anOuterLoop.NbUses(); ++anOffset)
      {
        const uint32_t aUseIndex = anOuterLoop.FirstUse() + anOffset;
        anEditor.CoEdges().SetFaceId(aCoEdges[aUseIndex], aFace);
        const BRepFont_PlanarRegion::Use& aUse     = theRegion.Uses()[aUseIndex];
        const occ::handle<Geom2d_Curve>&  aCurve2d = aCurves2d[aUse.CurveIndex().Value()];
        anEditor.CoEdges().SetPCurve(aCoEdges[aUseIndex],
                                     aCurve2d,
                                     curveParameter(aCurve2d, aUse.FirstParameter()),
                                     curveParameter(aCurve2d, aUse.LastParameter()));
      }
      for (uint32_t aHoleOffset = 0; aHoleOffset < aRegion.NbHoles(); ++aHoleOffset)
      {
        const uint32_t aHoleIndex =
          theRegion.HoleLoops()[aRegion.FirstHole() + aHoleOffset].Value();
        const BRepFont_PlanarRegion::Loop& aHoleLoop = theRegion.Loops()[aHoleIndex];
        for (uint32_t anOffset = 0; anOffset < aHoleLoop.NbUses(); ++anOffset)
        {
          const uint32_t aUseIndex = aHoleLoop.FirstUse() + anOffset;
          anEditor.CoEdges().SetFaceId(aCoEdges[aUseIndex], aFace);
          const BRepFont_PlanarRegion::Use& aUse     = theRegion.Uses()[aUseIndex];
          const occ::handle<Geom2d_Curve>&  aCurve2d = aCurves2d[aUse.CurveIndex().Value()];
          anEditor.CoEdges().SetPCurve(aCoEdges[aUseIndex],
                                       aCurve2d,
                                       curveParameter(aCurve2d, aUse.FirstParameter()),
                                       curveParameter(aCurve2d, aUse.LastParameter()));
        }
      }
    }
  }

  if (aRoots.IsEmpty())
  {
    return {};
  }
  if (aRoots.Size() == 1)
  {
    return aRoots[0];
  }
  else
  {
    const std::optional<int> aNbRoots = arrayLength(aRoots.Size());
    if (!aNbRoots.has_value())
    {
      return {};
    }
    NCollection_Array1<BRepGraph_NodeId> aRootArray(1, *aNbRoots);
    for (size_t aRootIndex = 0; aRootIndex < aRoots.Size(); ++aRootIndex)
    {
      aRootArray.SetValue(static_cast<int>(aRootIndex + 1), aRoots[aRootIndex]);
    }
    const BRepGraph_CompoundId aCompound = anEditor.Compounds().Add(aRootArray);
    if (!aCompound.IsValid())
    {
      return {};
    }
    return BRepGraph_NodeId(aCompound);
  }
}

namespace
{
//=================================================================================================

bool isFinite(const gp_Ax3& thePen) noexcept
{
  return std::isfinite(thePen.Location().X()) && std::isfinite(thePen.Location().Y())
         && std::isfinite(thePen.Location().Z()) && std::isfinite(thePen.Direction().X())
         && std::isfinite(thePen.Direction().Y()) && std::isfinite(thePen.Direction().Z())
         && std::isfinite(thePen.XDirection().X()) && std::isfinite(thePen.XDirection().Y())
         && std::isfinite(thePen.XDirection().Z());
}

//=================================================================================================

bool isIdentity(const gp_Trsf& theTransformation) noexcept
{
  for (int aRow = 1; aRow <= 3; ++aRow)
  {
    for (int aColumn = 1; aColumn <= 4; ++aColumn)
    {
      const double anIdentityValue = aColumn == aRow ? 1.0 : 0.0;
      if (theTransformation.Value(aRow, aColumn) != anIdentityValue)
      {
        return false;
      }
    }
  }
  return true;
}
} // namespace

//=================================================================================================

BRepFont_Builder::TextPlan::TextPlan(
  const double                                                             theSize,
  const double                                                             theTolerance,
  NCollection_LinearVector<std::shared_ptr<const BRepFont_PlanarRegion>>&& theRegions,
  NCollection_LinearVector<Item>&&                                         theItems)
    : mySize(theSize),
      myTolerance(theTolerance),
      myRegions(std::move(theRegions)),
      myItems(std::move(theItems))
{
}

//=================================================================================================

std::optional<BRepFont_Builder::TextPlan> BRepFont_Builder::CreateTextPlan(
  const double                                                             theSize,
  const double                                                             theTolerance,
  NCollection_LinearVector<std::shared_ptr<const BRepFont_PlanarRegion>>&& theRegions,
  NCollection_LinearVector<TextPlan::Item>&&                               theItems)
{
  if (!std::isfinite(theSize) || !(theSize > 0.0) || !std::isfinite(theTolerance)
      || !(theTolerance > 0.0))
  {
    return std::nullopt;
  }
  for (const std::shared_ptr<const BRepFont_PlanarRegion>& aRegion : theRegions)
  {
    if (!aRegion || !std::isfinite(aRegion->UnitsPerEm()) || !(aRegion->UnitsPerEm() > 0.0)
        || !std::isfinite(aRegion->Tolerance()) || !(aRegion->Tolerance() > 0.0))
    {
      return std::nullopt;
    }
  }
  for (const TextPlan::Item& anItem : theItems)
  {
    if (anItem.Region() >= theRegions.Size() || !std::isfinite(anItem.Position().X())
        || !std::isfinite(anItem.Position().Y()))
    {
      return std::nullopt;
    }
  }
  return TextPlan(theSize, theTolerance, std::move(theRegions), std::move(theItems));
}

//=================================================================================================

TopoDS_Shape BRepFont_Builder::BuildText(const TextPlan& thePlan)
{
  return BuildText(thePlan, TextOptions{});
}

//=================================================================================================

TopoDS_Shape BRepFont_Builder::BuildText(const TextPlan& thePlan, const TextOptions& theOptions)
{
  if (!std::isfinite(thePlan.Size()) || !(thePlan.Size() > 0.0)
      || !std::isfinite(thePlan.Tolerance()) || !(thePlan.Tolerance() > 0.0)
      || !isFinite(theOptions.Pen) || thePlan.Items().IsEmpty())
  {
    return {};
  }

  NCollection_LinearVector<std::optional<TopoDS_Shape>> aGlyphShapes;
  aGlyphShapes.Resize(thePlan.NbRegions());
  GlyphOptions aGlyphOptions;
  aGlyphOptions.Size                  = thePlan.Size();
  aGlyphOptions.Tolerance             = thePlan.Tolerance();
  aGlyphOptions.ToConcatenateContours = theOptions.ToConcatenateContours;
  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  gp_Trsf aPenTransform;
  aPenTransform.SetTransformation(theOptions.Pen, gp_Ax3(gp::XOY()));
  for (const TextPlan::Item& anItem : thePlan.Items())
  {
    if (anItem.Region() >= aGlyphShapes.Size())
    {
      return {};
    }
    std::optional<TopoDS_Shape>& aCachedShape = aGlyphShapes.ChangeValue(anItem.Region());
    if (!aCachedShape.has_value())
    {
      const BRepFont_PlanarRegion& aRegion = thePlan.Region(anItem.Region());
      aCachedShape                         = BuildGlyph(aRegion, aGlyphOptions);
      if (aCachedShape->IsNull() && !aRegion.IsEmpty())
      {
        return {};
      }
    }
    TopoDS_Shape aGlyphShape = *aCachedShape;
    if (aGlyphShape.IsNull())
    {
      continue;
    }
    gp_Trsf aPosition;
    aPosition.SetTranslation(gp_Vec(anItem.Position().X(), anItem.Position().Y(), 0.0));
    aGlyphShape.Move(aPosition);
    aBuilder.Add(aCompound, aGlyphShape);
  }
  if (aCompound.NbChildren() == 0)
  {
    return {};
  }
  aCompound.Move(aPenTransform);
  return aCompound;
}

//=================================================================================================

BRepGraph_NodeId BRepFont_Builder::AddText(BRepGraph& theGraph, const TextPlan& thePlan)
{
  return AddText(theGraph, thePlan, TextOptions{});
}

//=================================================================================================

BRepGraph_NodeId BRepFont_Builder::AddText(BRepGraph&         theGraph,
                                           const TextPlan&    thePlan,
                                           const TextOptions& theOptions)
{
  if (!std::isfinite(thePlan.Size()) || !(thePlan.Size() > 0.0)
      || !std::isfinite(thePlan.Tolerance()) || !(thePlan.Tolerance() > 0.0)
      || !isFinite(theOptions.Pen) || thePlan.Items().IsEmpty())
  {
    return {};
  }

  NCollection_LinearVector<std::optional<BRepGraph_NodeId>> aGlyphRoots;
  aGlyphRoots.Resize(thePlan.NbRegions());
  GlyphOptions aGlyphOptions;
  aGlyphOptions.Size      = thePlan.Size();
  aGlyphOptions.Tolerance = thePlan.Tolerance();
  bool hasGeometry        = false;
  for (const TextPlan::Item& anItem : thePlan.Items())
  {
    if (anItem.Region() >= aGlyphRoots.Size())
    {
      return {};
    }
    std::optional<BRepGraph_NodeId>& aCachedRoot = aGlyphRoots.ChangeValue(anItem.Region());
    if (!aCachedRoot.has_value())
    {
      const BRepFont_PlanarRegion& aRegion = thePlan.Region(anItem.Region());
      aCachedRoot                          = AddGlyph(theGraph, aRegion, aGlyphOptions);
      if (!aCachedRoot->IsValid() && !aRegion.IsEmpty())
      {
        return {};
      }
    }
    hasGeometry = hasGeometry || aCachedRoot->IsValid();
  }
  if (!hasGeometry)
  {
    return {};
  }

  NCollection_Array1<BRepGraph_NodeId> anEmptyChildren;
  const BRepGraph_CompoundId aTextRoot = theGraph.Editor().Compounds().Add(anEmptyChildren);
  if (!aTextRoot.IsValid())
  {
    return {};
  }
  gp_Trsf aPenTransform;
  aPenTransform.SetTransformation(theOptions.Pen, gp_Ax3(gp::XOY()));
  for (const TextPlan::Item& anItem : thePlan.Items())
  {
    const BRepGraph_NodeId aGlyphRoot = *aGlyphRoots[anItem.Region()];
    if (!aGlyphRoot.IsValid())
    {
      continue;
    }
    const BRepGraph_ChildRefId aReference =
      theGraph.Editor().Compounds().Append(aTextRoot, aGlyphRoot);
    if (!aReference.IsValid())
    {
      return {};
    }
    gp_Trsf aPosition;
    aPosition.SetTranslation(gp_Vec(anItem.Position().X(), anItem.Position().Y(), 0.0));
    const gp_Trsf aLocation = aPenTransform * aPosition;
    if (!isIdentity(aLocation))
    {
      theGraph.Editor().Gen().SetChildRefLocalLocation(aReference, TopLoc_Location(aLocation));
    }
  }
  return BRepGraph_NodeId(aTextRoot);
}
