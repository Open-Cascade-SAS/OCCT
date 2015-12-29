// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepShape_Sphere_HeaderFile
#define _StepShape_Sphere_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
class StepGeom_Point;
class TCollection_HAsciiString;


class StepShape_Sphere;
DEFINE_STANDARD_HANDLE(StepShape_Sphere, StepGeom_GeometricRepresentationItem)


class StepShape_Sphere : public StepGeom_GeometricRepresentationItem
{

public:

  
  //! Returns a Sphere
  Standard_EXPORT StepShape_Sphere();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Standard_Real aRadius, const Handle(StepGeom_Point)& aCentre);
  
  Standard_EXPORT void SetRadius (const Standard_Real aRadius);
  
  Standard_EXPORT Standard_Real Radius() const;
  
  Standard_EXPORT void SetCentre (const Handle(StepGeom_Point)& aCentre);
  
  Standard_EXPORT Handle(StepGeom_Point) Centre() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_Sphere,StepGeom_GeometricRepresentationItem)

protected:




private:


  Standard_Real radius;
  Handle(StepGeom_Point) centre;


};







#endif // _StepShape_Sphere_HeaderFile
