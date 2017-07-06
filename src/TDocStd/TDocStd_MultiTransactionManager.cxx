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


#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDocStd_ApplicationDelta.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_MultiTransactionManager.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDocStd_MultiTransactionManager,Standard_Transient)

//=======================================================================
//function : TDocStd_MultiTransactionManager
//purpose  : Constructor
//=======================================================================
TDocStd_MultiTransactionManager::TDocStd_MultiTransactionManager ()
{
  myUndoLimit = 0;
  myOpenTransaction = Standard_False;
  myIsNestedTransactionMode = Standard_False;
  myOnlyTransactionModification = Standard_False;
}

//=======================================================================
//function : SetUndoLimit
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::SetUndoLimit(const Standard_Integer theLimit)
{
  myUndoLimit = theLimit;

  CommitCommand ();
  
  Standard_Integer n = myUndos.Length() - myUndoLimit;
  while (n > 0) {
    RemoveLastUndo();
    --n;
  }

  Standard_Integer i;
  for(i = myDocuments.Length(); i > 0; i--)
    myDocuments.Value(i)->SetUndoLimit(myUndoLimit);

}

//=======================================================================
//function : Undo
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::Undo()
{
  if (myUndos.IsEmpty()) return;
  const TDocStd_SequenceOfDocument& docs = myUndos.First()->GetDocuments();
  Standard_Integer i;
  for (i = docs.Length(); i > 0; i--) {
    Handle(TDocStd_Document) doc = docs.Value(i);
    if (doc.IsNull() || doc->GetAvailableUndos() == 0) continue;
    doc->Undo();
  }
  myRedos.Prepend(myUndos.First());
  myUndos.Remove(1);
  myOpenTransaction = Standard_False;
}

//=======================================================================
//function : Redo
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::Redo() {
  if (myRedos.IsEmpty()) return;
  const TDocStd_SequenceOfDocument& docs = myRedos.First()->GetDocuments();
  Standard_Integer i;
  for (i = docs.Length(); i > 0; i--) {
    Handle(TDocStd_Document) doc = docs.Value(i);
    if (doc.IsNull() || doc->GetAvailableRedos() == 0) continue;
    doc->Redo();
  }
  myUndos.Prepend(myRedos.First());
  myRedos.Remove(1);
  myOpenTransaction = Standard_False;
}

//=======================================================================
//function : OpenCommand
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::OpenCommand() {
  if (myOpenTransaction) {
#ifdef OCCT_DEBUG
    cout << "TDocStd_MultiTransactionManager::OpenCommand(): "
            "Can't start new application transaction while a "
            "previous one is not commited or aborted" << endl;
#endif
    throw Standard_Failure("Can't start new application transaction"
                            "while a previous one is not commited or aborted");
  }
  myOpenTransaction = Standard_True;
  Standard_Integer i;
  for(i = myDocuments.Length(); i > 0; i--) {
    while(myDocuments.Value(i)->HasOpenCommand())
      myDocuments.Value(i)->AbortCommand();
    myDocuments.Value(i)->OpenCommand();
  }
}

//=======================================================================
//function : AbortCommand
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::AbortCommand() {
  myOpenTransaction = Standard_False;
  Standard_Integer i;
  for(i = myDocuments.Length(); i > 0; i--) {
    while(myDocuments.Value(i)->HasOpenCommand())
      myDocuments.Value(i)->AbortCommand();
  }
}

//=======================================================================
//function : CommitCommand
//purpose  : 
//=======================================================================

Standard_Boolean TDocStd_MultiTransactionManager::CommitCommand()
{
  Handle(TDocStd_ApplicationDelta) aDelta = new TDocStd_ApplicationDelta;
  Standard_Boolean isCommited = Standard_False;
  Standard_Integer i;
  for(i = myDocuments.Length(); i > 0; i--) {
    isCommited = Standard_False;
    while(myDocuments.Value(i)->HasOpenCommand())
      if (myDocuments.Value(i)->CommitCommand())
        isCommited = Standard_True;
    if(isCommited) {
      aDelta->GetDocuments().Append(myDocuments.Value(i));
    }
  }
  
  if (aDelta->GetDocuments().Length()) {
    myUndos.Prepend(aDelta);
    if (myUndos.Length() > myUndoLimit) {
      RemoveLastUndo();
    }
    myRedos.Clear();
    isCommited = Standard_True;
  }
  myOpenTransaction = Standard_False;
  return isCommited;
}

//=======================================================================
//function : CommitCommand
//purpose  : 
//=======================================================================

Standard_Boolean TDocStd_MultiTransactionManager::CommitCommand
                        (const TCollection_ExtendedString& theName)
{
  Standard_Boolean isCommited = CommitCommand();
  if (isCommited && myUndos.Length())
    myUndos.First()->SetName(theName);
  return isCommited;
}

