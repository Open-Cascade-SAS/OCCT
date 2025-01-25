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

#ifndef _StepRepr_RepresentationContextReference_HeaderFile_
#define _StepRepr_RepresentationContextReference_HeaderFile_

#include <Standard.hxx>
#include <Standard_Transient.hxx>

#include <TCollection_HAsciiString.hxx>

DEFINE_STANDARD_HANDLE(StepRepr_RepresentationContextReference, Standard_Transient)

//! Representation of STEP entity RepresentationContextReference
class StepRepr_RepresentationContextReference : public Standard_Transient
{
public:
  //! default constructor
  Standard_EXPORT StepRepr_RepresentationContextReference();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theContextIdentifier);

  //! Returns field ContextIdentifier
  Standard_EXPORT Handle(TCollection_HAsciiString) ContextIdentifier() const;
  //! Sets field ContextIdentifier
  Standard_EXPORT void SetContextIdentifier(
    const Handle(TCollection_HAsciiString)& theContextIdentifier);

  DEFINE_STANDARD_RTTIEXT(StepRepr_RepresentationContextReference, Standard_Transient)

private:
  Handle(TCollection_HAsciiString) myContextIdentifier;
};
#endif // _StepRepr_RepresentationContextReference_HeaderFile_
