#include <IFSelect_SignType.ixx>
#include <Standard_Type.hxx>
#include <Interface_Macros.hxx>

static Standard_CString nulsign = "";


    IFSelect_SignType::IFSelect_SignType (const Standard_Boolean nopk)
    : IFSelect_Signature ((Standard_CString ) (nopk ? "Class Type" : "Dynamic Type") ) ,
      thenopk (nopk)
{}

    Standard_CString IFSelect_SignType::Value
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  if (ent.IsNull()) return nulsign;
  DeclareAndCast(Standard_Type,atype,ent);
  if (atype.IsNull()) atype = ent->DynamicType();
  Standard_CString tn = atype->Name();
  if (!thenopk) return tn;
  for (int i = 0; tn[i] != '\0'; i ++) {
    if (tn[i] == '_') return &tn[i+1];
  }
  return tn;
}
