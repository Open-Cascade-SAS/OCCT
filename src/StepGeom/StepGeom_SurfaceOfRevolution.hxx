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

#ifndef _StepGeom_SurfaceOfRevolution_HeaderFile
#define _StepGeom_SurfaceOfRevolution_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_SweptSurface.hxx>
class StepGeom_Axis1Placement;
class TCollection_HAsciiString;
class StepGeom_Curve;


class StepGeom_SurfaceOfRevolution;
DEFINE_STANDARD_HANDLE(StepGeom_SurfaceOfRevolution, StepGeom_SweptSurface)


class StepGeom_SurfaceOfRevolution : public StepGeom_SweptSurface
{

public:

  
  //! Returns a SurfaceOfRevolution
  Standard_EXPORT StepGeom_SurfaceOfRevolution();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_Curve)& aSweptCurve, const Handle(StepGeom_Axis1Placement)& aAxisPosition);
  
  Standard_EXPORT void SetAxisPosition (const Handle(StepGeom_Axis1Placement)& aAxisPosition);
  
  Standard_EXPORT Handle(StepGeom_Axis1Placement) AxisPosition() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_SurfaceOfRevolution,StepGeom_SweptSurface)

protected:




private:


  Handle(StepGeom_Axis1Placement) axisPosition;


};







#endif // _StepGeom_SurfaceOfRevolution_HeaderFile
