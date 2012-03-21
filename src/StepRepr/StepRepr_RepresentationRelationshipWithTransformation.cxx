// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
