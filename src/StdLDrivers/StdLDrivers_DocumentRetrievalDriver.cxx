// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdLDrivers_DocumentRetrievalDriver.hxx>
#include <StdLDrivers.hxx>
#include <StdLPersistent_PDocStd_Document.hxx>

#include <StdObjMgt_MapOfInstantiators.hxx>
#include <StdObjMgt_ReadData.hxx>

#include <Storage_HeaderData.hxx>
#include <Storage_TypeData.hxx>
#include <Storage_RootData.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <Storage_StreamFormatError.hxx>
#include <Storage_StreamReadError.hxx>

#include <PCDM.hxx>
#include <PCDM_ReadWriter.hxx>

#include <Standard_ErrorHandler.hxx>
#include <NCollection_Array1.hxx>
#include <TDocStd_Document.hxx>

IMPLEMENT_STANDARD_RTTIEXT (StdLDrivers_DocumentRetrievalDriver, PCDM_RetrievalDriver)

//=======================================================================
//function : CreateDocument
//purpose  : Create an empty TDocStd_Document
//=======================================================================
Handle(CDM_Document) StdLDrivers_DocumentRetrievalDriver::CreateDocument()
{
  return new TDocStd_Document (PCDM_RetrievalDriver::GetFormat());
}

//=======================================================================
//function : Read
//purpose  : Retrieve the content of a file into a new document
//=======================================================================
void StdLDrivers_DocumentRetrievalDriver::Read (const TCollection_ExtendedString& theFileName,
                                                const Handle(CDM_Document)&       theNewDocument,
                                                const Handle(CDM_Application)&)
{
  Standard_Integer i;

  // Create a driver appropriate for the given file
  PCDM_BaseDriverPointer aFileDriver;
  if (PCDM::FileDriverType (TCollection_AsciiString (theFileName), aFileDriver) == PCDM_TOFD_Unknown)
  {
    myReaderStatus = PCDM_RS_UnknownFileDriver;
    return;
  }

  // Try to open the file
  {
    Standard_SStream aMsg;
    try
    {
      OCC_CATCH_SIGNALS
      PCDM_ReadWriter::Open (*aFileDriver, theFileName, Storage_VSRead);
      myReaderStatus = PCDM_RS_OK;
    } 
    catch (Standard_Failure)
    {
      aMsg << Standard_Failure::Caught() << endl;
      myReaderStatus = PCDM_RS_OpenError;
    }

    if (myReaderStatus != PCDM_RS_OK)
    {
      Standard_Failure::Raise (aMsg);
      return;
    }
  }
  
  // Read header section
  Storage_HeaderData hData;
  if (!hData.Read (*aFileDriver))
  {
    RaiseOnStorageError (hData.ErrorStatus());
    return;
  }

  // Read type section
  Storage_TypeData tData;
  if (!tData.Read (*aFileDriver))
  {
    RaiseOnStorageError (tData.ErrorStatus());
    return;
  }

  // Read root section
  Storage_RootData rData;
  if (!rData.Read (*aFileDriver))
  {
    RaiseOnStorageError (rData.ErrorStatus());
    return;
  }

  if (rData.NumberOfRoots() < 1)
  {
    myReaderStatus = PCDM_RS_NoDocument;

    Standard_SStream aMsg;
    aMsg << "could not find any document in this file" << endl;
    Standard_Failure::Raise (aMsg);

    return;
  }

  // Select instantiators for the used types
  NCollection_Array1<StdObjMgt_Persistent::Instantiator>
    anInstantiators (1, tData.NumberOfTypes());
  {
    StdObjMgt_MapOfInstantiators aMapOfInstantiators;
    BindTypes (aMapOfInstantiators);

    TColStd_SequenceOfAsciiString anUnknownTypes;
    Standard_Integer        aCurTypeNum;
    TCollection_AsciiString aCurTypeName;

    for (i = 1; i <= tData.NumberOfTypes(); i++)
    {
      aCurTypeName = tData.Type (i);
      aCurTypeNum  = tData.Type (aCurTypeName);

      StdObjMgt_Persistent::Instantiator anInstantiator;
      if (aMapOfInstantiators.Find (aCurTypeName, anInstantiator))
        anInstantiators (aCurTypeNum) = anInstantiator;
      else
        anUnknownTypes.Append (aCurTypeName);
    }

    if (!anUnknownTypes.IsEmpty())
    {
      myReaderStatus = PCDM_RS_TypeNotFoundInSchema;

      Standard_SStream aMsg;
      aMsg << "cannot read: `" << theFileName
            << "' because it contains the following unknown types: ";
      for (i = 1; i <= anUnknownTypes.Length(); i++)
      {
        aMsg << anUnknownTypes(i);
        if (i < anUnknownTypes.Length()) aMsg << ",";
        else                             aMsg << endl;
      }

      Standard_Failure::Raise (aMsg);
      return;
    }
  }

  // Read and parse reference section
  StdObjMgt_ReadData aReadData (*aFileDriver, hData.NumberOfObjects());

  if (RaiseOnStorageError (aFileDriver->BeginReadRefSection()))
    return;

  Standard_Integer len = aFileDriver->RefSectionSize();
  for (i = 1; i <= len; i++)
  {
    Standard_Integer aRef = 0, aType = 0;
    Storage_Error anError;
    try
    {
      OCC_CATCH_SIGNALS
      aFileDriver->ReadReferenceType (aRef, aType);
      anError = Storage_VSOk;
    }
    catch (Storage_StreamTypeMismatchError)
    {
      anError = Storage_VSTypeMismatch;
    }

    if (RaiseOnStorageError (anError))
      return;

    aReadData.CreateObject (aRef, anInstantiators (aType));
  }

  if (RaiseOnStorageError (aFileDriver->EndReadRefSection()))
    return;

  // Read and parse data section
  if (RaiseOnStorageError (aFileDriver->BeginReadDataSection()))
    return;

  for (i = 1; i <= hData.NumberOfObjects(); i++)
  {
    Handle(StdObjMgt_Persistent) aPersistent = aReadData.Object (i);
    if (!aPersistent.IsNull())
    {
      Standard_Integer aRef = 0, aType = 0;
      Storage_Error anError;
      try
      {
        OCC_CATCH_SIGNALS
        aFileDriver->ReadPersistentObjectHeader (aRef, aType);
        aFileDriver->BeginReadPersistentObjectData();
        aPersistent->Read (aReadData);
        aFileDriver->EndReadPersistentObjectData();
        anError = Storage_VSOk;
      }
      catch (Storage_StreamTypeMismatchError) { anError = Storage_VSTypeMismatch; }
      catch (Storage_StreamFormatError      ) { anError = Storage_VSFormatError;  }
      catch (Storage_StreamReadError        ) { anError = Storage_VSFormatError;  }

      if (RaiseOnStorageError (anError))
        return;
    }
  }

  if (RaiseOnStorageError (aFileDriver->EndReadDataSection()))
    return;

  // Close the file
  aFileDriver->Close();
  delete aFileDriver;

  // Initialize transient document using the root object and comments
  Handle(Storage_HSeqOfRoot) aRoots = rData.Roots();

  Handle(Storage_Root) aFirstRoot = aRoots->First();

  Handle(StdObjMgt_Persistent) aFirstRootObject =
    aReadData.Object (aFirstRoot->Reference());

  Handle(StdLPersistent_PDocStd_Document) aPDocument =
    Handle(StdLPersistent_PDocStd_Document)::DownCast (aFirstRootObject);

  if (!aPDocument.IsNull())
    aPDocument->Import (theNewDocument);

  theNewDocument->SetComments (hData.Comments());
}

