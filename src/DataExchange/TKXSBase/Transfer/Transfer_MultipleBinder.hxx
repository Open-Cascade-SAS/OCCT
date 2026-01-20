// Created on: 1993-04-07
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Transfer_MultipleBinder_HeaderFile
#define _Transfer_MultipleBinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Transfer_Binder.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;

//! Allows direct binding between a starting Object and the Result
//! of its transfer, when it can be made of several Transient
//! Objects. Compared to a Transcriptor, it has no Transfer Action
//!
//! Result is a list of Transient Results. Unique Result is not
//! available : SetResult is redefined to start the list on the
//! first call, and refuse the other times.
//!
//! rr
//!
//! Remark : MultipleBinder itself is intended to be created and
//! filled by TransferProcess itself (method Bind). In particular,
//! conflicts between Unique (Standard) result and Multiple result
//! are avoided through management made by TransferProcess.
//!
//! Also, a Transcriptor (with an effective Transfer Method) which
//! can produce a Multiple Result, may be defined as a sub-class
//! of MultipleBinder by redefining method Transfer.
class Transfer_MultipleBinder : public Transfer_Binder
{

public:
  //! normal standard constructor, creates an empty MultipleBinder
  Standard_EXPORT Transfer_MultipleBinder();

  //! Returns True if a starting object is bound with SEVERAL
  //! results : Here, returns always True
  Standard_EXPORT virtual bool IsMultiple() const override;

  //! Returns the Type permitted for Results, i.e. here Transient
  Standard_EXPORT occ::handle<Standard_Type> ResultType() const override;

  //! Returns the Name of the Type which characterizes the Result
  //! Here, returns "(list)"
  Standard_EXPORT const char* ResultTypeName() const override;

  //! Adds a new Item to the Multiple Result
  Standard_EXPORT void AddResult(const occ::handle<Standard_Transient>& res);

  //! Returns the actual count of recorded (Transient) results
  Standard_EXPORT int NbResults() const;

  //! Returns the value of the recorded result n0 <num>
  Standard_EXPORT occ::handle<Standard_Transient> ResultValue(const int num) const;

  //! Returns the Multiple Result, if it is defined (at least one
  //! Item). Else, returns a Null Handle
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> MultipleResult() const;

  //! Defines a Binding with a Multiple Result, given as a Sequence
  //! Error if a Unique Result has yet been defined
  Standard_EXPORT void SetMultipleResult(const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& mulres);

  DEFINE_STANDARD_RTTIEXT(Transfer_MultipleBinder, Transfer_Binder)

private:
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> themulres;
};

#endif // _Transfer_MultipleBinder_HeaderFile
