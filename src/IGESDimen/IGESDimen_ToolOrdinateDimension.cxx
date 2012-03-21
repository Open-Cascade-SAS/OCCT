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

#include <IGESDimen_ToolOrdinateDimension.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <Interface_Macros.hxx>


IGESDimen_ToolOrdinateDimension::IGESDimen_ToolOrdinateDimension ()    {  }


void IGESDimen_ToolOrdinateDimension::ReadOwnParams
  (const Handle(IGESDimen_OrdinateDimension)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Handle(IGESDimen_GeneralNote) tempNote;
  Handle(IGESDimen_WitnessLine) witLine;
  Handle(IGESDimen_LeaderArrow) leadArr;
  Standard_Boolean isLine=Standard_False;

  PR.ReadEntity(IR,PR.Current(),"General Note",
		STANDARD_TYPE(IGESDimen_GeneralNote),tempNote);

  if (ent->FormNumber() == 0)
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

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init ( tempNote, isLine, witLine, leadArr);
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
