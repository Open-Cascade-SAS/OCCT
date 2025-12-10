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

#include <BSplSLib_Cache.hxx>
#include <BSplSLib.hxx>

#include <NCollection_LocalArray.hxx>

#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray2OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BSplSLib_Cache, Standard_Transient)

//! Converts handle of array of Standard_Real into the pointer to Standard_Real
static Standard_Real* ConvertArray(const Handle(TColStd_HArray2OfReal)& theHArray)
{
  const TColStd_Array2OfReal& anArray = theHArray->Array2();
  return (Standard_Real*)&(anArray(anArray.LowerRow(), anArray.LowerCol()));
}

//! Evaluates the polynomials and their derivatives.
//! @param[in] thePolesWeights handle to the array of poles and weights
//! @param[in] theParamsU parameters for U direction
//! @param[in] theParamsV parameters for V direction
//! @param[in] theIsRational flag indicating if the surface is rational
//! @param[in] theLocalU local U parameter
//! @param[in] theLocalV local V parameter
//! @param[in] theUDerivMax maximum U derivative
//! @param[in] theVDerivMax maximum V derivative
//! @param[out] theResultArray array to store the results
static void EvaluatePolynomials(const Handle(TColStd_HArray2OfReal)& thePolesWeights,
                                const BSplCLib_CacheParams&          theParamsU,
                                const BSplCLib_CacheParams&          theParamsV,
                                const Standard_Boolean               theIsRational,
                                double                               theLocalU,
                                double                               theLocalV,
                                int                                  theUDerivMax,
                                int                                  theVDerivMax,
                                Standard_Real*                       theResultArray)
{
  Standard_Real* const   aPolesArray  = ConvertArray(thePolesWeights);
  const Standard_Integer aDimension   = theIsRational ? 4 : 3;
  const Standard_Integer aCacheCols   = thePolesWeights->RowLength();
  const auto [aMinDegree, aMaxDegree] = std::minmax(theParamsU.Degree, theParamsV.Degree);
  const auto [aMinParam, aMaxParam]   = (theParamsU.Degree > theParamsV.Degree)
                                          ? std::make_pair(theLocalV, theLocalU)
                                          : std::make_pair(theLocalU, theLocalV);

  // Determine actual max derivatives to calculate based on degrees
  const auto [aMinDeriv, aMaxDeriv] = std::minmax(theUDerivMax, theVDerivMax);

  // Transient coefficients array size:
  // (UDerivMax + 1) * CacheCols  for the first evaluation (along max degree parameter)
  // (VDerivMax + 1) * Dimension for the second evaluation (along min degree parameter)
  NCollection_LocalArray<Standard_Real> aTransientCoeffs(
    std::max((aMaxDeriv + 1) * aCacheCols, (aMinDeriv + 1) * aDimension));

  // Calculate intermediate values and derivatives of bivariate polynomial along variable with
  // maximal degree
  PLib::EvalPolynomial(aMaxParam,
                       aMaxDeriv,
                       aMaxDegree,
                       aCacheCols,
                       aPolesArray[0],
                       aTransientCoeffs[0]);

  // Calculate derivatives along variable with minimal degree
  PLib::EvalPolynomial(aMinParam,
                       aMinDeriv,
                       aMinDegree,
                       aDimension,
                       aTransientCoeffs[0],
                       theResultArray[0]);

  if (theUDerivMax > 0 || theVDerivMax > 0)
  {
    // If theMaxUDeriv is 1, and theMinUDeriv is 0 for example, but still need to calculate
    // The second part of cross derivative.
    const int anUDeriv = (theParamsU.Degree > theParamsV.Degree) ? theUDerivMax : theVDerivMax;

    // Calculate derivative along variable with maximal degree and mixed derivative
    PLib::EvalPolynomial(aMinParam,
                         anUDeriv,
                         aMinDegree,
                         aDimension,
                         aTransientCoeffs[aCacheCols],
                         theResultArray[3 * aDimension]);

    if (theUDerivMax > 1 || theVDerivMax > 1)
    {
      // Calculate second derivative along variable with maximal degree
      PLib::NoDerivativeEvalPolynomial(aMinParam,
                                       aMinDegree,
                                       aDimension,
                                       aMinDegree * aDimension,
                                       aTransientCoeffs[aCacheCols << 1],
                                       theResultArray[6 * aDimension]);
    }
  }

  if (theIsRational)
  {
    BSplSLib::RationalDerivative(theUDerivMax,
                                 theVDerivMax,
                                 theUDerivMax,
                                 theVDerivMax,
                                 theResultArray[0],
                                 theResultArray[0]); // Overwrite in place
  }
}

