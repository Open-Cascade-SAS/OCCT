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

#ifndef _StepVisual_CameraModelD2_HeaderFile
#define _StepVisual_CameraModelD2_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <StepVisual_CameraModel.hxx>
class StepVisual_PlanarBox;
class TCollection_HAsciiString;


class StepVisual_CameraModelD2;
DEFINE_STANDARD_HANDLE(StepVisual_CameraModelD2, StepVisual_CameraModel)


class StepVisual_CameraModelD2 : public StepVisual_CameraModel
{

public:

  
  //! Returns a CameraModelD2
  Standard_EXPORT StepVisual_CameraModelD2();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepVisual_PlanarBox)& aViewWindow, const Standard_Boolean aViewWindowClipping);
  
  Standard_EXPORT void SetViewWindow (const Handle(StepVisual_PlanarBox)& aViewWindow);
  
  Standard_EXPORT Handle(StepVisual_PlanarBox) ViewWindow() const;
  
  Standard_EXPORT void SetViewWindowClipping (const Standard_Boolean aViewWindowClipping);
  
  Standard_EXPORT Standard_Boolean ViewWindowClipping() const;




  DEFINE_STANDARD_RTTIEXT(StepVisual_CameraModelD2,StepVisual_CameraModel)

protected:




private:


  Handle(StepVisual_PlanarBox) viewWindow;
  Standard_Boolean viewWindowClipping;


};







#endif // _StepVisual_CameraModelD2_HeaderFile
