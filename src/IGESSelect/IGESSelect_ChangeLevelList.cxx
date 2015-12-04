// Created on: 1994-08-25
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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


#include <IFSelect_ContextModif.hxx>
#include <IFSelect_IntParam.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESSelect_ChangeLevelList.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_ChangeLevelList,IGESSelect_ModelModifier)

IGESSelect_ChangeLevelList::IGESSelect_ChangeLevelList ()
    :  IGESSelect_ModelModifier (Standard_True)    {  }

    Standard_Boolean  IGESSelect_ChangeLevelList::HasOldNumber () const
      {  return (!theold.IsNull());  }

    Handle(IFSelect_IntParam)  IGESSelect_ChangeLevelList::OldNumber () const
      {  return theold;  }

    void  IGESSelect_ChangeLevelList::SetOldNumber
  (const Handle(IFSelect_IntParam)& param)
      {  theold = param;  }

    Standard_Boolean  IGESSelect_ChangeLevelList::HasNewNumber () const
      {  return (!thenew.IsNull());  }

    Handle(IFSelect_IntParam)  IGESSelect_ChangeLevelList::NewNumber () const
      {  return thenew;  }

    void  IGESSelect_ChangeLevelList::SetNewNumber
  (const Handle(IFSelect_IntParam)& param)
      {  thenew = param;  }


    void  IGESSelect_ChangeLevelList::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& ,
   Interface_CopyTool& ) const
{
  Standard_Integer oldl = 0;
  Standard_Boolean yaold = HasOldNumber();
  if (yaold) oldl = theold->Value();
  Standard_Boolean yanew = HasOldNumber();
  Standard_Integer newl = 0;
  if (yanew) newl = thenew->Value();
  if (oldl < 0) ctx.CCheck()->AddFail("ChangeLevelList : OldNumber negative");
  if (newl < 0) ctx.CCheck()->AddFail("ChangeLevelList : NewNumber negative");
  if (oldl < 0 || newl < 0) return;

  Handle(IGESData_LevelListEntity) nulist;
  for (ctx.Start(); ctx.More(); ctx.Next()) {
    DeclareAndCast(IGESData_IGESEntity,ent,ctx.ValueResult());
    if (ent.IsNull()) continue;
    if (ent->DefLevel() != IGESData_DefSeveral) continue;
    if (yaold && ent->Level() != oldl) continue;
    if (!yanew) {
      Handle(IGESData_LevelListEntity) list = ent->LevelList();
      if (list.IsNull()) continue;
      newl = (list->NbLevelNumbers() > 0 ? list->LevelNumber(1) : 0);
      if (newl < 0) newl = 0;
    }
    ent->InitLevel(nulist,newl);
    ctx.Trace();
  }
}

    TCollection_AsciiString  IGESSelect_ChangeLevelList::Label () const
{
  char labl[100];
  Standard_Integer oldl = 0;
  Standard_Boolean yaold = HasOldNumber();
  if (yaold) oldl = theold->Value();
  Standard_Boolean yanew = HasOldNumber();
  Standard_Integer newl = 0;
  if (yanew) newl = thenew->Value();

  if (yaold) sprintf(labl,"Changes Level Lists containing %d", oldl);
  else       sprintf(labl,"Changes all Level Lists in D.E. %d", oldl);
  TCollection_AsciiString label(labl);
  if (yanew) sprintf(labl," to Number %d",newl);
  else       sprintf(labl," to Number = first value in List");
  label.AssignCat(labl);
  return label;
}
