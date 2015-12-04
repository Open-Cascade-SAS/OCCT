// Created on: 2000-04-18
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Standard_Type.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepShape_DimensionalLocationWithPath.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_DimensionalLocationWithPath,StepShape_DimensionalLocation)

//=======================================================================
//function : StepShape_DimensionalLocationWithPath
//purpose  : 
//=======================================================================
StepShape_DimensionalLocationWithPath::StepShape_DimensionalLocationWithPath ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_DimensionalLocationWithPath::Init (const Handle(TCollection_HAsciiString) &aShapeAspectRelationship_Name,
                                                  const Standard_Boolean hasShapeAspectRelationship_Description,
                                                  const Handle(TCollection_HAsciiString) &aShapeAspectRelationship_Description,
                                                  const Handle(StepRepr_ShapeAspect) &aShapeAspectRelationship_RelatingShapeAspect,
                                                  const Handle(StepRepr_ShapeAspect) &aShapeAspectRelationship_RelatedShapeAspect,
                                                  const Handle(StepRepr_ShapeAspect) &aPath)
{
  StepShape_DimensionalLocation::Init(aShapeAspectRelationship_Name,
                                      hasShapeAspectRelationship_Description,
                                      aShapeAspectRelationship_Description,
                                      aShapeAspectRelationship_RelatingShapeAspect,
                                      aShapeAspectRelationship_RelatedShapeAspect);

  thePath = aPath;
}

//=======================================================================
//function : Path
//purpose  : 
//=======================================================================

Handle(StepRepr_ShapeAspect) StepShape_DimensionalLocationWithPath::Path () const
{
  return thePath;
}

//=======================================================================
//function : SetPath
//purpose  : 
//=======================================================================

void StepShape_DimensionalLocationWithPath::SetPath (const Handle(StepRepr_ShapeAspect) &aPath)
{
  thePath = aPath;
}
