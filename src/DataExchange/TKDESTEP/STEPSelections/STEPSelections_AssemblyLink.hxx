// Created on: 1999-03-24
// Created by: data exchange team
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

#ifndef _STEPSelections_AssemblyLink_HeaderFile
#define _STEPSelections_AssemblyLink_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepRepr_NextAssemblyUsageOccurrence;
class STEPSelections_AssemblyComponent;

class STEPSelections_AssemblyLink : public Standard_Transient
{

public:
  Standard_EXPORT STEPSelections_AssemblyLink();

  Standard_EXPORT STEPSelections_AssemblyLink(
    const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& nauo,
    const occ::handle<Standard_Transient>&                   item,
    const occ::handle<STEPSelections_AssemblyComponent>&     part);

  occ::handle<StepRepr_NextAssemblyUsageOccurrence> GetNAUO() const;

  occ::handle<Standard_Transient> GetItem() const;

  occ::handle<STEPSelections_AssemblyComponent> GetComponent() const;

  void SetNAUO(const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& nauo);

  void SetItem(const occ::handle<Standard_Transient>& item);

  void SetComponent(const occ::handle<STEPSelections_AssemblyComponent>& part);

  DEFINE_STANDARD_RTTIEXT(STEPSelections_AssemblyLink, Standard_Transient)

private:
  occ::handle<StepRepr_NextAssemblyUsageOccurrence> myNAUO;
  occ::handle<Standard_Transient>                   myItem;
  occ::handle<STEPSelections_AssemblyComponent>     myComponent;
};

#include <STEPSelections_AssemblyLink.lxx>

#endif // _STEPSelections_AssemblyLink_HeaderFile
