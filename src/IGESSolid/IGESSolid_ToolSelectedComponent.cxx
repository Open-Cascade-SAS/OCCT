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

#include <IGESSolid_ToolSelectedComponent.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESSolid_BooleanTree.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


IGESSolid_ToolSelectedComponent::IGESSolid_ToolSelectedComponent ()    {  }


void  IGESSolid_ToolSelectedComponent::ReadOwnParams
  (const Handle(IGESSolid_SelectedComponent)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Handle(IGESSolid_BooleanTree) tempEntity;
  gp_XYZ tempSelectPoint;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadEntity(IR, PR.Current(), "Boolean Tree Entity",
		STANDARD_TYPE(IGESSolid_BooleanTree), tempEntity); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadXYZ(PR.CurrentList(1, 3), "Select Point", tempSelectPoint); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempEntity, tempSelectPoint);
}

void  IGESSolid_ToolSelectedComponent::WriteOwnParams
  (const Handle(IGESSolid_SelectedComponent)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Component());
  IW.Send(ent->SelectPoint().X());
  IW.Send(ent->SelectPoint().Y());
  IW.Send(ent->SelectPoint().Z());
}

void  IGESSolid_ToolSelectedComponent::OwnShared
  (const Handle(IGESSolid_SelectedComponent)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Component());
}

void  IGESSolid_ToolSelectedComponent::OwnCopy
  (const Handle(IGESSolid_SelectedComponent)& another,
   const Handle(IGESSolid_SelectedComponent)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESSolid_BooleanTree, tempEntity,
		 TC.Transferred(another->Component()));
  gp_XYZ tempSelectPoint = another->SelectPoint().XYZ();
  ent->Init (tempEntity, tempSelectPoint);
}

IGESData_DirChecker  IGESSolid_ToolSelectedComponent::DirChecker
  (const Handle(IGESSolid_SelectedComponent)& /* ent */ ) const
{
  IGESData_DirChecker DC(182, 0);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefVoid);
  DC.LineWeight (IGESData_DefVoid);
  DC.Color      (IGESData_DefAny);

  DC.BlankStatusIgnored ();
  DC.UseFlagRequired (3);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void  IGESSolid_ToolSelectedComponent::OwnCheck
  (const Handle(IGESSolid_SelectedComponent)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void  IGESSolid_ToolSelectedComponent::OwnDump
  (const Handle(IGESSolid_SelectedComponent)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_SelectedComponent" << endl;

  // the heading for boolean tree is in BooleanTree OwnDump
  S << "Boolean Tree Entity : " << endl;
  dumper.Dump(ent->Component(),S, (level <= 4) ? 0 : 1);
  S << "Seleted Point       : ";
  IGESData_DumpXYZL(S,level, ent->SelectPoint(), ent->Location());
  S << endl;
}
