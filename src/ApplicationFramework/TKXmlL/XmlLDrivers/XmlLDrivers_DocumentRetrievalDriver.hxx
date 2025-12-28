// Created on: 2001-07-25
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

#ifndef _XmlLDrivers_DocumentRetrievalDriver_HeaderFile
#define _XmlLDrivers_DocumentRetrievalDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XmlObjMgt_RRelocationTable.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCDM_RetrievalDriver.hxx>
#include <XmlObjMgt_Element.hxx>
#include <Standard_Integer.hxx>
#include <Storage_Data.hxx>
class XmlMDF_ADriverTable;
class CDM_Document;
class CDM_Application;
class Message_Messenger;
class XmlMDF_ADriver;

class XmlLDrivers_DocumentRetrievalDriver : public PCDM_RetrievalDriver
{

public:
  Standard_EXPORT XmlLDrivers_DocumentRetrievalDriver();

  Standard_EXPORT virtual void Read(
    const TCollection_ExtendedString& theFileName,
    const occ::handle<CDM_Document>&       theNewDocument,
    const occ::handle<CDM_Application>&    theApplication,
    const occ::handle<PCDM_ReaderFilter>&  theFilter = occ::handle<PCDM_ReaderFilter>(),
    const Message_ProgressRange&      theRange  = Message_ProgressRange()) override;

  Standard_EXPORT virtual void Read(
    Standard_IStream&                theIStream,
    const occ::handle<Storage_Data>&      theStorageData,
    const occ::handle<CDM_Document>&      theDoc,
    const occ::handle<CDM_Application>&   theApplication,
    const occ::handle<PCDM_ReaderFilter>& theFilter = occ::handle<PCDM_ReaderFilter>(),
    const Message_ProgressRange&     theRange  = Message_ProgressRange()) override;

  Standard_EXPORT virtual occ::handle<XmlMDF_ADriverTable> AttributeDrivers(
    const occ::handle<Message_Messenger>& theMsgDriver);

  DEFINE_STANDARD_RTTIEXT(XmlLDrivers_DocumentRetrievalDriver, PCDM_RetrievalDriver)

protected:
  Standard_EXPORT virtual void ReadFromDomDocument(
    const XmlObjMgt_Element&       theDomElement,
    const occ::handle<CDM_Document>&    theNewDocument,
    const occ::handle<CDM_Application>& theApplication,
    const Message_ProgressRange&   theRange = Message_ProgressRange());

  Standard_EXPORT virtual bool MakeDocument(
    const XmlObjMgt_Element&     thePDoc,
    const occ::handle<CDM_Document>&  theTDoc,
    const Message_ProgressRange& theRange = Message_ProgressRange());

  Standard_EXPORT virtual occ::handle<XmlMDF_ADriver> ReadShapeSection(
    const XmlObjMgt_Element&         thePDoc,
    const occ::handle<Message_Messenger>& theMsgDriver,
    const Message_ProgressRange&     theRange = Message_ProgressRange());

  Standard_EXPORT virtual void ShapeSetCleaning(const occ::handle<XmlMDF_ADriver>& theDriver);

  occ::handle<XmlMDF_ADriverTable> myDrivers;
  XmlObjMgt_RRelocationTable  myRelocTable;
  TCollection_ExtendedString  myFileName;

};

#endif // _XmlLDrivers_DocumentRetrievalDriver_HeaderFile
