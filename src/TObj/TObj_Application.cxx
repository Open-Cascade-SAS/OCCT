// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_Application.hxx>

#include <Standard_SStream.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <CDM_COutMessageDriver.hxx>
#include <Message_Msg.hxx>
#include <Message_MsgFile.hxx>
#include <Resource_Manager.hxx>

#include <stdio.h>

#include "TObj_TObj_msg.pxx"

IMPLEMENT_STANDARD_RTTIEXT(TObj_Application,TDocStd_Application)

//=======================================================================
//function : GetInstance
//purpose  :
//=======================================================================

Handle(TObj_Application) TObj_Application::GetInstance()
{
  static Handle(TObj_Application) anInstance = new TObj_Application;
  return anInstance;
}

//=======================================================================
//function : TObj_Application
//purpose  : 
//=======================================================================

TObj_Application::TObj_Application () : myIsError(Standard_False)
{
  if (!Message_MsgFile::HasMsg ("TObj_Appl_SUnknownFailure"))
  {
    // load messages into global map on first instantiation
    Message_MsgFile::LoadFromString (TObj_TObj_msg, sizeof(TObj_TObj_msg) - 1);
    if (!Message_MsgFile::HasMsg ("TObj_Appl_SUnknownFailure"))
    {
      throw Standard_ProgramError("Critical Error - message resources for TObj_Application are invalid or undefined!");
    }
  }

  myMessenger = new Message_Messenger;
  myIsVerbose = Standard_False;
}

//=======================================================================
//function : ResourcesName
//purpose  : 
//=======================================================================

Standard_CString TObj_Application::ResourcesName()
{
  return Standard_CString("TObj");
}

//=======================================================================
//function : SaveDocument
//purpose  : Saving the OCAF document
//=======================================================================

Standard_Boolean TObj_Application::SaveDocument
                        (const Handle(TDocStd_Document)&   theSourceDoc,
                         const TCollection_ExtendedString& theTargetFile)
{
  myIsError = Standard_False;
  PCDM_StoreStatus aStatus = SaveAs (theSourceDoc, theTargetFile);
  myIsError = aStatus != PCDM_SS_OK;
  if (myIsError)
  {
    switch (aStatus)
    {
    case PCDM_SS_DriverFailure:
      ErrorMessage (Message_Msg("TObj_Appl_SDriverFailure") << theTargetFile);
      break;
    case PCDM_SS_WriteFailure:
      ErrorMessage (Message_Msg("TObj_Appl_SWriteFailure") << theTargetFile);
      break;
    case PCDM_SS_Failure:
      ErrorMessage (Message_Msg("TObj_Appl_SFailure") << theTargetFile);
      break;
    case PCDM_SS_Doc_IsNull:
      ErrorMessage (Message_Msg("TObj_Appl_SDocIsNull") << theTargetFile);
      break;
    case PCDM_SS_No_Obj:
      ErrorMessage (Message_Msg("TObj_Appl_SNoObj") << theTargetFile);
      break;
    case PCDM_SS_Info_Section_Error:
      ErrorMessage (Message_Msg("TObj_Appl_SInfoSectionError") << theTargetFile);
      break;
    default:
      ErrorMessage (Message_Msg("TObj_Appl_SUnknownFailure") << theTargetFile);
      break;
    }
  }

  // Release free memory
  Standard::Purge();
  return myIsError ? Standard_False : Standard_True;
}

//=======================================================================
//function : LoadDocument
//purpose  : Loading the OCAF document
//=======================================================================

Standard_Boolean TObj_Application::LoadDocument
                        (const TCollection_ExtendedString& theSourceFile,
                         Handle(TDocStd_Document)&         theTargetDoc)
{
  myIsError = Standard_False;
  PCDM_ReaderStatus aStatus = PCDM_RS_ReaderException;
  {
    try
    {
      aStatus = Open (theSourceFile, theTargetDoc);
    }
    catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      ErrorMessage (Message_Msg("TObj_Appl_Exception") << 
                    anException.GetMessageString());
#endif
      (void)anException;
    }
  }
  myIsError = aStatus != PCDM_RS_OK;
  if (myIsError)
  {
    switch (aStatus)
    {
    case PCDM_RS_UnknownDocument:
      ErrorMessage (Message_Msg("TObj_Appl_RUnknownDocument") << theSourceFile);
      break;
    case PCDM_RS_AlreadyRetrieved:
      ErrorMessage (Message_Msg("TObj_Appl_RAlreadyRetrieved") << theSourceFile);
      break;
    case PCDM_RS_AlreadyRetrievedAndModified:
      ErrorMessage (Message_Msg("TObj_Appl_RAlreadyRetrievedAndModified") << theSourceFile);
      break;
    case PCDM_RS_NoDriver:
      ErrorMessage (Message_Msg("TObj_Appl_RNoDriver") << theSourceFile);
      break;
    case PCDM_RS_UnknownFileDriver:
      ErrorMessage (Message_Msg("TObj_Appl_RNoDriver") << theSourceFile);
      break;
    case PCDM_RS_OpenError:
      ErrorMessage (Message_Msg("TObj_Appl_ROpenError") << theSourceFile);
      break;
    case PCDM_RS_NoVersion:
      ErrorMessage (Message_Msg("TObj_Appl_RNoVersion") << theSourceFile);
      break;
    case PCDM_RS_NoModel:
      ErrorMessage (Message_Msg("TObj_Appl_RNoModel") << theSourceFile);
      break;
    case PCDM_RS_NoDocument:
      ErrorMessage (Message_Msg("TObj_Appl_RNoDocument") << theSourceFile);
      break;
    case PCDM_RS_FormatFailure:
      ErrorMessage (Message_Msg("TObj_Appl_RFormatFailure") << theSourceFile);
      break;
    case PCDM_RS_TypeNotFoundInSchema:
      ErrorMessage (Message_Msg("TObj_Appl_RTypeNotFound") << theSourceFile);
      break;
    case PCDM_RS_UnrecognizedFileFormat:
      ErrorMessage (Message_Msg("TObj_Appl_RBadFileFormat") << theSourceFile);
      break;
    case PCDM_RS_MakeFailure:
      ErrorMessage (Message_Msg("TObj_Appl_RMakeFailure") << theSourceFile);
      break;
    case PCDM_RS_PermissionDenied:
      ErrorMessage (Message_Msg("TObj_Appl_RPermissionDenied") << theSourceFile);
      break;
    case PCDM_RS_DriverFailure:
      ErrorMessage (Message_Msg("TObj_Appl_RDriverFailure") << theSourceFile);
      break;
    case PCDM_RS_ReaderException:
      ErrorMessage (Message_Msg("TObj_Appl_RException") << theSourceFile);
      break;
    default:
      ErrorMessage (Message_Msg("TObj_Appl_RUnknownFail") << theSourceFile);
      break;
    }
  }

  // Release free memory
  Standard::Purge();
  return myIsError ? Standard_False : Standard_True;
}

//=======================================================================
//function : CreateNewDocument
//purpose  : 
//=======================================================================

Standard_Boolean TObj_Application::CreateNewDocument
                        (Handle(TDocStd_Document)&         theDoc,
                         const TCollection_ExtendedString& theFormat)
{
  myIsError = Standard_False;

  // Create the Document
  NewDocument (theFormat, theDoc);

  return myIsError ? Standard_False : Standard_True;
}

//=======================================================================
//function : ErrorMessage
//purpose  : 
//=======================================================================

void TObj_Application::ErrorMessage (const TCollection_ExtendedString &theMsg,
					 const Message_Gravity theLevel)
{
  myMessenger->Send ( theMsg, theLevel );
}

