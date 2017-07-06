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
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Tool.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDocStd_Document.hxx>
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

IMPLEMENT_STANDARD_RTTIEXT(TObj_Model,Standard_Transient)

#ifdef _MSC_VER
  #include <io.h>
#else
  #include <unistd.h>
#endif


//=======================================================================
//function : TObj_Model
//purpose  :
//=======================================================================

TObj_Model::TObj_Model ()
{
  myMessenger = GetApplication()->Messenger();
}

//=======================================================================
//function : GetApplication
//purpose  :
//=======================================================================

const Handle(TObj_Application) TObj_Model::GetApplication()
{
  return TObj_Application::GetInstance();
}

//=======================================================================
//function : Destructor
//purpose  :
//=======================================================================

TObj_Model::~TObj_Model ()
{
  Close();
}

//=======================================================================
//function : CloseDocument
//purpose  : free OCAF document
//=======================================================================

void TObj_Model::CloseDocument (const Handle(TDocStd_Document)& theDoc)
{
  // prevent Abort of the following modifs at document destruction if
  // a transaction is open: see theDoc->myUndoTransaction.~()
  if ( theDoc->HasOpenCommand() )
    theDoc->AbortCommand();
 
  // Application
  const Handle(TObj_Application) anApplication = GetApplication();

  // just all other attributes
  theDoc->Main().Root().ForgetAllAttributes(Standard_True);
  anApplication->Close( theDoc );
}

//=======================================================================
//function : Load
//purpose  : Loads the model from the file
//=======================================================================

Standard_Boolean TObj_Model::Load (const TCollection_ExtendedString& theFile)
{
  // Return status
  Standard_Boolean aStatus = Standard_True;

  // Document
  Handle(TDocStd_Document) aDoc;

  // Application
  const Handle(TObj_Application) anApplication = GetApplication();

  // Current model
  const Handle(TObj_Model) me = this;
  TObj_Assistant::SetCurrentModel( me );
  TObj_Assistant::ClearTypeMap();

  Standard_Boolean isFileEmpty = checkDocumentEmpty( theFile );
  if ( isFileEmpty )
  {
    // theFile is empty, create new TDocStd_Document for this model
    aStatus = anApplication->CreateNewDocument(aDoc, GetFormat());

    if ( aStatus == Standard_True )
    {
      // Put model in a new attribute on root label
      TDF_Label aLabel = aDoc->Main();
      Handle(TObj_TModel) anAtr = new TObj_TModel;
      aLabel.AddAttribute(anAtr);
      anAtr->Set( me );
      // Record that label in the model object, and initialise the new model
      SetLabel(aLabel);
    }
  }
  else
  {
    // retrieve TDocStd_Document from <theFile>
    Messenger()->Send(Message_Msg("TObj_M_LoadDocument") << theFile,
			     Message_Info);
    aStatus = anApplication->LoadDocument(theFile,aDoc);

    if ( aStatus == Standard_True )
    {
      // Check for validity of the model read:
      // if it had wrong type, it has not been not properly restored
      TDF_Label aLabel = GetLabel();
      Standard_Boolean isValid = !aLabel.IsNull() && !aDoc.IsNull();
      {
        try
        {
          isValid = isValid && aLabel.Data() == aDoc->GetData();
        }
        catch (Standard_Failure)
        {
          isValid = Standard_False;
        }
      }
      if (!isValid)
      {
        if (!aDoc.IsNull()) CloseDocument (aDoc);
        myLabel.Nullify();
        Messenger()->Send(Message_Msg("TObj_M_WrongFile") << theFile,
				 Message_Alarm);
        aStatus = Standard_False;
      }
    }
    else
    {
      // release document from session
      // no message is needed as it has been put in anApplication->LoadDocument()
      if (!aDoc.IsNull()) CloseDocument (aDoc);
      myLabel.Nullify();
    }
  }
  //    initialise the new model
  if ( aStatus == Standard_True )
  {
    Standard_Boolean isInitOk = Standard_False;
    {
      try
      {
        isInitOk = initNewModel(isFileEmpty);
      }
      catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
        TCollection_ExtendedString aString(anException.DynamicType()->Name());
        aString = aString + ": " + anException.GetMessageString();
        Messenger()->Send(Message_Msg("TObj_Appl_Exception") << aString);
#endif
        (void)anException;
        Messenger()->Send(Message_Msg("TObj_M_WrongFile") << theFile,
				 Message_Alarm);
      }
    }
    if (!isInitOk )
    {
      if (!aDoc.IsNull()) CloseDocument (aDoc);
      myLabel.Nullify();
      aStatus = Standard_False;
    }
  }
  TObj_Assistant::UnSetCurrentModel();
  TObj_Assistant::ClearTypeMap();
  return aStatus;
}

