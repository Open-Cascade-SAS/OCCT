// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <Transfer_SimpleBinderOfTransient.ixx>

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
