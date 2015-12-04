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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Standard_Type.hxx>
#include <StepAP203_CcDesignSpecificationReference.hxx>
#include <StepBasic_Document.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP203_CcDesignSpecificationReference,StepBasic_DocumentReference)

//=======================================================================
//function : StepAP203_CcDesignSpecificationReference
//purpose  : 
//=======================================================================
StepAP203_CcDesignSpecificationReference::StepAP203_CcDesignSpecificationReference ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_CcDesignSpecificationReference::Init (const Handle(StepBasic_Document) &aDocumentReference_AssignedDocument,
                                                     const Handle(TCollection_HAsciiString) &aDocumentReference_Source,
                                                     const Handle(StepAP203_HArray1OfSpecifiedItem) &aItems)
{
  StepBasic_DocumentReference::Init0(aDocumentReference_AssignedDocument,
				     aDocumentReference_Source);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfSpecifiedItem) StepAP203_CcDesignSpecificationReference::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_CcDesignSpecificationReference::SetItems (const Handle(StepAP203_HArray1OfSpecifiedItem) &aItems)
{
  theItems = aItems;
}
