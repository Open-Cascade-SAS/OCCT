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

#include <Standard_ErrorHandler.hxx>
#include <StdStorage_HeaderData.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <Storage_StreamExtCharParityError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StdStorage_HeaderData, Standard_Transient)

StdStorage_HeaderData::StdStorage_HeaderData()
  : myNBObj(0), myErrorStatus(Storage_VSOk)
{
}

Standard_Boolean StdStorage_HeaderData::Read(Storage_BaseDriver& theDriver)
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

  try
  {
    OCC_CATCH_SIGNALS
    theDriver.ReadInfo(myNBObj,
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

  try
  {
    OCC_CATCH_SIGNALS
    theDriver.ReadComment(myComments);
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

  myErrorStatus = theDriver.EndReadCommentSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndReadCommentSection";
    return Standard_False;
  }

  return Standard_True;
}

Standard_Boolean StdStorage_HeaderData::Write(Storage_BaseDriver& theDriver)
{
  // Check driver open mode
  if (theDriver.OpenMode() != Storage_VSWrite
    && theDriver.OpenMode() != Storage_VSReadWrite)
  {
    myErrorStatus = Storage_VSModeError;
    myErrorStatusExt = "OpenMode";
    return Standard_False;
  }

  // Write info section
  myErrorStatus = theDriver.BeginWriteInfoSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginWriteInfoSection";
    return Standard_False;
  }

  try
  {
    OCC_CATCH_SIGNALS
    theDriver.WriteInfo(myNBObj,
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
    myErrorStatusExt = "WriteInfo";
    return Standard_False;
  }
  catch (Storage_StreamExtCharParityError)
  {
    myErrorStatus = Storage_VSExtCharParityError;
    myErrorStatusExt = "WriteInfo";
    return Standard_False;
  }

  myErrorStatus = theDriver.EndWriteInfoSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndWriteInfoSection";
    return Standard_False;
  }

  // Write comment section
  myErrorStatus = theDriver.BeginWriteCommentSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginWriteCommentSection";
    return Standard_False;
  }

  try
  {
    OCC_CATCH_SIGNALS
    theDriver.WriteComment(myComments);
  }
  catch (Storage_StreamTypeMismatchError)
  {
    myErrorStatus = Storage_VSTypeMismatch;
    myErrorStatusExt = "WriteComment";
    return Standard_False;
  }
  catch (Storage_StreamExtCharParityError)
  {
    myErrorStatus = Storage_VSExtCharParityError;
    myErrorStatusExt = "WriteComment";
    return Standard_False;
  }

  myErrorStatus = theDriver.EndWriteCommentSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndWriteCommentSection";
    return Standard_False;
  }

  return Standard_True;
}

TCollection_AsciiString StdStorage_HeaderData::CreationDate() const
{
  return myDate;
}

void StdStorage_HeaderData::SetSchemaVersion(const TCollection_AsciiString& aVersion)
{
  mySchemaVersion = aVersion;
}

TCollection_AsciiString StdStorage_HeaderData::SchemaVersion() const
{
  return mySchemaVersion;
}

void StdStorage_HeaderData::SetSchemaName(const TCollection_AsciiString& aSchemaName)
{
  mySchemaName = aSchemaName;
}

void StdStorage_HeaderData::SetApplicationVersion(const TCollection_AsciiString& aVersion)
{
  myApplicationVersion = aVersion;
}

TCollection_AsciiString StdStorage_HeaderData::ApplicationVersion() const
{
  return myApplicationVersion;
}

void StdStorage_HeaderData::SetApplicationName(const TCollection_ExtendedString& aName)
{
  myApplicationName = aName;
}

TCollection_ExtendedString StdStorage_HeaderData::ApplicationName() const
{
  return myApplicationName;
}

void StdStorage_HeaderData::SetDataType(const TCollection_ExtendedString& aName)
{
  myDataType = aName;
}

TCollection_ExtendedString StdStorage_HeaderData::DataType() const
{
  return myDataType;
}

void StdStorage_HeaderData::AddToUserInfo(const TCollection_AsciiString& theUserInfo)
{
  myUserInfo.Append(theUserInfo);
}

const TColStd_SequenceOfAsciiString& StdStorage_HeaderData::UserInfo() const
{
  return myUserInfo;
}

void StdStorage_HeaderData::AddToComments(const TCollection_ExtendedString& aComments)
{
  myComments.Append(aComments);
}

const TColStd_SequenceOfExtendedString& StdStorage_HeaderData::Comments() const
{
  return myComments;
}

Standard_Integer StdStorage_HeaderData::NumberOfObjects() const
{
  return myNBObj;
}

void StdStorage_HeaderData::SetNumberOfObjects(const Standard_Integer anObjectNumber)
{
  myNBObj = anObjectNumber;
}

void StdStorage_HeaderData::SetStorageVersion(const TCollection_AsciiString& v)
{
  myStorageVersion = v;
}

void StdStorage_HeaderData::SetCreationDate(const TCollection_AsciiString& d)
{
  myDate = d;
}

TCollection_AsciiString StdStorage_HeaderData::StorageVersion() const
{
  return myStorageVersion;
}

Storage_Error StdStorage_HeaderData::ErrorStatus() const
{
  return myErrorStatus;
}

void StdStorage_HeaderData::SetErrorStatus(const Storage_Error anError)
{
  myErrorStatus = anError;
}

TCollection_AsciiString StdStorage_HeaderData::ErrorStatusExtension() const
{
  return myErrorStatusExt;
}

void StdStorage_HeaderData::SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt)
{
  myErrorStatusExt = anErrorExt;
}

void StdStorage_HeaderData::ClearErrorStatus()
{
  myErrorStatus = Storage_VSOk;
  myErrorStatusExt.Clear();
}
