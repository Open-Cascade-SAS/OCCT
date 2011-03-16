#include <Interface_ReaderModule.ixx>


Standard_Boolean Interface_ReaderModule::NewRead(const Standard_Integer,
                                                 const Handle(Interface_FileReaderData)&,
                                                 const Standard_Integer,
                                                 Handle(Interface_Check)&,
                                                 Handle(Standard_Transient)& ) const
{
  return Standard_False;
}
