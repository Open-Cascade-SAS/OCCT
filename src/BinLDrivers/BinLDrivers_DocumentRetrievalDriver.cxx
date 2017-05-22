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
#include <BinMDF_ADriver.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_Application.hxx>
#include <CDM_Document.hxx>
#include <CDM_MessageDriver.hxx>
#include <FSD_BinaryFile.hxx>
#include <FSD_FileHeader.hxx>
#include <OSD_OpenFile.hxx>
#include <PCDM_Document.hxx>
#include <PCDM_ReadWriter.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Type.hxx>
#include <Storage_HeaderData.hxx>
#include <Storage_Schema.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Owner.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinLDrivers_DocumentRetrievalDriver,PCDM_RetrievalDriver)

#define SHAPESECTION_POS "SHAPE_SECTION_POS:"
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
//function : CreateDocument
//purpose  : pure virtual method definition
//=======================================================================

Handle(CDM_Document) BinLDrivers_DocumentRetrievalDriver::CreateDocument()
{
  return new TDocStd_Document(PCDM_RetrievalDriver::GetFormat());
}

//=======================================================================
//function : Read
//purpose  :
//=======================================================================
void BinLDrivers_DocumentRetrievalDriver::Read
                         (const TCollection_ExtendedString& theFileName,
                          const Handle(CDM_Document)&       theNewDocument,
                          const Handle(CDM_Application)&    theApplication)
{
  std::ifstream aFileStream;
  OSD_OpenStream (aFileStream, theFileName, std::ios::in | std::ios::binary);

  if (aFileStream.is_open() && aFileStream.good())
  {
    Handle(Storage_Data) dData;
    TCollection_ExtendedString aFormat = PCDM_ReadWriter::FileFormat (aFileStream, dData);

    Read (aFileStream, dData, theNewDocument, theApplication);
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
void BinLDrivers_DocumentRetrievalDriver::Read (Standard_IStream&               theIStream,
                                                const Handle(Storage_Data)&     theStorageData,
                                                const Handle(CDM_Document)&     theDoc,
                                                const Handle(CDM_Application)&  theApplication)
{
  myReaderStatus = PCDM_RS_DriverFailure;
  myMsgDriver = theApplication -> MessageDriver();

  const TCollection_ExtendedString aMethStr
    ("BinLDrivers_DocumentRetrievalDriver: ");

  Handle(TDocStd_Document) aDoc =
    Handle(TDocStd_Document)::DownCast(theDoc);
  if (aDoc.IsNull()) {
#ifdef OCCT_DEBUG
    WriteMessage (aMethStr + "error: null document");
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
    WriteMessage (aMethStr + "error: file has no format version");
    myReaderStatus = PCDM_RS_FormatFailure;
    return;
  }
  Standard_Integer aFileVer = aHeaderData->StorageVersion().IntegerValue();
  Standard_Integer aCurrVer = BinLDrivers::StorageVersion().IntegerValue();
  // maintain one-way compatibility starting from version 2+
  if (!CheckDocumentVersion(aFileVer, aCurrVer)) {
    myReaderStatus = PCDM_RS_NoVersion;
    // file was written with another version
    WriteMessage (aMethStr + "error: wrong file version: " +
		  aHeaderData->StorageVersion() + " while current is " +
		  BinLDrivers::StorageVersion());
    return;
  }

  // 1.b Retrieve the Types table
  TColStd_SequenceOfAsciiString aTypeNames; //Sequence of types in file
  const TColStd_SequenceOfAsciiString& aUserInfo = aHeaderData->UserInfo();
  Standard_Boolean begin = Standard_False;
  Standard_Integer i;
  for (i=1; i <= aUserInfo.Length(); i++) {
    //const TCollection_AsciiString& aStr = aUserInfo(i);
    TCollection_AsciiString aStr = aUserInfo(i);
    if (aStr == START_TYPES)
      begin = Standard_True;
    else if (aStr == END_TYPES)
      break;
    else if (begin) {
      if ( aFileVer < 8 ) {
#ifdef DATATYPE_MIGRATION
	TCollection_AsciiString  newName;	
	if(Storage_Schema::CheckTypeMigration(aStr, newName)) {
#ifdef OCCT_DEBUG
	  cout << "CheckTypeMigration:OldType = " <<aStr << " Len = "<<aStr.Length()<<endl;
	  cout << "CheckTypeMigration:NewType = " <<newName  << " Len = "<< newName.Length()<<endl;
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
    WriteMessage (aMethStr + "warning: "
                  "the following attributes have no driver:");
    for (i=1; i <= aTypeNames.Length(); i++)
      if (myMapUnsupported.Contains(i))
	WriteMessage (aTypeNames(i));
  }

  // propagate the opened document version to data drivers
  PropagateDocumentVersion(aFileVer);

  // 2. Read document contents

  // 2a. Retrieve data from the stream:
  myRelocTable.Clear();
  mySections.Clear();
  myPAtt.Init();
  Handle(TDF_Data) aData = new TDF_Data();
  streampos aDocumentPos = -1;

  // 2b. Read the TOC of Sections
  if (aFileVer >= 3) {
    BinLDrivers_DocumentSection aSection;
    do {
      BinLDrivers_DocumentSection::ReadTOC (aSection, theIStream);
      mySections.Append(aSection);
    } while(!aSection.Name().IsEqual((Standard_CString)SHAPESECTION_POS) && !theIStream.eof());

    if (theIStream.eof()) {
      // There is no shape section in the file.
      WriteMessage (aMethStr + "error: shape section is not found");
      myReaderStatus = PCDM_RS_ReaderException;
      return;
    }

    aDocumentPos = theIStream.tellg(); // position of root label

    BinLDrivers_VectorOfDocumentSection::Iterator anIterS (mySections);
    for (; anIterS.More(); anIterS.Next()) {
      BinLDrivers_DocumentSection& aCurSection = anIterS.ChangeValue();
      if (aCurSection.IsPostRead() == Standard_False) {
        theIStream.seekg ((streampos) aCurSection.Offset());
        if (aCurSection.Name().IsEqual ((Standard_CString)SHAPESECTION_POS)) 
          ReadShapeSection (aCurSection, theIStream);
        else
          ReadSection (aCurSection, theDoc, theIStream); 
      }
    }
  } else { //aFileVer < 3
    aDocumentPos = theIStream.tellg(); // position of root label

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
        WriteMessage (aMethStr + "error: Format failure");
        myReaderStatus = PCDM_RS_FormatFailure;
        return;
      }

      // retrieve ShapeSection Position
      Standard_Integer aShapeSectionPos; // go to ShapeSection
      theIStream.read ((char*)&aShapeSectionPos, sizeof(Standard_Integer));

#if DO_INVERSE
      aShapeSectionPos = InverseInt (aShapeSectionPos);
#endif
#ifdef OCCT_DEBUG
      cout <<"aShapeSectionPos = " <<aShapeSectionPos <<endl;
#endif
      if(aShapeSectionPos) { 
	aDocumentPos = theIStream.tellg();
	theIStream.seekg((streampos) aShapeSectionPos);

	CheckShapeSection(aShapeSectionPos, theIStream);
	// Read Shapes
	BinLDrivers_DocumentSection aCurSection;
	ReadShapeSection (aCurSection, theIStream, Standard_False);
      }
    }
  } // end of reading Sections or shape section

  // Return to read of the Document structure
  theIStream.seekg(aDocumentPos);

  // read the header (tag) of the root label
  Standard_Integer aTag;
  theIStream.read ((char*)&aTag, sizeof(Standard_Integer));

  // read sub-tree of the root label
  Standard_Integer nbRead = ReadSubTree (theIStream, aData->Root());
  Clear();
    
  if (nbRead > 0) {
    // attach data to the document
    aDoc->SetData (aData);
    TDocStd_Owner::SetDocument (aData, aDoc);
    aDoc->SetComments(aHeaderData->Comments());
    myReaderStatus = PCDM_RS_OK;
  }

  // Read Sections (post-reading type)
  if (aFileVer >= 3) {
    BinLDrivers_VectorOfDocumentSection::Iterator aSectIter (mySections);
    for (; aSectIter.More(); aSectIter.Next()) {
      BinLDrivers_DocumentSection& aCurSection = aSectIter.ChangeValue();
      if (aCurSection.IsPostRead()) {
	theIStream.seekg ((streampos) aCurSection.Offset());
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
                          const TDF_Label&  theLabel)
{
  Standard_Integer nbRead = 0;
  static TCollection_ExtendedString aMethStr
    ("BinLDrivers_DocumentRetrievalDriver: ");

  // Read attributes:
  theIS >> myPAtt;
  while (theIS && myPAtt.TypeId() > 0 &&             // not an end marker ?
         myPAtt.Id() > 0 &&                          // not a garbage ?
         !theIS.eof()) {
    // get a driver according to TypeId
    Handle(BinMDF_ADriver) aDriver = myDrivers->GetDriver (myPAtt.TypeId());
    if (!aDriver.IsNull()) {
      // create transient attribute
      nbRead++;
      Standard_Integer anID = myPAtt.Id();
      Handle(TDF_Attribute) tAtt;
      Standard_Boolean isBound = myRelocTable.IsBound(anID);
      if (isBound)
        tAtt = Handle(TDF_Attribute)::DownCast(myRelocTable.Find(anID));
      else
        tAtt = aDriver->NewEmpty();
      if (tAtt->Label().IsNull())
	theLabel.AddAttribute (tAtt);
      else
	WriteMessage (aMethStr +
		      "warning: attempt to attach attribute " +
		      aDriver->TypeName() + " to a second label");

      Standard_Boolean ok = aDriver->Paste (myPAtt, tAtt, myRelocTable);
      if (!ok) {
        // error converting persistent to transient
        WriteMessage (aMethStr + "warning: failure reading attribute " +
                      aDriver->TypeName());
      }
      else if (!isBound)
        myRelocTable.Bind (anID, tAtt);
    }
    else if (!myMapUnsupported.Contains(myPAtt.TypeId()))
      WriteMessage (aMethStr + "warning: type ID not registered in header: "
                    + myPAtt.TypeId());

    // read next attribute
    theIS >> myPAtt;
  }
  if (!theIS || myPAtt.TypeId() != BinLDrivers_ENDATTRLIST) {
    // unexpected EOF or garbage data
    WriteMessage (aMethStr + "error: unexpected EOF or garbage data");
    myReaderStatus = PCDM_RS_UnrecognizedFileFormat;
    return -1;
  }

  // Read children:
  // read the tag of a child label
  Standard_Integer aTag = BinLDrivers_ENDLABEL;
  theIS.read ((char*) &aTag, sizeof(Standard_Integer));
#if DO_INVERSE
  aTag = InverseInt (aTag);
#endif
  while (theIS && aTag >= 0 && !theIS.eof()) { // not an end marker ?
    // create sub-label
    TDF_Label aLab = theLabel.FindChild (aTag, Standard_True);

    // read sub-tree
    Standard_Integer nbSubRead = ReadSubTree(theIS, aLab);
    // check for error
    if (nbSubRead == -1)
      return -1;
    nbRead += nbSubRead;

    // read the tag of the next child
    theIS.read ((char*) &aTag, sizeof(Standard_Integer));
#if DO_INVERSE
    aTag = InverseInt (aTag);
#endif
  }
  if (aTag != BinLDrivers_ENDLABEL) {
    // invalid end label marker
    WriteMessage (aMethStr + "error: invalid end label marker");
    myReaderStatus = PCDM_RS_UnrecognizedFileFormat;
    return -1;
  }

  return nbRead;
}

//=======================================================================
//function : AttributeDrivers
//purpose  :
//=======================================================================

Handle(BinMDF_ADriverTable) BinLDrivers_DocumentRetrievalDriver::AttributeDrivers
       (const Handle(CDM_MessageDriver)& theMessageDriver)
{
  return BinLDrivers::AttributeDrivers (theMessageDriver);
}


//=======================================================================
//function : WriteMessage
//purpose  : write   theMessage   to   the   MessageDriver   of
//           theApplication
//=======================================================================

void BinLDrivers_DocumentRetrievalDriver::WriteMessage
                         (const TCollection_ExtendedString& theMsg)
{
  if (!myMsgDriver.IsNull())
    myMsgDriver->Write (theMsg.ToExtString());
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
			       const Standard_Boolean isMess)

{
  if(isMess && theSection.Length()) {
    const TCollection_ExtendedString aMethStr ("BinLDrivers_DocumentRetrievalDriver: ");
    WriteMessage (aMethStr + "warning: Geometry is not supported by Lite schema. ");
  }
}

//=======================================================================
//function : CheckShapeSection
//purpose  : 
//=======================================================================
void BinLDrivers_DocumentRetrievalDriver::CheckShapeSection(
				  const Storage_Position& ShapeSectionPos, 
						    Standard_IStream& IS)
{
  if (!IS.eof())
  {
    const std::streamoff endPos = IS.rdbuf()->pubseekoff(0L, std::ios_base::end, std::ios_base::in);
#ifdef OCCT_DEBUG
    cout << "endPos = " << endPos <<endl;
#endif
    if(ShapeSectionPos != endPos) {
      const TCollection_ExtendedString aMethStr ("BinLDrivers_DocumentRetrievalDriver: ");
      WriteMessage (aMethStr + "warning: Geometry is not supported by Lite schema. ");
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
//function : PropagateDocumentVersion
//purpose  : 
//=======================================================================
void BinLDrivers_DocumentRetrievalDriver::PropagateDocumentVersion(const Standard_Integer theDocVersion )
{
  BinMDataStd::SetDocumentVersion(theDocVersion);
}

//=======================================================================
//function : CheckDocumentVersion
//purpose  : 
//=======================================================================
Standard_Boolean BinLDrivers_DocumentRetrievalDriver::CheckDocumentVersion(
                                                          const Standard_Integer theFileVersion,
                                                          const Standard_Integer theCurVersion)
{
  if (theFileVersion < 2 || theFileVersion > theCurVersion) {
    // file was written with another version
    return Standard_False;
  }
  return Standard_True;
}

