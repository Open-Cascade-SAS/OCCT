// Created on: 2007-03-16
// Created by: Michael SAZONOV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_TIntSparseArray_HeaderFile
#define TObj_TIntSparseArray_HeaderFile

#include <TObj_Common.hxx>

#include <NCollection_SparseArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>

typedef NCollection_SparseArray<Standard_Integer> TObj_TIntSparseArray_VecOfData;
typedef NCollection_SparseArray<Standard_Integer> TObj_TIntSparseArray_MapOfData;

class Handle(TObj_TIntSparseArray);
class Standard_GUID;
class Handle(TDF_RelocationTable);

/**
 * OCAF Attribute to store a set of positive integer values in the OCAF tree.
 * Each value is identified by ID (positive integer).
 * The supporting underlying data structure is NCollection_SparseArray of integers.
 */

class TObj_TIntSparseArray : public TDF_Attribute
{
 public:

  //! Empty constructor
  Standard_EXPORT TObj_TIntSparseArray();

  //! This method is used in implementation of ID()
  static Standard_EXPORT const Standard_GUID& GetID();

  //! Returns the ID of this attribute.
  Standard_EXPORT const Standard_GUID& ID() const;

  //! Creates TObj_TIntSparseArray attribute on given label.
  static Standard_EXPORT Handle(TObj_TIntSparseArray) Set
                            (const TDF_Label& theLabel);

 public:
  //! Methods for access to data

  //! Returns the number of stored values in the set
  Standard_EXPORT Standard_Size Size() const
  { return myVector.Size(); }

  typedef TObj_TIntSparseArray_VecOfData::ConstIterator Iterator;

  //! Returns iterator on objects contained in the set
  Iterator GetIterator() const { return Iterator(myVector); }

  //! Returns true if the value with the given ID is present.
  Standard_Boolean HasValue (const Standard_Size theId) const
  { return myVector.HasValue(theId); }

  //! Returns the value by its ID.
  //! Raises an exception if no value is stored with this ID
  Standard_Integer Value (const Standard_Size theId) const
  { return myVector.Value(theId); }

  //! Sets the value with the given ID.
  //! Raises an exception if theId is not positive
  Standard_EXPORT void SetValue (const Standard_Size theId,
                                 const Standard_Integer theValue);

  //! Unsets the value with the given ID.
  //! Raises an exception if theId is not positive
  Standard_EXPORT void UnsetValue(const Standard_Size theId);

  //! Clears the set
  Standard_EXPORT void Clear ();

 public:
  //! Redefined OCAF abstract methods

  //! Returns an new empty TObj_TIntSparseArray attribute. It is used by the
  //! copy algorithm.
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const;

  //! Moves this delta into a new other attribute.
  Standard_EXPORT Handle(TDF_Attribute) BackupCopy() const;

  //! Restores the set using info saved in backup attribute theDelta.
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theDelta);

  //! This method is used when copying an attribute from a source structure
  //! into a target structure.
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theInto,
                             const Handle(TDF_RelocationTable)& theRT) const;

  //! It is called just before Commit or Abort transaction
  //! and does Backup() to create a delta
  Standard_EXPORT void BeforeCommitTransaction();

  //! Applies theDelta to this.
  Standard_EXPORT void DeltaOnModification
                        (const Handle(TDF_DeltaOnModification)& theDelta);

  //! Clears my modification delta; called after application of theDelta
  Standard_EXPORT Standard_Boolean AfterUndo
                        (const Handle(TDF_AttributeDelta)& theDelta,
                         const Standard_Boolean toForce);

 public:
  //! Methods to handle the modification delta

  //! Sets the flag pointing to the necessity to maintain a modification delta.
  //! It is called by the retrieval driver
  void SetDoBackup (const Standard_Boolean toDo)
  { myDoBackup = toDo; }

  void ClearDelta ()
  { myOldMap.Clear(); }

 private:
  //! Internal constant to recognize items in the backup array
  //! correspondent to absent values
  enum
  {
    AbsentValue = -1
  };

  //! backup one value
  void backupValue (const Standard_Size theId,
                    const Standard_Integer theCurrValue,
                    const Standard_Integer theNewValue);

  TObj_TIntSparseArray_VecOfData myVector;
  TObj_TIntSparseArray_MapOfData myOldMap;
  Standard_Boolean               myDoBackup;

 public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTI(TObj_TIntSparseArray)
};

//! Define handle class for TObj_TIntSparseArray
DEFINE_STANDARD_HANDLE(TObj_TIntSparseArray,TDF_Attribute)

#endif

#ifdef _MSC_VER
#pragma once
#endif
