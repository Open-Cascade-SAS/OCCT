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

#include <BinLDrivers.hxx>
#include <BinLDrivers_DocumentRetrievalDriver.hxx>
#include <BinLDrivers_DocumentSection.hxx>
#include <BinLDrivers_Marker.hxx>
#include <BinMDataStd.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_Application.hxx>
#include <FSD_BinaryFile.hxx>
#include <FSD_FileHeader.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_LinearVector.hxx>
#include <OSD_FileSystem.hxx>
#include <PCDM_ReadWriter.hxx>
#include <PCDM_ReaderFilter.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Type.hxx>
#include <Storage_HeaderData.hxx>
#include <Storage_Schema.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_FormatVersion.hxx>
#include <TDocStd_Owner.hxx>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

IMPLEMENT_STANDARD_RTTIEXT(BinLDrivers_DocumentRetrievalDriver, PCDM_RetrievalDriver)

#define SHAPESECTION_POS "SHAPE_SECTION_POS:"
#define ENDSECTION_POS ":"
#define SIZEOFSHAPELABEL 18

#define DATATYPE_MIGRATION

// #define DATATYPE_MIGRATION_DEB

namespace
{
void skipDirectData(Standard_IStream& theIS, BinObjMgt_Persistent& thePersistent)
{
  if (!thePersistent.IsDirect())
  {
    return;
  }

  uint64_t aStreamSize = 0;
  theIS.read(reinterpret_cast<char*>(&aStreamSize), sizeof(uint64_t));
#if DO_INVERSE
  aStreamSize = FSD_BinaryFile::InverseUint64(aStreamSize);
#endif
  constexpr uint64_t THE_SIZE_FIELD_SIZE = sizeof(uint64_t);
  if (!theIS || aStreamSize < THE_SIZE_FIELD_SIZE
      || aStreamSize - THE_SIZE_FIELD_SIZE
           > static_cast<uint64_t>(std::numeric_limits<std::streamoff>::max()))
  {
    theIS.setstate(std::ios_base::failbit);
    return;
  }
  const std::streamoff aPayloadSize =
    static_cast<std::streamoff>(aStreamSize - THE_SIZE_FIELD_SIZE);
  theIS.seekg(aPayloadSize, std::ios_base::cur);
}
} // namespace

//=================================================================================================

BinLDrivers_DocumentRetrievalDriver::BinLDrivers_DocumentRetrievalDriver()
{
  myReaderStatus = PCDM_RS_OK;
}

//=================================================================================================

void BinLDrivers_DocumentRetrievalDriver::Read(const TCollection_ExtendedString&     theFileName,
                                               const occ::handle<CDM_Document>&      theNewDocument,
                                               const occ::handle<CDM_Application>&   theApplication,
                                               const occ::handle<PCDM_ReaderFilter>& theFilter,
                                               const Message_ProgressRange&          theRange)
{
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream>      aFileStream =
    aFileSystem->OpenIStream(theFileName, std::ios::in | std::ios::binary);

  if (aFileStream.get() != nullptr && aFileStream->good())
  {
    occ::handle<Storage_Data>  dData;
    TCollection_ExtendedString aFormat = PCDM_ReadWriter::FileFormat(*aFileStream, dData);

    Read(*aFileStream, dData, theNewDocument, theApplication, theFilter, theRange);
    if (!theRange.More())
    {
      myReaderStatus = PCDM_RS_UserBreak;
      return;
    }
  }
  else
  {
    myReaderStatus = PCDM_RS_OpenError;
  }
}

#define MODIFICATION_COUNTER "MODIFICATION_COUNTER: "
#define REFERENCE_COUNTER "REFERENCE_COUNTER: "

#define START_TYPES "START_TYPES"
#define END_TYPES "END_TYPES"

//=================================================================================================

