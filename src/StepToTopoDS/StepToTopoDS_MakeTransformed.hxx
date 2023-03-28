// Created on: 1997-04-28
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepToTopoDS_MakeTransformed_HeaderFile
#define _StepToTopoDS_MakeTransformed_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Trsf.hxx>
#include <StepToTopoDS_Root.hxx>
#include <Message_ProgressRange.hxx>

class StepData_Factors;
class StepGeom_Axis2Placement3d;
class StepGeom_CartesianTransformationOperator3d;
class TopoDS_Shape;
class StepRepr_MappedItem;
class Transfer_TransientProcess;

//! Produces instances by Transformation of a basic item
class StepToTopoDS_MakeTransformed  : public StepToTopoDS_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT StepToTopoDS_MakeTransformed();
  
  //! Computes a transformation to pass from an Origin placement to
  //! a Target placement. Returns True when done
  //! If not done, the transformation will by Identity
  Standard_EXPORT Standard_Boolean Compute (const Handle(StepGeom_Axis2Placement3d)& Origin,
                                            const Handle(StepGeom_Axis2Placement3d)& Target,
                                            const StepData_Factors& theLocalFactors);
  
  //! Computes a transformation defined by an operator 3D
  Standard_EXPORT Standard_Boolean Compute (const Handle(StepGeom_CartesianTransformationOperator3d)& Operator,
                                            const StepData_Factors& theLocalFactors);
  
  //! Returns the computed transformation (Identity if not yet or
  //! if failed)
  Standard_EXPORT const gp_Trsf& Transformation() const;
  
  //! Applies the computed transformation to a shape
  //! Returns False if the transformation is Identity
  Standard_EXPORT Standard_Boolean Transform (TopoDS_Shape& shape) const;
  
  //! Translates a MappedItem. More precisely
  //! A MappedItem has a MappingSource and a MappingTarget
  //! MappingSource has a MappedRepresentation and a MappingOrigin
  //! MappedRepresentation is the basic item to be instanced
  //! MappingOrigin is the starting placement
  //! MappingTarget is the final placement
  //!
  //! Hence, the transformation from MappingOrigin and MappingTarget
  //! is computed, the MappedRepr. is converted to a Shape, then
  //! transformed as an instance of this Shape
  Standard_EXPORT TopoDS_Shape TranslateMappedItem (const Handle(StepRepr_MappedItem)& mapit,
                                                    const Handle(Transfer_TransientProcess)& TP,
                                                    const StepData_Factors& theLocalFactors,
                                                    const Message_ProgressRange& theProgress = Message_ProgressRange());




protected:





private:



  gp_Trsf theTrsf;


};







#endif // _StepToTopoDS_MakeTransformed_HeaderFile
