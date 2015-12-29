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

#ifndef _StepGeom_ReparametrisedCompositeCurveSegment_HeaderFile
#define _StepGeom_ReparametrisedCompositeCurveSegment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>
#include <StepGeom_TransitionCode.hxx>
#include <Standard_Boolean.hxx>
class StepGeom_Curve;


class StepGeom_ReparametrisedCompositeCurveSegment;
DEFINE_STANDARD_HANDLE(StepGeom_ReparametrisedCompositeCurveSegment, StepGeom_CompositeCurveSegment)


class StepGeom_ReparametrisedCompositeCurveSegment : public StepGeom_CompositeCurveSegment
{

public:

  
  //! Returns a ReparametrisedCompositeCurveSegment
  Standard_EXPORT StepGeom_ReparametrisedCompositeCurveSegment();
  
  Standard_EXPORT void Init (const StepGeom_TransitionCode aTransition, const Standard_Boolean aSameSense, const Handle(StepGeom_Curve)& aParentCurve, const Standard_Real aParamLength);
  
  Standard_EXPORT void SetParamLength (const Standard_Real aParamLength);
  
  Standard_EXPORT Standard_Real ParamLength() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_ReparametrisedCompositeCurveSegment,StepGeom_CompositeCurveSegment)

protected:




private:


  Standard_Real paramLength;


};







#endif // _StepGeom_ReparametrisedCompositeCurveSegment_HeaderFile
