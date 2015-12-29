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

#ifndef _StepGeom_QuasiUniformCurveAndRationalBSplineCurve_HeaderFile
#define _StepGeom_QuasiUniformCurveAndRationalBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_BSplineCurve.hxx>
#include <Standard_Integer.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepGeom_BSplineCurveForm.hxx>
#include <StepData_Logical.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Real.hxx>
class StepGeom_QuasiUniformCurve;
class StepGeom_RationalBSplineCurve;
class TCollection_HAsciiString;


class StepGeom_QuasiUniformCurveAndRationalBSplineCurve;
DEFINE_STANDARD_HANDLE(StepGeom_QuasiUniformCurveAndRationalBSplineCurve, StepGeom_BSplineCurve)


class StepGeom_QuasiUniformCurveAndRationalBSplineCurve : public StepGeom_BSplineCurve
{

public:

  
  //! Returns a QuasiUniformCurveAndRationalBSplineCurve
  Standard_EXPORT StepGeom_QuasiUniformCurveAndRationalBSplineCurve();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Standard_Integer aDegree, const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList, const StepGeom_BSplineCurveForm aCurveForm, const StepData_Logical aClosedCurve, const StepData_Logical aSelfIntersect, const Handle(StepGeom_QuasiUniformCurve)& aQuasiUniformCurve, const Handle(StepGeom_RationalBSplineCurve)& aRationalBSplineCurve);
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Standard_Integer aDegree, const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList, const StepGeom_BSplineCurveForm aCurveForm, const StepData_Logical aClosedCurve, const StepData_Logical aSelfIntersect, const Handle(TColStd_HArray1OfReal)& aWeightsData);
  
  Standard_EXPORT void SetQuasiUniformCurve (const Handle(StepGeom_QuasiUniformCurve)& aQuasiUniformCurve);
  
  Standard_EXPORT Handle(StepGeom_QuasiUniformCurve) QuasiUniformCurve() const;
  
  Standard_EXPORT void SetRationalBSplineCurve (const Handle(StepGeom_RationalBSplineCurve)& aRationalBSplineCurve);
  
  Standard_EXPORT Handle(StepGeom_RationalBSplineCurve) RationalBSplineCurve() const;
  
  Standard_EXPORT void SetWeightsData (const Handle(TColStd_HArray1OfReal)& aWeightsData);
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) WeightsData() const;
  
  Standard_EXPORT Standard_Real WeightsDataValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbWeightsData() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_QuasiUniformCurveAndRationalBSplineCurve,StepGeom_BSplineCurve)

protected:




private:


  Handle(StepGeom_QuasiUniformCurve) quasiUniformCurve;
  Handle(StepGeom_RationalBSplineCurve) rationalBSplineCurve;


};







#endif // _StepGeom_QuasiUniformCurveAndRationalBSplineCurve_HeaderFile
