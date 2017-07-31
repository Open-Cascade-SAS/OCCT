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

//:k3 abv 25.11.98 rp1sd.stp
//:n4 abv 11.02.99 S4132: recognition of GeometricSet (instead of GeometricCurveSet)
//    gka 05.04.99 S4136: eliminate parameter lastpreci
//gka,abv 14.04.99 S4136: maintain unit context, precision and maxtolerance values

#include <BRep_Builder.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepCheck_Status.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>
#include <HeaderSection_FileName.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressSentry.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductRelatedProductCategory.hxx>
#include <STEPConstruct_Assembly.hxx>
#include <STEPConstruct_UnitContext.hxx>
#include <STEPControl_ActorRead.hxx>
#include <StepData_StepModel.hxx>
#include <StepDimTol_DatumFeature.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepGeom_Direction.hxx>
#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_HSequenceOfRepresentationItem.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepRepr_RepresentationRelationship.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepRepr_ShapeRepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_Transformation.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_EdgeBasedWireframeModel.hxx>
#include <StepShape_EdgeBasedWireframeShapeRepresentation.hxx>
#include <StepShape_FaceBasedSurfaceModel.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_FacetedBrepShapeRepresentation.hxx>
#include <StepShape_GeometricallyBoundedSurfaceShapeRepresentation.hxx>
#include <StepShape_GeometricallyBoundedWireframeShapeRepresentation.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_ManifoldSurfaceShapeRepresentation.hxx>
#include <StepShape_NonManifoldSurfaceShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepToGeom.hxx>
#include <StepToTopoDS_Builder.hxx>
#include <StepToTopoDS_DataMapOfTRI.hxx>
#include <StepToTopoDS_MakeTransformed.hxx>
#include <StepToTopoDS_Tool.hxx>
#include <StepToTopoDS_TranslateFace.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <UnitsMethods.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPControl_ActorRead,Transfer_ActorOfTransientProcess)

// #include <Interface_InterfaceModel.hxx>  // pour mise au point
// MappedItem :
// FaceSurface :
//  Unites :
//#include <StepBasic_UncertaintyMeasureWithUnit.hxx>
//  Representation Relationship & cie
// For non-manifold topology processing (ssv; 12.11.2010)
#define TRANSLOG

// ============================================================================
// Function: DumpWhatIs   
// Purpose: Use it in debug mode to dump your shapes
// ============================================================================

#ifdef OCCT_DEBUG
static void DumpWhatIs(const TopoDS_Shape& S) {

  TopTools_MapOfShape aMapOfShape;
  aMapOfShape.Add(S);
  TopTools_ListOfShape aListOfShape;
  aListOfShape.Append(S);
  TopTools_ListIteratorOfListOfShape itL(aListOfShape);
  Standard_Integer nbSolids = 0,
                   nbShells = 0,
                   nbOpenShells = 0,
                   nbFaces = 0, 
                   nbWires = 0, 
                   nbEdges = 0, 
                   nbVertexes = 0,
                   nbCompounds = 0;

  if (S.ShapeType() == TopAbs_COMPOUND)
    nbCompounds++;

  for( ; itL.More(); itL.Next() ) {
    TopoDS_Iterator it( itL.Value() );
    for ( ; it.More(); it.Next() ) {
      TopoDS_Shape aSubShape = it.Value();
      if ( !aMapOfShape.Add(aSubShape) )
        continue;
      aListOfShape.Append(aSubShape);
      if (aSubShape.ShapeType() == TopAbs_COMPOUND)
        nbCompounds++;
      if (aSubShape.ShapeType() == TopAbs_SOLID)
        nbSolids++;
      if (aSubShape.ShapeType() == TopAbs_SHELL) {
        if ( !aSubShape.Closed() )
          nbOpenShells++;
        nbShells++;
      }
      if (aSubShape.ShapeType() == TopAbs_FACE)
        nbFaces++;
      if (aSubShape.ShapeType() == TopAbs_WIRE)
        nbWires++;
      if (aSubShape.ShapeType() == TopAbs_EDGE)
        nbEdges++;
      if (aSubShape.ShapeType() == TopAbs_VERTEX)
        nbVertexes++;
    }
  }

  cout << "//What is?// NB COMPOUNDS: " << nbCompounds << endl;
  cout << "//What is?// NB SOLIDS: " << nbSolids << endl;
  cout << "//What is?// NB SHELLS: " << nbShells << endl;
  cout << "//What is?//    OPEN SHELLS: " << nbOpenShells << endl;
  cout << "//What is?//    CLOSED SHELLS: " << nbShells - nbOpenShells << endl;
  cout << "//What is?// NB FACES: " << nbFaces << endl;
  cout << "//What is?// NB WIRES: " << nbWires << endl;
  cout << "//What is?// NB EDGES: " << nbEdges << endl;
  cout << "//What is?// NB VERTEXES: " << nbVertexes << endl;
}
#endif

namespace {
  // Set global var to inform outer methods that current representation item is non-manifold.
  // The better way is to pass this information via binder or via TopoDS_Shape itself, however,
  // this is very specific info to do so...
  Standard_Boolean NM_DETECTED = Standard_False;
}

// ============================================================================
// Method  : STEPControl_ActorRead::STEPControl_ActorRead  ()    
// Purpose : Empty constructor
// ============================================================================

STEPControl_ActorRead::STEPControl_ActorRead() {}

// ============================================================================
// Method  : STEPControl_ActorRead::Recognize
// Purpose : tells if an entity is valid for transfer by this Actor
// ============================================================================

Standard_Boolean  STEPControl_ActorRead::Recognize
  (const Handle(Standard_Transient)& start)
{
  if (start.IsNull()) return Standard_False;

  if (start->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return Standard_True;

  if (start->IsKind(STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence))) return Standard_True;

  TCollection_AsciiString aProdMode = Interface_Static::CVal("read.step.product.mode");
  if(!aProdMode.IsEqual("ON"))
    if(start->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation))) return Standard_True;

  DeclareAndCast(StepShape_ShapeRepresentation,sr,start);
  if (!sr.IsNull()) {
    Standard_Integer i,nb = sr->NbItems();
    for (i = 1; i <= nb; i ++) {
      if (Recognize (sr->ItemsValue(i)) ) return Standard_True;
    }
    return Standard_False;
  }

  if (start->IsKind(STANDARD_TYPE(StepShape_FacetedBrep))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(StepShape_BrepWithVoids))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(StepShape_ManifoldSolidBrep))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(StepShape_FacetedBrepAndBrepWithVoids))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(StepShape_GeometricSet))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(StepRepr_MappedItem))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(StepShape_FaceSurface))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(StepShape_EdgeBasedWireframeModel))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(StepShape_FaceBasedSurfaceModel))) return Standard_True;


//  REPRESENTATION_RELATIONSHIP et consorts : on regarde le contenu ...
//  On prend WithTransformation ou non ...

  if (start->IsKind (STANDARD_TYPE(StepRepr_ShapeRepresentationRelationship))) {
    DeclareAndCast(StepRepr_ShapeRepresentationRelationship,und,start);  

//  On prend son contenu

    if (Recognize(und->Rep1()) || Recognize(und->Rep2())) return Standard_True;
    return Standard_False;
  }

  if (start->IsKind(STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation))) {
    return Standard_True;
    //  on fait le pari que, si ce n est pas transferable tel quel,
    //  des CDSR implicitement references le sont ...
    //  Sinon cette entite n aurait pas grand sens ...
  }

  return Standard_False;
}


// ============================================================================
// Method  : STEPControl_ActorRead::Transfer
// Purpose : recursive method that acces to the root entities and start the 
//           mapping
// ============================================================================

Handle(Transfer_Binder)  STEPControl_ActorRead::Transfer
(const Handle(Standard_Transient)& start,
 const Handle(Transfer_TransientProcess)& TP)
{  
  // [BEGIN] Get version of preprocessor (to detect I-Deas case) (ssv; 23.11.2010)
  Handle(StepData_StepModel) aStepModel = Handle(StepData_StepModel)::DownCast ( TP->Model() );
  Interface_EntityIterator anEntIt = aStepModel->Header();
  for ( anEntIt.Start(); anEntIt.More(); anEntIt.Next() ) {
    DeclareAndCast( HeaderSection_FileName, aFileNameEntity, anEntIt.Value() );
    if ( !aFileNameEntity.IsNull() ) {
      Handle(TCollection_HAsciiString) aPPVersion = aFileNameEntity->PreprocessorVersion();
      if(aPPVersion.IsNull())
        continue;
      #ifdef OCCT_DEBUG
      cout << "Preprocessor version detected: " << aPPVersion->ToCString() << endl;
      #endif
      Standard_Integer anIDeasResult = aPPVersion->Search("I-DEAS");
      if (anIDeasResult != -1) {
        #ifdef OCCT_DEBUG
        cout << "Recognized as I-DEAS STP" << endl;
        #endif
        myNMTool.SetIDEASCase(Standard_True);
      }
    }
  }
  // [END] Get version of preprocessor (to detect I-Deas case) (ssv; 23.11.2010)
  return TransferShape (start,TP);  
}


// ============================================================================
// auxiliary function : ApplyTransformation
// ============================================================================
static void ApplyTransformation (TopoDS_Shape& shape, const gp_Trsf &Trsf) 
{
  if ( Trsf.Form() == gp_Identity ) return;
  TopLoc_Location theLoc ( Trsf );
  shape.Move (theLoc);
}


