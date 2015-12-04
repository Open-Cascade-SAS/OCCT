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


#include <IGESBasic_SingularSubfigure.hxx>
#include <IGESBasic_SubfigureDef.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDraw_CircArraySubfigure.hxx>
#include <IGESDraw_NetworkSubfigure.hxx>
#include <IGESDraw_NetworkSubfigureDef.hxx>
#include <IGESDraw_RectArraySubfigure.hxx>
#include <IGESSelect_SelectBypassSubfigure.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SelectBypassSubfigure,IFSelect_SelectExplore)

IGESSelect_SelectBypassSubfigure::IGESSelect_SelectBypassSubfigure
  (const Standard_Integer level)
  : IFSelect_SelectExplore (level)    {  }


    Standard_Boolean  IGESSelect_SelectBypassSubfigure::Explore
  (const Standard_Integer /*level*/, const Handle(Standard_Transient)& ent,
   const Interface_Graph& /*G*/,  Interface_EntityIterator& explored) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return Standard_False;
  Standard_Integer igt = igesent->TypeNumber();

//  SingularSubfigure
  if (igt == 308) {
    DeclareAndCast(IGESBasic_SubfigureDef,subf,ent);
    if (subf.IsNull()) return Standard_True;
    Standard_Integer i,nb = subf->NbEntities();
    for (i = 1; i <= nb; i ++) explored.AddItem (subf->AssociatedEntity(i));
    return Standard_True;
  }
  if (igt == 408) {
    DeclareAndCast(IGESBasic_SingularSubfigure,subf,ent);
    explored.AddItem (subf->Subfigure());
  }

//  NetworkSubfigure
  if (igt == 320) {
    DeclareAndCast(IGESDraw_NetworkSubfigureDef,subf,ent);
    if (subf.IsNull()) return Standard_True;
    Standard_Integer i,nb = subf->NbEntities();
    for (i = 1; i <= nb; i ++) explored.AddItem (subf->Entity(i));
    return Standard_True;
  }
  if (igt == 420) {
    DeclareAndCast(IGESDraw_NetworkSubfigure,subf,ent);
    explored.AddItem (subf->SubfigureDefinition());
  }

//  (Pattern)Subfigure
  if (igt == 412) {
    DeclareAndCast(IGESDraw_RectArraySubfigure,subf,ent);
    explored.AddItem (subf->BaseEntity());
  }
  if (igt == 414) {
    DeclareAndCast(IGESDraw_CircArraySubfigure,subf,ent);
    explored.AddItem (subf->BaseEntity());
  }

//  Si c est pas tout ca, c est un objet de base et on le prend tel quel
  return Standard_True;
}


    TCollection_AsciiString IGESSelect_SelectBypassSubfigure::ExploreLabel () const
      {  return TCollection_AsciiString ("Content of Subfigures");  }
