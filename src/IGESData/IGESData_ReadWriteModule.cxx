#include <IGESData_ReadWriteModule.ixx>
#include <IGESData_IGESType.hxx>
#include <Interface_Macros.hxx>




Standard_Integer  IGESData_ReadWriteModule::CaseNum
  (const Handle(Interface_FileReaderData)& data,
   const Standard_Integer num) const 
{
  IGESData_IGESType DT = GetCasted(IGESData_IGESReaderData,data)->DirType(num);
  return CaseIGES (DT.Type() , DT.Form());
}


void  IGESData_ReadWriteModule::Read
  (const Standard_Integer ,
   const Handle(Interface_FileReaderData)& , const Standard_Integer ,
   Handle(Interface_Check)& , const Handle(Standard_Transient)& ) const 
{
#ifdef DEB
  cout<<"IGESData_ReadWriteModule, Read called"<<endl;
#endif
}
  // IGESReaderTool fait tout
