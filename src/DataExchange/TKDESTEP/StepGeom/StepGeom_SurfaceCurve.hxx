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

#ifndef _StepGeom_SurfaceCurve_HeaderFile
#define _StepGeom_SurfaceCurve_HeaderFile

#include <Standard.hxx>

#include <StepGeom_HArray1OfPcurveOrSurface.hxx>
#include <StepGeom_PreferredSurfaceCurveRepresentation.hxx>
#include <StepGeom_Curve.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepGeom_PcurveOrSurface;

class StepGeom_SurfaceCurve;
DEFINE_STANDARD_HANDLE(StepGeom_SurfaceCurve, StepGeom_Curve)

class StepGeom_SurfaceCurve : public StepGeom_Curve
{

public:
  //! Returns a SurfaceCurve
  Standard_EXPORT StepGeom_SurfaceCurve();

  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&            aName,
    const Handle(StepGeom_Curve)&                      aCurve3d,
    const Handle(StepGeom_HArray1OfPcurveOrSurface)&   aAssociatedGeometry,
    const StepGeom_PreferredSurfaceCurveRepresentation aMasterRepresentation);

  Standard_EXPORT void SetCurve3d(const Handle(StepGeom_Curve)& aCurve3d);

  Standard_EXPORT Handle(StepGeom_Curve) Curve3d() const;

  Standard_EXPORT void SetAssociatedGeometry(
    const Handle(StepGeom_HArray1OfPcurveOrSurface)& aAssociatedGeometry);

  Standard_EXPORT Handle(StepGeom_HArray1OfPcurveOrSurface) AssociatedGeometry() const;

  Standard_EXPORT StepGeom_PcurveOrSurface
    AssociatedGeometryValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbAssociatedGeometry() const;

  Standard_EXPORT void SetMasterRepresentation(
    const StepGeom_PreferredSurfaceCurveRepresentation aMasterRepresentation);

  Standard_EXPORT StepGeom_PreferredSurfaceCurveRepresentation MasterRepresentation() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_SurfaceCurve, StepGeom_Curve)

protected:
private:
  Handle(StepGeom_Curve)                       curve3d;
  Handle(StepGeom_HArray1OfPcurveOrSurface)    associatedGeometry;
  StepGeom_PreferredSurfaceCurveRepresentation masterRepresentation;
};

#endif // _StepGeom_SurfaceCurve_HeaderFile
