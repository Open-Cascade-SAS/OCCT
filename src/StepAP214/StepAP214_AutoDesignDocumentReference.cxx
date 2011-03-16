#include <StepAP214_AutoDesignDocumentReference.ixx>


StepAP214_AutoDesignDocumentReference::StepAP214_AutoDesignDocumentReference ()    {  }

void  StepAP214_AutoDesignDocumentReference::Init
(const Handle(StepBasic_Document)& aAssignedDocument,
 const Handle(TCollection_HAsciiString)& aSource,
 const Handle(StepAP214_HArray1OfAutoDesignReferencingItem)& aItems)
{
  Init0 (aAssignedDocument,aSource);
  items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignReferencingItem) StepAP214_AutoDesignDocumentReference::Items () const
{  return items;  }

void  StepAP214_AutoDesignDocumentReference::SetItems (const Handle(StepAP214_HArray1OfAutoDesignReferencingItem)& aItems)
{  items = aItems;  }

StepAP214_AutoDesignReferencingItem  StepAP214_AutoDesignDocumentReference::ItemsValue (const Standard_Integer num) const
{  return items->Value(num);  }

Standard_Integer  StepAP214_AutoDesignDocumentReference::NbItems () const
{  return (items.IsNull() ? 0 : items->Length());  }