//=======================================================================
//function : GetFile
//purpose  : Returns the full file name this model is to be saved to, 
//           or null if the model was not saved yet
//=======================================================================

Handle(TCollection_HExtendedString) TObj_Model::GetFile() const
{
  Handle(TDocStd_Document) aDoc = GetDocument();
  if ( aDoc.IsNull()
   || !aDoc->IsStored())
  {
    return Handle(TCollection_HExtendedString)();
  }

  TCollection_ExtendedString aPath(aDoc->GetPath());
  return !aPath.IsEmpty()
       ? new TCollection_HExtendedString(aPath)
       : Handle(TCollection_HExtendedString)();
}

//=======================================================================
//function : Save
//purpose  : Save the model to the same file
//=======================================================================

Standard_Boolean TObj_Model::Save ()
{
  Handle(TDocStd_Document) aDoc = TDocStd_Document::Get(GetLabel());
  if (aDoc.IsNull())
    return Standard_False;

  
  if (!aDoc->GetPath().IsEmpty() )
    return SaveAs(aDoc->GetPath());
  return Standard_True;
}

//=======================================================================
//function : SaveAs
//purpose  : Save the model to a file
//=======================================================================

Standard_Boolean TObj_Model::SaveAs (const TCollection_ExtendedString& theFile)
{
  TObj_Assistant::ClearTypeMap();
  // OCAF document
  Handle(TDocStd_Document) aDoc = TDocStd_Document::Get(GetLabel());
  if ( aDoc.IsNull() )
    return Standard_False;

  // checking that file is present on disk is not needed because could try to save as new
  // document to existent file 
  // checking write access permission
  FILE *aF = OSD_OpenFile (theFile, "w");
  if (aF == NULL) {
    Messenger()->Send (Message_Msg("TObj_M_NoWriteAccess") << theFile, 
			      Message_Alarm);
    return Standard_False;
  }
  else
    fclose (aF);

  // store transaction mode
  Standard_Boolean aTrMode = aDoc->ModificationMode();
  aDoc->SetModificationMode( Standard_False );
  // store all trancienmt fields of object in OCAF document if any
  Handle(TObj_ObjectIterator) anIterator;
  for(anIterator = GetObjects(); anIterator->More(); anIterator->Next())
  {
    Handle(TObj_Object) anOCAFObj = anIterator->Value();
    if (anOCAFObj.IsNull())
      continue;
    anOCAFObj->BeforeStoring();
  } // end of for(anIterator = ...)
  // set transaction mode back
  aDoc->SetModificationMode( aTrMode );

  // Application
  const Handle(TObj_Application) anApplication = GetApplication();

  // call Application->SaveAs()
  Standard_Boolean aStatus = anApplication->SaveDocument (aDoc, theFile);

  TObj_Assistant::ClearTypeMap();
  return aStatus;
}

//=======================================================================
//function : Close
//purpose  : Close the model and free related OCAF document
//=======================================================================

Standard_Boolean TObj_Model::Close()
{
  // OCAF document
  TDF_Label aLabel = GetLabel();
  if ( aLabel.IsNull() )
    return Standard_False;
  Handle(TDocStd_Document) aDoc = TDocStd_Document::Get(aLabel);
  if ( aDoc.IsNull() )
    return Standard_False;

  CloseDocument (aDoc);

  myLabel.Nullify();
  return Standard_True;
}

