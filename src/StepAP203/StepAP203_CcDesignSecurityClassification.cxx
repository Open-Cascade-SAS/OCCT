// File:	StepAP203_CcDesignSecurityClassification.cxx
// Created:	Fri Nov 26 16:26:33 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_CcDesignSecurityClassification.ixx>

//=======================================================================
//function : StepAP203_CcDesignSecurityClassification
//purpose  : 
//=======================================================================

StepAP203_CcDesignSecurityClassification::StepAP203_CcDesignSecurityClassification ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_CcDesignSecurityClassification::Init (const Handle(StepBasic_SecurityClassification) &aSecurityClassificationAssignment_AssignedSecurityClassification,
                                                     const Handle(StepAP203_HArray1OfClassifiedItem) &aItems)
{
  StepBasic_SecurityClassificationAssignment::Init(aSecurityClassificationAssignment_AssignedSecurityClassification);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfClassifiedItem) StepAP203_CcDesignSecurityClassification::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_CcDesignSecurityClassification::SetItems (const Handle(StepAP203_HArray1OfClassifiedItem) &aItems)
{
  theItems = aItems;
}
