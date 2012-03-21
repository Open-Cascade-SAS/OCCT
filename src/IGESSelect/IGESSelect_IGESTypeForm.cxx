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
