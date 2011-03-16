// File:	RWStepDimTol_RWPlacedDatumTargetFeature.cxx
// Created:	Wed Jun  4 13:34:33 2003 
// Author:	Galina KULIKOVA
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepDimTol_RWPlacedDatumTargetFeature.ixx>

//=======================================================================
//function : RWStepDimTol_RWPlacedDatumTargetFeature
//purpose  : 
//=======================================================================

RWStepDimTol_RWPlacedDatumTargetFeature::RWStepDimTol_RWPlacedDatumTargetFeature ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWPlacedDatumTargetFeature::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                        const Standard_Integer num,
                                                        Handle(Interface_Check)& ach,
                                                        const Handle(StepDimTol_PlacedDatumTargetFeature) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"placed_datum_target_feature") ) return;

  // Inherited fields of ShapeAspect

  Handle(TCollection_HAsciiString) aShapeAspect_Name;
  data->ReadString (num, 1, "shape_aspect.name", ach, aShapeAspect_Name);

  Handle(TCollection_HAsciiString) aShapeAspect_Description;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "shape_aspect.description", ach, aShapeAspect_Description);
  }

  Handle(StepRepr_ProductDefinitionShape) aShapeAspect_OfShape;
  data->ReadEntity (num, 3, "shape_aspect.of_shape", ach, STANDARD_TYPE(StepRepr_ProductDefinitionShape), aShapeAspect_OfShape);

  StepData_Logical aShapeAspect_ProductDefinitional;
  data->ReadLogical (num, 4, "shape_aspect.product_definitional", ach, aShapeAspect_ProductDefinitional);

  // Inherited fields of DatumTarget

  Handle(TCollection_HAsciiString) aDatumTarget_TargetId;
  data->ReadString (num, 5, "datum_target.target_id", ach, aDatumTarget_TargetId);

  // Initialize entity
  ent->Init(aShapeAspect_Name,
            aShapeAspect_Description,
            aShapeAspect_OfShape,
            aShapeAspect_ProductDefinitional,
            aDatumTarget_TargetId);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWPlacedDatumTargetFeature::WriteStep (StepData_StepWriter& SW,
                                                         const Handle(StepDimTol_PlacedDatumTargetFeature) &ent) const
{

  // Inherited fields of ShapeAspect

  SW.Send (ent->StepRepr_ShapeAspect::Name());

  SW.Send (ent->StepRepr_ShapeAspect::Description());

  SW.Send (ent->StepRepr_ShapeAspect::OfShape());

  SW.SendLogical (ent->StepRepr_ShapeAspect::ProductDefinitional());

  // Inherited fields of DatumTarget

  SW.Send (ent->StepDimTol_DatumTarget::TargetId());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWPlacedDatumTargetFeature::Share (const Handle(StepDimTol_PlacedDatumTargetFeature) &ent,
                                                     Interface_EntityIterator& iter) const
{

  // Inherited fields of ShapeAspect

  iter.AddItem (ent->StepRepr_ShapeAspect::OfShape());

  // Inherited fields of DatumTarget
}
