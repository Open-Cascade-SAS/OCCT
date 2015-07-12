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

#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_FlagNote.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_HArray1OfLeaderArrow.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_ToolFlagNote.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESDimen_ToolFlagNote::IGESDimen_ToolFlagNote ()    {  }


void  IGESDimen_ToolFlagNote::ReadOwnParams
  (const Handle(IGESDimen_FlagNote)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down

  gp_XYZ lowerLeft; 
  Standard_Real angle; 
  Handle(IGESDimen_GeneralNote) note;
  Standard_Integer nbval;
  Handle(IGESDimen_HArray1OfLeaderArrow) leaders;

  PR.ReadXYZ (PR.CurrentList(1, 3), "Lower Left Corner Co-ords", lowerLeft); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadReal(PR.Current(), "Rotation Angle", angle); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "General Note Entity",
		STANDARD_TYPE(IGESDimen_GeneralNote), note); //szv#4:S4163:12Mar99 `st=` not needed

  Standard_Boolean st = PR.ReadInteger(PR.Current(), "Number of Leaders", nbval);
  if (st && nbval > 0)
    {
      leaders = new IGESDimen_HArray1OfLeaderArrow(1, nbval);

      for (Standard_Integer i = 1; i <= nbval; i++)
	{
	  Handle(IGESDimen_LeaderArrow) anentity;
	  //st = PR.ReadEntity(IR, PR.Current(), "Leaders",
			       //STANDARD_TYPE(IGESDimen_LeaderArrow), anentity); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadEntity(IR, PR.Current(), "Leaders", STANDARD_TYPE(IGESDimen_LeaderArrow), anentity))
	    leaders->SetValue(i, anentity);
	}
    }
  else if (nbval < 0)
    PR.AddFail("Number of Leaders: Less than zero");

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(lowerLeft, angle, note, leaders);
}

void  IGESDimen_ToolFlagNote::WriteOwnParams
  (const Handle(IGESDimen_FlagNote)& ent, IGESData_IGESWriter& IW) const 
{ 
  IW.Send(ent->LowerLeftCorner().X());
  IW.Send(ent->LowerLeftCorner().Y());
  IW.Send(ent->LowerLeftCorner().Z());
  IW.Send(ent->Angle());
  IW.Send(ent->Note());
  Standard_Integer upper = ent->NbLeaders();
  IW.Send(upper);
  for (Standard_Integer i = 1; i <= upper; i ++)
    IW.Send(ent->Leader(i));
}

void  IGESDimen_ToolFlagNote::OwnShared
  (const Handle(IGESDimen_FlagNote)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Note());
  Standard_Integer upper = ent->NbLeaders();
  for (Standard_Integer i = 1; i <= upper; i ++)
    iter.GetOneItem(ent->Leader(i));
}

void  IGESDimen_ToolFlagNote::OwnCopy
  (const Handle(IGESDimen_FlagNote)& another,
   const Handle(IGESDimen_FlagNote)& ent, Interface_CopyTool& TC) const
{
  gp_XYZ lowerLeft = (another->LowerLeftCorner()).XYZ();
  Standard_Real angle = another->Angle();
  DeclareAndCast(IGESDimen_GeneralNote, note,
		 TC.Transferred(another->Note()));
  Standard_Integer nbval = another->NbLeaders();

  Handle(IGESDimen_HArray1OfLeaderArrow) leaders;

  if ( nbval > 0 )
    {
      leaders = new IGESDimen_HArray1OfLeaderArrow(1, nbval);
      for (Standard_Integer i = 1; i <= nbval; i++)
	{
          DeclareAndCast(IGESDimen_LeaderArrow, new_ent, 
			 TC.Transferred(another->Leader(i)));
          leaders->SetValue(i, new_ent);
	}
    }
  ent->Init(lowerLeft, angle, note, leaders);
}

IGESData_DirChecker  IGESDimen_ToolFlagNote::DirChecker
  (const Handle(IGESDimen_FlagNote)& /* ent */ ) const 
{ 
  IGESData_DirChecker DC (208, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  return DC;
}

void  IGESDimen_ToolFlagNote::OwnCheck
  (const Handle(IGESDimen_FlagNote)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const 
{
}

void  IGESDimen_ToolFlagNote::OwnDump
  (const Handle(IGESDimen_FlagNote)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const 
{ 
  Standard_Integer sublevel = (level > 4) ? 1 : 0;

  S << "IGESDimen_FlagNote" << endl;
  S << "LowerLeftCorner : ";
  IGESData_DumpXYZL(S,level, ent->LowerLeftCorner(), ent->Location());
  S << endl << "Rotation Angle: " << ent->Angle() << endl;
  S << "General Note Entity : ";
  dumper.Dump(ent->Note(),S, sublevel);
  S << endl;
  S << "Number of Leaders : " << ent->NbLeaders() << "   Leaders : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbLeaders(),ent->Leader);
  S << endl;
}
