// Created on: 2005-12-09
// Created by: Sergey KHROMOV
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


#include <BRepGProp_Domain.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepGProp_TFunction.hxx>
#include <BRepGProp_VinertGK.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <math_KronrodSingleIntegration.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_HArray1OfReal.hxx>

//==========================================================================
//function : Constructor
//==========================================================================
BRepGProp_VinertGK::BRepGProp_VinertGK(BRepGProp_Face        &theSurface,
                                       const gp_Pnt          &theLocation,
                                       const Standard_Real    theTolerance,
                                       const Standard_Boolean theCGFlag,
                                       const Standard_Boolean theIFlag):
  myErrorReached(0.)
{
  SetLocation(theLocation);
  Perform(theSurface, theTolerance, theCGFlag, theIFlag);
}

//==========================================================================
//function : Constructor
//           
//==========================================================================

BRepGProp_VinertGK::BRepGProp_VinertGK(      BRepGProp_Face          &theSurface,
                                       const gp_Pnt        &thePoint,
                                       const gp_Pnt        &theLocation,
                                       const Standard_Real  theTolerance,
                                       const Standard_Boolean theCGFlag,
                                       const Standard_Boolean theIFlag):
  myErrorReached(0.)
{
  SetLocation(theLocation);
  Perform(theSurface, thePoint, theTolerance, theCGFlag, theIFlag);
}

//==========================================================================
//function : Constructor
//           
//==========================================================================

BRepGProp_VinertGK::BRepGProp_VinertGK(BRepGProp_Face        &theSurface,
                                       BRepGProp_Domain      &theDomain,
                                       const gp_Pnt          &theLocation,
                                       const Standard_Real    theTolerance,
                                       const Standard_Boolean theCGFlag,
                                       const Standard_Boolean theIFlag):
  myErrorReached(0.)
{
  SetLocation(theLocation);
  Perform(theSurface, theDomain, theTolerance, theCGFlag, theIFlag);
}

//==========================================================================
//function : Constructor
//           
//==========================================================================

BRepGProp_VinertGK::BRepGProp_VinertGK(BRepGProp_Face        &theSurface,
                                       BRepGProp_Domain      &theDomain,
                                       const gp_Pnt          &thePoint,
                                       const gp_Pnt          &theLocation,
                                       const Standard_Real    theTolerance,
                                       const Standard_Boolean theCGFlag,
                                       const Standard_Boolean theIFlag):
  myErrorReached(0.)
{
  SetLocation(theLocation);
  Perform(theSurface, theDomain, thePoint, theTolerance, theCGFlag, theIFlag);
}

//==========================================================================
//function : Constructor
//           
//==========================================================================

BRepGProp_VinertGK::BRepGProp_VinertGK(BRepGProp_Face        &theSurface,
                                       const gp_Pln          &thePlane,
                                       const gp_Pnt          &theLocation,
                                       const Standard_Real    theTolerance,
                                       const Standard_Boolean theCGFlag,
                                       const Standard_Boolean theIFlag):
  myErrorReached(0.)
{
  SetLocation(theLocation);
  Perform(theSurface, thePlane, theTolerance, theCGFlag, theIFlag);
}

//==========================================================================
//function : Constructor
//           
//==========================================================================

BRepGProp_VinertGK::BRepGProp_VinertGK(BRepGProp_Face        &theSurface,
                                       BRepGProp_Domain      &theDomain,
                                       const gp_Pln          &thePlane,
                                       const gp_Pnt          &theLocation,
                                       const Standard_Real    theTolerance,
                                       const Standard_Boolean theCGFlag,
                                       const Standard_Boolean theIFlag):
  myErrorReached(0.)
{
  SetLocation(theLocation);
  Perform(theSurface, theDomain, thePlane, theTolerance, theCGFlag, theIFlag);
}

//==========================================================================
//function : Perform
//           Compute the properties.
//==========================================================================

Standard_Real BRepGProp_VinertGK::Perform(BRepGProp_Face        &theSurface,
                                          const Standard_Real    theTolerance,
                                          const Standard_Boolean theCGFlag,
                                          const Standard_Boolean theIFlag)

