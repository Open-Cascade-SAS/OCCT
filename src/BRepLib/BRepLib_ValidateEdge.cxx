// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <BRepLib_ValidateEdge.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepCheck.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <GeomLib_CheckCurveOnSurface.hxx>

//=============================================================================
//function : BRepLib_ValidateEdge
//purpose  : Constructor
//=============================================================================
BRepLib_ValidateEdge::BRepLib_ValidateEdge(Handle(Adaptor3d_Curve) theReferenceCurve, 
                                           Handle(Adaptor3d_CurveOnSurface) theOtherCurve, 
                                           Standard_Boolean theSameParameter):
  myReferenceCurve(theReferenceCurve),
  myOtherCurve(theOtherCurve),
  mySameParameter(theSameParameter),
  myControlPointsNumber(22),
  myToleranceForChecking(0),
  myCalculatedDistance(0),
  myExitIfToleranceExceeded(Standard_False),
  myIsDone(Standard_False),
  myIsExactMethod(Standard_False),
  myIsMultiThread(Standard_False)
{ }

//=============================================================================
//function : CheckTolerance
//purpose  : 
//=============================================================================
Standard_Boolean BRepLib_ValidateEdge::CheckTolerance(Standard_Real theToleranceToCheck)
{
  return correctTolerance(theToleranceToCheck) > myCalculatedDistance;
}

//=============================================================================
//function : GetMaxDistance
//purpose  : 
//=============================================================================
Standard_Real BRepLib_ValidateEdge::GetMaxDistance()
{
  Standard_Real aCorrectedTolerance = myCalculatedDistance * 1.00001;
  return aCorrectedTolerance;
}

//=============================================================================
//function : UpdateTolerance
//purpose  : 
//=============================================================================
void BRepLib_ValidateEdge::UpdateTolerance(Standard_Real& theToleranceToUpdate)
{
  Standard_Real aCorrectedTolerance = myCalculatedDistance * 1.00001;
  if (aCorrectedTolerance > theToleranceToUpdate)
  {
    theToleranceToUpdate = aCorrectedTolerance;
  }
}

//=============================================================================
//function : correctTolerance
//purpose  : 
//=============================================================================
Standard_Real BRepLib_ValidateEdge::correctTolerance(Standard_Real theTolerance)
{
  const Handle(Adaptor3d_Surface)& aSurface = myOtherCurve->GetSurface();
  Standard_Real aCurvePrecision = BRepCheck::PrecCurve(*myReferenceCurve);
  Standard_Real aSurfacePrecision = BRepCheck::PrecSurface(aSurface);
  Standard_Real aToleranceDelta = (aCurvePrecision > aSurfacePrecision) ? aCurvePrecision : aSurfacePrecision;
  Standard_Real aCorrectedTolerance = theTolerance + aToleranceDelta;
  return aCorrectedTolerance;
}

//=============================================================================
//function : SetExitIfToleranceExceeded
//purpose  : 
//=============================================================================
void BRepLib_ValidateEdge::SetExitIfToleranceExceeded(Standard_Real theToleranceForChecking)
{
  myExitIfToleranceExceeded = Standard_True;
  myToleranceForChecking = correctTolerance(theToleranceForChecking);
}

//=============================================================================
//function : Process
//purpose  : 
//=============================================================================
void BRepLib_ValidateEdge::Process()
{
  if (myIsExactMethod && mySameParameter)
  {
    processExact();
  }
  else
  {
    processApprox();
  }
}

