// Created by: DAUTRY Philippe
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


#include <NCollection_IncAllocator.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_AttributeIndexedMap.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_Delta.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_DeltaOnForget.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_DeltaOnRemoval.hxx>
#include <TDF_DeltaOnResume.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelNode.hxx>
#include <TDF_LabelNodePtr.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Transaction.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDF_Data,Standard_Transient)

#undef DEB_DELTA_CREATION
#undef TDF_DATA_COMMIT_OPTIMIZED

#ifdef OCCT_DEBUG_DELTA
#define TDF_Data_DebugModified(ACTION) \
  cout<<"After "<<ACTION<<" #"<<myTransaction+1<<", DF "<<this<<" had "<<myNbTouchedAtt<<" attribute(s) touched. Time = "<<myTime<<endl; \
if (!myTransaction) { \
  TCollection_AsciiString entry; \
  for (TDF_ChildIterator itr(Root(),Standard_True); itr.More(); itr.Next()) { \
    const TDF_LabelNode* lnp = itr.Value().myLabelNode; \
    if (lnp->AttributesModified() || lnp->MayBeModified()) { \
      TDF_Tool::Entry(itr.Value(),entry); \
      cout<<ACTION<<" on "<<entry<<" : flag(s) "; \
      if (lnp->AttributesModified()) cout<<"AttributesModified "; \
      if (lnp->MayBeModified()) cout<<"MayBeModified already set in transaction 0! Please contact TDF developer."; \
      cout<<endl; \
      cout<<itr.Value()<<endl; \
      entry.Clear(); \
    }}}
#else
#define TDF_Data_DebugModified(ACTION)
#endif

#ifdef OCCT_DEBUG_DELTA_CREATION
#define TDF_DataDebugDeltaCreation(DELTATYPE) \
{ \
TCollection_AsciiString entry; \
TDF_Tool::Entry(currentAtt->Label(),entry); \
cout<<"Creation of a DeltaOn"<<DELTATYPE<<" \tat "<<entry<<" \ton "<<currentAtt->DynamicType()<<endl; \
}
#else
#define TDF_DataDebugDeltaCreation(DELTATYPE)
#endif

#define TDF_Data_DeltaCreation(DELTACOMMENT,DELTACREATION) \
if (withDelta) { \
  TDF_DataDebugDeltaCreation(DELTACOMMENT); \
  aDelta->AddAttributeDelta(DELTACREATION); \
}

//=======================================================================
//function : TDF_Data
//purpose  : empty constructor
//=======================================================================

TDF_Data::TDF_Data() :
myTransaction           (0),
myNbTouchedAtt          (0),
myNotUndoMode           (Standard_True),
myTime                  (0),
myAllowModification     (Standard_True)
{
  const Handle(NCollection_IncAllocator) anIncAllocator=
    new NCollection_IncAllocator (16000);
  myLabelNodeAllocator = anIncAllocator;
  myRoot = new (anIncAllocator) TDF_LabelNode (this);
}

//=======================================================================
//function : Destroy
//purpose  : Used to implement the destructor ~.
//=======================================================================

void TDF_Data::Destroy()
{
  AbortUntilTransaction(1);
  myRoot->Destroy (myLabelNodeAllocator);
  myRoot = NULL;
}


//=======================================================================
//function : OpenTransaction
//purpose  : 
//=======================================================================

Standard_Integer TDF_Data::OpenTransaction() 
{
  myTimes.Prepend(myTime);
  return ++myTransaction;
}


//=======================================================================
//function : CommitTransaction
//purpose  : Commits the current transaction.
//=======================================================================

Handle(TDF_Delta) TDF_Data::CommitTransaction
(const Standard_Boolean withDelta) 
{
  Handle(TDF_Delta) delta;
  if (myTransaction>0) {
    if (withDelta) delta = new TDF_Delta();
#ifdef OCCT_DEBUG_DELTA
    cout<<"TDF_Data::Begin Commit #"<<myTransaction<<endl;    
#endif
#ifdef TDF_DATA_COMMIT_OPTIMIZED
    myNbTouchedAtt = 0;
    if (Root().myLabelNode->MayBeModified())
#endif
      myNbTouchedAtt =
        TDF_Data::CommitTransaction(Root(),delta,withDelta);

    if (myNbTouchedAtt && !(withDelta && delta->IsEmpty())) ++myTime;
    --myTransaction;
    if (withDelta) {
      if (!delta->IsEmpty()) {
        delta->Validity(myTimes.First(),myTime);
#ifdef OCCT_DEBUG_DELTA
        if (myTransaction == 0) {
          cout<<"TDF_Data::Commit generated this delta in t=0:"<<endl;
          delta->Dump(cout);
        }
#endif
      }
#ifdef OCCT_DEBUG_DELTA
      else {
        if (myTransaction == 0)
          cout<<"TDF_Data::Commit generated NO delta."<<endl;
      }
#endif
    }
    myTimes.RemoveFirst();
  }
  TDF_Data_DebugModified("COMMIT");
  return delta;
}


