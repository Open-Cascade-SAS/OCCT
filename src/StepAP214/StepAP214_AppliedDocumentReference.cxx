#include <StepAP214_AppliedDocumentReference.ixx>


StepAP214_AppliedDocumentReference::StepAP214_AppliedDocumentReference ()    {  }

void  StepAP214_AppliedDocumentReference::Init
(const Handle(StepBasic_Document)& aAssignedDocument,
 const Handle(TCollection_HAsciiString)& aSource,
 const Handle(StepAP214_HArray1OfDocumentReferenceItem)& aItems)
{
  Init0 (aAssignedDocument,aSource);
  items = aItems;
}

Handle(StepAP214_HArray1OfDocumentReferenceItem) StepAP214_AppliedDocumentReference::Items () const
{  return items;  }

void  StepAP214_AppliedDocumentReference::SetItems (const Handle(StepAP214_HArray1OfDocumentReferenceItem)& aItems)
{  items = aItems;  }

StepAP214_DocumentReferenceItem StepAP214_AppliedDocumentReference::ItemsValue (const Standard_Integer num) const
{  return items->Value(num);  }

Standard_Integer  StepAP214_AppliedDocumentReference::NbItems () const
{  return (items.IsNull() ? 0 : items->Length());  }
