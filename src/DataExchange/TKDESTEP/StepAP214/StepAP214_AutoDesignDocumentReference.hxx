// Created on: 1998-08-04
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _StepAP214_AutoDesignDocumentReference_HeaderFile
#define _StepAP214_AutoDesignDocumentReference_HeaderFile

#include <Standard.hxx>

#include <StepAP214_AutoDesignReferencingItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_DocumentReference.hxx>
#include <Standard_Integer.hxx>
class StepBasic_Document;
class TCollection_HAsciiString;
class StepAP214_AutoDesignReferencingItem;

class StepAP214_AutoDesignDocumentReference : public StepBasic_DocumentReference
{

public:
  Standard_EXPORT StepAP214_AutoDesignDocumentReference();

  Standard_EXPORT void Init(const occ::handle<StepBasic_Document>&       aAssignedDocument,
                            const occ::handle<TCollection_HAsciiString>& aSource,
                            const occ::handle<NCollection_HArray1<StepAP214_AutoDesignReferencingItem>>& aItems);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepAP214_AutoDesignReferencingItem>> Items() const;

  Standard_EXPORT void SetItems(const occ::handle<NCollection_HArray1<StepAP214_AutoDesignReferencingItem>>& aItems);

  Standard_EXPORT StepAP214_AutoDesignReferencingItem ItemsValue(const int num) const;

  Standard_EXPORT int NbItems() const;

  DEFINE_STANDARD_RTTIEXT(StepAP214_AutoDesignDocumentReference, StepBasic_DocumentReference)

private:
  occ::handle<NCollection_HArray1<StepAP214_AutoDesignReferencingItem>> items;
};

#endif // _StepAP214_AutoDesignDocumentReference_HeaderFile
