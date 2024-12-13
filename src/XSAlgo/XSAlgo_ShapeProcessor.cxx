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

#include "XSAlgo_ShapeProcessor.hxx"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepTools_ReShape.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <Interface_Static.hxx>
#include <Message_ListOfMsg.hxx>
#include <Resource_Manager.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeExtend_MsgRegistrator.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_TransientListBinder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <UnitsMethods.hxx>

//=============================================================================

XSAlgo_ShapeProcessor::XSAlgo_ShapeProcessor(const ParameterMap&          theParameters,
                                             const DE_ShapeFixParameters& theShapeFixParameters)
: myParameters(theParameters)
{
  FillParameterMap(theShapeFixParameters, myParameters);
}

//=============================================================================

XSAlgo_ShapeProcessor::XSAlgo_ShapeProcessor(const DE_ShapeFixParameters& theParameters)
{
  ParameterMap aMap;
  FillParameterMap(theParameters, aMap);
  myParameters = aMap;
}

//=============================================================================

TopoDS_Shape XSAlgo_ShapeProcessor::ProcessShape(const TopoDS_Shape&          theShape,
                                                 const OperationsFlags&       theOperations,
                                                 const Message_ProgressRange& theProgress)
{
  initializeContext(theShape);
  return ShapeProcess::Perform(myContext, theOperations, theProgress) ? myContext->Result() : theShape;
}

//=============================================================================

void XSAlgo_ShapeProcessor::initializeContext(const TopoDS_Shape& theShape)
{
  myContext = new ShapeProcess_ShapeContext(theShape, nullptr);
  for (const auto& aParameter : myParameters)
  {
    myContext->ResourceManager()->SetResource(aParameter.first.c_str(), aParameter.second.c_str());
  }
  // Read and set detalization level.
  auto aDetalizationLevelPtr = myParameters.find("DetalizationLevel");
  if (aDetalizationLevelPtr != myParameters.end())
  {
    const TopAbs_ShapeEnum aDetalizationLevel = static_cast<TopAbs_ShapeEnum>(std::stoi(aDetalizationLevelPtr->second.c_str()));
    myContext->SetDetalisation(aDetalizationLevel);
  }
  // Read and set non-manifold flag.
  auto aNonManifoldPtr = myParameters.find("NonManifold");
  if (aNonManifoldPtr != myParameters.end())
  {
    const Standard_Boolean aNonManifold = static_cast<Standard_Boolean>(std::stoi(aNonManifoldPtr->second.c_str()));
    myContext->SetNonManifold(aNonManifold);
  }
}

//=============================================================================

void XSAlgo_ShapeProcessor::addMessages(const Handle(ShapeExtend_MsgRegistrator)& theMessages,
                                        const TopoDS_Shape&                       theShape,
                                        Handle(Transfer_Binder)&                  theBinder)
{
  if (theMessages.IsNull())
  {
    return;
  }

  const Message_ListOfMsg* aShapeMessages = theMessages->MapShape().Seek(theShape);
  if (!aShapeMessages)
  {
    return;
  }

  for (Message_ListIteratorOfListOfMsg aMsgIter(*aShapeMessages); aMsgIter.More(); aMsgIter.Next())
  {
    const Message_Msg& aMessage = aMsgIter.Value();
    theBinder->AddWarning(TCollection_AsciiString(aMessage.Value()).ToCString(),
                          TCollection_AsciiString(aMessage.Original()).ToCString());
  }
}

//=============================================================================