// ============================================================================
// auxiliary function : FindContext
// ============================================================================
static Handle(StepRepr_Representation) FindContext (const Handle(Standard_Transient) &start,
						    const Handle(Transfer_TransientProcess) &TP,
						    const Standard_Integer level=10 )
{
  Handle(StepRepr_Representation) rep;
  const Interface_Graph& graph = TP->Graph();
  Interface_EntityIterator subs = graph.Sharings(start);
  for (subs.Start(); subs.More() && rep.IsNull(); subs.Next()) {
    rep = Handle(StepRepr_Representation)::DownCast(subs.Value());
    if ( rep.IsNull() && level >0 ) rep = FindContext ( subs.Value(), TP, level-1 );
  }
  return rep;
}

//=======================================================================
//function : FindShapeReprType
//purpose  : Returns integer corresponding to the type of the representation
//           as defined in read.step.shape.repr_level parameter
//=======================================================================

static Standard_Integer FindShapeReprType (const Handle(Standard_Transient) &start)
{
  if(start->IsKind(STANDARD_TYPE(StepShape_AdvancedBrepShapeRepresentation)))
    return 2;
  if(start->IsKind(STANDARD_TYPE(StepShape_ManifoldSurfaceShapeRepresentation)))
    return 3;
  if(start->IsKind(STANDARD_TYPE(StepShape_GeometricallyBoundedSurfaceShapeRepresentation)))
    return 4;
  if(start->IsKind(STANDARD_TYPE(StepShape_FacetedBrepShapeRepresentation)))
    return 5;
  if(start->IsKind(STANDARD_TYPE(StepShape_EdgeBasedWireframeShapeRepresentation)))
    return 6;
  if(start->IsKind(STANDARD_TYPE(StepShape_GeometricallyBoundedWireframeShapeRepresentation)))
    return 7;
  return 8;
}

//=======================================================================
//function : getListSDR
//purpose  : Get SDRs assigned to ShapeAspect, which may potentially
//           correspond to hybrid models in AP203 before 1998
//=======================================================================

static void getListSDR(const Handle(StepRepr_ShapeAspect)& sa,
                       Handle(TColStd_HSequenceOfTransient)& listSDR,
                       const Handle(Transfer_TransientProcess)& TP)
{
  const Interface_Graph& graph = TP->Graph();
  
  // check whether this ShapeAspect is used in G&DT, and if yes, ignore it
  if(sa->IsKind(STANDARD_TYPE(StepDimTol_DatumFeature))) return;
  Interface_EntityIterator subs5 = graph.Sharings(sa);
  for (subs5.Start(); subs5.More(); subs5.Next()) {
    if(subs5.Value()->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance)))
      return;
  }

  // reiterate by referred entities and take all SDRs; note that SDRs that
  // refer to sub-shapes of main SDR will be filtered out further during translation
  subs5 = graph.Sharings(sa);
  for (subs5.Start(); subs5.More() ; subs5.Next()) {
    Handle(StepRepr_PropertyDefinition) propd = 
      Handle(StepRepr_PropertyDefinition)::DownCast(subs5.Value());
    if(propd.IsNull()) continue;
    Interface_EntityIterator subs6 = graph.Sharings(propd);
    for (subs6.Start(); subs6.More() ; subs6.Next()) {
      Handle(StepShape_ShapeDefinitionRepresentation) sdr = 
        Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs6.Value());
      if ( ! sdr.IsNull() )
        listSDR->Append(sdr);
    }
  }
}

//=======================================================================
//function : getSDR
//purpose  : Find all SDRs related to given PDS
//=======================================================================

static void getSDR(const Handle(StepRepr_ProductDefinitionShape)& PDS,
                   Handle(TColStd_HSequenceOfTransient)& listSDR,
                   Handle(TColStd_HSequenceOfTransient)& listNAUO,
                   Handle(TColStd_HSequenceOfTransient)& listSDRAspect,
                   const Handle(Transfer_TransientProcess)& TP)
{
  // Flag indicating preferred shape representation type, to be chosen if 
  // several different representations are attached to the same shape
  Standard_Integer delta = 100;
  Standard_Integer ICS = Interface_Static::IVal("read.step.shape.repr");
  Standard_Integer nbSDR0 = listSDR->Length();
  
  // Iterate by entities referring PDS
  const Interface_Graph& graph = TP->Graph();
  Handle(StepShape_ShapeDefinitionRepresentation) NeedSDR;
  Interface_EntityIterator subs4 = graph.Sharings(PDS);
  for (subs4.Start(); subs4.More(); subs4.Next()) {

    // Directly assigned SDR
    Handle(StepShape_ShapeDefinitionRepresentation) sdr = 
      Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs4.Value());
    if ( ! sdr.IsNull() ) {
      Handle(StepRepr_Representation) rep = sdr->UsedRepresentation();

      if ( !rep.IsNull())  {
        // if specific representation type is preferred, choose the 
        // representations of the closest type
        if ( ICS == 1 ) // translate all SDRs
          listSDR->Append(sdr);
        else {
          Standard_Integer iDiff = Abs ( FindShapeReprType(rep) - ICS );
          // if more suitable representation is found, drop previous if any selected
          if( iDiff < delta ) {
            while ( listSDR->Length() > nbSDR0 ) 
              listSDR->Remove(listSDR->Length());
            delta = iDiff;
          }
          // take all representations of preferred or closest type
          if ( iDiff <= delta )
            listSDR->Append(sdr);
        }
      }
      continue;
    }

    // SDRs assigned via ShapeAspect; may correspond to hybrid model in AP203 before 1998
    Handle(StepRepr_ShapeAspect) sa = 
      Handle(StepRepr_ShapeAspect)::DownCast(subs4.Value());
    if ( ! sa.IsNull() ) {
      getListSDR(sa,listSDRAspect,TP);
      continue;
    }

    // NAUO is used to find sub-assemblies
    Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO = 
      Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(subs4.Value());
    if ( ! NAUO.IsNull() ) {
      if ( PDS->Definition().ProductDefinition() == NAUO->RelatingProductDefinition() )
        listNAUO->Append(NAUO);
      continue;
    }
  }
}


//=======================================================================
//function : TransferEntity
//purpose  : 
//=======================================================================
 Handle(TransferBRep_ShapeBinder) STEPControl_ActorRead::TransferEntity(const Handle(StepBasic_ProductDefinition)& PD,
                                                                        const Handle(Transfer_TransientProcess)& TP) 

