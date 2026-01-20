// Created on: 1992-02-03
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Transfer_IteratorOfProcessForTransient_HeaderFile
#define _Transfer_IteratorOfProcessForTransient_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Transfer_TransferIterator.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Transfer_Binder.hxx>

class Standard_NoSuchObject;
class Standard_Transient;
class Transfer_ProcessForTransient;
class Transfer_ActorOfProcessForTransient;
class Transfer_Binder;

class Transfer_IteratorOfProcessForTransient : public Transfer_TransferIterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty Iterator
  //! if withstarts is True, each Binder to be iterated will
  //! be associated to its corresponding Starting Object
  Standard_EXPORT Transfer_IteratorOfProcessForTransient(const bool withstarts);

  //! Adds a Binder to the iteration list (construction)
  //! with no corresponding Starting Object
  //! (note that Result is brought by Binder)
  Standard_EXPORT void Add(const occ::handle<Transfer_Binder>& binder);

  //! Adds a Binder to the iteration list, associated with
  //! its corresponding Starting Object "start"
  //! Starting Object is ignored if not required at
  //! Creation time
  Standard_EXPORT void Add(const occ::handle<Transfer_Binder>&    binder,
                           const occ::handle<Standard_Transient>& start);

  //! After having added all items, keeps or rejects items
  //! which are attached to starting data given by <only>
  //! <keep> = True (D) : keeps. <keep> = False : rejects
  //! Does nothing if <withstarts> was False
  Standard_EXPORT void Filter(const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list,
                              const bool                      keep = true);

  //! Returns True if Starting Object is available
  //! (defined at Creation Time)
  Standard_EXPORT bool HasStarting() const;

  //! Returns corresponding Starting Object
  Standard_EXPORT const occ::handle<Standard_Transient>& Starting() const;

private:
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> thestarts;
};

#endif // _Transfer_IteratorOfProcessForTransient_HeaderFile
