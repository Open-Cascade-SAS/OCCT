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

#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Transfer_TransientListBinder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_TransientListBinder, Transfer_Binder)

// #include  <TColStd.hxx>
//=================================================================================================

Transfer_TransientListBinder::Transfer_TransientListBinder()
{
  theres = new TColStd_HSequenceOfTransient();
}

//=================================================================================================

Transfer_TransientListBinder::Transfer_TransientListBinder(
  const Handle(TColStd_HSequenceOfTransient)& list)
{
  theres = list;
}

//=================================================================================================

Standard_Boolean Transfer_TransientListBinder::IsMultiple() const
{
  return (NbTransients() > 1);
}

//=================================================================================================

Handle(Standard_Type) Transfer_TransientListBinder::ResultType() const
{
  return STANDARD_TYPE(Transfer_TransientListBinder);
}

//=================================================================================================

Standard_CString Transfer_TransientListBinder::ResultTypeName() const
{
  return "list(Standard_Transient)";
}

//=================================================================================================

void Transfer_TransientListBinder::AddResult(const Handle(Standard_Transient)& Transient)
{
  theres->Append(Transient);
}

//=================================================================================================

Handle(TColStd_HSequenceOfTransient) Transfer_TransientListBinder::Result() const
{
  return theres;
}

//=================================================================================================

void Transfer_TransientListBinder::SetResult(const Standard_Integer            num,
                                             const Handle(Standard_Transient)& Transient)
{
  theres->SetValue(num, Transient);
}

//=================================================================================================

Standard_Integer Transfer_TransientListBinder::NbTransients() const
{
  return theres->Length();
}

//=================================================================================================

const Handle(Standard_Transient)& Transfer_TransientListBinder::Transient(
  const Standard_Integer num) const
{
  return theres->Value(num);
}
