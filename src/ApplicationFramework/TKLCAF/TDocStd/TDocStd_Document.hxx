// Created on: 1999-04-07
// Created by: Denis PASCAL
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TDocStd_Document_HeaderFile
#define _TDocStd_Document_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <TDF_Transaction.hxx>
#include <TDF_Delta.hxx>
#include <NCollection_List.hxx>
#include <CDM_Document.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Map.hxx>
#include <TDocStd_FormatVersion.hxx>
class TDF_Data;
class TDF_Delta;
class TDF_Label;
class TCollection_AsciiString;
class TDocStd_CompoundDelta;

//! The contents of a TDocStd_Application, a
//! document is a container for a data framework
//! composed of labels and attributes. As such,
//! TDocStd_Document is the entry point into the data framework.
//! To gain access to the data, you create a document as follows:
//! occ::handle<TDocStd_Document> MyDF = new TDocStd_Document
//! The document also allows you to manage:
//! -   modifications, providing Undo and Redo functions.
//! -   command transactions.
//! Warning: The only data saved is the framework (TDF_Data)
class TDocStd_Document : public CDM_Document
{

public:
  //! Will Abort any execution, clear fields
  //! returns the document which contains <L>. raises an
  //! exception if the document is not found.
  Standard_EXPORT static occ::handle<TDocStd_Document> Get(const TDF_Label& L);

  //! Constructs a document object defined by the
  //! string astorageformat.
  //! If a document is created outside of an application using this constructor, it must be
  //! managed by a Handle. Otherwise memory problems could appear: call of
  //! TDocStd_Owner::GetDocument creates a occ::handle<TDocStd_Document>, so, releasing it will
  //! produce a crash.
  Standard_EXPORT TDocStd_Document(const TCollection_ExtendedString& astorageformat);

  //! the document is saved in a file.
  Standard_EXPORT bool IsSaved() const;

  //! returns True if document differs from the state of last saving.
  //! this method have to be called only working in the transaction mode
  bool IsChanged() const;

  //! This method have to be called to show document that it has been saved
  void SetSaved();

  //! Say to document what it is not saved.
  //! Use value, returned earlier by GetSavedTime().
  void SetSavedTime(const int theTime);

  //! Returns value of <mySavedTime> to be used later in SetSavedTime()
  int GetSavedTime() const;

  //! raise if <me> is not saved.
  Standard_EXPORT TCollection_ExtendedString GetName() const;

  //! returns the OS path of the file, in which one <me> is
  //! saved. Raise an exception if <me> is not saved.
  Standard_EXPORT TCollection_ExtendedString GetPath() const;

  Standard_EXPORT void SetData(const occ::handle<TDF_Data>& data);

  Standard_EXPORT occ::handle<TDF_Data> GetData() const;

  //! Returns the main label in this data framework.
  //! By definition, this is the label with the entry 0:1.
  Standard_EXPORT TDF_Label Main() const;

  //! Returns True if the main label has no attributes
  Standard_EXPORT bool IsEmpty() const;

  //! Returns False if the document has been modified
  //! but not recomputed.
  Standard_EXPORT bool IsValid() const;

  //! Notify the label as modified, the Document becomes UnValid.
  //! returns True if <L> has been notified as modified.
  Standard_EXPORT void SetModified(const TDF_Label& L);

  //! Remove all modifications. After this call The document
  //! becomesagain Valid.
  Standard_EXPORT void PurgeModified();

  //! Returns the labels which have been modified in
  //! this document.
  Standard_EXPORT const NCollection_Map<TDF_Label>& GetModified() const;

  //! Launches a new command. This command may be undone.
  Standard_EXPORT void NewCommand();

  //! returns True if a Command transaction is open in the current .
  Standard_EXPORT bool HasOpenCommand() const;

  //! Opens a new command transaction in this document.
  //! You can use HasOpenCommand to see whether a command is already open.
  //! Exceptions
  //! Standard_DomainError if a command is already open in this document.
  Standard_EXPORT void OpenCommand();

  //! Commits documents transactions and fills the
  //! transaction manager with documents that have
  //! been changed during the transaction.
  //! If no command transaction is open, nothing is done.
  //! Returns True if a new delta has been added to myUndos.
  Standard_EXPORT bool CommitCommand();

  //! Abort the Command transaction. Does nothing If there is
  //! no Command transaction open.
  Standard_EXPORT void AbortCommand();

  //! The current limit on the number of undos
  Standard_EXPORT int GetUndoLimit() const;

  //! Set the limit on the number of Undo Delta stored 0
  //! will disable Undo on the document A negative value
  //! means no limit. Note that by default Undo is disabled.
  //! Enabling it will take effect with the next call to
  //! NewCommand. Of course this limit is the same for Redo
  Standard_EXPORT void SetUndoLimit(const int L);

  //! Remove all stored Undos and Redos
  Standard_EXPORT void ClearUndos();

  //! Remove all stored Redos
  Standard_EXPORT void ClearRedos();

  //! Returns the number of undos stored in this
  //! document. If this figure is greater than 0, the method Undo
  //! can be used.
  Standard_EXPORT int GetAvailableUndos() const;

