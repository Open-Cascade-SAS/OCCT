// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_Model.hxx>

#include <OSD_File.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Owner.hxx>

#include <TObj_Assistant.hxx>
#include <TObj_Application.hxx>
#include <TObj_CheckModel.hxx>
#include <TObj_HiddenPartition.hxx>
#include <TObj_LabelIterator.hxx>
#include <TObj_ModelIterator.hxx>
#include <TObj_Object.hxx>
#include <TObj_Partition.hxx>
#include <TObj_TObject.hxx>
#include <TObj_TModel.hxx>
#include <TObj_TNameContainer.hxx>
#include <Message_Msg.hxx>
#include <OSD_OpenFile.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TObj_Model, Standard_Transient)

#ifdef _MSC_VER
  #include <io.h>
#else
  #include <unistd.h>
#endif

//=================================================================================================

TObj_Model::TObj_Model()
{
  myMessenger = GetApplication()->Messenger();
}

//=================================================================================================

const occ::handle<TObj_Application> TObj_Model::GetApplication()
{
  return TObj_Application::GetInstance();
}

//=================================================================================================

TObj_Model::~TObj_Model()
{
  Close();
}

//=================================================================================================

void TObj_Model::CloseDocument(const occ::handle<TDocStd_Document>& theDoc)
{
  // prevent Abort of the following modifs at document destruction if
  // a transaction is open: see theDoc->myUndoTransaction.~()
  if (theDoc->HasOpenCommand())
    theDoc->AbortCommand();

  // Application
  const occ::handle<TObj_Application> anApplication = GetApplication();

  // just all other attributes
  theDoc->Main().Root().ForgetAllAttributes(true);
  anApplication->Close(theDoc);
}

//=======================================================================
// function : Load
// purpose  : Loads the model from the file
//=======================================================================

bool TObj_Model::Load(const TCollection_ExtendedString& theFile)
{
  // Return status
  bool aStatus = true;

  // Document
  occ::handle<TDocStd_Document> aDoc;

  // Application
  const occ::handle<TObj_Application> anApplication = GetApplication();

  // Current model
  const occ::handle<TObj_Model> me = this;
  TObj_Assistant::SetCurrentModel(me);
  TObj_Assistant::ClearTypeMap();

  bool isFileEmpty = checkDocumentEmpty(theFile);
  if (isFileEmpty)
  {
    // theFile is empty, create new TDocStd_Document for this model
    aStatus = anApplication->CreateNewDocument(aDoc, GetFormat());

    if (aStatus == true)
    {
      // Put model in a new attribute on root label
      TDF_Label                aLabel = aDoc->Main();
      occ::handle<TObj_TModel> anAtr  = new TObj_TModel;
      aLabel.AddAttribute(anAtr);
      anAtr->Set(me);
      // Record that label in the model object, and initialise the new model
      SetLabel(aLabel);
    }
  }
  else
  {
    // retrieve TDocStd_Document from <theFile>
    Messenger()->Send(Message_Msg("TObj_M_LoadDocument") << theFile, Message_Info);
    aStatus = anApplication->LoadDocument(theFile, aDoc);

    if (aStatus == true)
    {
      // Check for validity of the model read:
      // if it had wrong type, it has not been not properly restored
      TDF_Label aLabel  = GetLabel();
      bool      isValid = !aLabel.IsNull() && !aDoc.IsNull();
      {
        try
        {
          isValid = isValid && aLabel.Data() == aDoc->GetData();
        }
        catch (Standard_Failure const&)
        {
          isValid = false;
        }
      }
      if (!isValid)
      {
        if (!aDoc.IsNull())
          CloseDocument(aDoc);
        myLabel.Nullify();
        Messenger()->Send(Message_Msg("TObj_M_WrongFile") << theFile, Message_Alarm);
        aStatus = false;
      }
    }
    else
    {
      // release document from session
      // no message is needed as it has been put in anApplication->LoadDocument()
      if (!aDoc.IsNull())
        CloseDocument(aDoc);
      myLabel.Nullify();
    }
  }
  //    initialise the new model
  if (aStatus == true)
  {
    bool isInitOk = false;
    {
      try
      {
        isInitOk = initNewModel(isFileEmpty);
      }
      catch (Standard_Failure const& anException)
      {
#ifdef OCCT_DEBUG
        TCollection_ExtendedString aString(anException.DynamicType()->Name());
        aString = aString + ": " + anException.GetMessageString();
        Messenger()->Send(Message_Msg("TObj_Appl_Exception") << aString);
#endif
        (void)anException;
        Messenger()->Send(Message_Msg("TObj_M_WrongFile") << theFile, Message_Alarm);
      }
    }
    if (!isInitOk)
    {
      if (!aDoc.IsNull())
        CloseDocument(aDoc);
      myLabel.Nullify();
      aStatus = false;
    }
  }
  TObj_Assistant::UnSetCurrentModel();
  TObj_Assistant::ClearTypeMap();
  return aStatus;
}

