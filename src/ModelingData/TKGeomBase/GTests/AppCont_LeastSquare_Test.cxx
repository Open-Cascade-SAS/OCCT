// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.

#include <AppCont_Function.hxx>
#include <AppCont_LeastSquare.hxx>
#include <AppParCurves_Constraint.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <gtest/gtest.h>

#include <algorithm>

namespace
{
class AppContTest_Polynomial : public AppCont_Function
{
public:
  explicit AppContTest_Polynomial(double theOffset);
  double FirstParameter() const override;
  double LastParameter() const override;
  bool   Value(double                        theU,
               NCollection_Array1<gp_Pnt2d>& thePoints2d,
               NCollection_Array1<gp_Pnt>&   thePoints) const override;
  bool   D1(double                        theU,
            NCollection_Array1<gp_Vec2d>& theVectors2d,
            NCollection_Array1<gp_Vec>&   theVectors) const override;

private:
  double myOffset; //!< Common coordinate added to the polynomial.
};

//==================================================================================================

AppContTest_Polynomial::AppContTest_Polynomial(double theOffset)
    : myOffset(theOffset)
{
  myNbPnt   = 1;
  myNbPnt2d = 1;
}

//==================================================================================================

double AppContTest_Polynomial::FirstParameter() const
{
  return 0.0;
}

//==================================================================================================

double AppContTest_Polynomial::LastParameter() const
{
  return 1.0;
}

//==================================================================================================

bool AppContTest_Polynomial::Value(double                        theU,
                                   NCollection_Array1<gp_Pnt2d>& thePoints2d,
                                   NCollection_Array1<gp_Pnt>&   thePoints) const
{
  thePoints.ChangeAt(0) =
    gp_Pnt(myOffset + theU, myOffset + theU * theU, myOffset + theU * theU * theU);
  thePoints2d.ChangeAt(0) = gp_Pnt2d(myOffset + theU * theU, myOffset - theU);
  return true;
}

//==================================================================================================

bool AppContTest_Polynomial::D1(double                        theU,
                                NCollection_Array1<gp_Vec2d>& theVectors2d,
                                NCollection_Array1<gp_Vec>&   theVectors) const
{
  theVectors.ChangeAt(0)   = gp_Vec(1.0, 2.0 * theU, 3.0 * theU * theU);
  theVectors2d.ChangeAt(0) = gp_Vec2d(2.0 * theU, -1.0);
  return true;
}
} // namespace

//==================================================================================================

TEST(AppContLeastSquareTest, TranslatedPolynomialPreservesAccuracy)
{
  NCollection_Array1<gp_Pnt>   aPoles(1, 9);
  NCollection_Array1<gp_Pnt2d> aPoles2d(1, 9);
  // Points use Cartesian distances; retain the existing accuracy near the origin.
  constexpr double aTolerance = Precision::Confusion();
  for (const double anOffset : {0.0, 1.e6, -1.e6})
  {
    SCOPED_TRACE(anOffset);
    const AppContTest_Polynomial aFunction(anOffset);
    for (const AppParCurves_Constraint aFirst :
         {AppParCurves_NoConstraint, AppParCurves_PassPoint, AppParCurves_TangencyPoint})
    {
      for (const AppParCurves_Constraint aLast :
           {AppParCurves_NoConstraint, AppParCurves_PassPoint, AppParCurves_TangencyPoint})
      {
        if (aFirst == AppParCurves_NoConstraint && aLast == AppParCurves_NoConstraint)
        {
          continue;
        }
        SCOPED_TRACE(aFirst);
        SCOPED_TRACE(aLast);
        AppCont_LeastSquare aFit(aFunction, 0.0, 1.0, aFirst, aLast, 8, 17);
        ASSERT_TRUE(aFit.IsDone());
        const AppParCurves_MultiCurve& aResult = aFit.Value();
        aResult.Curve(1, aPoles);
        aResult.Curve(2, aPoles2d);
        const Geom_BezierCurve   aCurve(aPoles);
        const Geom2d_BezierCurve aCurve2d(aPoles2d);
        double                   aMaximum = 0.0;
        for (int aSample = 0; aSample <= 100; ++aSample)
        {
          const double   aU = aSample / 100.0;
          const gp_Pnt   aPoint(anOffset + aU, anOffset + aU * aU, anOffset + aU * aU * aU);
          const gp_Pnt2d aPoint2d(anOffset + aU * aU, anOffset - aU);
          aMaximum = std::max(
            {aMaximum, aCurve.EvalD0(aU).Distance(aPoint), aCurve2d.EvalD0(aU).Distance(aPoint2d)});
        }
        EXPECT_LE(aMaximum, aTolerance);
        double anError, anError3d, anError2d;
        aFit.Error(anError, anError3d, anError2d);
        EXPECT_LE(anError3d, aTolerance);
        EXPECT_LE(anError2d, aTolerance);
      }
    }
  }
}
