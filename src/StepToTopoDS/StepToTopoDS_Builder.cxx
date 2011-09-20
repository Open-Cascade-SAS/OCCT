// File:	StepToTopoDS_Builder.cxx
// Created:	Tue Jan  3 11:26:22 1995
// Author:	Frederic MAUPAS
//		<fma@stylox>
//:i6 abv 17 Sep 98: ProSTEP TR9 r0601-ct.stp: to be able read GeometricSet
//gka 11.01.99 file PRO7755.stp #2018: work-around error in BRepLib_MakeFace
//:n4 abv 12.02.99: S4132: treatment of GEOMETRIC_SETs implemented
//:o7 abv 18.02.99: bm1_sy_fuel.stp #1427(items 1,2) protection against null entity
//abv,gka 05.04.99 S4136: parameter names changed; avoid extra call to FixSameParameter
  
#include <stdio.h>
#include <StepToTopoDS_Builder.ixx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Interface_Static.hxx>
#include <TransferBRep.hxx>

#include <StepToTopoDS_Tool.hxx>
#include <StepToTopoDS_DataMapOfTRI.hxx>
#include <StepToTopoDS_TranslateShell.hxx>
#include <StepToTopoDS_TranslateCompositeCurve.hxx>
#include <StepToTopoDS_TranslateCurveBoundedSurface.hxx>

//#include <StepShape_VertexShell.hxx>
#include <StepShape_Shell.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_OrientedClosedShell.hxx> //:e0
#include <StepGeom_Surface.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_CompositeCurve.hxx>
#include <StepGeom_RectangularCompositeSurface.hxx>
#include <StepGeom_SurfacePatch.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <StepToGeom_MakeCurve.hxx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <StepToGeom_MakeSurface.hxx>

#include <Geom_Surface.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CartesianPoint.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepLib.hxx>
#include <BRep_Builder.hxx>
#include <Precision.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>

#include <ShapeFix_ShapeTolerance.hxx>
#include <StepShape_ConnectedEdgeSet.hxx>
#include <StepShape_EdgeBasedWireframeModel.hxx>
#include <StepShape_HArray1OfConnectedEdgeSet.hxx>
#include <StepToTopoDS_TranslateEdge.hxx>
#include <StepShape_HArray1OfEdge.hxx>
#include <TopoDS.hxx>
#include <StepShape_HArray1OfConnectedFaceSet.hxx>
#include <StepToTopoDS_TranslateFace.hxx>
#include <StepShape_HArray1OfFace.hxx>

#include <Message_ProgressSentry.hxx>
#include <Message_Messenger.hxx>

static void ResetPreci (const TopoDS_Shape& S, Standard_Real maxtol)
{
  //:S4136
  Standard_Integer modetol = Interface_Static::IVal("read.maxprecision.mode");
  if (modetol) {
    ShapeFix_ShapeTolerance STU;
    STU.LimitTolerance (S,Precision::Confusion(),maxtol);
  }
//  Standard_Real ratio = Interface_Static::RVal("XSTEP.readprecision.ratio");
//  if (ratio >= 1) {
//    Standard_Real lastpre = Interface_Static::RVal("lastpreci");
//    ShapeFix_ShapeTolerance STU;
//    STU.LimitTolerance (S,lastpre/ratio,lastpre*ratio);
//  }
}

// ============================================================================
// Method  : StepToTopoDS_Builder::StepToTopoDS_Builder
// Purpose : Empty constructor
// ============================================================================

StepToTopoDS_Builder::StepToTopoDS_Builder()
{
  done = Standard_False;
}

// ============================================================================
// Method  : StepToTopoDS_Builder::StepToTopoDS_Builder
// Purpose : Constructor with a ManifoldSolidBrep
// ============================================================================

StepToTopoDS_Builder::StepToTopoDS_Builder
(const Handle(StepShape_ManifoldSolidBrep)& aManifoldSolid, 
 const Handle(Transfer_TransientProcess)& TP)
{
  Init(aManifoldSolid, TP);
}

