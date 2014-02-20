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

#include <stdio.h>
// declarations des programmes C de base :
#include <Interface_ParamType.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESReaderTool.hxx>
#include <IGESData_GeneralModule.hxx>
#include <Interface_Check.hxx>

#include <Interface_FileParameter.hxx>
#include <Interface_ParamList.hxx>
//  Pour traiter les exceptions :
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

// definition de base, a inclure pour utiliser
#include <IGESFile_Read.hxx>

#include "igesread.h"

//#define VERIFPRINT

// MGE 16/06/98
// To use Msg class
#include <Message_Msg.hxx>

// decoupage interne pour faciliter les recuperations d erreurs
static Standard_Integer recupne,recupnp;  // pour affichage en cas de pepin
static Handle(Interface_Check)& checkread()
{
  static Handle(Interface_Check) chrd = new Interface_Check;
  return chrd;
}

static void IGESFile_ReadHeader  (const Handle(IGESData_IGESReaderData)& IR);
static void IGESFile_ReadContent (const Handle(IGESData_IGESReaderData)& IR);
void IGESFile_Check (int mode,Message_Msg& amsg);
// void IGESFile_Check2 (int mode,char * code, int num, char * str);
// void IGESFile_Check3 (int mode,char * code);

//  Correspondance entre types igesread et types Interface_ParamFile ...
static Interface_ParamType LesTypes[10];


//  Nouvelle maniere : Protocol suffit

Standard_Integer IGESFile_Read
  (char* nomfic,
   const Handle(IGESData_IGESModel)& amodel,
   const Handle(IGESData_Protocol)& protocol)
{
  Handle(IGESData_FileRecognizer) nulreco;
  return IGESFile_Read(nomfic,amodel,protocol,nulreco,Standard_False);
}

Standard_Integer IGESFile_ReadFNES
  (char* nomfic,
   const Handle(IGESData_IGESModel)& amodel,
   const Handle(IGESData_Protocol)& protocol)
{
  Handle(IGESData_FileRecognizer) nulreco;
  return IGESFile_Read(nomfic,amodel,protocol,nulreco,Standard_True);
}


//  Ancienne maniere : avec Recognizer

Standard_Integer IGESFile_Read
  (char* nomfic,
   const Handle(IGESData_IGESModel)& amodel,
   const Handle(IGESData_Protocol)& protocol,
   const Handle(IGESData_FileRecognizer)& reco,
   const Standard_Boolean modefnes)
{
  //====================================
  Message_Msg Msg1  = Message_Msg("XSTEP_1");
  Message_Msg Msg15 = Message_Msg("XSTEP_15");
  //====================================

  char* ficnom = nomfic; // ficnom ?
  int lesect[6];
  
  // Sending of message : Beginning of the reading
  IGESFile_Check(2, Msg1);

  checkread()->Clear();
  int result = igesread(ficnom,lesect,modefnes);

  if (result != 0) return result;

//  Chargement des resultats dans un IGESReader

  LesTypes[ArgVide] = Interface_ParamVoid;
  LesTypes[ArgQuid] = Interface_ParamMisc;
  LesTypes[ArgChar] = Interface_ParamText;
  LesTypes[ArgInt]  = Interface_ParamInteger;
  LesTypes[ArgSign] = Interface_ParamInteger;
  LesTypes[ArgReal] = Interface_ParamReal;
  LesTypes[ArgExp ] = Interface_ParamMisc;  // exposant pas termine
  LesTypes[ArgRexp] = Interface_ParamReal;       // exposant complet
  LesTypes[ArgMexp] = Interface_ParamEnum;       // exposant mais pas de point


  int nbparts, nbparams;
  iges_stats(&nbparts,&nbparams);    // et fait les Initialisations necessaires
  Handle(IGESData_IGESReaderData) IR =
//    new IGESData_IGESReaderData (nbparts, nbparams);
    new IGESData_IGESReaderData((lesect[3]+1)/2, nbparams);
  {
   {
    try {
      OCC_CATCH_SIGNALS
      IGESFile_ReadHeader(IR);
    }    // fin essai 1 (global)
    catch (Standard_Failure) {
      // Sending of message : Internal error during the header reading 
      Message_Msg Msg11 = Message_Msg("XSTEP_11");
      IGESFile_Check (1,Msg11);
    }
   }

   {
    try {
      OCC_CATCH_SIGNALS
      if (nbparts > 0) IGESFile_ReadContent(IR);

  // Sending of message : Loaded data  
    }    // fin essai 2 (entites)
    catch (Standard_Failure) {
      // Sending of message : Internal error during the content reading 
      if (recupnp == 0) {
	Message_Msg Msg13 = Message_Msg("XSTEP_13");
	Msg13.Arg(recupne);
	IGESFile_Check(1,Msg13);
      }
      else {
	Message_Msg Msg14 = Message_Msg("XSTEP_14");
	Msg14.Arg(recupne);
	Msg14.Arg(recupnp);
	IGESFile_Check(1, Msg14);
      }
    }
   }
  }
  
  Standard_Integer nbr = IR->NbRecords();
  // Sending of message : Number of total loaded entities 
  Msg15.Arg(nbr);
  IGESFile_Check(2, Msg15);
  iges_finfile(1);
  IGESData_IGESReaderTool IT (IR,protocol);
  IT.Prepare(reco); 
  IT.SetErrorHandle(Standard_True);

  // Sending of message : Loading of Model : Beginning 
  IT.LoadModel(amodel);
  if (amodel->Protocol().IsNull()) amodel->SetProtocol (protocol);
  iges_finfile(2);

  //  A present, le check
  // Nb warning in global section.
  Standard_Integer nbWarn = checkread()->NbWarnings(), nbFail = checkread()->NbFails();
  const Handle(Interface_Check)& oldglob = amodel->GlobalCheck();
  if (nbWarn + nbFail > 0) {
    checkread()->GetMessages (oldglob);
    amodel->SetGlobalCheck (checkread());
  }

  checkread()->Trace(0,1);
 
  return 0;
}


