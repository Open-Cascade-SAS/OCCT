// Created on: 2000-09-29
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <Interface_EntityIterator.hxx>
#include <StepAP203_ApprovedItem.hxx>
#include <StepAP203_CcDesignApproval.hxx>
#include <StepAP203_CcDesignDateAndTimeAssignment.hxx>
#include <StepAP203_CcDesignPersonAndOrganizationAssignment.hxx>
#include <StepAP203_DateTimeItem.hxx>
#include <StepAP203_HArray1OfApprovedItem.hxx>
#include <StepAP203_HArray1OfDateTimeItem.hxx>
#include <StepAP203_HArray1OfPersonOrganizationItem.hxx>
#include <StepAP203_PersonOrganizationItem.hxx>
#include <StepAP214_AppliedDocumentReference.hxx>
#include <StepAP214_AppliedExternalIdentificationAssignment.hxx>
#include <StepAP214_ExternalIdentificationItem.hxx>
#include <StepAP214_HArray1OfDocumentReferenceItem.hxx>
#include <StepAP214_HArray1OfExternalIdentificationItem.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ApplicationProtocolDefinition.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepBasic_DocumentProductEquivalence.hxx>
#include <StepBasic_DocumentRepresentationType.hxx>
#include <StepBasic_DocumentType.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_HArray1OfDocument.hxx>
#include <StepBasic_HArray1OfProduct.hxx>
#include <StepBasic_HArray1OfProductContext.hxx>
#include <StepBasic_IdentificationRole.hxx>
#include <StepBasic_ObjectRole.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductContext.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionWithAssociatedDocuments.hxx>
#include <StepBasic_ProductOrFormationOrDefinition.hxx>
#include <StepBasic_ProductRelatedProductCategory.hxx>
#include <StepBasic_RoleAssociation.hxx>
#include <StepBasic_SourceItem.hxx>
#include <STEPConstruct_ExternRefs.hxx>
#include <StepData_SelectNamed.hxx>
#include <StepRepr_CharacterizedDefinition.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentedDefinition.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSControl_TransferReader.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>

//=======================================================================
//function : STEPConstruct_ExternRefs
//purpose  : 
//=======================================================================
STEPConstruct_ExternRefs::STEPConstruct_ExternRefs ()
{
}
     
//=======================================================================
//function : STEPConstruct_ExternRefs
//purpose  : 
//=======================================================================

