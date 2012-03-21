// Created on: 1993-09-20
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// :j4 16.03.99 gka S4134
// abv 18.11.99 renamed from StepPDR_ContextTool
#include <STEPConstruct_ContextTool.ixx>
#include <StepData_StepModel.hxx>

#include <Interface_Macros.hxx>

#include <StepBasic_ProductDefinition.hxx>
//#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductContext.hxx>
#include <StepBasic_HArray1OfProductContext.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ApplicationProtocolDefinition.hxx>
#include <StepRepr_PropertyDefinition.hxx>

#include <StepBasic_HArray1OfProduct.hxx>
#include <StepBasic_ProductType.hxx> //:i3
#include <Interface_Static.hxx> //:j4
#include <GeomToStep_MakeAxis2Placement3d.hxx>

#include "stdio.h"

//=======================================================================
//function : STEPConstruct_ContextTool
//purpose  :
//=======================================================================

STEPConstruct_ContextTool::STEPConstruct_ContextTool ()
{
}

//=======================================================================
//function : STEPConstruct_ContextTool
//purpose  :
//=======================================================================

STEPConstruct_ContextTool::STEPConstruct_ContextTool (const Handle(StepData_StepModel)& aStepModel)
{
  SetModel(aStepModel);
}

//=======================================================================
//function : SetModel
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::SetModel (const Handle(StepData_StepModel)& aStepModel)
{
  theAPD.Nullify();  //thePRPC.Nullify();

  Standard_Integer i, nb = aStepModel->NbEntities();
  for(i = 1; i<=nb && theAPD.IsNull(); i ++) {
    Handle(Standard_Transient) ent = aStepModel->Value(i);
    if (ent->IsKind(STANDARD_TYPE(StepBasic_ApplicationProtocolDefinition))) {
      if (theAPD.IsNull())  theAPD  = GetCasted(StepBasic_ApplicationProtocolDefinition, ent);
    }
    //if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductRelatedProductCategory))) {
    //  if (thePRPC.IsNull()) thePRPC = GetCasted(StepBasic_ProductRelatedProductCategory, ent);
    //}
  }
}

//=======================================================================
//function : GetAPD
//purpose  :
//=======================================================================

Handle(StepBasic_ApplicationProtocolDefinition)  STEPConstruct_ContextTool::GetAPD()
{
  return theAPD;
}

//=======================================================================
//function : AddAPD
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::AddAPD (const Standard_Boolean enforce)
{
  Standard_Boolean noapd = theAPD.IsNull();
  if (noapd || enforce) theAPD  = new StepBasic_ApplicationProtocolDefinition;

  switch (Interface_Static::IVal("write.step.schema")) { //j4
  default:
  case 1:
    theAPD->SetApplicationProtocolYear (1997);
    theAPD->SetStatus (new TCollection_HAsciiString("committee draft"));
    theAPD->SetApplicationInterpretedModelSchemaName
      (new TCollection_HAsciiString("automotive_design"));
    break;
  case 2:
    theAPD->SetApplicationProtocolYear (1998);
    theAPD->SetStatus (new TCollection_HAsciiString("draft international standard"));
    theAPD->SetApplicationInterpretedModelSchemaName
      (new TCollection_HAsciiString("automotive_design"));
    break;
  case 3:
    theAPD->SetApplicationProtocolYear (1994);
    theAPD->SetStatus (new TCollection_HAsciiString("international standard"));
    theAPD->SetApplicationInterpretedModelSchemaName
      (new TCollection_HAsciiString("config_control_design"));
    break;
    
  case 4: theAPD->SetApplicationProtocolYear (2000);
    theAPD->SetStatus (new TCollection_HAsciiString("international standard"));
    theAPD->SetApplicationInterpretedModelSchemaName
      (new TCollection_HAsciiString("automotive_design"));
    break;
  }

  if (theAPD->Application().IsNull())
    theAPD->SetApplication (new StepBasic_ApplicationContext);
  Handle(TCollection_HAsciiString) appl;
  switch (Interface_Static::IVal("write.step.schema")) { //j4
  default:
  case 1:
  case 2: appl = new TCollection_HAsciiString ( "core data for automotive mechanical design processes" );
          break;
  case 3: appl = new TCollection_HAsciiString ( "configuration controlled 3D designs of mechanical parts and assemblies" );
          break;
  }
  theAPD->Application()->SetApplication ( appl );
//  if (noapd || enforce) aStepModel->AddWithRefs (theAPD);
}

