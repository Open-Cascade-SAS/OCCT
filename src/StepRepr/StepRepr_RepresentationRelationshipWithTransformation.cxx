// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Standard_Type.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_Transformation.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_RepresentationRelationshipWithTransformation,StepRepr_ShapeRepresentationRelationship)

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
