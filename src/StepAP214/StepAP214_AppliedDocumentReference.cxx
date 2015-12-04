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
#include <StepAP214_AppliedDocumentReference.hxx>
#include <StepAP214_DocumentReferenceItem.hxx>
#include <StepBasic_Document.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AppliedDocumentReference,StepBasic_DocumentReference)

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
