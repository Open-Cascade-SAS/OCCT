// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESGeom_RuledSurface.hxx>
#include <IGESGeom_ToolRuledSurface.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>

// MGE 31/07/98
IGESGeom_ToolRuledSurface::IGESGeom_ToolRuledSurface ()    {  }


void IGESGeom_ToolRuledSurface::ReadOwnParams
  (const Handle(IGESGeom_RuledSurface)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  // MGE 31/07/98
 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer aDirFlag, aDevFlag;
  Handle(IGESData_IGESEntity) aCurve, anotherCurve;
  IGESData_Status aStatus;

  if (!PR.ReadEntity(IR, PR.Current(), aStatus, aCurve)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg148("XSTEP_148");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg148.Arg(Msg216.Value());
      PR.SendFail(Msg148);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg148.Arg(Msg217.Value());
      PR.SendFail(Msg148);
      break; }
    default:{
    }
    }
  }
  if (!PR.ReadEntity(IR, PR.Current(), aStatus, anotherCurve)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg149("XSTEP_149");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg149.Arg(Msg216.Value());
      PR.SendFail(Msg149);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg149.Arg(Msg217.Value());
      PR.SendFail(Msg149);
      break; }
    default:{
    }
    }
  }
  if (!PR.ReadInteger(PR.Current(), aDirFlag)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg150("XSTEP_150");
    PR.SendFail(Msg150);
  }
  if (!PR.ReadInteger(PR.Current(), aDevFlag)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg151("XSTEP_151");
    PR.SendFail(Msg151);
  }
/*
  st = PR.ReadEntity(IR, PR.Current(), "First Curve", aCurve);
  st = PR.ReadEntity(IR, PR.Current(), "Second Curve", anotherCurve);
  st = PR.ReadInteger(PR.Current(), "DirFlag", aDirFlag);
  st = PR.ReadInteger(PR.Current(), "DevFlag ", aDevFlag);
*/
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(aCurve, anotherCurve, aDirFlag, aDevFlag);
}

void IGESGeom_ToolRuledSurface::WriteOwnParams
  (const Handle(IGESGeom_RuledSurface)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->FirstCurve());
  IW.Send(ent->SecondCurve());
  IW.Send(ent->DirectionFlag());
  IW.SendBoolean(ent->IsDevelopable());
}

void  IGESGeom_ToolRuledSurface::OwnShared
  (const Handle(IGESGeom_RuledSurface)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->FirstCurve());
  iter.GetOneItem(ent->SecondCurve());
}

void IGESGeom_ToolRuledSurface::OwnCopy
  (const Handle(IGESGeom_RuledSurface)& another,
   const Handle(IGESGeom_RuledSurface)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESData_IGESEntity, aCurve,
		 TC.Transferred(another->FirstCurve()));
  DeclareAndCast(IGESData_IGESEntity, anotherCurve,
		 TC.Transferred(another->SecondCurve()));
  Standard_Integer aDirFlag = another->DirectionFlag();
  Standard_Integer aDevFlag = (another->IsDevelopable() ? 1 : 0);

  ent->Init(aCurve, anotherCurve, aDirFlag, aDevFlag);
}

IGESData_DirChecker IGESGeom_ToolRuledSurface::DirChecker
  (const Handle(IGESGeom_RuledSurface)& /*ent*/ )   const
{
  IGESData_DirChecker DC(118, 0, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.HierarchyStatusIgnored();

  return DC;
}

void IGESGeom_ToolRuledSurface::OwnCheck
  (const Handle(IGESGeom_RuledSurface)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/)  const
{
}

void IGESGeom_ToolRuledSurface::OwnDump
  (const Handle(IGESGeom_RuledSurface)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;

  S << "IGESGeom_RuledSurface" << endl;

  S << "First  Curve   : ";
  dumper.Dump(ent->FirstCurve(),S, tempSubLevel);
  S << endl;
  S << "Second Curve   : ";
  dumper.Dump(ent->SecondCurve(),S, tempSubLevel);
  S << endl;
  S << "Direction Flag : " << ent->DirectionFlag() << "  i.e.";
  if (ent->DirectionFlag() == 0) S<< "Join First to First, Last to Last"<<endl;
  else                           S<< "Join First to Last, Last to First"<<endl;
  if (ent->IsDevelopable()) S << " .. Is Developable" << endl;
  else                      S << " .. Is possibly not developable .." << endl;
}
