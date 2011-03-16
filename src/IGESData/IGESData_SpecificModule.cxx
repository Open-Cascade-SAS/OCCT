#include <IGESData_SpecificModule.ixx>


    Standard_Boolean  IGESData_SpecificModule::OwnCorrect
  (const Standard_Integer , const Handle(IGESData_IGESEntity)& ) const
{  return Standard_False;  }    // par defaut, ne fait rien
