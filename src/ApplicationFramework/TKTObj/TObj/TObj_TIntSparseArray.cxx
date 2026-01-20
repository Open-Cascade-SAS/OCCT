// Created on: 2007-03-16
// Created by: Michael SAZONOV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <TObj_TIntSparseArray.hxx>
#include <Standard_GUID.hxx>
#include <Standard_ImmutableObject.hxx>
#include <TDF_Data.hxx>
#include <TDF_DeltaOnModification.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TObj_TIntSparseArray, TDF_Attribute)

//=================================================================================================

TObj_TIntSparseArray::TObj_TIntSparseArray()
    : myVector(100),
      myOldMap(100),
      myDoBackup(true)
{
}

//=================================================================================================

const Standard_GUID& TObj_TIntSparseArray::GetID()
{
  static Standard_GUID GInterfaceID("7016dc0c-b118-4433-8ef3-aecdccc79198");
  return GInterfaceID;
}

//=================================================================================================

const Standard_GUID& TObj_TIntSparseArray::ID() const
{
  return GetID();
}

//=================================================================================================

occ::handle<TObj_TIntSparseArray> TObj_TIntSparseArray::Set(const TDF_Label& theLabel)
{
  occ::handle<TObj_TIntSparseArray> aTData;
  if (!theLabel.FindAttribute(GetID(), aTData))
  {
    aTData = new TObj_TIntSparseArray;
    theLabel.AddAttribute(aTData);
  }
  return aTData;
}

//=================================================================================================

void TObj_TIntSparseArray::SetValue(const size_t theId, const int theValue)
{
  // check that modification is allowed
  if (!Label().Data()->IsModificationAllowed())
    throw Standard_ImmutableObject("Attribute TObj_TIntSparseArray is changed outside transaction");

  if (theId < 1 || theValue < 1)
    throw Standard_OutOfRange("TObj_TIntSparseArray::SetValue");

  int anOld = AbsentValue;
  bool isOld = myVector.HasValue(theId);
  if (isOld)
  {
    int& aData = myVector(theId);
    if (aData == theValue)
      // no actual modification
      return;
    anOld = aData;
    // set new value
    aData = theValue;
  }
  else
  {
    // set the new value
    myVector.SetValue(theId, theValue);
  }

  TDF_Label aLabel = Label();
  if (!aLabel.IsNull())
  {
    occ::handle<TDF_Data> aData               = aLabel.Data();
    int aCurrentTransaction = aData->Transaction();
    int aMyTransaction      = Transaction();

    if (myDoBackup && aMyTransaction < aCurrentTransaction)
      backupValue(theId, anOld, theValue);
  }
}

//=================================================================================================

void TObj_TIntSparseArray::UnsetValue(const size_t theId)
{
  // check that modification is allowed
  if (!Label().Data()->IsModificationAllowed())
    throw Standard_ImmutableObject("Attribute TObj_TIntSparseArray is changed outside transaction");

  if (theId < 1)
    throw Standard_OutOfRange("TObj_TIntSparseArray::UnsetValue");

  int anOld = AbsentValue;
  bool isOld = myVector.HasValue(theId);
  if (isOld)
  {
    anOld = myVector(theId);
    // unset the value
    myVector.UnsetValue(theId);
  }
  else
    // no actual modification
    return;

  TDF_Label aLabel = Label();
  if (!aLabel.IsNull())
  {
    occ::handle<TDF_Data> aData               = aLabel.Data();
    int aCurrentTransaction = aData->Transaction();
    int aMyTransaction      = Transaction();

    if (myDoBackup && aMyTransaction < aCurrentTransaction)
      backupValue(theId, anOld, AbsentValue);
  }
}

//=================================================================================================

