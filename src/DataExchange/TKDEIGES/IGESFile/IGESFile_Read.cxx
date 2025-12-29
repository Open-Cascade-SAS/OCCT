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

// dce 21.01.99 : move of general message to IGESToBRep_Reader

#include <cstdio>
// declarations of basic C programs:
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESReaderTool.hxx>
#include <IGESData_GeneralModule.hxx>
#include <Interface_Check.hxx>

//  To handle exceptions:
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

// basic definition, to include for use
#include <IGESFile_Read.hxx>

#include "igesread.h"

// #define VERIFPRINT

// MGE 16/06/98
// To use Msg class
#include <Message_Msg.hxx>

// internal breakdown to facilitate error recovery
static int recupne, recupnp; // for display in case of problem

static occ::handle<Interface_Check>& checkread()
{
  static occ::handle<Interface_Check> chrd = new Interface_Check;
  return chrd;
}

static void IGESFile_ReadHeader(const occ::handle<IGESData_IGESReaderData>& IR);
static void IGESFile_ReadContent(const occ::handle<IGESData_IGESReaderData>& IR);
void        IGESFile_Check(int mode, Message_Msg& amsg);
// void IGESFile_Check2 (int mode,char * code, int num, char * str);
// void IGESFile_Check3 (int mode,char * code);

//  Correspondence between igesread types and Interface_ParamFile types ...
static Interface_ParamType LesTypes[10];

//  New way: Protocol is sufficient

int IGESFile_Read(char*                                  nomfic,
                  const occ::handle<IGESData_IGESModel>& amodel,
                  const occ::handle<IGESData_Protocol>&  protocol)
{
  occ::handle<IGESData_FileRecognizer> nulreco;
  return IGESFile_Read(nomfic, amodel, protocol, nulreco, false);
}

int IGESFile_ReadFNES(char*                                  nomfic,
                      const occ::handle<IGESData_IGESModel>& amodel,
                      const occ::handle<IGESData_Protocol>&  protocol)
{
  occ::handle<IGESData_FileRecognizer> nulreco;
  return IGESFile_Read(nomfic, amodel, protocol, nulreco, true);
}

//  Old way: with Recognizer

int IGESFile_Read(char*                                       nomfic,
                  const occ::handle<IGESData_IGESModel>&      amodel,
                  const occ::handle<IGESData_Protocol>&       protocol,
                  const occ::handle<IGESData_FileRecognizer>& reco,
                  const bool                                  modefnes)
{
  //====================================
  Message_Msg Msg1  = Message_Msg("XSTEP_1");
  Message_Msg Msg15 = Message_Msg("XSTEP_15");
  //====================================

  char* ficnom = nomfic; // ficnom ?
  int   lesect[6];

  // Sending of message : Beginning of the reading
  IGESFile_Check(2, Msg1);

  checkread()->Clear();
  int result = igesread(ficnom, lesect, modefnes);

  if (result != 0)
    return result;

  //  Loading results into an IGESReader

  LesTypes[ArgVide] = Interface_ParamVoid;
  LesTypes[ArgQuid] = Interface_ParamMisc;
  LesTypes[ArgChar] = Interface_ParamText;
  LesTypes[ArgInt]  = Interface_ParamInteger;
  LesTypes[ArgSign] = Interface_ParamInteger;
  LesTypes[ArgReal] = Interface_ParamReal;
  LesTypes[ArgExp]  = Interface_ParamMisc; // exponent not finished
  LesTypes[ArgRexp] = Interface_ParamReal; // complete exponent
  LesTypes[ArgMexp] = Interface_ParamEnum; // exponent but no decimal point

  int nbparts, nbparams;
  iges_stats(&nbparts, &nbparams); // and performs necessary initializations
  occ::handle<IGESData_IGESReaderData> IR =
    //    new IGESData_IGESReaderData (nbparts, nbparams);
    new IGESData_IGESReaderData((lesect[3] + 1) / 2, nbparams);
  {
    {
      try
      {
        OCC_CATCH_SIGNALS
        IGESFile_ReadHeader(IR);
      } // end attempt 1 (global)
      catch (Standard_Failure const&)
      {
        // Sending of message : Internal error during the header reading
        Message_Msg Msg11 = Message_Msg("XSTEP_11");
        IGESFile_Check(1, Msg11);
      }
    }

    {
      try
      {
        OCC_CATCH_SIGNALS
        if (nbparts > 0)
          IGESFile_ReadContent(IR);

        // Sending of message : Loaded data
      } // end attempt 2 (entities)
      catch (Standard_Failure const&)
      {
        // Sending of message : Internal error during the content reading
        if (recupnp == 0)
        {
          Message_Msg Msg13 = Message_Msg("XSTEP_13");
          Msg13.Arg(recupne);
          IGESFile_Check(1, Msg13);
        }
        else
        {
          Message_Msg Msg14 = Message_Msg("XSTEP_14");
          Msg14.Arg(recupne);
          Msg14.Arg(recupnp);
          IGESFile_Check(1, Msg14);
        }
      }
    }
  }

  int nbr = IR->NbRecords();
  // Sending of message : Number of total loaded entities
  Msg15.Arg(nbr);
  IGESFile_Check(2, Msg15);
  iges_finfile(1);
  IGESData_IGESReaderTool IT(IR, protocol);
  IT.Prepare(reco);
  IT.SetErrorHandle(true);

  // Sending of message : Loading of Model : Beginning
  IT.LoadModel(amodel);
  if (amodel->Protocol().IsNull())
    amodel->SetProtocol(protocol);
  iges_finfile(2);

  //  Now, the check
  // Nb warning in global section.
  int nbWarn = checkread()->NbWarnings(), nbFail = checkread()->NbFails();
  const occ::handle<Interface_Check>& oldglob = amodel->GlobalCheck();
  if (nbWarn + nbFail > 0)
  {
    checkread()->GetMessages(oldglob);
    amodel->SetGlobalCheck(checkread());
  }

  checkread()->Trace(0, 1);

  return 0;
}