//=======================================================================
//function : CommitUntilTransaction
//purpose  : Commits the transactions until AND including
//           the given transaction index.
//=======================================================================

Handle(TDF_Delta) TDF_Data::CommitUntilTransaction
(const Standard_Integer untilTransaction,
 const Standard_Boolean withDelta)
{
  Handle(TDF_Delta) delta;
  if ((untilTransaction>0) && (myTransaction >= untilTransaction)) {
    while (myTransaction > untilTransaction) {
      delta = TDF_Data::CommitTransaction(Standard_False);
    }
    delta = TDF_Data::CommitTransaction(withDelta);
  }
  return delta;
}


//=======================================================================
//function : CommitTransaction
//purpose  : Recursive method used to implement the commit action.
//=======================================================================

Standard_Integer TDF_Data::CommitTransaction
(const TDF_Label& aLabel,
 const Handle(TDF_Delta)& aDelta,
 const Standard_Boolean withDelta)
{
  aLabel.myLabelNode->MayBeModified(Standard_False);
  Standard_Integer nbTouchedAtt = 0;
#ifdef TDF_DATA_COMMIT_OPTIMIZED
  Standard_Boolean attMod = aLabel.myLabelNode->AttributesModified();
#else
  Standard_Boolean attMod = Standard_True;
#endif

  if (attMod) {
    Handle(TDF_Attribute)    lastAtt;
    Handle(TDF_Attribute)  backupAtt;
    Standard_Boolean currentIsRemoved = Standard_False;
    attMod = Standard_False;

    TDF_AttributeIterator itr1(aLabel, Standard_False);
    while (itr1.More()) {
      Handle(TDF_Attribute) aPtrCurrentAtt = itr1.Value();
      itr1.Next();
      //      currentAtt = itr1.Value();

      // A callback:
      aPtrCurrentAtt->BeforeCommitTransaction();

      backupAtt  = aPtrCurrentAtt->myBackup;

      if (aPtrCurrentAtt->myTransaction == myTransaction) {
        ++nbTouchedAtt;
        --(aPtrCurrentAtt->myTransaction);

        // ------------------------------------------------------- Forgotten
        if (aPtrCurrentAtt->IsForgotten()) {
          if (aPtrCurrentAtt->mySavedTransaction >=
              aPtrCurrentAtt->myTransaction)
          {
            const Handle(TDF_Attribute) currentAtt = aPtrCurrentAtt;
            // Collision with a not forgotten version.
            if (backupAtt.IsNull()) {
              TDF_Data_DeltaCreation
                ("Removal(1)",
                 currentAtt->DeltaOnRemoval());
              if (myNotUndoMode) currentAtt->BeforeRemoval();
              aLabel.myLabelNode->RemoveAttribute(lastAtt,currentAtt);
              currentIsRemoved = Standard_True;
              attMod = Standard_True;
            }
            else {
              // Modified then Forgotten...
              // Forgotten flag spreading?
              currentAtt->Resume();
              currentAtt->Restore(backupAtt);
              currentAtt->myTransaction = backupAtt->myTransaction;
              currentAtt->RemoveBackup();
              backupAtt = currentAtt->myBackup;
              if (myTransaction == 1) {
                TDF_Data_DeltaCreation
                  ("Removal(2)",
                   currentAtt->DeltaOnRemoval());
                if (myNotUndoMode) currentAtt->BeforeRemoval();
                aLabel.myLabelNode->RemoveAttribute(lastAtt,currentAtt);
                currentIsRemoved = Standard_True;
              }
              else {
                // BeforeForget has already been called once.
                // if (myNotUndoMode) currentAtt->BeforeForget();
                currentAtt->Forget(myTransaction-1);
                TDF_Data_DeltaCreation
                  ("Forget(1)",
                   currentAtt->DeltaOnForget());
                attMod = Standard_True;
              }
            }
          }
          else {
            // Forgotten in lower transaction than the current one.
            TDF_Data_DeltaCreation
              ("Forget(2)",
               aPtrCurrentAtt->DeltaOnForget());
          }
        }
        // ---------------------------------------------------------- Resumed.
        else if (aPtrCurrentAtt->mySavedTransaction < 0) {
          TDF_Data_DeltaCreation
            ("Resume",
             aPtrCurrentAtt->DeltaOnResume());
          aPtrCurrentAtt->mySavedTransaction = 0;
          attMod = attMod || (aPtrCurrentAtt->myTransaction > 0);
        }

        // ------------------------------------------------------------ Added.
        else if (backupAtt.IsNull()) {
          TDF_Data_DeltaCreation
            ("Addition",
             aPtrCurrentAtt->DeltaOnAddition());
          attMod = attMod || (aPtrCurrentAtt->myTransaction > 0);
        }
        // --------------------------------------------------------- Modified.
        else {
          const TDF_Attribute* anAttrPtr = aPtrCurrentAtt.operator->(); // to avoid ambiguity
          TDF_Data_DeltaCreation
            ("Modification",
             anAttrPtr->DeltaOnModification(backupAtt));
          if (aPtrCurrentAtt->myTransaction == backupAtt->myTransaction)
            aPtrCurrentAtt->RemoveBackup();
          attMod = attMod || (aPtrCurrentAtt->myTransaction > 0);
        }

      }
      else attMod = attMod || (aPtrCurrentAtt->myTransaction > 0);

      if (currentIsRemoved) currentIsRemoved = Standard_False;
      else lastAtt = aPtrCurrentAtt;
    }
    aLabel.myLabelNode->AttributesModified(attMod);
  }

  // Iteration on the children to do the same!
  //------------------------------------------
  for (TDF_ChildIterator itr2(aLabel); itr2.More(); itr2.Next()) {
#ifdef TDF_DATA_COMMIT_OPTIMIZED
    if (itr2.Value().myLabelNode->MayBeModified())
#endif
      nbTouchedAtt +=
        TDF_Data::CommitTransaction(itr2.Value(),aDelta,withDelta);
  }

  return nbTouchedAtt;
}


