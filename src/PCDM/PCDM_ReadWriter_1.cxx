// Created on: 1997-12-09
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



#include <PCDM_ReadWriter_1.ixx>
#include <UTL.hxx>
#include <Storage_Data.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <CDM_Document.hxx>
#include <CDM_ReferenceIterator.hxx>
#include <CDM_MetaData.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <Storage_Schema.hxx>
#include <Storage_HeaderData.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <PCDM_Reference.hxx>
#include <Standard_ErrorHandler.hxx>
#include <OSD_Path.hxx>
#include <PCDM_BaseDriverPointer.hxx>
#include <PCDM.hxx>
#include <PCDM_TypeOfFileDriver.hxx>

#define START_REF "START_REF"
#define END_REF "END_REF"
#define START_EXT "START_EXT"
#define END_EXT "END_EXT"
#define MODIFICATION_COUNTER "MODIFICATION_COUNTER: "
#define REFERENCE_COUNTER "REFERENCE_COUNTER: "

//=======================================================================
//function : PCDM_ReadWriter_1
//purpose  : 
//=======================================================================

PCDM_ReadWriter_1::PCDM_ReadWriter_1() {}
static Standard_Integer RemoveExtraSeparator(TCollection_AsciiString& aString) {

  Standard_Integer i, j, len ;

  len = aString.Length() ;
#ifdef WNT
  // Case of network path, such as \\MACHINE\dir
  for (i = j = 2 ; j <= len ; i++,j++) {
#else
  for (i = j = 1 ; j <= len ; i++,j++) {
#endif
      Standard_Character c = aString.Value(j) ;
      aString.SetValue(i,c) ;
      if (c == '/')
          while(j < len && aString.Value(j+1) == '/') j++ ;
  }
  len = i-1 ;
  if (aString.Value(len) == '/') len-- ;  
  aString.Trunc(len) ;
  return len ;
}
static TCollection_AsciiString AbsolutePath(
                            const TCollection_AsciiString& aDirPath,
                            const TCollection_AsciiString& aRelFilePath)
{
  TCollection_AsciiString EmptyString = "" ;
#ifdef WNT
  if (aRelFilePath.Search(":") == 2 ||
      (aRelFilePath.Search("\\") == 1 && aRelFilePath.Value(2) == '\\'))
#else
  if(aRelFilePath.Search("/") == 1)
#endif
    return aRelFilePath ;
  
  TCollection_AsciiString DirPath = aDirPath, RelFilePath = aRelFilePath  ;
  Standard_Integer i,len ;
  
#ifdef WNT
  if(DirPath.Search(":") != 2 &&
     (DirPath.Search("\\") != 1 || DirPath.Value(2) != '\\'))
#else
  if (DirPath.Search("/") != 1 )
#endif
    return EmptyString ;

#ifdef WNT
  DirPath.ChangeAll('\\','/') ;
  RelFilePath.ChangeAll('\\','/') ;      
#endif
  
  RemoveExtraSeparator(DirPath) ;
  len = RemoveExtraSeparator(RelFilePath) ;
  
  while (RelFilePath.Search("../") == 1) {
      if (len == 3)
	  return EmptyString ;
      RelFilePath = RelFilePath.SubString(4,len) ;
      len -= 3 ;
      if (DirPath.IsEmpty())
	  return EmptyString ;
      i = DirPath.SearchFromEnd("/") ;
      if (i < 0)
          return EmptyString ;
      DirPath.Trunc(i-1) ;
  }  
  TCollection_AsciiString retx;
  retx= DirPath;
  retx+= "/";
  retx+=RelFilePath ;
  return retx;
}

static TCollection_AsciiString GetDirFromFile(const TCollection_ExtendedString& aFileName) {
  TCollection_AsciiString theCFile=UTL::CString(aFileName);
  TCollection_AsciiString theDirectory;
  Standard_Integer i=theCFile.SearchFromEnd("/");
#ifdef WNT    
//    if(i==-1) i=theCFile.SearchFromEnd("\\");
  if(theCFile.SearchFromEnd("\\") > i)
    i=theCFile.SearchFromEnd("\\");
#endif
  if(i!=-1) theDirectory=theCFile.SubString(1,i);
  return theDirectory;
}
//=======================================================================
//function : Version
//purpose  : 
//=======================================================================

TCollection_AsciiString PCDM_ReadWriter_1::Version() const {
  return "PCDM_ReadWriter_1";
}
//=======================================================================
//function : WriteReferenceCounter
//purpose  : 
//=======================================================================

void PCDM_ReadWriter_1::WriteReferenceCounter(const Handle(Storage_Data)& aData, const Handle(CDM_Document)& aDocument) const { 
  TCollection_AsciiString ligne(REFERENCE_COUNTER);
  ligne+=aDocument->ReferenceCounter();
  aData->AddToUserInfo(ligne);
}
//=======================================================================
//function : WriteReferences
//purpose  : 
//=======================================================================

void PCDM_ReadWriter_1::WriteReferences(const Handle(Storage_Data)& aData, const Handle(CDM_Document)& aDocument,const TCollection_ExtendedString& theReferencerFileName) const {  

  Standard_Integer theNumber = aDocument->ToReferencesNumber();
  if(theNumber > 0) {

    aData->AddToUserInfo(START_REF);

    CDM_ReferenceIterator it(aDocument);

    TCollection_ExtendedString ligne;

    TCollection_AsciiString theAbsoluteDirectory=GetDirFromFile(theReferencerFileName);

    for (;it.More();it.Next()) {
      ligne = TCollection_ExtendedString(it.ReferenceIdentifier());
      ligne += " ";
      ligne += TCollection_ExtendedString(it.Document()->Modifications());
      ligne += " ";

      TCollection_AsciiString thePath=UTL::CString(it.Document()->MetaData()->FileName());
      TCollection_AsciiString theRelativePath;
      if(!theAbsoluteDirectory.IsEmpty()) {
	theRelativePath=OSD_Path::RelativePath(theAbsoluteDirectory,thePath);
	if(!theRelativePath.IsEmpty()) thePath=theRelativePath;
      }
      ligne +=  UTL::ExtendedString(thePath);
      UTL::AddToUserInfo(aData,ligne);
    }
    aData->AddToUserInfo(END_REF);
  }
}

//=======================================================================
//function : WriteExtensions
//purpose  : 
//=======================================================================

void PCDM_ReadWriter_1::WriteExtensions(const Handle(Storage_Data)& aData, const Handle(CDM_Document)& aDocument) const {  

  TColStd_SequenceOfExtendedString theExtensions;
  aDocument->Extensions(theExtensions);
  Standard_Integer theNumber = theExtensions.Length();
  if(theNumber > 0) {

    aData->AddToUserInfo(START_EXT);
    for (Standard_Integer i=1; i<=theNumber; i++) {
      UTL::AddToUserInfo(aData,theExtensions(i));
    }
    aData->AddToUserInfo(END_EXT);
  }
}
//=======================================================================
//function : WriteVersion
//purpose  : 
//=======================================================================

void PCDM_ReadWriter_1::WriteVersion(const Handle(Storage_Data)& aData, const Handle(CDM_Document)& aDocument) const { 
  TCollection_AsciiString ligne(MODIFICATION_COUNTER);
  ligne+=aDocument->Modifications();
  aData->AddToUserInfo(ligne);
}
//=======================================================================
//function : ReadReferenceCounter
//purpose  : 
//=======================================================================

Standard_Integer PCDM_ReadWriter_1::ReadReferenceCounter(const TCollection_ExtendedString& aFileName, const Handle(CDM_MessageDriver)& theMsgDriver) const {

  static Standard_Integer theReferencesCounter ;
  theReferencesCounter=0;
  static Standard_Integer i ;

  PCDM_BaseDriverPointer theFileDriver;
  if(PCDM::FileDriverType(TCollection_AsciiString(UTL::CString(aFileName)), theFileDriver) == PCDM_TOFD_Unknown) return theReferencesCounter;
  
  static Standard_Boolean theFileIsOpen ;
  theFileIsOpen=Standard_False;

  try {
    OCC_CATCH_SIGNALS
    PCDM_ReadWriter::Open(*theFileDriver,aFileName,Storage_VSRead);
    theFileIsOpen=Standard_True;
   
    Handle(Storage_Schema) s = new Storage_Schema;
    Handle(Storage_HeaderData) hd = s->ReadHeaderSection(*theFileDriver);
    const TColStd_SequenceOfAsciiString &refUserInfo = hd->UserInfo();
    
    for ( i =1; i<=  refUserInfo.Length() ; i++) {
      if(refUserInfo(i).Search(REFERENCE_COUNTER) != -1) {
	try { OCC_CATCH_SIGNALS theReferencesCounter=refUserInfo(i).Token(" ",2).IntegerValue();}
	catch (Standard_Failure) { 
//	  cout << "warning: could not read the reference counter in " << aFileName << endl;
	  TCollection_ExtendedString aMsg("Warning: ");
	  aMsg = aMsg.Cat("could not read the reference counter in ").Cat(aFileName).Cat("\0");
	  if(!theMsgDriver.IsNull()) 
	    theMsgDriver->Write(aMsg.ToExtString());
	}
      }
    }
    
  }
  catch (Standard_Failure) {}

  if(theFileIsOpen)  theFileDriver->Close();

  delete theFileDriver;
  return theReferencesCounter;
}
  
//=======================================================================
//function : ReadReferences
//purpose  : 
//=======================================================================

void PCDM_ReadWriter_1::ReadReferences(const TCollection_ExtendedString& aFileName, PCDM_SequenceOfReference& theReferences, const Handle(CDM_MessageDriver)& theMsgDriver) const  {

  TColStd_SequenceOfExtendedString ReadReferences;
  
  ReadUserInfo(aFileName,START_REF,END_REF,ReadReferences, theMsgDriver);

  Standard_Integer theReferenceIdentifier;
  TCollection_ExtendedString theFileName;
  Standard_Integer theDocumentVersion;

  TCollection_AsciiString theAbsoluteDirectory=GetDirFromFile(aFileName);

  for (Standard_Integer i=1; i<=ReadReferences.Length(); i++) {
    Standard_Integer pos=ReadReferences(i).Search(" ");
    if(pos != -1) {
      TCollection_ExtendedString theRest=ReadReferences(i).Split(pos);
      theReferenceIdentifier=UTL::IntegerValue(ReadReferences(i));
    
      Standard_Integer pos2=theRest.Search(" ");
      
      theFileName=theRest.Split(pos2);
      theDocumentVersion=UTL::IntegerValue(theRest);
      
      TCollection_AsciiString thePath=UTL::CString(theFileName);
      TCollection_AsciiString theAbsolutePath;
      if(!theAbsoluteDirectory.IsEmpty()) {
	theAbsolutePath=AbsolutePath(theAbsoluteDirectory,thePath);
	if(!theAbsolutePath.IsEmpty()) thePath=theAbsolutePath;
      }
      if(!theMsgDriver.IsNull()) {
//      cout << "reference found; ReferenceIdentifier: " << theReferenceIdentifier << "; File:" << thePath << ", version:" << theDocumentVersion;
	TCollection_ExtendedString aMsg("Warning: ");
	aMsg = aMsg.Cat("reference found; ReferenceIdentifier:  ").Cat(theReferenceIdentifier).Cat("; File:").Cat(thePath).Cat(", version:").Cat(theDocumentVersion).Cat("\0");
	theMsgDriver->Write(aMsg.ToExtString());
      }
      theReferences.Append(PCDM_Reference (theReferenceIdentifier,UTL::ExtendedString(thePath),theDocumentVersion));
    
    }
  }

}

//=======================================================================
//function : ReadExtensions
//purpose  : 
//=======================================================================

void PCDM_ReadWriter_1::ReadExtensions(const TCollection_ExtendedString& aFileName, TColStd_SequenceOfExtendedString& theExtensions, const Handle(CDM_MessageDriver)& theMsgDriver) const {
  
  ReadUserInfo(aFileName,START_EXT,END_EXT,theExtensions, theMsgDriver);
}


//=======================================================================
//function : ReadUserInfo
//purpose  : 
//=======================================================================

void PCDM_ReadWriter_1::ReadUserInfo(const TCollection_ExtendedString& aFileName,
                                     const TCollection_AsciiString& Start,
                                     const TCollection_AsciiString& End,
                                     TColStd_SequenceOfExtendedString& theUserInfo,
                                     const Handle(CDM_MessageDriver)&) {

  static Standard_Integer i ;
  PCDM_BaseDriverPointer theFileDriver;
  if(PCDM::FileDriverType(TCollection_AsciiString(UTL::CString(aFileName)), theFileDriver) == PCDM_TOFD_Unknown) return;

  PCDM_ReadWriter::Open(*theFileDriver,aFileName,Storage_VSRead);
  Handle(Storage_Schema) s = new Storage_Schema;
  Handle(Storage_HeaderData) hd = s->ReadHeaderSection(*theFileDriver);
  const TColStd_SequenceOfAsciiString &refUserInfo = hd->UserInfo();

  Standard_Integer debut=0,fin=0;
  
  for ( i =1; i<=  refUserInfo.Length() ; i++) {
    TCollection_ExtendedString theLine=refUserInfo(i);
    if(refUserInfo(i)== Start) debut=i;
    if(refUserInfo(i)== End) fin=i;
  }
  if(debut != 0) {
    for (i=debut+1 ; i<fin; i++) {
      theUserInfo.Append(UTL::ExtendedString(refUserInfo(i)));
    }
  }
  theFileDriver->Close();
  delete theFileDriver;
}

//=======================================================================
//function : ReadDocumentVersion
//purpose  : 
//=======================================================================

Standard_Integer PCDM_ReadWriter_1::ReadDocumentVersion(const TCollection_ExtendedString& aFileName, const Handle(CDM_MessageDriver)& theMsgDriver) const {

  static Standard_Integer theVersion ;
  theVersion=-1;

  PCDM_BaseDriverPointer theFileDriver;
  if(PCDM::FileDriverType(TCollection_AsciiString(UTL::CString(aFileName)), theFileDriver) == PCDM_TOFD_Unknown) return theVersion;

  static Standard_Boolean theFileIsOpen ;
  theFileIsOpen =Standard_False;

  try {
    OCC_CATCH_SIGNALS
    PCDM_ReadWriter::Open(*theFileDriver,aFileName,Storage_VSRead);
    theFileIsOpen=Standard_True;
    Handle(Storage_Schema) s = new Storage_Schema;
    Handle(Storage_HeaderData) hd = s->ReadHeaderSection(*theFileDriver);
    const TColStd_SequenceOfAsciiString &refUserInfo = hd->UserInfo();

    static Standard_Integer i ;
    for ( i =1; i<=  refUserInfo.Length() ; i++) {
      if(refUserInfo(i).Search(MODIFICATION_COUNTER) != -1) {
	try { OCC_CATCH_SIGNALS theVersion=refUserInfo(i).Token(" ",2).IntegerValue();}
	catch (Standard_Failure) { 
//	  cout << "warning: could not read the version in " << aFileName << endl;
	  TCollection_ExtendedString aMsg("Warning: ");
	  aMsg = aMsg.Cat("could not read the version in ").Cat(aFileName).Cat("\0");
	  if(!theMsgDriver.IsNull()) 
	    theMsgDriver->Write(aMsg.ToExtString());
	}

      }
    }
  }

  catch (Standard_Failure) {}

  if(theFileIsOpen) theFileDriver->Close();
  delete theFileDriver;
  return theVersion;
}
