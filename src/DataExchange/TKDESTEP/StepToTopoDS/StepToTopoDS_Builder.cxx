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

//: i6 abv 17 Sep 98: ProSTEP TR9 r0601-ct.stp: to be able read GeometricSet
// gka 11.01.99 file PRO7755.stp #2018: work-around error in BRepLib_MakeFace
//: n4 abv 12.02.99: S4132: treatment of GEOMETRIC_SETs implemented
//: o7 abv 18.02.99: bm1_sy_fuel.stp #1427(items 1,2) protection against null entity
// abv,gka 05.04.99 S4136: parameter names changed; avoid extra call to FixSameParameter

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepLib.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <Precision.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <StdFail_NotDone.hxx>
#include <STEPControl_ActorRead.hxx>
#include <StepData_Factors.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_CompositeCurve.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_CurveBoundedSurface.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepGeom_RectangularCompositeSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SurfacePatch.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_EdgeBasedWireframeModel.hxx>
#include <StepShape_FaceBasedSurfaceModel.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <StepShape_ConnectedEdgeSet.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_Edge.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepVisual_TessellatedFace.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <StepVisual_TessellatedSolid.hxx>
#include <StepVisual_TessellatedSurfaceSet.hxx>
#include <StepToGeom.hxx>
#include <StepToTopoDS_Builder.hxx>
#include <StepToTopoDS_NMTool.hxx>
#include <StepToTopoDS_Tool.hxx>
#include <StepToTopoDS_TranslateCompositeCurve.hxx>
#include <StepToTopoDS_TranslateCurveBoundedSurface.hxx>
#include <StepToTopoDS_TranslateEdge.hxx>
#include <StepToTopoDS_TranslateFace.hxx>
#include <StepToTopoDS_TranslateShell.hxx>
#include <StepToTopoDS_TranslateSolid.hxx>
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

#include <cstdio>

static void ResetPreci(occ::handle<StepData_StepModel>& theStepModel,
                       const TopoDS_Shape&              S,
                       double                           maxtol)
{
  //: S4136
  int modetol = theStepModel->InternalParameters.ReadMaxPrecisionMode;
  if (modetol)
  {
    ShapeFix_ShapeTolerance STU;
    STU.LimitTolerance(S, Precision::Confusion(), maxtol);
  }
}

//=================================================================================================

StepToTopoDS_Builder::StepToTopoDS_Builder()
    : myError(StepToTopoDS_BuilderOther)
{
  done = false;
}

// ============================================================================
// Method  : Init
// Purpose : Init with a ManifoldSolidBrep
// ============================================================================
void StepToTopoDS_Builder::Init(const occ::handle<StepShape_ManifoldSolidBrep>& theManifoldSolid,
                                const occ::handle<Transfer_TransientProcess>&   theTP,
                                const StepData_Factors&                         theLocalFactors,
                                const Message_ProgressRange&                    theProgress)
{
  Message_Messenger::StreamBuffer sout       = theTP->Messenger()->SendInfo();
  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(theTP->Model());
  // Initialisation of the Tool

  StepToTopoDS_Tool                                                                       aTool;
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;

  aTool.Init(aMap, theTP);

  // Start Mapping
  const occ::handle<StepShape_ConnectedFaceSet> aShell = theManifoldSolid->Outer();

  StepToTopoDS_TranslateShell myTranShell;
  myTranShell.SetPrecision(Precision());
  myTranShell.SetMaxTol(MaxTol());

  StepToTopoDS_NMTool dummyNMTool;
  myTranShell.Init(aShell, aTool, dummyNMTool, theLocalFactors, theProgress);
  if (!myTranShell.IsDone())
  {
    theTP->AddWarning(aShell, " OuterShell from ManifoldSolidBrep not mapped to TopoDS");
    myError = StepToTopoDS_BuilderOther;
    done    = false;
    return;
  }
  TopoDS_Shape aShape = myTranShell.Value();
  aShape.Closed(true);
  TopoDS_Solid aSolid;
  BRep_Builder aBuilder;
  aBuilder.MakeSolid(aSolid);
  aBuilder.Add(aSolid, aShape);
  myResult = aSolid;
  myError  = StepToTopoDS_BuilderDone;
  done     = true;

  // Get Statistics :
  if (theTP->TraceLevel() > 2)
  {
    sout << "Geometric Statistics : " << std::endl;
    sout << "   Surface Continuity : - C0 : " << aTool.C0Surf() << std::endl;
    sout << "                        - C1 : " << aTool.C1Surf() << std::endl;
    sout << "                        - C2 : " << aTool.C2Surf() << std::endl;
    sout << "   Curve Continuity :   - C0 : " << aTool.C0Cur3() << std::endl;
    sout << "                        - C1 : " << aTool.C1Cur3() << std::endl;
    sout << "                        - C2 : " << aTool.C2Cur3() << std::endl;
    sout << "   PCurve Continuity :  - C0 : " << aTool.C0Cur2() << std::endl;
    sout << "                        - C1 : " << aTool.C1Cur2() << std::endl;
    sout << "                        - C2 : " << aTool.C2Cur2() << std::endl;
  }

  ResetPreci(aStepModel, aSolid, MaxTol());
}

