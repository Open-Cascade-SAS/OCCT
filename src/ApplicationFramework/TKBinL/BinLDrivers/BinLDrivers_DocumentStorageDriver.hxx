// Created on: 2002-10-29
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

#ifndef _BinLDrivers_DocumentStorageDriver_HeaderFile
#define _BinLDrivers_DocumentStorageDriver_HeaderFile

#include <Standard.hxx>

#include <BinObjMgt_Persistent.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TDF_Label.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <BinLDrivers_DocumentSection.hxx>
#include <PCDM_StorageDriver.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Type.hxx>
#include <TDocStd_FormatVersion.hxx>
class BinMDF_ADriverTable;
class Message_Messenger;
class CDM_Document;
class TDF_Label;
class TCollection_AsciiString;
class BinLDrivers_DocumentSection;
class BinObjMgt_Position;

//! persistent implementation of storage a document in a binary file
class BinLDrivers_DocumentStorageDriver : public PCDM_StorageDriver
{

public:
  //! Constructor
  Standard_EXPORT BinLDrivers_DocumentStorageDriver();

  //! Write <theDocument> to the binary file <theFileName>
  Standard_EXPORT virtual void Write(
    const occ::handle<CDM_Document>&  theDocument,
    const TCollection_ExtendedString& theFileName,
    const Message_ProgressRange&      theRange = Message_ProgressRange()) override;

  //! Write <theDocument> to theOStream
  Standard_EXPORT virtual void Write(
    const occ::handle<CDM_Document>& theDocument,
    Standard_OStream&                theOStream,
    const Message_ProgressRange&     theRange = Message_ProgressRange()) override;

  Standard_EXPORT virtual occ::handle<BinMDF_ADriverTable> AttributeDrivers(
    const occ::handle<Message_Messenger>& theMsgDriver);

  //! Create a section that should be written after the OCAF data
  Standard_EXPORT void AddSection(const TCollection_AsciiString& theName,
                                  const bool                     isPostRead = true);

  //! Return true if document should be stored in quick mode for partial reading
  Standard_EXPORT bool IsQuickPart(const int theVersion) const;

  DEFINE_STANDARD_RTTIEXT(BinLDrivers_DocumentStorageDriver, PCDM_StorageDriver)

protected:
  //! Write the tree under <theLabel> to the stream <theOS>
  Standard_EXPORT void WriteSubTree(
    const TDF_Label&             theData,
    Standard_OStream&            theOS,
    const bool&                  theQuickPart,
    const Message_ProgressRange& theRange = Message_ProgressRange());

  //! define the procedure of writing a section to file.
  Standard_EXPORT virtual void WriteSection(const TCollection_AsciiString& /*theName*/,
                                            const occ::handle<CDM_Document>& /*theDoc*/,
                                            Standard_OStream& /*theOS*/);

  //! defines the procedure of writing a shape section to file
  Standard_EXPORT virtual void WriteShapeSection(
    BinLDrivers_DocumentSection& theDocSection,
    Standard_OStream&            theOS,
    const TDocStd_FormatVersion  theDocVer,
    const Message_ProgressRange& theRange = Message_ProgressRange());

  //! Enables writing in the quick part access mode.
  Standard_EXPORT virtual void EnableQuickPartWriting(
    const occ::handle<Message_Messenger>& /*theMessageDriver*/,
    const bool /*theValue*/)
  {
  }

  //! clears the writing-cash data in drivers if any.
  Standard_EXPORT virtual void Clear();

  occ::handle<BinMDF_ADriverTable>                        myDrivers;
  NCollection_IndexedMap<occ::handle<Standard_Transient>> myRelocTable;
  occ::handle<Message_Messenger>                          myMsgDriver;

private:
  Standard_EXPORT void FirstPass(const TDF_Label& theRoot);

  //! Returns true if <L> and its sub-labels do not contain
  //! attributes to store
  Standard_EXPORT bool FirstPassSubTree(const TDF_Label&             L,
                                        NCollection_List<TDF_Label>& ListOfEmptyL);

  //! Write info section using FSD_BinaryFile driver
  Standard_EXPORT void WriteInfoSection(const occ::handle<CDM_Document>& theDocument,
                                        Standard_OStream&                theOStream);

  Standard_EXPORT void UnsupportedAttrMsg(const occ::handle<Standard_Type>& theType);

  //! Writes sizes along the file where it is needed for quick part mode
  Standard_EXPORT void WriteSizes(Standard_OStream& theOS);

  BinObjMgt_Persistent                                    myPAtt;
  NCollection_List<TDF_Label>                             myEmptyLabels;
  NCollection_Map<occ::handle<Standard_Transient>>        myMapUnsupported;
  NCollection_IndexedMap<occ::handle<Standard_Transient>> myTypesMap;
  NCollection_Vector<BinLDrivers_DocumentSection>         mySections;
  TCollection_ExtendedString                              myFileName;
  //! Sizes of labels and some attributes that will be stored in the second pass
  NCollection_List<occ::handle<BinObjMgt_Position>> mySizesToWrite;
};

#endif // _BinLDrivers_DocumentStorageDriver_HeaderFile
