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


#include <IFSelect_SignValidity.hxx>
#include <Interface_Check.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SignValidity,IFSelect_Signature)

static Standard_CString nulsign = "";


//=======================================================================
//function : IFSelect_SignValidity
//purpose  : 
//=======================================================================

IFSelect_SignValidity::IFSelect_SignValidity() : IFSelect_Signature ("Validity")
{
  AddCase ("UNKNOWN");
  AddCase ("UNLOADED");
  AddCase ("Load-Error");
  AddCase ("Data-Error");
  AddCase ("Load-Warning");
  AddCase ("Data-Warning");
  AddCase ("OK");
}


//=======================================================================
//function : CVal
//purpose  : 
//=======================================================================

Standard_CString IFSelect_SignValidity::CVal(const Handle(Standard_Transient)& ent,
                                             const Handle(Interface_InterfaceModel)& model)
{
  if (ent.IsNull() || model.IsNull()) return nulsign;
  Standard_Integer num = model->Number(ent);
  Standard_Integer cas = 0;
  if (model->IsUnknownEntity(num))    return "UNKNOWN";
  if (model->IsRedefinedContent(num)) return "UNLOADED";

  const Handle(Interface_Check) ch1 = model->Check(num,Standard_True);
  const Handle(Interface_Check) ch2 = model->Check(num,Standard_False);
  if (ch1->NbFails() > 0) return "Load-Error";
  else if (ch1->NbWarnings() > 0) cas = 1;

  if (ch2->NbFails() > 0) return "Data-Error";
  else if (cas == 1) return "Load-Warning";
  else if (ch2->NbWarnings() > 0) return "Data-Warning";

  return "OK";
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_CString IFSelect_SignValidity::Value
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  return IFSelect_SignValidity::CVal(ent,model);
}
