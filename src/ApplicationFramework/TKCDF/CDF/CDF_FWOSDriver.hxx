// Created on: 1997-01-22
// Created by: Mister rmi
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

#ifndef _CDF_FWOSDriver_HeaderFile
#define _CDF_FWOSDriver_HeaderFile

#include <Standard.hxx>

#include <CDF_MetaDataDriver.hxx>
#include <CDM_MetaDataLookUpTable.hxx>

class TCollection_ExtendedString;
class CDM_MetaData;
class CDM_Document;

class CDF_FWOSDriver : public CDF_MetaDataDriver
{

public:
  //! Initializes the MetaDatadriver connected to specified look-up table.
  //! Note that the created driver will keep reference to the table,
  //! thus it must have life time longer than this object.
  Standard_EXPORT CDF_FWOSDriver(
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<CDM_MetaData>>& theLookUpTable);

  //! indicate whether a file exists corresponding to the folder and the name
  Standard_EXPORT bool Find(const TCollection_ExtendedString& aFolder,
                            const TCollection_ExtendedString& aName,
                            const TCollection_ExtendedString& aVersion) override;

  Standard_EXPORT bool HasReadPermission(const TCollection_ExtendedString& aFolder,
                                         const TCollection_ExtendedString& aName,
                                         const TCollection_ExtendedString& aVersion) override;

  Standard_EXPORT bool FindFolder(const TCollection_ExtendedString& aFolder) override;

  Standard_EXPORT TCollection_ExtendedString DefaultFolder() override;

  Standard_EXPORT TCollection_ExtendedString
    BuildFileName(const occ::handle<CDM_Document>& aDocument) override;

  Standard_EXPORT TCollection_ExtendedString SetName(
    const occ::handle<CDM_Document>&  aDocument,
    const TCollection_ExtendedString& aName) override;

  DEFINE_STANDARD_RTTIEXT(CDF_FWOSDriver, CDF_MetaDataDriver)

private:
  Standard_EXPORT occ::handle<CDM_MetaData> MetaData(
    const TCollection_ExtendedString& aFolder,
    const TCollection_ExtendedString& aName,
    const TCollection_ExtendedString& aVersion) override;

  Standard_EXPORT occ::handle<CDM_MetaData> CreateMetaData(
    const occ::handle<CDM_Document>&  aDocument,
    const TCollection_ExtendedString& aFileName) override;

  Standard_EXPORT static TCollection_ExtendedString Concatenate(
    const TCollection_ExtendedString& aFolder,
    const TCollection_ExtendedString& aName);

private:
  NCollection_DataMap<TCollection_ExtendedString, occ::handle<CDM_MetaData>>* myLookUpTable;
};

#endif // _CDF_FWOSDriver_HeaderFile
