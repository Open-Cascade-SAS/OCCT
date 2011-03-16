#include <StepRepr_RepresentationRelationshipWithTransformation.ixx>

StepRepr_RepresentationRelationshipWithTransformation::StepRepr_RepresentationRelationshipWithTransformation ()    {  }

void  StepRepr_RepresentationRelationshipWithTransformation::Init
  (const Handle(TCollection_HAsciiString)& aName,
   const Handle(TCollection_HAsciiString)& aDescription,
   const Handle(StepRepr_Representation)& aRep1,
   const Handle(StepRepr_Representation)& aRep2,
   const StepRepr_Transformation& aTrans)
{
  StepRepr_RepresentationRelationship::Init (aName,aDescription,aRep1,aRep2);
  theTrans = aTrans;
}

StepRepr_Transformation  StepRepr_RepresentationRelationshipWithTransformation::TransformationOperator () const
{  return theTrans;  }

void  StepRepr_RepresentationRelationshipWithTransformation::SetTransformationOperator (const StepRepr_Transformation& aTrans)
{  theTrans = aTrans;  }
