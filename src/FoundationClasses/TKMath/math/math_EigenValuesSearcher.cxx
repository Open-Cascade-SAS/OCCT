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

namespace
{
// Maximum number of QR iterations before convergence failure
const int MAX_ITERATIONS = 30;

// Computes sqrt(x*x + y*y) avoiding overflow and underflow
double computeHypotenuseLength(const double theX, const double theY)
{
  return std::sqrt(theX * theX + theY * theY);
}

// Shifts subdiagonal elements for QL algorithm initialization
void shiftSubdiagonalElements(NCollection_Array1<double>& theSubdiagWork,
                              const int             theSize)
{
  for (int anIdx = 2; anIdx <= theSize; anIdx++)
    theSubdiagWork(anIdx - 1) = theSubdiagWork(anIdx);
  theSubdiagWork(theSize) = 0.0;
}

// Finds the end of the current unreduced submatrix using deflation
int findSubmatrixEnd(const NCollection_Array1<double>& theDiagWork,
                                  const NCollection_Array1<double>& theSubdiagWork,
                                  const int                   theStart,
                                  const int                   theSize)
{
  int aSubmatrixEnd;
  for (aSubmatrixEnd = theStart; aSubmatrixEnd <= theSize - 1; aSubmatrixEnd++)
  {
    const double aDiagSum =
      std::abs(theDiagWork(aSubmatrixEnd)) + std::abs(theDiagWork(aSubmatrixEnd + 1));

    // Deflation test: Check if subdiagonal element is negligible
    // The condition |e[i]| + (|d[i]| + |d[i+1]|) == |d[i]| + |d[i+1]|
    // tests whether the subdiagonal element e[i] is smaller than machine epsilon
    // relative to the adjacent diagonal elements. This is more robust than
    // checking e[i] == 0.0 because it accounts for finite precision arithmetic.
    // When this condition is true in floating-point arithmetic, the subdiagonal
    // element can be treated as zero for convergence purposes.
    if (std::abs(theSubdiagWork(aSubmatrixEnd)) + aDiagSum == aDiagSum)
      break;
  }
  return aSubmatrixEnd;
}

// Computes Wilkinson's shift for accelerated convergence
double computeWilkinsonShift(const NCollection_Array1<double>& theDiagWork,
                                    const NCollection_Array1<double>& theSubdiagWork,
                                    const int                   theStart,
                                    const int                   theEnd)
{
  double aShift =
    (theDiagWork(theStart + 1) - theDiagWork(theStart)) / (2.0 * theSubdiagWork(theStart));
  const double aRadius = computeHypotenuseLength(1.0, aShift);

  if (aShift < 0.0)
    aShift =
      theDiagWork(theEnd) - theDiagWork(theStart) + theSubdiagWork(theStart) / (aShift - aRadius);
  else
    aShift =
      theDiagWork(theEnd) - theDiagWork(theStart) + theSubdiagWork(theStart) / (aShift + aRadius);

  return aShift;
}

// Performs a single QL step with implicit shift
bool performQLStep(NCollection_Array1<double>& theDiagWork,
                               NCollection_Array1<double>& theSubdiagWork,
                               NCollection_Array2<double>& theEigenVecWork,
                               const int             theStart,
                               const int             theEnd,
                               const double                theShift,
                               const int             theSize)
{
  double aSine     = 1.0;
  double aCosine   = 1.0;
  double aPrevDiag = 0.0;
  double aShift    = theShift;
  double aRadius   = 0.0;

  int aRowIdx;
  for (aRowIdx = theEnd - 1; aRowIdx >= theStart; aRowIdx--)
  {
    const double aTempVal     = aSine * theSubdiagWork(aRowIdx);
    const double aSubdiagTemp = aCosine * theSubdiagWork(aRowIdx);
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
    const double aRadiusTemp =
      (theDiagWork(aRowIdx) - aShift) * aSine + 2.0 * aCosine * aSubdiagTemp;
    aPrevDiag                = aSine * aRadiusTemp;
    theDiagWork(aRowIdx + 1) = aShift + aPrevDiag;
    aShift                   = aCosine * aRadiusTemp - aSubdiagTemp;

    // Update eigenvector matrix
    for (int aVecIdx = 1; aVecIdx <= theSize; aVecIdx++)
    {
      const double aTempVec = theEigenVecWork(aVecIdx, aRowIdx + 1);
      theEigenVecWork(aVecIdx, aRowIdx + 1) =
        aSine * theEigenVecWork(aVecIdx, aRowIdx) + aCosine * aTempVec;
      theEigenVecWork(aVecIdx, aRowIdx) =
        aCosine * theEigenVecWork(aVecIdx, aRowIdx) - aSine * aTempVec;
    }
  }

  // Handle special case and update final elements
  if (aRadius == 0.0 && aRowIdx >= 1)
    return true;

  theDiagWork(theStart) -= aPrevDiag;
  theSubdiagWork(theStart) = aShift;
  theSubdiagWork(theEnd)   = 0.0;

  return true;
}

// Performs the complete QL algorithm iteration
bool performQLAlgorithm(NCollection_Array1<double>& theDiagWork,
                                    NCollection_Array1<double>& theSubdiagWork,
                                    NCollection_Array2<double>& theEigenVecWork,
                                    const int             theSize)
{
  for (int aSubmatrixStart = 1; aSubmatrixStart <= theSize; aSubmatrixStart++)
  {
    int aIterCount = 0;
    int aSubmatrixEnd;

    do
    {
      aSubmatrixEnd = findSubmatrixEnd(theDiagWork, theSubdiagWork, aSubmatrixStart, theSize);

      if (aSubmatrixEnd != aSubmatrixStart)
      {
        if (aIterCount++ == MAX_ITERATIONS)
          return false;

        const double aShift =
          computeWilkinsonShift(theDiagWork, theSubdiagWork, aSubmatrixStart, aSubmatrixEnd);

        if (!performQLStep(theDiagWork,
                           theSubdiagWork,
                           theEigenVecWork,
                           aSubmatrixStart,
                           aSubmatrixEnd,
                           aShift,
                           theSize))
          return false;
      }
    } while (aSubmatrixEnd != aSubmatrixStart);
  }

  return true;
}

} // anonymous namespace