STEPConstruct_ExternRefs::STEPConstruct_ExternRefs (const Handle(XSControl_WorkSession) &WS)
     : STEPConstruct_Tool ( WS )
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean STEPConstruct_ExternRefs::Init (const Handle(XSControl_WorkSession) &WS)
{
  Clear();
  return SetWS ( WS );
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void STEPConstruct_ExternRefs::Clear ()
{
  myAEIAs.Clear();
  myRoles.Clear();
  myFormats.Clear();
  myShapes.Clear();
  myTypes.Clear();
  myIsAP214.Clear();
  // PTV 30.01.2003 TRJ11
  myDocFiles.Clear();
  mySharedPRPC.Nullify();
  mySharedDocType.Nullify();
  mySharedPDC.Nullify();
  mySharedPC.Nullify();
  myAPD.Nullify();
}
  
//=======================================================================
//function : LoadExternRefs
//purpose  : 
//=======================================================================

static Standard_Boolean findPDWADandExcludeExcess (Handle(StepAP214_AppliedDocumentReference)& ADR,
                                                   TColStd_SequenceOfTransient& aSeqOfPDWAD,
                                                   const Interface_Graph& Graph,
                                                   Handle(StepBasic_ProductDefinitionWithAssociatedDocuments)& aPDWAD)
{
  // WARNING! do not add check for aSeqOfPDWAD.Length() and exit if it < 1,
  // because this methods invokes with an empty sequence too to find PDWAD by ADR
  Interface_EntityIterator subsADR = Graph.Shareds(ADR);
  for ( subsADR.Start(); subsADR.More(); subsADR.Next() ) {
    if ( !subsADR.Value()->IsKind (STANDARD_TYPE(StepBasic_Document)) )
      continue;
    Handle(StepBasic_Document) aDoc = Handle(StepBasic_Document)::DownCast(subsADR.Value());
    // looking for Document Product Equivalence
    Interface_EntityIterator subsD = Graph.Sharings(aDoc);
    for ( subsD.Start(); subsD.More(); subsD.Next() ) {
      if ( !subsD.Value()->IsKind (STANDARD_TYPE(StepBasic_DocumentProductEquivalence)) )
        continue;
      Handle(StepBasic_DocumentProductEquivalence) aDPE =
        Handle(StepBasic_DocumentProductEquivalence)::DownCast(subsD.Value());
      // take PDF and search the same PDF by PDWAD chain
      Interface_EntityIterator subsDPE = Graph.Shareds(aDPE);
      for ( subsDPE.Start(); subsDPE.More(); subsDPE.Next() ) {
        if ( !subsDPE.Value()->IsKind (STANDARD_TYPE(StepBasic_ProductDefinitionFormation)) )
          continue;
        Handle(StepBasic_ProductDefinitionFormation) aPDF =
          Handle(StepBasic_ProductDefinitionFormation)::DownCast(subsDPE.Value());
        Interface_EntityIterator subs = Graph.Sharings(aPDF);
        for ( subs.Start(); subs.More(); subs.Next() ) {
          if ( !subs.Value()->IsKind (STANDARD_TYPE(StepBasic_ProductDefinitionWithAssociatedDocuments)) )
            continue;
          aPDWAD = Handle(StepBasic_ProductDefinitionWithAssociatedDocuments)::DownCast(subs.Value());
        }
        // now searching for PDWAD that refer to the same PDF
        for (Standard_Integer pdwadi = 1; pdwadi <= aSeqOfPDWAD.Length(); pdwadi++) {
          Handle(StepBasic_ProductDefinitionWithAssociatedDocuments) aCurPDWAD =
            Handle(StepBasic_ProductDefinitionWithAssociatedDocuments)::DownCast(aSeqOfPDWAD(pdwadi));
          if ( !aCurPDWAD.IsNull() && aPDWAD == aCurPDWAD ) {
            // found the same Product Definition Formation
            aSeqOfPDWAD.Remove( pdwadi );
            return Standard_True;
          }
        }
      } // end of looking for PDF by ADR chain
    } // end of looking for DPE
  } // end iterations on Shareds(ADR)
  return Standard_False;
}


Standard_Boolean STEPConstruct_ExternRefs::LoadExternRefs ()
{
  // iterate on entities in the model and find AEIAs
  // or PDWADs (for AP203)
  Handle(Interface_InterfaceModel) model = Model();
  Handle(Standard_Type) tADR = STANDARD_TYPE(StepAP214_AppliedDocumentReference);
  Handle(Standard_Type) tPDWAD = STANDARD_TYPE(StepBasic_ProductDefinitionWithAssociatedDocuments);
  Standard_Integer nb = model->NbEntities();
  
  // PTV 28.01.2003 CAX-IF TRJ11, file ext_ref_master.stp 
  // search all ADR and PDWAD and exclude excess PDWADs
  TColStd_SequenceOfTransient aSeqOfADR, aSeqOfPDWAD;
  for (Standard_Integer ient = 1; ient <= nb; ient ++) {
    Handle(Standard_Transient) enti = model->Value(ient);
    if ( enti->DynamicType() == tPDWAD )
      aSeqOfPDWAD.Append( enti );
    else if ( enti->DynamicType() == tADR )
      aSeqOfADR.Append( enti );
  }
  Standard_Integer IsAP214 = 0;
  // run on sequence aSeqOfADR of ADR and remove excess PDWAD from aSeqOfPDWAD
  for (Standard_Integer adri = 1; adri <= aSeqOfADR.Length(); adri++) {
    Handle(StepAP214_AppliedDocumentReference) ADR = 
      Handle(StepAP214_AppliedDocumentReference)::DownCast(aSeqOfADR.Value(adri));
    // looking for Product Definition Formation and exlude excess PDWAD from aSeqOfPDWAD
    Handle(StepBasic_ProductDefinitionWithAssociatedDocuments) aPDWAD;    
    findPDWADandExcludeExcess( ADR, aSeqOfPDWAD, Graph(), aPDWAD );
    
    // now add all necessary information as original implementation.
    IsAP214 = 1;
    Handle(StepBasic_RoleAssociation) Role;
    Handle(StepBasic_ProductDefinition) Shape;
    Handle(StepRepr_PropertyDefinitionRepresentation) Format;
    Handle(StepBasic_DocumentRepresentationType) Type;
    // AppliedDocumentReference with RoleAssociation...
    Interface_EntityIterator subs4 = Graph().Sharings(ADR);
    for (subs4.Start(); subs4.More(); subs4.Next()) {
      if ( subs4.Value()->IsKind ( STANDARD_TYPE(StepBasic_RoleAssociation) ) )
        Role = Handle(StepBasic_RoleAssociation)::DownCast(subs4.Value());
    }
    
    subs4 = Graph().Shareds(ADR);
    for (subs4.Start(); subs4.More(); subs4.Next()) {
      if ( subs4.Value()->IsKind ( STANDARD_TYPE(StepBasic_ProductDefinition) ) )
        Shape = Handle(StepBasic_ProductDefinition)::DownCast(subs4.Value());
    }
    // search for Document file
    Handle(StepBasic_DocumentFile) DocFile;
    if ( aPDWAD.IsNull() ) { // shoudnot be begin from TRJ11
      // lookinf from ADR
      subs4 = Graph().Shareds(ADR);
    } else 
      // looking from PDWAD
      subs4 = Graph().Shareds(aPDWAD);
    
    for (subs4.Start(); subs4.More(); subs4.Next()) {
      if ( !subs4.Value()->IsKind ( STANDARD_TYPE(StepBasic_DocumentFile) ) )
        continue;
      DocFile = Handle(StepBasic_DocumentFile)::DownCast(subs4.Value());
      if ( DocFile.IsNull() )
        continue;
      // for each DocumentFile, find associated with it data:
      Interface_EntityIterator subs = Graph().Sharings(DocFile);
      for (subs.Start(); subs.More(); subs.Next()) {
        Handle(Standard_Transient) sub = subs.Value();
        
        // FORMAT - ???????
        //
        // PDRs of a shape and of a file format
        if ( sub->IsKind ( STANDARD_TYPE(StepRepr_PropertyDefinition) ) ) {
          Handle(StepRepr_PropertyDefinition) PD = Handle(StepRepr_PropertyDefinition)::DownCast(sub);
          Interface_EntityIterator subs2 = Graph().Sharings(PD);
          for (subs2.Start(); subs2.More(); subs2.Next()) {
            Handle(StepRepr_PropertyDefinitionRepresentation) PDR =
              Handle(StepRepr_PropertyDefinitionRepresentation)::DownCast(subs2.Value());
            if ( PDR.IsNull() ) continue;
            if (  PDR->UsedRepresentation()->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation)) )
              Format = PDR;
          }
        }
        // DocumentRepresentationType
        if ( sub->IsKind ( STANDARD_TYPE(StepBasic_DocumentRepresentationType) ) ) {
          Type = Handle(StepBasic_DocumentRepresentationType)::DownCast(sub);
        }
        if ( !Type.IsNull() && !Format.IsNull() )
          break;
      }
      if ( !Type.IsNull() && !Format.IsNull() )
        break;
    }
    if ( DocFile.IsNull() )
      continue;
    myAEIAs.Append ( ADR );
    myRoles.Append ( Role );
    myFormats.Append ( Format );
    myShapes.Append ( Shape );
    myTypes.Append ( Type );
    myIsAP214.Append ( IsAP214 );
    myDocFiles.Append( DocFile );
  } // end iterations on aSeqOfADR
  
  // now iterates on sequence aSeqOfPDWAD of Product Definition With Associated Documents
  for (Standard_Integer pdwadi = 1; pdwadi <= aSeqOfPDWAD.Length(); pdwadi++) {
    IsAP214 = 0;
    Handle(StepBasic_ProductDefinitionWithAssociatedDocuments) aPDWAD =
      Handle(StepBasic_ProductDefinitionWithAssociatedDocuments)::DownCast(aSeqOfPDWAD(pdwadi));
    myShapes.Append(aPDWAD);
    myIsAP214.Append ( IsAP214 );
    Handle(StepAP214_AppliedExternalIdentificationAssignment) AEIA;
    Handle(StepBasic_RoleAssociation) Role;
    Handle(StepRepr_PropertyDefinitionRepresentation) Format;
    Handle(StepBasic_DocumentRepresentationType) Type;
    Handle(StepBasic_DocumentFile) DocFile;
    myAEIAs.Append ( AEIA );
    myRoles.Append ( Role );
    myFormats.Append ( Format );
    myTypes.Append ( Type );
    myDocFiles.Append( DocFile );
  }
  
  return myShapes.Length() >0;
}

