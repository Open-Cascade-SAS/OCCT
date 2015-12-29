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

#ifndef _StepGeom_Axis2Placement2d_HeaderFile
#define _StepGeom_Axis2Placement2d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <StepGeom_Placement.hxx>
class StepGeom_Direction;
class TCollection_HAsciiString;
class StepGeom_CartesianPoint;


class StepGeom_Axis2Placement2d;
DEFINE_STANDARD_HANDLE(StepGeom_Axis2Placement2d, StepGeom_Placement)


class StepGeom_Axis2Placement2d : public StepGeom_Placement
{

public:

  
  //! Returns a Axis2Placement2d
  Standard_EXPORT StepGeom_Axis2Placement2d();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_CartesianPoint)& aLocation, const Standard_Boolean hasArefDirection, const Handle(StepGeom_Direction)& aRefDirection);
  
  Standard_EXPORT void SetRefDirection (const Handle(StepGeom_Direction)& aRefDirection);
  
  Standard_EXPORT void UnSetRefDirection();
  
  Standard_EXPORT Handle(StepGeom_Direction) RefDirection() const;
  
  Standard_EXPORT Standard_Boolean HasRefDirection() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_Axis2Placement2d,StepGeom_Placement)

protected:




private:


  Handle(StepGeom_Direction) refDirection;
  Standard_Boolean hasRefDirection;


};







#endif // _StepGeom_Axis2Placement2d_HeaderFile
