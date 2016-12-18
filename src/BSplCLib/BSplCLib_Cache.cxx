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

#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>


IMPLEMENT_STANDARD_RTTIEXT(BSplCLib_Cache,Standard_Transient)

//! Converts handle of array of Standard_Real into the pointer to Standard_Real
static Standard_Real* ConvertArray(const Handle(TColStd_HArray2OfReal)& theHArray)
{
  const TColStd_Array2OfReal& anArray = theHArray->Array2();
  return (Standard_Real*) &(anArray(anArray.LowerRow(), anArray.LowerCol()));
}


BSplCLib_Cache::BSplCLib_Cache()
{
  myPolesWeights.Nullify();
  myIsRational = Standard_False;
  mySpanStart = 0.0;
  mySpanLength = 0.0;
  mySpanIndex = 0;
  myDegree = 0;
  myFlatKnots.Nullify();
}

BSplCLib_Cache::BSplCLib_Cache(const Standard_Integer&        theDegree,
                               const Standard_Boolean&        thePeriodic,
                               const TColStd_Array1OfReal&    theFlatKnots,
                               const TColgp_Array1OfPnt2d&    thePoles2d,
                               const TColStd_Array1OfReal*    theWeights)
{
  Standard_Real aCacheParam = theFlatKnots.Value(theFlatKnots.Lower() + theDegree);
  BuildCache(aCacheParam, theDegree, thePeriodic, 
             theFlatKnots, thePoles2d, theWeights);
}

BSplCLib_Cache::BSplCLib_Cache(const Standard_Integer&        theDegree,
                               const Standard_Boolean&        thePeriodic,
                               const TColStd_Array1OfReal&    theFlatKnots,
                               const TColgp_Array1OfPnt&      thePoles,
                               const TColStd_Array1OfReal*    theWeights)
{
  Standard_Real aCacheParam = theFlatKnots.Value(theFlatKnots.Lower() + theDegree);
  BuildCache(aCacheParam, theDegree, thePeriodic, 
             theFlatKnots, thePoles, theWeights);
}


Standard_Boolean BSplCLib_Cache::IsCacheValid(Standard_Real theParameter) const
{
  Standard_Real aNewParam = theParameter;
  if (!myFlatKnots.IsNull())
    PeriodicNormalization(myFlatKnots->Array1(), aNewParam);

  Standard_Real aDelta = aNewParam - mySpanStart;
  return ((aDelta >= 0.0 || mySpanIndex == mySpanIndexMin) &&
          (aDelta < mySpanLength || mySpanIndex == mySpanIndexMax));
}

void BSplCLib_Cache::PeriodicNormalization(const TColStd_Array1OfReal& theFlatKnots, 
                                           Standard_Real& theParameter) const
{
  Standard_Real aPeriod = theFlatKnots.Value(theFlatKnots.Upper() - myDegree) - 
                          theFlatKnots.Value(myDegree + 1) ;
  if (theParameter < theFlatKnots.Value(myDegree + 1))
  {
    Standard_Real aScale = IntegerPart(
        (theFlatKnots.Value(myDegree + 1) - theParameter) / aPeriod);
    theParameter += aPeriod * (aScale + 1.0);
  }
  if (theParameter > theFlatKnots.Value(theFlatKnots.Upper() - myDegree))
  {
    Standard_Real aScale = IntegerPart(
        (theParameter - theFlatKnots.Value(theFlatKnots.Upper() - myDegree)) / aPeriod);
    theParameter -= aPeriod * (aScale + 1.0);
  }
}


