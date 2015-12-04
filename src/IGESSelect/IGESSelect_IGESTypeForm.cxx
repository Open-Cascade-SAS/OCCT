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


#include <IGESData_IGESEntity.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <IGESSelect_IGESTypeForm.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_IGESTypeForm,IFSelect_Signature)

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
   const Handle(Interface_InterfaceModel)& /*model*/) const
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