  //! Will UNDO one step, returns False if no undo was
  //! done (Undos == 0).
  //! Otherwise, true is returned and one step in the
  //! list of undoes is undone.
  Standard_EXPORT bool Undo();

  //! Returns the number of redos stored in this
  //! document. If this figure is greater than 0, the method Redo
  //! can be used.
  Standard_EXPORT int GetAvailableRedos() const;

  //! Will REDO one step, returns False if no redo was
  //! done (Redos == 0).
  //! Otherwise, true is returned, and one step in the list of redoes is done again.
  Standard_EXPORT bool Redo();

  Standard_EXPORT const NCollection_List<occ::handle<TDF_Delta>>& GetUndos() const;

  Standard_EXPORT const NCollection_List<occ::handle<TDF_Delta>>& GetRedos() const;

  //! Removes the first undo in the list of document undos.
  //! It is used in the application when the undo limit is exceed.
  Standard_EXPORT void RemoveFirstUndo();

  //! Initializes the procedure of delta compaction
  //! Returns false if there is no delta to compact
  //! Marks the last delta as a "from" delta
  Standard_EXPORT bool InitDeltaCompaction();

  //! Performs the procedure of delta compaction
  //! Makes all deltas starting from "from" delta
  //! till the last one to be one delta.
  Standard_EXPORT bool PerformDeltaCompaction();

  //! Set modifications on labels impacted by external
  //! references to the entry. The document becomes invalid
  //! and must be recomputed.
  Standard_EXPORT void UpdateReferences(const TCollection_AsciiString& aDocEntry);

  //! Recompute if the document was not valid and propagate
  //! the recorded modification.
  Standard_EXPORT void Recompute();

  //! This method Update will be called
  //! to signal the end of the modified references list.
  //! The document should be recomputed and
  //! UpdateFromDocuments should be called. Update should
  //! returns True in case of success, false otherwise. In
  //! case of Failure, additional information can be given in
  //! ErrorString.
  //! Update the document by propagation
  //! ==================================
  //! Update the document from internal stored
  //! modifications. If you want to undoing this
  //! operation, please call NewCommand before.
  //! to change format (advanced programming)
  //! ================
  Standard_EXPORT void Update(const occ::handle<CDM_Document>& aToDocument,
                                      const int                        aReferenceIdentifier,
                                      void* const                      aModifContext) override;

  Standard_EXPORT TCollection_ExtendedString StorageFormat() const override;

  //! Sets saving mode for empty labels. If true, empty labels will be saved.
  void SetEmptyLabelsSavingMode(const bool isAllowed);

  //! Returns saving mode for empty labels.
  bool EmptyLabelsSavingMode() const;

  //! methods for the nested transaction mode
  Standard_EXPORT virtual void ChangeStorageFormat(
    const TCollection_ExtendedString& newStorageFormat);

  //! Sets nested transaction mode if isAllowed == true
  void SetNestedTransactionMode(const bool isAllowed = true);

  //! Returns true if mode is set
  bool IsNestedTransactionMode() const;

  //! if theTransactionOnly is True changes is denied outside transactions
  void SetModificationMode(const bool theTransactionOnly);

  //! returns True if changes allowed only inside transactions
  bool ModificationMode() const;

  //! Prepares document for closing
  Standard_EXPORT virtual void BeforeClose();

  //! Returns version of the format to be used to store the document
  Standard_EXPORT TDocStd_FormatVersion StorageFormatVersion() const;

  //! Sets version of the format to be used to store the document
  Standard_EXPORT void ChangeStorageFormatVersion(const TDocStd_FormatVersion theVersion);

  //! Returns current storage format version of the document.
  Standard_EXPORT static TDocStd_FormatVersion CurrentStorageFormatVersion();

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(TDocStd_Document, CDM_Document)

protected:
  //! Returns true done
  Standard_EXPORT virtual bool CommitTransaction();

  Standard_EXPORT virtual void AbortTransaction();

  //! methods for protection of changes outside transactions
  Standard_EXPORT virtual void OpenTransaction();

  TCollection_ExtendedString               myStorageFormat;
  NCollection_List<occ::handle<TDF_Delta>> myUndos;
  NCollection_List<occ::handle<TDF_Delta>> myRedos;

private:
  //! Appends delta to the first delta in the myUndoFILO
  //! private methods
  //! ===============
  Standard_EXPORT static void AppendDeltaToTheFirst(
    const occ::handle<TDocStd_CompoundDelta>& theDelta1,
    const occ::handle<TDF_Delta>&             theDelta2);

  occ::handle<TDF_Data>                    myData;
  int                                      myUndoLimit;
  TDF_Transaction                          myUndoTransaction;
  occ::handle<TDF_Delta>                   myFromUndo;
  occ::handle<TDF_Delta>                   myFromRedo;
  int                                      mySaveTime;
  bool                                     myIsNestedTransactionMode;
  NCollection_List<occ::handle<TDF_Delta>> myUndoFILO;
  bool                                     myOnlyTransactionModification;
  bool                                     mySaveEmptyLabels;
  TDocStd_FormatVersion                    myStorageFormatVersion;
};

#include <TDocStd_Document.lxx>

#endif // _TDocStd_Document_HeaderFile
