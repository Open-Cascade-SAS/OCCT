// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESData_IGESEntity.hxx>
#include <IGESSolid_SolidInstance.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_SolidInstance,IGESData_IGESEntity)

IGESSolid_SolidInstance::IGESSolid_SolidInstance ()    {  }


    void  IGESSolid_SolidInstance::Init
  (const Handle(IGESData_IGESEntity)& anEntity)
{
  theEntity = anEntity;
  InitTypeAndForm(430,0);
}

    Standard_Boolean  IGESSolid_SolidInstance::IsBrep () const
      {  return (FormNumber() == 1);  }

    void  IGESSolid_SolidInstance::SetBrep (const Standard_Boolean brep)
      {  InitTypeAndForm(430, (brep ? 1 : 0));  }

    Handle(IGESData_IGESEntity)  IGESSolid_SolidInstance::Entity () const
{
  return theEntity;
}
