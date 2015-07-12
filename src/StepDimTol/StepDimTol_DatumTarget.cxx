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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Standard_Type.hxx>
#include <StepDimTol_DatumTarget.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : StepDimTol_DatumTarget
//purpose  : 
//=======================================================================
StepDimTol_DatumTarget::StepDimTol_DatumTarget ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_DatumTarget::Init (const Handle(TCollection_HAsciiString) &aShapeAspect_Name,
                                   const Handle(TCollection_HAsciiString) &aShapeAspect_Description,
                                   const Handle(StepRepr_ProductDefinitionShape) &aShapeAspect_OfShape,
                                   const StepData_Logical aShapeAspect_ProductDefinitional,
                                   const Handle(TCollection_HAsciiString) &aTargetId)
{
  StepRepr_ShapeAspect::Init(aShapeAspect_Name,
                             aShapeAspect_Description,
                             aShapeAspect_OfShape,
                             aShapeAspect_ProductDefinitional);
  theTargetId = aTargetId;
}

//=======================================================================
//function : TargetId
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepDimTol_DatumTarget::TargetId () const
{
  return theTargetId;
}

//=======================================================================
//function : SetTargetId
//purpose  : 
//=======================================================================

void StepDimTol_DatumTarget::SetTargetId (const Handle(TCollection_HAsciiString) &aTargetId)
{
  theTargetId = aTargetId;
}
