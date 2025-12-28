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

#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDefs_AssociativityDef.hxx>
#include <IGESDefs_ToolAssociativityDef.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IGESDefs_ToolAssociativityDef::IGESDefs_ToolAssociativityDef() = default;

void IGESDefs_ToolAssociativityDef::ReadOwnParams(
  const occ::handle<IGESDefs_AssociativityDef>& ent,
  const occ::handle<IGESData_IGESReaderData>& /* IR */,
  IGESData_ParamReader& PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down
  occ::handle<NCollection_HArray1<int>>            requirements;
  occ::handle<NCollection_HArray1<int>>            orders;
  occ::handle<NCollection_HArray1<int>>            numItems;
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger> items;
  int                                              nbval;

  bool st = PR.ReadInteger(PR.Current(), "No. of Class definitions", nbval);
  if (st && nbval > 0)
  {
    requirements = new NCollection_HArray1<int>(1, nbval);
    orders       = new NCollection_HArray1<int>(1, nbval);
    numItems     = new NCollection_HArray1<int>(1, nbval);
    items        = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);
  }
  else
    PR.AddFail("No. of Class definitions: Not Positive");

  if (!requirements.IsNull())
    for (int i = 1; i <= nbval; i++)
    {
      int                                   requirement;
      int                                   order;
      int                                   numItem;
      occ::handle<NCollection_HArray1<int>> item;

      // st = PR.ReadInteger(PR.Current(), "Back Pointer Requirement", requirement);
      // //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadInteger(PR.Current(), "Back Pointer Requirement", requirement))
        requirements->SetValue(i, requirement);

      // st = PR.ReadInteger(PR.Current(), "Ordered/Unordered Class", order); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadInteger(PR.Current(), "Ordered/Unordered Class", order))
        orders->SetValue(i, order);

      // st = PR.ReadInteger(PR.Current(),"No. of items per entry", numItem); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadInteger(PR.Current(), "No. of items per entry", numItem))
      {
        int temp;
        numItems->SetValue(i, numItem);
        item = new NCollection_HArray1<int>(1, numItem);
        for (int j = 1; j <= numItem; j++)
        {
          // int temp; //szv#4:S4163:12Mar99 moved out of for
          // st = PR.ReadInteger(PR.Current(), "Item", temp); //szv#4:S4163:12Mar99 moved in if
          if (PR.ReadInteger(PR.Current(), "Item", temp))
            item->SetValue(j, temp);
        }
        items->SetValue(i, item);
      }
    }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(requirements, orders, numItems, items);
}

void IGESDefs_ToolAssociativityDef::WriteOwnParams(
  const occ::handle<IGESDefs_AssociativityDef>& ent,
  IGESData_IGESWriter&                          IW) const
{
  int upper = ent->NbClassDefs();
  IW.Send(upper);
  for (int i = 1; i <= upper; i++)
  {
    IW.Send(ent->BackPointerReq(i));
    IW.Send(ent->ClassOrder(i));
    IW.Send(ent->NbItemsPerClass(i));
    int items = ent->NbItemsPerClass(i);
    for (int j = 1; j <= items; j++)
      IW.Send(ent->Item(i, j));
  }
}

void IGESDefs_ToolAssociativityDef::OwnShared(
  const occ::handle<IGESDefs_AssociativityDef>& /* ent */,
  Interface_EntityIterator& /* iter */) const
{
}

void IGESDefs_ToolAssociativityDef::OwnCopy(const occ::handle<IGESDefs_AssociativityDef>& another,
                                            const occ::handle<IGESDefs_AssociativityDef>& ent,
                                            Interface_CopyTool& /* TC */) const
{

  occ::handle<NCollection_HArray1<int>>            requirements;
  occ::handle<NCollection_HArray1<int>>            orders;
  occ::handle<NCollection_HArray1<int>>            numItems;
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger> items;

  int nbval = another->NbClassDefs();

  requirements = new NCollection_HArray1<int>(1, nbval);
  orders       = new NCollection_HArray1<int>(1, nbval);
  numItems     = new NCollection_HArray1<int>(1, nbval);
  items        = new IGESBasic_HArray1OfHArray1OfInteger(1, nbval);

  for (int i = 1; i <= nbval; i++)
  {
    int requirement = another->BackPointerReq(i);
    requirements->SetValue(i, requirement);
    int order = another->ClassOrder(i);
    orders->SetValue(i, order);
    int numItem = another->NbItemsPerClass(i);
    numItems->SetValue(i, numItem);
    occ::handle<NCollection_HArray1<int>> item;
    item = new NCollection_HArray1<int>(1, numItem);

    for (int j = 1; j <= numItem; j++)
    {
      int temp = another->Item(i, j);
      item->SetValue(j, temp);
    }
    items->SetValue(i, item);
  }
  ent->Init(requirements, orders, numItems, items);
  ent->SetFormNumber(another->FormNumber());
}

IGESData_DirChecker IGESDefs_ToolAssociativityDef::DirChecker(
  const occ::handle<IGESDefs_AssociativityDef>& /* ent */) const
{
  IGESData_DirChecker DC(302, 5001, 9999);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDefs_ToolAssociativityDef::OwnCheck(
  const occ::handle<IGESDefs_AssociativityDef>& /* ent */,
  const Interface_ShareTool&,
  occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESDefs_ToolAssociativityDef::OwnDump(const occ::handle<IGESDefs_AssociativityDef>& ent,
                                            const IGESData_IGESDumper& /* dumper */,
                                            Standard_OStream& S,
                                            const int         level) const
{
  S << "IGESDefs_AssociativityDef\n"
    << "Number of Class Definitions : " << ent->NbClassDefs() << "\n"
    << "Back Pointer Requirement  :\n"
    << "Ordered / Unordered Class :\n"
    << "Number Of Items per Entry :\n"
    << "Items :\n";
  IGESData_DumpVals(S, -level, 1, ent->NbClassDefs(), ent->BackPointerReq);
  S << std::endl;
  if (level > 4)
  {
    // Warning : Item is a JAGGED Array
    int upper = ent->NbClassDefs();
    for (int i = 1; i <= upper; i++)
    {
      S << "[" << i << "]:\n";
      S << "Back Pointer Requirement : " << ent->BackPointerReq(i) << "  ";
      if (ent->IsBackPointerReq(i))
        S << "(Yes)  ";
      else
        S << "(No)   ";
      S << " Ordered/Unordered Class : " << ent->ClassOrder(i) << " ";
      if (ent->IsOrdered(i))
        S << "(Yes)\n";
      else
        S << "(No)\n";
      S << "Number Of Items per Entry : " << ent->NbItemsPerClass(i);
      if (level < 6)
      {
        S << " [ask level > 5 for more]\n";
        continue;
      }
      S << "\n [";
      for (int j = 1; j <= ent->NbItemsPerClass(i); j++)
        S << "  " << ent->Item(i, j);
      S << "]\n";
    }
  }
  S << std::endl;
}
