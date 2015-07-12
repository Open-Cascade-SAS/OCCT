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
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_PointDimension.hxx>
#include <IGESDimen_ToolPointDimension.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESDimen_ToolPointDimension::IGESDimen_ToolPointDimension ()    {  }


void IGESDimen_ToolPointDimension::ReadOwnParams
  (const Handle(IGESDimen_PointDimension)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Handle(IGESDimen_GeneralNote) tempNote;
  Handle(IGESDimen_LeaderArrow) leadArr;
  Handle(IGESData_IGESEntity) tempGeom;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadEntity(IR, PR.Current(), "General Note",
		STANDARD_TYPE(IGESDimen_GeneralNote), tempNote); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "Leader",
		STANDARD_TYPE(IGESDimen_LeaderArrow), leadArr); //szv#4:S4163:12Mar99 `st=` not needed

  if (PR.IsParamEntity(PR.CurrentNumber()))
    PR.ReadEntity(IR, PR.Current(), "Enclosing entity", tempGeom); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNote, leadArr, tempGeom);
}

void IGESDimen_ToolPointDimension::WriteOwnParams
  (const Handle(IGESDimen_PointDimension)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Note());
  IW.Send(ent->LeaderArrow());
  IW.Send(ent->Geom());
}

void  IGESDimen_ToolPointDimension::OwnShared
  (const Handle(IGESDimen_PointDimension)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Note());
  iter.GetOneItem(ent->LeaderArrow());
  iter.GetOneItem(ent->Geom());
}

void IGESDimen_ToolPointDimension::OwnCopy
  (const Handle(IGESDimen_PointDimension)& another,
   const Handle(IGESDimen_PointDimension)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESDimen_GeneralNote, tempNote,
		 TC.Transferred(another->Note()));
  DeclareAndCast(IGESDimen_LeaderArrow, tempArrow,
		 TC.Transferred(another->LeaderArrow()));
  DeclareAndCast(IGESData_IGESEntity, tempGeom,
		 TC.Transferred(another->Geom()));
  ent->Init(tempNote, tempArrow, tempGeom);
}

IGESData_DirChecker IGESDimen_ToolPointDimension::DirChecker
  (const Handle(IGESDimen_PointDimension)& /*ent*/) const
{
  IGESData_DirChecker DC(220, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);

  DC.UseFlagRequired (1);

  return DC;
}

void IGESDimen_ToolPointDimension::OwnCheck
  (const Handle(IGESDimen_PointDimension)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/) const
{
}

void IGESDimen_ToolPointDimension::OwnDump
  (const Handle(IGESDimen_PointDimension)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESDimen_PointDimension" << endl;
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "General Note : ";
  dumper.Dump(ent->Note(),S , sublevel);
  S << endl;
  S << "Leader Arrow : ";
  dumper.Dump(ent->LeaderArrow(),S , sublevel);
  S << endl;
  if (!ent->Geom().IsNull())
    {
      S << "Enclosing Entity : ";
      dumper.Dump(ent->Geom(),S , sublevel);
      S << endl;
    }
}
