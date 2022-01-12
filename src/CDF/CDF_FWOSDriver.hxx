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


class CDF_FWOSDriver;
DEFINE_STANDARD_HANDLE(CDF_FWOSDriver, CDF_MetaDataDriver)


class CDF_FWOSDriver : public CDF_MetaDataDriver
{

public:

  //! Initializes the MetaDatadriver connected to specified look-up table.
  //! Note that the created driver will keep reference to the table,
  //! thus it must have life time longer than this object.
  Standard_EXPORT CDF_FWOSDriver(CDM_MetaDataLookUpTable& theLookUpTable);
  
  //! indicate whether a file exists corresponding to the folder and the name
  Standard_EXPORT Standard_Boolean Find (const TCollection_ExtendedString& aFolder, const TCollection_ExtendedString& aName, const TCollection_ExtendedString& aVersion) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean HasReadPermission (const TCollection_ExtendedString& aFolder, const TCollection_ExtendedString& aName, const TCollection_ExtendedString& aVersion) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean FindFolder (const TCollection_ExtendedString& aFolder) Standard_OVERRIDE;
  
  Standard_EXPORT TCollection_ExtendedString DefaultFolder() Standard_OVERRIDE;
  
  Standard_EXPORT TCollection_ExtendedString BuildFileName (const Handle(CDM_Document)& aDocument) Standard_OVERRIDE;
  
  Standard_EXPORT virtual TCollection_ExtendedString SetName (const Handle(CDM_Document)& aDocument, const TCollection_ExtendedString& aName) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(CDF_FWOSDriver,CDF_MetaDataDriver)

private:

  Standard_EXPORT Handle(CDM_MetaData) MetaData (const TCollection_ExtendedString& aFolder, const TCollection_ExtendedString& aName, const TCollection_ExtendedString& aVersion) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(CDM_MetaData) CreateMetaData (const Handle(CDM_Document)& aDocument, const TCollection_ExtendedString& aFileName) Standard_OVERRIDE;
  
  Standard_EXPORT static TCollection_ExtendedString Concatenate (const TCollection_ExtendedString& aFolder, const TCollection_ExtendedString& aName);
  
private:
  CDM_MetaDataLookUpTable* myLookUpTable;
};

#endif // _CDF_FWOSDriver_HeaderFile
