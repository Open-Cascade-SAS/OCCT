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

#ifndef _StepGeom_Polyline_HeaderFile
#define _StepGeom_Polyline_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepGeom_CartesianPoint;

class StepGeom_Polyline;
DEFINE_STANDARD_HANDLE(StepGeom_Polyline, StepGeom_BoundedCurve)

class StepGeom_Polyline : public StepGeom_BoundedCurve
{

public:
  //! Returns a Polyline
  Standard_EXPORT StepGeom_Polyline();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&         aName,
                            const Handle(StepGeom_HArray1OfCartesianPoint)& aPoints);

  Standard_EXPORT void SetPoints(const Handle(StepGeom_HArray1OfCartesianPoint)& aPoints);

  Standard_EXPORT Handle(StepGeom_HArray1OfCartesianPoint) Points() const;

  Standard_EXPORT Handle(StepGeom_CartesianPoint) PointsValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbPoints() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_Polyline, StepGeom_BoundedCurve)

protected:
private:
  Handle(StepGeom_HArray1OfCartesianPoint) points;
};

#endif // _StepGeom_Polyline_HeaderFile
