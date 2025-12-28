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
  myDone = false;
}

//=================================================================================================

void STEPConstruct_Part::MakeSDR(const occ::handle<StepShape_ShapeRepresentation>& SR,
                                 const occ::handle<TCollection_HAsciiString>&      aName,
                                 const occ::handle<StepBasic_ApplicationContext>&  AC,
                                 occ::handle<StepData_StepModel>&                  theStepModel)
{
  // get current schema
  const int schema = theStepModel->InternalParameters.WriteSchema;

  // create PC
  occ::handle<StepBasic_ProductContext> PC;
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
  occ::handle<TCollection_HAsciiString> PCname = new TCollection_HAsciiString("");
  occ::handle<TCollection_HAsciiString> PCdisciplineType =
    new TCollection_HAsciiString("mechanical");
  PC->Init(PCname, AC, PCdisciplineType);

  // create product
  occ::handle<StepBasic_Product>                                          P = new StepBasic_Product;
  occ::handle<NCollection_HArray1<occ::handle<StepBasic_ProductContext>>> PCs =
    new NCollection_HArray1<occ::handle<StepBasic_ProductContext>>(1, 1);
  PCs->SetValue(1, PC);
  occ::handle<TCollection_HAsciiString> Pdescription = new TCollection_HAsciiString("");
  P->Init(aName, aName, Pdescription, PCs);

  // create PDF
  occ::handle<StepBasic_ProductDefinitionFormation> PDF;
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
      occ::down_cast<StepBasic_ProductDefinitionFormationWithSpecifiedSource>(PDF)->SetMakeOrBuy(
        StepBasic_sNotKnown);
      break;
  }
  occ::handle<TCollection_HAsciiString> PDFName        = new TCollection_HAsciiString("");
  occ::handle<TCollection_HAsciiString> PDFdescription = new TCollection_HAsciiString("");
  PDF->Init(PDFName, PDFdescription, P);

  // create PDC, depending on current schema
  occ::handle<StepBasic_ProductDefinitionContext> PDC;
  occ::handle<TCollection_HAsciiString>           PDCname;
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
  occ::handle<TCollection_HAsciiString> PDClifeCycleStage = new TCollection_HAsciiString("design");
  PDC->Init(PDCname, AC, PDClifeCycleStage);

  // create PD
  occ::handle<StepBasic_ProductDefinition> PD            = new StepBasic_ProductDefinition;
  occ::handle<TCollection_HAsciiString>    PDId          = new TCollection_HAsciiString("design");
  occ::handle<TCollection_HAsciiString>    PDdescription = new TCollection_HAsciiString("");
  PD->Init(PDId, PDdescription, PDF, PDC);

  // create PDS
  occ::handle<StepRepr_ProductDefinitionShape> PDS            = new StepRepr_ProductDefinitionShape;
  occ::handle<TCollection_HAsciiString>        PDSname        = new TCollection_HAsciiString("");
  occ::handle<TCollection_HAsciiString>        PDSdescription = new TCollection_HAsciiString("");
  StepRepr_CharacterizedDefinition             CD;
  CD.SetValue(PD);
  PDS->Init(PDSname, true, PDSdescription, CD);

  // finally, create SDR
  mySDR = new StepShape_ShapeDefinitionRepresentation;
  StepRepr_RepresentedDefinition RD;
  RD.SetValue(PDS);
  mySDR->Init(RD, SR);

  // and an associated PRPC
  occ::handle<TCollection_HAsciiString> PRPCName;
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
  occ::handle<NCollection_HArray1<occ::handle<StepBasic_Product>>> PRPCproducts =
    new NCollection_HArray1<occ::handle<StepBasic_Product>>(1, 1);
  PRPCproducts->SetValue(1, P);
  myPRPC->Init(PRPCName, false, 0, PRPCproducts);

  myDone = true;
}

//=================================================================================================

void STEPConstruct_Part::ReadSDR(const occ::handle<StepShape_ShapeDefinitionRepresentation>& aShape)
{
  mySDR  = aShape;
  myDone = (!mySDR.IsNull());
}

//=================================================================================================

bool STEPConstruct_Part::IsDone() const
{
  return myDone;
}

//=================================================================================================

occ::handle<StepShape_ShapeDefinitionRepresentation> STEPConstruct_Part::SDRValue() const
{
  return mySDR;
}

//=================================================================================================