// ============================================================================
// Method  : StepToTopoDS_Builder::StepToTopoDS_Builder
// Purpose : Constructor woth a BrepWithVoids
// ============================================================================

StepToTopoDS_Builder::StepToTopoDS_Builder
(const Handle(StepShape_BrepWithVoids)& aBRepWithVoids,
 const Handle(Transfer_TransientProcess)& TP)
{
  Init(aBRepWithVoids, TP);
}

// ============================================================================
// Method  : StepToTopoDS_Builder::StepToTopoDS_Builder
// Purpose : Constructor with a FacetedBrep
// ============================================================================

StepToTopoDS_Builder::StepToTopoDS_Builder
(const Handle(StepShape_FacetedBrep)& aFB,
 const Handle(Transfer_TransientProcess)& TP)
{
  Init(aFB, TP);
}

// ============================================================================
// Method  : StepToTopoDS_Builder::StepToTopoDS_Builder
// Purpose : Constructor with a FacetedBrepAndBrepWithVoids
// ============================================================================

StepToTopoDS_Builder::StepToTopoDS_Builder
(const Handle(StepShape_FacetedBrepAndBrepWithVoids)& aFBABWV,
 const Handle(Transfer_TransientProcess)& TP)
{
  Init(aFBABWV, TP);
}

// ============================================================================
// Method  : StepToTopoDS_Builder::StepToTopoDS_Builder
// Purpose : Constructor with a ShellBasedSurfaceModel
// ============================================================================

StepToTopoDS_Builder::StepToTopoDS_Builder
(const Handle(StepShape_ShellBasedSurfaceModel)& aSBSM,
 const Handle(Transfer_TransientProcess)& TP,
 StepToTopoDS_NMTool& NMTool)
{
  Init(aSBSM, TP, NMTool);
}

// ============================================================================
// Method  : Init
// Purpose : Init with a ManifoldSolidBrep
// ============================================================================

void StepToTopoDS_Builder::Init
(const Handle(StepShape_ManifoldSolidBrep)& aManifoldSolid,
 const Handle(Transfer_TransientProcess)& TP)
{
  Handle(Message_Messenger) sout = TP->Messenger();
  // Initialisation of the Tool

  StepToTopoDS_Tool         myTool;
  StepToTopoDS_DataMapOfTRI aMap;

  myTool.Init(aMap, TP);

  // Start Mapping

  Handle(StepShape_ClosedShell) aCShell;
  aCShell = aManifoldSolid->Outer();

  StepToTopoDS_TranslateShell myTranShell;
  myTranShell.SetPrecision(Precision());
  myTranShell.SetMaxTol(MaxTol());
  // Non-manifold topology is not referenced by ManifoldSolidBrep (ssv; 14.11.2010)
  StepToTopoDS_NMTool dummyNMTool;
  myTranShell.Init(aCShell, myTool, dummyNMTool);

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
      sout << "Geometric Statitics : " << endl;
      sout << "   Surface Continuity : - C0 : " << myTool.C0Surf() << endl;
      sout << "                        - C1 : " << myTool.C1Surf() << endl;
      sout << "                        - C2 : " << myTool.C2Surf() << endl;
      sout << "   Curve Continuity :   - C0 : " << myTool.C0Cur3() << endl;
      sout << "                        - C1 : " << myTool.C1Cur3() << endl;
      sout << "                        - C2 : " << myTool.C2Cur3() << endl;
      sout << "   PCurve Continuity :  - C0 : " << myTool.C0Cur2() << endl;
      sout << "                        - C1 : " << myTool.C1Cur2() << endl;
      sout << "                        - C2 : " << myTool.C2Cur2() << endl;
    }

