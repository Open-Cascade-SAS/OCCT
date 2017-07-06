// Copyright (c) 1998-1999 Matra Datavision
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


#include <Standard_ErrorHandler.hxx>
#include <Storage_HeaderData.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <Storage_StreamExtCharParityError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Storage_HeaderData,Standard_Transient)

Storage_HeaderData::Storage_HeaderData() : myNBObj(0), myErrorStatus(Storage_VSOk)
{
}

Standard_Boolean Storage_HeaderData::Read (Storage_BaseDriver& theDriver)
{
  // Check driver open mode
  if (theDriver.OpenMode() != Storage_VSRead
   && theDriver.OpenMode() != Storage_VSReadWrite)
  {
    myErrorStatus = Storage_VSModeError;
    myErrorStatusExt = "OpenMode";
    return Standard_False;
  }

  // Read info section
  myErrorStatus = theDriver.BeginReadInfoSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginReadInfoSection";
    return Standard_False;
  }

  {
    try
    {
      OCC_CATCH_SIGNALS
      theDriver.ReadInfo (myNBObj,
                          myStorageVersion,
                          myDate,
                          mySchemaName,
                          mySchemaVersion,
                          myApplicationName,
                          myApplicationVersion,
                          myDataType,
                          myUserInfo);
    }
    catch (Storage_StreamTypeMismatchError)
    {
      myErrorStatus = Storage_VSTypeMismatch;
      myErrorStatusExt = "ReadInfo";
      return Standard_False;
    }
    catch (Storage_StreamExtCharParityError)
    {
      myErrorStatus = Storage_VSExtCharParityError;
      myErrorStatusExt = "ReadInfo";
      return Standard_False;
    }
  }

  myErrorStatus = theDriver.EndReadInfoSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndReadInfoSection";
    return Standard_False;
  }

  // Read comment section
  myErrorStatus = theDriver.BeginReadCommentSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginReadCommentSection";
    return Standard_False;
  }

  {
    try
    {
      OCC_CATCH_SIGNALS
      theDriver.ReadComment (myComments);
    }
    catch (Storage_StreamTypeMismatchError)
    {
      myErrorStatus = Storage_VSTypeMismatch;
      myErrorStatusExt = "ReadComment";
      return Standard_False;
    }
    catch (Storage_StreamExtCharParityError)
    {
      myErrorStatus = Storage_VSExtCharParityError;
      myErrorStatusExt = "ReadComment";
      return Standard_False;
    }
  }

  myErrorStatus = theDriver.EndReadCommentSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndReadCommentSection";
    return Standard_False;
  }

  return Standard_True;
}

TCollection_AsciiString Storage_HeaderData::CreationDate() const
{
  return myDate;
}

void Storage_HeaderData::SetSchemaVersion(const TCollection_AsciiString& aVersion) 
{
  mySchemaVersion = aVersion;
}

TCollection_AsciiString Storage_HeaderData::SchemaVersion() const
{
  return mySchemaVersion;
}

void Storage_HeaderData::SetSchemaName(const TCollection_AsciiString& aSchemaName) 
{
  mySchemaName = aSchemaName;
}

TCollection_AsciiString Storage_HeaderData::SchemaName() const
{
  return mySchemaName;
}

void Storage_HeaderData::SetApplicationVersion(const TCollection_AsciiString& aVersion) 
{
  myApplicationVersion = aVersion;
}

TCollection_AsciiString Storage_HeaderData::ApplicationVersion() const
{
  return myApplicationVersion;
}

void Storage_HeaderData::SetApplicationName(const TCollection_ExtendedString& aName) 
{
  myApplicationName = aName;
}

TCollection_ExtendedString Storage_HeaderData::ApplicationName() const
{
  return myApplicationName;
}

void Storage_HeaderData::SetDataType(const TCollection_ExtendedString& aName) 
{
  myDataType = aName;
}

TCollection_ExtendedString Storage_HeaderData::DataType() const
{
  return myDataType;
}

void Storage_HeaderData::AddToUserInfo(const TCollection_AsciiString& theUserInfo) 
{
  myUserInfo.Append(theUserInfo);
}

const TColStd_SequenceOfAsciiString& Storage_HeaderData::UserInfo() const
{
  return myUserInfo;
}

void Storage_HeaderData::AddToComments(const TCollection_ExtendedString& aComments) 
{
  myComments.Append(aComments);
}

const TColStd_SequenceOfExtendedString& Storage_HeaderData::Comments() const
{
  return myComments;
}

Standard_Integer Storage_HeaderData::NumberOfObjects() const
{
  return myNBObj;
}

void Storage_HeaderData::SetNumberOfObjects(const Standard_Integer anObjectNumber) 
{
  myNBObj = anObjectNumber;
}

void Storage_HeaderData::SetStorageVersion(const TCollection_AsciiString& v)
{
  myStorageVersion = v;
}

void Storage_HeaderData::SetCreationDate(const TCollection_AsciiString& d)
{
  myDate = d;
}

TCollection_AsciiString Storage_HeaderData::StorageVersion() const
{
  return myStorageVersion;
}

Storage_Error  Storage_HeaderData::ErrorStatus() const
{
  return myErrorStatus;
}

void Storage_HeaderData::SetErrorStatus(const Storage_Error anError)
{
  myErrorStatus = anError;
}

TCollection_AsciiString Storage_HeaderData::ErrorStatusExtension() const
{
  return myErrorStatusExt;
}

void Storage_HeaderData::SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt)
{
  myErrorStatusExt = anErrorExt;
}

void Storage_HeaderData::ClearErrorStatus()
{
  myErrorStatus = Storage_VSOk;
  myErrorStatusExt.Clear();
}