void BinLDrivers_DocumentRetrievalDriver::Read(Standard_IStream&                     theIStream,
                                               const occ::handle<Storage_Data>&      theStorageData,
                                               const occ::handle<CDM_Document>&      theDoc,
                                               const occ::handle<CDM_Application>&   theApplication,
                                               const occ::handle<PCDM_ReaderFilter>& theFilter,
                                               const Message_ProgressRange&          theRange)
{
  myReaderStatus = PCDM_RS_DriverFailure;
  myMsgDriver    = theApplication->MessageDriver();

  const TCollection_ExtendedString aMethStr("BinLDrivers_DocumentRetrievalDriver: ");

  occ::handle<TDocStd_Document> aDoc = occ::down_cast<TDocStd_Document>(theDoc);
  if (aDoc.IsNull())
  {
#ifdef OCCT_DEBUG
    myMsgDriver->Send(aMethStr + "error: null document", Message_Fail);
#endif
    myReaderStatus = PCDM_RS_NoDocument;
    return;
  }

  // 1. the information section
  occ::handle<Storage_HeaderData> aHeaderData;

  if (!theStorageData.IsNull())
  {
    aHeaderData = theStorageData->HeaderData();
  }

  if (!aHeaderData.IsNull())
  {
    for (const TCollection_AsciiString& aLine : aHeaderData->UserInfo())
    {
      if (aLine.Search(REFERENCE_COUNTER) != -1)
      {
        theDoc->SetReferenceCounter(aLine.Token(" ", 2).IntegerValue());
      }
      else if (aLine.Search(MODIFICATION_COUNTER) != -1)
      {
        theDoc->SetModifications(aLine.Token(" ", 2).IntegerValue());
      }
    }
  }

  // 1.a Version of writer
  if (!aHeaderData->StorageVersion().IsIntegerValue())
  {
    // file has no format version
    myMsgDriver->Send(aMethStr + "error: file has no format version", Message_Fail);
    myReaderStatus = PCDM_RS_FormatFailure;
    return;
  }
  TDocStd_FormatVersion aFileVer =
    static_cast<TDocStd_FormatVersion>(aHeaderData->StorageVersion().IntegerValue());
  TDocStd_FormatVersion aCurrVer = TDocStd_Document::CurrentStorageFormatVersion();
  // maintain one-way compatibility starting from version 2+
  if (!CheckDocumentVersion(aFileVer, aCurrVer))
  {
    myReaderStatus = PCDM_RS_NoVersion;
    // file was written with another version
    myMsgDriver->Send(aMethStr + "error: wrong file version: " + aHeaderData->StorageVersion()
                        + " while current is " + TDocStd_Document::CurrentStorageFormatVersion(),
                      Message_Fail);
    return;
  }

  // 1.b Retrieve the Types table
  NCollection_Sequence<TCollection_AsciiString>        aTypeNames; // Sequence of types in file
  const NCollection_Sequence<TCollection_AsciiString>& aUserInfo = aHeaderData->UserInfo();
  bool                                                 begin     = false;
  for (const TCollection_AsciiString& theUserInfo : aUserInfo)
  {
    TCollection_AsciiString aStr = theUserInfo;
    if (aStr == START_TYPES)
    {
      begin = true;
    }
    else if (aStr == END_TYPES)
    {
      break;
    }
    else if (begin)
    {
      if (aFileVer < TDocStd_FormatVersion_VERSION_8)
      {
#ifdef DATATYPE_MIGRATION
        TCollection_AsciiString newName;
        if (Storage_Schema::CheckTypeMigration(aStr, newName))
        {
  #ifdef OCCT_DEBUG
          std::cout << "CheckTypeMigration:OldType = " << aStr << " Len = " << aStr.Length()
                    << std::endl;
          std::cout << "CheckTypeMigration:NewType = " << newName << " Len = " << newName.Length()
                    << std::endl;
  #endif
          aStr = newName;
        }
#endif
      }
      aTypeNames.Append(aStr);
    }
  }
  if (myDrivers.IsNull())
  {
    myDrivers = AttributeDrivers(myMsgDriver);
  }
  myDrivers->AssignIds(aTypeNames);

  myTreeNodeTypeId = 0;
  if (!theFilter.IsNull())
  {
    occ::handle<BinMDF_ADriver> aTreeNodeDriver;
    myTreeNodeTypeId = myDrivers->GetDriver(STANDARD_TYPE(TDataStd_TreeNode), aTreeNodeDriver);
  }

  // recognize types not supported by drivers
  myMapUnsupported.Clear();
  for (int aTypeId = 1; aTypeId <= aTypeNames.Length(); ++aTypeId)
  {
    if (myDrivers->GetDriver(aTypeId).IsNull())
    {
      myMapUnsupported.Add(aTypeId);
    }
  }
  if (!myMapUnsupported.IsEmpty())
  {
    myMsgDriver->Send(aMethStr
                        + "warning: "
                          "the following attributes have no driver:",
                      Message_Warning);
    for (int aTypeId = 1; aTypeId <= aTypeNames.Length(); ++aTypeId)
    {
      if (myMapUnsupported.Contains(aTypeId))
      {
        myMsgDriver->Send(aTypeNames.Value(aTypeId), Message_Warning);
      }
    }
  }

  // 2. Read document contents
  // 2a. Retrieve data from the stream:
  myRelocTable.Clear();
  myRelocTable.SetHeaderData(aHeaderData);
  mySections.Clear();
  myPAtt.Init();
  occ::handle<TDF_Data> aData =
    (!theFilter.IsNull() && theFilter->IsAppendMode()) ? aDoc->GetData() : new TDF_Data();

  Message_ProgressScope aPS(theRange, "Reading data", 4);
  bool                  aQuickPart = IsQuickPart(aFileVer);

  // 2b. Read the TOC of Sections
  if (aFileVer >= TDocStd_FormatVersion_VERSION_3)
  {
    BinLDrivers_DocumentSection aSection;
    do
    {
      if (!BinLDrivers_DocumentSection::ReadTOC(aSection, theIStream, aFileVer))
      {
        break;
      }
      mySections.Append(aSection);
    } while (!aSection.Name().IsEqual(aQuickPart ? ENDSECTION_POS : SHAPESECTION_POS)
             && !theIStream.eof());

    if (mySections.IsEmpty() || theIStream.eof())
    {
      // There is no shape section in the file.
      myMsgDriver->Send(aMethStr + "error: shape section is not found", Message_Fail);
      myReaderStatus = PCDM_RS_ReaderException;
      return;
    }

    NCollection_DynamicArray<BinLDrivers_DocumentSection>::Iterator anIterS(mySections);
    // if there is only empty section, do not call tellg and seekg
    if (!mySections.IsEmpty()
        && (mySections.Size() > 1 || !anIterS.Value().Name().IsEqual(ENDSECTION_POS)))
    {
      std::streampos aDocumentPos = theIStream.tellg(); // position of root label
      for (; anIterS.More(); anIterS.Next())
      {
        BinLDrivers_DocumentSection& aCurSection = anIterS.ChangeValue();
        if (!aCurSection.IsPostRead())
        {
          theIStream.seekg((std::streampos)aCurSection.Offset());
          if (aCurSection.Name().IsEqual(SHAPESECTION_POS))
          {
            ReadShapeSection(aCurSection, theIStream, false, aPS.Next());
            if (!aPS.More())
            {
              myReaderStatus = PCDM_RS_UserBreak;
              return;
            }
          }
          else if (!aCurSection.Name().IsEqual(ENDSECTION_POS))
          {
            ReadSection(aCurSection, theDoc, theIStream);
          }
        }
      }
      theIStream.seekg(aDocumentPos);
    }
  }
  else
  {                                                   // aFileVer < 3
    std::streampos aDocumentPos = theIStream.tellg(); // position of root label
    // retrieve SHAPESECTION_POS string
    char aShapeSecLabel[SIZEOFSHAPELABEL + 1];
    aShapeSecLabel[SIZEOFSHAPELABEL] = 0x00;
    theIStream.read(reinterpret_cast<char*>(&aShapeSecLabel), SIZEOFSHAPELABEL); // SHAPESECTION_POS
    TCollection_AsciiString aShapeLabel(aShapeSecLabel);
    // detect if a file was written in old fashion (version 2 without shapes)
    // and if so then skip reading ShapeSection
    if (aShapeLabel.Length() > 0)
    {
      // version 2+(with shapes) and higher goes here
      if (aShapeLabel.Length() <= 0 || aShapeLabel != SHAPESECTION_POS)
      {
        myMsgDriver->Send(aMethStr + "error: Format failure", Message_Fail);
        myReaderStatus = PCDM_RS_FormatFailure;
        return;
      }

      // retrieve ShapeSection Position
      int aShapeSectionPos; // go to ShapeSection
      theIStream.read(reinterpret_cast<char*>(&aShapeSectionPos), sizeof(int));

#ifdef DO_INVERSE
      aShapeSectionPos = InverseInt(aShapeSectionPos);
#endif
#ifdef OCCT_DEBUG
      std::cout << "aShapeSectionPos = " << aShapeSectionPos << std::endl;
#endif
      if (aShapeSectionPos)
      {
        aDocumentPos = theIStream.tellg();
        theIStream.seekg((std::streampos)aShapeSectionPos);

        CheckShapeSection(aShapeSectionPos, theIStream);
        // Read Shapes
        BinLDrivers_DocumentSection aCurSection;
        ReadShapeSection(aCurSection, theIStream, false, aPS.Next());
        if (!aPS.More())
        {
          myReaderStatus = PCDM_RS_UserBreak;
          return;
        }
      }
    }
    theIStream.seekg(aDocumentPos);
  } // end of reading Sections or shape section

  // Return to read of the Document structure

  // read the header (tag) of the root label
  int aTag;
  theIStream.read(reinterpret_cast<char*>(&aTag), sizeof(int));

  if (aQuickPart)
  {
    myPAtt.SetIStream(theIStream); // for reading shapes data from the stream directly
  }
  EnableQuickPartReading(myMsgDriver, aQuickPart);

  myUnresolvedLinks.Clear();
  myTreeNodeLocations.Clear();

  // read sub-tree of the root label
  if (!theFilter.IsNull())
  {
    theFilter->StartIteration();
  }
  const auto aStreamStartPosition = theIStream.tellg();
  int nbRead = ReadSubTree(theIStream, aData->Root(), theFilter, aQuickPart, false, aPS.Next());
  if (nbRead < 0)
  {
    Clear();
    return;
  }
  if (!myUnresolvedLinks.IsEmpty())
  {
    // Scan skipped labels once and remember TreeNode record positions. References to records
    // preceding their dependants are resolved by direct seeks after the scan.
    theFilter->StartIteration();
    theIStream.seekg(aStreamStartPosition, std::ios_base::beg);
    const int aNbMissing =
      ReadSubTree(theIStream, aData->Root(), theFilter, aQuickPart, true, aPS.Next());
    if (aNbMissing < 0)
    {
      Clear();
      return;
    }
    nbRead += aNbMissing;

    if (!myUnresolvedLinks.IsEmpty())
    {
      const int aNbResolved = resolveUnresolvedLinks(theIStream, theFilter, aPS.Next());
      if (aNbResolved < 0)
      {
        Clear();
        return;
      }
      nbRead += aNbResolved;
    }
  }
  if (!aPS.More())
  {
    myReaderStatus = PCDM_RS_UserBreak;
    return;
  }

  Clear();
  if (!aPS.More())
  {
    myReaderStatus = PCDM_RS_UserBreak;
    return;
  }
  if (nbRead > 0)
  {
    // attach data to the document
    if (theFilter.IsNull() || !theFilter->IsAppendMode())
    {
      aDoc->SetData(aData);
      TDocStd_Owner::SetDocument(aData, aDoc);
      aDoc->SetComments(aHeaderData->Comments());
    }
    myReaderStatus = PCDM_RS_OK;
  }

  // Read Sections (post-reading type)
  if (aFileVer >= TDocStd_FormatVersion_VERSION_3)
  {
    NCollection_DynamicArray<BinLDrivers_DocumentSection>::Iterator aSectIter(mySections);
    for (; aSectIter.More(); aSectIter.Next())
    {
      BinLDrivers_DocumentSection& aCurSection = aSectIter.ChangeValue();
      if (aCurSection.IsPostRead())
      {
        theIStream.seekg((std::streampos)aCurSection.Offset());
        ReadSection(aCurSection, theDoc, theIStream);
      }
    }
  }
}

