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

#include <IGESAppli_FiniteElement.hxx>
#include <IGESAppli_Node.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESAppli_ToolFiniteElement.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>

IGESAppli_ToolFiniteElement::IGESAppli_ToolFiniteElement() {}

void IGESAppli_ToolFiniteElement::ReadOwnParams(const occ::handle<IGESAppli_FiniteElement>& ent,
                                                const occ::handle<IGESData_IGESReaderData>& IR,
                                                IGESData_ParamReader& PR) const
{
  int                                   tempTopology;
  occ::handle<TCollection_HAsciiString> tempName;
  int                                   nbval = 0;
  // bool st; //szv#4:S4163:12Mar99 not needed
  occ::handle<NCollection_HArray1<occ::handle<IGESAppli_Node>>> tempData;

  // szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Topology type", tempTopology);
  PR.ReadInteger(PR.Current(), "No. of nodes defining element", nbval);
  tempData = new NCollection_HArray1<occ::handle<IGESAppli_Node>>(1, nbval);
  for (int i = 1; i <= nbval; i++)
  {
    occ::handle<IGESAppli_Node> tempNode;
    // szv#4:S4163:12Mar99 moved in if
    if (PR.ReadEntity(IR,
                      PR.Current(),
                      "Node defining element",
                      STANDARD_TYPE(IGESAppli_Node),
                      tempNode))
      tempData->SetValue(i, tempNode);
  }
  PR.ReadText(PR.Current(), "Element type name", tempName); // szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempTopology, tempData, tempName);
}

void IGESAppli_ToolFiniteElement::WriteOwnParams(const occ::handle<IGESAppli_FiniteElement>& ent,
                                                 IGESData_IGESWriter& IW) const
{
  int upper = ent->NbNodes();
  IW.Send(ent->Topology());
  IW.Send(upper);
  for (int i = 1; i <= upper; i++)
    IW.Send(ent->Node(i));
  IW.Send(ent->Name());
}

void IGESAppli_ToolFiniteElement::OwnShared(const occ::handle<IGESAppli_FiniteElement>& ent,
                                            Interface_EntityIterator&                   iter) const
{
  int upper = ent->NbNodes();
  for (int i = 1; i <= upper; i++)
    iter.GetOneItem(ent->Node(i));
}

void IGESAppli_ToolFiniteElement::OwnCopy(const occ::handle<IGESAppli_FiniteElement>& another,
                                          const occ::handle<IGESAppli_FiniteElement>& ent,
                                          Interface_CopyTool&                         TC) const
{
  int                                   aTopology = another->Topology();
  occ::handle<TCollection_HAsciiString> aName     = new TCollection_HAsciiString(another->Name());
  int                                   nbval     = another->NbNodes();
  occ::handle<NCollection_HArray1<occ::handle<IGESAppli_Node>>> aList =
    new NCollection_HArray1<occ::handle<IGESAppli_Node>>(1, nbval);
  for (int i = 1; i <= nbval; i++)
  {
    DeclareAndCast(IGESAppli_Node, aEntity, TC.Transferred(another->Node(i)));
    aList->SetValue(i, aEntity);
  }
  ent->Init(aTopology, aList, aName);
}

IGESData_DirChecker IGESAppli_ToolFiniteElement::DirChecker(
  const occ::handle<IGESAppli_FiniteElement>& /* ent */) const
{
  IGESData_DirChecker DC(136, 0); // Form no = 0 & Type = 136
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_ToolFiniteElement::OwnCheck(const occ::handle<IGESAppli_FiniteElement>& /* ent */,
                                           const Interface_ShareTool&,
                                           occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESAppli_ToolFiniteElement::OwnDump(const occ::handle<IGESAppli_FiniteElement>& ent,
                                          const IGESData_IGESDumper&                  dumper,
                                          Standard_OStream&                           S,
                                          const int                                   level) const
{
  S << "IGESAppli_FiniteElement\n";

  S << "Topology type : " << ent->Topology() << "\n";
  S << "Nodes : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbNodes(), ent->Node);
  S << "\nElement Name : " << ent->Name()->String();
  S << std::endl;
}
