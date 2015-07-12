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
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_HArray1OfLoop.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ToolFace.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>

// MGE 03/08/98
//=======================================================================
//function : IGESSolid_ToolFace
//purpose  : 
//=======================================================================
IGESSolid_ToolFace::IGESSolid_ToolFace ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolFace::ReadOwnParams(const Handle(IGESSolid_Face)& ent,
                                       const Handle(IGESData_IGESReaderData)& IR,
                                       IGESData_ParamReader& PR) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  Message_Msg Msg197("XSTEP_197");
  Message_Msg Msg198("XSTEP_198");
  //========================================

  Standard_Boolean outerLoopFlag; //szv#4:S4163:12Mar99 `st` moved down
  Handle(IGESData_IGESEntity) anent;
  Handle(IGESSolid_Loop) aloop;
  Handle(IGESData_IGESEntity) tempSurface;
  Standard_Integer nbloops;
  Handle(IGESSolid_HArray1OfLoop) tempLoops;
  IGESData_Status aStatus;

  if (!PR.ReadEntity(IR, PR.Current(), aStatus, tempSurface)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg196("XSTEP_196");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg196.Arg(Msg216.Value());
      PR.SendFail(Msg196);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg196.Arg(Msg217.Value());
      PR.SendFail(Msg196);
      break; }
    default:{
    }
    }
  }
  Standard_Boolean st = PR.ReadInteger(PR.Current(), nbloops);
  if(!st){
    PR.SendFail(Msg197);
  }
/*
  st = PR.ReadEntity(IR, PR.Current(), "Surface", tempSurface);
  st = PR.ReadInteger(PR.Current(), "Number of loops", nbloops);
*/
  if (st && nbloops > 0) tempLoops = new IGESSolid_HArray1OfLoop(1, nbloops);
  else  PR.SendFail(Msg197);

  PR.ReadBoolean(PR.Current(), Msg198, outerLoopFlag); //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadBoolean(PR.Current(), "Outer loop flag", outerLoopFlag);

  if (!tempLoops.IsNull()) {
    for (Standard_Integer i=1; i<=nbloops; i++) {
      //st = PR.ReadEntity(IR, PR.Current(), Msg199, STANDARD_TYPE(IGESSolid_Loop), aloop); //szv#4:S4163:12Mar99 moved in if
      //st = PR.ReadEntity(IR, PR.Current(), "Loops", STANDARD_TYPE(IGESSolid_Loop), aloop);
      if (PR.ReadEntity(IR, PR.Current(), aStatus, STANDARD_TYPE(IGESSolid_Loop), aloop))
	tempLoops->SetValue(i, aloop);
      else{
	Message_Msg Msg199("XSTEP_199");
	switch(aStatus) {
	case IGESData_ReferenceError: {  
	  Message_Msg Msg216 ("IGES_216");
	  Msg199.Arg(Msg216.Value());
	  PR.SendFail(Msg199);
	  break; }
	case IGESData_EntityError: {
	  Message_Msg Msg217 ("IGES_217");
	  Msg199.Arg(Msg217.Value());
	  PR.SendFail(Msg199);
	  break; }
	case IGESData_TypeError: {
	  Message_Msg Msg218 ("IGES_218");
	  Msg199.Arg(Msg218.Value());
	  PR.SendFail(Msg199);
	  break; }
	default:{
	}
	}
      }
    }
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempSurface, outerLoopFlag, tempLoops);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESSolid_ToolFace::WriteOwnParams(const Handle(IGESSolid_Face)& ent,
                                        IGESData_IGESWriter& IW) const
{
  Standard_Integer  upper = ent->NbLoops();
  IW.Send(ent->Surface());
  IW.Send(upper);
  IW.SendBoolean(ent->HasOuterLoop());
  for (Standard_Integer i = 1; i <= upper; i ++)
    IW.Send(ent->Loop(i));
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESSolid_ToolFace::OwnShared(const Handle(IGESSolid_Face)& ent,
                                   Interface_EntityIterator& iter) const
{
  Standard_Integer  upper = ent->NbLoops();
  iter.GetOneItem(ent->Surface());
  for (Standard_Integer i = 1; i <= upper; i ++)
    iter.GetOneItem(ent->Loop(i));
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESSolid_ToolFace::OwnCopy(const Handle(IGESSolid_Face)& another,
                                 const Handle(IGESSolid_Face)& ent,
                                 Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESData_IGESEntity, tempSurface,
		 TC.Transferred(another->Surface()));
  Standard_Integer nbloops = another->NbLoops();
  Standard_Boolean outerLoopFlag = another->HasOuterLoop();

  Handle(IGESSolid_HArray1OfLoop) tempLoops =
    new IGESSolid_HArray1OfLoop(1, nbloops);
  for (Standard_Integer i=1; i<=nbloops; i++)
    {
      DeclareAndCast(IGESSolid_Loop, anent,
		     TC.Transferred(another->Loop(i)));
      tempLoops->SetValue(i, anent);
    }

  ent->Init (tempSurface, outerLoopFlag, tempLoops);
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESSolid_ToolFace::DirChecker
  (const Handle(IGESSolid_Face)& /* ent */ ) const
{
  IGESData_DirChecker DC(510, 1);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefVoid);
  DC.LineWeight (IGESData_DefVoid);
  DC.Color      (IGESData_DefAny);

  DC.SubordinateStatusRequired(1);
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESSolid_ToolFace::OwnCheck(const Handle(IGESSolid_Face)& ent,
                                  const Interface_ShareTool&,
                                  Handle(Interface_Check)& ach) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  //Message_Msg Msg197("XSTEP_197");
  //========================================

  if (ent->NbLoops() <= 0) {
    Message_Msg Msg197("XSTEP_197");
    ach->SendFail(Msg197);
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESSolid_ToolFace::OwnDump(const Handle(IGESSolid_Face)& ent,
                                 const IGESData_IGESDumper& dumper,
                                 const Handle(Message_Messenger)& S,
                                 const Standard_Integer level) const
{
  S << "IGESSolid_Face" << endl;

  Standard_Integer sublevel = (level <= 4) ? 0 : 1;
  S << "Surface : ";
  dumper.Dump(ent->Surface(),S, sublevel);
  S << endl;
  if (ent->HasOuterLoop())  S << "Outer loop is present (First one)" << endl;
  else                      S << "Outer loop is not present" << endl;
  S << "Loops : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbLoops(),ent->Loop);
  S << endl;
}

