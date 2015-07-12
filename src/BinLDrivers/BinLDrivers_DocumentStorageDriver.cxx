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


#include <BinLDrivers.hxx>
#include <BinLDrivers_DocumentSection.hxx>
#include <BinLDrivers_DocumentStorageDriver.hxx>
#include <BinLDrivers_Marker.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_Application.hxx>
#include <CDM_Document.hxx>
#include <CDM_MessageDriver.hxx>
#include <FSD_BinaryFile.hxx>
#include <FSD_FileHeader.hxx>
#include <PCDM_ReadWriter.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Type.hxx>
#include <Storage_Schema.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>

#define SHAPESECTION_POS (Standard_CString)"SHAPE_SECTION_POS:"

//=======================================================================
//function : BinLDrivers_DocumentStorageDriver
//purpose  : Constructor
//=======================================================================

BinLDrivers_DocumentStorageDriver::BinLDrivers_DocumentStorageDriver ()
{
}

//=======================================================================
//function : SchemaName
//purpose  :
//=======================================================================

TCollection_ExtendedString BinLDrivers_DocumentStorageDriver::SchemaName() const
{
  TCollection_ExtendedString schemaname;
  return schemaname;
}

//=======================================================================
//function : Write
//purpose  :
//=======================================================================

void BinLDrivers_DocumentStorageDriver::Write
                          (const Handle(CDM_Document)&       theDocument,
                           const TCollection_ExtendedString& theFileName)
{
  SetIsError(Standard_False);
  SetStoreStatus(PCDM_SS_OK);

  myMsgDriver = theDocument -> Application() -> MessageDriver();
  myMapUnsupported.Clear();

  Handle(TDocStd_Document) aDoc =
    Handle(TDocStd_Document)::DownCast(theDocument);
  if (aDoc.IsNull()) {
    SetIsError(Standard_True);
    SetStoreStatus(PCDM_SS_Doc_IsNull);
  }
  else {
    // Open the file
    TCollection_AsciiString aFileName (theFileName);

    // First pass: collect empty labels, assign IDs to the types
    if (myDrivers.IsNull())
      myDrivers = AttributeDrivers (myMsgDriver);
    Handle(TDF_Data) aData = aDoc->GetData();
    FirstPass (aData->Root());

//  1. Write info section (including types table)
    WriteInfoSection(theDocument, aFileName);
    myTypesMap.Clear();
    if (IsError())
    {
        SetStoreStatus(PCDM_SS_Info_Section_Error);
        return;
    }

#if defined(_WIN32)
    ofstream anOS ((const wchar_t*) theFileName.ToExtString(), ios::in | ios::binary | ios::ate);
#elif !defined(IRIX) // 10.10.2005
    ofstream anOS (aFileName.ToCString(), ios::in | ios::binary | ios::ate);
#else
    ofstream anOS (aFileName.ToCString(), ios::ate);
    //ofstream anOS (aFileName.ToCString(), ios::out| ios::binary | ios::ate);
#endif
#ifdef OCCT_DEBUG
    const Standard_Integer aP = (Standard_Integer) anOS.tellp();
    cout << "POS = " << aP <<endl;
#endif
//#endif

    if (anOS) {

//  2. Write the Table of Contents of Sections
      BinLDrivers_VectorOfDocumentSection::Iterator anIterS (mySections);
      for (; anIterS.More(); anIterS.Next())
        anIterS.ChangeValue().WriteTOC (anOS);

      // Shapes Section is the last one, it indicates the end of the table.
      BinLDrivers_DocumentSection aShapesSection (SHAPESECTION_POS,
                                                  Standard_False);
      aShapesSection.WriteTOC (anOS);

//  3. Write document contents
      // (Storage data to the stream)
      myRelocTable.Clear();
      myPAtt.Init();

//    Write Doc structure
      WriteSubTree (aData->Root(), anOS); // Doc is written

//  4. Write Shapes section
      WriteShapeSection(aShapesSection, anOS);

// Write application-defined sections
      for (anIterS.Init (mySections); anIterS.More(); anIterS.Next()) {
        BinLDrivers_DocumentSection& aSection = anIterS.ChangeValue();
        const Standard_Size aSectionOffset = (Standard_Size) anOS.tellp();
        WriteSection (aSection.Name(), theDocument, anOS);
        aSection.Write (anOS, aSectionOffset);
      }

// End of processing: close structures and check the status
      myPAtt.Destroy();   // free buffer
      myEmptyLabels.Clear();
      myMapUnsupported.Clear();

      if (!myRelocTable.Extent()) {
        // No objects written
#ifdef OCCT_DEBUG
        WriteMessage ("BinLDrivers_DocumentStorageDriver, no objects written");
#endif
        SetIsError(Standard_True);
        SetStoreStatus(PCDM_SS_No_Obj);
      }
      myRelocTable.Clear();
    }

    if (!anOS) {
      // A problem with the stream
#ifdef OCCT_DEBUG
      TCollection_ExtendedString anErrorStr ("Error: ");
      WriteMessage (anErrorStr + "BinLDrivers_DocumentStorageDriver, Problem with the file stream, rdstate="
                    + (Standard_Integer )anOS.rdstate());
#endif
      SetIsError(Standard_True);
      SetStoreStatus(PCDM_SS_WriteFailure);
    }

  }
}

