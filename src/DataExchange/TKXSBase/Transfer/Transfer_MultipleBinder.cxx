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
#include <Transfer_MultipleBinder.hxx>
#include <Transfer_TransferFailure.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_MultipleBinder, Transfer_Binder)

// Multiple Result
// Possibility to define a Multiple Result : several objects resulting
//  from a Transfer, without being able to distinguish them
//  N.B. : For now, all Transients (may evolve)
Transfer_MultipleBinder::Transfer_MultipleBinder() {}

bool Transfer_MultipleBinder::IsMultiple() const
{
  if (themulres.IsNull())
    return false;
  return (themulres->Length() != 1);
}

occ::handle<Standard_Type> Transfer_MultipleBinder::ResultType() const
{
  return STANDARD_TYPE(Standard_Transient);
}

const char* Transfer_MultipleBinder::ResultTypeName() const
{
  return "(list)";
}

//  ....        Multiple Result Management        ....

void Transfer_MultipleBinder::AddResult(const occ::handle<Standard_Transient>& res)
{
  if (themulres.IsNull())
    themulres = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  themulres->Append(res);
}

int Transfer_MultipleBinder::NbResults() const
{
  return (themulres.IsNull() ? 0 : themulres->Length());
}

occ::handle<Standard_Transient> Transfer_MultipleBinder::ResultValue(const int num) const
{
  return themulres->Value(num);
}

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Transfer_MultipleBinder::MultipleResult() const
{
  if (!themulres.IsNull())
    return themulres;
  return new NCollection_HSequence<occ::handle<Standard_Transient>>();
}

void Transfer_MultipleBinder::SetMultipleResult(const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& mulres)
{
  themulres = mulres;
}
