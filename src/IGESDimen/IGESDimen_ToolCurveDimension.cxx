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
#include <IGESDimen_CurveDimension.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_ToolCurveDimension.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <IGESGeom_Line.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESDimen_ToolCurveDimension::IGESDimen_ToolCurveDimension ()    {  }


void  IGESDimen_ToolCurveDimension::ReadOwnParams
  (const Handle(IGESDimen_CurveDimension)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  Handle(IGESDimen_GeneralNote) note; 
  Handle(IGESData_IGESEntity) firstCurve;
  Handle(IGESData_IGESEntity) secondCurve;
  Handle(IGESDimen_LeaderArrow) firstLeader; 
  Handle(IGESDimen_LeaderArrow) secondLeader; 
  Handle(IGESDimen_WitnessLine) firstWitness; 
  Handle(IGESDimen_WitnessLine) secondWitness; 

  PR.ReadEntity(IR, PR.Current(), "General Note Entity",
		STANDARD_TYPE(IGESDimen_GeneralNote), note); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "First Curve Entity", firstCurve); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR,PR.Current(),"Second Curve Entity",
		secondCurve,Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "First Leader Entity",
		STANDARD_TYPE(IGESDimen_LeaderArrow), firstLeader); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(),"Second Leader Entity",
		STANDARD_TYPE(IGESDimen_LeaderArrow), secondLeader); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity (IR, PR.Current(), "First Witness Entity",
		 STANDARD_TYPE(IGESDimen_WitnessLine), firstWitness, Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity (IR,PR.Current(),"Second Witness Entity",
		 STANDARD_TYPE(IGESDimen_WitnessLine), secondWitness, Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (note, firstCurve, secondCurve, firstLeader, secondLeader, 
     firstWitness, secondWitness);
}

void  IGESDimen_ToolCurveDimension::WriteOwnParams
  (const Handle(IGESDimen_CurveDimension)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Note());
  IW.Send(ent->FirstCurve());
  IW.Send(ent->SecondCurve());
  IW.Send(ent->FirstLeader());
  IW.Send(ent->SecondLeader());
  IW.Send(ent->FirstWitnessLine());
  IW.Send(ent->SecondWitnessLine());
}

void  IGESDimen_ToolCurveDimension::OwnShared
  (const Handle(IGESDimen_CurveDimension)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Note());
  iter.GetOneItem(ent->FirstCurve());
  iter.GetOneItem(ent->SecondCurve());
  iter.GetOneItem(ent->FirstLeader());
  iter.GetOneItem(ent->SecondLeader());
  iter.GetOneItem(ent->FirstWitnessLine());
  iter.GetOneItem(ent->SecondWitnessLine());
}

void  IGESDimen_ToolCurveDimension::OwnCopy
  (const Handle(IGESDimen_CurveDimension)& another,
   const Handle(IGESDimen_CurveDimension)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESDimen_GeneralNote, note, 
		 TC.Transferred(another->Note()));
  DeclareAndCast(IGESData_IGESEntity, firstCurve, 
		 TC.Transferred(another->FirstCurve()));
  DeclareAndCast(IGESData_IGESEntity, secondCurve, 
		 TC.Transferred(another->SecondCurve()));
  DeclareAndCast(IGESDimen_LeaderArrow, firstLeader, 
		 TC.Transferred(another->FirstLeader()));
  DeclareAndCast(IGESDimen_LeaderArrow, secondLeader, 
		 TC.Transferred(another->SecondLeader()));
  DeclareAndCast(IGESDimen_WitnessLine, firstWitness, 
		 TC.Transferred(another->FirstWitnessLine()));
  DeclareAndCast(IGESDimen_WitnessLine, secondWitness, 
		 TC.Transferred(another->SecondWitnessLine()));

  ent->Init(note, firstCurve, secondCurve, firstLeader, secondLeader, 
	    firstWitness, secondWitness);
}

IGESData_DirChecker  IGESDimen_ToolCurveDimension::DirChecker
  (const Handle(IGESDimen_CurveDimension)& /*ent*/) const 
{ 
  IGESData_DirChecker DC (204, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  return DC;
}

void  IGESDimen_ToolCurveDimension::OwnCheck
  (const Handle(IGESDimen_CurveDimension)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const 
{
  if (ent->HasSecondCurve())
    {
      if ( ent->FirstCurve()->IsKind(STANDARD_TYPE(IGESGeom_Line)) )
	if ( ent->SecondCurve()->IsKind(STANDARD_TYPE(IGESGeom_Line)) )
	  ach->AddWarning("Both curves are IGESGeom_Line Entities");
    }
}

void  IGESDimen_ToolCurveDimension::OwnDump
  (const Handle(IGESDimen_CurveDimension)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const 
{ 
  Standard_Integer sublevel = (level > 4) ? 1 : 0;

  S << "IGESDimen_CurveDimension" << endl;
  S << "General Note Entity   : ";
  dumper.Dump(ent->Note(),S, sublevel);
  S << endl;
  S << "First  Curve   Entity : ";
  dumper.Dump(ent->FirstCurve(),S, sublevel);
  S << endl;
  S << "Second Curve   Entity : ";
  dumper.Dump(ent->SecondCurve(),S, sublevel);
  S << endl;
  S << "First  Leader  Entity : ";
  dumper.Dump(ent->FirstLeader(),S, sublevel);
  S << endl;
  S << "Second Leader  Entity : ";
  dumper.Dump(ent->SecondLeader(),S, sublevel);
  S << endl;
  S << "First  Witness Entity : ";
  dumper.Dump(ent->FirstWitnessLine(),S, sublevel);
  S << endl;
  S << "Second Witness Entity : ";
  dumper.Dump(ent->SecondWitnessLine(),S, sublevel);
  S << endl;
}
