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

//: k8 abv 6 Jan 99: unique names for PRODUCT_DEFINITION_FORMATIONs
//: k9 abv 6 Jan 99: TR10: eliminating duplicated APPLICATION_CONTEXT entities
//: j4 gka 16.03.99 S4134
//    abv 20.11.99 renamed from StepPDR_SDRtool

#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_DesignContext.hxx>
#include <StepBasic_MechanicalContext.hxx>
#include <StepBasic_ProductContext.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormationWithSpecifiedSource.hxx>
#include <StepBasic_ProductType.hxx>
#include <STEPConstruct_Part.hxx>
#include <StepData_StepModel.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>

// ------------------------------
// Modification a faire :
// ApplicationProtocolDefinition status
// ApplicationProtocolDefinition year
// ApplicationProtocolDefinition schema_name
// ces information ne sont plus accessibles en parcourant le graphe a partir
// du ShapeDefinitionRepresentation.
// Elles se trouvent dans Cc1 au niveau d`une entite racine !
// ------------------------------
//=================================================================================================

STEPConstruct_Part::STEPConstruct_Part()
{
  myDone = Standard_False;
}

//=================================================================================================

void STEPConstruct_Part::MakeSDR(const Handle(StepShape_ShapeRepresentation)& SR,
                                 const Handle(TCollection_HAsciiString)&      aName,
                                 const Handle(StepBasic_ApplicationContext)&  AC,
                                 Handle(StepData_StepModel)&                  theStepModel)
{
  // get current schema
  const Standard_Integer schema = theStepModel->InternalParameters.WriteSchema;

  // create PC
  Handle(StepBasic_ProductContext) PC;
  switch (schema)
  {
    default:
    case 1:
      PC = new StepBasic_MechanicalContext;
      break;
    case 4:
    case 5:
    case 2:
      PC = new StepBasic_ProductContext;
      break;
    case 3:
      PC = new StepBasic_MechanicalContext;
      break;
  }
  Handle(TCollection_HAsciiString) PCname           = new TCollection_HAsciiString("");
  Handle(TCollection_HAsciiString) PCdisciplineType = new TCollection_HAsciiString("mechanical");
  PC->Init(PCname, AC, PCdisciplineType);

  // create product
  Handle(StepBasic_Product)                 P   = new StepBasic_Product;
  Handle(StepBasic_HArray1OfProductContext) PCs = new StepBasic_HArray1OfProductContext(1, 1);
  PCs->SetValue(1, PC);
  Handle(TCollection_HAsciiString) Pdescription = new TCollection_HAsciiString("");
  P->Init(aName, aName, Pdescription, PCs);

  // create PDF
  Handle(StepBasic_ProductDefinitionFormation) PDF;
  switch (schema)
  {
    default:
    case 1:
    case 2:
    case 5:
      PDF = new StepBasic_ProductDefinitionFormation;
      break;
    case 3:
      PDF = new StepBasic_ProductDefinitionFormationWithSpecifiedSource;
      Handle(StepBasic_ProductDefinitionFormationWithSpecifiedSource)::DownCast(PDF)->SetMakeOrBuy(
        StepBasic_sNotKnown);
      break;
  }
  Handle(TCollection_HAsciiString) PDFName        = new TCollection_HAsciiString("");
  Handle(TCollection_HAsciiString) PDFdescription = new TCollection_HAsciiString("");
  PDF->Init(PDFName, PDFdescription, P);

  // create PDC, depending on current schema
  Handle(StepBasic_ProductDefinitionContext) PDC;
  Handle(TCollection_HAsciiString)           PDCname;
  switch (schema)
  {
    default:
    case 1:
    case 2:
    case 5:
      PDC     = new StepBasic_ProductDefinitionContext;
      PDCname = new TCollection_HAsciiString("part definition");
      break;
    case 3:
      PDC     = new StepBasic_DesignContext;
      PDCname = new TCollection_HAsciiString("");
      break;
  }
  Handle(TCollection_HAsciiString) PDClifeCycleStage = new TCollection_HAsciiString("design");
  PDC->Init(PDCname, AC, PDClifeCycleStage);

  // create PD
  Handle(StepBasic_ProductDefinition) PD            = new StepBasic_ProductDefinition;
  Handle(TCollection_HAsciiString)    PDId          = new TCollection_HAsciiString("design");
  Handle(TCollection_HAsciiString)    PDdescription = new TCollection_HAsciiString("");
  PD->Init(PDId, PDdescription, PDF, PDC);

  // create PDS
  Handle(StepRepr_ProductDefinitionShape) PDS            = new StepRepr_ProductDefinitionShape;
  Handle(TCollection_HAsciiString)        PDSname        = new TCollection_HAsciiString("");
  Handle(TCollection_HAsciiString)        PDSdescription = new TCollection_HAsciiString("");
  StepRepr_CharacterizedDefinition        CD;
  CD.SetValue(PD);
  PDS->Init(PDSname, Standard_True, PDSdescription, CD);

  // finally, create SDR
  mySDR = new StepShape_ShapeDefinitionRepresentation;
  StepRepr_RepresentedDefinition RD;
  RD.SetValue(PDS);
  mySDR->Init(RD, SR);

  // and an associated PRPC
  Handle(TCollection_HAsciiString) PRPCName;
  switch (theStepModel->InternalParameters.WriteSchema)
  {
    default:
    case 1:
      myPRPC   = new StepBasic_ProductType;
      PRPCName = new TCollection_HAsciiString("part");
      break;
    case 4:
    case 2:
    case 5:
      myPRPC   = new StepBasic_ProductRelatedProductCategory;
      PRPCName = new TCollection_HAsciiString("part");
      break;
    case 3:
      myPRPC   = new StepBasic_ProductRelatedProductCategory;
      PRPCName = new TCollection_HAsciiString("detail"); // !!!!! or "assembly"
      break;
  }
  Handle(StepBasic_HArray1OfProduct) PRPCproducts = new StepBasic_HArray1OfProduct(1, 1);
  PRPCproducts->SetValue(1, P);
  myPRPC->Init(PRPCName, Standard_False, 0, PRPCproducts);

  myDone = Standard_True;
}

