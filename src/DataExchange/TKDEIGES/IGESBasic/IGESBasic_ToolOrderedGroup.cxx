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

#include <IGESBasic_OrderedGroup.hxx>
#include <IGESBasic_ToolOrderedGroup.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESBasic_ToolOrderedGroup::IGESBasic_ToolOrderedGroup() {}

void IGESBasic_ToolOrderedGroup::ReadOwnParams(const occ::handle<IGESBasic_OrderedGroup>&  ent,
                                               const occ::handle<IGESData_IGESReaderData>& IR,
                                               IGESData_ParamReader&                  PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed
  int                     nbval = 0;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> EntArray;

  if (PR.ReadInteger(PR.Current(), "Count of Entities", nbval))
  { // szv#4:S4163:12Mar99 `st=` not needed
    // clang-format off
    PR.ReadEnts (IR,PR.CurrentList(nbval),"Entities",EntArray); //szv#4:S4163:12Mar99 `st=` not needed
    // clang-format on
    /*
        EntArray = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1,nbval);
        for (int i = 1;i <= nbval;i++)
          {
        occ::handle<IGESData_IGESEntity> anent;
        st = PR.ReadEntity (IR,PR.Current(),"Element of the Group",anent,true);
        if (st) EntArray->SetValue(i,anent);
          }
    */
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(EntArray);
}

void IGESBasic_ToolOrderedGroup::WriteOwnParams(const occ::handle<IGESBasic_OrderedGroup>& ent,
                                                IGESData_IGESWriter&                  IW) const
{
  int upper = ent->NbEntities();
  IW.Send(upper);
  for (int i = 1; i <= upper; i++)
    IW.Send(ent->Entity(i));
}

void IGESBasic_ToolOrderedGroup::OwnShared(const occ::handle<IGESBasic_OrderedGroup>& ent,
                                           Interface_EntityIterator&             iter) const
{
  int upper = ent->NbEntities();
  for (int i = 1; i <= upper; i++)
    iter.GetOneItem(ent->Entity(i));
}

void IGESBasic_ToolOrderedGroup::OwnCopy(const occ::handle<IGESBasic_OrderedGroup>& another,
                                         const occ::handle<IGESBasic_OrderedGroup>& ent,
                                         Interface_CopyTool&                   TC) const
{
  int lower, upper;
  lower                                         = 1;
  upper                                         = another->NbEntities();
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> EntArray = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(lower, upper);
  for (int i = lower; i <= upper; i++)
  {
    DeclareAndCast(IGESData_IGESEntity, myentity, TC.Transferred(another->Entity(i)));
    EntArray->SetValue(i, myentity);
  }
  ent->Init(EntArray);
}

bool IGESBasic_ToolOrderedGroup::OwnCorrect(
  const occ::handle<IGESBasic_OrderedGroup>& ent) const
{
  int ianul = 0;
  int i, nbtrue = 0, nb = ent->NbEntities();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<IGESData_IGESEntity> val = ent->Entity(i);
    if (val.IsNull())
      ianul++;
    else if (val->TypeNumber() == 0)
      ianul++;
  }
  if (ianul == 0)
    return false;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> EntArray;
  if (ianul < nb)
    EntArray = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nb - ianul);
  for (i = 1; i <= nb; i++)
  {
    occ::handle<IGESData_IGESEntity> val = ent->Entity(i);
    if (val.IsNull())
      continue;
    else if (val->TypeNumber() == 0)
      continue;
    nbtrue++;
    EntArray->SetValue(nbtrue, ent->Entity(i));
  }
  ent->Init(EntArray);
  return true;
}

IGESData_DirChecker IGESBasic_ToolOrderedGroup::DirChecker(
  const occ::handle<IGESBasic_OrderedGroup>& /* ent */) const
{
  IGESData_DirChecker DC(402, 14); // TypeNo. 402, Form no. 14
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESBasic_ToolOrderedGroup::OwnCheck(const occ::handle<IGESBasic_OrderedGroup>& ent,
                                          const Interface_ShareTool&,
                                          occ::handle<Interface_Check>& ach) const
{
  bool ianul = false;
  int i, nb = ent->NbEntities();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<IGESData_IGESEntity> val = ent->Entity(i);
    if (val.IsNull())
      ianul = true;
    else if (val->TypeNumber() == 0)
      ianul = true;
    if (ianul)
    {
      ach->AddWarning("At least one element is Null");
      break;
    }
  }
}

void IGESBasic_ToolOrderedGroup::OwnDump(const occ::handle<IGESBasic_OrderedGroup>& ent,
                                         const IGESData_IGESDumper&            dumper,
                                         Standard_OStream&                     S,
                                         const int                level) const
{
  S << "IGESBasic_OrderedGroup\n"
    << "Entries in the Group : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbEntities(), ent->Entity);
  S << std::endl;
}