//=======================================================================
// function : Load
// purpose  : Load the OCAF model from a stream. If case of failure,
//           it initializes the model by empty data.
//=======================================================================

bool TObj_Model::Load(Standard_IStream& theIStream)
{
  occ::handle<TDocStd_Document>       aDoc;
  bool                                aStatus = true, isFileLoaded = false;
  const occ::handle<TObj_Application> anApplication = GetApplication();

  // Current model
  const occ::handle<TObj_Model> me = this;
  TObj_Assistant::SetCurrentModel(me);
  TObj_Assistant::ClearTypeMap();

  // Retrieve TDocStd_Document from the stream.
  Messenger()->Send(Message_Msg("TObj_M_LoadDocument"), Message_Info);
  aStatus = anApplication->LoadDocument(theIStream, aDoc);
  if (aStatus)
  {
    // Check for validity of the model read:
    // if it had wrong type, it has not been not properly restored
    TDF_Label aLabel  = GetLabel();
    bool      isValid = (!aLabel.IsNull() && !aDoc.IsNull());
    try
    {
      isValid = (isValid && aLabel.Data() == aDoc->GetData());
    }
    catch (Standard_Failure const&)
    {
      isValid = false;
    }
    if (!isValid)
    {
      if (!aDoc.IsNull())
        CloseDocument(aDoc);
      myLabel.Nullify();
      Messenger()->Send(Message_Msg("TObj_M_WrongFile"), Message_Alarm);
      aStatus = false;
    }
    isFileLoaded = isValid;
  }
  else
  {
    // release document from session
    // no message is needed as it has been put in anApplication->LoadDocument()
    if (!aDoc.IsNull())
      CloseDocument(aDoc);
    myLabel.Nullify();

    aStatus = anApplication->CreateNewDocument(aDoc, GetFormat());
    if (aStatus)
    {
      // Put model in a new attribute on root label
      TDF_Label                aLabel = aDoc->Main();
      occ::handle<TObj_TModel> anAtr  = new TObj_TModel;
      aLabel.AddAttribute(anAtr);
      anAtr->Set(me);
      // Record that label in the model object, and initialise the new model
      SetLabel(aLabel);
    }
  }

  // Initialise the new model
  if (aStatus)
  {
    bool isInitOk = false;
    try
    {
      isInitOk = initNewModel(!isFileLoaded);
    }
    catch (Standard_Failure const& anException)
    {
#ifdef OCCT_DEBUG
      TCollection_ExtendedString aString(anException.DynamicType()->Name());
      aString = aString + ": " + anException.GetMessageString();
      Messenger()->Send(Message_Msg("TObj_Appl_Exception") << aString);
#endif
      (void)anException;
      Messenger()->Send(Message_Msg("TObj_M_WrongFile"), Message_Alarm);
    }
    if (!isInitOk)
    {
      if (!aDoc.IsNull())
        CloseDocument(aDoc);
      myLabel.Nullify();
      aStatus = false;
    }
  }

  TObj_Assistant::UnSetCurrentModel();
  TObj_Assistant::ClearTypeMap();
  return aStatus;
}

//=======================================================================
// function : GetFile
// purpose  : Returns the full file name this model is to be saved to,
//           or null if the model was not saved yet
//=======================================================================

occ::handle<TCollection_HExtendedString> TObj_Model::GetFile() const
{
  occ::handle<TDocStd_Document> aDoc = GetDocument();
  if (aDoc.IsNull() || !aDoc->IsStored())
  {
    return occ::handle<TCollection_HExtendedString>();
  }

  TCollection_ExtendedString aPath(aDoc->GetPath());
  return !aPath.IsEmpty() ? new TCollection_HExtendedString(aPath)
                          : occ::handle<TCollection_HExtendedString>();
}

//=======================================================================
// function : Save
// purpose  : Save the model to the same file
//=======================================================================

