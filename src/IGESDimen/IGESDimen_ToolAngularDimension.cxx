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

#include <IGESDimen_ToolAngularDimension.ixx>
#include <IGESData_ParamCursor.hxx>
#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


IGESDimen_ToolAngularDimension::IGESDimen_ToolAngularDimension ()    {  }


void  IGESDimen_ToolAngularDimension::ReadOwnParams
  (const Handle(IGESDimen_AngularDimension)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  Handle(IGESDimen_GeneralNote) note; 
  Handle(IGESDimen_WitnessLine) firstWitness; 
  Handle(IGESDimen_WitnessLine) secondWitness; 
  gp_XY vertex ; 
  Standard_Real radius ;
  Handle(IGESDimen_LeaderArrow) firstLeader; 
  Handle(IGESDimen_LeaderArrow) secondLeader; 

  PR.ReadEntity(IR, PR.Current(), "General Note Entity",
		STANDARD_TYPE(IGESDimen_GeneralNote), note); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity (IR, PR.Current(), "First Witness Entity",
		 STANDARD_TYPE(IGESDimen_WitnessLine), firstWitness, Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity (IR,PR.Current(),"Second Witness Entity",
		 STANDARD_TYPE(IGESDimen_WitnessLine), secondWitness, Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadXY(PR.CurrentList(1, 2), "Vertex Point Co-ords", vertex); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadReal(PR.Current(), "Radius of Leader arcs", radius); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "First Leader Entity",
		STANDARD_TYPE(IGESDimen_LeaderArrow), firstLeader); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(),"Second Leader Entity",
		STANDARD_TYPE(IGESDimen_LeaderArrow), secondLeader); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (note, firstWitness, secondWitness, vertex, radius,
     firstLeader, secondLeader);
}

void  IGESDimen_ToolAngularDimension::WriteOwnParams
  (const Handle(IGESDimen_AngularDimension)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Note());
  IW.Send(ent->FirstWitnessLine());
  IW.Send(ent->SecondWitnessLine());
  IW.Send(ent->Vertex().X());
  IW.Send(ent->Vertex().Y());
  IW.Send(ent->Radius());
  IW.Send(ent->FirstLeader());
  IW.Send(ent->SecondLeader());
}

void  IGESDimen_ToolAngularDimension::OwnShared
  (const Handle(IGESDimen_AngularDimension)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Note());
  iter.GetOneItem(ent->FirstWitnessLine());
  iter.GetOneItem(ent->SecondWitnessLine());
  iter.GetOneItem(ent->FirstLeader());
  iter.GetOneItem(ent->SecondLeader());
}

void  IGESDimen_ToolAngularDimension::OwnCopy
  (const Handle(IGESDimen_AngularDimension)& another,
   const Handle(IGESDimen_AngularDimension)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESDimen_GeneralNote, note, 
                 TC.Transferred(another->Note()));
  DeclareAndCast(IGESDimen_WitnessLine, firstWitness, 
		 TC.Transferred(another->FirstWitnessLine()));
  DeclareAndCast(IGESDimen_WitnessLine, secondWitness, 
		 TC.Transferred(another->SecondWitnessLine()));
  gp_XY vertex = (another->Vertex()).XY();
  Standard_Real radius = another->Radius();
  DeclareAndCast(IGESDimen_LeaderArrow, firstLeader, 
		 TC.Transferred(another->FirstLeader()));
  DeclareAndCast(IGESDimen_LeaderArrow, secondLeader, 
		 TC.Transferred(another->SecondLeader()));

  ent->Init(note, firstWitness, secondWitness, vertex, radius,
	    firstLeader, secondLeader);
}

IGESData_DirChecker  IGESDimen_ToolAngularDimension::DirChecker
  (const Handle(IGESDimen_AngularDimension)& /* ent */ ) const 
{ 
  IGESData_DirChecker DC (202, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  return DC;
}

void  IGESDimen_ToolAngularDimension::OwnCheck
  (const Handle(IGESDimen_AngularDimension)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const 
{  }

void  IGESDimen_ToolAngularDimension::OwnDump
  (const Handle(IGESDimen_AngularDimension)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const 
{ 
  Standard_Integer sublevel  = (level > 4) ? 1 : 0;

  S << "IGESDimen_AngularDimension" << endl;
  S << "General Note Entity   : ";
  dumper.Dump(ent->Note(),S, sublevel);
  S << endl;
  S << "First  Witness Entity : ";
  dumper.Dump(ent->FirstWitnessLine(),S, sublevel);
  S << endl;
  S << "Second Witness Entity : ";
  dumper.Dump(ent->SecondWitnessLine(),S, sublevel);
  S << endl;
  S << "Vertex Point Co-ords  : ";
  IGESData_DumpXYL(S,level, ent->Vertex(), ent->Location());  S << endl;
  S << "Radius of Leader arcs : " << ent->Radius() << endl;
  S << "First  Leader Entity  : ";
  dumper.Dump(ent->FirstLeader(),S, sublevel);
  S << endl;
  S << "Second Leader Entity  : ";
  dumper.Dump(ent->SecondLeader(),S, sublevel);
  S << endl;
}

