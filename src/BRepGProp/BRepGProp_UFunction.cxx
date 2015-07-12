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
#include <BRepGProp_UFunction.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

//=======================================================================
//function : Constructor.
//purpose  : 
//=======================================================================
BRepGProp_UFunction::BRepGProp_UFunction(const BRepGProp_Face   &theSurface,
                                         const gp_Pnt           &theVertex,
                                         const Standard_Boolean  IsByPoint,
                                         const Standard_Address  theCoeffs)
                                         : mySurface(theSurface),
                                         myVertex(theVertex),
                                         myCoeffs(theCoeffs),
                                         myVParam(0.),
                                         myValueType(GProp_Unknown),
                                         myIsByPoint(IsByPoint)
{
}

//=======================================================================
//function : Value
//purpose  : Returns a value of the function.
//=======================================================================

Standard_Boolean BRepGProp_UFunction::Value(const Standard_Real  X,
                                            Standard_Real &F)
{
  // Volume computation
  if (myValueType == GProp_Mass) {
    gp_XYZ        aPMP0;
    Standard_Real aTmpPar1;
    Standard_Real aTmpPar2;

    F = VolumeValue(X, aPMP0, aTmpPar1, aTmpPar2);

    return Standard_True;
  }

  // Center of mass computation
  if (myValueType == GProp_CenterMassX ||
    myValueType == GProp_CenterMassY ||
    myValueType == GProp_CenterMassZ)
    return CenterMassValue(X, F);

  // Inertia computation
  if (myValueType == GProp_InertiaXX ||
    myValueType == GProp_InertiaYY ||
    myValueType == GProp_InertiaZZ ||
    myValueType == GProp_InertiaXY ||
    myValueType == GProp_InertiaXZ ||
    myValueType == GProp_InertiaYZ)
    return InertiaValue(X, F);

  return Standard_False;
}

//=======================================================================
//function : VolumeValue
//purpose  : Returns the value for volume computation.
//=======================================================================

Standard_Real BRepGProp_UFunction::VolumeValue(const Standard_Real  X,
                                               gp_XYZ        &thePMP0,
                                               Standard_Real &theS,
                                               Standard_Real &theD1)
{
  gp_Pnt aPnt;
  gp_Vec aNorm;

  mySurface.Normal(X, myVParam, aPnt, aNorm);

  thePMP0 = aPnt.XYZ().Subtracted(myVertex.XYZ());

  // Volume computation for ByPoint mode.
  if (myIsByPoint)
    return thePMP0.Dot(aNorm.XYZ());

  // Volume and additional coefficients computation for ByPlane mode.
  Standard_Real *aCoeff = (Standard_Real *)myCoeffs;

  theS  =   aNorm.X()*aCoeff[0] + aNorm.Y()*aCoeff[1] + aNorm.Z()*aCoeff[2];
  theD1 =   thePMP0.X()*aCoeff[0] + thePMP0.Y()*aCoeff[1]
  + thePMP0.Z()*aCoeff[2] - aCoeff[3];

  return theS*theD1;
}

//=======================================================================
//function : CenterMassValue
//purpose  : Returns a value for the center of mass computation.
//=======================================================================

