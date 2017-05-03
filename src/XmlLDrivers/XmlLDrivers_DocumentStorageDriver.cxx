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
#include <CDM_NullMessageDriver.hxx>
#include <LDOM_DocumentType.hxx>
#include <LDOM_LDOMImplementation.hxx>
#include <LDOM_XmlWriter.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <OSD_OpenFile.hxx>
#include <PCDM.hxx>
#include <PCDM_ReadWriter.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <Storage_Data.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TDocStd_Document.hxx>
#include <XmlLDrivers.hxx>
#include <XmlLDrivers_DocumentStorageDriver.hxx>
#include <XmlLDrivers_NamespaceDef.hxx>
#include <XmlMDF.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>

#include <locale.h>
IMPLEMENT_STANDARD_RTTIEXT(XmlLDrivers_DocumentStorageDriver,PCDM_StorageDriver)

#define STORAGE_VERSION      "STORAGE_VERSION: "
#define REFERENCE_COUNTER    "REFERENCE_COUNTER: "
#define MODIFICATION_COUNTER "MODIFICATION_COUNTER: "
#define START_REF            "START_REF"
#define END_REF              "END_REF"

#define FAILSTR "Failed to write xsi:schemaLocation : "

//#define TAKE_TIMES
static void take_time (const Standard_Integer, const char *,
                       const Handle(CDM_MessageDriver)&)
#ifdef TAKE_TIMES
;
#else
{}
#endif

//=======================================================================
//function : XmlLDrivers_DocumentStorageDriver
//purpose  : Constructor
//=======================================================================
XmlLDrivers_DocumentStorageDriver::XmlLDrivers_DocumentStorageDriver
                                (const TCollection_ExtendedString& theCopyright)
     : myCopyright (theCopyright)
{ 
}

//=======================================================================
//function : AddNamespace
//purpose  : 
//=======================================================================

