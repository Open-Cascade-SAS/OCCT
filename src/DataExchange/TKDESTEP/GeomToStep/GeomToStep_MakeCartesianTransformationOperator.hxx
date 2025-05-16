// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _GeomToStep_MakeCartesianTransformationOperator_HeaderFile
#define _GeomToStep_MakeCartesianTransformationOperator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>

class gp_Trsf;

//! This class creates a cartesian_transformation_operator from gp_Trsf.
//! This entity is used in OCCT to implement a transformation with scaling.
//! In case of other inputs without scaling use Axis2Placement3d.
class GeomToStep_MakeCartesianTransformationOperator : public GeomToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Main constructor.
  //! @param[in] theTrsf Transformation to create the operator from it.
  //! @param[in] theLocalFactors Unit scale factors
  Standard_EXPORT GeomToStep_MakeCartesianTransformationOperator(
    const gp_Trsf&          theTrsf,
    const StepData_Factors& theLocalFactors = StepData_Factors());

  //! Returns the created entity.
  //! @return The created value.
  inline const Handle(StepGeom_CartesianTransformationOperator3d)& Value() const
  {
    return myTrsfOp;
  }

private:
  Handle(StepGeom_CartesianTransformationOperator3d) myTrsfOp;
};

#endif // _GeomToStep_MakeCartesianTransformationOperator_HeaderFile