//=======================================================================
//function : GetDocumentModel
//purpose  : returns model which contains a document with the label
//           returns NULL handle if label is NULL
//=======================================================================

Handle(TObj_Model) TObj_Model::GetDocumentModel
                         (const TDF_Label& theLabel)
{
  Handle(TObj_Model) aModel;
  if(theLabel.IsNull())
    return aModel;

  Handle(TDocStd_Document) aDoc;
  Handle(TDF_Data) aData = theLabel.Data();
  TDF_Label aRootL = aData->Root();
  if ( aRootL.IsNull())
    return aModel;
  Handle(TDocStd_Owner) aDocOwnerAtt;
  if (aRootL.FindAttribute (TDocStd_Owner::GetID(), aDocOwnerAtt))
    aDoc = aDocOwnerAtt->GetDocument();
  
  if ( aDoc.IsNull() )
    return aModel;

  TDF_Label aLabel = aDoc->Main();
  Handle(TObj_TModel) anAttr;
  if(aLabel.FindAttribute(TObj_TModel::GetID(), anAttr))
    aModel = anAttr->Model();

  return aModel;
}

//=======================================================================
//function : GetObjects
//purpose  :
//=======================================================================

Handle(TObj_ObjectIterator) TObj_Model::GetObjects() const
{
  Handle(TObj_Model) me = this;
  return new TObj_ModelIterator(me);
}

//=======================================================================
//function : GetChildren
//purpose  :
//=======================================================================

Handle(TObj_ObjectIterator) TObj_Model::GetChildren() const
{
  Handle(TObj_Partition) aMainPartition = GetMainPartition();
  if(aMainPartition.IsNull())
    return 0;
  return aMainPartition->GetChildren();
}

//=======================================================================
//function : FindObject
//purpose  :
//=======================================================================

Handle(TObj_Object) TObj_Model::FindObject
       (const Handle(TCollection_HExtendedString)& theName,
        const Handle(TObj_TNameContainer)& theDictionary ) const
{
  Handle(TObj_TNameContainer) aDictionary = theDictionary;
  if ( aDictionary.IsNull() )
    aDictionary = GetDictionary();
  Handle(TObj_Object) aResult;
  //Check is object with given name is present in model
  if( IsRegisteredName( theName, aDictionary ) )
  {
    TDF_Label aLabel = aDictionary->Get().Find( theName );
    TObj_Object::GetObj( aLabel, aResult );
  }

  return aResult;
}

//=======================================================================
//function : GetRoot
//purpose  :
//=======================================================================

Handle(TObj_Object) TObj_Model::GetRoot() const
{
  return getPartition(GetLabel());
}

//=======================================================================
//function : GetMainPartition
//purpose  :
//=======================================================================

Handle(TObj_Partition) TObj_Model::GetMainPartition() const
{
  return getPartition( GetLabel() );
}

//=======================================================================
//function : SetNewName
//purpose  :
//=======================================================================

void TObj_Model::SetNewName(const Handle(TObj_Object)& theObject)
{
  Handle(TObj_Partition) aPartition = TObj_Partition::GetPartition(theObject);

  //sets name if partition is found
  if(aPartition.IsNull()) return;

  Handle(TCollection_HExtendedString) name = aPartition->GetNewName();
  if ( ! name.IsNull() ) theObject->SetName(name);
}

//=======================================================================
//function : IsRegisteredName
//purpose  :
//=======================================================================

Standard_Boolean TObj_Model::IsRegisteredName(const Handle(TCollection_HExtendedString)& theName,
                                                  const Handle(TObj_TNameContainer)& theDictionary ) const
{
  Handle(TObj_TNameContainer) aDictionary = theDictionary;
  if ( aDictionary.IsNull() )
    aDictionary = GetDictionary();

  if ( aDictionary.IsNull() )
    return Standard_False;
  return aDictionary->IsRegistered( theName );
}

//=======================================================================
//function : RegisterName
//purpose  :
//=======================================================================

