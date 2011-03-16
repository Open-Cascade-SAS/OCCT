// File:        TObj_Application.cxx
// Created:     Tue Nov 23 11:08:43 2004
// Author:      Pavel TELKOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_Application.hxx>

#include <Standard_SStream.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <CDM_COutMessageDriver.hxx>
#include <Message_Msg.hxx>
#include <stdio.h>

IMPLEMENT_STANDARD_HANDLE(TObj_Application,TDocStd_Application)
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

TObj_Application::TObj_Application ()
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

  CDF_StoreStatus aStatus = SaveAs (theSourceDoc, aPath);
  myIsError = aStatus != CDF_SS_OK;
  if (myIsError)
  {
    switch (aStatus)
    {
    case CDF_SS_DriverFailure:
      ErrorMessage (Message_Msg("TObj_Appl_SDriverFailure") << aPath);
      break;
    case CDF_SS_WriteFailure:
      ErrorMessage (Message_Msg("TObj_Appl_SWriteFailure") << aPath);
      break;
    case CDF_SS_Failure:
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

  CDF_RetrievableStatus aStatus = (CDF_RetrievableStatus) -1;
  {
    try
    {
      aStatus = Open (aPath, theTargetDoc);
    }
    catch (Standard_Failure)
    {
#if defined(_DEBUG) || defined(DEB)
      ErrorMessage (Message_Msg("TObj_Appl_Exception") << 
                    Standard_Failure::Caught()->GetMessageString());
#endif
    }
  }
  myIsError = aStatus != CDF_RS_OK;
  if (myIsError)
  {
    switch (aStatus)
    {
    case CDF_RS_UnknownDocument:
      ErrorMessage (Message_Msg("TObj_Appl_RUnknownDocument") << aPath);
      break;
    case CDF_RS_AlreadyRetrieved:
      ErrorMessage (Message_Msg("TObj_Appl_RAlreadyRetrieved") << aPath);
      break;
    case CDF_RS_AlreadyRetrievedAndModified:
      ErrorMessage (Message_Msg("TObj_Appl_RAlreadyRetrievedAndModified") << aPath);
      break;
    case CDF_RS_NoDriver:
      ErrorMessage (Message_Msg("TObj_Appl_RNoDriver") << aPath);
      break;
    case CDF_RS_UnknownFileDriver:
      ErrorMessage (Message_Msg("TObj_Appl_RNoDriver") << aPath);
      break;
    case CDF_RS_OpenError:
      ErrorMessage (Message_Msg("TObj_Appl_ROpenError") << aPath);
      break;
    case CDF_RS_NoVersion:
      ErrorMessage (Message_Msg("TObj_Appl_RNoVersion") << aPath);
      break;
    case CDF_RS_NoModel:
      ErrorMessage (Message_Msg("TObj_Appl_RNoModel") << aPath);
      break;
    case CDF_RS_NoDocument:
      ErrorMessage (Message_Msg("TObj_Appl_RNoDocument") << aPath);
      break;
    case CDF_RS_FormatFailure:
      ErrorMessage (Message_Msg("TObj_Appl_RFormatFailure") << aPath);
      break;
    case CDF_RS_TypeNotFoundInSchema:
      ErrorMessage (Message_Msg("TObj_Appl_RTypeNotFound") << aPath);
      break;
    case CDF_RS_UnrecognizedFileFormat:
      ErrorMessage (Message_Msg("TObj_Appl_RBadFileFormat") << aPath);
      break;
    case CDF_RS_MakeFailure:
      ErrorMessage (Message_Msg("TObj_Appl_RMakeFailure") << aPath);
      break;
    case CDF_RS_PermissionDenied:
      ErrorMessage (Message_Msg("TObj_Appl_RPermissionDenied") << aPath);
      break;
    case CDF_RS_DriverFailure:
      ErrorMessage (Message_Msg("TObj_Appl_RDriverFailure") << aPath);
      break;
    case -1:
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

