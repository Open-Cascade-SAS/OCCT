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
#include <Standard_Dump.hxx>
#include <Standard_Type.hxx>
#include <Standard_GUID.hxx>
#include <NCollection_Array1.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_AttributeDelta.hxx>
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
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDF_Data, Standard_Transient)

#undef DEB_DELTA_CREATION
#define TDF_DATA_COMMIT_OPTIMIZED

#ifdef OCCT_DEBUG_DELTA
  #define TDF_Data_DebugModified(ACTION)                                                           \
    std::cout << "After " << ACTION << " #" << myTransaction + 1 << ", DF " << this << " had "     \
              << myNbTouchedAtt << " attribute(s) touched. Time = " << myTime << std::endl;        \
    if (!myTransaction)                                                                            \
    {                                                                                              \
      TCollection_AsciiString entry;                                                               \
      for (TDF_ChildIterator itr(Root(), true); itr.More(); itr.Next())                   \
      {                                                                                            \
        const TDF_LabelNode* lnp = itr.Value().myLabelNode;                                        \
        if (lnp->AttributesModified() || lnp->MayBeModified())                                     \
        {                                                                                          \
          TDF_Tool::Entry(itr.Value(), entry);                                                     \
          std::cout << ACTION << " on " << entry << " : flag(s) ";                                 \
          if (lnp->AttributesModified())                                                           \
            std::cout << "AttributesModified ";                                                    \
          if (lnp->MayBeModified())                                                                \
            std::cout                                                                              \
              << "MayBeModified already set in transaction 0! Please contact TDF developer.";      \
          std::cout << std::endl;                                                                  \
          std::cout << itr.Value() << std::endl;                                                   \
          entry.Clear();                                                                           \
        }                                                                                          \
      }                                                                                            \
    }
#else
  #define TDF_Data_DebugModified(ACTION)
#endif

#ifdef OCCT_DEBUG_DELTA_CREATION
  #define TDF_DataDebugDeltaCreation(DELTATYPE)                                                    \
    {                                                                                              \
      TCollection_AsciiString entry;                                                               \
      TDF_Tool::Entry(currentAtt->Label(), entry);                                                 \
      std::cout << "Creation of a DeltaOn" << DELTATYPE << " \tat " << entry << " \ton "           \
                << currentAtt->DynamicType() << std::endl;                                         \
    }
#else
  #define TDF_DataDebugDeltaCreation(DELTATYPE)
#endif

#define TDF_Data_DeltaCreation(DELTACOMMENT, DELTACREATION)                                        \
  if (withDelta)                                                                                   \
  {                                                                                                \
    TDF_DataDebugDeltaCreation(DELTACOMMENT);                                                      \
    aDelta->AddAttributeDelta(DELTACREATION);                                                      \
  }

//=================================================================================================

TDF_Data::TDF_Data()
    : myTransaction(0),
      myNbTouchedAtt(0),
      myNotUndoMode(true),
      myTime(0),
      myAllowModification(true),
      myAccessByEntries(false)
{
  const occ::handle<NCollection_IncAllocator> anIncAllocator = new NCollection_IncAllocator(16000);
  myLabelNodeAllocator                                  = anIncAllocator;
  myRoot                                                = new (anIncAllocator) TDF_LabelNode(this);
}

//=======================================================================
// function : Destroy
// purpose  : Used to implement the destructor ~.
//=======================================================================

void TDF_Data::Destroy()
{
  AbortUntilTransaction(1);
  // Forget the Owner attribute from the root label to avoid referencing document before
  // desctuction of the framework (on custom attributes forget). Don't call ForgetAll because
  // it may call backup.
  while (!myRoot->FirstAttribute().IsNull())
  {
    static occ::handle<TDF_Attribute> anEmpty;
    occ::handle<TDF_Attribute>        aFirst = myRoot->FirstAttribute();
    myRoot->RemoveAttribute(anEmpty, aFirst);
  }
  myAccessByEntriesTable.Clear();
  myRoot->Destroy(myLabelNodeAllocator);
  myRoot = NULL;
}

//=================================================================================================

int TDF_Data::OpenTransaction()
{
  myTimes.Prepend(myTime);
  return ++myTransaction;
}

