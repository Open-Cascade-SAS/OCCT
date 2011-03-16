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
