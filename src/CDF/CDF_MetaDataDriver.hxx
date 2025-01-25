// Created on: 1997-11-17
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _CDF_MetaDataDriver_HeaderFile
#define _CDF_MetaDataDriver_HeaderFile

#include <Standard_Type.hxx>

class CDM_MetaData;
class TCollection_ExtendedString;
class CDM_Document;
class PCDM_ReferenceIterator;
class Message_Messenger;

class CDF_MetaDataDriver;
DEFINE_STANDARD_HANDLE(CDF_MetaDataDriver, Standard_Transient)

//! this class list the method that must be available for
//! a specific DBMS
class CDF_MetaDataDriver : public Standard_Transient
{

public:
  //! returns true if the MetaDataDriver can manage different
  //! versions of a Data.
  //! By default, returns Standard_False.
  Standard_EXPORT virtual Standard_Boolean HasVersionCapability();

  //! Creates a "Depends On"  relation between two Datas.
  //! By default does nothing
  Standard_EXPORT virtual void CreateDependsOn(const Handle(CDM_MetaData)& aFirstData,
                                               const Handle(CDM_MetaData)& aSecondData);

  Standard_EXPORT virtual void CreateReference(const Handle(CDM_MetaData)& aFrom,
                                               const Handle(CDM_MetaData)& aTo,
                                               const Standard_Integer      aReferenceIdentifier,
                                               const Standard_Integer      aToDocumentVersion);

  //! by default return Standard_True.
  Standard_EXPORT virtual Standard_Boolean HasVersion(const TCollection_ExtendedString& aFolder,
                                                      const TCollection_ExtendedString& aName);

  Standard_EXPORT virtual TCollection_ExtendedString BuildFileName(
    const Handle(CDM_Document)& aDocument) = 0;

  //! this method is useful if the name of an object --
  //! depends on the metadatadriver. For  example a Driver
  //! -- based  on the operating  system can choose to  add
  //! the extension of file to create to the object.
  Standard_EXPORT virtual TCollection_ExtendedString SetName(
    const Handle(CDM_Document)&       aDocument,
    const TCollection_ExtendedString& aName);

  //! should indicate whether meta-data exist in the DBMS corresponding
  //! to the Data.
  //! aVersion may be NULL;
  Standard_EXPORT virtual Standard_Boolean Find(const TCollection_ExtendedString& aFolder,
                                                const TCollection_ExtendedString& aName,
                                                const TCollection_ExtendedString& aVersion) = 0;

  Standard_EXPORT virtual Standard_Boolean HasReadPermission(
    const TCollection_ExtendedString& aFolder,
    const TCollection_ExtendedString& aName,
    const TCollection_ExtendedString& aVersion) = 0;

  //! should return the MetaData stored in the DBMS with the meta-data
  //! corresponding to the Data. If the MetaDataDriver has version management capabilities
  //! the version has to be set in the returned MetaData.
  //! aVersion may be NULL
  //! MetaData is called by GetMetaData
  //! If the version is  set to NULL, MetaData should return
  //! the last version of the metadata
  Standard_EXPORT virtual Handle(CDM_MetaData) MetaData(
    const TCollection_ExtendedString& aFolder,
    const TCollection_ExtendedString& aName,
    const TCollection_ExtendedString& aVersion) = 0;

  //! by default returns aMetaDATA
  //! should return the MetaData stored in the DBMS with the meta-data
  //! corresponding to the path. If the MetaDataDriver has version management capabilities
  //! the version has to be set in the returned MetaData.
  //! MetaData is called by GetMetaData
  //! If the version is not included in the path , MetaData should return
  //! the last version of the metadata
  //! is deferred;
  Standard_EXPORT virtual Handle(CDM_MetaData) LastVersion(const Handle(CDM_MetaData)& aMetaData);

  //! should create meta-data corresponding to aData and maintaining a meta-link
  //! between these meta-data and aFileName
  //! CreateMetaData is called by CreateData
  //! If the metadata-driver
  //! has version capabilities, version must be set in the returned Data.
  Standard_EXPORT virtual Handle(CDM_MetaData) CreateMetaData(
    const Handle(CDM_Document)&       aDocument,
    const TCollection_ExtendedString& aFileName) = 0;

  Standard_EXPORT virtual Standard_Boolean FindFolder(
    const TCollection_ExtendedString& aFolder) = 0;

  Standard_EXPORT virtual TCollection_ExtendedString DefaultFolder() = 0;

  Standard_EXPORT virtual Handle(PCDM_ReferenceIterator) ReferenceIterator(
    const Handle(Message_Messenger)& theMessageDriver);

  //! calls Find with an empty version
  Standard_EXPORT Standard_Boolean Find(const TCollection_ExtendedString& aFolder,
                                        const TCollection_ExtendedString& aName);

  //! calls MetaData with an empty version
  Standard_EXPORT Handle(CDM_MetaData) MetaData(const TCollection_ExtendedString& aFolder,
                                                const TCollection_ExtendedString& aName);

  DEFINE_STANDARD_RTTIEXT(CDF_MetaDataDriver, Standard_Transient)

protected:
  Standard_EXPORT CDF_MetaDataDriver();

private:
};

#endif // _CDF_MetaDataDriver_HeaderFile
