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

#ifndef _StepGeom_SurfaceOfLinearExtrusion_HeaderFile
#define _StepGeom_SurfaceOfLinearExtrusion_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_SweptSurface.hxx>
class StepGeom_Vector;
class TCollection_HAsciiString;
class StepGeom_Curve;

class StepGeom_SurfaceOfLinearExtrusion;
DEFINE_STANDARD_HANDLE(StepGeom_SurfaceOfLinearExtrusion, StepGeom_SweptSurface)

class StepGeom_SurfaceOfLinearExtrusion : public StepGeom_SweptSurface
{

public:
  //! Returns a SurfaceOfLinearExtrusion
  Standard_EXPORT StepGeom_SurfaceOfLinearExtrusion();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepGeom_Curve)&           aSweptCurve,
                            const Handle(StepGeom_Vector)&          aExtrusionAxis);

  Standard_EXPORT void SetExtrusionAxis(const Handle(StepGeom_Vector)& aExtrusionAxis);

  Standard_EXPORT Handle(StepGeom_Vector) ExtrusionAxis() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_SurfaceOfLinearExtrusion, StepGeom_SweptSurface)

protected:
private:
  Handle(StepGeom_Vector) extrusionAxis;
};

#endif // _StepGeom_SurfaceOfLinearExtrusion_HeaderFile
