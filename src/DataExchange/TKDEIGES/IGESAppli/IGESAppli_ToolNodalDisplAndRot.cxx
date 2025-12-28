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

#include <gp_XYZ.hxx>
#include <IGESAppli_Node.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESAppli_NodalDisplAndRot.hxx>
#include <IGESAppli_ToolNodalDisplAndRot.hxx>
#include <IGESBasic_HArray1OfHArray1OfXYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
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
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Integer.hxx>

IGESAppli_ToolNodalDisplAndRot::IGESAppli_ToolNodalDisplAndRot() = default;

void IGESAppli_ToolNodalDisplAndRot::ReadOwnParams(
  const occ::handle<IGESAppli_NodalDisplAndRot>& ent,
  const occ::handle<IGESData_IGESReaderData>&    IR,
  IGESData_ParamReader&                          PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed
  int                                                                  nbcases = 0;
  int                                                                  nbnodes = 0;
  occ::handle<NCollection_HArray1<occ::handle<IGESDimen_GeneralNote>>> tempNotes;
  occ::handle<NCollection_HArray1<int>>                                tempNodeIdentifiers;
  occ::handle<NCollection_HArray1<occ::handle<IGESAppli_Node>>>        tempNodes;
  occ::handle<IGESBasic_HArray1OfHArray1OfXYZ>                         tempTransParam;
  occ::handle<IGESBasic_HArray1OfHArray1OfXYZ>                         tempRotParam;

  bool tempFlag = PR.ReadInteger(PR.Current(), "No. of analysis cases", nbcases);
  if (tempFlag)
  {
    tempNotes = new NCollection_HArray1<occ::handle<IGESDimen_GeneralNote>>(1, nbcases);
    for (int i = 1; i <= nbcases; i++)
    {
      occ::handle<IGESDimen_GeneralNote> anentity;
      // szv#4:S4163:12Mar99 moved in if
      if (PR.ReadEntity(IR,
                        PR.Current(),
                        "General Note",
                        STANDARD_TYPE(IGESDimen_GeneralNote),
                        anentity))
        tempNotes->SetValue(i, anentity);
    }
  }
  // szv#4:S4163:12Mar99 moved in if
  if (PR.ReadInteger(PR.Current(), "No. of nodes", nbnodes))
  {
    tempNodeIdentifiers = new NCollection_HArray1<int>(1, nbnodes);
    tempNodes           = new NCollection_HArray1<occ::handle<IGESAppli_Node>>(1, nbnodes);
    tempTransParam      = new IGESBasic_HArray1OfHArray1OfXYZ(1, nbnodes);
    tempRotParam        = new IGESBasic_HArray1OfHArray1OfXYZ(1, nbnodes);
    for (int j = 1; j <= nbnodes; j++)
    {
      occ::handle<IGESAppli_Node> aNode = new IGESAppli_Node();
      int                         nodeno;
      if (PR.ReadInteger(PR.Current(), "Node number", nodeno))
        tempNodeIdentifiers->SetValue(j, nodeno);
      if (PR.ReadEntity(IR,
                        PR.Current(),
                        "Node Directory Entry",
                        STANDARD_TYPE(IGESAppli_Node),
                        aNode))
        tempNodes->SetValue(j, aNode);

      occ::handle<NCollection_HArray1<gp_XYZ>> tempArray1;
      occ::handle<NCollection_HArray1<gp_XYZ>> tempArray;
      if (tempFlag)
      {
        // tempFlag indicates if nbcases has been properly read
        tempArray1 = new NCollection_HArray1<gp_XYZ>(1, nbcases);
        tempArray  = new NCollection_HArray1<gp_XYZ>(1, nbcases);
        for (int k = 1; k <= nbcases; k++)
        {
          gp_XYZ atrans, arot;
          if (PR.ReadXYZ(PR.CurrentList(1, 3), "Translation XYZ", atrans))
            tempArray->SetValue(k, atrans);
          if (PR.ReadXYZ(PR.CurrentList(1, 3), "Rotational XYZ", arot))
            tempArray1->SetValue(k, arot);
        }
      }
      tempTransParam->SetValue(j, tempArray);
      tempRotParam->SetValue(j, tempArray1);
    }
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempNotes, tempNodeIdentifiers, tempNodes, tempRotParam, tempTransParam);
}

