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
#include <Transfer_Binder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransferFailure.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_SimpleBinderOfTransient,Transfer_Binder)

//  "Handle(Standard_Transient)" : la classe de base pour le Resultat
Transfer_SimpleBinderOfTransient::Transfer_SimpleBinderOfTransient () { }


//    Standard_Boolean  Transfer_SimpleBinderOfTransient::IsMultiple() const
//      {  return Standard_False;  }


    Handle(Standard_Type)  Transfer_SimpleBinderOfTransient::ResultType () const
{
  if (!HasResult() || theres.IsNull()) return STANDARD_TYPE(Standard_Transient);
  return Result()->DynamicType();
}

    Standard_CString  Transfer_SimpleBinderOfTransient::ResultTypeName () const
{
  if (!HasResult() || theres.IsNull()) return "(void)";
  return Result()->DynamicType()->Name();
}


    void  Transfer_SimpleBinderOfTransient::SetResult
  (const Handle(Standard_Transient)& res)
{
  SetResultPresent();
  theres = res;
}


    const Handle(Standard_Transient)&  Transfer_SimpleBinderOfTransient::Result () const
      {  return theres;  }

    Standard_Boolean  Transfer_SimpleBinderOfTransient::GetTypedResult
  (const Handle(Transfer_Binder)& bnd, const Handle(Standard_Type)& atype,
   Handle(Standard_Transient)& res)
{
  if (atype.IsNull()) return Standard_False;
  Handle(Transfer_Binder) bn = bnd;
  while (!bn.IsNull()) {
    Handle(Transfer_SimpleBinderOfTransient) trb =
      Handle(Transfer_SimpleBinderOfTransient)::DownCast(bn);
    bn = bn->NextResult();
    if (trb.IsNull()) continue;
    Handle(Standard_Transient) rs = trb->Result();
    if (rs.IsNull()) continue;
    if (!rs->IsKind(atype)) continue;
    res = rs;
    return Standard_True;
  }
  return Standard_False;
}
