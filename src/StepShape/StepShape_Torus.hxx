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

#ifndef _StepShape_Torus_HeaderFile
#define _StepShape_Torus_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_GeometricRepresentationItem.hxx>
class StepGeom_Axis1Placement;
class TCollection_HAsciiString;


class StepShape_Torus;
DEFINE_STANDARD_HANDLE(StepShape_Torus, StepGeom_GeometricRepresentationItem)


class StepShape_Torus : public StepGeom_GeometricRepresentationItem
{

public:

  
  //! Returns a Torus
  Standard_EXPORT StepShape_Torus();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_Axis1Placement)& aPosition, const Standard_Real aMajorRadius, const Standard_Real aMinorRadius);
  
  Standard_EXPORT void SetPosition (const Handle(StepGeom_Axis1Placement)& aPosition);
  
  Standard_EXPORT Handle(StepGeom_Axis1Placement) Position() const;
  
  Standard_EXPORT void SetMajorRadius (const Standard_Real aMajorRadius);
  
  Standard_EXPORT Standard_Real MajorRadius() const;
  
  Standard_EXPORT void SetMinorRadius (const Standard_Real aMinorRadius);
  
  Standard_EXPORT Standard_Real MinorRadius() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_Torus,StepGeom_GeometricRepresentationItem)

protected:




private:


  Handle(StepGeom_Axis1Placement) position;
  Standard_Real majorRadius;
  Standard_Real minorRadius;


};







#endif // _StepShape_Torus_HeaderFile
