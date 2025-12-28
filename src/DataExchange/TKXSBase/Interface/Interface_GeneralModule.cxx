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

#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_GeneralModule, Standard_Transient)

void Interface_GeneralModule::FillShared(const occ::handle<Interface_InterfaceModel>& /*model*/,
                                         const int                              casenum,
                                         const occ::handle<Standard_Transient>& ent,
                                         Interface_EntityIterator&              iter) const
{
  FillSharedCase(casenum, ent, iter);
} // Par defaut, model ne sert pas

void Interface_GeneralModule::Share(Interface_EntityIterator&              iter,
                                    const occ::handle<Standard_Transient>& shared) const
{
  iter.GetOneItem(shared);
} // Plus joli d appeler Share

void Interface_GeneralModule::ListImplied(const occ::handle<Interface_InterfaceModel>& /*model*/,
                                          const int                              casenum,
                                          const occ::handle<Standard_Transient>& ent,
                                          Interface_EntityIterator&              iter) const
{
  ListImpliedCase(casenum, ent, iter);
} // Par defaut, model ne sert pas

void Interface_GeneralModule::ListImpliedCase(const int /*casenum*/,
                                              const occ::handle<Standard_Transient>& /*ent*/,
                                              Interface_EntityIterator& /*iter*/) const
{
} // Par defaut, pas d Imply

bool Interface_GeneralModule::CanCopy(const int /*CN*/,
                                      const occ::handle<Standard_Transient>& /*ent*/) const
{
  return false;
}

bool Interface_GeneralModule::Dispatch(const int,
                                       const occ::handle<Standard_Transient>& entfrom,
                                       occ::handle<Standard_Transient>&       entto,
                                       Interface_CopyTool&) const
{
  entto = entfrom;
  return false;
}

bool Interface_GeneralModule::NewCopiedCase(const int,
                                            const occ::handle<Standard_Transient>&,
                                            occ::handle<Standard_Transient>&,
                                            Interface_CopyTool&) const
{
  return false;
}

void Interface_GeneralModule::RenewImpliedCase(const int /*casenum*/,
                                               const occ::handle<Standard_Transient>& /*entfrom*/,
                                               const occ::handle<Standard_Transient>& /*entto*/,
                                               const Interface_CopyTool& /*TC*/) const
{
} // By default, does nothing

void Interface_GeneralModule::WhenDeleteCase(const int /*casenum*/,
                                             const occ::handle<Standard_Transient>& /*ent*/,
                                             const bool /*dispatched*/) const
{
} // by default, does nothing

int Interface_GeneralModule::CategoryNumber(const int,
                                            const occ::handle<Standard_Transient>&,
                                            const Interface_ShareTool&) const
{
  return 0;
} // by default, not specified

occ::handle<TCollection_HAsciiString> Interface_GeneralModule::Name(
  const int,
  const occ::handle<Standard_Transient>&,
  const Interface_ShareTool&) const
{
  occ::handle<TCollection_HAsciiString> str;
  return str;
} // by default, not specified