//=======================================================================
//function : UnsupportedAttrMsg
//purpose  :
//=======================================================================

void BinLDrivers_DocumentStorageDriver::UnsupportedAttrMsg
                        (const Handle(Standard_Type)& theType)
{
#ifdef OCCT_DEBUG
  static TCollection_ExtendedString aMsg
    ("BinDrivers_DocumentStorageDriver: warning: attribute driver for type ");
#endif
  if (!myMapUnsupported.Contains(theType)) {
    myMapUnsupported.Add(theType);
#ifdef OCCT_DEBUG
    WriteMessage (aMsg + theType->Name() + " not found");
#endif
  }
}

//=======================================================================
//function : WriteSubTree
//purpose  :
//=======================================================================

void BinLDrivers_DocumentStorageDriver::WriteSubTree
                        (const TDF_Label&          theLabel,
                         Standard_OStream&         theOS)
{
  // Skip empty labels
  if (!myEmptyLabels.IsEmpty() && myEmptyLabels.First() == theLabel) {
    myEmptyLabels.RemoveFirst();
    return;
  }

  // Write label header: tag
  Standard_Integer aTag = theLabel.Tag();
#if DO_INVERSE
  aTag = InverseInt (aTag);
#endif
  theOS.write ((char*)&aTag, sizeof(Standard_Integer));

  // Write attributes
  TDF_AttributeIterator itAtt (theLabel);
  for ( ; itAtt.More() && theOS; itAtt.Next()) {
    const Handle(TDF_Attribute)& tAtt = itAtt.Value();
    const Handle(Standard_Type)& aType = tAtt->DynamicType();
    // Get type ID and driver
    Handle(BinMDF_ADriver) aDriver;
    const Standard_Integer aTypeId = myDrivers->GetDriver (aType,aDriver);
    if (aTypeId > 0) {
      // Add source to relocation table
      const Standard_Integer anId = myRelocTable.Add (tAtt);

      // Create and fill data item
      myPAtt.SetTypeId (aTypeId);
      myPAtt.SetId (anId);
      aDriver->Paste (tAtt, myPAtt, myRelocTable);

      // Write data to the stream -->!!!
      theOS << myPAtt;
    }
#ifdef OCCT_DEBUG
    else
      UnsupportedAttrMsg (aType);
#endif
  }
  if (!theOS) {
    // Problem with the stream
    return;
  }

  // Write the end attributes list marker
  BinLDrivers_Marker anEndAttr = BinLDrivers_ENDATTRLIST;
#if DO_INVERSE
  anEndAttr = (BinLDrivers_Marker) InverseInt (anEndAttr);
#endif
  theOS.write ((char*)&anEndAttr, sizeof(anEndAttr));

  // Process sub-labels
  TDF_ChildIterator itChld (theLabel);
  for ( ; itChld.More(); itChld.Next())
  {
    const TDF_Label& aChildLab = itChld.Value();
    WriteSubTree (aChildLab, theOS);
  }

  // Write the end label marker
  BinLDrivers_Marker anEndLabel = BinLDrivers_ENDLABEL;
#if DO_INVERSE
  anEndLabel = (BinLDrivers_Marker) InverseInt (anEndLabel);
#endif
  theOS.write ((char*)&anEndLabel, sizeof(anEndLabel));

}