//:S4136    ShapeFix::SameParameter (S,Standard_False);
    ResetPreci (S, MaxTol());
  }
  else {
    TP->AddWarning(aCShell," OuterShell from ManifoldSolidBrep not mapped to TopoDS");
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
 const Handle(Transfer_TransientProcess)& TP)
{
  Handle(Message_Messenger) sout = TP->Messenger();
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
  
  Message_ProgressSentry PS ( TP->GetProgress(), "Shell", 0, Nb+1, 1 );

  StepToTopoDS_TranslateShell myTranShell;

  myTranShell.SetPrecision(Precision());//gka
  myTranShell.SetMaxTol(MaxTol());
  // OuterBound

  aCShell = aBRepWithVoids->Outer();
  // Non-manifold topology is not referenced by BrepWithVoids (ssv; 14.11.2010)
  StepToTopoDS_NMTool dummyNMTool;
  myTranShell.Init(aCShell, myTool, dummyNMTool);
  
  PS.Next();

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

  for (Standard_Integer i=1; i<=Nb && PS.More(); i++, PS.Next()) {

    aCShell = aBRepWithVoids->VoidsValue(i);
    myTranShell.Init(aCShell, myTool, dummyNMTool);
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
    sout << "Geometric Statitics : " << endl;
    sout << "   Surface Continuity : - C0 : " << myTool.C0Surf() << endl;
    sout << "                        - C1 : " << myTool.C1Surf() << endl;
    sout << "                        - C2 : " << myTool.C2Surf() << endl;
    sout << "   Curve Continuity :   - C0 : " << myTool.C0Cur3() << endl;
    sout << "                        - C1 : " << myTool.C1Cur3() << endl;
    sout << "                        - C2 : " << myTool.C2Cur3() << endl;
    sout << "   PCurve Continuity :  - C0 : " << myTool.C0Cur2() << endl;
    sout << "                        - C1 : " << myTool.C1Cur2() << endl;
    sout << "                        - C2 : " << myTool.C2Cur2() << endl;
  }

//:S4136  ShapeFix::SameParameter (S,Standard_False);
  ResetPreci (S, MaxTol());
}

// ============================================================================
// Method  : Init
// Purpose : Init with a FacetedBrep
// ============================================================================

