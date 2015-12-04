// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Standard_Type.hxx>
#include <StepRepr_CompoundRepresentationItem.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_CompoundRepresentationItem,StepRepr_RepresentationItem)

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
