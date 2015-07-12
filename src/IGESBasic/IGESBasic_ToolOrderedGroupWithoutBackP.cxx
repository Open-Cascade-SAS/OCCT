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

#include <IGESBasic_OrderedGroupWithoutBackP.hxx>
#include <IGESBasic_ToolOrderedGroupWithoutBackP.hxx>
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
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESBasic_ToolOrderedGroupWithoutBackP::IGESBasic_ToolOrderedGroupWithoutBackP()
      {  }


void  IGESBasic_ToolOrderedGroupWithoutBackP::ReadOwnParams
  (const Handle(IGESBasic_OrderedGroupWithoutBackP)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer nbval = 0;
  Handle(IGESData_HArray1OfIGESEntity)  EntArray;

  if (PR.ReadInteger( PR.Current(), "Count of Entities", nbval)) { //szv#4:S4163:12Mar99 `st=` not needed
    PR.ReadEnts (IR,PR.CurrentList(nbval),"Entities",EntArray); //szv#4:S4163:12Mar99 `st=` not needed
/*
    EntArray = new IGESData_HArray1OfIGESEntity(1,nbval);
    for (Standard_Integer i = 1;i <= nbval;i++)
      {
	Handle(IGESData_IGESEntity) anent;
	st = PR.ReadEntity (IR,PR.Current(),"Element of the Group",anent,Standard_True);
	if (st) EntArray->SetValue(i,anent);
      }
*/
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(EntArray);
}

void  IGESBasic_ToolOrderedGroupWithoutBackP::WriteOwnParams
  (const Handle(IGESBasic_OrderedGroupWithoutBackP)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer upper = ent->NbEntities();
  IW.Send(ent->NbEntities());
  for (Standard_Integer i = 1; i <= upper; i ++)
    IW.Send(ent->Entity(i));
}

void  IGESBasic_ToolOrderedGroupWithoutBackP::OwnShared
  (const Handle(IGESBasic_OrderedGroupWithoutBackP)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer upper = ent->NbEntities();
  for (Standard_Integer i = 1; i <= upper; i ++)
    iter.GetOneItem(ent->Entity(i));
}

void  IGESBasic_ToolOrderedGroupWithoutBackP::OwnCopy
  (const Handle(IGESBasic_OrderedGroupWithoutBackP)& another,
   const Handle(IGESBasic_OrderedGroupWithoutBackP)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer lower,upper;
  lower = 1;
  upper = another->NbEntities();
  Handle(IGESData_HArray1OfIGESEntity)  EntArray = new
    IGESData_HArray1OfIGESEntity(lower,upper);
  for (Standard_Integer i = lower;i <= upper;i++)
    {
      DeclareAndCast
	(IGESData_IGESEntity,myentity,TC.Transferred(another->Entity(i)));
      EntArray->SetValue(i,myentity);
    }
  ent->Init(EntArray);
}

Standard_Boolean IGESBasic_ToolOrderedGroupWithoutBackP::OwnCorrect
  (const Handle(IGESBasic_OrderedGroupWithoutBackP)& ent )  const
{
  Standard_Integer ianul = 0;
  Standard_Integer i, nbtrue = 0, nb = ent->NbEntities();
  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) val = ent->Entity(i);
    if (val.IsNull()) ianul ++;
    else if (val->TypeNumber() == 0) ianul ++;
  }
  if (ianul == 0) return Standard_False;
  Handle(IGESData_HArray1OfIGESEntity)  EntArray;
  if (ianul < nb) EntArray = new IGESData_HArray1OfIGESEntity(1,nb-ianul);
  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) val = ent->Entity(i);
    if (val.IsNull()) continue;
    else if (val->TypeNumber() == 0) continue;
    nbtrue ++;
    EntArray->SetValue (nbtrue,ent->Entity(i));
  }
  ent->Init(EntArray);
  return Standard_True;
}

IGESData_DirChecker IGESBasic_ToolOrderedGroupWithoutBackP::DirChecker
  (const Handle(IGESBasic_OrderedGroupWithoutBackP)& /* ent */) const
{
  IGESData_DirChecker DC(402,15);  //TypeNo. 402, Form no. 15
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESBasic_ToolOrderedGroupWithoutBackP::OwnCheck
  (const Handle(IGESBasic_OrderedGroupWithoutBackP)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  Standard_Boolean ianul = Standard_False;
  Standard_Integer i, nb = ent->NbEntities();
  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) val = ent->Entity(i);
    if (val.IsNull()) ianul = Standard_True;
    else if (val->TypeNumber() == 0) ianul = Standard_True;
    if (ianul) {
      ach->AddWarning ("At least one element is Null");
      break;
    }
  }
}

void  IGESBasic_ToolOrderedGroupWithoutBackP::OwnDump
  (const Handle(IGESBasic_OrderedGroupWithoutBackP)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESBasic_OrderedGroupWithoutBackP" << endl;

  S << "Entries in the Group : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbEntities(),ent->Entity);
  S << endl;
}

