// Created on : Sat May 02 12:41:14 2020
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include <StepGeom_SuParameters.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_SuParameters, StepGeom_GeometricRepresentationItem)

//=================================================================================================

StepGeom_SuParameters::StepGeom_SuParameters() {}

//=================================================================================================

void StepGeom_SuParameters::Init(
  const occ::handle<TCollection_HAsciiString>& theRepresentationItem_Name,
  const double                                 theA,
  const double                                 theAlpha,
  const double                                 theB,
  const double                                 theBeta,
  const double                                 theC,
  const double                                 theGamma)
{
  StepGeom_GeometricRepresentationItem::Init(theRepresentationItem_Name);

  myA = theA;

  myAlpha = theAlpha;

  myB = theB;

  myBeta = theBeta;

  myC = theC;

  myGamma = theGamma;
}

//=================================================================================================

double StepGeom_SuParameters::A() const
{
  return myA;
}

//=================================================================================================

void StepGeom_SuParameters::SetA(const double theA)
{
  myA = theA;
}

//=================================================================================================

double StepGeom_SuParameters::Alpha() const
{
  return myAlpha;
}

//=================================================================================================

void StepGeom_SuParameters::SetAlpha(const double theAlpha)
{
  myAlpha = theAlpha;
}

//=================================================================================================

double StepGeom_SuParameters::B() const
{
  return myB;
}

//=================================================================================================

void StepGeom_SuParameters::SetB(const double theB)
{
  myB = theB;
}

//=================================================================================================

double StepGeom_SuParameters::Beta() const
{
  return myBeta;
}

//=================================================================================================

void StepGeom_SuParameters::SetBeta(const double theBeta)
{
  myBeta = theBeta;
}

//=================================================================================================

double StepGeom_SuParameters::C() const
{
  return myC;
}

//=================================================================================================

void StepGeom_SuParameters::SetC(const double theC)
{
  myC = theC;
}

//=================================================================================================

double StepGeom_SuParameters::Gamma() const
{
  return myGamma;
}

//=================================================================================================

void StepGeom_SuParameters::SetGamma(const double theGamma)
{
  myGamma = theGamma;
}
