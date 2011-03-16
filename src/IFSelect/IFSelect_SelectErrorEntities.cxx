#include <IFSelect_SelectErrorEntities.ixx>


    IFSelect_SelectErrorEntities::IFSelect_SelectErrorEntities ()    {  }

    Standard_Boolean  IFSelect_SelectErrorEntities::Sort
  (const Standard_Integer , const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const 
//.. ne peut pas marcher, il faut aussi le modele ! ex. via le graphe ...
      {  return model->IsErrorEntity (model->Number(ent));  }

    TCollection_AsciiString IFSelect_SelectErrorEntities::ExtractLabel () const
      {  return TCollection_AsciiString("Error Entities");  }
