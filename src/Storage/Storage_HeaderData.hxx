// Created on: 1997-02-06
// Created by: Kernel
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

#ifndef _Storage_HeaderData_HeaderFile
#define _Storage_HeaderData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <Storage_Error.hxx>
#include <Standard_Transient.hxx>
class Storage_Schema;
class Storage_BaseDriver;
class TCollection_AsciiString;
class TCollection_ExtendedString;


class Storage_HeaderData;
DEFINE_STANDARD_HANDLE(Storage_HeaderData, Standard_Transient)


class Storage_HeaderData : public Standard_Transient
{

public:

  
  Standard_EXPORT Storage_HeaderData();

  Standard_EXPORT Standard_Boolean Read (Storage_BaseDriver& theDriver);
  
  //! return the creation date
  Standard_EXPORT TCollection_AsciiString CreationDate() const;
  
  //! return the Storage package version
  Standard_EXPORT TCollection_AsciiString StorageVersion() const;
  
  //! get the version of the schema
  Standard_EXPORT TCollection_AsciiString SchemaVersion() const;
  
  //! get the schema's name
  Standard_EXPORT TCollection_AsciiString SchemaName() const;
  
  //! set the version of the application
  Standard_EXPORT void SetApplicationVersion (const TCollection_AsciiString& aVersion);
  
  //! get the version of the application
  Standard_EXPORT TCollection_AsciiString ApplicationVersion() const;
  
  //! set the name of the application
  Standard_EXPORT void SetApplicationName (const TCollection_ExtendedString& aName);
  
  //! get the name of the application
  Standard_EXPORT TCollection_ExtendedString ApplicationName() const;
  
  //! set the data type
  Standard_EXPORT void SetDataType (const TCollection_ExtendedString& aType);
  
  //! returns data type
  Standard_EXPORT TCollection_ExtendedString DataType() const;
  
  //! add <theUserInfo> to the user informations
  Standard_EXPORT void AddToUserInfo (const TCollection_AsciiString& theUserInfo);
  
  //! return the user informations
  Standard_EXPORT const TColStd_SequenceOfAsciiString& UserInfo() const;
  
  //! add <theUserInfo> to the user informations
  Standard_EXPORT void AddToComments (const TCollection_ExtendedString& aComment);
  
  //! return the user informations
  Standard_EXPORT const TColStd_SequenceOfExtendedString& Comments() const;
  
  //! the the number of persistent objects
  //! Return:
  //! the number of persistent objects readed
  Standard_EXPORT Standard_Integer NumberOfObjects() const;
  
  Standard_EXPORT Storage_Error ErrorStatus() const;
  
  Standard_EXPORT TCollection_AsciiString ErrorStatusExtension() const;
  
  Standard_EXPORT void ClearErrorStatus();


friend class Storage_Schema;


  DEFINE_STANDARD_RTTIEXT(Storage_HeaderData,Standard_Transient)

public:
  
  Standard_EXPORT void SetNumberOfObjects (const Standard_Integer anObjectNumber);
  
  Standard_EXPORT void SetStorageVersion (const TCollection_AsciiString& aVersion);
  
  Standard_EXPORT void SetCreationDate (const TCollection_AsciiString& aDate);
  
  Standard_EXPORT void SetSchemaVersion (const TCollection_AsciiString& aVersion);
  
  Standard_EXPORT void SetSchemaName (const TCollection_AsciiString& aName);

private:
 
  Standard_EXPORT void SetErrorStatus (const Storage_Error anError);
  
  Standard_EXPORT void SetErrorStatusExtension (const TCollection_AsciiString& anErrorExt);

  Standard_Integer myNBObj;
  TCollection_AsciiString myStorageVersion;
  TCollection_AsciiString mySchemaVersion;
  TCollection_AsciiString mySchemaName;
  TCollection_AsciiString myApplicationVersion;
  TCollection_ExtendedString myApplicationName;
  TCollection_ExtendedString myDataType;
  TCollection_AsciiString myDate;
  TColStd_SequenceOfAsciiString myUserInfo;
  TColStd_SequenceOfExtendedString myComments;
  Storage_Error myErrorStatus;
  TCollection_AsciiString myErrorStatusExt;


};







#endif // _Storage_HeaderData_HeaderFile
