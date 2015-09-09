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
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_OrdinateDimension.hxx>
#include <IGESDimen_ToolOrdinateDimension.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESDimen_ToolOrdinateDimension::IGESDimen_ToolOrdinateDimension ()    {  }


void IGESDimen_ToolOrdinateDimension::ReadOwnParams
  (const Handle(IGESDimen_OrdinateDimension)& theEnt,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Handle(IGESDimen_GeneralNote) tempNote;
  Handle(IGESDimen_WitnessLine) witLine;
  Handle(IGESDimen_LeaderArrow) leadArr;
  Standard_Boolean isLine=Standard_False;

  PR.ReadEntity(IR,PR.Current(),"General Note",
		STANDARD_TYPE(IGESDimen_GeneralNote),tempNote);

  if (theEnt->FormNumber() == 0)
    {
      Handle(IGESData_IGESEntity) ent;
      if (!PR.ReadEntity(IR,PR.Current(),"Line or Leader", ent)) { }    // WARNING : Two possible Types allowed :
      else if (ent->IsKind(STANDARD_TYPE(IGESDimen_WitnessLine)))
	{
	  witLine = GetCasted(IGESDimen_WitnessLine,ent);
	  isLine = Standard_True;
	}
      else if (ent->IsKind(STANDARD_TYPE(IGESDimen_LeaderArrow)))
	{
	  leadArr = GetCasted(IGESDimen_LeaderArrow,ent);
	  isLine = Standard_False;
	}
      else PR.AddFail("Line or Leader : Type is incorrect");
    }
  else
    {
      PR.ReadEntity(IR, PR.Current(), "Line",
		    STANDARD_TYPE(IGESDimen_WitnessLine), witLine);
      PR.ReadEntity(IR, PR.Current(), "Leader",
		    STANDARD_TYPE(IGESDimen_LeaderArrow), leadArr);
    }

  DirChecker(theEnt).CheckTypeAndForm(PR.CCheck(), theEnt);
  theEnt->Init ( tempNote, isLine, witLine, leadArr);
}

void IGESDimen_ToolOrdinateDimension::WriteOwnParams
  (const Handle(IGESDimen_OrdinateDimension)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Note());
  if (ent->FormNumber() == 0)  // either WitnessLine or  LeaderArrow
    {
      if (ent->IsLine())
	IW.Send(ent->WitnessLine());
      else
	IW.Send(ent->Leader());
    }
  else                         // both   WitnessLine and LeaderArrow
    {
      IW.Send(ent->WitnessLine());
      IW.Send(ent->Leader());
    }
}

void  IGESDimen_ToolOrdinateDimension::OwnShared
  (const Handle(IGESDimen_OrdinateDimension)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Note());
  iter.GetOneItem(ent->WitnessLine());
  iter.GetOneItem(ent->Leader());
}

void IGESDimen_ToolOrdinateDimension::OwnCopy
  (const Handle(IGESDimen_OrdinateDimension)& another,
   const Handle(IGESDimen_OrdinateDimension)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESDimen_GeneralNote, tempNote,
		 TC.Transferred(another->Note()));
  DeclareAndCast(IGESDimen_WitnessLine, witLine,
		 TC.Transferred(another->WitnessLine()));
  DeclareAndCast(IGESDimen_LeaderArrow, leadArr,
		 TC.Transferred(another->Leader()));
  ent->Init(tempNote, another->IsLine(), witLine, leadArr);
}

IGESData_DirChecker IGESDimen_ToolOrdinateDimension::DirChecker
  (const Handle(IGESDimen_OrdinateDimension)& /*ent*/) const
{
  IGESData_DirChecker DC(218, 0, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);

  DC.UseFlagRequired (1);

  return DC;
}

void IGESDimen_ToolOrdinateDimension::OwnCheck
  (const Handle(IGESDimen_OrdinateDimension)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  Standard_Boolean nowitnes = ent->WitnessLine().IsNull();
  Standard_Boolean noleader = ent->Leader().IsNull();
  if (nowitnes && noleader) ach->AddFail
    ("Neither WitnessLine nor LeaderArrow is defined");
  else if (ent->FormNumber() == 0) {
    if (!nowitnes && !noleader) ach->AddFail
      ("Form 0 cannot afford both WitnessLine and LeaderArrow");
  }
  else {
    if (nowitnes || noleader) ach->AddFail
      ("Form 1 requires both WtnessLine and LeaderArrow");
  }
}

void IGESDimen_ToolOrdinateDimension::OwnDump
  (const Handle(IGESDimen_OrdinateDimension)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESDimen_OrdinateDimension" << endl;
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "General Note : ";
  dumper.Dump(ent->Note(),S, sublevel);
  S << endl;
  Handle(IGESDimen_WitnessLine) witLine = ent->WitnessLine();
  Handle(IGESDimen_LeaderArrow) leadArr = ent->Leader();
  if (!witLine.IsNull()) {
    S << "Witness line : ";
    dumper.Dump(witLine,S, sublevel);
    S << endl;
  }
  if (!leadArr.IsNull()) {
    S << "Leader arrow :";
    dumper.Dump(leadArr,S, sublevel);
    S << endl;
  }
}
