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

#include <IGESAppli_FlowLineSpec.hxx>
#include <IGESAppli_ToolFlowLineSpec.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Interface_ShareTool.hxx>

IGESAppli_ToolFlowLineSpec::IGESAppli_ToolFlowLineSpec() {}

void IGESAppli_ToolFlowLineSpec::ReadOwnParams(const occ::handle<IGESAppli_FlowLineSpec>& ent,
                                               const occ::handle<IGESData_IGESReaderData>& /* IR */,
                                               IGESData_ParamReader& PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed
  int                                                                     num;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> tempNameAndModifiers;
  if (!PR.ReadInteger(PR.Current(), "Number of property values", num))
    num = 0;
  if (num > 0)
    tempNameAndModifiers = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, num);
  else
    PR.AddFail("Number of property values: Not Positive");
  // szv#4:S4163:12Mar99 `st=` not needed
  if (!tempNameAndModifiers.IsNull())
    PR.ReadTexts(PR.CurrentList(num), "Name and Modifiers", tempNameAndModifiers);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempNameAndModifiers);
}

void IGESAppli_ToolFlowLineSpec::WriteOwnParams(const occ::handle<IGESAppli_FlowLineSpec>& ent,
                                                IGESData_IGESWriter&                       IW) const
{
  int i, num;
  IW.Send(ent->NbPropertyValues());
  for (num = ent->NbPropertyValues(), i = 1; i <= num; i++)
    IW.Send(ent->Modifier(i));
}

void IGESAppli_ToolFlowLineSpec::OwnShared(const occ::handle<IGESAppli_FlowLineSpec>& /* ent */,
                                           Interface_EntityIterator& /* iter */) const
{
}

void IGESAppli_ToolFlowLineSpec::OwnCopy(const occ::handle<IGESAppli_FlowLineSpec>& another,
                                         const occ::handle<IGESAppli_FlowLineSpec>& ent,
                                         Interface_CopyTool& /* TC */) const
{
  int num = another->NbPropertyValues();
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> tempNameAndModifiers =
    new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, num);
  for (int i = 1; i <= num; i++)
    tempNameAndModifiers->SetValue(i, new TCollection_HAsciiString(another->Modifier(i)));
  ent->Init(tempNameAndModifiers);
}

IGESData_DirChecker IGESAppli_ToolFlowLineSpec::DirChecker(
  const occ::handle<IGESAppli_FlowLineSpec>& /* ent */) const
{
  IGESData_DirChecker DC(406, 14);
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

void IGESAppli_ToolFlowLineSpec::OwnCheck(const occ::handle<IGESAppli_FlowLineSpec>& /* ent */,
                                          const Interface_ShareTool&,
                                          occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESAppli_ToolFlowLineSpec::OwnDump(const occ::handle<IGESAppli_FlowLineSpec>& ent,
                                         const IGESData_IGESDumper& /* dumper */,
                                         Standard_OStream& S,
                                         const int         level) const
{
  S << "IGESAppli_FlowLineSpec\n";
  S << "Name and Modifiers : ";
  IGESData_DumpStrings(S, level, 1, ent->NbPropertyValues(), ent->Modifier);
  S << "\n";
}