{
  Handle(Message_Messenger) sout = TP->Messenger();
  Handle(TransferBRep_ShapeBinder) shbinder;
  
  TopoDS_Compound Cund;
  TopoDS_Shape Result1;
  BRep_Builder B;
  B.MakeCompound (Cund);
  
  // Find subcomponents of assembly (NAUO)
  // and definitions of shape of the current product (SDR)
  Handle(TColStd_HSequenceOfTransient) listSDR = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) listNAUO = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) listSDRAspect = new TColStd_HSequenceOfTransient;
  const Interface_Graph& graph = TP->Graph();
  Interface_EntityIterator subs3 = graph.Sharings(PD);
  for (subs3.Start(); subs3.More() ; subs3.Next()) {
    // PDS is used to find shape definitions attached to this product
    Handle(StepRepr_ProductDefinitionShape) PDS = 
      Handle(StepRepr_ProductDefinitionShape)::DownCast(subs3.Value());
    if ( ! PDS.IsNull() ) {
      getSDR(PDS,listSDR,listNAUO,listSDRAspect,TP);
      continue;
    }
    // NAUO is used to find sub-assemblies
    Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO = 
      Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(subs3.Value());
    if ( ! NAUO.IsNull() ) {
      if ( PD == NAUO->RelatingProductDefinition() )
        listNAUO->Append(NAUO);
      continue;
    }
  }
  
  // Flag indicating whether SDRs associated with the product`s main SDR
  // by SRRs (which correspond to hybrid model representation in AP203 since 1998) 
  // should be taken into account 
  Standard_Integer readSRR = Interface_Static::IVal("read.step.shape.relationship");
  
  // Flag indicating whether SDRs associated with the product`s main SDR
  // by SAs (which correspond to hybrid model representation in AP203 before 1998) 
  // should be taken into account 
  Standard_Integer readSA = Interface_Static::IVal("read.step.shape.aspect");
  if ( ! readSA ) 
    listSDRAspect->Clear();  
    
  // remember number of normal SDRs (not those found via ShapeAspect)
  // and merge both lists in one
  Standard_Integer nbNotAspect = listSDR->Length();
  listSDR->Append(listSDRAspect);

  // Flag indicating level of reading assemblies: only structure (3),
  // structure with shapes of final parts (2), or everything, including shapes
  // possibly attached directly to intermediate assemblies (1)
  // Special mode (4) is used to translate shape attached to this product only,
  // ignoring sub-assemblies if any
  Standard_Integer readAssembly = Interface_Static::IVal("read.step.assembly.level");
  if ( readAssembly ==3 || ( readAssembly ==2 && listNAUO->Length() >0 ) ) 
    listSDR->Clear();
  else if ( readAssembly == 4 )
    listNAUO->Clear();
  
  Standard_Integer nbEnt = listSDR->Length() + listNAUO->Length();
  if ( nbEnt <=0 )
    return shbinder;

  // common progress indicator for translation of own shapes and sub-assemblies
  Message_ProgressSentry PS ( TP->GetProgress(), "Part", 0, nbEnt, 1 );
  Standard_Integer nbComponents=0, nbShapes=0;

  // translate sub-assemblies
  for ( Standard_Integer nbNauo =1; nbNauo <= listNAUO->Length() && PS.More(); nbNauo++, PS.Next()) {
    Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO = 
      Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(listNAUO->Value(nbNauo));
      
#ifdef TRANSLOG
    if (TP->TraceLevel() > 1) 
      sout<<" -- Actor : Ent.n0 "<<TP->Model()->Number(PD)<<" -> Shared Ent.no"<<TP->Model()->Number(NAUO)<<endl;
#endif
    Handle(Transfer_Binder) binder;
    if (!TP->IsBound(NAUO)) binder = TransferEntity(NAUO,TP);
    else	                binder = TP->Find(NAUO);

    TopoDS_Shape theResult = TransferBRep::ShapeResult (binder);
    if (!theResult.IsNull()) {
      Result1 = theResult;
      // [BEGIN] ssv: OCCT#22436: extra compound in NMSSR case
      if (NM_DETECTED && Result1.ShapeType() == TopAbs_COMPOUND)
      {
        TopoDS_Iterator it(Result1);
        for ( ; it.More(); it.Next() ) 
        {
          TopoDS_Shape aSubShape = it.Value();
          B.Add(Cund, aSubShape);
        }
      }
      else
        B.Add(Cund, theResult);
      // [END] ssv: OCCT#22436: extra compound in NMSSR case
      nbComponents++;
    }
  }

  // translate shapes assigned directly
  for(Standard_Integer i=1; i <= listSDR->Length() && PS.More(); i++, PS.Next()) {
    Handle(StepShape_ShapeDefinitionRepresentation) sdr = 
      Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(listSDR->Value(i));
    Handle(StepShape_ShapeRepresentation) rep =  Handle(StepShape_ShapeRepresentation)::DownCast(sdr->UsedRepresentation());
    if ( rep.IsNull() )
      continue;

    // translate SDR representation
    Standard_Boolean isBound = Standard_True;
    Handle(Transfer_Binder) binder = TP->Find(rep);
    if (binder.IsNull())
      binder = TransferEntity(rep,TP,isBound);

    // if SDR is obtained from ShapeAspect and representation items have already been tramnslated,
    // this means that that ShapeAspect is used to refer to sub-shape of the main shape
    // (e.g. for validation properties), and we do not need to translate it actually;
    // otherwise it is considered as part of hybrid representation in AP203 before 1998
    if ( i > nbNotAspect && isBound)
        continue;

    // record shape resulting from translation
    TopoDS_Shape theResult;
    if ( ! binder.IsNull() ) {
      theResult = TransferBRep::ShapeResult (binder);
      if (!theResult.IsNull()) {
        Result1 = theResult;
        // [BEGIN] ssv: OCCT#22436: extra compound in NMSSR case
        if (NM_DETECTED && Result1.ShapeType() == TopAbs_COMPOUND)
        {
          TopoDS_Iterator it(Result1);
          for ( ; it.More(); it.Next() ) 
          {
            TopoDS_Shape aSubShape = it.Value();
            B.Add(Cund, aSubShape);
          }
        }
        else
          B.Add(Cund, theResult);
        // [END] ssv: OCCT#22436: extra compound in NMSSR case
        nbShapes++;
      }
    }
    
    // translate possible shapes related by SRRs, which corresponds to
    // way of writing hybrid models in AP203 since 1998, and AP209
    // Note that both AP203 and AP209 allow main representation to be non-empty
    if ( readSRR && /*theResult.IsNull() &&*/ i <= nbNotAspect) {
      Interface_EntityIterator subs1 = graph.Sharings(rep);
      Handle(Standard_Type) tSRR = STANDARD_TYPE(StepRepr_ShapeRepresentationRelationship);
      for (subs1.Start(); subs1.More(); subs1.Next()) {
        Handle(Standard_Transient) anitem = subs1.Value();
        if ( anitem->DynamicType() != tSRR ) continue;
        Handle(StepRepr_ShapeRepresentationRelationship) SRR =
          Handle(StepRepr_ShapeRepresentationRelationship)::DownCast(anitem);
        Standard_Integer nbrep = ( rep == SRR->Rep1() ? 2 : 1 );
        binder = TransferEntity(SRR, TP, nbrep);
        if ( ! binder.IsNull() ) {
          theResult = TransferBRep::ShapeResult (binder);
          Result1 = theResult;
          B.Add(Cund, theResult);
          nbShapes++;
        }
      }
    }
  }

  // make a warning if both own shape and sub-assemblies are present
  if ( nbShapes >0 && nbComponents > 0 )
    TP->AddWarning ( PD, "Product has both sub-assemblies and directly assigned shape" );

  // if only single shape is read, add it as it is; otherwise add compound
  if( nbShapes == 1 && nbComponents == 0 )
    shbinder = new TransferBRep_ShapeBinder (Result1);
  else
    shbinder = new TransferBRep_ShapeBinder (Cund);

  //TP->Unbind( PD ); //:j3: unbind start (let it be bound by TransferProcess)
  TP->Bind(PD, shbinder);
  return shbinder;  
}

//=======================================================================
//function : TransferEntity
//purpose  : 
//=======================================================================

Handle(TransferBRep_ShapeBinder) STEPControl_ActorRead::TransferEntity(const Handle(StepRepr_NextAssemblyUsageOccurrence)& NAUO ,
                                                                       const Handle(Transfer_TransientProcess)& TP)
{
 Handle(TransferBRep_ShapeBinder) shbinder;
  Handle(StepBasic_ProductDefinition) PD;
  const Interface_Graph& graph = TP->Graph();
  gp_Trsf Trsf;
  Standard_Boolean iatrsf=Standard_False, SRRReversed=Standard_False, IsDepend=Standard_False;
  Handle(StepRepr_ShapeRepresentationRelationship) SRR;
  Interface_EntityIterator subs1 = graph.Sharings(NAUO);
  for (subs1.Start(); subs1.More(); subs1.Next()) {
    Handle(StepRepr_ProductDefinitionShape) PDS = 
      Handle(StepRepr_ProductDefinitionShape)::DownCast(subs1.Value());
    if(PDS.IsNull()) continue;
    Interface_EntityIterator subs2 = graph.Sharings(PDS);
    for (subs2.Start(); subs2.More(); subs2.Next()) {
      Handle(StepShape_ContextDependentShapeRepresentation) CDSR = 
        Handle(StepShape_ContextDependentShapeRepresentation)::DownCast(subs2.Value());
      if (CDSR.IsNull()) continue;
      IsDepend=Standard_True;
      Handle(StepRepr_RepresentationRelationship) RR = CDSR->RepresentationRelation();
      if (RR.IsNull()) continue;
      SRRReversed = STEPConstruct_Assembly::CheckSRRReversesNAUO ( graph, CDSR );
      Handle(StepRepr_Representation) rep = ( SRRReversed ? RR->Rep2() : RR->Rep1() );
      iatrsf = ComputeSRRWT ( RR, TP, Trsf );
      // find real ProductDefinition used rep
      Interface_EntityIterator subs3 = TP->Graph().Sharings(rep);
      for (subs3.Start(); subs3.More(); subs3.Next()) {
        if ( subs3.Value()->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation))) {
          DeclareAndCast(StepShape_ShapeDefinitionRepresentation,SDR,subs3.Value());
          Handle(StepRepr_ProductDefinitionShape) PDS1 = 
            Handle(StepRepr_ProductDefinitionShape)::DownCast(SDR->Definition().PropertyDefinition());
          if(PDS1.IsNull()) continue;
          Interface_EntityIterator subs4 = graph.Shareds(PDS1);
          for (subs4.Start(); subs4.More(); subs4.Next()) {
            Handle(StepBasic_ProductDefinition) PD1 = 
              Handle(StepBasic_ProductDefinition)::DownCast(subs4.Value());
            if(PD1.IsNull()) continue;
            PD=PD1;
          }
        }
        else if(subs3.Value()->IsKind(STANDARD_TYPE(StepRepr_ShapeRepresentationRelationship))) {
          SRR = Handle(StepRepr_ShapeRepresentationRelationship)::DownCast(subs3.Value());
        }
      }
    }
  }
  
  Handle(Transfer_Binder) binder;
  TopoDS_Shape theResult;
  shbinder.Nullify();
  
  if(IsDepend) {
    
    if(!PD.IsNull()) {
      binder = TP->Find(PD);
      if ( binder.IsNull() ) binder = TransferEntity(PD,TP);
      theResult = TransferBRep::ShapeResult(binder);
      if (!theResult.IsNull()) {
        if (iatrsf) {
          if ( SRRReversed ) ApplyTransformation ( theResult, Trsf.Inverted() );
          else               ApplyTransformation ( theResult, Trsf );
        }
        shbinder = new TransferBRep_ShapeBinder (theResult);
      }
    }
    
    if ( theResult.IsNull() && !SRR.IsNull() ) {
      binder = TP->Find(SRR);
      if ( binder.IsNull() ) {
        binder = TransferEntity(SRR,TP);
        theResult = TransferBRep::ShapeResult (binder);
        if (!theResult.IsNull())
          shbinder = new TransferBRep_ShapeBinder (theResult);
      }
    }
    
  }
  TP->Bind(NAUO, shbinder);
  return shbinder;
  
}
//=======================================================================
//function : TransferEntity
//purpose  : 
//=======================================================================

