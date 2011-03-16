#include <Interface_SignType.ixx>

    TCollection_AsciiString  Interface_SignType::Text
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Transient)& context) const
{
  TCollection_AsciiString atext;
  Handle(Interface_InterfaceModel) model =
    Handle(Interface_InterfaceModel)::DownCast(context);
  if (ent.IsNull() || model.IsNull()) return atext;
  atext.AssignCat (Value(ent,model));
  return atext;
}

    Standard_CString  Interface_SignType::ClassName
  (const Standard_CString typnam)
{
  char* tn =(char*) typnam;
  for (int i = 0; tn[i] != '\0'; i ++) {
    if (tn[i] == '_') return &tn[i+1];
  }
  return tn;
}