// ============================================================================
// Method  : Init
// Purpose : Init with a BrepWithVoids
// ============================================================================
void StepToTopoDS_Builder::Init(const occ::handle<StepShape_BrepWithVoids>&   theBRepWithVoids,
                                const occ::handle<Transfer_TransientProcess>& theTP,
                                const StepData_Factors&                       theLocalFactors,
                                const Message_ProgressRange&                  theProgress)
{
  Message_Messenger::StreamBuffer sout = theTP->Messenger()->SendInfo();
  // Initialisation of the Tool
  StepToTopoDS_Tool                                                                       aTool;
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;

  aTool.Init(aMap, theTP);

  // Start Mapping
  TopoDS_Solid aSolid;
  BRep_Builder aBuilder;
  aBuilder.MakeSolid(aSolid);

  Message_ProgressScope aPS(theProgress, "Shell", theBRepWithVoids->NbVoids() + 1);

  StepToTopoDS_TranslateShell aTranShell;

  aTranShell.SetPrecision(Precision()); // gka
  aTranShell.SetMaxTol(MaxTol());
  // OuterBound

  occ::handle<StepShape_ClosedShell> aCShell =
    occ::down_cast<StepShape_ClosedShell>(theBRepWithVoids->Outer());
  // Non-manifold topology is not referenced by BrepWithVoids (ssv; 14.11.2010)
  StepToTopoDS_NMTool aDummyNMTool;
  aTranShell.Init(aCShell, aTool, aDummyNMTool, theLocalFactors, aPS.Next());
  if (!aTranShell.IsDone())
  {
    theTP->AddWarning(aCShell, " OuterShell from BrepWithVoids not mapped to TopoDS");
    myError = StepToTopoDS_BuilderOther;
    done    = false;
    return;
  }
  TopoDS_Shape aShape = aTranShell.Value();
  aShape.Closed(true);
  aBuilder.MakeSolid(aSolid);
  aBuilder.Add(aSolid, aShape);
  myResult = aSolid;
  myError  = StepToTopoDS_BuilderDone;
  done     = true;

  // Voids
  for (NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>::Iterator anIt(
         theBRepWithVoids->Voids()->Array1());
       anIt.More() && aPS.More();
       anIt.Next())
  {
    occ::handle<StepShape_OrientedClosedShell> anOCShell = anIt.Value();
    aTranShell.Init(anOCShell, aTool, aDummyNMTool, theLocalFactors, aPS.Next());
    if (aTranShell.IsDone())
    {
      aShape = aTranShell.Value();
      aShape.Closed(true);

      //: e0 abv 25 Mar 98: void should be an OrientedShell
      if (!anOCShell.IsNull() && !anOCShell->Orientation())
        aShape.Reverse();

      aBuilder.Add(aSolid, aShape);
    }
    else
    {
      theTP->AddWarning(anOCShell, " A Void from BrepWithVoids not mapped to TopoDS");
    }
  }
  myResult = aSolid;

  // Get Statistics :
  if (theTP->TraceLevel() > 2)
  {
    sout << "Geometric Statistics : " << std::endl;
    sout << "   Surface Continuity : - C0 : " << aTool.C0Surf() << std::endl;
    sout << "                        - C1 : " << aTool.C1Surf() << std::endl;
    sout << "                        - C2 : " << aTool.C2Surf() << std::endl;
    sout << "   Curve Continuity :   - C0 : " << aTool.C0Cur3() << std::endl;
    sout << "                        - C1 : " << aTool.C1Cur3() << std::endl;
    sout << "                        - C2 : " << aTool.C2Cur3() << std::endl;
    sout << "   PCurve Continuity :  - C0 : " << aTool.C0Cur2() << std::endl;
    sout << "                        - C1 : " << aTool.C1Cur2() << std::endl;
    sout << "                        - C2 : " << aTool.C2Cur2() << std::endl;
  }

  //: S4136  ShapeFix::SameParameter (S,false);
  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(theTP->Model());
  ResetPreci(aStepModel, aSolid, MaxTol());
}

