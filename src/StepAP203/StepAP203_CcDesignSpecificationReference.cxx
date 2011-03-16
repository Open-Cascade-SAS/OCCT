// File:	StepAP203_CcDesignSpecificationReference.cxx
// Created:	Fri Nov 26 16:26:33 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

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