void BSplCLib_Cache::BuildCache(const Standard_Real&           theParameter,
                                const Standard_Integer&        theDegree,
                                const Standard_Boolean&        thePeriodic,
                                const TColStd_Array1OfReal&    theFlatKnots,
                                const TColgp_Array1OfPnt2d&    thePoles2d,
                                const TColStd_Array1OfReal*    theWeights)
{
  // Normalize theParameter for periodical B-splines
  Standard_Real aNewParam = theParameter;
  if (thePeriodic)
  {
    PeriodicNormalization(theFlatKnots, aNewParam);
    myFlatKnots = new TColStd_HArray1OfReal(1, theFlatKnots.Length());
    myFlatKnots->ChangeArray1() = theFlatKnots;
  }
  else if (!myFlatKnots.IsNull()) // Periodical curve became non-periodical
    myFlatKnots.Nullify();

  // Change the size of cached data if needed
  myIsRational = (theWeights != NULL);
  Standard_Integer aPWColNumber = myIsRational ? 3 : 2;
  if (theDegree > myDegree)
    myPolesWeights = new TColStd_HArray2OfReal(1, theDegree + 1, 1, aPWColNumber);

  myDegree = theDegree;
  mySpanIndex = 0;
  BSplCLib::LocateParameter(theDegree, theFlatKnots, BSplCLib::NoMults(), 
                            aNewParam, thePeriodic, mySpanIndex, aNewParam);
  mySpanStart  = theFlatKnots.Value(mySpanIndex);
  mySpanLength = theFlatKnots.Value(mySpanIndex + 1) - mySpanStart;
  mySpanIndexMin = thePeriodic ? 0 : myDegree + 1;
  mySpanIndexMax = theFlatKnots.Length() - 1 - theDegree;

  // Calculate new cache data
  BSplCLib::BuildCache(mySpanStart, mySpanLength, thePeriodic, theDegree, 
                       mySpanIndex, theFlatKnots, thePoles2d, theWeights, 
                       myPolesWeights->ChangeArray2());
}

void BSplCLib_Cache::BuildCache(const Standard_Real&           theParameter,
                                const Standard_Integer&        theDegree,
                                const Standard_Boolean&        thePeriodic,
                                const TColStd_Array1OfReal&    theFlatKnots,
                                const TColgp_Array1OfPnt&      thePoles,
                                const TColStd_Array1OfReal*    theWeights)
{
  // Create list of knots with repetitions and normalize theParameter for periodical B-splines
  Standard_Real aNewParam = theParameter;
  if (thePeriodic)
  {
    PeriodicNormalization(theFlatKnots, aNewParam);
    myFlatKnots = new TColStd_HArray1OfReal(1, theFlatKnots.Length());
    myFlatKnots->ChangeArray1() = theFlatKnots;
  }
  else if (!myFlatKnots.IsNull()) // Periodical curve became non-periodical
    myFlatKnots.Nullify();

  // Change the size of cached data if needed
  myIsRational = (theWeights != NULL);
  Standard_Integer aPWColNumber = myIsRational ? 4 : 3;
  if (theDegree > myDegree)
    myPolesWeights = new TColStd_HArray2OfReal(1, theDegree + 1, 1, aPWColNumber);

  myDegree = theDegree;
  mySpanIndex = 0;
  BSplCLib::LocateParameter(theDegree, theFlatKnots, BSplCLib::NoMults(), 
                            aNewParam, thePeriodic, mySpanIndex, aNewParam);
  mySpanStart  = theFlatKnots.Value(mySpanIndex);
  mySpanLength = theFlatKnots.Value(mySpanIndex + 1) - mySpanStart;
  mySpanIndexMin = thePeriodic ? 0 : myDegree + 1;
  mySpanIndexMax = theFlatKnots.Length() - 1 - theDegree;

  // Calculate new cache data
  BSplCLib::BuildCache(mySpanStart, mySpanLength, thePeriodic, theDegree, 
                       mySpanIndex, theFlatKnots, thePoles, theWeights, 
                       myPolesWeights->ChangeArray2());
}


void BSplCLib_Cache::CalculateDerivative(const Standard_Real&    theParameter, 
                                         const Standard_Integer& theDerivative, 
                                               Standard_Real&    theDerivArray) const
{
  Standard_Real aNewParameter = theParameter;
  if (!myFlatKnots.IsNull()) // B-spline is periodical
    PeriodicNormalization(myFlatKnots->Array1(), aNewParameter);
  aNewParameter = (aNewParameter - mySpanStart) / mySpanLength;

  Standard_Real* aPolesArray = ConvertArray(myPolesWeights);
  Standard_Integer aDimension = myPolesWeights->RowLength(); // number of columns

  // Temporary container. The maximal size of this container is defined by:
  //    1) maximal derivative for cache evaluation, which is 3, plus one row for function values, 
  //    2) and maximal dimension of the point, which is 3, plus one column for weights.
  Standard_Real aTmpContainer[16];

  // When the PLib::RationaDerivative needs to be called, use temporary container
  Standard_Real* aPntDeriv = myIsRational ? aTmpContainer : &theDerivArray;

  // When the degree of curve is lesser than the requested derivative,
  // nullify array cells corresponding to greater derivatives
  Standard_Integer aDerivative = theDerivative;
  if (myDegree < theDerivative)
  {
    aDerivative = myDegree;
    for (Standard_Integer ind = myDegree * aDimension; ind < (theDerivative + 1) * aDimension; ind++)
    {
      aPntDeriv[ind] = 0.0;
      (&theDerivArray)[ind] = 0.0; // should be cleared separately, because aPntDeriv may look to another memory area
    }
  }

  PLib::EvalPolynomial(aNewParameter, aDerivative, myDegree, aDimension, 
                       aPolesArray[0], aPntDeriv[0]);
  // Unnormalize derivatives since those are computed normalized
  Standard_Real aFactor = 1.0;
  for (Standard_Integer deriv = 1; deriv <= aDerivative; deriv++)
  {
    aFactor /= mySpanLength;
    for (Standard_Integer ind = 0; ind < aDimension; ind++)
      aPntDeriv[aDimension * deriv + ind] *= aFactor;
  }

  if (myIsRational) // calculate derivatives divided by weights derivatives
    PLib::RationalDerivative(aDerivative, aDerivative, aDimension - 1, aPntDeriv[0], theDerivArray);
}


