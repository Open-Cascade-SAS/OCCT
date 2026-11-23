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

#include <Adaptor3d_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

#include <cmath>
#include <utility>

namespace
{
constexpr double THE_TOLERANCE = 1.e-10;

//==================================================================================================

occ::handle<Geom_BSplineSurface> createSurface(const double theOffset)
{
  const size_t               aNbPoles = 6;
  NCollection_Array2<gp_Pnt> aPoles(1, int(aNbPoles), 1, int(aNbPoles));
  NCollection_Array2<double> aWeights(1, int(aNbPoles), 1, int(aNbPoles));
  for (size_t aU = 0; aU < aNbPoles; ++aU)
  {
    for (size_t aV = 0; aV < aNbPoles; ++aV)
    {
      aPoles.ChangeAt(aU, aV) =
        gp_Pnt(double(aU), double(aV), theOffset + std::sin(double(aU + aV)));
      aWeights.ChangeAt(aU, aV) = 1.0 + 0.05 * double(aU + aV);
    }
  }
  const double                     aKnotData[] = {0.0, 1.0, 2.0, 3.0};
  const int                        aMultData[] = {4, 1, 1, 4};
  const NCollection_Array1<double> aKnots(aKnotData[0], 1, 4);
  const NCollection_Array1<int>    aMults(aMultData[0], 1, 4);
  return new Geom_BSplineSurface(aPoles, aWeights, aKnots, aKnots, aMults, aMults, 3, 3);
}

//==================================================================================================

void checkDerivatives(const Adaptor3d_Surface& theAdaptor, const Geom_BSplineSurface& theSurface)
{
  const std::pair<double, double> aParameters[] = {
    {0.5, 0.5},
    {1.5, 0.5},
    {0.5, 0.5},
    {2.5, 2.5},
    {1.5, 0.5},
    {2.5, 2.5},
    {1.0, 2.0},
    {std::nextafter(1.0, 0.0), std::nextafter(2.0, 3.0)},
    {std::nextafter(1.0, 2.0), std::nextafter(2.0, 1.0)}};
  for (const auto& [aU, aV] : aParameters)
  {
    const auto [aRefPoint, aRefDU, aRefDV, aRefDUU, aRefDVV, aRefDUV] = theSurface.EvalD2(aU, aV);
    EXPECT_LE(theAdaptor.EvalD0(aU, aV).Distance(aRefPoint), THE_TOLERANCE);
    const auto [aPointD1, aD1U, aD1V] = theAdaptor.EvalD1(aU, aV);
    EXPECT_LE(aPointD1.Distance(aRefPoint), THE_TOLERANCE);
    EXPECT_LE((aD1U - aRefDU).Magnitude(), THE_TOLERANCE);
    EXPECT_LE((aD1V - aRefDV).Magnitude(), THE_TOLERANCE);
    const auto [aPoint, aDU, aDV, aDUU, aDVV, aDUV] = theAdaptor.EvalD2(aU, aV);
    EXPECT_LE(aPoint.Distance(aRefPoint), THE_TOLERANCE);
    EXPECT_LE((aDU - aRefDU).Magnitude(), THE_TOLERANCE);
    EXPECT_LE((aDV - aRefDV).Magnitude(), THE_TOLERANCE);
    EXPECT_LE((aDUU - aRefDUU).Magnitude(), THE_TOLERANCE);
    EXPECT_LE((aDVV - aRefDVV).Magnitude(), THE_TOLERANCE);
    EXPECT_LE((aDUV - aRefDUV).Magnitude(), THE_TOLERANCE);
  }
}
} // namespace

//==================================================================================================

TEST(GeomAdaptor_SurfaceTest, AlternatingSpansAfterLoadAndCopy)
{
  const occ::handle<Geom_BSplineSurface> aSurface       = createSurface(0.0);
  const occ::handle<Geom_BSplineSurface> anotherSurface = createSurface(7.0);
  GeomAdaptor_Surface                    anAdaptor(aSurface);
  checkDerivatives(anAdaptor, *aSurface);

  const occ::handle<Adaptor3d_Surface> aCopy = anAdaptor.ShallowCopy();
  anAdaptor.Load(anotherSurface);
  checkDerivatives(anAdaptor, *anotherSurface);
  checkDerivatives(*aCopy, *aSurface);
  anAdaptor.Load(aSurface);
  checkDerivatives(anAdaptor, *aSurface);
}
