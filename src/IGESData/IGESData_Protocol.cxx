// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <IGESData_FreeFormatEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_Protocol,Interface_Protocol)

IGESData_Protocol::IGESData_Protocol ()    {  }


    Standard_Integer IGESData_Protocol::NbResources () const  { return 0; }

    Handle(Interface_Protocol) IGESData_Protocol::Resource
  (const Standard_Integer /*num*/) const
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
