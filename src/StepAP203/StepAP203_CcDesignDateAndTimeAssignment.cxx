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
#include <StepAP203_CcDesignDateAndTimeAssignment.hxx>
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_DateTimeRole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP203_CcDesignDateAndTimeAssignment,StepBasic_DateAndTimeAssignment)

//=======================================================================
//function : StepAP203_CcDesignDateAndTimeAssignment
//purpose  : 
//=======================================================================
StepAP203_CcDesignDateAndTimeAssignment::StepAP203_CcDesignDateAndTimeAssignment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP203_CcDesignDateAndTimeAssignment::Init (const Handle(StepBasic_DateAndTime) &aDateAndTimeAssignment_AssignedDateAndTime,
                                                    const Handle(StepBasic_DateTimeRole) &aDateAndTimeAssignment_Role,
                                                    const Handle(StepAP203_HArray1OfDateTimeItem) &aItems)
{
  StepBasic_DateAndTimeAssignment::Init(aDateAndTimeAssignment_AssignedDateAndTime,
                                        aDateAndTimeAssignment_Role);

  theItems = aItems;
}

//=======================================================================
//function : Items
//purpose  : 
//=======================================================================

Handle(StepAP203_HArray1OfDateTimeItem) StepAP203_CcDesignDateAndTimeAssignment::Items () const
{
  return theItems;
}

//=======================================================================
//function : SetItems
//purpose  : 
//=======================================================================

void StepAP203_CcDesignDateAndTimeAssignment::SetItems (const Handle(StepAP203_HArray1OfDateTimeItem) &aItems)
{
  theItems = aItems;
}