// ============================================================================
// Method  : Init
// Purpose : Init with a FacetedBrep
// ============================================================================
void StepToTopoDS_Builder::Init(const occ::handle<StepShape_FacetedBrep>&     theFB,
                                const occ::handle<Transfer_TransientProcess>& theTP,
                                const StepData_Factors&                       theLocalFactors,
                                const Message_ProgressRange&                  theProgress)
{
  // Initialisation of the Tool
  StepToTopoDS_Tool                                                                       aTool;
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;

  aTool.Init(aMap, theTP);

  // Start Mapping
  const occ::handle<StepShape_ClosedShell> aCShell =
    occ::down_cast<StepShape_ClosedShell>(theFB->Outer());

  StepToTopoDS_TranslateShell aTranShell;
  aTranShell.SetPrecision(Precision()); // gka
  aTranShell.SetMaxTol(MaxTol());
  // Non-manifold topology is not referenced by FacetedBrep (ss; 14.11.2010)
  StepToTopoDS_NMTool aDummyNMTool;
  aTranShell.Init(aCShell, aTool, aDummyNMTool, theLocalFactors, theProgress);
  if (!aTranShell.IsDone())
  {
    theTP->AddWarning(aCShell, " OuterShell from FacetedBrep not mapped to TopoDS");
    myError = StepToTopoDS_BuilderOther;
    done    = false;
    return;
  }
  TopoDS_Shape aShape = aTranShell.Value();
  aShape.Closed(true);
  aTool.ClearEdgeMap();
  aTool.ClearVertexMap();
  TopoDS_Solid aSolid;
  BRep_Builder aBuilder;
  aBuilder.MakeSolid(aSolid);
  aBuilder.Add(aSolid, aShape);
  myResult = aSolid;
  myError  = StepToTopoDS_BuilderDone;
  done     = true;
}

// ============================================================================
// Method  : Init
// Purpose : Init with a FacetedBrepAndBrepWithVoids
// ============================================================================
void StepToTopoDS_Builder::Init(const occ::handle<StepShape_FacetedBrepAndBrepWithVoids>& theFBABWV,
                                const occ::handle<Transfer_TransientProcess>&             theTP,
                                const StepData_Factors&      theLocalFactors,
                                const Message_ProgressRange& theProgress)
{
  // Initialisation of the Tool
  StepToTopoDS_Tool                                                                       aTool;
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;

  aTool.Init(aMap, theTP);

  // Start Mapping
  occ::handle<StepShape_ClosedShell> aCShell =
    occ::down_cast<StepShape_ClosedShell>(theFBABWV->Outer());

  Message_ProgressScope aPSRoot(theProgress, nullptr, 2);

  StepToTopoDS_TranslateShell aTranShell;
  aTranShell.SetPrecision(Precision()); // gka
  aTranShell.SetMaxTol(MaxTol());
  // Non-manifold topology is not referenced by FacetedBrepAndBrepWithVoids (ss; 14.11.2010)
  StepToTopoDS_NMTool aDummyNMTool;
  aTranShell.Init(aCShell, aTool, aDummyNMTool, theLocalFactors, aPSRoot.Next());
  if (!aTranShell.IsDone())
  {
    theTP->AddWarning(aCShell, " OuterShell from FacetedBrepAndBrepWithVoids not mapped to TopoDS");
    done    = false;
    myError = StepToTopoDS_BuilderOther;
    return;
  }
  TopoDS_Shape aShape = aTranShell.Value();
  aShape.Closed(true);
  TopoDS_Solid aSolid;
  BRep_Builder aBuilder;
  aBuilder.MakeSolid(aSolid);
  aBuilder.Add(aSolid, aShape);
  Message_ProgressScope aPS(aPSRoot.Next(), nullptr, theFBABWV->NbVoids());
  for (NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>::Iterator anIt(
         theFBABWV->Voids()->Array1());
       anIt.More() && aPS.More();
       anIt.Next())
  {
    occ::handle<StepShape_OrientedClosedShell> anOCShell = anIt.Value();
    aTranShell.Init(anOCShell, aTool, aDummyNMTool, theLocalFactors, aPS.Next());
    if (aTranShell.IsDone())
    {
      aShape = aTranShell.Value();
      aShape.Closed(true);
      aBuilder.Add(aSolid, aShape);
    }
    else
    {
      theTP->AddWarning(anOCShell, " A Void from FacetedBrepAndBrepWithVoids not mapped to TopoDS");
    }
  }
  myResult = aSolid;
  myError  = StepToTopoDS_BuilderDone;
  done     = true;
}

