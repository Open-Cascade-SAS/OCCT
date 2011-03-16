#include <IGESSelect_IGESTypeForm.ixx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <Interface_Macros.hxx>
#include <stdio.h>


static char falsetype [] = "?";
static char typeval [30];    // une seule reponse a la fois ...



    IGESSelect_IGESTypeForm::IGESSelect_IGESTypeForm
  (const Standard_Boolean withform)
//JR/Hp
: IFSelect_Signature ((Standard_CString ) (withform ? "IGES Type & Form Numbers" : "IGES Type Number"))
//: IFSelect_Signature (withform ? "IGES Type & Form Numbers" : "IGES Type Number")
      {  theform = withform;  }

    void  IGESSelect_IGESTypeForm::SetForm (const Standard_Boolean withform)
      {  theform = withform;  thename.Clear();
//JR/Hp
         Standard_CString astr = (Standard_CString ) (withform ? "IGES Type & Form Numbers" : "IGES Type Number") ;
         thename.AssignCat ( astr );  }
//         thename.AssignCat (withform ? "IGES Type & Form Numbers" : "IGES Type Number") ;

    Standard_CString  IGESSelect_IGESTypeForm::Value
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return &falsetype[0];
  Standard_Boolean unk = ent->IsKind(STANDARD_TYPE(IGESData_UndefinedEntity));
  Standard_Integer typenum = igesent->TypeNumber();
  Standard_Integer formnum = igesent->FormNumber();
  if (unk) {
    if (theform) sprintf (typeval,"%d %d (?)",typenum,formnum);
    else         sprintf (typeval,"%d (?)",typenum);
  } else {
    if (theform) sprintf (typeval,"%d %d",typenum,formnum);
    else         sprintf (typeval,"%d",   typenum);
  }
  return &typeval[0];
}