Handle(TransferBRep_ShapeBinder) STEPControl_ActorRead::TransferEntity(const Handle(StepShape_ShapeRepresentation)& sr,
                                                                       const Handle(Transfer_TransientProcess)& TP,
                                                                       Standard_Boolean& isBound)
{
  NM_DETECTED = Standard_False;
  Handle(TransferBRep_ShapeBinder) shbinder;
  if(!Recognize(sr))
    return shbinder;
  isBound = Standard_False;
  Standard_Integer nb = sr->NbItems();
  // Used in XSAlgo::AlgoContainer()->ProcessShape (ssv; 13.11.2010)
  Standard_Integer nbTPitems = TP->NbMapped();
  Handle(Message_Messenger) sout = TP->Messenger();
  #ifdef TRANSLOG
  if (TP->TraceLevel() > 2) 
    sout<<" -- Actor : case  ShapeRepr. NbItems="<<nb<<endl;
  #endif
  
    // Compute unit convertion factors and geometric Accuracy
  Handle(StepRepr_Representation) oldSRContext = mySRContext; //:S4136
  PrepareUnits(sr,TP);
  
  BRep_Builder B;
  TopoDS_Compound comp;
  B.MakeCompound (comp);
  TopoDS_Shape OneResult;
  Standard_Integer nsh = 0;
  Message_ProgressSentry PS ( TP->GetProgress(), "Sub-assembly", 0, nb, 1 );

  // [BEGIN] Proceed with non-manifold topology (ssv; 12.11.2010)
  Standard_Boolean isNMMode = Interface_Static::IVal("read.step.nonmanifold") != 0;
  Standard_Boolean isManifold = Standard_True;
  if ( isNMMode && sr->IsKind(STANDARD_TYPE(StepShape_NonManifoldSurfaceShapeRepresentation)) ) {
    isManifold = Standard_False;
    NM_DETECTED = Standard_True;
    #ifdef OCCT_DEBUG
    Standard_Integer NMSSRItemsLen = sr->Items()->Length();
    cout << "NMSSR with " << NMSSRItemsLen << " items detected" << endl;
    #endif
  } 
  // Special processing for I-DEAS STP case (ssv; 15.11.2010)
  else {
    Standard_Integer isIDeasMode = Interface_Static::IVal("read.step.ideas");
    if (isNMMode && myNMTool.IsIDEASCase() && isIDeasMode) {
      isManifold = Standard_False;
      NM_DETECTED = Standard_True;
      #ifdef OCCT_DEBUG
      cout << "I-DEAS post processing for non-manifold topology ENABLED" << endl;
      #endif
    }
    #ifdef OCCT_DEBUG
    else if ( myNMTool.IsIDEASCase() )
      cout << "I-DEAS post processing for non-manifold topology DISABLED" << endl;
    #endif
  }
  myNMTool.CleanUp();
  // Don't use NM tool in manifold cases (ssv; 24.11.2010)
  myNMTool.SetActive(!isManifold && isNMMode);
  // [END] Proceed with non-manifold topology (ssv; 12.11.2010)

  for (Standard_Integer i = 1; i <= nb && PS.More(); i ++,PS.Next()) {
  //for (i = 1; i <= nb ; i ++) {
    #ifdef TRANSLOG
    if (TP->TraceLevel() > 2) 
      sout<<" -- Actor, shape_representation.item n0. "<<i<<endl;
    #endif
    Handle(StepRepr_RepresentationItem) anitem = sr->ItemsValue(i);
    Handle(Transfer_Binder) binder;
    if (!TP->IsBound(anitem)) {
      binder = TransferShape(anitem, TP, isManifold);
    }
    else {
      isBound = Standard_True;
      binder = TP->Find(anitem);
    }
    TopoDS_Shape theResult = TransferBRep::ShapeResult (binder);
    if (!theResult.IsNull()) {
      OneResult = theResult;
      B.Add(comp, theResult);
      nsh ++;
    }
  }

  // [BEGIN] Proceed with non-manifold topology (ssv; 12.11.2010)
  if (!isManifold) {

    Handle(Standard_Transient) info;
    // IMPORTANT: any fixing on non-manifold topology must be done after the shape is transferred from STEP
    TopoDS_Shape fixedResult = 
      XSAlgo::AlgoContainer()->ProcessShape( comp, myPrecision, myMaxTol,
                                             "read.step.resource.name", 
                                             "read.step.sequence", info,
                                             TP->GetProgress(), Standard_True);
    XSAlgo::AlgoContainer()->MergeTransferInfo(TP, info, nbTPitems);

    if (fixedResult.ShapeType() == TopAbs_COMPOUND)
    {
        comp = TopoDS::Compound(fixedResult);
    }
    else
    {
        comp.Nullify();
        B.MakeCompound(comp);
        B.Add(comp, fixedResult);
    }

    BRep_Builder brepBuilder;

    // [BEGIN] Try to close OPEN Shells in I-DEAS case (ssv; 17.11.2010)
    if ( myNMTool.IsIDEASCase() ) {
            
      // ==========================================================
      //  For each Shell (always OPEN in I-DEAS case) find all 
      //  the possibilities to close it with adjacent non-manifold
      //  Shells which are stored separately in I-DEAS-like STP
      // ==========================================================

      TopTools_IndexedDataMapOfShapeListOfShape shellClosingsMap;
      // Find possible closings for each shell
      this->computeIDEASClosings(comp, shellClosingsMap);

      // Make compound to store closed Shells
      TopoDS_Compound compWithClosings;
      brepBuilder.MakeCompound(compWithClosings);
      
      // Attempt to close Shells one-by-one
      for (Standard_Integer i = 1; i <= shellClosingsMap.Extent(); i++) {
        TopoDS_Shell adjustedShell = this->closeIDEASShell( TopoDS::Shell( shellClosingsMap.FindKey(i) ),
                                                            shellClosingsMap.FindFromIndex(i) );     
        brepBuilder.Add(compWithClosings, adjustedShell);
      }
      // Put not suspected open Shells as they are (updated 23.11.2010)
      TopExp_Explorer allShellsExp(comp, TopAbs_SHELL);
      for ( ; allShellsExp.More(); allShellsExp.Next() ) {
        TopoDS_Shape aCurrentShell = allShellsExp.Current();
        if ( !myNMTool.IsPureNMShell(aCurrentShell) && !shellClosingsMap.Contains(aCurrentShell) )
          brepBuilder.Add(compWithClosings, aCurrentShell);
      }
      comp = compWithClosings; 
    }
    // [END] Try to close OPEN Shells in I-DEAS case (ssv; 17.11.2010)

    // [BEGIN] Reconstruct Solids from Closed Shells (ssv; 15.11.2010)
    TopoDS_Compound reconstComp;
    brepBuilder.MakeCompound(reconstComp);
    TopExp_Explorer exp(comp, TopAbs_SHELL);
    for (; exp.More(); exp.Next())
    {
        TopoDS_Shape aSubShape = exp.Current();
        if (aSubShape.ShapeType() == TopAbs_SHELL && aSubShape.Closed()) {
            TopoDS_Solid nextSolid;
            brepBuilder.MakeSolid(nextSolid);
            brepBuilder.Add(nextSolid, aSubShape);
            brepBuilder.Add(reconstComp, nextSolid);
        }
        else if (aSubShape.ShapeType() == TopAbs_SHELL)
            brepBuilder.Add(reconstComp, aSubShape);
    }
    comp = reconstComp;
    // [END] Reconstruct Solids from Closed Shells (ssv; 15.11.2010)
  }

  // Bind the resulting shape
  if      (nsh == 0) shbinder.Nullify();
  else if (nsh == 1) shbinder = new TransferBRep_ShapeBinder (OneResult);
  else               shbinder = new TransferBRep_ShapeBinder (comp);

  PrepareUnits ( oldSRContext, TP ); //:S4136
  TP->Bind(sr, shbinder);

  
  #ifdef OCCT_DEBUG
  DumpWhatIs(comp);
  #endif
  
  return shbinder;
  
}
//=======================================================================
//function : TransferEntity
//purpose  : 
//=======================================================================

Handle(TransferBRep_ShapeBinder) STEPControl_ActorRead::TransferEntity(const Handle(StepShape_ContextDependentShapeRepresentation)& CDSR,
                                                                       const Handle(Transfer_TransientProcess)& TP)
{
  Handle(TransferBRep_ShapeBinder) shbinder;
  //:j2: treat SRRs here in order to compare them with NAUO
  Handle(StepRepr_ShapeRepresentationRelationship) SRR = CDSR->RepresentationRelation();
  if ( SRR.IsNull() ) return shbinder;
  
  Standard_Boolean SRRReversed = STEPConstruct_Assembly::CheckSRRReversesNAUO ( TP->Graph(), CDSR );
  Handle(StepRepr_Representation) rep1 = ( SRRReversed ? SRR->Rep2() : SRR->Rep1() );
  Handle(StepShape_ShapeRepresentation) rep = Handle(StepShape_ShapeRepresentation)::DownCast(rep1);

  if ( SRRReversed ) 
    TP->AddWarning ( SRR, "SRR reverses relation defined by NAUO; NAUO definition is taken" );
  
  TopoDS_Shape theResult;
  
  gp_Trsf Trsf;
  Standard_Boolean iatrsf = ComputeSRRWT ( SRR, TP, Trsf );

  Handle(Transfer_Binder) binder;
  Standard_Boolean isBound = Standard_False;
    if (!TP->IsBound(rep)) binder = TransferEntity(rep,TP,isBound);
    else binder = TP->Find(rep);
    theResult = TransferBRep::ShapeResult (binder);
      
  if ( ! theResult.IsNull() ) {
    if ( iatrsf ) {
      if ( SRRReversed ) ApplyTransformation ( theResult, Trsf.Inverted() );
      else               ApplyTransformation ( theResult, Trsf );
    }
    shbinder = new TransferBRep_ShapeBinder (theResult);
  }
  else shbinder.Nullify();
  TP->Bind(CDSR, shbinder);
  return shbinder;
  
}
//=======================================================================
//function : TransferEntity
//purpose  : 
//=======================================================================

