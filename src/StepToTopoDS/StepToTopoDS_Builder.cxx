// Created on: 1995-01-03
// Created by: Frederic MAUPAS
// Copyright (c) 1995-1999 Matra Datavision
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

//:i6 abv 17 Sep 98: ProSTEP TR9 r0601-ct.stp: to be able read GeometricSet
//gka 11.01.99 file PRO7755.stp #2018: work-around error in BRepLib_MakeFace
//:n4 abv 12.02.99: S4132: treatment of GEOMETRIC_SETs implemented
//:o7 abv 18.02.99: bm1_sy_fuel.stp #1427(items 1,2) protection against null entity
//abv,gka 05.04.99 S4136: parameter names changed; avoid extra call to FixSameParameter

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepLib.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Interface_Static.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <Precision.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <StdFail_NotDone.hxx>
#include <STEPControl_ActorRead.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_CompositeCurve.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_CurveBoundedSurface.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepGeom_RectangularCompositeSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SurfacePatch.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_ConnectedEdgeSet.hxx>
#include <StepShape_EdgeBasedWireframeModel.hxx>
#include <StepShape_FaceBasedSurfaceModel.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <StepShape_HArray1OfConnectedEdgeSet.hxx>
#include <StepShape_HArray1OfConnectedFaceSet.hxx>
#include <StepShape_HArray1OfEdge.hxx>
#include <StepShape_HArray1OfFace.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <StepShape_Shell.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepToGeom.hxx>
#include <StepToTopoDS_Builder.hxx>
#include <StepToTopoDS_DataMapOfTRI.hxx>
#include <StepToTopoDS_NMTool.hxx>
#include <StepToTopoDS_Tool.hxx>
#include <StepToTopoDS_TranslateCompositeCurve.hxx>
#include <StepToTopoDS_TranslateCurveBoundedSurface.hxx>
#include <StepToTopoDS_TranslateEdge.hxx>
#include <StepToTopoDS_TranslateFace.hxx>
#include <StepToTopoDS_TranslateShell.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>

#include <stdio.h>
static void ResetPreci (const TopoDS_Shape& S, Standard_Real maxtol)
{
  //:S4136
  Standard_Integer modetol = Interface_Static::IVal("read.maxprecision.mode");
  if (modetol) {
    ShapeFix_ShapeTolerance STU;
    STU.LimitTolerance (S,Precision::Confusion(),maxtol);
  }
}

// ============================================================================
// Method  : StepToTopoDS_Builder::StepToTopoDS_Builder
// Purpose : Empty constructor
// ============================================================================

StepToTopoDS_Builder::StepToTopoDS_Builder()
: myError(StepToTopoDS_BuilderOther)
{
  done = Standard_False;
}

// ============================================================================
// Method  : Init
// Purpose : Init with a ManifoldSolidBrep
// ============================================================================