occ::handle<StepShape_ShapeRepresentation> STEPConstruct_Part::SRValue() const
{
  if (!myDone)
    return 0;
  return occ::down_cast<StepShape_ShapeRepresentation>(mySDR->UsedRepresentation());
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::PCname() const
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

occ::handle<StepBasic_ProductContext> STEPConstruct_Part::PC() const
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

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::PCdisciplineType() const
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

void STEPConstruct_Part::SetPCname(const occ::handle<TCollection_HAsciiString>& name)
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

void STEPConstruct_Part::SetPCdisciplineType(const occ::handle<TCollection_HAsciiString>& label)
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

occ::handle<StepBasic_ApplicationContext> STEPConstruct_Part::AC() const
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

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::ACapplication() const
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

void STEPConstruct_Part::SetACapplication(const occ::handle<TCollection_HAsciiString>& text)
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

occ::handle<StepBasic_ProductDefinitionContext> STEPConstruct_Part::PDC() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->FrameOfReference();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::PDCname() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->FrameOfReference()
    ->Name();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::PDCstage() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->FrameOfReference()
    ->LifeCycleStage();
}

//=================================================================================================

void STEPConstruct_Part::SetPDCname(const occ::handle<TCollection_HAsciiString>& label)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->FrameOfReference()
    ->SetName(label);
}

//=================================================================================================

void STEPConstruct_Part::SetPDCstage(const occ::handle<TCollection_HAsciiString>& label)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->FrameOfReference()
    ->SetLifeCycleStage(label);
}

//=================================================================================================

occ::handle<StepBasic_Product> STEPConstruct_Part::Product() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->OfProduct();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::Pid() const
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

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::Pname() const
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

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::Pdescription() const
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

void STEPConstruct_Part::SetPid(const occ::handle<TCollection_HAsciiString>& id)
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

void STEPConstruct_Part::SetPname(const occ::handle<TCollection_HAsciiString>& label)
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

void STEPConstruct_Part::SetPdescription(const occ::handle<TCollection_HAsciiString>& text)
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

occ::handle<StepBasic_ProductDefinitionFormation> STEPConstruct_Part::PDF() const
{
  return mySDR->Definition().PropertyDefinition()->Definition().ProductDefinition()->Formation();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::PDFid() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->Id();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::PDFdescription() const
{
  return mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->Description();
}

//=================================================================================================

void STEPConstruct_Part::SetPDFid(const occ::handle<TCollection_HAsciiString>& id)
{
  mySDR->Definition().PropertyDefinition()->Definition().ProductDefinition()->Formation()->SetId(
    id);
}

//=================================================================================================

void STEPConstruct_Part::SetPDFdescription(const occ::handle<TCollection_HAsciiString>& text)
{
  mySDR->Definition()
    .PropertyDefinition()
    ->Definition()
    .ProductDefinition()
    ->Formation()
    ->SetDescription(text);
}

//=================================================================================================

occ::handle<StepRepr_ProductDefinitionShape> STEPConstruct_Part::PDS() const
{
  return occ::down_cast<StepRepr_ProductDefinitionShape>(mySDR->Definition().PropertyDefinition());
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::PDSname() const
{
  return mySDR->Definition().PropertyDefinition()->Name();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::PDSdescription() const
{
  return mySDR->Definition().PropertyDefinition()->Description();
}

//=================================================================================================

void STEPConstruct_Part::SetPDSname(const occ::handle<TCollection_HAsciiString>& label)
{
  mySDR->Definition().PropertyDefinition()->SetName(label);
}

//=================================================================================================

void STEPConstruct_Part::SetPDSdescription(const occ::handle<TCollection_HAsciiString>& text)
{
  mySDR->Definition().PropertyDefinition()->SetDescription(text);
}

//=================================================================================================

occ::handle<StepBasic_ProductDefinition> STEPConstruct_Part::PD() const
{
  return mySDR->Definition().PropertyDefinition()->Definition().ProductDefinition();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::PDdescription() const
{
  return mySDR->Definition().PropertyDefinition()->Definition().ProductDefinition()->Description();
}

//=================================================================================================

// a modifier
void STEPConstruct_Part::SetPDdescription(const occ::handle<TCollection_HAsciiString>& text)
{
  mySDR->Definition().PropertyDefinition()->Definition().ProductDefinition()->SetDescription(text);
}

//=================================================================================================

occ::handle<StepBasic_ProductRelatedProductCategory> STEPConstruct_Part::PRPC() const
{
  return myPRPC;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::PRPCname() const
{
  return myPRPC->Name();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPConstruct_Part::PRPCdescription() const
{
  return myPRPC->Description();
}

//=================================================================================================

void STEPConstruct_Part::SetPRPCname(const occ::handle<TCollection_HAsciiString>& text)
{
  myPRPC->SetName(text);
}

//=================================================================================================

void STEPConstruct_Part::SetPRPCdescription(const occ::handle<TCollection_HAsciiString>& text)
{
  myPRPC->SetDescription(text);
}
