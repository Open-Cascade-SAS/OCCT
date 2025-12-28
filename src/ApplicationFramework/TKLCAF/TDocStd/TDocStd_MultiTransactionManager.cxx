// Created on: 2002-11-19
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

#include <TDocStd_MultiTransactionManager.hxx>

#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDocStd_ApplicationDelta.hxx>
#include <TDocStd_Document.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDocStd_MultiTransactionManager, Standard_Transient)

//=================================================================================================

TDocStd_MultiTransactionManager::TDocStd_MultiTransactionManager()
{
  myUndoLimit                   = 0;
  myOpenTransaction             = false;
  myIsNestedTransactionMode     = false;
  myOnlyTransactionModification = false;
}

//=================================================================================================

void TDocStd_MultiTransactionManager::SetUndoLimit(const int theLimit)
{
  myUndoLimit = theLimit;

  CommitCommand();

  int n = myUndos.Length() - myUndoLimit;
  while (n > 0)
  {
    RemoveLastUndo();
    --n;
  }

  int i;
  for (i = myDocuments.Length(); i > 0; i--)
    myDocuments.Value(i)->SetUndoLimit(myUndoLimit);
}

//=================================================================================================

void TDocStd_MultiTransactionManager::Undo()
{
  if (myUndos.IsEmpty())
    return;
  const NCollection_Sequence<occ::handle<TDocStd_Document>>& docs = myUndos.First()->GetDocuments();
  int                                                        i;
  for (i = docs.Length(); i > 0; i--)
  {
    occ::handle<TDocStd_Document> doc = docs.Value(i);
    if (doc.IsNull() || doc->GetAvailableUndos() == 0)
      continue;
    doc->Undo();
  }
  myRedos.Prepend(myUndos.First());
  myUndos.Remove(1);
  myOpenTransaction = false;
}

//=================================================================================================

void TDocStd_MultiTransactionManager::Redo()
{
  if (myRedos.IsEmpty())
    return;
  const NCollection_Sequence<occ::handle<TDocStd_Document>>& docs = myRedos.First()->GetDocuments();
  int                                                        i;
  for (i = docs.Length(); i > 0; i--)
  {
    occ::handle<TDocStd_Document> doc = docs.Value(i);
    if (doc.IsNull() || doc->GetAvailableRedos() == 0)
      continue;
    doc->Redo();
  }
  myUndos.Prepend(myRedos.First());
  myRedos.Remove(1);
  myOpenTransaction = false;
}

//=================================================================================================

void TDocStd_MultiTransactionManager::OpenCommand()
{
  if (myOpenTransaction)
  {
#ifdef OCCT_DEBUG
    std::cout << "TDocStd_MultiTransactionManager::OpenCommand(): "
                 "Can't start new application transaction while a "
                 "previous one is not committed or aborted"
              << std::endl;
#endif
    throw Standard_Failure("Can't start new application transaction"
                           "while a previous one is not committed or aborted");
  }
  myOpenTransaction = true;
  int i;
  for (i = myDocuments.Length(); i > 0; i--)
  {
    while (myDocuments.Value(i)->HasOpenCommand())
      myDocuments.Value(i)->AbortCommand();
    myDocuments.Value(i)->OpenCommand();
  }
}

//=================================================================================================

void TDocStd_MultiTransactionManager::AbortCommand()
{
  myOpenTransaction = false;
  int i;
  for (i = myDocuments.Length(); i > 0; i--)
  {
    while (myDocuments.Value(i)->HasOpenCommand())
      myDocuments.Value(i)->AbortCommand();
  }
}

//=================================================================================================

bool TDocStd_MultiTransactionManager::CommitCommand()
{
  occ::handle<TDocStd_ApplicationDelta> aDelta     = new TDocStd_ApplicationDelta;
  bool                                  isCommited = false;
  int                                   i;
  for (i = myDocuments.Length(); i > 0; i--)
  {
    isCommited = false;
    while (myDocuments.Value(i)->HasOpenCommand())
      if (myDocuments.Value(i)->CommitCommand())
        isCommited = true;
    if (isCommited)
    {
      aDelta->GetDocuments().Append(myDocuments.Value(i));
    }
  }

  if (aDelta->GetDocuments().Length())
  {
    myUndos.Prepend(aDelta);
    if (myUndos.Length() > myUndoLimit)
    {
      RemoveLastUndo();
    }
    myRedos.Clear();
    isCommited = true;
  }
  myOpenTransaction = false;
  return isCommited;
}

//=================================================================================================

bool TDocStd_MultiTransactionManager::CommitCommand(const TCollection_ExtendedString& theName)
{
  bool isCommited = CommitCommand();
  if (isCommited && myUndos.Length())
    myUndos.First()->SetName(theName);
  return isCommited;
}

//=================================================================================================

