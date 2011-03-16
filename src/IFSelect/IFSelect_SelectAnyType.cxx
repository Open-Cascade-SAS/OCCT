#include <IFSelect_SelectAnyType.ixx>


    Standard_Boolean  IFSelect_SelectAnyType::Sort
  (const Standard_Integer , const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& ) const 
      {  return ent->IsKind(TypeForMatch());  }
