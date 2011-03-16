// File:	StepDimTol_CommonDatum.cxx
// Created:	Wed Jun  4 11:17:02 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepDimTol_CommonDatum.ixx>

//=======================================================================
//function : StepDimTol_CommonDatum
//purpose  : 
//=======================================================================

StepDimTol_CommonDatum::StepDimTol_CommonDatum ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_CommonDatum::Init (const Handle(TCollection_HAsciiString) &aShapeAspect_Name,
                                   const Handle(TCollection_HAsciiString) &aShapeAspect_Description,
                                   const Handle(StepRepr_ProductDefinitionShape) &aShapeAspect_OfShape,
                                   const StepData_Logical aShapeAspect_ProductDefinitional,
                                   const Handle(TCollection_HAsciiString) &aDatum_Name,
                                   const Handle(TCollection_HAsciiString) &aDatum_Description,
                                   const Handle(StepRepr_ProductDefinitionShape) &aDatum_OfShape,
                                   const StepData_Logical aDatum_ProductDefinitional,
                                   const Handle(TCollection_HAsciiString) &aDatum_Identification)
{
    StepRepr_CompositeShapeAspect::Init(aShapeAspect_Name,
                                        aShapeAspect_Description,
                                        aShapeAspect_OfShape,
                                        aShapeAspect_ProductDefinitional);
    theDatum->Init(aDatum_Name,
                   aDatum_Description,
                   aDatum_OfShape,
                   aDatum_ProductDefinitional,
                   aDatum_Identification);
}

//=======================================================================
//function : Datum
//purpose  : 
//=======================================================================

Handle(StepDimTol_Datum) StepDimTol_CommonDatum::Datum () const
{
  return theDatum;
}

//=======================================================================
//function : SetDatum
//purpose  : 
//=======================================================================

void StepDimTol_CommonDatum::SetDatum (const Handle(StepDimTol_Datum) &aDatum)
{
  theDatum = aDatum;
}
