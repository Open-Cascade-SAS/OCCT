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
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_ContextDependentShapeRepresentation,Standard_Transient)

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
