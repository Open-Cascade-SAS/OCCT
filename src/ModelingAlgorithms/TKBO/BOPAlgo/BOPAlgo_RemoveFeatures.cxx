// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <BOPAlgo_RemoveFeatures.hxx>

#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_BOP.hxx>
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_BuilderSolid.hxx>
#include <BOPAlgo_MakerVolume.hxx>
#include <BOPAlgo_Tools.hxx>
#include <Standard_ErrorHandler.hxx>

#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Parallel.hxx>
#include <BOPTools_Set.hxx>

#include <Bnd_Box.hxx>

#include <BRep_Builder.hxx>

#include <BRepBndLib.hxx>

#include <BRepLib.hxx>

#include <NCollection_Vector.hxx>

#include <ShapeUpgrade_UnifySameDomain.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

//=======================================================================
// static methods declaration
//=======================================================================

static void MakeRemoved(
  const NCollection_List<TopoDS_Shape>&       theShapes,
  BRepTools_History&                theHistory,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theKeepShapes = NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>());

static void FindInternals(const TopoDS_Shape& theS, NCollection_List<TopoDS_Shape>& theLInt);

static void RemoveInternalWires(const NCollection_List<TopoDS_Shape>& theShapes,
                                NCollection_List<TopoDS_Shape>*       theRemoved = NULL);

static void GetOriginalFaces(const TopoDS_Shape&                              theShape,
                             const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                theSolids,
                             const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                       theFeatureFacesMap,
                             const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theAdjFaces,
                             const occ::handle<BRepTools_History>&                 theHistory,
                             NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                      theFacesToBeKept,
                             NCollection_List<TopoDS_Shape>&                            theInternalShapes,
                             NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                             theFacesToCheckOri,
                             NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                      theSolidsToRebuild,
                             NCollection_List<TopoDS_Shape>&                            theSharedFaces,
                             NCollection_List<TopoDS_Shape>&                            theUnTouchedSolids);

static void FindShape(const TopoDS_Shape& theSWhat,
                      const TopoDS_Shape& theSWhere,
                      TopoDS_Shape&       theSFound);

static void GetValidSolids(BOPAlgo_MakerVolume&        theMV,
                           const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theFacesToCheckOri,
                           const NCollection_List<TopoDS_Shape>& aSharedFaces,
                           const TopoDS_Shape&         theOrigFaces,
                           const int      theNbSol,
                           NCollection_List<TopoDS_Shape>&       theLSRes,
                           NCollection_List<TopoDS_Shape>&       theRemovedShapes);

static void FindExtraShapes(const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theConnectionMap,
                            const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                       theShapesToCheckOri,
                            BOPAlgo_Builder&                                 theBuilder,
                            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                             theShapesToAvoid,
                            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>*                             theValidShapes = NULL);

static void AvoidExtraSharedFaces(NCollection_List<TopoDS_Shape>&       theLSolids,
                                  const NCollection_List<TopoDS_Shape>& theLFSharedToAvoid,
                                  BOPAlgo_Builder&            theBuilder,
                                  NCollection_List<TopoDS_Shape>&       theExtraFaces);

static void FillSolidsHistory(const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                theSolIn,
                              NCollection_List<TopoDS_Shape>&                            theSolidsRes,
                              const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theAdjFaces,
                              BOPAlgo_Builder&                                 theBuilder,
                              BRepTools_History&                               theSolidsHistory);

static void TakeModified(const TopoDS_Shape&   theS,
                         BOPAlgo_Builder&      theBuilder,
                         NCollection_List<TopoDS_Shape>& theList);

static void TakeModified(const TopoDS_Shape&  theS,
                         BOPAlgo_Builder&     theBuilder,
                         NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap);

static void FindSolid(const TopoDS_Shape&                              theSolIn,
                      const NCollection_List<TopoDS_Shape>&                      theSolidsRes,
                      const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theAdjFaces,
                      BOPAlgo_Builder&                                 theBuilder,
                      TopoDS_Shape&                                    theSolOut);

namespace
{
//=======================================================================
// function : BOPAlgo_PIOperation
// purpose  : List of operations to be supported by the Progress Indicator
//=======================================================================
enum BOPAlgo_PIOperation
{
  PIOperation_PrepareFeatures = 0,
  PIOperation_RemoveFeatures,
  PIOperation_UpdateHistory,
  PIOperation_SimplifyResult,

  PIOperation_Last
};
} // namespace

//=================================================================================================

void BOPAlgo_RemoveFeatures::fillPIConstants(const double theWhole,
                                             BOPAlgo_PISteps&    theSteps) const
{
  theSteps.SetStep(PIOperation_PrepareFeatures, 0.05 * theWhole);
  theSteps.SetStep(PIOperation_RemoveFeatures, 0.8 * theWhole);
  theSteps.SetStep(PIOperation_UpdateHistory, 0.05 * theWhole);
  theSteps.SetStep(PIOperation_SimplifyResult, 0.1 * theWhole);
}

//=======================================================================
// function: Perform
// purpose: Performs the removal of the requested faces from the input shape
//=======================================================================
void BOPAlgo_RemoveFeatures::Perform(const Message_ProgressRange& theRange)
{
  try
  {
    OCC_CATCH_SIGNALS

    if (HasHistory())
      myHistory = new BRepTools_History();

    // Check the input data
    CheckData();
    if (HasErrors())
      return;
    Message_ProgressScope aPS(theRange, "Removing features", 100);
    BOPAlgo_PISteps       aSteps(PIOperation_Last);
    analyzeProgress(100., aSteps);

    // Prepare the faces to remove.
    PrepareFeatures(aPS.Next(aSteps.GetStep(PIOperation_PrepareFeatures)));
    if (HasErrors())
    {
      return;
    }

    // Remove the features and fill the created gaps
    RemoveFeatures(aPS.Next(aSteps.GetStep(PIOperation_RemoveFeatures)));
    if (HasErrors())
    {
      return;
    }

    // Update history with the removed features
    UpdateHistory(aPS.Next(aSteps.GetStep(PIOperation_UpdateHistory)));
    if (HasErrors())
    {
      return;
    }
    // Simplify the result
    SimplifyResult(aPS.Next(aSteps.GetStep(PIOperation_SimplifyResult)));
    if (HasErrors())
    {
      return;
    }
    // Post treatment
    PostTreat();
  }
  catch (Standard_Failure const&)
  {
    AddError(new BOPAlgo_AlertRemoveFeaturesFailed());
  }
}

//=======================================================================
// function: CheckData
// purpose: Checks the input data on validity for the algorithm
//=======================================================================
void BOPAlgo_RemoveFeatures::CheckData()
{
  // Prepare the shape to work with
  myShape = myInputShape;

  // Check the type of input shape
  const TopAbs_ShapeEnum aType = myInputShape.ShapeType();

  if (aType == TopAbs_SOLID || aType == TopAbs_COMPSOLID)
    return; // OK

  if (aType == TopAbs_COMPOUND)
  {
    NCollection_List<TopoDS_Shape> aShapes;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>  aMFence;
    // Extract all shapes from the compound
    BOPTools_AlgoTools::TreatCompound(myInputShape, aShapes, &aMFence);
    if (aShapes.IsEmpty())
    {
      // Add error of empty input shape
      AddError(new BOPAlgo_AlertEmptyShape(myInputShape));
      return;
    }

    // Find all solids in the list of shapes
    NCollection_List<TopoDS_Shape>               aSolids;
    NCollection_List<TopoDS_Shape>               anOtherShapes;
    NCollection_List<TopoDS_Shape>::Iterator aIt(aShapes);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aS = aIt.Value();
      if (aS.ShapeType() == TopAbs_SOLID || aS.ShapeType() == TopAbs_COMPSOLID)
        aSolids.Append(aS);
      else
        anOtherShapes.Append(aS);
    }

    if (aSolids.IsEmpty())
    {
      // No solids have been found for processing.
      // Add error of unsupported type of input shape
      AddError(new BOPAlgo_AlertTooFewArguments());
    }
    else if (anOtherShapes.Extent() > 0)
    {
      // Add warning of unsupported type of input shape for all
      // non-solid shapes, contained in the input shape
      for (aIt.Initialize(anOtherShapes); aIt.More(); aIt.Next())
      {
        AddWarning(new BOPAlgo_AlertUnsupportedType(aIt.Value()));
      }

      // Collect all solids into compound and overwrite the shape to rebuild
      TopoDS_Compound aCS;
      BRep_Builder().MakeCompound(aCS);
      for (aIt.Initialize(aSolids); aIt.More(); aIt.Next())
        BRep_Builder().Add(aCS, aIt.Value());

      myShape = aCS;

      if (HasHistory())
      {
        // Make non solid shapes removed in the history
        MakeRemoved(anOtherShapes, *myHistory.get());
      }
    }
  }
  else
  {
    // Add error of unsupported type of input shape
    AddError(new BOPAlgo_AlertTooFewArguments());
  }
}

