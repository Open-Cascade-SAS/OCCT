// Created on: 1995-12-07
// Created by: Frederic MAUPAS
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

#ifndef _StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx_HeaderFile
#define _StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationContext.hxx>
#include <Standard_Integer.hxx>
#include <StepBasic_HArray1OfNamedUnit.hxx>
#include <StepBasic_HArray1OfUncertaintyMeasureWithUnit.hxx>
class StepGeom_GeometricRepresentationContext;
class StepRepr_GlobalUnitAssignedContext;
class StepRepr_GlobalUncertaintyAssignedContext;
class TCollection_HAsciiString;
class StepBasic_NamedUnit;
class StepBasic_UncertaintyMeasureWithUnit;


class StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx;
DEFINE_STANDARD_HANDLE(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx, StepRepr_RepresentationContext)


class StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx : public StepRepr_RepresentationContext
{

public:

  
  Standard_EXPORT StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aContextIdentifier, const Handle(TCollection_HAsciiString)& aContextType, const Handle(StepGeom_GeometricRepresentationContext)& aGeometricRepresentationCtx, const Handle(StepRepr_GlobalUnitAssignedContext)& aGlobalUnitAssignedCtx, const Handle(StepRepr_GlobalUncertaintyAssignedContext)& aGlobalUncertaintyAssignedCtx);
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aContextIdentifier, const Handle(TCollection_HAsciiString)& aContextType, const Standard_Integer aCoordinateSpaceDimension, const Handle(StepBasic_HArray1OfNamedUnit)& aUnits, const Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit)& anUncertainty);
  
  Standard_EXPORT void SetGeometricRepresentationContext (const Handle(StepGeom_GeometricRepresentationContext)& aGeometricRepresentationContext);
  
  Standard_EXPORT Handle(StepGeom_GeometricRepresentationContext) GeometricRepresentationContext() const;
  
  Standard_EXPORT void SetGlobalUnitAssignedContext (const Handle(StepRepr_GlobalUnitAssignedContext)& aGlobalUnitAssignedContext);
  
  Standard_EXPORT Handle(StepRepr_GlobalUnitAssignedContext) GlobalUnitAssignedContext() const;
  
  Standard_EXPORT void SetGlobalUncertaintyAssignedContext (const Handle(StepRepr_GlobalUncertaintyAssignedContext)& aGlobalUncertaintyAssignedCtx);
  
  Standard_EXPORT Handle(StepRepr_GlobalUncertaintyAssignedContext) GlobalUncertaintyAssignedContext() const;
  
  Standard_EXPORT void SetCoordinateSpaceDimension (const Standard_Integer aCoordinateSpaceDimension);
  
  Standard_EXPORT Standard_Integer CoordinateSpaceDimension() const;
  
  Standard_EXPORT void SetUnits (const Handle(StepBasic_HArray1OfNamedUnit)& aUnits);
  
  Standard_EXPORT Handle(StepBasic_HArray1OfNamedUnit) Units() const;
  
  Standard_EXPORT Handle(StepBasic_NamedUnit) UnitsValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbUnits() const;
  
  Standard_EXPORT void SetUncertainty (const Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit)& aUncertainty);
  
  Standard_EXPORT Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit) Uncertainty() const;
  
  Standard_EXPORT Handle(StepBasic_UncertaintyMeasureWithUnit) UncertaintyValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbUncertainty() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx,StepRepr_RepresentationContext)

protected:




private:


  Handle(StepGeom_GeometricRepresentationContext) geometricRepresentationContext;
  Handle(StepRepr_GlobalUnitAssignedContext) globalUnitAssignedContext;
  Handle(StepRepr_GlobalUncertaintyAssignedContext) globalUncertaintyAssignedContext;


};







#endif // _StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx_HeaderFile
