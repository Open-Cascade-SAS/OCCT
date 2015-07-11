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
#include <stdio.h>


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
  myMessenger = new Message_Messenger;
  myMessageDriver = new CDM_COutMessageDriver;
  myIsVerbose = Standard_False;
}

//=======================================================================
//function : Formats
//purpose  : 
//=======================================================================

void TObj_Application::Formats(TColStd_SequenceOfExtendedString& theFormats) 
{
  theFormats.Append(TCollection_ExtendedString ("TObjXml"));
  theFormats.Append(TCollection_ExtendedString ("TObjBin"));
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
                         const char*                       theTargetFile)
{
  myIsError = Standard_False;
  TCollection_ExtendedString aPath ((const Standard_CString)theTargetFile);

  PCDM_StoreStatus aStatus = SaveAs (theSourceDoc, aPath);
  myIsError = aStatus != PCDM_SS_OK;
  if (myIsError)
  {
    switch (aStatus)
    {
    case PCDM_SS_DriverFailure:
      ErrorMessage (Message_Msg("TObj_Appl_SDriverFailure") << aPath);
      break;
    case PCDM_SS_WriteFailure:
      ErrorMessage (Message_Msg("TObj_Appl_SWriteFailure") << aPath);
      break;
    case PCDM_SS_Failure:
      ErrorMessage (Message_Msg("TObj_Appl_SFailure") << aPath);
      break;
    case PCDM_SS_Doc_IsNull:
      ErrorMessage (Message_Msg("TObj_Appl_SDocIsNull") << aPath);
      break;
    case PCDM_SS_No_Obj:
      ErrorMessage (Message_Msg("TObj_Appl_SNoObj") << aPath);
      break;
    case PCDM_SS_Info_Section_Error:
      ErrorMessage (Message_Msg("TObj_Appl_SInfoSectionError") << aPath);
      break;
    default:
      ErrorMessage (Message_Msg("TObj_Appl_SUnknownFailure") << aPath);
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
                        (const char*                       theSourceFile,
                         Handle(TDocStd_Document)&         theTargetDoc)
{
  myIsError = Standard_False;
  TCollection_ExtendedString aPath ((const Standard_CString)theSourceFile);

  PCDM_ReaderStatus aStatus = PCDM_RS_ReaderException;
  {
    try
    {
      aStatus = Open (aPath, theTargetDoc);
    }
    catch (Standard_Failure)
    {
#ifdef OCCT_DEBUG
      ErrorMessage (Message_Msg("TObj_Appl_Exception") << 
                    Standard_Failure::Caught()->GetMessageString());
#endif
    }
  }
  myIsError = aStatus != PCDM_RS_OK;
  if (myIsError)
  {
    switch (aStatus)
    {
    case PCDM_RS_UnknownDocument:
      ErrorMessage (Message_Msg("TObj_Appl_RUnknownDocument") << aPath);
      break;
    case PCDM_RS_AlreadyRetrieved:
      ErrorMessage (Message_Msg("TObj_Appl_RAlreadyRetrieved") << aPath);
      break;
    case PCDM_RS_AlreadyRetrievedAndModified:
      ErrorMessage (Message_Msg("TObj_Appl_RAlreadyRetrievedAndModified") << aPath);
      break;
    case PCDM_RS_NoDriver:
      ErrorMessage (Message_Msg("TObj_Appl_RNoDriver") << aPath);
      break;
    case PCDM_RS_UnknownFileDriver:
      ErrorMessage (Message_Msg("TObj_Appl_RNoDriver") << aPath);
      break;
    case PCDM_RS_OpenError:
      ErrorMessage (Message_Msg("TObj_Appl_ROpenError") << aPath);
      break;
    case PCDM_RS_NoVersion:
      ErrorMessage (Message_Msg("TObj_Appl_RNoVersion") << aPath);
      break;
    case PCDM_RS_NoModel:
      ErrorMessage (Message_Msg("TObj_Appl_RNoModel") << aPath);
      break;
    case PCDM_RS_NoDocument:
      ErrorMessage (Message_Msg("TObj_Appl_RNoDocument") << aPath);
      break;
    case PCDM_RS_FormatFailure:
      ErrorMessage (Message_Msg("TObj_Appl_RFormatFailure") << aPath);
      break;
    case PCDM_RS_TypeNotFoundInSchema:
      ErrorMessage (Message_Msg("TObj_Appl_RTypeNotFound") << aPath);
      break;
    case PCDM_RS_UnrecognizedFileFormat:
      ErrorMessage (Message_Msg("TObj_Appl_RBadFileFormat") << aPath);
      break;
    case PCDM_RS_MakeFailure:
      ErrorMessage (Message_Msg("TObj_Appl_RMakeFailure") << aPath);
      break;
    case PCDM_RS_PermissionDenied:
      ErrorMessage (Message_Msg("TObj_Appl_RPermissionDenied") << aPath);
      break;
    case PCDM_RS_DriverFailure:
      ErrorMessage (Message_Msg("TObj_Appl_RDriverFailure") << aPath);
      break;
    case PCDM_RS_ReaderException:
      ErrorMessage (Message_Msg("TObj_Appl_RException") << aPath);
      break;
    default:
      ErrorMessage (Message_Msg("TObj_Appl_RUnknownFail") << aPath);
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

