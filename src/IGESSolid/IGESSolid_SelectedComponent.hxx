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

#ifndef _IGESSolid_SelectedComponent_HeaderFile
#define _IGESSolid_SelectedComponent_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESSolid_BooleanTree;
class gp_Pnt;


class IGESSolid_SelectedComponent;
DEFINE_STANDARD_HANDLE(IGESSolid_SelectedComponent, IGESData_IGESEntity)

//! defines SelectedComponent, Type <182> Form Number <0>
//! in package IGESSolid
//! The Selected Component entity provides a means of
//! selecting one component of a disjoint CSG solid
class IGESSolid_SelectedComponent : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_SelectedComponent();
  
  //! This method is used to set the fields of the class
  //! SelectedComponent
  //! - anEntity  : the Boolean tree entity
  //! - selectPnt : Point in or on the desired component
  Standard_EXPORT void Init (const Handle(IGESSolid_BooleanTree)& anEntity, const gp_XYZ& selectPnt);
  
  //! returns the Boolean tree entity
  Standard_EXPORT Handle(IGESSolid_BooleanTree) Component() const;
  
  //! returns the point on/in the selected component
  Standard_EXPORT gp_Pnt SelectPoint() const;
  
  //! returns the point on/in the selected component
  //! after applying TransformationMatrix
  Standard_EXPORT gp_Pnt TransformedSelectPoint() const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_SelectedComponent,IGESData_IGESEntity)

protected:




private:


  Handle(IGESSolid_BooleanTree) theEntity;
  gp_XYZ theSelectPoint;


};







#endif // _IGESSolid_SelectedComponent_HeaderFile
