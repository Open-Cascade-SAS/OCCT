// Created on: 2002-10-31
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _BinLDrivers_DocumentRetrievalDriver_HeaderFile
#define _BinLDrivers_DocumentRetrievalDriver_HeaderFile

#include <Standard.hxx>

#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <BinLDrivers_VectorOfDocumentSection.hxx>
#include <PCDM_RetrievalDriver.hxx>
#include <Standard_Integer.hxx>
#include <Standard_IStream.hxx>
#include <Storage_Position.hxx>
#include <Storage_Data.hxx>

class BinMDF_ADriverTable;
class Message_Messenger;
class TCollection_ExtendedString;
class CDM_Document;
class CDM_Application;
class TDF_Label;
class BinLDrivers_DocumentSection;

class BinLDrivers_DocumentRetrievalDriver;
DEFINE_STANDARD_HANDLE(BinLDrivers_DocumentRetrievalDriver, PCDM_RetrievalDriver)

class BinLDrivers_DocumentRetrievalDriver : public PCDM_RetrievalDriver
{

public:
  //! Constructor
  Standard_EXPORT BinLDrivers_DocumentRetrievalDriver();

  //! retrieves the content of the file into a new Document.
  Standard_EXPORT virtual void Read(
    const TCollection_ExtendedString& theFileName,
    const Handle(CDM_Document)&       theNewDocument,
    const Handle(CDM_Application)&    theApplication,
    const Handle(PCDM_ReaderFilter)&  theFilter   = Handle(PCDM_ReaderFilter)(),
    const Message_ProgressRange&      theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  Standard_EXPORT virtual void Read(
    Standard_IStream&                theIStream,
    const Handle(Storage_Data)&      theStorageData,
    const Handle(CDM_Document)&      theDoc,
    const Handle(CDM_Application)&   theApplication,
    const Handle(PCDM_ReaderFilter)& theFilter   = Handle(PCDM_ReaderFilter)(),
    const Message_ProgressRange&     theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(BinMDF_ADriverTable) AttributeDrivers(
    const Handle(Message_Messenger)& theMsgDriver);

  DEFINE_STANDARD_RTTIEXT(BinLDrivers_DocumentRetrievalDriver, PCDM_RetrievalDriver)

protected:
  //! Read the tree from the stream <theIS> to <theLabel>
  Standard_EXPORT virtual Standard_Integer ReadSubTree(
    Standard_IStream&                theIS,
    const TDF_Label&                 theData,
    const Handle(PCDM_ReaderFilter)& theFilter,
    const Standard_Boolean&          theQuickPart,
    const Standard_Boolean           theReadMissing,
    const Message_ProgressRange&     theRanges = Message_ProgressRange());

  //! define the procedure of reading a section to file.
  Standard_EXPORT virtual void ReadSection(BinLDrivers_DocumentSection& theSection,
                                           const Handle(CDM_Document)&  theDoc,
                                           Standard_IStream&            theIS);

  //! define the procedure of reading a shapes section to file.
  Standard_EXPORT virtual void ReadShapeSection(
    BinLDrivers_DocumentSection& theSection,
    Standard_IStream&            theIS,
    const Standard_Boolean       isMess   = Standard_False,
    const Message_ProgressRange& theRange = Message_ProgressRange());

  //! checks the shapes section can be correctly retrieved.
  Standard_EXPORT virtual void CheckShapeSection(const Storage_Position& thePos,
                                                 Standard_IStream&       theIS);

  //! clears the reading-cash data in drivers if any.
  Standard_EXPORT virtual void Clear();

  //! Check a file version(in which file was written) with a current version.
  //! Redefining this method is a chance for application to read files
  //! written by newer applications.
  //! The default implementation: if the version of the file is greater than the
  //! current or lesser than 2, then return false, else true
  Standard_EXPORT virtual Standard_Boolean CheckDocumentVersion(
    const Standard_Integer theFileVersion,
    const Standard_Integer theCurVersion);

  //! Return true if retrieved document allows to read parts quickly.
  static Standard_Boolean IsQuickPart(const Standard_Integer theFileVer);

  //! Enables reading in the quick part access mode.
  Standard_EXPORT virtual void EnableQuickPartReading(
    const Handle(Message_Messenger)& /*theMessageDriver*/,
    Standard_Boolean /*theValue*/)
  {
  }

  Handle(BinMDF_ADriverTable) myDrivers;
  BinObjMgt_RRelocationTable  myRelocTable;
  Handle(Message_Messenger)   myMsgDriver;

private:
  BinObjMgt_Persistent                myPAtt;
  TColStd_MapOfInteger                myMapUnsupported;
  BinLDrivers_VectorOfDocumentSection mySections;
  NCollection_Map<Standard_Integer>   myUnresolvedLinks;
};

#endif // _BinLDrivers_DocumentRetrievalDriver_HeaderFile
