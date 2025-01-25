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
  Standard_Integer aMinDegree   = Min(theDegreeU, theDegreeV);
  Standard_Integer aMaxDegree   = Max(theDegreeU, theDegreeV);
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
  Standard_Real aSpanLengthU = 0.5 * myParamsU.SpanLength;
  Standard_Real aSpanStartU  = myParamsU.SpanStart + aSpanLengthU;
  Standard_Real aSpanLengthV = 0.5 * myParamsV.SpanLength;
  Standard_Real aSpanStartV  = myParamsV.SpanStart + aSpanLengthV;

  aNewU = (aNewU - aSpanStartU) / aSpanLengthU;
  aNewV = (aNewV - aSpanStartV) / aSpanLengthV;

  Standard_Real* aPolesArray = ConvertArray(myPolesWeights);
  Standard_Real  aPoint[4];

  Standard_Integer aDimension       = myIsRational ? 4 : 3;
  Standard_Integer aCacheCols       = myPolesWeights->RowLength();
  Standard_Integer aMinMaxDegree[2] = {Min(myParamsU.Degree, myParamsV.Degree),
                                       Max(myParamsU.Degree, myParamsV.Degree)};
  Standard_Real    aParameters[2];
  if (myParamsU.Degree > myParamsV.Degree)
  {
    aParameters[0] = aNewV;
    aParameters[1] = aNewU;
  }
  else
  {
    aParameters[0] = aNewU;
    aParameters[1] = aNewV;
  }

  // clang-format off
  NCollection_LocalArray<Standard_Real> aTransientCoeffs(aCacheCols); // array for intermediate results
  // clang-format on

  // Calculate intermediate value of cached polynomial along columns
  PLib::NoDerivativeEvalPolynomial(aParameters[1],
                                   aMinMaxDegree[1],
                                   aCacheCols,
                                   aMinMaxDegree[1] * aCacheCols,
                                   aPolesArray[0],
                                   aTransientCoeffs[0]);

  // Calculate total value
  PLib::NoDerivativeEvalPolynomial(aParameters[0],
                                   aMinMaxDegree[0],
                                   aDimension,
                                   aDimension * aMinMaxDegree[0],
                                   aTransientCoeffs[0],
                                   aPoint[0]);

  thePoint.SetCoord(aPoint[0], aPoint[1], aPoint[2]);
  if (myIsRational)
    thePoint.ChangeCoord().Divide(aPoint[3]);
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
  Standard_Real aSpanLengthU = 0.5 * myParamsU.SpanLength;
  Standard_Real aSpanStartU  = myParamsU.SpanStart + aSpanLengthU;
  Standard_Real aSpanLengthV = 0.5 * myParamsV.SpanLength;
  Standard_Real aSpanStartV  = myParamsV.SpanStart + aSpanLengthV;

  Standard_Real anInvU = 1.0 / aSpanLengthU;
  Standard_Real anInvV = 1.0 / aSpanLengthV;
  aNewU                = (aNewU - aSpanStartU) * anInvU;
  aNewV                = (aNewV - aSpanStartV) * anInvV;

  Standard_Real* aPolesArray = ConvertArray(myPolesWeights);
  Standard_Real  aPntDeriv[16]; // result storage (point and derivative coordinates)
  for (Standard_Integer i = 0; i < 16; i++)
    aPntDeriv[i] = 0.0;

  Standard_Integer aDimension       = myIsRational ? 4 : 3;
  Standard_Integer aCacheCols       = myPolesWeights->RowLength();
  Standard_Integer aMinMaxDegree[2] = {Min(myParamsU.Degree, myParamsV.Degree),
                                       Max(myParamsU.Degree, myParamsV.Degree)};

  Standard_Real aParameters[2];
  if (myParamsU.Degree > myParamsV.Degree)
  {
    aParameters[0] = aNewV;
    aParameters[1] = aNewU;
  }
  else
  {
    aParameters[0] = aNewU;
    aParameters[1] = aNewV;
  }

  // clang-format off
  NCollection_LocalArray<Standard_Real> aTransientCoeffs(aCacheCols<<1); // array for intermediate results
  // clang-format on

  // Calculate intermediate values and derivatives of bivariate polynomial along variable with
  // maximal degree
  PLib::EvalPolynomial(aParameters[1],
                       1,
                       aMinMaxDegree[1],
                       aCacheCols,
                       aPolesArray[0],
                       aTransientCoeffs[0]);

  // Calculate a point on surface and a derivative along variable with minimal degree
  PLib::EvalPolynomial(aParameters[0],
                       1,
                       aMinMaxDegree[0],
                       aDimension,
                       aTransientCoeffs[0],
                       aPntDeriv[0]);

  // Calculate derivative along variable with maximal degree
  PLib::NoDerivativeEvalPolynomial(aParameters[0],
                                   aMinMaxDegree[0],
                                   aDimension,
                                   aMinMaxDegree[0] * aDimension,
                                   aTransientCoeffs[aCacheCols],
                                   aPntDeriv[aDimension << 1]);

  Standard_Real* aResult = aPntDeriv;
  Standard_Real  aTempStorage[12];
  if (myIsRational) // calculate derivatives divided by weight's derivatives
  {
    BSplSLib::RationalDerivative(1, 1, 1, 1, aPntDeriv[0], aTempStorage[0]);
    aResult = aTempStorage;
    aDimension--;
  }

  thePoint.SetCoord(aResult[0], aResult[1], aResult[2]);
  if (myParamsU.Degree > myParamsV.Degree)
  {
    theTangentV.SetCoord(aResult[aDimension], aResult[aDimension + 1], aResult[aDimension + 2]);
    Standard_Integer aShift = aDimension << 1;
    theTangentU.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
  }
  else
  {
    theTangentU.SetCoord(aResult[aDimension], aResult[aDimension + 1], aResult[aDimension + 2]);
    Standard_Integer aShift = aDimension << 1;
    theTangentV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
  }
  theTangentU.Multiply(anInvU);
  theTangentV.Multiply(anInvV);
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
  Standard_Real aSpanLengthU = 0.5 * myParamsU.SpanLength;
  Standard_Real aSpanStartU  = myParamsU.SpanStart + aSpanLengthU;
  Standard_Real aSpanLengthV = 0.5 * myParamsV.SpanLength;
  Standard_Real aSpanStartV  = myParamsV.SpanStart + aSpanLengthV;

  Standard_Real anInvU = 1.0 / aSpanLengthU;
  Standard_Real anInvV = 1.0 / aSpanLengthV;
  aNewU                = (aNewU - aSpanStartU) * anInvU;
  aNewV                = (aNewV - aSpanStartV) * anInvV;

  Standard_Real* aPolesArray = ConvertArray(myPolesWeights);
  Standard_Real  aPntDeriv[36]; // result storage (point and derivative coordinates)
  for (Standard_Integer i = 0; i < 36; i++)
    aPntDeriv[i] = 0.0;

  Standard_Integer aDimension       = myIsRational ? 4 : 3;
  Standard_Integer aCacheCols       = myPolesWeights->RowLength();
  Standard_Integer aMinMaxDegree[2] = {Min(myParamsU.Degree, myParamsV.Degree),
                                       Max(myParamsU.Degree, myParamsV.Degree)};

  Standard_Real aParameters[2];
  if (myParamsU.Degree > myParamsV.Degree)
  {
    aParameters[0] = aNewV;
    aParameters[1] = aNewU;
  }
  else
  {
    aParameters[0] = aNewU;
    aParameters[1] = aNewV;
  }

  // clang-format off
  NCollection_LocalArray<Standard_Real> aTransientCoeffs(3 * aCacheCols); // array for intermediate results
  // Calculating derivative to be evaluate and
  // nulling transient coefficients when max or min derivative is less than 2
  // clang-format on
  Standard_Integer aMinMaxDeriv[2] = {Min(2, aMinMaxDegree[0]), Min(2, aMinMaxDegree[1])};
  for (Standard_Integer i = aMinMaxDeriv[1] + 1; i < 3; i++)
  {
    Standard_Integer index = i * aCacheCols;
    for (Standard_Integer j = 0; j < aCacheCols; j++)
      aTransientCoeffs[index++] = 0.0;
  }

  // Calculate intermediate values and derivatives of bivariate polynomial along variable with
  // maximal degree
  PLib::EvalPolynomial(aParameters[1],
                       aMinMaxDeriv[1],
                       aMinMaxDegree[1],
                       aCacheCols,
                       aPolesArray[0],
                       aTransientCoeffs[0]);

  // Calculate a point on surface and a derivatives along variable with minimal degree
  PLib::EvalPolynomial(aParameters[0],
                       aMinMaxDeriv[0],
                       aMinMaxDegree[0],
                       aDimension,
                       aTransientCoeffs[0],
                       aPntDeriv[0]);

  // Calculate derivative along variable with maximal degree and mixed derivative
  PLib::EvalPolynomial(aParameters[0],
                       1,
                       aMinMaxDegree[0],
                       aDimension,
                       aTransientCoeffs[aCacheCols],
                       aPntDeriv[3 * aDimension]);

  // Calculate second derivative along variable with maximal degree
  PLib::NoDerivativeEvalPolynomial(aParameters[0],
                                   aMinMaxDegree[0],
                                   aDimension,
                                   aMinMaxDegree[0] * aDimension,
                                   aTransientCoeffs[aCacheCols << 1],
                                   aPntDeriv[6 * aDimension]);

  Standard_Real* aResult = aPntDeriv;
  Standard_Real  aTempStorage[36];
  if (myIsRational) // calculate derivatives divided by weight's derivatives
  {
    BSplSLib::RationalDerivative(2, 2, 2, 2, aPntDeriv[0], aTempStorage[0]);
    aResult = aTempStorage;
    aDimension--;
  }

  thePoint.SetCoord(aResult[0], aResult[1], aResult[2]);
  if (myParamsU.Degree > myParamsV.Degree)
  {
    theTangentV.SetCoord(aResult[aDimension], aResult[aDimension + 1], aResult[aDimension + 2]);
    Standard_Integer aShift = aDimension << 1;
    theCurvatureV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += aDimension;
    theTangentU.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += aDimension;
    theCurvatureUV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += (aDimension << 1);
    theCurvatureU.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
  }
  else
  {
    theTangentU.SetCoord(aResult[aDimension], aResult[aDimension + 1], aResult[aDimension + 2]);
    Standard_Integer aShift = aDimension << 1;
    theCurvatureU.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += aDimension;
    theTangentV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += aDimension;
    theCurvatureUV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
    aShift += (aDimension << 1);
    theCurvatureV.SetCoord(aResult[aShift], aResult[aShift + 1], aResult[aShift + 2]);
  }
  theTangentU.Multiply(anInvU);
  theTangentV.Multiply(anInvV);
  theCurvatureU.Multiply(anInvU * anInvU);
  theCurvatureV.Multiply(anInvV * anInvV);
  theCurvatureUV.Multiply(anInvU * anInvV);
}
