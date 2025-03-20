// Created on: 1993-09-20
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// :j4 16.03.99 gka S4134
// abv 18.11.99 renamed from StepPDR_ContextTool

#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <Interface_Macros.hxx>
#include <StepAP203_CcDesignApproval.hxx>
#include <StepAP203_CcDesignDateAndTimeAssignment.hxx>
#include <StepAP203_CcDesignPersonAndOrganizationAssignment.hxx>
#include <StepAP203_CcDesignSecurityClassification.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ApplicationProtocolDefinition.hxx>
#include <StepBasic_ApprovalDateTime.hxx>
#include <StepBasic_ApprovalPersonOrganization.hxx>
#include <StepBasic_ProductCategoryRelationship.hxx>
#include <StepBasic_ProductRelatedProductCategory.hxx>
#include <STEPConstruct_Assembly.hxx>
#include <STEPConstruct_ContextTool.hxx>
#include <STEPConstruct_Part.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>

#include "stdio.h"

// #include <StepBasic_ProductDefinitionContext.hxx>
//: i3
//: j4
//=================================================================================================

STEPConstruct_ContextTool::STEPConstruct_ContextTool() {}

//=================================================================================================

STEPConstruct_ContextTool::STEPConstruct_ContextTool(const Handle(StepData_StepModel)& aStepModel)
{
  SetModel(aStepModel);
}

//=================================================================================================

void STEPConstruct_ContextTool::SetModel(const Handle(StepData_StepModel)& aStepModel)
{
  theAPD.Nullify(); // thePRPC.Nullify();
  mySchema      = aStepModel->InternalParameters.WriteSchema;
  myProductName = aStepModel->InternalParameters.WriteProductName;
  Standard_Integer i, nb = aStepModel->NbEntities();
  for (i = 1; i <= nb && theAPD.IsNull(); i++)
  {
    Handle(Standard_Transient) ent = aStepModel->Value(i);
    if (ent->IsKind(STANDARD_TYPE(StepBasic_ApplicationProtocolDefinition)))
    {
      if (theAPD.IsNull())
        theAPD = GetCasted(StepBasic_ApplicationProtocolDefinition, ent);
    }
    // if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductRelatedProductCategory))) {
    //   if (thePRPC.IsNull()) thePRPC = GetCasted(StepBasic_ProductRelatedProductCategory, ent);
    // }
  }
}

//=================================================================================================

void STEPConstruct_ContextTool::SetGlobalFactor(const StepData_Factors& theGlobalFactor)
{
  myGlobalFactor = theGlobalFactor;
}

//=================================================================================================

Handle(StepBasic_ApplicationProtocolDefinition) STEPConstruct_ContextTool::GetAPD()
{
  return theAPD;
}

//=================================================================================================

void STEPConstruct_ContextTool::AddAPD(const Standard_Boolean enforce)
{
  Standard_Boolean noapd = theAPD.IsNull();
  if (noapd || enforce)
    theAPD = new StepBasic_ApplicationProtocolDefinition;

  switch (mySchema)
  { // j4
    default:
    case 1:
      theAPD->SetApplicationProtocolYear(1997);
      theAPD->SetStatus(new TCollection_HAsciiString("committee draft"));
      theAPD->SetApplicationInterpretedModelSchemaName(
        new TCollection_HAsciiString("automotive_design"));
      break;
    case 2:
      theAPD->SetApplicationProtocolYear(1998);
      theAPD->SetStatus(new TCollection_HAsciiString("draft international standard"));
      theAPD->SetApplicationInterpretedModelSchemaName(
        new TCollection_HAsciiString("automotive_design"));
      break;
    case 3:
      theAPD->SetApplicationProtocolYear(1994);
      theAPD->SetStatus(new TCollection_HAsciiString("international standard"));
      theAPD->SetApplicationInterpretedModelSchemaName(
        new TCollection_HAsciiString("config_control_design"));
      break;

    case 4:
      theAPD->SetApplicationProtocolYear(2000);
      theAPD->SetStatus(new TCollection_HAsciiString("international standard"));
      theAPD->SetApplicationInterpretedModelSchemaName(
        new TCollection_HAsciiString("automotive_design"));
      break;
    case 5:
      theAPD->SetApplicationProtocolYear(2013);
      theAPD->SetStatus(new TCollection_HAsciiString("international standard"));
      theAPD->SetApplicationInterpretedModelSchemaName(
        new TCollection_HAsciiString("ap242_managed_model_based_3d_engineering"));
      break;
  }

  if (theAPD->Application().IsNull())
    theAPD->SetApplication(new StepBasic_ApplicationContext);
  Handle(TCollection_HAsciiString) appl;
  switch (mySchema)
  { // j4
    default:
    case 1:
    case 2:
      appl = new TCollection_HAsciiString("core data for automotive mechanical design processes");
      break;
    case 3:
      appl = new TCollection_HAsciiString(
        "configuration controlled 3D designs of mechanical parts and assemblies");
      break;
    case 5:
      appl = new TCollection_HAsciiString("Managed model based 3d engineering");
      break;
  }
  theAPD->Application()->SetApplication(appl);
  //  if (noapd || enforce) aStepModel->AddWithRefs (theAPD);
}

