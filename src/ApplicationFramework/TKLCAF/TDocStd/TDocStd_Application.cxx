// Created on: 1999-06-30
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
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

#include <TDocStd_Application.hxx>

#include <CDF_Directory.hxx>
#include <CDF_DirectoryIterator.hxx>
#include <CDF_Store.hxx>
#include <PCDM_RetrievalDriver.hxx>
#include <PCDM_StorageDriver.hxx>
#include <PCDM_ReaderFilter.hxx>
#include <Plugin_Failure.hxx>
#include <Resource_Manager.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Dump.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Owner.hxx>
#include <TDocStd_PathParser.hxx>
#include <OSD_Thread.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDocStd_Application, CDF_Application)

// TDocStd_Owner attribute have pointer of closed TDocStd_Document
//=================================================================================================

TDocStd_Application::TDocStd_Application()
    : myIsDriverLoaded(Standard_True)
{
  if (myMetaDataDriver.IsNull())
    myIsDriverLoaded = Standard_False;
}

//=================================================================================================

Standard_Boolean TDocStd_Application::IsDriverLoaded() const
{
  return myIsDriverLoaded;
}

//=================================================================================================

Handle(Resource_Manager) TDocStd_Application::Resources()
{
  if (myResources.IsNull())
  {
    myResources = new Resource_Manager(ResourcesName());
  }
  return myResources;
}

//=================================================================================================

Standard_CString TDocStd_Application::ResourcesName()
{
  return "";
}

//=================================================================================================

void TDocStd_Application::DefineFormat(const TCollection_AsciiString&      theFormat,
                                       const TCollection_AsciiString&      theDescription,
                                       const TCollection_AsciiString&      theExtension,
                                       const Handle(PCDM_RetrievalDriver)& theReader,
                                       const Handle(PCDM_StorageDriver)&   theWriter)
{
  // register resources for CDM mechanics to work
  Handle(Resource_Manager) aResources = Resources();
  aResources->SetResource((theFormat + ".Description").ToCString(), theDescription.ToCString());
  aResources->SetResource((theFormat + ".FileExtension").ToCString(), theExtension.ToCString());
  aResources->SetResource((theExtension + ".FileFormat").ToCString(), theFormat.ToCString());

  // set format ID in the drivers to allow them putting it in
  // the OCAF documents opened by these drivers
  if (!theReader.IsNull())
    theReader->SetFormat(theFormat);
  if (!theWriter.IsNull())
    theWriter->SetFormat(theFormat);

  // register drivers
  myReaders.Add(theFormat, theReader);
  myWriters.Add(theFormat, theWriter);
}

//=================================================================================================

void TDocStd_Application::ReadingFormats(TColStd_SequenceOfAsciiString& theFormats)
{
  theFormats.Clear();

  NCollection_IndexedDataMap<TCollection_ExtendedString, Handle(PCDM_RetrievalDriver)>::Iterator
    anIter(myReaders);
  for (; anIter.More(); anIter.Next())
  {
    const Handle(PCDM_RetrievalDriver)& aDriver = anIter.Value();
    if (aDriver.IsNull() == Standard_False)
    {
      theFormats.Append(anIter.Key());
    }
  }
}

//=================================================================================================

void TDocStd_Application::WritingFormats(TColStd_SequenceOfAsciiString& theFormats)
{
  theFormats.Clear();

  NCollection_IndexedDataMap<TCollection_ExtendedString, Handle(PCDM_StorageDriver)>::Iterator
    anIter(myWriters);
  for (; anIter.More(); anIter.Next())
  {
    const Handle(PCDM_StorageDriver)& aDriver = anIter.Value();
    if (aDriver.IsNull() == Standard_False)
    {
      theFormats.Append(anIter.Key());
    }
  }
}

//=================================================================================================

Standard_Integer TDocStd_Application::NbDocuments() const
{
  return myDirectory->Length();
}

//=================================================================================================

void TDocStd_Application::GetDocument(const Standard_Integer    index,
                                      Handle(TDocStd_Document)& theDoc) const
{
  CDF_DirectoryIterator it(myDirectory);
  Standard_Integer      current = 0;
  for (; it.MoreDocument(); it.NextDocument())
  {
    current++;
    if (index == current)
    {
      Handle(TDocStd_Document) D = Handle(TDocStd_Document)::DownCast(it.Document());
      theDoc                     = D;
      return;
    }
  }
}

//=================================================================================================

void TDocStd_Application::NewDocument(const TCollection_ExtendedString& format,
                                      Handle(CDM_Document)&             theDoc)
{
  Handle(TDocStd_Document) D = new TDocStd_Document(format);
  InitDocument(D);
  CDF_Application::Open(D); // add the document in the session
  theDoc = D;
}