void StepToTopoDS_Builder::Init
(const Handle(StepShape_ManifoldSolidBrep)& aManifoldSolid,
 const Handle(Transfer_TransientProcess)& TP,
 const Message_ProgressRange& theProgress)
{
  Message_Messenger::StreamBuffer sout = TP->Messenger()->SendInfo();
  // Initialisation of the Tool

  StepToTopoDS_Tool         myTool;
  StepToTopoDS_DataMapOfTRI aMap;

  myTool.Init(aMap, TP);

  // Start Mapping

  Handle(StepShape_ConnectedFaceSet) aShell;
  aShell = aManifoldSolid->Outer();

  StepToTopoDS_TranslateShell myTranShell;
  myTranShell.SetPrecision(Precision());
  myTranShell.SetMaxTol(MaxTol());
  // Non-manifold topology is not referenced by ManifoldSolidBrep (ssv; 14.11.2010)
  StepToTopoDS_NMTool dummyNMTool;
  myTranShell.Init(aShell, myTool, dummyNMTool, theProgress);

  if (myTranShell.IsDone()) {
    TopoDS_Shape Sh = myTranShell.Value();
    Sh.Closed(Standard_True);
    //BRepLib::SameParameter(Sh);
    TopoDS_Solid S;
    BRep_Builder B;
    B.MakeSolid(S);
    B.Add(S,Sh);
    myResult = S;
    myError  = StepToTopoDS_BuilderDone;
    done     = Standard_True;

    // Get Statistics :

    if ( TP->TraceLevel() > 2 )
    {
      sout << "Geometric Statitics : " << std::endl;
      sout << "   Surface Continuity : - C0 : " << myTool.C0Surf() << std::endl;
      sout << "                        - C1 : " << myTool.C1Surf() << std::endl;
      sout << "                        - C2 : " << myTool.C2Surf() << std::endl;
      sout << "   Curve Continuity :   - C0 : " << myTool.C0Cur3() << std::endl;
      sout << "                        - C1 : " << myTool.C1Cur3() << std::endl;
      sout << "                        - C2 : " << myTool.C2Cur3() << std::endl;
      sout << "   PCurve Continuity :  - C0 : " << myTool.C0Cur2() << std::endl;
      sout << "                        - C1 : " << myTool.C1Cur2() << std::endl;
      sout << "                        - C2 : " << myTool.C2Cur2() << std::endl;
    }

//:S4136    ShapeFix::SameParameter (S,Standard_False);
    ResetPreci (S, MaxTol());
  }
  else {
    TP->AddWarning(aShell," OuterShell from ManifoldSolidBrep not mapped to TopoDS");
    myError  = StepToTopoDS_BuilderOther;
    done     = Standard_False;
  }
}

// ============================================================================
// Method  : Init
// Purpose : Init with a BrepWithVoids
// ============================================================================

void StepToTopoDS_Builder::Init
(const Handle(StepShape_BrepWithVoids)& aBRepWithVoids,
 const Handle(Transfer_TransientProcess)& TP,
 const Message_ProgressRange& theProgress)
{
  Message_Messenger::StreamBuffer sout = TP->Messenger()->SendInfo();
 // Initialisation of the Tool
  StepToTopoDS_Tool         myTool;
  StepToTopoDS_DataMapOfTRI aMap;

  myTool.Init(aMap, TP);

  // Start Mapping

  Standard_Integer Nb = aBRepWithVoids->NbVoids();
  Handle(StepShape_ClosedShell) aCShell;
  TopoDS_Solid S;
  TopoDS_Shape Sh;
  BRep_Builder B;
  B.MakeSolid(S);
  
  Message_ProgressScope PS (theProgress, "Shell", Nb+1);

  StepToTopoDS_TranslateShell myTranShell;

  myTranShell.SetPrecision(Precision());//gka
  myTranShell.SetMaxTol(MaxTol());
  // OuterBound

  aCShell = Handle(StepShape_ClosedShell)::DownCast(aBRepWithVoids->Outer());
  // Non-manifold topology is not referenced by BrepWithVoids (ssv; 14.11.2010)
  StepToTopoDS_NMTool dummyNMTool;
  myTranShell.Init(aCShell, myTool, dummyNMTool, PS.Next());

  if (myTranShell.IsDone()) {
    Sh = myTranShell.Value();
    Sh.Closed(Standard_True);
    //BRepLib::SameParameter(Sh);
    B.MakeSolid(S);
    B.Add(S,Sh);
    myResult = S;
    myError  = StepToTopoDS_BuilderDone;
    done     = Standard_True;
  }
  else {
    TP->AddWarning(aCShell," OuterShell from BrepWithVoids not mapped to TopoDS");
    myError  = StepToTopoDS_BuilderOther;
    done     = Standard_False;
    return;
  }

  // Voids

  for (Standard_Integer i=1; i<=Nb && PS.More(); i++) {

    aCShell = aBRepWithVoids->VoidsValue(i);
    myTranShell.Init(aCShell, myTool, dummyNMTool, PS.Next());
    if (myTranShell.IsDone()) {
      Sh = myTranShell.Value();
      Sh.Closed(Standard_True);

      //:e0 abv 25 Mar 98: void should be an OrientedShell 
      Handle(StepShape_OrientedClosedShell) aOCShell = aBRepWithVoids->VoidsValue(i);
      if ( ! aOCShell.IsNull() && ! aOCShell->Orientation() ) Sh.Reverse();
      
      //BRepLib::SameParameter(Sh);
      B.Add(S,Sh);
    }
    else {
      TP->AddWarning
	(aCShell," A Void from BrepWithVoids not mapped to TopoDS");
    }
  }
  myResult = S;


  // Get Statistics :
  
  if ( TP->TraceLevel() > 2 )
  {
    sout << "Geometric Statitics : " << std::endl;
    sout << "   Surface Continuity : - C0 : " << myTool.C0Surf() << std::endl;
    sout << "                        - C1 : " << myTool.C1Surf() << std::endl;
    sout << "                        - C2 : " << myTool.C2Surf() << std::endl;
    sout << "   Curve Continuity :   - C0 : " << myTool.C0Cur3() << std::endl;
    sout << "                        - C1 : " << myTool.C1Cur3() << std::endl;
    sout << "                        - C2 : " << myTool.C2Cur3() << std::endl;
    sout << "   PCurve Continuity :  - C0 : " << myTool.C0Cur2() << std::endl;
    sout << "                        - C1 : " << myTool.C1Cur2() << std::endl;
    sout << "                        - C2 : " << myTool.C2Cur2() << std::endl;
  }

//:S4136  ShapeFix::SameParameter (S,Standard_False);
  ResetPreci (S, MaxTol());
}

