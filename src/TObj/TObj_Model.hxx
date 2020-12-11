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

#ifndef TObj_Model_HeaderFile
#define TObj_Model_HeaderFile

#include <Message_Messenger.hxx>
#include <TDF_Label.hxx>
#include <TObj_Partition.hxx>
#include <TCollection_ExtendedString.hxx>

class TObj_TNameContainer;
class TObj_Partition;
class TCollection_HExtendedString;
class TDocStd_Document;
class TObj_CheckModel;
class TObj_Application;

class TObj_Model;
DEFINE_STANDARD_HANDLE(TObj_Model,Standard_Transient)

/**
* Base class for OCAF based models.
* Defines common behaviour for all models based on TObject 
* classes, basic services to access model objects and common
* operations with the model.
* Provides default implementation for many methods.
*/

class TObj_Model : public Standard_Transient
{
 protected:

  //! Model may store some data on the data labels of its main partition
  //! See TObj_Object for 
  enum DataTag
  {
    DataTag_FormatVersion = TObj_Partition::DataTag_Last,
    DataTag_Last
  };

 protected:
  /**
  * Constructors and Persistence
  */

  //! Empty constructor
  Standard_EXPORT TObj_Model ();

  //! Destructor closes the model
  Standard_EXPORT ~TObj_Model ();

  //! Check whether the document contains the OCAF data.
  Standard_EXPORT virtual Standard_Boolean checkDocumentEmpty(const TCollection_ExtendedString& theFile);

 public:
  /**
  * Messages mechanism
  */
  
  //! Set messenger to use for messages output
  void SetMessenger (const Handle(Message_Messenger) &theMsgr) { myMessenger = theMsgr; }

  //! Get messenger used for messages output (by default, the messenger from
  //! application is used)
  Handle(Message_Messenger) Messenger () const { return myMessenger; }
   
 public:
  /**
  * Implementation of Load/Save for OCAF based models
  */

  //! Load the OCAF model from a file. If the filename is empty or file does
  //! not exists, it just initializes model by empty data.
  Standard_EXPORT virtual Standard_Boolean Load (const TCollection_ExtendedString& theFile);

  //! Load the OCAF model from a stream. If case of failure,
  //! it initializes the model by empty data.
  Standard_EXPORT virtual Standard_Boolean Load (Standard_IStream& theIStream);

  //! Save the model to a file
  Standard_EXPORT virtual Standard_Boolean SaveAs (const TCollection_ExtendedString& theFile);

  //! Save the model to a stream
  Standard_EXPORT virtual Standard_Boolean SaveAs (Standard_OStream& theOStream);

  //! Save the model to the same file
  Standard_EXPORT Standard_Boolean Save ();

 public:
  /**
  * Work with document 
  */

  //! Close the model
  virtual Standard_EXPORT Standard_Boolean Close();

  //! Close Free OCAF document
  Standard_EXPORT void CloseDocument (const Handle(TDocStd_Document)& theDoc);

  //! Returns model which contains a document with the label,
  //! or NULL handle if label is NULL
  static Standard_EXPORT Handle(TObj_Model) GetDocumentModel (const TDF_Label& theLabel);

  //! Returns the full file name this model is to be saved to, 
  //! or null if the model was not saved yet
  virtual Standard_EXPORT Handle(TCollection_HExtendedString) GetFile() const;

 public:
  /**
  * Access to the objects in the model
  */

  //! Returns an Iterator on all objects in the Model
  virtual Standard_EXPORT Handle(TObj_ObjectIterator) GetObjects () const;

  //! Returns an Iterator on objects in the main partition
  virtual Standard_EXPORT Handle(TObj_ObjectIterator) GetChildren () const;

  //! Returns an Object by given Name (or Null if not found).
  virtual Standard_EXPORT Handle(TObj_Object) FindObject
                        (const Handle(TCollection_HExtendedString)& theName,
                         const Handle(TObj_TNameContainer)& theDictionary ) const;

  //! Returns the tool checking model consistency.
  //! Descendant may redefine it to return its own tool.
  virtual Standard_EXPORT Handle(TObj_CheckModel) GetChecker() const;

 public:
  /**
  * Methods for iteration on the model
  */

  //! Returns root object of model
  virtual Standard_EXPORT Handle(TObj_Object) GetRoot() const;

  //! Returns root object of model
  Standard_EXPORT Handle(TObj_Partition) GetMainPartition() const;

 public:
  /**
  * OCAF methods
  */

  //! Returns OCAF label on which model data are stored.
  TDF_Label GetLabel() const { return myLabel; }

 public:
  /**
  * Methods for supporting unique naming of the objects in model
  */

  //! Returns the name of the model
  virtual Standard_EXPORT Handle(TCollection_HExtendedString) GetModelName() const;
  
  //! Sets new unique name for the object
  static Standard_EXPORT void SetNewName
                        (const Handle(TObj_Object)& theObject);

  //! Returns True is name is registered in the names map
  //! The input argument may be NULL handle, then model check in own global container
  Standard_EXPORT Standard_Boolean IsRegisteredName
                        (const Handle(TCollection_HExtendedString)& theName,
                         const Handle(TObj_TNameContainer)& theDictionary ) const;

  //! Register name in the map
  //! The input argument may be NULL handle, then model check in own global container
  Standard_EXPORT void RegisterName
                        (const Handle(TCollection_HExtendedString)& theName,
                         const TDF_Label& theLabel,
                         const Handle(TObj_TNameContainer)& theDictionary ) const;

