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
#include <RWStepFEA_RWFeaModel.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_FeaModel.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfAsciiString.hxx>

//=======================================================================
//function : RWStepFEA_RWFeaModel
//purpose  : 
//=======================================================================
RWStepFEA_RWFeaModel::RWStepFEA_RWFeaModel ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaModel::ReadStep (const Handle(StepData_StepReaderData)& data,
                                     const Standard_Integer num,
                                     Handle(Interface_Check)& ach,
                                     const Handle(StepFEA_FeaModel) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,7,ach,"fea_model") ) return;

  // Inherited fields of Representation

  Handle(TCollection_HAsciiString) aRepresentation_Name;
  data->ReadString (num, 1, "representation.name", ach, aRepresentation_Name);

  Handle(StepRepr_HArray1OfRepresentationItem) aRepresentation_Items;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "representation.items", ach, sub2) ) {
    Standard_Integer nb0 = data->NbParams(sub2);
    aRepresentation_Items = new StepRepr_HArray1OfRepresentationItem (1, nb0);
    Standard_Integer num2 = sub2;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepRepr_RepresentationItem) anIt0;
      data->ReadEntity (num2, i0, "representation_item", ach, STANDARD_TYPE(StepRepr_RepresentationItem), anIt0);
      aRepresentation_Items->SetValue(i0, anIt0);
    }
  }

  Handle(StepRepr_RepresentationContext) aRepresentation_ContextOfItems;
  data->ReadEntity (num, 3, "representation.context_of_items", ach, STANDARD_TYPE(StepRepr_RepresentationContext), aRepresentation_ContextOfItems);

  // Own fields of FeaModel

  Handle(TCollection_HAsciiString) aCreatingSoftware;
  data->ReadString (num, 4, "creating_software", ach, aCreatingSoftware);

  Handle(TColStd_HArray1OfAsciiString) aIntendedAnalysisCode;
  Standard_Integer sub5 = 0;
  if ( data->ReadSubList (num, 5, "intended_analysis_code", ach, sub5) ) {
    Standard_Integer nb0 = data->NbParams(sub5);
    aIntendedAnalysisCode = new TColStd_HArray1OfAsciiString (1, nb0);
    Standard_Integer num2 = sub5;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(TCollection_HAsciiString) anIt0;
      data->ReadString (num2, i0, "h_ascii_string", ach, anIt0);
      aIntendedAnalysisCode->SetValue(i0, anIt0->String());
    }
  }

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 6, "description", ach, aDescription);

  Handle(TCollection_HAsciiString) aAnalysisType;
  data->ReadString (num, 7, "analysis_type", ach, aAnalysisType);

  // Initialize entity
  ent->Init(aRepresentation_Name,
            aRepresentation_Items,
            aRepresentation_ContextOfItems,
            aCreatingSoftware,
            aIntendedAnalysisCode,
            aDescription,
            aAnalysisType);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaModel::WriteStep (StepData_StepWriter& SW,
                                      const Handle(StepFEA_FeaModel) &ent) const
{

  // Inherited fields of Representation

  SW.Send (ent->StepRepr_Representation::Name());

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->StepRepr_Representation::NbItems(); i1++ ) {
    Handle(StepRepr_RepresentationItem) Var0 = ent->StepRepr_Representation::Items()->Value(i1);
    SW.Send (Var0);
  }
  SW.CloseSub();

  SW.Send (ent->StepRepr_Representation::ContextOfItems());

  // Own fields of FeaModel

  SW.Send (ent->CreatingSoftware());

  SW.OpenSub();
  for (Standard_Integer i4=1; i4 <= ent->IntendedAnalysisCode()->Length(); i4++ ) {
    Handle(TCollection_HAsciiString) Var0 = 
      new TCollection_HAsciiString(ent->IntendedAnalysisCode()->Value(i4));
    SW.Send (Var0);
  }
  SW.CloseSub();

  SW.Send (ent->Description());

  SW.Send (ent->AnalysisType());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaModel::Share (const Handle(StepFEA_FeaModel) &ent,
                                  Interface_EntityIterator& iter) const
{

  // Inherited fields of Representation

  for (Standard_Integer i1=1; i1 <= ent->StepRepr_Representation::NbItems(); i1++ ) {
    Handle(StepRepr_RepresentationItem) Var0 = ent->StepRepr_Representation::Items()->Value(i1);
    iter.AddItem (Var0);
  }

  iter.AddItem (ent->StepRepr_Representation::ContextOfItems());

  // Own fields of FeaModel
}
