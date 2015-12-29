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

#ifndef _StepGeom_BSplineCurveWithKnots_HeaderFile
#define _StepGeom_BSplineCurveWithKnots_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <StepGeom_KnotType.hxx>
#include <StepGeom_BSplineCurve.hxx>
#include <Standard_Integer.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepGeom_BSplineCurveForm.hxx>
#include <StepData_Logical.hxx>
#include <Standard_Real.hxx>
class TCollection_HAsciiString;


class StepGeom_BSplineCurveWithKnots;
DEFINE_STANDARD_HANDLE(StepGeom_BSplineCurveWithKnots, StepGeom_BSplineCurve)


class StepGeom_BSplineCurveWithKnots : public StepGeom_BSplineCurve
{

public:

  
  //! Returns a BSplineCurveWithKnots
  Standard_EXPORT StepGeom_BSplineCurveWithKnots();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Standard_Integer aDegree, const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList, const StepGeom_BSplineCurveForm aCurveForm, const StepData_Logical aClosedCurve, const StepData_Logical aSelfIntersect, const Handle(TColStd_HArray1OfInteger)& aKnotMultiplicities, const Handle(TColStd_HArray1OfReal)& aKnots, const StepGeom_KnotType aKnotSpec);
  
  Standard_EXPORT void SetKnotMultiplicities (const Handle(TColStd_HArray1OfInteger)& aKnotMultiplicities);
  
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) KnotMultiplicities() const;
  
  Standard_EXPORT Standard_Integer KnotMultiplicitiesValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbKnotMultiplicities() const;
  
  Standard_EXPORT void SetKnots (const Handle(TColStd_HArray1OfReal)& aKnots);
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) Knots() const;
  
  Standard_EXPORT Standard_Real KnotsValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbKnots() const;
  
  Standard_EXPORT void SetKnotSpec (const StepGeom_KnotType aKnotSpec);
  
  Standard_EXPORT StepGeom_KnotType KnotSpec() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_BSplineCurveWithKnots,StepGeom_BSplineCurve)

protected:




private:


  Handle(TColStd_HArray1OfInteger) knotMultiplicities;
  Handle(TColStd_HArray1OfReal) knots;
  StepGeom_KnotType knotSpec;


};







#endif // _StepGeom_BSplineCurveWithKnots_HeaderFile
