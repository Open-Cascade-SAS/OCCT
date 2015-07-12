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

#include <IGESBasic_ExternalRefFileIndex.hxx>
#include <IGESBasic_ToolExternalRefFileIndex.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>

IGESBasic_ToolExternalRefFileIndex::IGESBasic_ToolExternalRefFileIndex ()  {  }


void  IGESBasic_ToolExternalRefFileIndex::ReadOwnParams
  (const Handle(IGESBasic_ExternalRefFileIndex)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer num, i;
  Handle(Interface_HArray1OfHAsciiString) tempNames;
  Handle(IGESData_HArray1OfIGESEntity) tempEntities;
  Standard_Boolean st = PR.ReadInteger(PR.Current(), "Number of index entries", num);
  if (st && num > 0)
    {
      tempNames = new Interface_HArray1OfHAsciiString(1, num);
      tempEntities = new IGESData_HArray1OfIGESEntity(1, num);
    }
  else  PR.AddFail("Number of index entries: Not Positive");
  if (!tempNames.IsNull() && !tempEntities.IsNull())
    for ( i = 1; i <= num; i++ )
      {
	Handle(TCollection_HAsciiString) tempNam;
	if (PR.ReadText(PR.Current(), "External Reference Entity", tempNam)) //szv#4:S4163:12Mar99 `st=` not needed
	  tempNames->SetValue(i, tempNam);
	Handle(IGESData_IGESEntity) tempEnt;
	if (PR.ReadEntity(IR, PR.Current(), "Internal Entity", tempEnt)) //szv#4:S4163:12Mar99 `st=` not needed
	  tempEntities->SetValue(i, tempEnt);
      }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNames, tempEntities);
}

void  IGESBasic_ToolExternalRefFileIndex::WriteOwnParams
  (const Handle(IGESBasic_ExternalRefFileIndex)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i, num;
  IW.Send(ent->NbEntries());
  for ( num = ent->NbEntries(), i = 1; i <= num; i++ )
    {
      IW.Send(ent->Name(i));
      IW.Send(ent->Entity(i));
    }
}

void  IGESBasic_ToolExternalRefFileIndex::OwnShared
  (const Handle(IGESBasic_ExternalRefFileIndex)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer i, num;
  for ( num = ent->NbEntries(), i = 1; i <= num; i++ )
    iter.GetOneItem(ent->Entity(i));
}

void  IGESBasic_ToolExternalRefFileIndex::OwnCopy
  (const Handle(IGESBasic_ExternalRefFileIndex)& another,
   const Handle(IGESBasic_ExternalRefFileIndex)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer num = another->NbEntries();
  Handle(Interface_HArray1OfHAsciiString) tempNames =
    new Interface_HArray1OfHAsciiString(1, num);
  Handle(IGESData_HArray1OfIGESEntity) tempEntities =
    new IGESData_HArray1OfIGESEntity(1, num);
  for ( Standard_Integer i = 1; i <= num; i++ )
    {
      tempNames->SetValue(i, new TCollection_HAsciiString
			  (another->Name(i)));
      DeclareAndCast(IGESData_IGESEntity, new_item,
		     TC.Transferred(another->Entity(i)));
      tempEntities->SetValue(i, new_item);
    }
  ent->Init(tempNames, tempEntities);
}

IGESData_DirChecker  IGESBasic_ToolExternalRefFileIndex::DirChecker
  (const Handle(IGESBasic_ExternalRefFileIndex)& /* ent */ ) const
{
  IGESData_DirChecker DC(402, 12);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESBasic_ToolExternalRefFileIndex::OwnCheck
  (const Handle(IGESBasic_ExternalRefFileIndex)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void  IGESBasic_ToolExternalRefFileIndex::OwnDump
  (const Handle(IGESBasic_ExternalRefFileIndex)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  Standard_Integer i, num;
  S << "IGESBasic_ExternalRefFileIndex" << endl;
  S << "External Reference Names : " << endl;
  S << "Internal Entities : ";
  IGESData_DumpEntities(S,dumper,-level,1, ent->NbEntries(),ent->Entity);
  S << endl;
  if (level > 4)
    for ( num = ent->NbEntries(), i = 1; i <= num; i++ )
      {
	S << "[" << i << "]: ";
	S << "External Reference Name : ";
	IGESData_DumpString(S,ent->Name(i));
	S << "  Internal Entity : ";
	dumper.Dump (ent->Entity(i),S, 1);
	S << endl;
      }
  S << endl;
}