void TObj_Model::RegisterName(const Handle(TCollection_HExtendedString)& theName,
                                  const TDF_Label& theLabel,
                                  const Handle(TObj_TNameContainer)& theDictionary ) const
{
  Handle(TObj_TNameContainer) aDictionary = theDictionary;
  if ( aDictionary.IsNull() )
    aDictionary = GetDictionary();

  if ( !aDictionary.IsNull() )
    aDictionary->RecordName( theName, theLabel );
}

//=======================================================================
//function : UnRegisterName
//purpose  :
//=======================================================================

void TObj_Model::UnRegisterName(const Handle(TCollection_HExtendedString)& theName,
                                    const Handle(TObj_TNameContainer)& theDictionary ) const
{
  Handle(TObj_TNameContainer) aDictionary = theDictionary;
  if ( aDictionary.IsNull() )
    aDictionary = GetDictionary();

  if ( !aDictionary.IsNull() )
    aDictionary->RemoveName( theName );
}

//=======================================================================
//function : GetDictionary
//purpose  :
//=======================================================================

Handle(TObj_TNameContainer) TObj_Model::GetDictionary() const
{
  Handle(TObj_TNameContainer) A;
  TDF_Label aLabel = GetLabel();
  if (!aLabel.IsNull())
    aLabel.FindAttribute(TObj_TNameContainer::GetID(),A);
  return A;
}

//=======================================================================
//function : getPartition
//purpose  :
//=======================================================================

Handle(TObj_Partition) TObj_Model::getPartition
                         (const TDF_Label&       theLabel,
                          const Standard_Boolean theHidden) const
{
  Handle(TObj_Partition) aPartition;
  if(theLabel.IsNull()) return aPartition;
  Handle(TObj_TObject) A;

  if (!theLabel.FindAttribute (TObj_TObject::GetID(), A))
  {
    if (theHidden)
      aPartition = new TObj_HiddenPartition(theLabel);
    else
      aPartition = TObj_Partition::Create(theLabel);
  }
  else
    aPartition = Handle(TObj_Partition)::DownCast(A->Get());

  return aPartition;
}

//=======================================================================
//function : getPartition
//purpose  :
//=======================================================================

Handle(TObj_Partition) TObj_Model::getPartition
                         (const TDF_Label&                  theLabel,
                          const Standard_Integer            theIndex,
                          const TCollection_ExtendedString& theName,
                          const Standard_Boolean            theHidden) const
{
  Handle(TObj_Partition) aPartition;
  if(theLabel.IsNull()) return aPartition;

  TDF_Label aLabel = theLabel.FindChild(theIndex,Standard_False);
  Standard_Boolean isNew = Standard_False;
  // defining is partition new
  if ( aLabel.IsNull() )
  {
    aLabel = theLabel.FindChild(theIndex,Standard_True);
    isNew = Standard_True;
  }
  // obtaining the partition
  aPartition = getPartition( aLabel, theHidden );

  //setting name to new partition
  if(isNew)
    aPartition->SetName(new TCollection_HExtendedString(theName));
  return aPartition;
}


//=======================================================================
//function : getPartition
//purpose  :
//=======================================================================

Handle(TObj_Partition) TObj_Model::getPartition
                         (const Standard_Integer            theIndex,
                          const TCollection_ExtendedString& theName,
                          const Standard_Boolean            theHidden) const
{
  return getPartition (GetMainPartition()->GetChildLabel(),
                       theIndex, theName, theHidden);
}

//=======================================================================
//function : initNewModel
//purpose  :
//=======================================================================

Standard_Boolean TObj_Model::initNewModel (const Standard_Boolean IsNew)
{
  // set names map
  TObj_TNameContainer::Set(GetLabel());

  // do something for loaded model.
  if (!IsNew)
  {
    // Register names of model in names map.
    Handle(TObj_ObjectIterator) anIterator;
    for(anIterator = GetObjects(); anIterator->More(); anIterator->Next())
    {
      Handle(TObj_Object) anOCAFObj = anIterator->Value();
      if (anOCAFObj.IsNull())
        continue;
      anOCAFObj->AfterRetrieval();
    } // end of for(anIterator = ...)
    // update back references for loaded model by references
    updateBackReferences( GetMainPartition() );

    if ( isToCheck() )
    {
      // check model consistency
      Handle(TObj_CheckModel) aCheck = GetChecker();
      aCheck->Perform();
      aCheck->SendMessages();
      // tell that the model has been modified
      SetModified(Standard_True);
    }
  }
  return Standard_True;
}

