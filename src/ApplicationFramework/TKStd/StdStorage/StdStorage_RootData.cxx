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

#include <StdObjMgt_Persistent.hxx>
#include <Standard_ErrorHandler.hxx>
#include <StdStorage_RootData.hxx>
#include <StdStorage_Root.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StdStorage_RootData, Standard_Transient)

StdStorage_RootData::StdStorage_RootData()
    : myErrorStatus(Storage_VSOk)
{
}

bool StdStorage_RootData::Read(const occ::handle<Storage_BaseDriver>& theDriver)
{
  // Check driver open mode
  if (theDriver->OpenMode() != Storage_VSRead && theDriver->OpenMode() != Storage_VSReadWrite)
  {
    myErrorStatus    = Storage_VSModeError;
    myErrorStatusExt = "OpenMode";
    return false;
  }

  // Read root section
  myErrorStatus = theDriver->BeginReadRootSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginReadRootSection";
    return false;
  }

  TCollection_AsciiString aRootName, aTypeName;
  int                     aRef;

  int len = theDriver->RootSectionSize();
  for (int i = 1; i <= len; i++)
  {
    try
    {
      OCC_CATCH_SIGNALS
      theDriver->ReadRoot(aRootName, aRef, aTypeName);
    }
    catch (Storage_StreamTypeMismatchError const&)
    {
      myErrorStatus    = Storage_VSTypeMismatch;
      myErrorStatusExt = "ReadRoot";
      return false;
    }

    occ::handle<StdStorage_Root> aRoot = new StdStorage_Root(aRootName, aRef, aTypeName);
    myObjects.Add(aRootName, aRoot);
  }

  myErrorStatus = theDriver->EndReadRootSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndReadRootSection";
    return false;
  }

  return true;
}

bool StdStorage_RootData::Write(const occ::handle<Storage_BaseDriver>& theDriver)
{
  // Check driver open mode
  if (theDriver->OpenMode() != Storage_VSWrite && theDriver->OpenMode() != Storage_VSReadWrite)
  {
    myErrorStatus    = Storage_VSModeError;
    myErrorStatusExt = "OpenMode";
    return false;
  }

  // Write root section
  myErrorStatus = theDriver->BeginWriteRootSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginWriteRootSection";
    return false;
  }

  theDriver->SetRootSectionSize(NumberOfRoots());
  for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<StdStorage_Root>>::Iterator
         anIt(myObjects);
       anIt.More();
       anIt.Next())
  {
    const occ::handle<StdStorage_Root>& aRoot = anIt.Value();
    try
    {
      OCC_CATCH_SIGNALS
      theDriver->WriteRoot(aRoot->Name(), aRoot->Reference(), aRoot->Type());
    }
    catch (Storage_StreamTypeMismatchError const&)
    {
      myErrorStatus    = Storage_VSTypeMismatch;
      myErrorStatusExt = "ReadRoot";
      return false;
    }
  }

  myErrorStatus = theDriver->EndWriteRootSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndWriteRootSection";
    return false;
  }

  return true;
}

int StdStorage_RootData::NumberOfRoots() const
{
  return myObjects.Extent();
}

void StdStorage_RootData::AddRoot(const occ::handle<StdStorage_Root>& aRoot)
{
  myObjects.Add(aRoot->Name(), aRoot);
  aRoot->myRef = myObjects.Size();
}

occ::handle<NCollection_HSequence<occ::handle<StdStorage_Root>>> StdStorage_RootData::Roots() const
{
  occ::handle<NCollection_HSequence<occ::handle<StdStorage_Root>>> anObjectsSeq =
    new NCollection_HSequence<occ::handle<StdStorage_Root>>;
  NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<StdStorage_Root>>::Iterator it(
    myObjects);

  for (; it.More(); it.Next())
  {
    anObjectsSeq->Append(it.Value());
  }

  return anObjectsSeq;
}

occ::handle<StdStorage_Root> StdStorage_RootData::Find(const TCollection_AsciiString& aName) const
{
  const occ::handle<StdStorage_Root>* pRoot = myObjects.Seek(aName);
  return pRoot ? *pRoot : occ::handle<StdStorage_Root>();
}

bool StdStorage_RootData::IsRoot(const TCollection_AsciiString& aName) const
{
  return myObjects.Contains(aName);
}

void StdStorage_RootData::RemoveRoot(const TCollection_AsciiString& aName)
{
  occ::handle<StdStorage_Root>* pRoot = myObjects.ChangeSeek(aName);
  if (pRoot)
  {
    (*pRoot)->myRef = 0;
    myObjects.RemoveKey(aName);
    int aRef = 1;
    for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<StdStorage_Root>>::Iterator
           anIt(myObjects);
         anIt.More();
         anIt.Next(), ++aRef)
      anIt.ChangeValue()->myRef = aRef;
  }
}

void StdStorage_RootData::Clear()
{
  for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<StdStorage_Root>>::Iterator
         anIt(myObjects);
       anIt.More();
       anIt.Next())
    anIt.ChangeValue()->myRef = 0;

  myObjects.Clear();
}

Storage_Error StdStorage_RootData::ErrorStatus() const
{
  return myErrorStatus;
}

void StdStorage_RootData::SetErrorStatus(const Storage_Error anError)
{
  myErrorStatus = anError;
}

void StdStorage_RootData::ClearErrorStatus()
{
  myErrorStatus = Storage_VSOk;
  myErrorStatusExt.Clear();
}

TCollection_AsciiString StdStorage_RootData::ErrorStatusExtension() const
{
  return myErrorStatusExt;
}

void StdStorage_RootData::SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt)
{
  myErrorStatusExt = anErrorExt;
}
