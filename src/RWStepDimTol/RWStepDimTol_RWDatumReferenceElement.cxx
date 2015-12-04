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

#include <RWStepDimTol_RWDatumReferenceElement.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_DatumReferenceElement.hxx>
#include <StepDimTol_DatumReferenceModifier.hxx>
#include <StepDimTol_GeneralDatumReference.hxx>
#include <StepDimTol_HArray1OfDatumReferenceElement.hxx>
#include <StepDimTol_HArray1OfDatumReferenceModifier.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepDimTol_DatumReferenceModifierWithValue.hxx>

//=======================================================================
//function : RWStepDimTol_RWDatumReferenceElement
//purpose  : 
//=======================================================================

RWStepDimTol_RWDatumReferenceElement::RWStepDimTol_RWDatumReferenceElement ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWDatumReferenceElement::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                     const Standard_Integer num,
                                                     Handle(Interface_Check)& ach,
                                                     const Handle(StepDimTol_DatumReferenceElement) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,6,ach,"datum_reference_element") ) return;

  // Inherited fields of ShapeAspect

  Handle(TCollection_HAsciiString) aShapeAspect_Name;
  data->ReadString (num, 1, "shape_aspect.name", ach, aShapeAspect_Name);

  Handle(TCollection_HAsciiString) aShapeAspect_Description;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "shape_aspect.description", ach, aShapeAspect_Description);
  }

  Handle(StepRepr_ProductDefinitionShape) aShapeAspect_OfShape;
  data->ReadEntity (num, 3, "shape_aspect.of_shape", ach, STANDARD_TYPE(StepRepr_ProductDefinitionShape), aShapeAspect_OfShape);

  StepData_Logical aShapeAspect_ProductDefinitional;
  data->ReadLogical (num, 4, "shape_aspect.product_definitional", ach, aShapeAspect_ProductDefinitional);
  
  // Inherited fields from GeneralDatumReference
  
  StepDimTol_DatumOrCommonDatum aBase;
  Handle(StepDimTol_Datum) aDatum;
  Interface_ParamType aType = data->ParamType(num, 5);
  if (aType == Interface_ParamIdent) {
    data->ReadEntity(num, 5, "general_datum_reference.base", ach, STANDARD_TYPE(StepDimTol_Datum), aDatum);
    aBase.SetValue(aDatum);
  }
  else {
    Handle(StepDimTol_HArray1OfDatumReferenceElement) anItems;
    Handle(StepDimTol_DatumReferenceElement) anEnt;
    Standard_Integer nbSub;
    if (data->ReadSubList (num,5,"general_datum_reference.base",ach,nbSub)) {
      aType = data->ParamType(nbSub, 1);      
      if (aType == Interface_ParamSub) {
        Standard_Integer aNewNbSub;
        if (data->ReadSubList (nbSub,1,"general_datum_reference.base",ach,aNewNbSub)) {
          nbSub = aNewNbSub;
        }
      }
      Standard_Integer nbElements = data->NbParams(nbSub);
      anItems = new StepDimTol_HArray1OfDatumReferenceElement (1, nbElements);
      for (Standard_Integer i = 1; i <= nbElements; i++) {
        if (data->ReadEntity(nbSub, i,"datum_reference_element", ach, STANDARD_TYPE(StepDimTol_DatumReferenceElement), anEnt))
          anItems->SetValue(i, anEnt);
      }
    }
    aBase.SetValue(anItems);
  }

  Standard_Integer nbSub;
  Standard_Boolean hasModifiers = data->ReadSubList(num, 6, "general_datum_reference.modifiers", ach, nbSub, Standard_True);
  Handle(StepDimTol_HArray1OfDatumReferenceModifier) aModifiers;
  if (hasModifiers) {
    StepDimTol_DatumReferenceModifier anEnt;
    Standard_Integer nbElements = data->NbParams(nbSub);
    aModifiers = new StepDimTol_HArray1OfDatumReferenceModifier (1, nbElements);
    for (Standard_Integer i = 1; i <= nbElements; i++) {
      aType = data->ParamType (nbSub, i);
      if (aType == Interface_ParamIdent) {
        Handle(StepDimTol_DatumReferenceModifierWithValue) aDRMWV;
        data->ReadEntity(nbSub, i,"datum_reference_modifier_with_value", ach, STANDARD_TYPE(StepDimTol_DatumReferenceModifierWithValue), aDRMWV);
        anEnt.SetValue(aDRMWV);
      }
      else {
        Handle(StepData_SelectMember) aMember;
        data->ReadMember(nbSub, i, "simple_datum_reference_modifier", ach, aMember);
        Standard_CString anEnumText = aMember->EnumText();
        Handle(StepDimTol_SimpleDatumReferenceModifierMember) aSDRM = new StepDimTol_SimpleDatumReferenceModifierMember();
        aSDRM->SetEnumText(0, anEnumText);
        anEnt.SetValue(aSDRM);
      }
      aModifiers->SetValue(i, anEnt);
    }
  }
  
  // Initialize entity
  ent->Init(aShapeAspect_Name,
            aShapeAspect_Description,
            aShapeAspect_OfShape,
            aShapeAspect_ProductDefinitional,
            aBase,
            hasModifiers,
            aModifiers);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWDatumReferenceElement::WriteStep (StepData_StepWriter& SW,
                                                      const Handle(StepDimTol_DatumReferenceElement) &ent) const
{

  // Inherited fields of ShapeAspect

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.Send (ent->OfShape());

  SW.SendLogical (ent->ProductDefinitional());
  
  // Inherited fields from GeneralDatumReference
  Standard_Integer aBaseType = ent->Base().CaseNum(ent->Base().Value());
  if (aBaseType == 1) {
    SW.Send(ent->Base().Datum());
  }
  else if (aBaseType == 2) {
    Handle(StepDimTol_HArray1OfDatumReferenceElement) anArray = (ent->Base()).CommonDatumList();
    Standard_Integer i, nb = (anArray.IsNull() ? 0 : anArray->Length());
    SW.OpenTypedSub("COMMON_DATUM_LIST");
    for (i = 1; i <= nb; i++)  
      SW.Send (anArray->Value(i));
    SW.CloseSub();
  }

  if (ent->HasModifiers()) {
    Standard_Integer i, nb = ent->NbModifiers();
    SW.OpenSub();
    for (i = 1; i <= nb; i++) {
      StepDimTol_DatumReferenceModifier aModifier = ent->ModifiersValue(i);
      Standard_Integer aType = aModifier.CaseNum(aModifier.Value());
      switch (aType) {
        case 1: SW.Send(aModifier.DatumReferenceModifierWithValue()); break;
        case 2: SW.SendEnum(aModifier.SimpleDatumReferenceModifierMember()->EnumText());break;
      }
    }
    SW.CloseSub();
  }
  else {
    SW.SendUndef();
  }
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWDatumReferenceElement::Share (const Handle(StepDimTol_DatumReferenceElement) &ent,
                                                  Interface_EntityIterator& iter) const
{

  // Inherited fields of ShapeAspect

  iter.AddItem (ent->OfShape());
  
  // Inherited fields from GeneralDatumReference
  Standard_Integer aBaseType = ent->Base().CaseNum(ent->Base().Value());
  if (aBaseType == 1) {
    iter.AddItem(ent->Base().Datum());
  }
  else if (aBaseType == 2) {
    Handle(StepDimTol_HArray1OfDatumReferenceElement) anArray = ent->Base().CommonDatumList();
    Standard_Integer i, nb = (anArray.IsNull() ? 0 : anArray->Length());
    for (i = 1; i <= nb; i++)  
      iter.AddItem (anArray->Value(i));
  }
}