//=======================================================================
// function : NewDocument
// purpose  : A non-virtual method taking a TDocStd_Documment object as an input.
//         : Internally it calls a virtual method NewDocument() with CDM_Document object.
//=======================================================================

void TDocStd_Application::NewDocument(const TCollection_ExtendedString& format,
                                      Handle(TDocStd_Document)&         theDoc)
{
  Handle(CDM_Document) aCDMDoc;
  NewDocument(format, aCDMDoc);
  theDoc = Handle(TDocStd_Document)::DownCast(aCDMDoc);
}

//=================================================================================================

void TDocStd_Application::InitDocument(const Handle(CDM_Document)& /*aDoc*/) const {}

//=================================================================================================

void TDocStd_Application::Close(const Handle(TDocStd_Document)& theDoc)
{
  if (theDoc.IsNull())
  {
    return;
  }

  Handle(TDocStd_Owner) Owner;
  if (theDoc->Main().Root().FindAttribute(TDocStd_Owner::GetID(), Owner))
  {
    Handle(TDocStd_Document) emptyDoc;
    Owner->SetDocument(emptyDoc);
  }
  theDoc->BeforeClose();
  CDF_Application::Close(theDoc);
}

//=================================================================================================

Standard_Integer TDocStd_Application::IsInSession(const TCollection_ExtendedString& path) const
{
  TCollection_ExtendedString unifiedPath(path);
  unifiedPath.ChangeAll('/', '|');
  unifiedPath.ChangeAll('\\', '|');

  Standard_Integer         nbdoc = NbDocuments();
  Handle(TDocStd_Document) D;
  for (Standard_Integer i = 1; i <= nbdoc; i++)
  {
    GetDocument(i, D);
    if (D->IsSaved())
    {
      TCollection_ExtendedString unifiedDocPath(D->GetPath());
      unifiedDocPath.ChangeAll('/', '|');
      unifiedDocPath.ChangeAll('\\', '|');

      if (unifiedPath == unifiedDocPath)
        return i;
    }
  }
  return 0;
}

//=================================================================================================

PCDM_ReaderStatus TDocStd_Application::Open(const TCollection_ExtendedString& path,
                                            Handle(TDocStd_Document)&         theDoc,
                                            const Handle(PCDM_ReaderFilter)&  theFilter,
                                            const Message_ProgressRange&      theRange)
{
  PCDM_ReaderStatus          status = PCDM_RS_DriverFailure;
  TDocStd_PathParser         tool(path);
  TCollection_ExtendedString directory = tool.Trek();
  TCollection_ExtendedString file      = tool.Name();
  file += ".";
  file += tool.Extension();
  status = CanRetrieve(directory, file, !theFilter.IsNull() && theFilter->IsAppendMode());

  if (status != PCDM_RS_OK)
  {
    return status;
  }

  try
  {
    OCC_CATCH_SIGNALS
    Handle(TDocStd_Document) D = Handle(TDocStd_Document)::DownCast(
      Retrieve(directory, file, Standard_True, theFilter, theRange));
    if (theFilter.IsNull() || !theFilter->IsAppendMode())
      CDF_Application::Open(D);
    theDoc = D;
  }
  catch (Standard_Failure const& anException)
  {
    //    status = GetRetrieveStatus();
    if (!MessageDriver().IsNull())
    {
      //      Standard_SStream aMsg;
      //      aMsg << Standard_Failure::Caught() << std::endl;
      //      std::cout << "TDocStd_Application::Open(): " << aMsg.rdbuf()->str() << std::endl;
      TCollection_ExtendedString aString(anException.GetMessageString());
      MessageDriver()->Send(aString.ToExtString(), Message_Fail);
    }
  }
  status = GetRetrieveStatus();
#ifdef OCCT_DEBUG
  std::cout << "TDocStd_Application::Open(): The status = " << status << std::endl;
#endif
  return status;
}

//=================================================================================================

