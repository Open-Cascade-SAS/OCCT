// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _StdStorage_HeaderData_HeaderFile
#define _StdStorage_HeaderData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <Storage_Error.hxx>
#include <Standard_Transient.hxx>
class Storage_BaseDriver;
class TCollection_AsciiString;
class TCollection_ExtendedString;

class StdStorage_HeaderData;
DEFINE_STANDARD_HANDLE(StdStorage_HeaderData, Standard_Transient)

//! Storage header data section that contains some
//! auxiliary information (application name, schema version,
//! creation date, comments and so on...)
class StdStorage_HeaderData 
  : public Standard_Transient
{
  friend class StdStorage_Data;

public:

  DEFINE_STANDARD_RTTIEXT(StdStorage_HeaderData, Standard_Transient)

  //! Reads the header data section from the container defined by theDriver. 
  //! Returns Standard_True in case of success. Otherwise, one need to get 
  //! an error code and description using ErrorStatus and ErrorStatusExtension
  //! functions correspondingly.
  Standard_EXPORT Standard_Boolean Read(Storage_BaseDriver& theDriver);

  //! Writes the header data section to the container defined by theDriver. 
  //! Returns Standard_True in case of success. Otherwise, one need to get 
  //! an error code and description using ErrorStatus and ErrorStatusExtension
  //! functions correspondingly.
  Standard_EXPORT Standard_Boolean Write(Storage_BaseDriver& theDriver);

  //! Return the creation date
  Standard_EXPORT TCollection_AsciiString CreationDate() const;

  //! Return the Storage package version
  Standard_EXPORT TCollection_AsciiString StorageVersion() const;

  //! Get the version of the schema
  Standard_EXPORT TCollection_AsciiString SchemaVersion() const;

  //! Set the version of the application
  Standard_EXPORT void SetApplicationVersion(const TCollection_AsciiString& aVersion);

  //! Get the version of the application
  Standard_EXPORT TCollection_AsciiString ApplicationVersion() const;

  //! Set the name of the application
  Standard_EXPORT void SetApplicationName(const TCollection_ExtendedString& aName);

  //! Get the name of the application
  Standard_EXPORT TCollection_ExtendedString ApplicationName() const;

  //! Set the data type
  Standard_EXPORT void SetDataType(const TCollection_ExtendedString& aType);

  //! Returns data type
  Standard_EXPORT TCollection_ExtendedString DataType() const;

  //! Add <theUserInfo> to the user informations
  Standard_EXPORT void AddToUserInfo(const TCollection_AsciiString& theUserInfo);

  //! Return the user informations
  Standard_EXPORT const TColStd_SequenceOfAsciiString& UserInfo() const;

  //! Add <theUserInfo> to the user informations
  Standard_EXPORT void AddToComments(const TCollection_ExtendedString& aComment);

  //! Return the user informations
  Standard_EXPORT const TColStd_SequenceOfExtendedString& Comments() const;

  //! Returns the number of persistent objects
  Standard_EXPORT Standard_Integer NumberOfObjects() const;

  //! Returns a status of the latest call to Read / Write functions
  Standard_EXPORT Storage_Error ErrorStatus() const;

  //! Returns an error message if any of the latest call to Read / Write functions
  Standard_EXPORT TCollection_AsciiString ErrorStatusExtension() const;

  //! Clears error status
  Standard_EXPORT void ClearErrorStatus();

  Standard_EXPORT void SetNumberOfObjects(const Standard_Integer anObjectNumber);

  Standard_EXPORT void SetStorageVersion(const TCollection_AsciiString& aVersion);

  Standard_EXPORT void SetCreationDate(const TCollection_AsciiString& aDate);

  Standard_EXPORT void SetSchemaVersion(const TCollection_AsciiString& aVersion);

  Standard_EXPORT void SetSchemaName(const TCollection_AsciiString& aName);

private:

  Standard_EXPORT StdStorage_HeaderData();

  Standard_EXPORT void SetErrorStatus(const Storage_Error anError);

  Standard_EXPORT void SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt);

  Standard_Integer                 myNBObj;
  TCollection_AsciiString          myStorageVersion;
  TCollection_AsciiString          mySchemaVersion;
  TCollection_AsciiString          mySchemaName;
  TCollection_AsciiString          myApplicationVersion;
  TCollection_ExtendedString       myApplicationName;
  TCollection_ExtendedString       myDataType;
  TCollection_AsciiString          myDate;
  TColStd_SequenceOfAsciiString    myUserInfo;
  TColStd_SequenceOfExtendedString myComments;
  Storage_Error                    myErrorStatus;
  TCollection_AsciiString          myErrorStatusExt;

};

#endif // _StdStorage_HeaderData_HeaderFile
