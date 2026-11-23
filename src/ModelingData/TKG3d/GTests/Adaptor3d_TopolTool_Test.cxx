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
#include <Adaptor3d_TopolTool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <algorithm>

namespace
{
//! Counts point evaluations during surface sampling.
class CountingSurface : public Adaptor3d_Surface
{
public:
  explicit CountingSurface(const occ::handle<Geom_BSplineSurface>& theSurface);
  double                           FirstUParameter() const override;
  double                           LastUParameter() const override;
  double                           FirstVParameter() const override;
  double                           LastVParameter() const override;
  GeomAbs_SurfaceType              GetType() const override;
  occ::handle<Geom_BSplineSurface> BSpline() const override;
  gp_Pnt                           EvalD0(double theU, double theV) const override;
  size_t                           NbEvaluations() const;

private:
  GeomAdaptor_Surface mySurface;           //!< Surface being sampled.
  mutable size_t      myNbEvaluations = 0; //!< Number of evaluated points.
};

//==================================================================================================

CountingSurface::CountingSurface(const occ::handle<Geom_BSplineSurface>& theSurface)
    : mySurface(theSurface)
{
}

//==================================================================================================

double CountingSurface::FirstUParameter() const
{
  return mySurface.FirstUParameter();
}

//==================================================================================================

double CountingSurface::LastUParameter() const
{
  return mySurface.LastUParameter();
}

//==================================================================================================

double CountingSurface::FirstVParameter() const
{
  return mySurface.FirstVParameter();
}

//==================================================================================================

double CountingSurface::LastVParameter() const
{
  return mySurface.LastVParameter();
}

//==================================================================================================

GeomAbs_SurfaceType CountingSurface::GetType() const
{
  return mySurface.GetType();
}

//==================================================================================================

occ::handle<Geom_BSplineSurface> CountingSurface::BSpline() const
{
  return mySurface.BSpline();
}

//==================================================================================================

gp_Pnt CountingSurface::EvalD0(double theU, double theV) const
{
  ++myNbEvaluations;
  return mySurface.EvalD0(theU, theV);
}

//==================================================================================================

size_t CountingSurface::NbEvaluations() const
{
  return myNbEvaluations;
}
} // namespace

TEST(Adaptor3d_TopolToolTest, BSplineSamplingReusesCandidatePoints)
{
  const double                     aKnotValues[] = {0.0, 1.0};
  const int                        aMultValues[] = {2, 2};
  const NCollection_Array1<double> aKnots(aKnotValues[0], 1, 2);
  const NCollection_Array1<int>    aMults(aMultValues[0], 1, 2);
  NCollection_Array2<gp_Pnt>       aPoles(1, 2, 1, 2);
  NCollection_Array2<double>       aWeights(1, 2, 1, 2);
  for (size_t i = 0; i < 2; ++i)
  {
    for (size_t j = 0; j < 2; ++j)
    {
      aPoles.ChangeAt(i, j)   = gp_Pnt(static_cast<double>(i), static_cast<double>(j), 0.0);
      aWeights.ChangeAt(i, j) = static_cast<double>((i + 1) * (j + 1));
    }
  }
  const int aMinNbSamples = 4;
  for (const int aMinSamples : {2, aMinNbSamples})
  {
    for (const int aDegree : {1, 3, 12, 25})
    {
      for (const bool isAsymmetric : {false, true})
      {
        const int aVDegree =
          isAsymmetric ? Geom_BSplineSurface::MaxDegree() + 1 - aDegree : aDegree;
        const int aMinSamplesV = isAsymmetric ? aMinSamples + 3 : aMinSamples;
        for (const bool isRational : {false, true})
        {
          for (const bool isCurved : {false, true})
          {
            SCOPED_TRACE(::testing::Message()
                         << aMinSamples << ' ' << aMinSamplesV << ' ' << isCurved << ' ' << aDegree
                         << ' ' << aVDegree << ' ' << isRational);
            const occ::handle<Geom_BSplineSurface> aGeometry =
              isRational
                ? new Geom_BSplineSurface(aPoles, aWeights, aKnots, aKnots, aMults, aMults, 1, 1)
                : new Geom_BSplineSurface(aPoles, aKnots, aKnots, aMults, aMults, 1, 1);
            aGeometry->IncreaseDegree(aDegree, aVDegree);
            if (isCurved)
            {
              const int aMiddleU = (aGeometry->NbUPoles() + 1) / 2;
              const int aMiddleV = (aGeometry->NbVPoles() + 1) / 2;
              gp_Pnt    aPole    = aGeometry->Pole(aMiddleU, aMiddleV);
              aPole.SetZ(1.0);
              aGeometry->SetPole(aMiddleU, aMiddleV, aPole);
            }
            const occ::handle<CountingSurface> aSurface = new CountingSurface(aGeometry);
            Adaptor3d_TopolTool                aTool(aSurface);
            aTool.SamplePnts(Precision::Confusion(), aMinSamples, aMinSamplesV);
            if (!isAsymmetric)
            {
              const size_t aNbCandidates =
                static_cast<size_t>(std::max({aDegree + 1, aMinSamples, aMinNbSamples}));
              EXPECT_LE(aSurface->NbEvaluations(), 2 * aNbCandidates * aNbCandidates);
            }
            ASSERT_GE(aTool.NbSamplesU(), aMinNbSamples);
            ASSERT_GE(aTool.NbSamplesV(), std::max(aMinNbSamples, aMinSamplesV));
            NCollection_Array1<double> aU(1, aTool.NbSamplesU()), aV(1, aTool.NbSamplesV());
            aTool.UParameters(aU);
            aTool.VParameters(aV);
            for (const NCollection_Array1<double>* aParameters : {&aU, &aV})
            {
              EXPECT_DOUBLE_EQ(aParameters->First(), 0.0);
              EXPECT_DOUBLE_EQ(aParameters->Last(), 1.0);
              for (size_t i = 1; i < aParameters->Size(); ++i)
              {
                EXPECT_LT(aParameters->At(i - 1), aParameters->At(i));
              }
            }
          }
        }
      }
    }
  }
}
