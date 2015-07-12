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
#include <IGESAppli_HArray1OfFiniteElement.hxx>
#include <IGESAppli_ToolElementResults.hxx>
#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESBasic_HArray1OfHArray1OfReal.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

#include <stdio.h>
IGESAppli_ToolElementResults::IGESAppli_ToolElementResults ()    {  }

     
void  IGESAppli_ToolElementResults::ReadOwnParams
  (const Handle(IGESAppli_ElementResults)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR)  const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer num = 0;
  Handle(IGESDimen_GeneralNote) aNote;
  Standard_Integer aSubcaseNumber = 0;
  Standard_Real    aTime = 0;
  Standard_Integer nv = 0;
  Standard_Integer aResultReportFlag = 0;
  Handle(TColStd_HArray1OfInteger) allElementIdentifiers;
  Handle(IGESAppli_HArray1OfFiniteElement)    allElements;
  Handle(TColStd_HArray1OfInteger) allElementTopologyType;
  Handle(TColStd_HArray1OfInteger) allNbLayers;
  Handle(TColStd_HArray1OfInteger) allDataLayerFlag;
  Handle(TColStd_HArray1OfInteger) allNbResultDataLocs;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) allResultDataLoc;
  Handle(IGESBasic_HArray1OfHArray1OfReal)    allResultData;

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadEntity(IR, PR.Current(), "General Note", STANDARD_TYPE(IGESDimen_GeneralNote), aNote);
  PR.ReadInteger(PR.Current(), "Subcase Number", aSubcaseNumber);
  PR.ReadReal(PR.Current(), "Time", aTime);
  PR.ReadInteger(PR.Current(), "Number of Result Values", nv);
  PR.ReadInteger(PR.Current(), "Result Report Flag", aResultReportFlag);
  PR.ReadInteger(PR.Current(), "Number of Finite Elements", num);

  if (num > 0) {
    allElementIdentifiers  = new TColStd_HArray1OfInteger(1, num);
    allElements            = new IGESAppli_HArray1OfFiniteElement(1, num);
    allElementTopologyType = new TColStd_HArray1OfInteger(1, num);
    allNbLayers            = new TColStd_HArray1OfInteger(1, num);
    allDataLayerFlag       = new TColStd_HArray1OfInteger(1, num);
    allNbResultDataLocs    = new TColStd_HArray1OfInteger(1, num);
    allResultDataLoc       = new IGESBasic_HArray1OfHArray1OfInteger(1, num);
    allResultData          = new IGESBasic_HArray1OfHArray1OfReal(1, num);
// ??  WHAT ABOUT FILLING  ?
  }
  else PR.AddFail("Number of Finite Elements: Not Positive");

  for (Standard_Integer nume = 1; nume <= num; nume ++) {
    Standard_Integer ival, nl, nrl, numv;
    ival = nl = nrl = numv = 0;
    Handle(IGESAppli_FiniteElement) aFEM;
    Handle(TColStd_HArray1OfInteger) rdrl;
    Handle(TColStd_HArray1OfReal) vres;
    //szv#4:S4163:12Mar99 moved in if
    if (PR.ReadInteger (PR.Current(), "FEM Element Identifier", ival))
      allElementIdentifiers->SetValue(nume,ival);
    if (PR.ReadEntity (IR,PR.Current(), "FEM Element Entity", STANDARD_TYPE(IGESAppli_FiniteElement), aFEM))
      allElements->SetValue (nume,aFEM);
    if (PR.ReadInteger (PR.Current(), "FEM Element Topology Type", ival))
      allElementTopologyType->SetValue (nume,ival);
    if (PR.ReadInteger (PR.Current(), "Nb. of layers", nl))
      allNbLayers->SetValue (nume,nl);
    if (PR.ReadInteger (PR.Current(), "Data Layer Flag", ival))
      allDataLayerFlag->SetValue (nume,ival);
    if (PR.ReadInteger (PR.Current(), "Nb. of result data locations", nrl))
      allNbResultDataLocs->SetValue (nume,nrl);
    if (PR.ReadInts (PR.CurrentList(nrl), "Result data locations", rdrl))
      allResultDataLoc->SetValue (nume,rdrl);
    PR.ReadInteger (PR.Current(), "Nb. of result data locations", numv); //szv#4:S4163:12Mar99 `st=` not needed
    if (PR.ReadReals (PR.CurrentList(numv), "Result Data", vres))
      allResultData->SetValue(nume,vres);
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(aNote, aSubcaseNumber, aTime, nv, aResultReportFlag,
	    allElementIdentifiers, allElements, allElementTopologyType,
	    allNbLayers, allDataLayerFlag, allNbResultDataLocs,
	    allResultDataLoc, allResultData);
}

