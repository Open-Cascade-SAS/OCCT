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
#include <StepShape_DimensionalCharacteristic.hxx>
#include <StepShape_PlusMinusTolerance.hxx>
#include <StepShape_ToleranceMethodDefinition.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_PlusMinusTolerance,Standard_Transient)

StepShape_PlusMinusTolerance::StepShape_PlusMinusTolerance  ()    {  }

void  StepShape_PlusMinusTolerance::Init
  (const StepShape_ToleranceMethodDefinition& range,
   const StepShape_DimensionalCharacteristic& toleranced_dimension)
{
  theRange = range;
  theTolerancedDimension = toleranced_dimension;
}

StepShape_ToleranceMethodDefinition  StepShape_PlusMinusTolerance::Range () const
{  return theRange;  }

void  StepShape_PlusMinusTolerance::SetRange (const StepShape_ToleranceMethodDefinition& range)
{  theRange = range;  }

StepShape_DimensionalCharacteristic  StepShape_PlusMinusTolerance::TolerancedDimension () const
{  return theTolerancedDimension;  }

void  StepShape_PlusMinusTolerance::SetTolerancedDimension (const StepShape_DimensionalCharacteristic& toleranced_dimension)
{  theTolerancedDimension = toleranced_dimension;  }
