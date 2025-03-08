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

#ifndef _StepGeom_PointOnCurve_HeaderFile
#define _StepGeom_PointOnCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_Point.hxx>
class StepGeom_Curve;
class TCollection_HAsciiString;

class StepGeom_PointOnCurve;
DEFINE_STANDARD_HANDLE(StepGeom_PointOnCurve, StepGeom_Point)

class StepGeom_PointOnCurve : public StepGeom_Point
{

public:
  //! Returns a PointOnCurve
  Standard_EXPORT StepGeom_PointOnCurve();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepGeom_Curve)&           aBasisCurve,
                            const Standard_Real                     aPointParameter);

  Standard_EXPORT void SetBasisCurve(const Handle(StepGeom_Curve)& aBasisCurve);

  Standard_EXPORT Handle(StepGeom_Curve) BasisCurve() const;

  Standard_EXPORT void SetPointParameter(const Standard_Real aPointParameter);

  Standard_EXPORT Standard_Real PointParameter() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_PointOnCurve, StepGeom_Point)

protected:
private:
  Handle(StepGeom_Curve) basisCurve;
  Standard_Real          pointParameter;
};

#endif // _StepGeom_PointOnCurve_HeaderFile