BSplSLib_Cache::BSplSLib_Cache(const Standard_Integer&     theDegreeU,
                               const Standard_Boolean&     thePeriodicU,
                               const TColStd_Array1OfReal& theFlatKnotsU,
                               const Standard_Integer&     theDegreeV,
                               const Standard_Boolean&     thePeriodicV,
                               const TColStd_Array1OfReal& theFlatKnotsV,
                               const TColStd_Array2OfReal* theWeights)
    : myIsRational(theWeights != NULL),
      myParamsU(theDegreeU, thePeriodicU, theFlatKnotsU),
      myParamsV(theDegreeV, thePeriodicV, theFlatKnotsV)
{
  Standard_Integer aMinDegree   = std::min(theDegreeU, theDegreeV);
  Standard_Integer aMaxDegree   = std::max(theDegreeU, theDegreeV);
  Standard_Integer aPWColNumber = (myIsRational ? 4 : 3);
  myPolesWeights = new TColStd_HArray2OfReal(1, aMaxDegree + 1, 1, aPWColNumber * (aMinDegree + 1));
}

Standard_Boolean BSplSLib_Cache::IsCacheValid(Standard_Real theParameterU,
                                              Standard_Real theParameterV) const
{
  return myParamsU.IsCacheValid(theParameterU) && myParamsV.IsCacheValid(theParameterV);
}

void BSplSLib_Cache::BuildCache(const Standard_Real&        theParameterU,
                                const Standard_Real&        theParameterV,
                                const TColStd_Array1OfReal& theFlatKnotsU,
                                const TColStd_Array1OfReal& theFlatKnotsV,
                                const TColgp_Array2OfPnt&   thePoles,
                                const TColStd_Array2OfReal* theWeights)
{
  // Normalize the parameters for periodical B-splines
  Standard_Real aNewParamU = myParamsU.PeriodicNormalization(theParameterU);
  Standard_Real aNewParamV = myParamsV.PeriodicNormalization(theParameterV);

  myParamsU.LocateParameter(aNewParamU, theFlatKnotsU);
  myParamsV.LocateParameter(aNewParamV, theFlatKnotsV);

  // BSplSLib uses different convention for span parameters than BSplCLib
  // (Start is in the middle of the span and length is half-span),
  // thus we need to amend them here
  Standard_Real aSpanLengthU = 0.5 * myParamsU.SpanLength;
  Standard_Real aSpanStartU  = myParamsU.SpanStart + aSpanLengthU;
  Standard_Real aSpanLengthV = 0.5 * myParamsV.SpanLength;
  Standard_Real aSpanStartV  = myParamsV.SpanStart + aSpanLengthV;

  // Calculate new cache data
  BSplSLib::BuildCache(aSpanStartU,
                       aSpanStartV,
                       aSpanLengthU,
                       aSpanLengthV,
                       myParamsU.IsPeriodic,
                       myParamsV.IsPeriodic,
                       myParamsU.Degree,
                       myParamsV.Degree,
                       myParamsU.SpanIndex,
                       myParamsV.SpanIndex,
                       theFlatKnotsU,
                       theFlatKnotsV,
                       thePoles,
                       theWeights,
                       myPolesWeights->ChangeArray2());
}

void BSplSLib_Cache::D0(const Standard_Real& theU,
                        const Standard_Real& theV,
                        gp_Pnt&              thePoint) const
{
  Standard_Real aNewU = myParamsU.PeriodicNormalization(theU);
  Standard_Real aNewV = myParamsV.PeriodicNormalization(theV);

  // BSplSLib uses different convention for span parameters than BSplCLib
  // (Start is in the middle of the span and length is half-span),
  // thus we need to amend them here
  const Standard_Real aSpanLengthU = 0.5 * myParamsU.SpanLength;
  const Standard_Real aSpanStartU  = myParamsU.SpanStart + aSpanLengthU;
  const Standard_Real aSpanLengthV = 0.5 * myParamsV.SpanLength;
  const Standard_Real aSpanStartV  = myParamsV.SpanStart + aSpanLengthV;

  const Standard_Real aLocalU = (aNewU - aSpanStartU) / aSpanLengthU;
  const Standard_Real aLocalV = (aNewV - aSpanStartV) / aSpanLengthV;

  D0Local(aLocalU, aLocalV, thePoint);
}