void TDocStd_MultiTransactionManager::DumpTransaction(Standard_OStream& anOS) const
{
  int i;
  if (myDocuments.Length() == 0)
    anOS << "Manager is empty" << std::endl;
  else
  {
    if (myDocuments.Length() == 1)
      anOS << "There is one document ( ";
    else
      anOS << "There are " << myDocuments.Length() << " documents ( ";
    for (i = 1; i <= myDocuments.Length(); i++)
    {
      occ::handle<Standard_Transient> aDoc(myDocuments.Value(i));
      anOS << "\"" << aDoc.get();
      anOS << "\" ";
    }
    anOS << ") in the manager " << std::endl;

    if (myIsNestedTransactionMode)
      anOS << "Nested transaction mode is on" << std::endl;
    else
      anOS << "Nested transaction mode is off" << std::endl;

    anOS << " " << std::endl;
  }

  for (i = myUndos.Length(); i > 0; i--)
  {
    occ::handle<TDocStd_ApplicationDelta> delta = myUndos.Value(i);
    anOS << " Undo: ";
    delta->Dump(anOS);
    if (i == 1)
    {
      anOS << "  < Last action" << std::endl;
    }
    else
    {
      anOS << std::endl;
    }
  }
  for (i = 1; i <= myRedos.Length(); i++)
  {
    occ::handle<TDocStd_ApplicationDelta> delta = myRedos.Value(i);
    anOS << " Redo: ";
    delta->Dump(anOS);
    anOS << std::endl;
  }
}

//=================================================================================================

void TDocStd_MultiTransactionManager::RemoveLastUndo()
{
  if (myUndos.Length() == 0)
    return;
  const NCollection_Sequence<occ::handle<TDocStd_Document>>& docs = myUndos.Last()->GetDocuments();
  int                                                        i;
  for (i = 1; i <= docs.Length(); i++)
  {
    docs.Value(i)->RemoveFirstUndo();
  }
  myUndos.Remove(myUndos.Length());
}

//=================================================================================================

void TDocStd_MultiTransactionManager::AddDocument(const occ::handle<TDocStd_Document>& theDoc)
{
  int i;
  for (i = myDocuments.Length(); i > 0; i--)
    if (myDocuments.Value(i) == theDoc)
      return; // the document is already added to the list

  if (theDoc->IsNestedTransactionMode() != myIsNestedTransactionMode)
    theDoc->SetNestedTransactionMode(myIsNestedTransactionMode);

  theDoc->SetModificationMode(myOnlyTransactionModification);

  myDocuments.Append(theDoc);
  theDoc->SetUndoLimit(myUndoLimit);
  if (myOpenTransaction)
  {
    if (!theDoc->HasOpenCommand())
      theDoc->OpenCommand();
  }
  else
  {
    if (theDoc->HasOpenCommand())
      theDoc->CommitCommand();
  }
  theDoc->ClearUndos();
  theDoc->ClearRedos();
}

//=================================================================================================

void TDocStd_MultiTransactionManager::RemoveDocument(const occ::handle<TDocStd_Document>& theDoc)
{
  int i;
  for (i = myDocuments.Length(); i > 0; i--)
  {
    if (myDocuments.Value(i) == theDoc)
      myDocuments.Remove(i);
  }
  for (i = myUndos.Length(); i > 0; i--)
  {
    occ::handle<TDocStd_ApplicationDelta>                delta = myUndos.Value(i);
    NCollection_Sequence<occ::handle<TDocStd_Document>>& docs  = delta->GetDocuments();
    for (int j = docs.Length(); j > 0; j--)
    {
      if (docs.Value(j) == theDoc)
      {
        docs.Remove(j);
        if (docs.Length() == 0)
          myUndos.Remove(i);
      }
    }
  }
  for (i = myRedos.Length(); i > 0; i--)
  {
    occ::handle<TDocStd_ApplicationDelta>                delta = myRedos.Value(i);
    NCollection_Sequence<occ::handle<TDocStd_Document>>& docs  = delta->GetDocuments();
    for (int j = docs.Length(); j > 0; j--)
    {
      if (docs.Value(j) == theDoc)
      {
        docs.Remove(j);
        if (docs.Length() == 0)
          myRedos.Remove(i);
      }
    }
  }
}

//=================================================================================================

void TDocStd_MultiTransactionManager::SetNestedTransactionMode(const bool isAllowed)
{
  myIsNestedTransactionMode = isAllowed;
  int i;
  for (i = myDocuments.Length(); i > 0; i--)
  {
    if (myDocuments.Value(i)->IsNestedTransactionMode() != myIsNestedTransactionMode)
      myDocuments.Value(i)->SetNestedTransactionMode(myIsNestedTransactionMode);
  }
}

//=======================================================================
// function : SetModificationMode
// purpose  : if theTransactionOnly is True changes is denied outside transactions
//=======================================================================

void TDocStd_MultiTransactionManager::SetModificationMode(const bool theTransactionOnly)
{
  myOnlyTransactionModification = theTransactionOnly;

  int i;
  for (i = myDocuments.Length(); i > 0; i--)
  {
    myDocuments.Value(i)->SetModificationMode(myOnlyTransactionModification);
  }
}

//=================================================================================================

void TDocStd_MultiTransactionManager::ClearUndos()
{
  AbortCommand();

  myUndos.Clear();
  int i;
  for (i = myDocuments.Length(); i > 0; i--)
  {
    myDocuments.Value(i)->ClearUndos();
  }
}

//=================================================================================================

void TDocStd_MultiTransactionManager::ClearRedos()
{
  AbortCommand();

  myRedos.Clear();
  int i;
  for (i = myDocuments.Length(); i > 0; i--)
  {
    myDocuments.Value(i)->ClearRedos();
  }
}
