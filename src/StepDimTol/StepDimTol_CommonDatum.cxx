// Created on: 2003-06-04
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepDimTol_CommonDatum.ixx>

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

void StepDimTol_CommonDatum::Init (const Handle(TCollection_HAsciiString) &aShapeAspect_Name,
                                   const Handle(TCollection_HAsciiString) &aShapeAspect_Description,
                                   const Handle(StepRepr_ProductDefinitionShape) &aShapeAspect_OfShape,
                                   const StepData_Logical aShapeAspect_ProductDefinitional,
                                   const Handle(TCollection_HAsciiString) &aDatum_Name,
                                   const Handle(TCollection_HAsciiString) &aDatum_Description,
                                   const Handle(StepRepr_ProductDefinitionShape) &aDatum_OfShape,
                                   const StepData_Logical aDatum_ProductDefinitional,
                                   const Handle(TCollection_HAsciiString) &aDatum_Identification)
{
    StepRepr_CompositeShapeAspect::Init(aShapeAspect_Name,
                                        aShapeAspect_Description,
                                        aShapeAspect_OfShape,
                                        aShapeAspect_ProductDefinitional);
    theDatum->Init(aDatum_Name,
                   aDatum_Description,
                   aDatum_OfShape,
                   aDatum_ProductDefinitional,
                   aDatum_Identification);
}

//=======================================================================
//function : Datum
//purpose  : 
//=======================================================================

Handle(StepDimTol_Datum) StepDimTol_CommonDatum::Datum () const
{
  return theDatum;
}

//=======================================================================
//function : SetDatum
//purpose  : 
//=======================================================================

void StepDimTol_CommonDatum::SetDatum (const Handle(StepDimTol_Datum) &aDatum)
{
  theDatum = aDatum;
}