//=======================================================================
//function : NbExternRefs
//purpose  : 
//=======================================================================

Standard_Integer STEPConstruct_ExternRefs::NbExternRefs () const
{
  return myShapes.Length();
}

//=======================================================================
//function : FileName
//purpose  : 
//=======================================================================

Standard_CString STEPConstruct_ExternRefs::FileName (const Standard_Integer num) const
{
  Handle(StepBasic_DocumentFile) DocFile;
  Handle(StepAP214_AppliedExternalIdentificationAssignment) AEIA;
  Standard_CString aCStringFileName = 0;
  if ( myDocFiles.Length() >= num && !myDocFiles.Value(num).IsNull() )
    DocFile = Handle(StepBasic_DocumentFile)::DownCast(myDocFiles.Value( num ));
  else if (myIsAP214(num)==1)
  {
    Handle(StepAP214_AppliedDocumentReference) ADR = 
      Handle(StepAP214_AppliedDocumentReference)::DownCast ( myAEIAs(num) );

    // PTV 28.01.2003 CAX-IF TRJ11, file ext_ref_master.stp 
    // serach document file name by long chain ADR->D<-DPE->PDF<-PDWAD->DF
    Handle(StepBasic_ProductDefinitionWithAssociatedDocuments) aPDWAD;
    // create an empty aSeqOfPDWAD
    TColStd_SequenceOfTransient aSeqOfPDWAD; 
    // we do not need to exclude, just find PDWAD
    findPDWADandExcludeExcess( ADR, aSeqOfPDWAD, Graph(), aPDWAD );
    
    // search for Document file
    Interface_EntityIterator subs4;
    if ( aPDWAD.IsNull() ) { // shoudnot be begin from TRJ11
      // lookinf from ADR
      subs4 = Graph().Shareds(ADR);
    } else 
      // looking from PDWAD
      subs4 = Graph().Shareds(aPDWAD);
    for (subs4.Start(); subs4.More(); subs4.Next()) {
      if ( !subs4.Value()->IsKind ( STANDARD_TYPE(StepBasic_DocumentFile) ) )
        continue;
      DocFile = Handle(StepBasic_DocumentFile)::DownCast(subs4.Value());
      if ( DocFile.IsNull() ) continue;
    }
  }
  else  {
    Handle(StepBasic_ProductDefinitionWithAssociatedDocuments) aPDWAD =
      Handle(StepBasic_ProductDefinitionWithAssociatedDocuments)::DownCast(myShapes(num));
    if ( aPDWAD.IsNull() || aPDWAD->DocIds().IsNull() )
      return "";
    Standard_Integer i;
    for ( i=1; i <= aPDWAD->NbDocIds(); i++ ) {
      Handle(StepBasic_Document) Doc = aPDWAD->DocIdsValue(i);
      Handle(TCollection_HAsciiString) aFilename = Doc->Name();
      if (!aFilename.IsNull() && !aFilename->IsEmpty()) return aFilename->ToCString();
    }
    return "";
  }
  // take name from AEIA and from DF  
  if(!DocFile.IsNull()) {
    Interface_EntityIterator subs3 = Graph().Sharings(DocFile);
    for (subs3.Start(); subs3.More(); subs3.Next()) {
      if (subs3.Value()->IsKind(STANDARD_TYPE(StepAP214_AppliedExternalIdentificationAssignment))) {
        AEIA = Handle(StepAP214_AppliedExternalIdentificationAssignment)::DownCast(subs3.Value());
        if (!AEIA.IsNull())
          break;
      }
    }
  }
  if(!AEIA.IsNull()) {
    Handle(TCollection_HAsciiString) aFilename;
    aFilename = AEIA->AssignedId();
    if (!aFilename.IsNull() && !aFilename->IsEmpty()) {
      aCStringFileName = aFilename->ToCString();
      // ptv 29.01.2003 file trj4_xr1-tc-214.stp entity #71 have id "#71"
      if ( aCStringFileName && aCStringFileName[0] == '#')
        aCStringFileName = 0;
    }
    if ( ! aCStringFileName || ! aCStringFileName[0] ) {
      // try to take name from external source 
      Handle(StepBasic_ExternalSource) theSource = AEIA->Source();
      if (!theSource.IsNull()) {
        StepBasic_SourceItem theSourceId = theSource->SourceId();
        if (!theSourceId.IsNull()) {
          Handle(StepData_SelectNamed) theFileName;
          theFileName = Handle(StepData_SelectNamed)::DownCast (theSourceId.Value());
          if (theFileName.IsNull() || theFileName->Kind()!=6 ) {
            // nothing to do, hope could take name later.
          }
          else
            aCStringFileName = theFileName->String();
        }
      }
    }
  }
  Standard_CString oldFileName = 0;
  // compute true path to the extern file
  OSD_Path mainfile(WS()->LoadedFile());
  mainfile.SetName("");
  mainfile.SetExtension("");
  TCollection_AsciiString dpath;
  mainfile.SystemName(dpath);
  if (aCStringFileName && aCStringFileName[0]) {
    TCollection_AsciiString fullname = OSD_Path::AbsolutePath(dpath, aCStringFileName);
    if (fullname.Length() <= 0) fullname = aCStringFileName;
    if (!OSD_File(fullname).Exists()) {
      oldFileName = aCStringFileName;
      aCStringFileName = 0;
    }
  }
  if (!aCStringFileName || !aCStringFileName[0]) {
    // try to find name of the directory from DocFile
    if ( !DocFile.IsNull() ) {
      Handle(TCollection_HAsciiString) aFilename = DocFile->Id();
      if (!aFilename.IsNull() && !aFilename->IsEmpty())
        aCStringFileName = aFilename->ToCString();
      if ( ! aCStringFileName || ! aCStringFileName[0] ) {
        aFilename = DocFile->Name();
      if (!aFilename.IsNull() && !aFilename->IsEmpty())
        aCStringFileName = aFilename->ToCString();
      }
      if ( ! aCStringFileName || ! aCStringFileName[0] ) {
        if (oldFileName) {
          aCStringFileName = oldFileName;
        }
        else {
          return "";
        }
      }
    }
  }
  TCollection_AsciiString fullname = OSD_Path::AbsolutePath(dpath, aCStringFileName);
  if (fullname.Length() <= 0) fullname = aCStringFileName;
  if (!OSD_File(fullname).Exists()) {
    if (oldFileName) {
      aCStringFileName = oldFileName;
    }
    Handle(Transfer_TransientProcess) aTP = WS()->TransferReader()->TransientProcess();
    TCollection_AsciiString aMess("Can not read external file ");
    aMess.AssignCat(aCStringFileName);
    aTP->AddFail(DocFile, aMess.ToCString());
  }
  else {
    if (oldFileName && strcmp(oldFileName, aCStringFileName) != 0) {
      Handle(Transfer_TransientProcess) aTP = WS()->TransferReader()->TransientProcess();
      TCollection_AsciiString aMess("External file with name from entity AEIA (");
      aMess.AssignCat(oldFileName);
      aMess.AssignCat(") not existed => use file name from DocumentFile entity - ");
      aMess.AssignCat(aCStringFileName);
      aTP->AddWarning(DocFile, aMess.ToCString());
    }
  }
  return aCStringFileName;
}

