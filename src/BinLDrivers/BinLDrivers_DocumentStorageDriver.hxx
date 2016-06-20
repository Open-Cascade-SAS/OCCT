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
#include <Standard_Type.hxx>

#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <TDF_LabelList.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>
#include <BinLDrivers_VectorOfDocumentSection.hxx>
#include <PCDM_StorageDriver.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Type.hxx>
class BinMDF_ADriverTable;
class CDM_MessageDriver;
class TCollection_ExtendedString;
class CDM_Document;
class TDF_Label;
class TCollection_AsciiString;
class BinLDrivers_DocumentSection;


class BinLDrivers_DocumentStorageDriver;
DEFINE_STANDARD_HANDLE(BinLDrivers_DocumentStorageDriver, PCDM_StorageDriver)

//! persistent implemention of storage a document in a binary file
class BinLDrivers_DocumentStorageDriver : public PCDM_StorageDriver
{

public:

  
  //! Constructor
  Standard_EXPORT BinLDrivers_DocumentStorageDriver();
    
  //! Write <theDocument> to the binary file <theFileName>
  Standard_EXPORT virtual void Write (const Handle(CDM_Document)& theDocument, const TCollection_ExtendedString& theFileName) Standard_OVERRIDE;

  //! Write <theDocument> to theOStream
  Standard_EXPORT virtual void Write (const Handle(CDM_Document)& theDocument, Standard_OStream& theOStream) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(BinMDF_ADriverTable) AttributeDrivers (const Handle(CDM_MessageDriver)& theMsgDriver);
  
  //! Create a section that should be written after the OCAF data
  Standard_EXPORT void AddSection (const TCollection_AsciiString& theName, const Standard_Boolean isPostRead = Standard_True);




  DEFINE_STANDARD_RTTIEXT(BinLDrivers_DocumentStorageDriver,PCDM_StorageDriver)

protected:

  
  //! Write the tree under <theLabel> to the stream <theOS>
  Standard_EXPORT void WriteSubTree (const TDF_Label& theData, Standard_OStream& theOS);
  
  //! define the procedure of writing a section to file.
  Standard_EXPORT virtual void WriteSection (const TCollection_AsciiString& theName, const Handle(CDM_Document)& theDoc, Standard_OStream& theOS);
  
  //! defines the procedure of writing a shape  section to file
  Standard_EXPORT virtual void WriteShapeSection (BinLDrivers_DocumentSection& theDocSection, Standard_OStream& theOS);
  
  //! write  theMessage  to  the  MessageDriver  of  the
  //! Application
  Standard_EXPORT void WriteMessage (const TCollection_ExtendedString& theMessage);

  Handle(BinMDF_ADriverTable) myDrivers;
  BinObjMgt_SRelocationTable myRelocTable;


private:

  
  Standard_EXPORT void FirstPass (const TDF_Label& theRoot);
  
  //! Returns true if <L> and its sub-labels do not contain
  //! attributes to store
  Standard_EXPORT Standard_Boolean FirstPassSubTree (const TDF_Label& L, TDF_LabelList& ListOfEmptyL);
  
  //! Write info secton using FSD_BinaryFile driver
  Standard_EXPORT void WriteInfoSection (const Handle(CDM_Document)& theDocument, Standard_OStream& theOStream);
  
  Standard_EXPORT void UnsupportedAttrMsg (const Handle(Standard_Type)& theType);

  BinObjMgt_Persistent myPAtt;
  Handle(CDM_MessageDriver) myMsgDriver;
  TDF_LabelList myEmptyLabels;
  TColStd_MapOfTransient myMapUnsupported;
  TColStd_IndexedMapOfTransient myTypesMap;
  BinLDrivers_VectorOfDocumentSection mySections;
  TCollection_ExtendedString myFileName;


};







#endif // _BinLDrivers_DocumentStorageDriver_HeaderFile
