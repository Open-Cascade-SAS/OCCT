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

#include <BRep_Builder.hxx>
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

bool isValidReal(const double theValue) noexcept
{
  return !std::isnan(theValue) && !Precision::IsInfinite(theValue);
}

//=================================================================================================

bool isFiniteScaled(const gp_XY& thePoint, const double theScale) noexcept
{
  const double aX = thePoint.X() * theScale;
  const double aY = thePoint.Y() * theScale;
  return isValidReal(aX) && isValidReal(aY);
}

//=================================================================================================

occ::handle<Geom2d_Curve> makeCurve2d(const BRepFont_PlanarRegion::Curve& theCurve,
                                      const double                        theScale)
{
  if (!isValidReal(theScale) || !(theScale > 0.0) || !isFiniteScaled(theCurve.Start(), theScale)
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
    occ::handle<Geom2d_Curve> aResult;
    aResult = aMaker.IsDone() ? aMaker.Value() : nullptr;
    return aResult;
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
  BRep_Builder                          aBuilder;
  Geom2dConvert_CompCurveToBSplineCurve aCompositeMaker;
  TopoDS_Wire                           aWire;
  aBuilder.MakeWire(aWire);

  const BRepFont_PlanarRegion::Use& aFirstUse       = theRegion.Uses()[aLoop.FirstUse()];
  uint32_t                          aPreviousVertex = aFirstUse.FirstVertex().Value();

  for (uint32_t anOffset = 0; anOffset < aLoop.NbUses(); ++anOffset)
  {
    const uint32_t                    aUseIndex    = aLoop.FirstUse() + anOffset;
    const BRepFont_PlanarRegion::Use& aUse         = theRegion.Uses()[aUseIndex];
    const uint32_t                    aCurveIndex  = aUse.CurveIndex().Value();
    const uint32_t                    aFirstVertex = aUse.FirstVertex().Value();
    const uint32_t                    aLastVertex  = aUse.LastVertex().Value();
    if (aCurveIndex >= theCurves2d.Size() || aFirstVertex >= theRegion.Vertices().Size()
        || aLastVertex >= theRegion.Vertices().Size() || aFirstVertex != aPreviousVertex)
    {
      return std::nullopt;
    }
    aPreviousVertex = aLastVertex;

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
    aBuilder.Add(aWire, anEdge);
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
    aBuilder.Add(aWire, anEdge);
  }
  aWire.Closed(aPreviousVertex == aFirstUse.FirstVertex().Value());
  return aWire;
}
} // namespace

//=================================================================================================

TopoDS_Shape BRepFont_Builder::BuildGlyph(const BRepFont_PlanarRegion& theRegion,
                                          const GlyphOptions&          theOptions)
{
  const bool aToConcatenateContours = theOptions.ToConcatenateContours;
  if (!isValidReal(theRegion.UnitsPerEm()) || !(theRegion.UnitsPerEm() > 0.0)
      || !isValidReal(theOptions.Size) || !(theOptions.Size > 0.0)
      || !isValidReal(theOptions.Tolerance) || !(theOptions.Tolerance > 0.0))
  {
    return {};
  }
  if (theRegion.IsEmpty())
  {
    return {};
  }

  const double aScale = theOptions.Size / theRegion.UnitsPerEm();
  if (!isValidReal(aScale))
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
    if (!isValidReal(aX) || !isValidReal(aY))
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

bool isValidPosition(const gp_Ax3& thePen) noexcept
{
  return isValidReal(thePen.Location().X()) && isValidReal(thePen.Location().Y())
         && isValidReal(thePen.Location().Z()) && isValidReal(thePen.Direction().X())
         && isValidReal(thePen.Direction().Y()) && isValidReal(thePen.Direction().Z())
         && isValidReal(thePen.XDirection().X()) && isValidReal(thePen.XDirection().Y())
         && isValidReal(thePen.XDirection().Z());
}

} // namespace

//=================================================================================================

BRepFont_Builder::TextPlan::TextPlan(
  const double                                                             theSize,
  const double                                                             theTolerance,
  const gp_XY&                                                             theLowerLeft,
  const gp_XY&                                                             theUpperRight,
  NCollection_LinearVector<std::shared_ptr<const BRepFont_PlanarRegion>>&& theRegions,
  NCollection_LinearVector<Item>&&                                         theItems)
    : mySize(theSize),
      myTolerance(theTolerance),
      myLowerLeft(theLowerLeft),
      myUpperRight(theUpperRight),
      myRegions(std::move(theRegions)),
      myItems(std::move(theItems))
{
}

//=================================================================================================

