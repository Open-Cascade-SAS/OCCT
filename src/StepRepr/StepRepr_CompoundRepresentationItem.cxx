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