//=======================================================================
//function : AbortTransaction
//purpose  : Aborts the current transaction.
//=======================================================================

void TDF_Data::AbortTransaction() 
{
  if (myTransaction>0)
    Undo(TDF_Data::CommitTransaction(Standard_True),Standard_False);
  TDF_Data_DebugModified("New ABORT");
}


//=======================================================================
//function : AbortUntilTransaction
//purpose  : Aborts the transactions until AND including the given index.
//=======================================================================

void TDF_Data::AbortUntilTransaction(const Standard_Integer untilTransaction)
{
  if (untilTransaction>0)
    Undo(TDF_Data::CommitUntilTransaction(untilTransaction,Standard_True),Standard_False);
}


//=======================================================================
//function : IsApplicable
//purpose  : 
//=======================================================================

Standard_Boolean TDF_Data::IsApplicable
(const Handle(TDF_Delta)& aDelta) const
{
  return !aDelta.IsNull() && aDelta->IsApplicable(myTime);
}


//=======================================================================
//function : Undo
//purpose  : Applies a delta to undo  actions.
//=======================================================================

Handle(TDF_Delta) TDF_Data::Undo
(const Handle(TDF_Delta)& aDelta,
 const Standard_Boolean withDelta)
{
  Handle(TDF_Delta) newDelta;
  if (!aDelta.IsNull ()) {
    if (aDelta->IsApplicable(myTime)) {
      if (withDelta) OpenTransaction();
#ifdef OCCT_DEBUG_DELTA
      cout<<"TDF_Data::Undo applies this delta:"<<endl;
      aDelta->Dump(cout);
#endif
      aDelta->BeforeOrAfterApply(Standard_True);
      myNotUndoMode = Standard_False;
      aDelta->Apply ();
      myNotUndoMode = Standard_True;
      if (withDelta) {
        newDelta = CommitTransaction(Standard_True);
        newDelta->Validity(aDelta->EndTime(),aDelta->BeginTime());
#ifdef OCCT_DEBUG_DELTA
        cout<<"TDF_Data::Undo, after validity correction, Delta is now available from time \t#"<<newDelta->BeginTime()<<" to time \t#"<<newDelta->EndTime()<<endl;
#endif
      }
      myTime = aDelta->BeginTime();
      aDelta->BeforeOrAfterApply(Standard_False);
    }
  }
  return newDelta;
}



//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDF_Data::Dump(Standard_OStream& anOS) const
{
  anOS<<"Dump of a TDF_Data."<<endl;
  anOS<<"Current transaction: "<<myTransaction;
  anOS<<"; Current tick: "<<myTime<<";"<<endl;
  return anOS;
}
