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

#ifndef _StepGeom_RationalBSplineSurface_HeaderFile
#define _StepGeom_RationalBSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray2OfReal.hxx>
#include <StepGeom_BSplineSurface.hxx>
#include <Standard_Integer.hxx>
#include <StepGeom_HArray2OfCartesianPoint.hxx>
#include <StepGeom_BSplineSurfaceForm.hxx>
#include <StepData_Logical.hxx>
#include <Standard_Real.hxx>
class TCollection_HAsciiString;

class StepGeom_RationalBSplineSurface;
DEFINE_STANDARD_HANDLE(StepGeom_RationalBSplineSurface, StepGeom_BSplineSurface)

class StepGeom_RationalBSplineSurface : public StepGeom_BSplineSurface
{

public:
  //! Returns a RationalBSplineSurface
  Standard_EXPORT StepGeom_RationalBSplineSurface();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&         aName,
                            const Standard_Integer                          aUDegree,
                            const Standard_Integer                          aVDegree,
                            const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList,
                            const StepGeom_BSplineSurfaceForm               aSurfaceForm,
                            const StepData_Logical                          aUClosed,
                            const StepData_Logical                          aVClosed,
                            const StepData_Logical                          aSelfIntersect,
                            const Handle(TColStd_HArray2OfReal)&            aWeightsData);

  Standard_EXPORT void SetWeightsData(const Handle(TColStd_HArray2OfReal)& aWeightsData);

  Standard_EXPORT Handle(TColStd_HArray2OfReal) WeightsData() const;

  Standard_EXPORT Standard_Real WeightsDataValue(const Standard_Integer num1,
                                                 const Standard_Integer num2) const;

  Standard_EXPORT Standard_Integer NbWeightsDataI() const;

  Standard_EXPORT Standard_Integer NbWeightsDataJ() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_RationalBSplineSurface, StepGeom_BSplineSurface)

protected:
private:
  Handle(TColStd_HArray2OfReal) weightsData;
};

#endif // _StepGeom_RationalBSplineSurface_HeaderFile