Handle(TransferBRep_ShapeBinder) STEPControl_ActorRead::TransferEntity(const Handle(StepRepr_ShapeRepresentationRelationship)& und,
                                                                       const Handle(Transfer_TransientProcess)& TP,
                                                                       const Standard_Integer nbrep)
{
  //  REPRESENTATION_RELATIONSHIP et la famille
  Handle(TransferBRep_ShapeBinder) shbinder;
  
  if (und.IsNull()) return shbinder;
  
  //  On prend son contenu : Rep1 ou Rep2 , that is the question
  //    on prend les 2. Mais quoi faire des axes
  
  TopoDS_Compound Cund;
  TopoDS_Shape OneResult;
  BRep_Builder B;
  B.MakeCompound (Cund);
  Standard_Integer nsh = 0;
  
  gp_Trsf Trsf;
  Standard_Boolean iatrsf = ComputeSRRWT ( und, TP, Trsf );
  
  //    Transfert : que faut-il prendre au juste ?
  
  for (Standard_Integer i = 1; i <= 2; i ++) {
    if(nbrep && nbrep != i)
      continue;
    Handle(StepRepr_Representation) anitemt;
    if (i == 1) anitemt = und->Rep1();
    if (i == 2) anitemt = und->Rep2();
    Handle(StepShape_ShapeRepresentation) anitem = Handle(StepShape_ShapeRepresentation)::DownCast(anitemt);
    Handle(Transfer_Binder) binder;
    Standard_Boolean isBound = Standard_False;
    if (!TP->IsBound(anitem)) binder = TransferEntity(anitem,TP,isBound);
    else	                binder = TP->Find(anitem);
    TopoDS_Shape theResult = TransferBRep::ShapeResult (binder);
    if (!theResult.IsNull()) {
      OneResult = theResult;
      B.Add(Cund, theResult);
      nsh ++;
    }
  }
  
  //   La Transformation : selon les cas
  
  //    Appliquer la transformation
  
  if (iatrsf) {
    if      (nsh == 1) ApplyTransformation ( OneResult, Trsf );
    else if (nsh >  1) ApplyTransformation ( Cund, Trsf );
  }
  if      (nsh == 0) shbinder.Nullify();
  else if (nsh == 1) shbinder = new TransferBRep_ShapeBinder (OneResult);
  else               shbinder = new TransferBRep_ShapeBinder (Cund);
  TP->Bind(und, shbinder);
  return shbinder;
  
}

//=======================================================================
//function : IsNeedRepresentation
//purpose  : 
//=======================================================================

static Standard_Boolean IsNeedRepresentation(const Handle(StepRepr_ShapeAspect)& sa,
                                             const Handle(StepRepr_Representation)&repA ,
                                             const Handle(Transfer_TransientProcess)& TP)
{
  Standard_Boolean IsSDRaspect=Standard_True;
  Handle(StepRepr_ProductDefinitionShape) PDSA = sa->OfShape();
  const Interface_Graph& graph = TP->Graph();
  Interface_EntityIterator subs7 = graph.Sharings(PDSA);
  for (subs7.Start(); ! PDSA.IsNull() && subs7.More(); subs7.Next()) {
    Handle(StepShape_ShapeDefinitionRepresentation) sdrA = 
      Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs7.Value());
    if ( sdrA.IsNull() ) continue;
    // abv 23 Feb 00: use iterator to take into account AP203 hybrid models
    Interface_EntityIterator subs8 = graph.Shareds(sdrA);
    for (subs8.Start(); subs8.More(); subs8.Next()) {
      Handle(StepRepr_Representation) rep2 = 
        Handle(StepRepr_Representation)::DownCast ( subs8.Value() );
      if ( rep2.IsNull() ) {
        Handle(StepRepr_ShapeRepresentationRelationship) SRR = 
          Handle(StepRepr_ShapeRepresentationRelationship)::DownCast ( subs8.Value() );
        if ( SRR.IsNull() ) continue;
        rep2 = ( sdrA->UsedRepresentation() == SRR->Rep1() ? SRR->Rep2() : SRR->Rep1() );
      }
      Standard_Integer i, j; // svv Jan 11 2000 : porting on DEC
      for ( i=1; i <= repA->NbItems(); i++ ) {
        Handle(StepRepr_RepresentationItem) it = repA->ItemsValue(i);
        for ( j=1; j <= rep2->NbItems(); j++ )
          if ( it == rep2->ItemsValue(j) ) break;
        if ( j > rep2->NbItems() ) break;
      }
      if ( i > repA->NbItems() ) IsSDRaspect=Standard_False;
    }
  }
  return IsSDRaspect;
}
//=======================================================================
//function : OldWay
//purpose  : 
//=======================================================================

Handle(TransferBRep_ShapeBinder) STEPControl_ActorRead::OldWay(const Handle(Standard_Transient)& start,
                                                               const Handle(Transfer_TransientProcess)& TP)
{
  Handle(Message_Messenger) sout = TP->Messenger();
  const Interface_Graph& graph = TP->Graph();
  Handle(TransferBRep_ShapeBinder) shbinder;
  DeclareAndCast(StepShape_ShapeDefinitionRepresentation,sdr,start);
  Handle(StepRepr_Representation) rep = sdr->UsedRepresentation();

  // abv 7 Oct 99: TRJ2: skip SDRs used only for defining SHAPE_ASPECT (validation properties)
  // BUT ONLY if they have representation duplicated with other SDR,
  // (SHAPE_ASPECT also used by some systems to define geometry)
  Handle(StepRepr_PropertyDefinition) PD = sdr->Definition().PropertyDefinition();
  if ( ! PD.IsNull() ) {
    Handle(StepRepr_ShapeAspect) SA = PD->Definition().ShapeAspect();
    if ( ! SA.IsNull() ) {
      if(!IsNeedRepresentation(SA,rep,TP))
        return shbinder;
    }
  }
    
#ifdef TRANSLOG
  if (TP->TraceLevel() > 2) 
    sout<<" -- Actor : case  shape_definition_representation."<<endl;
#endif
  Handle(Transfer_Binder) binder = TP->Find(rep);
  if (binder.IsNull()) binder = TP->Transferring(rep);
//:j2    if (!binder.IsNull()) return binder;

//    SDR designant des CDSR (lien implicite, via la UsedRepr)

  TopoDS_Compound Cund;
  TopoDS_Shape OneResult;
  BRep_Builder B;
  B.MakeCompound (Cund);
  Standard_Integer nsh = 0;

  //:j2
  shbinder = new TransferBRep_ShapeBinder;
  TP->Bind ( start, shbinder ); //:j3 abv 23 Oct 98: rp1sd.stp: bind something to protect against loops
  if (!binder.IsNull()) {
    TopoDS_Shape theResult = TransferBRep::ShapeResult (binder);
    if (!theResult.IsNull()) {
      OneResult = theResult;
      B.Add(Cund, theResult);
      nsh ++;
      shbinder->SetResult (theResult);
    }
  }

  // process subcomponents of assembly (CDSR) and hybrid models (SRR)
  Interface_EntityIterator subs = graph.Shareds(start);
  Handle(Standard_Type) tCDSR = STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation);
  Handle(Standard_Type) tSRR = STANDARD_TYPE(StepRepr_ShapeRepresentationRelationship);
  Standard_Integer nbitem=0;
  for (subs.Start();subs.More();subs.Next()) nbitem++;
  Message_ProgressSentry PS ( TP->GetProgress(), "Sub", 0, nbitem, 1 );
  for (subs.Start(); subs.More() && PS.More(); subs.Next() ,PS.Next()) {
    Handle(Standard_Transient) anitem = subs.Value();
    if ( anitem->DynamicType() != tCDSR && anitem->DynamicType() != tSRR ) continue;
//      DeclareAndCast(StepShape_ContextDependentShapeRepresentation,anitem,subs.Value());
//      if (anitem.IsNull()) continue;
#ifdef TRANSLOG
    if (TP->TraceLevel() > 1) 
      sout<<" -- Actor : Ent.n0 "<<TP->Model()->Number(start)<<" -> Shared Ent.no"<<TP->Model()->Number(anitem)<<endl;
#endif

    if (!TP->IsBound(anitem)) binder = TP->Transferring(anitem);
    else	                binder = TP->Find(anitem);
    TopoDS_Shape theResult = TransferBRep::ShapeResult (binder);
    if (!theResult.IsNull()) {
      OneResult = theResult;
      B.Add(Cund, theResult);
      nsh += 2; //abv 11.10.00: 2 instead of 1 in order to keep assembly structure
    }
  }

  if      (nsh == 0) shbinder = new TransferBRep_ShapeBinder (Cund);//shbinder.Nullify();
  else if (nsh == 1) shbinder = new TransferBRep_ShapeBinder (OneResult);
  else               shbinder = new TransferBRep_ShapeBinder (Cund);
  TP->Unbind( start ); //:j3: unbind start (let it be bound by TransferProcess)
  return shbinder;
}

