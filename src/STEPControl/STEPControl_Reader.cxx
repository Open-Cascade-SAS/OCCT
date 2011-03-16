#include <STEPControl_Reader.ixx>
#include <STEPControl_Controller.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferReader.hxx>
#include <Interface_ShareFlags.hxx>

#include <STEPControl_ActorRead.hxx>

#include <StepBasic_ProductDefinition.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_Static.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <TCollection_AsciiString.hxx>
#include <StepBasic_ProductDefinitionWithAssociatedDocuments.hxx>
#include <StepBasic_DocumentProductEquivalence.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_RepresentationRelationship.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepRepr_MappedItem.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

//=======================================================================
//function : STEPControl_Reader
//purpose  : 
//=======================================================================

STEPControl_Reader::STEPControl_Reader ()
{
  STEPControl_Controller::Init();
  SetNorm ("STEP");
}

//=======================================================================
//function : STEPControl_Reader
//purpose  : 
//=======================================================================

STEPControl_Reader::STEPControl_Reader
  (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch)
{
  STEPControl_Controller::Init();
  SetWS (WS,scratch);
  SetNorm ("STEP");
}

//=======================================================================
//function : StepModel
//purpose  : 
//=======================================================================

Handle(StepData_StepModel) STEPControl_Reader::StepModel () const
{
  return Handle(StepData_StepModel)::DownCast(Model());
}

//=======================================================================
//function : TransferRoot
//purpose  : 
//=======================================================================

Standard_Boolean STEPControl_Reader::TransferRoot (const Standard_Integer num)
{
  return TransferOneRoot (num);
}

//=======================================================================
//function : NbRootsForTransfer
//purpose  : 
//=======================================================================

