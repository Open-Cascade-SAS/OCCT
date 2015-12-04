// Created on: 2003-06-04
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepDimTol_DatumTarget_HeaderFile
#define _StepDimTol_DatumTarget_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_ShapeAspect.hxx>
#include <StepData_Logical.hxx>
class TCollection_HAsciiString;
class StepRepr_ProductDefinitionShape;


class StepDimTol_DatumTarget;
DEFINE_STANDARD_HANDLE(StepDimTol_DatumTarget, StepRepr_ShapeAspect)

//! Representation of STEP entity DatumTarget
class StepDimTol_DatumTarget : public StepRepr_ShapeAspect
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepDimTol_DatumTarget();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theShapeAspect_Name, 
                            const Handle(TCollection_HAsciiString)& theShapeAspect_Description, 
                            const Handle(StepRepr_ProductDefinitionShape)& theShapeAspect_OfShape, 
                            const StepData_Logical                  theShapeAspect_ProductDefinitional, 
                            const Handle(TCollection_HAsciiString)& theTargetId);
  
  //! Returns field TargetId
  Standard_EXPORT Handle(TCollection_HAsciiString) TargetId() const;
  
  //! Set field TargetId
  Standard_EXPORT void SetTargetId (const Handle(TCollection_HAsciiString)& theTargetId);




  DEFINE_STANDARD_RTTIEXT(StepDimTol_DatumTarget,StepRepr_ShapeAspect)

protected:




private:


  Handle(TCollection_HAsciiString) myTargetId;


};







#endif // _StepDimTol_DatumTarget_HeaderFile
