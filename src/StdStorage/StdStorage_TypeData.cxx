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
#include <Standard_NoSuchObject.hxx>
#include <StdDrivers.hxx>
#include <StdStorage_TypeData.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StdStorage_TypeData, Standard_Transient)

StdStorage_TypeData::StdStorage_TypeData() 
: myTypeId(0),
  myErrorStatus(Storage_VSOk)
{
  StdDrivers::BindTypes(myMapOfPInst);
}

Standard_Boolean StdStorage_TypeData::Read(Storage_BaseDriver& theDriver)
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

Standard_Boolean StdStorage_TypeData::Write(Storage_BaseDriver& theDriver)
{
  // Check driver open mode
  if (theDriver.OpenMode() != Storage_VSWrite
    && theDriver.OpenMode() != Storage_VSReadWrite)
  {
    myErrorStatus = Storage_VSModeError;
    myErrorStatusExt = "OpenMode";
    return Standard_False;
  }

  // Write type section
  myErrorStatus = theDriver.BeginWriteTypeSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginWriteTypeSection";
    return Standard_False;
  }

  Standard_Integer len = NumberOfTypes();
  theDriver.SetTypeSectionSize(len);
  for (Standard_Integer i = 1; i <= len; i++)
  {
    try
    {
      OCC_CATCH_SIGNALS
      theDriver.WriteTypeInformations(i, Type(i));
    }
    catch (Storage_StreamTypeMismatchError)
    {
      myErrorStatus = Storage_VSTypeMismatch;
      myErrorStatusExt = "WriteTypeInformations";
      return Standard_False;
    }
  }

  myErrorStatus = theDriver.EndWriteTypeSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndWriteTypeSection";
    return Standard_False;
  }

  return Standard_True;
}

Standard_Integer StdStorage_TypeData::NumberOfTypes() const
{
  return myPt.Extent();
}

Standard_Boolean StdStorage_TypeData::IsType(const TCollection_AsciiString& aName) const
{
  return myPt.Contains(aName);
}

Handle(TColStd_HSequenceOfAsciiString) StdStorage_TypeData::Types() const
{
  Handle(TColStd_HSequenceOfAsciiString) r = new TColStd_HSequenceOfAsciiString;
  Standard_Integer                       i;

  for (i = 1; i <= myPt.Extent(); i++) {
    r->Append(myPt.FindKey(i));
  }

  return r;
}

void StdStorage_TypeData::AddType(const TCollection_AsciiString& aTypeName, const Standard_Integer aTypeNum)
{
  myPt.Add(aTypeName, aTypeNum);
  myTypeId = Max(aTypeNum, myTypeId);
}

Standard_Integer StdStorage_TypeData::AddType(const Handle(StdObjMgt_Persistent)& aPObj)
{
  TCollection_AsciiString aTypeName = aPObj->PName();
  if (IsType(aTypeName))
    return Type(aTypeName);

  if (!myMapOfPInst.IsBound(aTypeName)) {
    Standard_SStream aSS;
    aSS << "StdStorage_TypeData::Type " << aTypeName << " isn't registered";
    throw Standard_NoSuchObject(aSS.str().c_str());
  }

  Standard_Integer aTypeId = ++myTypeId;
  AddType(aTypeName, aTypeId);

  return aTypeId;
}

TCollection_AsciiString StdStorage_TypeData::Type(const Standard_Integer aTypeNum) const
{
  TCollection_AsciiString r;

  if (aTypeNum <= myPt.Extent() && aTypeNum > 0)
    r = myPt.FindKey(aTypeNum);
  else {
    Standard_SStream aSS;
    aSS << "StdStorage_TypeData::Type " << aTypeNum << " not in range";
    throw Standard_NoSuchObject(aSS.str().c_str());
  }

  return r;
}

Standard_Integer StdStorage_TypeData::Type(const TCollection_AsciiString& aTypeName) const
{
  Standard_Integer r = 0;

  if (myPt.Contains(aTypeName))
    r = myPt.FindFromKey(aTypeName);
  else {
    Standard_SStream aSS;
    aSS << "StdStorage_TypeData::Type " << aTypeName << " not found";
    throw Standard_NoSuchObject(aSS.str().c_str());
  }

  return r;
}

StdObjMgt_Persistent::Instantiator 
StdStorage_TypeData::Instantiator(const Standard_Integer aTypeNum) const
{
  TCollection_AsciiString aTypeName = Type(aTypeNum);
  StdObjMgt_Persistent::Instantiator anInstantiator = 0;
  if (!myMapOfPInst.Find(aTypeName, anInstantiator))
    return 0;
  return anInstantiator;
}

void StdStorage_TypeData::Clear()
{
  myPt.Clear();
}

Storage_Error StdStorage_TypeData::ErrorStatus() const
{
  return myErrorStatus;
}

void StdStorage_TypeData::SetErrorStatus(const Storage_Error anError)
{
  myErrorStatus = anError;
}

void StdStorage_TypeData::ClearErrorStatus()
{
  myErrorStatus = Storage_VSOk;
  myErrorStatusExt.Clear();
}

TCollection_AsciiString StdStorage_TypeData::ErrorStatusExtension() const
{
  return myErrorStatusExt;
}

void StdStorage_TypeData::SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt)
{
  myErrorStatusExt = anErrorExt;
}