//=================================================================================================

int BinLDrivers_DocumentRetrievalDriver::readAttribute(
  Standard_IStream&                     theIS,
  const TDF_Label&                      theLabel,
  const occ::handle<PCDM_ReaderFilter>& theFilter)
{
  const occ::handle<BinMDF_ADriver> aDriver = myDrivers->GetDriver(myPAtt.TypeId());
  if (aDriver.IsNull())
  {
    if (!myMapUnsupported.Contains(myPAtt.TypeId()))
    {
      myMsgDriver->Send(TCollection_ExtendedString(
                          "BinLDrivers_DocumentRetrievalDriver: warning: type ID not registered in "
                          "header: ")
                          + myPAtt.TypeId(),
                        Message_Warning);
    }
    skipDirectData(theIS, myPAtt);
    return 0;
  }

  const int                              anId            = myPAtt.Id();
  const occ::handle<Standard_Transient>* aBoundAttribute = myRelocTable.Seek(anId);
  const bool                             isBound         = aBoundAttribute != nullptr;
  occ::handle<TDF_Attribute>             anAttrib =
    isBound ? occ::down_cast<TDF_Attribute>(*aBoundAttribute) : aDriver->NewEmpty();

  if (!theFilter.IsNull() && !theFilter->IsPassed(anAttrib->DynamicType()))
  {
    skipDirectData(theIS, myPAtt);
    return 0;
  }

  if (anAttrib->Label().IsNull())
  {
    if (!theFilter.IsNull() && theFilter->Mode() != PCDM_ReaderFilter::AppendMode_Forbid
        && theLabel.IsAttribute(anAttrib->ID()))
    {
      if (theFilter->Mode() == PCDM_ReaderFilter::AppendMode_Protect)
      {
        skipDirectData(theIS, myPAtt);
        return 1;
      }
      if (theFilter->Mode() == PCDM_ReaderFilter::AppendMode_Overwrite)
      {
        theLabel.ForgetAttribute(anAttrib->ID());
      }
    }
    try
    {
      theLabel.AddAttribute(anAttrib);
    }
    catch (const Standard_DomainError&)
    {
      // The persistent GUID of attributes with user-defined IDs is read by the driver.
      static const Standard_GUID aFallbackId;
      anAttrib->SetID(aFallbackId);
      theLabel.AddAttribute(anAttrib);
    }
  }
  else
  {
    myMsgDriver->Send(
      TCollection_ExtendedString(
        "BinLDrivers_DocumentRetrievalDriver: warning: attempt to attach attribute ")
        + aDriver->TypeName() + " to a second label",
      Message_Warning);
  }

  if (!aDriver->Paste(myPAtt, anAttrib, myRelocTable))
  {
    myMsgDriver->Send(TCollection_ExtendedString(
                        "BinLDrivers_DocumentRetrievalDriver: warning: failure reading attribute ")
                        + aDriver->TypeName(),
                      Message_Warning);
    return 1;
  }

  if (!isBound)
  {
    myRelocTable.Bind(anId, anAttrib);
  }

  if (!theFilter.IsNull() && myPAtt.TypeId() == myTreeNodeTypeId)
  {
    myUnresolvedLinks.Remove(anId);
    int aFatherId = -1;
    if (myPAtt.SetPosition(BP_HEADSIZE) && (myPAtt >> aFatherId) && aFatherId > 0
        && !isAttributeLoaded(aFatherId))
    {
      myUnresolvedLinks.Add(aFatherId);
    }
  }
  return 1;
}