//=======================================================================
// function : CommitTransaction
// purpose  : Commits the current transaction.
//=======================================================================

occ::handle<TDF_Delta> TDF_Data::CommitTransaction(const bool withDelta)
{
  occ::handle<TDF_Delta> delta;
  if (myTransaction > 0)
  {
    if (withDelta)
      delta = new TDF_Delta();
#ifdef OCCT_DEBUG_DELTA
    std::cout << "TDF_Data::Begin Commit #" << myTransaction << std::endl;
#endif
#ifdef TDF_DATA_COMMIT_OPTIMIZED
    myNbTouchedAtt = 0;
    if (Root().myLabelNode->MayBeModified())
#endif
      myNbTouchedAtt = TDF_Data::CommitTransaction(Root(), delta, withDelta);

    if (myNbTouchedAtt && !(withDelta && delta->IsEmpty()))
      ++myTime;
    --myTransaction;
    if (withDelta)
    {
      if (!delta->IsEmpty())
      {
        delta->Validity(myTimes.First(), myTime);
#ifdef OCCT_DEBUG_DELTA
        if (myTransaction == 0)
        {
          std::cout << "TDF_Data::Commit generated this delta in t=0:" << std::endl;
          delta->Dump(std::cout);
        }
#endif
      }
#ifdef OCCT_DEBUG_DELTA
      else
      {
        if (myTransaction == 0)
          std::cout << "TDF_Data::Commit generated NO delta." << std::endl;
      }
#endif
    }
    myTimes.RemoveFirst();
  }
  TDF_Data_DebugModified("COMMIT");
  return delta;
}

//=======================================================================
// function : CommitUntilTransaction
// purpose  : Commits the transactions until AND including
//           the given transaction index.
//=======================================================================

occ::handle<TDF_Delta> TDF_Data::CommitUntilTransaction(const int untilTransaction,
                                                   const bool withDelta)
{
  occ::handle<TDF_Delta> delta;
  if ((untilTransaction > 0) && (myTransaction >= untilTransaction))
  {
    while (myTransaction > untilTransaction)
    {
      delta = TDF_Data::CommitTransaction(false);
    }
    delta = TDF_Data::CommitTransaction(withDelta);
  }
  return delta;
}

//=======================================================================
// function : CommitTransaction
// purpose  : Recursive method used to implement the commit action.
//=======================================================================

