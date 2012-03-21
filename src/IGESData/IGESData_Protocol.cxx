// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
