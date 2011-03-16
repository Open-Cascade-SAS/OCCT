#include <IFSelect_SelectUnknownEntities.ixx>


    IFSelect_SelectUnknownEntities::IFSelect_SelectUnknownEntities ()    {  }

    Standard_Boolean  IFSelect_SelectUnknownEntities::Sort
  (const Standard_Integer , const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const 
      {  return model->IsUnknownEntity(model->Number(ent));  }

    TCollection_AsciiString  IFSelect_SelectUnknownEntities::ExtractLabel () const
      {  return TCollection_AsciiString("Unrecognized type Entities");  }