int TDF_Data::CommitTransaction(const TDF_Label&         aLabel,
                                             const occ::handle<TDF_Delta>& aDelta,
                                             const bool   withDelta)
{
  aLabel.myLabelNode->MayBeModified(false);
  int nbTouchedAtt = 0;
#ifdef TDF_DATA_COMMIT_OPTIMIZED
  bool attMod = aLabel.myLabelNode->AttributesModified();
#else
  bool attMod = true;
#endif

  if (attMod)
  {
    occ::handle<TDF_Attribute> lastAtt;
    occ::handle<TDF_Attribute> backupAtt;
    bool      currentIsRemoved = false;
    attMod                                 = false;

    TDF_AttributeIterator itr1(aLabel, false);
    while (itr1.More())
    {
      occ::handle<TDF_Attribute> aPtrCurrentAtt = itr1.Value();
      itr1.Next();
      //      currentAtt = itr1.Value();

      // A callback:
      aPtrCurrentAtt->BeforeCommitTransaction();

      backupAtt = aPtrCurrentAtt->myBackup;

      if (aPtrCurrentAtt->myTransaction == myTransaction)
      {
        ++nbTouchedAtt;
        --(aPtrCurrentAtt->myTransaction);

        // ------------------------------------------------------- Forgotten
        if (aPtrCurrentAtt->IsForgotten())
        {
          if (aPtrCurrentAtt->mySavedTransaction >= aPtrCurrentAtt->myTransaction)
          {
            const occ::handle<TDF_Attribute>& currentAtt = aPtrCurrentAtt;
            // Collision with a not forgotten version.
            if (backupAtt.IsNull())
            {
              TDF_Data_DeltaCreation("Removal(1)", currentAtt->DeltaOnRemoval());
              if (myNotUndoMode)
                currentAtt->BeforeRemoval();
              aLabel.myLabelNode->RemoveAttribute(lastAtt, currentAtt);
              currentIsRemoved = true;
              attMod           = true;
            }
            else
            {
              // Modified then Forgotten...
              // Forgotten flag spreading?
              currentAtt->Resume();
              currentAtt->Restore(backupAtt);
              currentAtt->myTransaction = backupAtt->myTransaction;
              currentAtt->RemoveBackup();
              backupAtt = currentAtt->myBackup;
              if (myTransaction == 1)
              {
                TDF_Data_DeltaCreation("Removal(2)", currentAtt->DeltaOnRemoval());
                if (myNotUndoMode)
                  currentAtt->BeforeRemoval();
                aLabel.myLabelNode->RemoveAttribute(lastAtt, currentAtt);
                currentIsRemoved = true;
              }
              else
              {
                // BeforeForget has already been called once.
                // if (myNotUndoMode) currentAtt->BeforeForget();
                currentAtt->Forget(myTransaction - 1);
                TDF_Data_DeltaCreation("Forget(1)", currentAtt->DeltaOnForget());
                attMod = true;
              }
            }
          }
          else
          {
            // Forgotten in lower transaction than the current one.
            TDF_Data_DeltaCreation("Forget(2)", aPtrCurrentAtt->DeltaOnForget());
          }
        }
        // ---------------------------------------------------------- Resumed.
        else if (aPtrCurrentAtt->mySavedTransaction < 0)
        {
          TDF_Data_DeltaCreation("Resume", aPtrCurrentAtt->DeltaOnResume());
          aPtrCurrentAtt->mySavedTransaction = 0;
          attMod                             = attMod || (aPtrCurrentAtt->myTransaction > 0);
        }

        // ------------------------------------------------------------ Added.
        else if (backupAtt.IsNull())
        {
          TDF_Data_DeltaCreation("Addition", aPtrCurrentAtt->DeltaOnAddition());
          attMod = attMod || (aPtrCurrentAtt->myTransaction > 0);
        }
        // --------------------------------------------------------- Modified.
        else
        {
          const TDF_Attribute* anAttrPtr = aPtrCurrentAtt.operator->(); // to avoid ambiguity
          TDF_Data_DeltaCreation("Modification", anAttrPtr->DeltaOnModification(backupAtt));
          if (aPtrCurrentAtt->myTransaction == backupAtt->myTransaction)
            aPtrCurrentAtt->RemoveBackup();
          attMod = attMod || (aPtrCurrentAtt->myTransaction > 0);
        }
      }
      else
        attMod = attMod || (aPtrCurrentAtt->myTransaction > 0);

      if (currentIsRemoved)
        currentIsRemoved = false;
      else
        lastAtt = aPtrCurrentAtt;
    }
    aLabel.myLabelNode->AttributesModified(attMod);
  }

  // Iteration on the children to do the same!
  //------------------------------------------
  for (TDF_ChildIterator itr2(aLabel); itr2.More(); itr2.Next())
  {
#ifdef TDF_DATA_COMMIT_OPTIMIZED
    if (itr2.Value().myLabelNode->MayBeModified())
#endif
      nbTouchedAtt += TDF_Data::CommitTransaction(itr2.Value(), aDelta, withDelta);
  }

  return nbTouchedAtt;
}

//=======================================================================
// function : AbortTransaction
// purpose  : Aborts the current transaction.
//=======================================================================

void TDF_Data::AbortTransaction()
{
  if (myTransaction > 0)
    Undo(TDF_Data::CommitTransaction(true), false);
  TDF_Data_DebugModified("New ABORT");
}

//=======================================================================
// function : AbortUntilTransaction
// purpose  : Aborts the transactions until AND including the given index.
//=======================================================================

void TDF_Data::AbortUntilTransaction(const int untilTransaction)
{
  if (untilTransaction > 0)
    Undo(TDF_Data::CommitUntilTransaction(untilTransaction, true), false);
}

//=================================================================================================

bool TDF_Data::IsApplicable(const occ::handle<TDF_Delta>& aDelta) const
{
  return !aDelta.IsNull() && aDelta->IsApplicable(myTime);
}

//=================================================================================================

