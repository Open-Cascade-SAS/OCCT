// Created on: 1995-05-12
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Interface_IntList_HeaderFile
#define _Interface_IntList_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Boolean.hxx>

//! This class detains the data which describe a Graph. A Graph
//! has two lists, one for shared refs, one for sharing refs
//! (the reverses). Each list comprises, for each Entity of the
//! Model of the Graph, a list of Entities (shared or sharing).
//! In fact, entities are identified by their numbers in the Model
//! or Graph : this gives better performances.
//!
//! A simple way to implement this is to instantiate a HArray1
//! with a HSequenceOfInteger : each Entity Number designates a
//! value, which is a Sequence (if it is null, it is considered as
//! empty : this is a little optimisation).
//!
//! This class gives a more efficient way to implement this.
//! It has two lists (two arrays of integers), one to describe
//! list (empty, one value given immediately, or negated index in
//! the second list), one to store refs (pointed from the first
//! list). This is much more efficient than a list of sequences,
//! in terms of speed (especially for read) and of memory
//!
//! An IntList can also be set to access data for a given entity
//! number, it then acts as a single sequence
//!
//! Remark that if an IntList is created from another one, it can
//! be read, but if it is created without copying, it may not be
//! edited
class Interface_IntList
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates empty IntList.
  Standard_EXPORT Interface_IntList();

  //! Creates an IntList for <nbe> entities
  Standard_EXPORT Interface_IntList(const Standard_Integer nbe);

  //! Creates an IntList from another one.
  //! if <copied> is True, copies data
  //! else, data are not copied, only the header object is
  Standard_EXPORT Interface_IntList(const Interface_IntList& other, const Standard_Boolean copied);

  //! Initialize IntList by number of entities.
  Standard_EXPORT void Initialize(const Standard_Integer nbe);

  //! Returns internal values, used for copying
  Standard_EXPORT void Internals(Standard_Integer&                 nbrefs,
                                 Handle(TColStd_HArray1OfInteger)& ents,
                                 Handle(TColStd_HArray1OfInteger)& refs) const;

  //! Returns count of entities to be acknowledged
  Standard_EXPORT Standard_Integer NbEntities() const;

  //! Changes the count of entities (ignored if decreased)
  Standard_EXPORT void SetNbEntities(const Standard_Integer nbe);

  //! Sets an entity number as current (for read and fill)
  Standard_EXPORT void SetNumber(const Standard_Integer number);

  //! Returns the current entity number
  Standard_EXPORT Standard_Integer Number() const;

  //! Returns an IntList, identical to <me> but set to a specified
  //! entity Number
  //! By default, not copied (in order to be read)
  //! Specified <copied> to produce another list and edit it
  Standard_EXPORT Interface_IntList List(const Standard_Integer number,
                                         const Standard_Boolean copied = Standard_False) const;

  //! Sets current entity list to be redefined or not
  //! This is used in a Graph for redefinition list : it can be
  //! disable (no redefinition, i.e. list is cleared), or enabled
  //! (starts as empty). The original list has not to be "redefined"
  Standard_EXPORT void SetRedefined(const Standard_Boolean mode);

  //! Makes a reservation for <count> references to be later
  //! attached to the current entity. If required, it increases
  //! the size of array used to store refs. Remark that if count is
  //! less than two, it does nothing (because immediate storing)
  Standard_EXPORT void Reservate(const Standard_Integer count);

  //! Adds a reference (as an integer value, an entity number) to
  //! the current entity number. Zero is ignored
  Standard_EXPORT void Add(const Standard_Integer ref);

  //! Returns the count of refs attached to current entity number
  Standard_EXPORT Standard_Integer Length() const;

  //! Returns True if the list for a number
  //! (default is taken as current) is "redefined" (useful for empty list)
  Standard_EXPORT Standard_Boolean IsRedefined(const Standard_Integer num = 0) const;

  //! Returns a reference number in the list for current number,
  //! according to its rank
  Standard_EXPORT Standard_Integer Value(const Standard_Integer num) const;

  //! Removes an item in the list for current number, given its rank
  //! Returns True if done, False else
  Standard_EXPORT Standard_Boolean Remove(const Standard_Integer num);

  //! Clears all data, hence each entity number has an empty list
  Standard_EXPORT void Clear();

  //! Resizes lists to exact sizes. For list of refs, a positive
  //! margin can be added.
  Standard_EXPORT void AdjustSize(const Standard_Integer margin = 0);

private:
  Standard_Integer                 thenbe;
  Standard_Integer                 thenbr;
  Standard_Integer                 thenum;
  Standard_Integer                 thecount;
  Standard_Integer                 therank;
  Handle(TColStd_HArray1OfInteger) theents;
  Handle(TColStd_HArray1OfInteger) therefs;
};

#endif // _Interface_IntList_HeaderFile
