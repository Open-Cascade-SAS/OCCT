#include <StepBasic_DocumentRelationship.ixx>

StepBasic_DocumentRelationship::StepBasic_DocumentRelationship  ()    {  }

void  StepBasic_DocumentRelationship::Init
  (const Handle(TCollection_HAsciiString)& aName,
   const Handle(TCollection_HAsciiString)& aDescription,
   const Handle(StepBasic_Document)& aRelating,
   const Handle(StepBasic_Document)& aRelated)
{
  theName = aName;
  theDescription = aDescription;
  theRelating = aRelating;
  theRelated  = aRelated;
}

Handle(TCollection_HAsciiString)  StepBasic_DocumentRelationship::Name () const
{  return theName;  }

void  StepBasic_DocumentRelationship::SetName (const Handle(TCollection_HAsciiString)& aName)
{  theName = aName;  }

Handle(TCollection_HAsciiString)  StepBasic_DocumentRelationship::Description () const
{  return theDescription;  }

void  StepBasic_DocumentRelationship::SetDescription (const Handle(TCollection_HAsciiString)& aDescription)
{  theDescription = aDescription;  }

Handle(StepBasic_Document)  StepBasic_DocumentRelationship::RelatingDocument () const
{  return theRelating;  }

void  StepBasic_DocumentRelationship::SetRelatingDocument (const Handle(StepBasic_Document)& aRelating)
{  theRelating = aRelating;  }

Handle(StepBasic_Document)  StepBasic_DocumentRelationship::RelatedDocument () const
{  return theRelated;  }

void  StepBasic_DocumentRelationship::SetRelatedDocument (const Handle(StepBasic_Document)& aRelated)
{  theRelated = aRelated;  }