//=================================================================================================

void STEPConstruct_Part::ReadSDR(const Handle(StepShape_ShapeDefinitionRepresentation)& aShape)
{
  mySDR  = aShape;
  myDone = (!mySDR.IsNull());
}

//=================================================================================================

Standard_Boolean STEPConstruct_Part::IsDone() const
{
  return myDone;
}

//=================================================================================================

Handle(StepShape_ShapeDefinitionRepresentation) STEPConstruct_Part::SDRValue() const
{
  return mySDR;
}

//=================================================================================================

Handle(StepShape_ShapeRepresentation) STEPConstruct_Part::SRValue() const
{
  if (!myDone)
    return 0;
  return Handle(StepShape_ShapeRepresentation)::DownCast(mySDR->UsedRepresentation());
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::PCname() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->FrameOfReferenceValue(1)
    ->Name();
}

//=================================================================================================

Handle(StepBasic_ProductContext) STEPConstruct_Part::PC() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->FrameOfReferenceValue(1);
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::PCdisciplineType() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->FrameOfReferenceValue(1)
    ->DisciplineType();
}

//=================================================================================================

void STEPConstruct_Part::SetPCname(const Handle(TCollection_HAsciiString)& name)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->FrameOfReferenceValue(1)
    ->SetName(name);
}

//=================================================================================================

void STEPConstruct_Part::SetPCdisciplineType(const Handle(TCollection_HAsciiString)& label)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->FrameOfReferenceValue(1)
    ->SetDisciplineType(label);
}

//=================================================================================================

Handle(StepBasic_ApplicationContext) STEPConstruct_Part::AC() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->FrameOfReferenceValue(1)
    ->FrameOfReference();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::ACapplication() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->FrameOfReferenceValue(1)
    ->FrameOfReference()
    ->Application();
}

//=================================================================================================

void STEPConstruct_Part::SetACapplication(const Handle(TCollection_HAsciiString)& text)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->FrameOfReferenceValue(1)
    ->FrameOfReference()
    ->SetApplication(text);
}

//=================================================================================================

