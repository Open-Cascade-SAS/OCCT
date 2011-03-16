// File:	RWStepElement_RWElementMaterial.cxx
// Created:	Thu Dec 12 17:29:01 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepElement_RWElementMaterial.ixx>
#include <StepRepr_HArray1OfMaterialPropertyRepresentation.hxx>
#include <StepRepr_MaterialPropertyRepresentation.hxx>

//=======================================================================
//function : RWStepElement_RWElementMaterial
//purpose  : 
//=======================================================================

RWStepElement_RWElementMaterial::RWStepElement_RWElementMaterial ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWElementMaterial::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                const Standard_Integer num,
                                                Handle(Interface_Check)& ach,
                                                const Handle(StepElement_ElementMaterial) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"element_material") ) return;

  // Own fields of ElementMaterial

  Handle(TCollection_HAsciiString) aMaterialId;
  data->ReadString (num, 1, "material_id", ach, aMaterialId);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);

  Handle(StepRepr_HArray1OfMaterialPropertyRepresentation) aProperties;
  Standard_Integer sub3 = 0;
  if ( data->ReadSubList (num, 3, "properties", ach, sub3) ) {
    Standard_Integer nb0 = data->NbParams(sub3);
    aProperties = new StepRepr_HArray1OfMaterialPropertyRepresentation (1, nb0);
    Standard_Integer num2 = sub3;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepRepr_MaterialPropertyRepresentation) anIt0;
      data->ReadEntity (num2, i0, "material_property_representation", ach, STANDARD_TYPE(StepRepr_MaterialPropertyRepresentation), anIt0);
      aProperties->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aMaterialId,
            aDescription,
            aProperties);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWElementMaterial::WriteStep (StepData_StepWriter& SW,
                                                 const Handle(StepElement_ElementMaterial) &ent) const
{

  // Own fields of ElementMaterial

  SW.Send (ent->MaterialId());

  SW.Send (ent->Description());

  SW.OpenSub();
  for (Standard_Integer i2=1; i2 <= ent->Properties()->Length(); i2++ ) {
    Handle(StepRepr_MaterialPropertyRepresentation) Var0 = ent->Properties()->Value(i2);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWElementMaterial::Share (const Handle(StepElement_ElementMaterial) &ent,
                                             Interface_EntityIterator& iter) const
{

  // Own fields of ElementMaterial

  for (Standard_Integer i1=1; i1 <= ent->Properties()->Length(); i1++ ) {
    Handle(StepRepr_MaterialPropertyRepresentation) Var0 = ent->Properties()->Value(i1);
    iter.AddItem (Var0);
  }
}