//=======================================================================
//function : updateBackReferences
//purpose  :
//=======================================================================

void TObj_Model::updateBackReferences (const Handle(TObj_Object)& theObject)
{
  // recursive update back references
  if ( theObject.IsNull() )
    return;
  Handle(TObj_ObjectIterator) aChildren = theObject->GetChildren();
  for(;aChildren->More(); aChildren->Next())
  {
    Handle(TObj_Object) aChild = aChildren->Value();
    updateBackReferences( aChild );
  }
  // update back references of reference objects
  Handle(TObj_LabelIterator) anIter =
    Handle(TObj_LabelIterator)::DownCast(theObject->GetReferences());

  if(anIter.IsNull()) // to avoid exception
    return;

  // LH3D15722. Remove all back references to make sure there will be no unnecessary
  // duplicates, since some back references may already exist after model upgrading.
  // (do not take care that object can be from other document, because 
  // we do not modify document, all modifications are made in transient fields)
  for( ; anIter->More() ; anIter->Next())
  {
    Handle(TObj_Object) anObject = anIter->Value();
    if ( !anObject.IsNull() )
      anObject->RemoveBackReference( theObject, Standard_False );
  }
  // and at last create back references
  anIter = Handle(TObj_LabelIterator)::DownCast(theObject->GetReferences());
  if(!anIter.IsNull())
    for( ; anIter->More() ; anIter->Next())
    {
      Handle(TObj_Object) anObject = anIter->Value();
      if ( !anObject.IsNull() )
        anObject->AddBackReference( theObject );
    }
}

//=======================================================================
//function : GetDocument
//purpose  :
//=======================================================================

Handle(TDocStd_Document) TObj_Model::GetDocument() const
{
  Handle(TDocStd_Document) D;
  TDF_Label aLabel = GetLabel();
  if (!aLabel.IsNull())
    D = TDocStd_Document::Get(aLabel);
  return D;
}

//=======================================================================
//function : HasOpenCommand
//purpose  :
//=======================================================================

Standard_Boolean TObj_Model::HasOpenCommand() const
{
  return GetDocument()->HasOpenCommand();
}

//=======================================================================
//function : OpenCommand
//purpose  :
//=======================================================================

void TObj_Model::OpenCommand() const
{
  GetDocument()->OpenCommand();
}

//=======================================================================
//function : CommitCommand
//purpose  :
//=======================================================================

void TObj_Model::CommitCommand() const
{
  GetDocument()->CommitCommand();
}

//=======================================================================
//function : AbortCommand
//purpose  :
//=======================================================================

void TObj_Model::AbortCommand() const
{
  GetDocument()->AbortCommand();
}

//=======================================================================
//function : IsModified
//purpose  : Status of modification
//=======================================================================

Standard_Boolean TObj_Model::IsModified () const
{
  Handle(TDocStd_Document) aDoc = GetDocument();
  return aDoc.IsNull() ? Standard_False : aDoc->IsChanged();
}

//=======================================================================
//function : SetModified
//purpose  : Status of modification
//=======================================================================

void TObj_Model::SetModified (const Standard_Boolean theModified)
{
  Handle(TDocStd_Document) aDoc = GetDocument();
  if (!aDoc.IsNull())
  {
    Standard_Integer aSavedTime = aDoc->GetData()->Time();
    if (theModified)
      --aSavedTime;
    aDoc->SetSavedTime (aSavedTime);
  }
}

//=======================================================================
//function : checkDocumentEmpty
//purpose  : Check whether the document contains the Ocaf data
//=======================================================================

