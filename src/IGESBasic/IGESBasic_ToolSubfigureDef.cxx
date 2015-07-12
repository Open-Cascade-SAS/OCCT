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

#include <IGESBasic_SubfigureDef.hxx>
#include <IGESBasic_ToolSubfigureDef.hxx>
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
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>

// MGE 03/08/98
IGESBasic_ToolSubfigureDef::IGESBasic_ToolSubfigureDef ()    {  }


void  IGESBasic_ToolSubfigureDef::ReadOwnParams
  (const Handle(IGESBasic_SubfigureDef)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
//  Message_Msg Msg208("XSTEP_208");
  Message_Msg Msg209("XSTEP_209");
//  Message_Msg Msg210("XSTEP_210");
//  Message_Msg Msg211("XSTEP_211");
  //========================================

  Standard_Integer tempDepth;
  Handle(TCollection_HAsciiString) tempName;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer nbval = 0;
  Handle(IGESData_HArray1OfIGESEntity)  EntArray;

  if(!PR.ReadInteger(PR.Current(),tempDepth)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg208("XSTEP_208");
    PR.SendFail(Msg208);
  }
  PR.ReadText(PR.Current(),Msg209,tempName); //szv#4:S4163:12Mar99 `st=` not needed
  if (PR.ReadInteger( PR.Current(), nbval)) { //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg211("XSTEP_211");
    PR.ReadEnts (IR,PR.CurrentList(nbval),Msg211,EntArray); //szv#4:S4163:12Mar99 `st=` not needed
  }
  else{
    Message_Msg Msg210("XSTEP_210");
    PR.SendFail(Msg210);
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempDepth,tempName,EntArray);
}

void  IGESBasic_ToolSubfigureDef::WriteOwnParams
  (const Handle(IGESBasic_SubfigureDef)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer nb = ent->NbEntities();

  IW.Send(ent->Depth());
  IW.Send(ent->Name());
  IW.Send(nb);
  for (Standard_Integer i = 1; i <= nb; i ++)
    IW.Send(ent->AssociatedEntity(i));
}

void  IGESBasic_ToolSubfigureDef::OwnShared
  (const Handle(IGESBasic_SubfigureDef)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer nb = ent->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++)
    iter.GetOneItem(ent->AssociatedEntity(i));
}

void  IGESBasic_ToolSubfigureDef::OwnCopy
  (const Handle(IGESBasic_SubfigureDef)& another,
   const Handle(IGESBasic_SubfigureDef)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer lower,upper;
  Standard_Integer aDepth = another->Depth();
  Handle(TCollection_HAsciiString) aName =
    new TCollection_HAsciiString(another->Name());

  lower = 1;
  upper = another->NbEntities();
  Handle(IGESData_HArray1OfIGESEntity)  EntArray = new
    IGESData_HArray1OfIGESEntity(lower,upper);
  for (Standard_Integer i = lower;i <= upper;i++)
    {
      DeclareAndCast(IGESData_IGESEntity,myentity,
                     TC.Transferred(another->AssociatedEntity(i)));
      EntArray->SetValue(i,myentity);
    }
  ent->Init(aDepth,aName,EntArray);
}

IGESData_DirChecker  IGESBasic_ToolSubfigureDef::DirChecker
  (const Handle(IGESBasic_SubfigureDef)& ent ) const
{
  IGESData_DirChecker DC(308,0);  //TypeNo. 308, Form no. 0
  DC.Structure(IGESData_DefVoid);
  if (ent->HierarchyStatus() == 1) DC.GraphicsIgnored(01);
   // GraphicsIgnored if hierarchy status = 01
  else
    {
      DC.BlankStatusIgnored();
    }
  DC.UseFlagRequired(02);
  return DC;
}

void  IGESBasic_ToolSubfigureDef::OwnCheck
  (const Handle(IGESBasic_SubfigureDef)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void  IGESBasic_ToolSubfigureDef::OwnDump
  (const Handle(IGESBasic_SubfigureDef)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESBasic_SubfigureDef" << endl;

  S << "Depth of the subfigure : " << ent->Depth() << endl;
  S << "Name of subfigure : ";
  IGESData_DumpString(S,ent->Name());
  S << endl;
  S << "The Associated Entities : " ;
  IGESData_DumpEntities(S,dumper,level,1,ent->NbEntities(),ent->AssociatedEntity);
  S << endl;
}
