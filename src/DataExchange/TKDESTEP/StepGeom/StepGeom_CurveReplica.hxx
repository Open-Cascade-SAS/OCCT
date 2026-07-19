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

#ifndef _StepGeom_CurveReplica_HeaderFile
#define _StepGeom_CurveReplica_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_Curve.hxx>
class StepGeom_CartesianTransformationOperator;
class TCollection_HAsciiString;

class StepGeom_CurveReplica : public StepGeom_Curve
{

public:
  //! Returns a CurveReplica
  Standard_EXPORT StepGeom_CurveReplica();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                 aName,
    const occ::handle<StepGeom_Curve>&                           aParentCurve,
    const occ::handle<StepGeom_CartesianTransformationOperator>& aTransformation);

  Standard_EXPORT void SetParentCurve(const occ::handle<StepGeom_Curve>& aParentCurve);

  Standard_EXPORT occ::handle<StepGeom_Curve> ParentCurve() const;

  Standard_EXPORT void SetTransformation(
    const occ::handle<StepGeom_CartesianTransformationOperator>& aTransformation);

  Standard_EXPORT occ::handle<StepGeom_CartesianTransformationOperator> Transformation() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_CurveReplica, StepGeom_Curve)

private:
  occ::handle<StepGeom_Curve>                           parentCurve;
  occ::handle<StepGeom_CartesianTransformationOperator> transformation;
};

#endif // _StepGeom_CurveReplica_HeaderFile