//=======================================================================
//function : RaiseOnStorageError
//purpose  : Update the reader status and raise an exception
//           appropriate for the given storage error
//=======================================================================
Standard_Boolean StdLDrivers_DocumentRetrievalDriver::RaiseOnStorageError (Storage_Error theError)
{
  Standard_SStream aMsg;

  switch (theError)
  {
  case Storage_VSOk:
    return Standard_False;

  case Storage_VSOpenError:
  case Storage_VSNotOpen:
  case Storage_VSAlreadyOpen:
    myReaderStatus = PCDM_RS_OpenError;
    aMsg << "Stream Open Error" << endl;
    break;

  case Storage_VSModeError:
    myReaderStatus = PCDM_RS_WrongStreamMode;
    aMsg << "Stream is opened with a wrong mode for operation" << endl;
    break;

  case Storage_VSSectionNotFound:
    myReaderStatus = PCDM_RS_FormatFailure;
    aMsg << "Section is not found" << endl;
    break;

  case Storage_VSFormatError:
    myReaderStatus = PCDM_RS_FormatFailure;
    aMsg << "Wrong format error" << endl;
    break;

  case Storage_VSUnknownType:
    myReaderStatus = PCDM_RS_TypeFailure;
    aMsg << "Try to read an unknown type" << endl;
    break;

  case Storage_VSTypeMismatch:
    myReaderStatus = PCDM_RS_TypeFailure;
    aMsg << "Try to read a wrong primitive type" << endl;
    break;

  default:
    myReaderStatus = PCDM_RS_DriverFailure;
    aMsg << "Retrieval Driver Failure" << endl;
  }

  Standard_Failure::Raise (aMsg);
  return Standard_True;
}

//=======================================================================
//function : BindTypes
//purpose  : Register types
//=======================================================================
void StdLDrivers_DocumentRetrievalDriver::BindTypes (StdObjMgt_MapOfInstantiators& theMap)
{
  StdLDrivers::BindTypes (theMap);
}
