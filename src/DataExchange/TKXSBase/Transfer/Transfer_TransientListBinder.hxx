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

#include <TColStd_HSequenceOfTransient.hxx>
#include <Transfer_Binder.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;

class Transfer_TransientListBinder;
DEFINE_STANDARD_HANDLE(Transfer_TransientListBinder, Transfer_Binder)

//! This binder binds several (a list of) Transients with a starting
//! entity, when this entity itself corresponds to a simple list
//! of Transients. Each part is not seen as a sub-result of an
//! independent component, but as an item of a built-in list
class Transfer_TransientListBinder : public Transfer_Binder
{

public:
  Standard_EXPORT Transfer_TransientListBinder();

  Standard_EXPORT Transfer_TransientListBinder(const Handle(TColStd_HSequenceOfTransient)& list);

  Standard_EXPORT virtual Standard_Boolean IsMultiple() const Standard_OVERRIDE;

  Standard_EXPORT Handle(Standard_Type) ResultType() const Standard_OVERRIDE;

  Standard_EXPORT Standard_CString ResultTypeName() const Standard_OVERRIDE;

  //! Adds an item to the result list
  Standard_EXPORT void AddResult(const Handle(Standard_Transient)& res);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) Result() const;

  //! Changes an already defined sub-result
  Standard_EXPORT void SetResult(const Standard_Integer num, const Handle(Standard_Transient)& res);

  Standard_EXPORT Standard_Integer NbTransients() const;

  Standard_EXPORT const Handle(Standard_Transient)& Transient(const Standard_Integer num) const;

  DEFINE_STANDARD_RTTIEXT(Transfer_TransientListBinder, Transfer_Binder)

protected:
private:
  Handle(TColStd_HSequenceOfTransient) theres;
};

#endif // _Transfer_TransientListBinder_HeaderFile