void BSplCLib_Cache::D0(const Standard_Real& theParameter, gp_Pnt2d& thePoint) const
{
  Standard_Real aNewParameter = theParameter;
  if (!myFlatKnots.IsNull()) // B-spline is periodical
    PeriodicNormalization(myFlatKnots->Array1(), aNewParameter);
  aNewParameter = (aNewParameter - mySpanStart) / mySpanLength;

  Standard_Real* aPolesArray = ConvertArray(myPolesWeights);
  Standard_Real aPoint[4];
  Standard_Integer aDimension = myPolesWeights->RowLength(); // number of columns

  PLib::NoDerivativeEvalPolynomial(aNewParameter, myDegree,
                                   aDimension, myDegree * aDimension,
                                   aPolesArray[0], aPoint[0]);

  thePoint.SetCoord(aPoint[0], aPoint[1]);
  if (myIsRational)
    thePoint.ChangeCoord().Divide(aPoint[2]);
}

void BSplCLib_Cache::D0(const Standard_Real& theParameter, gp_Pnt& thePoint) const
{
  Standard_Real aNewParameter = theParameter;
  if (!myFlatKnots.IsNull()) // B-spline is periodical
    PeriodicNormalization(myFlatKnots->Array1(), aNewParameter);
  aNewParameter = (aNewParameter - mySpanStart) / mySpanLength;

  Standard_Real* aPolesArray = ConvertArray(myPolesWeights);
  Standard_Real aPoint[4];
  Standard_Integer aDimension = myPolesWeights->RowLength(); // number of columns

  PLib::NoDerivativeEvalPolynomial(aNewParameter, myDegree,
                                   aDimension, myDegree * aDimension,
                                   aPolesArray[0], aPoint[0]);

  thePoint.SetCoord(aPoint[0], aPoint[1], aPoint[2]);
  if (myIsRational)
    thePoint.ChangeCoord().Divide(aPoint[3]);
}


void BSplCLib_Cache::D1(const Standard_Real& theParameter, gp_Pnt2d& thePoint, gp_Vec2d& theTangent) const
{
  Standard_Integer aDimension = myPolesWeights->RowLength(); // number of columns
  Standard_Real aPntDeriv[8]; // result storage (point and derivative coordinates)

  this->CalculateDerivative(theParameter, 1, aPntDeriv[0]);
  if (myIsRational) // the size of aPntDeriv was changed by PLib::RationalDerivative
    aDimension -= 1;

  thePoint.SetCoord(aPntDeriv[0], aPntDeriv[1]);
  theTangent.SetCoord(aPntDeriv[aDimension], aPntDeriv[aDimension + 1]);
}

void BSplCLib_Cache::D1(const Standard_Real& theParameter, gp_Pnt& thePoint, gp_Vec& theTangent) const
{
  Standard_Integer aDimension = myPolesWeights->RowLength(); // number of columns
  Standard_Real aPntDeriv[8]; // result storage (point and derivative coordinates)

  this->CalculateDerivative(theParameter, 1, aPntDeriv[0]);
  if (myIsRational) // the size of aPntDeriv was changed by PLib::RationalDerivative
    aDimension -= 1;

  thePoint.SetCoord(aPntDeriv[0], aPntDeriv[1], aPntDeriv[2]);
  theTangent.SetCoord(aPntDeriv[aDimension], aPntDeriv[aDimension + 1], aPntDeriv[aDimension + 2]);
}