// ============================================================================
// Method  : Init
// Purpose : Init with a FacetedBrep
// ============================================================================

void StepToTopoDS_Builder::Init(const Handle(StepShape_FacetedBrep)& aFB,
                                const Handle(Transfer_TransientProcess)& TP,
                                const Message_ProgressRange& theProgress)
{
  // Initialisation of the Tool

  StepToTopoDS_Tool         myTool;
  StepToTopoDS_DataMapOfTRI aMap;

  myTool.Init(aMap, TP);

  // Start Mapping

  Handle(StepShape_ClosedShell) aCShell;
  aCShell = Handle(StepShape_ClosedShell)::DownCast(aFB->Outer());
  TopoDS_Shape Sh;

  StepToTopoDS_TranslateShell myTranShell; 
  myTranShell.SetPrecision(Precision()); //gka
  myTranShell.SetMaxTol(MaxTol());  
  // Non-manifold topology is not referenced by FacetedBrep (ss; 14.11.2010)
  StepToTopoDS_NMTool dummyNMTool;
  myTranShell.Init(aCShell, myTool, dummyNMTool, theProgress);

  if (myTranShell.IsDone()) {
    Sh = myTranShell.Value();
    Sh.Closed(Standard_True);
    //BRepLib::SameParameter(Sh);
    myTool.ClearEdgeMap();
    myTool.ClearVertexMap();
    TopoDS_Solid S;
    BRep_Builder B;
    B.MakeSolid(S);
    B.Add(S,Sh);
    myResult = S;
    myError  = StepToTopoDS_BuilderDone;
    done     = Standard_True;
  }
  else {
    TP->AddWarning
      (aCShell," OuterShell from FacetedBrep not mapped to TopoDS");
    myError  = StepToTopoDS_BuilderOther;
    done     = Standard_True;
  }
}

// ============================================================================
// Method  : Init
// Purpose : Init with a FacetedBrepAndBrepWithVoids
// ============================================================================

