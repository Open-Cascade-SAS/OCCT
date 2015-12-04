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

#ifndef _IGESSolid_ToroidalSurface_HeaderFile
#define _IGESSolid_ToroidalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
class IGESGeom_Point;
class IGESGeom_Direction;
class gp_Pnt;


class IGESSolid_ToroidalSurface;
DEFINE_STANDARD_HANDLE(IGESSolid_ToroidalSurface, IGESData_IGESEntity)

//! defines ToroidalSurface, Type <198> Form Number <0,1>
//! in package IGESSolid
//! This entity is defined by the center point, the axis
//! direction and the major and minor radii. In case of
//! parametrised surface a reference direction is provided.
class IGESSolid_ToroidalSurface : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_ToroidalSurface();
  
  //! This method is used to set the fields of the class
  //! ToroidalSurface
  //! - aCenter   : the center point coordinates
  //! - anAxis    : the direction of the axis
  //! - majRadius : the major radius
  //! - minRadius : the minor radius
  //! - Refdir    : the reference direction (parametrised)
  //! default Null for unparametrised surface
  Standard_EXPORT void Init (const Handle(IGESGeom_Point)& aCenter, const Handle(IGESGeom_Direction)& anAxis, const Standard_Real majRadius, const Standard_Real minRadius, const Handle(IGESGeom_Direction)& Refdir);
  
  //! returns the center point coordinates of the surface
  Standard_EXPORT Handle(IGESGeom_Point) Center() const;
  
  //! returns the center point coordinates of the surface
  //! after applying TransformationMatrix
  Standard_EXPORT gp_Pnt TransformedCenter() const;
  
  //! returns the direction of the axis
  Standard_EXPORT Handle(IGESGeom_Direction) Axis() const;
  
  //! returns the major radius of the surface
  Standard_EXPORT Standard_Real MajorRadius() const;
  
  //! returns the minor radius of the surface
  Standard_EXPORT Standard_Real MinorRadius() const;
  
  //! returns the reference direction (parametrised surface)
  //! Null is returned if the surface is not parametrised
  Standard_EXPORT Handle(IGESGeom_Direction) ReferenceDir() const;
  
  //! Returns True if the surface is parametrised, else False
  Standard_EXPORT Standard_Boolean IsParametrised() const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_ToroidalSurface,IGESData_IGESEntity)

protected:




private:


  Handle(IGESGeom_Point) theCenter;
  Handle(IGESGeom_Direction) theAxis;
  Standard_Real theMajorRadius;
  Standard_Real theMinorRadius;
  Handle(IGESGeom_Direction) theRefDir;


};







#endif // _IGESSolid_ToroidalSurface_HeaderFile
