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

#include <step.tab.hxx>
#include <StepFile_Read.hxx>
#include <StepFile_ReadData.hxx>

#include <Interface_Check.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_ParamType.hxx>
#include <Interface_Protocol.hxx>

#include <StepData_FileRecognizer.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepReaderTool.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>

#include <OSD_OpenFile.hxx>
#include <OSD_Timer.hxx>

#ifdef OCCT_DEBUG
#define CHRONOMESURE
#endif

void StepFile_Interrupt(Standard_CString theErrorMessage, const Standard_Boolean theIsFail)
{
  if (theErrorMessage == NULL)
    return;

  Message_Messenger::StreamBuffer sout = theIsFail ? Message::SendFail() : Message::SendTrace();
  sout << "**** ERR StepFile : " << theErrorMessage << "    ****" << std::endl;
}

static Standard_Integer StepFile_Read (const char* theName,
                                       std::istream* theIStream,
                                       const Handle(StepData_StepModel)& theStepModel,
                                       const Handle(StepData_Protocol)& theProtocol,
                                       const Handle(StepData_FileRecognizer)& theRecogHeader,
                                       const Handle(StepData_FileRecognizer)& theRecogData)
{
  // if stream is not provided, open file stream here
  std::istream *aStreamPtr = theIStream;
  std::ifstream aFileStream;
  if (!aStreamPtr) {
    OSD_OpenStream(aFileStream, theName, std::ios_base::in | std::ios_base::binary);
    aStreamPtr = &aFileStream;
  }

  if (aStreamPtr->fail())
  {
    return -1;
  }

#ifdef CHRONOMESURE
  OSD_Timer c;
  c.Reset();
  c.Start();
#endif

  Message_Messenger::StreamBuffer sout = Message::SendTrace();
  sout << "      ...    Step File Reading : '" << theName << "'";

  StepFile_ReadData aFileDataModel;
  try {
    OCC_CATCH_SIGNALS
    int aLetat = 0;
    step::scanner aScanner(&aFileDataModel, aStreamPtr);
    aScanner.yyrestart(aStreamPtr);
    step::parser aParser(&aScanner);
    aLetat = aParser.parse();
    if (aLetat != 0) {
      StepFile_Interrupt(aFileDataModel.GetLastError(), Standard_True);
      return 1;
    }
  }
  catch (Standard_Failure const& anException) {
    Message::SendFail() << " ...  Exception Raised while reading Step File : '" << theName << "':\n"
                        << anException << "    ...";
    return 1;
  }

#ifdef CHRONOMESURE
  c.Show(sout);
#endif

  sout << "      ...    STEP File   Read    ...\n";

  Standard_Integer nbhead, nbrec, nbpar;
  aFileDataModel.GetFileNbR (&nbhead,&nbrec,&nbpar);  // renvoi par lex/yacc
  Handle(StepData_StepReaderData) undirec =
    new StepData_StepReaderData(nbhead,nbrec,nbpar, theStepModel->SourceCodePage());  // creation tableau de records
  for ( Standard_Integer nr = 1; nr <= nbrec; nr ++) {
    int nbarg; char* ident; char* typrec = 0;
    aFileDataModel.GetRecordDescription(&ident, &typrec, &nbarg);
    undirec->SetRecord (nr, ident, typrec, nbarg);

    if (nbarg>0) {
      Interface_ParamType typa; char* val;
      while(aFileDataModel.GetArgDescription (&typa, &val) == 1) {
        undirec->AddStepParam (nr, val, typa);
      }
    }
    undirec->InitParams(nr);
    aFileDataModel.NextRecord();
  }

  aFileDataModel.ErrorHandle(undirec->GlobalCheck());
  Standard_Integer anFailsCount = undirec->GlobalCheck()->NbFails();
  if (anFailsCount > 0)
  {
    Message::SendInfo() << "**** ERR StepFile : Incorrect Syntax : Fails Count : "
      << anFailsCount << " ****";
  }

  aFileDataModel.ClearRecorder(1);

  sout << "      ... Step File loaded  ...\n";
  sout << "   " << undirec->NbRecords() << " records (entities,sub-lists,scopes), " << nbpar << " parameters";

#ifdef CHRONOMESURE
  c.Show(sout);
#endif

//   Analyse : par StepReaderTool

  StepData_StepReaderTool readtool (undirec, theProtocol);
  readtool.SetErrorHandle (Standard_True);

  readtool.PrepareHeader(theRecogHeader);  // Header. reco nul -> pour Protocol
  readtool.Prepare(theRecogData);          // Data.   reco nul -> pour Protocol

  sout << "      ... Parameters prepared ...\n";

#ifdef CHRONOMESURE
  c.Show(sout);
#endif

  readtool.LoadModel(theStepModel);
  if (theStepModel->Protocol().IsNull()) theStepModel->SetProtocol (theProtocol);
  aFileDataModel.ClearRecorder(2);
  anFailsCount = undirec->GlobalCheck()->NbFails() - anFailsCount;
  if (anFailsCount > 0)
  {
    Message::SendInfo() << "*** ERR StepReaderData : Unresolved Reference : Fails Count : "
      << anFailsCount << " ***";
  }

  readtool.Clear();
  undirec.Nullify();

  sout << "      ...   Objects analysed  ...\n";
  Standard_Integer n = theStepModel->NbEntities();
  sout << "  STEP Loading done : " << n << " Entities";

#ifdef CHRONOMESURE
  c.Show(sout);
#endif

  return 0;
}

Standard_Integer StepFile_Read(const char* theName,
                               std::istream* theIStream,
                               const Handle(StepData_StepModel)& theStepModel,
                               const Handle(StepData_Protocol)& theProtocol)
{
  Handle(StepData_FileRecognizer) aNulRecog;
  return StepFile_Read(theName, theIStream, theStepModel, theProtocol, aNulRecog, aNulRecog);
}