Standard_Boolean TObj_Model::checkDocumentEmpty (const TCollection_ExtendedString& theFile)
{
  if (theFile.IsEmpty())
    return Standard_True;

  OSD_Path aPath (theFile);
  OSD_File osdfile (aPath);
  if ( !osdfile.Exists() )
    return Standard_True;
  
  FILE* f = OSD_OpenFile( theFile, "r" );
  if ( f )
  {
    Standard_Boolean isZeroLengh = Standard_False;
    fseek( f, 0, SEEK_END );
    if ( ftell( f ) == 0 )
      isZeroLengh = Standard_True;

    fclose( f );
    return isZeroLengh;
  }
  return Standard_False;
}

//=======================================================================
//function : GetGUID
//purpose  :
//=======================================================================

Standard_GUID TObj_Model::GetGUID() const
{
  Standard_GUID aGUID("3bbefb49-e618-11d4-ba38-0060b0ee18ea");
  return aGUID;
}

//=======================================================================
//function : GetFormat
//purpose  :
//=======================================================================

TCollection_ExtendedString TObj_Model::GetFormat() const
{
  return TCollection_ExtendedString ("TObjBin");
}

//=======================================================================
//function : GetFormatVersion
//purpose  :
//=======================================================================

Standard_Integer TObj_Model::GetFormatVersion() const
{
  TDF_Label aLabel = GetDataLabel().FindChild(DataTag_FormatVersion,Standard_False);
  if(aLabel.IsNull())
    return -1;

  Handle(TDataStd_Integer) aNum;
  if(!aLabel.FindAttribute ( TDataStd_Integer::GetID(), aNum ))
    return -1;
  else
    return aNum->Get();
}

//=======================================================================
//function : SetFormatVersion
//purpose  :
//=======================================================================

void TObj_Model::SetFormatVersion(const Standard_Integer theVersion)
{
  TDF_Label aLabel = GetDataLabel().FindChild(DataTag_FormatVersion,Standard_True);
  TDataStd_Integer::Set(aLabel,theVersion);
}


//=======================================================================
//function : GetDataLabel
//purpose  :
//=======================================================================

TDF_Label TObj_Model::GetDataLabel() const
{
  return GetMainPartition()->GetDataLabel();
}

//=======================================================================
//function : Paste
//purpose  :
//=======================================================================

Standard_Boolean TObj_Model::Paste (Handle(TObj_Model)      theModel,
                                        Handle(TDF_RelocationTable) theRelocTable)
{
  if(theModel.IsNull()) return Standard_False;
  // clearing dictionary of objects names
//  theModel->GetDictionary()->NewEmpty()->Paste(theModel->GetDictionary(),
//                                               new TDF_RelocationTable);
//  theModel->GetLabel().ForgetAllAttributes(Standard_True);
  TObj_TNameContainer::Set(theModel->GetLabel());
  GetMainPartition()->Clone(theModel->GetLabel(), theRelocTable);
  return Standard_True;
}

//=======================================================================
//function : CopyReferences
//purpose  :
//=======================================================================

void TObj_Model::CopyReferences(const Handle(TObj_Model)& theTarget,
                                    const Handle(TDF_RelocationTable)& theRelocTable)
{
  Handle(TObj_Object) aMyRoot = GetMainPartition();
  Handle(TObj_Object) aTargetRoot = theTarget->GetMainPartition();
  aMyRoot->CopyReferences(aTargetRoot, theRelocTable);
}

//=======================================================================
//function : GetModelName
//purpose  : Returns the name of the model
//           by default returns TObj
//=======================================================================

Handle(TCollection_HExtendedString) TObj_Model::GetModelName() const
{
  Handle(TCollection_HExtendedString) aName =
    new TCollection_HExtendedString("TObj");
  return aName;
}

//=======================================================================
//function : Update
//purpose  : default implementation is empty
//=======================================================================

Standard_Boolean TObj_Model::Update ()
{
  return Standard_True;
}

//=======================================================================
//function : GetChecker
//purpose  :
//=======================================================================

Handle(TObj_CheckModel) TObj_Model::GetChecker() const
{
  return new TObj_CheckModel (this);
}