//=======================================================================
//function : TransferEntity
//purpose  : 
//=======================================================================

Handle(TransferBRep_ShapeBinder) STEPControl_ActorRead::TransferEntity(const Handle(StepGeom_GeometricRepresentationItem)& start,
                                                                       const Handle(Transfer_TransientProcess)& TP,
                                                                       const Standard_Boolean isManifold)
{
  Handle(Message_Messenger) sout = TP->Messenger();
  Handle(TransferBRep_ShapeBinder) shbinder;
  Standard_Boolean found = Standard_False;
  StepToTopoDS_Builder myShapeBuilder;
  TopoDS_Shape mappedShape;
  Standard_Integer nbTPitems = TP->NbMapped();
#ifdef TRANSLOG
  OSD_Timer chrono;
  if (TP->TraceLevel() > 2) 
    sout << "Begin transfer STEP -> CASCADE, Type "
         << start->DynamicType()->Name() << endl;
  chrono.Start();
#endif
  
  //:S4136
  Handle(StepRepr_Representation) oldSRContext = mySRContext;
  if ( mySRContext.IsNull() ) { // if no context, try to find it (ex: r0701_ug.stp #4790)
    Handle(StepRepr_Representation) context = FindContext ( start, TP );
    if ( context.IsNull() ) {
      TP->AddWarning ( start, "Entity with no unit context; default units taken" );
      ResetUnits();
    }
    else PrepareUnits ( context, TP );
  }
  myShapeBuilder.SetPrecision(myPrecision);
  myShapeBuilder.SetMaxTol(myMaxTol);

  // Start progress scope (no need to check if progress exists -- it is safe)
  Message_ProgressSentry aPSentry(TP->GetProgress(), "Transfer stage", 0, 2, 1);

  try {
    OCC_CATCH_SIGNALS
  if (start->IsKind(STANDARD_TYPE(StepShape_FacetedBrep))) {
    myShapeBuilder.Init(GetCasted(StepShape_FacetedBrep, start), TP);
    found = Standard_True;
  } 
  else if (start->IsKind(STANDARD_TYPE(StepShape_BrepWithVoids))) {
    myShapeBuilder.Init(GetCasted(StepShape_BrepWithVoids, start), TP);
    found = Standard_True;
  } 
  else if (start->IsKind(STANDARD_TYPE(StepShape_ManifoldSolidBrep))) {
    myShapeBuilder.Init(GetCasted(StepShape_ManifoldSolidBrep, start), TP);
    found = Standard_True;
  } 
  else if (start->IsKind(STANDARD_TYPE(StepShape_ShellBasedSurfaceModel))) {
    myShapeBuilder.Init(GetCasted(StepShape_ShellBasedSurfaceModel, start), TP, myNMTool);
    found = Standard_True;
  } 
  else if (start->IsKind(STANDARD_TYPE(StepShape_FacetedBrepAndBrepWithVoids))) {
    myShapeBuilder.Init(GetCasted(StepShape_FacetedBrepAndBrepWithVoids, start), TP);
    found = Standard_True;
  } 
  else if (start->IsKind(STANDARD_TYPE(StepShape_GeometricSet))) {
    myShapeBuilder.Init(GetCasted(StepShape_GeometricSet, start), TP, this, isManifold);
    found = Standard_True;
  }
  else if (start->IsKind(STANDARD_TYPE(StepShape_EdgeBasedWireframeModel))) {
    myShapeBuilder.Init(GetCasted(StepShape_EdgeBasedWireframeModel, start), TP);
    found = Standard_True;
  }
  else if (start->IsKind(STANDARD_TYPE(StepShape_FaceBasedSurfaceModel))) {
    myShapeBuilder.Init(GetCasted(StepShape_FaceBasedSurfaceModel, start), TP);
    found = Standard_True;
  }
}
  catch(Standard_Failure) {
    TP->AddFail(start,"Exeption is raised. Entity was not translated.");
    TP->Bind(start, shbinder);
    return shbinder;
  }

  aPSentry.Next();
  
  if (found && myShapeBuilder.IsDone()) {
    mappedShape = myShapeBuilder.Value();
    // Apply ShapeFix (on manifold shapes only. Non-manifold topology is processed separately: ssv; 13.11.2010)
    if (isManifold) {
      Handle(Standard_Transient) info;
      mappedShape = 
        XSAlgo::AlgoContainer()->ProcessShape( mappedShape, myPrecision, myMaxTol,
                                               "read.step.resource.name", 
                                               "read.step.sequence", info,
                                               TP->GetProgress() );
      XSAlgo::AlgoContainer()->MergeTransferInfo(TP, info, nbTPitems);
    }
  }
  found = !mappedShape.IsNull();
  if (found && shbinder.IsNull()) shbinder = new TransferBRep_ShapeBinder (mappedShape);
#ifdef TRANSLOG
  chrono.Stop();
  if (TP->TraceLevel() > 2) 
    sout<<"End transfer STEP -> CASCADE :" << (found ? "OK" : " : no result")<<endl;
  if (TP->TraceLevel() > 2) 
    chrono.Show();
#endif
  if ( oldSRContext.IsNull() && ! mySRContext.IsNull() ) //:S4136
    PrepareUnits ( oldSRContext, TP );
  TP->Bind(start, shbinder);
  return shbinder;
}

 
//=======================================================================
//function : TransferEntity
//purpose  : 
//=======================================================================

Handle(TransferBRep_ShapeBinder) STEPControl_ActorRead::TransferEntity(const Handle(StepRepr_MappedItem)&  mapit,
                                                              const Handle(Transfer_TransientProcess)& TP)
{
  Handle(TransferBRep_ShapeBinder) shbinder;
  
    // --------------------------------------------------------------
  // On se trouve ici dans un contexte " d'assemblage geometrique "
  //    - MappedItem
  // --------------------------------------------------------------
  
  //:S4136: abv 20 Apr 99: as1ug.stp: MAPPED_ITEM transformation computed
  //        taking into account units of origin and target SHAPE_REPRESENTATIONs
  
  //  La Shape, et la mise en position
  Handle(StepShape_ShapeRepresentation) maprep =  Handle(StepShape_ShapeRepresentation)::
    DownCast(mapit->MappingSource()->MappedRepresentation());
  Standard_Boolean isBound = Standard_False; 
  Handle(Transfer_Binder) binder = TP->Find(maprep);
  if (binder.IsNull())    binder = TransferEntity(maprep,TP,isBound);
  shbinder = Handle(TransferBRep_ShapeBinder)::DownCast(binder);
  if (shbinder.IsNull()) TP->AddWarning(mapit,"No Shape Produced");
  else {
    TopoDS_Shape mappedShape = shbinder->Result();
    if ( ! mappedShape.IsNull() ) {
      
      //  Positionnement : 2 formules
      //  1/ Ax2 dans Source et comme Target  : passage de Source a Target
      //  2/ CartesianOperator3d comme Target : on applique
      
      gp_Trsf Trsf;
      Standard_Boolean ok = Standard_False;
      
      Handle(StepGeom_CartesianTransformationOperator3d) CartOp =
        Handle(StepGeom_CartesianTransformationOperator3d)::DownCast(mapit->MappingTarget());
      if ( ! CartOp.IsNull() ) {
        ok = StepToGeom::MakeTransformation3d (CartOp, Trsf);
      }
      else {
        Handle(StepGeom_Axis2Placement3d) Origin = 
          Handle(StepGeom_Axis2Placement3d)::DownCast(mapit->MappingSource()->MappingOrigin());
        Handle(StepGeom_Axis2Placement3d) Target = 
          Handle(StepGeom_Axis2Placement3d)::DownCast(mapit->MappingTarget());
        if ( ! Origin.IsNull() && ! Target.IsNull() ) {
          ok = Standard_True;
          Handle(StepRepr_Representation) rep = mySRContext; // NOTE: copy of handle !
          ComputeTransformation ( Origin, Target, maprep, rep, TP, Trsf );
          ok = Standard_True;
        }
      }
      
      if ( ok ) ApplyTransformation ( mappedShape, Trsf );
      else TP->AddWarning (mapit,"Mapped Item, case not recognized, location ignored");
      
      shbinder = new TransferBRep_ShapeBinder (mappedShape);
    }
  }
  TP->Bind(mapit, shbinder);
  return shbinder;
}
//=======================================================================
//function : TransferEntity
//purpose  : 
//=======================================================================

