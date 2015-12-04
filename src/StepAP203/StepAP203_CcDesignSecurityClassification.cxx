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
#include <StepAP203_CcDesignSecurityClassification.hxx>
#include <StepBasic_SecurityClassification.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP203_CcDesignSecurityClassification,StepBasic_SecurityClassificationAssignment)

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