void XSAlgo_ShapeProcessor::MergeTransferInfo(const Handle(Transfer_TransientProcess)& theTransientProcess,
                                              const Standard_Integer                   theFirstTPItemIndex) const
{
  if (myContext.IsNull())
  {
    return;
  }

  const TopTools_DataMapOfShapeShape& aShapesMap = myContext->Map();
  Handle(ShapeExtend_MsgRegistrator)  aMessages  = myContext->Messages();
  if (aShapesMap.IsEmpty() && (aMessages.IsNull() || aMessages->MapShape().IsEmpty()))
  {
    return;
  }

  for (Standard_Integer i = std::max(theFirstTPItemIndex, 1); i <= theTransientProcess->NbMapped(); ++i)
  {
    Handle(TransferBRep_ShapeBinder) aShapeBinder = Handle(TransferBRep_ShapeBinder)::DownCast(theTransientProcess->MapItem(i));
    if (aShapeBinder.IsNull() || aShapeBinder->Result().IsNull())
    {
      continue;
    }

    const TopoDS_Shape anOriginalShape = aShapeBinder->Result();

    if (aShapesMap.IsBound(anOriginalShape))
    {
      aShapeBinder->SetResult(aShapesMap.Find(anOriginalShape));
    }
    else if (!anOriginalShape.Location().IsIdentity())
    {
      TopLoc_Location aNullLoc;
      TopoDS_Shape    aTemporaryShape = anOriginalShape.Located(aNullLoc);
      if (aShapesMap.IsBound(aTemporaryShape))
      {
        aShapeBinder->SetResult(aShapesMap.Find(aTemporaryShape));
      }
    }
    else
    {
      // Some of edges may be modified.
      BRepTools_ReShape aReShaper;
      Standard_Boolean  aHasModifiedEdges = Standard_False;
      // Remember modifications.
      for (TopExp_Explorer anExpSE(anOriginalShape, TopAbs_EDGE); anExpSE.More(); anExpSE.Next())
      {
        if (aShapesMap.IsBound(anExpSE.Current()))
        {
          aHasModifiedEdges           = Standard_True;
          TopoDS_Shape aModifiedShape = aShapesMap.Find(anExpSE.Current());
          aReShaper.Replace(anExpSE.Current(), aModifiedShape);
        }
      }
      // Apply modifications and store result in binder.
      if (aHasModifiedEdges)
      {
        TopoDS_Shape aReshapedShape = aReShaper.Apply(anOriginalShape);
        aShapeBinder->SetResult(aReshapedShape);
      }
    }

    // update messages
    addMessages(aMessages, anOriginalShape, aShapeBinder);
  }
}

//=============================================================================

void XSAlgo_ShapeProcessor::MergeTransferInfo(const Handle(Transfer_FinderProcess)& theFinderProcess) const
{
  if (myContext.IsNull())
  {
    return;
  }

  const TopTools_DataMapOfShapeShape& aShapesMap = myContext->Map();
  Handle(ShapeExtend_MsgRegistrator)  aMessages  = myContext->Messages();

  for (TopTools_DataMapIteratorOfDataMapOfShapeShape ShapeShapeIterator(aShapesMap); ShapeShapeIterator.More();
       ShapeShapeIterator.Next())
  {
    const TopoDS_Shape anOriginalShape             = ShapeShapeIterator.Key();
    const TopoDS_Shape aResultShape                = ShapeShapeIterator.Value();

    Handle(TransferBRep_ShapeMapper) aResultMapper = TransferBRep::ShapeMapper(theFinderProcess, aResultShape);
    Handle(Transfer_Binder)          aResultBinder = theFinderProcess->Find(aResultMapper);

    if (aResultBinder.IsNull())
    {
      aResultBinder = new TransferBRep_ShapeBinder(aResultShape);
      //if <orig> shape was split, put entities corresponding to new shapes
      // into Transfer_TransientListBinder.
      if (anOriginalShape.ShapeType() > aResultShape.ShapeType())
      {
        TopoDS_Shape                         aSubShape;
        Handle(Transfer_TransientListBinder) aTransientListBinder = new Transfer_TransientListBinder;
        for (TopoDS_Iterator aSubShapeIter(aResultShape); aSubShapeIter.More(); aSubShapeIter.Next())
        {
          const TopoDS_Shape      aCurrentSubShape = aSubShapeIter.Value();
          Handle(Transfer_Finder) aSubShapeMapper  = TransferBRep::ShapeMapper(theFinderProcess, aCurrentSubShape);
          if (aSubShapeMapper.IsNull())
          {
            continue;
          }

          Handle(Standard_Transient) aTransientResult = theFinderProcess->FindTransient(aSubShapeMapper);
          if (aTransientResult.IsNull())
          {
            continue;
          }
          aTransientListBinder->AddResult(aTransientResult);
          aSubShape = aCurrentSubShape;
        }
        if (aTransientListBinder->NbTransients() == 1)
        {
          aResultBinder = new TransferBRep_ShapeBinder(aSubShape);
        }
        else if (aTransientListBinder->NbTransients() > 1)
        {
          aResultBinder->AddResult(aTransientListBinder);
        }
      }
    }

    Handle(TransferBRep_ShapeMapper) anOriginalMapper = TransferBRep::ShapeMapper(theFinderProcess, anOriginalShape);
    Handle(Transfer_Binder)          anOriginalBinder = theFinderProcess->Find(anOriginalMapper);
    if (anOriginalBinder.IsNull())
    {
      theFinderProcess->Bind(anOriginalMapper, aResultBinder);
    }
    else
    {
      anOriginalBinder->AddResult(aResultBinder);
    }

    // update messages
    addMessages(aMessages, anOriginalShape, aResultBinder);
  }
}

