#include <Interface_SignLabel.ixx>
#include <Interface_InterfaceModel.hxx>
#include <TCollection_HAsciiString.hxx>


    Interface_SignLabel::Interface_SignLabel  ()    {  }

    Standard_CString  Interface_SignLabel::Name () const
      {  return "Entity Label";  }

    TCollection_AsciiString  Interface_SignLabel::Text
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Transient)& context) const
{
  TCollection_AsciiString atext;
  Handle(Interface_InterfaceModel) model =
    Handle(Interface_InterfaceModel)::DownCast(context);
  if (ent.IsNull() || model.IsNull()) return atext;
  Handle(TCollection_HAsciiString) lab = model->StringLabel (ent);
  if (!lab.IsNull()) atext = lab->String();
  return atext;
}
