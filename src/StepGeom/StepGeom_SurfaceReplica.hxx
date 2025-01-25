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

#ifndef _StepGeom_SurfaceReplica_HeaderFile
#define _StepGeom_SurfaceReplica_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_Surface.hxx>
class StepGeom_CartesianTransformationOperator3d;
class TCollection_HAsciiString;

class StepGeom_SurfaceReplica;
DEFINE_STANDARD_HANDLE(StepGeom_SurfaceReplica, StepGeom_Surface)

class StepGeom_SurfaceReplica : public StepGeom_Surface
{

public:
  //! Returns a SurfaceReplica
  Standard_EXPORT StepGeom_SurfaceReplica();

  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&                   aName,
    const Handle(StepGeom_Surface)&                           aParentSurface,
    const Handle(StepGeom_CartesianTransformationOperator3d)& aTransformation);

  Standard_EXPORT void SetParentSurface(const Handle(StepGeom_Surface)& aParentSurface);

  Standard_EXPORT Handle(StepGeom_Surface) ParentSurface() const;

  Standard_EXPORT void SetTransformation(
    const Handle(StepGeom_CartesianTransformationOperator3d)& aTransformation);

  Standard_EXPORT Handle(StepGeom_CartesianTransformationOperator3d) Transformation() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_SurfaceReplica, StepGeom_Surface)

protected:
private:
  Handle(StepGeom_Surface)                           parentSurface;
  Handle(StepGeom_CartesianTransformationOperator3d) transformation;
};

#endif // _StepGeom_SurfaceReplica_HeaderFile
