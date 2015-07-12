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
#include <IGESData_Dump.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_GeneralSymbol.hxx>
#include <IGESDimen_HArray1OfLeaderArrow.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_ToolGeneralSymbol.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESDimen_ToolGeneralSymbol::IGESDimen_ToolGeneralSymbol ()    {  }


void  IGESDimen_ToolGeneralSymbol::ReadOwnParams
  (const Handle(IGESDimen_GeneralSymbol)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer i, num;
  Handle(IGESData_HArray1OfIGESEntity) tempGeoms;
  Handle(IGESDimen_HArray1OfLeaderArrow) tempLeaders;
  Handle(IGESDimen_GeneralNote) tempNote;

  if ((ent->FormNumber() == 0) && (!PR.IsParamEntity(PR.CurrentNumber())))
    PR.SetCurrentNumber(PR.CurrentNumber()+1);
  else
    PR.ReadEntity(IR, PR.Current(), "General Note Entity",
		  STANDARD_TYPE(IGESDimen_GeneralNote), tempNote); //szv#4:S4163:12Mar99 `st=` not needed

  Standard_Boolean st = PR.ReadInteger(PR.Current(), "Number of Geometries", num);
  if (!st || num <= 0) PR.AddFail("Number of Geometries: Not Positive");
  if (num > 0) {
    PR.ReadEnts (IR,PR.CurrentList(num),"Geometry Entities",tempGeoms); //szv#4:S4163:12Mar99 `st=` not needed
/*
    tempGeoms = new IGESData_HArray1OfIGESEntity(1, num);
    for ( i = 1; i <= num; i++)
      {
	Handle(IGESData_IGESEntity) tempEnt;
	st = PR.ReadEntity(IR, PR.Current(), "Geometry Entity", tempEnt);
	if (st) tempGeoms->SetValue(i, tempEnt);
      }
*/
  }
  else {
    return;
  }
  st = PR.ReadInteger(PR.Current(), "Number of Leaders", num);
  if (st && num > 0)  tempLeaders = new IGESDimen_HArray1OfLeaderArrow(1, num);
  else if (num < 0)  PR.AddFail("Number of Leaders: Less than zero");
  if (!tempLeaders.IsNull())
    for ( i = 1; i <= num; i++)
      {
	Handle(IGESDimen_LeaderArrow) tempEnt;
	//st = PR.ReadEntity(IR, PR.Current(), "Leader Entity",
			     //STANDARD_TYPE(IGESDimen_LeaderArrow), tempEnt); //szv#4:S4163:12Mar99 moved in if
	if (PR.ReadEntity(IR, PR.Current(), "Leader Entity", STANDARD_TYPE(IGESDimen_LeaderArrow), tempEnt))
	  tempLeaders->SetValue(i, tempEnt);
      }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempNote, tempGeoms, tempLeaders);
}

void  IGESDimen_ToolGeneralSymbol::WriteOwnParams
  (const Handle(IGESDimen_GeneralSymbol)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i, num;
  IW.Send(ent->Note());
  IW.Send(ent->NbGeomEntities());
  for (num = ent->NbGeomEntities(), i = 1; i <= num; i++)
    IW.Send(ent->GeomEntity(i));
  IW.Send(ent->NbLeaders());
  for (num = ent->NbLeaders(), i = 1; i <= num; i++)
    IW.Send(ent->LeaderArrow(i));
}

void  IGESDimen_ToolGeneralSymbol::OwnShared
  (const Handle(IGESDimen_GeneralSymbol)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer i, num;
  iter.GetOneItem(ent->Note());
  for (num = ent->NbGeomEntities(), i = 1; i <= num; i++)
    iter.GetOneItem(ent->GeomEntity(i));
  for (num = ent->NbLeaders(), i = 1; i <= num; i++)
    iter.GetOneItem(ent->LeaderArrow(i));
}

void  IGESDimen_ToolGeneralSymbol::OwnCopy
  (const Handle(IGESDimen_GeneralSymbol)& another,
   const Handle(IGESDimen_GeneralSymbol)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESDimen_GeneralNote, tempNote,
		 TC.Transferred(another->Note()));
  Standard_Integer num = another->NbGeomEntities();
  Handle(IGESData_HArray1OfIGESEntity) tempGeoms =
    new IGESData_HArray1OfIGESEntity(1, num);
  Standard_Integer i;
  for (i = 1; i <= num; i++)
    {
      DeclareAndCast(IGESData_IGESEntity, new_item,
		     TC.Transferred(another->GeomEntity(i)));
      tempGeoms->SetValue(i, new_item);
    }
  Handle(IGESDimen_HArray1OfLeaderArrow) tempLeaders;
  num = another->NbLeaders();
  if (num > 0)
    {
      tempLeaders = new IGESDimen_HArray1OfLeaderArrow(1, num);
      for (i = 1; i <= num; i++)
	{
          DeclareAndCast(IGESDimen_LeaderArrow, new_item,
			 TC.Transferred(another->LeaderArrow(i)));
          tempLeaders->SetValue(i, new_item);
	}
    }
  ent->Init (tempNote, tempGeoms, tempLeaders);
  ent->SetFormNumber (another->FormNumber());
}

IGESData_DirChecker  IGESDimen_ToolGeneralSymbol::DirChecker
  (const Handle(IGESDimen_GeneralSymbol)& /* ent */ ) const
{
  IGESData_DirChecker DC(228, 0, 9999);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  return DC;
}

void  IGESDimen_ToolGeneralSymbol::OwnCheck
  (const Handle(IGESDimen_GeneralSymbol)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if ( (ent->FormNumber() <    0 || ent->FormNumber() >    3) &&
      ( ent->FormNumber() < 5001 || ent->FormNumber() > 9999 ) )
    ach->AddFail("Invalid Form Number");
  if (ent->FormNumber() != 0)   if (ent->Note().IsNull())
    ach->AddFail("No General Note defined for form number non 0");
}

void  IGESDimen_ToolGeneralSymbol::OwnDump
  (const Handle(IGESDimen_GeneralSymbol)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  Standard_Integer sublevel = (level > 4) ? 1 : 0;
  S << "IGESDimen_GeneralSymbol" << endl;
  S << "General Note : ";
  dumper.Dump(ent->Note(),S, sublevel);
  S << endl << "Geometric Entities : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbGeomEntities(),ent->GeomEntity);
  S << endl << "Leader Arrows : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbLeaders(),ent->LeaderArrow);
  S << endl;
}
