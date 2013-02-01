// Created on: 1997-08-07
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <PCDM_StorageDriver.ixx>
#include <Storage_Data.hxx>
#include <Storage_OpenMode.hxx>
#include <Storage_Schema.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <FSD_CmpFile.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_ErrorHandler.hxx>
#include <PCDM_ReadWriter.hxx>
#include <Resource_Manager.hxx>
#include <Standard_NotImplemented.hxx>
#include <PCDM_SequenceOfDocument.hxx>
#include <PCDM.hxx>
#include <locale.h>

#define STORAGE_VERSION "STORAGE_VERSION:"



void PCDM_StorageDriver::Write(const Handle(CDM_Document)& aDocument, const TCollection_ExtendedString&  aFileName) 
{
  Handle(Storage_Schema) theSchema=PCDM::Schema(SchemaName(),aDocument->Application());

  TColStd_SequenceOfExtendedString theExtensions;
  aDocument->Extensions(theExtensions);
  LoadExtensions(theSchema,theExtensions);


  Handle(Storage_Data) theData = new Storage_Data;

  static Standard_Boolean Failure;
  Failure=Standard_False;
  Standard_SStream aMsg; aMsg << "error during Make:";
  PCDM_SequenceOfDocument thePersistentDocuments;
  {
    try { OCC_CATCH_SIGNALS Make(aDocument,thePersistentDocuments);}
    catch (Standard_Failure) {
      aMsg << Standard_Failure::Caught()<<(char)0;
      Failure=Standard_True;
    }
  }

  if(Failure) PCDM_DriverError::Raise(aMsg);

  if(thePersistentDocuments.IsEmpty()) {
    aMsg << "the storage driver: " << DynamicType()->Name() << "returned no documents to store" << (char) 0;
    PCDM_DriverError::Raise(aMsg);
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
    PCDM_DriverError::Raise(theData->ErrorStatusExtension().ToCString());
}

//void PCDM_StorageDriver::LoadExtensions(const Handle(Storage_Schema)& aSchema, const TColStd_SequenceOfExtendedString& Extensions) {}
void PCDM_StorageDriver::LoadExtensions(const Handle(Storage_Schema)& , const TColStd_SequenceOfExtendedString& ) {}


//Handle(PCDM_Document) PCDM_StorageDriver::Make(const Handle(CDM_Document)& aDocument) {
Handle(PCDM_Document) PCDM_StorageDriver::Make(const Handle(CDM_Document)& ) {
  Handle(PCDM_Document) voidDocument;
  Standard_SStream aMsg;
  aMsg << "No Make method were implemented in this Driver" << DynamicType()->Name() << (char) 0;
  Standard_NotImplemented::Raise(aMsg);
  return voidDocument;
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