//=======================================================================
//function : AttributeDrivers
//purpose  :
//=======================================================================

Handle(BinMDF_ADriverTable) BinLDrivers_DocumentStorageDriver::AttributeDrivers
       (const Handle(CDM_MessageDriver)& theMessageDriver)
{
  return BinLDrivers::AttributeDrivers (theMessageDriver);
}

//=======================================================================
//function : FirstPassSubTree
//purpose  :
//=======================================================================

Standard_Boolean BinLDrivers_DocumentStorageDriver::FirstPassSubTree
                         (const TDF_Label&                   L,
                          TDF_LabelList&                     ListOfEmptyL)
{
  // are there writable attributes on L ?
  Standard_Boolean hasAttr = Standard_False;
  TDF_AttributeIterator itAtt (L);
  for ( ; itAtt.More(); itAtt.Next()) {
    const Handle(Standard_Type)& aType = itAtt.Value()->DynamicType();
    Handle(BinMDF_ADriver) aDriver;
    // do not rely on a value returned by GetDriver here, because
    // the IDs have not yet been assigned to the types
    myDrivers->GetDriver (aType, aDriver);
    if (!aDriver.IsNull()) {
      hasAttr = Standard_True;
      myTypesMap.Add (aType);
    }
#ifdef OCCT_DEBUG
    else
      UnsupportedAttrMsg (aType);
#endif
  }

  // are there writable attributes on sub-labels ?
  Standard_Boolean hasChildAttr = Standard_False;
  TDF_LabelList emptyChildrenList;
  TDF_ChildIterator itChld (L);
  for ( ; itChld.More(); itChld.Next())
  {
    if (FirstPassSubTree (itChld.Value(), emptyChildrenList))
      emptyChildrenList.Append( itChld.Value() );
    else
      hasChildAttr = Standard_True;
  }

  Standard_Boolean isEmpty = !(hasAttr || hasChildAttr);

  if (!isEmpty)
    ListOfEmptyL.Append( emptyChildrenList );

  return isEmpty;
}

//=======================================================================
//function : FirstPass
//purpose  :
//=======================================================================

void BinLDrivers_DocumentStorageDriver::FirstPass
                         (const TDF_Label& theRoot)
{
  myTypesMap.Clear();
  myEmptyLabels.Clear();

  if (FirstPassSubTree( theRoot, myEmptyLabels))
    myEmptyLabels.Append( theRoot );

  myDrivers->AssignIds (myTypesMap);
}

//=======================================================================
//function : WriteInfoSection
//purpose  : Write info secton using FSD_BinaryFile driver
//=======================================================================

#define START_TYPES "START_TYPES"
#define END_TYPES "END_TYPES"

