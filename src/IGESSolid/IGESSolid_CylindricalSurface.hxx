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

#ifndef _IGESSolid_CylindricalSurface_HeaderFile
#define _IGESSolid_CylindricalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
class IGESGeom_Point;
class IGESGeom_Direction;


class IGESSolid_CylindricalSurface;
DEFINE_STANDARD_HANDLE(IGESSolid_CylindricalSurface, IGESData_IGESEntity)

//! defines CylindricalSurface, Type <192> Form Number <0,1>
//! in package IGESSolid
class IGESSolid_CylindricalSurface : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_CylindricalSurface();
  
  //! This method is used to set the fields of the class
  //! CylindricalSurface
  //! - aLocation : the location of the point on axis
  //! - anAxis    : the direction of the axis
  //! - aRadius   : the radius at the axis point
  //! - aRefdir   : the reference direction (parametrised surface)
  //! default NULL (unparametrised surface)
  Standard_EXPORT void Init (const Handle(IGESGeom_Point)& aLocation, const Handle(IGESGeom_Direction)& anAxis, const Standard_Real aRadius, const Handle(IGESGeom_Direction)& aRefdir);
  
  //! returns the point on the axis
  Standard_EXPORT Handle(IGESGeom_Point) LocationPoint() const;
  
  //! returns the direction on the axis
  Standard_EXPORT Handle(IGESGeom_Direction) Axis() const;
  
  //! returns the radius at the axis point
  Standard_EXPORT Standard_Real Radius() const;
  
  //! returns whether the surface is parametrised or not
  Standard_EXPORT Standard_Boolean IsParametrised() const;
  
  //! returns the reference direction only for parametrised surface
  //! else returns NULL
  Standard_EXPORT Handle(IGESGeom_Direction) ReferenceDir() const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_CylindricalSurface,IGESData_IGESEntity)

protected:




private:


  Handle(IGESGeom_Point) theLocationPoint;
  Handle(IGESGeom_Direction) theAxis;
  Standard_Real theRadius;
  Handle(IGESGeom_Direction) theRefDir;


};







#endif // _IGESSolid_CylindricalSurface_HeaderFile
