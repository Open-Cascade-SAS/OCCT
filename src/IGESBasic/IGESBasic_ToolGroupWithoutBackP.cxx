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

#include <IGESBasic_ToolGroupWithoutBackP.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>

// MGE 03/08/98
#include <Message_Msg.hxx>


IGESBasic_ToolGroupWithoutBackP::IGESBasic_ToolGroupWithoutBackP ()    {  }


void  IGESBasic_ToolGroupWithoutBackP::ReadOwnParams
  (const Handle(IGESBasic_GroupWithoutBackP)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
//  Message_Msg Msg202("XSTEP_202");
//  Message_Msg Msg203("XSTEP_203");
  //========================================

  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer nbval = 0;
  Handle(IGESData_HArray1OfIGESEntity)  EntArray;
//  Msg202.Arg(7);
  //st = PR.ReadInteger( PR.Current(), Msg202, nbval); //szv#4:S4163:12Mar99 not needed
  //st = PR.ReadInteger( PR.Current(), "Count of Entities", nbval);
  if (PR.ReadInteger( PR.Current(), nbval)) { //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg203("XSTEP_203");
    Msg203.Arg(7);
    PR.ReadEnts (IR,PR.CurrentList(nbval),Msg203,EntArray); //szv#4:S4163:12Mar99 `st=` not needed
    //st = PR.ReadEnts (IR,PR.CurrentList(nbval),"Entities",EntArray);
  }
  else{
    Message_Msg Msg202("XSTEP_202");
    Msg202.Arg(7);
    PR.SendFail(Msg202);
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(EntArray);
}

void  IGESBasic_ToolGroupWithoutBackP::WriteOwnParams
  (const Handle(IGESBasic_GroupWithoutBackP)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer upper = ent->NbEntities();
  IW.Send(upper);
  for (Standard_Integer i = 1; i <= upper;i++)
    IW.Send(ent->Entity(i));
}

void  IGESBasic_ToolGroupWithoutBackP::OwnShared
  (const Handle(IGESBasic_GroupWithoutBackP)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer upper = ent->NbEntities();
  for (Standard_Integer i = 1; i <= upper; i ++)
    iter.GetOneItem(ent->Entity(i));
}

void  IGESBasic_ToolGroupWithoutBackP::OwnCopy
  (const Handle(IGESBasic_GroupWithoutBackP)& another,
   const Handle(IGESBasic_GroupWithoutBackP)& ent, Interface_CopyTool& TC) const
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

Standard_Boolean IGESBasic_ToolGroupWithoutBackP::OwnCorrect
  (const Handle(IGESBasic_GroupWithoutBackP)& ent )  const
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

IGESData_DirChecker  IGESBasic_ToolGroupWithoutBackP::DirChecker
  (const Handle(IGESBasic_GroupWithoutBackP)& /* ent */ ) const
{
  IGESData_DirChecker DC(402,7);  //TypeNo. 402, Form no. 7
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESBasic_ToolGroupWithoutBackP::OwnCheck
  (const Handle(IGESBasic_GroupWithoutBackP)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
  Standard_Boolean ianul = Standard_False;
  Standard_Integer i, nb = ent->NbEntities();
  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) val = ent->Entity(i);
    if (val.IsNull()) ianul = Standard_True;
    else if (val->TypeNumber() == 0) ianul = Standard_True;
    if (ianul) {
      break;
    }
  }
}

void  IGESBasic_ToolGroupWithoutBackP::OwnDump
(const Handle(IGESBasic_GroupWithoutBackP)& ent, const IGESData_IGESDumper& dumper,
 const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESBasic_GroupWithoutBackP" << endl;
  S << "Entries in the Group : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbEntities(),ent->Entity);
  S << endl;
}