//=============================================================================

TopoDS_Edge XSAlgo_ShapeProcessor::MakeEdgeOnCurve(const TopoDS_Edge& aSourceEdge)
{
  TopoDS_Edge aResult;

  Handle(Geom_Curve) aSourceGeomCurve;
  Standard_Real      aStartParam;
  Standard_Real      anEndParam;
  ShapeAnalysis_Edge anEdgeAnalyzer;
  if (!anEdgeAnalyzer.Curve3d(aSourceEdge, aSourceGeomCurve, aStartParam, anEndParam, Standard_False))
  {
    return aResult;
  }
  const gp_Pnt            aCurveStartPt = aSourceGeomCurve->Value(aStartParam);
  const gp_Pnt            aCurveEndPt   = aSourceGeomCurve->Value(anEndParam);
  BRepBuilderAPI_MakeEdge anEdgeMaker(aSourceGeomCurve, aCurveStartPt, aCurveEndPt, aStartParam, anEndParam);
  ShapeBuild_Edge         SBE;
  SBE.SetRange3d(anEdgeMaker, aStartParam, anEndParam);
  aResult = anEdgeMaker.Edge();
  return aResult;
}

//=============================================================================

Standard_Boolean XSAlgo_ShapeProcessor::CheckPCurve(const TopoDS_Edge&     theEdge,
                                                    const TopoDS_Face&     theFace,
                                                    const Standard_Real    thePrecision,
                                                    const Standard_Boolean theIsSeam)
{
  ShapeAnalysis_Edge anEdgeAnalyzer;

  // Retrieve pcurve and its parameters.
  Standard_Real        aCurve2DParam1;
  Standard_Real        aCurve2DParam2;
  Handle(Geom2d_Curve) aCurve2D;
  if (!anEdgeAnalyzer.PCurve(theEdge, theFace, aCurve2D, aCurve2DParam1, aCurve2DParam2, Standard_False))
  {
    return Standard_False;
  }

  // Check for pcurve longer than surface.
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface(theFace);
  Standard_Real        aFaceSurfaceU1, aFaceSurfaceU2, aFaceSurfaceV1, aFaceSurfaceV2;
  aSurface->Bounds(aFaceSurfaceU1, aFaceSurfaceU2, aFaceSurfaceV1, aFaceSurfaceV2);
  const gp_Pnt2d aCurve2DPoint1   = aCurve2D->Value(aCurve2DParam1);
  const gp_Pnt2d aCurve2DPoint2   = aCurve2D->Value(aCurve2DParam2);
  // Multi-periodic? Better to discard (beware of infinite values)
  const Standard_Real anEdgeSpanX = Abs(aCurve2DPoint1.X() - aCurve2DPoint2.X());
  const Standard_Real anEdgeSpanY = Abs(aCurve2DPoint1.Y() - aCurve2DPoint2.Y());
  // So if span of pcurve along U or V is longer than 6/8 of the surface span, discard it.
  // Why exactly 6/8? No idea, but it's the same as in the original code.
  if (anEdgeSpanX / 8. > (aFaceSurfaceU2 / 6. - aFaceSurfaceU1 / 6.)
      || anEdgeSpanY / 8. > (aFaceSurfaceV2 / 6. - aFaceSurfaceV1 / 6.))
  {
    ShapeBuild_Edge().RemovePCurve(theEdge, theFace);
    return Standard_False;
  }

  // Second Check: 2D and 3D consistency (if the Pcurve has not been dropped)
  // We also check that it does not wrap around too much...
  // Example: UVV in DEGREES on a surface in RADIANS, adjusted = 57 turns!
  Handle(Geom_Curve) aCurve3D;
  Standard_Real      aCurve3DParam1;
  Standard_Real      aCurve3DParam2;
  anEdgeAnalyzer.Curve3d(theEdge, aCurve3D, aCurve3DParam1, aCurve3DParam2, Standard_False);

  const gp_Pnt        aCurve3DPoint1 = aSurface->Value(aCurve2DPoint1.X(), aCurve2DPoint1.Y());
  const gp_Pnt        aCurve3DPoint2 = aSurface->Value(aCurve2DPoint2.X(), aCurve2DPoint2.Y());
  const TopoDS_Vertex aVertex1       = TopExp::FirstVertex(theEdge);
  const TopoDS_Vertex aVertex2       = TopExp::LastVertex(theEdge);
  const gp_Pnt        aPV1           = (aCurve3D.IsNull() ? BRep_Tool::Pnt(aVertex1) : aCurve3D->Value(aCurve3DParam1));
  const gp_Pnt        aPV2           = (aCurve3D.IsNull() ? BRep_Tool::Pnt(aVertex2) : aCurve3D->Value(aCurve3DParam2));
  const Standard_Real aDist11        = aPV1.Distance(aCurve3DPoint1);
  const Standard_Real aDist22        = aPV2.Distance(aCurve3DPoint2);

  if (!((aDist11 <= thePrecision) && (aDist22 <= thePrecision)))
  {
    ShapeBuild_Edge().RemovePCurve(theEdge, theFace);
    return Standard_False;
  }

  //
  // pdn checking deviation between pcurve and 3D curve
  //

  // Make temporary edge for analysis
  if (aCurve3D.IsNull())
  {
    return Standard_False;
  }
  TopoDS_Edge aTmpEdge = MakeEdgeOnCurve(theEdge);

  // fill it with pcurve(s)
  BRep_Builder         aBuilder;
  Handle(Geom2d_Curve) aSeamPCurve;
  if (theIsSeam)
  {
    Standard_Real aSeamPCurveParam1;
    Standard_Real aSeamPCurveParam2;
    TopoDS_Edge   aReversedEdge = TopoDS::Edge(theEdge.Reversed());
    if (!anEdgeAnalyzer.PCurve(aReversedEdge, theFace, aSeamPCurve, aSeamPCurveParam1, aSeamPCurveParam2, Standard_False)
        || aSeamPCurve == aCurve2D)
    {
      aSeamPCurve = Handle(Geom2d_Curve)::DownCast(aCurve2D->Copy());
    }
    aBuilder.UpdateEdge(aTmpEdge, aCurve2D, aSeamPCurve, theFace, 0.);
  }
  else
  {
    aBuilder.UpdateEdge(aTmpEdge, aCurve2D, theFace, 0.);
  }
  aBuilder.Range(aTmpEdge, theFace, aCurve2DParam1, aCurve2DParam2);
  aBuilder.SameRange(aTmpEdge, Standard_False);
  if (Interface_Static::IVal("read.stdsameparameter.mode"))
  {
    aBuilder.SameParameter(aTmpEdge, Standard_False);
  }

  // call FixSP to see what it will do
  Handle(ShapeFix_Edge) anEdgeFixer = new ShapeFix_Edge;
  anEdgeFixer->FixSameParameter(aTmpEdge);
  Standard_Real    aTolerance         = BRep_Tool::Tolerance(aTmpEdge);
  Standard_Boolean aSameRangeFlag     = BRep_Tool::SameRange(aTmpEdge);
  Standard_Boolean aSameParameterFlag = BRep_Tool::SameParameter(aTmpEdge);

  // if result is not nice, try to call projection and take the best
  if (aTolerance > Min(1., 2. * thePrecision) || !aSameRangeFlag)
  {
    //pdn trying to recompute pcurve
    TopoDS_Edge anEdgePr = MakeEdgeOnCurve(theEdge);
    anEdgeFixer->FixAddPCurve(anEdgePr, theFace, theIsSeam, thePrecision);
    anEdgeFixer->FixSameParameter(anEdgePr);
    const Standard_Real aTolerancePr = BRep_Tool::Tolerance(anEdgePr);
    //pdn choose the best pcurve
    if (aTolerancePr < aTolerance || !aSameRangeFlag)
    {
      aSameRangeFlag     = BRep_Tool::SameRange(anEdgePr);
      aSameParameterFlag = BRep_Tool::SameParameter(anEdgePr);
      aTolerance         = aTolerancePr;
      aTmpEdge           = anEdgePr;
    }
  }

  // get corrected pcurve from the temporary edge, and put to original
  anEdgeAnalyzer.PCurve(aTmpEdge, theFace, aCurve2D, aCurve2DParam1, aCurve2DParam2, Standard_False);
  if (theIsSeam)
  {
    Standard_Real aReversedTmpEdgeParam1;
    Standard_Real aReversedTmpEdgeParam2;
    TopoDS_Edge   aReversedTmpEdge = TopoDS::Edge(aTmpEdge.Reversed());
    anEdgeAnalyzer
      .PCurve(aReversedTmpEdge, theFace, aSeamPCurve, aReversedTmpEdgeParam1, aReversedTmpEdgeParam2, Standard_False);
    if (theEdge.Orientation() == TopAbs_REVERSED) //:abv 14.11.01: coneEl.sat loop
    {
      aBuilder.UpdateEdge(theEdge, aSeamPCurve, aCurve2D, theFace, aTolerance);
    }
    else
    {
      aBuilder.UpdateEdge(theEdge, aCurve2D, aSeamPCurve, theFace, aTolerance);
    }
  }
  else
  {
    aBuilder.UpdateEdge(theEdge, aCurve2D, theFace, aTolerance);
  }

  aBuilder.UpdateVertex(aVertex1, aTolerance);
  aBuilder.UpdateVertex(aVertex2, aTolerance);
  aBuilder.Range(theEdge, theFace, aCurve2DParam1, aCurve2DParam2);
  if (BRep_Tool::SameRange(theEdge))
  {
    aBuilder.SameRange(theEdge, aSameRangeFlag);
  }
  if (BRep_Tool::SameParameter(theEdge))
  {
    aBuilder.SameParameter(theEdge, aSameParameterFlag);
  }

  return Standard_True;
}

