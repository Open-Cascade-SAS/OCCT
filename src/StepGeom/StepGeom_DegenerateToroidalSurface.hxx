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

#ifndef _StepGeom_DegenerateToroidalSurface_HeaderFile
#define _StepGeom_DegenerateToroidalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_ToroidalSurface.hxx>
class TCollection_HAsciiString;
class StepGeom_Axis2Placement3d;


class StepGeom_DegenerateToroidalSurface;
DEFINE_STANDARD_HANDLE(StepGeom_DegenerateToroidalSurface, StepGeom_ToroidalSurface)


class StepGeom_DegenerateToroidalSurface : public StepGeom_ToroidalSurface
{

public:

  
  //! Returns a DegenerateToroidalSurface
  Standard_EXPORT StepGeom_DegenerateToroidalSurface();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_Axis2Placement3d)& aPosition, const Standard_Real aMajorRadius, const Standard_Real aMinorRadius, const Standard_Boolean aSelectOuter);
  
  Standard_EXPORT void SetSelectOuter (const Standard_Boolean aSelectOuter);
  
  Standard_EXPORT Standard_Boolean SelectOuter() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_DegenerateToroidalSurface,StepGeom_ToroidalSurface)

protected:




private:


  Standard_Boolean selectOuter;


};







#endif // _StepGeom_DegenerateToroidalSurface_HeaderFile
