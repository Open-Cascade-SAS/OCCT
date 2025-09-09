// Created on: 2005-12-15
// Created by: Julia GERASIMOVA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <math_EigenValuesSearcher.hxx>

#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

namespace
{
// Maximum number of QR iterations before convergence failure
const Standard_Integer MAX_ITERATIONS = 30;

// Computes sqrt(x*x + y*y) avoiding overflow and underflow
Standard_Real computeHypotenuseLength(const Standard_Real theX, const Standard_Real theY)
{
  return Sqrt(theX * theX + theY * theY);
}

// Shifts subdiagonal elements for QL algorithm initialization
void shiftSubdiagonalElements(NCollection_Array1<Standard_Real>& theSubdiagWork,
                              const Standard_Integer             theSize)
{
  for (Standard_Integer anIdx = 2; anIdx <= theSize; anIdx++)
    theSubdiagWork(anIdx - 1) = theSubdiagWork(anIdx);
  theSubdiagWork(theSize) = 0.0;
}

// Finds the end of the current unreduced submatrix using deflation
Standard_Integer findSubmatrixEnd(const NCollection_Array1<Standard_Real>& theDiagWork,
                                  const NCollection_Array1<Standard_Real>& theSubdiagWork,
                                  const Standard_Integer                   theStart,
                                  const Standard_Integer                   theSize)
{
  Standard_Integer aSubmatrixEnd;
  for (aSubmatrixEnd = theStart; aSubmatrixEnd <= theSize - 1; aSubmatrixEnd++)
  {
    const Standard_Real aDiagSum =
      Abs(theDiagWork(aSubmatrixEnd)) + Abs(theDiagWork(aSubmatrixEnd + 1));
    if (Abs(theSubdiagWork(aSubmatrixEnd)) + aDiagSum == aDiagSum)
      break;
  }
  return aSubmatrixEnd;
}

// Computes Wilkinson's shift for accelerated convergence
Standard_Real computeWilkinsonShift(const NCollection_Array1<Standard_Real>& theDiagWork,
                                    const NCollection_Array1<Standard_Real>& theSubdiagWork,
                                    const Standard_Integer                   theStart,
                                    const Standard_Integer                   theEnd)
{
  Standard_Real aShift =
    (theDiagWork(theStart + 1) - theDiagWork(theStart)) / (2.0 * theSubdiagWork(theStart));
  const Standard_Real aRadius = computeHypotenuseLength(1.0, aShift);

  if (aShift < 0.0)
    aShift =
      theDiagWork(theEnd) - theDiagWork(theStart) + theSubdiagWork(theStart) / (aShift - aRadius);
  else
    aShift =
      theDiagWork(theEnd) - theDiagWork(theStart) + theSubdiagWork(theStart) / (aShift + aRadius);

  return aShift;
}

// Performs a single QL step with implicit shift
Standard_Boolean performQLStep(NCollection_Array1<Standard_Real>& theDiagWork,
                               NCollection_Array1<Standard_Real>& theSubdiagWork,
                               NCollection_Array2<Standard_Real>& theEigenVecWork,
                               const Standard_Integer             theStart,
                               const Standard_Integer             theEnd,
                               const Standard_Real                theShift,
                               const Standard_Integer             theSize)
{
  Standard_Real aSine     = 1.0;
  Standard_Real aCosine   = 1.0;
  Standard_Real aPrevDiag = 0.0;
  Standard_Real aShift    = theShift;
  Standard_Real aRadius   = 0.0;

  Standard_Integer aRowIdx;
  for (aRowIdx = theEnd - 1; aRowIdx >= theStart; aRowIdx--)
  {
    const Standard_Real aTempVal     = aSine * theSubdiagWork(aRowIdx);
    const Standard_Real aSubdiagTemp = aCosine * theSubdiagWork(aRowIdx);
    aRadius                          = computeHypotenuseLength(aTempVal, aShift);
    theSubdiagWork(aRowIdx + 1)      = aRadius;

    if (aRadius == 0.0)
    {
      theDiagWork(aRowIdx + 1) -= aPrevDiag;
      theSubdiagWork(theEnd) = 0.0;
      break;
    }

    aSine   = aTempVal / aRadius;
    aCosine = aShift / aRadius;
    aShift  = theDiagWork(aRowIdx + 1) - aPrevDiag;
    const Standard_Real aRadiusTemp =
      (theDiagWork(aRowIdx) - aShift) * aSine + 2.0 * aCosine * aSubdiagTemp;
    aPrevDiag                = aSine * aRadiusTemp;
    theDiagWork(aRowIdx + 1) = aShift + aPrevDiag;
    aShift                   = aCosine * aRadiusTemp - aSubdiagTemp;

    // Update eigenvector matrix
    for (Standard_Integer aVecIdx = 1; aVecIdx <= theSize; aVecIdx++)
    {
      const Standard_Real aTempVec = theEigenVecWork(aVecIdx, aRowIdx + 1);
      theEigenVecWork(aVecIdx, aRowIdx + 1) =
        aSine * theEigenVecWork(aVecIdx, aRowIdx) + aCosine * aTempVec;
      theEigenVecWork(aVecIdx, aRowIdx) =
        aCosine * theEigenVecWork(aVecIdx, aRowIdx) - aSine * aTempVec;
    }
  }

  // Handle special case and update final elements
  if (aRadius == 0.0 && aRowIdx >= 1)
    return Standard_True;

  theDiagWork(theStart) -= aPrevDiag;
  theSubdiagWork(theStart) = aShift;
  theSubdiagWork(theEnd)   = 0.0;

  return Standard_True;
}

// Performs the complete QL algorithm iteration
Standard_Boolean performQLAlgorithm(NCollection_Array1<Standard_Real>& theDiagWork,
                                    NCollection_Array1<Standard_Real>& theSubdiagWork,
                                    NCollection_Array2<Standard_Real>& theEigenVecWork,
                                    const Standard_Integer             theSize)
{
  for (Standard_Integer aSubmatrixStart = 1; aSubmatrixStart <= theSize; aSubmatrixStart++)
  {
    Standard_Integer aIterCount = 0;
    Standard_Integer aSubmatrixEnd;

    do
    {
      aSubmatrixEnd = findSubmatrixEnd(theDiagWork, theSubdiagWork, aSubmatrixStart, theSize);

      if (aSubmatrixEnd != aSubmatrixStart)
      {
        if (aIterCount++ == MAX_ITERATIONS)
          return Standard_False;

        const Standard_Real aShift =
          computeWilkinsonShift(theDiagWork, theSubdiagWork, aSubmatrixStart, aSubmatrixEnd);

        if (!performQLStep(theDiagWork,
                           theSubdiagWork,
                           theEigenVecWork,
                           aSubmatrixStart,
                           aSubmatrixEnd,
                           aShift,
                           theSize))
          return Standard_False;
      }
    } while (aSubmatrixEnd != aSubmatrixStart);
  }

  return Standard_True;
}

} // anonymous namespace

