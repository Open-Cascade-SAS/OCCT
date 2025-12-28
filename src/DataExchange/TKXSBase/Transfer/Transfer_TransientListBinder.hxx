// Created on: 1994-10-03
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Transfer_TransientListBinder_HeaderFile
#define _Transfer_TransientListBinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Transfer_Binder.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;

//! This binder binds several (a list of) Transients with a starting
//! entity, when this entity itself corresponds to a simple list
//! of Transients. Each part is not seen as a sub-result of an
//! independent component, but as an item of a built-in list
class Transfer_TransientListBinder : public Transfer_Binder
{

public:
  Standard_EXPORT Transfer_TransientListBinder();

  Standard_EXPORT Transfer_TransientListBinder(
    const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list);

  Standard_EXPORT virtual bool IsMultiple() const override;

  Standard_EXPORT occ::handle<Standard_Type> ResultType() const override;

  Standard_EXPORT const char* ResultTypeName() const override;

  //! Adds an item to the result list
  Standard_EXPORT void AddResult(const occ::handle<Standard_Transient>& res);

  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Result()
    const;

  //! Changes an already defined sub-result
  Standard_EXPORT void SetResult(const int num, const occ::handle<Standard_Transient>& res);

  Standard_EXPORT int NbTransients() const;

  Standard_EXPORT const occ::handle<Standard_Transient>& Transient(const int num) const;

  DEFINE_STANDARD_RTTIEXT(Transfer_TransientListBinder, Transfer_Binder)

private:
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> theres;
};

#endif // _Transfer_TransientListBinder_HeaderFile
