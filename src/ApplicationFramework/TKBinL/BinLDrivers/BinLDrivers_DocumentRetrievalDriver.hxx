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
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <BinLDrivers_DocumentSection.hxx>
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

class BinLDrivers_DocumentRetrievalDriver : public PCDM_RetrievalDriver
{

public:
  //! Constructor
  Standard_EXPORT BinLDrivers_DocumentRetrievalDriver();

  //! retrieves the content of the file into a new Document.
  Standard_EXPORT virtual void Read(
    const TCollection_ExtendedString& theFileName,
    const occ::handle<CDM_Document>&       theNewDocument,
    const occ::handle<CDM_Application>&    theApplication,
    const occ::handle<PCDM_ReaderFilter>&  theFilter   = occ::handle<PCDM_ReaderFilter>(),
    const Message_ProgressRange&      theProgress = Message_ProgressRange()) override;

  Standard_EXPORT virtual void Read(
    Standard_IStream&                theIStream,
    const occ::handle<Storage_Data>&      theStorageData,
    const occ::handle<CDM_Document>&      theDoc,
    const occ::handle<CDM_Application>&   theApplication,
    const occ::handle<PCDM_ReaderFilter>& theFilter   = occ::handle<PCDM_ReaderFilter>(),
    const Message_ProgressRange&     theProgress = Message_ProgressRange()) override;

  Standard_EXPORT virtual occ::handle<BinMDF_ADriverTable> AttributeDrivers(
    const occ::handle<Message_Messenger>& theMsgDriver);

  DEFINE_STANDARD_RTTIEXT(BinLDrivers_DocumentRetrievalDriver, PCDM_RetrievalDriver)

protected:
  //! Read the tree from the stream <theIS> to <theLabel>
  Standard_EXPORT virtual int ReadSubTree(
    Standard_IStream&                theIS,
    const TDF_Label&                 theData,
    const occ::handle<PCDM_ReaderFilter>& theFilter,
    const bool&          theQuickPart,
    const bool           theReadMissing,
    const Message_ProgressRange&     theRanges = Message_ProgressRange());

  //! define the procedure of reading a section to file.
  Standard_EXPORT virtual void ReadSection(BinLDrivers_DocumentSection& theSection,
                                           const occ::handle<CDM_Document>&  theDoc,
                                           Standard_IStream&            theIS);

  //! define the procedure of reading a shapes section to file.
  Standard_EXPORT virtual void ReadShapeSection(
    BinLDrivers_DocumentSection& theSection,
    Standard_IStream&            theIS,
    const bool       isMess   = false,
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
  Standard_EXPORT virtual bool CheckDocumentVersion(
    const int theFileVersion,
    const int theCurVersion);

  //! Return true if retrieved document allows to read parts quickly.
  static bool IsQuickPart(const int theFileVer);

  //! Enables reading in the quick part access mode.
  Standard_EXPORT virtual void EnableQuickPartReading(
    const occ::handle<Message_Messenger>& /*theMessageDriver*/,
    bool /*theValue*/)
  {
  }

  occ::handle<BinMDF_ADriverTable> myDrivers;
  BinObjMgt_RRelocationTable  myRelocTable;
  occ::handle<Message_Messenger>   myMsgDriver;

private:
  BinObjMgt_Persistent                myPAtt;
  NCollection_Map<int>                myMapUnsupported;
  NCollection_Vector<BinLDrivers_DocumentSection> mySections;
  NCollection_Map<int>   myUnresolvedLinks;
};

#endif // _BinLDrivers_DocumentRetrievalDriver_HeaderFile
