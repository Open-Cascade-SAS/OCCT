#include <StepBasic_DocumentUsageConstraint.ixx>

StepBasic_DocumentUsageConstraint::StepBasic_DocumentUsageConstraint  ()    {  }

void  StepBasic_DocumentUsageConstraint::Init
  (const Handle(StepBasic_Document)& aSource,
   const Handle(TCollection_HAsciiString)& ase,
   const Handle(TCollection_HAsciiString)& asev)
{
  theSource = aSource;
  theSE  = ase;
  theSEV = asev;
}

Handle(StepBasic_Document)  StepBasic_DocumentUsageConstraint::Source () const
{  return theSource;  }

void  StepBasic_DocumentUsageConstraint::SetSource (const Handle(StepBasic_Document)& aSource)
{  theSource = aSource;  }

Handle(TCollection_HAsciiString)  StepBasic_DocumentUsageConstraint::SubjectElement () const
{  return theSE;  }

void  StepBasic_DocumentUsageConstraint::SetSubjectElement (const Handle(TCollection_HAsciiString)& ase)
{  theSE  = ase;  }

Handle(TCollection_HAsciiString)  StepBasic_DocumentUsageConstraint::SubjectElementValue () const
{  return theSEV;  }

void  StepBasic_DocumentUsageConstraint::SetSubjectElementValue (const Handle(TCollection_HAsciiString)& asev)
{  theSEV = asev;  }
