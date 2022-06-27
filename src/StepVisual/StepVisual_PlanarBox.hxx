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

#ifndef _StepVisual_PlanarBox_HeaderFile
#define _StepVisual_PlanarBox_HeaderFile

#include <Standard.hxx>

#include <StepGeom_Axis2Placement.hxx>
#include <StepVisual_PlanarExtent.hxx>
#include <Standard_Real.hxx>
class TCollection_HAsciiString;


class StepVisual_PlanarBox;
DEFINE_STANDARD_HANDLE(StepVisual_PlanarBox, StepVisual_PlanarExtent)


class StepVisual_PlanarBox : public StepVisual_PlanarExtent
{

public:

  
  //! Returns a PlanarBox
  Standard_EXPORT StepVisual_PlanarBox();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Standard_Real aSizeInX, const Standard_Real aSizeInY, const StepGeom_Axis2Placement& aPlacement);
  
  Standard_EXPORT void SetPlacement (const StepGeom_Axis2Placement& aPlacement);
  
  Standard_EXPORT StepGeom_Axis2Placement Placement() const;




  DEFINE_STANDARD_RTTIEXT(StepVisual_PlanarBox,StepVisual_PlanarExtent)

protected:




private:


  StepGeom_Axis2Placement placement;


};







#endif // _StepVisual_PlanarBox_HeaderFile