bool TObj_Model::Save()
{
  occ::handle<TDocStd_Document> aDoc = TDocStd_Document::Get(GetLabel());
  if (aDoc.IsNull())
    return false;

  if (!aDoc->GetPath().IsEmpty())
    return SaveAs(aDoc->GetPath());
  return true;
}

//=======================================================================
// function : SaveAs
// purpose  : Save the model to a file
//=======================================================================

bool TObj_Model::SaveAs(const TCollection_ExtendedString& theFile)
{
  TObj_Assistant::ClearTypeMap();
  // OCAF document
  occ::handle<TDocStd_Document> aDoc = TDocStd_Document::Get(GetLabel());
  if (aDoc.IsNull())
    return false;

  // checking that file is present on disk is not needed because could try to save as new
  // document to existent file
  // checking write access permission
  FILE* aF = OSD_OpenFile(theFile, "w");
  if (aF == NULL)
  {
    Messenger()->Send(Message_Msg("TObj_M_NoWriteAccess") << theFile, Message_Alarm);
    return false;
  }
  else
    fclose(aF);

  // store transaction mode
  bool aTrMode = aDoc->ModificationMode();
  aDoc->SetModificationMode(false);
  // store all trancienmt fields of object in OCAF document if any
  occ::handle<TObj_ObjectIterator> anIterator;
  for (anIterator = GetObjects(); anIterator->More(); anIterator->Next())
  {
    occ::handle<TObj_Object> anOCAFObj = anIterator->Value();
    if (anOCAFObj.IsNull())
      continue;
    anOCAFObj->BeforeStoring();
  } // end of for(anIterator = ...)
  // set transaction mode back
  aDoc->SetModificationMode(aTrMode);

  // Application
  const occ::handle<TObj_Application> anApplication = GetApplication();

  // call Application->SaveAs()
  bool aStatus = anApplication->SaveDocument(aDoc, theFile);

  TObj_Assistant::ClearTypeMap();
  return aStatus;
}

//=======================================================================
// function : SaveAs
// purpose  : Save the model to a stream
//=======================================================================

bool TObj_Model::SaveAs(Standard_OStream& theOStream)
{
  TObj_Assistant::ClearTypeMap();
  // OCAF document
  occ::handle<TDocStd_Document> aDoc = TDocStd_Document::Get(GetLabel());
  if (aDoc.IsNull())
    return false;

  // store transaction mode
  bool aTrMode = aDoc->ModificationMode();
  aDoc->SetModificationMode(false);
  // store all trancienmt fields of object in OCAF document if any
  occ::handle<TObj_ObjectIterator> anIterator;
  for (anIterator = GetObjects(); anIterator->More(); anIterator->Next())
  {
    occ::handle<TObj_Object> anOCAFObj = anIterator->Value();
    if (anOCAFObj.IsNull())
      continue;
    anOCAFObj->BeforeStoring();
  } // end of for(anIterator = ...)
    // set transaction mode back
  aDoc->SetModificationMode(aTrMode);

  // call Application->SaveAs()
  bool aStatus = GetApplication()->SaveDocument(aDoc, theOStream);
  TObj_Assistant::ClearTypeMap();
  return aStatus;
}

//=======================================================================
// function : Close
// purpose  : Close the model and free related OCAF document
//=======================================================================

bool TObj_Model::Close()
{
  // OCAF document
  TDF_Label aLabel = GetLabel();
  if (aLabel.IsNull())
    return false;
  occ::handle<TDocStd_Document> aDoc = TDocStd_Document::Get(aLabel);
  if (aDoc.IsNull())
    return false;

  CloseDocument(aDoc);

  myLabel.Nullify();
  return true;
}

//=======================================================================
// function : GetDocumentModel
// purpose  : returns model which contains a document with the label
//           returns NULL handle if label is NULL
//=======================================================================

occ::handle<TObj_Model> TObj_Model::GetDocumentModel(const TDF_Label& theLabel)
{
  occ::handle<TObj_Model> aModel;
  if (theLabel.IsNull())
    return aModel;

  occ::handle<TDocStd_Document> aDoc;
  occ::handle<TDF_Data>         aData  = theLabel.Data();
  TDF_Label                     aRootL = aData->Root();
  if (aRootL.IsNull())
    return aModel;
  occ::handle<TDocStd_Owner> aDocOwnerAtt;
  if (aRootL.FindAttribute(TDocStd_Owner::GetID(), aDocOwnerAtt))
    aDoc = aDocOwnerAtt->GetDocument();

  if (aDoc.IsNull())
    return aModel;

  TDF_Label                aLabel = aDoc->Main();
  occ::handle<TObj_TModel> anAttr;
  if (aLabel.FindAttribute(TObj_TModel::GetID(), anAttr))
    aModel = anAttr->Model();

  return aModel;
}

