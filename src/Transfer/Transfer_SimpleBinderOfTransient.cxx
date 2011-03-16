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