//=================================================================================================

int BinLDrivers_DocumentRetrievalDriver::resolveUnresolvedLinks(
  Standard_IStream&                     theIS,
  const occ::handle<PCDM_ReaderFilter>& theFilter,
  const Message_ProgressRange&          theRange)
{
  Message_ProgressScope         aPS(theRange, "Resolving attribute links", 1);
  NCollection_LinearVector<int> aPendingIds;
  int                           aNbRead = 0;
  while (!myUnresolvedLinks.IsEmpty())
  {
    aPendingIds.Clear();
    aPendingIds.Reserve(myUnresolvedLinks.Size());
    for (const int anId : myUnresolvedLinks)
    {
      aPendingIds.Append(anId);
    }
    myUnresolvedLinks.Clear();

    for (const int anId : aPendingIds)
    {
      if (!aPS.More())
      {
        myReaderStatus = PCDM_RS_UserBreak;
        return -1;
      }
      myUnresolvedLinks.Remove(anId);
      if (isAttributeLoaded(anId))
      {
        continue;
      }
      const LabelLocation* aLocation = myTreeNodeLocations.Seek(anId);
      if (aLocation == nullptr)
      {
        myMsgDriver->Send(TCollection_ExtendedString(
                            "BinLDrivers_DocumentRetrievalDriver: error: unresolved TreeNode ")
                            + anId,
                          Message_Fail);
        myReaderStatus = PCDM_RS_UnrecognizedFileFormat;
        return -1;
      }

      const auto& [aPosition, aLabel] = *aLocation;
      theIS.clear();
      theIS.seekg(aPosition, std::ios_base::beg);
      for (theIS >> myPAtt; theIS && myPAtt.TypeId() > 0 && myPAtt.Id() > 0 && !theIS.eof();
           theIS >> myPAtt)
      {
        if (isAttributeLoaded(myPAtt.Id()))
        {
          skipDirectData(theIS, myPAtt);
          continue;
        }
        aNbRead += readAttribute(theIS, aLabel, theFilter);
      }
      if (!theIS || myPAtt.TypeId() != BinLDrivers_ENDATTRLIST)
      {
        myMsgDriver->Send("BinLDrivers_DocumentRetrievalDriver: error: invalid attribute list",
                          Message_Fail);
        myReaderStatus = PCDM_RS_UnrecognizedFileFormat;
        return -1;
      }
      if (!isAttributeLoaded(anId))
      {
        myMsgDriver->Send(
          TCollection_ExtendedString(
            "BinLDrivers_DocumentRetrievalDriver: error: failed to resolve TreeNode ")
            + anId,
          Message_Fail);
        myReaderStatus = PCDM_RS_UnrecognizedFileFormat;
        return -1;
      }
    }
  }
  return aNbRead;
}

