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

#ifndef _StepGeom_Vector_HeaderFile
#define _StepGeom_Vector_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
class StepGeom_Direction;
class TCollection_HAsciiString;


class StepGeom_Vector;
DEFINE_STANDARD_HANDLE(StepGeom_Vector, StepGeom_GeometricRepresentationItem)


class StepGeom_Vector : public StepGeom_GeometricRepresentationItem
{

public:

  
  //! Returns a Vector
  Standard_EXPORT StepGeom_Vector();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_Direction)& aOrientation, const Standard_Real aMagnitude);
  
  Standard_EXPORT void SetOrientation (const Handle(StepGeom_Direction)& aOrientation);
  
  Standard_EXPORT Handle(StepGeom_Direction) Orientation() const;
  
  Standard_EXPORT void SetMagnitude (const Standard_Real aMagnitude);
  
  Standard_EXPORT Standard_Real Magnitude() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_Vector,StepGeom_GeometricRepresentationItem)

protected:




private:


  Handle(StepGeom_Direction) orientation;
  Standard_Real magnitude;


};







#endif // _StepGeom_Vector_HeaderFile
