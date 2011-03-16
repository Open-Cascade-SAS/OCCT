// File:	RWStepShape_RWDimensionalLocationWithPath.cxx
// Created:	Tue Apr 18 16:42:58 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <RWStepShape_RWDimensionalLocationWithPath.ixx>

//=======================================================================
//function : RWStepShape_RWDimensionalLocationWithPath
//purpose  : 
//=======================================================================

RWStepShape_RWDimensionalLocationWithPath::RWStepShape_RWDimensionalLocationWithPath ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalLocationWithPath::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                          const Standard_Integer num,
                                                          Handle(Interface_Check)& ach,
                                                          const Handle(StepShape_DimensionalLocationWithPath) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"dimensional_location_with_path") ) return;

  // Inherited fields of ShapeAspectRelationship

  Handle(TCollection_HAsciiString) aShapeAspectRelationship_Name;
  data->ReadString (num, 1, "shape_aspect_relationship.name", ach, aShapeAspectRelationship_Name);

  Handle(TCollection_HAsciiString) aShapeAspectRelationship_Description;
  Standard_Boolean hasShapeAspectRelationship_Description = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "shape_aspect_relationship.description", ach, aShapeAspectRelationship_Description);
  }
  else {
    hasShapeAspectRelationship_Description = Standard_False;
  }

  Handle(StepRepr_ShapeAspect) aShapeAspectRelationship_RelatingShapeAspect;
  data->ReadEntity (num, 3, "shape_aspect_relationship.relating_shape_aspect", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aShapeAspectRelationship_RelatingShapeAspect);

  Handle(StepRepr_ShapeAspect) aShapeAspectRelationship_RelatedShapeAspect;
  data->ReadEntity (num, 4, "shape_aspect_relationship.related_shape_aspect", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aShapeAspectRelationship_RelatedShapeAspect);

  // Own fields of DimensionalLocationWithPath

  Handle(StepRepr_ShapeAspect) aPath;
  data->ReadEntity (num, 5, "path", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aPath);

  // Initialize entity
  ent->Init(aShapeAspectRelationship_Name,
            hasShapeAspectRelationship_Description,
            aShapeAspectRelationship_Description,
            aShapeAspectRelationship_RelatingShapeAspect,
            aShapeAspectRelationship_RelatedShapeAspect,
            aPath);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalLocationWithPath::WriteStep (StepData_StepWriter& SW,
                                                           const Handle(StepShape_DimensionalLocationWithPath) &ent) const
{

  // Inherited fields of ShapeAspectRelationship

  SW.Send (ent->StepRepr_ShapeAspectRelationship::Name());

  if ( ent->StepRepr_ShapeAspectRelationship::HasDescription() ) {
    SW.Send (ent->StepRepr_ShapeAspectRelationship::Description());
  }
  else SW.SendUndef();

  SW.Send (ent->StepRepr_ShapeAspectRelationship::RelatingShapeAspect());

  SW.Send (ent->StepRepr_ShapeAspectRelationship::RelatedShapeAspect());

  // Own fields of DimensionalLocationWithPath

  SW.Send (ent->Path());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalLocationWithPath::Share (const Handle(StepShape_DimensionalLocationWithPath) &ent,
                                                       Interface_EntityIterator& iter) const
{

  // Inherited fields of ShapeAspectRelationship

  iter.AddItem (ent->StepRepr_ShapeAspectRelationship::RelatingShapeAspect());

  iter.AddItem (ent->StepRepr_ShapeAspectRelationship::RelatedShapeAspect());

  // Own fields of DimensionalLocationWithPath

  iter.AddItem (ent->Path());
}