//=================================================================================================

bool BinLDrivers_DocumentRetrievalDriver::isAttributeLoaded(const int theId) const
{
  const occ::handle<Standard_Transient>* anObject = myRelocTable.Seek(theId);
  if (anObject == nullptr)
  {
    return false;
  }
  const occ::handle<TDF_Attribute> anAttribute = occ::down_cast<TDF_Attribute>(*anObject);
  return !anAttribute.IsNull() && !anAttribute->Label().IsNull();
}

//=================================================================================================

int BinLDrivers_DocumentRetrievalDriver::ReadSubTree(
  Standard_IStream&                     theIS,
  const TDF_Label&                      theLabel,
  const occ::handle<PCDM_ReaderFilter>& theFilter,
  const bool&                           theQuickPart,
  const bool                            theReadMissing,
  const Message_ProgressRange&          theRange)
{
  int                        nbRead = 0;
  TCollection_ExtendedString aMethStr("BinLDrivers_DocumentRetrievalDriver: ");

  Message_ProgressScope aPS(theRange, "Reading sub tree", 2, true);

  bool aSkipAttrs = false;
  if (!theFilter.IsNull() && theFilter->IsPartTree())
  {
    aSkipAttrs = !theFilter->IsPassed();
  }

  if (theQuickPart)
  {
    uint64_t aLabelSize = 0;
    theIS.read(reinterpret_cast<char*>(&aLabelSize), sizeof(uint64_t));
#if DO_INVERSE
    aLabelSize = InverseUint64(aLabelSize);
#endif
    // no one sub-label is needed, so, skip everything
    if (aSkipAttrs && !theFilter->IsSubPassed() && myUnresolvedLinks.IsEmpty())
    {
      aLabelSize -= sizeof(uint64_t);
      theIS.seekg(aLabelSize, std::ios_base::cur);
      if (!theFilter.IsNull())
      {
        theFilter->Up();
      }
      return 0;
    }
  }

  if (theReadMissing)
  {
    aSkipAttrs = true;
  }
  const auto anAttStartPosition = theIS.tellg();
  // Read attributes:
  for (theIS >> myPAtt; theIS && myPAtt.TypeId() > 0 && // not an end marker ?
                        myPAtt.Id() > 0 &&              // not a garbage ?
                        !theIS.eof();
       theIS >> myPAtt)
  {
    if (!aPS.More())
    {
      myReaderStatus = PCDM_RS_UserBreak;
      return -1;
    }
    if (theReadMissing && myPAtt.TypeId() == myTreeNodeTypeId)
    {
      myTreeNodeLocations.TryEmplace(myPAtt.Id(), anAttStartPosition, theLabel);
    }
    if (myUnresolvedLinks.Remove(myPAtt.Id()) && aSkipAttrs)
    {
      aSkipAttrs = false;
      theIS.seekg(anAttStartPosition, std::ios_base::beg);
      continue;
    }
    if (aSkipAttrs)
    {
      skipDirectData(theIS, myPAtt);
      continue;
    }
    if (theReadMissing && isAttributeLoaded(myPAtt.Id()))
    {
      skipDirectData(theIS, myPAtt);
      continue;
    }
    nbRead += readAttribute(theIS, theLabel, theFilter);
  }
  if (!theIS || myPAtt.TypeId() != BinLDrivers_ENDATTRLIST)
  {
    // unexpected EOF or garbage data
    myMsgDriver->Send(aMethStr + "error: unexpected EOF or garbage data", Message_Fail);
    myReaderStatus = PCDM_RS_UnrecognizedFileFormat;
    return -1;
  }

  // Read children:
  // read the tag of a child label
  int aTag = BinLDrivers_ENDLABEL;
  theIS.read(reinterpret_cast<char*>(&aTag), sizeof(int));
#ifdef DO_INVERSE
  aTag = InverseInt(aTag);
#endif

  while (theIS && aTag >= 0 && !theIS.eof())
  { // not an end marker ?
    // create sub-label
    TDF_Label aLab = theLabel.FindChild(aTag, true);
    if (!aPS.More())
    {
      myReaderStatus = PCDM_RS_UserBreak;
      return -1;
    }

    // read sub-tree
    if (!theFilter.IsNull())
    {
      theFilter->Down(aTag);
    }
    int nbSubRead = ReadSubTree(theIS, aLab, theFilter, theQuickPart, theReadMissing, aPS.Next());
    // check for error
    if (nbSubRead == -1)
    {
      return -1;
    }
    nbRead += nbSubRead;

    // read the tag of the next child
    theIS.read(reinterpret_cast<char*>(&aTag), sizeof(int));
#ifdef DO_INVERSE
    aTag = InverseInt(aTag);
#endif
  }

  if (aTag != BinLDrivers_ENDLABEL)
  {
    // invalid end label marker
    myMsgDriver->Send(aMethStr + "error: invalid end label marker", Message_Fail);
    myReaderStatus = PCDM_RS_UnrecognizedFileFormat;
    return -1;
  }
  if (!theFilter.IsNull())
  {
    theFilter->Up();
  }

  return nbRead;
}