//=======================================================================
//function : ProdDef
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinition) STEPConstruct_ExternRefs::ProdDef (const Standard_Integer num) const
{
  return Handle(StepBasic_ProductDefinition)::DownCast( myShapes(num) );
}

//=======================================================================
//function : DocFile
//purpose  : 
//=======================================================================

Handle(StepBasic_DocumentFile) STEPConstruct_ExternRefs::DocFile(const Standard_Integer num) const
{
  return Handle(StepBasic_DocumentFile)::DownCast(myDocFiles.Value(num));
}

//=======================================================================
//function : Format
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) STEPConstruct_ExternRefs::Format (const Standard_Integer num) const
{
  Handle(TCollection_HAsciiString) Format;
  
  if (myIsAP214(num)==0) return Format;
  
  Handle(StepRepr_PropertyDefinitionRepresentation) PDR =
    Handle(StepRepr_PropertyDefinitionRepresentation)::DownCast ( myFormats(num) );
  if (PDR.IsNull()) return Format;
  
  Handle(StepRepr_Representation) rep = PDR->UsedRepresentation();
  for ( Standard_Integer i=1; i <= rep->NbItems(); i++ ) {
    if ( rep->ItemsValue(i)->IsKind ( STANDARD_TYPE(StepRepr_DescriptiveRepresentationItem) ) ) {
      Handle(StepRepr_DescriptiveRepresentationItem) DRI =
        Handle(StepRepr_DescriptiveRepresentationItem)::DownCast ( rep->ItemsValue(i) );
      Format = DRI->Description();
      break;
    }
  }
  
  return Format;
}

//=======================================================================
//function : AddExternRef
//purpose  : 
//=======================================================================

