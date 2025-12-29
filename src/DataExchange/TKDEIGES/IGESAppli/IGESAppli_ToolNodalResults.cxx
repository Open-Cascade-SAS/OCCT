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

#include <IGESAppli_Node.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESAppli_NodalResults.hxx>
#include <IGESAppli_ToolNodalResults.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

IGESAppli_ToolNodalResults::IGESAppli_ToolNodalResults() = default;

void IGESAppli_ToolNodalResults::ReadOwnParams(const occ::handle<IGESAppli_NodalResults>&  ent,
                                               const occ::handle<IGESData_IGESReaderData>& IR,
                                               IGESData_ParamReader&                       PR) const
{
  int    tempSubCaseNum = 0;
  double tempTime;
  int    nbval   = 0;
  int    nbnodes = 0;
  // bool st; //szv#4:S4163:12Mar99 not needed
  occ::handle<IGESDimen_GeneralNote>                            tempNote;
  occ::handle<NCollection_HArray2<double>>                      tempData;
  occ::handle<NCollection_HArray1<occ::handle<IGESAppli_Node>>> tempNodes;
  occ::handle<NCollection_HArray1<int>>                         tempNodeIdentifiers;

  // szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadEntity(IR,
                PR.Current(),
                "General Note describing the analysis case",
                STANDARD_TYPE(IGESDimen_GeneralNote),
                tempNote);
  PR.ReadInteger(PR.Current(), "Subcase number", tempSubCaseNum);
  PR.ReadReal(PR.Current(), "Analysis time used", tempTime);
  bool tempFlag = PR.ReadInteger(PR.Current(), "No. of values", nbval);
  // szv#4:S4163:12Mar99 moved in if
  if (PR.ReadInteger(PR.Current(), "No. of nodes", nbnodes))
  {
    tempData            = new NCollection_HArray2<double>(1, nbnodes, 1, nbval);
    tempNodes           = new NCollection_HArray1<occ::handle<IGESAppli_Node>>(1, nbnodes);
    tempNodeIdentifiers = new NCollection_HArray1<int>(1, nbnodes);
    for (int i = 1; i <= nbnodes; i++)
    {
      int aitem;
      // Check  whether nbval properly read or not.
      occ::handle<IGESAppli_Node> aNode;

      if (PR.ReadInteger(PR.Current(), "Node no. identifier", aitem))
        tempNodeIdentifiers->SetValue(i, aitem);
      if (PR.ReadEntity(IR, PR.Current(), "FEM Node", STANDARD_TYPE(IGESAppli_Node), aNode))
        tempNodes->SetValue(i, aNode);
      if (tempFlag)
        // Check  whether nbval properly read or not.
        for (int j = 1; j <= nbval; j++)
        {
          double aval;
          if (PR.ReadReal(PR.Current(), "Value", aval))
            tempData->SetValue(i, j, aval);
        }
    }
  }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempNote, tempSubCaseNum, tempTime, tempNodeIdentifiers, tempNodes, tempData);
}

void IGESAppli_ToolNodalResults::WriteOwnParams(const occ::handle<IGESAppli_NodalResults>& ent,
                                                IGESData_IGESWriter&                       IW) const
{
  int nbnodes = ent->NbNodes();
  int nbdata  = ent->NbData();
  IW.Send(ent->Note());
  IW.Send(ent->SubCaseNumber());
  IW.Send(ent->Time());
  IW.Send(nbdata);
  IW.Send(nbnodes);
  for (int i = 1; i <= nbnodes; i++)
  {
    IW.Send(ent->NodeIdentifier(i));
    IW.Send(ent->Node(i));
    for (int j = 1; j <= nbdata; j++)
      IW.Send(ent->Data(i, j));
  }
}

void IGESAppli_ToolNodalResults::OwnShared(const occ::handle<IGESAppli_NodalResults>& ent,
                                           Interface_EntityIterator&                  iter) const
{
  int nbnodes = ent->NbNodes();
  iter.GetOneItem(ent->Note());
  for (int i = 1; i <= nbnodes; i++)
    iter.GetOneItem(ent->Node(i));
}

void IGESAppli_ToolNodalResults::OwnCopy(const occ::handle<IGESAppli_NodalResults>& another,
                                         const occ::handle<IGESAppli_NodalResults>& ent,
                                         Interface_CopyTool&                        TC) const
{
  DeclareAndCast(IGESDimen_GeneralNote, aNote, TC.Transferred(another->Note()));
  int                                   aSubCaseNum      = another->SubCaseNumber();
  double                                aTime            = another->Time();
  int                                   nbnodes          = another->NbNodes();
  int                                   nbval            = another->NbData();
  occ::handle<NCollection_HArray1<int>> aNodeIdentifiers = new NCollection_HArray1<int>(1, nbnodes);
  occ::handle<NCollection_HArray1<occ::handle<IGESAppli_Node>>> aNodes =
    new NCollection_HArray1<occ::handle<IGESAppli_Node>>(1, nbnodes);
  occ::handle<NCollection_HArray2<double>> aData =
    new NCollection_HArray2<double>(1, nbnodes, 1, nbval);

  for (int i = 1; i <= nbnodes; i++)
  {
    int aItem = another->NodeIdentifier(i);
    aNodeIdentifiers->SetValue(i, aItem);
    DeclareAndCast(IGESAppli_Node, anentity, TC.Transferred(another->Node(i)));
    aNodes->SetValue(i, anentity);
    for (int j = 1; j <= nbval; j++)
      aData->SetValue(i, j, another->Data(i, j));
  }

  ent->Init(aNote, aSubCaseNum, aTime, aNodeIdentifiers, aNodes, aData);
  ent->SetFormNumber(another->FormNumber());
}

