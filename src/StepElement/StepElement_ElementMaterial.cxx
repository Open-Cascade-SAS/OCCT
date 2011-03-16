// File:	StepElement_ElementMaterial.cxx
// Created:	Thu Dec 12 17:29:01 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_ElementMaterial.ixx>

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
