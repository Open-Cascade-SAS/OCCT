#include <StepData_DefaultGeneral.ixx>
#include <StepData.hxx>
#include <Interface_GeneralLib.hxx>
#include <StepData_UndefinedEntity.hxx>
#include <Interface_UndefinedContent.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ParamType.hxx>


//  DefaultGeneral de StepData  reconnait  UN SEUL TYPE : UndefinedEntity


    StepData_DefaultGeneral::StepData_DefaultGeneral ()
{
  Interface_GeneralLib::SetGlobal (this, StepData::Protocol());
}

    void  StepData_DefaultGeneral::FillSharedCase
  (const Standard_Integer casenum, const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& iter) const 
{
  if (casenum != 1) return;
  DeclareAndCast(StepData_UndefinedEntity,undf,ent);
  Handle(Interface_UndefinedContent) cont = undf->UndefinedContent();
  Standard_Integer nb = cont->NbParams();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Interface_ParamType ptype = cont->ParamType(i);
    if (ptype == Interface_ParamSub) {
      DeclareAndCast(StepData_UndefinedEntity,subent,cont->ParamEntity(i));
      FillSharedCase(casenum,cont->ParamEntity(i),iter);
    } else if (ptype == Interface_ParamIdent) {
      iter.GetOneItem(cont->ParamEntity(i));
    }
  }
}


void  StepData_DefaultGeneral::CheckCase(const Standard_Integer casenum,
                                         const Handle(Standard_Transient)& ent,
                                         const Interface_ShareTool& shares,
                                         Handle(Interface_Check)& ach) const 
{
}  //  pas de Check sur une UndefinedEntity


    Standard_Boolean  StepData_DefaultGeneral::NewVoid
  (const Standard_Integer CN, Handle(Standard_Transient)& ent) const 
{
  if (CN != 1) return Standard_False;
  ent = new StepData_UndefinedEntity;
  return Standard_True;
}

    void  StepData_DefaultGeneral::CopyCase
  (const Standard_Integer casenum, const Handle(Standard_Transient)& entfrom,
   const Handle(Standard_Transient)& entto, Interface_CopyTool& TC) const 
{
  if (casenum != 1) return;
  DeclareAndCast(StepData_UndefinedEntity,undfrom,entfrom);
  DeclareAndCast(StepData_UndefinedEntity,undto,entto);
  undto->GetFromAnother(undfrom,TC);  //  On pourrait rapatrier cela
}
