#include <StepBasic_DocumentReference.ixx>

void  StepBasic_DocumentReference::Init0
  (const Handle(StepBasic_Document)& aAssignedDocument,
   const Handle(TCollection_HAsciiString)& aSource)
{
  theAssignedDocument = aAssignedDocument;
  theSource = aSource;
}

Handle(StepBasic_Document)  StepBasic_DocumentReference::AssignedDocument () const
{  return theAssignedDocument;  }

void  StepBasic_DocumentReference::SetAssignedDocument (const Handle(StepBasic_Document)& aAssignedDocument)
{  theAssignedDocument = aAssignedDocument;  }

Handle(TCollection_HAsciiString)  StepBasic_DocumentReference::Source () const
{  return theSource;  }

void  StepBasic_DocumentReference::SetSource (const Handle(TCollection_HAsciiString)& aSource)
{  theSource = aSource;  }
