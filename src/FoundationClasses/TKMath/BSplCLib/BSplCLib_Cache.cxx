// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <BSplCLib_Cache.hxx>
#include <BSplCLib.hxx>

#include <NCollection_LocalArray.hxx>

#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt2d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BSplCLib_Cache, Standard_Transient)

//==================================================================================================

BSplCLib_Cache::BSplCLib_Cache(
  const int&                        theDegree,
  const bool&                       thePeriodic,
  const NCollection_Array1<double>& theFlatKnots,
  const NCollection_Array1<gp_Pnt2d>& /* only used to distinguish from 3d variant */,
  const NCollection_Array1<double>* theWeights)
    : myIsRational(theWeights != nullptr),
      myParams(theDegree, thePeriodic, theFlatKnots),
      myRowLength(myIsRational ? 3 : 2)
{
}

//==================================================================================================

BSplCLib_Cache::BSplCLib_Cache(
  const int&                        theDegree,
  const bool&                       thePeriodic,
  const NCollection_Array1<double>& theFlatKnots,
  const NCollection_Array1<gp_Pnt>& /* only used to distinguish from 2d variant */,
  const NCollection_Array1<double>* theWeights)
    : myIsRational(theWeights != nullptr),
      myParams(theDegree, thePeriodic, theFlatKnots),
      myRowLength(myIsRational ? 4 : 3)
{
}

//==================================================================================================

bool BSplCLib_Cache::IsCacheValid(double theParameter) const
{
  return myParams.IsCacheValid(theParameter);
}

//==================================================================================================

void BSplCLib_Cache::BuildCache(const double&                       theParameter,
                                const NCollection_Array1<double>&   theFlatKnots,
                                const NCollection_Array1<gp_Pnt2d>& thePoles2d,
                                const NCollection_Array1<double>*   theWeights)
{
  // Normalize theParameter for periodical B-splines
  double aNewParam = myParams.PeriodicNormalization(theParameter);
  myParams.LocateParameter(aNewParam, theFlatKnots);

  // Create array wrapper referencing the stack buffer
  NCollection_Array2<double> aPolesWeights(myPolesWeightsBuffer[0],
                                           1,
                                           myParams.Degree + 1,
                                           1,
                                           myRowLength);

  // Calculate new cache data
  BSplCLib::BuildCache(myParams.SpanStart,
                       myParams.SpanLength,
                       myParams.IsPeriodic,
                       myParams.Degree,
                       myParams.SpanIndex,
                       theFlatKnots,
                       thePoles2d,
                       theWeights,
                       aPolesWeights);
}

void BSplCLib_Cache::BuildCache(const double&                     theParameter,
                                const NCollection_Array1<double>& theFlatKnots,
                                const NCollection_Array1<gp_Pnt>& thePoles,
                                const NCollection_Array1<double>* theWeights)
{
  // Create list of knots with repetitions and normalize theParameter for periodical B-splines
  double aNewParam = myParams.PeriodicNormalization(theParameter);
  myParams.LocateParameter(aNewParam, theFlatKnots);

  // Create array wrapper referencing the stack buffer
  NCollection_Array2<double> aPolesWeights(myPolesWeightsBuffer[0],
                                           1,
                                           myParams.Degree + 1,
                                           1,
                                           myRowLength);

  // Calculate new cache data
  BSplCLib::BuildCache(myParams.SpanStart,
                       myParams.SpanLength,
                       myParams.IsPeriodic,
                       myParams.Degree,
                       myParams.SpanIndex,
                       theFlatKnots,
                       thePoles,
                       theWeights,
                       aPolesWeights);
}

//==================================================================================================

void BSplCLib_Cache::calculateDerivative(double  theParameter,
                                         int     theDerivative,
                                         double* theDerivArray) const
{
  double aLocalParam = myParams.PeriodicNormalization(theParameter);
  aLocalParam        = (aLocalParam - myParams.SpanStart) / myParams.SpanLength;
  calculateDerivativeLocal(aLocalParam, theDerivative, theDerivArray);
}

//==================================================================================================

void BSplCLib_Cache::calculateDerivativeLocal(double  theLocalParam,
                                              int     theDerivative,
                                              double* theDerivArray) const
{
  const int aDimension = myRowLength;

  // Temporary container. The maximal size of this container is defined by:
  //    1) maximal derivative for cache evaluation, which is 3, plus one row for function values,
  //    2) and maximal dimension of the point, which is 3, plus one column for weights.
  double aTmpContainer[16];

  // When the PLib::RationalDerivative needs to be called, use temporary container
  double* aPntDeriv = myIsRational ? aTmpContainer : theDerivArray;

  // When the degree of curve is lesser than the requested derivative,
  // nullify array cells corresponding to greater derivatives
  int aDerivative = theDerivative;
  if (!myIsRational && myParams.Degree < theDerivative)
  {
    aDerivative = myParams.Degree;
    for (int ind = myParams.Degree * aDimension; ind < (theDerivative + 1) * aDimension; ind++)
    {
      aPntDeriv[ind] = 0.0;
    }
  }

  PLib::EvalPolynomial(theLocalParam,
                       aDerivative,
                       myParams.Degree,
                       aDimension,
                       myPolesWeightsBuffer[0],
                       aPntDeriv[0]);

  // Unnormalize derivatives since those are computed normalized
  // Use division by SpanLength instead of multiplication by precomputed inverse
  // for better numerical stability with very small span lengths
  double aFactor = 1.0;
  for (int deriv = 1; deriv <= aDerivative; deriv++)
  {
    aFactor /= myParams.SpanLength;
    for (int ind = 0; ind < aDimension; ind++)
    {
      aPntDeriv[aDimension * deriv + ind] *= aFactor;
    }
  }

  if (myIsRational)
  {
    PLib::RationalDerivative(aDerivative,
                             aDerivative,
                             aDimension - 1,
                             aPntDeriv[0],
                             theDerivArray[0]);
  }
}

