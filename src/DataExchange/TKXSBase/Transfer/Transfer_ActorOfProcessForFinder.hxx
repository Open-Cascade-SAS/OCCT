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

#ifndef _Transfer_ActorOfProcessForFinder_HeaderFile
#define _Transfer_ActorOfProcessForFinder_HeaderFile

#include <Standard.hxx>

#include <Standard_Transient.hxx>
#include <Transfer_TransferMapOfProcessForFinder.hxx>
#include <Message_ProgressRange.hxx>

class Standard_DomainError;
class Transfer_Finder;
class Transfer_FindHasher;
class Transfer_ProcessForFinder;
class Transfer_IteratorOfProcessForFinder;
class Transfer_Binder;
class Transfer_SimpleBinderOfTransient;
class Standard_Transient;

class Transfer_ActorOfProcessForFinder;
DEFINE_STANDARD_HANDLE(Transfer_ActorOfProcessForFinder, Standard_Transient)

class Transfer_ActorOfProcessForFinder : public Standard_Transient
{

public:
  Standard_EXPORT Transfer_ActorOfProcessForFinder();

  //! Prerequisite for Transfer : the method Transfer is
  //! called on a starting object only if Recognize has
  //! returned True on it
  //! This allows to define a list of Actors, each one
  //! processing a definite kind of data
  //! TransferProcess calls Recognize on each one before
  //! calling Transfer. But even if Recognize has returned
  //! True, Transfer can reject by returning a Null Binder
  //! (afterwards rejection), the next actor is then invoked
  //!
  //! The provided default returns True, can be redefined
  Standard_EXPORT virtual Standard_Boolean Recognize(const Handle(Transfer_Finder)& start);

  //! Specific action of Transfer. The Result is stored in
  //! the returned Binder, or a Null Handle for "No result"
  //! (Default defined as doing nothing; should be deferred)
  //! "mutable" allows the Actor to record intermediate
  //! information, in addition to those of TransferProcess
  Standard_EXPORT virtual Handle(Transfer_Binder) Transferring(
    const Handle(Transfer_Finder)&           start,
    const Handle(Transfer_ProcessForFinder)& TP,
    const Message_ProgressRange&             theProgress = Message_ProgressRange());

  //! Prepares and Returns a Binder for a Transient Result
  //! Returns a Null Handle if <res> is itself Null
  Standard_EXPORT Handle(Transfer_SimpleBinderOfTransient) TransientResult(
    const Handle(Standard_Transient)& res) const;

  //! Returns a Binder for No Result, i.e. a Null Handle
  Standard_EXPORT Handle(Transfer_Binder) NullResult() const;

  //! If <mode> is True, commands an Actor to be set at the
  //! end of the list of Actors (see SetNext)
  //! If it is False (creation default), each add Actor is
  //! set at the beginning of the list
  //! This allows to define default Actors (which are Last)
  Standard_EXPORT void SetLast(const Standard_Boolean mode = Standard_True);

  //! Returns the Last status (see SetLast).
  Standard_EXPORT Standard_Boolean IsLast() const;

  //! Defines a Next Actor : it can then be asked to work if
  //! <me> produces no result for a given type of Object.
  //! If Next is already set and is not "Last", calls
  //! SetNext on it. If Next defined and "Last", the new
  //! actor is added before it in the list
  Standard_EXPORT void SetNext(const Handle(Transfer_ActorOfProcessForFinder)& next);

  //! Returns the Actor defined as Next, or a Null Handle
  Standard_EXPORT Handle(Transfer_ActorOfProcessForFinder) Next() const;

  DEFINE_STANDARD_RTTI_INLINE(Transfer_ActorOfProcessForFinder, Standard_Transient)

protected:
private:
  Handle(Transfer_ActorOfProcessForFinder) thenext;
  Standard_Boolean                         thelast;
};

#endif // _Transfer_ActorOfProcessForFinder_HeaderFile
