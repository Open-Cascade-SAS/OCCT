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

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransferFailure.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_SimpleBinderOfTransient, Transfer_Binder)

//  "occ::handle<Standard_Transient>": the base class for the Result
Transfer_SimpleBinderOfTransient::Transfer_SimpleBinderOfTransient() = default;

//    bool  Transfer_SimpleBinderOfTransient::IsMultiple() const
//      {  return false;  }

occ::handle<Standard_Type> Transfer_SimpleBinderOfTransient::ResultType() const
{
  if (!HasResult() || theres.IsNull())
    return STANDARD_TYPE(Standard_Transient);
  return Result()->DynamicType();
}

const char* Transfer_SimpleBinderOfTransient::ResultTypeName() const
{
  if (!HasResult() || theres.IsNull())
    return "(void)";
  return Result()->DynamicType()->Name();
}

void Transfer_SimpleBinderOfTransient::SetResult(const occ::handle<Standard_Transient>& res)
{
  SetResultPresent();
  theres = res;
}

const occ::handle<Standard_Transient>& Transfer_SimpleBinderOfTransient::Result() const
{
  return theres;
}

bool Transfer_SimpleBinderOfTransient::GetTypedResult(const occ::handle<Transfer_Binder>& bnd,
                                                      const occ::handle<Standard_Type>&   atype,
                                                      occ::handle<Standard_Transient>&    res)
{
  if (atype.IsNull())
    return false;
  occ::handle<Transfer_Binder> bn = bnd;
  while (!bn.IsNull())
  {
    occ::handle<Transfer_SimpleBinderOfTransient> trb =
      occ::down_cast<Transfer_SimpleBinderOfTransient>(bn);
    bn = bn->NextResult();
    if (trb.IsNull())
      continue;
    const occ::handle<Standard_Transient>& rs = trb->Result();
    if (rs.IsNull())
      continue;
    if (!rs->IsKind(atype))
      continue;
    res = rs;
    return true;
  }
  return false;
}