Standard_Integer STEPControl_Reader::NbRootsForTransfer() 
{
  if (therootsta) return theroots.Length();
  therootsta = Standard_True;

  //theroots.Clear();
  Standard_Integer nb = Model()->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) ent = Model()->Value(i);
    if(ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) {
      // PTV 31.01.2003 TRJ11 exclude Product Definition With Associated Document from roots
      if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionWithAssociatedDocuments))) {
        // check if PDWAD-> PDF <-Document_Product_Equivalence.
        Standard_Boolean iSexclude = Standard_False;
        Handle(StepBasic_ProductDefinitionWithAssociatedDocuments) PDWAD =
          Handle(StepBasic_ProductDefinitionWithAssociatedDocuments)::DownCast(ent);
        Interface_EntityIterator PDWADsubs = WS()->Graph().Shareds(PDWAD);
        for (PDWADsubs.Start(); PDWADsubs.More(); PDWADsubs.Next()) {
          if ( !PDWADsubs.Value()->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation)))
            continue;
          Handle(StepBasic_ProductDefinitionFormation) localPDF = 
            Handle(StepBasic_ProductDefinitionFormation)::DownCast(PDWADsubs.Value());
          Interface_EntityIterator PDFsubs = WS()->Graph().Sharings(localPDF);
          for( PDFsubs.Start(); PDFsubs.More(); PDFsubs.Next() )
            if (PDFsubs.Value()->IsKind(STANDARD_TYPE(StepBasic_DocumentProductEquivalence))) {
              iSexclude = Standard_True;
              break;
            }
          if (iSexclude)
            break;
        }
        if (iSexclude) {
#ifdef DEB
          cout << "Warning: STEPControl_Reader::NbRootsForTransfer exclude PDWAD from roots" << endl;
#endif
          continue;
        }
      }
      Handle(StepBasic_ProductDefinition) PD = 
        Handle(StepBasic_ProductDefinition)::DownCast(ent);
      Standard_Boolean IsRoot = Standard_True;
      const Interface_Graph& graph = WS()->Graph();
      // determinate roots used NextAssemblyUsageOccurrence
      Interface_EntityIterator subs = graph.Sharings(PD);
      for(subs.Start(); subs.More(); subs.Next()) {
        Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO = 
          Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(subs.Value());
        if (NAUO.IsNull()) continue;
        if (PD==NAUO->RelatedProductDefinition()) IsRoot=Standard_False;
      }
      // determinate roots used ProductDefinitionContext
      if(IsRoot) {
        const char *str1 = Interface_Static::CVal("read.step.product.context");
        Standard_Integer ICS = Interface_Static::IVal("read.step.product.context");
        if(ICS>1) {
          subs = graph.Shareds(PD);
          for(subs.Start(); subs.More(); subs.Next()) {
            Handle(StepBasic_ProductDefinitionContext) PDC = 
              Handle(StepBasic_ProductDefinitionContext)::DownCast(subs.Value());
            if (PDC.IsNull()) continue;
            const char *str2 = PDC->LifeCycleStage()->String().ToCString();
            const char *str3 = PDC->Name()->String().ToCString();
            if( !( strcasecmp(str1,str2)==0 || strcasecmp(str1,str3)==0 ) )
              IsRoot=Standard_False;
          }
        }
      }
      // determinate roots used ProductDefinitionFormationRelationship
      //subs = graph.Shareds(PD);
      //for(subs.Start(); subs.More(); subs.Next()) {
      //  Handle(StepBasic_ProductDefinitionFormation) PDF = 
      //    Handle(StepBasic_ProductDefinitionFormation)::DownCast(subs.Value());
      //  if (PDF.IsNull()) continue;
      //  Interface_EntityIterator subs1 = graph.Sharings(PDF);
      //  for(subs1.Start(); subs1.More(); subs1.Next()) {
      //    Handle(StepBasic_ProductDefinitionFormationRelationship) PDFR = 
      //      Handle(StepBasic_ProductDefinitionFormationRelationship)::DownCast(subs1.Value());
      //    if (PDFR.IsNull()) continue;
      //    if (PDF==PDFR->RelatedProductDefinition()) IsRoot=Standard_False;
      //  }
      //}
      if (IsRoot) {
        theroots.Append(ent);
        WS()->MapReader()->RootsForTransfer()->Append(ent);
      }
    }
    TCollection_AsciiString aProdMode = Interface_Static::CVal("read.step.product.mode");
    if(!aProdMode.IsEqual("ON")) {
      if(ent->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation))) {
        Standard_Boolean IsRoot = Standard_True;
        Handle(StepShape_ShapeDefinitionRepresentation) SDR =
          Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(ent);
        Handle(StepRepr_PropertyDefinition) PropDef = SDR->Definition().PropertyDefinition();
        if(!PropDef.IsNull()) {
          Handle(StepBasic_ProductDefinition) PD = PropDef->Definition().ProductDefinition();
          if(!PD.IsNull()) IsRoot = Standard_False;
          if(IsRoot) {
            Handle(StepRepr_ShapeAspect) SA = PropDef->Definition().ShapeAspect();
            if(!SA.IsNull()) {
              Handle(StepRepr_ProductDefinitionShape) PDS = SA->OfShape();
              PD = PDS->Definition().ProductDefinition();
              if(!PD.IsNull()) IsRoot = Standard_False;
            }
          }
          if(IsRoot) {
            Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO =
              Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(PropDef->Definition().ProductDefinitionRelationship());
            if(!NAUO.IsNull()) IsRoot = Standard_False;
          }
          if(IsRoot) {
            Handle(StepShape_ShapeRepresentation) SR =
              Handle(StepShape_ShapeRepresentation)::DownCast(SDR->UsedRepresentation());
            if(SR.IsNull()) IsRoot = Standard_False;
          }
        }
        if(IsRoot) {
          theroots.Append(ent);
          WS()->MapReader()->RootsForTransfer()->Append(ent);
        }
      }
      if(ent->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation))) {
        Standard_Boolean IsRoot = Standard_True;
        Handle(StepShape_ShapeRepresentation) SR =
          Handle(StepShape_ShapeRepresentation)::DownCast(ent);
        const Interface_Graph& graph = WS()->Graph();
        Interface_EntityIterator subs = graph.Sharings(SR);
        for(subs.Start(); subs.More() && IsRoot; subs.Next()) {
          Handle(StepShape_ShapeDefinitionRepresentation) SDR = 
            Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs.Value());
          if(!SDR.IsNull()) IsRoot = Standard_False;
          if(IsRoot) {
            Handle(StepRepr_RepresentationRelationship) RR =
              Handle(StepRepr_RepresentationRelationship)::DownCast(subs.Value());
            if(!RR.IsNull()) {
              Handle(StepShape_ShapeRepresentation) SR2 =
                Handle(StepShape_ShapeRepresentation)::DownCast(RR->Rep1());
              if(SR==SR2)
                SR2 = Handle(StepShape_ShapeRepresentation)::DownCast(RR->Rep2());
              Interface_EntityIterator subs2 = graph.Sharings(SR2);
              for(subs2.Start(); subs2.More(); subs2.Next()) {
                Handle(StepShape_ShapeDefinitionRepresentation) SDR2 = 
                  Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs2.Value());
                if(!SDR2.IsNull()) IsRoot = Standard_False;
                //else {
                //  if(SR==SRR->Rep2()) IsRoot = Standard_False;
                //}
              }
            }
          }
          if(IsRoot) {
            Handle(StepRepr_RepresentationMap) RM =
              Handle(StepRepr_RepresentationMap)::DownCast(subs.Value());
            if(!RM.IsNull()) {
              Interface_EntityIterator subs2 = graph.Sharings(RM);
              for(subs2.Start(); subs2.More(); subs2.Next()) {
                Handle(StepRepr_MappedItem) MI = Handle(StepRepr_MappedItem)::DownCast(subs2.Value());
                if(!MI.IsNull()) {
                  Interface_EntityIterator subs3 = graph.Sharings(MI);
                  for(subs3.Start(); subs3.More(); subs3.Next()) {
                    Handle(StepShape_ShapeRepresentation) SR2 =
                      Handle(StepShape_ShapeRepresentation)::DownCast(subs3.Value());
                    if(!SR2.IsNull()) IsRoot = Standard_False;
                  }
                }
              }
            }
          }
        }
        if(IsRoot) {
          theroots.Append(ent);
          WS()->MapReader()->RootsForTransfer()->Append(ent);
        }
      }
    }

  }


  return theroots.Length();
}