//=================================================================================================

occ::handle<BinMDF_ADriverTable> BinLDrivers_DocumentRetrievalDriver::AttributeDrivers(
  const occ::handle<Message_Messenger>& theMessageDriver)
{
  return BinLDrivers::AttributeDrivers(theMessageDriver);
}

//=================================================================================================

void BinLDrivers_DocumentRetrievalDriver::ReadSection(
  BinLDrivers_DocumentSection& /*theSection*/,
  const occ::handle<CDM_Document>& /*theDocument*/,
  Standard_IStream& /*theIS*/)
{
  // empty; should be redefined in subclasses
}

//=================================================================================================

void BinLDrivers_DocumentRetrievalDriver::ReadShapeSection(
  BinLDrivers_DocumentSection& theSection,
  Standard_IStream& /*theIS*/,
  const bool isMess,
  const Message_ProgressRange& /*theRange*/)

{
  if (isMess && theSection.Length())
  {
    const TCollection_ExtendedString aMethStr("BinLDrivers_DocumentRetrievalDriver: ");
    myMsgDriver->Send(aMethStr + "warning: Geometry is not supported by Lite schema. ",
                      Message_Warning);
  }
}

//=================================================================================================

void BinLDrivers_DocumentRetrievalDriver::CheckShapeSection(const Storage_Position& ShapeSectionPos,
                                                            Standard_IStream&       IS)
{
  if (!IS.eof())
  {
    const std::streamoff endPos = IS.rdbuf()->pubseekoff(0L, std::ios_base::end, std::ios_base::in);
#ifdef OCCT_DEBUG
    std::cout << "endPos = " << endPos << std::endl;
#endif
    if (ShapeSectionPos != endPos)
    {
      const TCollection_ExtendedString aMethStr("BinLDrivers_DocumentRetrievalDriver: ");
      myMsgDriver->Send(aMethStr + "warning: Geometry is not supported by Lite schema. ",
                        Message_Warning);
    }
  }
}

//=================================================================================================

void BinLDrivers_DocumentRetrievalDriver::Clear()
{
  myPAtt.Destroy(); // free buffer
  myRelocTable.Clear();
  myMapUnsupported.Clear();
  myUnresolvedLinks.Clear();
  myTreeNodeLocations.Clear();
  myTreeNodeTypeId = 0;
}

//=================================================================================================

bool BinLDrivers_DocumentRetrievalDriver::CheckDocumentVersion(const int theFileVersion,
                                                               const int theCurVersion)
{
  // file was written with another version
  return theFileVersion >= TDocStd_FormatVersion_LOWER && theFileVersion <= theCurVersion;
}

//=================================================================================================

bool BinLDrivers_DocumentRetrievalDriver::IsQuickPart(const int theFileVer)
{
  return theFileVer >= TDocStd_FormatVersion_VERSION_12;
}