//=================================================================================================

Standard_Boolean STEPConstruct_ContextTool::IsAP203() const
{
  if (theAPD.IsNull())
    return Standard_False;
  Handle(TCollection_HAsciiString) schema = theAPD->ApplicationInterpretedModelSchemaName();
  if (schema.IsNull())
    return Standard_False;
  TCollection_AsciiString sch = schema->String();
  sch.LowerCase();
  return sch == "config_control_design";
}

//=================================================================================================

Standard_Boolean STEPConstruct_ContextTool::IsAP214() const
{
  if (theAPD.IsNull())
    return Standard_False;
  Handle(TCollection_HAsciiString) schema = theAPD->ApplicationInterpretedModelSchemaName();
  if (schema.IsNull())
    return Standard_False;
  TCollection_AsciiString sch = schema->String();
  sch.LowerCase();
  return sch == "automotive_design";
}

//=================================================================================================

Standard_Boolean STEPConstruct_ContextTool::IsAP242() const
{
  if (theAPD.IsNull())
    return Standard_False;
  Handle(TCollection_HAsciiString) schema = theAPD->ApplicationInterpretedModelSchemaName();
  if (schema.IsNull())
    return Standard_False;
  TCollection_AsciiString sch = schema->String();
  sch.LowerCase();
  return sch == "ap242_managed_model_based_3d_engineering";
}

