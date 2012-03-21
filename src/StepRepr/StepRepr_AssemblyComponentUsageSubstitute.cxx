// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepRepr_AssemblyComponentUsageSubstitute.ixx>

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