// Decoupage interne

 void IGESFile_ReadHeader  (const Handle(IGESData_IGESReaderData)& IR)
{
  Standard_Integer l=0; //szv#4:S4163:12Mar99 i,j,k not needed
  char* parval;
  int typarg;
  //  d abord les start lines (commentaires)
  //szv#4:S4163:12Mar99 optimized
/*
  while ( (j = iges_lirparam(&typarg,&parval)) != 0) {
    k = -1;
    for (Standard_Integer j = 72; j >= 0; j --) {
      if (parval[j] > 32) {  k = j;  break;  }
    }
    parval[k+1] = '\0';
    if (k >= 0 || l > 0) IR->AddStartLine (parval);
    l ++;
  }
  //  puis la Global Section
  iges_setglobal();
  while ( (i = iges_lirparam(&typarg,&parval)) != 0) {
    IR->AddGlobal(LesTypes[typarg],parval);
  }
*/
  while (iges_lirparam(&typarg,&parval) != 0) {
    Standard_Integer j; // svv Jan11 2000 : porting on DEC
    for (j = 72; j >= 0; j--)
      if (parval[j] > 32) break;
    parval[j+1] = '\0';
    if (j >= 0 || l > 0) IR->AddStartLine (parval);
    l++;
  }
  //  puis la Global Section
  iges_setglobal();
  while (iges_lirparam(&typarg,&parval) != 0) IR->AddGlobal(LesTypes[typarg],parval);
  IR->SetGlobalSection();
}

 void IGESFile_ReadContent (const Handle(IGESData_IGESReaderData)& IR)
{
  char *res1, *res2, *nom, *num; char* parval;
  int *v; int typarg;
  int nbparam;
    

  Standard_Integer nn=0;
  int ns; //szv#4:S4163:12Mar99 i unused
  while ( (ns = iges_lirpart(&v,&res1,&res2,&nom,&num,&nbparam)) != 0) {
    nn++;
    recupnp = 0;
    recupne = (ns+1)/2;  // numero entite
//    if(recupne > IR->NbEntities()) {
//      iges_nextpart();
//      continue;
//    }
    IR->SetDirPart(recupne,
		   v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],
		   v[11],v[12],v[13],v[14],v[15],v[16],res1,res2,nom,num);
    while (iges_lirparam(&typarg,&parval) != 0) { //szv#4:S4163:12Mar99 `i=` not needed
      recupnp ++;
      if (typarg == ArgInt || typarg == ArgSign) {
	Standard_Integer nument = atoi(parval);
	if (nument < 0) nument = -nument;
	if (nument & 1) nument = (nument+1)/2;
	else nument = 0;
	IR->AddParam(recupne,parval,LesTypes[typarg],nument);
      }
      else IR->AddParam(recupne,parval,LesTypes[typarg]);
    }
    IR->InitParams(recupne);
    iges_nextpart();
  }
}


void IGESFile_Check (int mode,Message_Msg& amsg)
{
  // MGE 20/07/98
  switch (mode)
   {
    case 0 : checkread()->SendFail (amsg); break;
    case 1 : checkread()->SendWarning (amsg); break;
    case 2 : checkread()->SendMsg (amsg);break;
    default : checkread()->SendMsg (amsg); 
   }
  //checkread().Trace(3,-1);
}

void IGESFile_Check2 (int mode,char * code, int num, char * str)
{
  // MGE 20/07/98
  Message_Msg amsg (code);
  amsg.Arg(num);
  amsg.Arg(str);

  switch (mode)
   {
    case 0 : checkread()->SendFail (amsg); break;
    case 1 : checkread()->SendWarning (amsg); break;
    case 2 : checkread()->SendMsg (amsg); break;
    default : checkread()->SendMsg (amsg); 
   }
  //checkread().Trace(3,-1);
}


void IGESFile_Check3 (int mode,char * code)
{
  // MGE 20/07/98
  Message_Msg amsg (code);
  switch (mode)
   {
    case 0 : checkread()->SendFail (amsg); break;
    case 1 : checkread()->SendWarning (amsg); break;
    case 2 : checkread()->SendMsg (amsg); break;
    default : checkread()->SendMsg (amsg);
   }
  //checkread().Trace(3,-1);
}
