// Created on: 1997-08-07
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
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


#include <CDM_Document.hxx>
#include <FSD_CmpFile.hxx>
#include <PCDM.hxx>
#include <PCDM_Document.hxx>
#include <PCDM_DriverError.hxx>
#include <PCDM_ReadWriter.hxx>
#include <PCDM_SequenceOfDocument.hxx>
#include <PCDM_StorageDriver.hxx>
#include <Resource_Manager.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <Storage_Data.hxx>
#include <Storage_OpenMode.hxx>
#include <Storage_Schema.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>

#include <locale.h>
IMPLEMENT_STANDARD_RTTIEXT(PCDM_StorageDriver,PCDM_Writer)

#define STORAGE_VERSION "STORAGE_VERSION:"



void PCDM_StorageDriver::Write(const Handle(CDM_Document)& aDocument, const TCollection_ExtendedString&  aFileName) 
{
  Handle(Storage_Schema) theSchema = new Storage_Schema;

  Handle(Storage_Data) theData = new Storage_Data;

  static Standard_Boolean Failure;
  Failure=Standard_False;
  Standard_SStream aMsg; aMsg << "error during Make:";
  PCDM_SequenceOfDocument thePersistentDocuments;
  {
    try { OCC_CATCH_SIGNALS Make(aDocument,thePersistentDocuments);}
    catch (Standard_Failure const& anException) {
      aMsg << anException<<(char)0;
      Failure=Standard_True;
    }
  }

  if(Failure) throw PCDM_DriverError(aMsg.str().c_str());

  if(thePersistentDocuments.IsEmpty()) {
    aMsg << "the storage driver: " << DynamicType()->Name() << "returned no documents to store" << (char) 0;
    throw PCDM_DriverError(aMsg.str().c_str());
  }

  Standard_Integer i=1;
  for(; i<=thePersistentDocuments.Length(); i++) {
    theData->AddRoot(thePersistentDocuments(i));
  }

  TCollection_AsciiString ligne(STORAGE_VERSION);
  ligne+=PCDM_ReadWriter::Writer()->Version();
  theData->AddToUserInfo(ligne);

  PCDM_ReadWriter::WriteFileFormat(theData,aDocument);
  PCDM_ReadWriter::Writer()->WriteReferenceCounter(theData,aDocument);
  PCDM_ReadWriter::Writer()->WriteReferences(theData,aDocument,aFileName);
  PCDM_ReadWriter::Writer()->WriteExtensions(theData,aDocument);
  PCDM_ReadWriter::Writer()->WriteVersion(theData,aDocument);

  // add document comments
  TColStd_SequenceOfExtendedString aComments;
  aDocument->Comments(aComments);
  Standard_Integer aLen = aComments.Length();
  for (i = 1; i <= aLen; i++)
  {
    theData->AddToComments(aComments(i));
  }

  FSD_CmpFile theFile;
  PCDM_ReadWriter::Open(theFile,aFileName,Storage_VSWrite);
  theSchema->Write(theFile,theData);
  theFile.Close();

  if ( theData->ErrorStatus() != Storage_VSOk )
    throw PCDM_DriverError(theData->ErrorStatusExtension().ToCString());
}


//=======================================================================
//function : Write
//purpose  : 
//=======================================================================
void PCDM_StorageDriver::Write (const Handle(CDM_Document)& /*aDocument*/, Standard_OStream& /*theOStream*/) 
{
  
}

//Handle(PCDM_Document) PCDM_StorageDriver::Make(const Handle(CDM_Document)& aDocument) {
Handle(PCDM_Document) PCDM_StorageDriver::Make(const Handle(CDM_Document)& ) {
  Handle(PCDM_Document) voidDocument;
  Standard_SStream aMsg;
  aMsg << "No Make method were implemented in this Driver" << DynamicType()->Name() << (char) 0;
  throw Standard_NotImplemented(aMsg.str().c_str());
}

void PCDM_StorageDriver::Make(const Handle(CDM_Document)& aDocument, PCDM_SequenceOfDocument& Documents) {
  Documents.Append(Make(aDocument));
}

//=======================================================================
//function : SetFormat
//purpose  : 
//=======================================================================

void PCDM_StorageDriver::SetFormat (const TCollection_ExtendedString& aformat)
{
  myFormat = aformat;
}


//=======================================================================
//function : GetFormat
//purpose  : 
//=======================================================================

TCollection_ExtendedString PCDM_StorageDriver::GetFormat () const
{
  return myFormat;
}

//=======================================================================
//function : IsError
//purpose  :
//=======================================================================

Standard_Boolean PCDM_StorageDriver::IsError () const 
{
  return myIsError;
}

//=======================================================================
//function : SetIsError
//purpose  :
//=======================================================================
void PCDM_StorageDriver::SetIsError(const Standard_Boolean theIsError)
{
    myIsError = theIsError;
}

//=======================================================================
//function : GetStoreStatus
//purpose  :
//=======================================================================

PCDM_StoreStatus PCDM_StorageDriver::GetStoreStatus () const 
{
  return myStoreStatus;
}
//=======================================================================
//function : SetStoreStatus
//purpose  :
//=======================================================================
void PCDM_StorageDriver::SetStoreStatus(const PCDM_StoreStatus theStoreStatus)
{
    myStoreStatus = theStoreStatus;
}
