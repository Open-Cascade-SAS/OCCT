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

#include <sstream>
#include <unordered_set>

namespace
{
//! Function to split a string based on multiple delimiters.
//! @param aString String to split.
//! @param delimiters Set of delimiters.
//! @return Vector of tokens.
std::vector<std::string> splitString(const std::string&              aString,
                                     const std::unordered_set<char>& delimiters)
{
  std::vector<std::string> aResult;
  std::string              aCurrentToken;

  for (char aCurrentCharacter : aString)
  {
    if (delimiters.find(aCurrentCharacter) != delimiters.end())
    {
      if (!aCurrentToken.empty())
      {
        aResult.emplace_back(std::move(aCurrentToken));
        aCurrentToken.clear();
      }
    }
    else
    {
      aCurrentToken += aCurrentCharacter;
    }
  }

  if (!aCurrentToken.empty())
  {
    aResult.emplace_back(std::move(aCurrentToken));
  }

  return aResult;
}
} // namespace

//=============================================================================

XSAlgo_ShapeProcessor::XSAlgo_ShapeProcessor(const ParameterMap&          theParameters,
                                             const DE_ShapeFixParameters& theShapeFixParameters)
    : myParameters(theParameters)
{
  FillParameterMap(theShapeFixParameters, false, myParameters);
}

//=============================================================================

XSAlgo_ShapeProcessor::XSAlgo_ShapeProcessor(const DE_ShapeFixParameters& theParameters)
{
  ParameterMap aMap;
  FillParameterMap(theParameters, false, aMap);
  myParameters = aMap;
}

//=============================================================================

