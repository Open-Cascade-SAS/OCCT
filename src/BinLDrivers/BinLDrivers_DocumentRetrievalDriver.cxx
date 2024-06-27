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
#include <Message_Messenger.hxx>
#include <FSD_BinaryFile.hxx>
#include <FSD_FileHeader.hxx>
#include <OSD_FileSystem.hxx>
#include <PCDM_ReadWriter.hxx>
#include <Standard_Stream.hxx>
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
#include <Message_ProgressScope.hxx>
#include <PCDM_ReaderFilter.hxx>


IMPLEMENT_STANDARD_RTTIEXT(BinLDrivers_DocumentRetrievalDriver,PCDM_RetrievalDriver)

#define SHAPESECTION_POS "SHAPE_SECTION_POS:"
#define ENDSECTION_POS ":"
#define SIZEOFSHAPELABEL  18

#define DATATYPE_MIGRATION
//#define DATATYPE_MIGRATION_DEB
//=======================================================================
//function : BinLDrivers_DocumentRetrievalDriver
//purpose  : Constructor
//=======================================================================

BinLDrivers_DocumentRetrievalDriver::BinLDrivers_DocumentRetrievalDriver ()
{
  myReaderStatus = PCDM_RS_OK;
}

//=======================================================================
//function : Read
//purpose  :
//=======================================================================
void BinLDrivers_DocumentRetrievalDriver::Read
                         (const TCollection_ExtendedString& theFileName,
                          const Handle(CDM_Document)&       theNewDocument,
                          const Handle(CDM_Application)&    theApplication,
                          const Handle(PCDM_ReaderFilter)&  theFilter,
                          const Message_ProgressRange&      theRange)
{
  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aFileStream = aFileSystem->OpenIStream (theFileName, std::ios::in | std::ios::binary);

  if (aFileStream.get() != NULL && aFileStream->good())
  {
    Handle(Storage_Data) dData;
    TCollection_ExtendedString aFormat = PCDM_ReadWriter::FileFormat (*aFileStream, dData);

    Read (*aFileStream, dData, theNewDocument, theApplication, theFilter, theRange);
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

//=======================================================================
//function : Read
//purpose  :
//=======================================================================
void BinLDrivers_DocumentRetrievalDriver::Read (Standard_IStream&                theIStream,
                                                const Handle(Storage_Data)&      theStorageData,
                                                const Handle(CDM_Document)&      theDoc,
                                                const Handle(CDM_Application)&   theApplication,
                                                const Handle(PCDM_ReaderFilter)& theFilter,
                                                const Message_ProgressRange&     theRange)
{
  myReaderStatus = PCDM_RS_DriverFailure;
  myMsgDriver = theApplication -> MessageDriver();

  const TCollection_ExtendedString aMethStr
    ("BinLDrivers_DocumentRetrievalDriver: ");

  Handle(TDocStd_Document) aDoc =
    Handle(TDocStd_Document)::DownCast(theDoc);
  if (aDoc.IsNull()) {
#ifdef OCCT_DEBUG
    myMsgDriver->Send (aMethStr + "error: null document", Message_Fail);
#endif
    myReaderStatus = PCDM_RS_NoDocument;
    return;
  }

  // 1. the information section
  Handle(Storage_HeaderData) aHeaderData;
  
  if (!theStorageData.IsNull())
  {
    aHeaderData = theStorageData->HeaderData();
  }

  if (!aHeaderData.IsNull())
  {
    for (Standard_Integer i = 1; i <= aHeaderData->UserInfo().Length(); i++)
    {
      const TCollection_AsciiString& aLine = aHeaderData->UserInfo().Value(i);

      if(aLine.Search(REFERENCE_COUNTER) != -1)
      {
        theDoc->SetReferenceCounter (aLine.Token(" ", 2).IntegerValue());
      }
      else if(aLine.Search(MODIFICATION_COUNTER) != -1)
      {
        theDoc->SetModifications (aLine.Token(" ", 2).IntegerValue());
      }
    }
  }

  // 1.a Version of writer
  if (!aHeaderData->StorageVersion().IsIntegerValue()) {
    // file has no format version
    myMsgDriver->Send (aMethStr + "error: file has no format version", Message_Fail);
    myReaderStatus = PCDM_RS_FormatFailure;
    return;
  }
  TDocStd_FormatVersion aFileVer = static_cast<TDocStd_FormatVersion>(aHeaderData->StorageVersion().IntegerValue());
  TDocStd_FormatVersion aCurrVer = TDocStd_Document::CurrentStorageFormatVersion();
  // maintain one-way compatibility starting from version 2+
  if (!CheckDocumentVersion(aFileVer, aCurrVer)) {
    myReaderStatus = PCDM_RS_NoVersion;
    // file was written with another version
    myMsgDriver->Send (aMethStr + "error: wrong file version: " +
                 aHeaderData->StorageVersion() + " while current is " +
                 TDocStd_Document::CurrentStorageFormatVersion(), Message_Fail);
    return;
  }

  // 1.b Retrieve the Types table
  TColStd_SequenceOfAsciiString aTypeNames; //Sequence of types in file
  const TColStd_SequenceOfAsciiString& aUserInfo = aHeaderData->UserInfo();
  Standard_Boolean begin = Standard_False;
  Standard_Integer i;
  for (i=1; i <= aUserInfo.Length(); i++) {
    TCollection_AsciiString aStr = aUserInfo(i);
    if (aStr == START_TYPES)
      begin = Standard_True;
    else if (aStr == END_TYPES)
      break;
    else if (begin) {
      if ( aFileVer < TDocStd_FormatVersion_VERSION_8) {
#ifdef DATATYPE_MIGRATION
        TCollection_AsciiString  newName;	
        if(Storage_Schema::CheckTypeMigration(aStr, newName)) {
#ifdef OCCT_DEBUG
          std::cout << "CheckTypeMigration:OldType = " <<aStr << " Len = "<<aStr.Length()<<std::endl;
          std::cout << "CheckTypeMigration:NewType = " <<newName  << " Len = "<< newName.Length()<<std::endl;
#endif
          aStr = newName;
        }
#endif  
      } 
      aTypeNames.Append (aStr);    
    }
  }
  if (myDrivers.IsNull())
    myDrivers = AttributeDrivers (myMsgDriver);
  myDrivers->AssignIds (aTypeNames); 

  // recognize types not supported by drivers
  myMapUnsupported.Clear();
  for (i=1; i <= aTypeNames.Length(); i++)
    if (myDrivers->GetDriver(i).IsNull()) 
      myMapUnsupported.Add(i);
  if (!myMapUnsupported.IsEmpty()) {
    myMsgDriver->Send (aMethStr + "warning: "
                  "the following attributes have no driver:", Message_Warning);
    for (i=1; i <= aTypeNames.Length(); i++)
      if (myMapUnsupported.Contains(i))
        myMsgDriver->Send (aTypeNames(i), Message_Warning);
  }

  // 2. Read document contents
  // 2a. Retrieve data from the stream:
  myRelocTable.Clear();
  myRelocTable.SetHeaderData(aHeaderData);
  mySections.Clear();
  myPAtt.Init();
  Handle(TDF_Data) aData = (!theFilter.IsNull() && theFilter->IsAppendMode()) ? aDoc->GetData() : new TDF_Data();

  Message_ProgressScope aPS (theRange, "Reading data", 3);
  Standard_Boolean aQuickPart = IsQuickPart (aFileVer);

  // 2b. Read the TOC of Sections
  if (aFileVer >= TDocStd_FormatVersion_VERSION_3) {
    BinLDrivers_DocumentSection aSection;
    do {
      if (!BinLDrivers_DocumentSection::ReadTOC (aSection, theIStream, aFileVer))
        break;
      mySections.Append(aSection);
    } while (!aSection.Name().IsEqual (aQuickPart ? ENDSECTION_POS : SHAPESECTION_POS) && !theIStream.eof());

    if (mySections.IsEmpty() || theIStream.eof()) {
      // There is no shape section in the file.
      myMsgDriver->Send (aMethStr + "error: shape section is not found", Message_Fail);
      myReaderStatus = PCDM_RS_ReaderException;
      return;
    }


    BinLDrivers_VectorOfDocumentSection::Iterator anIterS (mySections);
    // if there is only empty section, do not call tellg and seekg
    if (!mySections.IsEmpty() && (mySections.Size() > 1 || !anIterS.Value().Name().IsEqual(ENDSECTION_POS)))
    {
      std::streampos aDocumentPos = theIStream.tellg(); // position of root label
      for (; anIterS.More(); anIterS.Next()) {
        BinLDrivers_DocumentSection& aCurSection = anIterS.ChangeValue();
        if (aCurSection.IsPostRead() == Standard_False) {
          theIStream.seekg ((std::streampos) aCurSection.Offset());
          if (aCurSection.Name().IsEqual (SHAPESECTION_POS))
          {
            ReadShapeSection (aCurSection, theIStream, false, aPS.Next());
            if (!aPS.More())
            {
              myReaderStatus = PCDM_RS_UserBreak;
              return;
            }
          }
          else if (!aCurSection.Name().IsEqual (ENDSECTION_POS))
            ReadSection (aCurSection, theDoc, theIStream);
        }
      }
      theIStream.seekg(aDocumentPos);
    }
  } else { //aFileVer < 3
    std::streampos aDocumentPos = theIStream.tellg(); // position of root label
    // retrieve SHAPESECTION_POS string
    char aShapeSecLabel[SIZEOFSHAPELABEL + 1];
    aShapeSecLabel[SIZEOFSHAPELABEL] = 0x00;
    theIStream.read ((char*)&aShapeSecLabel, SIZEOFSHAPELABEL);// SHAPESECTION_POS
    TCollection_AsciiString aShapeLabel(aShapeSecLabel);
    // detect if a file was written in old fashion (version 2 without shapes)
    // and if so then skip reading ShapeSection
    if (aShapeLabel.Length() > 0) {
      // version 2+(with shapes) and higher goes here
      if(aShapeLabel.Length() <= 0 || aShapeLabel != SHAPESECTION_POS) {
        myMsgDriver->Send (aMethStr + "error: Format failure", Message_Fail);
        myReaderStatus = PCDM_RS_FormatFailure;
        return;
      }

      // retrieve ShapeSection Position
      Standard_Integer aShapeSectionPos; // go to ShapeSection
      theIStream.read ((char*)&aShapeSectionPos, sizeof(Standard_Integer));

#ifdef DO_INVERSE
      aShapeSectionPos = InverseInt (aShapeSectionPos);
#endif
#ifdef OCCT_DEBUG
      std::cout <<"aShapeSectionPos = " <<aShapeSectionPos <<std::endl;
#endif
      if(aShapeSectionPos) { 
        aDocumentPos = theIStream.tellg();
        theIStream.seekg((std::streampos) aShapeSectionPos);

        CheckShapeSection(aShapeSectionPos, theIStream);
        // Read Shapes
        BinLDrivers_DocumentSection aCurSection;
        ReadShapeSection (aCurSection, theIStream, Standard_False, aPS.Next());
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
  Standard_Integer aTag;
  theIStream.read ((char*)&aTag, sizeof(Standard_Integer));

  if (aQuickPart)
    myPAtt.SetIStream (theIStream); // for reading shapes data from the stream directly
  EnableQuickPartReading (myMsgDriver, aQuickPart);

  // read sub-tree of the root label
  if (!theFilter.IsNull())
    theFilter->StartIteration();
  const auto aStreamStartPosition = theIStream.tellg();
  Standard_Integer nbRead = ReadSubTree (theIStream, aData->Root(), theFilter, aQuickPart, Standard_False, aPS.Next());
  if (!myUnresolvedLinks.IsEmpty())
  {
    // In case we have skipped some linked TreeNodes before getting to
    // their children.
    theFilter->StartIteration();
    theIStream.seekg(aStreamStartPosition, std::ios_base::beg);
    nbRead += ReadSubTree(theIStream, aData->Root(), theFilter, aQuickPart, Standard_True, aPS.Next());
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
  aPS.Next();
    
  if (nbRead > 0) {
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
  if (aFileVer >= TDocStd_FormatVersion_VERSION_3) {
    BinLDrivers_VectorOfDocumentSection::Iterator aSectIter (mySections);
    for (; aSectIter.More(); aSectIter.Next()) {
      BinLDrivers_DocumentSection& aCurSection = aSectIter.ChangeValue();
      if (aCurSection.IsPostRead()) {
        theIStream.seekg ((std::streampos) aCurSection.Offset());
        ReadSection (aCurSection, theDoc, theIStream); 
      }
    }
  }
}

//=======================================================================
//function : ReadSubTree
//purpose  :
//=======================================================================

Standard_Integer BinLDrivers_DocumentRetrievalDriver::ReadSubTree
(Standard_IStream& theIS,
  const TDF_Label& theLabel,
  const Handle(PCDM_ReaderFilter)& theFilter,
  const Standard_Boolean& theQuickPart,
  const Standard_Boolean theReadMissing,
  const Message_ProgressRange& theRange)
{
  Standard_Integer nbRead = 0;
  TCollection_ExtendedString aMethStr
  ("BinLDrivers_DocumentRetrievalDriver: ");

  Message_ProgressScope aPS(theRange, "Reading sub tree", 2, true);

  bool aSkipAttrs = Standard_False;
  if (!theFilter.IsNull() && theFilter->IsPartTree())
    aSkipAttrs = !theFilter->IsPassed();

  if (theQuickPart)
  {
    uint64_t aLabelSize = 0;
    theIS.read((char*)&aLabelSize, sizeof(uint64_t));
#if DO_INVERSE
    aLabelSize = InverseUint64(aLabelSize);
#endif
    // no one sub-label is needed, so, skip everything
    if (aSkipAttrs && !theFilter->IsSubPassed() && myUnresolvedLinks.IsEmpty())
    {
      aLabelSize -= sizeof (uint64_t);
      theIS.seekg (aLabelSize, std::ios_base::cur);
      if (!theFilter.IsNull())
        theFilter->Up();
      return 0;
    }
  }

  if (theReadMissing)
  {
    aSkipAttrs = Standard_True;
  }
  const auto anAttStartPosition = theIS.tellg();
  // Read attributes:
  for (theIS >> myPAtt;
    theIS && myPAtt.TypeId() > 0 &&             // not an end marker ?
    myPAtt.Id() > 0 &&                          // not a garbage ?
    !theIS.eof();
    theIS >> myPAtt)
  {
    if (!aPS.More())
    {
      myReaderStatus = PCDM_RS_UserBreak;
      return -1;
    }
    if (myUnresolvedLinks.Remove(myPAtt.Id()) && aSkipAttrs)
    {
      aSkipAttrs = Standard_False;
      theIS.seekg(anAttStartPosition, std::ios_base::beg);
      continue;
    }
    if (aSkipAttrs)
    {
      if (myPAtt.IsDirect()) // skip direct written stream
      {
        uint64_t aStreamSize = 0;
        theIS.read ((char*)&aStreamSize, sizeof (uint64_t));
        aStreamSize -= sizeof (uint64_t); // size is already passed, so, reduce it by size
        theIS.seekg (aStreamSize, std::ios_base::cur);
      }
      continue;
    }

    // get a driver according to TypeId
    Handle(BinMDF_ADriver) aDriver = myDrivers->GetDriver(myPAtt.TypeId());
    if (!aDriver.IsNull()) {
      // create transient attribute
      Standard_Integer anID = myPAtt.Id();
      Handle(TDF_Attribute) tAtt;
      Standard_Boolean isBound = myRelocTable.IsBound(anID);
      if (isBound)
        tAtt = Handle(TDF_Attribute)::DownCast(myRelocTable.Find(anID));
      else
        tAtt = aDriver->NewEmpty();

      if (!theFilter.IsNull() && !theFilter->IsPassed (tAtt->DynamicType())) {
        if (myPAtt.IsDirect()) // skip direct written stream
        {
          uint64_t aStreamSize = 0;
          theIS.read ((char*)&aStreamSize, sizeof (uint64_t));
          aStreamSize -= sizeof (uint64_t); // size is already passed, so, reduce it by size
          theIS.seekg (aStreamSize, std::ios_base::cur);
        }
        continue;
      }
      nbRead++;

      if (tAtt->Label().IsNull())
      {
        if (!theFilter.IsNull() && theFilter->Mode() != PCDM_ReaderFilter::AppendMode_Forbid && theLabel.IsAttribute(tAtt->ID()))
        {
          if (theFilter->Mode() == PCDM_ReaderFilter::AppendMode_Protect)
            continue; // do not overwrite the existing attribute
          if (theFilter->Mode() == PCDM_ReaderFilter::AppendMode_Overwrite)
            theLabel.ForgetAttribute(tAtt->ID()); // forget old attribute to write a new one
        }
        try
        {
          theLabel.AddAttribute(tAtt);
        }
        catch (const Standard_DomainError&)
        {
          // For attributes that can have arbitrary GUID (e.g. TDataStd_Integer), exception
          // will be raised in valid case if attribute of that type with default GUID is already
          // present  on the same label; the reason is that actual GUID will be read later.
          // To avoid this, set invalid (null) GUID to the newly added attribute (see #29669)
          static const Standard_GUID fbidGuid;
          tAtt->SetID(fbidGuid);
          theLabel.AddAttribute(tAtt);
        }
      }
      else
        myMsgDriver->Send(aMethStr +
          "warning: attempt to attach attribute " +
          aDriver->TypeName() + " to a second label", Message_Warning);

      Standard_Boolean ok = aDriver->Paste(myPAtt, tAtt, myRelocTable);
      if (!ok) {
        // error converting persistent to transient
        myMsgDriver->Send(aMethStr + "warning: failure reading attribute " +
          aDriver->TypeName(), Message_Warning);
      }
      else if (!isBound)
      {
        myRelocTable.Bind(anID, tAtt);
        Handle(TDataStd_TreeNode) aNode = Handle(TDataStd_TreeNode)::DownCast(tAtt);
        if (!theFilter.IsNull() && !aNode.IsNull() && !aNode->Father().IsNull() && aNode->Father()->IsNew())
        {
          Standard_Integer anUnresolvedLink;
          myPAtt.SetPosition(BP_HEADSIZE);
          myPAtt >> anUnresolvedLink;
          myUnresolvedLinks.Add(anUnresolvedLink);
        }
      }
    }
    else if (!myMapUnsupported.Contains(myPAtt.TypeId()))
      myMsgDriver->Send(aMethStr + "warning: type ID not registered in header: "
        + myPAtt.TypeId(), Message_Warning);

  }
  if (!theIS || myPAtt.TypeId() != BinLDrivers_ENDATTRLIST) {
    // unexpected EOF or garbage data
    myMsgDriver->Send(aMethStr + "error: unexpected EOF or garbage data", Message_Fail);
    myReaderStatus = PCDM_RS_UnrecognizedFileFormat;
    return -1;
  }

  // Read children:
  // read the tag of a child label
  Standard_Integer aTag = BinLDrivers_ENDLABEL;
  theIS.read((char*)&aTag, sizeof(Standard_Integer));
#ifdef DO_INVERSE
  aTag = InverseInt(aTag);
#endif

  while (theIS && aTag >= 0 && !theIS.eof()) { // not an end marker ?
    // create sub-label
    TDF_Label aLab = theLabel.FindChild(aTag, Standard_True);
    if (!aPS.More())
    {
      myReaderStatus = PCDM_RS_UserBreak;
      return -1;
    }


    // read sub-tree
    if (!theFilter.IsNull())
      theFilter->Down (aTag);
    Standard_Integer nbSubRead = ReadSubTree (theIS, aLab, theFilter, theQuickPart, theReadMissing, aPS.Next());
    // check for error
    if (nbSubRead == -1)
      return -1;
    nbRead += nbSubRead;

    // read the tag of the next child
    theIS.read((char*)&aTag, sizeof(Standard_Integer));
#ifdef DO_INVERSE
    aTag = InverseInt(aTag);
#endif
  }

  if (aTag != BinLDrivers_ENDLABEL) {
    // invalid end label marker
    myMsgDriver->Send(aMethStr + "error: invalid end label marker", Message_Fail);
    myReaderStatus = PCDM_RS_UnrecognizedFileFormat;
    return -1;
  }
  if (!theFilter.IsNull())
    theFilter->Up();

  return nbRead;
}

//=======================================================================
//function : AttributeDrivers
//purpose  :
//=======================================================================

Handle(BinMDF_ADriverTable) BinLDrivers_DocumentRetrievalDriver::AttributeDrivers
       (const Handle(Message_Messenger)& theMessageDriver)
{
  return BinLDrivers::AttributeDrivers (theMessageDriver);
}

//=======================================================================
//function : ReadSection
//purpose  : 
//=======================================================================

void BinLDrivers_DocumentRetrievalDriver::ReadSection
                                (BinLDrivers_DocumentSection& /*theSection*/,
                                 const Handle(CDM_Document)&  /*theDocument*/,
                                 Standard_IStream&            /*theIS*/)
{
  // empty; should be redefined in subclasses
}

//=======================================================================
//function : ReadShapeSection
//purpose  : 
//=======================================================================

void BinLDrivers_DocumentRetrievalDriver::ReadShapeSection
                              (BinLDrivers_DocumentSection& theSection,
                               Standard_IStream&            /*theIS*/,
                               const Standard_Boolean isMess,
                               const Message_ProgressRange &/*theRange*/)

{
  if(isMess && theSection.Length()) {
    const TCollection_ExtendedString aMethStr ("BinLDrivers_DocumentRetrievalDriver: ");
    myMsgDriver->Send (aMethStr + "warning: Geometry is not supported by Lite schema. ", Message_Warning);
  }
}

//=======================================================================
//function : CheckShapeSection
//purpose  : 
//=======================================================================
void BinLDrivers_DocumentRetrievalDriver::CheckShapeSection
  (const Storage_Position& ShapeSectionPos, Standard_IStream& IS)
{
  if (!IS.eof())
  {
    const std::streamoff endPos = IS.rdbuf()->pubseekoff (0L, std::ios_base::end, std::ios_base::in);
#ifdef OCCT_DEBUG
    std::cout << "endPos = " << endPos <<std::endl;
#endif
    if(ShapeSectionPos != endPos) {
      const TCollection_ExtendedString aMethStr ("BinLDrivers_DocumentRetrievalDriver: ");
      myMsgDriver->Send (aMethStr + "warning: Geometry is not supported by Lite schema. ", Message_Warning);
    }
  }
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BinLDrivers_DocumentRetrievalDriver::Clear()
{
  myPAtt.Destroy();    // free buffer
  myRelocTable.Clear();
  myMapUnsupported.Clear();
}

//=======================================================================
//function : CheckDocumentVersion
//purpose  : 
//=======================================================================
Standard_Boolean BinLDrivers_DocumentRetrievalDriver::CheckDocumentVersion
  (const Standard_Integer theFileVersion, const Standard_Integer theCurVersion)
{
  if (theFileVersion < TDocStd_FormatVersion_LOWER || theFileVersion > theCurVersion) {
    // file was written with another version
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : IsQuickPart
//purpose  : 
//=======================================================================
Standard_Boolean BinLDrivers_DocumentRetrievalDriver::IsQuickPart (const Standard_Integer theFileVer)
{
  return theFileVer >= TDocStd_FormatVersion_VERSION_12;
}