void StepToTopoDS_Builder::Init(const Handle(StepShape_FacetedBrep)& aFB,
			       const Handle(Transfer_TransientProcess)& TP)
{
  // Initialisation of the Tool

  StepToTopoDS_Tool         myTool;
  StepToTopoDS_DataMapOfTRI aMap;

  myTool.Init(aMap, TP);

  // Start Mapping

  Handle(StepShape_ClosedShell) aCShell;
  aCShell = aFB->Outer();
  TopoDS_Shape Sh;

  StepToTopoDS_TranslateShell myTranShell; 
  myTranShell.SetPrecision(Precision()); //gka
  myTranShell.SetMaxTol(MaxTol());  
  // Non-manifold topology is not referenced by FacetedBrep (ss; 14.11.2010)
  StepToTopoDS_NMTool dummyNMTool;
  myTranShell.Init(aCShell, myTool, dummyNMTool);

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
 const Handle(Transfer_TransientProcess)& TP)
{
  // Initialisation of the Tool

  StepToTopoDS_Tool         myTool;
  StepToTopoDS_DataMapOfTRI aMap;

  myTool.Init(aMap, TP);

  // Start Mapping

  Handle(StepShape_ClosedShell) aCShell;
  aCShell = aFBABWV->Outer();
  TopoDS_Shape Sh;

  StepToTopoDS_TranslateShell myTranShell;
  myTranShell.SetPrecision(Precision()); //gka
  myTranShell.SetMaxTol(MaxTol());
  // Non-manifold topology is not referenced by FacetedBrepAndBrepWithVoids (ss; 14.11.2010)
  StepToTopoDS_NMTool dummyNMTool;
  myTranShell.Init(aCShell, myTool, dummyNMTool);

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
    for ( i=1; i<=Nb; i++ ) {
      aCShell = aFBABWV->VoidsValue(i);
      myTranShell.Init(aCShell, myTool, dummyNMTool);
      if (myTranShell.IsDone()) {
	Sh = myTranShell.Value();
	Sh.Closed(Standard_True);
	//BRepLib::SameParameter(Sh);
	B.Add(S,Sh);
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
 StepToTopoDS_NMTool& NMTool)
{
  Handle(Message_Messenger) sout = TP->Messenger();
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

  Message_ProgressSentry PS ( TP->GetProgress(), "Shell", 0, Nb, 1 );
  for (Standard_Integer i=1; i<=Nb && PS.More(); i++, PS.Next()) {
  //for (Standard_Integer i=1; i<=Nb; i++) {
    aShell = aSBSM->SbsmBoundaryValue(i);
    //aVertexShell = aShell.VertexShell();
    aOpenShell   = aShell.OpenShell();
    aClosedShell = aShell.ClosedShell();
    //if (!aVertexShell.IsNull()) {
    //TP->AddWarning
    //(aVertexShell,
    //" VertexShell from ShellBasedSurfaceModel not mapped to TopoDS");
    //}
    //else 
    if (!aOpenShell.IsNull()) {
      myTranShell.Init(aOpenShell, myTool, NMTool);
      if (myTranShell.IsDone()) {
	//Sh = myTranShell.Value();
	//Sh.Closed(Standard_False);
	//BRepLib::SameParameter(Sh);
	//B.Add(S,Sh);
	Shl = TopoDS::Shell(myTranShell.Value());
	Shl.Closed(Standard_False);
	B.Add(S,Shl);
      }
      else { 
	TP->AddWarning
	  (aOpenShell," OpenShell from ShellBasedSurfaceModel not mapped to TopoDS");
      }
    }
    else if (!aClosedShell.IsNull()) {
      myTranShell.Init(aClosedShell, myTool, NMTool);
      if (myTranShell.IsDone()) {
	//Sh = myTranShell.Value();
	//Sh.Closed(Standard_True);
	//BRepLib::SameParameter(Sh);
	//B.Add(S,Sh);
	Shl = TopoDS::Shell(myTranShell.Value());
	Shl.Closed(Standard_True);
	B.Add(S,Shl);
      }
      else { 
	TP->AddWarning
	  (aClosedShell," ClosedShell from ShellBasedSurfaceModel not mapped to TopoDS");
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
    sout << "Geometric Statitics : " << endl;
    sout << "   Surface Continuity : - C0 : " << myTool.C0Surf() << endl;
    sout << "                        - C1 : " << myTool.C1Surf() << endl;
    sout << "                        - C2 : " << myTool.C2Surf() << endl;
    sout << "   Curve Continuity :   - C0 : " << myTool.C0Cur3() << endl;
    sout << "                        - C1 : " << myTool.C1Cur3() << endl;
    sout << "                        - C2 : " << myTool.C2Cur3() << endl;
    sout << "   PCurve Continuity :  - C0 : " << myTool.C0Cur2() << endl;
    sout << "                        - C1 : " << myTool.C1Cur2() << endl;
    sout << "                        - C2 : " << myTool.C2Cur2() << endl;
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

  Handle(Geom_Surface) theSurf;
  if (!StepToGeom_MakeSurface::Convert(surf,theSurf) || //:i6: protection
      !theSurf->IsKind(STANDARD_TYPE(Geom_BoundedSurface))) return res;

  //gka 11.01.99 file PRO7755.stp entity #2018 surface #1895: error BRepLib_MakeFace func IsDegenerated
  BRepBuilderAPI_MakeFace myMkFace(theSurf, TolDegen);
  return myMkFace.Face();
}

void StepToTopoDS_Builder::Init
(const Handle(StepShape_GeometricSet)& GCS,
 const Handle(Transfer_TransientProcess)& TP)
{
  // Initialisation of the Tool

//  StepToTopoDS_Tool         myTool;
//  StepToTopoDS_DataMapOfTRI aMap;

//  myTool.Init(aMap, TP);

  // Start Mapping
  TopoDS_Compound S;
  BRep_Builder B;
  B.MakeCompound(S);
  TopoDS_Edge theEdge;
  Standard_Integer i;
  Standard_Real preci = Precision();   //gka
  Standard_Real maxtol = MaxTol();
  Standard_Integer nbElem = GCS->NbElements();
  for (i = 1; i <= nbElem ; i++) {
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
      if ( ! CC.IsNull() ) {
	StepToTopoDS_TranslateCompositeCurve TrCC;
	TrCC.SetPrecision(preci);
	TrCC.SetMaxTol(maxtol);
	TrCC.Init( CC, TP );
	if ( TrCC.IsDone() ) res = TrCC.Value();
      }
      else { // try other curves
	Handle(Geom_Curve) aGeomCrv;
	try {
	  OCC_CATCH_SIGNALS
      StepToGeom_MakeCurve::Convert(aCrv,aGeomCrv);
	}
	catch(Standard_Failure) {
	  Handle(Message_Messenger) sout = TP->Messenger();
	  sout<<"StepToTopoDS, GeometricSet, elem "<<i<<" of "<<nbElem<<": exception ";
	  sout<<Standard_Failure::Caught()->GetMessageString() << endl;
	}
	if ( ! aGeomCrv.IsNull() ) {
	  BRepBuilderAPI_MakeEdge anEdge(aGeomCrv, aGeomCrv->FirstParameter(), aGeomCrv->LastParameter());
	  if ( anEdge.IsDone() ) res = anEdge.Edge();
	}
      }
    }
    // try point
    else if ( ent->IsKind(STANDARD_TYPE(StepGeom_CartesianPoint)) ) {
      Handle(StepGeom_CartesianPoint) aPnt = Handle(StepGeom_CartesianPoint)::DownCast ( ent );
      Handle(Geom_CartesianPoint) thePnt;
      if (StepToGeom_MakeCartesianPoint::Convert(aPnt,thePnt)) {
        BRepBuilderAPI_MakeVertex myMkVtx(thePnt->Pnt());
        if ( myMkVtx.IsDone() ) res = myMkVtx.Vertex();
      }
    }
    // Element should finally be a Surface
    else if ( ent->IsKind(STANDARD_TYPE(StepGeom_Surface)) ) {
      Handle(StepGeom_Surface) aSurf = 
	Handle(StepGeom_Surface)::DownCast(ent);
	
      // try curve_bounded_surf
      if ( ent->IsKind(STANDARD_TYPE(StepGeom_CurveBoundedSurface)) ) {
	Handle(StepGeom_CurveBoundedSurface) CBS = 
	  Handle(StepGeom_CurveBoundedSurface)::DownCast(aSurf);
	StepToTopoDS_TranslateCurveBoundedSurface TrCBS;
	TrCBS.SetPrecision(preci);
	TrCBS.SetMaxTol(maxtol);
	
	TrCBS.Init( CBS, TP );
	if ( TrCBS.IsDone() ) res = TrCBS.Value();
      }
      // try RectangularCompositeSurface
      else if ( ent->IsKind(STANDARD_TYPE(StepGeom_RectangularCompositeSurface)) ) {
	Handle(StepGeom_RectangularCompositeSurface) RCS = 
	  Handle(StepGeom_RectangularCompositeSurface)::DownCast(aSurf);
	Standard_Integer nbi = RCS->NbSegmentsI();
	Standard_Integer nbj = RCS->NbSegmentsJ();
	TopoDS_Compound C;
	B.MakeCompound ( C );
	for ( Standard_Integer ii=1; ii <= nbi; ii++ ) 
	  for ( Standard_Integer j=1; j <= nbj; j++ ) {
	    Handle(StepGeom_SurfacePatch) patch = RCS->SegmentsValue ( ii, j );
	    TopoDS_Face f = TranslateBoundedSurf (patch->ParentSurface(), preci);
	    if ( ! f.IsNull() ) B.Add ( C, f );
	  }
	res = C;
      }
      // try other surfs
      else res = TranslateBoundedSurf (aSurf, preci);
    }
    else TP->AddWarning (ent," Entity is not a Curve, Point or Surface");
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
  StdFail_NotDone_Raise_if(!done,"");
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