Standard_Integer STEPConstruct_ExternRefs::AddExternRef (const Standard_CString filename,
                                                         const Handle(StepBasic_ProductDefinition) &PD,
                                                         const Standard_CString format)
{
  Handle(TCollection_HAsciiString) EmptyString = new TCollection_HAsciiString("");
  Handle(TCollection_HAsciiString) fmt = new TCollection_HAsciiString(format);
  Handle(TCollection_HAsciiString) tmp = new TCollection_HAsciiString("203");
  Standard_Integer np = fmt->Location(tmp,1,fmt->Length());

//  if( !(fmt==tmp) ) {
  if( !(np>0) ) {
  
    // create core entity DocumentFile
    Handle(StepBasic_DocumentType) DT = new StepBasic_DocumentType;
    DT->Init(EmptyString);
    Handle(TCollection_HAsciiString) DFid = new TCollection_HAsciiString(filename);
    // PTV 30.01.2003 TRJ11 -  copy external filename as is
//     DFid->AssignCat ( " file id" );
    Handle(StepBasic_DocumentFile) DF = new StepBasic_DocumentFile;
    DF->Init(DFid, EmptyString, Standard_False, EmptyString, DT, EmptyString, Standard_False, EmptyString);
  
    // create AppliedExternalIdentificationAssignment et al
    Handle(StepBasic_IdentificationRole) IR = new StepBasic_IdentificationRole;
    // PTV 30.01.2003 TRJ11 
    //    - set the ("external document id and location", $) without unmeaning description
    Handle(TCollection_HAsciiString) aName = 
      new TCollection_HAsciiString("external document id and location");
    IR->SetName( aName );
//     Handle(TCollection_HAsciiString) aIRdescr = new TCollection_HAsciiString("source system");
//     IR->Init(aName, Standard_True, aIRdescr);
      
    Handle(StepData_SelectNamed) SDS = new StepData_SelectNamed;
    SDS->SetString ( filename );
    SDS->SetName("IDENTIFIER");
    StepBasic_SourceItem SID;
    SID.SetValue(SDS);
    Handle(StepBasic_ExternalSource) ES = new StepBasic_ExternalSource;
    ES->Init(SID);
    
    StepAP214_ExternalIdentificationItem Item;
    Item.SetValue(DF);
    Handle(StepAP214_HArray1OfExternalIdentificationItem) Items = 
      new StepAP214_HArray1OfExternalIdentificationItem(1,1);
    Items->SetValue(1, Item);

    Handle(StepAP214_AppliedExternalIdentificationAssignment) ExtIdent = 
      new StepAP214_AppliedExternalIdentificationAssignment;
//     ExtIdent->Init(EmptyString, IR, ES, Items);
    // PTV 30.01.2003 TRJ11 - store filename in AEIA
    Handle(TCollection_HAsciiString) aFName = new TCollection_HAsciiString(filename);
    ExtIdent->Init(aFName, IR, ES, Items);
    // create DocumentRepresentationType
    Handle(TCollection_HAsciiString) Dig = new TCollection_HAsciiString("digital");
    Handle(StepBasic_DocumentRepresentationType) Type = new StepBasic_DocumentRepresentationType;
    Type->Init(Dig, DF);
    
    // create AppliedDocumentReference, 
    Handle(StepAP214_AppliedDocumentReference) ADR = new StepAP214_AppliedDocumentReference;
    // PTV 30.01.2003 TRJ11 - create additional entities for AP214
    addAP214ExterRef( ADR, PD, DF, filename );
    
    // create RoleAssociation etc.
    Handle(StepBasic_ObjectRole) OR = new StepBasic_ObjectRole;
    Handle(TCollection_HAsciiString) mandatory = new TCollection_HAsciiString("mandatory");
    OR->Init(mandatory, Standard_False, EmptyString);
    StepBasic_RoleSelect RS;
    RS.SetValue(ADR);
    Handle(StepBasic_RoleAssociation) Role = new StepBasic_RoleAssociation;
    Role->Init(OR, RS);
      
    // create PDR for association with SR
    StepRepr_CharacterizedDefinition CD; 
    CD.SetValue(DF);
    Handle(TCollection_HAsciiString) PDname = new TCollection_HAsciiString("external definition");
    Handle(StepRepr_PropertyDefinition) PropD = new StepRepr_PropertyDefinition;
    PropD->Init(PDname, Standard_True, EmptyString, CD);
    StepRepr_RepresentedDefinition RD;
    RD.SetValue(PropD);
//    Handle(StepRepr_PropertyDefinitionRepresentation) PDRshape = new StepRepr_PropertyDefinitionRepresentation;
//    PDRshape->Init ( RD, SDR->UsedRepresentation() );
  
    // create PDR for definition of document format (if defined)
    Handle(StepRepr_PropertyDefinitionRepresentation) PDRformat;
    if ( format && format[0] ) {

      Handle(TCollection_HAsciiString) RCftype = new TCollection_HAsciiString ( "document parameters" );
      Handle(StepRepr_RepresentationContext) RCf = new StepRepr_RepresentationContext;
      RCf->Init ( EmptyString, RCftype );
      
      Handle(TCollection_HAsciiString) DRIname = new TCollection_HAsciiString ( "data format" );
      Handle(TCollection_HAsciiString) DRIdscr = new TCollection_HAsciiString ( format );
      Handle(StepRepr_DescriptiveRepresentationItem) DRI = new StepRepr_DescriptiveRepresentationItem;
      DRI->Init ( DRIname, DRIdscr );
      Handle(StepRepr_HArray1OfRepresentationItem) fItems = new StepRepr_HArray1OfRepresentationItem(1,1);
      fItems->SetValue ( 1, DRI );
    
      Handle(TCollection_HAsciiString) SRfname = new TCollection_HAsciiString ( "document format" );
      Handle(StepRepr_Representation) SRformat = new StepRepr_Representation;
      SRformat->Init(SRfname, fItems, RCf);
    
      StepRepr_CharacterizedDefinition CDf; 
      CDf.SetValue(DF);
      Handle(TCollection_HAsciiString) PDfname = new TCollection_HAsciiString("document property");
      Handle(StepRepr_PropertyDefinition) PDf = new StepRepr_PropertyDefinition;
      PDf->Init(PDfname, Standard_True, EmptyString, CDf);
      StepRepr_RepresentedDefinition RDf;
      RDf.SetValue(PDf);
      
      PDRformat = new StepRepr_PropertyDefinitionRepresentation;
      PDRformat->Init ( RDf, SRformat );
    }
  
    // add all the created root entities to sequences
    myAEIAs.Append ( ExtIdent );     //StepAP214_AppliedExternalIdentificationAssignment
    myRoles.Append ( Role );         //StepBasic_RoleAssociation
    myFormats.Append ( PDRformat );  //StepRepr_PropertyDefinitionRepresentation
//    myShapes.Append ( PDRshape );    //StepRepr_PropertyDefinitionRepresentation
    myShapes.Append ( PD );          //StepBasic_ProductDefinition
    myTypes.Append ( Type );         //StepBasic_DocumentRepresentationType

  }

  else { // format=="AP203"
    
//    StepRepr_RepresentedDefinition aRD = SDR->Definition();
//    Handle(StepRepr_PropertyDefinition) aPD = aRD.PropertyDefinition();
//    StepRepr_CharacterizedDefinition aCD = aPD->Definition();
//    Handle(StepBasic_ProductDefinition) aProdDef = aCD.ProductDefinition();
    Handle(StepBasic_ProductDefinitionFormation) ProdDefForm = PD->Formation();
    Handle(StepBasic_ProductDefinitionContext) ProdDefCont = PD->FrameOfReference();

    // create document
    Handle(TCollection_HAsciiString) fname = new TCollection_HAsciiString(filename);
    Handle(StepBasic_DocumentType) aDocType = new StepBasic_DocumentType;
    Handle(TCollection_HAsciiString) aDT = new TCollection_HAsciiString("cad_filename");
    aDocType->Init(aDT);
    Handle(StepBasic_Document) aDoc = new StepBasic_Document;
    Handle(TCollection_HAsciiString) aDescription = 
      new TCollection_HAsciiString("CAD Model associated to the part");
    aDoc->Init(EmptyString,fname,Standard_True,aDescription,aDocType);
    Handle(StepBasic_HArray1OfDocument) aDocIds = new StepBasic_HArray1OfDocument(1,1);
    aDocIds->SetValue(1,aDoc);
    
    // create ProductDefinitionWithAssociatedDocuments
    aDescription = PD->Description();
    Handle(StepBasic_ProductDefinitionWithAssociatedDocuments) PDWAD = 
      new StepBasic_ProductDefinitionWithAssociatedDocuments;
    PDWAD->Init(EmptyString,aDescription,ProdDefForm,ProdDefCont,aDocIds);
    //Handle(StepBasic_ProductDefinitionWithAssociatedDocuments) PDWAD = 
    //  Handle(StepBasic_ProductDefinitionWithAssociatedDocuments)::DownCast(PD);
    
    // searh in graph for replace
//    Standard_Integer numProdDef;
//    Interface_EntityIterator subs = Graph().Shareds(SDR);
//    for (subs.Start(); subs.More(); subs.Next()) {
//      Handle(Standard_Transient) sub = subs.Value();
    Interface_EntityIterator subs = Graph().Sharings(PD);
    for (subs.Start(); subs.More(); subs.Next()) {
      Handle(Standard_Transient) sub = subs.Value();
      if (!sub->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape))) continue;
      Handle(StepRepr_ProductDefinitionShape) ProdDefSh = 
        Handle(StepRepr_ProductDefinitionShape)::DownCast ( sub );
      if(ProdDefSh.IsNull()) continue;
      StepRepr_CharacterizedDefinition CDf;
      CDf.SetValue(PDWAD);
      ProdDefSh->SetDefinition(CDf);
    }