//=======================================================================
//function : IsAP203
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ContextTool::IsAP203 () const
{
  if ( theAPD.IsNull() ) return Standard_False;
  Handle(TCollection_HAsciiString) schema = theAPD->ApplicationInterpretedModelSchemaName();
  if ( schema.IsNull() ) return Standard_False;
  TCollection_AsciiString sch = schema->String();
  sch.LowerCase();
  return sch == "config_control_design";
}

//=======================================================================
//function : IsAP214
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ContextTool::IsAP214 () const
{
  if ( theAPD.IsNull() ) return Standard_False;
  Handle(TCollection_HAsciiString) schema = theAPD->ApplicationInterpretedModelSchemaName();
  if ( schema.IsNull() ) return Standard_False;
  TCollection_AsciiString sch = schema->String();
  sch.LowerCase();
  return sch == "automotive_design";
}

// ================================================================
// Data Section : Basic Product Information (level S1)
//                   * Get methods
//                   * Set methods
// ================================================================

//=======================================================================
//function : GetACstatus
//purpose  :
//=======================================================================

Handle(TCollection_HAsciiString)  STEPConstruct_ContextTool::GetACstatus()
{
  if (GetAPD().IsNull()) return new TCollection_HAsciiString("");
  return GetAPD()->Status();
}

//=======================================================================
//function : GetACschemaName
//purpose  :
//=======================================================================

Handle(TCollection_HAsciiString)  STEPConstruct_ContextTool::GetACschemaName()
{
  if (GetAPD().IsNull()) return new TCollection_HAsciiString("");
  return  GetAPD()->ApplicationInterpretedModelSchemaName();
}

//=======================================================================
//function : GetACyear
//purpose  :
//=======================================================================

Standard_Integer  STEPConstruct_ContextTool::GetACyear()
{
  return (GetAPD().IsNull() ? 1998 :
	  GetAPD()->ApplicationProtocolYear());
}

/*
//=======================================================================
//function : GetACapplication
//purpose  :
//=======================================================================

Handle(TCollection_HAsciiString)  STEPConstruct_ContextTool::GetACapplication()
{
  return GetPDC()->Formation()->OfProduct()->FrameOfReferenceValue(1)
    ->FrameOfReference()->Application();
}
*/

//=======================================================================
//function : GetACname
//purpose  :
//=======================================================================

Handle(TCollection_HAsciiString)  STEPConstruct_ContextTool::GetACname()
{
  if (GetAPD().IsNull()) return new TCollection_HAsciiString("");
  if (GetAPD()->Application().IsNull()) return new TCollection_HAsciiString("");
  return GetAPD()->Application()->Application();
}

//=======================================================================
//function : SetACstatus
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::SetACstatus (const Handle(TCollection_HAsciiString)& status)
{
  if (GetAPD().IsNull()) return;
  GetAPD()->SetStatus(status);
}

//=======================================================================
//function : SetACschemaName
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::SetACschemaName (const Handle(TCollection_HAsciiString)& schemaName)
{
  if (GetAPD().IsNull()) return;
  GetAPD()->SetApplicationInterpretedModelSchemaName(schemaName);
}

//=======================================================================
//function : SetACyear
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::SetACyear (const Standard_Integer year)
{
  if (GetAPD().IsNull()) return;
  GetAPD()->SetApplicationProtocolYear(year);
}

//=======================================================================
//function : SetACname
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::SetACname (const Handle(TCollection_HAsciiString)& name)
{
  if (GetAPD().IsNull()) return;
  if (GetAPD()->Application().IsNull()) GetAPD()->SetApplication
    (new StepBasic_ApplicationContext);
  GetAPD()->Application()->SetApplication (name);
}

/*
//=======================================================================
//function : SetACapplication
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::SetACapplication (const Handle(TCollection_HAsciiString)& application)
{
  GetPDC()->Formation()->OfProduct()->FrameOfReferenceValue(1)
    ->FrameOfReference()->SetApplication(application);

}
*/

// --------------------------------
// Product Related Product Category
// --------------------------------

