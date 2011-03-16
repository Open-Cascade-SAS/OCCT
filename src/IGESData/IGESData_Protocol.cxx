#include <IGESData_Protocol.ixx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <IGESData_FreeFormatEntity.hxx>
#include <IGESData_IGESModel.hxx>



    IGESData_Protocol::IGESData_Protocol ()    {  }


    Standard_Integer IGESData_Protocol::NbResources () const  { return 0; }

    Handle(Interface_Protocol) IGESData_Protocol::Resource
  (const Standard_Integer num) const
      {  Handle(Interface_Protocol) nulpro;  return nulpro;  }


//  TypeNumber : Ici, on reconnait UndefinedEntity (faut bien quelqu un)

    Standard_Integer IGESData_Protocol::TypeNumber
  (const Handle(Standard_Type)& atype) const
{
  if (atype->SubType(STANDARD_TYPE(IGESData_UndefinedEntity))) return 1;
  return 0;
}

    Handle(Interface_InterfaceModel) IGESData_Protocol::NewModel () const
      {  return new IGESData_IGESModel;  }

    Standard_Boolean IGESData_Protocol::IsSuitableModel
  (const Handle(Interface_InterfaceModel)& model) const
      {  return model->IsKind(STANDARD_TYPE(IGESData_IGESModel));  }

    Handle(Standard_Transient) IGESData_Protocol::UnknownEntity () const
      {  return new IGESData_UndefinedEntity;  }

    Standard_Boolean IGESData_Protocol::IsUnknownEntity
  (const Handle(Standard_Transient)& ent) const
      {  return ent->IsKind(STANDARD_TYPE(IGESData_UndefinedEntity));  }
