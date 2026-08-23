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

#include <BRepFont_Regularizer.hxx>

#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace
{
enum class IntersectionKind : uint8_t
{
  None,
  BetweenLoops,
  WithinLoop
};

struct LoopInfo
{
  enum class Role : uint8_t
  {
    None,
    Outer,
    Hole
  };

  std::optional<uint32_t> Parent;
  gp_XY                   InteriorPoint;
  int                     Winding      = 0;
  bool                    IsFilled     = false;
  Role                    BoundaryRole = Role::None;
};

struct HoleBinding
{
  HoleBinding(const uint32_t theOuterLoop, const uint32_t theHoleLoop)
      : OuterLoop(theOuterLoop),
        HoleLoop(theHoleLoop)
  {
  }

  uint32_t OuterLoop;
  uint32_t HoleLoop;
};

//! Packed range of approximation points used only during loop classification.
struct FlattenedLoop
{
  FlattenedLoop(const uint32_t theFirstPoint, const uint32_t theNbPoints)
      : FirstPoint(theFirstPoint),
        NbPoints(theNbPoints)
  {
  }

  uint32_t FirstPoint;
  uint32_t NbPoints;
};

//! Curve bounds used by the intersection sweep.
struct IntersectionCurve
{
  IntersectionCurve(const uint32_t   theIndex,
                    const Bnd_Box2d& theBounds,
                    const double     theMinX,
                    const double     theMaxX)
      : Index(theIndex),
        Bounds(theBounds),
        MinX(theMinX),
        MaxX(theMaxX)
  {
  }

  uint32_t  Index;
  Bnd_Box2d Bounds;
  double    MinX;
  double    MaxX;
};

std::optional<uint32_t> index(const size_t theValue);

bool isPointLike(const Font_GlyphOutline::Segment& theSegment,
                 const gp_XY&                      thePoint,
                 double                            theSquareTolerance);

bool isFinitePoint(const gp_XY& thePoint);

Font_GlyphOutline::Segment healEndpoints(const Font_GlyphOutline::Segment& theSegment,
                                         const gp_XY&                      theStart,
                                         const gp_XY&                      theEnd);

uint64_t loopPairKey(uint32_t theFirst, uint32_t theSecond) noexcept;

IntersectionKind intersections(const BRepFont_PlanarRegion&   theRegion,
                               double                         theTolerance,
                               NCollection_FlatMap<uint64_t>& theLoopPairs);

occ::handle<Geom2d_Curve> curveGeometry(const BRepFont_PlanarRegion::Curve& theCurve);

bool contains(const BRepFont_PlanarRegion::Loop&     theLoop,
              const NCollection_LinearVector<gp_XY>& thePoints,
              const FlattenedLoop&                   theFlattened,
              const gp_XY&                           thePoint);

bool flattenLoops(const BRepFont_PlanarRegion&             theRegion,
                  double                                   theTolerance,
                  NCollection_LinearVector<gp_XY>&         thePoints,
                  NCollection_LinearVector<FlattenedLoop>& theLoops);

void appendFlattened(const BRepFont_PlanarRegion::Curve& theCurve,
                     double                              theFirst,
                     const gp_XY&                        theFirstPoint,
                     double                              theLast,
                     const gp_XY&                        theLastPoint,
                     double                              theSquareTolerance,
                     int                                 theDepth,
                     NCollection_LinearVector<gp_XY>&    thePoints);

std::optional<gp_XY> interiorPoint(const BRepFont_PlanarRegion&           theRegion,
                                   const BRepFont_PlanarRegion::Loop&     theLoop,
                                   const NCollection_LinearVector<gp_XY>& thePoints,
                                   const FlattenedLoop&                   theFlattened,
                                   double                                 theTolerance);

//=================================================================================================

std::optional<uint32_t> index(const size_t theValue)
{
  const uint32_t anIndex = static_cast<uint32_t>(theValue);
  return static_cast<size_t>(anIndex) == theValue ? std::optional<uint32_t>(anIndex) : std::nullopt;
}
} // namespace

//=================================================================================================

BRepFont_Regularizer::Result BRepFont_Regularizer::Build(const Font_GlyphOutline& theOutline) const
{
  return Build(theOutline, Options{});
}

//=================================================================================================

BRepFont_Regularizer::Result BRepFont_Regularizer::Build(const Font_GlyphOutline& theOutline,
                                                         const Options&           theOptions) const
{
  Result aResult;
  if (!(theOutline.UnitsPerEm() > 0.0) || !std::isfinite(theOutline.UnitsPerEm())
      || !(theOptions.Tolerance > 0.0) || !std::isfinite(theOptions.Tolerance)
      || !std::isfinite(theOptions.Tolerance * theOptions.Tolerance))
  {
    return aResult;
  }

  BRepFont_PlanarRegion aRegion;
  aRegion.myUnitsPerEm     = theOutline.UnitsPerEm();
  aRegion.myTolerance      = theOptions.Tolerance;
  aRegion.myIsSingleStroke = theOutline.IsSingleStroke();
  if (theOutline.IsEmpty())
  {
    if (!theOutline.Contours().IsEmpty())
    {
      return aResult;
    }
    aResult.myStatus = Status::Empty;
    aResult.myRegion.emplace(std::move(aRegion));
    return aResult;
  }

  Status aStatus = Status::Success;
  if (!appendOutline(theOutline, theOptions.Tolerance, aRegion, aStatus))
  {
    aResult.myStatus = aStatus;
    return aResult;
  }
  NCollection_FlatMap<uint64_t> anIntersectingLoopPairs;
  const IntersectionKind        anIntersectionKind =
    aRegion.IsSingleStroke()
      ? IntersectionKind::None
      : intersections(aRegion, theOptions.Tolerance, anIntersectingLoopPairs);
  if (anIntersectionKind != IntersectionKind::None)
  {
    if (!theOutline.HasPossibleOverlaps())
    {
      aResult.myStatus = Status::IntersectingContours;
      return aResult;
    }
    if (anIntersectionKind == IntersectionKind::WithinLoop)
    {
      aResult.myStatus = Status::SelfIntersectingContour;
      return aResult;
    }
  }
  if (!aRegion.IsSingleStroke())
  {
    if (!classifyLoops(theOutline, aRegion, theOptions.Tolerance, anIntersectingLoopPairs))
    {
      aResult.myStatus = Status::InvalidOutline;
      return aResult;
    }
  }

  aResult.myStatus = aRegion.IsEmpty() ? Status::Empty : Status::Success;
  aResult.myRegion.emplace(std::move(aRegion));
  return aResult;
}