//=================================================================================================

occ::handle<TObj_ObjectIterator> TObj_Model::GetObjects() const
{
  occ::handle<TObj_Model> me = this;
  return new TObj_ModelIterator(me);
}

//=================================================================================================

occ::handle<TObj_ObjectIterator> TObj_Model::GetChildren() const
{
  occ::handle<TObj_Partition> aMainPartition = GetMainPartition();
  if (aMainPartition.IsNull())
    return 0;
  return aMainPartition->GetChildren();
}

//=================================================================================================

occ::handle<TObj_Object> TObj_Model::FindObject(
  const occ::handle<TCollection_HExtendedString>& theName,
  const occ::handle<TObj_TNameContainer>&         theDictionary) const
{
  occ::handle<TObj_TNameContainer> aDictionary = theDictionary;
  if (aDictionary.IsNull())
    aDictionary = GetDictionary();
  occ::handle<TObj_Object> aResult;
  // Check is object with given name is present in model
  if (IsRegisteredName(theName, aDictionary))
  {
    TDF_Label aLabel = aDictionary->Get().Find(theName);
    TObj_Object::GetObj(aLabel, aResult);
  }

  return aResult;
}

//=================================================================================================

occ::handle<TObj_Object> TObj_Model::GetRoot() const
{
  return getPartition(GetLabel());
}

//=================================================================================================

occ::handle<TObj_Partition> TObj_Model::GetMainPartition() const
{
  return getPartition(GetLabel());
}

//=================================================================================================

void TObj_Model::SetNewName(const occ::handle<TObj_Object>& theObject)
{
  occ::handle<TObj_Partition> aPartition = TObj_Partition::GetPartition(theObject);

  // sets name if partition is found
  if (aPartition.IsNull())
    return;

  occ::handle<TCollection_HExtendedString> name = aPartition->GetNewName();
  if (!name.IsNull())
    theObject->SetName(name);
}

//=================================================================================================

bool TObj_Model::IsRegisteredName(const occ::handle<TCollection_HExtendedString>& theName,
                                  const occ::handle<TObj_TNameContainer>& theDictionary) const
{
  occ::handle<TObj_TNameContainer> aDictionary = theDictionary;
  if (aDictionary.IsNull())
    aDictionary = GetDictionary();

  if (aDictionary.IsNull())
    return false;
  return aDictionary->IsRegistered(theName);
}

//=================================================================================================

void TObj_Model::RegisterName(const occ::handle<TCollection_HExtendedString>& theName,
                              const TDF_Label&                                theLabel,
                              const occ::handle<TObj_TNameContainer>&         theDictionary) const
{
  occ::handle<TObj_TNameContainer> aDictionary = theDictionary;
  if (aDictionary.IsNull())
    aDictionary = GetDictionary();

  if (!aDictionary.IsNull())
    aDictionary->RecordName(theName, theLabel);
}

//=================================================================================================

void TObj_Model::UnRegisterName(const occ::handle<TCollection_HExtendedString>& theName,
                                const occ::handle<TObj_TNameContainer>&         theDictionary) const
{
  occ::handle<TObj_TNameContainer> aDictionary = theDictionary;
  if (aDictionary.IsNull())
    aDictionary = GetDictionary();

  if (!aDictionary.IsNull())
    aDictionary->RemoveName(theName);
}

//=================================================================================================

occ::handle<TObj_TNameContainer> TObj_Model::GetDictionary() const
{
  occ::handle<TObj_TNameContainer> A;
  TDF_Label                        aLabel = GetLabel();
  if (!aLabel.IsNull())
    aLabel.FindAttribute(TObj_TNameContainer::GetID(), A);
  return A;
}

//=================================================================================================

occ::handle<TObj_Partition> TObj_Model::getPartition(const TDF_Label& theLabel,
                                                     const bool       theHidden) const
{
  occ::handle<TObj_Partition> aPartition;
  if (theLabel.IsNull())
    return aPartition;
  occ::handle<TObj_TObject> A;

  if (!theLabel.FindAttribute(TObj_TObject::GetID(), A))
  {
    if (theHidden)
      aPartition = new TObj_HiddenPartition(theLabel);
    else
      aPartition = TObj_Partition::Create(theLabel);
  }
  else
    aPartition = occ::down_cast<TObj_Partition>(A->Get());

  return aPartition;
}