//=============================================================================

void XSAlgo_ShapeProcessor::FillParameterMap(const DE_ShapeFixParameters&         theParameters,
                                             XSAlgo_ShapeProcessor::ParameterMap& theMap)
{
  // Helper lambda to convert enum to string.
  auto makeString = [](const DE_ShapeFixParameters::FixMode theMode)
  {
    return std::to_string(static_cast<std::underlying_type<DE_ShapeFixParameters::FixMode>::type>(theMode));
  };

  theMap.emplace("ShapeFix.Tolerance3d", std::to_string(theParameters.Tolerance3d));
  theMap.emplace("ShapeFix.MaxTolerance3d", std::to_string(theParameters.MaxTolerance3d));
  theMap.emplace("ShapeFix.MinTolerance3d", std::to_string(theParameters.MinTolerance3d));
  theMap.emplace("DetalizationLevel", std::to_string(theParameters.DetalizationLevel));
  theMap.emplace("NonManifold", std::to_string(theParameters.NonManifold));
  theMap.emplace("ShapeFix.FixFreeShellMode", makeString(theParameters.FixFreeShellMode));
  theMap.emplace("ShapeFix.FixFreeFaceMode", makeString(theParameters.FixFreeFaceMode));
  theMap.emplace("ShapeFix.FixFreeWireMode", makeString(theParameters.FixFreeWireMode));
  theMap.emplace("ShapeFix.FixSameParameterMode", makeString(theParameters.FixSameParameterMode));
  theMap.emplace("ShapeFix.FixSolidMode", makeString(theParameters.FixSolidMode));
  theMap.emplace("ShapeFix.FixShellOrientationMode", makeString(theParameters.FixShellOrientationMode));
  theMap.emplace("ShapeFix.CreateOpenSolidMode", makeString(theParameters.CreateOpenSolidMode));
  theMap.emplace("ShapeFix.FixShellMode", makeString(theParameters.FixShellMode));
  theMap.emplace("ShapeFix.FixFaceOrientationMode", makeString(theParameters.FixFaceOrientationMode));
  theMap.emplace("ShapeFix.FixFaceMode", makeString(theParameters.FixFaceMode));
  theMap.emplace("ShapeFix.FixWireMode", makeString(theParameters.FixWireMode));
  theMap.emplace("ShapeFix.FixOrientationMode", makeString(theParameters.FixOrientationMode));
  theMap.emplace("ShapeFix.FixAddNaturalBoundMode", makeString(theParameters.FixAddNaturalBoundMode));
  theMap.emplace("ShapeFix.FixMissingSeamMode", makeString(theParameters.FixMissingSeamMode));
  theMap.emplace("ShapeFix.FixSmallAreaWireMode", makeString(theParameters.FixSmallAreaWireMode));
  theMap.emplace("ShapeFix.RemoveSmallAreaFaceMode", makeString(theParameters.RemoveSmallAreaFaceMode));
  theMap.emplace("ShapeFix.FixIntersectingWiresMode", makeString(theParameters.FixIntersectingWiresMode));
  theMap.emplace("ShapeFix.FixLoopWiresMode", makeString(theParameters.FixLoopWiresMode));
  theMap.emplace("ShapeFix.FixSplitFaceMode", makeString(theParameters.FixSplitFaceMode));
  theMap.emplace("ShapeFix.AutoCorrectPrecisionMode", makeString(theParameters.AutoCorrectPrecisionMode));
  theMap.emplace("ShapeFix.ModifyTopologyMode", makeString(theParameters.ModifyTopologyMode));
  theMap.emplace("ShapeFix.ModifyGeometryMode", makeString(theParameters.ModifyGeometryMode));
  theMap.emplace("ShapeFix.ClosedWireMode", makeString(theParameters.ClosedWireMode));
  theMap.emplace("ShapeFix.PreferencePCurveMode", makeString(theParameters.PreferencePCurveMode));
  theMap.emplace("ShapeFix.FixReorderMode", makeString(theParameters.FixReorderMode));
  theMap.emplace("ShapeFix.FixSmallMode", makeString(theParameters.FixSmallMode));
  theMap.emplace("ShapeFix.FixConnectedMode", makeString(theParameters.FixConnectedMode));
  theMap.emplace("ShapeFix.FixEdgeCurvesMode", makeString(theParameters.FixEdgeCurvesMode));
  theMap.emplace("ShapeFix.FixDegeneratedMode", makeString(theParameters.FixDegeneratedMode));
  theMap.emplace("ShapeFix.FixLackingMode", makeString(theParameters.FixLackingMode));
  theMap.emplace("ShapeFix.FixSelfIntersectionMode", makeString(theParameters.FixSelfIntersectionMode));
  theMap.emplace("ShapeFix.RemoveLoopMode", makeString(theParameters.RemoveLoopMode));
  theMap.emplace("ShapeFix.FixReversed2dMode", makeString(theParameters.FixReversed2dMode));
  theMap.emplace("ShapeFix.FixRemovePCurveMode", makeString(theParameters.FixRemovePCurveMode));
  theMap.emplace("ShapeFix.FixRemoveCurve3dMode", makeString(theParameters.FixRemoveCurve3dMode));
  theMap.emplace("ShapeFix.FixAddPCurveMode", makeString(theParameters.FixAddPCurveMode));
  theMap.emplace("ShapeFix.FixAddCurve3dMode", makeString(theParameters.FixAddCurve3dMode));
  theMap.emplace("ShapeFix.FixSeamMode", makeString(theParameters.FixSeamMode));
  theMap.emplace("ShapeFix.FixShiftedMode", makeString(theParameters.FixShiftedMode));
  theMap.emplace("ShapeFix.FixEdgeSameParameterMode", makeString(theParameters.FixEdgeSameParameterMode));
  theMap.emplace("ShapeFix.FixNotchedEdgesMode", makeString(theParameters.FixNotchedEdgesMode));
  theMap.emplace("ShapeFix.FixTailMode", makeString(theParameters.FixTailMode));
  theMap.emplace("ShapeFix.MaxTailAngle", makeString(theParameters.MaxTailAngle));
  theMap.emplace("ShapeFix.MaxTailWidth", makeString(theParameters.MaxTailWidth));
  theMap.emplace("ShapeFix.FixSelfIntersectingEdgeMode", makeString(theParameters.FixSelfIntersectingEdgeMode));
  theMap.emplace("ShapeFix.FixIntersectingEdgesMode", makeString(theParameters.FixIntersectingEdgesMode));
  theMap.emplace("ShapeFix.FixNonAdjacentIntersectingEdgesMode", makeString(theParameters.FixNonAdjacentIntersectingEdgesMode));
  theMap.emplace("ShapeFix.FixVertexPositionMode", makeString(theParameters.FixVertexPositionMode));
  theMap.emplace("ShapeFix.FixVertexToleranceMode", makeString(theParameters.FixVertexToleranceMode));
}

//=============================================================================

void XSAlgo_ShapeProcessor::PrepareForTransfer()
{
  UnitsMethods::SetCasCadeLengthUnit(Interface_Static::IVal("xstep.cascade.unit"));
}
