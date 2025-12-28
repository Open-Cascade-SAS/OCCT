// Created on: 1999-02-18
// Created by: Pavel DURANDIN
// Copyright (c) 1999-1999 Matra Datavision
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

#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include "../RWStepAP214/RWStepAP214_GeneralModule.pxx"
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <STEPSelections_SelectDerived.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPSelections_SelectDerived, StepSelect_StepType)

STEPSelections_SelectDerived::STEPSelections_SelectDerived()
     
= default;

static occ::handle<Standard_Type> GetStepType(const occ::handle<StepData_ReadWriteModule>& module,
                                              const TCollection_AsciiString&               type)
{
  occ::handle<Standard_Type> atype;
  if (module.IsNull())
    return atype;
  int num = module->CaseStep(type);
  if (num == 0)
    return atype;
  occ::handle<Standard_Transient> ent;
  RWStepAP214_GeneralModule       genModul;
  genModul.NewVoid(num, ent);
  atype = ent->DynamicType();
  return atype;
}

bool STEPSelections_SelectDerived::Matches(const occ::handle<Standard_Transient>& ent,
                                           const occ::handle<Interface_InterfaceModel>& /*model*/,
                                           const TCollection_AsciiString& text,
                                           const bool /*exact*/) const
{
  int                                   CN;
  occ::handle<StepData_ReadWriteModule> module;
  bool                                  ok = thelib.Select(ent, module, CN);
  if (!ok)
    return false;
  occ::handle<Standard_Type> checker = GetStepType(module, text);
  if (checker.IsNull())
    return false;

  bool plex = module->IsComplex(CN);
  if (!plex)
  {
    DeclareAndCast(Standard_Type, atype, ent);
    if (atype.IsNull())
      atype = ent->DynamicType();
    return atype->SubType(checker);
  }
  else
  {
    NCollection_Sequence<TCollection_AsciiString> list;
    module->ComplexType(CN, list);
    int nb = list.Length();
    for (int i = 1; i <= nb; i++)
    {
      occ::handle<Standard_Type> atype = GetStepType(module, list.Value(i));
      if (atype->SubType(checker))
        return true;
    }
  }
  return false;
}
