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
#include <Standard_Dump.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Message_Msg.hxx>
#include <Message_MsgFile.hxx>
#include <Resource_Manager.hxx>
#include <stdio.h>

#include "TObj_TObj_msg.pxx"

IMPLEMENT_STANDARD_RTTIEXT(TObj_Application, TDocStd_Application)

//=================================================================================================

Handle(TObj_Application) TObj_Application::GetInstance()
{
  static Handle(TObj_Application) THE_TOBJ_APP(new TObj_Application);
  return THE_TOBJ_APP;
}

//=================================================================================================

TObj_Application::TObj_Application()
    : myIsError(Standard_False)
{
  if (!Message_MsgFile::HasMsg("TObj_Appl_SUnknownFailure"))
  {
    // load messages into global map on first instantiation
    Message_MsgFile::LoadFromString(TObj_TObj_msg, sizeof(TObj_TObj_msg) - 1);
    if (!Message_MsgFile::HasMsg("TObj_Appl_SUnknownFailure"))
    {
      throw Standard_ProgramError(
        "Critical Error - message resources for TObj_Application are invalid or undefined!");
    }
  }

  myMessenger = new Message_Messenger;
  myIsVerbose = Standard_False;
}

//=================================================================================================

Standard_CString TObj_Application::ResourcesName()
{
  return Standard_CString("TObj");
}

//=======================================================================
// function : SaveDocument
// purpose  : Saving the OCAF document
//=======================================================================

Standard_Boolean TObj_Application::SaveDocument(const Handle(TDocStd_Document)&   theSourceDoc,
                                                const TCollection_ExtendedString& theTargetFile)
{
  const PCDM_StoreStatus aStatus = SaveAs(theSourceDoc, theTargetFile);
  myIsError                      = (aStatus != PCDM_SS_OK);
  if (myIsError)
    SetError(aStatus, theTargetFile);

  // Release free memory
  Standard::Purge();
  return myIsError ? Standard_False : Standard_True;
}

//=======================================================================
// function : SaveDocument
// purpose  : Saving the OCAF document to a stream
//=======================================================================

Standard_Boolean TObj_Application::SaveDocument(const Handle(TDocStd_Document)& theSourceDoc,
                                                Standard_OStream&               theOStream)
{
  const PCDM_StoreStatus aStatus = SaveAs(theSourceDoc, theOStream);
  myIsError                      = (aStatus != PCDM_SS_OK);
  if (myIsError)
    SetError(aStatus, "");

  // Release free memory
  Standard::Purge();
  return myIsError ? Standard_False : Standard_True;
}

//=======================================================================
// function : LoadDocument
// purpose  : Loading the OCAF document
//=======================================================================

Standard_Boolean TObj_Application::LoadDocument(const TCollection_ExtendedString& theSourceFile,
                                                Handle(TDocStd_Document)&         theTargetDoc)
{
  PCDM_ReaderStatus aStatus = PCDM_RS_ReaderException;
  {
    try
    {
      aStatus = Open(theSourceFile, theTargetDoc);
    }
    catch (Standard_Failure const& anException)
    {
#ifdef OCCT_DEBUG
      ErrorMessage(Message_Msg("TObj_Appl_Exception") << anException.GetMessageString());
#endif
      (void)anException;
    }
  }
  myIsError = (aStatus != PCDM_RS_OK);
  if (myIsError)
    SetError(aStatus, theSourceFile);

  // Release free memory
  Standard::Purge();
  return myIsError ? Standard_False : Standard_True;
}

//=======================================================================
// function : LoadDocument
// purpose  : Loading the OCAF document from a stream
//=======================================================================

Standard_Boolean TObj_Application::LoadDocument(Standard_IStream&         theIStream,
                                                Handle(TDocStd_Document)& theTargetDoc)
{
  PCDM_ReaderStatus aStatus = PCDM_RS_ReaderException;
  {
    try
    {
      aStatus = Open(theIStream, theTargetDoc);
    }
    catch (Standard_Failure const& anException)
    {
#ifdef OCCT_DEBUG
      ErrorMessage(Message_Msg("TObj_Appl_Exception") << anException.GetMessageString());
#endif
      (void)anException;
    }
  }
  myIsError = (aStatus != PCDM_RS_OK);
  if (myIsError)
    SetError(aStatus, "");

  // Release free memory
  Standard::Purge();
  return myIsError ? Standard_False : Standard_True;
}

//=================================================================================================

Standard_Boolean TObj_Application::CreateNewDocument(Handle(TDocStd_Document)&         theDoc,
                                                     const TCollection_ExtendedString& theFormat)
{
  myIsError = Standard_False;

  // Create the Document
  NewDocument(theFormat, theDoc);

  return myIsError ? Standard_False : Standard_True;
}