void TObj_TIntSparseArray::Clear()
{
  // backup old values
  TDF_Label aLabel = Label();
  if (!aLabel.IsNull())
  {
    occ::handle<TDF_Data> aData               = aLabel.Data();
    int aCurrentTransaction = aData->Transaction();
    int aMyTransaction      = Transaction();

    if (myDoBackup && aMyTransaction < aCurrentTransaction)
    {
      NCollection_SparseArray<int>::Iterator anIt(myVector);
      for (; anIt.More(); anIt.Next())
      {
        size_t    anId = anIt.Key();
        int aVal = anIt.Value();
        backupValue(anId, aVal, AbsentValue);
      }
    }
  }
  myVector.Clear();
}

//=================================================================================================

void TObj_TIntSparseArray::backupValue(const size_t    theId,
                                       const int theCurrValue,
                                       const int theNewValue)
{
  // save the current value if it has not been saved in previous time
  if (!myOldMap.IsBound(theId))
    myOldMap.Bind(theId, theCurrValue);
  else
  {
    // if value in Undo is the same as the new one, the item in Undo map may be cleared
    int aUData = myOldMap.Value(theId);
    if (aUData == theNewValue)
      myOldMap.UnBind(theId);
  }
}

//=================================================================================================

occ::handle<TDF_Attribute> TObj_TIntSparseArray::NewEmpty() const
{
  return new TObj_TIntSparseArray;
}

//=======================================================================
// function : BackupCopy
// purpose  : Moves <this> delta into a new other attribute.
//=======================================================================

occ::handle<TDF_Attribute> TObj_TIntSparseArray::BackupCopy() const
{
  occ::handle<TObj_TIntSparseArray> aCopy = occ::down_cast<TObj_TIntSparseArray>(NewEmpty());

  // save delta data in a copy
  if (!myOldMap.IsEmpty())
    aCopy->myOldMap.Exchange((NCollection_SparseArray<int>&)myOldMap);

  return aCopy;
}

//=======================================================================
// function : Restore
// purpose  : Restores contents of this with theDelta
//=======================================================================

void TObj_TIntSparseArray::Restore(const occ::handle<TDF_Attribute>& theDelta)
{
  occ::handle<TObj_TIntSparseArray> aDelta = occ::down_cast<TObj_TIntSparseArray>(theDelta);
  if (aDelta.IsNull())
    return;

  // restore the values from aDelta->myOldMap
  if (!aDelta->myOldMap.IsEmpty())
  {
    NCollection_SparseArray<int>::Iterator anIt(aDelta->myOldMap);
    for (; anIt.More(); anIt.Next())
    {
      size_t    anId  = anIt.Key();
      int anOld = anIt.Value();
      if (anOld == AbsentValue)
        UnsetValue(anId);
      else
        SetValue(anId, anOld);
    }
  }
}

//=================================================================================================

void TObj_TIntSparseArray::Paste(const occ::handle<TDF_Attribute>& theInto,
                                 const occ::handle<TDF_RelocationTable>&) const
{
  occ::handle<TObj_TIntSparseArray> aInto = occ::down_cast<TObj_TIntSparseArray>(theInto);
  if (aInto.IsNull())
    return;

  aInto->myVector.Assign(myVector);
}

//=======================================================================
// function : BeforeCommitTransaction
// purpose  : It is called just before Commit or Abort transaction
//=======================================================================

void TObj_TIntSparseArray::BeforeCommitTransaction()
{
  if (!myOldMap.IsEmpty())
  {
    Backup();
    ClearDelta();
  }
}

//=======================================================================
// function : DeltaOnModification
// purpose  : Applies aDelta to <me>
//=======================================================================

void TObj_TIntSparseArray::DeltaOnModification(const occ::handle<TDF_DeltaOnModification>& theDelta)
{
  // we do not call Backup here, because a backup data is formed inside Restore.
  // Backup is called rather from BeforeCommitTransaction
  Restore(theDelta->Attribute());
}

//=======================================================================
// function : AfterUndo
// purpose  : After application of a TDF_Delta.
//=======================================================================

bool TObj_TIntSparseArray::AfterUndo(const occ::handle<TDF_AttributeDelta>&,
                                                 const bool)
{
  // we must be sure that a delta in <me> is cleared
  ClearDelta();
  return true;
}
