// Created on: 2003-06-04
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Standard_Type.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_Datum,StepRepr_ShapeAspect)

//=======================================================================
//function : StepDimTol_Datum
//purpose  : 
//=======================================================================
StepDimTol_Datum::StepDimTol_Datum ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_Datum::Init (const Handle(TCollection_HAsciiString)& theShapeAspect_Name,
                             const Handle(TCollection_HAsciiString)& theShapeAspect_Description,
                             const Handle(StepRepr_ProductDefinitionShape)& theShapeAspect_OfShape,
                             const StepData_Logical theShapeAspect_ProductDefinitional,
                             const Handle(TCollection_HAsciiString)& theIdentification)
{
  StepRepr_ShapeAspect::Init(theShapeAspect_Name,
                             theShapeAspect_Description,
                             theShapeAspect_OfShape,
                             theShapeAspect_ProductDefinitional);
  myIdentification = theIdentification;
}

//=======================================================================
//function : Identification
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepDimTol_Datum::Identification () const
{
  return myIdentification;
}

//=======================================================================
//function : SetIdentification
//purpose  : 
//=======================================================================

void StepDimTol_Datum::SetIdentification (const Handle(TCollection_HAsciiString) &theIdentification)
{
  myIdentification = theIdentification;
}