void  IGESAppli_ToolElementResults::WriteOwnParams
  (const Handle(IGESAppli_ElementResults)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i,j;  Standard_Integer num = ent->NbElements();
  IW.Send(ent->Note());
  IW.Send(ent->SubCaseNumber());
  IW.Send(ent->Time());
  IW.Send(ent->NbResultValues());
  IW.Send(ent->ResultReportFlag());
  IW.Send(num);
  for ( i = 1; i <= num; i++ ) {
    Standard_Integer nl   = ent->NbLayers(i);
    Standard_Integer nrl  = ent->NbResultDataLocs(i);
    Standard_Integer numv = ent->NbResults(i);
    IW.Send(ent->ElementIdentifier(i));
    IW.Send(ent->Element(i));
    IW.Send(ent->ElementTopologyType(i));
    IW.Send(nl);
    IW.Send(ent->DataLayerFlag(i));
    IW.Send(nrl);
    for (j = 1; j <= nrl;  j ++) IW.Send(ent->ResultDataLoc(i,j));
    IW.Send(numv);
    for (j = 1; j <= numv; j ++) IW.Send(ent->ResultData(i,j));
  }
}

void  IGESAppli_ToolElementResults::OwnShared
  (const Handle(IGESAppli_ElementResults)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer i; Standard_Integer num = ent->NbElements();
  iter.GetOneItem(ent->Note());
  for ( i = 1; i <= num; i++ )
    iter.GetOneItem(ent->Element(i));
}

void  IGESAppli_ToolElementResults::OwnCopy
  (const Handle(IGESAppli_ElementResults)& another,
   const Handle(IGESAppli_ElementResults)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer i,j;
  Standard_Integer num = another->NbElements();
  DeclareAndCast(IGESDimen_GeneralNote,aNote,TC.Transferred(another->Note()));
  Standard_Integer aSubcaseNumber = another->SubCaseNumber();
  Standard_Real    aTime = another->Time();
  Standard_Integer nv    = another->NbResultValues();
  Standard_Integer aResultReportFlag = another->ResultReportFlag();

  Handle(TColStd_HArray1OfInteger) allElementIdentifiers;
  Handle(IGESAppli_HArray1OfFiniteElement)    allElements;
  Handle(TColStd_HArray1OfInteger) allElementTopologyType;
  Handle(TColStd_HArray1OfInteger) allNbLayers;
  Handle(TColStd_HArray1OfInteger) allDataLayerFlag;
  Handle(TColStd_HArray1OfInteger) allNbResultDataLocs;
  Handle(IGESBasic_HArray1OfHArray1OfInteger) allResultDataLoc;
  Handle(IGESBasic_HArray1OfHArray1OfReal)    allResultData;
  if (num > 0) {
    allElementIdentifiers  = new TColStd_HArray1OfInteger(1, num);
    allElements            = new IGESAppli_HArray1OfFiniteElement(1, num);
    allElementTopologyType = new TColStd_HArray1OfInteger(1, num);
    allNbLayers            = new TColStd_HArray1OfInteger(1, num);
    allDataLayerFlag       = new TColStd_HArray1OfInteger(1, num);
    allNbResultDataLocs    = new TColStd_HArray1OfInteger(1, num);
    allResultDataLoc       = new IGESBasic_HArray1OfHArray1OfInteger(1, num);
    allResultData          = new IGESBasic_HArray1OfHArray1OfReal(1, num);
  }
  for (i = 1; i <= num; i ++) {
    Standard_Integer nl,nrl,numv;
    allElementIdentifiers->SetValue(i,another->ElementIdentifier(i));
    allElements->SetValue
      (i,GetCasted(IGESAppli_FiniteElement,TC.Transferred(another->Element(i))));
    allElementTopologyType->SetValue(i,another->ElementTopologyType(i));
    nl   = another->NbLayers(i);
    allNbLayers->SetValue(i,nl);
    allDataLayerFlag->SetValue(i,another->DataLayerFlag(i));
    nrl  = another->NbResultDataLocs(i);
    allNbResultDataLocs->SetValue(i,nrl);
    Handle(TColStd_HArray1OfInteger) rdrl = new TColStd_HArray1OfInteger(1,nrl);
    allResultDataLoc->SetValue(i,rdrl);
    for (j = 1; j <= nrl; j ++) rdrl->SetValue(j,another->ResultDataLoc(i,j));
    numv = another->NbResults(i);
    Handle(TColStd_HArray1OfReal) vres = new TColStd_HArray1OfReal(1,numv);
    for (j = 1; j <= numv; j ++) vres->SetValue(j,another->ResultData(i,j));
    allResultData->SetValue(i,vres);
  }
  ent->Init
    (aNote, aSubcaseNumber, aTime, nv, aResultReportFlag,
     allElementIdentifiers, allElements, allElementTopologyType,
     allNbLayers, allDataLayerFlag, allNbResultDataLocs,
     allResultDataLoc, allResultData);
  ent->SetFormNumber(another->FormNumber());
}


