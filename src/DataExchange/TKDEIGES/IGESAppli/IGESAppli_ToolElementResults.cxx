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
// UNFINISHED & UNSTARTED

#include <IGESAppli_ElementResults.hxx>
#include <IGESAppli_FiniteElement.hxx>
#include <IGESAppli_FiniteElement.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESAppli_ToolElementResults.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESBasic_HArray1OfHArray1OfReal.hxx>
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
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include <stdio.h>

IGESAppli_ToolElementResults::IGESAppli_ToolElementResults() {}

void IGESAppli_ToolElementResults::ReadOwnParams(const occ::handle<IGESAppli_ElementResults>& ent,
                                                 const occ::handle<IGESData_IGESReaderData>&  IR,
                                                 IGESData_ParamReader&                   PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed
  int                            num = 0;
  occ::handle<IGESDimen_GeneralNote>               aNote;
  int                            aSubcaseNumber    = 0;
  double                               aTime             = 0;
  int                            nv                = 0;
  int                            aResultReportFlag = 0;
  occ::handle<NCollection_HArray1<int>>            allElementIdentifiers;
  occ::handle<NCollection_HArray1<occ::handle<IGESAppli_FiniteElement>>>    allElements;
  occ::handle<NCollection_HArray1<int>>            allElementTopologyType;
  occ::handle<NCollection_HArray1<int>>            allNbLayers;
  occ::handle<NCollection_HArray1<int>>            allDataLayerFlag;
  occ::handle<NCollection_HArray1<int>>            allNbResultDataLocs;
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger> allResultDataLoc;
  occ::handle<IGESBasic_HArray1OfHArray1OfReal>    allResultData;

  // szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadEntity(IR, PR.Current(), "General Note", STANDARD_TYPE(IGESDimen_GeneralNote), aNote);
  PR.ReadInteger(PR.Current(), "Subcase Number", aSubcaseNumber);
  PR.ReadReal(PR.Current(), "Time", aTime);
  PR.ReadInteger(PR.Current(), "Number of Result Values", nv);
  PR.ReadInteger(PR.Current(), "Result Report Flag", aResultReportFlag);
  PR.ReadInteger(PR.Current(), "Number of Finite Elements", num);

  if (num > 0)
  {
    allElementIdentifiers  = new NCollection_HArray1<int>(1, num);
    allElements            = new NCollection_HArray1<occ::handle<IGESAppli_FiniteElement>>(1, num);
    allElementTopologyType = new NCollection_HArray1<int>(1, num);
    allNbLayers            = new NCollection_HArray1<int>(1, num);
    allDataLayerFlag       = new NCollection_HArray1<int>(1, num);
    allNbResultDataLocs    = new NCollection_HArray1<int>(1, num);
    allResultDataLoc       = new IGESBasic_HArray1OfHArray1OfInteger(1, num);
    allResultData          = new IGESBasic_HArray1OfHArray1OfReal(1, num);
    // ??  WHAT ABOUT FILLING  ?
  }
  else
    PR.AddFail("Number of Finite Elements: Not Positive");

  for (int nume = 1; nume <= num; nume++)
  {
    int ival, nl, nrl, numv;
    ival = nl = nrl = numv = 0;
    occ::handle<IGESAppli_FiniteElement>  aFEM;
    occ::handle<NCollection_HArray1<int>> rdrl;
    occ::handle<NCollection_HArray1<double>>    vres;
    // szv#4:S4163:12Mar99 moved in if
    if (PR.ReadInteger(PR.Current(), "FEM Element Identifier", ival))
      allElementIdentifiers->SetValue(nume, ival);
    if (PR.ReadEntity(IR,
                      PR.Current(),
                      "FEM Element Entity",
                      STANDARD_TYPE(IGESAppli_FiniteElement),
                      aFEM))
      allElements->SetValue(nume, aFEM);
    if (PR.ReadInteger(PR.Current(), "FEM Element Topology Type", ival))
      allElementTopologyType->SetValue(nume, ival);
    if (PR.ReadInteger(PR.Current(), "Nb. of layers", nl))
      allNbLayers->SetValue(nume, nl);
    if (PR.ReadInteger(PR.Current(), "Data Layer Flag", ival))
      allDataLayerFlag->SetValue(nume, ival);
    if (PR.ReadInteger(PR.Current(), "Nb. of result data locations", nrl))
      allNbResultDataLocs->SetValue(nume, nrl);
    if (PR.ReadInts(PR.CurrentList(nrl), "Result data locations", rdrl))
      allResultDataLoc->SetValue(nume, rdrl);
    // clang-format off
    PR.ReadInteger (PR.Current(), "Nb. of result data locations", numv); //szv#4:S4163:12Mar99 `st=` not needed
    // clang-format on
    if (PR.ReadReals(PR.CurrentList(numv), "Result Data", vres))
      allResultData->SetValue(nume, vres);
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(aNote,
            aSubcaseNumber,
            aTime,
            nv,
            aResultReportFlag,
            allElementIdentifiers,
            allElements,
            allElementTopologyType,
            allNbLayers,
            allDataLayerFlag,
            allNbResultDataLocs,
            allResultDataLoc,
            allResultData);
}

void IGESAppli_ToolElementResults::WriteOwnParams(const occ::handle<IGESAppli_ElementResults>& ent,
                                                  IGESData_IGESWriter&                    IW) const
{
  int i, j;
  int num = ent->NbElements();
  IW.Send(ent->Note());
  IW.Send(ent->SubCaseNumber());
  IW.Send(ent->Time());
  IW.Send(ent->NbResultValues());
  IW.Send(ent->ResultReportFlag());
  IW.Send(num);
  for (i = 1; i <= num; i++)
  {
    int nl   = ent->NbLayers(i);
    int nrl  = ent->NbResultDataLocs(i);
    int numv = ent->NbResults(i);
    IW.Send(ent->ElementIdentifier(i));
    IW.Send(ent->Element(i));
    IW.Send(ent->ElementTopologyType(i));
    IW.Send(nl);
    IW.Send(ent->DataLayerFlag(i));
    IW.Send(nrl);
    for (j = 1; j <= nrl; j++)
      IW.Send(ent->ResultDataLoc(i, j));
    IW.Send(numv);
    for (j = 1; j <= numv; j++)
      IW.Send(ent->ResultData(i, j));
  }
}

void IGESAppli_ToolElementResults::OwnShared(const occ::handle<IGESAppli_ElementResults>& ent,
                                             Interface_EntityIterator&               iter) const
{
  int i;
  int num = ent->NbElements();
  iter.GetOneItem(ent->Note());
  for (i = 1; i <= num; i++)
    iter.GetOneItem(ent->Element(i));
}

void IGESAppli_ToolElementResults::OwnCopy(const occ::handle<IGESAppli_ElementResults>& another,
                                           const occ::handle<IGESAppli_ElementResults>& ent,
                                           Interface_CopyTool&                     TC) const
{
  int i, j;
  int num = another->NbElements();
  DeclareAndCast(IGESDimen_GeneralNote, aNote, TC.Transferred(another->Note()));
  int aSubcaseNumber    = another->SubCaseNumber();
  double    aTime             = another->Time();
  int nv                = another->NbResultValues();
  int aResultReportFlag = another->ResultReportFlag();

  occ::handle<NCollection_HArray1<int>>            allElementIdentifiers;
  occ::handle<NCollection_HArray1<occ::handle<IGESAppli_FiniteElement>>>    allElements;
  occ::handle<NCollection_HArray1<int>>            allElementTopologyType;
  occ::handle<NCollection_HArray1<int>>            allNbLayers;
  occ::handle<NCollection_HArray1<int>>            allDataLayerFlag;
  occ::handle<NCollection_HArray1<int>>            allNbResultDataLocs;
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger> allResultDataLoc;
  occ::handle<IGESBasic_HArray1OfHArray1OfReal>    allResultData;
  if (num > 0)
  {
    allElementIdentifiers  = new NCollection_HArray1<int>(1, num);
    allElements            = new NCollection_HArray1<occ::handle<IGESAppli_FiniteElement>>(1, num);
    allElementTopologyType = new NCollection_HArray1<int>(1, num);
    allNbLayers            = new NCollection_HArray1<int>(1, num);
    allDataLayerFlag       = new NCollection_HArray1<int>(1, num);
    allNbResultDataLocs    = new NCollection_HArray1<int>(1, num);
    allResultDataLoc       = new IGESBasic_HArray1OfHArray1OfInteger(1, num);
    allResultData          = new IGESBasic_HArray1OfHArray1OfReal(1, num);
  }
  for (i = 1; i <= num; i++)
  {
    int nl, nrl, numv;
    allElementIdentifiers->SetValue(i, another->ElementIdentifier(i));
    allElements->SetValue(i,
                          GetCasted(IGESAppli_FiniteElement, TC.Transferred(another->Element(i))));
    allElementTopologyType->SetValue(i, another->ElementTopologyType(i));
    nl = another->NbLayers(i);
    allNbLayers->SetValue(i, nl);
    allDataLayerFlag->SetValue(i, another->DataLayerFlag(i));
    nrl = another->NbResultDataLocs(i);
    allNbResultDataLocs->SetValue(i, nrl);
    occ::handle<NCollection_HArray1<int>> rdrl = new NCollection_HArray1<int>(1, nrl);
    allResultDataLoc->SetValue(i, rdrl);
    for (j = 1; j <= nrl; j++)
      rdrl->SetValue(j, another->ResultDataLoc(i, j));
    numv                               = another->NbResults(i);
    occ::handle<NCollection_HArray1<double>> vres = new NCollection_HArray1<double>(1, numv);
    for (j = 1; j <= numv; j++)
      vres->SetValue(j, another->ResultData(i, j));
    allResultData->SetValue(i, vres);
  }
  ent->Init(aNote,
            aSubcaseNumber,
            aTime,
            nv,
            aResultReportFlag,
            allElementIdentifiers,
            allElements,
            allElementTopologyType,
            allNbLayers,
            allDataLayerFlag,
            allNbResultDataLocs,
            allResultDataLoc,
            allResultData);
  ent->SetFormNumber(another->FormNumber());
}

IGESData_DirChecker IGESAppli_ToolElementResults::DirChecker(
  const occ::handle<IGESAppli_ElementResults>& /* ent  */) const
{
  IGESData_DirChecker DC(148, 0, 34);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(3);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_ToolElementResults::OwnCheck(const occ::handle<IGESAppli_ElementResults>& ent,
                                            const Interface_ShareTool&,
                                            occ::handle<Interface_Check>& ach) const
// UNFINISHED
{
  int rrf = ent->ResultReportFlag();
  if (rrf < 0 || rrf > 3)
    ach->AddFail("Result Report Flag not in [0-3]");
  int nv = ent->NbResultValues();
  bool OK = true;
  switch (ent->FormNumber())
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
  int ne = ent->NbElements();
  for (int i = 1; i <= ne; i++)
  {
    char             mess[100];
    int dlf = ent->DataLayerFlag(i);
    int nl  = ent->NbLayers(i);
    int nrl = ent->NbResultDataLocs(i);
    if (dlf < 0 || dlf > 4)
      ach->AddFail("One of the Data Layer Flags not in [0-4]");
    if (dlf < 4 && ent->NbLayers(i) != 1)
    {
      Sprintf(mess, "Nb. of Layers n0.%d not ONE while Data Layer Flag is in [0-3]", i);
      ach->AddFail(mess);
    }
    if (rrf == 1 || rrf == 2)
      if (nrl != 1 || ent->ResultDataLoc(i, 1) != 0)
      {
        Sprintf(mess, "Result Data Locs n0.%d incorrect for Result Report = 1 or 2", i);
        ach->AddFail(mess);
      }
    if (ent->NbResults(i) != (nv * nl * nrl))
    {
      Sprintf(mess, "Nb. of results for Element n0.%d incorrect, should be %d", i, nv * nl * nrl);
      ach->AddFail(mess);
    }
  }
}

void IGESAppli_ToolElementResults::OwnDump(const occ::handle<IGESAppli_ElementResults>& ent,
                                           const IGESData_IGESDumper&              dumper,
                                           Standard_OStream&                       S,
                                           const int level) const // UNFINISHED
{
  int sublevel = (level > 4) ? 1 : 0;
  S << "IGESAppli_ElementResults\n";
  S << "General Note   : ";
  dumper.Dump(ent->Note(), S, sublevel);
  S << "\n";
  S << "Subcase Number : " << ent->SubCaseNumber() << "\n";
  S << "Time           : " << ent->Time() << "\n";
  S << "Number of Result Values : " << ent->NbResultValues() << "\n";
  S << "Result Report Flag  : " << ent->ResultReportFlag() << "\n";
  S << "Element Identifiers : ";
  IGESData_DumpVals(S, level, 1, ent->NbElements(), ent->ElementIdentifier);
  S << "\nElements : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbElements(), ent->Element);
  S << "\nElement Topology Types : ";
  IGESData_DumpVals(S, level, 1, ent->NbElements(), ent->ElementTopologyType);
  S << "\nNumbers of Layers      : ";
  IGESData_DumpVals(S, level, 1, ent->NbElements(), ent->NbLayers);
  S << "\nData Layer Flags       : ";
  IGESData_DumpVals(S, level, 1, ent->NbElements(), ent->DataLayerFlag);
  S << "\nNumbers of Result Data Locations : ";
  IGESData_DumpVals(S, level, 1, ent->NbElements(), ent->NbResultDataLocs);
  S << "\nResult Data Locations : ";
  S << " TO BE DONE  ";

  //  ??  TO VERIFY CLOSELY, not at all sure this is correct
  //      see also Write and Copy
  if (level <= 4)
    S << " [ ask level > 4 for more, > 5 for complete ]\n";
  else
  {
    int i; // svv Jan 10 2000 : porting on DEC
    for (i = 1; i <= ent->NbElements(); i++)
    {
      int nloc = ent->NbResultDataLocs(i);
      S << " [" << i << ":NbLoc=" << nloc << "]:";
      for (int j = 1; j <= nloc; j++)
        S << " " << ent->ResultDataLoc(i, j);
      S << "\n";
    }
    S << "Result Data : ";

    for (i = 1; i <= ent->NbElements(); i++)
    {
      int nres = ent->NbResults(i);
      S << " [" << i << ":NbRes=" << nres << "]:";
      if (level <= 5)
        S << " [ ask level > 5 for complete Data ]\n";
      else
      {
        for (int j = 1; j <= nres; j++)
          S << " " << ent->ResultData(i, j); // ?? is it all ?? UNFINISHED
      }
    }
  }
}