void TDF_Data::FixOrder(const occ::handle<TDF_Delta>& theDelta)
{
  // make all OnRemoval (which will cause addition of the attribute) are in the end
  // to do not put two attributes with the same GUID at one label during undo/redo
  NCollection_List<occ::handle<TDF_AttributeDelta>> anOrderedList;

  const NCollection_List<occ::handle<TDF_AttributeDelta>>&        attList = theDelta->AttributeDeltas();
  NCollection_List<occ::handle<TDF_AttributeDelta>>::Iterator anIt(attList);
  for (; anIt.More(); anIt.Next())
  { // append not-removal
    occ::handle<TDF_AttributeDelta> attDelta = anIt.Value();
    if (!attDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnRemoval)))
    {
      anOrderedList.Append(attDelta);
    }
  }
  for (anIt.Initialize(attList); anIt.More(); anIt.Next())
  { // append removal
    occ::handle<TDF_AttributeDelta> attDelta = anIt.Value();
    if (attDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnRemoval)))
    {
      anOrderedList.Append(attDelta);
    }
  }
  theDelta->ReplaceDeltaList(anOrderedList);
}

//=======================================================================
// function : Undo
// purpose  : Applies a delta to undo  actions.
//=======================================================================

occ::handle<TDF_Delta> TDF_Data::Undo(const occ::handle<TDF_Delta>& aDelta, const bool withDelta)
{
  occ::handle<TDF_Delta> newDelta;
  if (!aDelta.IsNull())
  {
    if (aDelta->IsApplicable(myTime))
    {
      if (withDelta)
        OpenTransaction();
#ifdef OCCT_DEBUG_DELTA
      std::cout << "TDF_Data::Undo applies this delta:" << std::endl;
      aDelta->Dump(std::cout);
#endif
      aDelta->BeforeOrAfterApply(true);
      myNotUndoMode = false;
      FixOrder(aDelta);
      aDelta->Apply();
      myNotUndoMode = true;
      if (withDelta)
      {
        newDelta = CommitTransaction(true);
        newDelta->Validity(aDelta->EndTime(), aDelta->BeginTime());
#ifdef OCCT_DEBUG_DELTA
        std::cout
          << "TDF_Data::Undo, after validity correction, Delta is now available from time \t#"
          << newDelta->BeginTime() << " to time \t#" << newDelta->EndTime() << std::endl;
#endif
      }
      myTime = aDelta->BeginTime();
      aDelta->BeforeOrAfterApply(false);
    }
  }
  return newDelta;
}

//=================================================================================================

void TDF_Data::SetAccessByEntries(const bool aSet)
{
  myAccessByEntries = aSet;

  myAccessByEntriesTable.Clear();
  if (myAccessByEntries)
  {
    // Add root label.
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(myRoot, anEntry);
    myAccessByEntriesTable.Bind(anEntry, myRoot);

    // Add all other labels.
    TDF_ChildIterator itr(myRoot, true);
    for (; itr.More(); itr.Next())
    {
      const TDF_Label aLabel = itr.Value();
      TDF_Tool::Entry(aLabel, anEntry);
      myAccessByEntriesTable.Bind(anEntry, aLabel);
    }
  }
}

//=================================================================================================

void TDF_Data::RegisterLabel(const TDF_Label& aLabel)
{
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(aLabel, anEntry);
  myAccessByEntriesTable.Bind(anEntry, aLabel);
}

//=================================================================================================

Standard_OStream& TDF_Data::Dump(Standard_OStream& anOS) const
{
  anOS << "Dump of a TDF_Data." << std::endl;
  anOS << "Current transaction: " << myTransaction;
  anOS << "; Current tick: " << myTime << ";" << std::endl;
  return anOS;
}

//=================================================================================================

void TDF_Data::DumpJson(Standard_OStream& theOStream, int /*theDepth*/) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  TCollection_AsciiString aStrForTDF_Label;
  TDF_Tool::Entry(myRoot, aStrForTDF_Label);
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aStrForTDF_Label)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTransaction)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myNbTouchedAtt)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myNotUndoMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTime)
  for (NCollection_List<int>::Iterator aTimeIt(myTimes); aTimeIt.More(); aTimeIt.Next())
  {
    const int aTime = aTimeIt.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aTime)
  }
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAllowModification)
}