//=======================================================================
// function: PrepareFeatures
// purpose: Prepares the features to remove
//=======================================================================
void BOPAlgo_RemoveFeatures::PrepareFeatures(const Message_ProgressRange& theRange)
{
  // Map all sub-shapes of the input solids
  TopExp::MapShapes(myInputShape, myInputsMap);

  // Collect all faces of the input shape requested for removal
  NCollection_List<TopoDS_Shape>               aFacesToRemove;
  NCollection_List<TopoDS_Shape>::Iterator aIt(myFacesToRemove);
  Message_ProgressScope              aPSOuter(theRange, "Preparing features", 2);
  Message_ProgressScope              aPS(aPSOuter.Next(),
                            "Preparing the faces to remove",
                            myFacesToRemove.Size());
  for (; aIt.More(); aIt.Next(), aPS.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();
    TopExp_Explorer     anExpF(aS, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
    {
      if (UserBreak(aPS))
      {
        return;
      }
      const TopoDS_Shape& aF = anExpF.Current();
      if (myInputsMap.Contains(aF))
        aFacesToRemove.Append(aF);
    }
  }

  if (aFacesToRemove.IsEmpty())
  {
    // Add error, that no features to remove have been found
    AddError(new BOPAlgo_AlertNoFacesToRemove());
    return;
  }

  // Build connexity blocks of the faces to remove
  TopoDS_Compound aCFToRemove;
  BRep_Builder().MakeCompound(aCFToRemove);
  for (aIt.Initialize(aFacesToRemove); aIt.More(); aIt.Next())
    BRep_Builder().Add(aCFToRemove, aIt.Value());

  // Fill the list of features with connexity blocks of faces
  BOPTools_AlgoTools::MakeConnexityBlocks(aCFToRemove, TopAbs_EDGE, TopAbs_FACE, myFeatures);
}

//=======================================================================
// Adjacent faces extension block

//=======================================================================
// class: FillGaps
// purpose: Auxiliary class for creation of the faces for filling the gap
//          created by removal of the single feature
//=======================================================================
class FillGap
{
public: //! @name Constructors
  //! Empty constructor
  FillGap()
      : myRunParallel(false),
        myHasAdjacentFaces(false)
  {
  }

public: //! @name Setters/Getters
  //! Sets the feature to remove
  void SetFeature(const TopoDS_Shape& theFeature) { myFeature = theFeature; }

  //! Returns the feature
  const TopoDS_Shape& Feature() const { return myFeature; }

  //! Sets the EF connection map
  void SetEFConnectionMap(const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theEFMap)
  {
    myEFMap = (NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>*)&theEFMap;
  }

  //! Sets the FS connection map
  void SetFSConnectionMap(const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theFSMap)
  {
    myFSMap = (NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>*)&theFSMap;
  }

  //! Defines the parallel processing mode
  void SetRunParallel(const bool bRunParallel) { myRunParallel = bRunParallel; }

  //! Gets the History object
  const occ::handle<BRepTools_History>& History() { return myHistory; }

  void SetRange(const Message_ProgressRange& theRange) { myRange = theRange; }

public: //! @name Perform the operation
  //! Performs the extension of the adjacent faces and
  //! then trims the extended faces to fill the gaps
  void Perform()
  {
    OCC_CATCH_SIGNALS

    try
    {
      Message_ProgressScope aPS(myRange, NULL, 3);

      myHistory = new BRepTools_History();

      // Find the faces adjacent to the faces of the feature
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMFAdjacent;
      FindAdjacentFaces(aMFAdjacent, aPS.Next());
      if (!aPS.More())
      {
        return;
      }

      myHasAdjacentFaces = (aMFAdjacent.Extent() > 0);
      if (!myHasAdjacentFaces)
        return;

      // Extend the adjacent faces keeping the connection to the original faces
      NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aFaceExtFaceMap;
      ExtendAdjacentFaces(aMFAdjacent, aFaceExtFaceMap, aPS.Next());
      if (!aPS.More())
      {
        return;
      }

      // Trim the extended faces
      TrimExtendedFaces(aFaceExtFaceMap, aPS.Next());
    }
    catch (Standard_Failure const&)
    {
      // Make sure the warning will be given on the higher level
      myHasAdjacentFaces = true;
      myFaces.Clear();
    }
  }

public: //! @name Obtain the result
  //! Returns the map of faces of the feature
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& FeatureFacesMap() const { return myFeatureFacesMap; }

  //! Shows whether the adjacent faces have been found for the feature
  bool HasAdjacentFaces() const { return myHasAdjacentFaces; }

  //! Returns the Images map of the adjacent faces
  const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& Faces() const { return myFaces; }

  //! Returns the initial solids participating in the feature removal
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Solids() const { return mySolids; }

private: //! @name Private methods performing the operation
  //! Finds the faces adjacent to the feature and stores them into outgoing map.
  void FindAdjacentFaces(NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&  theMFAdjacent,
                         const Message_ProgressRange& theRange)
  {
    // Map the faces of the feature to avoid them in the map of adjacent faces
    TopoDS_Iterator aIt(myFeature);
    for (; aIt.More(); aIt.Next())
      myFeatureFacesMap.Add(aIt.Value());
    Message_ProgressScope aPSOuter(theRange, NULL, 2);
    // Find faces adjacent to the feature using the connection map
    aIt.Initialize(myFeature);
    Message_ProgressScope aPSF(aPSOuter.Next(), "Looking for adjacent faces", 1, true);
    for (; aIt.More(); aIt.Next(), aPSF.Next())
    {
      if (!aPSF.More())
      {
        return;
      }
      const TopoDS_Shape& aF = aIt.Value();
      TopExp_Explorer     anExpE(aF, TopAbs_EDGE);
      for (; anExpE.More(); anExpE.Next())
      {
        const TopoDS_Shape&         aE             = anExpE.Current();
        const NCollection_List<TopoDS_Shape>* pAdjacentFaces = myEFMap->Seek(aE);
        if (pAdjacentFaces)
        {
          NCollection_List<TopoDS_Shape>::Iterator itLFA(*pAdjacentFaces);
          for (; itLFA.More(); itLFA.Next())
          {
            const TopoDS_Shape& anAF = itLFA.Value();
            if (!myFeatureFacesMap.Contains(anAF))
              theMFAdjacent.Add(anAF);
          }
        }
      }
      // Find solids containing the feature face
      const NCollection_List<TopoDS_Shape>* pLS = myFSMap->Seek(aF);
      if (pLS)
      {
        NCollection_List<TopoDS_Shape>::Iterator itLS(*pLS);
        for (; itLS.More(); itLS.Next())
          mySolids.Add(itLS.Value());
      }
    }

    // Find solids containing the edges of adjacent faces
    const int aNbFA = theMFAdjacent.Extent();
    Message_ProgressScope  aPSS(aPSOuter.Next(), "Looking for adjacent solids", aNbFA);
    for (int i = 1; i <= aNbFA; ++i, aPSS.Next())
    {
      if (!aPSS.More())
      {
        return;
      }
      TopExp_Explorer anExpEA(theMFAdjacent(i), TopAbs_EDGE);
      for (; anExpEA.More(); anExpEA.Next())
      {
        // Faces adjacent to the face adjacent to the feature
        const NCollection_List<TopoDS_Shape>* pLFAA = myEFMap->Seek(anExpEA.Current());
        if (pLFAA)
        {
          NCollection_List<TopoDS_Shape>::Iterator itLFAA(*pLFAA);
          for (; itLFAA.More(); itLFAA.Next())
          {
            // Solids containing the faces
            const NCollection_List<TopoDS_Shape>* pLS = myFSMap->Seek(itLFAA.Value());
            if (pLS)
            {
              NCollection_List<TopoDS_Shape>::Iterator itLS(*pLS);
              for (; itLS.More(); itLS.Next())
                mySolids.Add(itLS.Value());
            }
          }
        }
      }
    }
  }

  //! Extends the found adjacent faces and binds them to the original faces.
  void ExtendAdjacentFaces(const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&    theMFAdjacent,
                           NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theFaceExtFaceMap,
                           const Message_ProgressRange&         theRange)
  {
    // Get the extension value for the faces - half of the diagonal of bounding box of the feature
    Bnd_Box aFeatureBox;
    BRepBndLib::Add(myFeature, aFeatureBox);

    const double anExtLength = sqrt(aFeatureBox.SquareExtent());

    const int aNbFA = theMFAdjacent.Extent();
    Message_ProgressScope  aPS(theRange, "Extending adjacent faces", aNbFA);
    for (int i = 1; i <= aNbFA && aPS.More(); ++i, aPS.Next())
    {
      const TopoDS_Face& aF = TopoDS::Face(theMFAdjacent(i));
      // Extend the face
      TopoDS_Face aFExt;
      BRepLib::ExtendFace(aF,
                          anExtLength,
                          true,
                          true,
                          true,
                          true,
                          aFExt);
      theFaceExtFaceMap.Add(aF, aFExt);
      myHistory->AddModified(aF, aFExt);
    }
  }

  //! Trims the extended adjacent faces by intersection with each other
  //! and following intersection with the bounds of original faces.
  void TrimExtendedFaces(const NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theFaceExtFaceMap,
                         const Message_ProgressRange&               theRange)
  {
    // Intersect the extended faces first
    BOPAlgo_Builder aGFInter;
    // Add faces for intersection
    const int aNbF = theFaceExtFaceMap.Extent();
    for (int i = 1; i <= aNbF; ++i)
      aGFInter.AddArgument(theFaceExtFaceMap(i));

    aGFInter.SetRunParallel(myRunParallel);

    // Intersection result
    TopoDS_Shape          anIntResult;
    Message_ProgressScope aPSOuter(theRange, NULL, (aGFInter.Arguments().Extent() > 1) ? 2 : 1);
    if (aGFInter.Arguments().Extent() > 1)
    {
      aGFInter.Perform(aPSOuter.Next());
      if (aGFInter.HasErrors())
        return;

      anIntResult = aGFInter.Shape();

      myHistory->Merge(aGFInter.History());
    }
    else
      anIntResult = aGFInter.Arguments().First();

    // Prepare the EF map of the extended faces after intersection
    // to select from them only boundary edges
    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> anEFExtMap;
    TopExp::MapShapesAndAncestors(anIntResult, TopAbs_EDGE, TopAbs_FACE, anEFExtMap);

    // Get the splits of the extended faces after intersection
    // and trim them by the edges of the original faces

    // Map the edges of the Feature to avoid them during the trim
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFeatureEdgesMap;
    TopExp::MapShapes(myFeature, TopAbs_EDGE, aFeatureEdgesMap);

    Message_ProgressScope aPS(aPSOuter.Next(), "Trimming faces", aNbF);
    for (int i = 1; i <= aNbF && aPS.More(); ++i, aPS.Next())
    {
      const TopoDS_Face& aFOriginal = TopoDS::Face(theFaceExtFaceMap.FindKey(i));
      const TopoDS_Face& aFExt      = TopoDS::Face(theFaceExtFaceMap(i));
      TrimFace(aFExt, aFOriginal, aFeatureEdgesMap, anEFExtMap, aGFInter);
    }
  }

  //! Trim the extended faces by the bounds of the original face,
  //! except those contained in the feature to remove.
  void TrimFace(const TopoDS_Face&                               theFExt,
                const TopoDS_Face&                               theFOriginal,
                const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                theFeatureEdgesMap,
                const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theEFExtMap,
                BOPAlgo_Builder&                                 theGFInter)
  {
    // Map all edges of the extended face, to filter the result of trim
    // from the faces containing these edges
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMExtEdges;
    TopExp_Explorer     anExpE(theFExt, TopAbs_EDGE);
    for (; anExpE.More(); anExpE.Next())
    {
      const TopoDS_Edge& aE = TopoDS::Edge(anExpE.Current());
      // skip degenerated and seam edges
      if (BRep_Tool::Degenerated(aE) || BRep_Tool::IsClosed(aE, theFExt))
        continue;
      NCollection_List<TopoDS_Shape> aLEIm;
      TakeModified(aE, theGFInter, aLEIm);
      NCollection_List<TopoDS_Shape>::Iterator itLEIm(aLEIm);
      for (; itLEIm.More(); itLEIm.Next())
      {
        const TopoDS_Shape& aEIm = itLEIm.Value();
        if (theEFExtMap.FindFromKey(aEIm).Extent() == 1)
          aMExtEdges.Add(aEIm);
      }
    }

    // Trimming tool
    BOPAlgo_Builder aGFTrim;

    // Get the splits of the face and add them for trimming
    NCollection_List<TopoDS_Shape> anExtSplits;
    TakeModified(theFExt, theGFInter, anExtSplits);
    aGFTrim.SetArguments(anExtSplits);

    // Add edges of the original faces
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEdgesToCheckOri;
    anExpE.Init(theFOriginal, TopAbs_EDGE);
    for (; anExpE.More(); anExpE.Next())
    {
      const TopoDS_Edge& aE = TopoDS::Edge(anExpE.Current());
      if (!theFeatureEdgesMap.Contains(aE))
      {
        aGFTrim.AddArgument(aE);
        if (!BRep_Tool::Degenerated(aE) && !BRep_Tool::IsClosed(aE, theFOriginal))
        {
          if (!aMEdgesToCheckOri.Add(aE))
            aMEdgesToCheckOri.Remove(aE);
        }
      }
    }

    // Avoid faces intersection
    aGFTrim.SetGlue(BOPAlgo_GlueShift);
    aGFTrim.SetRunParallel(myRunParallel);
    aGFTrim.SetNonDestructive(true);

    aGFTrim.Perform();
    if (aGFTrim.HasErrors())
      return;

    // Get all splits
    const TopoDS_Shape& aSplits = aGFTrim.Shape();
    // Filter the trimmed faces and save the valid ones into result map
    NCollection_List<TopoDS_Shape> aLFTrimmed;

    TopExp_Explorer anExpF(aSplits, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
    {
      const TopoDS_Shape& aSp = anExpF.Current();
      anExpE.Init(aSp, TopAbs_EDGE);
      for (; anExpE.More(); anExpE.Next())
      {
        if (aMExtEdges.Contains(anExpE.Current()))
          break;
      }
      if (!anExpE.More())
        aLFTrimmed.Append(aSp);
    }

    if (aLFTrimmed.Extent() > 1)
    {
      // Chose the correct faces - the ones that contains edges with proper
      // bi-normal direction
      NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> anEFMap;
      NCollection_List<TopoDS_Shape>::Iterator        itLF(aLFTrimmed);
      for (; itLF.More(); itLF.Next())
        TopExp::MapShapesAndAncestors(itLF.Value(), TopAbs_EDGE, TopAbs_FACE, anEFMap);

      // Check edges orientations
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aFacesToAvoid, aValidFaces;
      FindExtraShapes(anEFMap, aMEdgesToCheckOri, aGFTrim, aFacesToAvoid, &aValidFaces);

      if (aLFTrimmed.Extent() - aFacesToAvoid.Extent() > 1)
      {
        // It is possible that the splits are forming the different blocks.
        // Take only those containing the valid faces.
        TopoDS_Compound aCF;
        BRep_Builder().MakeCompound(aCF);
        itLF.Initialize(aLFTrimmed);
        for (; itLF.More(); itLF.Next())
        {
          if (!aFacesToAvoid.Contains(itLF.Value()))
            BRep_Builder().Add(aCF, itLF.Value());
        }

        NCollection_List<TopoDS_Shape> aLCB;
        BOPTools_AlgoTools::MakeConnexityBlocks(aCF, TopAbs_EDGE, TopAbs_FACE, aLCB);
        if (aLCB.Extent() > 1)
        {
          NCollection_List<TopoDS_Shape>::Iterator itLCB(aLCB);
          for (; itLCB.More(); itLCB.Next())
          {
            // Check if the block contains any valid faces
            const TopoDS_Shape& aCB = itLCB.Value();
            TopoDS_Iterator     itF(aCB);
            for (; itF.More(); itF.Next())
            {
              if (aValidFaces.Contains(itF.Value()))
                break;
            }
            if (!itF.More())
            {
              // Invalid block
              for (itF.Initialize(aCB); itF.More(); itF.Next())
                aFacesToAvoid.Add(itF.Value());
            }
          }
        }
      }

      itLF.Initialize(aLFTrimmed);
      for (; itLF.More();)
      {
        if (aFacesToAvoid.Contains(itLF.Value()))
          aLFTrimmed.Remove(itLF);
        else
          itLF.Next();
      }
    }
    else if (aLFTrimmed.IsEmpty())
    {
      // Use all splits, including those having the bounds of extended face
      anExpF.ReInit();
      for (; anExpF.More(); anExpF.Next())
        aLFTrimmed.Append(anExpF.Current());
    }

    if (aLFTrimmed.Extent())
    {
      // Remove the internal edges and vertices from the faces
      RemoveInternalWires(aLFTrimmed);

      myFaces.Add(theFOriginal, aLFTrimmed);
    }

    // Update history after intersection of the extended face with bounds
    myHistory->Merge(aGFTrim.History());

    // Update history with all removed shapes
    BRepTools_History aHistRem;

    // Map of the result splits
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>         aResMap;
    NCollection_List<TopoDS_Shape>::Iterator itLF(aLFTrimmed);
    for (; itLF.More(); itLF.Next())
      TopExp::MapShapes(itLF.Value(), aResMap);

    NCollection_List<TopoDS_Shape> aLSplits;
    aLSplits.Append(aSplits);

    // Update the history with removed shapes
    MakeRemoved(aLSplits, aHistRem, aResMap);
    myHistory->Merge(aHistRem);
  }

private: //! @name Fields
  // Inputs
  // clang-format off
  bool myRunParallel;                     //!< Defines the mode of processing of the single feature
  TopoDS_Shape myFeature;                             //!< Feature to remove
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>* myEFMap; //!< EF Connection map to find adjacent faces
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>* myFSMap; //!< FS Connection map to find solids participating in the feature removal
  Message_ProgressRange                      myRange; //!< Indication of progress

  // Results
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> myFeatureFacesMap;              //!< Faces of the feature
  bool myHasAdjacentFaces;                //!< Flag to show whether the adjacent faces have been found or not
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mySolids;                //!< Solids participating in the feature removal
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myFaces;  //!< Reconstructed adjacent faces
  occ::handle<BRepTools_History> myHistory;                //!< History of the adjacent faces reconstruction
  // clang-format on
};

typedef NCollection_Vector<FillGap> VectorOfFillGap;

//=======================================================================
// function: RemoveFeatures
// purpose: Remove features by filling the gaps by extension of the
//          adjacent faces
//=======================================================================
void BOPAlgo_RemoveFeatures::RemoveFeatures(const Message_ProgressRange& theRange)
{
  // For each feature:
  // - Find the faces adjacent to the feature;
  // - Extend the adjacent faces;
  // - Trim the extended faces to fill the gap;
  // - Rebuild the solids with reconstructed adjacent faces
  //   and avoiding the feature faces.

  Message_ProgressScope aPSOuter(theRange, "Removing features", 2);

  // Make Edge-Face connection map of the input
  // shape to find faces adjacent to the feature
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> anEFMap;
  TopExp::MapShapesAndAncestors(myInputShape, TopAbs_EDGE, TopAbs_FACE, anEFMap);

  // Make Face-Solid connection map to find the solids
  // participating in the removal of each feature
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> anFSMap;
  TopExp::MapShapesAndAncestors(myInputShape, TopAbs_FACE, TopAbs_SOLID, anFSMap);

  // Tool for reconstruction of the faces adjacent to the feature
  // in parallel threads if necessary.
  VectorOfFillGap aVFG;
  // Fill the vector
  NCollection_List<TopoDS_Shape>::Iterator itF(myFeatures);
  for (; itF.More(); itF.Next())
  {
    const TopoDS_Shape& aFeature = itF.Value();
    FillGap&            aFG      = aVFG.Appended();
    aFG.SetFeature(aFeature);
    aFG.SetEFConnectionMap(anEFMap);
    aFG.SetFSConnectionMap(anFSMap);
    aFG.SetRunParallel(myRunParallel);
  }

  const int aNbF = aVFG.Length();
  Message_ProgressScope  aPS(aPSOuter.Next(), "Filling gaps", aNbF);
  for (int i = 0; i < aNbF; ++i)
  {
    FillGap& aFG = aVFG.ChangeValue(i);
    aFG.SetRange(aPS.Next());
  }
  // Perform the reconstruction of the adjacent faces
  BOPTools_Parallel::Perform(myRunParallel, aVFG);
  if (UserBreak(aPSOuter))
  {
    return;
  }
  // Even if the history is not requested, it is necessary to track:
  // - The solids modification after each feature removal to find
  //   the necessary solids to rebuild on the next step.
  // - The faces modification after each feature removal to find the
  //   splits of the adjacent and feature faces for the next steps.
  if (myHistory.IsNull())
    myHistory = new BRepTools_History();

  // Remove the features one by one.
  // It will allow removing the features even if there were
  // some problems with removal of the previous features.
  Message_ProgressScope aPSLoop(aPSOuter.Next(), "Removing features one by one", aNbF);
  for (int i = 0; i < aNbF; ++i)
  {
    if (UserBreak(aPSLoop))
    {
      return;
    }
    FillGap& aFG = aVFG(i);

    // No need to fill the history for solids if the history is not
    // requested and the current feature is the last one.
    bool isSolidsHistoryNeeded = HasHistory() || (i < (aNbF - 1));

    // Perform removal of the single feature
    RemoveFeature(aFG.Feature(),
                  aFG.Solids(),
                  aFG.FeatureFacesMap(),
                  aFG.HasAdjacentFaces(),
                  aFG.Faces(),
                  aFG.History(),
                  isSolidsHistoryNeeded,
                  aPSLoop.Next());
  }
}

//=======================================================================
// function: RemoveFeature
// purpose: Remove the single feature
//=======================================================================
void BOPAlgo_RemoveFeatures::RemoveFeature(
  const TopoDS_Shape&                              theFeature,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                theSolids,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                       theFeatureFacesMap,
  const bool                           theHasAdjacentFaces,
  const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theAdjFaces,
  const occ::handle<BRepTools_History>&                 theAdjFacesHistory,
  const bool                           theSolidsHistoryNeeded,
  const Message_ProgressRange&                     theRange)
{
  bool       bFuseShapes = true;
  const int aNbAF       = theAdjFaces.Extent();
  if (aNbAF == 0)
  {
    if (theHasAdjacentFaces)
    {
      // The adjacent faces have been found for the feature,
      // but something went wrong during their rebuilding.
      // Add error
      AddWarning(new BOPAlgo_AlertUnableToRemoveTheFeature(theFeature));
      return;
    }

    // No adjacent faces have been found for the feature.
    // Most likely, the intention is to remove some internal part of the shape.
    // We just have to rebuild the shape without the feature, no fuse is necessary.
    bFuseShapes = false;
  }

  Message_ProgressScope aPS(theRange, NULL, 100);
  // Rebuild the shape using MakerVolume algorithm avoiding the faces of the
  // feature and replacing the adjacent faces with their images

  BRep_Builder aBB;

  // From the faces of input shape build map of faces with which the result will be validated:
  // - all non-internal faces of the input shape (except for adjacent and feature faces)
  //   must have some trace in the result solids;
  // - all adjacent faces (if any) must have some trace in the result solids.
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFacesToBeKept;
  // Build also the map of faces to compare orientation of the original and result faces
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aFacesToCheckOri;

  // Collect list of internal entities of the input shape to be avoided in result
  // and to make them removed in the history.
  NCollection_List<TopoDS_Shape> anInternalShapes;

  // In the feature removal will participate only the solids connected to the feature
  // or the faces adjacent to the feature.

  // Solids to be rebuilt  after the feature removal
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSolidsToRebuild;
  // Find faces shared between solids
  NCollection_List<TopoDS_Shape> aSharedFaces;
  // Solids to be avoided in the feature removal and added into result directly
  NCollection_List<TopoDS_Shape> anUnTouchedSolids;

  // Prepare to the feature removal - fill all necessary containers
  GetOriginalFaces(myShape,
                   theSolids,
                   theFeatureFacesMap,
                   theAdjFaces,
                   myHistory,
                   aFacesToBeKept,
                   anInternalShapes,
                   aFacesToCheckOri,
                   aSolidsToRebuild,
                   aSharedFaces,
                   anUnTouchedSolids);

  aPS.Next(3);
  // To avoid excessive intersection of the faces collect the faces
  // of the input shape into a compound
  TopoDS_Compound anOrigF;
  aBB.MakeCompound(anOrigF);
  int aNbFK = aFacesToBeKept.Extent();
  for (int i = 1; i <= aNbFK; ++i)
    aBB.Add(anOrigF, aFacesToBeKept(i));

  // Tool for solids reconstruction
  BOPAlgo_MakerVolume aMV;
  aMV.SetRunParallel(myRunParallel);
  aMV.SetAvoidInternalShapes(true);
  aMV.SetIntersect(bFuseShapes);
  aMV.SetNonDestructive(true);
  // Add faces of the input shape
  aMV.AddArgument(anOrigF);

  // Add reconstructed adjacent faces
  for (int i = 1; i <= aNbAF; ++i)
  {
    const NCollection_List<TopoDS_Shape>& aLFA = theAdjFaces(i);
    if (aLFA.Extent() == 1)
    {
      const TopoDS_Shape& aFA = aLFA.First();
      aMV.AddArgument(aFA);
      aFacesToBeKept.Add(aFA);
    }
    else
    {
      // To avoid intersection among the images, collect them into compound
      TopoDS_Compound anAdjF;
      aBB.MakeCompound(anAdjF);
      NCollection_List<TopoDS_Shape>::Iterator itLFA(aLFA);
      for (; itLFA.More(); itLFA.Next())
        aBB.Add(anAdjF, itLFA.Value());

      aMV.AddArgument(anAdjF);
      aFacesToBeKept.Add(anAdjF);
    }

    if (HasHistory())
    {
      // Look for internal edges in the original adjacent faces
      const TopoDS_Shape& aFOr = theAdjFaces.FindKey(i);
      FindInternals(aFOr, anInternalShapes);
    }
  }

  // Build solids
  aMV.Perform(aPS.Next(90));
  if (aMV.HasErrors())
  {
    // Add warning for the feature
    AddWarning(new BOPAlgo_AlertUnableToRemoveTheFeature(theFeature));
    return;
  }

  // Result of MV operation
  const TopoDS_Shape& aSolids = aMV.Shape();
  TopExp_Explorer     anExpS(aSolids, TopAbs_SOLID);
  if (!anExpS.More())
  {
    // No solids have been built - add warning for the feature
    AddWarning(new BOPAlgo_AlertUnableToRemoveTheFeature(theFeature));
    return;
  }

  // Now, it is necessary to:
  // - Validate the result by checking faces of the map <aFacesToBeKept>
  //   to have some parts kept in the resulting solids;
  // - Remove the solids possibly filling the holes in the original shapes;
  // - Update history with the history of MakerVolume algorithm.

  // Splits of adjacent faces from previous runs
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anAdjFacesSplits;
  for (int i = 1; i <= aNbAF; ++i)
  {
    if (!aPS.More())
    {
      return;
    }
    const TopoDS_Shape&         aF    = theAdjFaces.FindKey(i);
    const NCollection_List<TopoDS_Shape>& aLFIm = myHistory->Modified(aF);
    if (aLFIm.IsEmpty())
      anAdjFacesSplits.Add(aF);
    else
    {
      NCollection_List<TopoDS_Shape>::Iterator itLFIm(aLFIm);
      for (; itLFIm.More(); itLFIm.Next())
        anAdjFacesSplits.Add(itLFIm.Value());
    }
  }

  // Validate the result
  bool bValid = true;
  aNbFK                   = aFacesToBeKept.Extent();
  for (int i = 1; i <= aNbFK && bValid; ++i)
  {

    const TopoDS_Shape& aS = aFacesToBeKept(i);
    if (anAdjFacesSplits.Contains(aS))
      continue;
    if (!aPS.More())
    {
      return;
    }
    TopExp_Explorer anExpF(aS, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
    {
      const TopoDS_Shape& aF = anExpF.Current();
      if (!aMV.IsDeleted(aF))
        break;
    }
    bValid = anExpF.More();
  }

  if (!bValid)
  {
    // Add warning for the feature
    AddWarning(new BOPAlgo_AlertUnableToRemoveTheFeature(theFeature));
    return;
  }

  // It is possible that the result of MakerVolume operation contains
  // some extra solids. Get only correct solids.
  NCollection_List<TopoDS_Shape> aLSRes;
  // Remember the removed shapes
  NCollection_List<TopoDS_Shape> aRemovedShapes;
  GetValidSolids(aMV,
                 aFacesToCheckOri,
                 aSharedFaces,
                 anOrigF,
                 theSolids.Extent(),
                 aLSRes,
                 aRemovedShapes);

  if (aLSRes.Extent() != theSolids.Extent())
  {
    // Add warning for the feature
    AddWarning(new BOPAlgo_AlertUnableToRemoveTheFeature(theFeature));
    return;
  }
  aPS.Next(3);
  // Remove internal wires from the faces, possibly appeared after intersection
  RemoveInternalWires(aLSRes, &anInternalShapes);

  // Update history with:
  // History of adjacent faces reconstruction
  myHistory->Merge(theAdjFacesHistory);
  // History of intersection
  myHistory->Merge(aMV.History());

  if (HasHistory())
  {
    // Map the result to check if the shape is removed
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>         aMSRes;
    NCollection_List<TopoDS_Shape>::Iterator itLS(aLSRes);
    for (; itLS.More(); itLS.Next())
      TopExp::MapShapes(itLS.Value(), aMSRes);

    // Remove internal shapes and extra faces
    BRepTools_History aRemHist;
    anInternalShapes.Append(aRemovedShapes);
    MakeRemoved(anInternalShapes, aRemHist, aMSRes);
    myHistory->Merge(aRemHist);
  }
  aPS.Next(3);
  // Fill the history for the solids
  if (theSolidsHistoryNeeded)
  {
    BRepTools_History aSolidsHistory;
    FillSolidsHistory(aSolidsToRebuild, aLSRes, theAdjFaces, aMV, aSolidsHistory);
    myHistory->Merge(aSolidsHistory);
  }

  TopoDS_Compound aCRes;
  aBB.MakeCompound(aCRes);
  // Add reconstructed solids
  NCollection_List<TopoDS_Shape>::Iterator itLS(aLSRes);
  for (; itLS.More(); itLS.Next())
    aBB.Add(aCRes, itLS.Value());

  // Add unmodified solids
  itLS.Initialize(anUnTouchedSolids);
  for (; itLS.More(); itLS.Next())
    aBB.Add(aCRes, itLS.Value());

  // Save the result
  myShape = aCRes;
}

//=======================================================================
// function: UpdateHistory
// purpose: Update history with the removed features
//=======================================================================
void BOPAlgo_RemoveFeatures::UpdateHistory(const Message_ProgressRange& theRange)
{
  if (!HasHistory())
    return;

  // Map the result
  myMapShape.Clear();
  TopExp::MapShapes(myShape, myMapShape);

  // Update the history
  BRepTools_History aHistory;

  const int aNbS = myInputsMap.Extent();
  Message_ProgressScope  aPS(theRange, "Updating history", aNbS);
  for (int i = 1; i <= aNbS; ++i, aPS.Next())
  {
    const TopoDS_Shape& aS = myInputsMap(i);
    if (!BRepTools_History::IsSupportedType(aS))
      continue;

    if (myHistory->IsRemoved(aS))
      continue;

    if (UserBreak(aPS))
    {
      return;
    }
    // Check if the shape has any trace in the result
    const NCollection_List<TopoDS_Shape>& aLSIm = myHistory->Modified(aS);
    if (aLSIm.IsEmpty())
    {
      if (!myMapShape.Contains(aS))
        aHistory.Remove(aS);
    }

    NCollection_List<TopoDS_Shape>::Iterator itLSIm(aLSIm);
    for (; itLSIm.More(); itLSIm.Next())
    {
      if (!myMapShape.Contains(itLSIm.Value()))
        aHistory.Remove(itLSIm.Value());
    }
  }

  myHistory->Merge(aHistory);
}

//=======================================================================
// function: SimplifyResult
// purpose: Simplifies the result by removing extra edges and vertices
//          created during operation
//=======================================================================
void BOPAlgo_RemoveFeatures::SimplifyResult(const Message_ProgressRange& theRange)
{
  if (myShape.IsSame(myInputShape))
    return;
  Message_ProgressScope        aPSOuter(theRange, "Simplifyingthe result", 2);
  ShapeUpgrade_UnifySameDomain aSDTool;
  aSDTool.Initialize(myShape, true, true);
  // Do not allow producing internal edges
  aSDTool.AllowInternalEdges(false);
  // Avoid removal of the input edges and vertices
  if (myMapShape.IsEmpty())
    TopExp::MapShapes(myShape, myMapShape);

  const int aNbS = myInputsMap.Extent();
  Message_ProgressScope  aPS(aPSOuter.Next(), NULL, aNbS);
  for (int i = 1; i <= aNbS; ++i, aPS.Next())
  {
    if (UserBreak(aPS))
    {
      return;
    }
    if (myMapShape.Contains(myInputsMap(i)))
      aSDTool.KeepShape(myInputsMap(i));
  }

  // Perform unification
  aSDTool.Build();
  aPSOuter.Next();
  myShape = aSDTool.Shape();
  if (HasHistory())
    myHistory->Merge(aSDTool.History());
}

//=======================================================================
// function: PostTreat
// purpose: Restore the type of the initial shape
//=======================================================================
void BOPAlgo_RemoveFeatures::PostTreat()
{
  const TopAbs_ShapeEnum anInputType = myInputShape.ShapeType();
  const TopAbs_ShapeEnum aResType    = myShape.ShapeType();
  if (aResType == anInputType)
    return;

  TopExp_Explorer anExpS(myShape, TopAbs_SOLID);

  if (anInputType == TopAbs_SOLID)
  {
    myShape = anExpS.Current();
    return;
  }

  TopoDS_Shape aRes;
  if (anInputType == TopAbs_COMPOUND)
    BRep_Builder().MakeCompound(TopoDS::Compound(aRes));
  else
    BRep_Builder().MakeCompSolid(TopoDS::CompSolid(aRes));

  for (; anExpS.More(); anExpS.Next())
    BRep_Builder().Add(aRes, anExpS.Current());

  myShape = aRes;
}

//=======================================================================
// static methods definition
//=======================================================================

//=======================================================================
// function: MakeRemoved
// purpose: Makes the shapes in the list removed in the history.
//          Keeps the shapes contained in the map.
//=======================================================================
void MakeRemoved(const NCollection_List<TopoDS_Shape>&       theShapes,
                 BRepTools_History&                theHistory,
                 const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theKeepShapes)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>         aShapesMap;
  NCollection_List<TopoDS_Shape>::Iterator it(theShapes);
  for (; it.More(); it.Next())
    TopExp::MapShapes(it.Value(), aShapesMap);

  const int aNbS = aShapesMap.Extent();
  for (int i = 1; i <= aNbS; ++i)
  {
    const TopoDS_Shape& aS = aShapesMap(i);
    if (!theKeepShapes.Contains(aS) && BRepTools_History::IsSupportedType(aS))
    {
      theHistory.Remove(aS);
    }
  }
}

//=======================================================================
// function: FindInternals
// purpose: Looks for internal shapes inside the face or solid
//=======================================================================
void FindInternals(const TopoDS_Shape& theS, NCollection_List<TopoDS_Shape>& theLInt)
{
  TopoDS_Iterator itS(theS);
  for (; itS.More(); itS.Next())
  {
    const TopoDS_Shape& aSS = itS.Value();
    if (aSS.Orientation() == TopAbs_INTERNAL)
      theLInt.Append(aSS);
    else
    {
      TopoDS_Iterator itSS(aSS);
      for (; itSS.More(); itSS.Next())
      {
        if (itSS.Value().Orientation() == TopAbs_INTERNAL)
        {
          theLInt.Append(aSS);
          break;
        }
      }
    }
  }
}

//=======================================================================
// function: RemoveInternalWires
// purpose: Removes internal wires from the faces
//=======================================================================
void RemoveInternalWires(const NCollection_List<TopoDS_Shape>& theShapes, NCollection_List<TopoDS_Shape>* theRemoved)
{
  NCollection_List<TopoDS_Shape>::Iterator itLS(theShapes);
  for (; itLS.More(); itLS.Next())
  {
    const TopoDS_Shape& aShape = itLS.Value();
    TopExp_Explorer     anExpF(aShape, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
    {
      TopoDS_Face&         aF = *(TopoDS_Face*)&anExpF.Current();
      NCollection_List<TopoDS_Shape> aLWToRemove;
      FindInternals(aF, aLWToRemove);
      if (aLWToRemove.Extent())
      {
        aF.Free(true);
        NCollection_List<TopoDS_Shape>::Iterator itR(aLWToRemove);
        for (; itR.More(); itR.Next())
        {
          if (theRemoved)
            theRemoved->Append(itR.Value());
          BRep_Builder().Remove(aF, itR.Value());
        }
        aF.Free(false);
      }
    }
  }
}

//=======================================================================
// function: GetOriginalFaces
// purpose: Get original faces from my face
//=======================================================================
void GetOriginalFaces(const TopoDS_Shape&                              theShape,
                      const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                theSolids,
                      const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                       theFeatureFacesMap,
                      const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theAdjFaces,
                      const occ::handle<BRepTools_History>&                 theHistory,
                      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                      theFacesToBeKept,
                      NCollection_List<TopoDS_Shape>&                            theInternalShapes,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                             theFacesToCheckOri,
                      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                      theSolidsToRebuild,
                      NCollection_List<TopoDS_Shape>&                            theSharedFaces,
                      NCollection_List<TopoDS_Shape>&                            theUnTouchedSolids)
{
  // Use only solids which has to be reconstructed by the feature.
  // All other solids should be avoided in the feature removal and added
  // into result directly.

  // Update solids after removal of previous features
  const int aNbSols = theSolids.Extent();
  for (int i = 1; i <= aNbSols; ++i)
  {
    const TopoDS_Shape&         aSol  = theSolids(i);
    const NCollection_List<TopoDS_Shape>& aLFIm = theHistory->Modified(aSol);
    if (aLFIm.IsEmpty())
      theSolidsToRebuild.Add(aSol);
    else
      theSolidsToRebuild.Add(aLFIm.First());
  }

  // Splits of the feature faces
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>              aFeatureFacesSplits;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itM(theFeatureFacesMap);
  for (; itM.More(); itM.Next())
  {
    const TopoDS_Shape&         aF    = itM.Value();
    const NCollection_List<TopoDS_Shape>& aLFIm = theHistory->Modified(aF);
    if (aLFIm.IsEmpty())
      aFeatureFacesSplits.Add(aF);
    else
    {
      NCollection_List<TopoDS_Shape>::Iterator itLFIm(aLFIm);
      for (; itLFIm.More(); itLFIm.Next())
        aFeatureFacesSplits.Add(itLFIm.Value());
    }
  }

  TopExp_Explorer anExpS(theShape, TopAbs_SOLID);
  for (; anExpS.More(); anExpS.Next())
  {
    const TopoDS_Shape& aSol = anExpS.Current();

    // Check if the solid has to be reconstructed
    if (!theSolidsToRebuild.Contains(aSol))
    {
      // untouched solid
      theUnTouchedSolids.Append(aSol);
      continue;
    }

    TopoDS_Iterator itSh(aSol);
    for (; itSh.More(); itSh.Next())
    {
      const TopoDS_Shape& aSh = itSh.Value();
      if (aSh.ShapeType() != TopAbs_SHELL)
      {
        theInternalShapes.Append(aSh);
        continue;
      }

      TopoDS_Iterator itF(aSh);
      for (; itF.More(); itF.Next())
      {
        const TopoDS_Shape& aF = itF.Value();
        // Avoid the feature faces
        if (aFeatureFacesSplits.Contains(aF))
          continue;

        // Avoid the adjacent faces
        if (theAdjFaces.Contains(aF))
          continue;

        if (aF.Orientation() != TopAbs_INTERNAL)
        {
          theFacesToBeKept.Add(aF);

          if (!theFacesToCheckOri.Add(aF))
          {
            theFacesToCheckOri.Remove(aF);
            theSharedFaces.Append(aF);
          }
        }
        else
          theInternalShapes.Append(aSh);
      }
    }
  }
}

//=======================================================================
// function: FindShape
// purpose: Find the shape in the other shape
//=======================================================================
void FindShape(const TopoDS_Shape& theSWhat, const TopoDS_Shape& theSWhere, TopoDS_Shape& theSFound)
{
  TopExp_Explorer anExp(theSWhere, theSWhat.ShapeType());
  for (; anExp.More(); anExp.Next())
  {
    const TopoDS_Shape& aS = anExp.Current();
    if (aS.IsSame(theSWhat))
    {
      theSFound = aS;
      break;
    }
  }
}

//=======================================================================
// function: GetValidSolids
// purpose: Checks the validity of the solids and keeps only valid ones
//=======================================================================
void GetValidSolids(BOPAlgo_MakerVolume&        theMV,
                    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theFacesToCheckOri,
                    const NCollection_List<TopoDS_Shape>& aSharedFaces,
                    const TopoDS_Shape&         theOrigFaces,
                    const int      theNbSol,
                    NCollection_List<TopoDS_Shape>&       theLSRes,
                    NCollection_List<TopoDS_Shape>&       theRemovedShapes)
{
  TopExp_Explorer anExpS(theMV.Shape(), TopAbs_SOLID);
  for (; anExpS.More(); anExpS.Next())
    theLSRes.Append(anExpS.Current());

  if (theLSRes.Extent() > theNbSol)
  {
    // Find Solids filling the holes in the initial shape
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                       aSolidsToAvoid;
    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aFSMap;
    TopExp::MapShapesAndAncestors(theMV.Shape(), TopAbs_FACE, TopAbs_SOLID, aFSMap);
    FindExtraShapes(aFSMap, theFacesToCheckOri, theMV, aSolidsToAvoid);

    NCollection_List<TopoDS_Shape>::Iterator itLS(theLSRes);
    for (; itLS.More();)
    {
      if (aSolidsToAvoid.Contains(itLS.Value()))
        theLSRes.Remove(itLS);
      else
        itLS.Next();
    }
  }

  if (theLSRes.Extent() > theNbSol)
  {
    // Check if the splits of the adjacent faces split the solids
    AvoidExtraSharedFaces(theLSRes, aSharedFaces, theMV, theRemovedShapes);
  }

  if (theLSRes.Extent() > theNbSol)
  {
    // Remove solids containing only the adjacent faces
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anOrigFacesRes;
    TopExp_Explorer     anExpF(theOrigFaces, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
      TakeModified(anExpF.Current(), theMV, anOrigFacesRes);

    NCollection_List<TopoDS_Shape>::Iterator itLS(theLSRes);
    for (; itLS.More();)
    {
      anExpF.Init(itLS.Value(), TopAbs_FACE);
      for (; anExpF.More(); anExpF.Next())
      {
        if (anOrigFacesRes.Contains(anExpF.Current()))
          break;
      }
      if (!anExpF.More())
      {
        theRemovedShapes.Append(itLS.Value());
        theLSRes.Remove(itLS);
      }
      else
        itLS.Next();
    }
  }
}

//=======================================================================
// function: FindExtraShape
// purpose: Find shapes possibly filling the holes in the original shape
//=======================================================================
void FindExtraShapes(const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theConnectionMap,
                     const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                       theShapesToCheckOri,
                     BOPAlgo_Builder&                                 theBuilder,
                     NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                             theShapesToAvoid,
                     NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>*                             theValidShapes)
{
  occ::handle<IntTools_Context>         aCtx = theBuilder.Context();
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>              aValidShapes;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>*             pValidShapes = theValidShapes ? theValidShapes : &aValidShapes;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itM(theShapesToCheckOri);
  for (; itM.More(); itM.Next())
  {
    const TopoDS_Shape& aSToCheckOri = itM.Value();
    // Check modification of the shape during intersection
    NCollection_List<TopoDS_Shape> aLSIm;
    TakeModified(aSToCheckOri, theBuilder, aLSIm);

    NCollection_List<TopoDS_Shape>::Iterator itLSIm(aLSIm);
    for (; itLSIm.More(); itLSIm.Next())
    {
      const TopoDS_Shape& aSIm = itLSIm.Value();

      const NCollection_List<TopoDS_Shape>* pShapesToValidate = theConnectionMap.Seek(aSIm);
      if (!pShapesToValidate)
        continue;

      NCollection_List<TopoDS_Shape>::Iterator itSV(*pShapesToValidate);
      for (; itSV.More(); itSV.Next())
      {
        const TopoDS_Shape& aShapeToValidate = itSV.Value();
        if (pValidShapes->Contains(aShapeToValidate))
          continue;

        TopoDS_Face aSInShape;
        FindShape(aSIm, aShapeToValidate, aSInShape);

        bool bSameOri =
          !BOPTools_AlgoTools::IsSplitToReverse(aSInShape, aSToCheckOri, aCtx);

        if (bSameOri)
          pValidShapes->Add(aShapeToValidate);
        else
          theShapesToAvoid.Add(aShapeToValidate);
      }
    }
  }

  itM.Initialize(*pValidShapes);
  for (; itM.More(); itM.Next())
    theShapesToAvoid.Remove(itM.Value());
}

//=======================================================================
// function: AvoidExtraSharedFaces
// purpose: Looks for the extra faces splitting the solids
//=======================================================================
void AvoidExtraSharedFaces(NCollection_List<TopoDS_Shape>&       theLSolids,
                           const NCollection_List<TopoDS_Shape>& theLFSharedToAvoid,
                           BOPAlgo_Builder&            theBuilder,
                           NCollection_List<TopoDS_Shape>&       theExtraFaces)
{
  // Get all splits of shared faces to avoid in the check
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFSharedSp;
  {
    NCollection_List<TopoDS_Shape>               aLFSharedSp;
    NCollection_List<TopoDS_Shape>::Iterator itLFS(theLFSharedToAvoid);
    for (; itLFS.More(); itLFS.Next())
      TakeModified(itLFS.Value(), theBuilder, aMFSharedSp);
  }

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aFSMap;
  NCollection_List<TopoDS_Shape>::Iterator        itLS(theLSolids);
  for (; itLS.More(); itLS.Next())
    TopExp::MapShapesAndAncestors(itLS.Value(), TopAbs_FACE, TopAbs_SOLID, aFSMap);

  NCollection_List<TopoDS_Shape> anExtraFaces;
  NCollection_List<TopoDS_Shape> aLFArguments;
  itLS.Initialize(theLSolids);
  for (; itLS.More(); itLS.Next())
  {
    const TopoDS_Shape& aSol = itLS.Value();
    TopExp_Explorer     anExpF(aSol, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
    {
      const TopoDS_Shape&         aF    = anExpF.Current();
      const NCollection_List<TopoDS_Shape>& aLSol = aFSMap.FindFromKey(aF);
      if (aLSol.Extent() != 2 || aMFSharedSp.Contains(aF))
        aLFArguments.Append(aF);
      else
        anExtraFaces.Append(aF);
    }
  }

  if (anExtraFaces.IsEmpty())
    return;

  // Rebuild the solids avoiding the extra faces
  BOPAlgo_BuilderSolid aBS;
  aBS.SetAvoidInternalShapes(true);
  aBS.SetShapes(aLFArguments);
  aBS.Perform();
  if (aBS.HasErrors())
    return;

  theLSolids = aBS.Areas();
  theExtraFaces.Append(anExtraFaces);
}

//=======================================================================
// function: FillSolidsHistory
// purpose: Fills the history of solids modifications
//=======================================================================
void FillSolidsHistory(const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                theSolIn,
                       NCollection_List<TopoDS_Shape>&                            theSolidsOut,
                       const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theAdjFaces,
                       BOPAlgo_Builder&                                 theBuilder,
                       BRepTools_History&                               theSolidsHistory)
{
  const int aNbS = theSolIn.Extent();
  for (int i = 1; i <= aNbS; ++i)
  {
    const TopoDS_Shape& aSolIn = theSolIn(i);

    TopoDS_Shape aSolOut;
    FindSolid(aSolIn, theSolidsOut, theAdjFaces, theBuilder, aSolOut);

    if (aSolOut.IsNull())
    {
      theSolidsHistory.Remove(aSolIn);
      continue;
    }

    // Check if the solid has really been modified
    BOPTools_Set aSTIn, aSTOut;
    aSTIn.Add(aSolIn, TopAbs_FACE);
    aSTOut.Add(aSolOut, TopAbs_FACE);
    if (aSTIn.IsEqual(aSTOut))
    {
      // The solids are the same. Replace the resulting solid in the result list
      // with the initial solid.
      NCollection_List<TopoDS_Shape>::Iterator itLS(theSolidsOut);
      for (; itLS.More(); itLS.Next())
      {
        if (itLS.Value().IsSame(aSolOut))
        {
          theSolidsOut.InsertBefore(aSolIn, itLS);
          theSolidsOut.Remove(itLS);
          break;
        }
      }
    }
    else
    {
      theSolidsHistory.AddModified(aSolIn, aSolOut);
    }
  }
}

//=======================================================================
// function: TakeModified
// purpose: Stores the modified object into the list
//=======================================================================
void TakeModified(const TopoDS_Shape&   theS,
                  BOPAlgo_Builder&      theBuilder,
                  NCollection_List<TopoDS_Shape>& theList)
{
  const NCollection_List<TopoDS_Shape>& aModified = theBuilder.Modified(theS);
  if (aModified.IsEmpty() && !theBuilder.IsDeleted(theS))
    theList.Append(theS);
  else
  {
    NCollection_List<TopoDS_Shape>::Iterator itM(aModified);
    for (; itM.More(); itM.Next())
      theList.Append(itM.Value());
  }
}

//=======================================================================
// function: TakeModified
// purpose: Stores the modified object into the map
//=======================================================================
void TakeModified(const TopoDS_Shape&  theS,
                  BOPAlgo_Builder&     theBuilder,
                  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap)
{
  const NCollection_List<TopoDS_Shape>& aModified = theBuilder.Modified(theS);
  if (aModified.IsEmpty() && !theBuilder.IsDeleted(theS))
    theMap.Add(theS);
  else
  {
    NCollection_List<TopoDS_Shape>::Iterator itM(aModified);
    for (; itM.More(); itM.Next())
      theMap.Add(itM.Value());
  }
}

//=======================================================================
// function: FindSolid
// purpose: Looks for the image of the solid in the list of resulting solids
//=======================================================================
void FindSolid(const TopoDS_Shape&                              theSolIn,
               const NCollection_List<TopoDS_Shape>&                      theSolidsRes,
               const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theAdjFaces,
               BOPAlgo_Builder&                                 theBuilder,
               TopoDS_Shape&                                    theSolOut)
{
  occ::handle<IntTools_Context> aCtx = theBuilder.Context();

  // Take the face in the IN solid, and find it in the OUT list
  TopExp_Explorer anExpF(theSolIn, TopAbs_FACE);
  for (; anExpF.More(); anExpF.Next())
  {
    const TopoDS_Shape& aFS = anExpF.Current();
    // Images of the face
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>         aMFSIm;
    const NCollection_List<TopoDS_Shape>* pLFA = theAdjFaces.Seek(aFS);
    if (pLFA)
    {
      NCollection_List<TopoDS_Shape>::Iterator itLFA(*pLFA);
      for (; itLFA.More(); itLFA.Next())
        TakeModified(itLFA.Value(), theBuilder, aMFSIm);
    }
    else
    {
      TakeModified(aFS, theBuilder, aMFSIm);
    }

    // Find any of these faces in the list of solids
    NCollection_List<TopoDS_Shape>::Iterator itLS(theSolidsRes);
    for (; itLS.More(); itLS.Next())
    {
      const TopoDS_Shape& aSol = itLS.Value();
      TopExp_Explorer     anExpFOut(aSol, TopAbs_FACE);
      for (; anExpFOut.More(); anExpFOut.Next())
      {
        const TopoDS_Shape& aF = anExpFOut.Current();
        if (aMFSIm.Contains(aF))
        {
          // check orientations
          if (!BOPTools_AlgoTools::IsSplitToReverse(aF, aFS, aCtx))
          {
            theSolOut = aSol;
            return;
          }
        }
      }
    }
  }
}
