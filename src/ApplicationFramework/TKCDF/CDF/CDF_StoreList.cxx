// Created on: 1997-08-08
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
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

#include <CDF_Application.hxx>
#include <CDF_MetaDataDriver.hxx>
#include <CDF_StoreList.hxx>
#include <CDM_Document.hxx>
#include <CDM_MetaData.hxx>
#include <Standard_ErrorHandler.hxx>
#include <CDM_ReferenceIterator.hxx>
#include <PCDM_StorageDriver.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDF_StoreList, Standard_Transient)

static void CAUGHT(const Standard_Failure&           theException,
                   TCollection_ExtendedString&       status,
                   const TCollection_ExtendedString& what)
{
  status += what;
  status += theException.GetMessageString();
}

CDF_StoreList::CDF_StoreList(const occ::handle<CDM_Document>& aDocument)
{
  myMainDocument = aDocument;
  Add(aDocument);
}

void CDF_StoreList::Add(const occ::handle<CDM_Document>& aDocument)
{

  if (!myItems.Contains(aDocument) && aDocument != myMainDocument)
    myItems.Add(aDocument);
  myStack.Prepend(aDocument);

  CDM_ReferenceIterator it(aDocument);
  for (; it.More(); it.Next())
  {
    if (it.Document()->IsModified())
      Add(it.Document());
  }
}

bool CDF_StoreList::IsConsistent() const
{
  bool                                                 yes = true;
  NCollection_Map<occ::handle<CDM_Document>>::Iterator it(myItems);
  for (; it.More() && yes; it.Next())
  {
    yes = it.Key()->HasRequestedFolder();
  }
  return yes && myMainDocument->HasRequestedFolder();
}

void CDF_StoreList::Init()
{
  myIterator = NCollection_Map<occ::handle<CDM_Document>>::Iterator(myItems);
}

bool CDF_StoreList::More() const
{
  return myIterator.More();
}

void CDF_StoreList::Next()
{
  myIterator.Next();
}

occ::handle<CDM_Document> CDF_StoreList::Value() const
{
  return myIterator.Key();
}

PCDM_StoreStatus CDF_StoreList::Store(occ::handle<CDM_MetaData>&   aMetaData,
                                      TCollection_ExtendedString&  aStatusAssociatedText,
                                      const Message_ProgressRange& theRange)
{
  PCDM_StoreStatus                status = PCDM_SS_OK;
  occ::handle<CDF_MetaDataDriver> theMetaDataDriver =
    occ::down_cast<CDF_Application>((myMainDocument->Application()))->MetaDataDriver();
  for (; !myStack.IsEmpty(); myStack.RemoveFirst())
  {
    occ::handle<CDM_Document> theDocument = myStack.First();
    if (theDocument == myMainDocument || theDocument->IsModified())
    {
      try
      {
        OCC_CATCH_SIGNALS
        occ::handle<CDF_Application> anApp =
          occ::down_cast<CDF_Application>(theDocument->Application());
        if (anApp.IsNull())
        {
          aStatusAssociatedText = "driver failed; reason: ";
          aStatusAssociatedText += "document has no application, cannot save!";
          status = PCDM_SS_Failure;
        }
        else
        {
          occ::handle<PCDM_StorageDriver> aDocumentStorageDriver =
            anApp->WriterFromFormat(theDocument->StorageFormat());
          if (aDocumentStorageDriver.IsNull())
          {
            aStatusAssociatedText =
              "driver not found; reason: no storage driver does exist for this format: ";
            aStatusAssociatedText += theDocument->StorageFormat();
            status = PCDM_SS_UnrecognizedFormat;
          }
          else
          {
            // Reset the store-status.
            // It has sense in multi-threaded access to the storage driver - this way we reset the
            // status for each call.
            aDocumentStorageDriver->SetStoreStatus(PCDM_SS_OK);

            if (!theMetaDataDriver->FindFolder(theDocument->RequestedFolder()))
            {
              aStatusAssociatedText = "driver not found; reason: ";
              aStatusAssociatedText += "could not find the active dbunit ";
              aStatusAssociatedText += theDocument->RequestedFolder();
              status = PCDM_SS_UnrecognizedFormat;
            }
            else
            {
              TCollection_ExtendedString theName = theMetaDataDriver->BuildFileName(theDocument);
              aDocumentStorageDriver->Write(theDocument, theName, theRange);
              status    = aDocumentStorageDriver->GetStoreStatus();
              aMetaData = theMetaDataDriver->CreateMetaData(theDocument, theName);
              theDocument->SetMetaData(aMetaData);

              CDM_ReferenceIterator it(theDocument);
              for (; it.More(); it.Next())
                theMetaDataDriver->CreateReference(aMetaData,
                                                   it.Document()->MetaData(),
                                                   it.ReferenceIdentifier(),
                                                   it.DocumentVersion());
            }
          }
        }
      }
      catch (Standard_Failure const& anException)
      {
        CAUGHT(anException,
               aStatusAssociatedText,
               TCollection_ExtendedString("driver failed; reason: "));
        status = PCDM_SS_DriverFailure;
      }
    }
  }

  return status;
}