//=================================================================================================

occ::handle<TObj_Partition> TObj_Model::getPartition(const TDF_Label&                  theLabel,
                                                     const int                         theIndex,
                                                     const TCollection_ExtendedString& theName,
                                                     const bool theHidden) const
{
  occ::handle<TObj_Partition> aPartition;
  if (theLabel.IsNull())
    return aPartition;

  TDF_Label aLabel = theLabel.FindChild(theIndex, false);
  bool      isNew  = false;
  // defining is partition new
  if (aLabel.IsNull())
  {
    aLabel = theLabel.FindChild(theIndex, true);
    isNew  = true;
  }
  // obtaining the partition
  aPartition = getPartition(aLabel, theHidden);

  // setting name to new partition
  if (isNew)
    aPartition->SetName(new TCollection_HExtendedString(theName));
  return aPartition;
}

//=================================================================================================

occ::handle<TObj_Partition> TObj_Model::getPartition(const int                         theIndex,
                                                     const TCollection_ExtendedString& theName,
                                                     const bool theHidden) const
{
  return getPartition(GetMainPartition()->GetChildLabel(), theIndex, theName, theHidden);
}

//=================================================================================================

bool TObj_Model::initNewModel(const bool IsNew)
{
  // set names map
  TObj_TNameContainer::Set(GetLabel());

  // do something for loaded model.
  if (!IsNew)
  {
    // Register names of model in names map.
    occ::handle<TObj_ObjectIterator> anIterator;
    for (anIterator = GetObjects(); anIterator->More(); anIterator->Next())
    {
      occ::handle<TObj_Object> anOCAFObj = anIterator->Value();
      if (anOCAFObj.IsNull())
        continue;
      anOCAFObj->AfterRetrieval();
    } // end of for(anIterator = ...)
    // update back references for loaded model by references
    updateBackReferences(GetMainPartition());

    if (isToCheck())
    {
      // check model consistency
      occ::handle<TObj_CheckModel> aCheck = GetChecker();
      aCheck->Perform();
      aCheck->SendMessages();
      // tell that the model has been modified
      SetModified(true);
    }
  }
  return true;
}

//=================================================================================================

void TObj_Model::updateBackReferences(const occ::handle<TObj_Object>& theObject)
{
  // recursive update back references
  if (theObject.IsNull())
    return;
  occ::handle<TObj_ObjectIterator> aChildren = theObject->GetChildren();
  for (; aChildren->More(); aChildren->Next())
  {
    occ::handle<TObj_Object> aChild = aChildren->Value();
    updateBackReferences(aChild);
  }
  // update back references of reference objects
  occ::handle<TObj_LabelIterator> anIter =
    occ::down_cast<TObj_LabelIterator>(theObject->GetReferences());

  if (anIter.IsNull()) // to avoid exception
    return;

  // LH3D15722. Remove all back references to make sure there will be no unnecessary
  // duplicates, since some back references may already exist after model upgrading.
  // (do not take care that object can be from other document, because
  // we do not modify document, all modifications are made in transient fields)
  for (; anIter->More(); anIter->Next())
  {
    occ::handle<TObj_Object> anObject = anIter->Value();
    if (!anObject.IsNull())
      anObject->RemoveBackReference(theObject, false);
  }
  // and at last create back references
  anIter = occ::down_cast<TObj_LabelIterator>(theObject->GetReferences());
  if (!anIter.IsNull())
    for (; anIter->More(); anIter->Next())
    {
      occ::handle<TObj_Object> anObject = anIter->Value();
      if (!anObject.IsNull())
        anObject->AddBackReference(theObject);
    }
}

//=================================================================================================

occ::handle<TDocStd_Document> TObj_Model::GetDocument() const
{
  occ::handle<TDocStd_Document> D;
  TDF_Label                     aLabel = GetLabel();
  if (!aLabel.IsNull())
    D = TDocStd_Document::Get(aLabel);
  return D;
}

//=================================================================================================

bool TObj_Model::HasOpenCommand() const
{
  return GetDocument()->HasOpenCommand();
}

//=================================================================================================

void TObj_Model::OpenCommand() const
{
  GetDocument()->OpenCommand();
}

//=================================================================================================

