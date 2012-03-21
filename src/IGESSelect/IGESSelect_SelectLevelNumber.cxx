// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <IGESSelect_SelectLevelNumber.ixx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGraph_DefinitionLevel.hxx>
#include <Interface_Macros.hxx>
#include <stdio.h>



IGESSelect_SelectLevelNumber::IGESSelect_SelectLevelNumber ()    {  }


    void  IGESSelect_SelectLevelNumber::SetLevelNumber
  (const Handle(IFSelect_IntParam)& levnum)
      {  thelevnum = levnum;  }

    Handle(IFSelect_IntParam)  IGESSelect_SelectLevelNumber::LevelNumber () const
      {  return thelevnum;  }


    Standard_Boolean  IGESSelect_SelectLevelNumber::Sort
  (const Standard_Integer rank, const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
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
