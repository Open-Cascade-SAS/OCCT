// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <IFSelect_SelectUnknownEntities.ixx>


IFSelect_SelectUnknownEntities::IFSelect_SelectUnknownEntities ()    {  }

    Standard_Boolean  IFSelect_SelectUnknownEntities::Sort
  (const Standard_Integer , const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const 
      {  return model->IsUnknownEntity(model->Number(ent));  }

    TCollection_AsciiString  IFSelect_SelectUnknownEntities::ExtractLabel () const
      {  return TCollection_AsciiString("Unrecognized type Entities");  }
