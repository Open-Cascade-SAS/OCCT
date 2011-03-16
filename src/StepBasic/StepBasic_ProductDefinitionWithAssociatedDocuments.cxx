#include <StepBasic_ProductDefinitionWithAssociatedDocuments.ixx>

StepBasic_ProductDefinitionWithAssociatedDocuments::StepBasic_ProductDefinitionWithAssociatedDocuments  ()    {  }

void  StepBasic_ProductDefinitionWithAssociatedDocuments::Init
  (const Handle(TCollection_HAsciiString)& aId,
   const Handle(TCollection_HAsciiString)& aDescription,
   const Handle(StepBasic_ProductDefinitionFormation)& aFormation,
   const Handle(StepBasic_ProductDefinitionContext)& aFrame,
   const Handle(StepBasic_HArray1OfDocument)& aDocIds)
{
  StepBasic_ProductDefinition::Init (aId,aDescription,aFormation,aFrame);
  theDocIds = aDocIds;
}

Handle(StepBasic_HArray1OfDocument)  StepBasic_ProductDefinitionWithAssociatedDocuments::DocIds () const
{  return theDocIds;  }

void  StepBasic_ProductDefinitionWithAssociatedDocuments::SetDocIds (const Handle(StepBasic_HArray1OfDocument)& aDocIds)
{  theDocIds = aDocIds;  }

Standard_Integer  StepBasic_ProductDefinitionWithAssociatedDocuments::NbDocIds () const
{  return (theDocIds.IsNull() ? 0 : theDocIds->Length());  }

Handle(StepBasic_Document)  StepBasic_ProductDefinitionWithAssociatedDocuments::DocIdsValue (const Standard_Integer num) const
{  return theDocIds->Value(num);  }

void  StepBasic_ProductDefinitionWithAssociatedDocuments::SetDocIdsValue (const Standard_Integer num, const Handle(StepBasic_Document)& adoc)
{  theDocIds->SetValue (num,adoc);  }