void StepToTopoDS_Builder::Init
(const Handle(StepShape_FacetedBrepAndBrepWithVoids)& aFBABWV,
 const Handle(Transfer_TransientProcess)& TP,
 const Message_ProgressRange& theProgress)
{
  // Initialisation of the Tool

  StepToTopoDS_Tool         myTool;
  StepToTopoDS_DataMapOfTRI aMap;

  myTool.Init(aMap, TP);

  // Start Mapping

  Handle(StepShape_ClosedShell) aCShell;
  aCShell = Handle(StepShape_ClosedShell)::DownCast(aFBABWV->Outer());
  TopoDS_Shape Sh;

  Message_ProgressScope aPSRoot(theProgress, NULL, 2);

  StepToTopoDS_TranslateShell myTranShell;
  myTranShell.SetPrecision(Precision()); //gka
  myTranShell.SetMaxTol(MaxTol());
  // Non-manifold topology is not referenced by FacetedBrepAndBrepWithVoids (ss; 14.11.2010)
  StepToTopoDS_NMTool dummyNMTool;
  myTranShell.Init(aCShell, myTool, dummyNMTool, aPSRoot.Next());

  if (myTranShell.IsDone()) {
    Sh = myTranShell.Value();
    Sh.Closed(Standard_True);
    //BRepLib::SameParameter(Sh);
    TopoDS_Solid S;
    BRep_Builder B;
    B.MakeSolid(S);
    B.Add(S,Sh);
    Standard_Integer Nb, i;
    Nb = aFBABWV->NbVoids();
    Message_ProgressScope aPS (aPSRoot.Next(), NULL, Nb);
    for ( i=1; i<=Nb && aPS.More(); i++) {
      aCShell = aFBABWV->VoidsValue(i);
      myTranShell.Init(aCShell, myTool, dummyNMTool, aPS.Next());
      if (myTranShell.IsDone()) {
        Sh = myTranShell.Value();
        Sh.Closed(Standard_True);
        //BRepLib::SameParameter(Sh);
        B.Add(S, Sh);
      }
      else {
	TP->AddWarning
	  (aCShell," A Void from FacetedBrepAndBrepWithVoids not mapped to TopoDS");
      }
    }
    myResult = S;
    myError  = StepToTopoDS_BuilderDone;
    done     = Standard_True;
  }
  else {
    TP->AddWarning
      (aCShell," OuterShell from FacetedBrepAndBrepWithVoids not mapped to TopoDS");
    done    = Standard_False;
    myError = StepToTopoDS_BuilderOther;
  }
}

// ============================================================================
// Method  : Init
// Purpose : Init with a ShellBasedSurfaceModel
// ============================================================================

