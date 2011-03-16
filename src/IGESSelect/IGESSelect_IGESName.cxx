#include <IGESSelect_IGESName.ixx>
#include <IGESData_IGESEntity.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Macros.hxx>

static char falsetype [] = "?";
static char voidlabel [] = "";



    IGESSelect_IGESName::IGESSelect_IGESName ()
    : IFSelect_Signature ("IGES Name (Short Label)")      {  }

    Standard_CString  IGESSelect_IGESName::Value
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return &falsetype[0];
  Handle(TCollection_HAsciiString) label = igesent->ShortLabel();
  if (label.IsNull()) return &voidlabel[0];
  return label->ToCString();
}
