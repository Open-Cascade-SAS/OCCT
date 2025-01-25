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

#ifndef _StepShape_CsgPrimitive_HeaderFile
#define _StepShape_CsgPrimitive_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepShape_Sphere;
class StepShape_Block;
class StepShape_RightAngularWedge;
class StepShape_Torus;
class StepShape_RightCircularCone;
class StepShape_RightCircularCylinder;

class StepShape_CsgPrimitive : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns a CsgPrimitive SelectType
  Standard_EXPORT StepShape_CsgPrimitive();

  //! Recognizes a CsgPrimitive Kind Entity that is :
  //! 1 -> Sphere
  //! 2 -> Block
  //! 3 -> RightAngularWedge
  //! 4 -> Torus
  //! 5 -> RightCircularCone
  //! 6 -> RightCircularCylinder
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! returns Value as a Sphere (Null if another type)
  Standard_EXPORT Handle(StepShape_Sphere) Sphere() const;

  //! returns Value as a Block (Null if another type)
  Standard_EXPORT Handle(StepShape_Block) Block() const;

  //! returns Value as a RightAngularWedge (Null if another type)
  Standard_EXPORT Handle(StepShape_RightAngularWedge) RightAngularWedge() const;

  //! returns Value as a Torus (Null if another type)
  Standard_EXPORT Handle(StepShape_Torus) Torus() const;

  //! returns Value as a RightCircularCone (Null if another type)
  Standard_EXPORT Handle(StepShape_RightCircularCone) RightCircularCone() const;

  //! returns Value as a RightCircularCylinder (Null if another type)
  Standard_EXPORT Handle(StepShape_RightCircularCylinder) RightCircularCylinder() const;

protected:
private:
};

#endif // _StepShape_CsgPrimitive_HeaderFile