void StepToTopoDS_Builder::Init
(const Handle(StepShape_ShellBasedSurfaceModel)& aSBSM,
 const Handle(Transfer_TransientProcess)& TP,
 StepToTopoDS_NMTool& NMTool,
 const Message_ProgressRange& theProgress)
{
  Message_Messenger::StreamBuffer sout = TP->Messenger()->SendInfo();
  // Initialisation of the Tool

  StepToTopoDS_Tool         myTool;
  StepToTopoDS_DataMapOfTRI aMap;

  myTool.Init(aMap, TP);

  // Start Mapping

  Standard_Integer Nb = aSBSM->NbSbsmBoundary();
  StepShape_Shell aShell;
  //Handle(StepShape_VertexShell) aVertexShell;
  Handle(StepShape_OpenShell)   aOpenShell;
  Handle(StepShape_ClosedShell) aClosedShell;
  TopoDS_Compound S;
  //TopoDS_Shape Sh;
  TopoDS_Shape Shl;
  BRep_Builder B;
  B.MakeCompound(S);
  StepToTopoDS_TranslateShell myTranShell;
  
  myTranShell.SetPrecision(Precision());
  myTranShell.SetMaxTol(MaxTol());

  Message_ProgressScope PS ( theProgress, "Shell", Nb);
  for (Standard_Integer i = 1; i <= Nb && PS.More(); i++)
  {
    Message_ProgressRange aRange = PS.Next();
    aShell = aSBSM->SbsmBoundaryValue(i);
    aOpenShell = aShell.OpenShell();
    aClosedShell = aShell.ClosedShell();
    if (!aOpenShell.IsNull()) {
      myTranShell.Init(aOpenShell, myTool, NMTool, aRange);
      if (myTranShell.IsDone()) {
        Shl = TopoDS::Shell(myTranShell.Value());
        Shl.Closed(Standard_False);
        B.Add(S, Shl);
      }
      else {
        TP->AddWarning
          (aOpenShell, " OpenShell from ShellBasedSurfaceModel not mapped to TopoDS");
      }
    }
    else if (!aClosedShell.IsNull()) {
      myTranShell.Init(aClosedShell, myTool, NMTool, aRange);
      if (myTranShell.IsDone()) {
        Shl = TopoDS::Shell(myTranShell.Value());
        Shl.Closed(Standard_True);
        B.Add(S, Shl);
      }
      else {
        TP->AddWarning
          (aClosedShell, " ClosedShell from ShellBasedSurfaceModel not mapped to TopoDS");
      }
    }
  }
  if(Nb>1)
    myResult = S;
  else
    myResult = Shl;
  myError  = StepToTopoDS_BuilderDone;
  done     = Standard_True;

  // Get Statistics :
  
  if ( TP->TraceLevel() > 2 )
  {
    sout << "Geometric Statitics : " << std::endl;
    sout << "   Surface Continuity : - C0 : " << myTool.C0Surf() << std::endl;
    sout << "                        - C1 : " << myTool.C1Surf() << std::endl;
    sout << "                        - C2 : " << myTool.C2Surf() << std::endl;
    sout << "   Curve Continuity :   - C0 : " << myTool.C0Cur3() << std::endl;
    sout << "                        - C1 : " << myTool.C1Cur3() << std::endl;
    sout << "                        - C2 : " << myTool.C2Cur3() << std::endl;
    sout << "   PCurve Continuity :  - C0 : " << myTool.C0Cur2() << std::endl;
    sout << "                        - C1 : " << myTool.C1Cur2() << std::endl;
    sout << "                        - C2 : " << myTool.C2Cur2() << std::endl;
  }

//:S4136  ShapeFix::SameParameter (S,Standard_False);
  ResetPreci (S, MaxTol());
  ResetPreci (Shl, MaxTol()); //skl
}

// ============================================================================
// Method  : Init
// Purpose : Init with a EdgeBasedWireframeModel
// ============================================================================

void StepToTopoDS_Builder::Init (const Handle(StepShape_EdgeBasedWireframeModel)& aEBWM,
                                 const Handle(Transfer_TransientProcess)& TP)
{
  myResult.Nullify();
  
  Handle(StepShape_HArray1OfConnectedEdgeSet) boundary = aEBWM->EbwmBoundary();
  if ( boundary.IsNull() || boundary->Length() <1 ) {
    TP->AddWarning ( aEBWM, "List of boundaries is empty" );
    return;
  }

  StepToTopoDS_Tool myTool;
  StepToTopoDS_DataMapOfTRI aMap;
  myTool.Init(aMap, TP);

  StepToTopoDS_TranslateEdge myTranEdge;
  myTranEdge.SetPrecision(Precision());
  myTranEdge.SetMaxTol(MaxTol());
  
  TopoDS_Compound C;
  BRep_Builder B;
  B.MakeCompound ( C );

  StepToTopoDS_NMTool dummyNMTool;
  
  for ( Standard_Integer i=1; i <= boundary->Length(); i++ ) {
    Handle(StepShape_ConnectedEdgeSet) ces = boundary->Value(i);
    if ( ces.IsNull() ) continue;
    Handle(StepShape_HArray1OfEdge) edges = ces->CesEdges();
    if ( edges.IsNull() || edges->Length() <1 ) {
      TP->AddWarning ( ces, "No edges in connected_edge_set" );
      continue;
    }
    TopoDS_Wire W;
    for ( Standard_Integer j=1; j <= edges->Length(); j++ ) {
      myTranEdge.Init (edges->Value(j), myTool, dummyNMTool);
      if ( ! myTranEdge.IsDone() ) continue;
      TopoDS_Edge E = TopoDS::Edge(myTranEdge.Value());
      if (E.IsNull()) continue;  // NULL, on saute
      if ( W.IsNull() ) B.MakeWire ( W );
      B.Add ( W, E );
    }
    if ( W.IsNull() ) continue;
    W.Closed (BRep_Tool::IsClosed (W));
    B.Add ( C, W );
    if ( myResult.IsNull() ) myResult = W;
    else myResult = C;
  }
  
  myError  = ( myResult.IsNull() ? StepToTopoDS_BuilderDone : StepToTopoDS_BuilderOther );
  done     = ! myResult.IsNull();

  ResetPreci (myResult, MaxTol());
}