Handle(TransferBRep_ShapeBinder) STEPControl_ActorRead::TransferEntity(const Handle(StepShape_FaceSurface)& fs,
                                                                       const Handle(Transfer_TransientProcess)& TP)
{

  //    Cas bien utile meme si non reconnu explicitement
  Handle(TransferBRep_ShapeBinder) sb;
  Standard_Integer nbTPitems = TP->NbMapped();
  try {
    OCC_CATCH_SIGNALS

    StepToTopoDS_Tool         myTool;
    StepToTopoDS_DataMapOfTRI aMap;

    myTool.Init(aMap, TP);
    StepToTopoDS_TranslateFace myTF;
    myTF.SetPrecision(myPrecision);
    myTF.SetMaxTol(myMaxTol);

    // Non-manifold topology is not processed here (ssv; 15.11.2010)
    StepToTopoDS_NMTool dummyNMTool;
    myTF.Init(fs, myTool, dummyNMTool);
    Handle(StepRepr_Representation) oldSRContext = mySRContext;
    if (mySRContext.IsNull()) { // if no context, try to find it (ex: r0701_ug.stp #4790)
      Handle(StepRepr_Representation) context = FindContext(fs, TP);
      if (context.IsNull()) {
        TP->AddWarning(fs, "Entity with no unit context; default units taken");
        ResetUnits();
      }
      else PrepareUnits(context, TP);
    }

    // Apply ShapeFix
    Handle(Transfer_Binder) binder = TP->Find(fs);
    sb = Handle(TransferBRep_ShapeBinder)::DownCast(binder);
    if (!sb.IsNull() && !sb->Result().IsNull()) {
      TopoDS_Shape S = sb->Result();

      Handle(Standard_Transient) info;
      TopoDS_Shape shape = XSAlgo::AlgoContainer()->ProcessShape(S, myPrecision, myMaxTol,
        "read.step.resource.name",
        "read.step.sequence", info,
        TP->GetProgress());
      //      TopoDS_Shape shape = XSAlgo::AlgoContainer()->PerformFixShape( S, TP, myPrecision, myMaxTol );
      if (shape != S)
        sb->SetResult(shape);

      XSAlgo::AlgoContainer()->MergeTransferInfo(TP, info, nbTPitems);
    }


    if (oldSRContext.IsNull() && !mySRContext.IsNull()) //:S4136
      PrepareUnits(oldSRContext, TP);
    TP->Bind(fs, sb);
    return sb; // TP->Find (start);
  }
  catch(Standard_Failure)
  {
    TP->AddFail(fs,"Exeption is raised. Entity was not translated.");
    sb.Nullify();
    TP->Bind(fs, sb);
    return sb;
  }
}
//=======================================================================
//function : TransferShape
//purpose  : 
//=======================================================================

Handle(Transfer_Binder) STEPControl_ActorRead::TransferShape(const Handle(Standard_Transient)& start,
                                                             const Handle(Transfer_TransientProcess)& TP,
                                                             const Standard_Boolean isManifold)
{
  if (start.IsNull()) return NullResult();
  XSAlgo::AlgoContainer()->PrepareForTransfer();

  Handle(Message_Messenger) sout = TP->Messenger();
#ifdef TRANSLOG
//  POUR MISE AU POINT, a supprimer ensuite
  if (TP->TraceLevel() > 1) 
    sout<<" -- Actor : Transfer Ent.n0 "<<TP->Model()->Number(start)<<"  Type "<<start->DynamicType()->Name()<<endl;
#endif
  
  Handle(TransferBRep_ShapeBinder) shbinder;
  

  // Product Definition Entities
  // They should be treated with Design Manager
   // case ShapeDefinitionRepresentation if ProductMode != ON
  TCollection_AsciiString aProdMode = Interface_Static::CVal("read.step.product.mode");
  if(!aProdMode.IsEqual("ON") && 
     start->IsKind(STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation))) 
    shbinder = OldWay(start,TP);
  //skl
  
  else if (start->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) {
    Handle(StepBasic_ProductDefinition) PD =
      Handle(StepBasic_ProductDefinition)::DownCast(start);
    shbinder = TransferEntity(PD, TP);
  }
  
  // NextAssemblyUsageOccurrence
  else if (start->IsKind(STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence))) {
    Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO =
      Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(start);
    shbinder = TransferEntity(NAUO, TP);
  }
  //end skl
    
  // Shape Representation
  else if (start->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation))) {
    DeclareAndCast(StepShape_ShapeRepresentation,sr,start);
    Standard_Boolean isBound = Standard_False;
    shbinder = TransferEntity(sr,TP,isBound);
  }
  
    // --------------------------------------------------------------
    // On se trouve ici aussi dans un contexte " d'assemblage geometrique "
    //    - ShapeRepresentationRelationship + Transformation ou non
    // --------------------------------------------------------------

  else if (start->IsKind(STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation))) {
    DeclareAndCast(StepShape_ContextDependentShapeRepresentation,CDSR,start);
    shbinder =  TransferEntity(CDSR,TP);
  }

  else if (start->IsKind (STANDARD_TYPE(StepRepr_ShapeRepresentationRelationship)) ) {
    //  REPRESENTATION_RELATIONSHIP et la famille

    DeclareAndCast(StepRepr_ShapeRepresentationRelationship,und,start);
    shbinder =  TransferEntity(und,TP);
  }

  else if (start->IsKind (STANDARD_TYPE(StepGeom_GeometricRepresentationItem)) ) {
    // Here starts the entity to be treated : Shape Representation Subtype
  // It can be also other Root entities
    DeclareAndCast(StepGeom_GeometricRepresentationItem,git,start);
    shbinder = TransferEntity(git, TP, isManifold);
  }
  else if (start->IsKind(STANDARD_TYPE(StepRepr_MappedItem))) {
    DeclareAndCast(StepRepr_MappedItem,mapit,start);
    shbinder=  TransferEntity(mapit,TP);
  }
  else if (start->IsKind(STANDARD_TYPE(StepShape_FaceSurface))) {
    DeclareAndCast(StepShape_FaceSurface,fs,start);
    shbinder =  TransferEntity(fs,TP);
  }

//  if (!shbinder.IsNull()) TP->Bind(start,binder);
  
  return shbinder;
}
// ============================================================================
// Method  : STEPControl_ActorRead::PrepareUnits
// Purpose : Set the unit conversion factors
// ============================================================================

void STEPControl_ActorRead::PrepareUnits(const Handle(StepRepr_Representation)& rep,
                                         const Handle(Transfer_TransientProcess)& TP)
{
  mySRContext = rep;
  
  Standard_Integer stat1, stat2 = 0;  // sera alimente par STEPControl_Unit
  if (rep.IsNull()) {
    ResetUnits();
    return;
  }
  
  // Get Units Applied to this model
  Handle(StepRepr_RepresentationContext) theRepCont = rep->ContextOfItems();
  if (theRepCont.IsNull()) {
    TP->AddWarning(rep,"Bad RepresentationContext, default unit taken");
    ResetUnits();
    return;
  }

  // --------------------------------------------------
  // Complex ENTITY : GeometricRepresentationContext &&
  //                  GlobalUnitAssignedContext
  // --------------------------------------------------

  STEPConstruct_UnitContext myUnit;
  Handle(StepRepr_GlobalUnitAssignedContext)        theGUAC;
  Handle(StepRepr_GlobalUncertaintyAssignedContext) aTol;

  if (theRepCont->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext))) {
    DeclareAndCast(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext, theGRCAGAUC,theRepCont);
    theGUAC = theGRCAGAUC->GlobalUnitAssignedContext();
  }

  // ----------------------------------------------------
  // Complex ENTITY : GeometricRepresentationContext   &&
  //                  GlobalUnitAssignedContext        &&
  //                  GlobalUncertaintyAssignedContext
  // ----------------------------------------------------

  if (theRepCont->IsKind(STANDARD_TYPE(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx))) {
    DeclareAndCast(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx,
		   theGRCAGAUC,theRepCont);
    theGUAC = theGRCAGAUC->GlobalUnitAssignedContext();
    aTol    = theGRCAGAUC->GlobalUncertaintyAssignedContext();
  }


  // ----------------------------------------------------
  // Decoding and Setting the Values
  // ----------------------------------------------------

  if (!theGUAC.IsNull()) {
    stat1 = myUnit.ComputeFactors(theGUAC);
    Standard_Integer anglemode = Interface_Static::IVal("step.angleunit.mode");
    Standard_Real angleFactor = ( anglemode == 0 ? myUnit.PlaneAngleFactor() :
				  anglemode == 1 ? 1. : M_PI/180. );
    UnitsMethods::InitializeFactors(myUnit.LengthFactor(),
				    angleFactor,
				    myUnit.SolidAngleFactor());
    if (stat1 != 0) TP->AddWarning (theRepCont,myUnit.StatusMessage(stat1));
  }

  if (!aTol.IsNull()) {
    stat2 = myUnit.ComputeTolerance (aTol);
    if (stat2 != 0) TP->AddWarning (theRepCont,myUnit.StatusMessage(stat2));
  }

//  myPrecision = Precision::Confusion();
  if (Interface_Static::IVal("read.precision.mode") == 1)  //:i1 gka S4136 05.04.99
    myPrecision = Interface_Static::RVal("read.precision.val");
  else if (myUnit.HasUncertainty())
    myPrecision = myUnit.Uncertainty() * myUnit.LengthFactor();
  else {
    TP->AddWarning(theRepCont,"No Length Uncertainty, value of read.precision.val is taken");
    myPrecision = Interface_Static::RVal("read.precision.val");
  }
  myMaxTol = Max ( myPrecision, Interface_Static::RVal("read.maxprecision.val") );
  // Assign uncertainty
#ifdef TRANSLOG
  if (TP->TraceLevel() > 1) 
    TP->Messenger() <<"  Cc1ToTopoDS : Length Unit = "<<myUnit.LengthFactor()<<"  Tolerance CASCADE = "<<myPrecision<<endl;
#endif
}

