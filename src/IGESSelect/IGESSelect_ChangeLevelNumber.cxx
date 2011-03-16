// Copyright: 	Matra-Datavision 1994
// File:	IGESSelect_ChangeLevelNumber.cxx
// Created:	Thu Aug 25 15:21:12 1994
// Author:	Christian CAILLET
//		<cky>
#include <IGESSelect_ChangeLevelNumber.ixx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>
#include <stdio.h>
#include <Interface_Check.hxx>


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