TopoDS_Shape XSAlgo_ShapeProcessor::ProcessShape(const TopoDS_Shape&          theShape,
                                                 const OperationsFlags&       theOperations,
                                                 const Message_ProgressRange& theProgress)
{
  if (theShape.IsNull())
  {
    return theShape;
  }

  initializeContext(theShape);
  return ShapeProcess::Perform(myContext, theOperations, theProgress) ? myContext->Result()
                                                                      : theShape;
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
    const TopAbs_ShapeEnum aDetalizationLevel =
      static_cast<TopAbs_ShapeEnum>(std::stoi(aDetalizationLevelPtr->second.c_str()));
    myContext->SetDetalisation(aDetalizationLevel);
  }
  // Read and set non-manifold flag.
  auto aNonManifoldPtr = myParameters.find("NonManifold");
  if (aNonManifoldPtr != myParameters.end())
  {
    const Standard_Boolean aNonManifold =
      static_cast<Standard_Boolean>(std::stoi(aNonManifoldPtr->second.c_str()));
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

void XSAlgo_ShapeProcessor::MergeShapeTransferInfo(
  const Handle(Transfer_TransientProcess)& theTransientProcess,
  const TopTools_DataMapOfShapeShape&      theModifiedShapesMap,
  const Standard_Integer                   theFirstTPItemIndex,
  Handle(ShapeExtend_MsgRegistrator)       theMessages)
{
  if (theModifiedShapesMap.IsEmpty())
  {
    return;
  }
  const bool aToPrint = !theMessages.IsNull() && !theMessages->MapShape().IsEmpty();
  for (Standard_Integer i = std::max(theFirstTPItemIndex, 1); i <= theTransientProcess->NbMapped();
       ++i)
  {
    Handle(TransferBRep_ShapeBinder) aShapeBinder =
      Handle(TransferBRep_ShapeBinder)::DownCast(theTransientProcess->MapItem(i));
    if (aShapeBinder.IsNull() || aShapeBinder->Result().IsNull())
    {
      continue;
    }

    const TopoDS_Shape anOriginalShape = aShapeBinder->Result();

    if (theModifiedShapesMap.IsBound(anOriginalShape))
    {
      aShapeBinder->SetResult(theModifiedShapesMap.Find(anOriginalShape));
    }
    else if (!anOriginalShape.Location().IsIdentity())
    {
      TopLoc_Location aNullLoc;
      TopoDS_Shape    aTemporaryShape = anOriginalShape.Located(aNullLoc);
      if (theModifiedShapesMap.IsBound(aTemporaryShape))
      {
        aShapeBinder->SetResult(theModifiedShapesMap.Find(aTemporaryShape));
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
        if (theModifiedShapesMap.IsBound(anExpSE.Current()))
        {
          aHasModifiedEdges           = Standard_True;
          TopoDS_Shape aModifiedShape = theModifiedShapesMap.Find(anExpSE.Current());
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
    if (aToPrint)
    {
      addMessages(theMessages, anOriginalShape, aShapeBinder);
    }
  }
}

//=============================================================================

void XSAlgo_ShapeProcessor::MergeTransferInfo(
  const Handle(Transfer_TransientProcess)& theTransientProcess,
  const Standard_Integer                   theFirstTPItemIndex) const
{
  if (myContext.IsNull())
  {
    return;
  }
  return MergeShapeTransferInfo(theTransientProcess,
                                myContext->Map(),
                                theFirstTPItemIndex,
                                myContext->Messages());
}

//=============================================================================

void XSAlgo_ShapeProcessor::MergeShapeTransferInfo(
  const Handle(Transfer_FinderProcess)& theFinderProcess,
  const TopTools_DataMapOfShapeShape&   theModifiedShapesMap,
  Handle(ShapeExtend_MsgRegistrator)    theMessages)
{
  if (theModifiedShapesMap.IsEmpty())
  {
    return;
  }
  const bool aToPrint = !theMessages.IsNull() && !theMessages->MapShape().IsEmpty();

  for (TopTools_DataMapIteratorOfDataMapOfShapeShape ShapeShapeIterator(theModifiedShapesMap);
       ShapeShapeIterator.More();
       ShapeShapeIterator.Next())
  {
    const TopoDS_Shape anOriginalShape = ShapeShapeIterator.Key();
    const TopoDS_Shape aResultShape    = ShapeShapeIterator.Value();

    Handle(TransferBRep_ShapeMapper) aResultMapper =
      TransferBRep::ShapeMapper(theFinderProcess, aResultShape);
    Handle(Transfer_Binder) aResultBinder = theFinderProcess->Find(aResultMapper);

    if (aResultBinder.IsNull())
    {
      aResultBinder = new TransferBRep_ShapeBinder(aResultShape);
      // if <orig> shape was split, put entities corresponding to new shapes
      //  into Transfer_TransientListBinder.
      if (anOriginalShape.ShapeType() > aResultShape.ShapeType())
      {
        TopoDS_Shape                         aSubShape;
        Handle(Transfer_TransientListBinder) aTransientListBinder =
          new Transfer_TransientListBinder;
        for (TopoDS_Iterator aSubShapeIter(aResultShape); aSubShapeIter.More();
             aSubShapeIter.Next())
        {
          const TopoDS_Shape      aCurrentSubShape = aSubShapeIter.Value();
          Handle(Transfer_Finder) aSubShapeMapper =
            TransferBRep::ShapeMapper(theFinderProcess, aCurrentSubShape);
          if (aSubShapeMapper.IsNull())
          {
            continue;
          }

          Handle(Standard_Transient) aTransientResult =
            theFinderProcess->FindTransient(aSubShapeMapper);
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

    Handle(TransferBRep_ShapeMapper) anOriginalMapper =
      TransferBRep::ShapeMapper(theFinderProcess, anOriginalShape);
    Handle(Transfer_Binder) anOriginalBinder = theFinderProcess->Find(anOriginalMapper);
    if (anOriginalBinder.IsNull())
    {
      theFinderProcess->Bind(anOriginalMapper, aResultBinder);
    }
    else
    {
      anOriginalBinder->AddResult(aResultBinder);
    }

    // update messages
    if (aToPrint)
    {
      addMessages(theMessages, anOriginalShape, aResultBinder);
    }
  }
}

//=============================================================================

void XSAlgo_ShapeProcessor::MergeTransferInfo(
  const Handle(Transfer_FinderProcess)& theFinderProcess) const
{
  if (myContext.IsNull())
  {
    return;
  }
  return MergeShapeTransferInfo(theFinderProcess, myContext->Map(), myContext->Messages());
}

//=============================================================================

TopoDS_Edge XSAlgo_ShapeProcessor::MakeEdgeOnCurve(const TopoDS_Edge& aSourceEdge)
{
  TopoDS_Edge aResult;

  Handle(Geom_Curve) aSourceGeomCurve;
  Standard_Real      aStartParam;
  Standard_Real      anEndParam;
  ShapeAnalysis_Edge anEdgeAnalyzer;
  if (!anEdgeAnalyzer
         .Curve3d(aSourceEdge, aSourceGeomCurve, aStartParam, anEndParam, Standard_False))
  {
    return aResult;
  }
  const gp_Pnt            aCurveStartPt = aSourceGeomCurve->Value(aStartParam);
  const gp_Pnt            aCurveEndPt   = aSourceGeomCurve->Value(anEndParam);
  BRepBuilderAPI_MakeEdge anEdgeMaker(aSourceGeomCurve,
                                      aCurveStartPt,
                                      aCurveEndPt,
                                      aStartParam,
                                      anEndParam);
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
  if (!anEdgeAnalyzer
         .PCurve(theEdge, theFace, aCurve2D, aCurve2DParam1, aCurve2DParam2, Standard_False))
  {
    return Standard_False;
  }

  // Check for pcurve longer than surface.
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface(theFace);
  Standard_Real        aFaceSurfaceU1, aFaceSurfaceU2, aFaceSurfaceV1, aFaceSurfaceV2;
  aSurface->Bounds(aFaceSurfaceU1, aFaceSurfaceU2, aFaceSurfaceV1, aFaceSurfaceV2);
  const gp_Pnt2d aCurve2DPoint1 = aCurve2D->Value(aCurve2DParam1);
  const gp_Pnt2d aCurve2DPoint2 = aCurve2D->Value(aCurve2DParam2);
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
  const gp_Pnt        aPV1 =
    (aCurve3D.IsNull() ? BRep_Tool::Pnt(aVertex1) : aCurve3D->Value(aCurve3DParam1));
  const gp_Pnt aPV2 =
    (aCurve3D.IsNull() ? BRep_Tool::Pnt(aVertex2) : aCurve3D->Value(aCurve3DParam2));
  const Standard_Real aDist11 = aPV1.Distance(aCurve3DPoint1);
  const Standard_Real aDist22 = aPV2.Distance(aCurve3DPoint2);

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
    if (!anEdgeAnalyzer.PCurve(aReversedEdge,
                               theFace,
                               aSeamPCurve,
                               aSeamPCurveParam1,
                               aSeamPCurveParam2,
                               Standard_False)
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
    // pdn trying to recompute pcurve
    TopoDS_Edge anEdgePr = MakeEdgeOnCurve(theEdge);
    anEdgeFixer->FixAddPCurve(anEdgePr, theFace, theIsSeam, thePrecision);
    anEdgeFixer->FixSameParameter(anEdgePr);
    const Standard_Real aTolerancePr = BRep_Tool::Tolerance(anEdgePr);
    // pdn choose the best pcurve
    if (aTolerancePr < aTolerance || !aSameRangeFlag)
    {
      aSameRangeFlag     = BRep_Tool::SameRange(anEdgePr);
      aSameParameterFlag = BRep_Tool::SameParameter(anEdgePr);
      aTolerance         = aTolerancePr;
      aTmpEdge           = anEdgePr;
    }
  }

  // get corrected pcurve from the temporary edge, and put to original
  anEdgeAnalyzer
    .PCurve(aTmpEdge, theFace, aCurve2D, aCurve2DParam1, aCurve2DParam2, Standard_False);
  if (theIsSeam)
  {
    Standard_Real aReversedTmpEdgeParam1;
    Standard_Real aReversedTmpEdgeParam2;
    TopoDS_Edge   aReversedTmpEdge = TopoDS::Edge(aTmpEdge.Reversed());
    anEdgeAnalyzer.PCurve(aReversedTmpEdge,
                          theFace,
                          aSeamPCurve,
                          aReversedTmpEdgeParam1,
                          aReversedTmpEdgeParam2,
                          Standard_False);
    if (theEdge.Orientation() == TopAbs_REVERSED) //: abv 14.11.01: coneEl.sat loop
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

XSAlgo_ShapeProcessor::ProcessingData XSAlgo_ShapeProcessor::ReadProcessingData(
  const std::string& theFileResourceName,
  const std::string& theScopeResourceName)
{
  const Standard_CString            aFileName = Interface_Static::CVal(theFileResourceName.c_str());
  Handle(ShapeProcess_ShapeContext) aContext =
    new ShapeProcess_ShapeContext(TopoDS_Shape(), aFileName);
  if (!aContext->ResourceManager()->IsInitialized())
  {
    // If resource file wasn't found, use static values instead
    Interface_Static::FillMap(aContext->ResourceManager()->GetMap());
  }
  const std::string aScope = Interface_Static::CVal(theScopeResourceName.c_str());

  // Copy parameters to the result.
  ParameterMap                                    aResultParameters;
  OperationsFlags                                 aResultFlags;
  const Resource_DataMapOfAsciiStringAsciiString& aMap = aContext->ResourceManager()->GetMap();
  using RMapIter = Resource_DataMapOfAsciiStringAsciiString::Iterator;
  for (RMapIter anIter(aMap); anIter.More(); anIter.Next())
  {
    std::string  aKey           = anIter.Key().ToCString();
    const size_t aScopePosition = aKey.find(aScope);
    if (aScopePosition != 0)
    {
      // Ignore all parameters that don't start with the specified scope.
      continue;
    }
    // Remove the scope from the key + 1 for the dot.
    // "FromIGES.FixShape.FixFreeFaceMode" -> "FixShape.FixFreeFaceMode"
    aKey.erase(0, aScope.size() + 1);
    if (aKey != "exec.op")
    {
      // If it is not an operation flag, add it to the parameters.
      aResultParameters[aKey] = anIter.Value().ToCString();
    }
    else
    {
      // Parse operations flags.
      const std::vector<std::string> anOperationStrings =
        splitString(anIter.Value().ToCString(), {' ', '\t', ',', ';'});
      for (const auto& anOperationString : anOperationStrings)
      {
        std::pair<ShapeProcess::Operation, bool> anOperationFlag =
          ShapeProcess::ToOperationFlag(anOperationString.c_str());
        if (anOperationFlag.second)
        {
          aResultFlags.set(anOperationFlag.first);
        }
      }
    }
  }
  return {aResultParameters, aResultFlags};
}

//=============================================================================

void XSAlgo_ShapeProcessor::FillParameterMap(const DE_ShapeFixParameters&         theParameters,
                                             const bool                           theIsReplace,
                                             XSAlgo_ShapeProcessor::ParameterMap& theMap)
{
  SetParameter("FixShape.Tolerance3d", theParameters.Tolerance3d, theIsReplace, theMap);
  SetParameter("FixShape.MaxTolerance3d", theParameters.MaxTolerance3d, theIsReplace, theMap);
  SetParameter("FixShape.MinTolerance3d", theParameters.MinTolerance3d, theIsReplace, theMap);
  SetParameter("DetalizationLevel",
               std::to_string(theParameters.DetalizationLevel),
               theIsReplace,
               theMap);
  SetParameter("NonManifold", std::to_string(theParameters.NonManifold), theIsReplace, theMap);
  SetParameter("FixShape.FixFreeShellMode", theParameters.FixFreeShellMode, theIsReplace, theMap);
  SetParameter("FixShape.FixFreeFaceMode", theParameters.FixFreeFaceMode, theIsReplace, theMap);
  SetParameter("FixShape.FixFreeWireMode", theParameters.FixFreeWireMode, theIsReplace, theMap);
  SetParameter("FixShape.FixSameParameterMode",
               theParameters.FixSameParameterMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixSolidMode", theParameters.FixSolidMode, theIsReplace, theMap);
  SetParameter("FixShape.FixShellOrientationMode",
               theParameters.FixShellOrientationMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.CreateOpenSolidMode",
               theParameters.CreateOpenSolidMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixShellMode", theParameters.FixShellMode, theIsReplace, theMap);
  SetParameter("FixShape.FixFaceOrientationMode",
               theParameters.FixFaceOrientationMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixFaceMode", theParameters.FixFaceMode, theIsReplace, theMap);
  SetParameter("FixShape.FixWireMode", theParameters.FixWireMode, theIsReplace, theMap);
  SetParameter("FixShape.FixOrientationMode",
               theParameters.FixOrientationMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixAddNaturalBoundMode",
               theParameters.FixAddNaturalBoundMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixMissingSeamMode",
               theParameters.FixMissingSeamMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixSmallAreaWireMode",
               theParameters.FixSmallAreaWireMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.RemoveSmallAreaFaceMode",
               theParameters.RemoveSmallAreaFaceMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixIntersectingWiresMode",
               theParameters.FixIntersectingWiresMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixLoopWiresMode", theParameters.FixLoopWiresMode, theIsReplace, theMap);
  SetParameter("FixShape.FixSplitFaceMode", theParameters.FixSplitFaceMode, theIsReplace, theMap);
  SetParameter("FixShape.AutoCorrectPrecisionMode",
               theParameters.AutoCorrectPrecisionMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.ModifyTopologyMode",
               theParameters.ModifyTopologyMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.ModifyGeometryMode",
               theParameters.ModifyGeometryMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.ClosedWireMode", theParameters.ClosedWireMode, theIsReplace, theMap);
  SetParameter("FixShape.PreferencePCurveMode",
               theParameters.PreferencePCurveMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixReorderMode", theParameters.FixReorderMode, theIsReplace, theMap);
  SetParameter("FixShape.FixSmallMode", theParameters.FixSmallMode, theIsReplace, theMap);
  SetParameter("FixShape.FixConnectedMode", theParameters.FixConnectedMode, theIsReplace, theMap);
  SetParameter("FixShape.FixEdgeCurvesMode", theParameters.FixEdgeCurvesMode, theIsReplace, theMap);
  SetParameter("FixShape.FixDegeneratedMode",
               theParameters.FixDegeneratedMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixLackingMode", theParameters.FixLackingMode, theIsReplace, theMap);
  SetParameter("FixShape.FixSelfIntersectionMode",
               theParameters.FixSelfIntersectionMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.RemoveLoopMode", theParameters.RemoveLoopMode, theIsReplace, theMap);
  SetParameter("FixShape.FixReversed2dMode", theParameters.FixReversed2dMode, theIsReplace, theMap);
  SetParameter("FixShape.FixRemovePCurveMode",
               theParameters.FixRemovePCurveMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixRemoveCurve3dMode",
               theParameters.FixRemoveCurve3dMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixAddPCurveMode", theParameters.FixAddPCurveMode, theIsReplace, theMap);
  SetParameter("FixShape.FixAddCurve3dMode", theParameters.FixAddCurve3dMode, theIsReplace, theMap);
  SetParameter("FixShape.FixSeamMode", theParameters.FixSeamMode, theIsReplace, theMap);
  SetParameter("FixShape.FixShiftedMode", theParameters.FixShiftedMode, theIsReplace, theMap);
  SetParameter("FixShape.FixEdgeSameParameterMode",
               theParameters.FixEdgeSameParameterMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixNotchedEdgesMode",
               theParameters.FixNotchedEdgesMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixTailMode", theParameters.FixTailMode, theIsReplace, theMap);
  SetParameter("FixShape.MaxTailAngle", theParameters.MaxTailAngle, theIsReplace, theMap);
  SetParameter("FixShape.MaxTailWidth", theParameters.MaxTailWidth, theIsReplace, theMap);
  SetParameter("FixShape.FixSelfIntersectingEdgeMode",
               theParameters.FixSelfIntersectingEdgeMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixIntersectingEdgesMode",
               theParameters.FixIntersectingEdgesMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixNonAdjacentIntersectingEdgesMode",
               theParameters.FixNonAdjacentIntersectingEdgesMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixVertexPositionMode",
               theParameters.FixVertexPositionMode,
               theIsReplace,
               theMap);
  SetParameter("FixShape.FixVertexToleranceMode",
               theParameters.FixVertexToleranceMode,
               theIsReplace,
               theMap);
}

//=============================================================================

void XSAlgo_ShapeProcessor::SetParameter(const char*                    theKey,
                                         DE_ShapeFixParameters::FixMode theValue,
                                         const bool                     theIsReplace,
                                         ParameterMap&                  theMap)
{
  SetParameter(theKey,
               std::to_string(
                 static_cast<std::underlying_type<DE_ShapeFixParameters::FixMode>::type>(theValue)),
               theIsReplace,
               theMap);
}

//=============================================================================

void XSAlgo_ShapeProcessor::SetParameter(const char*   theKey,
                                         double        theValue,
                                         const bool    theIsReplace,
                                         ParameterMap& theMap)
{
  // Note that conversion with std::to_string() here is not possible, since it normally preserves
  // only first 6 digits (before C++26). As a result, any value of 1e-7 or below will turn into 0.
  // By using std::ostringstream with std::setprecision(6) formatting we will preserve first 6
  // SIGNIFICANT digits.
  std::ostringstream aStrStream;
  aStrStream << std::setprecision(6) << theValue;
  SetParameter(theKey, aStrStream.str(), theIsReplace, theMap);
}

//=============================================================================

void XSAlgo_ShapeProcessor::SetParameter(const char*   theKey,
                                         std::string&& theValue,
                                         const bool    theIsReplace,
                                         ParameterMap& theMap)
{
  if (theIsReplace)
  {
    theMap[theKey] = std::move(theValue);
  }
  else
  {
    theMap.emplace(theKey, std::move(theValue));
  }
}

//=============================================================================

void XSAlgo_ShapeProcessor::PrepareForTransfer()
{
  UnitsMethods::SetCasCadeLengthUnit(Interface_Static::IVal("xstep.cascade.unit"));
}