PCDM_ReaderStatus TDocStd_Application::Open(Standard_IStream&                theIStream,
                                            Handle(TDocStd_Document)&        theDoc,
                                            const Handle(PCDM_ReaderFilter)& theFilter,
                                            const Message_ProgressRange&     theRange)
{
  try
  {
    OCC_CATCH_SIGNALS
    Handle(CDM_Document) aCDMDoc = theDoc;
    Read(theIStream, aCDMDoc, theFilter, theRange);
    // Read calls NewDocument of TDocStd_Application, so, it should the TDocStd_Document in the
    // result anyway
    theDoc = Handle(TDocStd_Document)::DownCast(aCDMDoc);

    if (!theDoc.IsNull() && (theFilter.IsNull() || !theFilter->IsAppendMode()))
    {
      CDF_Application::Open(theDoc);
    }
  }

  catch (Standard_Failure const& anException)
  {
    if (!MessageDriver().IsNull())
    {
      TCollection_ExtendedString aFailureMessage(anException.GetMessageString());
      MessageDriver()->Send(aFailureMessage.ToExtString(), Message_Fail);
    }
  }
  return GetRetrieveStatus();
}

//=================================================================================================

PCDM_StoreStatus TDocStd_Application::SaveAs(const Handle(TDocStd_Document)&   theDoc,
                                             const TCollection_ExtendedString& path,
                                             const Message_ProgressRange&      theRange)
{
  TDocStd_PathParser         tool(path);
  TCollection_ExtendedString directory = tool.Trek();
  TCollection_ExtendedString file      = tool.Name();
  file += ".";
  file += tool.Extension();
  theDoc->Open(this);
  CDF_Store storer(theDoc);
  if (!storer.SetFolder(directory))
  {
    TCollection_ExtendedString aMsg("TDocStd_Application::SaveAs() - folder ");
    aMsg += directory;
    aMsg += " does not exist";
    if (!MessageDriver().IsNull())
      MessageDriver()->Send(aMsg.ToExtString(), Message_Fail);
    return storer.StoreStatus(); // CDF_SS_Failure;
  }
  storer.SetName(file);
  try
  {
    OCC_CATCH_SIGNALS
    storer.Realize(theRange);
  }
  catch (Standard_Failure const& anException)
  {
    if (!MessageDriver().IsNull())
    {
      TCollection_ExtendedString aString(anException.GetMessageString());
      MessageDriver()->Send(aString.ToExtString(), Message_Fail);
    }
  }
  if (storer.StoreStatus() == PCDM_SS_OK)
    theDoc->SetSaved();
  else if (!MessageDriver().IsNull())
    MessageDriver()->Send(storer.AssociatedStatusText(), Message_Fail);
#ifdef OCCT_DEBUG
  std::cout << "TDocStd_Application::SaveAs(): The status = " << storer.StoreStatus() << std::endl;
#endif
  return storer.StoreStatus();
}

//=================================================================================================

PCDM_StoreStatus TDocStd_Application::SaveAs(const Handle(TDocStd_Document)& theDoc,
                                             Standard_OStream&               theOStream,
                                             const Message_ProgressRange&    theRange)
{
  try
  {
    Handle(PCDM_StorageDriver) aDocStorageDriver = WriterFromFormat(theDoc->StorageFormat());

    if (aDocStorageDriver.IsNull())
    {
      return PCDM_SS_DriverFailure;
    }

    aDocStorageDriver->SetFormat(theDoc->StorageFormat());
    aDocStorageDriver->Write(theDoc, theOStream, theRange);

    if (aDocStorageDriver->GetStoreStatus() == PCDM_SS_OK)
    {
      theDoc->SetSaved();
    }

    return aDocStorageDriver->GetStoreStatus();
  }
  catch (Standard_Failure const& anException)
  {
    if (!MessageDriver().IsNull())
    {
      TCollection_ExtendedString aString(anException.GetMessageString());
      MessageDriver()->Send(aString.ToExtString(), Message_Fail);
    }
  }
  return PCDM_SS_Failure;
}

//=================================================================================================

PCDM_StoreStatus TDocStd_Application::Save(const Handle(TDocStd_Document)& D,
                                           const Message_ProgressRange&    theRange)
{
  PCDM_StoreStatus status = PCDM_SS_OK;
  if (D->IsSaved())
  {
    CDF_Store storer(D);
    try
    {
      OCC_CATCH_SIGNALS
      storer.Realize(theRange);
    }
    catch (Standard_Failure const& anException)
    {
      if (!MessageDriver().IsNull())
      {
        TCollection_ExtendedString aString(anException.GetMessageString());
        MessageDriver()->Send(aString.ToExtString(), Message_Fail);
      }
    }
    if (storer.StoreStatus() == PCDM_SS_OK)
      D->SetSaved();
    status = storer.StoreStatus();
  }
  else
  {
    if (!MessageDriver().IsNull())
    {
      TCollection_ExtendedString aMsg("Document has not been saved yet");
      MessageDriver()->Send(aMsg.ToExtString(), Message_Fail);
    }
    status = PCDM_SS_Failure;
  }
#ifdef OCCT_DEBUG
  std::cout << "TDocStd_Application::Save(): The status = " << status << std::endl;
#endif
  return status;
}

