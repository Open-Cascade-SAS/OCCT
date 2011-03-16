// File:	StepDimTol_Datum.cxx
// Created:	Wed Jun  4 11:17:04 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepDimTol_Datum.ixx>

//=======================================================================
//function : StepDimTol_Datum
//purpose  : 
//=======================================================================

StepDimTol_Datum::StepDimTol_Datum ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_Datum::Init (const Handle(TCollection_HAsciiString)& aShapeAspect_Name,
                             const Handle(TCollection_HAsciiString)& aShapeAspect_Description,
                             const Handle(StepRepr_ProductDefinitionShape)& aShapeAspect_OfShape,
                             const StepData_Logical aShapeAspect_ProductDefinitional,
                             const Handle(TCollection_HAsciiString)& aIdentification)
{
  StepRepr_ShapeAspect::Init(aShapeAspect_Name,
                             aShapeAspect_Description,
                             aShapeAspect_OfShape,
                             aShapeAspect_ProductDefinitional);
  theIdentification = aIdentification;
}

//=======================================================================
//function : Identification
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepDimTol_Datum::Identification () const
{
  return theIdentification;
}

//=======================================================================
//function : SetIdentification
//purpose  : 
//=======================================================================

void StepDimTol_Datum::SetIdentification (const Handle(TCollection_HAsciiString) &aIdentification)
{
  theIdentification = aIdentification;
}
