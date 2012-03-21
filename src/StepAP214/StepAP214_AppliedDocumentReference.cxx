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
