// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_CurveElementEndCoordinateSystem_HeaderFile
#define _StepFEA_CurveElementEndCoordinateSystem_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepFEA_FeaAxis2Placement3d;
class StepFEA_AlignedCurve3dElementCoordinateSystem;
class StepFEA_ParametricCurve3dElementCoordinateSystem;

//! Representation of STEP SELECT type CurveElementEndCoordinateSystem
class StepFEA_CurveElementEndCoordinateSystem : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepFEA_CurveElementEndCoordinateSystem();

  //! Recognizes a kind of CurveElementEndCoordinateSystem select type
  //! 1 -> FeaAxis2Placement3d from StepFEA
  //! 2 -> AlignedCurve3dElementCoordinateSystem from StepFEA
  //! 3 -> ParametricCurve3dElementCoordinateSystem from StepFEA
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! Returns Value as FeaAxis2Placement3d (or Null if another type)
  Standard_EXPORT Handle(StepFEA_FeaAxis2Placement3d) FeaAxis2Placement3d() const;

  //! Returns Value as AlignedCurve3dElementCoordinateSystem (or Null if another type)
  Standard_EXPORT Handle(StepFEA_AlignedCurve3dElementCoordinateSystem)
    AlignedCurve3dElementCoordinateSystem() const;

  //! Returns Value as ParametricCurve3dElementCoordinateSystem (or Null if another type)
  Standard_EXPORT Handle(StepFEA_ParametricCurve3dElementCoordinateSystem)
    ParametricCurve3dElementCoordinateSystem() const;

protected:
private:
};

#endif // _StepFEA_CurveElementEndCoordinateSystem_HeaderFile
