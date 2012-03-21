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

#include <IGESSolid_ToolSolidInstance.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>
#include <Message_Messenger.hxx>

IGESSolid_ToolSolidInstance::IGESSolid_ToolSolidInstance ()    {  }


void  IGESSolid_ToolSolidInstance::ReadOwnParams
  (const Handle(IGESSolid_SolidInstance)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Handle(IGESData_IGESEntity) tempEntity;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadEntity(IR, PR.Current(), "Solid Entity", tempEntity); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempEntity);
}

void  IGESSolid_ToolSolidInstance::WriteOwnParams
  (const Handle(IGESSolid_SolidInstance)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Entity());
}

void  IGESSolid_ToolSolidInstance::OwnShared
  (const Handle(IGESSolid_SolidInstance)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Entity());
}

void  IGESSolid_ToolSolidInstance::OwnCopy
  (const Handle(IGESSolid_SolidInstance)& another,
   const Handle(IGESSolid_SolidInstance)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESData_IGESEntity, tempEntity,
		 TC.Transferred(another->Entity()));
  ent->Init (tempEntity);
}

IGESData_DirChecker  IGESSolid_ToolSolidInstance::DirChecker
  (const Handle(IGESSolid_SolidInstance)& /*ent*/) const
{
  IGESData_DirChecker DC(430, 0,1);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.GraphicsIgnored (1);
  return DC;
}

void  IGESSolid_ToolSolidInstance::OwnCheck
  (const Handle(IGESSolid_SolidInstance)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/) const
{
}

void  IGESSolid_ToolSolidInstance::OwnDump
  (const Handle(IGESSolid_SolidInstance)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_SolidInstance" << endl;

  S << "Solid entity : ";
  dumper.Dump(ent->Entity(),S, (level <= 4) ? 0 : 1);
  S << endl;
}
