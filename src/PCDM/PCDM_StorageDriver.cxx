// File:	PCDM_StorageDriver.cxx
// Created:	Thu Aug  7 14:40:31 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


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



void PCDM_StorageDriver::Write(const Handle(CDM_Document)& aDocument, const TCollection_ExtendedString&  aFileName) {

 // on sauvegarde l'ancien LC_NUMERIC
  char *oldnum,*plocal ;
  plocal =   setlocale(LC_NUMERIC, NULL) ;
  oldnum = new char[strlen(plocal)+1] ;
  strcpy(oldnum,plocal);

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

  // on remet le LC_NUMERIC a la precedente valeur
  setlocale(LC_NUMERIC, oldnum) ;
  delete[] oldnum;

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
