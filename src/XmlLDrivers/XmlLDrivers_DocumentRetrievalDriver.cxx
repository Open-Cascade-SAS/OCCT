// Created on: 2001-07-09
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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


#include <CDM_Application.hxx>
#include <CDM_Document.hxx>
#include <CDM_MessageDriver.hxx>
#include <CDM_MetaData.hxx>
#include <LDOM_DocumentType.hxx>
#include <LDOM_LDOMImplementation.hxx>
#include <LDOMParser.hxx>
#include <OSD_Path.hxx>
#include <OSD_OpenFile.hxx>
#include <PCDM_Document.hxx>
#include <PCDM_DOMHeaderParser.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF_Data.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Owner.hxx>
#include <UTL.hxx>
#include <XmlLDrivers.hxx>
#include <XmlLDrivers_DocumentRetrievalDriver.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMDF.hxx>
#include <XmlMDF_ADriver.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlLDrivers_DocumentRetrievalDriver,PCDM_RetrievalDriver)

#ifdef _MSC_VER
# include <tchar.h>
#endif  // _MSC_VER

#include <locale.h>
#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

#define START_REF         "START_REF"
#define END_REF           "END_REF"

#define MODIFICATION_COUNTER "MODIFICATION_COUNTER: "
#define REFERENCE_COUNTER    "REFERENCE_COUNTER: "

//#define TAKE_TIMES
static void take_time (const Standard_Integer, const char *,
                       const Handle(CDM_MessageDriver)&)
#ifdef TAKE_TIMES
;
#else
{}
#endif