//=======================================================================
//function : DumpTransaction
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::DumpTransaction(Standard_OStream& anOS) const
{
  Standard_Integer i;
  if(myDocuments.Length() == 0)
    anOS << "Manager is empty" << endl;
  else {
    if(myDocuments.Length() == 1)
      anOS << "There is one document ( ";
    else
      anOS << "There are " << myDocuments.Length() << " documents ( ";
    for(i = 1; i <= myDocuments.Length(); i++) {
      Handle(Standard_Transient) aDoc (myDocuments.Value(i));
      anOS << "\"" << aDoc.get();
      anOS << "\" ";
    }
    anOS << ") in the manager "  << endl;

    if(myIsNestedTransactionMode)
      anOS << "Nested transaction mode is on" << endl;
    else
      anOS << "Nested transaction mode is off" << endl;

    anOS << " " << endl;
  }

  for (i = myUndos.Length(); i > 0; i--) {
    Handle(TDocStd_ApplicationDelta) delta = myUndos.Value(i);
    anOS<<" Undo: ";
    delta->Dump(anOS);
    if (i == 1) {
      anOS<<"  < Last action"<<endl;
    } else {
      anOS<<endl;
    }
  }
  for (i = 1; i <= myRedos.Length(); i++) {
    Handle(TDocStd_ApplicationDelta) delta = myRedos.Value(i);
    anOS<<" Redo: ";
    delta->Dump(anOS);
    anOS<<endl;
  }
}

//=======================================================================
//function : RemoveLastUndo
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::RemoveLastUndo()
{
  if(myUndos.Length() == 0) return;
  const TDocStd_SequenceOfDocument& docs = myUndos.Last()->GetDocuments();
  Standard_Integer i;
  for (i = 1; i <= docs.Length(); i++) {
    docs.Value(i)->RemoveFirstUndo();
  }
  myUndos.Remove(myUndos.Length());
}

//=======================================================================
//function : AddDocument
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::AddDocument
  (const Handle(TDocStd_Document)& theDoc)
{
  Standard_Integer i;
  for(i = myDocuments.Length(); i > 0; i--)
    if(myDocuments.Value(i) == theDoc)
      return; // the document is already added to the list

  if(theDoc->IsNestedTransactionMode() !=
     myIsNestedTransactionMode)
    theDoc->SetNestedTransactionMode(myIsNestedTransactionMode);

  theDoc->SetModificationMode(myOnlyTransactionModification);
  
  myDocuments.Append(theDoc);
  theDoc->SetUndoLimit(myUndoLimit);
  if(myOpenTransaction) {
    if(!theDoc->HasOpenCommand())
      theDoc->OpenCommand();
  }
  else {
    if(theDoc->HasOpenCommand())
      theDoc->CommitCommand();
  }
  theDoc->ClearUndos();
  theDoc->ClearRedos();
}

//=======================================================================
//function : RemoveDocument
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::RemoveDocument
  (const Handle(TDocStd_Document)& theDoc)
{
  Standard_Integer i;
  for(i = myDocuments.Length(); i > 0; i--) {
    if(myDocuments.Value(i) == theDoc)
      myDocuments.Remove(i);
  }
  for (i = myUndos.Length(); i > 0; i--) {
    Handle(TDocStd_ApplicationDelta) delta = myUndos.Value(i);
    TDocStd_SequenceOfDocument& docs = delta->GetDocuments();
    for(Standard_Integer j = docs.Length(); j > 0; j--) {
      if(docs.Value(j) == theDoc) {
        docs.Remove(j);
        if(docs.Length() == 0)
          myUndos.Remove(i);
      }
    }
  }
  for (i = myRedos.Length(); i > 0; i--) {
    Handle(TDocStd_ApplicationDelta) delta = myRedos.Value(i);
    TDocStd_SequenceOfDocument& docs = delta->GetDocuments();
    for(Standard_Integer j = docs.Length(); j > 0; j--) {
      if(docs.Value(j) == theDoc) {
        docs.Remove(j);
        if(docs.Length() == 0)
          myRedos.Remove(i);
      }
    }
  }
}

//=======================================================================
//function : SetNestedTransactionMode
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::SetNestedTransactionMode
  (const Standard_Boolean isAllowed)
{
  myIsNestedTransactionMode = isAllowed;
  Standard_Integer i;
  for(i = myDocuments.Length(); i > 0; i--) {
    if(myDocuments.Value(i)->IsNestedTransactionMode() != myIsNestedTransactionMode)
      myDocuments.Value(i)->SetNestedTransactionMode(myIsNestedTransactionMode);
  }
}

//=======================================================================
//function : SetModificationMode
//purpose  : if theTransactionOnly is True changes is denied outside transactions
//=======================================================================

void TDocStd_MultiTransactionManager::SetModificationMode
  (const Standard_Boolean theTransactionOnly)
{
  myOnlyTransactionModification = theTransactionOnly;

  Standard_Integer i;
  for(i = myDocuments.Length(); i > 0; i--) {
    myDocuments.Value(i)->SetModificationMode(myOnlyTransactionModification);
  }
}

//=======================================================================
//function : ClearUndos
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::ClearUndos()
{
  AbortCommand();

  myUndos.Clear();
  Standard_Integer i;
  for(i = myDocuments.Length(); i > 0; i--) {
    myDocuments.Value(i)->ClearUndos();
  }
}

//=======================================================================
//function : ClearRedos
//purpose  : 
//=======================================================================

void TDocStd_MultiTransactionManager::ClearRedos()
{
  AbortCommand();

  myRedos.Clear();
  Standard_Integer i;
  for(i = myDocuments.Length(); i > 0; i--) {
    myDocuments.Value(i)->ClearRedos();
  }
}



