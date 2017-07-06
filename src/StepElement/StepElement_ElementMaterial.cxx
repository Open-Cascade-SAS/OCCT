// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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
#include <StepElement_ElementMaterial.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_ElementMaterial,Standard_Transient)

//=======================================================================
//function : StepElement_ElementMaterial
//purpose  : 
//=======================================================================
StepElement_ElementMaterial::StepElement_ElementMaterial ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_ElementMaterial::Init (const Handle(TCollection_HAsciiString) &aMaterialId,
                                        const Handle(TCollection_HAsciiString) &aDescription,
                                        const Handle(StepRepr_HArray1OfMaterialPropertyRepresentation) &aProperties)
{

  theMaterialId = aMaterialId;

  theDescription = aDescription;

  theProperties = aProperties;
}

//=======================================================================
//function : MaterialId
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepElement_ElementMaterial::MaterialId () const
{
  return theMaterialId;
}

//=======================================================================
//function : SetMaterialId
//purpose  : 
//=======================================================================

void StepElement_ElementMaterial::SetMaterialId (const Handle(TCollection_HAsciiString) &aMaterialId)
{
  theMaterialId = aMaterialId;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepElement_ElementMaterial::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepElement_ElementMaterial::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : Properties
//purpose  : 
//=======================================================================

Handle(StepRepr_HArray1OfMaterialPropertyRepresentation) StepElement_ElementMaterial::Properties () const
{
  return theProperties;
}

//=======================================================================
//function : SetProperties
//purpose  : 
//=======================================================================

void StepElement_ElementMaterial::SetProperties (const Handle(StepRepr_HArray1OfMaterialPropertyRepresentation) &aProperties)
{
  theProperties = aProperties;
}
