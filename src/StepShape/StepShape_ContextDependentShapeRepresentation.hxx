// Created on: 1998-07-01
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _StepShape_ContextDependentShapeRepresentation_HeaderFile
#define _StepShape_ContextDependentShapeRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepRepr_ShapeRepresentationRelationship;
class StepRepr_ProductDefinitionShape;


class StepShape_ContextDependentShapeRepresentation;
DEFINE_STANDARD_HANDLE(StepShape_ContextDependentShapeRepresentation, Standard_Transient)


class StepShape_ContextDependentShapeRepresentation : public Standard_Transient
{

public:

  
  Standard_EXPORT StepShape_ContextDependentShapeRepresentation();
  
  Standard_EXPORT void Init (const Handle(StepRepr_ShapeRepresentationRelationship)& aRepRel, const Handle(StepRepr_ProductDefinitionShape)& aProRel);
  
  Standard_EXPORT Handle(StepRepr_ShapeRepresentationRelationship) RepresentationRelation() const;
  
  Standard_EXPORT void SetRepresentationRelation (const Handle(StepRepr_ShapeRepresentationRelationship)& aRepRel);
  
  Standard_EXPORT Handle(StepRepr_ProductDefinitionShape) RepresentedProductRelation() const;
  
  Standard_EXPORT void SetRepresentedProductRelation (const Handle(StepRepr_ProductDefinitionShape)& aProRel);




  DEFINE_STANDARD_RTTIEXT(StepShape_ContextDependentShapeRepresentation,Standard_Transient)

protected:




private:


  Handle(StepRepr_ShapeRepresentationRelationship) theRepRel;
  Handle(StepRepr_ProductDefinitionShape) theProRel;


};







#endif // _StepShape_ContextDependentShapeRepresentation_HeaderFile