IGESData_DirChecker IGESAppli_ToolNodalResults::DirChecker(
  const occ::handle<IGESAppli_NodalResults>& /* ent */) const
{
  IGESData_DirChecker DC(146, 0, 34); // Type = 146 Form No. = 0 to 34
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(03);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_ToolNodalResults::OwnCheck(const occ::handle<IGESAppli_NodalResults>& ent,
                                          const Interface_ShareTool&,
                                          occ::handle<Interface_Check>& ach) const
{
  int  FormNum = ent->FormNumber();
  int  nv      = ent->NbData();
  bool OK      = true;
  switch (FormNum)
  {
    case 0:
      if (nv < 0)
        OK = false;
      break;
    case 1:
      if (nv != 1)
        OK = false;
      break;
    case 2:
      if (nv != 1)
        OK = false;
      break;
    case 3:
      if (nv != 3)
        OK = false;
      break;
    case 4:
      if (nv != 6)
        OK = false;
      break;
    case 5:
      if (nv != 3)
        OK = false;
      break;
    case 6:
      if (nv != 3)
        OK = false;
      break;
    case 7:
      if (nv != 3)
        OK = false;
      break;
    case 8:
      if (nv != 3)
        OK = false;
      break;
    case 9:
      if (nv != 3)
        OK = false;
      break;
    case 10:
      if (nv != 1)
        OK = false;
      break;
    case 11:
      if (nv != 1)
        OK = false;
      break;
    case 12:
      if (nv != 3)
        OK = false;
      break;
    case 13:
      if (nv != 1)
        OK = false;
      break;
    case 14:
      if (nv != 1)
        OK = false;
      break;
    case 15:
      if (nv != 3)
        OK = false;
      break;
    case 16:
      if (nv != 1)
        OK = false;
      break;
    case 17:
      if (nv != 3)
        OK = false;
      break;
    case 18:
      if (nv != 3)
        OK = false;
      break;
    case 19:
      if (nv != 3)
        OK = false;
      break;
    case 20:
      if (nv != 3)
        OK = false;
      break;
    case 21:
      if (nv != 3)
        OK = false;
      break;
    case 22:
      if (nv != 3)
        OK = false;
      break;
    case 23:
      if (nv != 6)
        OK = false;
      break;
    case 24:
      if (nv != 6)
        OK = false;
      break;
    case 25:
      if (nv != 6)
        OK = false;
      break;
    case 26:
      if (nv != 6)
        OK = false;
      break;
    case 27:
      if (nv != 6)
        OK = false;
      break;
    case 28:
      if (nv != 6)
        OK = false;
      break;
    case 29:
      if (nv != 9)
        OK = false;
      break;
    case 30:
      if (nv != 9)
        OK = false;
      break;
    case 31:
      if (nv != 9)
        OK = false;
      break;
    case 32:
      if (nv != 9)
        OK = false;
      break;
    case 33:
      if (nv != 9)
        OK = false;
      break;
    case 34:
      if (nv != 9)
        OK = false;
      break;
    default:
      ach->AddFail("Incorrect Form Number");
      break;
  }
  if (!OK)
    ach->AddFail("Incorrect count of real values in array V for FEM node");
}

void IGESAppli_ToolNodalResults::OwnDump(const occ::handle<IGESAppli_NodalResults>& ent,
                                         const IGESData_IGESDumper&                 dumper,
                                         Standard_OStream&                          S,
                                         const int                                  level) const
{
  //  int nbnodes = ent->NbNodes();
  //  int nbdata  = ent->NbData ();
  S << "IGESAppli_NodalResults\n";

  S << "General Note : ";
  dumper.Dump(ent->Note(), S, (level <= 4) ? 0 : 1);
  S << "\n";
  S << "Analysis subcase number : " << ent->SubCaseNumber() << "  ";
  S << "Time used : " << ent->Time() << "\n";
  S << "No. of nodes : " << ent->NbNodes() << "  ";
  S << "No. of values for a node : " << ent->NbData() << "\n";
  S << "Node Identifiers :\n";
  S << "Nodes :\n";
  S << "Data : ";
  if (level < 6)
    S << " [ask level > 5]";
  //  IGESData_DumpRectVals(S ,-level,1, ent->NbData(),ent->Data);
  S << "\n";
  if (level > 4)
  {
    for (int i = 1; i <= ent->NbNodes(); i++)
    {
      S << "[" << i << "]: ";
      S << "NodeIdentifier : " << ent->NodeIdentifier(i) << "  ";
      S << "Node : ";
      dumper.Dump(ent->Node(i), S, 1);
      S << "\n";
      if (level < 6)
        continue;
      S << "Data : [ ";
      for (int j = 1; j <= ent->NbData(); j++)
        S << "  " << ent->Data(i, j);
      S << " ]\n";
    }
  }
}
