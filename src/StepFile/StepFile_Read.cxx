// Created on: 1991-08-30
// Created by: Christian CAILLET
// Copyright (c) 1991-1999 Matra Datavision
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

//   StepFile_Read

//   routine assurant l enchainement des operations de lecture d un fichier
//   STEP dans un StepModel, en fonction d une cle de reconnaissance
//   Retour de la fonction :
//     0 si OK  (le StepModel a ete charge)
//    -1 si abandon car fichier pas pu etre ouvert
//     1 si erreur en cours de lecture

//   Compilation conditionnelle : concerne les mesures de performances

#include <stdio.h>
#include <iostream> 
#include "recfile.ph"
#include "stepread.ph"

#include <Interface_ParamType.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_Check.hxx>
#include <StepData_Protocol.hxx>

#include <StepData_StepReaderData.hxx>
#include <StepData_StepReaderTool.hxx>
#include <StepFile_Read.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Interface_InterfaceError.hxx>

#include <Message_Messenger.hxx>
#include <Message.hxx>

#include <OSD_OpenFile.hxx>
#include <OSD_Timer.hxx>

#ifdef OCCT_DEBUG
#define CHRONOMESURE
#endif

//  ##  ##  ##  ##    ON SAURA AU MOINS TRAITER UndefinedEntity  ##  ##  ##  ##

extern "C" void recfile_modeprint (int mode);  // controle trace recfile

static Handle(Interface_Check) checkread = new Interface_Check;
static Standard_Integer modepr = 1;

void StepFile_ReadTrace (const Standard_Integer mode)
{
  modepr = mode;   // recfile_modeprint est rappele a chaque lecture de fichier
}

//  ##  ##  ##  ##  ##  ##    Corps de la Routine    ##  ##  ##  ##  ##  ##

static Interface_ParamType LesTypes[10];   // passage types (recstep/Interface)

static Standard_Integer StepFile_Read (const char* theName,
                                       std::istream* theIStream,
                                       const Handle(StepData_StepModel)& stepmodel,
                                       const Handle(StepData_Protocol)& protocol,
                                       const Handle(StepData_FileRecognizer)& recoheader,
                                       const Handle(StepData_FileRecognizer)& recodata)
{
  checkread->Clear();
  recfile_modeprint ( (modepr > 0 ? modepr-1 : 0) );

  // if stream is not provided, open file stream here
  std::istream *aStreamPtr = theIStream;
  std::ifstream aFileStream;
  if (!aStreamPtr) {
    OSD_OpenStream(aFileStream, theName, std::ios_base::in | std::ios_base::binary);
    aStreamPtr = &aFileStream;
  }

  if (aStreamPtr->bad())
  {
    return -1;
  }

#ifdef CHRONOMESURE
  OSD_Timer c;
  c.Reset();
  c.Start();
  Message::SendInfo() << "      ...    Step File Reading : '" << theName << "'";
#endif

  try {
    OCC_CATCH_SIGNALS
    if (stepread(*aStreamPtr) != 0) {
      lir_file_fin(3);
      return 1;
    }
  }
  catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    Message::SendAlarm() << " ...  Exception Raised while reading Step File : '" << theName << "':\n"
                         << anException.GetMessageString()
                         << "    ...";
#endif
    (void)anException;
    lir_file_fin(3);  
    return 1;
  }

// Continue reading of file despite of possible fails
//  if (checkread->HasFailed()) { lir_file_fin(3); stepread_endinput(newifstream, ficnom);  return 1; }
#ifdef CHRONOMESURE
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "      ...    STEP File   Read    ...\n";
    c.Show (sout);
  }
#endif

//  Creation du StepReaderData

  LesTypes[rec_argNondef]    = Interface_ParamVoid ;
  LesTypes[rec_argSub]       = Interface_ParamSub ;
  LesTypes[rec_argIdent]     = Interface_ParamIdent ;
  LesTypes[rec_argInteger]   = Interface_ParamInteger ;
  LesTypes[rec_argFloat]     = Interface_ParamReal ;
  LesTypes[rec_argEnum]      = Interface_ParamEnum ;
  LesTypes[rec_argBinary]    = Interface_ParamBinary ;
  LesTypes[rec_argText]      = Interface_ParamText ;
  LesTypes[rec_argHexa]      = Interface_ParamHexa ;
  LesTypes[rec_argMisc]      = Interface_ParamMisc ;

  Standard_Integer nbhead, nbrec, nbpar;
  lir_file_nbr (&nbhead,&nbrec,&nbpar);  // renvoi par lex/yacc
  Handle(StepData_StepReaderData) undirec =
    new StepData_StepReaderData(nbhead,nbrec,nbpar, stepmodel->SourceCodePage());  // creation tableau de records

  for ( Standard_Integer nr = 1; nr <= nbrec; nr ++) {
    int nbarg; char* ident; char* typrec = 0;
    lir_file_rec (&ident, &typrec, &nbarg);
    undirec->SetRecord (nr, ident, typrec, nbarg);

    if (nbarg>0) {
      int typa; char* val;
      Interface_ParamType newtype;
      while(lir_file_arg (&typa, &val) == 1) {
        newtype = LesTypes[typa] ;
        undirec->AddStepParam (nr, val, newtype);
      }
    }
    undirec->InitParams(nr);
    lir_file_finrec();
  }
  lir_file_fin(1);
//  on a undirec pret pour la suite

#ifdef CHRONOMESURE
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "      ... Step File loaded  ...\n";
    c.Show (sout);
    sout << "   "<< undirec->NbRecords() << " records (entities,sub-lists,scopes), "<< nbpar << " parameters";
  }
#endif

//   Analyse : par StepReaderTool

  StepData_StepReaderTool readtool (undirec,protocol);
  readtool.SetErrorHandle (Standard_True);

  readtool.PrepareHeader(recoheader);  // Header. reco nul -> pour Protocol
  readtool.Prepare(recodata);          // Data.   reco nul -> pour Protocol

#ifdef CHRONOMESURE
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "      ... Parameters prepared ...\n";
    c.Show (sout);
  }
#endif

  readtool.LoadModel(stepmodel);
  if (stepmodel->Protocol().IsNull()) stepmodel->SetProtocol (protocol);
  lir_file_fin(2);
  
  readtool.Clear();
  undirec.Nullify();
#ifdef CHRONOMESURE
  {
    Message_Messenger::StreamBuffer sout = Message::SendInfo();
    sout << "      ...   Objets analysed  ...\n";
    c.Show (sout);
    Standard_Integer n = stepmodel->NbEntities();
    sout << "  STEP Loading done : " << n << " Entities";
  }
#endif
  return 0;
}

Standard_Integer StepFile_Read(const char* theName,
                               std::istream* theIStream,
                               const Handle(StepData_StepModel)& stepmodel,
                               const Handle(StepData_Protocol)& protocol)
{
  Handle(StepData_FileRecognizer) nulreco;
  return StepFile_Read (theName,theIStream,stepmodel,protocol,nulreco,nulreco);
}

void StepFile_Interrupt (char* mess)
{
#ifdef OCCT_DEBUG
  Message_Messenger::StreamBuffer sout = Message::SendInfo();
  sout << "    ****    StepFile Error : " << mess << "    ****" << std::endl;
#endif
  checkread->AddFail(mess);
}