//=======================================================================

math_EigenValuesSearcher::math_EigenValuesSearcher(const NCollection_Array1<double>& theDiagonal,
                                                   const NCollection_Array1<double>& theSubdiagonal)
    : myDiagonal(theDiagonal),
      mySubdiagonal(theSubdiagonal),
      myIsDone(false),
      myN(theDiagonal.Length()),
      myEigenValues(1, theDiagonal.Length()),
      myEigenVectors(1, theDiagonal.Length(), 1, theDiagonal.Length())
{
  if (theSubdiagonal.Length() != myN)
  {
    return;
  }

  // Move lower bounds to 1 for consistent indexing
  myDiagonal.UpdateLowerBound(1);
  mySubdiagonal.UpdateLowerBound(1);

  // Use internal arrays directly as working arrays
  shiftSubdiagonalElements(mySubdiagonal, myN);

  // Initialize eigenvector matrix as identity matrix
  for (int aRowIdx = 1; aRowIdx <= myN; aRowIdx++)
    for (int aColIdx = 1; aColIdx <= myN; aColIdx++)
      myEigenVectors(aRowIdx, aColIdx) = (aRowIdx == aColIdx) ? 1.0 : 0.0;

  bool isConverged = true;

  if (myN != 1)
  {
    isConverged = performQLAlgorithm(myDiagonal, mySubdiagonal, myEigenVectors, myN);
  }

  // Store results directly in myEigenValues (myDiagonal contains eigenvalues after QL algorithm)
  if (isConverged)
  {
    for (int anIdx = 1; anIdx <= myN; anIdx++)
      myEigenValues(anIdx) = myDiagonal(anIdx);
  }

  myIsDone = isConverged;
}

//=======================================================================

bool math_EigenValuesSearcher::IsDone() const
{
  return myIsDone;
}

//=======================================================================

int math_EigenValuesSearcher::Dimension() const
{
  return myN;
}

//=======================================================================

double math_EigenValuesSearcher::EigenValue(const int theIndex) const
{
  return myEigenValues(theIndex);
}

//=======================================================================

math_Vector math_EigenValuesSearcher::EigenVector(const int theIndex) const
{
  math_Vector aVector(1, myN);

  for (int anIdx = 1; anIdx <= myN; anIdx++)
    aVector(anIdx) = myEigenVectors(anIdx, theIndex);

  return aVector;
}