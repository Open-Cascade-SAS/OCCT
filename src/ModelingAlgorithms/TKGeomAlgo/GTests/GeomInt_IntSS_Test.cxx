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

#include <GeomAdaptor_Curve.hxx>
#include <GeomConvert.hxx>
#include <GeomInt_IntSS.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Precision.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>
#include <array>
#include <cmath>
#include <utility>

TEST(GeomInt_IntSSTest, OffsetCylinderPlaneAtRoundedBoundary)
{
  const std::array<std::pair<double, double>, 2> aBounds = {{{20.0, 50.0}, {1.e6, 1.e6 + 30.0}}};
  for (const auto& [aFirstV, aLastV] : aBounds)
  {
    for (const double aRadius : {10.0, std::nextafter(10.0, 0.0)})
    {
      const occ::handle<Geom_BSplineSurface> aCylinder =
        GeomConvert::SurfaceToBSplineSurface(new Geom_RectangularTrimmedSurface(
          new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(1.e-10, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                                      aRadius),
          0.0,
          2.0 * M_PI,
          0.0,
          aLastV));
      const occ::handle<Geom_Surface> anOffset = new Geom_OffsetSurface(
        new Geom_RectangularTrimmedSurface(aCylinder, 0.0, 2.0 * M_PI, aFirstV, aLastV),
        -1.0);
      for (const double aBoundary : {aFirstV, aLastV})
      {
        for (const double aHeight :
             {std::nextafter(aBoundary, 0.0), aBoundary, std::nextafter(aBoundary, 2.0 * aLastV)})
        {
          SCOPED_TRACE(aFirstV);
          SCOPED_TRACE(aRadius);
          SCOPED_TRACE(aHeight);
          const occ::handle<Geom_Surface> aPlane =
            GeomConvert::SurfaceToBSplineSurface(new Geom_RectangularTrimmedSurface(
              new Geom_Plane(gp_Ax3(gp_Pnt(-20.0, -20.0, aHeight), gp_Dir(0.0, 0.0, 1.0))),
              0.0,
              40.0,
              0.0,
              40.0));

          GeomInt_IntSS anIntersection(anOffset, aPlane, Precision::Confusion());
          ASSERT_TRUE(anIntersection.IsDone());
          ASSERT_GT(anIntersection.NbLines(), 0);
          // Include the marching tolerance and the error of the fitted curve.
          const double aTolerance  = Precision::Confusion() + anIntersection.TolReached3d();
          double       aTotalAngle = 0.0;
          for (int anIndex = 1; anIndex <= anIntersection.NbLines(); ++anIndex)
          {
            const GeomAdaptor_Curve aCurve(anIntersection.Line(anIndex));
            double                  aPreviousAngle = 0.0;
            double                  anAngle        = 0.0;
            for (int aSample = 0; aSample <= 32; ++aSample)
            {
              const double aParameter =
                aCurve.FirstParameter()
                + (aCurve.LastParameter() - aCurve.FirstParameter()) * aSample / 32.0;
              const gp_Pnt aPoint = aCurve.EvalD0(aParameter);
              EXPECT_NEAR(std::hypot(aPoint.X() - 1.e-10, aPoint.Y()), aRadius - 1.0, aTolerance);
              EXPECT_NEAR(aPoint.Z(), aHeight, aTolerance);
              const double aCurrentAngle = std::atan2(aPoint.Y(), aPoint.X() - 1.e-10);
              if (aSample != 0)
              {
                anAngle += std::remainder(aCurrentAngle - aPreviousAngle, 2.0 * M_PI);
              }
              aPreviousAngle = aCurrentAngle;
            }
            aTotalAngle += std::abs(anAngle);
          }
          // The circular intersection covers one turn without gaps or overlaps.
          const double anAngleTolerance =
            2.0 * anIntersection.NbLines() * aTolerance / (aRadius - 1.0);
          EXPECT_NEAR(aTotalAngle, 2.0 * M_PI, anAngleTolerance);
        }
      }
    }
  }
}