//=======================================================================

math_EigenValuesSearcher::math_EigenValuesSearcher(const TColStd_Array1OfReal& theDiagonal,
                                                   const TColStd_Array1OfReal& theSubdiagonal)
{
  myIsDone = Standard_False;

  const Standard_Integer aN = theDiagonal.Length();
  if (theSubdiagonal.Length() != aN)
  {
    return;
  }

  // Initialize internal arrays using OCCT handle-based memory management
  myDiagonal                    = new TColStd_HArray1OfReal(1, aN);
  myDiagonal->ChangeArray1()    = theDiagonal;
  mySubdiagonal                 = new TColStd_HArray1OfReal(1, aN);
  mySubdiagonal->ChangeArray1() = theSubdiagonal;
  myN                           = aN;
  myEigenValues                 = new TColStd_HArray1OfReal(1, aN);
  myEigenVectors                = new TColStd_HArray2OfReal(1, aN, 1, aN);

  // Allocate working arrays using OCCT collections
  NCollection_Array1<Standard_Real> aDiagWork(1, aN);
  NCollection_Array1<Standard_Real> aSubdiagWork(1, aN);
  NCollection_Array2<Standard_Real> aEigenVecWork(1, aN, 1, aN);

  // Initialize working arrays with input data
  for (Standard_Integer anIdx = 1; anIdx <= aN; anIdx++)
    aDiagWork(anIdx) = myDiagonal->Value(anIdx);
  for (Standard_Integer anIdx = 2; anIdx <= aN; anIdx++)
    aSubdiagWork(anIdx) = mySubdiagonal->Value(anIdx);

  // Initialize eigenvector matrix as identity matrix
  for (Standard_Integer aRowIdx = 1; aRowIdx <= aN; aRowIdx++)
    for (Standard_Integer aColIdx = 1; aColIdx <= aN; aColIdx++)
      aEigenVecWork(aRowIdx, aColIdx) = (aRowIdx == aColIdx) ? 1.0 : 0.0;

  Standard_Boolean isConverged = Standard_True;

  if (aN != 1)
  {
    shiftSubdiagonalElements(aSubdiagWork, aN);
    isConverged = performQLAlgorithm(aDiagWork, aSubdiagWork, aEigenVecWork, aN);
  }

  // Copy results to class members if computation was successful
  if (isConverged)
  {
    for (Standard_Integer anIdx = 1; anIdx <= aN; anIdx++)
      myEigenValues->ChangeValue(anIdx) = aDiagWork(anIdx);
    for (Standard_Integer aRowIdx = 1; aRowIdx <= aN; aRowIdx++)
      for (Standard_Integer aColIdx = 1; aColIdx <= aN; aColIdx++)
        myEigenVectors->ChangeValue(aRowIdx, aColIdx) = aEigenVecWork(aRowIdx, aColIdx);
  }

  myIsDone = isConverged;
}

//=======================================================================

Standard_Boolean math_EigenValuesSearcher::IsDone() const
{
  return myIsDone;
}

//=======================================================================

Standard_Integer math_EigenValuesSearcher::Dimension() const
{
  return myN;
}

//=======================================================================

Standard_Real math_EigenValuesSearcher::EigenValue(const Standard_Integer theIndex) const
{
  return myEigenValues->Value(theIndex);
}

//=======================================================================

math_Vector math_EigenValuesSearcher::EigenVector(const Standard_Integer theIndex) const
{
  math_Vector aVector(1, myN);

  for (Standard_Integer anIdx = 1; anIdx <= myN; anIdx++)
    aVector(anIdx) = myEigenVectors->Value(anIdx, theIndex);

  return aVector;
}