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

#include <StepRepr_RepresentationContextReference.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_RepresentationContextReference, Standard_Transient)

//=======================================================================
//function : StepRepr_RepresentationContextReference
//purpose  : 
//=======================================================================

StepRepr_RepresentationContextReference::StepRepr_RepresentationContextReference ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_RepresentationContextReference::Init (const Handle(TCollection_HAsciiString)& theContextIdentifier)
{

  myContextIdentifier = theContextIdentifier;
}

//=======================================================================
//function : ContextIdentifier
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_RepresentationContextReference::ContextIdentifier () const
{
  return myContextIdentifier;
}

//=======================================================================
//function : SetContextIdentifier
//purpose  : 
//=======================================================================

void StepRepr_RepresentationContextReference::SetContextIdentifier (const Handle(TCollection_HAsciiString)& theContextIdentifier)
{
  myContextIdentifier = theContextIdentifier;
}
