// Created on: 1999-11-26
// Created by: Andrey BETENEV
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

#ifndef _StepAP203_CcDesignSpecificationReference_HeaderFile
#define _StepAP203_CcDesignSpecificationReference_HeaderFile

#include <Standard.hxx>

#include <StepAP203_HArray1OfSpecifiedItem.hxx>
#include <StepBasic_DocumentReference.hxx>
class StepBasic_Document;
class TCollection_HAsciiString;

class StepAP203_CcDesignSpecificationReference;
DEFINE_STANDARD_HANDLE(StepAP203_CcDesignSpecificationReference, StepBasic_DocumentReference)

//! Representation of STEP entity CcDesignSpecificationReference
class StepAP203_CcDesignSpecificationReference : public StepBasic_DocumentReference
{

public:
  //! Empty constructor
  Standard_EXPORT StepAP203_CcDesignSpecificationReference();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(StepBasic_Document)& aDocumentReference_AssignedDocument,
                            const Handle(TCollection_HAsciiString)& aDocumentReference_Source,
                            const Handle(StepAP203_HArray1OfSpecifiedItem)& aItems);

  //! Returns field Items
  Standard_EXPORT Handle(StepAP203_HArray1OfSpecifiedItem) Items() const;

  //! Set field Items
  Standard_EXPORT void SetItems(const Handle(StepAP203_HArray1OfSpecifiedItem)& Items);

  DEFINE_STANDARD_RTTIEXT(StepAP203_CcDesignSpecificationReference, StepBasic_DocumentReference)

protected:
private:
  Handle(StepAP203_HArray1OfSpecifiedItem) theItems;
};

#endif // _StepAP203_CcDesignSpecificationReference_HeaderFile
