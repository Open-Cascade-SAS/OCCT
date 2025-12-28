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

#include <IFSelect_IntParam.hxx>
#include <IGESGraph_DefinitionLevel.hxx>
#include <IGESSelect_SelectLevelNumber.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SelectLevelNumber, IFSelect_SelectExtract)

IGESSelect_SelectLevelNumber::IGESSelect_SelectLevelNumber() {}

void IGESSelect_SelectLevelNumber::SetLevelNumber(const occ::handle<IFSelect_IntParam>& levnum)
{
  thelevnum = levnum;
}

occ::handle<IFSelect_IntParam> IGESSelect_SelectLevelNumber::LevelNumber() const
{
  return thelevnum;
}

bool IGESSelect_SelectLevelNumber::Sort(
  const int /*rank*/,
  const occ::handle<Standard_Transient>& ent,
  const occ::handle<Interface_InterfaceModel>& /*model*/) const
{
  DeclareAndCast(IGESData_IGESEntity, igesent, ent);
  if (igesent.IsNull())
    return false;
  int numlev = 0;
  if (!thelevnum.IsNull())
    numlev = thelevnum->Value();
  DeclareAndCast(IGESGraph_DefinitionLevel, levelist, igesent->LevelList());
  int level = igesent->Level();
  if (levelist.IsNull())
    return (level == numlev);
  //  Cas d une liste
  if (numlev == 0)
    return false;
  int nb = levelist->NbPropertyValues();
  for (int i = 1; i <= nb; i++)
  {
    level = levelist->LevelNumber(i);
    if (level == numlev)
      return true;
  }
  return false;
}

TCollection_AsciiString IGESSelect_SelectLevelNumber::ExtractLabel() const
{
  char labl[50];
  int  numlev = 0;
  if (!thelevnum.IsNull())
    numlev = thelevnum->Value();
  if (numlev == 0)
    return TCollection_AsciiString("IGES Entity attached to no Level");

  Sprintf(labl, "IGES Entity, Level Number admitting %d", numlev);
  return TCollection_AsciiString(labl);
}