/*

//=======================================================================
//function : GetPRPC
//purpose  :
//=======================================================================

Handle(StepBasic_ProductRelatedProductCategory) STEPConstruct_ContextTool::GetPRPC()
{
  return thePRPC;
}

//=======================================================================
//function : AddPRPC
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::AddPRPC (const Standard_Boolean enforce)
{
  Standard_Boolean noprpc = thePRPC.IsNull();
  if (noprpc || enforce) {
    //:i3 abv 1 Sep 98: ProSTEP TR9: generate PRODUCT_TYPE (derived) instead of PRPC
    switch (Interface_Static::IVal("write.step.schema")) { //j4
    default:
    case 1:
      thePRPC = new StepBasic_ProductType;
      thePRPC->SetName (new TCollection_HAsciiString("part"));
      break;
    case 4:
    case 2:
      thePRPC = new StepBasic_ProductRelatedProductCategory;
      thePRPC->SetName (new TCollection_HAsciiString("part"));
      break;
    case 3:
      thePRPC = new StepBasic_ProductRelatedProductCategory;
      thePRPC->SetName (new TCollection_HAsciiString("detail")); // !!!!! or "assembly"
      break;
    }
    thePRPC->UnSetDescription(); //:i3
  }
//  if (noprpc || enforce) aStepModel->AddWithRefs(thePRPC);
}

//=======================================================================
//function : SetPRPCName
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::SetPRPCName(const Handle(TCollection_HAsciiString)& aName)
{
  GetPRPC()->SetName(aName);
}

//=======================================================================
//function : GetPRPCName
//purpose  :
//=======================================================================

Handle(TCollection_HAsciiString)  STEPConstruct_ContextTool::GetPRPCName()
{
  if (GetPRPC().IsNull()) return new TCollection_HAsciiString("");
  return GetPRPC()->Name();
}

//=======================================================================
//function : SetPRPCDescription
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::SetPRPCDescription (const Handle(TCollection_HAsciiString)& aDescription)
{
  Handle(StepBasic_ProductRelatedProductCategory) aPRPC = GetPRPC();
  aPRPC->SetDescription(aDescription);
//  aPRPC->HasDescription(Standard_True);
}

//=======================================================================
//function : GetPRPCDescription
//purpose  :
//=======================================================================

Handle(TCollection_HAsciiString) STEPConstruct_ContextTool::GetPRPCDescription()
{
  Handle(StepBasic_ProductRelatedProductCategory) aPRPC = GetPRPC();
  if (!aPRPC.IsNull() && aPRPC->HasDescription())
    return aPRPC->Description();
  else {
    return new TCollection_HAsciiString("");
  }
}

// ================================================================
//
// Settings from an already done SDR (see SDRtool)
//
// ================================================================

void STEPConstruct_ContextTool::SetSDR (const Handle(StepShape_ShapeDefinitionRepresentation)& sdr)
{
//  SDR partage des choses avec le contexte ... On raccroche les wagons
  Handle(StepBasic_Product) theProduct =
    sdr->Definition()->Definition().ProductDefinition()->Formation()->OfProduct();
  Handle(StepBasic_ApplicationContext) theAppli =
    theProduct->FrameOfReferenceValue(1)->FrameOfReference();
  Handle(StepBasic_HArray1OfProduct) ProdList =
    new StepBasic_HArray1OfProduct(1,1);
  ProdList->SetValue(1,theProduct);
  thePRPC->SetProducts (ProdList);
  theAPD->SetApplication (theAppli);
}
*/

//=======================================================================
//function : GetDefaultAxis
//purpose  :
//=======================================================================

Handle(StepGeom_Axis2Placement3d) STEPConstruct_ContextTool::GetDefaultAxis ()
{
  if ( myAxis.IsNull() ) {
    GeomToStep_MakeAxis2Placement3d mkax;
    myAxis = mkax.Value();
  }
  return myAxis;
}

//=======================================================================
//function : AP203Context
//purpose  :
//=======================================================================

STEPConstruct_AP203Context &STEPConstruct_ContextTool::AP203Context ()
{
  return theAP203;
}

//=======================================================================
//function : Level
//purpose  :
//=======================================================================

Standard_Integer STEPConstruct_ContextTool::Level () const
{
  return myLevel.Length();
}

//=======================================================================
//function : NextLevel
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::NextLevel ()
{
  myLevel.Append ( 1 );
}