void IGESAppli_ToolNodalDisplAndRot::WriteOwnParams(
  const occ::handle<IGESAppli_NodalDisplAndRot>& ent,
  IGESData_IGESWriter&                           IW) const
{
  int nbcases = ent->NbCases();
  int nbnodes = ent->NbNodes();

  IW.Send(nbcases);
  for (int i = 1; i <= nbcases; i++)
    IW.Send(ent->Note(i));
  IW.Send(nbnodes);
  for (int j = 1; j <= nbnodes; j++)
  {
    IW.Send(ent->NodeIdentifier(j));
    IW.Send(ent->Node(j));
    for (int k = 1; k <= nbcases; k++)
    {
      IW.Send(ent->TranslationParameter(j, k).X());
      IW.Send(ent->TranslationParameter(j, k).Y());
      IW.Send(ent->TranslationParameter(j, k).Z());
      IW.Send(ent->RotationalParameter(j, k).X());
      IW.Send(ent->RotationalParameter(j, k).Y());
      IW.Send(ent->RotationalParameter(j, k).Z());
    }
  }
}

void IGESAppli_ToolNodalDisplAndRot::OwnShared(const occ::handle<IGESAppli_NodalDisplAndRot>& ent,
                                               Interface_EntityIterator& iter) const
{
  int nbcases = ent->NbCases();
  int nbnodes = ent->NbNodes();

  for (int i = 1; i <= nbcases; i++)
    iter.GetOneItem(ent->Note(i));
  for (int j = 1; j <= nbnodes; j++)
    iter.GetOneItem(ent->Node(j));
}

void IGESAppli_ToolNodalDisplAndRot::OwnCopy(const occ::handle<IGESAppli_NodalDisplAndRot>& another,
                                             const occ::handle<IGESAppli_NodalDisplAndRot>& ent,
                                             Interface_CopyTool& TC) const
{
  int                                                                  nbcases = another->NbCases();
  int                                                                  nbnodes = another->NbNodes();
  occ::handle<NCollection_HArray1<occ::handle<IGESDimen_GeneralNote>>> aNotes =
    new NCollection_HArray1<occ::handle<IGESDimen_GeneralNote>>(1, nbcases);
  occ::handle<NCollection_HArray1<int>> aNodeIdentifiers = new NCollection_HArray1<int>(1, nbnodes);
  occ::handle<NCollection_HArray1<occ::handle<IGESAppli_Node>>> aNodes =
    new NCollection_HArray1<occ::handle<IGESAppli_Node>>(1, nbnodes);
  occ::handle<IGESBasic_HArray1OfHArray1OfXYZ> aTransParam =
    new IGESBasic_HArray1OfHArray1OfXYZ(1, nbnodes);
  occ::handle<IGESBasic_HArray1OfHArray1OfXYZ> aRotParam =
    new IGESBasic_HArray1OfHArray1OfXYZ(1, nbnodes);

  for (int i = 1; i <= nbnodes; i++)
  {
    aNodeIdentifiers->SetValue(i, (another->NodeIdentifier(i)));
    DeclareAndCast(IGESAppli_Node, anitem, TC.Transferred(another->Node(i)));
    aNodes->SetValue(i, anitem);
  }
  for (int j = 1; j <= nbcases; j++)
  {
    DeclareAndCast(IGESDimen_GeneralNote, anitem, TC.Transferred(another->Note(j)));
    aNotes->SetValue(j, anitem);
  }
  for (int n = 1; n <= nbnodes; n++)
  {
    occ::handle<NCollection_HArray1<gp_XYZ>> tempArray1 =
      new NCollection_HArray1<gp_XYZ>(1, nbcases);
    occ::handle<NCollection_HArray1<gp_XYZ>> tempArray2 =
      new NCollection_HArray1<gp_XYZ>(1, nbcases);
    for (int k = 1; k <= nbcases; k++)
    {
      tempArray1->SetValue(k, another->TranslationParameter(n, k));
      tempArray2->SetValue(k, another->RotationalParameter(n, k));
    }
    aTransParam->SetValue(n, tempArray1);
    aRotParam->SetValue(n, tempArray2);
  }

  ent->Init(aNotes, aNodeIdentifiers, aNodes, aRotParam, aTransParam);
}