void BinLDrivers_DocumentStorageDriver::WriteInfoSection
                         (const Handle(CDM_Document)&    theDocument,
                          const TCollection_AsciiString& theFileName)
{
  FSD_BinaryFile aFileDriver;
  if (aFileDriver.Open( theFileName, Storage_VSWrite ) != Storage_VSOk) {
    WriteMessage (TCollection_ExtendedString("Error: Cannot open file ") +
                  theFileName);
    SetIsError(Standard_True);
    return;
  }

  if (aFileDriver.BeginWriteInfoSection() == Storage_VSOk)
  {
    // add format
    Handle(Storage_Data) theData = new Storage_Data;
    PCDM_ReadWriter::WriteFileFormat( theData, theDocument );
    PCDM_ReadWriter::Writer()->WriteReferenceCounter(theData,theDocument);
    PCDM_ReadWriter::Writer()->WriteReferences(theData,theDocument,theFileName);
    PCDM_ReadWriter::Writer()->WriteExtensions(theData,theDocument);
    PCDM_ReadWriter::Writer()->WriteVersion(theData,theDocument);

    // add the types table
    theData->AddToUserInfo(START_TYPES);
    Standard_Integer i;
    for (i = 1; i <= myTypesMap.Extent(); i++) {
      Handle(BinMDF_ADriver) aDriver = myDrivers->GetDriver(i);
      if (!aDriver.IsNull()) {
        const TCollection_AsciiString& aTypeName = aDriver->TypeName();
        theData->AddToUserInfo(aTypeName);
      }
    }
    theData->AddToUserInfo(END_TYPES);

    // add document comments
    TColStd_SequenceOfExtendedString aComments;
    theDocument->Comments(aComments);
    for (i = 1; i <= aComments.Length(); i++)
      theData->AddToComments(aComments(i));

    // Info
    aFileDriver.WriteInfo
      (1,           //   nbObj
       BinLDrivers::StorageVersion(),
       Storage_Schema::ICreationDate(),
       TCollection_AsciiString(SchemaName(),'?'),
       1,           //   schemaVersion
       theData->ApplicationName(),
       theData->ApplicationVersion(),
       theData->DataType(),
       theData->UserInfo()
       );

    // we write a complete header section: info and comments
    aFileDriver.EndWriteInfoSection();
    aFileDriver.BeginWriteCommentSection();
    aFileDriver.WriteComment(theData->Comments());// <=== !!! szy - it was missed
    aFileDriver.EndWriteCommentSection();
    // here the location of info and comment sections is written
    aFileDriver.EndWriteDataSection();
  }
  else {
    WriteMessage(TCollection_ExtendedString("Error: Problem writing header "
                                            "into file ") + theFileName);
    SetIsError(Standard_True);
  }
#ifdef OCCT_DEBUG
    const Standard_Integer aP = (Standard_Integer) aFileDriver.Tell();
    cout << "POS = " << aP <<endl;
#endif
  aFileDriver.Close();
}

//=======================================================================
//function : WriteMessage
//purpose  : write  theMessage  to  the  MessageDriver  of  the
//           Application
//=======================================================================

void BinLDrivers_DocumentStorageDriver::WriteMessage
                         (const TCollection_ExtendedString& theMsg)
{
  if (!myMsgDriver.IsNull())
    myMsgDriver->Write (theMsg.ToExtString());
}

//=======================================================================
//function : AddSection
//purpose  :
//=======================================================================

void BinLDrivers_DocumentStorageDriver::AddSection
                                (const TCollection_AsciiString& theName,
                                 const Standard_Boolean         isPostRead)
{
  mySections.Append (BinLDrivers_DocumentSection (theName, isPostRead));
}

//=======================================================================
//function : WriteSection
//purpose  :
//=======================================================================

void BinLDrivers_DocumentStorageDriver::WriteSection
                                (const TCollection_AsciiString& /*theName*/,
                                 const Handle(CDM_Document)&     /*theDocument*/,
                                 Standard_OStream&              /*theOS*/)
{
  // empty; should be redefined in subclasses
}

//=======================================================================
//function : WriteShapeSection
//purpose  : defines WriteShapeSection
//=======================================================================
void BinLDrivers_DocumentStorageDriver::WriteShapeSection
                                (BinLDrivers_DocumentSection&   theSection,
                                 Standard_OStream&              theOS)
{
  const Standard_Size aShapesSectionOffset = (Standard_Size) theOS.tellp();
  theSection.Write (theOS, aShapesSectionOffset);
}
