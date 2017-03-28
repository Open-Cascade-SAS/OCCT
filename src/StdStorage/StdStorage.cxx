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

#include <NCollection_Handle.hxx>
#include <NCollection_Array1.hxx>
#include <PCDM.hxx>
#include <PCDM_BaseDriverPointer.hxx>
#include <PCDM_ReadWriter.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NullObject.hxx>
#include <StdObjMgt_Persistent.hxx>
#include <StdObjMgt_ReadData.hxx>
#include <StdObjMgt_WriteData.hxx>
#include <StdStorage.hxx>
#include <StdStorage_Data.hxx>
#include <StdStorage_HeaderData.hxx>
#include <StdStorage_TypeData.hxx>
#include <StdStorage_RootData.hxx>
#include <StdStorage_HSequenceOfRoots.hxx>
#include <StdStorage_BacketOfPersistent.hxx>
#include <Storage.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <Storage_StreamFormatError.hxx>
#include <Storage_StreamWriteError.hxx>

#include <locale.h>
#include <stdio.h>

//=======================================================================
// StdStorage::Version
//=======================================================================
TCollection_AsciiString StdStorage::Version()
{
  TCollection_AsciiString v("1.3");
  return v;
}

//=======================================================================
// StdStorage::Read
// Reads data from a file
//=======================================================================
Storage_Error StdStorage::Read(const TCollection_AsciiString& theFileName, 
                               Handle(StdStorage_Data)&       theData)
{
  // Create a driver appropriate for the given file
  PCDM_BaseDriverPointer aDriverPtr;
  if (PCDM::FileDriverType(theFileName, aDriverPtr) == PCDM_TOFD_Unknown)
    return Storage_VSWrongFileDriver;

  NCollection_Handle<Storage_BaseDriver> aDriver(aDriverPtr);

  // Try to open the file
  try
  {
    OCC_CATCH_SIGNALS
    PCDM_ReadWriter::Open(*aDriver, theFileName, Storage_VSRead);
  }
  catch (Standard_Failure)
  {
    return Storage_VSOpenError;
  }

  return Read(*aDriver, theData);
}

//=======================================================================
// StdStorage::Read
// Reads data from a pre-opened for reading driver
//=======================================================================
Storage_Error StdStorage::Read(Storage_BaseDriver&      theDriver, 
                               Handle(StdStorage_Data)& theData)
{
  if (theData.IsNull())
    theData = new StdStorage_Data;
  else
    theData->Clear();

  Handle(StdStorage_HeaderData) aHeaderData = theData->HeaderData();
  Handle(StdStorage_TypeData)   aTypeData   = theData->TypeData();
  Handle(StdStorage_RootData)   aRootData   = theData->RootData();

  // Read header section
  if (!aHeaderData->Read(theDriver))
    return aHeaderData->ErrorStatus();

  // Read types section
  if (!aTypeData->Read(theDriver))
    return aTypeData->ErrorStatus();

  // Select instantiators for the used types
  NCollection_Array1<StdObjMgt_Persistent::Instantiator>
    anInstantiators(1, aTypeData->NumberOfTypes());
  for (Standard_Integer i = 1; i <= aTypeData->NumberOfTypes(); i++)
  {
    StdObjMgt_Persistent::Instantiator anInstantiator = 
      aTypeData->Instantiator(i);
    if (anInstantiator)
      anInstantiators(i) = anInstantiator;
    else
      return Storage_VSUnknownType;
  }

  // Read root section
  if (!aRootData->Read(theDriver))
    return aRootData->ErrorStatus();

  Storage_Error anError;

  // Read and parse reference section
  StdObjMgt_ReadData aReadData(theDriver, aHeaderData->NumberOfObjects());

  anError = theDriver.BeginReadRefSection();
  if (anError != Storage_VSOk)
    return anError;

  Standard_Integer aNbRefs = theDriver.RefSectionSize();
  for (Standard_Integer i = 1; i <= aNbRefs; i++)
  {
    Standard_Integer aRef = 0, aType = 0;
    try
    {
      OCC_CATCH_SIGNALS
      theDriver.ReadReferenceType(aRef, aType);
      anError = Storage_VSOk;
    }
    catch (Storage_StreamTypeMismatchError)
    {
      anError = Storage_VSTypeMismatch;
    }

    if (anError != Storage_VSOk)
      return anError;

    aReadData.CreatePersistentObject(aRef, anInstantiators(aType));
  }

  anError = theDriver.EndReadRefSection();
  if (anError != Storage_VSOk)
    return anError;

  // Read and parse data section
  anError = theDriver.BeginReadDataSection();
  if (anError != Storage_VSOk)
    return anError;

  for (Standard_Integer i = 1; i <= aHeaderData->NumberOfObjects(); i++)
  {
    try
    {
      OCC_CATCH_SIGNALS
      aReadData.ReadPersistentObject(i);
      anError = Storage_VSOk;
    }
    catch (Storage_StreamTypeMismatchError) { anError = Storage_VSTypeMismatch; }
    catch (Storage_StreamFormatError) { anError = Storage_VSFormatError; }
    catch (Storage_StreamReadError) { anError = Storage_VSFormatError; }

    if (anError != Storage_VSOk)
      return anError;
  }

  anError = theDriver.EndReadDataSection();
  if (anError != Storage_VSOk)
    return anError;

  Handle(StdStorage_HSequenceOfRoots) aRoots = aRootData->Roots();
  if (!aRoots.IsNull())
  {
    for (StdStorage_HSequenceOfRoots::Iterator anIt(*aRoots); anIt.More(); anIt.Next())
    {
      Handle(StdStorage_Root)& aRoot = anIt.ChangeValue();
      aRoot->SetObject(aReadData.PersistentObject(aRoot->Reference()));
    }
  }

  return Storage_VSOk;
}

