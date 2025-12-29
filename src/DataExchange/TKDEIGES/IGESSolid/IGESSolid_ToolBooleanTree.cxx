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
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESSolid_BooleanTree.hxx>
#include <IGESSolid_ToolBooleanTree.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Integer.hxx>

#include <cstdio>

IGESSolid_ToolBooleanTree::IGESSolid_ToolBooleanTree() = default;

void IGESSolid_ToolBooleanTree::ReadOwnParams(const occ::handle<IGESSolid_BooleanTree>&   ent,
                                              const occ::handle<IGESData_IGESReaderData>& IR,
                                              IGESData_ParamReader&                       PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down
  int                                                                length, intvalue;
  occ::handle<IGESData_IGESEntity>                                   entvalue;
  occ::handle<NCollection_HArray1<int>>                              tempOperations;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempOperands;

  bool st = PR.ReadInteger(PR.Current(), "Length of post-order notation", length);
  if (st && length > 0)
  {
    tempOperations = new NCollection_HArray1<int>(1, length);
    tempOperations->Init(0);
    tempOperands = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, length);

    // Op. 1-2 : Operands
    // st = PR.ReadEntity(IR, PR.Current(), "Operand 1", entvalue); //szv#4:S4163:12Mar99 moved in
    // if
    if (PR.ReadEntity(IR, PR.Current(), "Operand 1", entvalue))
      tempOperands->SetValue(1, entvalue);

    // st = PR.ReadEntity(IR, PR.Current(), "Operand 2", entvalue); //szv#4:S4163:12Mar99 moved in
    // if
    if (PR.ReadEntity(IR, PR.Current(), "Operand 2", entvalue))
      tempOperands->SetValue(2, entvalue);

    // Op. 3 -> length-1 : Operand or Operation
    for (int i = 3; i < length; i++)
    {
      int curnum = PR.CurrentNumber();
      // clang-format off
	  PR.ReadInteger(PR.Current(), "Operation code", intvalue); //szv#4:S4163:12Mar99 `st=` not needed
      // clang-format on
      if (intvalue < 0)
      {
        entvalue = PR.ParamEntity(IR, curnum);
        if (entvalue.IsNull())
          PR.AddFail("Operand : incorrect reference");
        else
          tempOperands->SetValue(i, entvalue);
      }
      else
        tempOperations->SetValue(i, intvalue);
    }
    // Last Op. : Operation
    // st = PR.ReadInteger(PR.Current(), "Operation code", intvalue); //szv#4:S4163:12Mar99 moved in
    // if
    if (PR.ReadInteger(PR.Current(), "Operation code", intvalue))
      tempOperations->SetValue(length, intvalue);
  }
  else
    PR.AddFail("Length of post-order : Not Positive");

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempOperands, tempOperations);
}

void IGESSolid_ToolBooleanTree::WriteOwnParams(const occ::handle<IGESSolid_BooleanTree>& ent,
                                               IGESData_IGESWriter&                      IW) const
{
  int length = ent->Length();

  IW.Send(length);
  for (int i = 1; i <= length; i++)
  {
    if (ent->IsOperand(i))
      IW.Send(ent->Operand(i), true);
    else
      IW.Send(ent->Operation(i));
  }
}

void IGESSolid_ToolBooleanTree::OwnShared(const occ::handle<IGESSolid_BooleanTree>& ent,
                                          Interface_EntityIterator&                 iter) const
{
  int length = ent->Length();
  for (int i = 1; i <= length; i++)
  {
    if (ent->IsOperand(i))
      iter.GetOneItem(ent->Operand(i));
  }
}

void IGESSolid_ToolBooleanTree::OwnCopy(const occ::handle<IGESSolid_BooleanTree>& another,
                                        const occ::handle<IGESSolid_BooleanTree>& ent,
                                        Interface_CopyTool&                       TC) const
{
  int i;

  int                                   length         = another->Length();
  occ::handle<NCollection_HArray1<int>> tempOperations = new NCollection_HArray1<int>(1, length);
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempOperands =
    new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, length);

  for (i = 1; i <= length; i++)
  {
    if (another->IsOperand(i)) // Operand
    {
      DeclareAndCast(IGESData_IGESEntity, new_ent, TC.Transferred(another->Operand(i)));
      tempOperands->SetValue(i, new_ent);
    }
    else // Operation
      tempOperations->SetValue(i, another->Operation(i));
  }
  ent->Init(tempOperands, tempOperations);
}

IGESData_DirChecker IGESSolid_ToolBooleanTree::DirChecker(
  const occ::handle<IGESSolid_BooleanTree>& /*ent*/) const
{
  IGESData_DirChecker DC(180, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.Color(IGESData_DefAny);

  DC.UseFlagRequired(0);
  DC.GraphicsIgnored(1);
  return DC;
}

void IGESSolid_ToolBooleanTree::OwnCheck(const occ::handle<IGESSolid_BooleanTree>& ent,
                                         const Interface_ShareTool&,
                                         occ::handle<Interface_Check>& ach) const
{
  int length = ent->Length();
  if (length <= 2)
    ach->AddFail("Length of post-order notation : Less than three");
  else
  {
    if (!ent->IsOperand(1))
      ach->AddFail("First Item is not an Operand");
    if (!ent->IsOperand(2))
      ach->AddFail("Second Item is not an Operand");
    if (ent->IsOperand(length))
      ach->AddFail("Last Item is not an Operation");
  }
  for (int i = 1; i <= length; i++)
  {
    if (!ent->Operand(i).IsNull())
      continue;
    if (ent->Operation(i) < 1 || ent->Operation(i) > 3)
    {
      char mess[80];
      Sprintf(mess, "Item no. %d Incorrect", i);
      ach->AddFail(mess);
    }
  }
}

void IGESSolid_ToolBooleanTree::OwnDump(const occ::handle<IGESSolid_BooleanTree>& ent,
                                        const IGESData_IGESDumper&                dumper,
                                        Standard_OStream&                         S,
                                        const int                                 level) const
{
  int i, length = ent->Length();

  S << "IGESSolid_Boolean Tree\n"
    << "Length of the post-order notation :" << length << "\n";
  if (level > 4)
  {
    S << "Post-order notation of the Boolean Tree :\n";
    for (i = 1; i <= length; i++)
    {
      if (ent->IsOperand(i))
      {
        S << "[" << i << "] Operand : ";
        dumper.Dump(ent->Operand(i), S, 1);
        S << "\n";
      }
      else
      {
        int opcode = ent->Operation(i);
        S << "[" << i << "] Operator : " << opcode;
        if (opcode == 1)
          S << " (Union)";
        else if (opcode == 2)
          S << " (Intersection)\n";
        else if (opcode == 3)
          S << " (Difference)\n";
        else
          S << " (incorrect value)\n";
      }
    }
  }
  //  aSender << std::endl;
}
