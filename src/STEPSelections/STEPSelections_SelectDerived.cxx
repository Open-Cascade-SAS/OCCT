// File:	StepSelect_StepDerived.cxx
// Created:	Thu Feb 18 12:36:51 1999
// Author:	Pavel DURANDIN
//		<pdn@friendox.nnov.matra-dtv.fr>


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
