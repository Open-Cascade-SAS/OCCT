// Created on: 1999-02-18
// Created by: Pavel DURANDIN
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

#ifndef _STEPSelections_SelectDerived_HeaderFile
#define _STEPSelections_SelectDerived_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepSelect_StepType.hxx>
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;

class STEPSelections_SelectDerived : public StepSelect_StepType
{

public:
  Standard_EXPORT STEPSelections_SelectDerived();

  Standard_EXPORT virtual bool Matches(const occ::handle<Standard_Transient>&       ent,
                                       const occ::handle<Interface_InterfaceModel>& model,
                                       const TCollection_AsciiString&               text,
                                       const bool exact) const override;

  DEFINE_STANDARD_RTTIEXT(STEPSelections_SelectDerived, StepSelect_StepType)
};

#endif // _STEPSelections_SelectDerived_HeaderFile
