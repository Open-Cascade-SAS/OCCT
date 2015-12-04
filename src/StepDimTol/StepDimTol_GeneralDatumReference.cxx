// Created on: 2015-07-16
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StepDimTol_GeneralDatumReference.hxx>

#include <StepDimTol_HArray1OfDatumReferenceModifier.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_GeneralDatumReference,StepRepr_ShapeAspect)

//=======================================================================
//function : StepDimTol_GeneralDatumReference
//purpose  : 
//=======================================================================

StepDimTol_GeneralDatumReference::StepDimTol_GeneralDatumReference ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepDimTol_GeneralDatumReference::Init (const Handle(TCollection_HAsciiString)& theName,
                                             const Handle(TCollection_HAsciiString)& theDescription,
                                             const Handle(StepRepr_ProductDefinitionShape)& theOfShape,
                                             const StepData_Logical theProductDefinitional,
                                             const StepDimTol_DatumOrCommonDatum& theBase,
                                             const Standard_Boolean theHasModifiers,
                                             const Handle(StepDimTol_HArray1OfDatumReferenceModifier)& theModifiers)
{
  StepRepr_ShapeAspect::Init(theName, theDescription, theOfShape, theProductDefinitional);
  myBase = theBase;
  if (theHasModifiers)
    myModifiers = theModifiers;
  else
    myModifiers.Nullify();
}
    