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

#ifndef _StepGeom_CompositeCurveSegment_HeaderFile
#define _StepGeom_CompositeCurveSegment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_TransitionCode.hxx>
#include <Standard_Transient.hxx>
class StepGeom_Curve;

class StepGeom_CompositeCurveSegment;
DEFINE_STANDARD_HANDLE(StepGeom_CompositeCurveSegment, Standard_Transient)

class StepGeom_CompositeCurveSegment : public Standard_Transient
{

public:
  //! Returns a CompositeCurveSegment
  Standard_EXPORT StepGeom_CompositeCurveSegment();

  Standard_EXPORT void Init(const StepGeom_TransitionCode aTransition,
                            const Standard_Boolean        aSameSense,
                            const Handle(StepGeom_Curve)& aParentCurve);

  Standard_EXPORT void SetTransition(const StepGeom_TransitionCode aTransition);

  Standard_EXPORT StepGeom_TransitionCode Transition() const;

  Standard_EXPORT void SetSameSense(const Standard_Boolean aSameSense);

  Standard_EXPORT Standard_Boolean SameSense() const;

  Standard_EXPORT void SetParentCurve(const Handle(StepGeom_Curve)& aParentCurve);

  Standard_EXPORT Handle(StepGeom_Curve) ParentCurve() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_CompositeCurveSegment, Standard_Transient)

protected:
private:
  StepGeom_TransitionCode transition;
  Standard_Boolean        sameSense;
  Handle(StepGeom_Curve)  parentCurve;
};

#endif // _StepGeom_CompositeCurveSegment_HeaderFile
