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


#include <Standard_Transient.hxx>
#include <StepShape_LimitsAndFits.hxx>
#include <StepShape_ToleranceMethodDefinition.hxx>
#include <StepShape_ToleranceValue.hxx>

StepShape_ToleranceMethodDefinition::StepShape_ToleranceMethodDefinition  ()    {  }

Standard_Integer  StepShape_ToleranceMethodDefinition::CaseNum
  (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepShape_ToleranceValue))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_LimitsAndFits))) return 2;
  return 0;
}

Handle(StepShape_ToleranceValue)  StepShape_ToleranceMethodDefinition::ToleranceValue () const
{  return Handle(StepShape_ToleranceValue)::DownCast(Value());  }

Handle(StepShape_LimitsAndFits)  StepShape_ToleranceMethodDefinition::LimitsAndFits () const
{  return Handle(StepShape_LimitsAndFits)::DownCast(Value());  }