IGESData_DirChecker IGESAppli_ToolNodalDisplAndRot::DirChecker(
  const occ::handle<IGESAppli_NodalDisplAndRot>& /* ent */) const
{
  IGESData_DirChecker DC(138, 0); // Form no = 0 & Type = 138
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_ToolNodalDisplAndRot::OwnCheck(
  const occ::handle<IGESAppli_NodalDisplAndRot>& /* ent */,
  const Interface_ShareTool&,
  occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESAppli_ToolNodalDisplAndRot::OwnDump(const occ::handle<IGESAppli_NodalDisplAndRot>& ent,
                                             const IGESData_IGESDumper&                     dumper,
                                             Standard_OStream&                              S,
                                             const int level) const
{
  S << "IGESAppli_NodalDisplAndRot\n";

  int nbcases = ent->NbCases();
  int nbnodes = ent->NbNodes();

  S << "No. of analysis cases : " << nbcases << "\n";
  S << "General Notes : ";
  IGESData_DumpEntities(S, dumper, level, 1, nbcases, ent->Note);
  S << "\n";

  //  gp_GTrsf loca;  // true location n.u.
  switch (level)
  {
    case 4:
      S << "Nodes : ";
      IGESData_DumpEntities(S, dumper, -level, 1, nbnodes, ent->Node);
      S << "NodeIdentifiers : ";
      IGESData_DumpVals(S, -level, 1, nbnodes, ent->NodeIdentifier);
      S << "Translation Parameters : ";
      //  ?? Possibly JAGGED ??
      //      IGESData_DumpListXYZL(S,-level,1,nbcases,ent->TranslationParameter,loca);
      S << "Rotational Parameters : ";
      //      IGESData_DumpListXYZL(S,-level,1,nbcases,ent->RotationalParameter,loca);
      S << "  TO BE DONE" << std::endl;
      break;
    case 5:
    case 6: {
      for (int i = 1; i <= nbcases; i++)
      {
        S << "[" << i << "]: ";
        dumper.Dump(ent->Note(i), S, 1);
        S << std::endl;
      }
      S << "Nodes :\n";
      S << "Node Identifiers :\n";
      S << "Translation Parameters :\n";
      S << "Rotational Parameters : Count = " << nbcases;
      //     IGESData_DumpListXYZL(S,-level,1,nbcases,ent->RotationalParameter,loca);
      S << "\n";
      for (int j = 1; j <= nbnodes; j++)
      {
        S << "[" << j << "]:  -  NodeIdentifier : " << ent->NodeIdentifier(j) << "  -  Node :\n";
        S << "Node : ";
        dumper.Dump(ent->Node(j), S, 1);
        S << "  -  Parameters :\n";
        for (int k = 1; k <= nbcases; k++)
        {
          S << " [" << k << "]:\n";
          S << "Translational Parameter : X=" << ent->TranslationParameter(j, k).X()
            << ", Y=" << ent->TranslationParameter(j, k).Y()
            << ", Z=" << ent->TranslationParameter(j, k).Z() << "\n";
          S << "Rotational Parameter : X=" << ent->RotationalParameter(j, k).X()
            << ", Y=" << ent->RotationalParameter(j, k).Y()
            << ", Z=" << ent->RotationalParameter(j, k).Z();
          if (k == nbcases)
            S << "] ";
          S << "\n";
        }
      }
    }
    break;
    default:
      break;
  }
  S << std::endl;
}