void XmlLDrivers_DocumentStorageDriver::AddNamespace
                                (const TCollection_AsciiString& thePrefix,
                                 const TCollection_AsciiString& theURI)
{
  for (Standard_Integer i = 1; i <= mySeqOfNS.Length(); i++)
    if (thePrefix == mySeqOfNS(i).Prefix()) return;
  mySeqOfNS.Append (XmlLDrivers_NamespaceDef(thePrefix, theURI));
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================
void XmlLDrivers_DocumentStorageDriver::Write (const Handle(CDM_Document)&       theDocument,
                                               const TCollection_ExtendedString& theFileName)
{
  myFileName = theFileName;

  std::ofstream aFileStream;
  OSD_OpenStream (aFileStream, theFileName, std::ios::out);

  if (aFileStream.is_open() && aFileStream.good())
  {
    Write (theDocument, aFileStream);
  }
  else
  {
    SetIsError (Standard_True);
    SetStoreStatus(PCDM_SS_WriteFailure);
    
    TCollection_ExtendedString aMsg = TCollection_ExtendedString("Error: the file ") +
                                      theFileName + " cannot be opened for writing";

    theDocument->Application()->MessageDriver()->Write (aMsg.ToExtString());
    throw Standard_Failure("File cannot be opened for writing");
  }
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================
Standard_EXPORT void XmlLDrivers_DocumentStorageDriver::Write (const Handle(CDM_Document)& theDocument,
                                                               Standard_OStream&           theOStream)
{
  Handle(CDM_MessageDriver) aMessageDriver = theDocument->Application()->MessageDriver();
  ::take_time (~0, " +++++ Start STORAGE procedures ++++++", aMessageDriver);

  // Create new DOM_Document
  XmlObjMgt_Document aDOMDoc = XmlObjMgt_Document::createDocument ("document");

  // Fill the document with data
  XmlObjMgt_Element anElement = aDOMDoc.getDocumentElement();

  if (WriteToDomDocument (theDocument, anElement) == Standard_False) {

    LDOM_XmlWriter aWriter;
    aWriter.SetIndentation(1);
  
    if (theOStream.good())
    {
      aWriter.Write (theOStream, aDOMDoc);
    }
    else
    {
      SetIsError (Standard_True);
      SetStoreStatus(PCDM_SS_WriteFailure);

      TCollection_ExtendedString aMsg = TCollection_ExtendedString("Error: the stream is bad and") +
                                        " cannot be used for writing";
      theDocument->Application()->MessageDriver()->Write (aMsg.ToExtString());
      
      throw Standard_Failure("File cannot be opened for writing");
    }

    ::take_time (0, " +++++ Fin formatting to XML : ", aMessageDriver);
  }
}

//=======================================================================
//function : WriteToDomDocument
//purpose  : management of the macro-structure of XML document data
//remark   : If the application needs to use myRelocTable to store additional
//           data to XML, this method should be reimplemented avoiding step 3
//=======================================================================

Standard_Boolean XmlLDrivers_DocumentStorageDriver::WriteToDomDocument (const Handle(CDM_Document)&  theDocument,
                                                                        XmlObjMgt_Element&           theElement)
{
  SetIsError(Standard_False);
  Handle(CDM_MessageDriver) aMessageDriver =
    theDocument -> Application() -> MessageDriver();
  // 1. Write header information
  Standard_Integer i;
  XmlObjMgt_Document aDOMDoc = theElement.getOwnerDocument();

  // 1.a File Format
  TCollection_AsciiString aStorageFormat (theDocument->StorageFormat(), '?');
  theElement.setAttribute ("format", aStorageFormat.ToCString());
//  theElement.setAttribute ("schema", "XSD");

  theElement.setAttribute ("xmlns" , "http://www.opencascade.org/OCAF/XML");
  for (i = 1; i <= mySeqOfNS.Length(); i++) {
    TCollection_AsciiString aPrefix =
      TCollection_AsciiString("xmlns:") + mySeqOfNS(i).Prefix().ToCString();
    theElement.setAttribute (aPrefix.ToCString(),
                             mySeqOfNS(i).URI().ToCString());
  }
  theElement.setAttribute ("xmlns:xsi",
                           "http://www.w3.org/2001/XMLSchema-instance");
  //mkv 15.09.05 OCC10001
  //theElement.setAttribute ("xsi:schemaLocation",
  //                         "http://www.opencascade.org/OCAF/XML"
  //                         " http://www.nnov.matra-dtv.fr/~agv/XmlOcaf.xsd");
  //
  // the order of search : by CSF_XmlOcafResource and then by CASROOT
  TCollection_AsciiString anHTTP = "http://www.opencascade.org/OCAF/XML";
  Standard_Boolean aToSetCSFVariable = Standard_False;
  const char * aCSFVariable [2] = {
    "CSF_XmlOcafResource",
    "CASROOT"
  };
  OSD_Environment anEnv (aCSFVariable[0]);
  TCollection_AsciiString aResourceDir = anEnv.Value();
  if (aResourceDir.IsEmpty()) {
    // now try by CASROOT
    OSD_Environment anEnv2(aCSFVariable[1]);
    aResourceDir = anEnv2.Value();
    if ( !aResourceDir.IsEmpty() ) {
      aResourceDir += "/src/XmlOcafResource" ;
      aToSetCSFVariable = Standard_True; //CSF variable to be set later
    }
#ifdef OCCT_DEBUG
    else {
      TCollection_ExtendedString aWarn = FAILSTR "Neither ";
      aWarn = (aWarn + aCSFVariable[0] + ", nor " + aCSFVariable[1]
               + " variables have been set");
      aMessageDriver->Write (aWarn.ToExtString());
    }
#endif
  }
  if (!aResourceDir.IsEmpty()) {
    TCollection_AsciiString aResourceFileName =  aResourceDir + "/XmlOcaf.xsd";
    // search directory name that has been constructed, now check whether
    // it and the file exist
    OSD_File aResourceFile ( aResourceFileName );
    if ( aResourceFile.Exists() ) {
      if (aToSetCSFVariable) {
        OSD_Environment aCSFVarEnv ( aCSFVariable[0], aResourceDir );
        aCSFVarEnv.Build();
#ifdef OCCT_DEBUG
        TCollection_ExtendedString aWarn1 = "Variable ";
        aWarn1 = (aWarn1 + aCSFVariable[0]
                  + " has not been explicitly defined. Set to " + aResourceDir);
        aMessageDriver->Write (aWarn1.ToExtString());
#endif
        if ( aCSFVarEnv.Failed() ) {
          TCollection_ExtendedString aWarn = FAILSTR "Failed to initialize ";
          aWarn = aWarn + aCSFVariable[0] + " with " + aResourceDir;
          aMessageDriver->Write (aWarn.ToExtString());
        }
      }
    }
#ifdef OCCT_DEBUG
    else {
      TCollection_ExtendedString aWarn = FAILSTR "Schema definition file ";
      aWarn += (aResourceFileName + " was not found");
      aMessageDriver->Write (aWarn.ToExtString());
    }
#endif
    anHTTP = anHTTP + ' ' + aResourceFileName;
  }
  theElement.setAttribute ("xsi:schemaLocation", anHTTP.ToCString() );

  // 1.b Info section
  XmlObjMgt_Element anInfoElem = aDOMDoc.createElement("info");
  theElement.appendChild(anInfoElem);

  TCollection_AsciiString aCreationDate = XmlLDrivers::CreationDate();

//  anInfoElem.setAttribute("dbv", 0);
  anInfoElem.setAttribute("date", aCreationDate.ToCString());
  anInfoElem.setAttribute("schemav", 0);
//  anInfoElem.setAttribute("appv", anAppVersion.ToCString());

  // Document version
  anInfoElem.setAttribute("DocVersion", XmlLDrivers::StorageVersion());
 
  // User info with Copyright
  TColStd_SequenceOfAsciiString aUserInfo;
  if (myCopyright.Length() > 0)
    aUserInfo.Append (TCollection_AsciiString(myCopyright,'?'));

  Handle(Storage_Data) theData = new Storage_Data;
  //PCDM_ReadWriter::WriteFileFormat( theData, theDocument );
  PCDM_ReadWriter::Writer()->WriteReferenceCounter(theData,theDocument);
  PCDM_ReadWriter::Writer()->WriteReferences(theData,theDocument, myFileName);
  PCDM_ReadWriter::Writer()->WriteExtensions(theData,theDocument);
  PCDM_ReadWriter::Writer()->WriteVersion(theData,theDocument);

  const TColStd_SequenceOfAsciiString& aRefs = theData->UserInfo();
  for(i = 1; i <= aRefs.Length(); i++)
    aUserInfo.Append(aRefs.Value(i));

  for (i = 1; i <= aUserInfo.Length(); i++)
  {
    XmlObjMgt_Element aUIItem = aDOMDoc.createElement ("iitem");
    anInfoElem.appendChild (aUIItem);
    LDOM_Text aUIText = aDOMDoc.createTextNode (aUserInfo(i).ToCString());
    aUIItem.appendChild (aUIText);
  }

  // 1.c Comments section
  TColStd_SequenceOfExtendedString aComments;
  theDocument->Comments(aComments);

  XmlObjMgt_Element aCommentsElem = aDOMDoc.createElement ("comments");
  theElement.appendChild (aCommentsElem);

  for (i = 1; i <= aComments.Length(); i++)
  {
    XmlObjMgt_Element aCItem = aDOMDoc.createElement ("citem");
    aCommentsElem.appendChild (aCItem);
    XmlObjMgt::SetExtendedString (aCItem, aComments(i));
  }

  // 2a. Write document contents
  Standard_Integer anObjNb = 0;
  {
    try
    {
      OCC_CATCH_SIGNALS
      anObjNb = MakeDocument(theDocument, theElement);
    }
    catch (Standard_Failure const& anException)
    {
      SetIsError (Standard_True);
      SetStoreStatus(PCDM_SS_Failure);
      TCollection_ExtendedString anErrorString (anException.GetMessageString());
      aMessageDriver -> Write (anErrorString.ToExtString());
    }
  }
  if (anObjNb <= 0 && IsError() == Standard_False) {
    SetIsError (Standard_True);
    SetStoreStatus(PCDM_SS_No_Obj);
    TCollection_ExtendedString anErrorString ("error occurred");
    aMessageDriver -> Write (anErrorString.ToExtString());
  }
  // 2b. Write number of objects into the info section
  anInfoElem.setAttribute("objnb", anObjNb);
  ::take_time (0, " +++++ Fin DOM data for OCAF : ", aMessageDriver);

  // 3. Clear relocation table
  //    If the application needs to use myRelocTable to store additional
  //    data to XML, this method should be reimplemented avoiding this step
  myRelocTable.Clear();

  // 4. Write Shapes section
  if(WriteShapeSection(theElement))
    ::take_time (0, " +++ Fin DOM data for Shapes : ", aMessageDriver);
  return IsError();
}

//=======================================================================
//function : MakeDocument
//purpose  : 
//=======================================================================
Standard_Integer XmlLDrivers_DocumentStorageDriver::MakeDocument
                                    (const Handle(CDM_Document)& theTDoc,
                                     XmlObjMgt_Element&          theElement) 
{  
  TCollection_ExtendedString aMessage;
  Handle(TDocStd_Document) TDOC = Handle(TDocStd_Document)::DownCast(theTDoc);  
  myRelocTable.Clear();
  if (!TDOC.IsNull()) 
  {
//    myRelocTable.SetDocument (theElement.getOwnerDocument());
    Handle(TDF_Data) aTDF = TDOC->GetData();

//      Find MessageDriver and pass it to AttributeDrivers()
    Handle(CDM_Application) anApplication= theTDoc -> Application();
    Handle(CDM_MessageDriver) aMessageDriver;
    if (anApplication.IsNull())
      aMessageDriver = new CDM_NullMessageDriver;
    else
      aMessageDriver = anApplication -> MessageDriver();
    if (myDrivers.IsNull()) myDrivers = AttributeDrivers (aMessageDriver);

//      Retrieve from DOM_Document
    XmlMDF::FromTo (aTDF, theElement, myRelocTable, myDrivers); 
#ifdef OCCT_DEBUG
    aMessage = "First step successfull";
    aMessageDriver -> Write (aMessage.ToExtString());
#endif
    return myRelocTable.Extent();
  }
#ifdef OCCT_DEBUG
  cout << "First step failed" << endl;  // No MessageDriver available
#endif
  return -1; // error
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================
Handle(XmlMDF_ADriverTable) XmlLDrivers_DocumentStorageDriver::AttributeDrivers
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
struct timeb  tmbuf0;

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
//function : WriteShapeSection
//purpose  : defines WriteShapeSection
//=======================================================================
Standard_Boolean XmlLDrivers_DocumentStorageDriver::WriteShapeSection
                                (XmlObjMgt_Element&  /*theElement*/)
{
  // empty; should be redefined in subclasses
  return Standard_False;
}

