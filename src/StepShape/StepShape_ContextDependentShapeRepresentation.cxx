#include <StepShape_ContextDependentShapeRepresentation.ixx>

StepShape_ContextDependentShapeRepresentation::StepShape_ContextDependentShapeRepresentation ()    {  }

void  StepShape_ContextDependentShapeRepresentation::Init
  (const Handle(StepRepr_ShapeRepresentationRelationship)& aRepRel,
   const Handle(StepRepr_ProductDefinitionShape)& aProRel)
{
  theRepRel = aRepRel;
  theProRel = aProRel;
}

Handle(StepRepr_ShapeRepresentationRelationship)  StepShape_ContextDependentShapeRepresentation::RepresentationRelation () const
{  return theRepRel;  }

void  StepShape_ContextDependentShapeRepresentation::SetRepresentationRelation (const Handle(StepRepr_ShapeRepresentationRelationship)& aRepRel)
{  theRepRel = aRepRel;  }

Handle(StepRepr_ProductDefinitionShape)  StepShape_ContextDependentShapeRepresentation::RepresentedProductRelation () const
{  return theProRel;  }

void  StepShape_ContextDependentShapeRepresentation::SetRepresentedProductRelation (const Handle(StepRepr_ProductDefinitionShape)& aProRel)
{  theProRel = aProRel;  }
