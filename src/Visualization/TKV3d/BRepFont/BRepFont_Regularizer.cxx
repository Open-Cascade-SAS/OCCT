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
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>
#include <gp_Pnt2d.hxx>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>

namespace
{
bool isValidReal(const double theValue) noexcept
{
  return !std::isnan(theValue) && !Precision::IsInfinite(theValue);
}

enum class IntersectionKind : uint8_t
{
  None,
  BetweenLoops,
  WithinLoop
};

enum class LineIntersectionKind : uint8_t
{
  None,
  Point,
  Overlap
};

struct LineIntersection
{
  LineIntersectionKind Kind = LineIntersectionKind::None;
  std::array<gp_XY, 2> Points;
  size_t               NbPoints = 0;
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

bool isValidPoint(const gp_XY& thePoint);

Font_GlyphOutline::Segment healEndpoints(const Font_GlyphOutline::Segment& theSegment,
                                         const gp_XY&                      theStart,
                                         const gp_XY&                      theEnd);

uint64_t loopPairKey(uint32_t theFirst, uint32_t theSecond) noexcept;

IntersectionKind intersections(const BRepFont_PlanarRegion&   theRegion,
                               double                         theTolerance,
                               NCollection_FlatMap<uint64_t>& theLoopPairs);

occ::handle<Geom2d_Curve> curveGeometry(const BRepFont_PlanarRegion::Curve& theCurve);

LineIntersection intersectLines(const BRepFont_PlanarRegion::Curve& theFirst,
                                const BRepFont_PlanarRegion::Curve& theSecond,
                                double                              theTolerance);

LineIntersection intersectLineQuadratic(const BRepFont_PlanarRegion::Curve& theLine,
                                        const BRepFont_PlanarRegion::Curve& theQuadratic,
                                        double                              theTolerance);

bool contains(const BRepFont_PlanarRegion&       theRegion,
              const BRepFont_PlanarRegion::Loop& theLoop,
              const gp_XY&                       thePoint);

std::optional<gp_XY> interiorPoint(const BRepFont_PlanarRegion&       theRegion,
                                   const BRepFont_PlanarRegion::Loop& theLoop);

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
  if (!(theOutline.UnitsPerEm() > 0.0) || !isValidReal(theOutline.UnitsPerEm())
      || !(theOptions.Tolerance > 0.0) || !isValidReal(theOptions.Tolerance)
      || !isValidReal(theOptions.Tolerance * theOptions.Tolerance))
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
    if (!classifyLoops(theOutline, aRegion, anIntersectingLoopPairs))
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

bool isValidPoint(const gp_XY& thePoint)
{
  return isValidReal(thePoint.X()) && isValidReal(thePoint.Y());
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

    for (size_t aLocalIndex = 0; aLocalIndex < aContour.NbSegments(); ++aLocalIndex)
    {
      const size_t                      aSourceIndex = aContour.FirstSegment() + aLocalIndex;
      const Font_GlyphOutline::Segment& aSegment     = aSegments[aSourceIndex];
      if (!isValidPoint(aSegment.Start()) || !isValidPoint(aSegment.End())
          || (aSegment.Type() != Font_GlyphOutline::Segment::Kind::Line
              && !isValidPoint(aSegment.Control1()))
          || (aSegment.Type() == Font_GlyphOutline::Segment::Kind::CubicBezier
              && !isValidPoint(aSegment.Control2())))
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
      if (!isValidReal(aCurveArea) || !isValidReal(aSignedArea + aCurveArea))
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
        if (!isValidReal(anOldArea) || !isValidReal(aHealedArea)
            || !isValidReal(aSignedArea + aHealedArea - anOldArea))
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

LineIntersection intersectLines(const BRepFont_PlanarRegion::Curve& theFirst,
                                const BRepFont_PlanarRegion::Curve& theSecond,
                                const double                        theTolerance)
{
  const gp_XY  aFirstDirection  = theFirst.End() - theFirst.Start();
  const gp_XY  aSecondDirection = theSecond.End() - theSecond.Start();
  const double aFirstLength     = aFirstDirection.Modulus();
  const double aSecondLength    = aSecondDirection.Modulus();
  if (aFirstLength == 0.0 || aSecondLength == 0.0)
  {
    return {};
  }

  const gp_XY  anOffset        = theSecond.Start() - theFirst.Start();
  const double aCross          = aFirstDirection.Crossed(aSecondDirection);
  const double aCrossTolerance = theTolerance * (aFirstLength + aSecondLength);
  const double aFirstParamTol  = theTolerance / aFirstLength;
  const double aSecondParamTol = theTolerance / aSecondLength;
  if (std::abs(aCross) <= aCrossTolerance)
  {
    const bool isCoincident =
      std::abs(anOffset.Crossed(aFirstDirection)) <= theTolerance * aFirstLength
      && std::abs((theSecond.End() - theFirst.Start()).Crossed(aFirstDirection))
           <= theTolerance * aFirstLength;
    if (isCoincident)
    {
      const double aFirstSquareLength = aFirstDirection.SquareModulus();
      double       aFirstParameter    = anOffset.Dot(aFirstDirection) / aFirstSquareLength;
      double       aLastParameter =
        (theSecond.End() - theFirst.Start()).Dot(aFirstDirection) / aFirstSquareLength;
      if (aLastParameter < aFirstParameter)
      {
        std::swap(aFirstParameter, aLastParameter);
      }
      const double anOverlapFirst = std::max(0.0, aFirstParameter);
      const double anOverlapLast  = std::min(1.0, aLastParameter);
      if (anOverlapLast - anOverlapFirst > aFirstParamTol)
      {
        LineIntersection aResult;
        aResult.Kind = LineIntersectionKind::Overlap;
        return aResult;
      }
      if (anOverlapLast + aFirstParamTol < anOverlapFirst)
      {
        return {};
      }

      const double     aParameter = std::clamp(0.5 * (anOverlapFirst + anOverlapLast), 0.0, 1.0);
      LineIntersection aResult;
      aResult.Kind      = LineIntersectionKind::Point;
      aResult.Points[0] = theFirst.Start() + aFirstDirection * aParameter;
      aResult.NbPoints  = 1;
      return aResult;
    }
    if (aCross == 0.0)
    {
      return {};
    }
  }

  const double aFirstParameter  = anOffset.Crossed(aSecondDirection) / aCross;
  const double aSecondParameter = anOffset.Crossed(aFirstDirection) / aCross;
  if (aFirstParameter < -aFirstParamTol || aFirstParameter > 1.0 + aFirstParamTol
      || aSecondParameter < -aSecondParamTol || aSecondParameter > 1.0 + aSecondParamTol)
  {
    return {};
  }

  const gp_XY aFirstPoint =
    theFirst.Start() + aFirstDirection * std::clamp(aFirstParameter, 0.0, 1.0);
  const gp_XY aSecondPoint =
    theSecond.Start() + aSecondDirection * std::clamp(aSecondParameter, 0.0, 1.0);
  LineIntersection aResult;
  aResult.Kind      = LineIntersectionKind::Point;
  aResult.Points[0] = 0.5 * (aFirstPoint + aSecondPoint);
  aResult.NbPoints  = 1;
  return aResult;
}

//=================================================================================================

LineIntersection intersectLineQuadratic(const BRepFont_PlanarRegion::Curve& theLine,
                                        const BRepFont_PlanarRegion::Curve& theQuadratic,
                                        const double                        theTolerance)
{
  const gp_XY  aLineDirection = theLine.End() - theLine.Start();
  const double aLineLength    = aLineDirection.Modulus();
  if (aLineLength == 0.0)
  {
    return {};
  }

  const double aLineSquareLength = aLineDirection.SquareModulus();
  const double aLineParamTol     = theTolerance / aLineLength;
  const double aCrossTolerance   = theTolerance * aLineLength;
  const auto   crossFromLine     = [&](const gp_XY& thePoint) {
    return (thePoint - theLine.Start()).Crossed(aLineDirection);
  };
  const double aStartCross   = crossFromLine(theQuadratic.Start());
  const double aControlCross = crossFromLine(theQuadratic.Control1());
  const double anEndCross    = crossFromLine(theQuadratic.End());

  if (std::abs(aStartCross) <= aCrossTolerance && std::abs(aControlCross) <= aCrossTolerance
      && std::abs(anEndCross) <= aCrossTolerance)
  {
    const auto lineParameter = [&](const gp_XY& thePoint) {
      return (thePoint - theLine.Start()).Dot(aLineDirection) / aLineSquareLength;
    };
    const double aStartParameter   = lineParameter(theQuadratic.Start());
    const double aControlParameter = lineParameter(theQuadratic.Control1());
    const double anEndParameter    = lineParameter(theQuadratic.End());
    double       aMinParameter     = std::min(aStartParameter, anEndParameter);
    double       aMaxParameter     = std::max(aStartParameter, anEndParameter);
    const double aDenominator      = aStartParameter - 2.0 * aControlParameter + anEndParameter;
    if (aDenominator != 0.0)
    {
      const double anExtremum = (aStartParameter - aControlParameter) / aDenominator;
      if (anExtremum > 0.0 && anExtremum < 1.0)
      {
        const double aU     = 1.0 - anExtremum;
        const double aValue = aStartParameter * (aU * aU)
                              + aControlParameter * (2.0 * aU * anExtremum)
                              + anEndParameter * (anExtremum * anExtremum);
        aMinParameter = std::min(aMinParameter, aValue);
        aMaxParameter = std::max(aMaxParameter, aValue);
      }
    }

    const double anOverlapFirst = std::max(0.0, aMinParameter);
    const double anOverlapLast  = std::min(1.0, aMaxParameter);
    if (anOverlapLast - anOverlapFirst > aLineParamTol)
    {
      LineIntersection aResult;
      aResult.Kind = LineIntersectionKind::Overlap;
      return aResult;
    }
    if (anOverlapLast + aLineParamTol < anOverlapFirst)
    {
      return {};
    }

    LineIntersection aResult;
    aResult.Kind = LineIntersectionKind::Point;
    aResult.Points[0] =
      theLine.Start()
      + aLineDirection * std::clamp(0.5 * (anOverlapFirst + anOverlapLast), 0.0, 1.0);
    aResult.NbPoints = 1;
    return aResult;
  }

  LineIntersection aResult;
  const auto       appendParameter = [&](const long double theParameter) {
    const double aParameter = static_cast<double>(theParameter);
    if (!isValidReal(aParameter) || aParameter < 0.0 || aParameter > 1.0)
    {
      return;
    }
    const gp_XY  aCurvePoint = theQuadratic.Value(aParameter);
    const double aCross      = crossFromLine(aCurvePoint);
    if (std::abs(aCross) > aCrossTolerance)
    {
      return;
    }
    const double aLineParameter =
      (aCurvePoint - theLine.Start()).Dot(aLineDirection) / aLineSquareLength;
    if (aLineParameter < -aLineParamTol || aLineParameter > 1.0 + aLineParamTol)
    {
      return;
    }
    const gp_XY aLinePoint =
      theLine.Start() + aLineDirection * std::clamp(aLineParameter, 0.0, 1.0);
    const gp_XY anIntersection = 0.5 * (aCurvePoint + aLinePoint);
    for (size_t aPointIndex = 0; aPointIndex < aResult.NbPoints; ++aPointIndex)
    {
      if ((aResult.Points[aPointIndex] - anIntersection).SquareModulus()
          <= theTolerance * theTolerance)
      {
        return;
      }
    }
    if (aResult.NbPoints < aResult.Points.size())
    {
      aResult.Points[aResult.NbPoints++] = anIntersection;
      aResult.Kind                       = LineIntersectionKind::Point;
    }
  };

  const long double anA = static_cast<long double>(aStartCross) - 2.0L * aControlCross + anEndCross;
  const long double aB  = 2.0L * (static_cast<long double>(aControlCross) - aStartCross);
  const long double aC  = aStartCross;
  if (anA == 0.0L)
  {
    if (aB != 0.0L)
    {
      appendParameter(-aC / aB);
    }
  }
  else
  {
    const long double aDiscriminant = aB * aB - 4.0L * anA * aC;
    if (aDiscriminant == 0.0L)
    {
      appendParameter(-aB / (2.0L * anA));
    }
    else if (aDiscriminant > 0.0L)
    {
      const long double aSquareRoot = std::sqrt(aDiscriminant);
      const long double aQ          = -0.5L * (aB + std::copysign(aSquareRoot, aB));
      appendParameter(aQ / anA);
      appendParameter(aC / aQ);
    }
  }

  appendParameter(0.0L);
  appendParameter(1.0L);
  if (anA != 0.0L)
  {
    appendParameter(-aB / (2.0L * anA));
  }
  return aResult;
}

//=================================================================================================

IntersectionKind intersections(const BRepFont_PlanarRegion&   theRegion,
                               const double                   theTolerance,
                               NCollection_FlatMap<uint64_t>& theLoopPairs)
{
  const auto&      aCurves = theRegion.Curves();
  IntersectionKind aResult = IntersectionKind::None;

  NCollection_LinearVector<uint32_t> aCurveLoops(aCurves.Size(), 0);
  NCollection_LinearVector<uint32_t> aCurveOffsets(aCurves.Size(), 0);
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

  NCollection_LinearVector<occ::handle<Geom2d_Curve>> aGeometry(aCurves.Size(),
                                                                occ::handle<Geom2d_Curve>());
  NCollection_LinearVector<IntersectionCurve>         aSweep(aCurves.Size());
  const auto geometry = [&](const uint32_t theCurveIndex) -> const occ::handle<Geom2d_Curve>& {
    occ::handle<Geom2d_Curve>& aCurveGeometry = aGeometry[theCurveIndex];
    if (aCurveGeometry.IsNull())
    {
      aCurveGeometry = curveGeometry(aCurves[theCurveIndex]);
    }
    return aCurveGeometry;
  };

  const double aSquareTolerance = theTolerance * theTolerance;
  for (uint32_t aCurveIndex = 0; aCurveIndex < aCurves.Size(); ++aCurveIndex)
  {
    const BRepFont_PlanarRegion::Curve& aCurve = aCurves[aCurveIndex];

    bool toCheckSelfIntersection = aCurve.Type() == Font_GlyphOutline::Segment::Kind::CubicBezier;
    if (aCurve.Type() == Font_GlyphOutline::Segment::Kind::QuadraticBezier)
    {
      const gp_XY aFirstTangent = aCurve.Control1() - aCurve.Start();
      const gp_XY aLastTangent  = aCurve.End() - aCurve.Control1();
      const bool  isMonotonicX  = (aFirstTangent.X() >= 0.0 && aLastTangent.X() >= 0.0
                                 && (aFirstTangent.X() > 0.0 || aLastTangent.X() > 0.0))
                                || (aFirstTangent.X() <= 0.0 && aLastTangent.X() <= 0.0
                                    && (aFirstTangent.X() < 0.0 || aLastTangent.X() < 0.0));
      const bool isMonotonicY = (aFirstTangent.Y() >= 0.0 && aLastTangent.Y() >= 0.0
                                 && (aFirstTangent.Y() > 0.0 || aLastTangent.Y() > 0.0))
                                || (aFirstTangent.Y() <= 0.0 && aLastTangent.Y() <= 0.0
                                    && (aFirstTangent.Y() < 0.0 || aLastTangent.Y() < 0.0));
      toCheckSelfIntersection = !isMonotonicX && !isMonotonicY;
    }
    if (toCheckSelfIntersection)
    {
      Geom2dAPI_InterCurveCurve aSelfIntersector(geometry(aCurveIndex), theTolerance);
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

      const uint32_t aSecondIndex = aSecond.Index;
      const bool     isSameLoop   = aCurveLoops[aFirstIndex] == aCurveLoops[aSecondIndex];

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

      const Font_GlyphOutline::Segment::Kind aFirstType  = aCurves[aFirstIndex].Type();
      const Font_GlyphOutline::Segment::Kind aSecondType = aCurves[aSecondIndex].Type();
      std::optional<LineIntersection>        aDirectIntersection;
      if (aFirstType == Font_GlyphOutline::Segment::Kind::Line
          && aSecondType == Font_GlyphOutline::Segment::Kind::Line)
      {
        aDirectIntersection =
          intersectLines(aCurves[aFirstIndex], aCurves[aSecondIndex], theTolerance);
      }
      else if ((aFirstType == Font_GlyphOutline::Segment::Kind::Line
                && aSecondType == Font_GlyphOutline::Segment::Kind::QuadraticBezier)
               || (aFirstType == Font_GlyphOutline::Segment::Kind::QuadraticBezier
                   && aSecondType == Font_GlyphOutline::Segment::Kind::Line))
      {
        const BRepFont_PlanarRegion::Curve& aLine =
          aFirstType == Font_GlyphOutline::Segment::Kind::Line ? aCurves[aFirstIndex]
                                                               : aCurves[aSecondIndex];
        const BRepFont_PlanarRegion::Curve& aQuadratic =
          aFirstType == Font_GlyphOutline::Segment::Kind::QuadraticBezier ? aCurves[aFirstIndex]
                                                                          : aCurves[aSecondIndex];
        aDirectIntersection = intersectLineQuadratic(aLine, aQuadratic, theTolerance);
      }

      if (aDirectIntersection.has_value())
      {
        if (aDirectIntersection->Kind == LineIntersectionKind::Overlap)
        {
          if (isSameLoop)
          {
            return IntersectionKind::WithinLoop;
          }
          theLoopPairs.Add(loopPairKey(aCurveLoops[aFirstIndex], aCurveLoops[aSecondIndex]));
          aResult = IntersectionKind::BetweenLoops;
        }
        else
        {
          for (size_t aPointIndex = 0; aPointIndex < aDirectIntersection->NbPoints; ++aPointIndex)
          {
            if (isAdjacent
                && (aDirectIntersection->Points[aPointIndex] - aSharedPoint).SquareModulus()
                     <= aSquareTolerance)
            {
              continue;
            }
            if (isSameLoop)
            {
              return IntersectionKind::WithinLoop;
            }
            theLoopPairs.Add(loopPairKey(aCurveLoops[aFirstIndex], aCurveLoops[aSecondIndex]));
            aResult = IntersectionKind::BetweenLoops;
            break;
          }
        }
        continue;
      }

      Geom2dAPI_InterCurveCurve anIntersector(geometry(aFirstIndex),
                                              geometry(aSecondIndex),
                                              theTolerance);
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

double coordinateValue(const Font_GlyphOutline::Segment::Kind theKind,
                       const double                           theStart,
                       const double                           theControl1,
                       const double                           theControl2,
                       const double                           theEnd,
                       const double                           theParameter)
{
  const double aT = theParameter;
  const double aU = 1.0 - aT;
  switch (theKind)
  {
    case Font_GlyphOutline::Segment::Kind::Line:
      return theStart * aU + theEnd * aT;
    case Font_GlyphOutline::Segment::Kind::QuadraticBezier:
      return theStart * (aU * aU) + theControl1 * (2.0 * aU * aT) + theEnd * (aT * aT);
    case Font_GlyphOutline::Segment::Kind::CubicBezier:
      return theStart * (aU * aU * aU) + theControl1 * (3.0 * aU * aU * aT)
             + theControl2 * (3.0 * aU * aT * aT) + theEnd * (aT * aT * aT);
  }
  return theStart;
}

//=================================================================================================

double curveX(const BRepFont_PlanarRegion::Curve& theCurve, const double theParameter)
{
  return coordinateValue(theCurve.Type(),
                         theCurve.Start().X(),
                         theCurve.Control1().X(),
                         theCurve.Control2().X(),
                         theCurve.End().X(),
                         theParameter);
}

//=================================================================================================

double curveY(const BRepFont_PlanarRegion::Curve& theCurve, const double theParameter)
{
  return coordinateValue(theCurve.Type(),
                         theCurve.Start().Y(),
                         theCurve.Control1().Y(),
                         theCurve.Control2().Y(),
                         theCurve.End().Y(),
                         theParameter);
}

//=================================================================================================

size_t yIntervals(const BRepFont_PlanarRegion::Curve& theCurve,
                  const double                        theFirst,
                  const double                        theLast,
                  std::array<double, 4>&              theParameters)
{
  theParameters[0]         = theFirst;
  size_t     aNbParameters = 1;
  const auto appendRoot    = [&](const long double theRoot) {
    const double aRoot = static_cast<double>(theRoot);
    if (isValidReal(aRoot) && aRoot > theFirst && aRoot < theLast)
    {
      theParameters[aNbParameters++] = aRoot;
    }
  };
  switch (theCurve.Type())
  {
    case Font_GlyphOutline::Segment::Kind::Line:
      break;
    case Font_GlyphOutline::Segment::Kind::QuadraticBezier: {
      const long double aQuadratic = static_cast<long double>(theCurve.Start().Y())
                                     - 2.0L * theCurve.Control1().Y() + theCurve.End().Y();
      if (aQuadratic != 0.0L)
      {
        const long double aLinear =
          2.0L * (static_cast<long double>(theCurve.Control1().Y()) - theCurve.Start().Y());
        appendRoot(-aLinear / (2.0L * aQuadratic));
      }
      break;
    }
    case Font_GlyphOutline::Segment::Kind::CubicBezier: {
      const long double aCubic = -static_cast<long double>(theCurve.Start().Y())
                                 + 3.0L * theCurve.Control1().Y() - 3.0L * theCurve.Control2().Y()
                                 + theCurve.End().Y();
      const long double aQuadratic = 3.0L
                                     * (static_cast<long double>(theCurve.Start().Y())
                                        - 2.0L * theCurve.Control1().Y() + theCurve.Control2().Y());
      const long double aLinear =
        3.0L * (static_cast<long double>(theCurve.Control1().Y()) - theCurve.Start().Y());
      const long double anA = 3.0L * aCubic;
      const long double aB  = 2.0L * aQuadratic;
      if (anA == 0.0L)
      {
        if (aB != 0.0L)
        {
          appendRoot(-aLinear / aB);
        }
        break;
      }
      const long double aDiscriminant = aB * aB - 4.0L * anA * aLinear;
      if (aDiscriminant < 0.0L)
      {
        break;
      }
      if (aDiscriminant == 0.0L)
      {
        appendRoot(-aB / (2.0L * anA));
        break;
      }
      const long double aSquareRoot = std::sqrt(aDiscriminant);
      const long double aQ          = -0.5L * (aB + std::copysign(aSquareRoot, aB));
      appendRoot(aQ / anA);
      appendRoot(aLinear / aQ);
      break;
    }
  }
  std::sort(theParameters.begin(), theParameters.begin() + aNbParameters);
  theParameters[aNbParameters++] = theLast;
  return aNbParameters;
}

//=================================================================================================

double horizontalCrossing(const BRepFont_PlanarRegion::Curve& theCurve,
                          const double                        theFirst,
                          const double                        theFirstY,
                          const double                        theLast,
                          const double                        theLastY,
                          const double                        theY)
{
  if (theCurve.Type() == Font_GlyphOutline::Segment::Kind::Line)
  {
    return theFirst + (theLast - theFirst) * (theY - theFirstY) / (theLastY - theFirstY);
  }
  double     aFirst       = theFirst;
  double     aLast        = theLast;
  const bool isFirstAbove = theFirstY > theY;
  for (;;)
  {
    const double aMiddle = 0.5 * (aFirst + aLast);
    if (aMiddle == aFirst || aMiddle == aLast)
    {
      break;
    }
    if ((curveY(theCurve, aMiddle) > theY) == isFirstAbove)
    {
      aFirst = aMiddle;
    }
    else
    {
      aLast = aMiddle;
    }
  }
  return 0.5 * (aFirst + aLast);
}

//=================================================================================================

bool contains(const BRepFont_PlanarRegion&       theRegion,
              const BRepFont_PlanarRegion::Loop& theLoop,
              const gp_XY&                       thePoint)
{
  if (theLoop.Bounds().IsOut(gp_Pnt2d(thePoint)))
  {
    return false;
  }

  int aWinding = 0;
  for (uint32_t anOffset = 0; anOffset < theLoop.NbUses(); ++anOffset)
  {
    const BRepFont_PlanarRegion::Use& aUse = theRegion.Uses()[theLoop.FirstUse() + anOffset];
    if (aUse.CurveIndex().Value() >= theRegion.Curves().Size())
    {
      return false;
    }
    const BRepFont_PlanarRegion::Curve& aCurve = theRegion.Curves()[aUse.CurveIndex().Value()];
    std::array<double, 4>               aParameters;
    const size_t                        aNbParameters =
      yIntervals(aCurve, aUse.FirstParameter(), aUse.LastParameter(), aParameters);
    for (size_t anInterval = 0; anInterval + 1 < aNbParameters; ++anInterval)
    {
      const double aFirst =
        aUse.IsReversed() ? aParameters[aNbParameters - anInterval - 1] : aParameters[anInterval];
      const double aLast      = aUse.IsReversed() ? aParameters[aNbParameters - anInterval - 2]
                                                  : aParameters[anInterval + 1];
      const double aFirstY    = curveY(aCurve, aFirst);
      const double aLastY     = curveY(aCurve, aLast);
      const bool   isUpward   = aFirstY <= thePoint.Y() && aLastY > thePoint.Y();
      const bool   isDownward = aFirstY > thePoint.Y() && aLastY <= thePoint.Y();
      if (!isUpward && !isDownward)
      {
        continue;
      }
      const double aParameter =
        horizontalCrossing(aCurve, aFirst, aFirstY, aLast, aLastY, thePoint.Y());
      if (curveX(aCurve, aParameter) > thePoint.X())
      {
        aWinding += isUpward ? 1 : -1;
      }
    }
  }
  return aWinding != 0;
}

//=================================================================================================

std::optional<gp_XY> interiorPoint(const BRepFont_PlanarRegion&       theRegion,
                                   const BRepFont_PlanarRegion::Loop& theLoop)
{
  NCollection_LinearVector<double> aCriticalY;
  for (uint32_t anOffset = 0; anOffset < theLoop.NbUses(); ++anOffset)
  {
    const BRepFont_PlanarRegion::Use& aUse = theRegion.Uses()[theLoop.FirstUse() + anOffset];
    if (aUse.CurveIndex().Value() >= theRegion.Curves().Size())
    {
      return std::nullopt;
    }
    const BRepFont_PlanarRegion::Curve& aCurve = theRegion.Curves()[aUse.CurveIndex().Value()];
    std::array<double, 4>               aParameters;
    const size_t                        aNbParameters =
      yIntervals(aCurve, aUse.FirstParameter(), aUse.LastParameter(), aParameters);
    for (size_t aParameter = 0; aParameter < aNbParameters; ++aParameter)
    {
      aCriticalY.Append(curveY(aCurve, aParameters[aParameter]));
    }
  }
  std::sort(aCriticalY.begin(), aCriticalY.end());
  size_t aNbCriticalY = 0;
  for (const double aY : aCriticalY)
  {
    if (aNbCriticalY == 0 || aY != aCriticalY[aNbCriticalY - 1])
    {
      aCriticalY.ChangeValue(aNbCriticalY++) = aY;
    }
  }
  aCriticalY.Resize(aNbCriticalY);

  NCollection_LinearVector<double> aCrossings;
  for (size_t aSlab = 0; aSlab + 1 < aCriticalY.Size(); ++aSlab)
  {
    const double aY = 0.5 * aCriticalY[aSlab] + 0.5 * aCriticalY[aSlab + 1];
    if (!(aY > aCriticalY[aSlab] && aY < aCriticalY[aSlab + 1]))
    {
      continue;
    }
    aCrossings.Clear();
    for (uint32_t anOffset = 0; anOffset < theLoop.NbUses(); ++anOffset)
    {
      const BRepFont_PlanarRegion::Use&   aUse   = theRegion.Uses()[theLoop.FirstUse() + anOffset];
      const BRepFont_PlanarRegion::Curve& aCurve = theRegion.Curves()[aUse.CurveIndex().Value()];
      std::array<double, 4>               aParameters;
      const size_t                        aNbParameters =
        yIntervals(aCurve, aUse.FirstParameter(), aUse.LastParameter(), aParameters);
      for (size_t anInterval = 0; anInterval + 1 < aNbParameters; ++anInterval)
      {
        const double aFirst =
          aUse.IsReversed() ? aParameters[aNbParameters - anInterval - 1] : aParameters[anInterval];
        const double aLast   = aUse.IsReversed() ? aParameters[aNbParameters - anInterval - 2]
                                                 : aParameters[anInterval + 1];
        const double aFirstY = curveY(aCurve, aFirst);
        const double aLastY  = curveY(aCurve, aLast);
        if ((aFirstY <= aY && aLastY > aY) || (aFirstY > aY && aLastY <= aY))
        {
          const double aParameter = horizontalCrossing(aCurve, aFirst, aFirstY, aLast, aLastY, aY);
          aCrossings.Append(curveX(aCurve, aParameter));
        }
      }
    }
    std::sort(aCrossings.begin(), aCrossings.end());
    for (size_t aCrossing = 0; aCrossing + 1 < aCrossings.Size(); ++aCrossing)
    {
      const double anX = 0.5 * aCrossings[aCrossing] + 0.5 * aCrossings[aCrossing + 1];
      if (anX > aCrossings[aCrossing] && anX < aCrossings[aCrossing + 1]
          && contains(theRegion, theLoop, gp_XY(anX, aY)))
      {
        return gp_XY(anX, aY);
      }
    }
  }
  return std::nullopt;
}
} // namespace

//=================================================================================================

bool BRepFont_Regularizer::classifyLoops(
  const Font_GlyphOutline&             theOutline,
  BRepFont_PlanarRegion&               theRegion,
  const NCollection_FlatMap<uint64_t>& theIntersectingLoopPairs)
{
  NCollection_LinearVector<LoopInfo> aLoopInfo(theRegion.myLoops.Size(), LoopInfo{});
  NCollection_LinearVector<uint32_t> anOrder(theRegion.myLoops.Size());
  for (uint32_t aLoopIndex = 0; aLoopIndex < theRegion.myLoops.Size(); ++aLoopIndex)
  {
    const std::optional<gp_XY> anInteriorPoint =
      interiorPoint(theRegion, theRegion.myLoops[aLoopIndex]);
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
          || !contains(theRegion, aCandidate, aLoopInfo[aLoopIndex].InteriorPoint))
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
