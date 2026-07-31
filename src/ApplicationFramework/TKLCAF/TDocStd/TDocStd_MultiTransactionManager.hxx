// Created on: 2002-11-18
// Created by: Vladimir ANIKIN
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _TDocStd_MultiTransactionManager_HeaderFile
#define _TDocStd_MultiTransactionManager_HeaderFile

#include <Standard.hxx>

#include <TDocStd_ApplicationDelta.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>
class TCollection_ExtendedString;
class TDocStd_Document;

//! Class for synchronization of transactions within multiple documents.
//! Each transaction of this class involvess one transaction in each modified document.
//!
//! The documents to be synchronized should be added explicitly to
//! the manager; then its interface is used to ensure that all transactions
//! (Open/Commit, Undo/Redo) are performed synchronously in all managed documents.
//!
//! The current implementation does not support nested transactions
//! on multitransaction manager level. It only sets the flag enabling
//! or disabling nested transactions in all its documents, so that
//! a nested transaction can be opened for each particular document
//! with TDocStd_Document class interface.
//!
//! NOTE: When you invoke CommitTransaction of multi transaction
//! manager, all nested transaction of its documents will be closed (committed).
class TDocStd_MultiTransactionManager : public Standard_Transient
{

public:
  //! Constructor
  Standard_EXPORT TDocStd_MultiTransactionManager();

  //! Sets undo limit for the manager and all documents.
  Standard_EXPORT void SetUndoLimit(const int theLimit);

  //! Returns undo limit for the manager.
  int GetUndoLimit() const;

  //! Undoes the current transaction of the manager.
  //! It calls the Undo () method of the document being
  //! on top of the manager list of undos (list.First())
  //! and moves the list item to the top of the list of manager
  //! redos (list.Prepend(item)).
  Standard_EXPORT void Undo();

  //! Redoes the current transaction of the application. It calls
  //! the Redo () method of the document being on top of the
  //! manager list of redos (list.First()) and moves the list
  //! item to the top of the list of manager undos (list.Prepend(item)).
  Standard_EXPORT void Redo();

  //! Returns available manager undos.
  const NCollection_Sequence<occ::handle<TDocStd_ApplicationDelta>>& GetAvailableUndos() const;

  //! Returns available manager redos.
  const NCollection_Sequence<occ::handle<TDocStd_ApplicationDelta>>& GetAvailableRedos() const;

  //! Opens transaction in each document and sets the flag that
  //! transaction is opened. If there are already opened transactions in the documents,
  //! these transactions will be aborted before opening new ones.
  Standard_EXPORT void OpenCommand();

  //! Unsets the flag of started manager transaction and aborts
  //! transaction in each document.
  Standard_EXPORT void AbortCommand();

  //! Commits transaction in all documents and fills the transaction manager
  //! with the documents that have been changed during the transaction.
  //! Returns True if new data has been added to myUndos.
  //! NOTE: All nested transactions in the documents will be committed.
  Standard_EXPORT bool CommitCommand();

  //! Makes the same steps as the previous function but defines the name for transaction.
  //! Returns True if new data has been added to myUndos.
  Standard_EXPORT bool CommitCommand(const TCollection_ExtendedString& theName);

  //! Returns true if a transaction is opened.
  bool HasOpenCommand() const;

  //! Removes undo information from the list of undos of the manager and
  //! all documents which have been modified during the transaction.
  Standard_EXPORT void RemoveLastUndo();

  //! Dumps transactions in undos and redos
  Standard_EXPORT void DumpTransaction(Standard_OStream& theOS) const;

  //! Adds the document to the transaction manager and
  //! checks if it has been already added
  Standard_EXPORT void AddDocument(const occ::handle<TDocStd_Document>& theDoc);

  //! Removes the document from the transaction manager.
  Standard_EXPORT void RemoveDocument(const occ::handle<TDocStd_Document>& theDoc);

  //! Returns the added documents to the transaction manager.
  const NCollection_Sequence<occ::handle<TDocStd_Document>>& Documents() const;

  //! Sets nested transaction mode if isAllowed == true
  //! NOTE: field myIsNestedTransactionMode exists only for synchronization
  //! between several documents and has no effect on transactions
  //! of multitransaction manager.
  Standard_EXPORT void SetNestedTransactionMode(const bool isAllowed = true);

  //! Returns true if NestedTransaction mode is set.
  //! Methods for protection of changes outside transactions
  bool IsNestedTransactionMode() const;

  //! If theTransactionOnly is True, denies all changes outside transactions.
  Standard_EXPORT void SetModificationMode(const bool theTransactionOnly);

  //! Returns True if changes are allowed only inside transactions.
  bool ModificationMode() const;

  //! Clears undos in the manager and in documents.
  Standard_EXPORT void ClearUndos();

  //! Clears redos in the manager and in documents.
  Standard_EXPORT void ClearRedos();

  DEFINE_STANDARD_RTTIEXT(TDocStd_MultiTransactionManager, Standard_Transient)

private:
  NCollection_Sequence<occ::handle<TDocStd_Document>>         myDocuments;
  NCollection_Sequence<occ::handle<TDocStd_ApplicationDelta>> myUndos;
  NCollection_Sequence<occ::handle<TDocStd_ApplicationDelta>> myRedos;
  int                                                         myUndoLimit;
  bool                                                        myOpenTransaction;
  bool                                                        myIsNestedTransactionMode;
  bool                                                        myOnlyTransactionModification;
};

#include <TDocStd_MultiTransactionManager.lxx>

#endif // _TDocStd_MultiTransactionManager_HeaderFile
