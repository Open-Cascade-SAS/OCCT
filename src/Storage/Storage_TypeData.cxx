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
#include <Standard_NoSuchObject.hxx>
#include <Storage_TypeData.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Storage_TypeData,Standard_Transient)

Storage_TypeData::Storage_TypeData() : myErrorStatus(Storage_VSOk)
{
}

Standard_Boolean Storage_TypeData::Read (Storage_BaseDriver& theDriver)
{
  // Check driver open mode
  if (theDriver.OpenMode() != Storage_VSRead
   && theDriver.OpenMode() != Storage_VSReadWrite)
  {
    myErrorStatus = Storage_VSModeError;
    myErrorStatusExt = "OpenMode";
    return Standard_False;
  }

  // Read type section
  myErrorStatus = theDriver.BeginReadTypeSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginReadTypeSection";
    return Standard_False;
  }

  Standard_Integer        aTypeNum;
  TCollection_AsciiString aTypeName;

  Standard_Integer len = theDriver.TypeSectionSize();
  for (Standard_Integer i = 1; i <= len; i++)
  {
    try
    {
      OCC_CATCH_SIGNALS
      theDriver.ReadTypeInformations (aTypeNum, aTypeName);
    }
    catch (Storage_StreamTypeMismatchError)
    {
      myErrorStatus = Storage_VSTypeMismatch;
      myErrorStatusExt = "ReadTypeInformations";
      return Standard_False;
    }

    myPt.Add (aTypeName, aTypeNum);
  }

  myErrorStatus = theDriver.EndReadTypeSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndReadTypeSection";
    return Standard_False;
  }

  return Standard_True;
}

Standard_Integer Storage_TypeData::NumberOfTypes() const
{
  return myPt.Extent();
}

Standard_Boolean Storage_TypeData::IsType(const TCollection_AsciiString& aName) const
{
  return myPt.Contains(aName);
}

Handle(TColStd_HSequenceOfAsciiString) Storage_TypeData::Types() const
{
  Handle(TColStd_HSequenceOfAsciiString) r = new TColStd_HSequenceOfAsciiString;
  Standard_Integer                       i;

  for (i = 1; i <= myPt.Extent(); i++) {
    r->Append(myPt.FindKey(i));
  }

  return r;
}

void Storage_TypeData::AddType(const TCollection_AsciiString& aName,const Standard_Integer aTypeNum) 
{
  myPt.Add(aName,aTypeNum);
}

TCollection_AsciiString Storage_TypeData::Type(const Standard_Integer aTypeNum) const
{
  TCollection_AsciiString r;

  if (aTypeNum <= myPt.Extent() && aTypeNum > 0) {
    r = myPt.FindKey(aTypeNum);
  }
  else {
    throw Standard_NoSuchObject("Storage_TypeData::Type - aTypeNum not in range");
  }

  return r;
}

Standard_Integer Storage_TypeData::Type(const TCollection_AsciiString& aTypeName) const
{
  Standard_Integer r = 0;

  if (myPt.Contains(aTypeName)) {
    r = myPt.FindFromKey(aTypeName);
  }
  else {
    throw Standard_NoSuchObject("Storage_TypeData::Type - aTypeName not found");
  }

  return r;
}

void Storage_TypeData::Clear()
{
  myPt.Clear();
}

Storage_Error  Storage_TypeData::ErrorStatus() const
{
  return myErrorStatus;
}

void Storage_TypeData::SetErrorStatus(const Storage_Error anError)
{
  myErrorStatus = anError;
}

void Storage_TypeData::ClearErrorStatus()
{
  myErrorStatus = Storage_VSOk;
  myErrorStatusExt.Clear();
}

TCollection_AsciiString Storage_TypeData::ErrorStatusExtension() const
{
  return myErrorStatusExt;
}

void Storage_TypeData::SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt)
{
  myErrorStatusExt = anErrorExt;
}
