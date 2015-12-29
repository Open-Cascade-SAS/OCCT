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

#ifndef _StepGeom_BSplineCurve_HeaderFile
#define _StepGeom_BSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepGeom_BSplineCurveForm.hxx>
#include <StepData_Logical.hxx>
#include <StepGeom_BoundedCurve.hxx>
class TCollection_HAsciiString;
class StepGeom_CartesianPoint;


class StepGeom_BSplineCurve;
DEFINE_STANDARD_HANDLE(StepGeom_BSplineCurve, StepGeom_BoundedCurve)


class StepGeom_BSplineCurve : public StepGeom_BoundedCurve
{

public:

  
  //! Returns a BSplineCurve
  Standard_EXPORT StepGeom_BSplineCurve();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Standard_Integer aDegree, const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList, const StepGeom_BSplineCurveForm aCurveForm, const StepData_Logical aClosedCurve, const StepData_Logical aSelfIntersect);
  
  Standard_EXPORT void SetDegree (const Standard_Integer aDegree);
  
  Standard_EXPORT Standard_Integer Degree() const;
  
  Standard_EXPORT void SetControlPointsList (const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList);
  
  Standard_EXPORT Handle(StepGeom_HArray1OfCartesianPoint) ControlPointsList() const;
  
  Standard_EXPORT Handle(StepGeom_CartesianPoint) ControlPointsListValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbControlPointsList() const;
  
  Standard_EXPORT void SetCurveForm (const StepGeom_BSplineCurveForm aCurveForm);
  
  Standard_EXPORT StepGeom_BSplineCurveForm CurveForm() const;
  
  Standard_EXPORT void SetClosedCurve (const StepData_Logical aClosedCurve);
  
  Standard_EXPORT StepData_Logical ClosedCurve() const;
  
  Standard_EXPORT void SetSelfIntersect (const StepData_Logical aSelfIntersect);
  
  Standard_EXPORT StepData_Logical SelfIntersect() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_BSplineCurve,StepGeom_BoundedCurve)

protected:




private:


  Standard_Integer degree;
  Handle(StepGeom_HArray1OfCartesianPoint) controlPointsList;
  StepGeom_BSplineCurveForm curveForm;
  StepData_Logical closedCurve;
  StepData_Logical selfIntersect;


};







#endif // _StepGeom_BSplineCurve_HeaderFile
