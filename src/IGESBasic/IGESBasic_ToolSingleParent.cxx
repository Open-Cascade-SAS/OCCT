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

#include <IGESBasic_ToolSingleParent.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>

// MGE 03/08/98
#include <Message_Msg.hxx>
#include <IGESData_Status.hxx>


IGESBasic_ToolSingleParent::IGESBasic_ToolSingleParent ()    {  }


void  IGESBasic_ToolSingleParent::ReadOwnParams
  (const Handle(IGESBasic_SingleParent)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  Message_Msg Msg207("XSTEP_207");
  //========================================

  Standard_Integer tempNbParentEntities;
  Handle(IGESData_IGESEntity) tempParent;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer nbval = 0;
  Handle(IGESData_HArray1OfIGESEntity)  tempChildren;
  IGESData_Status aStatus;

  if(!PR.ReadInteger (PR.Current(),tempNbParentEntities)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg204("XSTEP_204");
    PR.SendFail(Msg204);
  }
  //st = PR.ReadInteger(PR.Current(), Msg205, nbval); //szv#4:S4163:12Mar99 moved down

//  st = PR.ReadInteger (PR.Current(),"Number of Parent entities",tempNbParentEntities);
//  st = PR.ReadInteger(PR.Current(), "Count of Children", nbval);
  if (!PR.ReadInteger(PR.Current(), nbval)){
    Message_Msg Msg205("XSTEP_205");
    PR.SendFail(Msg205);
    nbval = -1;
  }
  if (!PR.ReadEntity(IR,PR.Current(),aStatus,tempParent)){ //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadEntity(IR,PR.Current(),"ParentEntity",tempParent);
    Message_Msg Msg206("XSTEP_206");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg206.Arg(Msg216.Value());
      PR.SendFail(Msg206);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg206.Arg(Msg217.Value());
      PR.SendFail(Msg206);
      break; }
    default:{
    }
    }
  }

  if (nbval > 0) PR.ReadEnts (IR,PR.CurrentList(nbval),Msg207,tempChildren); //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadEnts (IR,PR.CurrentList(nbval),"Child Entities",tempChildren);
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNbParentEntities,tempParent,tempChildren);
}

void  IGESBasic_ToolSingleParent::WriteOwnParams
  (const Handle(IGESBasic_SingleParent)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer upper = ent->NbChildren();
  IW.Send(ent->NbParentEntities());
  IW.Send(upper);
  IW.Send(ent->SingleParent());
  for (Standard_Integer i = 1; i <= upper; i ++)
    IW.Send(ent->Child(i));
}

void  IGESBasic_ToolSingleParent::OwnShared
  (const Handle(IGESBasic_SingleParent)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->SingleParent());
  Standard_Integer upper = ent->NbChildren();
  for (Standard_Integer i = 1; i <= upper; i ++)
    iter.GetOneItem(ent->Child(i));
}

void  IGESBasic_ToolSingleParent::OwnCopy
  (const Handle(IGESBasic_SingleParent)& another,
   const Handle(IGESBasic_SingleParent)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer aNbParentEntities = another->NbParentEntities();
  DeclareAndCast(IGESData_IGESEntity,aparent,
		 TC.Transferred(another->SingleParent()));
  Standard_Integer upper = another->NbChildren();
  Handle(IGESData_HArray1OfIGESEntity)  EntArray = new
    IGESData_HArray1OfIGESEntity(1,upper);
  for (Standard_Integer i = 1; i <= upper; i ++)
    {
      DeclareAndCast
	(IGESData_IGESEntity,myentity,TC.Transferred(another->Child(i)));
      EntArray->SetValue(i,myentity);
    }
  ent->Init(aNbParentEntities,aparent,EntArray);
}

Standard_Boolean  IGESBasic_ToolSingleParent::OwnCorrect
  (const Handle(IGESBasic_SingleParent)& ent) const
{
  if (ent->NbParentEntities() == 1) return Standard_False;
  Standard_Integer nb = ent->NbChildren();
  Handle(IGESData_HArray1OfIGESEntity)  EntArray = new
    IGESData_HArray1OfIGESEntity (1,nb);
  for (Standard_Integer i = 1; i <= nb; i ++)
    EntArray->SetValue(i,ent->Child(i));
  ent->Init (1,ent->SingleParent(),EntArray);
  return Standard_True;    // nbparents = 1
}

IGESData_DirChecker  IGESBasic_ToolSingleParent::DirChecker
  (const Handle(IGESBasic_SingleParent)& /* ent */ ) const
{
  IGESData_DirChecker DC(402,9);  //Form no = 9 & Type = 402
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESBasic_ToolSingleParent::OwnCheck
  (const Handle(IGESBasic_SingleParent)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
   // MGE 03/08/98
  // Building of messages
  //========================================
//  Message_Msg Msg204("XSTEP_204");
  //========================================
  if (ent->NbParentEntities() != 1){
    Message_Msg Msg204("XSTEP_204");
    ach->SendFail(Msg204);
  }
}

void  IGESBasic_ToolSingleParent::OwnDump
  (const Handle(IGESBasic_SingleParent)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESBasic_SingleParent" << endl;

  S << "Number of ParentEntities : " << ent->NbParentEntities() << endl;
  S << "ParentEntity : ";
  dumper.Dump(ent->SingleParent(),S,(level <= 4) ? 0 : 1);
  S << endl;
  S << "Children : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbChildren(),ent->Child);
  S << endl;
}