//      Interface_EntityIterator subs1 = Graph().Shareds(ProdDefSh);
//      for (subs1.Start(); subs1.More(); subs1.Next()) {

//        Handle(Standard_Transient) sub1 = subs1.Value();
//        if (!sub1->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) continue;
//        Handle(StepBasic_ProductDefinition) ProdDef = 
//          Handle(StepBasic_ProductDefinition)::DownCast ( sub1 );
//        numProdDef = Model()->Number(ProdDef);
    Standard_Integer numProdDef = Model()->Number(PD);

//        Interface_EntityIterator subs2 = Graph().Sharings(ProdDef);
    Interface_EntityIterator subs2 = Graph().Sharings(PD);
    for (subs2.Start(); subs2.More(); subs2.Next()) {
      Handle(Standard_Transient) sub2 = subs2.Value();

      if (sub2->IsKind(STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence))) {
        Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO = 
          Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast ( sub2 );
        NAUO->SetRelatedProductDefinition(PDWAD);
      }

      if (sub2->IsKind(STANDARD_TYPE(StepAP203_CcDesignPersonAndOrganizationAssignment))) {
        Handle(StepAP203_CcDesignPersonAndOrganizationAssignment) CDPAOA = 
          Handle(StepAP203_CcDesignPersonAndOrganizationAssignment)::DownCast ( sub2 );
        Handle(StepAP203_HArray1OfPersonOrganizationItem) HAPOI = CDPAOA->Items();
        for(Standard_Integer i=1; i<=HAPOI->Length(); i++) {
          StepAP203_PersonOrganizationItem POI = HAPOI->Value(i);
          Handle(StepBasic_ProductDefinition) PDtmp = POI.ProductDefinition();
          Standard_Integer numPDtmp = Model()->Number(PDtmp);
          if(numProdDef==numPDtmp) {
            POI.SetValue(PDWAD);
            HAPOI->SetValue(i,POI);
          }
        }
      }

      if (sub2->IsKind(STANDARD_TYPE(StepAP203_CcDesignDateAndTimeAssignment))) {
        Handle(StepAP203_CcDesignDateAndTimeAssignment) CDDATA = 
          Handle(StepAP203_CcDesignDateAndTimeAssignment)::DownCast ( sub2 );
        Handle(StepAP203_HArray1OfDateTimeItem) HADTI = CDDATA->Items();
        for(Standard_Integer i=1; i<=HADTI->Length(); i++) {
          StepAP203_DateTimeItem DTI = HADTI->Value(i);
          Handle(StepBasic_ProductDefinition) PDtmp = DTI.ProductDefinition();
          Standard_Integer numPDtmp = Model()->Number(PDtmp);
          if(numProdDef==numPDtmp) {
            DTI.SetValue(PDWAD);
            HADTI->SetValue(i,DTI);
          }
        }
      }

      if (sub2->IsKind(STANDARD_TYPE(StepAP203_CcDesignApproval))) {
        Handle(StepAP203_CcDesignApproval) CDA = 
          Handle(StepAP203_CcDesignApproval)::DownCast ( sub2 );
        Handle(StepAP203_HArray1OfApprovedItem) HAAI = CDA->Items();
        for(Standard_Integer i=1; i<=HAAI->Length(); i++) {
          StepAP203_ApprovedItem AI = HAAI->Value(i);
          Handle(StepBasic_ProductDefinition) PDtmp = AI.ProductDefinition();
          Standard_Integer numPDtmp = Model()->Number(PDtmp);
          if(numProdDef==numPDtmp) {
            AI.SetValue(PDWAD);
            HAAI->SetValue(i,AI);
          }
        }
      }
    }
