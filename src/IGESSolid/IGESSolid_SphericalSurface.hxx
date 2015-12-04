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

#ifndef _IGESSolid_SphericalSurface_HeaderFile
#define _IGESSolid_SphericalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
class IGESGeom_Point;
class IGESGeom_Direction;
class gp_Pnt;


class IGESSolid_SphericalSurface;
DEFINE_STANDARD_HANDLE(IGESSolid_SphericalSurface, IGESData_IGESEntity)

//! defines SphericalSurface, Type <196> Form Number <0,1>
//! in package IGESSolid
//! Spherical surface is defined by a center and radius.
//! In case of parametrised surface an axis and a
//! reference direction is provided.
class IGESSolid_SphericalSurface : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_SphericalSurface();
  
  //! This method is used to set the fields of the class
  //! SphericalSurface
  //! - aCenter : the coordinates of the center point
  //! - aRadius : value of radius
  //! - anAxis  : the direction of the axis
  //! Null in case of Unparametrised surface
  //! - aRefdir : the reference direction
  //! Null in case of Unparametrised surface
  Standard_EXPORT void Init (const Handle(IGESGeom_Point)& aCenter, const Standard_Real aRadius, const Handle(IGESGeom_Direction)& anAxis, const Handle(IGESGeom_Direction)& aRefdir);
  
  //! returns the center of the spherical surface
  Standard_EXPORT Handle(IGESGeom_Point) Center() const;
  
  //! returns the center of the spherical surface after applying
  //! TransformationMatrix
  Standard_EXPORT gp_Pnt TransformedCenter() const;
  
  //! returns the radius of the spherical surface
  Standard_EXPORT Standard_Real Radius() const;
  
  //! returns the direction of the axis (Parametrised surface)
  //! Null is returned if the surface is not parametrised
  Standard_EXPORT Handle(IGESGeom_Direction) Axis() const;
  
  //! returns the reference direction (Parametrised surface)
  //! Null is returned if the surface is not parametrised
  Standard_EXPORT Handle(IGESGeom_Direction) ReferenceDir() const;
  
  //! Returns True if the surface is parametrised, else False
  Standard_EXPORT Standard_Boolean IsParametrised() const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_SphericalSurface,IGESData_IGESEntity)

protected:




private:


  Handle(IGESGeom_Point) theCenter;
  Standard_Real theRadius;
  Handle(IGESGeom_Direction) theAxis;
  Handle(IGESGeom_Direction) theRefDir;


};







#endif // _IGESSolid_SphericalSurface_HeaderFile
