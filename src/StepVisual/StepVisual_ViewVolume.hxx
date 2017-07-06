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

#ifndef _StepVisual_ViewVolume_HeaderFile
#define _StepVisual_ViewVolume_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_CentralOrParallel.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
class StepGeom_CartesianPoint;
class StepVisual_PlanarBox;


class StepVisual_ViewVolume;
DEFINE_STANDARD_HANDLE(StepVisual_ViewVolume, Standard_Transient)


class StepVisual_ViewVolume : public Standard_Transient
{

public:

  
  //! Returns a ViewVolume
  Standard_EXPORT StepVisual_ViewVolume();
  
  Standard_EXPORT void Init (const StepVisual_CentralOrParallel aProjectionType, const Handle(StepGeom_CartesianPoint)& aProjectionPoint, const Standard_Real aViewPlaneDistance, const Standard_Real aFrontPlaneDistance, const Standard_Boolean aFrontPlaneClipping, const Standard_Real aBackPlaneDistance, const Standard_Boolean aBackPlaneClipping, const Standard_Boolean aViewVolumeSidesClipping, const Handle(StepVisual_PlanarBox)& aViewWindow);
  
  Standard_EXPORT void SetProjectionType (const StepVisual_CentralOrParallel aProjectionType);
  
  Standard_EXPORT StepVisual_CentralOrParallel ProjectionType() const;
  
  Standard_EXPORT void SetProjectionPoint (const Handle(StepGeom_CartesianPoint)& aProjectionPoint);
  
  Standard_EXPORT Handle(StepGeom_CartesianPoint) ProjectionPoint() const;
  
  Standard_EXPORT void SetViewPlaneDistance (const Standard_Real aViewPlaneDistance);
  
  Standard_EXPORT Standard_Real ViewPlaneDistance() const;
  
  Standard_EXPORT void SetFrontPlaneDistance (const Standard_Real aFrontPlaneDistance);
  
  Standard_EXPORT Standard_Real FrontPlaneDistance() const;
  
  Standard_EXPORT void SetFrontPlaneClipping (const Standard_Boolean aFrontPlaneClipping);
  
  Standard_EXPORT Standard_Boolean FrontPlaneClipping() const;
  
  Standard_EXPORT void SetBackPlaneDistance (const Standard_Real aBackPlaneDistance);
  
  Standard_EXPORT Standard_Real BackPlaneDistance() const;
  
  Standard_EXPORT void SetBackPlaneClipping (const Standard_Boolean aBackPlaneClipping);
  
  Standard_EXPORT Standard_Boolean BackPlaneClipping() const;
  
  Standard_EXPORT void SetViewVolumeSidesClipping (const Standard_Boolean aViewVolumeSidesClipping);
  
  Standard_EXPORT Standard_Boolean ViewVolumeSidesClipping() const;
  
  Standard_EXPORT void SetViewWindow (const Handle(StepVisual_PlanarBox)& aViewWindow);
  
  Standard_EXPORT Handle(StepVisual_PlanarBox) ViewWindow() const;




  DEFINE_STANDARD_RTTIEXT(StepVisual_ViewVolume,Standard_Transient)

protected:




private:


  StepVisual_CentralOrParallel projectionType;
  Handle(StepGeom_CartesianPoint) projectionPoint;
  Standard_Real viewPlaneDistance;
  Standard_Real frontPlaneDistance;
  Standard_Boolean frontPlaneClipping;
  Standard_Real backPlaneDistance;
  Standard_Boolean backPlaneClipping;
  Standard_Boolean viewVolumeSidesClipping;
  Handle(StepVisual_PlanarBox) viewWindow;


};







#endif // _StepVisual_ViewVolume_HeaderFile