//=================================================================================================

namespace
{
bool isPointLike(const Font_GlyphOutline::Segment& theSegment,
                 const gp_XY&                      thePoint,
                 const double                      theSquareTolerance)
{
  if ((theSegment.Start() - thePoint).SquareModulus() > theSquareTolerance
      || (theSegment.End() - thePoint).SquareModulus() > theSquareTolerance)
  {
    return false;
  }
  if (theSegment.Type() != Font_GlyphOutline::Segment::Kind::Line
      && (theSegment.Control1() - thePoint).SquareModulus() > theSquareTolerance)
  {
    return false;
  }
  return theSegment.Type() != Font_GlyphOutline::Segment::Kind::CubicBezier
         || (theSegment.Control2() - thePoint).SquareModulus() <= theSquareTolerance;
}

//=================================================================================================

bool isFinitePoint(const gp_XY& thePoint)
{
  return std::isfinite(thePoint.X()) && std::isfinite(thePoint.Y());
}

//=================================================================================================

Font_GlyphOutline::Segment healEndpoints(const Font_GlyphOutline::Segment& theSegment,
                                         const gp_XY&                      theStart,
                                         const gp_XY&                      theEnd)
{
  switch (theSegment.Type())
  {
    case Font_GlyphOutline::Segment::Kind::Line:
      return Font_GlyphOutline::Segment::Line(theStart, theEnd);
    case Font_GlyphOutline::Segment::Kind::QuadraticBezier:
      return Font_GlyphOutline::Segment::Quadratic(theStart, theSegment.Control1(), theEnd);
    case Font_GlyphOutline::Segment::Kind::CubicBezier:
      return Font_GlyphOutline::Segment::Cubic(theStart,
                                               theSegment.Control1(),
                                               theSegment.Control2(),
                                               theEnd);
  }
  return Font_GlyphOutline::Segment::Line(theStart, theEnd);
}
} // namespace

//=================================================================================================