void BSplCLib_Cache::D2(const Standard_Real& theParameter, gp_Pnt2d& thePoint, gp_Vec2d& theTangent, gp_Vec2d& theCurvature) const
{
  Standard_Integer aDimension = myPolesWeights->RowLength(); // number of columns
  Standard_Real aPntDeriv[12]; // result storage (point and derivatives coordinates)

  this->CalculateDerivative(theParameter, 2, aPntDeriv[0]);
  if (myIsRational) // the size of aPntDeriv was changed by PLib::RationalDerivative
    aDimension -= 1;

  thePoint.SetCoord(aPntDeriv[0], aPntDeriv[1]);
  theTangent.SetCoord(aPntDeriv[aDimension], aPntDeriv[aDimension + 1]);
  theCurvature.SetCoord(aPntDeriv[aDimension<<1], aPntDeriv[(aDimension<<1) + 1]);
}

void BSplCLib_Cache::D2(const Standard_Real& theParameter, gp_Pnt& thePoint, gp_Vec& theTangent, gp_Vec& theCurvature) const
{
  Standard_Integer aDimension = myPolesWeights->RowLength(); // number of columns
  Standard_Real aPntDeriv[12]; // result storage (point and derivatives coordinates)

  this->CalculateDerivative(theParameter, 2, aPntDeriv[0]);
  if (myIsRational) // the size of aPntDeriv was changed by PLib::RationalDerivative
    aDimension -= 1;

  thePoint.SetCoord(aPntDeriv[0], aPntDeriv[1], aPntDeriv[2]);
  theTangent.SetCoord(aPntDeriv[aDimension], aPntDeriv[aDimension + 1], aPntDeriv[aDimension + 2]);
  theCurvature.SetCoord(aPntDeriv[aDimension<<1], aPntDeriv[(aDimension<<1) + 1], aPntDeriv[(aDimension<<1) + 2]);
}


void BSplCLib_Cache::D3(const Standard_Real& theParameter, 
                              gp_Pnt2d&      thePoint, 
                              gp_Vec2d&      theTangent, 
                              gp_Vec2d&      theCurvature,
                              gp_Vec2d&      theTorsion) const
{
  Standard_Integer aDimension = myPolesWeights->RowLength(); // number of columns
  Standard_Real aPntDeriv[16]; // result storage (point and derivatives coordinates)

  this->CalculateDerivative(theParameter, 3, aPntDeriv[0]);
  if (myIsRational) // the size of aPntDeriv was changed by PLib::RationalDerivative
    aDimension -= 1;

  thePoint.SetCoord(aPntDeriv[0], aPntDeriv[1]);
  theTangent.SetCoord(aPntDeriv[aDimension], aPntDeriv[aDimension + 1]);
  Standard_Integer aShift = aDimension << 1;
  theCurvature.SetCoord(aPntDeriv[aShift], aPntDeriv[aShift + 1]);
  aShift += aDimension;
  theTorsion.SetCoord(aPntDeriv[aShift], aPntDeriv[aShift + 1]);
}

void BSplCLib_Cache::D3(const Standard_Real& theParameter, 
                              gp_Pnt&        thePoint, 
                              gp_Vec&        theTangent, 
                              gp_Vec&        theCurvature,
                              gp_Vec&        theTorsion) const
{
  Standard_Integer aDimension = myPolesWeights->RowLength(); // number of columns
  Standard_Real aPntDeriv[16]; // result storage (point and derivatives coordinates)

  this->CalculateDerivative(theParameter, 3, aPntDeriv[0]);
  if (myIsRational) // the size of aPntDeriv was changed by PLib::RationalDerivative
    aDimension -= 1;

  thePoint.SetCoord(aPntDeriv[0], aPntDeriv[1], aPntDeriv[2]);
  theTangent.SetCoord(aPntDeriv[aDimension], aPntDeriv[aDimension + 1], aPntDeriv[aDimension + 2]);
  Standard_Integer aShift = aDimension << 1;
  theCurvature.SetCoord(aPntDeriv[aShift], aPntDeriv[aShift + 1], aPntDeriv[aShift + 2]);
  aShift += aDimension;
  theTorsion.SetCoord(aPntDeriv[aShift], aPntDeriv[aShift + 1], aPntDeriv[aShift + 2]);
}

