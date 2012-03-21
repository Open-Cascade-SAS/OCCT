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

#include <IGESGraph_ToolLineFontDefTemplate.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESBasic_SubfigureDef.hxx>
#include <Interface_Macros.hxx>


IGESGraph_ToolLineFontDefTemplate::IGESGraph_ToolLineFontDefTemplate ()    {  }


void IGESGraph_ToolLineFontDefTemplate::ReadOwnParams
  (const Handle(IGESGraph_LineFontDefTemplate)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  Standard_Integer tempOrientation;
  Standard_Real    tempDistance, tempScale;
  Handle(IGESBasic_SubfigureDef) tempTemplateEntity;

  PR.ReadInteger(PR.Current(), "Template Orientation", tempOrientation); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(),
		"Subfigure Definition Entity for Template Display",
		STANDARD_TYPE(IGESBasic_SubfigureDef), tempTemplateEntity); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadReal(PR.Current(), "Distance between successive Template",
	      tempDistance); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadReal(PR.Current(), "Scale Factor For Subfigure", tempScale); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempOrientation, tempTemplateEntity, tempDistance, tempScale);
}

void IGESGraph_ToolLineFontDefTemplate::WriteOwnParams
  (const Handle(IGESGraph_LineFontDefTemplate)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->Orientation());
  IW.Send(ent->TemplateEntity());
  IW.Send(ent->Distance());
  IW.Send(ent->Scale());
}

void  IGESGraph_ToolLineFontDefTemplate::OwnShared
  (const Handle(IGESGraph_LineFontDefTemplate)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->TemplateEntity());
}

void IGESGraph_ToolLineFontDefTemplate::OwnCopy
  (const Handle(IGESGraph_LineFontDefTemplate)& another,
   const Handle(IGESGraph_LineFontDefTemplate)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer tempOrientation = another->Orientation();
  DeclareAndCast(IGESBasic_SubfigureDef, tempTemplateSubfigure,
                 TC.Transferred(another->TemplateEntity()));
  Standard_Real tempDistance = another->Distance();
  Standard_Real tempScale = another->Scale();

  ent->Init(tempOrientation, tempTemplateSubfigure, tempDistance, tempScale);
}

IGESData_DirChecker IGESGraph_ToolLineFontDefTemplate::DirChecker
  (const Handle(IGESGraph_LineFontDefTemplate)& /*ent*/)  const
{
  IGESData_DirChecker DC(304, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefValue);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();

  return DC;
}

void IGESGraph_ToolLineFontDefTemplate::OwnCheck
  (const Handle(IGESGraph_LineFontDefTemplate)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if (ent->RankLineFont() == 0)
    ach->AddWarning("Line Font Rank is zero");
  else if ((ent->RankLineFont() < 1) || (ent->RankLineFont() > 5))
    ach->AddWarning("Invalid Value As Line Font Rank");
}

void IGESGraph_ToolLineFontDefTemplate::OwnDump
  (const Handle(IGESGraph_LineFontDefTemplate)& ent, const IGESData_IGESDumper& dumper,
  const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer tempSubLevel = (level <= 4) ? 0 : 1;

  S << "IGESGraph_LineFontDefTemplate" << endl;

  S << "Orientation : " << ent->Orientation() << endl;
  S << "Subfigure Display Entity For Template Display : ";
  dumper.Dump(ent->TemplateEntity(),S, tempSubLevel);
  S << endl;
  S << "Length Between Successive Template Figure : " << ent->Distance()<<endl;
  S << "Scale Factor for Subfigure : " << ent->Scale() << endl;
  S << endl;
}