Handle(StepBasic_ProductDefinitionContext) STEPConstruct_Part::PDC() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->FrameOfReference();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::PDCname() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->FrameOfReference()
    ->Name();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::PDCstage() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->FrameOfReference()
    ->LifeCycleStage();
}

//=================================================================================================

void STEPConstruct_Part::SetPDCname(const Handle(TCollection_HAsciiString)& label)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->FrameOfReference()
    ->SetName(label);
}

//=================================================================================================

void STEPConstruct_Part::SetPDCstage(const Handle(TCollection_HAsciiString)& label)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->FrameOfReference()
    ->SetLifeCycleStage(label);
}

//=================================================================================================

Handle(StepBasic_Product) STEPConstruct_Part::Product() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::Pid() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->Id();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::Pname() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->Name();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::Pdescription() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->Description();
}

//=================================================================================================

void STEPConstruct_Part::SetPid(const Handle(TCollection_HAsciiString)& id)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->SetId(id);
}

//=================================================================================================

void STEPConstruct_Part::SetPname(const Handle(TCollection_HAsciiString)& label)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->SetName(label);
}

//=================================================================================================

void STEPConstruct_Part::SetPdescription(const Handle(TCollection_HAsciiString)& text)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct()
    ->SetDescription(text);
}

//=================================================================================================

Handle(StepBasic_ProductDefinitionFormation) STEPConstruct_Part::PDF() const
{
  return mySDR->Definition().PropertyDefinition()->Definition().ProductDefinition()->Formation();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::PDFid() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->Id();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::PDFdescription() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->Description();
}

//=================================================================================================

void STEPConstruct_Part::SetPDFid(const Handle(TCollection_HAsciiString)& id)
{
  mySDR->Definition().PropertyDefinition()->Definition().ProductDefinition()->Formation()->SetId(
    id);
}

//=================================================================================================

void STEPConstruct_Part::SetPDFdescription(const Handle(TCollection_HAsciiString)& text)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->SetDescription(text);
}

//=================================================================================================

Handle(StepRepr_ProductDefinitionShape) STEPConstruct_Part::PDS() const
{
  return Handle(StepRepr_ProductDefinitionShape)::DownCast(
    mySDR->Definition().PropertyDefinition());
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::PDSname() const
{
  return mySDR->Definition().PropertyDefinition()->Name();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::PDSdescription() const
{
  return mySDR->Definition().PropertyDefinition()->Description();
}

//=================================================================================================

void STEPConstruct_Part::SetPDSname(const Handle(TCollection_HAsciiString)& label)
{
  mySDR->Definition().PropertyDefinition()->SetName(label);
}

//=================================================================================================

void STEPConstruct_Part::SetPDSdescription(const Handle(TCollection_HAsciiString)& text)
{
  mySDR->Definition().PropertyDefinition()->SetDescription(text);
}

//=================================================================================================

Handle(StepBasic_ProductDefinition) STEPConstruct_Part::PD() const
{
  return mySDR->Definition().PropertyDefinition()->Definition().ProductDefinition();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::PDdescription() const
{
  return mySDR->Definition().PropertyDefinition()->Definition().ProductDefinition()->Description();
}

//=================================================================================================

// a modifier
void STEPConstruct_Part::SetPDdescription(const Handle(TCollection_HAsciiString)& text)
{
  mySDR->Definition().PropertyDefinition()->Definition().ProductDefinition()->SetDescription(text);
}

//=================================================================================================

Handle(StepBasic_ProductRelatedProductCategory) STEPConstruct_Part::PRPC() const
{
  return myPRPC;
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::PRPCname() const
{
  return myPRPC->Name();
}

//=================================================================================================

Handle(TCollection_HAsciiString) STEPConstruct_Part::PRPCdescription() const
{
  return myPRPC->Description();
}

//=================================================================================================

void STEPConstruct_Part::SetPRPCname(const Handle(TCollection_HAsciiString)& text)
{
  myPRPC->SetName(text);
}

//=================================================================================================

void STEPConstruct_Part::SetPRPCdescription(const Handle(TCollection_HAsciiString)& text)
{
  myPRPC->SetDescription(text);
}
