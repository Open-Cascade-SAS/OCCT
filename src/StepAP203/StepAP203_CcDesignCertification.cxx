// File:	StepAP203_CcDesignCertification.cxx
// Created:	Fri Nov 26 16:26:31 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepAP203_CcDesignCertification.ixx>

//=======================================================================
//function : StepAP203_CcDesignCertification
//purpose  : 
//=======================================================================

StepAP203_CcDesignCertification::StepAP203_CcDesignCertification ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_CcDesignCertification::Init (const Handle(StepBasic_Certification) &aCertificationAssignment_AssignedCertification,
                                            const Handle(StepAP203_HArray1OfCertifiedItem) &aItems)
{
  StepBasic_CertificationAssignment::Init(aCertificationAssignment_AssignedCertification);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfCertifiedItem) StepAP203_CcDesignCertification::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_CcDesignCertification::SetItems (const Handle(StepAP203_HArray1OfCertifiedItem) &aItems)
{
  theItems = aItems;
}
