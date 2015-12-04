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
#include <IGESData_IGESEntity.hxx>
#include <IGESGraph_DefinitionLevel.hxx>
#include <IGESSelect_SelectLevelNumber.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SelectLevelNumber,IFSelect_SelectExtract)

IGESSelect_SelectLevelNumber::IGESSelect_SelectLevelNumber ()    {  }


    void  IGESSelect_SelectLevelNumber::SetLevelNumber
  (const Handle(IFSelect_IntParam)& levnum)
      {  thelevnum = levnum;  }

    Handle(IFSelect_IntParam)  IGESSelect_SelectLevelNumber::LevelNumber () const
      {  return thelevnum;  }


    Standard_Boolean  IGESSelect_SelectLevelNumber::Sort
  (const Standard_Integer /*rank*/, 
   const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& /*model*/) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return Standard_False;
  Standard_Integer numlev = 0;
  if (!thelevnum.IsNull()) numlev = thelevnum->Value();
  DeclareAndCast(IGESGraph_DefinitionLevel,levelist,igesent->LevelList());
  Standard_Integer level = igesent->Level();
  if (levelist.IsNull()) return (level == numlev);
//  Cas d une liste
  if (numlev == 0) return Standard_False;
  Standard_Integer nb = levelist->NbPropertyValues();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    level = levelist->LevelNumber(i);
    if (level == numlev) return Standard_True;
  }
  return Standard_False;
}


    TCollection_AsciiString  IGESSelect_SelectLevelNumber::ExtractLabel
  () const
{
  char labl [50];
  Standard_Integer numlev = 0;
  if (!thelevnum.IsNull()) numlev = thelevnum->Value();
  if (numlev == 0) return TCollection_AsciiString
    ("IGES Entity attached to no Level");

  sprintf(labl,"IGES Entity, Level Number admitting %d",numlev);
  return TCollection_AsciiString (labl);
}
