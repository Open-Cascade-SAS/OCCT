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
#include <TDF_AttributeDelta.hxx>
#include <TDF_DeltaOnModification.hxx>


IMPLEMENT_STANDARD_RTTIEXT(TObj_TIntSparseArray,TDF_Attribute)

//=======================================================================
//function : TObj_TIntSparseArray
//purpose  : Empty constructor
//=======================================================================

TObj_TIntSparseArray::TObj_TIntSparseArray ()
: myVector(100), myOldMap(100), myDoBackup (Standard_True)
{
}

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================

const Standard_GUID& TObj_TIntSparseArray::GetID()
{
  static Standard_GUID GInterfaceID ("7016dc0c-b118-4433-8ef3-aecdccc79198");
  return GInterfaceID;
}

//=======================================================================
//function : ID
//purpose  :
//=======================================================================

const Standard_GUID& TObj_TIntSparseArray::ID() const
{
  return GetID();
}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================

Handle(TObj_TIntSparseArray) TObj_TIntSparseArray::Set
                           (const TDF_Label& theLabel)
{
  Handle(TObj_TIntSparseArray) aTData;
  if (! theLabel.FindAttribute( GetID(), aTData))
  {
    aTData = new TObj_TIntSparseArray;
    theLabel.AddAttribute(aTData);
  }
  return aTData;
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void TObj_TIntSparseArray::SetValue (const Standard_Size theId,
                                         const Standard_Integer theValue)
{
  // check that modification is allowed
  if ( !Label().Data()->IsModificationAllowed() )
    throw Standard_ImmutableObject("Attribute TObj_TIntSparseArray is changed outside transaction");

  if (theId < 1 || theValue < 1)
    throw Standard_OutOfRange("TObj_TIntSparseArray::SetValue");

  Standard_Integer anOld = AbsentValue;
  Standard_Boolean isOld = myVector.HasValue(theId);
  if (isOld)
  {
    Standard_Integer& aData = myVector(theId);
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
    myVector.SetValue (theId, theValue);
  }

  TDF_Label aLabel = Label();
  if (!aLabel.IsNull())
  {
    Handle(TDF_Data) aData = aLabel.Data();
    Standard_Integer aCurrentTransaction = aData->Transaction();
    Standard_Integer aMyTransaction = Transaction();

    if (myDoBackup && aMyTransaction < aCurrentTransaction)
      backupValue(theId, anOld, theValue);
  }
}

//=======================================================================
//function : UnsetValue
//purpose  : 
//=======================================================================

void TObj_TIntSparseArray::UnsetValue (const Standard_Size theId)
{
  // check that modification is allowed
  if ( !Label().Data()->IsModificationAllowed() )
    throw Standard_ImmutableObject("Attribute TObj_TIntSparseArray is changed outside transaction");

  if (theId < 1)
    throw Standard_OutOfRange("TObj_TIntSparseArray::UnsetValue");

  Standard_Integer anOld = AbsentValue;
  Standard_Boolean isOld = myVector.HasValue(theId);
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
    Handle(TDF_Data) aData = aLabel.Data();
    Standard_Integer aCurrentTransaction = aData->Transaction();
    Standard_Integer aMyTransaction = Transaction();

    if (myDoBackup && aMyTransaction < aCurrentTransaction)
      backupValue(theId, anOld, AbsentValue);
  }
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================

void TObj_TIntSparseArray::Clear ()
{
  // backup old values
  TDF_Label aLabel = Label();
  if (!aLabel.IsNull())
  {
    Handle(TDF_Data) aData = aLabel.Data();
    Standard_Integer aCurrentTransaction = aData->Transaction();
    Standard_Integer aMyTransaction = Transaction();

    if (myDoBackup && aMyTransaction < aCurrentTransaction)
    {
      TObj_TIntSparseArray_VecOfData::Iterator anIt (myVector);
      for (; anIt.More(); anIt.Next())
      {
        Standard_Size anId = anIt.Key();
        Standard_Integer aVal = anIt.Value();
        backupValue(anId, aVal, AbsentValue);
      }
    }
  }
  myVector.Clear();
}

//=======================================================================
//function : backupValue
//purpose  :
//=======================================================================

void TObj_TIntSparseArray::backupValue (const Standard_Size theId,
                                            const Standard_Integer theCurrValue,
                                            const Standard_Integer theNewValue)
{
  // save the current value if it has not been saved in previous time
  if ( !myOldMap.IsBound( theId ) )
    myOldMap.Bind(theId, theCurrValue);
  else
  {
    // if value in Undo is the same as the new one, the item in Undo map may be cleared
    Standard_Integer aUData = myOldMap.Value(theId);
    if (aUData == theNewValue)
      myOldMap.UnBind(theId);
  }
}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================

Handle(TDF_Attribute) TObj_TIntSparseArray::NewEmpty () const
{
  return new TObj_TIntSparseArray;
}

//=======================================================================
//function : BackupCopy
//purpose  : Moves <this> delta into a new other attribute.
//=======================================================================

Handle(TDF_Attribute) TObj_TIntSparseArray::BackupCopy() const
{
  Handle(TObj_TIntSparseArray) aCopy = 
    Handle(TObj_TIntSparseArray)::DownCast(NewEmpty());

  // save delta data in a copy
  if (!myOldMap.IsEmpty())
    aCopy->myOldMap.Exchange ( (TObj_TIntSparseArray_MapOfData&)myOldMap );

  return aCopy;
}

//=======================================================================
//function : Restore
//purpose  : Restores contents of this with theDelta
//=======================================================================

void TObj_TIntSparseArray::Restore(const Handle(TDF_Attribute)& theDelta)
{
  Handle(TObj_TIntSparseArray) aDelta = 
    Handle(TObj_TIntSparseArray)::DownCast(theDelta);
  if (aDelta.IsNull())
    return;

  // restore the values from aDelta->myOldMap
  if (!aDelta->myOldMap.IsEmpty())
  {
    TObj_TIntSparseArray_MapOfData::Iterator anIt (aDelta->myOldMap);
    for (; anIt.More(); anIt.Next())
    {
      Standard_Size anId = anIt.Key();
      Standard_Integer anOld = anIt.Value();
      if (anOld == AbsentValue)
        UnsetValue (anId);
      else
        SetValue (anId, anOld);
    }
  }
}

//=======================================================================
//function : Paste
//purpose  : copy this
//=======================================================================

void TObj_TIntSparseArray::Paste (const Handle(TDF_Attribute)& theInto,
                                    const Handle(TDF_RelocationTable)&) const
{
  Handle(TObj_TIntSparseArray) aInto =
    Handle(TObj_TIntSparseArray)::DownCast(theInto);
  if(aInto.IsNull())
    return;

  aInto->myVector.Assign(myVector);
}

//=======================================================================
//function : BeforeCommitTransaction
//purpose  : It is called just before Commit or Abort transaction
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
//function : DeltaOnModification
//purpose  : Applies aDelta to <me>
//=======================================================================

void TObj_TIntSparseArray::DeltaOnModification
                        (const Handle(TDF_DeltaOnModification)& theDelta)
{
  // we do not call Backup here, because a backup data is formed inside Restore.
  // Backup is called rather from BeforeCommitTransaction
  Restore(theDelta->Attribute());
}

//=======================================================================
//function : AfterUndo
//purpose  : After application of a TDF_Delta.
//=======================================================================

Standard_Boolean TObj_TIntSparseArray::AfterUndo
                        (const Handle(TDF_AttributeDelta)&,
                         const Standard_Boolean)
{
  // we must be sure that a delta in <me> is cleared
  ClearDelta();
  return Standard_True;
}
