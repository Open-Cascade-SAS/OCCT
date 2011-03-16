#include <StepRepr_CompoundRepresentationItem.ixx>

StepRepr_CompoundRepresentationItem::StepRepr_CompoundRepresentationItem  ()    {  }

void  StepRepr_CompoundRepresentationItem::Init
  (const Handle(TCollection_HAsciiString)& aName,
   const Handle(StepRepr_HArray1OfRepresentationItem)& item_element)
{
  StepRepr_RepresentationItem::Init (aName);
  theItemElement = item_element;
}

Handle(StepRepr_HArray1OfRepresentationItem) StepRepr_CompoundRepresentationItem::ItemElement () const
{  return theItemElement;  }

Standard_Integer  StepRepr_CompoundRepresentationItem::NbItemElement () const
{  return (theItemElement.IsNull() ? 0 : theItemElement->Length());  }

void  StepRepr_CompoundRepresentationItem::SetItemElement
  (const Handle(StepRepr_HArray1OfRepresentationItem)& item_element)
{  theItemElement = item_element;  }

Handle(StepRepr_RepresentationItem)  StepRepr_CompoundRepresentationItem::ItemElementValue
  (const Standard_Integer num) const
{  return theItemElement->Value(num);  }

void  StepRepr_CompoundRepresentationItem::SetItemElementValue
  (const Standard_Integer num, const Handle(StepRepr_RepresentationItem)& anelement)
{  theItemElement->SetValue (num,anelement);  }