//      }
//      
//      StepRepr_CharacterizedDefinition ChartDef;
//      ChartDef.SetValue(PDWAD);
//      ProdDefSh->SetDefinition(ChartDef);
//    }
             
    myAEIAs.Append ( PDWAD );
    myReplaceNum.Append(numProdDef);
    myRoles.Append ( aDoc );
    myTypes.Append ( aDocType );

  }

  return myAEIAs.Length();

}

//=======================================================================
//function : WriteExternRefs
//purpose  : 
//=======================================================================

Standard_Integer STEPConstruct_ExternRefs::WriteExternRefs (const Standard_Integer num) const
{
  if(num==3) {
    for ( Standard_Integer i=1; i <= myAEIAs.Length(); i++ ) {
      Model()->ReplaceEntity(myReplaceNum(i),myAEIAs(i));
      if ( ! myRoles(i).IsNull() ) 
        Model()->AddWithRefs ( myRoles(i) );
      if ( ! myTypes(i).IsNull() ) 
        Model()->AddWithRefs ( myTypes(i) );
    }
  }
  else {
    for ( Standard_Integer i=1; i <= myAEIAs.Length(); i++ ) {
      Model()->AddWithRefs ( myAEIAs(i) );
      if ( ! myRoles(i).IsNull() ) 
        Model()->AddWithRefs ( myRoles(i) );
      if ( ! myFormats(i).IsNull() ) 
        Model()->AddWithRefs ( myFormats(i) );
      if ( ! myShapes(i).IsNull() ) 
        Model()->AddWithRefs ( myShapes(i) );
      if ( ! myTypes(i).IsNull() ) 
        Model()->AddWithRefs ( myTypes(i) );
    }
  }
  // PTV 30.01.2003 TRJ11
  if ( !myAPD.IsNull() )
    Model()->AddWithRefs( myAPD );
  if ( !mySharedPRPC.IsNull() )
    Model()->AddWithRefs( mySharedPRPC );
  
  return myAEIAs.Length();
}

//=======================================================================
//function : addAP214ExterRef
//purpose  : PTV 30.01.2003 TRJ11
//=======================================================================

