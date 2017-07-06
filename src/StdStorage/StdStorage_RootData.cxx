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
#include <Standard_NoSuchObject.hxx>
#include <StdStorage_RootData.hxx>
#include <StdStorage_Root.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <Storage_DataMapIteratorOfMapOfPers.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StdStorage_RootData, Standard_Transient)

StdStorage_RootData::StdStorage_RootData() 
  : myErrorStatus(Storage_VSOk)
{
}

Standard_Boolean StdStorage_RootData::Read(Storage_BaseDriver& theDriver)
{
  // Check driver open mode
  if (theDriver.OpenMode() != Storage_VSRead
    && theDriver.OpenMode() != Storage_VSReadWrite)
  {
    myErrorStatus = Storage_VSModeError;
    myErrorStatusExt = "OpenMode";
    return Standard_False;
  }

  // Read root section
  myErrorStatus = theDriver.BeginReadRootSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginReadRootSection";
    return Standard_False;
  }

  TCollection_AsciiString aRootName, aTypeName;
  Standard_Integer aRef;

  Standard_Integer len = theDriver.RootSectionSize();
  for (Standard_Integer i = 1; i <= len; i++)
  {
    try
    {
      OCC_CATCH_SIGNALS
      theDriver.ReadRoot(aRootName, aRef, aTypeName);
    }
    catch (Storage_StreamTypeMismatchError)
    {
      myErrorStatus = Storage_VSTypeMismatch;
      myErrorStatusExt = "ReadRoot";
      return Standard_False;
    }

    Handle(StdStorage_Root) aRoot = new StdStorage_Root(aRootName, aRef, aTypeName);
    myObjects.Bind(aRootName, aRoot);
  }

  myErrorStatus = theDriver.EndReadRootSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndReadRootSection";
    return Standard_False;
  }

  return Standard_True;
}

Standard_Boolean StdStorage_RootData::Write(Storage_BaseDriver& theDriver)
{
  // Check driver open mode
  if (theDriver.OpenMode() != Storage_VSWrite
    && theDriver.OpenMode() != Storage_VSReadWrite)
  {
    myErrorStatus = Storage_VSModeError;
    myErrorStatusExt = "OpenMode";
    return Standard_False;
  }

  // Write root section
  myErrorStatus = theDriver.BeginWriteRootSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "BeginWriteRootSection";
    return Standard_False;
  }

  theDriver.SetRootSectionSize(NumberOfRoots());
  for (StdStorage_MapOfRoots::Iterator anIt(myObjects); anIt.More(); anIt.Next())
  {
    const Handle(StdStorage_Root)& aRoot = anIt.Value();
    try
    {
      OCC_CATCH_SIGNALS
      theDriver.WriteRoot(aRoot->Name(), aRoot->Reference(), aRoot->Type());
    }
    catch (Storage_StreamTypeMismatchError)
    {
      myErrorStatus = Storage_VSTypeMismatch;
      myErrorStatusExt = "ReadRoot";
      return Standard_False;
    }
  }

  myErrorStatus = theDriver.EndWriteRootSection();
  if (myErrorStatus != Storage_VSOk)
  {
    myErrorStatusExt = "EndWriteRootSection";
    return Standard_False;
  }

  return Standard_True;
}

Standard_Integer StdStorage_RootData::NumberOfRoots() const
{
  return myObjects.Extent();
}

void StdStorage_RootData::AddRoot(const Handle(StdStorage_Root)& aRoot)
{
  myObjects.Bind(aRoot->Name(), aRoot);
  aRoot->myRef = myObjects.Size();
}

Handle(StdStorage_HSequenceOfRoots) StdStorage_RootData::Roots() const
{
  Handle(StdStorage_HSequenceOfRoots)    anObjectsSeq = new StdStorage_HSequenceOfRoots;
  StdStorage_DataMapIteratorOfMapOfRoots it(myObjects);

  for (; it.More(); it.Next()) {
    anObjectsSeq->Append(it.Value());
  }

  return anObjectsSeq;
}

Handle(StdStorage_Root) StdStorage_RootData::Find(const TCollection_AsciiString& aName) const
{
  Handle(StdStorage_Root) p;

  if (myObjects.IsBound(aName)) {
    p = myObjects.Find(aName);
  }

  return p;
}

Standard_Boolean StdStorage_RootData::IsRoot(const TCollection_AsciiString& aName) const
{
  return myObjects.IsBound(aName);
}

void StdStorage_RootData::RemoveRoot(const TCollection_AsciiString& aName)
{
  if (myObjects.IsBound(aName)) {
    myObjects.ChangeFind(aName)->myRef = 0;
    myObjects.UnBind(aName);
    Standard_Integer aRef = 1;
    for (StdStorage_MapOfRoots::Iterator anIt(myObjects); anIt.More(); anIt.Next(), ++aRef)
      anIt.ChangeValue()->myRef = aRef;
  }
}

void StdStorage_RootData::Clear()
{
  for (StdStorage_MapOfRoots::Iterator anIt(myObjects); anIt.More(); anIt.Next())
    anIt.ChangeValue()->myRef = 0;

  myObjects.Clear();
}

Storage_Error  StdStorage_RootData::ErrorStatus() const
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