IGESData_DirChecker  IGESAppli_ToolElementResults::DirChecker
  (const Handle(IGESAppli_ElementResults)& /* ent  */) const
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

void  IGESAppli_ToolElementResults::OwnCheck
  (const Handle(IGESAppli_ElementResults)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
  // UNFINISHED
{
  Standard_Integer rrf = ent->ResultReportFlag();
  if (rrf < 0 || rrf > 3) ach->AddFail("Result Report Flag not in [0-3]");
  Standard_Integer nv = ent->NbResultValues();
  Standard_Boolean OK = Standard_True;
  switch (ent->FormNumber()) {
    case  0 : if (nv <  0) OK = Standard_False;  break;
    case  1 : if (nv != 1) OK = Standard_False;  break;
    case  2 : if (nv != 1) OK = Standard_False;  break;
    case  3 : if (nv != 3) OK = Standard_False;  break;
    case  4 : if (nv != 6) OK = Standard_False;  break;
    case  5 : if (nv != 3) OK = Standard_False;  break;
    case  6 : if (nv != 3) OK = Standard_False;  break;
    case  7 : if (nv != 3) OK = Standard_False;  break;
    case  8 : if (nv != 3) OK = Standard_False;  break;
    case  9 : if (nv != 3) OK = Standard_False;  break;
    case 10 : if (nv != 1) OK = Standard_False;  break;
    case 11 : if (nv != 1) OK = Standard_False;  break;
    case 12 : if (nv != 3) OK = Standard_False;  break;
    case 13 : if (nv != 1) OK = Standard_False;  break;
    case 14 : if (nv != 1) OK = Standard_False;  break;
    case 15 : if (nv != 3) OK = Standard_False;  break;
    case 16 : if (nv != 1) OK = Standard_False;  break;
    case 17 : if (nv != 3) OK = Standard_False;  break;
    case 18 : if (nv != 3) OK = Standard_False;  break;
    case 19 : if (nv != 3) OK = Standard_False;  break;
    case 20 : if (nv != 3) OK = Standard_False;  break;
    case 21 : if (nv != 3) OK = Standard_False;  break;
    case 22 : if (nv != 3) OK = Standard_False;  break;
    case 23 : if (nv != 6) OK = Standard_False;  break;
    case 24 : if (nv != 6) OK = Standard_False;  break;
    case 25 : if (nv != 6) OK = Standard_False;  break;
    case 26 : if (nv != 6) OK = Standard_False;  break;
    case 27 : if (nv != 6) OK = Standard_False;  break;
    case 28 : if (nv != 6) OK = Standard_False;  break;
    case 29 : if (nv != 9) OK = Standard_False;  break;
    case 30 : if (nv != 9) OK = Standard_False;  break;
    case 31 : if (nv != 9) OK = Standard_False;  break;
    case 32 : if (nv != 9) OK = Standard_False;  break;
    case 33 : if (nv != 9) OK = Standard_False;  break;
    case 34 : if (nv != 9) OK = Standard_False;  break;
    default : ach->AddFail("Incorrect Form Number");    break;
  }
  if (!OK) ach->AddFail
    ("Incorrect count of real values in array V for FEM node");
  Standard_Integer ne = ent->NbElements();
  for (Standard_Integer i = 1; i <= ne; i ++) {
    char mess[100];
    Standard_Integer dlf = ent->DataLayerFlag(i);
    Standard_Integer nl  = ent->NbLayers(i);
    Standard_Integer nrl = ent->NbResultDataLocs(i);
    if (dlf < 0 || dlf > 4)
      ach->AddFail("One of the Data Layer Flags not in [0-4]");
    if (dlf < 4 && ent->NbLayers(i) != 1) {
      sprintf(mess,"Nb. of Layers n0.%d not ONE while Data Layer Flag is in [0-3]",i);
      ach->AddFail(mess);
    }
    if (rrf == 1 || rrf == 2)
      if (nrl != 1 || ent->ResultDataLoc(i,1) != 0) {
	sprintf(mess,"Result Data Locs n0.%d incorrect for Result Report = 1 or 2",i);
	ach->AddFail(mess);
      }
    if (ent->NbResults(i) != (nv*nl*nrl)) {
      sprintf(mess,"Nb. of results for Element n0.%d incorrect, should be %d",
	      i,nv*nl*nrl);
      ach->AddFail(mess);
    }
  }
}

void  IGESAppli_ToolElementResults::OwnDump
  (const Handle(IGESAppli_ElementResults)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const // UNFINISHED
{
  Standard_Integer sublevel = (level > 4) ? 1 : 0;
  S << "IGESAppli_ElementResults" << endl;
  S << "General Note   : ";
  dumper.Dump(ent->Note(),S, sublevel);
  S << endl;
  S << "Subcase Number : " << ent->SubCaseNumber() << endl;
  S << "Time           : " << ent->Time() << endl;
  S << "Number of Result Values : " << ent->NbResultValues() << endl;
  S << "Result Report Flag  : " << ent->ResultReportFlag() << endl;
  S << "Element Identifiers : ";
  IGESData_DumpVals(S ,level,1, ent->NbElements(),ent->ElementIdentifier);
  S << endl << "Elements : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbElements(),ent->Element);
  S << endl << "Element Topology Types : ";
  IGESData_DumpVals(S ,level,1, ent->NbElements(),ent->ElementTopologyType);
  S << endl << "Numbers of Layers      : ";
  IGESData_DumpVals(S ,level,1, ent->NbElements(),ent->NbLayers);
  S << endl << "Data Layer Flags       : ";
  IGESData_DumpVals(S ,level,1, ent->NbElements(),ent->DataLayerFlag);
  S << endl << "Numbers of Result Data Locations : ";
  IGESData_DumpVals(S ,level,1, ent->NbElements(),ent->NbResultDataLocs);
  S << endl << "Result Data Locations : ";  S << " TO BE DONE  ";

//  ??  A VERIFIER DE PRES, pas du tout sur que ce soit bon
//      cf aussi Write et Copy
  if (level <= 4) S << " [ ask level > 4 for more, > 5 for complete ]" << endl;
  else {
    Standard_Integer i;// svv Jan 10 2000 : porting on DEC 
    for (i = 1; i <= ent->NbElements(); i ++) {
      Standard_Integer nloc = ent->NbResultDataLocs(i);
      S << " ["<<i<<":NbLoc="<<nloc<<"]:";
      for (Standard_Integer j = 1; j <= nloc; j ++)
	S << " " << ent->ResultDataLoc (i,j);
      S << endl;
    }
    S << "Result Data : ";

    for (i = 1; i <= ent->NbElements(); i ++) {
      Standard_Integer nres = ent->NbResults(i);
      S << " ["<<i<<":NbRes="<<nres<<"]:";
      if (level <= 5) S << " [ ask level > 5 for complete Data ]" << endl;
      else {
	for (Standard_Integer j = 1; j <= nres; j ++)
	  S << " " << ent->ResultData(i,j);    // ?? is it all ?? UNFINISHED
      }
    }
  }
}
