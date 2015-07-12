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


#include <BRepGProp_Face.hxx>
#include <BRepGProp_TFunction.hxx>
#include <gp_Pnt.hxx>
#include <math_KronrodSingleIntegration.hxx>
#include <Precision.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//function : Constructor.
//purpose  : 
//=======================================================================
BRepGProp_TFunction::BRepGProp_TFunction(const BRepGProp_Face   &theSurface,
                                         const gp_Pnt           &theVertex,
                                         const Standard_Boolean  IsByPoint,
                                         const Standard_Address  theCoeffs,
                                         const Standard_Real     theUMin,
                                         const Standard_Real     theTolerance):
  mySurface(theSurface),
  myUFunction(mySurface, theVertex, IsByPoint, theCoeffs),
  myUMin(theUMin),
  myTolerance(theTolerance),
  myTolReached(0.),
  myErrReached(0.),
  myAbsError(0.),
  myValueType(GProp_Unknown),
  myIsByPoint(IsByPoint),
  myNbPntOuter(3)
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepGProp_TFunction::Init() 
{
  myTolReached = 0.;
  myErrReached = 0.;
  myAbsError = 0.;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BRepGProp_TFunction::Value(const Standard_Real X,
                                            Standard_Real &F)
{
  const Standard_Real tolU = 1.e-9;

  gp_Pnt2d                      aP2d;
  gp_Vec2d                      aV2d;
  Standard_Real                 aUMax;
  Handle(TColStd_HArray1OfReal) anUKnots;

  mySurface.D12d(X, aP2d, aV2d);
  aUMax = aP2d.X();

  if(aUMax - myUMin < tolU)
  {
    F = 0.;
    return Standard_True;
  }

  mySurface.GetUKnots(myUMin, aUMax, anUKnots);
  myUFunction.SetVParam(aP2d.Y());

  // Compute the integral from myUMin to aUMax of myUFunction.
  Standard_Integer i;
  Standard_Real    aCoeff        = aV2d.Y();
  //Standard_Integer aNbUIntervals = anUKnots->Length() - 1;
  //Standard_Real    aTol          = myTolerance/aNbUIntervals;
  Standard_Real    aTol          = myTolerance;

  //aTol /= myNbPntOuter;

  if (myValueType == GProp_Mass) {
    if (myIsByPoint)
      aCoeff /= 3.;
  } else if (myValueType == GProp_CenterMassX ||
    myValueType == GProp_CenterMassY ||
    myValueType == GProp_CenterMassZ) {
      if (myIsByPoint)
        aCoeff *= 0.25;
  } else if (myValueType == GProp_InertiaXX ||
    myValueType == GProp_InertiaYY ||
    myValueType == GProp_InertiaZZ ||
    myValueType == GProp_InertiaXY ||
    myValueType == GProp_InertiaXZ ||
    myValueType == GProp_InertiaYZ) {
      if (myIsByPoint)
        aCoeff *= 0.2;
  } else
    return Standard_False;

  Standard_Real aAbsCoeff = Abs(aCoeff);

  if (aAbsCoeff <= Precision::Angular()) {
    // No need to compute the integral. The value will be equal to 0.
    F = 0.;
    return Standard_True;
  } 
  //else if (aAbsCoeff > 10.*aTol)
  //  aTol /= aAbsCoeff;
  //else
  //  aTol = 0.1;

  Standard_Integer              iU           = anUKnots->Upper();
  Standard_Integer              aNbPntsStart;
  Standard_Integer              aNbMaxIter   = 1000;
  math_KronrodSingleIntegration anIntegral;
  Standard_Real                 aLocalErr    = 0.;

  i            = anUKnots->Lower();
  F            = 0.;

  // Epmirical criterion
  aNbPntsStart = Min(15, mySurface.UIntegrationOrder()/(anUKnots->Length() - 1)+1);
  aNbPntsStart = Max(5, aNbPntsStart);

  while (i < iU) {
    Standard_Real aU1 = anUKnots->Value(i++);
    Standard_Real aU2 = anUKnots->Value(i);

    if(aU2 - aU1 < tolU) continue;

    anIntegral.Perform(myUFunction, aU1, aU2, aNbPntsStart, aTol, aNbMaxIter);

    if (!anIntegral.IsDone())
      return Standard_False;

    F         += anIntegral.Value();
    aLocalErr += anIntegral.AbsolutError();
    //cout << " TFunction : " << anIntegral.NbIterReached() << " " << anIntegral.AbsolutError() << endl;
  }

  F            *= aCoeff;
  aLocalErr *= aAbsCoeff;
  myAbsError = Max(myAbsError, aLocalErr);

  myTolReached += aLocalErr;

  if(Abs(F) > Epsilon(1.)) aLocalErr /= Abs(F);

  myErrReached = Max(myErrReached, aLocalErr);


  return Standard_True;
}

//=======================================================================
//function : GetStateNumber
//purpose  : 
//=======================================================================

Standard_Integer BRepGProp_TFunction::GetStateNumber()
{
  //myErrReached  = myTolReached;
  //myTolReached  = 0.;
  //myNbPntOuter += RealToInt(0.5*myNbPntOuter);

  //if (myNbPntOuter%2 == 0)
  //myNbPntOuter++;

  return 0;
}