  //! Unregisters name from the map
  //! The input argument may be NULL handle, then model check in own global container
  Standard_EXPORT void UnRegisterName
                        (const Handle(TCollection_HExtendedString)& theName,
                         const Handle(TObj_TNameContainer)& theDictionary ) const;

 public:
  /**
  * API for transaction mechanism
  */

  //! Returns True if a Command transaction is open
  //! Starting, finishing the transaction
  Standard_EXPORT Standard_Boolean HasOpenCommand() const;

  //! Open a new command transaction.
  Standard_EXPORT void OpenCommand() const;

  //! Commit the Command transaction. Do nothing If there is no Command
  //! transaction open.
  Standard_EXPORT void CommitCommand() const;

  //! Abort the  Command  transaction. Do nothing If there is no Command
  //! transaction open.
  Standard_EXPORT void AbortCommand() const;

  //! Modification status
  virtual Standard_EXPORT Standard_Boolean IsModified () const;

  //! Sets modification status
  Standard_EXPORT void SetModified (const Standard_Boolean theModified);

 public:
  /**
  * Methods for obtaining application
  */

  //! Returns handle to static instance of the relevant application class
  virtual Standard_EXPORT const Handle(TObj_Application) GetApplication();

 public:
  /**
  * Methods for obtaining the version of Format
  */

  //! Returns the format for save/restore.
  //! This implementation returns "BinOcaf". The method should be redefined
  //! for those models that should use another format.
  virtual Standard_EXPORT TCollection_ExtendedString GetFormat() const;

  //! Returns the version of format stored in TObj file
  Standard_EXPORT Standard_Integer GetFormatVersion() const;

 protected:
  /**
  * Methods for handling the version of the Format
  */

  //! Sets the format version to save
  Standard_EXPORT void SetFormatVersion(const Standard_Integer theVersion);

 public:
  /**
  * Methods for updating model
  */

  //! this method is called before activating this model
  virtual Standard_EXPORT Standard_Boolean Update ();

 public:
  /**
  * Definition of interface GUID
  */

  //! Defines interface GUID for TObj_Model
  virtual Standard_EXPORT Standard_GUID GetGUID () const;

 public:
  /**
  * Internal methods
  */

  //! Returns the map of names of the objects
  Standard_EXPORT Handle(TObj_TNameContainer) GetDictionary() const;

 protected:
  /**
  * Internal methods
  */

  //! Returns (or creates a new) partition on a given label
  Standard_EXPORT Handle(TObj_Partition) getPartition
                         (const TDF_Label&       theLabel,
                          const Standard_Boolean theHidden=Standard_False) const;

  //! Returns Partition specified by its index number on a given label
  //! If not exists, creates anew with specified name
  Standard_EXPORT Handle(TObj_Partition) getPartition
              (const TDF_Label&                  theLabel,
               const Standard_Integer            theIndex,
               const TCollection_ExtendedString& theName,
               const Standard_Boolean            theHidden=Standard_False) const;

  //! Returns Partition specified by its index number
  //! If not exists, creates anew with specified name
  Standard_EXPORT Handle(TObj_Partition) getPartition
              (const Standard_Integer            theIndex,
               const TCollection_ExtendedString& theName,
               const Standard_Boolean            theHidden=Standard_False) const;

 public:

  //! Returns OCAF document of Model
  Standard_EXPORT Handle(TDocStd_Document) GetDocument() const;

 protected:
   // all that labels is sublabels of main partition

  //! Returns the labels under which the data is stored.
  //! the data stored from the third sublabel of this one.
  Standard_EXPORT TDF_Label GetDataLabel() const;

 public:

  //! Sets OCAF label on which model data are stored.
  //! Used by persistence mechanism.
  void SetLabel(const TDF_Label& theLabel) { myLabel = theLabel; }

 protected:

  //! Do the necessary initialisations after creation of a new model.
  //! This function is called by LoadModel after creation of OCAF document
  //! and setting myModel on its main label.
  //! Default implementation does nothing.
  //! Returns True is model sucsesfully initialized
  virtual Standard_EXPORT Standard_Boolean initNewModel
                        (const Standard_Boolean IsNew);

  //! Updates back references of object
  //! Recursive method.
  virtual Standard_EXPORT void updateBackReferences
                        (const Handle(TObj_Object)& theObject);

  //! Returns boolean value is to check model in Init new model
  //! The check could be useful if version of model changed
  //! Default implementation returns FALSE (check turned OFF)
  virtual Standard_Boolean isToCheck() const
  { return Standard_True; }

 public:
  /**
  * Methods for clone model
  */

  //! Pastes me to the new model
  //! references will not be copied if theRelocTable is not 0
  //! if theRelocTable is not NULL theRelocTable is filled by objects
  virtual Standard_EXPORT Standard_Boolean Paste 
                         (Handle(TObj_Model) theModel,
                          Handle(TDF_RelocationTable) theRelocTable = 0);

  //! This function have to create a new model with type like me
  virtual Standard_EXPORT Handle(TObj_Model) NewEmpty() = 0;

  //! Copy references from me to the other
  Standard_EXPORT void CopyReferences
    (const Handle(TObj_Model)& theTarget,
     const Handle(TDF_RelocationTable)& theRelocTable);

 private:
  /**
  * Fields
  */

  TDF_Label myLabel;                     //!< Root label of the model in OCAF document
  Handle(Message_Messenger) myMessenger; //!< Messenger object

 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_Model,Standard_Transient)
};

//! The Model Handle is defined in a separate header file

#endif
