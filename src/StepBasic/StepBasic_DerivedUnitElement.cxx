// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Standard_Type.hxx>
#include <StepBasic_DerivedUnitElement.hxx>
#include <StepBasic_NamedUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_DerivedUnitElement,Standard_Transient)

StepBasic_DerivedUnitElement::StepBasic_DerivedUnitElement ()    {  }

void  StepBasic_DerivedUnitElement::Init (const Handle(StepBasic_NamedUnit)& aUnit, const Standard_Real aExponent)
{  theUnit = aUnit;  theExponent = aExponent;  }

void  StepBasic_DerivedUnitElement::SetUnit (const Handle(StepBasic_NamedUnit)& aUnit)
{  theUnit = aUnit;  }

Handle(StepBasic_NamedUnit)  StepBasic_DerivedUnitElement::Unit () const
{  return theUnit;  }

void  StepBasic_DerivedUnitElement::SetExponent (const Standard_Real aExponent)
{  theExponent = aExponent;  }

Standard_Real  StepBasic_DerivedUnitElement::Exponent () const
{  return theExponent;  }
