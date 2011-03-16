#include <StepData_DescrGeneral.ixx>
#include <StepData_EDescr.hxx>
#include <StepData_Described.hxx>


    StepData_DescrGeneral::StepData_DescrGeneral
  (const Handle(StepData_Protocol)& proto)
    :  theproto (proto)    {  }


    void  StepData_DescrGeneral::FillSharedCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& iter) const
{
  Handle(StepData_Described) ds = Handle(StepData_Described)::DownCast(ent);
  if (!ds.IsNull()) ds->Shared (iter);
}


void StepData_DescrGeneral::CheckCase(const Standard_Integer CN,
                                      const Handle(Standard_Transient)& ent,
                                      const Interface_ShareTool& shares,
                                      Handle(Interface_Check)& ach) const
{
}    // pour l instant


    void  StepData_DescrGeneral::CopyCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& entfrom,
   const Handle(Standard_Transient)& entto, Interface_CopyTool& TC) const
{  }    // pour l instant

    Standard_Boolean  StepData_DescrGeneral::NewVoid
  (const Standard_Integer CN, Handle(Standard_Transient)& ent) const
{
  ent = theproto->Descr(CN)->NewEntity();
  return (!ent.IsNull());
}
