// Created on: 1999-02-18
// Created by: Pavel DURANDIN
// Copyright (c) 1999-1999 Matra Datavision
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



#include <STEPSelections_SelectDerived.ixx>
#include <StepData_ReadWriteModule.hxx>
#include <RWStepAP214_GeneralModule.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <Interface_Macros.hxx>

STEPSelections_SelectDerived::STEPSelections_SelectDerived():StepSelect_StepType()
{
}

static Handle(Standard_Type) GetStepType(const Handle(StepData_ReadWriteModule)& module,
					 const TCollection_AsciiString& type)
{
  Handle(Standard_Type) atype;
  if(module.IsNull()) return atype;
  Standard_Integer num = module->CaseStep(type);
  if(num == 0) return atype;
  Handle(Standard_Transient) ent;
  RWStepAP214_GeneralModule genModul;
  genModul.NewVoid(num,ent);
  atype = ent->DynamicType();
  return atype;
}
				 

Standard_Boolean STEPSelections_SelectDerived::Matches(const Handle(Standard_Transient)& ent,
						      const Handle(Interface_InterfaceModel)& model,
						      const TCollection_AsciiString& text,
						      const Standard_Boolean exact) const
{
  Standard_Integer CN;
  Handle(StepData_ReadWriteModule) module;
  Standard_Boolean ok = thelib.Select (ent,module,CN);
  if(!ok) return Standard_False;
  Handle(Standard_Type) checker = GetStepType(module,text);
  if(checker.IsNull()) return Standard_False;
    
  Standard_Boolean plex = module->IsComplex(CN);
    if (!plex) {
      DeclareAndCast(Standard_Type,atype,ent);
      if (atype.IsNull()) atype = ent->DynamicType();
      return atype->SubType(checker);
    } else {
      TColStd_SequenceOfAsciiString list;
      module->ComplexType (CN,list);
      Standard_Integer nb = list.Length();
      for (Standard_Integer i = 1; i <= nb; i ++) {
	Handle(Standard_Type) atype = GetStepType(module,list.Value(i));
	if(atype->SubType(checker)) return Standard_True;
    }
  }
  return Standard_False;
}
