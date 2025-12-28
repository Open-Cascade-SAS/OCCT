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
#include <Standard_Transient.hxx>
class StepGeom_CartesianPoint;
class StepVisual_PlanarBox;

class StepVisual_ViewVolume : public Standard_Transient
{

public:
  //! Returns a ViewVolume
  Standard_EXPORT StepVisual_ViewVolume();

  Standard_EXPORT void Init(const StepVisual_CentralOrParallel          aProjectionType,
                            const occ::handle<StepGeom_CartesianPoint>& aProjectionPoint,
                            const double                                aViewPlaneDistance,
                            const double                                aFrontPlaneDistance,
                            const bool                                  aFrontPlaneClipping,
                            const double                                aBackPlaneDistance,
                            const bool                                  aBackPlaneClipping,
                            const bool                                  aViewVolumeSidesClipping,
                            const occ::handle<StepVisual_PlanarBox>&    aViewWindow);

  Standard_EXPORT void SetProjectionType(const StepVisual_CentralOrParallel aProjectionType);

  Standard_EXPORT StepVisual_CentralOrParallel ProjectionType() const;

  Standard_EXPORT void SetProjectionPoint(
    const occ::handle<StepGeom_CartesianPoint>& aProjectionPoint);

  Standard_EXPORT occ::handle<StepGeom_CartesianPoint> ProjectionPoint() const;

  Standard_EXPORT void SetViewPlaneDistance(const double aViewPlaneDistance);

  Standard_EXPORT double ViewPlaneDistance() const;

  Standard_EXPORT void SetFrontPlaneDistance(const double aFrontPlaneDistance);

  Standard_EXPORT double FrontPlaneDistance() const;

  Standard_EXPORT void SetFrontPlaneClipping(const bool aFrontPlaneClipping);

  Standard_EXPORT bool FrontPlaneClipping() const;

  Standard_EXPORT void SetBackPlaneDistance(const double aBackPlaneDistance);

  Standard_EXPORT double BackPlaneDistance() const;

  Standard_EXPORT void SetBackPlaneClipping(const bool aBackPlaneClipping);

  Standard_EXPORT bool BackPlaneClipping() const;

  Standard_EXPORT void SetViewVolumeSidesClipping(const bool aViewVolumeSidesClipping);

  Standard_EXPORT bool ViewVolumeSidesClipping() const;

  Standard_EXPORT void SetViewWindow(const occ::handle<StepVisual_PlanarBox>& aViewWindow);

  Standard_EXPORT occ::handle<StepVisual_PlanarBox> ViewWindow() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_ViewVolume, Standard_Transient)

private:
  StepVisual_CentralOrParallel         projectionType;
  occ::handle<StepGeom_CartesianPoint> projectionPoint;
  double                               viewPlaneDistance;
  double                               frontPlaneDistance;
  bool                                 frontPlaneClipping;
  double                               backPlaneDistance;
  bool                                 backPlaneClipping;
  bool                                 viewVolumeSidesClipping;
  occ::handle<StepVisual_PlanarBox>    viewWindow;
};

#endif // _StepVisual_ViewVolume_HeaderFile