void TObj_Model::CommitCommand() const
{
  GetDocument()->CommitCommand();
}

//=================================================================================================

void TObj_Model::AbortCommand() const
{
  GetDocument()->AbortCommand();
}

//=======================================================================
// function : IsModified
// purpose  : Status of modification
//=======================================================================

bool TObj_Model::IsModified() const
{
  occ::handle<TDocStd_Document> aDoc = GetDocument();
  return aDoc.IsNull() ? false : aDoc->IsChanged();
}

//=======================================================================
// function : SetModified
// purpose  : Status of modification
//=======================================================================

void TObj_Model::SetModified(const bool theModified)
{
  occ::handle<TDocStd_Document> aDoc = GetDocument();
  if (!aDoc.IsNull())
  {
    int aSavedTime = aDoc->GetData()->Time();
    if (theModified)
      --aSavedTime;
    aDoc->SetSavedTime(aSavedTime);
  }
}

//=======================================================================
// function : checkDocumentEmpty
// purpose  : Check whether the document contains the Ocaf data
//=======================================================================

bool TObj_Model::checkDocumentEmpty(const TCollection_ExtendedString& theFile)
{
  if (theFile.IsEmpty())
    return true;

  OSD_Path aPath(theFile);
  OSD_File osdfile(aPath);
  if (!osdfile.Exists())
    return true;

  FILE* f = OSD_OpenFile(theFile, "r");
  if (f)
  {
    bool isZeroLengh = false;
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0)
      isZeroLengh = true;

    fclose(f);
    return isZeroLengh;
  }
  return false;
}

//=================================================================================================

Standard_GUID TObj_Model::GetGUID() const
{
  Standard_GUID aGUID("3bbefb49-e618-11d4-ba38-0060b0ee18ea");
  return aGUID;
}

//=================================================================================================

TCollection_ExtendedString TObj_Model::GetFormat() const
{
  return TCollection_ExtendedString("TObjBin");
}

//=================================================================================================

int TObj_Model::GetFormatVersion() const
{
  TDF_Label aLabel = GetDataLabel().FindChild(DataTag_FormatVersion, false);
  if (aLabel.IsNull())
    return -1;

  occ::handle<TDataStd_Integer> aNum;
  if (!aLabel.FindAttribute(TDataStd_Integer::GetID(), aNum))
    return -1;
  else
    return aNum->Get();
}

//=================================================================================================

void TObj_Model::SetFormatVersion(const int theVersion)
{
  TDF_Label aLabel = GetDataLabel().FindChild(DataTag_FormatVersion, true);
  TDataStd_Integer::Set(aLabel, theVersion);
}

//=================================================================================================

TDF_Label TObj_Model::GetDataLabel() const
{
  return GetMainPartition()->GetDataLabel();
}

//=================================================================================================

bool TObj_Model::Paste(occ::handle<TObj_Model>          theModel,
                       occ::handle<TDF_RelocationTable> theRelocTable)
{
  if (theModel.IsNull())
    return false;
  // clearing dictionary of objects names
  //  theModel->GetDictionary()->NewEmpty()->Paste(theModel->GetDictionary(),
  //                                               new TDF_RelocationTable);
  //  theModel->GetLabel().ForgetAllAttributes(true);
  TObj_TNameContainer::Set(theModel->GetLabel());
  GetMainPartition()->Clone(theModel->GetLabel(), theRelocTable);
  return true;
}

//=================================================================================================

void TObj_Model::CopyReferences(const occ::handle<TObj_Model>&          theTarget,
                                const occ::handle<TDF_RelocationTable>& theRelocTable)
{
  occ::handle<TObj_Object> aMyRoot     = GetMainPartition();
  occ::handle<TObj_Object> aTargetRoot = theTarget->GetMainPartition();
  aMyRoot->CopyReferences(aTargetRoot, theRelocTable);
}

//=======================================================================
// function : GetModelName
// purpose  : Returns the name of the model
//           by default returns TObj
//=======================================================================

occ::handle<TCollection_HExtendedString> TObj_Model::GetModelName() const
{
  occ::handle<TCollection_HExtendedString> aName = new TCollection_HExtendedString("TObj");
  return aName;
}

//=======================================================================
// function : Update
// purpose  : default implementation is empty
//=======================================================================

bool TObj_Model::Update()
{
  return true;
}

//=================================================================================================

occ::handle<TObj_CheckModel> TObj_Model::GetChecker() const
{
  return new TObj_CheckModel(this);
}
