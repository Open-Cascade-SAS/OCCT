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

#include <IFSelect_SignValidity.ixx>
#include <Interface_Check.hxx>


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