//=======================================================================
//function : PrevLevel
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::PrevLevel ()
{
  if ( myLevel.Length() >0 ) myLevel.Remove ( myLevel.Length() );
}

//=======================================================================
//function : SetLevel
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::SetLevel (const Standard_Integer lev)
{
  if ( lev < myLevel.Length() ) {
    while ( lev < myLevel.Length() && myLevel.Length() >0 )
      myLevel.Remove ( myLevel.Length() );
  }
  else {
    while ( myLevel.Length() < lev ) myLevel.Append ( 1 );
  }
}

//=======================================================================
//function : Index
//purpose  :
//=======================================================================

Standard_Integer STEPConstruct_ContextTool::Index () const
{
  return ( myLevel.Length() >0 ? myLevel.Last() : 0 );
}

//=======================================================================
//function : NextIndex
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::NextIndex ()
{
  if ( myLevel.Length() >0 )
    myLevel.SetValue ( myLevel.Length(), myLevel.Last() + 1 );
}

//=======================================================================
//function : PrevIndex
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::PrevIndex ()
{
  if ( myLevel.Length() >0 )
    myLevel.SetValue ( myLevel.Length(), myLevel.Last() - 1 );
}

//=======================================================================
//function : SetIndex
//purpose  :
//=======================================================================

void STEPConstruct_ContextTool::SetIndex (const Standard_Integer ind)
{
  if ( myLevel.Length() >0 )
    myLevel.SetValue ( myLevel.Length(), ind );
}

//=======================================================================
//function : GetProductName
//purpose  :
//=======================================================================

Handle(TCollection_HAsciiString) STEPConstruct_ContextTool::GetProductName () const
{
  Handle(TCollection_HAsciiString) PdtName;
  if (Interface_Static::IsSet("write.step.product.name"))
    PdtName = new TCollection_HAsciiString(Interface_Static::CVal("write.step.product.name"));
  else PdtName = new TCollection_HAsciiString("Product");

  for ( Standard_Integer i=1; i <= myLevel.Length(); i++ ) {
    PdtName->AssignCat ((char*)( i >1 ? "." : " " ));
    char buf[100];
    sprintf ( buf, "%d", myLevel.Value(i) );
    PdtName->AssignCat ( buf );
  }

  return PdtName;
}

//=======================================================================
//function : GetRootsForPart
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) STEPConstruct_ContextTool::GetRootsForPart (const STEPConstruct_Part &SDRTool)
{
  Handle(TColStd_HSequenceOfTransient) seq = new TColStd_HSequenceOfTransient;

  seq->Append ( SDRTool.SDRValue() );
//  seq->Append ( GetAPD() );

  if ( ! SDRTool.PRPC().IsNull() ) seq->Append ( SDRTool.PRPC() );

  // for AP203, add required product management data
  if ( Interface_Static::IVal("write.step.schema") == 3 ) {
    theAP203.Init ( SDRTool );
    seq->Append (theAP203.GetProductCategoryRelationship());
    seq->Append (theAP203.GetCreator());
    seq->Append (theAP203.GetDesignOwner());
    seq->Append (theAP203.GetDesignSupplier());
    seq->Append (theAP203.GetClassificationOfficer());
    seq->Append (theAP203.GetSecurity());
    seq->Append (theAP203.GetCreationDate());
    seq->Append (theAP203.GetClassificationDate());
    seq->Append (theAP203.GetApproval());
    seq->Append (theAP203.GetApprover());
    seq->Append (theAP203.GetApprovalDateTime());
  }

  return seq;
}

//=======================================================================
//function : GetRootsForAssemblyLink
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) STEPConstruct_ContextTool::GetRootsForAssemblyLink (const STEPConstruct_Assembly &assembly)
{
  Handle(TColStd_HSequenceOfTransient) seq = new TColStd_HSequenceOfTransient;

  seq->Append ( assembly.ItemValue() );
  
  // for AP203, write required product management data
  if ( Interface_Static::IVal("write.step.schema") == 3 ) {
    theAP203.Init ( assembly.GetNAUO() );
    seq->Append (theAP203.GetSecurity());
    seq->Append (theAP203.GetClassificationOfficer());
    seq->Append (theAP203.GetClassificationDate());
    seq->Append (theAP203.GetApproval());
    seq->Append (theAP203.GetApprover());
    seq->Append (theAP203.GetApprovalDateTime());
  }

  return seq;
}