std::optional<BRepFont_Builder::TextPlan> BRepFont_Builder::CreateTextPlan(
  const double                                                             theSize,
  const double                                                             theTolerance,
  const gp_XY&                                                             theLowerLeft,
  const gp_XY&                                                             theUpperRight,
  NCollection_LinearVector<std::shared_ptr<const BRepFont_PlanarRegion>>&& theRegions,
  NCollection_LinearVector<TextPlan::Item>&&                               theItems)
{
  if (!isValidReal(theSize) || !(theSize > 0.0) || !isValidReal(theTolerance)
      || !(theTolerance > 0.0) || !isValidReal(theLowerLeft.X()) || !isValidReal(theLowerLeft.Y())
      || !isValidReal(theUpperRight.X()) || !isValidReal(theUpperRight.Y())
      || theUpperRight.X() < theLowerLeft.X() || theUpperRight.Y() < theLowerLeft.Y())
  {
    return std::nullopt;
  }
  for (const std::shared_ptr<const BRepFont_PlanarRegion>& aRegion : theRegions)
  {
    if (!aRegion || !isValidReal(aRegion->UnitsPerEm()) || !(aRegion->UnitsPerEm() > 0.0)
        || !isValidReal(aRegion->Tolerance()) || !(aRegion->Tolerance() > 0.0))
    {
      return std::nullopt;
    }
  }
  for (const TextPlan::Item& anItem : theItems)
  {
    if (anItem.Region() >= theRegions.Size() || !isValidReal(anItem.Position().X())
        || !isValidReal(anItem.Position().Y()))
    {
      return std::nullopt;
    }
  }
  return TextPlan(theSize,
                  theTolerance,
                  theLowerLeft,
                  theUpperRight,
                  std::move(theRegions),
                  std::move(theItems));
}

//=================================================================================================

TopoDS_Shape BRepFont_Builder::BuildText(const TextPlan& thePlan)
{
  return BuildText(thePlan, TextOptions{});
}

//=================================================================================================

TopoDS_Shape BRepFont_Builder::BuildText(const TextPlan& thePlan, const TextOptions& theOptions)
{
  if (!isValidReal(thePlan.Size()) || !(thePlan.Size() > 0.0) || !isValidReal(thePlan.Tolerance())
      || !(thePlan.Tolerance() > 0.0) || !isValidPosition(theOptions.Pen)
      || thePlan.Items().IsEmpty())
  {
    return {};
  }

  GlyphOptions aGlyphOptions;
  aGlyphOptions.Size                  = thePlan.Size();
  aGlyphOptions.Tolerance             = thePlan.Tolerance();
  aGlyphOptions.ToConcatenateContours = theOptions.ToConcatenateContours;
  NCollection_LinearVector<std::optional<TopoDS_Shape>> aGlyphShapes(thePlan.NbRegions(),
                                                                     std::nullopt);
  for (const TextPlan::Item& anItem : thePlan.Items())
  {
    std::optional<TopoDS_Shape>& aShape = aGlyphShapes.ChangeValue(anItem.Region());
    if (aShape.has_value())
    {
      continue;
    }
    const BRepFont_PlanarRegion& aRegion = thePlan.Region(anItem.Region());
    aShape                               = BuildGlyph(aRegion, aGlyphOptions);
    if (aShape->IsNull() && !aRegion.IsEmpty())
    {
      return {};
    }
  }
  NCollection_LinearVector<TopoDS_Shape> aBuiltShapes(thePlan.NbRegions(), TopoDS_Shape{});
  for (size_t aRegionIndex = 0; aRegionIndex < thePlan.NbRegions(); ++aRegionIndex)
  {
    if (aGlyphShapes.Value(aRegionIndex).has_value())
    {
      aBuiltShapes.ChangeValue(aRegionIndex) = *aGlyphShapes.Value(aRegionIndex);
    }
  }
  return BuildText(thePlan, aBuiltShapes, theOptions);
}

//=================================================================================================

TopoDS_Shape BRepFont_Builder::BuildText(
  const TextPlan&                               thePlan,
  const NCollection_LinearVector<TopoDS_Shape>& theGlyphShapes,
  const TextOptions&                            theOptions)
{
  if (!isValidReal(thePlan.Size()) || !(thePlan.Size() > 0.0) || !isValidReal(thePlan.Tolerance())
      || !(thePlan.Tolerance() > 0.0) || !isValidPosition(theOptions.Pen)
      || thePlan.Items().IsEmpty() || theGlyphShapes.Size() != thePlan.NbRegions())
  {
    return {};
  }
  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  for (const TextPlan::Item& anItem : thePlan.Items())
  {
    if (anItem.Region() >= theGlyphShapes.Size())
    {
      return {};
    }
    TopoDS_Shape aGlyphShape = theGlyphShapes.Value(anItem.Region());
    if (aGlyphShape.IsNull() && !thePlan.Region(anItem.Region()).IsEmpty())
    {
      return {};
    }
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
  gp_Trsf aPenTransform;
  aPenTransform.SetTransformation(theOptions.Pen, gp_Ax3(gp::XOY()));
  aCompound.Move(aPenTransform);
  return aCompound;
}