// ============================================================================
// Method  : Init
// Purpose : Init with a FaceBasedSurfaceModel
// ============================================================================

void StepToTopoDS_Builder::Init (const Handle(StepShape_FaceBasedSurfaceModel)& aFBSM,
                                 const Handle(Transfer_TransientProcess)& TP)
{
  myResult.Nullify();
  
  Handle(StepShape_HArray1OfConnectedFaceSet) boundary = aFBSM->FbsmFaces();
  if ( boundary.IsNull() || boundary->Length() <1 ) {
    TP->AddWarning ( aFBSM, "List of faces is empty" );
    return;
  }

  StepToTopoDS_Tool myTool;
  StepToTopoDS_DataMapOfTRI aMap;
  myTool.Init(aMap, TP);

  StepToTopoDS_TranslateFace myTranFace;
  myTranFace.SetPrecision(Precision());
  myTranFace.SetMaxTol(MaxTol());
  
  TopoDS_Compound C;
  BRep_Builder B;
  B.MakeCompound ( C );

  // Non-manifold topology is not mapped via FaceBasedSurfaceModel (ssv; 14.11.2010)
  StepToTopoDS_NMTool dummyNMTool;
  
  for ( Standard_Integer i=1; i <= boundary->Length(); i++ ) {
    Handle(StepShape_ConnectedFaceSet) cfs = boundary->Value(i);
    if ( cfs.IsNull() ) continue;
    Handle(StepShape_HArray1OfFace) faces = cfs->CfsFaces();
    if ( faces.IsNull() || faces->Length() <1 ) {
      TP->AddWarning ( cfs, "No faces in connected_face_set" );
      continue;
    }
    TopoDS_Shell S;
    for ( Standard_Integer j=1; j <= faces->Length(); j++ ) {
      Handle(StepShape_FaceSurface) fs = Handle(StepShape_FaceSurface)::DownCast ( faces->Value(j) );
      myTranFace.Init(fs, myTool, dummyNMTool);
      if ( ! myTranFace.IsDone() ) continue;
      TopoDS_Face F = TopoDS::Face(myTranFace.Value());
      if (F.IsNull()) continue;  // NULL, on saute
      if ( S.IsNull() ) B.MakeShell ( S );
      B.Add ( S, F );
    }
    if ( S.IsNull() ) continue;
    S.Closed (BRep_Tool::IsClosed (S));
    B.Add ( C, S );
    if ( myResult.IsNull() ) myResult = S;
    else myResult = C;
  }
  
  myError  = ( myResult.IsNull() ? StepToTopoDS_BuilderDone : StepToTopoDS_BuilderOther );
  done     = ! myResult.IsNull();

  ResetPreci (myResult, MaxTol());
}


// ***start DTH Apr/6
// ============================================================================
// Method  : Init
// Purpose : Init with a GeometricCurveSet
// ============================================================================
//:i6 abv 17 Sep 98: ProSTEP TR9 r0601-ct.stp: to be able read GS: GeometricCurveSet -> GeometricSet

