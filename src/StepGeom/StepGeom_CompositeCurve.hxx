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

#ifndef _StepGeom_CompositeCurve_HeaderFile
#define _StepGeom_CompositeCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_HArray1OfCompositeCurveSegment.hxx>
#include <StepData_Logical.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepGeom_CompositeCurveSegment;


class StepGeom_CompositeCurve;
DEFINE_STANDARD_HANDLE(StepGeom_CompositeCurve, StepGeom_BoundedCurve)


class StepGeom_CompositeCurve : public StepGeom_BoundedCurve
{

public:

  
  //! Returns a CompositeCurve
  Standard_EXPORT StepGeom_CompositeCurve();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepGeom_HArray1OfCompositeCurveSegment)& aSegments, const StepData_Logical aSelfIntersect);
  
  Standard_EXPORT void SetSegments (const Handle(StepGeom_HArray1OfCompositeCurveSegment)& aSegments);
  
  Standard_EXPORT Handle(StepGeom_HArray1OfCompositeCurveSegment) Segments() const;
  
  Standard_EXPORT Handle(StepGeom_CompositeCurveSegment) SegmentsValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbSegments() const;
  
  Standard_EXPORT void SetSelfIntersect (const StepData_Logical aSelfIntersect);
  
  Standard_EXPORT StepData_Logical SelfIntersect() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_CompositeCurve,StepGeom_BoundedCurve)

protected:




private:


  Handle(StepGeom_HArray1OfCompositeCurveSegment) segments;
  StepData_Logical selfIntersect;


};







#endif // _StepGeom_CompositeCurve_HeaderFile
