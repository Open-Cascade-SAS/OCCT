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

#include <IGESGraph_DefinitionLevel.hxx>
#include <IGESSelect_SignLevelNumber.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SignLevelNumber, IFSelect_Signature)

static TCollection_AsciiString laval;

IGESSelect_SignLevelNumber::IGESSelect_SignLevelNumber(const bool countmode)
    : IFSelect_Signature("Level Number"),
      thecountmode(countmode)
{
}

const char* IGESSelect_SignLevelNumber::Value(
  const occ::handle<Standard_Transient>& ent,
  const occ::handle<Interface_InterfaceModel>& /* model */) const
{
  char carlev[20];
  DeclareAndCast(IGESData_IGESEntity, igesent, ent);
  // JR/Hp
  if (igesent.IsNull())
  {
    const char* astr;
    astr = (const char*)(thecountmode ? " NO LEVEL" : "/0/");
    return astr;
  }
  //  if (igesent.IsNull()) return (thecountmode ? " NO LEVEL" : "/0/");
  DeclareAndCast(IGESGraph_DefinitionLevel, levelist, igesent->LevelList());
  int level = igesent->Level();

  if (levelist.IsNull())
  {
    // JR/Hp
    if (level < 0)
    {
      const char* astr;
      astr = (const char*)(thecountmode ? " NO LEVEL" : "/0/");
      return astr;
    }
    //    if (level < 0) return (thecountmode ? " NO LEVEL" : "/0/");
    laval.Clear();
    if (thecountmode)
      Sprintf(carlev, "%7d", level);
    else
      Sprintf(carlev, "/%d/", level);
    laval.AssignCat(carlev);
  }
  else if (thecountmode)
    return "LEVEL LIST";
  else
  {
    int i, nblev = levelist->NbLevelNumbers();
    laval.Clear();
    laval.AssignCat("LIST:/");
    for (i = 1; i <= nblev; i++)
    {
      Sprintf(carlev, "%d/", levelist->LevelNumber(i));
      laval.AssignCat(carlev);
    }
  }
  return laval.ToCString();
}
