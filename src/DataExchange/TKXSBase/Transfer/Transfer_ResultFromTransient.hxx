// Created on: 1995-11-16
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

#ifndef _Transfer_ResultFromTransient_HeaderFile
#define _Transfer_ResultFromTransient_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HSequenceOfTransient.hxx>
#include <Standard_Transient.hxx>
#include <Interface_CheckStatus.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>
class Transfer_Binder;
class Interface_Check;
class Transfer_TransientProcess;

class Transfer_ResultFromTransient;
DEFINE_STANDARD_HANDLE(Transfer_ResultFromTransient, Standard_Transient)

//! This class, in conjunction with ResultFromModel, allows to
//! record the result of a transfer initially stored in a
//! TransientProcess.
//!
//! A ResultFromTransient records a couple (Transient,Binder for
//! the result and checks) plus a list of "sub-results", which
//! have been recorded in the TrabsientProcess, under scope
//! attached to the starting transient.
class Transfer_ResultFromTransient : public Standard_Transient
{

public:
  //! Creates a ResultFromTransient, empty
  Standard_EXPORT Transfer_ResultFromTransient();

  //! Sets starting entity
  Standard_EXPORT void SetStart(const Handle(Standard_Transient)& start);

  //! Sets Binder (for result plus individual check)
  Standard_EXPORT void SetBinder(const Handle(Transfer_Binder)& binder);

  //! Returns the starting entity
  Standard_EXPORT Handle(Standard_Transient) Start() const;

  //! Returns the binder
  Standard_EXPORT Handle(Transfer_Binder) Binder() const;

  //! Returns True if a result is recorded
  Standard_EXPORT Standard_Boolean HasResult() const;

  //! Returns the check (or an empty one if no binder)
  Standard_EXPORT const Handle(Interface_Check) Check() const;

  //! Returns the check status
  Standard_EXPORT Interface_CheckStatus CheckStatus() const;

  //! Clears the list of (immediate) sub-results
  Standard_EXPORT void ClearSubs();

  //! Adds a sub-result
  Standard_EXPORT void AddSubResult(const Handle(Transfer_ResultFromTransient)& sub);

  //! Returns the count of recorded sub-results
  Standard_EXPORT Standard_Integer NbSubResults() const;

  //! Returns a sub-result, given its rank
  Standard_EXPORT Handle(Transfer_ResultFromTransient) SubResult(const Standard_Integer num) const;

  //! Returns the ResultFromTransient attached to a given starting
  //! entity (the key). Returns a null handle if not found
  Standard_EXPORT Handle(Transfer_ResultFromTransient) ResultFromKey(
    const Handle(Standard_Transient)& key) const;

  //! This method is used by ResultFromModel to collate the list of
  //! ResultFromTransient, avoiding duplications with a map
  //! Remark : <me> is already in the map and has not to be bound
  Standard_EXPORT void FillMap(TColStd_IndexedMapOfTransient& map) const;

  //! Fills from a TransientProcess, with the starting entity which
  //! must have been set before. It works with scopes, calls Fill
  //! on each of its sub-results
  Standard_EXPORT void Fill(const Handle(Transfer_TransientProcess)& TP);

  //! Clears some data attached to binders used by TransientProcess,
  //! which become useless once the transfer has been done :
  //! the list of sub-scoped binders, which is now recorded as
  //! sub-results
  Standard_EXPORT void Strip();

  //! Fills back a TransientProcess with definition of a
  //! ResultFromTransient, respectfully to its structuration in
  //! scopes
  Standard_EXPORT void FillBack(const Handle(Transfer_TransientProcess)& TP) const;

  DEFINE_STANDARD_RTTIEXT(Transfer_ResultFromTransient, Standard_Transient)

protected:
private:
  Handle(Standard_Transient)           thestart;
  Handle(Transfer_Binder)              thebinder;
  Handle(TColStd_HSequenceOfTransient) thesubs;
};

#endif // _Transfer_ResultFromTransient_HeaderFile
