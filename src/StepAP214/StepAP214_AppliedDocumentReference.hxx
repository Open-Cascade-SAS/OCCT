// Created on: 1999-03-10
// Created by: data exchange team
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepAP214_AppliedDocumentReference_HeaderFile
#define _StepAP214_AppliedDocumentReference_HeaderFile

#include <Standard.hxx>

#include <StepAP214_HArray1OfDocumentReferenceItem.hxx>
#include <StepBasic_DocumentReference.hxx>
#include <Standard_Integer.hxx>
class StepBasic_Document;
class TCollection_HAsciiString;
class StepAP214_DocumentReferenceItem;

class StepAP214_AppliedDocumentReference;
DEFINE_STANDARD_HANDLE(StepAP214_AppliedDocumentReference, StepBasic_DocumentReference)

class StepAP214_AppliedDocumentReference : public StepBasic_DocumentReference
{

public:
  Standard_EXPORT StepAP214_AppliedDocumentReference();

  Standard_EXPORT void Init(const Handle(StepBasic_Document)&       aAssignedDocument,
                            const Handle(TCollection_HAsciiString)& aSource,
                            const Handle(StepAP214_HArray1OfDocumentReferenceItem)& aItems);

  Standard_EXPORT Handle(StepAP214_HArray1OfDocumentReferenceItem) Items() const;

  Standard_EXPORT void SetItems(const Handle(StepAP214_HArray1OfDocumentReferenceItem)& aItems);

  Standard_EXPORT StepAP214_DocumentReferenceItem ItemsValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbItems() const;

  DEFINE_STANDARD_RTTIEXT(StepAP214_AppliedDocumentReference, StepBasic_DocumentReference)

protected:
private:
  Handle(StepAP214_HArray1OfDocumentReferenceItem) items;
};

#endif // _StepAP214_AppliedDocumentReference_HeaderFile
