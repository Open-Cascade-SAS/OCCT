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

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include "RWStepFEA_RWFeaParametricPoint.pxx"
#include <Standard_Real.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_FeaParametricPoint.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

RWStepFEA_RWFeaParametricPoint::RWStepFEA_RWFeaParametricPoint() {}

//=================================================================================================

void RWStepFEA_RWFeaParametricPoint::ReadStep(const occ::handle<StepData_StepReaderData>&    data,
                                              const int                    num,
                                              occ::handle<Interface_Check>&                  ach,
                                              const occ::handle<StepFEA_FeaParametricPoint>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 2, ach, "fea_parametric_point"))
    return;

  // Inherited fields of RepresentationItem

  occ::handle<TCollection_HAsciiString> aRepresentationItem_Name;
  data->ReadString(num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of FeaParametricPoint

  occ::handle<NCollection_HArray1<double>> aCoordinates;
  int              sub2 = 0;
  if (data->ReadSubList(num, 2, "coordinates", ach, sub2))
  {
    int nb0  = data->NbParams(sub2);
    aCoordinates          = new NCollection_HArray1<double>(1, nb0);
    int num2 = sub2;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      double anIt0;
      data->ReadReal(num2, i0, "real", ach, anIt0);
      aCoordinates->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aRepresentationItem_Name, aCoordinates);
}

//=================================================================================================

void RWStepFEA_RWFeaParametricPoint::WriteStep(StepData_StepWriter&                      SW,
                                               const occ::handle<StepFEA_FeaParametricPoint>& ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send(ent->StepRepr_RepresentationItem::Name());

  // Own fields of FeaParametricPoint

  SW.OpenSub();
  for (int i1 = 1; i1 <= ent->Coordinates()->Length(); i1++)
  {
    double Var0 = ent->Coordinates()->Value(i1);
    SW.Send(Var0);
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepFEA_RWFeaParametricPoint::Share(const occ::handle<StepFEA_FeaParametricPoint>&,
                                           Interface_EntityIterator&) const
{
  // Inherited fields of RepresentationItem
  // Own fields of FeaParametricPoint
}