{
  Standard_Real aShift[] = { 0., 0., 0. };

  return PrivatePerform(theSurface, NULL, Standard_True, &aShift, theTolerance, 
    theCGFlag, theIFlag);
}

//==========================================================================
//function : Perform
//           Compute the properties.
//==========================================================================

Standard_Real BRepGProp_VinertGK::Perform(BRepGProp_Face        &theSurface,
                                          const gp_Pnt          &thePoint,
                                          const Standard_Real    theTolerance,
                                          const Standard_Boolean theCGFlag,
                                          const Standard_Boolean theIFlag)

{
  gp_XYZ        aXYZ(thePoint.XYZ().Subtracted(loc.XYZ()));
  Standard_Real aShift[3];

  aXYZ.Coord(aShift[0], aShift[1], aShift[2]);

  return PrivatePerform(theSurface, NULL, Standard_True, &aShift, theTolerance, 
    theCGFlag, theIFlag);
}

//==========================================================================
//function : Perform
//           Compute the properties.
//==========================================================================

Standard_Real BRepGProp_VinertGK::Perform(BRepGProp_Face        &theSurface,
                                          BRepGProp_Domain      &theDomain,
                                          const Standard_Real    theTolerance,
                                          const Standard_Boolean theCGFlag,
                                          const Standard_Boolean theIFlag)

{
  Standard_Real aShift[] = { 0., 0., 0. };

  return PrivatePerform(theSurface, &theDomain,
    Standard_True, &aShift, theTolerance, 
    theCGFlag, theIFlag);
}

//==========================================================================
//function : Perform
//           Compute the properties.
//==========================================================================

Standard_Real BRepGProp_VinertGK::Perform(BRepGProp_Face        &theSurface,
                                          BRepGProp_Domain      &theDomain,
                                          const gp_Pnt          &thePoint,
                                          const Standard_Real    theTolerance,
                                          const Standard_Boolean theCGFlag,
                                          const Standard_Boolean theIFlag)

{
  gp_XYZ        aXYZ(thePoint.XYZ().Subtracted(loc.XYZ()));
  Standard_Real aShift[3];

  aXYZ.Coord(aShift[0], aShift[1], aShift[2]);

  return PrivatePerform(theSurface, &theDomain,
    Standard_True, &aShift, theTolerance, 
    theCGFlag, theIFlag);
}

//==========================================================================
//function : Perform
//           Compute the properties.
//==========================================================================

Standard_Real BRepGProp_VinertGK::Perform(BRepGProp_Face        &theSurface,
                                          const gp_Pln          &thePlane,
                                          const Standard_Real    theTolerance,
                                          const Standard_Boolean theCGFlag,
                                          const Standard_Boolean theIFlag)

{
  Standard_Real aCoeff[4];
  Standard_Real aXLoc;
  Standard_Real aYLoc;
  Standard_Real aZLoc;

  loc.Coord(aXLoc, aYLoc, aZLoc);
  thePlane.Coefficients (aCoeff[0], aCoeff[1], aCoeff[2], aCoeff[3]);
  aCoeff[3] = aCoeff[3] - aCoeff[0]*aXLoc - aCoeff[1]*aYLoc - aCoeff[2]*aZLoc;

  return PrivatePerform(theSurface, NULL,
    Standard_False, &aCoeff, theTolerance, 
    theCGFlag, theIFlag);
}

//==========================================================================
//function : Perform
//           Compute the properties.
//==========================================================================

Standard_Real BRepGProp_VinertGK::Perform(BRepGProp_Face        &theSurface,
                                          BRepGProp_Domain      &theDomain,
                                          const gp_Pln          &thePlane,
                                          const Standard_Real    theTolerance,
                                          const Standard_Boolean theCGFlag,
                                          const Standard_Boolean theIFlag)

{
  Standard_Real aCoeff[4];
  Standard_Real aXLoc;
  Standard_Real aYLoc;
  Standard_Real aZLoc;

  loc.Coord(aXLoc, aYLoc, aZLoc);
  thePlane.Coefficients (aCoeff[0], aCoeff[1], aCoeff[2], aCoeff[3]);
  aCoeff[3] = aCoeff[3] - aCoeff[0]*aXLoc - aCoeff[1]*aYLoc - aCoeff[2]*aZLoc;

  return PrivatePerform(theSurface, &theDomain,
    Standard_False, &aCoeff, theTolerance, 
    theCGFlag, theIFlag);
}