Standard_Boolean STEPConstruct_ExternRefs::addAP214ExterRef (const Handle(StepAP214_AppliedDocumentReference)& ADR,
                                                             const Handle(StepBasic_ProductDefinition)& PD,
                                                             const Handle(StepBasic_DocumentFile)& DF,
                                                             const Standard_CString filename )
{
  Handle(StepAP214_HArray1OfDocumentReferenceItem) DRIs = new StepAP214_HArray1OfDocumentReferenceItem(1,1);
  StepAP214_DocumentReferenceItem aDRI;
  aDRI.SetValue(PD);
  DRIs->SetValue(1, aDRI);
  Handle(TCollection_HAsciiString) EmptyString = new TCollection_HAsciiString("");

  // create/get created shared entities: 
  // DocumentType, ProductDefinitionContext, ProductRelatedProductCategory, ProductContext
  checkAP214Shared();
  
  // create document
  Handle(StepBasic_Document) aDocument = new StepBasic_Document;
  aDocument->Init( EmptyString, EmptyString, Standard_False, EmptyString, mySharedDocType );
  ADR->Init(aDocument, EmptyString, DRIs);
  
  // create new product 
  Handle(StepBasic_Product) Product = new StepBasic_Product;
  Handle(StepBasic_HArray1OfProduct) HProducts = mySharedPRPC->Products();
  Standard_Integer nbProducts = 0;
  if (!HProducts.IsNull())
    nbProducts = HProducts->Length();
  Standard_Integer intProdId = 20001 + nbProducts;
  Handle(TCollection_HAsciiString) ProductID = new TCollection_HAsciiString( intProdId );
  Handle(TCollection_HAsciiString) ProductName = new TCollection_HAsciiString(filename);
  ProductName->AssignCat( "-Doc" );
  Handle(StepBasic_HArray1OfProductContext) aHProdContext = new StepBasic_HArray1OfProductContext(1, 1);
  aHProdContext->SetValue( 1, mySharedPC );
  Product->Init( ProductID, ProductName, EmptyString, aHProdContext );

  // create new product definition formation
  Handle(StepBasic_ProductDefinitionFormation) PDF = new StepBasic_ProductDefinitionFormation;
  // name id taked from exapmle Standard_ExtString_ref_master.stp
  Handle(TCollection_HAsciiString) PDF_ID = new TCollection_HAsciiString("1");
  PDF->Init( PDF_ID, EmptyString, Product );
  
  Handle(StepBasic_DocumentProductEquivalence) DPE = new StepBasic_DocumentProductEquivalence;
  Handle(TCollection_HAsciiString) DPEname = new TCollection_HAsciiString("equivalence");
  StepBasic_ProductOrFormationOrDefinition aPOFOD;
  aPOFOD.SetValue( PDF );
  DPE->Init( DPEname, Standard_False, EmptyString, aDocument, aPOFOD );
  // add to the model with references
  Model()->AddWithRefs( DPE );
   
  // add products to shared PRPC
  Handle(StepBasic_HArray1OfProduct) newHProducts = new StepBasic_HArray1OfProduct(1, nbProducts + 1);
  for (Standard_Integer pi = 1; pi <= nbProducts; pi++)
    newHProducts->SetValue( pi, HProducts->Value( pi ) );
  newHProducts->SetValue( nbProducts + 1, Product );
  // set the hArray to the PRPC
  mySharedPRPC->SetProducts( newHProducts );
  
  // create new PDWAD
  Handle(StepBasic_ProductDefinitionWithAssociatedDocuments) PDWAD =
    new StepBasic_ProductDefinitionWithAssociatedDocuments;
  Handle(StepBasic_HArray1OfDocument) aDocIds = new StepBasic_HArray1OfDocument(1,1);
  aDocIds->SetValue( 1, DF );
  Handle(TCollection_HAsciiString) PDWAD_ID = new TCollection_HAsciiString("1");
  PDWAD->Init( PDWAD_ID, EmptyString, PDF, mySharedPDC, aDocIds );
  // add to the model with references
  Model()->AddWithRefs( PDWAD );
  
  return Standard_True;
}

//=======================================================================
//function : SetAP214APD
//purpose  : 
//=======================================================================

void STEPConstruct_ExternRefs::SetAP214APD (const Handle(StepBasic_ApplicationProtocolDefinition)& APD)
{
  myAPD = APD;
}

//=======================================================================
//function : GetAP214APD
//purpose  : 
//=======================================================================

Handle(StepBasic_ApplicationProtocolDefinition) STEPConstruct_ExternRefs::GetAP214APD()
{
  if (myAPD.IsNull()) {
    // create new APD with new Application Context
    myAPD = new StepBasic_ApplicationProtocolDefinition;
    // examples of the values taken from ext_ref_master.stp
    Handle(TCollection_HAsciiString) status =
      new TCollection_HAsciiString("version 1.1");
    Handle(TCollection_HAsciiString) appSchemaName =
      new TCollection_HAsciiString("pdm_schema");
    Standard_Integer intProtocolYear = 1999;
    Handle(StepBasic_ApplicationContext) aApplication = new StepBasic_ApplicationContext;
    Handle(TCollection_HAsciiString) EmptyString = new TCollection_HAsciiString("");
    aApplication->Init( EmptyString );
    myAPD->Init( status, appSchemaName, intProtocolYear, aApplication );
  }
  return myAPD;
}

void STEPConstruct_ExternRefs::checkAP214Shared ()
{
  Handle(TCollection_HAsciiString) EmptyString = new TCollection_HAsciiString("");
  if ( mySharedPRPC.IsNull() ) {
    // create new ProductRelatedProductCategory for all extern files.
    Handle(TCollection_HAsciiString) PRPCname = new TCollection_HAsciiString("document");
    mySharedPRPC = new StepBasic_ProductRelatedProductCategory;
    mySharedPRPC->Init( PRPCname, Standard_False, EmptyString, 0 );
  }
  if ( mySharedDocType.IsNull() ) {
    // create new shared Document Type
    mySharedDocType = new StepBasic_DocumentType;
    Handle(TCollection_HAsciiString) prod_dat_type =
      new TCollection_HAsciiString("configuration controlled document version");
    mySharedDocType->Init( prod_dat_type );
  }
  if ( mySharedPDC.IsNull() ) {
    // create new shared Product Definition Context
    mySharedPDC = new StepBasic_ProductDefinitionContext;
    Handle(TCollection_HAsciiString) aPDCname =
      new TCollection_HAsciiString("digital document definition");
    Handle(StepBasic_ApplicationContext) anAppContext = GetAP214APD()->Application();
    mySharedPDC->Init( aPDCname, anAppContext, EmptyString );
  }
  if ( mySharedPC.IsNull() ) {
    // create new shared ProductContext
    mySharedPC = new StepBasic_ProductContext;
    Handle(StepBasic_ApplicationContext) anAppContext = GetAP214APD()->Application();
    mySharedPC->Init( EmptyString, anAppContext, EmptyString );
  }
  
}
