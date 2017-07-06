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
#include <StepRepr_ConfigurationDesign.hxx>
#include <StepRepr_ConfigurationDesignItem.hxx>
#include <StepRepr_ConfigurationItem.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ConfigurationDesign,Standard_Transient)

//=======================================================================
//function : StepRepr_ConfigurationDesign
//purpose  : 
//=======================================================================
StepRepr_ConfigurationDesign::StepRepr_ConfigurationDesign ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationDesign::Init (const Handle(StepRepr_ConfigurationItem) &aConfiguration,
                                         const StepRepr_ConfigurationDesignItem &aDesign)
{

  theConfiguration = aConfiguration;

  theDesign = aDesign;
}

//=======================================================================
//function : Configuration
//purpose  : 
//=======================================================================

Handle(StepRepr_ConfigurationItem) StepRepr_ConfigurationDesign::Configuration () const
{
  return theConfiguration;
}

//=======================================================================
//function : SetConfiguration
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationDesign::SetConfiguration (const Handle(StepRepr_ConfigurationItem) &aConfiguration)
{
  theConfiguration = aConfiguration;
}

//=======================================================================
//function : Design
//purpose  : 
//=======================================================================

StepRepr_ConfigurationDesignItem StepRepr_ConfigurationDesign::Design () const
{
  return theDesign;
}

//=======================================================================
//function : SetDesign
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationDesign::SetDesign (const StepRepr_ConfigurationDesignItem &aDesign)
{
  theDesign = aDesign;
}
