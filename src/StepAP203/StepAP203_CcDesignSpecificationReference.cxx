// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepAP203_CcDesignSpecificationReference.ixx>

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
