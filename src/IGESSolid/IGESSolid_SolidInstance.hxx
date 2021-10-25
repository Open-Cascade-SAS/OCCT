// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
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

#ifndef _IGESSolid_SolidInstance_HeaderFile
#define _IGESSolid_SolidInstance_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>


class IGESSolid_SolidInstance;
DEFINE_STANDARD_HANDLE(IGESSolid_SolidInstance, IGESData_IGESEntity)

//! defines SolidInstance, Type <430> Form Number <0>
//! in package IGESSolid
//! This provides a mechanism for replicating a solid
//! representation.
//!
//! From IGES-5.3, Form may be <1> for a BREP
//! Else it is for a Boolean Tree, Primitive, other Solid Inst.
class IGESSolid_SolidInstance : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_SolidInstance();
  
  //! This method is used to set the fields of the class
  //! SolidInstance
  //! - anEntity : the entity corresponding to the solid
  Standard_EXPORT void Init (const Handle(IGESData_IGESEntity)& anEntity);
  
  //! Tells if a SolidInstance is for a BREP
  //! Default is False
  Standard_EXPORT Standard_Boolean IsBrep() const;
  
  //! Sets or unsets the Brep status (FormNumber = 1 else 0)
  Standard_EXPORT void SetBrep (const Standard_Boolean brep);
  
  //! returns the solid entity
  Standard_EXPORT Handle(IGESData_IGESEntity) Entity() const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_SolidInstance,IGESData_IGESEntity)

protected:




private:


  Handle(IGESData_IGESEntity) theEntity;


};







#endif // _IGESSolid_SolidInstance_HeaderFile
