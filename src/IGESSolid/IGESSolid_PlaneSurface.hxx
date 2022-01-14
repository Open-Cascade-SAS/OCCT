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

#ifndef _IGESSolid_PlaneSurface_HeaderFile
#define _IGESSolid_PlaneSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESEntity.hxx>
class IGESGeom_Point;
class IGESGeom_Direction;


class IGESSolid_PlaneSurface;
DEFINE_STANDARD_HANDLE(IGESSolid_PlaneSurface, IGESData_IGESEntity)

//! defines PlaneSurface, Type <190> Form Number <0,1>
//! in package IGESSolid
//! A plane surface entity is defined by a point on the
//! surface and a normal to it.
class IGESSolid_PlaneSurface : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_PlaneSurface();
  
  //! This method is used to set the fields of the class
  //! PlaneSurface
  //! - aLocation : the point on the surface
  //! - aNormal   : the surface normal direction
  //! - refdir    : the reference direction (default NULL) for
  //! unparameterised curves
  Standard_EXPORT void Init (const Handle(IGESGeom_Point)& aLocation, const Handle(IGESGeom_Direction)& aNormal, const Handle(IGESGeom_Direction)& refdir);
  
  //! returns the point on the surface
  Standard_EXPORT Handle(IGESGeom_Point) LocationPoint() const;
  
  //! returns the normal to the surface
  Standard_EXPORT Handle(IGESGeom_Direction) Normal() const;
  
  //! returns the reference direction (for parameterised curve)
  //! returns NULL for unparameterised curve
  Standard_EXPORT Handle(IGESGeom_Direction) ReferenceDir() const;
  
  //! returns True if parameterised, else False
  Standard_EXPORT Standard_Boolean IsParametrised() const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_PlaneSurface,IGESData_IGESEntity)

protected:




private:


  Handle(IGESGeom_Point) theLocationPoint;
  Handle(IGESGeom_Direction) theNormal;
  Handle(IGESGeom_Direction) theRefDir;


};







#endif // _IGESSolid_PlaneSurface_HeaderFile
