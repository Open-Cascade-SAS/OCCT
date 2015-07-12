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

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESSolid_BooleanTree.hxx>
#include <IGESSolid_SelectedComponent.hxx>
#include <IGESSolid_ToolSelectedComponent.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

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
