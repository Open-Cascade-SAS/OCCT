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

#ifndef _Transfer_IteratorOfProcessForFinder_HeaderFile
#define _Transfer_IteratorOfProcessForFinder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Transfer_HSequenceOfFinder.hxx>
#include <Transfer_TransferIterator.hxx>

class Standard_NoSuchObject;
class Transfer_Finder;
class Transfer_FindHasher;
class Transfer_ProcessForFinder;
class Transfer_ActorOfProcessForFinder;
class Transfer_Binder;

class Transfer_IteratorOfProcessForFinder : public Transfer_TransferIterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty Iterator
  //! if withstarts is True, each Binder to be iterated will
  //! be associated to its corresponding Starting Object
  Standard_EXPORT Transfer_IteratorOfProcessForFinder(const Standard_Boolean withstarts);

  //! Adds a Binder to the iteration list (construction)
  //! with no corresponding Starting Object
  //! (note that Result is brought by Binder)
  Standard_EXPORT void Add(const Handle(Transfer_Binder)& binder);

  //! Adds a Binder to the iteration list, associated with
  //! its corresponding Starting Object "start"
  //! Starting Object is ignored if not required at
  //! Creation time
  Standard_EXPORT void Add(const Handle(Transfer_Binder)& binder,
                           const Handle(Transfer_Finder)& start);

  //! After having added all items, keeps or rejects items
  //! which are attached to starting data given by <only>
  //! <keep> = True (D) : keeps. <keep> = False : rejects
  //! Does nothing if <withstarts> was False
  Standard_EXPORT void Filter(const Handle(Transfer_HSequenceOfFinder)& list,
                              const Standard_Boolean                    keep = Standard_True);

  //! Returns True if Starting Object is available
  //! (defined at Creation Time)
  Standard_EXPORT Standard_Boolean HasStarting() const;

  //! Returns corresponding Starting Object
  Standard_EXPORT const Handle(Transfer_Finder)& Starting() const;

protected:
private:
  Handle(Transfer_HSequenceOfFinder) thestarts;
};

#endif // _Transfer_IteratorOfProcessForFinder_HeaderFile
