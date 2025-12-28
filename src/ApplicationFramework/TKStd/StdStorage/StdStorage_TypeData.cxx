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

bool StdStorage_TypeData::Read(const occ::handle<Storage_BaseDriver>& theDriver)
{
  // Check driver open mode
  if (theDriver->OpenMode() != Storage_VSRead && theDriver->OpenMode() != Storage_VSReadWrite)
  {
    myErrorStatus    = Storage_VSModeError;
    myErrorStatusExt = "OpenMode";
    return false;
  }

  // Read type section
  myErrorStatus = theDriver->BeginReadTypeSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginReadTypeSection";
    return false;
  }

  int                     aTypeNum;
  TCollection_AsciiString aTypeName;

  int len = theDriver->TypeSectionSize();
  for (int i = 1; i <= len; i++)
  {
    try
    {
      OCC_CATCH_SIGNALS
      theDriver->ReadTypeInformations(aTypeNum, aTypeName);
    }
    catch (Storage_StreamTypeMismatchError const&)
    {
      myErrorStatus    = Storage_VSTypeMismatch;
      myErrorStatusExt = "ReadTypeInformations";
      return false;
    }

    myPt.Add(aTypeName, aTypeNum);
  }

  myErrorStatus = theDriver->EndReadTypeSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndReadTypeSection";
    return false;
  }

  return true;
}

bool StdStorage_TypeData::Write(const occ::handle<Storage_BaseDriver>& theDriver)
{
  // Check driver open mode
  if (theDriver->OpenMode() != Storage_VSWrite && theDriver->OpenMode() != Storage_VSReadWrite)
  {
    myErrorStatus    = Storage_VSModeError;
    myErrorStatusExt = "OpenMode";
    return false;
  }

  // Write type section
  myErrorStatus = theDriver->BeginWriteTypeSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginWriteTypeSection";
    return false;
  }

  int len = NumberOfTypes();
  theDriver->SetTypeSectionSize(len);
  for (int i = 1; i <= len; i++)
  {
    try
    {
      OCC_CATCH_SIGNALS
      theDriver->WriteTypeInformations(i, Type(i));
    }
    catch (Storage_StreamTypeMismatchError const&)
    {
      myErrorStatus    = Storage_VSTypeMismatch;
      myErrorStatusExt = "WriteTypeInformations";
      return false;
    }
  }

  myErrorStatus = theDriver->EndWriteTypeSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndWriteTypeSection";
    return false;
  }

  return true;
}

int StdStorage_TypeData::NumberOfTypes() const
{
  return myPt.Extent();
}

bool StdStorage_TypeData::IsType(const TCollection_AsciiString& aName) const
{
  return myPt.Contains(aName);
}

occ::handle<NCollection_HSequence<TCollection_AsciiString>> StdStorage_TypeData::Types() const
{
  occ::handle<NCollection_HSequence<TCollection_AsciiString>> r =
    new NCollection_HSequence<TCollection_AsciiString>;
  int i;

  for (i = 1; i <= myPt.Extent(); i++)
  {
    r->Append(myPt.FindKey(i));
  }

  return r;
}

void StdStorage_TypeData::AddType(const TCollection_AsciiString& aTypeName, const int aTypeNum)
{
  myPt.Add(aTypeName, aTypeNum);
  myTypeId = std::max(aTypeNum, myTypeId);
}

int StdStorage_TypeData::AddType(const occ::handle<StdObjMgt_Persistent>& aPObj)
{
  TCollection_AsciiString aTypeName = aPObj->PName();
  if (IsType(aTypeName))
    return Type(aTypeName);

  if (!myMapOfPInst.IsBound(aTypeName))
  {
    Standard_SStream aSS;
    aSS << "StdStorage_TypeData::Type " << aTypeName << " isn't registered";
    throw Standard_NoSuchObject(aSS.str().c_str());
  }

  int aTypeId = ++myTypeId;
  AddType(aTypeName, aTypeId);

  return aTypeId;
}

TCollection_AsciiString StdStorage_TypeData::Type(const int aTypeNum) const
{
  TCollection_AsciiString r;

  if (aTypeNum <= myPt.Extent() && aTypeNum > 0)
    r = myPt.FindKey(aTypeNum);
  else
  {
    Standard_SStream aSS;
    aSS << "StdStorage_TypeData::Type " << aTypeNum << " not in range";
    throw Standard_NoSuchObject(aSS.str().c_str());
  }

  return r;
}

int StdStorage_TypeData::Type(const TCollection_AsciiString& aTypeName) const
{
  int r = 0;

  if (myPt.Contains(aTypeName))
    r = myPt.FindFromKey(aTypeName);
  else
  {
    Standard_SStream aSS;
    aSS << "StdStorage_TypeData::Type " << aTypeName << " not found";
    throw Standard_NoSuchObject(aSS.str().c_str());
  }

  return r;
}

StdObjMgt_Persistent::Instantiator StdStorage_TypeData::Instantiator(const int aTypeNum) const
{
  TCollection_AsciiString            aTypeName      = Type(aTypeNum);
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
