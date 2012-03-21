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

#include <IGESDimen_ToolDiameterDimension.ixx>
#include <IGESData_ParamCursor.hxx>
#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


IGESDimen_ToolDiameterDimension::IGESDimen_ToolDiameterDimension ()    {  }


void  IGESDimen_ToolDiameterDimension::ReadOwnParams
  (const Handle(IGESDimen_DiameterDimension)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  Handle(IGESDimen_GeneralNote) note; 
  Handle(IGESDimen_LeaderArrow) firstLeader; 
  Handle(IGESDimen_LeaderArrow) secondLeader; 
  gp_XY center;

  PR.ReadEntity(IR, PR.Current(), "General Note Entity",
		STANDARD_TYPE(IGESDimen_GeneralNote), note); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "First Leader Entity",
		STANDARD_TYPE(IGESDimen_LeaderArrow), firstLeader); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity (IR,PR.Current(),"Second Leader Entity",
		 STANDARD_TYPE(IGESDimen_LeaderArrow), secondLeader, Standard_True); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadXY(PR.CurrentList(1, 2), "Arc Center Co-ords", center); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(note, firstLeader, secondLeader, center);
}

void  IGESDimen_ToolDiameterDimension::WriteOwnParams
  (const Handle(IGESDimen_DiameterDimension)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Note());
  IW.Send(ent->FirstLeader());
  IW.Send(ent->SecondLeader());
  IW.Send(ent->Center().X());
  IW.Send(ent->Center().Y());
}

void  IGESDimen_ToolDiameterDimension::OwnShared
  (const Handle(IGESDimen_DiameterDimension)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Note());
  iter.GetOneItem(ent->FirstLeader());
  iter.GetOneItem(ent->SecondLeader());
}

void  IGESDimen_ToolDiameterDimension::OwnCopy
  (const Handle(IGESDimen_DiameterDimension)& another,
   const Handle(IGESDimen_DiameterDimension)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESDimen_GeneralNote, note, 
		 TC.Transferred(another->Note()));
  DeclareAndCast(IGESDimen_LeaderArrow, firstLeader, 
		 TC.Transferred(another->FirstLeader()));
  DeclareAndCast(IGESDimen_LeaderArrow, secondLeader, 
		 TC.Transferred(another->SecondLeader()));
  gp_XY center = (another->Center()).XY();

  ent->Init(note, firstLeader, secondLeader, center);
}

IGESData_DirChecker  IGESDimen_ToolDiameterDimension::DirChecker
  (const Handle(IGESDimen_DiameterDimension)& /* ent */) const 
{ 
  IGESData_DirChecker DC (206, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  return DC;
}

void  IGESDimen_ToolDiameterDimension::OwnCheck
  (const Handle(IGESDimen_DiameterDimension)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const 
{
}

void  IGESDimen_ToolDiameterDimension::OwnDump
  (const Handle(IGESDimen_DiameterDimension)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const 
{ 
  Standard_Integer sublevel = (level > 4) ? 1 : 0;

  S << "IGESDimen_DiameterDimension" << endl;
  S << "General Note Entity : ";
  dumper.Dump(ent->Note(),S, sublevel);
  S << endl;
  S << "First  Leader Entity : ";
  dumper.Dump(ent->FirstLeader(),S, sublevel);
  S << endl;
  S << "Second Leader Entity : ";
  dumper.Dump(ent->SecondLeader(),S, sublevel);
  S << endl;
  S << "Center Point : ";
  IGESData_DumpXYL(S,level, ent->Center(), ent->Location());
  S << endl;
}