//==================================================================================================

void BSplCLib_Cache::D0(const double& theParameter, gp_Pnt2d& thePoint) const
{
  double aLocalParam = myParams.PeriodicNormalization(theParameter);
  aLocalParam        = (aLocalParam - myParams.SpanStart) / myParams.SpanLength;
  D0Local(aLocalParam, thePoint);
}

//==================================================================================================

void BSplCLib_Cache::D0(const double& theParameter, gp_Pnt& thePoint) const
{
  double aLocalParam = myParams.PeriodicNormalization(theParameter);
  aLocalParam        = (aLocalParam - myParams.SpanStart) / myParams.SpanLength;
  D0Local(aLocalParam, thePoint);
}

//==================================================================================================

void BSplCLib_Cache::D0Local(double theLocalParam, gp_Pnt& thePoint) const
{
  // theLocalParam is already computed as (param - SpanStart) / SpanLength
  double aPoint[4];

  PLib::NoDerivativeEvalPolynomial(theLocalParam,
                                   myParams.Degree,
                                   myRowLength,
                                   myParams.Degree * myRowLength,
                                   myPolesWeightsBuffer[0],
                                   aPoint[0]);

  thePoint.SetCoord(aPoint[0], aPoint[1], aPoint[2]);
  if (myIsRational)
  {
    thePoint.ChangeCoord().Divide(aPoint[3]);
  }
}

void BSplCLib_Cache::D1(const double& theParameter, gp_Pnt2d& thePoint, gp_Vec2d& theTangent) const
{
  double aLocalParam = myParams.PeriodicNormalization(theParameter);
  aLocalParam        = (aLocalParam - myParams.SpanStart) / myParams.SpanLength;
  D1Local(aLocalParam, thePoint, theTangent);
}

void BSplCLib_Cache::D1(const double& theParameter, gp_Pnt& thePoint, gp_Vec& theTangent) const
{
  double aLocalParam = myParams.PeriodicNormalization(theParameter);
  aLocalParam        = (aLocalParam - myParams.SpanStart) / myParams.SpanLength;
  D1Local(aLocalParam, thePoint, theTangent);
}

//==================================================================================================

void BSplCLib_Cache::D1Local(double theLocalParam, gp_Pnt& thePoint, gp_Vec& theTangent) const
{
  double aDerivArray[8];
  calculateDerivativeLocal(theLocalParam, 1, aDerivArray);

  const int aDim = myIsRational ? myRowLength - 1 : myRowLength;
  thePoint.SetCoord(aDerivArray[0], aDerivArray[1], aDerivArray[2]);
  theTangent.SetCoord(aDerivArray[aDim], aDerivArray[aDim + 1], aDerivArray[aDim + 2]);
}

void BSplCLib_Cache::D2(const double& theParameter,
                        gp_Pnt2d&     thePoint,
                        gp_Vec2d&     theTangent,
                        gp_Vec2d&     theCurvature) const
{
  double aLocalParam = myParams.PeriodicNormalization(theParameter);
  aLocalParam        = (aLocalParam - myParams.SpanStart) / myParams.SpanLength;
  D2Local(aLocalParam, thePoint, theTangent, theCurvature);
}

void BSplCLib_Cache::D2(const double& theParameter,
                        gp_Pnt&       thePoint,
                        gp_Vec&       theTangent,
                        gp_Vec&       theCurvature) const
{
  double aLocalParam = myParams.PeriodicNormalization(theParameter);
  aLocalParam        = (aLocalParam - myParams.SpanStart) / myParams.SpanLength;
  D2Local(aLocalParam, thePoint, theTangent, theCurvature);
}

//==================================================================================================

void BSplCLib_Cache::D2Local(double  theLocalParam,
                             gp_Pnt& thePoint,
                             gp_Vec& theTangent,
                             gp_Vec& theCurvature) const
{
  double aDerivArray[12];
  calculateDerivativeLocal(theLocalParam, 2, aDerivArray);

  const int aDim   = myIsRational ? myRowLength - 1 : myRowLength;
  const int aShift = aDim << 1;
  thePoint.SetCoord(aDerivArray[0], aDerivArray[1], aDerivArray[2]);
  theTangent.SetCoord(aDerivArray[aDim], aDerivArray[aDim + 1], aDerivArray[aDim + 2]);
  theCurvature.SetCoord(aDerivArray[aShift], aDerivArray[aShift + 1], aDerivArray[aShift + 2]);
}

