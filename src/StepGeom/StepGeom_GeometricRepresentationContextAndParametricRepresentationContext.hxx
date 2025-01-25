// Created on: 1995-12-07
// Created by: FMA
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

#ifndef _StepGeom_GeometricRepresentationContextAndParametricRepresentationContext_HeaderFile
#define _StepGeom_GeometricRepresentationContextAndParametricRepresentationContext_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationContext.hxx>
#include <Standard_Integer.hxx>
class StepGeom_GeometricRepresentationContext;
class StepRepr_ParametricRepresentationContext;
class TCollection_HAsciiString;

class StepGeom_GeometricRepresentationContextAndParametricRepresentationContext;
DEFINE_STANDARD_HANDLE(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext,
                       StepRepr_RepresentationContext)

class StepGeom_GeometricRepresentationContextAndParametricRepresentationContext
    : public StepRepr_RepresentationContext
{

public:
  //! empty constructor
  Standard_EXPORT StepGeom_GeometricRepresentationContextAndParametricRepresentationContext();

  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&                 aContextIdentifier,
    const Handle(TCollection_HAsciiString)&                 aContextType,
    const Handle(StepGeom_GeometricRepresentationContext)&  aGeometricRepresentationContext,
    const Handle(StepRepr_ParametricRepresentationContext)& aParametricRepresentationContext);

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aContextIdentifier,
                            const Handle(TCollection_HAsciiString)& aContextType,
                            const Standard_Integer                  aCoordinateSpaceDimension);

  Standard_EXPORT void SetGeometricRepresentationContext(
    const Handle(StepGeom_GeometricRepresentationContext)& aGeometricRepresentationContext);

  Standard_EXPORT Handle(StepGeom_GeometricRepresentationContext) GeometricRepresentationContext()
    const;

  Standard_EXPORT void SetParametricRepresentationContext(
    const Handle(StepRepr_ParametricRepresentationContext)& aParametricRepresentationContext);

  Standard_EXPORT Handle(StepRepr_ParametricRepresentationContext) ParametricRepresentationContext()
    const;

  Standard_EXPORT void SetCoordinateSpaceDimension(
    const Standard_Integer aCoordinateSpaceDimension);

  Standard_EXPORT Standard_Integer CoordinateSpaceDimension() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext,
                          StepRepr_RepresentationContext)

protected:
private:
  Handle(StepGeom_GeometricRepresentationContext)  geometricRepresentationContext;
  Handle(StepRepr_ParametricRepresentationContext) parametricRepresentationContext;
};

#endif // _StepGeom_GeometricRepresentationContextAndParametricRepresentationContext_HeaderFile
