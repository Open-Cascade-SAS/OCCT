// Created on : Sat May 02 12:41:14 2020 
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include <StepRepr_RepresentationReference.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_RepresentationReference, Standard_Transient)

//=======================================================================
//function : StepRepr_RepresentationReference
//purpose  : 
//=======================================================================

StepRepr_RepresentationReference::StepRepr_RepresentationReference ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_RepresentationReference::Init (const Handle(TCollection_HAsciiString)& theId,
                                             const Handle(StepRepr_RepresentationContextReference)& theContextOfItems)
{

  myId = theId;

  myContextOfItems = theContextOfItems;
}

//=======================================================================
//function : Id
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_RepresentationReference::Id () const
{
  return myId;
}

//=======================================================================
//function : SetId
//purpose  : 
//=======================================================================

void StepRepr_RepresentationReference::SetId (const Handle(TCollection_HAsciiString)& theId)
{
  myId = theId;
}

//=======================================================================
//function : ContextOfItems
//purpose  : 
//=======================================================================

Handle(StepRepr_RepresentationContextReference) StepRepr_RepresentationReference::ContextOfItems () const
{
  return myContextOfItems;
}

//=======================================================================
//function : SetContextOfItems
//purpose  : 
//=======================================================================

void StepRepr_RepresentationReference::SetContextOfItems (const Handle(StepRepr_RepresentationContextReference)& theContextOfItems)
{
  myContextOfItems = theContextOfItems;
}