//=================================================================================================

void TObj_Application::ErrorMessage(const TCollection_ExtendedString& theMsg,
                                    const Message_Gravity             theLevel)
{
  myMessenger->Send(theMsg, theLevel);
}

//=================================================================================================

void TObj_Application::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDocStd_Application)
}

//=======================================================================
// function : SetError
// purpose  : Sets an error occurred on storage of a document.
//=======================================================================

void TObj_Application::SetError(const PCDM_StoreStatus            theStatus,
                                const TCollection_ExtendedString& theInfo)
{
  switch (theStatus)
  {
    case PCDM_SS_DriverFailure:
      ErrorMessage(Message_Msg("TObj_Appl_SDriverFailure") << theInfo);
      break;
    case PCDM_SS_WriteFailure:
      ErrorMessage(Message_Msg("TObj_Appl_SWriteFailure") << theInfo);
      break;
    case PCDM_SS_Failure:
      ErrorMessage(Message_Msg("TObj_Appl_SFailure") << theInfo);
      break;
    case PCDM_SS_Doc_IsNull:
      ErrorMessage(Message_Msg("TObj_Appl_SDocIsNull") << theInfo);
      break;
    case PCDM_SS_No_Obj:
      ErrorMessage(Message_Msg("TObj_Appl_SNoObj") << theInfo);
      break;
    case PCDM_SS_Info_Section_Error:
      ErrorMessage(Message_Msg("TObj_Appl_SInfoSectionError") << theInfo);
      break;
    default:
      ErrorMessage(Message_Msg("TObj_Appl_SUnknownFailure") << theInfo);
      break;
  }
}

//=======================================================================
// function : SetError
// purpose  : Sets an error occurred on reading of a document.
//=======================================================================

void TObj_Application::SetError(const PCDM_ReaderStatus           theStatus,
                                const TCollection_ExtendedString& theInfo)
{
  switch (theStatus)
  {
    case PCDM_RS_UnknownDocument:
      ErrorMessage(Message_Msg("TObj_Appl_RUnknownDocument") << theInfo);
      break;
    case PCDM_RS_AlreadyRetrieved:
      ErrorMessage(Message_Msg("TObj_Appl_RAlreadyRetrieved") << theInfo);
      break;
    case PCDM_RS_AlreadyRetrievedAndModified:
      ErrorMessage(Message_Msg("TObj_Appl_RAlreadyRetrievedAndModified") << theInfo);
      break;
    case PCDM_RS_NoDriver:
      ErrorMessage(Message_Msg("TObj_Appl_RNoDriver") << theInfo);
      break;
    case PCDM_RS_UnknownFileDriver:
      ErrorMessage(Message_Msg("TObj_Appl_RNoDriver") << theInfo);
      break;
    case PCDM_RS_OpenError:
      ErrorMessage(Message_Msg("TObj_Appl_ROpenError") << theInfo);
      break;
    case PCDM_RS_NoVersion:
      ErrorMessage(Message_Msg("TObj_Appl_RNoVersion") << theInfo);
      break;
    case PCDM_RS_NoModel:
      ErrorMessage(Message_Msg("TObj_Appl_RNoModel") << theInfo);
      break;
    case PCDM_RS_NoDocument:
      ErrorMessage(Message_Msg("TObj_Appl_RNoDocument") << theInfo);
      break;
    case PCDM_RS_FormatFailure:
      ErrorMessage(Message_Msg("TObj_Appl_RFormatFailure") << theInfo);
      break;
    case PCDM_RS_TypeNotFoundInSchema:
      ErrorMessage(Message_Msg("TObj_Appl_RTypeNotFound") << theInfo);
      break;
    case PCDM_RS_UnrecognizedFileFormat:
      ErrorMessage(Message_Msg("TObj_Appl_RBadFileFormat") << theInfo);
      break;
    case PCDM_RS_MakeFailure:
      ErrorMessage(Message_Msg("TObj_Appl_RMakeFailure") << theInfo);
      break;
    case PCDM_RS_PermissionDenied:
      ErrorMessage(Message_Msg("TObj_Appl_RPermissionDenied") << theInfo);
      break;
    case PCDM_RS_DriverFailure:
      ErrorMessage(Message_Msg("TObj_Appl_RDriverFailure") << theInfo);
      break;
    case PCDM_RS_ReaderException:
      ErrorMessage(Message_Msg("TObj_Appl_RException") << theInfo);
      break;
    default:
      ErrorMessage(Message_Msg("TObj_Appl_RUnknownFail") << theInfo);
      break;
  }
}
