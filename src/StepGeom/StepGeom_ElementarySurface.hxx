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

#ifndef _StepGeom_ElementarySurface_HeaderFile
#define _StepGeom_ElementarySurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_Surface.hxx>
class StepGeom_Axis2Placement3d;
class TCollection_HAsciiString;


class StepGeom_ElementarySurface;
DEFINE_STANDARD_HANDLE(StepGeom_ElementarySurface, StepGeom_Surface)


class StepGeom_ElementarySurface : public StepGeom_Surface
{

public:

  
  //! Returns a ElementarySurface
  Standard_EXPORT StepGeom_ElementarySurface();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_Axis2Placement3d)& aPosition);
  
  Standard_EXPORT void SetPosition (const Handle(StepGeom_Axis2Placement3d)& aPosition);
  
  Standard_EXPORT Handle(StepGeom_Axis2Placement3d) Position() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_ElementarySurface,StepGeom_Surface)

protected:




private:


  Handle(StepGeom_Axis2Placement3d) position;


};







#endif // _StepGeom_ElementarySurface_HeaderFile
