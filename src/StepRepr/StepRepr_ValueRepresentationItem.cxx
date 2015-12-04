// Created on: 2015-09-09
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <Standard_Type.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepRepr_ValueRepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ValueRepresentationItem,StepRepr_RepresentationItem)

StepRepr_ValueRepresentationItem::StepRepr_ValueRepresentationItem ()  {}

void StepRepr_ValueRepresentationItem::Init(
  const Handle(TCollection_HAsciiString)& theName,
  const Handle(StepBasic_MeasureValueMember)& theValueComponentMember)
{
  valueComponentMember = theValueComponentMember;
  StepRepr_RepresentationItem::Init(theName);
}
