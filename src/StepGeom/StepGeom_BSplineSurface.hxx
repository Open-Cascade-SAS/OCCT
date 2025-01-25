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

#ifndef _StepGeom_BSplineSurface_HeaderFile
#define _StepGeom_BSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <StepGeom_HArray2OfCartesianPoint.hxx>
#include <StepGeom_BSplineSurfaceForm.hxx>
#include <StepData_Logical.hxx>
#include <StepGeom_BoundedSurface.hxx>
class TCollection_HAsciiString;
class StepGeom_CartesianPoint;

class StepGeom_BSplineSurface;
DEFINE_STANDARD_HANDLE(StepGeom_BSplineSurface, StepGeom_BoundedSurface)

class StepGeom_BSplineSurface : public StepGeom_BoundedSurface
{

public:
  //! Returns a BSplineSurface
  Standard_EXPORT StepGeom_BSplineSurface();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&         aName,
                            const Standard_Integer                          aUDegree,
                            const Standard_Integer                          aVDegree,
                            const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
                            const StepGeom_BSplineSurfaceForm               aSurfaceForm,
                            const StepData_Logical                          aUClosed,
                            const StepData_Logical                          aVClosed,
                            const StepData_Logical                          aSelfIntersect);

  Standard_EXPORT void SetUDegree(const Standard_Integer aUDegree);

  Standard_EXPORT Standard_Integer UDegree() const;

  Standard_EXPORT void SetVDegree(const Standard_Integer aVDegree);

  Standard_EXPORT Standard_Integer VDegree() const;

  Standard_EXPORT void SetControlPointsList(
    const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList);

  Standard_EXPORT Handle(StepGeom_HArray2OfCartesianPoint) ControlPointsList() const;

  Standard_EXPORT Handle(StepGeom_CartesianPoint) ControlPointsListValue(
    const Standard_Integer num1,
    const Standard_Integer num2) const;

  Standard_EXPORT Standard_Integer NbControlPointsListI() const;

  Standard_EXPORT Standard_Integer NbControlPointsListJ() const;

  Standard_EXPORT void SetSurfaceForm(const StepGeom_BSplineSurfaceForm aSurfaceForm);

  Standard_EXPORT StepGeom_BSplineSurfaceForm SurfaceForm() const;

  Standard_EXPORT void SetUClosed(const StepData_Logical aUClosed);

  Standard_EXPORT StepData_Logical UClosed() const;

  Standard_EXPORT void SetVClosed(const StepData_Logical aVClosed);

  Standard_EXPORT StepData_Logical VClosed() const;

  Standard_EXPORT void SetSelfIntersect(const StepData_Logical aSelfIntersect);

  Standard_EXPORT StepData_Logical SelfIntersect() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_BSplineSurface, StepGeom_BoundedSurface)

protected:
private:
  Standard_Integer                         uDegree;
  Standard_Integer                         vDegree;
  Handle(StepGeom_HArray2OfCartesianPoint) controlPointsList;
  StepGeom_BSplineSurfaceForm              surfaceForm;
  StepData_Logical                         uClosed;
  StepData_Logical                         vClosed;
  StepData_Logical                         selfIntersect;
};

#endif // _StepGeom_BSplineSurface_HeaderFile
