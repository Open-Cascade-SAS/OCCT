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

#include "RWStepDimTol_RWDatumSystem.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_DatumSystem.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>

//=================================================================================================

RWStepDimTol_RWDatumSystem::RWStepDimTol_RWDatumSystem() {}

//=================================================================================================

void RWStepDimTol_RWDatumSystem::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                          const int                 num,
                                          occ::handle<Interface_Check>&               ach,
                                          const occ::handle<StepDimTol_DatumSystem>&  ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 5, ach, "datum_system"))
    return;

  // Inherited fields of ShapeAspect

  occ::handle<TCollection_HAsciiString> aShapeAspect_Name;
  data->ReadString(num, 1, "shape_aspect.name", ach, aShapeAspect_Name);

  occ::handle<TCollection_HAsciiString> aShapeAspect_Description;
  if (data->IsParamDefined(num, 2))
  {
    data->ReadString(num, 2, "shape_aspect.description", ach, aShapeAspect_Description);
  }

  occ::handle<StepRepr_ProductDefinitionShape> aShapeAspect_OfShape;
  data->ReadEntity(num,
                   3,
                   "shape_aspect.of_shape",
                   ach,
                   STANDARD_TYPE(StepRepr_ProductDefinitionShape),
                   aShapeAspect_OfShape);

  StepData_Logical aShapeAspect_ProductDefinitional;
  data->ReadLogical(num,
                    4,
                    "shape_aspect.product_definitional",
                    ach,
                    aShapeAspect_ProductDefinitional);

  // Own fields of DatumSystem

  occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>> aConstituents;
  occ::handle<StepDimTol_DatumReferenceCompartment>          anEnt;
  int                                      nbSub;
  if (data->ReadSubList(num, 5, "base", ach, nbSub))
  {
    int nbElements = data->NbParams(nbSub);
    aConstituents               = new NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>(1, nbElements);
    for (int i = 1; i <= nbElements; i++)
    {
      if (data->ReadEntity(nbSub,
                           i,
                           "datum_reference_compartment",
                           ach,
                           STANDARD_TYPE(StepDimTol_DatumReferenceCompartment),
                           anEnt))
        aConstituents->SetValue(i, anEnt);
    }
  }

  // Initialize entity
  ent->Init(aShapeAspect_Name,
            aShapeAspect_Description,
            aShapeAspect_OfShape,
            aShapeAspect_ProductDefinitional,
            aConstituents);
}

//=================================================================================================

void RWStepDimTol_RWDatumSystem::WriteStep(StepData_StepWriter&                  SW,
                                           const occ::handle<StepDimTol_DatumSystem>& ent) const
{

  // Inherited fields of ShapeAspect

  SW.Send(ent->Name());

  SW.Send(ent->Description());

  SW.Send(ent->OfShape());

  SW.SendLogical(ent->ProductDefinitional());

  // Own fields of DatumSystem
  int i, nb = ent->NbConstituents();
  SW.OpenSub();
  for (i = 1; i <= nb; i++)
    SW.Send(ent->ConstituentsValue(i));
  SW.CloseSub();
}

//=================================================================================================

void RWStepDimTol_RWDatumSystem::Share(const occ::handle<StepDimTol_DatumSystem>& ent,
                                       Interface_EntityIterator&             iter) const
{

  // Inherited fields of ShapeAspect

  iter.AddItem(ent->OfShape());

  // Own fields of DatumSystem
  int i, nb = ent->NbConstituents();
  for (i = 1; i <= nb; i++)
    iter.AddItem(ent->ConstituentsValue(i));
}
