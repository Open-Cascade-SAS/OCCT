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

//=======================================================================
//function : StepGeom_SuParameters
//purpose  : 
//=======================================================================

StepGeom_SuParameters::StepGeom_SuParameters ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepGeom_SuParameters::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                        const Standard_Real theA,
                                        const Standard_Real theAlpha,
                                        const Standard_Real theB,
                                        const Standard_Real theBeta,
                                        const Standard_Real theC,
                                        const Standard_Real theGamma)
{
  StepGeom_GeometricRepresentationItem::Init(theRepresentationItem_Name);

  myA = theA;

  myAlpha = theAlpha;

  myB = theB;

  myBeta = theBeta;

  myC = theC;

  myGamma = theGamma;
}

//=======================================================================
//function : A
//purpose  : 
//=======================================================================

Standard_Real StepGeom_SuParameters::A () const
{
  return myA;
}

//=======================================================================
//function : SetA
//purpose  : 
//=======================================================================

void StepGeom_SuParameters::SetA (const Standard_Real theA)
{
  myA = theA;
}

//=======================================================================
//function : Alpha
//purpose  : 
//=======================================================================

Standard_Real StepGeom_SuParameters::Alpha () const
{
  return myAlpha;
}

//=======================================================================
//function : SetAlpha
//purpose  : 
//=======================================================================

void StepGeom_SuParameters::SetAlpha (const Standard_Real theAlpha)
{
  myAlpha = theAlpha;
}

//=======================================================================
//function : B
//purpose  : 
//=======================================================================

Standard_Real StepGeom_SuParameters::B () const
{
  return myB;
}

//=======================================================================
//function : SetB
//purpose  : 
//=======================================================================

void StepGeom_SuParameters::SetB (const Standard_Real theB)
{
  myB = theB;
}

//=======================================================================
//function : Beta
//purpose  : 
//=======================================================================

Standard_Real StepGeom_SuParameters::Beta () const
{
  return myBeta;
}

//=======================================================================
//function : SetBeta
//purpose  : 
//=======================================================================

void StepGeom_SuParameters::SetBeta (const Standard_Real theBeta)
{
  myBeta = theBeta;
}

//=======================================================================
//function : C
//purpose  : 
//=======================================================================

Standard_Real StepGeom_SuParameters::C () const
{
  return myC;
}

//=======================================================================
//function : SetC
//purpose  : 
//=======================================================================

void StepGeom_SuParameters::SetC (const Standard_Real theC)
{
  myC = theC;
}

//=======================================================================
//function : Gamma
//purpose  : 
//=======================================================================

Standard_Real StepGeom_SuParameters::Gamma () const
{
  return myGamma;
}

//=======================================================================
//function : SetGamma
//purpose  : 
//=======================================================================

void StepGeom_SuParameters::SetGamma (const Standard_Real theGamma)
{
  myGamma = theGamma;
}
