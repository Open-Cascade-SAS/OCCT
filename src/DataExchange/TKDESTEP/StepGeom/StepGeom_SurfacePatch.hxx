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

#ifndef _StepGeom_SurfacePatch_HeaderFile
#define _StepGeom_SurfacePatch_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_TransitionCode.hxx>
#include <Standard_Transient.hxx>
class StepGeom_BoundedSurface;

class StepGeom_SurfacePatch;
DEFINE_STANDARD_HANDLE(StepGeom_SurfacePatch, Standard_Transient)

class StepGeom_SurfacePatch : public Standard_Transient
{

public:
  //! Returns a SurfacePatch
  Standard_EXPORT StepGeom_SurfacePatch();

  Standard_EXPORT void Init(const Handle(StepGeom_BoundedSurface)& aParentSurface,
                            const StepGeom_TransitionCode          aUTransition,
                            const StepGeom_TransitionCode          aVTransition,
                            const Standard_Boolean                 aUSense,
                            const Standard_Boolean                 aVSense);

  Standard_EXPORT void SetParentSurface(const Handle(StepGeom_BoundedSurface)& aParentSurface);

  Standard_EXPORT Handle(StepGeom_BoundedSurface) ParentSurface() const;

  Standard_EXPORT void SetUTransition(const StepGeom_TransitionCode aUTransition);

  Standard_EXPORT StepGeom_TransitionCode UTransition() const;

  Standard_EXPORT void SetVTransition(const StepGeom_TransitionCode aVTransition);

  Standard_EXPORT StepGeom_TransitionCode VTransition() const;

  Standard_EXPORT void SetUSense(const Standard_Boolean aUSense);

  Standard_EXPORT Standard_Boolean USense() const;

  Standard_EXPORT void SetVSense(const Standard_Boolean aVSense);

  Standard_EXPORT Standard_Boolean VSense() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_SurfacePatch, Standard_Transient)

protected:
private:
  Handle(StepGeom_BoundedSurface) parentSurface;
  StepGeom_TransitionCode         uTransition;
  StepGeom_TransitionCode         vTransition;
  Standard_Boolean                uSense;
  Standard_Boolean                vSense;
};

#endif // _StepGeom_SurfacePatch_HeaderFile