//=======================================================================
// StdStorage::currentDate
//=======================================================================
static TCollection_AsciiString currentDate()
{
#define SLENGTH 80

  char nowstr[SLENGTH];
  time_t nowbin;
  struct tm *nowstruct;
  if (time(&nowbin) != (time_t)-1)
  {
    nowstruct = localtime(&nowbin);
    strftime(nowstr, SLENGTH, "%m/%d/%Y", nowstruct);
  }
  TCollection_AsciiString t(nowstr);
  return t;

#undef SLENGTH
}

//=======================================================================
// StdStorage::Write
//=======================================================================
Storage_Error StdStorage::Write(Storage_BaseDriver&            theDriver, 
                                const Handle(StdStorage_Data)& theData)
{
  Standard_NullObject_Raise_if(theData.IsNull(), "Null storage data");

  Handle(StdStorage_HeaderData) aHeaderData = theData->HeaderData();
  Handle(StdStorage_TypeData)   aTypeData = theData->TypeData();
  Handle(StdStorage_RootData)   aRootData = theData->RootData();

  aHeaderData->SetCreationDate(currentDate());

  Handle(StdStorage_HSequenceOfRoots) aRoots = aRootData->Roots();

  StdStorage_BucketOfPersistent aPObjs;

  if (!aRoots.IsNull())
  {
    StdObjMgt_Persistent::SequenceOfPersistent aPQueue;
    for (StdStorage_HSequenceOfRoots::Iterator anIt(*aRoots); anIt.More(); anIt.Next())
    {
      Handle(StdStorage_Root) aRoot = anIt.ChangeValue();
      Handle(StdObjMgt_Persistent) aPObj = aRoot->Object();
      if (!aPObj.IsNull()) {
        aPQueue.Append(aPObj);
      }
    }
    while (!aPQueue.IsEmpty())
    {
      StdObjMgt_Persistent::SequenceOfPersistent aPQueue1;
      for (StdObjMgt_Persistent::SequenceOfPersistent::Iterator anIt(aPQueue); anIt.More(); anIt.Next())
      {
        Handle(StdObjMgt_Persistent)& aPObj = anIt.ChangeValue();
        if (!aPObj.IsNull())
        {
          if (aPObj->TypeNum() == 0 && aPObj->RefNum() == 0)
          {
            aPObj->TypeNum(aTypeData->AddType(aPObj));
            Standard_Integer anPObjId = aPObjs.Length() + 1;
            aPObj->RefNum(anPObjId);
            aPObjs.Append(aPObj);
            aPObj->PChildren(aPQueue1);
          }
        }
      }
      aPQueue.Assign(aPQueue1);
    }
  }

  aHeaderData->SetStorageVersion(StdStorage::Version());
  aHeaderData->SetNumberOfObjects(aPObjs.Length());

  try {
    // Write header section
    if (!aHeaderData->Write(theDriver))
      return aHeaderData->ErrorStatus();

    // Write types section
    if (!aTypeData->Write(theDriver))
      return aTypeData->ErrorStatus();

    // Write root section
    if (!aRootData->Write(theDriver))
      return aRootData->ErrorStatus();

    Storage_Error anError;

    // Write reference section
    anError = theDriver.BeginWriteRefSection();
    if (anError != Storage_VSOk)
      return anError;

    theDriver.SetRefSectionSize(aPObjs.Length());
    for (StdStorage_BucketIterator anIt(&aPObjs); anIt.More(); anIt.Next())
    {
      Handle(StdObjMgt_Persistent) aPObj = anIt.Value();
      if (!aPObj.IsNull())
        theDriver.WriteReferenceType(aPObj->RefNum(), aPObj->TypeNum());
    }

    anError = theDriver.EndWriteRefSection();
    if (anError != Storage_VSOk)
      return anError;

    // Write data section
    anError = theDriver.BeginWriteDataSection();
    if (anError != Storage_VSOk)
      return anError;

    StdObjMgt_WriteData aWriteData(theDriver);
    for (StdStorage_BucketIterator anIt(&aPObjs); anIt.More(); anIt.Next())
    {
      Handle(StdObjMgt_Persistent) aPObj = anIt.Value();
      if (!aPObj.IsNull())
        aWriteData.WritePersistentObject(aPObj);
    }

    anError = theDriver.EndWriteDataSection();
    if (anError != Storage_VSOk)
      return anError;
  }
  catch (Storage_StreamWriteError) {
    return Storage_VSWriteError;
  }

  return Storage_VSOk;
}
