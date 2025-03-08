// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_ElementRepresentation_HeaderFile
#define _StepFEA_ElementRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepFEA_HArray1OfNodeRepresentation.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
class TCollection_HAsciiString;
class StepRepr_RepresentationContext;

class StepFEA_ElementRepresentation;
DEFINE_STANDARD_HANDLE(StepFEA_ElementRepresentation, StepRepr_Representation)

//! Representation of STEP entity ElementRepresentation
class StepFEA_ElementRepresentation : public StepRepr_Representation
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_ElementRepresentation();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&             aRepresentation_Name,
    const Handle(StepRepr_HArray1OfRepresentationItem)& aRepresentation_Items,
    const Handle(StepRepr_RepresentationContext)&       aRepresentation_ContextOfItems,
    const Handle(StepFEA_HArray1OfNodeRepresentation)&  aNodeList);

  //! Returns field NodeList
  Standard_EXPORT Handle(StepFEA_HArray1OfNodeRepresentation) NodeList() const;

  //! Set field NodeList
  Standard_EXPORT void SetNodeList(const Handle(StepFEA_HArray1OfNodeRepresentation)& NodeList);

  DEFINE_STANDARD_RTTIEXT(StepFEA_ElementRepresentation, StepRepr_Representation)

protected:
private:
  Handle(StepFEA_HArray1OfNodeRepresentation) theNodeList;
};

#endif // _StepFEA_ElementRepresentation_HeaderFile
