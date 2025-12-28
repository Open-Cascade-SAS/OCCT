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

#include <IGESData_IGESEntity.hxx>
#include <IGESSelect_SelectName.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <cstdio>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SelectName, IFSelect_SelectExtract)

IGESSelect_SelectName::IGESSelect_SelectName() = default;

void IGESSelect_SelectName::SetName(const occ::handle<TCollection_HAsciiString>& levnum)
{
  thename = levnum;
}

occ::handle<TCollection_HAsciiString> IGESSelect_SelectName::Name() const
{
  return thename;
}

bool IGESSelect_SelectName::Sort(const int /*rank*/,
                                 const occ::handle<Standard_Transient>& ent,
                                 const occ::handle<Interface_InterfaceModel>& /*model*/) const
{
  DeclareAndCast(IGESData_IGESEntity, igesent, ent);
  if (igesent.IsNull())
    return false;
  if (!igesent->HasName())
    return false;
  if (thename.IsNull())
    return false;
  occ::handle<TCollection_HAsciiString> name = igesent->NameValue();
  //  std::cout<<"SelectName:"<<thename->ToCString()<<",with:"<<name->ToCString()<<",IsSameString="<<thename->IsSameString
  //  (name,false)<<std::endl;
  int nb0 = thename->Length();
  int nb1 = name->Length();
  int nbf = (nb1 <= nb0 ? nb1 : nb0);
  int nbt = (nb1 >= nb0 ? nb1 : nb0);
  int i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nbf; i++)
  {
    if (name->Value(i) != thename->Value(i))
      return false;
  }
  if (nb0 > nb1)
    name = thename;
  for (i = nbf + 1; i <= nbt; i++)
  {
    if (name->Value(i) != ' ')
      return false;
  }
  return true;
}

TCollection_AsciiString IGESSelect_SelectName::ExtractLabel() const
{
  char labl[50];
  if (!thename.IsNull())
  {
    Sprintf(labl, "IGES Entity, Name : %s", thename->ToCString());
    return TCollection_AsciiString(labl);
  }
  else
    return TCollection_AsciiString("IGES Entity, Name : (undefined)");
}
