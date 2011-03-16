// File:	StepDimTol_DatumTarget.cxx
// Created:	Wed Jun  4 11:17:02 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepDimTol_DatumTarget.ixx>

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