static Standard_Integer RemoveExtraSeparator(TCollection_AsciiString& aString) {

  Standard_Integer i, j, len ;

  len = aString.Length() ;
#ifdef _WIN32
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
static TCollection_AsciiString GetDirFromFile(const TCollection_ExtendedString& aFileName) {
  TCollection_AsciiString theCFile=UTL::CString(aFileName);
  TCollection_AsciiString theDirectory;
  Standard_Integer i=theCFile.SearchFromEnd("/");
#ifdef _WIN32    
//    if(i==-1) i=theCFile.SearchFromEnd("\\");
  if(theCFile.SearchFromEnd("\\") > i)
    i=theCFile.SearchFromEnd("\\");
#endif
  if(i!=-1) theDirectory=theCFile.SubString(1,i);
  return theDirectory;
}

static TCollection_AsciiString AbsolutePath(
                            const TCollection_AsciiString& aDirPath,
                            const TCollection_AsciiString& aRelFilePath)
{
  TCollection_AsciiString EmptyString = "" ;
#ifdef _WIN32
  if (aRelFilePath.Search(":") == 2 ||
      (aRelFilePath.Search("\\") == 1 && aRelFilePath.Value(2) == '\\'))
#else
  if(aRelFilePath.Search("/") == 1)
#endif
    return aRelFilePath ;
  
  TCollection_AsciiString DirPath = aDirPath, RelFilePath = aRelFilePath  ;
  Standard_Integer i,len ;
  
#ifdef _WIN32
  if(DirPath.Search(":") != 2 &&
     (DirPath.Search("\\") != 1 || DirPath.Value(2) != '\\'))
#else
  if (DirPath.Search("/") != 1 )
#endif
    return EmptyString ;

#ifdef _WIN32
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

//=======================================================================
//function : XmlLDrivers_DocumentRetrievalDriver
//purpose  : Constructor
//=======================================================================
XmlLDrivers_DocumentRetrievalDriver::XmlLDrivers_DocumentRetrievalDriver()
{
  myReaderStatus = PCDM_RS_OK;
}

//=======================================================================
//function : CreateDocument
//purpose  : pure virtual method definition
//=======================================================================
Handle(CDM_Document) XmlLDrivers_DocumentRetrievalDriver::CreateDocument()
{
  return new TDocStd_Document(PCDM_RetrievalDriver::GetFormat());
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================
void XmlLDrivers_DocumentRetrievalDriver::Read
  (const TCollection_ExtendedString& theFileName,
   const Handle(CDM_Document)&       theNewDocument,
   const Handle(CDM_Application)&    theApplication)
{
  myReaderStatus = PCDM_RS_DriverFailure;
  myFileName = theFileName;

  std::ifstream aFileStream;
  OSD_OpenStream (aFileStream, myFileName, std::ios::in);

  if (aFileStream.is_open() && aFileStream.good())
  {
    Read (aFileStream, NULL, theNewDocument, theApplication);
  }
  else
  {
    myReaderStatus = PCDM_RS_OpenError;
   
    TCollection_ExtendedString aMsg = TCollection_ExtendedString("Error: the file ") +
                                      theFileName + " cannot be opened for reading";

    theApplication->MessageDriver()->Write (aMsg.ToExtString());
    throw Standard_Failure("File cannot be opened for reading");
  }
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================
void XmlLDrivers_DocumentRetrievalDriver::Read (Standard_IStream&              theIStream,
                                                const Handle(Storage_Data)&    /*theStorageData*/,
                                                const Handle(CDM_Document)&    theNewDocument,
                                                const Handle(CDM_Application)& theApplication)
{
  Handle(CDM_MessageDriver) aMessageDriver = theApplication -> MessageDriver();
  ::take_time (~0, " +++++ Start RETRIEVE procedures ++++++", aMessageDriver);

  // 1. Read DOM_Document from file
  LDOMParser aParser;

  // if myFileName is not empty, "document" tag is required to be read 
  // from the received document
  Standard_Boolean aWithoutRoot = myFileName.IsEmpty();

  if (aParser.parse(theIStream, Standard_False, aWithoutRoot))
  {
    TCollection_AsciiString aData;
    cout << aParser.GetError(aData) << ": " << aData << endl;
    myReaderStatus = PCDM_RS_FormatFailure;
    return;
  }
  const XmlObjMgt_Element anElement= aParser.getDocument().getDocumentElement();
  ::take_time (0, " +++++ Fin parsing XML :       ", aMessageDriver);

  ReadFromDomDocument (anElement, theNewDocument, theApplication);
}

//=======================================================================
//function : ReadFromDomDocument
//purpose  : management of the macro-structure of XML document data
//remark   : If the application needs to use myRelocTable to retrieve additional
//           data from LDOM, this method should be reimplemented
//=======================================================================

void XmlLDrivers_DocumentRetrievalDriver::ReadFromDomDocument
                                (const XmlObjMgt_Element&       theElement,
                                 const Handle(CDM_Document)&    theNewDocument,
                                 const Handle(CDM_Application)& theApplication)
{
  const Handle(CDM_MessageDriver) aMsgDriver =
    theApplication -> MessageDriver();
  // 1. Read info // to be done
  TCollection_AsciiString anAbsoluteDirectory = GetDirFromFile(myFileName);
  Standard_Integer aCurDocVersion = 0;
  TCollection_ExtendedString anInfo;
  const XmlObjMgt_Element anInfoElem =
    theElement.GetChildByTagName ("info");
  if (anInfoElem != NULL) {
    XmlObjMgt_DOMString aDocVerStr = anInfoElem.getAttribute("DocVersion");
    if(aDocVerStr == NULL)
      aCurDocVersion = 2;
    else if (!aDocVerStr.GetInteger(aCurDocVersion)) {
      TCollection_ExtendedString aMsg =
        TCollection_ExtendedString ("Cannot retrieve the current Document version"
                                    " attribute as \"") + aDocVerStr + "\"";
      if(!aMsgDriver.IsNull()) 
        aMsgDriver->Write(aMsg.ToExtString());
    }
    
    // oan: OCC22305 - check a document verison and if it's greater than
    // current version of storage driver set an error status and return
    if( aCurDocVersion > XmlLDrivers::StorageVersion() )
    {
      TCollection_ExtendedString aMsg =
        TCollection_ExtendedString ("error: wrong file version: ") +
                                    aDocVerStr  + " while current is " +
                                    XmlLDrivers::StorageVersion();
      myReaderStatus = PCDM_RS_NoVersion;
      if(!aMsgDriver.IsNull()) 
        aMsgDriver->Write(aMsg.ToExtString());
      return;
    }

    if( aCurDocVersion < 2) aCurDocVersion = 2;

    PropagateDocumentVersion(aCurDocVersion);

    Standard_Boolean isRef = Standard_False;
    for (LDOM_Node aNode = anInfoElem.getFirstChild();
         aNode != NULL; aNode = aNode.getNextSibling()) {
      if (aNode.getNodeType() == LDOM_Node::ELEMENT_NODE) {
        if (XmlObjMgt::GetExtendedString ((LDOM_Element&)aNode, anInfo)) {

    // Read ref counter
    if(anInfo.Search(REFERENCE_COUNTER) != -1) {
      try {
        OCC_CATCH_SIGNALS
        TCollection_AsciiString anInf(anInfo,'?');
        Standard_Integer aRefCounter = anInf.Token(" ",2).IntegerValue();
        theNewDocument->SetReferenceCounter(aRefCounter);
      }
      catch (Standard_Failure) { 
        //    cout << "warning: could not read the reference counter in " << aFileName << endl;
        TCollection_ExtendedString aMsg("Warning: ");
        aMsg = aMsg.Cat("could not read the reference counter").Cat("\0");
        if(!aMsgDriver.IsNull()) 
    aMsgDriver->Write(aMsg.ToExtString());
      }
    }
    else if (anInfo.Search(MODIFICATION_COUNTER) != -1) {
      try {
        OCC_CATCH_SIGNALS
        
        TCollection_AsciiString anInf(anInfo,'?');
        Standard_Integer aModCounter = anInf.Token(" ",2).IntegerValue();
        theNewDocument->SetModifications (aModCounter);
      }
      catch (Standard_Failure) { 
        TCollection_ExtendedString aMsg("Warning: could not read the modification counter\0");
        if(!aMsgDriver.IsNull()) 
          aMsgDriver->Write(aMsg.ToExtString());
      }
    }
    
    if(anInfo == END_REF)
      isRef = Standard_False;
    if(isRef) { // Process References
      
      Standard_Integer pos=anInfo.Search(" ");
      if(pos != -1) {
        // Parce RefId, DocumentVersion and FileName
        Standard_Integer aRefId;
        TCollection_ExtendedString aFileName;
        Standard_Integer aDocumentVersion;


        TCollection_ExtendedString aRest=anInfo.Split(pos);
        aRefId = UTL::IntegerValue(anInfo);
        
        Standard_Integer pos2 = aRest.Search(" ");
        
        aFileName = aRest.Split(pos2);
        aDocumentVersion = UTL::IntegerValue(aRest);
        
        TCollection_AsciiString aPath = UTL::CString(aFileName);
        TCollection_AsciiString anAbsolutePath;
        if(!anAbsoluteDirectory.IsEmpty()) {
    anAbsolutePath = AbsolutePath(anAbsoluteDirectory,aPath);
    if(!anAbsolutePath.IsEmpty()) aPath=anAbsolutePath;
        }
        if(!aMsgDriver.IsNull()) {
    //      cout << "reference found; ReferenceIdentifier: " << theReferenceIdentifier << "; File:" << thePath << ", version:" << theDocumentVersion;
    TCollection_ExtendedString aMsg("Warning: ");
    aMsg = aMsg.Cat("reference found; ReferenceIdentifier:  ").Cat(aRefId).Cat("; File:").Cat(aPath).Cat(", version:").Cat(aDocumentVersion).Cat("\0");
    aMsgDriver->Write(aMsg.ToExtString());
        }
        // Add new ref!
        /////////////
    TCollection_ExtendedString theFolder,theName;
        //TCollection_ExtendedString theFile=myReferences(myIterator).FileName();
        TCollection_ExtendedString f(aPath);
#ifndef _WIN32
        
        Standard_Integer i= f.SearchFromEnd("/");
        TCollection_ExtendedString n = f.Split(i); 
        f.Trunc(f.Length()-1);
        theFolder = f;
        theName = n;
#else
        OSD_Path p = UTL::Path(f);
        Standard_ExtCharacter      chr;
        TCollection_ExtendedString dir, dirRet, name;
        
        dir = UTL::Disk(p);
        dir += UTL::Trek(p);
        
        for ( int i = 1; i <= dir.Length (); ++i ) {
    
    chr = dir.Value ( i );
    
    switch ( chr ) {

    case '|':
      dirRet += "/";
      break;

    case '^':

      dirRet += "..";
      break;
      
    default:
      dirRet += chr;
      
    }  
        }
        theFolder = dirRet;
        theName   = UTL::Name(p); theName+= UTL::Extension(p);
#endif  // _WIN32
        
        Handle(CDM_MetaData) aMetaData =  CDM_MetaData::LookUp(theFolder,theName,aPath,aPath,UTL::IsReadOnly(aFileName));
////////////
        theNewDocument->CreateReference(aMetaData,aRefId,
             theApplication,aDocumentVersion,Standard_False);

        
      }

      
    }
    if(anInfo == START_REF)
      isRef = Standard_True;
        }
      }
    }
  }

  // 2. Read comments
  TCollection_ExtendedString aComment;
  const XmlObjMgt_Element aCommentsElem =
    theElement.GetChildByTagName ("comments");
  if (aCommentsElem != NULL)
  {
    for (LDOM_Node aNode = aCommentsElem.getFirstChild();
         aNode != NULL; aNode = aNode.getNextSibling())
    {
      if (aNode.getNodeType() == LDOM_Node::ELEMENT_NODE)
      {
        if (XmlObjMgt::GetExtendedString ((LDOM_Element&)aNode, aComment))
        {
          theNewDocument->AddComment(aComment);
        }
      }
    }
  }

  // 2. Read Shapes section
  if (myDrivers.IsNull()) myDrivers = AttributeDrivers (aMsgDriver);  
  const Handle(XmlMDF_ADriver) aNSDriver = ReadShapeSection(theElement, aMsgDriver);
  if(!aNSDriver.IsNull())
    ::take_time (0, " +++++ Fin reading Shapes :    ", aMsgDriver);

  // 5. Read document contents
  try
  {
    OCC_CATCH_SIGNALS
#ifdef OCCT_DEBUG
    TCollection_ExtendedString aMessage ("PasteDocument");
    aMsgDriver -> Write (aMessage.ToExtString());
#endif
    if (!MakeDocument(theElement, theNewDocument))
      myReaderStatus = PCDM_RS_MakeFailure;
    else
      myReaderStatus = PCDM_RS_OK;
  }
  catch (Standard_Failure const& anException)
  {
    TCollection_ExtendedString anErrorString (anException.GetMessageString());
    aMsgDriver -> Write (anErrorString.ToExtString());
  }

  //    Wipe off the shapes written to the <shapes> section
  ShapeSetCleaning(aNSDriver);

  //    Clean the relocation table.
  //    If the application needs to use myRelocTable to retrieve additional
  //    data from LDOM, this method should be reimplemented avoiding this step
  myRelocTable.Clear();
  ::take_time (0, " +++++ Fin reading data OCAF : ", aMsgDriver);
}

//=======================================================================
//function : MakeDocument
//purpose  : 
//=======================================================================
Standard_Boolean XmlLDrivers_DocumentRetrievalDriver::MakeDocument
                                    (const XmlObjMgt_Element&    theElement,
                                     const Handle(CDM_Document)& theTDoc)
{
  Standard_Boolean aResult = Standard_False;
  Handle(TDocStd_Document) TDOC = Handle(TDocStd_Document)::DownCast(theTDoc);
  myRelocTable.Clear();
  if (!TDOC.IsNull()) 
  {
    Handle(TDF_Data) aTDF = new TDF_Data();
    aResult = XmlMDF::FromTo (theElement, aTDF, myRelocTable, myDrivers);
    if (aResult) {
      TDOC->SetData (aTDF);
      TDocStd_Owner::SetDocument (aTDF, TDOC);
    }
  }
  return aResult;
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================
Handle(XmlMDF_ADriverTable) XmlLDrivers_DocumentRetrievalDriver::AttributeDrivers
       (const Handle(CDM_MessageDriver)& theMessageDriver) 
{
  return XmlLDrivers::AttributeDrivers (theMessageDriver);
}

//=======================================================================
//function : take_time
//class    : static
//purpose  : output astronomical time elapsed
//=======================================================================
#ifdef TAKE_TIMES
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <stdio.h>
#ifndef _WIN32
extern "C" int ftime (struct timeb *tp);
#endif
extern struct timeb  tmbuf0;

static void take_time (const Standard_Integer isReset, const char * aHeader,
                       const Handle(CDM_MessageDriver)& aMessageDriver)
{
  struct timeb  tmbuf;
  ftime (&tmbuf);
  TCollection_ExtendedString aMessage ((Standard_CString)aHeader);
  if (isReset) tmbuf0 = tmbuf;
  else {
    char take_tm_buf [64];
    Sprintf (take_tm_buf, "%9.2f s ++++",
             double(tmbuf.time - tmbuf0.time) +
             double(tmbuf.millitm - tmbuf0.millitm)/1000.);
    aMessage += take_tm_buf;
  }
  aMessageDriver -> Write (aMessage.ToExtString());
}
#endif

//=======================================================================
//function : PropagateDocumentVersion
//purpose  : 
//=======================================================================
void XmlLDrivers_DocumentRetrievalDriver::PropagateDocumentVersion(
                                   const Standard_Integer theDocVersion )
{
  XmlMDataStd::SetDocumentVersion(theDocVersion);
}

//=======================================================================
//function : ReadShapeSection
//purpose  : definition of ReadShapeSection
//=======================================================================
Handle(XmlMDF_ADriver) XmlLDrivers_DocumentRetrievalDriver::ReadShapeSection(
                               const XmlObjMgt_Element&       /*theElement*/,
             const Handle(CDM_MessageDriver)& /*aMsgDriver*/)
{
  Handle(XmlMDF_ADriver) aDriver;
  //empty; to be redefined
  return aDriver;
}

//=======================================================================
//function : ShapeSetCleaning
//purpose  : definition of ShapeSetCleaning
//=======================================================================
void XmlLDrivers_DocumentRetrievalDriver::ShapeSetCleaning(
            const Handle(XmlMDF_ADriver)& /*theDriver*/) 
{}