// ============================================================================
// Method  : Init
// Purpose : Init with a ShellBasedSurfaceModel
// ============================================================================

void StepToTopoDS_Builder::Init(const occ::handle<StepShape_ShellBasedSurfaceModel>& aSBSM,
                                const occ::handle<Transfer_TransientProcess>&        TP,
                                StepToTopoDS_NMTool&                                 NMTool,
                                const StepData_Factors&      theLocalFactors,
                                const Message_ProgressRange& theProgress)
{
  Message_Messenger::StreamBuffer sout = TP->Messenger()->SendInfo();
  // Initialisation of the Tool

  StepToTopoDS_Tool                                                                       myTool;
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;

  myTool.Init(aMap, TP);

  // Start Mapping

  int             Nb = aSBSM->NbSbsmBoundary();
  StepShape_Shell aShell;
  // occ::handle<StepShape_VertexShell> aVertexShell;
  occ::handle<StepShape_OpenShell>   aOpenShell;
  occ::handle<StepShape_ClosedShell> aClosedShell;
  TopoDS_Compound                    S;
  // TopoDS_Shape Sh;
  TopoDS_Shape Shl;
  BRep_Builder B;
  B.MakeCompound(S);
  StepToTopoDS_TranslateShell myTranShell;

  myTranShell.SetPrecision(Precision());
  myTranShell.SetMaxTol(MaxTol());

  Message_ProgressScope PS(theProgress, "Shell", Nb);
  for (int i = 1; i <= Nb && PS.More(); i++)
  {
    Message_ProgressRange aRange = PS.Next();
    aShell                       = aSBSM->SbsmBoundaryValue(i);
    aOpenShell                   = aShell.OpenShell();
    aClosedShell                 = aShell.ClosedShell();
    if (!aOpenShell.IsNull())
    {
      myTranShell.Init(aOpenShell, myTool, NMTool, theLocalFactors, aRange);
      if (myTranShell.IsDone())
      {
        Shl = TopoDS::Shell(myTranShell.Value());
        Shl.Closed(false);
        B.Add(S, Shl);
      }
      else
      {
        TP->AddWarning(aOpenShell, " OpenShell from ShellBasedSurfaceModel not mapped to TopoDS");
      }
    }
    else if (!aClosedShell.IsNull())
    {
      myTranShell.Init(aClosedShell, myTool, NMTool, theLocalFactors, aRange);
      if (myTranShell.IsDone())
      {
        Shl = TopoDS::Shell(myTranShell.Value());
        Shl.Closed(true);
        B.Add(S, Shl);
      }
      else
      {
        TP->AddWarning(aClosedShell,
                       " ClosedShell from ShellBasedSurfaceModel not mapped to TopoDS");
      }
    }
  }
  if (Nb > 1)
    myResult = S;
  else
    myResult = Shl;
  myError = StepToTopoDS_BuilderDone;
  done    = true;

  // Get Statistics :

  if (TP->TraceLevel() > 2)
  {
    sout << "Geometric Statistics : " << std::endl;
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

  //: S4136  ShapeFix::SameParameter (S,false);
  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(TP->Model());
  ResetPreci(aStepModel, S, MaxTol());
  ResetPreci(aStepModel, Shl, MaxTol()); // skl
}

// ============================================================================
// Method  : Init
// Purpose : Init with a EdgeBasedWireframeModel
// ============================================================================

void StepToTopoDS_Builder::Init(const occ::handle<StepShape_EdgeBasedWireframeModel>& aEBWM,
                                const occ::handle<Transfer_TransientProcess>&         TP,
                                const StepData_Factors& theLocalFactors)
{
  myResult.Nullify();

  occ::handle<NCollection_HArray1<occ::handle<StepShape_ConnectedEdgeSet>>> boundary =
    aEBWM->EbwmBoundary();
  if (boundary.IsNull() || boundary->Length() < 1)
  {
    TP->AddWarning(aEBWM, "List of boundaries is empty");
    return;
  }

  StepToTopoDS_Tool                                                                       myTool;
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;
  myTool.Init(aMap, TP);

  StepToTopoDS_TranslateEdge myTranEdge;
  myTranEdge.SetPrecision(Precision());
  myTranEdge.SetMaxTol(MaxTol());

  TopoDS_Compound C;
  BRep_Builder    B;
  B.MakeCompound(C);

  StepToTopoDS_NMTool dummyNMTool;

  for (int i = 1; i <= boundary->Length(); i++)
  {
    occ::handle<StepShape_ConnectedEdgeSet> ces = boundary->Value(i);
    if (ces.IsNull())
      continue;
    occ::handle<NCollection_HArray1<occ::handle<StepShape_Edge>>> edges = ces->CesEdges();
    if (edges.IsNull() || edges->Length() < 1)
    {
      TP->AddWarning(ces, "No edges in connected_edge_set");
      continue;
    }
    TopoDS_Wire W;
    for (int j = 1; j <= edges->Length(); j++)
    {
      myTranEdge.Init(edges->Value(j), myTool, dummyNMTool, theLocalFactors);
      if (!myTranEdge.IsDone())
        continue;
      TopoDS_Edge E = TopoDS::Edge(myTranEdge.Value());
      if (E.IsNull())
        continue; // NULL, on saute
      if (W.IsNull())
        B.MakeWire(W);
      B.Add(W, E);
    }
    if (W.IsNull())
      continue;
    W.Closed(BRep_Tool::IsClosed(W));
    B.Add(C, W);
    if (myResult.IsNull())
      myResult = W;
    else
      myResult = C;
  }

  myError = (myResult.IsNull() ? StepToTopoDS_BuilderDone : StepToTopoDS_BuilderOther);
  done    = !myResult.IsNull();

  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(TP->Model());
  ResetPreci(aStepModel, myResult, MaxTol());
}

// ============================================================================
// Method  : Init
// Purpose : Init with a FaceBasedSurfaceModel
// ============================================================================

void StepToTopoDS_Builder::Init(const occ::handle<StepShape_FaceBasedSurfaceModel>& aFBSM,
                                const occ::handle<Transfer_TransientProcess>&       TP,
                                const StepData_Factors&                             theLocalFactors)
{
  myResult.Nullify();

  occ::handle<NCollection_HArray1<occ::handle<StepShape_ConnectedFaceSet>>> boundary =
    aFBSM->FbsmFaces();
  if (boundary.IsNull() || boundary->Length() < 1)
  {
    TP->AddWarning(aFBSM, "List of faces is empty");
    return;
  }

  StepToTopoDS_Tool                                                                       myTool;
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;
  myTool.Init(aMap, TP);

  StepToTopoDS_TranslateFace myTranFace;
  myTranFace.SetPrecision(Precision());
  myTranFace.SetMaxTol(MaxTol());

  TopoDS_Compound C;
  BRep_Builder    B;
  B.MakeCompound(C);

  // Non-manifold topology is not mapped via FaceBasedSurfaceModel (ssv; 14.11.2010)
  StepToTopoDS_NMTool dummyNMTool;

  for (int i = 1; i <= boundary->Length(); i++)
  {
    occ::handle<StepShape_ConnectedFaceSet> cfs = boundary->Value(i);
    if (cfs.IsNull())
      continue;
    occ::handle<NCollection_HArray1<occ::handle<StepShape_Face>>> faces = cfs->CfsFaces();
    if (faces.IsNull() || faces->Length() < 1)
    {
      TP->AddWarning(cfs, "No faces in connected_face_set");
      continue;
    }
    TopoDS_Shell S;
    for (int j = 1; j <= faces->Length(); j++)
    {
      occ::handle<StepShape_FaceSurface> fs =
        occ::down_cast<StepShape_FaceSurface>(faces->Value(j));
      myTranFace.Init(fs, myTool, dummyNMTool, theLocalFactors);
      if (!myTranFace.IsDone())
        continue;
      TopoDS_Face F = TopoDS::Face(myTranFace.Value());
      if (F.IsNull())
        continue; // NULL, on saute
      if (S.IsNull())
        B.MakeShell(S);
      B.Add(S, F);
    }
    if (S.IsNull())
      continue;
    S.Closed(BRep_Tool::IsClosed(S));
    B.Add(C, S);
    if (myResult.IsNull())
      myResult = S;
    else
      myResult = C;
  }

  myError = (myResult.IsNull() ? StepToTopoDS_BuilderDone : StepToTopoDS_BuilderOther);
  done    = !myResult.IsNull();

  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(TP->Model());
  ResetPreci(aStepModel, myResult, MaxTol());
}

// ***start DTH Apr/6
// ============================================================================
// Method  : Init
// Purpose : Init with a GeometricCurveSet
// ============================================================================
//: i6 abv 17 Sep 98: ProSTEP TR9 r0601-ct.stp: to be able read GS: GeometricCurveSet ->
//: GeometricSet

static TopoDS_Face TranslateBoundedSurf(const occ::handle<StepGeom_Surface>& surf,
                                        const double                         TolDegen,
                                        const StepData_Factors&              theLocalFactors)
{
  TopoDS_Face res;

  occ::handle<Geom_Surface> theSurf = StepToGeom::MakeSurface(surf, theLocalFactors);
  if (theSurf.IsNull() || //: i6: protection
      !theSurf->IsKind(STANDARD_TYPE(Geom_BoundedSurface)))
    return res;

  BRepBuilderAPI_MakeFace                     myMkFace;
  occ::handle<Geom_RectangularTrimmedSurface> RS =
    occ::down_cast<Geom_RectangularTrimmedSurface>(theSurf);

  if (!RS.IsNull())
  {
    double umin, umax, vmin, vmax;
    theSurf->Bounds(umin, umax, vmin, vmax);

    myMkFace.Init(RS->BasisSurface(), umin, umax, vmin, vmax, TolDegen);
  }
  else
  {
    myMkFace.Init(theSurf, true, TolDegen);
  }

  return myMkFace.Face();
}

void StepToTopoDS_Builder::Init(const occ::handle<StepShape_GeometricSet>&    GCS,
                                const occ::handle<Transfer_TransientProcess>& TP,
                                const StepData_Factors&                       theLocalFactors,
                                const occ::handle<Transfer_ActorOfTransientProcess>& RA,
                                const bool                                           isManifold,
                                const Message_ProgressRange&                         theProgress)
{
  // Start Mapping
  TopoDS_Compound S;
  BRep_Builder    B;
  B.MakeCompound(S);
  TopoDS_Edge           theEdge;
  int                   i;
  double                preci  = Precision(); // gka
  double                maxtol = MaxTol();
  int                   nbElem = GCS->NbElements();
  Message_ProgressScope aPS(theProgress, nullptr, nbElem);
  for (i = 1; i <= nbElem && aPS.More(); i++)
  {
    Message_ProgressRange                  aRange = aPS.Next();
    StepShape_GeometricSetSelect           aGSS   = GCS->ElementsValue(i);
    const occ::handle<Standard_Transient>& ent    = aGSS.Value();

    TopoDS_Shape res = TransferBRep::ShapeResult(TP, ent);
    if (!res.IsNull())
    { // already translated
      B.Add(S, res);
      continue;
    }
    //: o7 abv 18 Feb 99: bm1_sy_fuel.stp #1427(1,2) protection against null entity
    if (ent.IsNull())
    {
      char buff[100];
      Sprintf(buff, "Entity %d is a Null entity", i);
      TP->AddWarning(GCS, buff);
      continue;
    }
    // try curve
    else if (ent->IsKind(STANDARD_TYPE(StepGeom_Curve)))
    {
      occ::handle<StepGeom_Curve> aCrv = occ::down_cast<StepGeom_Curve>(ent);

      // try composite_curve
      occ::handle<StepGeom_CompositeCurve> CC = occ::down_cast<StepGeom_CompositeCurve>(aCrv);
      if (!CC.IsNull())
      {
        StepToTopoDS_TranslateCompositeCurve TrCC;
        TrCC.SetPrecision(preci);
        TrCC.SetMaxTol(maxtol);
        TrCC.Init(CC, TP, theLocalFactors);
        if (TrCC.IsDone())
        {
          if (TrCC.IsInfiniteSegment())
          {
            BRep_Builder    aB;
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
      else
      { // try other curves
        occ::handle<Geom_Curve> aGeomCrv;
        try
        {
          OCC_CATCH_SIGNALS
          aGeomCrv = StepToGeom::MakeCurve(aCrv, theLocalFactors);
        }
        catch (Standard_Failure const& anException)
        {
          Message_Messenger::StreamBuffer sout = TP->Messenger()->SendInfo();
          sout << "StepToTopoDS, GeometricSet, elem " << i << " of " << nbElem << ": exception ";
          sout << anException.GetMessageString() << std::endl;
        }
        if (!aGeomCrv.IsNull())
        {
          BRepBuilderAPI_MakeEdge anEdge(aGeomCrv,
                                         aGeomCrv->FirstParameter(),
                                         aGeomCrv->LastParameter());
          if (anEdge.IsDone())
            res = anEdge.Edge();
        }
      }
    }
    // try point
    else if (ent->IsKind(STANDARD_TYPE(StepGeom_CartesianPoint)))
    {
      occ::handle<StepGeom_CartesianPoint> aPnt = occ::down_cast<StepGeom_CartesianPoint>(ent);
      occ::handle<Geom_CartesianPoint>     thePnt =
        StepToGeom::MakeCartesianPoint(aPnt, theLocalFactors);
      if (!thePnt.IsNull())
      {
        BRepBuilderAPI_MakeVertex myMkVtx(thePnt->Pnt());
        if (myMkVtx.IsDone())
          res = myMkVtx.Vertex();
      }
    }
    // Element should finally be a Surface
    else if (ent->IsKind(STANDARD_TYPE(StepGeom_Surface)))
    {
      occ::handle<StepGeom_Surface> aSurf = occ::down_cast<StepGeom_Surface>(ent);

      // try curve_bounded_surf
      if (ent->IsKind(STANDARD_TYPE(StepGeom_CurveBoundedSurface)))
      {
        occ::handle<StepGeom_CurveBoundedSurface> CBS =
          occ::down_cast<StepGeom_CurveBoundedSurface>(aSurf);
        StepToTopoDS_TranslateCurveBoundedSurface TrCBS;
        TrCBS.SetPrecision(preci);
        TrCBS.SetMaxTol(maxtol);

        TrCBS.Init(CBS, TP, theLocalFactors);
        if (TrCBS.IsDone())
          res = TrCBS.Value();
      }
      // try RectangularCompositeSurface
      else if (ent->IsKind(STANDARD_TYPE(StepGeom_RectangularCompositeSurface)))
      {
        occ::handle<StepGeom_RectangularCompositeSurface> RCS =
          occ::down_cast<StepGeom_RectangularCompositeSurface>(aSurf);
        int             nbi = RCS->NbSegmentsI();
        int             nbj = RCS->NbSegmentsJ();
        TopoDS_Compound C;
        B.MakeCompound(C);
        for (int ii = 1; ii <= nbi; ii++)
          for (int j = 1; j <= nbj; j++)
          {
            occ::handle<StepGeom_SurfacePatch> patch = RCS->SegmentsValue(ii, j);
            TopoDS_Face f = TranslateBoundedSurf(patch->ParentSurface(), preci, theLocalFactors);
            if (!f.IsNull())
              B.Add(C, f);
          }
        res = C;
      }
      // try other surfs
      else
        res = TranslateBoundedSurf(aSurf, preci, theLocalFactors);
    }
    else if (ent->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem)))
    {
      occ::handle<StepGeom_GeometricRepresentationItem> GRI =
        occ::down_cast<StepGeom_GeometricRepresentationItem>(ent);
      if (!RA.IsNull())
      {
        occ::handle<STEPControl_ActorRead> anActor = occ::down_cast<STEPControl_ActorRead>(RA);
        occ::handle<Transfer_Binder>       binder;
        if (!anActor.IsNull())
          binder = anActor->TransferShape(GRI, TP, theLocalFactors, isManifold, false, aRange);
        if (!binder.IsNull())
        {
          res = TransferBRep::ShapeResult(binder);
        }
      }
    }
    else
      TP->AddWarning(ent, " Entity is not a Curve, Point, Surface or GeometricRepresentationItem");
    if (!res.IsNull())
    {
      B.Add(S, res);
      TransferBRep::SetShapeResult(TP, ent, res);
    }
    else
      TP->AddWarning(ent, " Entity not mapped to TopoDS");
  }
  myResult = S;
  myError  = StepToTopoDS_BuilderDone;
  done     = true;
}

// ***end DTH Apr/6

// ============================================================================
// Method  : Init
// Purpose : Builds a TopoDS_Solid from StepVisual_TessellatedSolid
// ============================================================================
void StepToTopoDS_Builder::Init(const occ::handle<StepVisual_TessellatedSolid>& theTSo,
                                const occ::handle<Transfer_TransientProcess>&   theTP,
                                const bool                   theReadTessellatedWhenNoBRepOnly,
                                bool&                        theHasGeom,
                                const StepData_Factors&      theLocalFactors,
                                const Message_ProgressRange& theProgress)
{
  StepToTopoDS_TranslateSolid aTranSolid;
  aTranSolid.SetPrecision(Precision());
  aTranSolid.SetMaxTol(MaxTol());

  StepToTopoDS_Tool                                                                       aTool;
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;
  aTool.Init(aMap, theTP);

  StepToTopoDS_NMTool aDummyNMTool;
  aTranSolid.Init(theTSo,
                  theTP,
                  aTool,
                  aDummyNMTool,
                  theReadTessellatedWhenNoBRepOnly,
                  theHasGeom,
                  theLocalFactors,
                  theProgress);

  if (!aTranSolid.IsDone())
  {
    theTP->AddWarning(theTSo, " TessellatedSolid not mapped to TopoDS");
    myError = StepToTopoDS_BuilderOther;
    done    = false;
    return;
  }
  TopoDS_Shape aS = aTranSolid.Value();
  myResult        = TopoDS::Solid(aS);
  myError         = StepToTopoDS_BuilderDone;
  done            = true;
}

// ============================================================================
// Method  : Init
// Purpose : Builds a TopoDS_Shell from StepVisual_TessellatedShell
// ============================================================================
void StepToTopoDS_Builder::Init(const occ::handle<StepVisual_TessellatedShell>& theTSh,
                                const occ::handle<Transfer_TransientProcess>&   theTP,
                                const bool                   theReadTessellatedWhenNoBRepOnly,
                                bool&                        theHasGeom,
                                const StepData_Factors&      theLocalFactors,
                                const Message_ProgressRange& theProgress)
{
  StepToTopoDS_TranslateShell aTranShell;
  aTranShell.SetPrecision(Precision());
  aTranShell.SetMaxTol(MaxTol());

  StepToTopoDS_Tool                                                                       aTool;
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;
  aTool.Init(aMap, theTP);

  StepToTopoDS_NMTool aDummyNMTool;
  aTranShell.Init(theTSh,
                  aTool,
                  aDummyNMTool,
                  theReadTessellatedWhenNoBRepOnly,
                  theHasGeom,
                  theLocalFactors,
                  theProgress);

  if (!aTranShell.IsDone())
  {
    theTP->AddWarning(theTSh, " TessellatedShell not mapped to TopoDS");
    myError = StepToTopoDS_BuilderOther;
    done    = false;
    return;
  }
  TopoDS_Shape aS = aTranShell.Value();
  myResult        = TopoDS::Shell(aS);
  myError         = StepToTopoDS_BuilderDone;
  done            = true;
}

// ============================================================================
// Method  : Init
// Purpose : Builds a TopoDS_Face from StepVisual_TessellatedFace
// ============================================================================
void StepToTopoDS_Builder::Init(const occ::handle<StepVisual_TessellatedFace>& theTF,
                                const occ::handle<Transfer_TransientProcess>&  theTP,
                                const bool              theReadTessellatedWhenNoBRepOnly,
                                bool&                   theHasGeom,
                                const StepData_Factors& theLocalFactors)
{
  StepToTopoDS_TranslateFace aTranFace;
  aTranFace.SetPrecision(Precision());
  aTranFace.SetMaxTol(MaxTol());

  StepToTopoDS_Tool                                                                       aTool;
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;
  aTool.Init(aMap, theTP);

  StepToTopoDS_NMTool aDummyNMTool;
  aTranFace.Init(theTF,
                 aTool,
                 aDummyNMTool,
                 theReadTessellatedWhenNoBRepOnly,
                 theHasGeom,
                 theLocalFactors);
  if (!aTranFace.IsDone())
  {
    theTP->AddWarning(theTF, " TessellatedFace not mapped to TopoDS");
    myError = StepToTopoDS_BuilderOther;
    done    = false;
    return;
  }
  TopoDS_Shape aS = aTranFace.Value();
  myResult        = TopoDS::Face(aS);
  myError         = StepToTopoDS_BuilderDone;
  done            = true;
}

// ============================================================================
// Method  : Init
// Purpose : Builds a TopoDS_Face from StepVisual_TessellatedSurfaceSet
// ============================================================================
void StepToTopoDS_Builder::Init(const occ::handle<StepVisual_TessellatedSurfaceSet>& theTSS,
                                const occ::handle<Transfer_TransientProcess>&        theTP,
                                bool&                                                theHasGeom,
                                const StepData_Factors& theLocalFactors)
{
  StepToTopoDS_TranslateFace aTranFace;
  aTranFace.SetPrecision(Precision());
  aTranFace.SetMaxTol(MaxTol());

  StepToTopoDS_Tool                                                                       aTool;
  NCollection_DataMap<occ::handle<StepShape_TopologicalRepresentationItem>, TopoDS_Shape> aMap;
  aTool.Init(aMap, theTP);

  StepToTopoDS_NMTool aDummyNMTool;
  aTranFace.Init(theTSS, aTool, aDummyNMTool, theLocalFactors);
  theHasGeom = false;

  if (!aTranFace.IsDone())
  {
    theTP->AddWarning(theTSS, " TessellatedSurfaceSet not mapped to TopoDS");
    myError = StepToTopoDS_BuilderOther;
    done    = false;
    return;
  }
  TopoDS_Shape aS = aTranFace.Value();
  myResult        = TopoDS::Face(aS);
  myError         = StepToTopoDS_BuilderDone;
  done            = true;
}

// ============================================================================
// Method  : Value
// Purpose : Returns the result of the mapping
// ============================================================================

const TopoDS_Shape& StepToTopoDS_Builder::Value() const
{
  StdFail_NotDone_Raise_if(!done, "StepToTopoDS_Builder::Value() - no result");
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