//==========================================================================
//function : PrivatePerform
//           Compute the properties.
//==========================================================================

Standard_Real BRepGProp_VinertGK::PrivatePerform
(BRepGProp_Face        &theSurface,
 const Standard_Address thePtrDomain,
 const Standard_Boolean IsByPoint,
 const Standard_Address theCoeffs,
 const Standard_Real    theTolerance,
 const Standard_Boolean theCGFlag,
 const Standard_Boolean theIFlag)

{

  const Standard_Real aTTol = 1.e-9;
  Standard_Real *aCoeffs = (Standard_Real *)theCoeffs;

  // Compute the number of 2d bounding curves of the face.
  BRepGProp_Domain           *aPDomain = NULL;
  Standard_Integer  aNbCurves = 0;

  // If the pointer to the domain is NULL, there is only one curve to treat:
  // U isoline with the UMax parameter.
  if (thePtrDomain == NULL)
    aNbCurves = 1;
  else {
    aPDomain = (BRepGProp_Domain *)thePtrDomain;

    for (aPDomain->Init(); aPDomain->More(); aPDomain->Next())
      aNbCurves++;
  }

  if (aNbCurves == 0) {
    myErrorReached = -1.;

    return myErrorReached;
  }

  //Standard_Real    aCrvTol = 0.5*theTolerance/aNbCurves;
  Standard_Real    aCrvTol = 0.1*theTolerance;
  Standard_Real    aUMin;
  Standard_Real    aUMax;
  Standard_Real    aTMin;
  Standard_Real    aTMax;
  Standard_Integer aNbPnts;
  Standard_Integer aNbMaxIter = 1000;
  Standard_Integer aNbVal = 10;
  Standard_Integer k;
  math_Vector      aLocalValue(1, aNbVal);
  math_Vector      aLocalTolReached(1, aNbVal);
  math_Vector      aValue(1, aNbVal);
  math_Vector      aTolReached(1, aNbVal);
  TColStd_Array1OfBoolean CFlags(1, aNbVal); 
  CFlags.Init(Standard_False);
  Standard_Boolean isMore;

  //aNbVal = 1;
  aValue.Init(0.);
  aTolReached.Init(0.);

  CFlags.Init(Standard_False);
  CFlags(1) = Standard_True;

  if(theCGFlag || theIFlag) {
    Standard_Integer i;
    for(i = 2; i <= 4; ++i) {CFlags(i) = Standard_True;}
  }

  if(theIFlag) {
    Standard_Integer i;
    for(i = 5; i <= 10; ++i) {CFlags(i) = Standard_True;}
  }

  theSurface.Bounds(aUMin, aUMax, aTMin, aTMax);

  if (thePtrDomain == NULL)
    isMore = Standard_True;
  else {
    aPDomain->Init();
    isMore = aPDomain->More();
  }

  while(isMore) {
    // If the pointer to the domain is NULL, there is only one curve to treat:
    // U isoline with the UMax parameter.

    if (thePtrDomain == NULL)
      theSurface.Load(Standard_False, GeomAbs_IsoU);
    else
      theSurface.Load(aPDomain->Value());

    aTMin = theSurface.FirstParameter();
    aTMax = theSurface.LastParameter();


    // Get the spans on the curve.
    Handle(TColStd_HArray1OfReal) aTKnots;
    BRepGProp_TFunction               aTFunc(theSurface, loc, IsByPoint, theCoeffs,
      aUMin, aCrvTol);

    theSurface.GetTKnots(aTMin, aTMax, aTKnots);

    Standard_Integer              iU             = aTKnots->Upper();
    Standard_Integer              aNbTIntervals  = aTKnots->Length() - 1;
    //Standard_Real                 aTolSpan       = aCrvTol/aNbTIntervals;
    Standard_Real                 aTolSpan       = 0.9*theTolerance; //Relative error
    math_KronrodSingleIntegration anIntegral;
    GProp_ValueType               aValueType;


    // Empirical criterion.
    aNbPnts = Min(15, theSurface.IntegrationOrder()/aNbTIntervals + 1);
    aNbPnts = Max(5, aNbPnts);
    //     aNbPnts = theSurface.IntegrationOrder();

    aLocalValue.Init(0.);
    aLocalTolReached.Init(0.);

    for (k = 1; k <= aNbVal; k++) {

      if(!CFlags(k)) continue;

      Standard_Integer i = aTKnots->Lower();

      switch (k) {
      case 1:   aValueType = GProp_Mass;        break;
      case 2:   aValueType = GProp_CenterMassX; break;
      case 3:   aValueType = GProp_CenterMassY; break;
      case 4:   aValueType = GProp_CenterMassZ; break;
      case 5:   aValueType = GProp_InertiaXX;   break;
      case 6:   aValueType = GProp_InertiaYY;   break;
      case 7:   aValueType = GProp_InertiaZZ;   break;
      case 8:   aValueType = GProp_InertiaXY;   break;
      case 9:   aValueType = GProp_InertiaXZ;   break;
      case 10:  aValueType = GProp_InertiaYZ;   break;

      default: myErrorReached = -1.; return myErrorReached;
      }
      aTFunc.SetValueType(aValueType);

      Standard_Real err1 = 0.;
      while (i < iU) {

        //cout << "-------------- Span " << i << " nbp: " << aNbPnts << endl;
        Standard_Real aT1 = aTKnots->Value(i++);
        Standard_Real aT2 = aTKnots->Value(i);

        if(aT2 - aT1 < aTTol) continue;

        aTFunc.SetNbKronrodPoints(aNbPnts);
        aTFunc.Init();
        aTFunc.SetTolerance(aCrvTol/(aT2-aT1));
        anIntegral.Perform(aTFunc, aT1, aT2, aNbPnts, aTolSpan, aNbMaxIter);

        if (!anIntegral.IsDone()) {
          myErrorReached = -1.;

          return myErrorReached;
        }

        aLocalValue(k)      += anIntegral.Value();
        err1 = aTFunc.AbsolutError()*(aT2 - aT1); 
        //cout << "Errors: " << anIntegral.NbIterReached() << " " << anIntegral.AbsolutError() << " " << err1 << endl;
        aLocalTolReached(k) += anIntegral.AbsolutError() + err1; 
        //cout << "--- Errors: " << anIntegral.NbIterReached() << " " << anIntegral.AbsolutError() << " " << err1 << endl;
      }

      aValue(k)      += aLocalValue(k);
      aTolReached(k) += aLocalTolReached(k);
    }

    // If the pointer to the domain is NULL, there is only one curve to treat:
    // U isoline with the UMax parameter.
    if (thePtrDomain == NULL)
      isMore = Standard_False;
    else {
      aPDomain->Next();
      isMore = aPDomain->More();
    }
  }

  // Get volume value.
  dim            = aValue(1);
  myErrorReached = aTolReached(1);
  myAbsolutError = myErrorReached;
  Standard_Real anAbsDim = Abs(dim);
  Standard_Real aVolTol  = Epsilon(myAbsolutError);
  if(anAbsDim >= aVolTol) myErrorReached /= anAbsDim;

  if(theCGFlag || theIFlag) {
    // Compute values of center of mass.
    if(anAbsDim >= aVolTol) {
      if (IsByPoint) {
        aValue(2) = aCoeffs[0] + aValue(2)/dim;
        aValue(3) = aCoeffs[1] + aValue(3)/dim;
        aValue(4) = aCoeffs[2] + aValue(4)/dim;
      } else {
        aValue(2) /= dim;
        aValue(3) /= dim;
        aValue(4) /= dim;
      }
    } else {
      aValue(2) = 0.;
      aValue(3) = 0.;
      aValue(4) = 0.;
      dim = 0.;
    }
    g.SetCoord(aValue(2), aValue(3), aValue(4));
  }

  if(theIFlag) {
    // Fill the matrix of inertia.
    inertia.SetCols (gp_XYZ (aValue(5), aValue(8),  aValue(9)),
      gp_XYZ (aValue(8), aValue(6),  aValue(10)),
      gp_XYZ (aValue(9), aValue(10), aValue(7)));
  }
  //return myErrorReached;
  return myAbsolutError;
}