//=================================================================================================

PCDM_StoreStatus TDocStd_Application::SaveAs(const Handle(TDocStd_Document)&   D,
                                             const TCollection_ExtendedString& path,
                                             TCollection_ExtendedString&       theStatusMessage,
                                             const Message_ProgressRange&      theRange)
{
  TDocStd_PathParser         tool(path);
  PCDM_StoreStatus           aStatus   = PCDM_SS_Failure;
  TCollection_ExtendedString directory = tool.Trek();
  TCollection_ExtendedString file      = tool.Name();
  file += ".";
  file += tool.Extension();
  D->Open(this);
  CDF_Store storer(D);
  if (storer.SetFolder(directory))
  {
    storer.SetName(file);
    try
    {
      OCC_CATCH_SIGNALS
      storer.Realize(theRange);
    }
    catch (Standard_Failure const& anException)
    {
      if (!MessageDriver().IsNull())
      {
        TCollection_ExtendedString aString(anException.GetMessageString());
        MessageDriver()->Send(aString.ToExtString(), Message_Fail);
      }
    }
    if (storer.StoreStatus() == PCDM_SS_OK)
      D->SetSaved();
    theStatusMessage = storer.AssociatedStatusText();
    aStatus          = storer.StoreStatus();
  }
  else
  {
    theStatusMessage = TCollection_ExtendedString("TDocStd_Application::SaveAs"
                                                  ": No such directory ")
                       + directory;
    aStatus = PCDM_SS_Failure;
  }
  return aStatus;
}

//=================================================================================================

PCDM_StoreStatus TDocStd_Application::SaveAs(const Handle(TDocStd_Document)& theDoc,
                                             Standard_OStream&               theOStream,
                                             TCollection_ExtendedString&     theStatusMessage,
                                             const Message_ProgressRange&    theRange)
{
  try
  {
    Handle(PCDM_StorageDriver) aDocStorageDriver = WriterFromFormat(theDoc->StorageFormat());
    if (aDocStorageDriver.IsNull())
    {
      theStatusMessage =
        TCollection_ExtendedString("TDocStd_Application::SaveAs: no storage driver");
      return PCDM_SS_DriverFailure;
    }

    aDocStorageDriver->SetFormat(theDoc->StorageFormat());
    aDocStorageDriver->Write(theDoc, theOStream, theRange);

    if (aDocStorageDriver->GetStoreStatus() == PCDM_SS_OK)
    {
      theDoc->SetSaved();
    }

    return aDocStorageDriver->GetStoreStatus();
  }
  catch (Standard_Failure const& anException)
  {
    if (!MessageDriver().IsNull())
    {
      TCollection_ExtendedString aString(anException.GetMessageString());
      MessageDriver()->Send(aString.ToExtString(), Message_Fail);
    }
  }
  return PCDM_SS_Failure;
}

//=================================================================================================

PCDM_StoreStatus TDocStd_Application::Save(const Handle(TDocStd_Document)& D,
                                           TCollection_ExtendedString&     theStatusMessage,
                                           const Message_ProgressRange&    theRange)
{
  PCDM_StoreStatus status = PCDM_SS_OK;
  if (D->IsSaved())
  {
    CDF_Store storer(D);
    try
    {
      OCC_CATCH_SIGNALS
      storer.Realize(theRange);
    }
    catch (Standard_Failure const& anException)
    {
      if (!MessageDriver().IsNull())
      {
        TCollection_ExtendedString aString(anException.GetMessageString());
        MessageDriver()->Send(aString.ToExtString(), Message_Fail);
      }
    }
    if (storer.StoreStatus() == PCDM_SS_OK)
      D->SetSaved();
    status           = storer.StoreStatus();
    theStatusMessage = storer.AssociatedStatusText();
  }
  else
  {
    theStatusMessage = "TDocStd_Application::the document has not been saved yet";
    status           = PCDM_SS_Failure;
  }
  return status;
}

//=================================================================================================

void TDocStd_Application::OnOpenTransaction(const Handle(TDocStd_Document)&)
{
  // nothing to do on this level
}

//=================================================================================================

void TDocStd_Application::OnAbortTransaction(const Handle(TDocStd_Document)&)
{
  // nothing to do on this level
}

//=================================================================================================

void TDocStd_Application::OnCommitTransaction(const Handle(TDocStd_Document)&)
{
  // nothing to do on this level
}

//=================================================================================================

void TDocStd_Application::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsDriverLoaded)
}
