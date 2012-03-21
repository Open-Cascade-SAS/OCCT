// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDimen_ToolCurveDimension.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <IGESGeom_Line.hxx>
#include <Interface_Macros.hxx>


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