void BSplSLib_Cache::D0Local(double theLocalU, double theLocalV, gp_Pnt& thePoint) const
{
  Standard_Real aPntDeriv[4]; // Result storage for D0
  EvaluatePolynomials(myPolesWeights,
                      myParamsU,
                      myParamsV,
                      myIsRational,
                      theLocalU,
                      theLocalV,
                      0,
                      0,
                      aPntDeriv);

  thePoint.SetCoord(aPntDeriv[0], aPntDeriv[1], aPntDeriv[2]);
  if (myIsRational)
  {
    thePoint.ChangeCoord().Divide(aPntDeriv[3]);
  }
}

//==================================================================================================

void BSplSLib_Cache::D1Local(double  theLocalU,
                             double  theLocalV,
                             gp_Pnt& thePoint,
                             gp_Vec& theTangentU,
                             gp_Vec& theTangentV) const
{
  Standard_Real aPntDeriv[16]; // Result storage for D1
  EvaluatePolynomials(myPolesWeights,
                      myParamsU,
                      myParamsV,
                      myIsRational,
                      theLocalU,
                      theLocalV,
                      1,
                      1,
                      aPntDeriv);

  const Standard_Integer aDimension = myIsRational ? 4 : 3;
  const Standard_Integer aShift     = aDimension; // Shift for first derivatives

  thePoint.SetCoord(aPntDeriv[0], aPntDeriv[1], aPntDeriv[2]);

  // Tangents are stored after the point coordinates
  // Order: D0, DU, DV (if U max degree, else D0, DV, DU)
  if (myParamsU.Degree > myParamsV.Degree)
  {
    theTangentV.SetCoord(aPntDeriv[aShift], aPntDeriv[aShift + 1], aPntDeriv[aShift + 2]);
    theTangentU.SetCoord(aPntDeriv[aShift + aDimension],
                         aPntDeriv[aShift + aDimension + 1],
                         aPntDeriv[aShift + aDimension + 2]);
  }
  else
  {
    theTangentU.SetCoord(aPntDeriv[aShift], aPntDeriv[aShift + 1], aPntDeriv[aShift + 2]);
    theTangentV.SetCoord(aPntDeriv[aShift + aDimension],
                         aPntDeriv[aShift + aDimension + 1],
                         aPntDeriv[aShift + aDimension + 2]);
  }

  const Standard_Real anInvU = 2.0 * myParamsU.InvSpanLength;
  const Standard_Real anInvV = 2.0 * myParamsV.InvSpanLength;
  theTangentU.Multiply(anInvU);
  theTangentV.Multiply(anInvV);
}

//==================================================================================================

