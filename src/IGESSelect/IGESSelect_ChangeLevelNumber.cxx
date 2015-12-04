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
#include <IGESSelect_ChangeLevelNumber.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_ChangeLevelNumber,IGESSelect_ModelModifier)

IGESSelect_ChangeLevelNumber::IGESSelect_ChangeLevelNumber ()
    :  IGESSelect_ModelModifier (Standard_False)    {  }

    Standard_Boolean  IGESSelect_ChangeLevelNumber::HasOldNumber () const
      {  return (!theold.IsNull());  }

    Handle(IFSelect_IntParam)  IGESSelect_ChangeLevelNumber::OldNumber () const
      {  return theold;  }

    void  IGESSelect_ChangeLevelNumber::SetOldNumber
  (const Handle(IFSelect_IntParam)& param)
      {  theold = param;  }

    Handle(IFSelect_IntParam)  IGESSelect_ChangeLevelNumber::NewNumber () const
      {  return thenew;  }

    void  IGESSelect_ChangeLevelNumber::SetNewNumber
  (const Handle(IFSelect_IntParam)& param)
      {  thenew = param;  }


    void  IGESSelect_ChangeLevelNumber::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& ,
   Interface_CopyTool& ) const
{
  Standard_Integer oldl = 0;
  Standard_Boolean yaold = HasOldNumber();
  if (yaold) oldl = theold->Value();
  Standard_Integer newl = 0;
  if (!thenew.IsNull()) newl = thenew->Value();
  if (oldl < 0) ctx.CCheck()->AddFail("ChangeLevelNumber : OldNumber negative");
  if (newl < 0) ctx.CCheck()->AddFail("ChangeLevelNumber : NewNumber negative");
  if (oldl < 0 || newl < 0) return;

  Handle(IGESData_LevelListEntity) nulist;
  for (ctx.Start(); ctx.More(); ctx.Next()) {
    DeclareAndCast(IGESData_IGESEntity,ent,ctx.ValueResult());
    if (ent.IsNull()) continue;
    if (ent->DefLevel() == IGESData_DefSeveral) continue;
    if (yaold && ent->Level() != oldl) continue;
    ent->InitLevel(nulist,newl);
    ctx.Trace();
  }
}

    TCollection_AsciiString  IGESSelect_ChangeLevelNumber::Label () const
{
  char labl[100];
  Standard_Integer oldl = 0;
  Standard_Boolean yaold = HasOldNumber();
  if (yaold) oldl = theold->Value();
  Standard_Integer newl = 0;
  if (!thenew.IsNull()) newl = thenew->Value();

  if (yaold) sprintf(labl,"Changes Level Number %d to %d",oldl,newl);
  else       sprintf(labl,"Changes all Level Numbers positive and zero to %d",
		     newl);
  return TCollection_AsciiString(labl);
}