Standard_Boolean BRepGProp_UFunction::CenterMassValue(const Standard_Real  X,
                                                      Standard_Real &F)
{
  gp_XYZ        aPmP0;
  Standard_Real aS;
  Standard_Real aD1;

  F = VolumeValue(X, aPmP0, aS, aD1);

  // Center of mass computation for ByPoint mode.
  if (myIsByPoint) {
    switch (myValueType) {
    case GProp_CenterMassX:   F *= aPmP0.X(); break;
    case GProp_CenterMassY:   F *= aPmP0.Y(); break;
    case GProp_CenterMassZ:   F *= aPmP0.Z(); break;
    default:
      return Standard_False;
    }

    return Standard_True;
  }

  // Center of mass computation for ByPlane mode.
  Standard_Real *aCoeff = (Standard_Real *)myCoeffs;

  switch (myValueType) {
  case GProp_CenterMassX:   F *= (aPmP0.X() - 0.5*aCoeff[0]*aD1); break;
  case GProp_CenterMassY:   F *= (aPmP0.Y() - 0.5*aCoeff[1]*aD1); break;
  case GProp_CenterMassZ:   F *= (aPmP0.Z() - 0.5*aCoeff[2]*aD1); break;
  default:
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : InertiaValue
//purpose  : Compute the value of intertia.
//=======================================================================

Standard_Boolean BRepGProp_UFunction::InertiaValue(const Standard_Real  X,
                                                   Standard_Real &F)
{
  gp_XYZ        aPmP0;
  Standard_Real aS;
  Standard_Real aD1;
  Standard_Real aParam1;
  Standard_Real aParam2;
  Standard_Real *aCoeffs = (Standard_Real *)myCoeffs;

  F = VolumeValue(X, aPmP0, aS, aD1);

  // Inertia computation for ByPoint mode.
  if (myIsByPoint) {
    switch(myValueType) {
    case GProp_InertiaXX:
    case GProp_InertiaYZ:
      aParam1 = aPmP0.Y() - aCoeffs[1];
      aParam2 = aPmP0.Z() - aCoeffs[2];
      break;
    case GProp_InertiaYY:
    case GProp_InertiaXZ:
      aParam1 = aPmP0.X() - aCoeffs[0];
      aParam2 = aPmP0.Z() - aCoeffs[2];
      break;
    case GProp_InertiaZZ:
    case GProp_InertiaXY:
      aParam1 = aPmP0.X() - aCoeffs[0];
      aParam2 = aPmP0.Y() - aCoeffs[1];
      break;
    default:
      return Standard_False;
    }

    if (myValueType == GProp_InertiaXX ||
      myValueType == GProp_InertiaYY ||
      myValueType == GProp_InertiaZZ)
      F *=  aParam1*aParam1 + aParam2*aParam2;
    else
      F *= -aParam1*aParam2;

    return Standard_True;
  }

  // Inertia computation for ByPlane mode.
  Standard_Real aD2 = aD1*aD1;
  Standard_Real aD3 = aD1*aD2/3.;
  Standard_Real aPPar1;
  Standard_Real aPPar2;
  Standard_Real aCoeff1;
  Standard_Real aCoeff2;

  // Inertia computation for XX, YY and ZZ.
  if (myValueType == GProp_InertiaXX ||
    myValueType == GProp_InertiaYY ||
    myValueType == GProp_InertiaZZ) {

      if (myValueType == GProp_InertiaXX) {
        aPPar1  = aPmP0.Y();
        aPPar2  = aPmP0.Z();
        aCoeff1 = aCoeffs[1];
        aCoeff2 = aCoeffs[2];
      } else if (myValueType == GProp_InertiaYY) {
        aPPar1  = aPmP0.X();
        aPPar2  = aPmP0.Z();
        aCoeff1 = aCoeffs[0];
        aCoeff2 = aCoeffs[2];
      } else { // myValueType == GProp_InertiaZZ
        aPPar1  = aPmP0.X();
        aPPar2  = aPmP0.Y();
        aCoeff1 = aCoeffs[0];
        aCoeff2 = aCoeffs[1];
      }

      aPPar1  -= aCoeff1*aD1;
      aPPar2  -= aCoeff2*aD1;
      aParam1  = aPPar1*aPPar1*aD1 + aPPar1*aCoeff1*aD2 + aCoeff1*aCoeff1*aD3;
      aParam2  = aPPar2*aPPar2*aD1 + aPPar2*aCoeff2*aD2 + aCoeff2*aCoeff2*aD3;

      F = (aParam1 + aParam2)*aS;

      return Standard_True;
  }

  // Inertia computation for XY, YZ and XZ.
  if (myValueType == GProp_InertiaXY ||
    myValueType == GProp_InertiaYZ ||
    myValueType == GProp_InertiaXZ) {

      if (myValueType == GProp_InertiaXY) {
        aPPar1  = aPmP0.X();
        aPPar2  = aPmP0.Y();
        aCoeff1 = aCoeffs[0];
        aCoeff2 = aCoeffs[1];
      } else if (myValueType == GProp_InertiaYZ) {
        aPPar1  = aPmP0.Y();
        aPPar2  = aPmP0.Z();
        aCoeff1 = aCoeffs[1];
        aCoeff2 = aCoeffs[2];
      } else { // myValueType == GProp_InertiaXZ
        aPPar1  = aPmP0.X();
        aPPar2  = aPmP0.Z();
        aCoeff1 = aCoeffs[0];
        aCoeff2 = aCoeffs[2];
      }

      aD2     *=   0.5;
      aPPar1  -=   aCoeff1*aD1;
      aPPar2  -=   aCoeff2*aD1;
      aParam1  =   aPPar1*aPPar2*aD1
        + (aPPar1*aCoeff2 + aPPar2*aCoeff1)*aD2 + aCoeff1*aCoeff2*aD3;

      F = -aParam1*aS;

      return Standard_True;
  }

  return Standard_False;
}
