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
#include <StepRepr_AssemblyComponentUsage.hxx>
#include <StepRepr_AssemblyComponentUsageSubstitute.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_AssemblyComponentUsageSubstitute,Standard_Transient)

StepRepr_AssemblyComponentUsageSubstitute::StepRepr_AssemblyComponentUsageSubstitute ()    {  }

void  StepRepr_AssemblyComponentUsageSubstitute::Init
  (const Handle(TCollection_HAsciiString)& aName,
   const Handle(TCollection_HAsciiString)& aDef,
   const Handle(StepRepr_AssemblyComponentUsage)& aBase,
   const Handle(StepRepr_AssemblyComponentUsage)& aSubs)
{
  theName = aName;
  theDef  = aDef;
  theBase = aBase;
  theSubs = aSubs;
}

Handle(TCollection_HAsciiString)  StepRepr_AssemblyComponentUsageSubstitute::Name () const
{  return theName;  }

void  StepRepr_AssemblyComponentUsageSubstitute::SetName (const Handle(TCollection_HAsciiString)& aName)
{  theName = aName;  }

Handle(TCollection_HAsciiString)  StepRepr_AssemblyComponentUsageSubstitute::Definition () const
{  return theDef;  }

void  StepRepr_AssemblyComponentUsageSubstitute::SetDefinition (const Handle(TCollection_HAsciiString)& aDefinition)
{  theDef = aDefinition;  }

Handle(StepRepr_AssemblyComponentUsage)  StepRepr_AssemblyComponentUsageSubstitute::Base () const
{  return theBase;  }

void  StepRepr_AssemblyComponentUsageSubstitute::SetBase (const Handle(StepRepr_AssemblyComponentUsage)& aBase)
{  theBase = aBase;  }

Handle(StepRepr_AssemblyComponentUsage)  StepRepr_AssemblyComponentUsageSubstitute::Substitute () const
{  return theSubs;  }

void  StepRepr_AssemblyComponentUsageSubstitute::SetSubstitute (const Handle(StepRepr_AssemblyComponentUsage)& aSubs)
{  theSubs = aSubs;  }
