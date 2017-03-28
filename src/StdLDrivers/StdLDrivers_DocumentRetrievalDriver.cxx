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

#include <StdObjMgt_Persistent.hxx>
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
#include <Standard_NotImplemented.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Handle.hxx>
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
  // Read header data and persistent document
  Storage_HeaderData aHeaderData;
  Handle(StdObjMgt_Persistent) aPDocument = read (theFileName, aHeaderData);
  if (aPDocument.IsNull())
    return;

  // Import transient document from the persistent one
  aPDocument->ImportDocument (
    Handle(TDocStd_Document)::DownCast (theNewDocument));

  // Copy comments from the header data
  theNewDocument->SetComments (aHeaderData.Comments());
}

//=======================================================================
//function : read
//purpose  : Read persistent document from a file
//=======================================================================
Handle(StdObjMgt_Persistent) StdLDrivers_DocumentRetrievalDriver::read (
  const TCollection_ExtendedString& theFileName,
  Storage_HeaderData&               theHeaderData)
{
  Standard_Integer i;

  // Create a driver appropriate for the given file
  PCDM_BaseDriverPointer aFileDriverPtr;
  if (PCDM::FileDriverType (TCollection_AsciiString (theFileName), aFileDriverPtr) == PCDM_TOFD_Unknown)
  {
    myReaderStatus = PCDM_RS_UnknownFileDriver;
    return NULL;
  }

  NCollection_Handle<Storage_BaseDriver> aFileDriver (aFileDriverPtr);

  // Try to open the file
  try
  {
    OCC_CATCH_SIGNALS
    PCDM_ReadWriter::Open (*aFileDriver, theFileName, Storage_VSRead);
    myReaderStatus = PCDM_RS_OK;
  } 
  catch (Standard_Failure const& anException)
  {
    myReaderStatus = PCDM_RS_OpenError;

    Standard_SStream aMsg;
    aMsg << anException << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
  
  // Read header section
  if (!theHeaderData.Read (*aFileDriver))
    raiseOnStorageError (theHeaderData.ErrorStatus());

  // Read type section
  Storage_TypeData aTypeData;
  if (!aTypeData.Read (*aFileDriver))
    raiseOnStorageError (aTypeData.ErrorStatus());

  // Read root section
  Storage_RootData aRootData;
  if (!aRootData.Read (*aFileDriver))
    raiseOnStorageError (aRootData.ErrorStatus());

  if (aRootData.NumberOfRoots() < 1)
  {
    myReaderStatus = PCDM_RS_NoDocument;

    Standard_SStream aMsg;
    aMsg << "could not find any document in this file" << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }

  // Select instantiators for the used types
  NCollection_Array1<StdObjMgt_Persistent::Instantiator>
    anInstantiators (1, aTypeData.NumberOfTypes());
  {
    StdObjMgt_MapOfInstantiators aMapOfInst;
    bindTypes (aMapOfInst);

    TColStd_SequenceOfAsciiString anUnknownTypes;
    Standard_Integer        aCurTypeNum;
    TCollection_AsciiString aCurTypeName;

    for (i = 1; i <= aTypeData.NumberOfTypes(); i++)
    {
      aCurTypeName = aTypeData.Type (i);
      aCurTypeNum  = aTypeData.Type (aCurTypeName);

      StdObjMgt_Persistent::Instantiator anInstantiator;
      if (aMapOfInst.Find(aCurTypeName, anInstantiator))
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

      throw Standard_Failure(aMsg.str().c_str());
    }
  }

  // Read and parse reference section
  StdObjMgt_ReadData aReadData (*aFileDriver, theHeaderData.NumberOfObjects());

  raiseOnStorageError (aFileDriver->BeginReadRefSection());

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

    raiseOnStorageError (anError);

    aReadData.CreatePersistentObject (aRef, anInstantiators (aType));
  }

  raiseOnStorageError (aFileDriver->EndReadRefSection());

  // Read and parse data section
  raiseOnStorageError (aFileDriver->BeginReadDataSection());

  for (i = 1; i <= theHeaderData.NumberOfObjects(); i++)
  {
    Storage_Error anError;
    try
    {
      OCC_CATCH_SIGNALS
      aReadData.ReadPersistentObject (i);
      anError = Storage_VSOk;
    }
    catch (Storage_StreamTypeMismatchError) { anError = Storage_VSTypeMismatch; }
    catch (Storage_StreamFormatError      ) { anError = Storage_VSFormatError;  }
    catch (Storage_StreamReadError        ) { anError = Storage_VSFormatError;  }

    raiseOnStorageError (anError);
  }

  raiseOnStorageError (aFileDriver->EndReadDataSection());

  // Get persistent document from the root object
  return aReadData.PersistentObject (aRootData.Roots()->First()->Reference());
}

//=======================================================================
//function : Read
//purpose  : not implemented
//=======================================================================

void StdLDrivers_DocumentRetrievalDriver::Read (Standard_IStream&               /*theIStream*/,
                                                const Handle(Storage_Data)&     /*theStorageData*/,
                                                const Handle(CDM_Document)&     /*theDoc*/,
                                                const Handle(CDM_Application)&  /*theApplication*/)
{
  throw Standard_NotImplemented("Reading from stream is not supported by StdLDrivers_DocumentRetrievalDriver");
}

//=======================================================================
//function : raiseOnStorageError
//purpose  : Update the reader status and raise an exception
//           appropriate for the given storage error
//=======================================================================
void StdLDrivers_DocumentRetrievalDriver::raiseOnStorageError (Storage_Error theError)
{
  Standard_SStream aMsg;

  switch (theError)
  {
  case Storage_VSOk:
    break;

  case Storage_VSOpenError:
  case Storage_VSNotOpen:
  case Storage_VSAlreadyOpen:
    myReaderStatus = PCDM_RS_OpenError;
    aMsg << "Stream Open Error" << endl;
    throw Standard_Failure(aMsg.str().c_str());

  case Storage_VSModeError:
    myReaderStatus = PCDM_RS_WrongStreamMode;
    aMsg << "Stream is opened with a wrong mode for operation" << endl;
    throw Standard_Failure(aMsg.str().c_str());

  case Storage_VSSectionNotFound:
    myReaderStatus = PCDM_RS_FormatFailure;
    aMsg << "Section is not found" << endl;
    throw Standard_Failure(aMsg.str().c_str());

  case Storage_VSFormatError:
    myReaderStatus = PCDM_RS_FormatFailure;
    aMsg << "Wrong format error" << endl;
    throw Standard_Failure(aMsg.str().c_str());

  case Storage_VSUnknownType:
    myReaderStatus = PCDM_RS_TypeFailure;
    aMsg << "Try to read an unknown type" << endl;
    throw Standard_Failure(aMsg.str().c_str());

  case Storage_VSTypeMismatch:
    myReaderStatus = PCDM_RS_TypeFailure;
    aMsg << "Try to read a wrong primitive type" << endl;
    throw Standard_Failure(aMsg.str().c_str());

  default:
    myReaderStatus = PCDM_RS_DriverFailure;
    aMsg << "Retrieval Driver Failure" << endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=======================================================================
//function : bindTypes
//purpose  : Register types
//=======================================================================
void StdLDrivers_DocumentRetrievalDriver::bindTypes (StdObjMgt_MapOfInstantiators& theMap)
{
  StdLDrivers::BindTypes (theMap);
}