void BSplSLib_Cache::D2Local(double  theLocalU,
                             double  theLocalV,
                             gp_Pnt& thePoint,
                             gp_Vec& theTangentU,
                             gp_Vec& theTangentV,
                             gp_Vec& theCurvatureU,
                             gp_Vec& theCurvatureV,
                             gp_Vec& theCurvatureUV) const
{
  Standard_Real aPntDeriv[36]; // Result storage for D2
  EvaluatePolynomials(myPolesWeights,
                      myParamsU,
                      myParamsV,
                      myIsRational,
                      theLocalU,
                      theLocalV,
                      2,
                      2,
                      aPntDeriv);

  const Standard_Integer aDimension = myIsRational ? 4 : 3;
  const Standard_Integer aShift     = aDimension; // Shift for first derivatives
  const Standard_Integer aShift2    = aDimension << 1;
  const Standard_Integer aShift3    = aShift2 + aDimension;
  const Standard_Integer aShift4    = aShift3 + aDimension;
  const Standard_Integer aShift5    = aShift4 + aDimension;

  thePoint.SetCoord(aPntDeriv[0], aPntDeriv[1], aPntDeriv[2]);

  // Derivatives are stored consecutively: D0, DU, DV, DUU, DVV, DUV
  if (myParamsU.Degree > myParamsV.Degree)
  {
    theTangentV.SetCoord(aPntDeriv[aShift], aPntDeriv[aShift + 1], aPntDeriv[aShift + 2]);
    theCurvatureV.SetCoord(aPntDeriv[aShift2], aPntDeriv[aShift2 + 1], aPntDeriv[aShift2 + 2]);
    theTangentU.SetCoord(aPntDeriv[aShift3], aPntDeriv[aShift3 + 1], aPntDeriv[aShift3 + 2]);
    theCurvatureUV.SetCoord(aPntDeriv[aShift4], aPntDeriv[aShift4 + 1], aPntDeriv[aShift4 + 2]);
    theCurvatureU.SetCoord(aPntDeriv[aShift5], aPntDeriv[aShift5 + 1], aPntDeriv[aShift5 + 2]);
  }
  else
  {
    theTangentU.SetCoord(aPntDeriv[aShift], aPntDeriv[aShift + 1], aPntDeriv[aShift + 2]);
    theCurvatureU.SetCoord(aPntDeriv[aShift2], aPntDeriv[aShift2 + 1], aPntDeriv[aShift2 + 2]);
    theTangentV.SetCoord(aPntDeriv[aShift3], aPntDeriv[aShift3 + 1], aPntDeriv[aShift3 + 2]);
    theCurvatureUV.SetCoord(aPntDeriv[aShift4], aPntDeriv[aShift4 + 1], aPntDeriv[aShift4 + 2]);
    theCurvatureV.SetCoord(aPntDeriv[aShift5], aPntDeriv[aShift5 + 1], aPntDeriv[aShift5 + 2]);
  }

  const Standard_Real anInvU = 2.0 * myParamsU.InvSpanLength;
  const Standard_Real anInvV = 2.0 * myParamsV.InvSpanLength;
  theTangentU.Multiply(anInvU);
  theTangentV.Multiply(anInvV);
  theCurvatureU.Multiply(anInvU * anInvU);
  theCurvatureV.Multiply(anInvV * anInvV);
  theCurvatureUV.Multiply(anInvU * anInvV);
}

void BSplSLib_Cache::D1(const Standard_Real& theU,
                        const Standard_Real& theV,
                        gp_Pnt&              thePoint,
                        gp_Vec&              theTangentU,
                        gp_Vec&              theTangentV) const
{
  Standard_Real aNewU = myParamsU.PeriodicNormalization(theU);
  Standard_Real aNewV = myParamsV.PeriodicNormalization(theV);

  // BSplSLib uses different convention for span parameters than BSplCLib
  // (Start is in the middle of the span and length is half-span),
  // thus we need to amend them here
  const Standard_Real aSpanLengthU = 0.5 * myParamsU.SpanLength;
  const Standard_Real aSpanStartU  = myParamsU.SpanStart + aSpanLengthU;
  const Standard_Real aSpanLengthV = 0.5 * myParamsV.SpanLength;
  const Standard_Real aSpanStartV  = myParamsV.SpanStart + aSpanLengthV;

  const Standard_Real aLocalU = (aNewU - aSpanStartU) / aSpanLengthU;
  const Standard_Real aLocalV = (aNewV - aSpanStartV) / aSpanLengthV;

  D1Local(aLocalU, aLocalV, thePoint, theTangentU, theTangentV);
}

void BSplSLib_Cache::D2(const Standard_Real& theU,
                        const Standard_Real& theV,
                        gp_Pnt&              thePoint,
                        gp_Vec&              theTangentU,
                        gp_Vec&              theTangentV,
                        gp_Vec&              theCurvatureU,
                        gp_Vec&              theCurvatureV,
                        gp_Vec&              theCurvatureUV) const
{
  Standard_Real aNewU = myParamsU.PeriodicNormalization(theU);
  Standard_Real aNewV = myParamsV.PeriodicNormalization(theV);

  // BSplSLib uses different convention for span parameters than BSplCLib
  // (Start is in the middle of the span and length is half-span),
  // thus we need to amend them here
  const Standard_Real aSpanLengthU = 0.5 * myParamsU.SpanLength;
  const Standard_Real aSpanStartU  = myParamsU.SpanStart + aSpanLengthU;
  const Standard_Real aSpanLengthV = 0.5 * myParamsV.SpanLength;
  const Standard_Real aSpanStartV  = myParamsV.SpanStart + aSpanLengthV;

  const Standard_Real aLocalU = (aNewU - aSpanStartU) / aSpanLengthU;
  const Standard_Real aLocalV = (aNewV - aSpanStartV) / aSpanLengthV;

  D2Local(aLocalU,
          aLocalV,
          thePoint,
          theTangentU,
          theTangentV,
          theCurvatureU,
          theCurvatureV,
          theCurvatureUV);
}