bool BRepFont_Regularizer::appendOutline(const Font_GlyphOutline& theOutline,
                                         const double             theTolerance,
                                         BRepFont_PlanarRegion&   theRegion,
                                         Status&                  theStatus)
{
  const auto& aSegments = theOutline.Segments();
  const auto& aContours = theOutline.Contours();
  if (!index(aSegments.Size()).has_value() || !index(aContours.Size()).has_value())
  {
    theStatus = Status::InvalidOutline;
    return false;
  }

  theRegion.myVertices.Reserve(aSegments.Size() + aContours.Size());
  theRegion.myCurves.Reserve(aSegments.Size());
  theRegion.myUses.Reserve(aSegments.Size());
  theRegion.myLoops.Reserve(aContours.Size());
  const double aSquareTolerance = theTolerance * theTolerance;
  uint64_t     anExpectedFirst  = 0;

  for (const Font_GlyphOutline::Contour& aContour : aContours)
  {
    const uint64_t aFirst = aContour.FirstSegment();
    const uint64_t aLast  = aFirst + aContour.NbSegments();
    if (aFirst != anExpectedFirst || aContour.NbSegments() == 0 || aLast > aSegments.Size()
        || !index(theRegion.myUses.Size()).has_value())
    {
      theStatus = Status::InvalidOutline;
      return false;
    }
    anExpectedFirst = aLast;

    const Font_GlyphOutline::Segment& aFirstSegment = aSegments[aContour.FirstSegment()];
    const std::optional<uint32_t>     aFirstVertex  = index(theRegion.myVertices.Size());
    if (!aFirstVertex.has_value())
    {
      theStatus = Status::InvalidOutline;
      return false;
    }
    const uint32_t aFirstVertexIndex = *aFirstVertex;
    theRegion.myVertices.Append(aFirstSegment.Start());
    uint32_t                      aPreviousVertexIndex = aFirstVertexIndex;
    const std::optional<uint32_t> aFirstUseValue       = index(theRegion.myUses.Size());
    if (!aFirstUseValue.has_value())
    {
      theStatus = Status::InvalidOutline;
      return false;
    }
    const uint32_t aFirstUse   = *aFirstUseValue;
    double         aSignedArea = 0.0;
    Bnd_Box2d      aBounds;

    for (uint32_t aLocalIndex = 0; aLocalIndex < aContour.NbSegments(); ++aLocalIndex)
    {
      const uint32_t                    aSourceIndex = aContour.FirstSegment() + aLocalIndex;
      const Font_GlyphOutline::Segment& aSegment     = aSegments[aSourceIndex];
      if (!isFinitePoint(aSegment.Start()) || !isFinitePoint(aSegment.End())
          || (aSegment.Type() != Font_GlyphOutline::Segment::Kind::Line
              && !isFinitePoint(aSegment.Control1()))
          || (aSegment.Type() == Font_GlyphOutline::Segment::Kind::CubicBezier
              && !isFinitePoint(aSegment.Control2())))
      {
        theStatus = Status::InvalidOutline;
        return false;
      }
      const gp_XY& anExpectedStart = theRegion.myVertices[aPreviousVertexIndex];
      if ((aSegment.Start() - anExpectedStart).SquareModulus() > aSquareTolerance)
      {
        theStatus = Status::OpenContour;
        return false;
      }

      const bool  isLastSource = aLocalIndex + 1 == aContour.NbSegments();
      const gp_XY aHealedEnd   = !isLastSource || theOutline.IsSingleStroke()
                                   ? aSegment.End()
                                   : theRegion.myVertices[aFirstVertexIndex];
      if (!theOutline.IsSingleStroke() && isLastSource
          && (aSegment.End() - aHealedEnd).SquareModulus() > aSquareTolerance)
      {
        theStatus = Status::OpenContour;
        return false;
      }
      if (isPointLike(aSegment, anExpectedStart, aSquareTolerance))
      {
        continue;
      }

      uint32_t aLastVertexIndex = aFirstVertexIndex;
      if (!isLastSource || theOutline.IsSingleStroke())
      {
        const std::optional<uint32_t> aLastVertex = index(theRegion.myVertices.Size());
        if (!aLastVertex.has_value())
        {
          theStatus = Status::InvalidOutline;
          return false;
        }
        aLastVertexIndex = *aLastVertex;
        theRegion.myVertices.Append(aHealedEnd);
      }

      const std::optional<uint32_t> aCurveIndexValue = index(theRegion.myCurves.Size());
      if (!aCurveIndexValue.has_value())
      {
        theStatus = Status::InvalidOutline;
        return false;
      }
      const uint32_t                aCurveIndex = *aCurveIndexValue;
      BRepFont_PlanarRegion::Curve& aCurve =
        theRegion.myCurves.EmplaceAppend(healEndpoints(aSegment, anExpectedStart, aHealedEnd));
      theRegion.myUses.EmplaceAppend(BRepFont_PlanarRegion::CurveId(aCurveIndex),
                                     BRepFont_PlanarRegion::VertexId(aPreviousVertexIndex),
                                     BRepFont_PlanarRegion::VertexId(aLastVertexIndex));
      const double aCurveArea = aCurve.SignedArea();
      if (!std::isfinite(aCurveArea) || !std::isfinite(aSignedArea + aCurveArea))
      {
        theStatus = Status::InvalidOutline;
        return false;
      }
      aSignedArea += aCurveArea;
      aBounds.Add(aCurve.Bounds());
      aPreviousVertexIndex = aLastVertexIndex;
    }

    const std::optional<uint32_t> anAfterLastUse = index(theRegion.myUses.Size());
    if (!anAfterLastUse.has_value())
    {
      theStatus = Status::InvalidOutline;
      return false;
    }
    const uint32_t aNbUses = *anAfterLastUse - aFirstUse;
    if (aNbUses == 0)
    {
      theRegion.myVertices.EraseLast();
      continue;
    }
    if (!theOutline.IsSingleStroke())
    {
      BRepFont_PlanarRegion::Use& aLastUse = theRegion.myUses[*anAfterLastUse - 1];
      if (aLastUse.LastVertex().Value() != aFirstVertexIndex)
      {
        const uint32_t aLastCurveIndex       = aLastUse.CurveIndex().Value();
        const uint32_t aRedundantVertexIndex = aLastUse.LastVertex().Value();
        if (aLastCurveIndex >= theRegion.myCurves.Size()
            || static_cast<uint64_t>(aRedundantVertexIndex) + 1 != theRegion.myVertices.Size())
        {
          theStatus = Status::InvalidOutline;
          return false;
        }
        const BRepFont_PlanarRegion::CurveId  aCurveId     = aLastUse.CurveIndex();
        const BRepFont_PlanarRegion::VertexId aFirstUseVtx = aLastUse.FirstVertex();
        const double                          aFirstParam  = aLastUse.FirstParameter();
        const double                          aLastParam   = aLastUse.LastParameter();
        const bool                            isReversed   = aLastUse.IsReversed();
        const BRepFont_PlanarRegion::Curve&   anOldCurve   = theRegion.myCurves[aLastCurveIndex];
        const gp_XY                           anOldStart   = anOldCurve.Start();
        const gp_XY                           aFirstPoint = theRegion.myVertices[aFirstVertexIndex];
        if ((anOldCurve.End() - aFirstPoint).SquareModulus() > aSquareTolerance)
        {
          theStatus = Status::OpenContour;
          return false;
        }
        const double anOldArea              = anOldCurve.SignedArea();
        theRegion.myCurves[aLastCurveIndex] = anOldCurve.WithEndpoints(anOldStart, aFirstPoint);
        const BRepFont_PlanarRegion::Curve& aHealedCurve = theRegion.myCurves[aLastCurveIndex];
        const double                        aHealedArea  = aHealedCurve.SignedArea();
        if (!std::isfinite(anOldArea) || !std::isfinite(aHealedArea)
            || !std::isfinite(aSignedArea + aHealedArea - anOldArea))
        {
          theStatus = Status::InvalidOutline;
          return false;
        }
        aSignedArea += aHealedArea - anOldArea;
        aLastUse = BRepFont_PlanarRegion::Use(aCurveId,
                                              aFirstUseVtx,
                                              BRepFont_PlanarRegion::VertexId(aFirstVertexIndex),
                                              aFirstParam,
                                              aLastParam,
                                              isReversed);
        theRegion.myVertices.EraseLast();

        aBounds.SetVoid();
        for (uint32_t aUseIndex = aFirstUse; aUseIndex < *anAfterLastUse; ++aUseIndex)
        {
          const uint32_t aCurveIndex = theRegion.myUses[aUseIndex].CurveIndex().Value();
          if (aCurveIndex >= theRegion.myCurves.Size())
          {
            theStatus = Status::InvalidOutline;
            return false;
          }
          aBounds.Add(theRegion.myCurves[aCurveIndex].Bounds());
        }
      }
    }
    if (!theOutline.IsSingleStroke() && std::abs(aSignedArea) <= aSquareTolerance)
    {
      theStatus = Status::InvalidOutline;
      return false;
    }
    theRegion.myLoops.EmplaceAppend(aFirstUse, aNbUses, aSignedArea, aBounds);
  }
  if (anExpectedFirst != aSegments.Size())
  {
    theStatus = Status::InvalidOutline;
    return false;
  }
  return true;
}

//=================================================================================================