// ================================================================
// Data Section : Basic Product Information (level S1)
//                   * Get methods
//                   * Set methods
// ================================================================

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_ContextTool::GetACstatus()
{
  if (GetAPD().IsNull())
    return new TCollection_HAsciiString("");
  return GetAPD()->Status();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_ContextTool::GetACschemaName()
{
  if (GetAPD().IsNull())
    return new TCollection_HAsciiString("");
  return GetAPD()->ApplicationInterpretedModelSchemaName();
}

//=================================================================================================

Standard_Integer STEPConstruct_ContextTool::GetACyear()
{
  return (GetAPD().IsNull() ? 1998 : GetAPD()->ApplicationProtocolYear());
}

/*
//=================================================================================================

Handle(TCollection_HAsciiString)  STEPConstruct_ContextTool::GetACapplication()
{
  return GetPDC()->Formation()->OfProduct()->FrameOfReferenceValue(1)
    ->FrameOfReference()->Application();
}
*/

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_ContextTool::GetACname()
{
  if (GetAPD().IsNull())
    return new TCollection_HAsciiString("");
  if (GetAPD()->Application().IsNull())
    return new TCollection_HAsciiString("");
  return GetAPD()->Application()->Application();
}

//=================================================================================================

void STEPConstruct_ContextTool::SetACstatus(const Handle(TCollection_HAsciiString)& status)
{
  if (GetAPD().IsNull())
    return;
  GetAPD()->SetStatus(status);
}

//=================================================================================================

void STEPConstruct_ContextTool::SetACschemaName(const Handle(TCollection_HAsciiString)& schemaName)
{
  if (GetAPD().IsNull())
    return;
  GetAPD()->SetApplicationInterpretedModelSchemaName(schemaName);
}

//=================================================================================================

void STEPConstruct_ContextTool::SetACyear(const Standard_Integer year)
{
  if (GetAPD().IsNull())
    return;
  GetAPD()->SetApplicationProtocolYear(year);
}

//=================================================================================================

void STEPConstruct_ContextTool::SetACname(const Handle(TCollection_HAsciiString)& name)
{
  if (GetAPD().IsNull())
    return;
  if (GetAPD()->Application().IsNull())
    GetAPD()->SetApplication(new StepBasic_ApplicationContext);
  GetAPD()->Application()->SetApplication(name);
}

//=================================================================================================

Handle(StepGeom_Axis2Placement3d) STEPConstruct_ContextTool::GetDefaultAxis()
{
  if (myAxis.IsNull())
  {
    GeomToStep_MakeAxis2Placement3d mkax(myGlobalFactor);
    myAxis = mkax.Value();
  }
  return myAxis;
}

//=================================================================================================

STEPConstruct_AP203Context& STEPConstruct_ContextTool::AP203Context()
{
  return theAP203;
}

//=================================================================================================

Standard_Integer STEPConstruct_ContextTool::Level() const
{
  return myLevel.Length();
}

//=================================================================================================

void STEPConstruct_ContextTool::NextLevel()
{
  myLevel.Append(1);
}

//=================================================================================================

void STEPConstruct_ContextTool::PrevLevel()
{
  if (myLevel.Length() > 0)
    myLevel.Remove(myLevel.Length());
}

//=================================================================================================

void STEPConstruct_ContextTool::SetLevel(const Standard_Integer lev)
{
  if (lev < myLevel.Length())
  {
    while (lev < myLevel.Length() && myLevel.Length() > 0)
      myLevel.Remove(myLevel.Length());
  }
  else
  {
    while (myLevel.Length() < lev)
      myLevel.Append(1);
  }
}

//=================================================================================================

Standard_Integer STEPConstruct_ContextTool::Index() const
{
  return (myLevel.Length() > 0 ? myLevel.Last() : 0);
}

//=================================================================================================

void STEPConstruct_ContextTool::NextIndex()
{
  if (myLevel.Length() > 0)
    myLevel.SetValue(myLevel.Length(), myLevel.Last() + 1);
}

//=================================================================================================

void STEPConstruct_ContextTool::PrevIndex()
{
  if (myLevel.Length() > 0)
    myLevel.SetValue(myLevel.Length(), myLevel.Last() - 1);
}

//=================================================================================================

void STEPConstruct_ContextTool::SetIndex(const Standard_Integer ind)
{
  if (myLevel.Length() > 0)
    myLevel.SetValue(myLevel.Length(), ind);
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_ContextTool::GetProductName() const
{
  Handle(TCollection_HAsciiString) PdtName;
  if (!myProductName.IsEmpty())
    PdtName = new TCollection_HAsciiString(myProductName);
  else
    PdtName = new TCollection_HAsciiString("Product");

  for (Standard_Integer i = 1; i <= myLevel.Length(); i++)
  {
    PdtName->AssignCat((char*)(i > 1 ? "." : " "));
    char buf[100];
    sprintf(buf, "%d", myLevel.Value(i));
    PdtName->AssignCat(buf);
  }

  return PdtName;
}

//=================================================================================================

Handle(TColStd_HSequenceOfTransient) STEPConstruct_ContextTool::GetRootsForPart(
  const STEPConstruct_Part& SDRTool)
{
  Handle(TColStd_HSequenceOfTransient) seq = new TColStd_HSequenceOfTransient;

  seq->Append(SDRTool.SDRValue());
  //  seq->Append ( GetAPD() );

  if (!SDRTool.PRPC().IsNull())
    seq->Append(SDRTool.PRPC());

  // for AP203, add required product management data
  if (mySchema == 3)
  {
    theAP203.Init(SDRTool);
    seq->Append(theAP203.GetProductCategoryRelationship());
    seq->Append(theAP203.GetCreator());
    seq->Append(theAP203.GetDesignOwner());
    seq->Append(theAP203.GetDesignSupplier());
    seq->Append(theAP203.GetClassificationOfficer());
    seq->Append(theAP203.GetSecurity());
    seq->Append(theAP203.GetCreationDate());
    seq->Append(theAP203.GetClassificationDate());
    seq->Append(theAP203.GetApproval());
    seq->Append(theAP203.GetApprover());
    seq->Append(theAP203.GetApprovalDateTime());
  }

  return seq;
}

//=================================================================================================

Handle(TColStd_HSequenceOfTransient) STEPConstruct_ContextTool::GetRootsForAssemblyLink(
  const STEPConstruct_Assembly& assembly)
{
  Handle(TColStd_HSequenceOfTransient) seq = new TColStd_HSequenceOfTransient;

  seq->Append(assembly.ItemValue());

  // for AP203, write required product management data
  if (mySchema == 3)
  {
    theAP203.Init(assembly.GetNAUO());
    seq->Append(theAP203.GetSecurity());
    seq->Append(theAP203.GetClassificationOfficer());
    seq->Append(theAP203.GetClassificationDate());
    seq->Append(theAP203.GetApproval());
    seq->Append(theAP203.GetApprover());
    seq->Append(theAP203.GetApprovalDateTime());
  }

  return seq;
}
