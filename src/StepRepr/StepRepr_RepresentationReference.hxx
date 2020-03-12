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

#ifndef _StepRepr_RepresentationReference_HeaderFile_
#define _StepRepr_RepresentationReference_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationContextReference.hxx>

DEFINE_STANDARD_HANDLE(StepRepr_RepresentationReference, Standard_Transient)

//! Representation of STEP entity RepresentationReference
class StepRepr_RepresentationReference : public Standard_Transient
{
public :

  //! default constructor
  Standard_EXPORT StepRepr_RepresentationReference();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theId,
                           const Handle(StepRepr_RepresentationContextReference)& theContextOfItems);

  //! Returns field Id
  Standard_EXPORT Handle(TCollection_HAsciiString) Id() const;
  //! Sets field Id
  Standard_EXPORT void SetId (const Handle(TCollection_HAsciiString)& theId);

  //! Returns field ContextOfItems
  Standard_EXPORT Handle(StepRepr_RepresentationContextReference) ContextOfItems() const;
  //! Sets field ContextOfItems
  Standard_EXPORT void SetContextOfItems (const Handle(StepRepr_RepresentationContextReference)& theContextOfItems);

DEFINE_STANDARD_RTTIEXT(StepRepr_RepresentationReference, Standard_Transient)

private:
  Handle(TCollection_HAsciiString) myId;
  Handle(StepRepr_RepresentationContextReference) myContextOfItems;

};
#endif // _StepRepr_RepresentationReference_HeaderFile_