static TopoDS_Face TranslateBoundedSurf (const Handle(StepGeom_Surface) &surf,
                                         const Standard_Real TolDegen)
{
  TopoDS_Face res;

  Handle(Geom_Surface) theSurf = StepToGeom::MakeSurface (surf);
  if (theSurf.IsNull() || //:i6: protection
      !theSurf->IsKind(STANDARD_TYPE(Geom_BoundedSurface))) return res;

  BRepBuilderAPI_MakeFace myMkFace;
  Handle(Geom_RectangularTrimmedSurface) RS = 
                    Handle(Geom_RectangularTrimmedSurface)::DownCast(theSurf);

  if (!RS.IsNull())
  {
    Standard_Real umin, umax, vmin, vmax;
    theSurf->Bounds(umin, umax, vmin, vmax);

    myMkFace.Init(RS->BasisSurface(), umin, umax, vmin, vmax, TolDegen);
  }
  else
  {
    myMkFace.Init(theSurf, Standard_True, TolDegen);
  }

  return myMkFace.Face();
}

void StepToTopoDS_Builder::Init
(const Handle(StepShape_GeometricSet)& GCS,
 const Handle(Transfer_TransientProcess)& TP,
 const Handle(Transfer_ActorOfTransientProcess)& RA,
 const Standard_Boolean isManifold,
 const Message_ProgressRange& theProgress)
{
  // Start Mapping
  TopoDS_Compound S;
  BRep_Builder B;
  B.MakeCompound(S);
  TopoDS_Edge theEdge;
  Standard_Integer i;
  Standard_Real preci = Precision();   //gka
  Standard_Real maxtol = MaxTol();
  Standard_Integer nbElem = GCS->NbElements();
  Message_ProgressScope aPS(theProgress, NULL, nbElem);
  for (i = 1; i <= nbElem && aPS.More(); i++)
  {
    Message_ProgressRange aRange = aPS.Next();
    StepShape_GeometricSetSelect aGSS = GCS->ElementsValue(i);
    Handle(Standard_Transient) ent = aGSS.Value();

    TopoDS_Shape res = TransferBRep::ShapeResult ( TP, ent );
    if ( ! res.IsNull() ) { // already translated
      B.Add ( S, res );
      continue;
    }
    //:o7 abv 18 Feb 99: bm1_sy_fuel.stp #1427(1,2) protection against null entity
    if ( ent.IsNull() ) {
      char buff[100];
      sprintf ( buff, "Entity %d is a Null entity", i );
      TP->AddWarning (GCS,buff);
      continue;
    } 
    // try curve
    else if ( ent->IsKind(STANDARD_TYPE(StepGeom_Curve)) ) {
      Handle(StepGeom_Curve) aCrv = Handle(StepGeom_Curve)::DownCast ( ent );

      // try composite_curve
      Handle(StepGeom_CompositeCurve) CC = Handle(StepGeom_CompositeCurve)::DownCast(aCrv);
      if (!CC.IsNull()) {
        StepToTopoDS_TranslateCompositeCurve TrCC;
        TrCC.SetPrecision(preci);
        TrCC.SetMaxTol(maxtol);
        TrCC.Init(CC, TP);
        if (TrCC.IsDone())
        {
          if (TrCC.IsInfiniteSegment())
          {
            BRep_Builder aB;
            TopoDS_Compound aComp;
            aB.MakeCompound(aComp);
            TopExp_Explorer anExp;
            for (anExp.Init(TrCC.Value(), TopAbs_EDGE); anExp.More(); anExp.Next())
              aB.Add(aComp, anExp.Current());
            res = aComp;
          }
          else
            res = TrCC.Value();
        }
      }
      else { // try other curves
        Handle(Geom_Curve) aGeomCrv;
        try {
          OCC_CATCH_SIGNALS
            aGeomCrv = StepToGeom::MakeCurve(aCrv);
        }
        catch (Standard_Failure const& anException) {
          Message_Messenger::StreamBuffer sout = TP->Messenger()->SendInfo();
          sout << "StepToTopoDS, GeometricSet, elem " << i << " of " << nbElem << ": exception ";
          sout << anException.GetMessageString() << std::endl;
        }
        if (!aGeomCrv.IsNull()) {
          BRepBuilderAPI_MakeEdge anEdge(aGeomCrv, aGeomCrv->FirstParameter(), aGeomCrv->LastParameter());
          if (anEdge.IsDone()) res = anEdge.Edge();
        }
      }
    }
    // try point
    else if ( ent->IsKind(STANDARD_TYPE(StepGeom_CartesianPoint)) ) {
      Handle(StepGeom_CartesianPoint) aPnt = Handle(StepGeom_CartesianPoint)::DownCast ( ent );
      Handle(Geom_CartesianPoint) thePnt = StepToGeom::MakeCartesianPoint (aPnt);
      if (! thePnt.IsNull()) {
        BRepBuilderAPI_MakeVertex myMkVtx(thePnt->Pnt());
        if ( myMkVtx.IsDone() ) res = myMkVtx.Vertex();
      }
    }
    // Element should finally be a Surface
    else if (ent->IsKind(STANDARD_TYPE(StepGeom_Surface))) {
      Handle(StepGeom_Surface) aSurf =
        Handle(StepGeom_Surface)::DownCast(ent);

      // try curve_bounded_surf
      if (ent->IsKind(STANDARD_TYPE(StepGeom_CurveBoundedSurface))) {
        Handle(StepGeom_CurveBoundedSurface) CBS =
          Handle(StepGeom_CurveBoundedSurface)::DownCast(aSurf);
        StepToTopoDS_TranslateCurveBoundedSurface TrCBS;
        TrCBS.SetPrecision(preci);
        TrCBS.SetMaxTol(maxtol);

        TrCBS.Init(CBS, TP);
        if (TrCBS.IsDone()) res = TrCBS.Value();
      }
      // try RectangularCompositeSurface
      else if (ent->IsKind(STANDARD_TYPE(StepGeom_RectangularCompositeSurface))) {
        Handle(StepGeom_RectangularCompositeSurface) RCS =
          Handle(StepGeom_RectangularCompositeSurface)::DownCast(aSurf);
        Standard_Integer nbi = RCS->NbSegmentsI();
        Standard_Integer nbj = RCS->NbSegmentsJ();
        TopoDS_Compound C;
        B.MakeCompound(C);
        for (Standard_Integer ii = 1; ii <= nbi; ii++)
          for (Standard_Integer j = 1; j <= nbj; j++) {
            Handle(StepGeom_SurfacePatch) patch = RCS->SegmentsValue(ii, j);
            TopoDS_Face f = TranslateBoundedSurf(patch->ParentSurface(), preci);
            if (!f.IsNull()) B.Add(C, f);
          }
        res = C;
      }
      // try other surfs
      else res = TranslateBoundedSurf(aSurf, preci);
    }
    else if ( ent->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem)) )
    {
      Handle(StepGeom_GeometricRepresentationItem) GRI = 
        Handle(StepGeom_GeometricRepresentationItem)::DownCast(ent);
      if (!RA.IsNull())
      {
        Handle(STEPControl_ActorRead) anActor = Handle(STEPControl_ActorRead)::DownCast(RA);
        Handle(Transfer_Binder) binder;
        if( !anActor.IsNull())
          binder = anActor->TransferShape(GRI, TP, isManifold, Standard_False, aRange);
        if (!binder.IsNull())
        {
          res = TransferBRep::ShapeResult(binder);
        }
      }
    }
    else TP->AddWarning (ent," Entity is not a Curve, Point, Surface or GeometricRepresentationItem");
    if ( ! res.IsNull() ) {
      B.Add(S, res);
      TransferBRep::SetShapeResult ( TP, ent, res );
    }
    else TP->AddWarning (ent," Entity not mapped to TopoDS");
  }
  myResult = S;
  myError  = StepToTopoDS_BuilderDone;
  done     = Standard_True;
}
// ***end DTH Apr/6 


// ============================================================================
// Method  : Value
// Purpose : Returns the result of the mapping
// ============================================================================

const TopoDS_Shape& StepToTopoDS_Builder::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "StepToTopoDS_Builder::Value() - no result");
  return myResult;
}

// ============================================================================
// Method  : Error
// Purpose : Returns the Builder Error code
// ============================================================================

StepToTopoDS_BuilderError StepToTopoDS_Builder::Error() const 
{
  return myError;
}

