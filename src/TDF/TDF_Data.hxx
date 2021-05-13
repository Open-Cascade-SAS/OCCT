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

#ifndef _TDF_Data_HeaderFile
#define _TDF_Data_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_LabelNodePtr.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TDF_HAllocator.hxx>
#include <Standard_Transient.hxx>
#include <TDF_Label.hxx>
#include <Standard_OStream.hxx>
#include <NCollection_DataMap.hxx>
class Standard_NoMoreObject;
class TDF_Transaction;
class TDF_LabelNode;
class TDF_Delta;
class TDF_Label;


class TDF_Data;
DEFINE_STANDARD_HANDLE(TDF_Data, Standard_Transient)

//! This class is used to manipulate a complete independent,
//! self sufficient data structure and its services:
//!
//! Access to the root label;
//!
//! Opens, aborts, commits a transaction;
//!
//! Generation and use of Delta, depending on the time.
//! This class uses a special allocator
//! (see LabelNodeAllocator() method)
//! for more efficient allocation of objects in memory.
class TDF_Data : public Standard_Transient
{

public:

  
  //! A new and empty Data structure.
  Standard_EXPORT TDF_Data();
  
  //! Returns the root label of the Data structure.
    const TDF_Label Root() const;
  
  //! Returns the current transaction number.
  Standard_Integer Transaction() const;
  
  //! Returns the current tick. It is incremented each Commit.
    Standard_Integer Time() const;
  
  //! Returns true if <aDelta> is applicable HERE and NOW.
  Standard_EXPORT Standard_Boolean IsApplicable (const Handle(TDF_Delta)& aDelta) const;
  
  //! Apply <aDelta> to undo a set of attribute modifications.
  //!
  //! Optional <withDelta> set to True indicates a
  //! Delta Set must be generated. (See above)
  Standard_EXPORT Handle(TDF_Delta) Undo (const Handle(TDF_Delta)& aDelta, const Standard_Boolean withDelta = Standard_False);
  
  Standard_EXPORT void Destroy();
~TDF_Data()
{
  Destroy();
}
  
  //! Returns the undo mode status.
    Standard_Boolean NotUndoMode() const;
  
  //! Dumps the Data on <aStream>.
    Standard_EXPORT Standard_OStream& Dump (Standard_OStream& anOS) const;
Standard_OStream& operator<< (Standard_OStream& anOS) const
{
  return Dump(anOS);
}
  
  //! Sets modification mode.
    void AllowModification (const Standard_Boolean isAllowed);
  
  //! returns modification mode.
    Standard_Boolean IsModificationAllowed() const;
  
  //! Initializes a mechanism for fast access to the labels by their entries.
  //! The fast access is useful for large documents and often access to the labels 
  //! via entries. Internally, a table of entry - label is created, 
  //! which allows to obtain a label by its entry in a very fast way.
  //! If the mechanism is turned off, the internal table is cleaned.
  //! New labels are added to the table, if the mechanism is on
  //! (no need to re-initialize the mechanism).
  Standard_EXPORT void SetAccessByEntries (const Standard_Boolean aSet);

  //! Returns a status of mechanism for fast access to the labels via entries.
  Standard_Boolean IsAccessByEntries() const { return myAccessByEntries; }

  //! Returns a label by an entry.
  //! Returns Standard_False, if such a label doesn't exist
  //! or mechanism for fast access to the label by entry is not initialized.
  Standard_Boolean GetLabel (const TCollection_AsciiString& anEntry, TDF_Label& aLabel) { return myAccessByEntriesTable.Find(anEntry, aLabel); }

  //! An internal method. It is used internally on creation of new labels.
  //! It adds a new label into internal table for fast access to the labels by entry.
  Standard_EXPORT void RegisterLabel (const TDF_Label& aLabel);

  //! Returns TDF_HAllocator, which is an
  //! incremental allocator used by
  //! TDF_LabelNode.
  //! This allocator is used to
  //! manage TDF_LabelNode objects,
  //! but it can also be used for
  //! allocating memory to
  //! application-specific data (be
  //! careful because this
  //! allocator does not release
  //! the memory).
  //! The benefits of this
  //! allocation scheme are
  //! noticeable when dealing with
  //! large OCAF documents, due to:
  //! 1.    Very quick allocation of
  //! objects (memory heap is not
  //! used, the algorithm that
  //! replaces it is very simple).
  //! 2.    Very quick destruction of
  //! objects (memory is released not
  //! by destructors of TDF_LabelNode,
  //! but rather by the destructor of
  //! TDF_Data).
  //! 3.  TDF_LabelNode objects do not
  //! fragmentize the memory; they are
  //! kept compactly in a number of
  //! arrays of 16K each.
  //! 4.    Swapping is reduced on large
  //! data, because each document now
  //! occupies a smaller number of
  //! memory pages.
    const TDF_HAllocator& LabelNodeAllocator() const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

friend class TDF_Transaction;
friend class TDF_LabelNode;


  DEFINE_STANDARD_RTTIEXT(TDF_Data,Standard_Transient)

protected:




private:

  //! Fixes order of Attributes' Deltas to perform undo/redo without exceptions:
  //! puts OnRemoval deltas to the end of the list.
  void FixOrder(const Handle(TDF_Delta)& theDelta);
  
  //! Increments the transaction number and returns it.
  Standard_EXPORT Standard_Integer OpenTransaction();
  
  //! Decrements the transaction number and commits the
  //! modifications.
  //!
  //! Raises if there is no current transaction.
  //!
  //! Optional <withDelta> set to True indicates a
  //! Delta must be generated.
  Standard_EXPORT Handle(TDF_Delta) CommitTransaction (const Standard_Boolean withDelta = Standard_False);
  
  //! Decrements the transaction number and commits the
  //! modifications until AND including the transaction
  //! <untilTransaction>.
  Standard_EXPORT Handle(TDF_Delta) CommitUntilTransaction (const Standard_Integer untilTransaction, const Standard_Boolean withDelta = Standard_False);
  
  //! Decrements the transaction number and forgets the
  //! modifications.
  //!
  //! Raises if there is no current transaction.
  Standard_EXPORT void AbortTransaction();
  
  //! Decrements the transaction number and forgets the
  //! modifications until AND including the transaction
  //! <untilTransaction>.
  Standard_EXPORT void AbortUntilTransaction (const Standard_Integer untilTransaction);
  
  //! Decrements the transaction number and commits the
  //! modifications. Used to implement the recursif
  //! commit process. The returned boolean says how many
  //! attributes (new, modified or deleted) has been
  //! committed from the previous transaction into the
  //! current one.
  Standard_EXPORT Standard_Integer CommitTransaction (const TDF_Label& aLabel, const Handle(TDF_Delta)& aDelta, const Standard_Boolean withDelta);

  TDF_LabelNodePtr myRoot;
  Standard_Integer myTransaction;
  Standard_Integer myNbTouchedAtt;
  Standard_Boolean myNotUndoMode;
  Standard_Integer myTime;
  TColStd_ListOfInteger myTimes;
  TDF_HAllocator myLabelNodeAllocator;
  Standard_Boolean myAllowModification;
  Standard_Boolean myAccessByEntries;
  NCollection_DataMap<TCollection_AsciiString, TDF_Label> myAccessByEntriesTable;
};


#include <TDF_Data.lxx>





#endif // _TDF_Data_HeaderFile