namespace
{
occ::handle<Geom2d_Curve> curveGeometry(const BRepFont_PlanarRegion::Curve& theCurve)
{
  int aNbPoles = 2;
  if (theCurve.Type() == Font_GlyphOutline::Segment::Kind::QuadraticBezier)
  {
    aNbPoles = 3;
  }
  else if (theCurve.Type() == Font_GlyphOutline::Segment::Kind::CubicBezier)
  {
    aNbPoles = 4;
  }

  std::array<gp_Pnt2d, 4> aPoleStorage;
  aPoleStorage[0] = gp_Pnt2d(theCurve.Start());
  if (aNbPoles == 2)
  {
    aPoleStorage[1] = gp_Pnt2d(theCurve.End());
  }
  else if (aNbPoles == 3)
  {
    aPoleStorage[1] = gp_Pnt2d(theCurve.Control1());
    aPoleStorage[2] = gp_Pnt2d(theCurve.End());
  }
  else
  {
    aPoleStorage[1] = gp_Pnt2d(theCurve.Control1());
    aPoleStorage[2] = gp_Pnt2d(theCurve.Control2());
    aPoleStorage[3] = gp_Pnt2d(theCurve.End());
  }
  const NCollection_Array1<gp_Pnt2d> aPoles(aPoleStorage.data(), static_cast<size_t>(aNbPoles));
  return new Geom2d_BezierCurve(aPoles);
}

//=================================================================================================

uint64_t loopPairKey(const uint32_t theFirst, const uint32_t theSecond) noexcept
{
  const uint32_t aLower  = std::min(theFirst, theSecond);
  const uint32_t anUpper = std::max(theFirst, theSecond);
  return (static_cast<uint64_t>(aLower) << 32) | anUpper;
}

//=================================================================================================

IntersectionKind intersections(const BRepFont_PlanarRegion&   theRegion,
                               const double                   theTolerance,
                               NCollection_FlatMap<uint64_t>& theLoopPairs)
{
  const auto&      aCurves = theRegion.Curves();
  IntersectionKind aResult = IntersectionKind::None;

  NCollection_LinearVector<uint32_t> aCurveLoops(aCurves.Size());
  NCollection_LinearVector<uint32_t> aCurveOffsets(aCurves.Size());
  aCurveLoops.Resize(aCurves.Size());
  aCurveOffsets.Resize(aCurves.Size());
  for (uint32_t aLoopIndex = 0; aLoopIndex < theRegion.Loops().Size(); ++aLoopIndex)
  {
    const BRepFont_PlanarRegion::Loop& aLoop = theRegion.Loops()[aLoopIndex];
    for (uint32_t anOffset = 0; anOffset < aLoop.NbUses(); ++anOffset)
    {
      const BRepFont_PlanarRegion::Use& aUse        = theRegion.Uses()[aLoop.FirstUse() + anOffset];
      const uint32_t                    aCurveIndex = aUse.CurveIndex().Value();
      if (aCurveIndex >= aCurves.Size())
      {
        return IntersectionKind::WithinLoop;
      }
      aCurveLoops[aCurveIndex]   = aLoopIndex;
      aCurveOffsets[aCurveIndex] = anOffset;
    }
  }

  NCollection_LinearVector<occ::handle<Geom2d_Curve>> aGeometry(aCurves.Size());
  NCollection_LinearVector<IntersectionCurve>         aSweep(aCurves.Size());
  aGeometry.Resize(aCurves.Size());

  const double aSquareTolerance = theTolerance * theTolerance;
  for (uint32_t aCurveIndex = 0; aCurveIndex < aCurves.Size(); ++aCurveIndex)
  {
    const BRepFont_PlanarRegion::Curve& aCurve = aCurves[aCurveIndex];
    aGeometry[aCurveIndex]                     = curveGeometry(aCurve);

    bool toCheckSelfIntersection = aCurve.Type() == Font_GlyphOutline::Segment::Kind::CubicBezier;
    if (aCurve.Type() == Font_GlyphOutline::Segment::Kind::QuadraticBezier)
    {
      const gp_XY aFirstTangent = aCurve.Control1() - aCurve.Start();
      const gp_XY aLastTangent  = aCurve.End() - aCurve.Control1();
      const bool  isMonotonicX  = (aFirstTangent.X() >= 0.0 && aLastTangent.X() >= 0.0
                                   && (aFirstTangent.X() > 0.0 || aLastTangent.X() > 0.0))
                                  || (aFirstTangent.X() <= 0.0 && aLastTangent.X() <= 0.0
                                      && (aFirstTangent.X() < 0.0 || aLastTangent.X() < 0.0));
      const bool  isMonotonicY  = (aFirstTangent.Y() >= 0.0 && aLastTangent.Y() >= 0.0
                                   && (aFirstTangent.Y() > 0.0 || aLastTangent.Y() > 0.0))
                                  || (aFirstTangent.Y() <= 0.0 && aLastTangent.Y() <= 0.0
                                      && (aFirstTangent.Y() < 0.0 || aLastTangent.Y() < 0.0));
      toCheckSelfIntersection   = !isMonotonicX && !isMonotonicY;
    }
    if (toCheckSelfIntersection)
    {
      Geom2dAPI_InterCurveCurve aSelfIntersector(aGeometry[aCurveIndex], theTolerance);
      if (aSelfIntersector.NbPoints() > 0 || aSelfIntersector.NbSegments() > 0)
      {
        return IntersectionKind::WithinLoop;
      }
    }

    Bnd_Box2d aBounds = aCurve.Bounds();
    aBounds.Enlarge(theTolerance);
    aSweep.EmplaceAppend(aCurveIndex, aBounds, aBounds.GetXMin(), aBounds.GetXMax());
  }

  std::sort(aSweep.begin(),
            aSweep.end(),
            [](const IntersectionCurve& theLeft, const IntersectionCurve& theRight) {
              if (theLeft.MinX != theRight.MinX)
              {
                return theLeft.MinX < theRight.MinX;
              }
              if (theLeft.MaxX != theRight.MaxX)
              {
                return theLeft.MaxX < theRight.MaxX;
              }
              return theLeft.Index < theRight.Index;
            });

  for (size_t aFirstPosition = 0; aFirstPosition < aSweep.Size(); ++aFirstPosition)
  {
    const IntersectionCurve& aFirst      = aSweep[aFirstPosition];
    const uint32_t           aFirstIndex = aFirst.Index;
    for (size_t aSecondPosition = aFirstPosition + 1; aSecondPosition < aSweep.Size();
         ++aSecondPosition)
    {
      const IntersectionCurve& aSecond = aSweep[aSecondPosition];
      if (aSecond.MinX > aFirst.MaxX)
      {
        break;
      }
      if (aFirst.Bounds.IsOut(aSecond.Bounds))
      {
        continue;
      }

      const uint32_t            aSecondIndex = aSecond.Index;
      Geom2dAPI_InterCurveCurve anIntersector(aGeometry[aFirstIndex],
                                              aGeometry[aSecondIndex],
                                              theTolerance);
      const bool                isSameLoop = aCurveLoops[aFirstIndex] == aCurveLoops[aSecondIndex];
      if (anIntersector.NbSegments() > 0)
      {
        if (isSameLoop)
        {
          return IntersectionKind::WithinLoop;
        }
        theLoopPairs.Add(loopPairKey(aCurveLoops[aFirstIndex], aCurveLoops[aSecondIndex]));
        aResult = IntersectionKind::BetweenLoops;
        continue;
      }

      bool  isAdjacent = false;
      gp_XY aSharedPoint;
      if (isSameLoop)
      {
        const BRepFont_PlanarRegion::Loop& aLoop = theRegion.Loops()[aCurveLoops[aFirstIndex]];
        uint32_t                           anEarlierIndex  = aFirstIndex;
        uint32_t                           anEarlierOffset = aCurveOffsets[aFirstIndex];
        uint32_t                           aLaterIndex     = aSecondIndex;
        uint32_t                           aLaterOffset    = aCurveOffsets[aSecondIndex];
        if (aLaterOffset < anEarlierOffset)
        {
          std::swap(anEarlierIndex, aLaterIndex);
          std::swap(anEarlierOffset, aLaterOffset);
        }
        if (aLaterOffset == anEarlierOffset + 1)
        {
          isAdjacent   = true;
          aSharedPoint = aCurves[anEarlierIndex].End();
        }
        else if (anEarlierOffset == 0 && aLaterOffset + 1 == aLoop.NbUses())
        {
          isAdjacent   = true;
          aSharedPoint = aCurves[anEarlierIndex].Start();
        }
      }

      for (int aPointIndex = 1; aPointIndex <= anIntersector.NbPoints(); ++aPointIndex)
      {
        if (!isAdjacent
            || (anIntersector.Point(aPointIndex).XY() - aSharedPoint).SquareModulus()
                 > aSquareTolerance)
        {
          if (isSameLoop)
          {
            return IntersectionKind::WithinLoop;
          }
          theLoopPairs.Add(loopPairKey(aCurveLoops[aFirstIndex], aCurveLoops[aSecondIndex]));
          aResult = IntersectionKind::BetweenLoops;
          break;
        }
      }
    }
  }
  return aResult;
}

//=================================================================================================

void appendFlattened(const BRepFont_PlanarRegion::Curve& theCurve,
                     const double                        theFirst,
                     const gp_XY&                        theFirstPoint,
                     const double                        theLast,
                     const gp_XY&                        theLastPoint,
                     const double                        theSquareTolerance,
                     const int                           theDepth,
                     NCollection_LinearVector<gp_XY>&    thePoints)
{
  const double aMiddle             = 0.5 * (theFirst + theLast);
  const gp_XY  aMiddlePoint        = theCurve.Value(aMiddle);
  const gp_XY  aChord              = theLastPoint - theFirstPoint;
  const gp_XY  aQuarterPoint       = theCurve.Value(0.5 * (theFirst + aMiddle));
  const gp_XY  aThreeQuartersPoint = theCurve.Value(0.5 * (aMiddle + theLast));
  double       aSquareDistance     = 0.0;
  if (aChord.SquareModulus() > 0.0)
  {
    const std::array<gp_XY, 3> aSamplePoints = {aQuarterPoint, aMiddlePoint, aThreeQuartersPoint};
    for (const gp_XY& aSamplePoint : aSamplePoints)
    {
      const gp_XY  aDelta = aSamplePoint - theFirstPoint;
      const double aCross = aChord.X() * aDelta.Y() - aChord.Y() * aDelta.X();
      aSquareDistance     = std::max(aSquareDistance, aCross * aCross / aChord.SquareModulus());
    }
  }
  else
  {
    aSquareDistance = std::max({(aQuarterPoint - theFirstPoint).SquareModulus(),
                                (aMiddlePoint - theFirstPoint).SquareModulus(),
                                (aThreeQuartersPoint - theFirstPoint).SquareModulus()});
  }

  if (theDepth >= 16 || aSquareDistance <= theSquareTolerance)
  {
    thePoints.Append(theLastPoint);
    return;
  }
  appendFlattened(theCurve,
                  theFirst,
                  theFirstPoint,
                  aMiddle,
                  aMiddlePoint,
                  theSquareTolerance,
                  theDepth + 1,
                  thePoints);
  appendFlattened(theCurve,
                  aMiddle,
                  aMiddlePoint,
                  theLast,
                  theLastPoint,
                  theSquareTolerance,
                  theDepth + 1,
                  thePoints);
}

//=================================================================================================

bool contains(const BRepFont_PlanarRegion::Loop&     theLoop,
              const NCollection_LinearVector<gp_XY>& thePoints,
              const FlattenedLoop&                   theFlattened,
              const gp_XY&                           thePoint)
{
  if (theLoop.Bounds().IsOut(gp_Pnt2d(thePoint)))
  {
    return false;
  }

  int          aWinding   = 0;
  const size_t anEndPoint = static_cast<size_t>(theFlattened.FirstPoint) + theFlattened.NbPoints;
  for (size_t aPointIndex = theFlattened.FirstPoint; aPointIndex + 1 < anEndPoint; ++aPointIndex)
  {
    const gp_XY& aFirst  = thePoints[aPointIndex];
    const gp_XY& aSecond = thePoints[aPointIndex + 1];
    if (aFirst.Y() <= thePoint.Y())
    {
      if (aSecond.Y() > thePoint.Y())
      {
        const double aCross = (aSecond.X() - aFirst.X()) * (thePoint.Y() - aFirst.Y())
                              - (thePoint.X() - aFirst.X()) * (aSecond.Y() - aFirst.Y());
        if (aCross > 0.0)
        {
          ++aWinding;
        }
      }
    }
    else if (aSecond.Y() <= thePoint.Y())
    {
      const double aCross = (aSecond.X() - aFirst.X()) * (thePoint.Y() - aFirst.Y())
                            - (thePoint.X() - aFirst.X()) * (aSecond.Y() - aFirst.Y());
      if (aCross < 0.0)
      {
        --aWinding;
      }
    }
  }
  return aWinding != 0;
}

//=================================================================================================

bool flattenLoops(const BRepFont_PlanarRegion&             theRegion,
                  const double                             theTolerance,
                  NCollection_LinearVector<gp_XY>&         thePoints,
                  NCollection_LinearVector<FlattenedLoop>& theLoops)
{
  thePoints.Reserve(theRegion.Uses().Size() * 2);
  theLoops.Reserve(theRegion.Loops().Size());
  const double aSquareTolerance = theTolerance * theTolerance;
  for (const BRepFont_PlanarRegion::Loop& aLoop : theRegion.Loops())
  {
    const std::optional<uint32_t> aFirstPoint = index(thePoints.Size());
    if (!aFirstPoint.has_value())
    {
      return false;
    }

    const BRepFont_PlanarRegion::Use& aFirstUse   = theRegion.Uses()[aLoop.FirstUse()];
    const uint32_t                    aFirstCurve = aFirstUse.CurveIndex().Value();
    if (aFirstCurve >= theRegion.Curves().Size())
    {
      return false;
    }
    thePoints.Append(theRegion.Curves()[aFirstCurve].Start());

    for (uint32_t anOffset = 0; anOffset < aLoop.NbUses(); ++anOffset)
    {
      const BRepFont_PlanarRegion::Use& aUse = theRegion.Uses()[aLoop.FirstUse() + anOffset];
      if (aUse.CurveIndex().Value() >= theRegion.Curves().Size())
      {
        return false;
      }
      const BRepFont_PlanarRegion::Curve& aCurve = theRegion.Curves()[aUse.CurveIndex().Value()];
      appendFlattened(aCurve,
                      0.0,
                      aCurve.Start(),
                      1.0,
                      aCurve.End(),
                      aSquareTolerance,
                      0,
                      thePoints);
    }

    const std::optional<uint32_t> anAfterLastPoint = index(thePoints.Size());
    if (!anAfterLastPoint.has_value())
    {
      return false;
    }
    theLoops.EmplaceAppend(*aFirstPoint, *anAfterLastPoint - *aFirstPoint);
  }
  return true;
}

//=================================================================================================

std::optional<gp_XY> interiorPoint(const BRepFont_PlanarRegion&           theRegion,
                                   const BRepFont_PlanarRegion::Loop&     theLoop,
                                   const NCollection_LinearVector<gp_XY>& thePoints,
                                   const FlattenedLoop&                   theFlattened,
                                   const double                           theTolerance)
{
  double aMinX = 0.0;
  double aMinY = 0.0;
  double aMaxX = 0.0;
  double aMaxY = 0.0;
  theLoop.Bounds().Get(aMinX, aMinY, aMaxX, aMaxY);
  const double aDiagonal       = std::hypot(aMaxX - aMinX, aMaxY - aMinY);
  const double anInitialOffset = std::max(8.0 * theTolerance, aDiagonal * 1.0e-6);

  for (uint32_t anOffset = 0; anOffset < theLoop.NbUses(); ++anOffset)
  {
    const BRepFont_PlanarRegion::Use&   aUse   = theRegion.Uses()[theLoop.FirstUse() + anOffset];
    const BRepFont_PlanarRegion::Curve& aCurve = theRegion.Curves()[aUse.CurveIndex().Value()];
    const gp_XY                         aDerivative = aCurve.D1(0.5);
    const double                        aLength     = aDerivative.Modulus();
    if (aLength <= theTolerance)
    {
      continue;
    }
    gp_XY aNormal(-aDerivative.Y() / aLength, aDerivative.X() / aLength);
    if (theLoop.SignedArea() < 0.0)
    {
      aNormal.Reverse();
    }
    double anInteriorOffset = anInitialOffset;
    for (int anAttempt = 0; anAttempt < 24; ++anAttempt)
    {
      const gp_XY aCandidate = aCurve.Value(0.5) + aNormal * anInteriorOffset;
      if (contains(theLoop, thePoints, theFlattened, aCandidate))
      {
        return aCandidate;
      }
      anInteriorOffset *= 0.5;
    }
  }

  const size_t anAfterLastPoint =
    static_cast<size_t>(theFlattened.FirstPoint) + theFlattened.NbPoints;
  for (size_t aPointIndex = theFlattened.FirstPoint; aPointIndex + 1 < anAfterLastPoint;
       ++aPointIndex)
  {
    const gp_XY& aFirst      = thePoints[aPointIndex];
    const gp_XY& aLast       = thePoints[aPointIndex + 1];
    const gp_XY  aDerivative = aLast - aFirst;
    const double aLength     = aDerivative.Modulus();
    if (aLength <= theTolerance)
    {
      continue;
    }
    gp_XY aNormal(-aDerivative.Y() / aLength, aDerivative.X() / aLength);
    if (theLoop.SignedArea() < 0.0)
    {
      aNormal.Reverse();
    }
    double anInteriorOffset = anInitialOffset;
    for (int anAttempt = 0; anAttempt < 24; ++anAttempt)
    {
      const gp_XY aCandidate = 0.5 * (aFirst + aLast) + aNormal * anInteriorOffset;
      if (contains(theLoop, thePoints, theFlattened, aCandidate))
      {
        return aCandidate;
      }
      anInteriorOffset *= 0.5;
    }
  }

  const gp_XY aBoundsCenter(0.5 * (aMinX + aMaxX), 0.5 * (aMinY + aMaxY));
  return contains(theLoop, thePoints, theFlattened, aBoundsCenter)
           ? std::optional<gp_XY>(aBoundsCenter)
           : std::nullopt;
}
} // namespace

