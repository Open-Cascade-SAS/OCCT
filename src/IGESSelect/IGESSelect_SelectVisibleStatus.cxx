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

#include <IGESSelect_SelectVisibleStatus.ixx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>



IGESSelect_SelectVisibleStatus::IGESSelect_SelectVisibleStatus ()    {  }


    Standard_Boolean  IGESSelect_SelectVisibleStatus::Sort
  (const Standard_Integer /*rank*/, 
   const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& /*model*/) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return Standard_False;
  return (igesent->BlankStatus() == 0);
}

    TCollection_AsciiString  IGESSelect_SelectVisibleStatus::ExtractLabel
  () const
      {  return TCollection_AsciiString ("IGES Entity, Status Visible");  }