// Internal breakdown

void IGESFile_ReadHeader(const occ::handle<IGESData_IGESReaderData>& IR)
{
  int   l = 0; // szv#4:S4163:12Mar99 i,j,k not needed
  char* parval;
  int   typarg;
  //  first the start lines (comments)
  // szv#4:S4163:12Mar99 optimized
  /*
    while ( (j = iges_lirparam(&typarg,&parval)) != 0) {
      k = -1;
      for (int j = 72; j >= 0; j --) {
        if (parval[j] > 32) {  k = j;  break;  }
      }
      parval[k+1] = '\0';
      if (k >= 0 || l > 0) IR->AddStartLine (parval);
      l ++;
    }
    //  then the Global Section
    iges_setglobal();
    while ( (i = iges_lirparam(&typarg,&parval)) != 0) {
      IR->AddGlobal(LesTypes[typarg],parval);
    }
  */
  while (iges_lirparam(&typarg, &parval) != 0)
  {
    int j; // svv Jan11 2000 : porting on DEC
    for (j = 72; j >= 0; j--)
      if (parval[j] > 32)
        break;
    parval[j + 1] = '\0';
    if (j >= 0 || l > 0)
      IR->AddStartLine(parval);
    l++;
  }
  //  then the Global Section
  iges_setglobal();
  while (iges_lirparam(&typarg, &parval) != 0)
    IR->AddGlobal(LesTypes[typarg], parval);
  IR->SetGlobalSection();
}

void IGESFile_ReadContent(const occ::handle<IGESData_IGESReaderData>& IR)
{
  char *res1, *res2, *nom, *num;
  char* parval;
  int*  v;
  int   typarg;
  int   nbparam;

  int ns; // szv#4:S4163:12Mar99 i unused
  while ((ns = iges_lirpart(&v, &res1, &res2, &nom, &num, &nbparam)) != 0)
  {
    recupnp = 0;
    recupne = (ns + 1) / 2; // numero entite
                            //    if(recupne > IR->NbEntities()) {
                            //      iges_nextpart();
                            //      continue;
                            //    }
    IR->SetDirPart(recupne,
                   v[0],
                   v[1],
                   v[2],
                   v[3],
                   v[4],
                   v[5],
                   v[6],
                   v[7],
                   v[8],
                   v[9],
                   v[10],
                   v[11],
                   v[12],
                   v[13],
                   v[14],
                   v[15],
                   v[16],
                   res1,
                   res2,
                   nom,
                   num);
    while (iges_lirparam(&typarg, &parval) != 0)
    { // szv#4:S4163:12Mar99 `i=` not needed
      recupnp++;
      if (typarg == ArgInt || typarg == ArgSign)
      {
        int nument = atoi(parval);
        if (nument < 0)
          nument = -nument;
        if (nument & 1)
          nument = (nument + 1) / 2;
        else
          nument = 0;
        IR->AddParam(recupne, parval, LesTypes[typarg], nument);
      }
      else
        IR->AddParam(recupne, parval, LesTypes[typarg]);
    }
    IR->InitParams(recupne);
    iges_nextpart();
  }
}

void IGESFile_Check(int mode, Message_Msg& amsg)
{
  // MGE 20/07/98
  switch (mode)
  {
    case 0:
      checkread()->SendFail(amsg);
      break;
    case 1:
      checkread()->SendWarning(amsg);
      break;
    case 2:
      checkread()->SendMsg(amsg);
      break;
    default:
      checkread()->SendMsg(amsg);
  }
  // checkread().Trace(3,-1);
}

void IGESFile_Check2(int mode, char* code, int num, char* str)
{
  // MGE 20/07/98
  Message_Msg amsg(code);
  amsg.Arg(num);
  amsg.Arg(str);

  switch (mode)
  {
    case 0:
      checkread()->SendFail(amsg);
      break;
    case 1:
      checkread()->SendWarning(amsg);
      break;
    case 2:
      checkread()->SendMsg(amsg);
      break;
    default:
      checkread()->SendMsg(amsg);
  }
  // checkread().Trace(3,-1);
}

void IGESFile_Check3(int mode, char* code)
{
  // MGE 20/07/98
  Message_Msg amsg(code);
  switch (mode)
  {
    case 0:
      checkread()->SendFail(amsg);
      break;
    case 1:
      checkread()->SendWarning(amsg);
      break;
    case 2:
      checkread()->SendMsg(amsg);
      break;
    default:
      checkread()->SendMsg(amsg);
  }
  // checkread().Trace(3,-1);
}
