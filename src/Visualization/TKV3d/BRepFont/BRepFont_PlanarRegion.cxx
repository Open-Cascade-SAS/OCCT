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

#include <BRepFont_PlanarRegion.hxx>

#include <MathPoly_Quadratic.hxx>
#include <gp_Pnt2d.hxx>

#include <algorithm>
#include <cmath>
#include <limits>

namespace
{
//! Add curve points at the roots of one coordinate derivative.
bool addCoordinateExtrema(Bnd_Box2d&                          theBounds,
                          const BRepFont_PlanarRegion::Curve& theCurve,
                          const double                        theLinear,
                          const double                        theQuadratic,
                          const double                        theCubic)
{
  const MathPoly::PolyResult aRoots =
    MathPoly::Quadratic(3.0 * theCubic, 2.0 * theQuadratic, theLinear);
  if (aRoots.Status != MathUtils::Status::OK && aRoots.Status != MathUtils::Status::NoSolution
      && aRoots.Status != MathUtils::Status::InfiniteSolutions)
  {
    return false;
  }
  for (size_t aRootIndex = 0; aRootIndex < aRoots.NbRoots; ++aRootIndex)
  {
    const double aParameter = aRoots.Roots[aRootIndex];
    if (aParameter > 0.0 && aParameter < 1.0)
    {
      theBounds.Add(gp_Pnt2d(theCurve.Value(aParameter)));
    }
  }
  return true;
}
} // namespace

BRepFont_PlanarRegion::Curve::PowerCoefficients BRepFont_PlanarRegion::Curve::powerCoefficients()
  const
{
  PowerCoefficients aResult;
  const gp_XY&      aP0 = Start();
  const gp_XY&      aP1 = Control1();
  const gp_XY&      aP2 = Control2();
  const gp_XY&      aP3 = End();
  aResult.X[0]          = aP0.X();
  aResult.Y[0]          = aP0.Y();
  switch (Type())
  {
    case Font_GlyphOutline::Segment::Kind::Line:
      aResult.Degree = 1;
      aResult.X[1]   = aP3.X() - aP0.X();
      aResult.Y[1]   = aP3.Y() - aP0.Y();
      break;
    case Font_GlyphOutline::Segment::Kind::QuadraticBezier:
      aResult.Degree = 2;
      aResult.X[1]   = 2.0 * (aP1.X() - aP0.X());
      aResult.Y[1]   = 2.0 * (aP1.Y() - aP0.Y());
      aResult.X[2]   = aP0.X() - 2.0 * aP1.X() + aP3.X();
      aResult.Y[2]   = aP0.Y() - 2.0 * aP1.Y() + aP3.Y();
      break;
    case Font_GlyphOutline::Segment::Kind::CubicBezier:
      aResult.Degree = 3;
      aResult.X[1]   = 3.0 * (aP1.X() - aP0.X());
      aResult.Y[1]   = 3.0 * (aP1.Y() - aP0.Y());
      aResult.X[2]   = 3.0 * (aP0.X() - 2.0 * aP1.X() + aP2.X());
      aResult.Y[2]   = 3.0 * (aP0.Y() - 2.0 * aP1.Y() + aP2.Y());
      aResult.X[3]   = -aP0.X() + 3.0 * aP1.X() - 3.0 * aP2.X() + aP3.X();
      aResult.Y[3]   = -aP0.Y() + 3.0 * aP1.Y() - 3.0 * aP2.Y() + aP3.Y();
      break;
  }
  return aResult;
}

//=================================================================================================

BRepFont_PlanarRegion::Curve::Curve(const Font_GlyphOutline::Segment& theSegment)
    : myKind(theSegment.Type()),
      myStart(theSegment.Start()),
      myControl1(theSegment.Control1()),
      myControl2(theSegment.Control2()),
      myEnd(theSegment.End())
{
}

//=================================================================================================