//=======================================================================
//function : ResetUnits
//purpose  : 
//=======================================================================

void  STEPControl_ActorRead::ResetUnits ()
{
  UnitsMethods::InitializeFactors ( 1, 1, 1 );
  myPrecision = Interface_Static::RVal("read.precision.val");
  myMaxTol = Max ( myPrecision, Interface_Static::RVal("read.maxprecision.val") );
}

//=======================================================================
//function : ComputeTransformation
//purpose  : 
//=======================================================================

//:S4136 abv 20 Apr 99: as1ug.stp: compute transformation taking units into account
Standard_Boolean STEPControl_ActorRead::ComputeTransformation (const Handle(StepGeom_Axis2Placement3d) &Origin, 
							       const Handle(StepGeom_Axis2Placement3d) &Target,
							       const Handle(StepRepr_Representation) &OrigContext, 
							       const Handle(StepRepr_Representation) &TargContext,
							       const Handle(Transfer_TransientProcess) &TP,
							       gp_Trsf &Trsf)
{
  Trsf = gp_Trsf();  // reinit
  if ( Origin.IsNull() || Target.IsNull() ) return Standard_False;

  //:abv 31.10.01: TEST_MCI_2.step: check that Ax1 and Ax2 belong to 
  // corresponding reps and fix case of inversion error
  Handle(StepGeom_Axis2Placement3d) org = Origin;
  Handle(StepGeom_Axis2Placement3d) trg = Target;
  Standard_Integer code1=0, code2=0, i;
  for ( i=1; code1 != 1 && i <= OrigContext->NbItems(); i++ ) {
    if ( OrigContext->ItemsValue(i) == org ) code1 = 1;
    else if ( OrigContext->ItemsValue(i) == trg ) code1 = -1;
  }
  for ( i=1; code2 != 1 && i <= TargContext->NbItems(); i++ ) {
    if ( TargContext->ItemsValue(i) == org ) code2 = -1;
    else if ( TargContext->ItemsValue(i) == trg ) code2 = 1;
  }
  if ( code1 != 1 && code2 != 1 ) {
    if ( code1 == -1 && code2 == -1 ) {
      Handle(StepGeom_Axis2Placement3d) swp = org; org = trg; trg = swp;
      TP->AddWarning ( org, "Axis placements are swapped in SRRWT; corrected" );
    }
    else {
      TP->AddWarning ( ( code1 == 1 ? trg : org ), 
                       "Axis placement used by SRRWT does not belong to corresponding representation" );
    }
  }
  
  // translate axis_placements taking units into account 
  Handle(StepRepr_Representation) oldSRContext = mySRContext;
  if ( OrigContext  != oldSRContext ) PrepareUnits(OrigContext,TP);
  Handle(Geom_Axis2Placement) theOrig = StepToGeom::MakeAxis2Placement (org);
  if ( TargContext  != OrigContext  ) PrepareUnits(TargContext,TP);
  Handle(Geom_Axis2Placement) theTarg = StepToGeom::MakeAxis2Placement (trg);
  if ( oldSRContext != TargContext  ) PrepareUnits(oldSRContext,TP);

  gp_Ax3 ax3Orig(theOrig->Ax2());
  gp_Ax3 ax3Targ(theTarg->Ax2());

  //  ne pas se tromper de sens !
  Trsf.SetTransformation(ax3Targ, ax3Orig);
  return Trsf.Form() != gp_Identity;
}

//=======================================================================
//function : ReadSRRWT
//purpose  : 
//=======================================================================

//:j2 abv 22 Oct 98: auxiliary function: reading transformation from SRRWT
Standard_Boolean STEPControl_ActorRead::ComputeSRRWT (const Handle(StepRepr_RepresentationRelationship) &SRR,
						      const Handle(Transfer_TransientProcess) &TP,
						      gp_Trsf &Trsf)
{
  Trsf = gp_Trsf(); // init
  
  DeclareAndCast(StepRepr_ShapeRepresentationRelationshipWithTransformation,srwt,SRR);
  if ( srwt.IsNull() ) return Standard_False;

  StepRepr_Transformation SelectTrans = srwt->TransformationOperator();
  
  // cartesian transformation
  Handle(StepGeom_CartesianTransformationOperator3d) CartOp = 
    Handle(StepGeom_CartesianTransformationOperator3d)::DownCast(SelectTrans.Value());
  if ( ! CartOp.IsNull() ) {
    // reset units (by Rep2 - ?)
    Handle(StepRepr_Representation) oldSRContext = mySRContext;
    if ( SRR->Rep2() != oldSRContext ) PrepareUnits(SRR->Rep2(),TP);
    StepToGeom::MakeTransformation3d (CartOp, Trsf);
    if ( SRR->Rep2() != oldSRContext ) PrepareUnits(oldSRContext,TP);
    return Trsf.Form() != gp_Identity;
  }

  // item-defined transformation
  Handle(StepRepr_ItemDefinedTransformation) ItemDef =
    SelectTrans.ItemDefinedTransformation();
  if ( ItemDef.IsNull() ) return Standard_False;
  
  Handle(StepGeom_Axis2Placement3d) Ax1 =
    Handle(StepGeom_Axis2Placement3d)::DownCast(ItemDef->TransformItem1());
  Handle(StepGeom_Axis2Placement3d) Ax2 =
    Handle(StepGeom_Axis2Placement3d)::DownCast(ItemDef->TransformItem2());
  if ( Ax1.IsNull() || Ax2.IsNull() ) return Standard_False;
  return ComputeTransformation ( Ax1, Ax2, SRR->Rep1(), SRR->Rep2(), TP, Trsf);
}

//=======================================================================
// Method  : closeIDEASShell
// Purpose : Attempts to close the passed Shell with the passed closing
//           Shells. Cuts redundant Faces from the closing Shells if any
//=======================================================================

TopoDS_Shell STEPControl_ActorRead::closeIDEASShell(const TopoDS_Shell& shell,
                                                    const TopTools_ListOfShape& closingShells) {

  // Make Shell to provide closeness adjustments
  TopoDS_Shell result;
  BRep_Builder brepBuilder;
  brepBuilder.MakeShell(result);
        
  // Firstly, add all existing faces to the new Shell
  TopExp_Explorer currentFExp(shell, TopAbs_FACE);
  for ( ; currentFExp.More(); currentFExp.Next() ) {
    TopoDS_Face currentFace = TopoDS::Face( currentFExp.Current() ); 
    brepBuilder.Add(result, currentFace);
  }

  TopTools_ListIteratorOfListOfShape itL(closingShells);
  TopTools_ListOfShape closingFaces;

  // Then add the closing faces
  for ( ; itL.More(); itL.Next() ) {
    TopoDS_Shape currentClosing = itL.Value();
    TopExp_Explorer faceExp(currentClosing, TopAbs_FACE);

    for ( ; faceExp.More(); faceExp.Next() ) {
      TopoDS_Face currentFace = TopoDS::Face( faceExp.Current() ); 
      brepBuilder.Add(result, currentFace);
      // Store each added closing face for subsequent processing
      closingFaces.Append(currentFace);
    }
  }

  // Check if the result is closed
  BRepCheck_Shell checker( TopoDS::Shell(result) );
  BRepCheck_Status checkStatus = checker.Closed();
  if (checkStatus == BRepCheck_NoError)
    result.Closed(Standard_True);
  else
    return shell; // Cannot close this shell, skip it so...

  // Try to remove redundant Faces
  for ( itL.Initialize(closingFaces); itL.More(); itL.Next() ) {
    TopoDS_Face currentFace = TopoDS::Face( itL.Value() );
    // Remove face to see if Shell is still closed
    brepBuilder.Remove(result, currentFace);
    BRepCheck_Shell subChecker( TopoDS::Shell(result) );
    BRepCheck_Status subCheckStatus = subChecker.Closed();
    // If Shell becomes open, just put the deleted face back
    if (subCheckStatus != BRepCheck_NoError)
      brepBuilder.Add(result, currentFace);
    else {
      #ifdef OCCT_DEBUG
      cout << "Redundant closing face detected: REMOVED from shell";
      #endif
    }
  }

  return result;

}

//=======================================================================
// Method  : computeIDEASClosings
// Purpose : For each Shell from the compound passed (comp), find all
//           non-manifold adjacent Shells and put the results into
//           the passed map (shellClosingMap)
//=======================================================================

void STEPControl_ActorRead::computeIDEASClosings(const TopoDS_Compound& comp,
                                                 TopTools_IndexedDataMapOfShapeListOfShape& shellClosingsMap) {
  TopExp_Explorer shellExpA(comp, TopAbs_SHELL);

  for ( ; shellExpA.More(); shellExpA.Next() ) {
    TopoDS_Shape shellA = shellExpA.Current();
    TopExp_Explorer shellExpB(comp, TopAbs_SHELL);
    TopTools_ListOfShape closingShells;

    for ( ; shellExpB.More(); shellExpB.Next() ) {
      TopoDS_Shape shellB = shellExpB.Current();
      if ( shellA.IsSame(shellB) )
        continue;
      // Check whether ShellB is non-manifold and adjacent to ShellA.
      // If so, ShellA has a chance to be closed with ShellB
      if ( myNMTool.IsSuspectedAsClosing(shellA, shellB) )
        closingShells.Append(shellB);
    }
      
    if ( !closingShells.IsEmpty() )
      shellClosingsMap.Add(shellA, closingShells);
  }
}