void BSplCLib_Cache::D3(const double& theParameter,
                        gp_Pnt2d&     thePoint,
                        gp_Vec2d&     theTangent,
                        gp_Vec2d&     theCurvature,
                        gp_Vec2d&     theTorsion) const
{
  double aLocalParam = myParams.PeriodicNormalization(theParameter);
  aLocalParam        = (aLocalParam - myParams.SpanStart) / myParams.SpanLength;
  D3Local(aLocalParam, thePoint, theTangent, theCurvature, theTorsion);
}

void BSplCLib_Cache::D3(const double& theParameter,
                        gp_Pnt&       thePoint,
                        gp_Vec&       theTangent,
                        gp_Vec&       theCurvature,
                        gp_Vec&       theTorsion) const
{
  double aLocalParam = myParams.PeriodicNormalization(theParameter);
  aLocalParam        = (aLocalParam - myParams.SpanStart) / myParams.SpanLength;
  D3Local(aLocalParam, thePoint, theTangent, theCurvature, theTorsion);
}

//==================================================================================================

void BSplCLib_Cache::D3Local(double  theLocalParam,
                             gp_Pnt& thePoint,
                             gp_Vec& theTangent,
                             gp_Vec& theCurvature,
                             gp_Vec& theTorsion) const
{
  double aDerivArray[16];
  calculateDerivativeLocal(theLocalParam, 3, aDerivArray);

  const int aDim    = myIsRational ? myRowLength - 1 : myRowLength;
  const int aShift2 = aDim << 1;
  const int aShift3 = aShift2 + aDim;
  thePoint.SetCoord(aDerivArray[0], aDerivArray[1], aDerivArray[2]);
  theTangent.SetCoord(aDerivArray[aDim], aDerivArray[aDim + 1], aDerivArray[aDim + 2]);
  theCurvature.SetCoord(aDerivArray[aShift2], aDerivArray[aShift2 + 1], aDerivArray[aShift2 + 2]);
  theTorsion.SetCoord(aDerivArray[aShift3], aDerivArray[aShift3 + 1], aDerivArray[aShift3 + 2]);
}

//==================================================================================================

void BSplCLib_Cache::D0Local(double theLocalParam, gp_Pnt2d& thePoint) const
{
  double aPoint[4];

  PLib::NoDerivativeEvalPolynomial(theLocalParam,
                                   myParams.Degree,
                                   myRowLength,
                                   myParams.Degree * myRowLength,
                                   myPolesWeightsBuffer[0],
                                   aPoint[0]);

  thePoint.SetCoord(aPoint[0], aPoint[1]);
  if (myIsRational)
  {
    thePoint.ChangeCoord().Divide(aPoint[2]);
  }
}

//==================================================================================================

void BSplCLib_Cache::D1Local(double theLocalParam, gp_Pnt2d& thePoint, gp_Vec2d& theTangent) const
{
  double aDerivArray[8];
  calculateDerivativeLocal(theLocalParam, 1, aDerivArray);

  const int aDim = myIsRational ? myRowLength - 1 : myRowLength;
  thePoint.SetCoord(aDerivArray[0], aDerivArray[1]);
  theTangent.SetCoord(aDerivArray[aDim], aDerivArray[aDim + 1]);
}

//==================================================================================================

void BSplCLib_Cache::D2Local(double    theLocalParam,
                             gp_Pnt2d& thePoint,
                             gp_Vec2d& theTangent,
                             gp_Vec2d& theCurvature) const
{
  double aDerivArray[12];
  calculateDerivativeLocal(theLocalParam, 2, aDerivArray);

  const int aDim   = myIsRational ? myRowLength - 1 : myRowLength;
  const int aShift = aDim << 1;
  thePoint.SetCoord(aDerivArray[0], aDerivArray[1]);
  theTangent.SetCoord(aDerivArray[aDim], aDerivArray[aDim + 1]);
  theCurvature.SetCoord(aDerivArray[aShift], aDerivArray[aShift + 1]);
}

//==================================================================================================

void BSplCLib_Cache::D3Local(double    theLocalParam,
                             gp_Pnt2d& thePoint,
                             gp_Vec2d& theTangent,
                             gp_Vec2d& theCurvature,
                             gp_Vec2d& theTorsion) const
{
  double aDerivArray[16];
  calculateDerivativeLocal(theLocalParam, 3, aDerivArray);

  const int aDim    = myIsRational ? myRowLength - 1 : myRowLength;
  const int aShift2 = aDim << 1;
  const int aShift3 = aShift2 + aDim;
  thePoint.SetCoord(aDerivArray[0], aDerivArray[1]);
  theTangent.SetCoord(aDerivArray[aDim], aDerivArray[aDim + 1]);
  theCurvature.SetCoord(aDerivArray[aShift2], aDerivArray[aShift2 + 1]);
  theTorsion.SetCoord(aDerivArray[aShift3], aDerivArray[aShift3 + 1]);
}