//=================================================================================================

bool BRepFont_Regularizer::classifyLoops(
  const Font_GlyphOutline&             theOutline,
  BRepFont_PlanarRegion&               theRegion,
  const double                         theTolerance,
  const NCollection_FlatMap<uint64_t>& theIntersectingLoopPairs)
{
  NCollection_LinearVector<gp_XY>         aFlattenedPoints;
  NCollection_LinearVector<FlattenedLoop> aFlattenedLoops;
  if (!flattenLoops(theRegion, theTolerance, aFlattenedPoints, aFlattenedLoops))
  {
    return false;
  }

  NCollection_LinearVector<LoopInfo> aLoopInfo(theRegion.myLoops.Size());
  aLoopInfo.Resize(theRegion.myLoops.Size());
  NCollection_LinearVector<uint32_t> anOrder(theRegion.myLoops.Size());
  for (uint32_t aLoopIndex = 0; aLoopIndex < theRegion.myLoops.Size(); ++aLoopIndex)
  {
    const std::optional<gp_XY> anInteriorPoint = interiorPoint(theRegion,
                                                               theRegion.myLoops[aLoopIndex],
                                                               aFlattenedPoints,
                                                               aFlattenedLoops[aLoopIndex],
                                                               theTolerance);
    if (!anInteriorPoint.has_value())
    {
      return false;
    }
    aLoopInfo[aLoopIndex].InteriorPoint = *anInteriorPoint;
    anOrder.Append(aLoopIndex);
  }
  std::sort(anOrder.begin(), anOrder.end(), [&](const uint32_t theLeft, const uint32_t theRight) {
    const double aLeftArea  = std::abs(theRegion.myLoops[theLeft].SignedArea());
    const double aRightArea = std::abs(theRegion.myLoops[theRight].SignedArea());
    return aLeftArea == aRightArea ? theLeft < theRight : aLeftArea > aRightArea;
  });

  for (size_t anOrderPosition = 0; anOrderPosition < anOrder.Size(); ++anOrderPosition)
  {
    const uint32_t                     aLoopIndex = anOrder[anOrderPosition];
    const BRepFont_PlanarRegion::Loop& aLoop      = theRegion.myLoops[aLoopIndex];
    for (size_t aCandidatePosition = anOrderPosition; aCandidatePosition > 0; --aCandidatePosition)
    {
      const uint32_t                     aCandidateIndex = anOrder[aCandidatePosition - 1];
      const BRepFont_PlanarRegion::Loop& aCandidate      = theRegion.myLoops[aCandidateIndex];
      const double                       aCandidateArea  = std::abs(aCandidate.SignedArea());
      if (aCandidateArea <= std::abs(aLoop.SignedArea())
          || (!theIntersectingLoopPairs.IsEmpty()
              && theIntersectingLoopPairs.Contains(loopPairKey(aLoopIndex, aCandidateIndex)))
          || aCandidate.Bounds().IsOut(aLoop.Bounds())
          || !contains(aCandidate,
                       aFlattenedPoints,
                       aFlattenedLoops[aCandidateIndex],
                       aLoopInfo[aLoopIndex].InteriorPoint))
      {
        continue;
      }
      aLoopInfo[aLoopIndex].Parent = aCandidateIndex;
      break;
    }

    int  aParentWinding = 0;
    bool isParentFilled = false;
    if (aLoopInfo[aLoopIndex].Parent.has_value())
    {
      const LoopInfo& aParent = aLoopInfo[*aLoopInfo[aLoopIndex].Parent];
      aParentWinding          = aParent.Winding;
      isParentFilled          = aParent.IsFilled;
    }
    const int aDirection           = aLoop.SignedArea() > 0.0 ? 1 : -1;
    aLoopInfo[aLoopIndex].Winding  = aParentWinding + aDirection;
    aLoopInfo[aLoopIndex].IsFilled = theOutline.Rule() == Font_GlyphOutline::FillRule::EvenOdd
                                       ? !isParentFilled
                                       : aLoopInfo[aLoopIndex].Winding != 0;
    if (!isParentFilled && aLoopInfo[aLoopIndex].IsFilled)
    {
      aLoopInfo[aLoopIndex].BoundaryRole = LoopInfo::Role::Outer;
    }
    else if (isParentFilled && !aLoopInfo[aLoopIndex].IsFilled)
    {
      aLoopInfo[aLoopIndex].BoundaryRole = LoopInfo::Role::Hole;
    }
  }

  NCollection_LinearVector<HoleBinding> aHoles;
  for (uint32_t aLoopIndex = 0; aLoopIndex < aLoopInfo.Size(); ++aLoopIndex)
  {
    if (aLoopInfo[aLoopIndex].BoundaryRole != LoopInfo::Role::Hole)
    {
      continue;
    }
    std::optional<uint32_t> anAncestor = aLoopInfo[aLoopIndex].Parent;
    while (anAncestor.has_value() && aLoopInfo[*anAncestor].BoundaryRole != LoopInfo::Role::Outer)
    {
      anAncestor = aLoopInfo[*anAncestor].Parent;
    }
    if (anAncestor.has_value())
    {
      aHoles.EmplaceAppend(*anAncestor, aLoopIndex);
    }
  }
  std::sort(aHoles.begin(),
            aHoles.end(),
            [](const HoleBinding& theLeft, const HoleBinding& theRight) {
              return theLeft.OuterLoop == theRight.OuterLoop
                       ? theLeft.HoleLoop < theRight.HoleLoop
                       : theLeft.OuterLoop < theRight.OuterLoop;
            });

  bool toCompact = false;
  for (const LoopInfo& aLoop : aLoopInfo)
  {
    if (aLoop.BoundaryRole == LoopInfo::Role::None)
    {
      toCompact = true;
      break;
    }
  }

  constexpr uint32_t                 THE_INVALID_INDEX = std::numeric_limits<uint32_t>::max();
  NCollection_LinearVector<uint32_t> aVertexMap;
  NCollection_LinearVector<uint32_t> aCurveMap;
  NCollection_LinearVector<uint32_t> aLoopMap;
  BRepFont_PlanarRegion              aBoundaryRegion;
  if (!toCompact)
  {
    aBoundaryRegion = std::move(theRegion);
  }
  else
  {
    aVertexMap.Resize(theRegion.myVertices.Size(), THE_INVALID_INDEX);
    aCurveMap.Resize(theRegion.myCurves.Size(), THE_INVALID_INDEX);
    aLoopMap.Resize(theRegion.myLoops.Size(), THE_INVALID_INDEX);
    aBoundaryRegion.myUnitsPerEm     = theRegion.myUnitsPerEm;
    aBoundaryRegion.myTolerance      = theRegion.myTolerance;
    aBoundaryRegion.myIsSingleStroke = theRegion.myIsSingleStroke;

    for (uint32_t aLoopIndex = 0; aLoopIndex < aLoopInfo.Size(); ++aLoopIndex)
    {
      if (aLoopInfo[aLoopIndex].BoundaryRole == LoopInfo::Role::None)
      {
        continue;
      }

      const BRepFont_PlanarRegion::Loop& anOldLoop     = theRegion.myLoops[aLoopIndex];
      const std::optional<uint32_t>      aNewLoopIndex = index(aBoundaryRegion.myLoops.Size());
      const std::optional<uint32_t>      aFirstUse     = index(aBoundaryRegion.myUses.Size());
      if (!aNewLoopIndex.has_value() || !aFirstUse.has_value())
      {
        return false;
      }
      aLoopMap[aLoopIndex] = *aNewLoopIndex;

      for (uint32_t anOffset = 0; anOffset < anOldLoop.NbUses(); ++anOffset)
      {
        const BRepFont_PlanarRegion::Use& anOldUse =
          theRegion.myUses[anOldLoop.FirstUse() + anOffset];
        const uint32_t anOldCurve       = anOldUse.CurveIndex().Value();
        const uint32_t anOldFirstVertex = anOldUse.FirstVertex().Value();
        const uint32_t anOldLastVertex  = anOldUse.LastVertex().Value();
        if (anOldCurve >= aCurveMap.Size() || anOldFirstVertex >= aVertexMap.Size()
            || anOldLastVertex >= aVertexMap.Size())
        {
          return false;
        }

        if (aCurveMap[anOldCurve] == THE_INVALID_INDEX)
        {
          const std::optional<uint32_t> aNewCurve = index(aBoundaryRegion.myCurves.Size());
          if (!aNewCurve.has_value())
          {
            return false;
          }
          aCurveMap[anOldCurve] = *aNewCurve;
          aBoundaryRegion.myCurves.Append(theRegion.myCurves[anOldCurve]);
        }
        if (aVertexMap[anOldFirstVertex] == THE_INVALID_INDEX)
        {
          const std::optional<uint32_t> aNewVertex = index(aBoundaryRegion.myVertices.Size());
          if (!aNewVertex.has_value())
          {
            return false;
          }
          aVertexMap[anOldFirstVertex] = *aNewVertex;
          aBoundaryRegion.myVertices.Append(theRegion.myVertices[anOldFirstVertex]);
        }
        if (aVertexMap[anOldLastVertex] == THE_INVALID_INDEX)
        {
          const std::optional<uint32_t> aNewVertex = index(aBoundaryRegion.myVertices.Size());
          if (!aNewVertex.has_value())
          {
            return false;
          }
          aVertexMap[anOldLastVertex] = *aNewVertex;
          aBoundaryRegion.myVertices.Append(theRegion.myVertices[anOldLastVertex]);
        }

        aBoundaryRegion.myUses.EmplaceAppend(
          BRepFont_PlanarRegion::CurveId(aCurveMap[anOldCurve]),
          BRepFont_PlanarRegion::VertexId(aVertexMap[anOldFirstVertex]),
          BRepFont_PlanarRegion::VertexId(aVertexMap[anOldLastVertex]),
          anOldUse.FirstParameter(),
          anOldUse.LastParameter(),
          anOldUse.IsReversed());
      }

      const std::optional<uint32_t> anAfterLastUse = index(aBoundaryRegion.myUses.Size());
      if (!anAfterLastUse.has_value())
      {
        return false;
      }
      aBoundaryRegion.myLoops.EmplaceAppend(*aFirstUse,
                                            *anAfterLastUse - *aFirstUse,
                                            anOldLoop.SignedArea(),
                                            anOldLoop.Bounds());
    }
  }

  size_t aHoleIndex = 0;
  for (uint32_t anOldLoopIndex = 0; anOldLoopIndex < aLoopInfo.Size(); ++anOldLoopIndex)
  {
    const uint32_t aNewOuterLoop = toCompact ? aLoopMap[anOldLoopIndex] : anOldLoopIndex;
    if (aLoopInfo[anOldLoopIndex].BoundaryRole != LoopInfo::Role::Outer
        || aNewOuterLoop == THE_INVALID_INDEX)
    {
      continue;
    }
    const std::optional<uint32_t> aFirstHole = index(aBoundaryRegion.myHoleLoops.Size());
    if (!aFirstHole.has_value())
    {
      return false;
    }
    while (aHoleIndex < aHoles.Size() && aHoles[aHoleIndex].OuterLoop == anOldLoopIndex)
    {
      const uint32_t aNewHole =
        toCompact ? aLoopMap[aHoles[aHoleIndex].HoleLoop] : aHoles[aHoleIndex].HoleLoop;
      if (aNewHole == THE_INVALID_INDEX)
      {
        return false;
      }
      aBoundaryRegion.myHoleLoops.Append(BRepFont_PlanarRegion::LoopId(aNewHole));
      ++aHoleIndex;
    }
    const std::optional<uint32_t> anAfterLastHole = index(aBoundaryRegion.myHoleLoops.Size());
    if (!anAfterLastHole.has_value())
    {
      return false;
    }
    aBoundaryRegion.myRegions.EmplaceAppend(BRepFont_PlanarRegion::LoopId(aNewOuterLoop),
                                            *aFirstHole,
                                            *anAfterLastHole - *aFirstHole);
  }
  theRegion = std::move(aBoundaryRegion);
  return true;
}
