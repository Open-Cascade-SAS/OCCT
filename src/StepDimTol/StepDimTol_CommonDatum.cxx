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
#include <StepDimTol_CommonDatum.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_CommonDatum,StepRepr_CompositeShapeAspect)

//=======================================================================
//function : StepDimTol_CommonDatum
//purpose  : 
//=======================================================================
StepDimTol_CommonDatum::StepDimTol_CommonDatum ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_CommonDatum::Init (const Handle(TCollection_HAsciiString) &theShapeAspect_Name,
                                   const Handle(TCollection_HAsciiString) &theShapeAspect_Description,
                                   const Handle(StepRepr_ProductDefinitionShape) &theShapeAspect_OfShape,
                                   const StepData_Logical theShapeAspect_ProductDefinitional,
                                   const Handle(TCollection_HAsciiString) &theDatum_Name,
                                   const Handle(TCollection_HAsciiString) &theDatum_Description,
                                   const Handle(StepRepr_ProductDefinitionShape) &theDatum_OfShape,
                                   const StepData_Logical theDatum_ProductDefinitional,
                                   const Handle(TCollection_HAsciiString) &theDatum_Identification)
{
    StepRepr_CompositeShapeAspect::Init(theShapeAspect_Name,
                                        theShapeAspect_Description,
                                        theShapeAspect_OfShape,
                                        theShapeAspect_ProductDefinitional);
    myDatum->Init(theDatum_Name,
                   theDatum_Description,
                   theDatum_OfShape,
                   theDatum_ProductDefinitional,
                   theDatum_Identification);
}

//=======================================================================
//function : Datum
//purpose  : 
//=======================================================================

Handle(StepDimTol_Datum) StepDimTol_CommonDatum::Datum () const
{
  return myDatum;
}

//=======================================================================
//function : SetDatum
//purpose  : 
//=======================================================================

void StepDimTol_CommonDatum::SetDatum (const Handle(StepDimTol_Datum) &theDatum)
{
  myDatum = theDatum;
}