//=============================================================================
//function : processApprox
//purpose  : 
//=============================================================================
void BRepLib_ValidateEdge::processApprox()
{
  myIsDone = Standard_True;
  Standard_Real aSquareToleranceForChecking = myToleranceForChecking * myToleranceForChecking;
  Standard_Real aReferenceFirstParam = myReferenceCurve->FirstParameter();
  Standard_Real aReferenceLastParam = myReferenceCurve->LastParameter();
  Standard_Real anOtherFirstParam = myOtherCurve->FirstParameter();
  Standard_Real anOtherLastParam = myOtherCurve->LastParameter();
  Standard_Real aMaxSquareDistance = 0.;

  Standard_Integer aControlPointsNumber = (myControlPointsNumber < 1) ? 1 : myControlPointsNumber;
  Standard_Boolean anIsProjection = (!mySameParameter ||
    Abs(anOtherFirstParam - aReferenceFirstParam) > Precision::PConfusion() ||
    Abs(anOtherLastParam - aReferenceLastParam) > Precision::PConfusion());

  if (!anIsProjection)
  {
    for (Standard_Integer i = 0; i <= aControlPointsNumber; ++i)
    {
      Standard_Real aControlPointParam =
        ((aControlPointsNumber - i) * aReferenceFirstParam + i * aReferenceLastParam) / aControlPointsNumber;
      gp_Pnt aReferencePoint = myReferenceCurve->Value(aControlPointParam);
      gp_Pnt anOtherPoint = myOtherCurve->Value(aControlPointParam);
      Standard_Real aSquareDistance = aReferencePoint.SquareDistance(anOtherPoint);
      if (aSquareDistance > aMaxSquareDistance)
      {
          aMaxSquareDistance = aSquareDistance;
      }
      // Stop process for best performance
      if (myExitIfToleranceExceeded && aMaxSquareDistance > aSquareToleranceForChecking)
      {
        myCalculatedDistance = Sqrt(aMaxSquareDistance);
        return;
      }
    }
  }
  else
  {
    gp_Pnt aReferencePoint = myReferenceCurve->Value(aReferenceFirstParam);
    gp_Pnt anOtherPoint = myOtherCurve->Value(anOtherFirstParam);
    Standard_Real aSquareDistance = aReferencePoint.SquareDistance(anOtherPoint);
    if (aSquareDistance > aMaxSquareDistance)
    {
        aMaxSquareDistance = aSquareDistance;
    }
    if (myExitIfToleranceExceeded && aMaxSquareDistance > aSquareToleranceForChecking)
    {
      myCalculatedDistance = Sqrt(aMaxSquareDistance);
      return;
    }

    aReferencePoint = myReferenceCurve->Value(aReferenceLastParam);
    anOtherPoint = myOtherCurve->Value(anOtherLastParam);
    aSquareDistance = aReferencePoint.SquareDistance(anOtherPoint);
    if (aSquareDistance > aMaxSquareDistance)
    {
      aMaxSquareDistance = aSquareDistance;
    }
    if (myExitIfToleranceExceeded && aMaxSquareDistance > aSquareToleranceForChecking)
    {
      myCalculatedDistance = Sqrt(aMaxSquareDistance);
      return;
    }

    Extrema_LocateExtPC aReferenceExtrema, anOtherExtrema;
    aReferenceExtrema.Initialize(*myReferenceCurve, aReferenceFirstParam, aReferenceLastParam, myReferenceCurve->Resolution(Precision::Confusion()));
    anOtherExtrema.Initialize(*myOtherCurve, anOtherFirstParam, anOtherLastParam, myOtherCurve->Resolution(Precision::Confusion()));
    for (Standard_Integer i = 1; i < aControlPointsNumber; i++)
    {
      Standard_Real aReferenceParam = ((aControlPointsNumber - i) * aReferenceFirstParam + i * aReferenceLastParam) / aControlPointsNumber;
      gp_Pnt aReferenceExtremaPoint = myReferenceCurve->Value(aReferenceParam);
      Standard_Real anOtherParam = ((aControlPointsNumber - i) * anOtherFirstParam + i * anOtherLastParam) / aControlPointsNumber;
      gp_Pnt anOtherExtremaPoint = myOtherCurve->Value(anOtherParam);

      aReferenceExtrema.Perform(anOtherExtremaPoint, aReferenceParam);
      if (aReferenceExtrema.IsDone())
      {
        if (aReferenceExtrema.SquareDistance() > aMaxSquareDistance)
        {
          aMaxSquareDistance = aReferenceExtrema.SquareDistance();
        }
        if (myExitIfToleranceExceeded && aMaxSquareDistance > aSquareToleranceForChecking)
        {
          myCalculatedDistance = Sqrt(aMaxSquareDistance);
          return;
        }
      }
      else
      {
        myIsDone = Standard_False;
        // Stop process for best performance
        return;
      }

      anOtherExtrema.Perform(aReferenceExtremaPoint, anOtherParam);
      if (anOtherExtrema.IsDone())
      {
        if (anOtherExtrema.SquareDistance() > aMaxSquareDistance)
        {
          aMaxSquareDistance = anOtherExtrema.SquareDistance();
        }
        if (myExitIfToleranceExceeded && aMaxSquareDistance > aSquareToleranceForChecking)
        {
          myCalculatedDistance = Sqrt(aMaxSquareDistance);
          return;
        }
      }
      else
      {
        myIsDone = Standard_False;
        // Stop process for best performance
        return;
      }
    }
  }
  myCalculatedDistance = Sqrt(aMaxSquareDistance);
}

//=============================================================================
//function : processExact
//purpose  : 
//=============================================================================
void BRepLib_ValidateEdge::processExact()
{
  GeomLib_CheckCurveOnSurface aCheckCurveOnSurface(myReferenceCurve);
  aCheckCurveOnSurface.SetParallel(myIsMultiThread);
  aCheckCurveOnSurface.Perform(myOtherCurve);
  myIsDone = aCheckCurveOnSurface.IsDone();
  if (myIsDone)
  {
    myCalculatedDistance = aCheckCurveOnSurface.MaxDistance();
  }
}