gp_XY BRepFont_PlanarRegion::Curve::Value(const double theParameter) const
{
  const double aT = theParameter;
  const double aU = 1.0 - aT;
  switch (myKind)
  {
    case Font_GlyphOutline::Segment::Kind::Line:
      return myStart * aU + myEnd * aT;
    case Font_GlyphOutline::Segment::Kind::QuadraticBezier:
      return myStart * (aU * aU) + myControl1 * (2.0 * aU * aT) + myEnd * (aT * aT);
    case Font_GlyphOutline::Segment::Kind::CubicBezier:
      return myStart * (aU * aU * aU) + myControl1 * (3.0 * aU * aU * aT)
             + myControl2 * (3.0 * aU * aT * aT) + myEnd * (aT * aT * aT);
  }
  return myStart;
}

//=================================================================================================

gp_XY BRepFont_PlanarRegion::Curve::D1(const double theParameter) const
{
  const double aT = theParameter;
  const double aU = 1.0 - aT;
  switch (myKind)
  {
    case Font_GlyphOutline::Segment::Kind::Line:
      return myEnd - myStart;
    case Font_GlyphOutline::Segment::Kind::QuadraticBezier:
      return (myControl1 - myStart) * (2.0 * aU) + (myEnd - myControl1) * (2.0 * aT);
    case Font_GlyphOutline::Segment::Kind::CubicBezier:
      return (myControl1 - myStart) * (3.0 * aU * aU) + (myControl2 - myControl1) * (6.0 * aU * aT)
             + (myEnd - myControl2) * (3.0 * aT * aT);
  }
  return gp_XY();
}

//=================================================================================================

double BRepFont_PlanarRegion::Curve::SignedArea() const
{
  const PowerCoefficients aCoefficients = powerCoefficients();
  double                  anIntegral    = 0.0;
  for (int anI = 0; anI <= aCoefficients.Degree; ++anI)
  {
    for (int aJ = 1; aJ <= aCoefficients.Degree; ++aJ)
    {
      anIntegral += (aCoefficients.X[anI] * static_cast<double>(aJ) * aCoefficients.Y[aJ]
                     - aCoefficients.Y[anI] * static_cast<double>(aJ) * aCoefficients.X[aJ])
                    / static_cast<double>(anI + aJ);
    }
  }
  return 0.5 * anIntegral;
}

//=================================================================================================

Bnd_Box2d BRepFont_PlanarRegion::Curve::Bounds() const
{
  Bnd_Box2d aBounds;
  aBounds.Add(gp_Pnt2d(myStart));
  aBounds.Add(gp_Pnt2d(myEnd));
  if (myKind == Font_GlyphOutline::Segment::Kind::Line)
  {
    return aBounds;
  }

  const PowerCoefficients aCoefficients = powerCoefficients();
  const bool              hasXExtrema   = addCoordinateExtrema(aBounds,
                                                *this,
                                                aCoefficients.X[1],
                                                aCoefficients.X[2],
                                                aCoefficients.X[3]);
  const bool              hasYExtrema   = addCoordinateExtrema(aBounds,
                                                *this,
                                                aCoefficients.Y[1],
                                                aCoefficients.Y[2],
                                                aCoefficients.Y[3]);
  if (!hasXExtrema || !hasYExtrema)
  {
    // Preserve the conservative control-hull bound if polynomial solving fails.
    aBounds.Add(gp_Pnt2d(myControl1));
    if (myKind == Font_GlyphOutline::Segment::Kind::CubicBezier)
    {
      aBounds.Add(gp_Pnt2d(myControl2));
    }
    return aBounds;
  }

  const double aCoordinateScale = std::max({1.0,
                                            std::abs(myStart.X()),
                                            std::abs(myStart.Y()),
                                            std::abs(myControl1.X()),
                                            std::abs(myControl1.Y()),
                                            std::abs(myControl2.X()),
                                            std::abs(myControl2.Y()),
                                            std::abs(myEnd.X()),
                                            std::abs(myEnd.Y())});
  aBounds.SetGap(64.0 * std::numeric_limits<double>::epsilon() * aCoordinateScale);
  return aBounds;
}
