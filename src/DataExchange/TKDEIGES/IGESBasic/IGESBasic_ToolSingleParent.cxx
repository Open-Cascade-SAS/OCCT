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

#include <IGESBasic_SingleParent.hxx>
#include <IGESBasic_ToolSingleParent.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>

// MGE 03/08/98
IGESBasic_ToolSingleParent::IGESBasic_ToolSingleParent() {}

void IGESBasic_ToolSingleParent::ReadOwnParams(const occ::handle<IGESBasic_SingleParent>&  ent,
                                               const occ::handle<IGESData_IGESReaderData>& IR,
                                               IGESData_ParamReader&                       PR) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  Message_Msg Msg207("XSTEP_207");
  //========================================

  int                              tempNbParentEntities;
  occ::handle<IGESData_IGESEntity> tempParent;
  // bool st; //szv#4:S4163:12Mar99 not needed
  int                                                                nbval = 0;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempChildren;
  IGESData_Status                                                    aStatus;

  if (!PR.ReadInteger(PR.Current(), tempNbParentEntities))
  { // szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg204("XSTEP_204");
    PR.SendFail(Msg204);
  }
  // st = PR.ReadInteger(PR.Current(), Msg205, nbval); //szv#4:S4163:12Mar99 moved down

  //  st = PR.ReadInteger (PR.Current(),"Number of Parent entities",tempNbParentEntities);
  //  st = PR.ReadInteger(PR.Current(), "Count of Children", nbval);
  if (!PR.ReadInteger(PR.Current(), nbval))
  {
    Message_Msg Msg205("XSTEP_205");
    PR.SendFail(Msg205);
    nbval = -1;
  }
  if (!PR.ReadEntity(IR, PR.Current(), aStatus, tempParent))
  { // szv#4:S4163:12Mar99 `st=` not needed
    // st = PR.ReadEntity(IR,PR.Current(),"ParentEntity",tempParent);
    Message_Msg Msg206("XSTEP_206");
    switch (aStatus)
    {
      case IGESData_ReferenceError: {
        Message_Msg Msg216("IGES_216");
        Msg206.Arg(Msg216.Value());
        PR.SendFail(Msg206);
        break;
      }
      case IGESData_EntityError: {
        Message_Msg Msg217("IGES_217");
        Msg206.Arg(Msg217.Value());
        PR.SendFail(Msg206);
        break;
      }
      default: {
      }
    }
  }

  // clang-format off
  if (nbval > 0) PR.ReadEnts (IR,PR.CurrentList(nbval),Msg207,tempChildren); //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadEnts (IR,PR.CurrentList(nbval),"Child Entities",tempChildren);
  // clang-format on
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempNbParentEntities, tempParent, tempChildren);
}

void IGESBasic_ToolSingleParent::WriteOwnParams(const occ::handle<IGESBasic_SingleParent>& ent,
                                                IGESData_IGESWriter&                       IW) const
{
  int upper = ent->NbChildren();
  IW.Send(ent->NbParentEntities());
  IW.Send(upper);
  IW.Send(ent->SingleParent());
  for (int i = 1; i <= upper; i++)
    IW.Send(ent->Child(i));
}

void IGESBasic_ToolSingleParent::OwnShared(const occ::handle<IGESBasic_SingleParent>& ent,
                                           Interface_EntityIterator&                  iter) const
{
  iter.GetOneItem(ent->SingleParent());
  int upper = ent->NbChildren();
  for (int i = 1; i <= upper; i++)
    iter.GetOneItem(ent->Child(i));
}

void IGESBasic_ToolSingleParent::OwnCopy(const occ::handle<IGESBasic_SingleParent>& another,
                                         const occ::handle<IGESBasic_SingleParent>& ent,
                                         Interface_CopyTool&                        TC) const
{
  int aNbParentEntities = another->NbParentEntities();
  DeclareAndCast(IGESData_IGESEntity, aparent, TC.Transferred(another->SingleParent()));
  int                                                                upper = another->NbChildren();
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> EntArray =
    new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, upper);
  for (int i = 1; i <= upper; i++)
  {
    DeclareAndCast(IGESData_IGESEntity, myentity, TC.Transferred(another->Child(i)));
    EntArray->SetValue(i, myentity);
  }
  ent->Init(aNbParentEntities, aparent, EntArray);
}

bool IGESBasic_ToolSingleParent::OwnCorrect(const occ::handle<IGESBasic_SingleParent>& ent) const
{
  if (ent->NbParentEntities() == 1)
    return false;
  int                                                                nb = ent->NbChildren();
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> EntArray =
    new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nb);
  for (int i = 1; i <= nb; i++)
    EntArray->SetValue(i, ent->Child(i));
  ent->Init(1, ent->SingleParent(), EntArray);
  return true; // nbparents = 1
}

IGESData_DirChecker IGESBasic_ToolSingleParent::DirChecker(
  const occ::handle<IGESBasic_SingleParent>& /* ent */) const
{
  IGESData_DirChecker DC(402, 9); // Form no = 9 & Type = 402
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESBasic_ToolSingleParent::OwnCheck(const occ::handle<IGESBasic_SingleParent>& ent,
                                          const Interface_ShareTool&,
                                          occ::handle<Interface_Check>& ach) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  //  Message_Msg Msg204("XSTEP_204");
  //========================================
  if (ent->NbParentEntities() != 1)
  {
    Message_Msg Msg204("XSTEP_204");
    ach->SendFail(Msg204);
  }
}

void IGESBasic_ToolSingleParent::OwnDump(const occ::handle<IGESBasic_SingleParent>& ent,
                                         const IGESData_IGESDumper&                 dumper,
                                         Standard_OStream&                          S,
                                         const int                                  level) const
{
  S << "IGESBasic_SingleParent\n"
    << "Number of ParentEntities : " << ent->NbParentEntities() << "\n"
    << "ParentEntity : ";
  dumper.Dump(ent->SingleParent(), S, (level <= 4) ? 0 : 1);
  S << "\n"
    << "Children : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbChildren(), ent->Child);
  S << std::endl;
}
