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

#include <TopTools_IndexedDataMapOfShapeShape.hxx>


//=======================================================================
// static methods declaration
//=======================================================================

static void MakeRemoved(const TopTools_ListOfShape& theShapes,
                        BRepTools_History& theHistory,
                        const TopTools_IndexedMapOfShape& theKeepShapes = TopTools_IndexedMapOfShape());

static void FindInternals(const TopoDS_Shape& theS,
                          TopTools_ListOfShape& theLInt);

static void RemoveInternalWires(const TopTools_ListOfShape& theShapes,
                                TopTools_ListOfShape* theRemoved = NULL);

static void GetOriginalFaces(const TopoDS_Shape& theShape,
                             const TopTools_IndexedMapOfShape& theSolids,
                             const TopTools_MapOfShape& theFeatureFacesMap,
                             const TopTools_IndexedDataMapOfShapeListOfShape& theAdjFaces,
                             const Handle(BRepTools_History)& theHistory,
                             TopTools_IndexedMapOfShape& theFacesToBeKept,
                             TopTools_ListOfShape& theInternalShapes,
                             TopTools_MapOfShape& theFacesToCheckOri,
                             TopTools_IndexedMapOfShape& theSolidsToRebuild,
                             TopTools_ListOfShape& theSharedFaces,
                             TopTools_ListOfShape& theUnTouchedSolids);

static void FindShape(const TopoDS_Shape& theSWhat,
                      const TopoDS_Shape& theSWhere,
                      TopoDS_Shape& theSFound);

static void GetValidSolids(BOPAlgo_MakerVolume& theMV,
                           const TopTools_MapOfShape& theFacesToCheckOri,
                           const TopTools_ListOfShape& aSharedFaces,
                           const TopoDS_Shape& theOrigFaces,
                           const Standard_Integer theNbSol,
                           TopTools_ListOfShape& theLSRes,
                           TopTools_ListOfShape& theRemovedShapes);

static void FindExtraShapes(const TopTools_IndexedDataMapOfShapeListOfShape& theConnectionMap,
                            const TopTools_MapOfShape& theShapesToCheckOri,
                            BOPAlgo_Builder& theBuilder,
                            TopTools_MapOfShape& theShapesToAvoid,
                            TopTools_MapOfShape* theValidShapes = NULL);

static void AvoidExtraSharedFaces(TopTools_ListOfShape& theLSolids,
                                  const TopTools_ListOfShape& theLFSharedToAvoid,
                                  BOPAlgo_Builder& theBuilder,
                                  TopTools_ListOfShape& theExtraFaces);

static void FillSolidsHistory(const TopTools_IndexedMapOfShape& theSolIn,
                              TopTools_ListOfShape& theSolidsRes,
                              const TopTools_IndexedDataMapOfShapeListOfShape& theAdjFaces,
                              BOPAlgo_Builder& theBuilder,
                              BRepTools_History& theSolidsHistory);

static void TakeModified(const TopoDS_Shape& theS,
                         BOPAlgo_Builder& theBuilder,
                         TopTools_ListOfShape& theList);

static void TakeModified(const TopoDS_Shape& theS,
                         BOPAlgo_Builder& theBuilder,
                         TopTools_MapOfShape& theMap);

static void FindSolid(const TopoDS_Shape& theSolIn,
                      const TopTools_ListOfShape& theSolidsRes,
                      const TopTools_IndexedDataMapOfShapeListOfShape& theAdjFaces,
                      BOPAlgo_Builder& theBuilder,
                      TopoDS_Shape& theSolOut);


//=======================================================================
// function: Perform
// purpose: Performs the removal of the requested faces from the input shape
//=======================================================================
void BOPAlgo_RemoveFeatures::Perform()
{
  try
  {
    OCC_CATCH_SIGNALS

    if (myTrackHistory)
      myHistory = new BRepTools_History();

    // Check the input data
    CheckData();
    if (HasErrors())
      return;

    // Prepare the faces to remove.
    PrepareFeatures();
    if (HasErrors())
      return;

    // Remove the features and fill the created gaps
    RemoveFeatures();

    // Update history with the removed features
    UpdateHistory();

    // Simplify the result
    SimplifyResult();

    // Post treatment
    PostTreat();
  }
  catch (Standard_Failure)
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
    TopTools_ListOfShape aShapes;
    TopTools_MapOfShape aMFence;
    // Extract all shapes from the compound
    BOPAlgo_Tools::TreatCompound(myInputShape, aMFence, aShapes);
    if (aShapes.IsEmpty())
    {
      // Add error of empty input shape
      AddError(new BOPAlgo_AlertEmptyShape(myInputShape));
      return;
    }

    // Find all solids in the list of shapes
    TopTools_ListOfShape aSolids;
    TopTools_ListOfShape anOtherShapes;
    TopTools_ListIteratorOfListOfShape aIt(aShapes);
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

      if (myTrackHistory)
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
void BOPAlgo_RemoveFeatures::PrepareFeatures()
{
  // Map all sub-shapes of the input solids
  TopExp::MapShapes(myInputShape, myInputsMap);

  // Collect all faces of the input shape requested for removal
  TopTools_ListOfShape aFacesToRemove;
  TopTools_ListIteratorOfListOfShape aIt(myFacesToRemove);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();
    TopExp_Explorer anExpF(aS, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
    {
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
  FillGap() :
    myRunParallel(Standard_False),
    myHasAdjacentFaces(Standard_False)
  {}

public: //! @name Setters/Getters

  //! Sets the feature to remove
  void SetFeature(const TopoDS_Shape& theFeature) { myFeature = theFeature; }

  //! Returns the feature
  const TopoDS_Shape& Feature() const { return myFeature; }

  //! Sets the EF connection map
  void SetEFConnectionMap(const TopTools_IndexedDataMapOfShapeListOfShape& theEFMap)
  {
    myEFMap = (TopTools_IndexedDataMapOfShapeListOfShape*)&theEFMap;
  }

  //! Sets the FS connection map
  void SetFSConnectionMap(const TopTools_IndexedDataMapOfShapeListOfShape& theFSMap)
  {
    myFSMap = (TopTools_IndexedDataMapOfShapeListOfShape*)&theFSMap;
  }

  //! Defines the parallel processing mode
  void SetRunParallel(const Standard_Boolean bRunParallel) { myRunParallel = bRunParallel; }

  //! Gets the History object
  const Handle(BRepTools_History)& History()
  {
    return myHistory;
  }

public: //! @name Perform the operation

  //! Performs the extension of the adjacent faces and
  //! then trims the extended faces to fill the gaps
  void Perform()
  {
    OCC_CATCH_SIGNALS

    try
    {
      myHistory = new BRepTools_History();

      // Find the faces adjacent to the faces of the feature
      TopTools_IndexedMapOfShape aMFAdjacent;
      FindAdjacentFaces(aMFAdjacent);

      myHasAdjacentFaces = (aMFAdjacent.Extent() > 0);
      if (!myHasAdjacentFaces)
        return;

      // Extend the adjacent faces keeping the connection to the original faces
      TopTools_IndexedDataMapOfShapeShape aFaceExtFaceMap;
      ExtendAdjacentFaces(aMFAdjacent, aFaceExtFaceMap);

      // Trim the extended faces
      TrimExtendedFaces(aFaceExtFaceMap);
    }
    catch (Standard_Failure)
    {
      // Make sure the warning will be given on the higher level
      myHasAdjacentFaces = Standard_True;
      myFaces.Clear();
    }
  }

public: //! @name Obtain the result

  //! Returns the map of faces of the feature
  const TopTools_MapOfShape& FeatureFacesMap() const { return myFeatureFacesMap; }

  //! Shows whether the adjacent faces have been found for the feature
  Standard_Boolean HasAdjacentFaces() const { return myHasAdjacentFaces; }

  //! Returns the Images map of the adjacent faces
  const TopTools_IndexedDataMapOfShapeListOfShape& Faces() const { return myFaces; }

  //! Returns the initial solids participating in the feature removal
  const TopTools_IndexedMapOfShape& Solids() const { return mySolids; }


private: //! @name Private methods performing the operation

  //! Finds the faces adjacent to the feature and stores them into outgoing map.
  void FindAdjacentFaces(TopTools_IndexedMapOfShape& theMFAdjacent)
  {
    // Map the faces of the feature to avoid them in the map of adjacent faces
    TopoDS_Iterator aIt(myFeature);
    for (; aIt.More(); aIt.Next())
      myFeatureFacesMap.Add(aIt.Value());

    // Find faces adjacent to the feature using the connection map
    aIt.Initialize(myFeature);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aF = aIt.Value();
      TopExp_Explorer anExpE(aF, TopAbs_EDGE);
      for (; anExpE.More(); anExpE.Next())
      {
        const TopoDS_Shape& aE = anExpE.Current();
        const TopTools_ListOfShape* pAdjacentFaces = myEFMap->Seek(aE);
        if (pAdjacentFaces)
        {
          TopTools_ListIteratorOfListOfShape itLFA(*pAdjacentFaces);
          for (; itLFA.More(); itLFA.Next())
          {
            const TopoDS_Shape& anAF = itLFA.Value();
            if (!myFeatureFacesMap.Contains(anAF))
              theMFAdjacent.Add(anAF);
          }
        }
      }
      // Find solids containing the feature face
      const TopTools_ListOfShape* pLS = myFSMap->Seek(aF);
      if (pLS)
      {
        TopTools_ListIteratorOfListOfShape itLS(*pLS);
        for (; itLS.More(); itLS.Next())
          mySolids.Add(itLS.Value());
      }
    }

    // Find solids containing the edges of adjacent faces
    const Standard_Integer aNbFA = theMFAdjacent.Extent();
    for (Standard_Integer i = 1; i <= aNbFA; ++i)
    {
      TopExp_Explorer anExpEA(theMFAdjacent(i), TopAbs_EDGE);
      for (; anExpEA.More(); anExpEA.Next())
      {
        // Faces adjacent to the face adjacent to the feature
        const TopTools_ListOfShape* pLFAA = myEFMap->Seek(anExpEA.Current());
        if (pLFAA)
        {
          TopTools_ListIteratorOfListOfShape itLFAA(*pLFAA);
          for (; itLFAA.More(); itLFAA.Next())
          {
            // Solids containing the faces
            const TopTools_ListOfShape* pLS = myFSMap->Seek(itLFAA.Value());
            if (pLS)
            {
              TopTools_ListIteratorOfListOfShape itLS(*pLS);
              for (; itLS.More(); itLS.Next())
                mySolids.Add(itLS.Value());
            }
          }
        }
      }
    }
  }

  //! Extends the found adjacent faces and binds them to the original faces.
  void ExtendAdjacentFaces(const TopTools_IndexedMapOfShape& theMFAdjacent,
                           TopTools_IndexedDataMapOfShapeShape& theFaceExtFaceMap)
  {
    // Get the extension value for the faces - half of the diagonal of bounding box of the feature
    Bnd_Box aFeatureBox;
    BRepBndLib::Add(myFeature, aFeatureBox);

    const Standard_Real anExtLength = sqrt(aFeatureBox.SquareExtent());

    const Standard_Integer aNbFA = theMFAdjacent.Extent();
    for (Standard_Integer i = 1; i <= aNbFA; ++i)
    {
      const TopoDS_Face& aF = TopoDS::Face(theMFAdjacent(i));
      // Extend the face
      TopoDS_Face aFExt;
      BRepLib::ExtendFace(aF, anExtLength,
                          Standard_True, Standard_True,
                          Standard_True, Standard_True,
                          aFExt);
      theFaceExtFaceMap.Add(aF, aFExt);
      myHistory->AddModified(aF, aFExt);
    }
  }

  //! Trims the extended adjacent faces by intersection with each other
  //! and following intersection with the bounds of original faces.
  void TrimExtendedFaces(const TopTools_IndexedDataMapOfShapeShape& theFaceExtFaceMap)
  {
    // Intersect the extended faces first
    BOPAlgo_Builder aGFInter;
    // Add faces for intersection
    const Standard_Integer aNbF = theFaceExtFaceMap.Extent();
    for (Standard_Integer i = 1; i <= aNbF; ++i)
      aGFInter.AddArgument(theFaceExtFaceMap(i));

    aGFInter.SetRunParallel(myRunParallel);

    // Intersection result
    TopoDS_Shape anIntResult;
    if (aGFInter.Arguments().Extent() > 1)
    {
      aGFInter.Perform();
      if (aGFInter.HasErrors())
        return;

      anIntResult = aGFInter.Shape();

      myHistory->Merge<BOPAlgo_Builder>(aGFInter.Arguments(), aGFInter);
    }
    else
      anIntResult = aGFInter.Arguments().First();

    // Prepare the EF map of the extended faces after intersection
    // to select from them only boundary edges
    TopTools_IndexedDataMapOfShapeListOfShape anEFExtMap;
    TopExp::MapShapesAndAncestors(anIntResult, TopAbs_EDGE, TopAbs_FACE, anEFExtMap);

    // Get the splits of the extended faces after intersection
    // and trim them by the edges of the original faces

    // Map the edges of the Feature to avoid them during the trim
    TopTools_IndexedMapOfShape aFeatureEdgesMap;
    TopExp::MapShapes(myFeature, TopAbs_EDGE, aFeatureEdgesMap);

    for (Standard_Integer i = 1; i <= aNbF; ++i)
    {
      const TopoDS_Face& aFOriginal = TopoDS::Face(theFaceExtFaceMap.FindKey(i));
      const TopoDS_Face& aFExt = TopoDS::Face(theFaceExtFaceMap(i));
      TrimFace(aFExt, aFOriginal, aFeatureEdgesMap, anEFExtMap, aGFInter);
    }
  }

  //! Trim the extended faces by the bounds of the original face,
  //! except those contained in the feature to remove.
  void TrimFace(const TopoDS_Face& theFExt,
                const TopoDS_Face& theFOriginal,
                const TopTools_IndexedMapOfShape& theFeatureEdgesMap,
                const TopTools_IndexedDataMapOfShapeListOfShape& theEFExtMap,
                BOPAlgo_Builder& theGFInter)
  {
    // Map all edges of the extended face, to filter the result of trim
    // from the faces containing these edges
    TopTools_MapOfShape aMExtEdges;
    TopExp_Explorer anExpE(theFExt, TopAbs_EDGE);
    for (; anExpE.More(); anExpE.Next())
    {
      const TopoDS_Edge& aE = TopoDS::Edge(anExpE.Current());
      // skip degenerated and seam edges
      if (BRep_Tool::Degenerated(aE) || BRep_Tool::IsClosed(aE, theFExt))
        continue;
      TopTools_ListOfShape aLEIm;
      TakeModified(aE, theGFInter, aLEIm);
      TopTools_ListIteratorOfListOfShape itLEIm(aLEIm);
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
    TopTools_ListOfShape anExtSplits;
    TakeModified(theFExt, theGFInter, anExtSplits);
    aGFTrim.SetArguments(anExtSplits);

    // Add edges of the original faces
    TopTools_MapOfShape aMEdgesToCheckOri;
    anExpE.Init(theFOriginal, TopAbs_EDGE);
    for (; anExpE.More(); anExpE.Next())
    {
      const TopoDS_Edge& aE = TopoDS::Edge(anExpE.Current());
      if (!theFeatureEdgesMap.Contains(aE))
      {
        aGFTrim.AddArgument(aE);
        if (!BRep_Tool::Degenerated(aE) &&
            !BRep_Tool::IsClosed(aE, theFOriginal))
        {
          if (!aMEdgesToCheckOri.Add(aE))
            aMEdgesToCheckOri.Remove(aE);
        }
      }
    }

    // Avoid faces intersection
    aGFTrim.SetGlue(BOPAlgo_GlueShift);
    aGFTrim.SetRunParallel(myRunParallel);
    aGFTrim.SetNonDestructive(Standard_True);

    aGFTrim.Perform();
    if (aGFTrim.HasErrors())
      return;

    // Get all splits
    const TopoDS_Shape& aSplits = aGFTrim.Shape();
    // Filter the trimmed faces and save the valid ones into result map
    TopTools_ListOfShape aLFTrimmed;

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
      TopTools_IndexedDataMapOfShapeListOfShape anEFMap;
      TopTools_ListIteratorOfListOfShape itLF(aLFTrimmed);
      for (; itLF.More(); itLF.Next())
        TopExp::MapShapesAndAncestors(itLF.Value(), TopAbs_EDGE, TopAbs_FACE, anEFMap);

      // Check edges orientations
      TopTools_MapOfShape aFacesToAvoid, aValidFaces;
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

        TopTools_ListOfShape aLCB;
        BOPTools_AlgoTools::MakeConnexityBlocks(aCF, TopAbs_EDGE, TopAbs_FACE, aLCB);
        if (aLCB.Extent() > 1)
        {
          TopTools_ListIteratorOfListOfShape itLCB(aLCB);
          for (; itLCB.More(); itLCB.Next())
          {
            // Check if the block contains any valid faces
            const TopoDS_Shape& aCB = itLCB.Value();
            TopoDS_Iterator itF(aCB);
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
    myHistory->Merge<BOPAlgo_Builder>(aGFTrim.Arguments(), aGFTrim);

    // Update history with all removed shapes
    BRepTools_History aHistRem;

    // Map of the result splits
    TopTools_IndexedMapOfShape aResMap;
    TopTools_ListIteratorOfListOfShape itLF(aLFTrimmed);
    for (; itLF.More(); itLF.Next())
      TopExp::MapShapes(itLF.Value(), aResMap);

    TopTools_ListOfShape aLSplits;
    aLSplits.Append(aSplits);

    // Update the history with removed shapes
    MakeRemoved(aLSplits, aHistRem, aResMap);
    myHistory->Merge(aHistRem);
  }

private: //! @name Fields

  // Inputs
  Standard_Boolean myRunParallel;                     //!< Defines the mode of processing of the single feature
  TopoDS_Shape myFeature;                             //!< Feature to remove
  TopTools_IndexedDataMapOfShapeListOfShape* myEFMap; //!< EF Connection map to find adjacent faces
  TopTools_IndexedDataMapOfShapeListOfShape* myFSMap; //!< FS Connection map to find solids participating in the feature removal

  // Results
  TopTools_MapOfShape myFeatureFacesMap;              //!< Faces of the feature
  Standard_Boolean myHasAdjacentFaces;                //!< Flag to show whether the adjacent faces have been found or not
  TopTools_IndexedMapOfShape mySolids;                //!< Solids participating in the feature removal
  TopTools_IndexedDataMapOfShapeListOfShape myFaces;  //!< Reconstructed adjacent faces
  Handle(BRepTools_History) myHistory;                //!< History of the adjacent faces reconstruction
};

typedef NCollection_Vector<FillGap> VectorOfFillGap;

typedef BOPTools_Functor <FillGap, VectorOfFillGap> FillGapFunctor;

typedef BOPTools_Cnt <FillGapFunctor, VectorOfFillGap> FillGapCnt;

//=======================================================================

//=======================================================================
// function: RemoveFeatures
// purpose: Remove features by filling the gaps by extension of the
//          adjacent faces
//=======================================================================
void BOPAlgo_RemoveFeatures::RemoveFeatures()
{
  // For each feature:
  // - Find the faces adjacent to the feature;
  // - Extend the adjacent faces;
  // - Trim the extended faces to fill the gap;
  // - Rebuild the solids with reconstructed adjacent faces
  //   and avoiding the feature faces.

  // Make Edge-Face connection map of the input
  // shape to find faces adjacent to the feature
  TopTools_IndexedDataMapOfShapeListOfShape anEFMap;
  TopExp::MapShapesAndAncestors(myInputShape, TopAbs_EDGE, TopAbs_FACE, anEFMap);

  // Make Face-Solid connection map to find the solids
  // participating in the removal of each feature
  TopTools_IndexedDataMapOfShapeListOfShape anFSMap;
  TopExp::MapShapesAndAncestors(myInputShape, TopAbs_FACE, TopAbs_SOLID, anFSMap);

  // Tool for reconstruction of the faces adjacent to the feature
  // in parallel threads if necessary.
  VectorOfFillGap aVFG;
  // Fill the vector
  TopTools_ListIteratorOfListOfShape itF(myFeatures);
  for (; itF.More(); itF.Next())
  {
    const TopoDS_Shape& aFeature = itF.Value();
    FillGap& aFG = aVFG.Appended();
    aFG.SetFeature(aFeature);
    aFG.SetEFConnectionMap(anEFMap);
    aFG.SetFSConnectionMap(anFSMap);
    aFG.SetRunParallel(myRunParallel);
  }

  // Perform the reconstruction of the adjacent faces
  FillGapCnt::Perform(myRunParallel, aVFG);

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
  const Standard_Integer aNbF = aVFG.Length();
  for (Standard_Integer i = 0; i < aNbF; ++i)
  {
    FillGap& aFG = aVFG(i);

    // No need to fill the history for solids if the history is not
    // requested and the current feature is the last one.
    Standard_Boolean isSolidsHistoryNeeded = myTrackHistory || (i < (aNbF - 1));

    // Perform removal of the single feature
    RemoveFeature(aFG.Feature(), aFG.Solids(), aFG.FeatureFacesMap(),
                  aFG.HasAdjacentFaces(), aFG.Faces(), aFG.History(),
                  isSolidsHistoryNeeded);
  }
}

//=======================================================================
// function: RemoveFeature
// purpose: Remove the single feature
//=======================================================================
void BOPAlgo_RemoveFeatures::RemoveFeature
  (const TopoDS_Shape& theFeature,
   const TopTools_IndexedMapOfShape& theSolids,
   const TopTools_MapOfShape& theFeatureFacesMap,
   const Standard_Boolean theHasAdjacentFaces,
   const TopTools_IndexedDataMapOfShapeListOfShape& theAdjFaces,
   const Handle(BRepTools_History)& theAdjFacesHistory,
   const Standard_Boolean theSolidsHistoryNeeded)
{
  Standard_Boolean bFuseShapes = Standard_True;
  const Standard_Integer aNbAF = theAdjFaces.Extent();
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
    bFuseShapes = Standard_False;
  }

  // Rebuild the shape using MakerVolume algorithm avoiding the faces of the
  // feature and replacing the adjacent faces with their images

  BRep_Builder aBB;

  // From the faces of input shape build map of faces with which the result will be validated:
  // - all non-internal faces of the input shape (except for adjacent and feature faces)
  //   must have some trace in the result solids;
  // - all adjacent faces (if any) must have some trace in the result solids.
  TopTools_IndexedMapOfShape aFacesToBeKept;
  // Build also the map of faces to compare orientation of the original and result faces
  TopTools_MapOfShape aFacesToCheckOri;

  // Collect list of internal entities of the input shape to be avoided in result
  // and to make them removed in the history.
  TopTools_ListOfShape anInternalShapes;

  // In the feature removal will participate only the solids connected to the feature
  // or the faces adjacent to the feature.

  // Solids to be rebuilt  after the feature removal
  TopTools_IndexedMapOfShape aSolidsToRebuild;
  // Find faces shared between solids
  TopTools_ListOfShape aSharedFaces;
  // Solids to be avoided in the feature removal and added into result directly
  TopTools_ListOfShape anUnTouchedSolids;

  // Prepare to the feature removal - fill all necessary containers
  GetOriginalFaces(myShape, theSolids, theFeatureFacesMap, theAdjFaces, myHistory,
                   aFacesToBeKept, anInternalShapes, aFacesToCheckOri,
                   aSolidsToRebuild, aSharedFaces, anUnTouchedSolids);

  // To avoid excessive intersection of the faces collect the faces
  // of the input shape into a compound
  TopoDS_Compound anOrigF;
  aBB.MakeCompound(anOrigF);
  Standard_Integer aNbFK = aFacesToBeKept.Extent();
  for (Standard_Integer i = 1; i <= aNbFK; ++i)
    aBB.Add(anOrigF, aFacesToBeKept(i));

  // Tool for solids reconstruction
  BOPAlgo_MakerVolume aMV;
  aMV.SetRunParallel(myRunParallel);
  aMV.SetAvoidInternalShapes(Standard_True);
  aMV.SetIntersect(bFuseShapes);
  aMV.SetNonDestructive(Standard_True);
  // Add faces of the input shape
  aMV.AddArgument(anOrigF);

  // Add reconstructed adjacent faces
  for (Standard_Integer i = 1; i <= aNbAF; ++i)
  {
    const TopTools_ListOfShape& aLFA = theAdjFaces(i);
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
      TopTools_ListIteratorOfListOfShape itLFA(aLFA);
      for (; itLFA.More(); itLFA.Next())
        aBB.Add(anAdjF, itLFA.Value());

      aMV.AddArgument(anAdjF);
      aFacesToBeKept.Add(anAdjF);
    }

    if (myTrackHistory)
    {
      // Look for internal edges in the original adjacent faces
      const TopoDS_Shape& aFOr = theAdjFaces.FindKey(i);
      FindInternals(aFOr, anInternalShapes);
    }
  }

  // Build solids
  aMV.Perform();
  if (aMV.HasErrors())
  {
    // Add warning for the feature
    AddWarning(new BOPAlgo_AlertUnableToRemoveTheFeature(theFeature));
    return;
  }

  // Result of MV operation
  const TopoDS_Shape& aSolids = aMV.Shape();
  TopExp_Explorer anExpS(aSolids, TopAbs_SOLID);
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
  TopTools_MapOfShape anAdjFacesSplits;
  for (Standard_Integer i = 1; i <= aNbAF; ++i)
  {
    const TopoDS_Shape& aF = theAdjFaces.FindKey(i);
    const TopTools_ListOfShape& aLFIm = myHistory->Modified(aF);
    if (aLFIm.IsEmpty())
      anAdjFacesSplits.Add(aF);
    else
    {
      TopTools_ListIteratorOfListOfShape itLFIm(aLFIm);
      for (; itLFIm.More(); itLFIm.Next())
        anAdjFacesSplits.Add(itLFIm.Value());
    }
  }

  // Validate the result
  Standard_Boolean bValid = Standard_True;
  aNbFK = aFacesToBeKept.Extent();
  for (Standard_Integer i = 1; i <= aNbFK && bValid; ++i)
  {
    const TopoDS_Shape& aS = aFacesToBeKept(i);
    if (anAdjFacesSplits.Contains(aS))
      continue;
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
  TopTools_ListOfShape aLSRes;
  // Remember the removed shapes
  TopTools_ListOfShape aRemovedShapes;
  GetValidSolids(aMV, aFacesToCheckOri, aSharedFaces, anOrigF, theSolids.Extent(), aLSRes, aRemovedShapes);

  if (aLSRes.Extent() != theSolids.Extent())
  {
    // Add warning for the feature
    AddWarning(new BOPAlgo_AlertUnableToRemoveTheFeature(theFeature));
    return;
  }

  // Remove internal wires from the faces, possibly appeared after intersection
  RemoveInternalWires(aLSRes, &anInternalShapes);

  // Update history with:
  // History of adjacent faces reconstruction
  myHistory->Merge(theAdjFacesHistory);
  // History of intersection
  myHistory->Merge<BOPAlgo_MakerVolume>(aMV.Arguments(), aMV);

  if (myTrackHistory)
  {
    // Map the result to check if the shape is removed
    TopTools_IndexedMapOfShape aMSRes;
    TopTools_ListIteratorOfListOfShape itLS(aLSRes);
    for (; itLS.More(); itLS.Next())
      TopExp::MapShapes(itLS.Value(), aMSRes);

    // Remove internal shapes and extra faces
    BRepTools_History aRemHist;
    anInternalShapes.Append(aRemovedShapes);
    MakeRemoved(anInternalShapes, aRemHist, aMSRes);
    myHistory->Merge(aRemHist);
  }

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
  TopTools_ListIteratorOfListOfShape itLS(aLSRes);
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
void BOPAlgo_RemoveFeatures::UpdateHistory()
{
  if (!myTrackHistory)
    return;

  // Map the result
  TopExp::MapShapes(myShape, myResultMap);

  // Update the history
  BRepTools_History aHistory;

  const Standard_Integer aNbS = myInputsMap.Extent();
  for (Standard_Integer i = 1; i <= aNbS; ++i)
  {
    const TopoDS_Shape& aS = myInputsMap(i);
    if (!BRepTools_History::IsSupportedType(aS))
      continue;

    if (myHistory->IsRemoved(aS))
      continue;

    // Check if the shape has any trace in the result
    const TopTools_ListOfShape& aLSIm = myHistory->Modified(aS);
    if (aLSIm.IsEmpty())
    {
      if (!myResultMap.Contains(aS))
        aHistory.Remove(aS);
    }

    TopTools_ListIteratorOfListOfShape itLSIm(aLSIm);
    for (; itLSIm.More(); itLSIm.Next())
    {
      if (!myResultMap.Contains(itLSIm.Value()))
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
void BOPAlgo_RemoveFeatures::SimplifyResult()
{
  if (myShape.IsSame(myInputShape))
    return;
  ShapeUpgrade_UnifySameDomain aSDTool;
  aSDTool.Initialize(myShape, Standard_True, Standard_True);
  // Do not allow producing internal edges
  aSDTool.AllowInternalEdges(Standard_False);
  // Avoid removal of the input edges and vertices
  if (myResultMap.IsEmpty())
    TopExp::MapShapes(myShape, myResultMap);

  const Standard_Integer aNbS = myInputsMap.Extent();
  for (Standard_Integer i = 1; i <= aNbS; ++i)
  {
    if (myResultMap.Contains(myInputsMap(i)))
      aSDTool.KeepShape(myInputsMap(i));
  }

  // Perform unification
  aSDTool.Build();
  myShape = aSDTool.Shape();
  if (myTrackHistory)
    myHistory->Merge(aSDTool.History());
}

//=======================================================================
// function: PostTreat
// purpose: Restore the type of the initial shape
//=======================================================================
void BOPAlgo_RemoveFeatures::PostTreat()
{
  const TopAbs_ShapeEnum anInputType = myInputShape.ShapeType();
  const TopAbs_ShapeEnum aResType = myShape.ShapeType();
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
void MakeRemoved(const TopTools_ListOfShape& theShapes,
                 BRepTools_History& theHistory,
                 const TopTools_IndexedMapOfShape& theKeepShapes)
{
  TopTools_IndexedMapOfShape aShapesMap;
  TopTools_ListIteratorOfListOfShape it(theShapes);
  for (; it.More(); it.Next())
    TopExp::MapShapes(it.Value(), aShapesMap);

  const Standard_Integer aNbS = aShapesMap.Extent();
  for (Standard_Integer i = 1; i <= aNbS; ++i)
  {
    const TopoDS_Shape& aS = aShapesMap(i);
    if (!theKeepShapes.Contains(aS) &&
        BRepTools_History::IsSupportedType(aS))
    {
      theHistory.Remove(aS);
    }
  }
}

//=======================================================================
// function: FindInternals
// purpose: Looks for internal shapes inside the face or solid
//=======================================================================
void FindInternals(const TopoDS_Shape& theS,
                   TopTools_ListOfShape& theLInt)
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
void RemoveInternalWires(const TopTools_ListOfShape& theShapes,
                         TopTools_ListOfShape *theRemoved)
{
  TopTools_ListIteratorOfListOfShape itLS(theShapes);
  for (; itLS.More(); itLS.Next())
  {
    const TopoDS_Shape& aShape = itLS.Value();
    TopExp_Explorer anExpF(aShape, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
    {
      TopoDS_Face& aF = *(TopoDS_Face*)&anExpF.Current();
      TopTools_ListOfShape aLWToRemove;
      FindInternals(aF, aLWToRemove);
      if (aLWToRemove.Extent())
      {
        aF.Free(Standard_True);
        TopTools_ListIteratorOfListOfShape itR(aLWToRemove);
        for (; itR.More(); itR.Next())
        {
          if (theRemoved)
            theRemoved->Append(itR.Value());
          BRep_Builder().Remove(aF, itR.Value());
        }
        aF.Free(Standard_False);
      }
    }
  }
}

//=======================================================================
// function: GetOriginalFaces
// purpose: Get original faces from my face
//=======================================================================
void GetOriginalFaces(const TopoDS_Shape& theShape,
                      const TopTools_IndexedMapOfShape& theSolids,
                      const TopTools_MapOfShape& theFeatureFacesMap,
                      const TopTools_IndexedDataMapOfShapeListOfShape& theAdjFaces,
                      const Handle(BRepTools_History)& theHistory,
                      TopTools_IndexedMapOfShape& theFacesToBeKept,
                      TopTools_ListOfShape& theInternalShapes,
                      TopTools_MapOfShape& theFacesToCheckOri,
                      TopTools_IndexedMapOfShape& theSolidsToRebuild,
                      TopTools_ListOfShape& theSharedFaces,
                      TopTools_ListOfShape& theUnTouchedSolids)
{
  // Use only solids which has to be reconstructed by the feature.
  // All other solids should be avoided in the feature removal and added
  // into result directly.

  // Update solids after removal of previous features
  const Standard_Integer aNbSols = theSolids.Extent();
  for (Standard_Integer i = 1; i <= aNbSols; ++i)
  {
    const TopoDS_Shape& aSol = theSolids(i);
    const TopTools_ListOfShape& aLFIm = theHistory->Modified(aSol);
    if (aLFIm.IsEmpty())
      theSolidsToRebuild.Add(aSol);
    else
      theSolidsToRebuild.Add(aLFIm.First());
  }

  // Splits of the feature faces
  TopTools_MapOfShape aFeatureFacesSplits;
  TopTools_MapIteratorOfMapOfShape itM(theFeatureFacesMap);
  for (; itM.More(); itM.Next())
  {
    const TopoDS_Shape& aF = itM.Value();
    const TopTools_ListOfShape& aLFIm = theHistory->Modified(aF);
    if (aLFIm.IsEmpty())
      aFeatureFacesSplits.Add(aF);
    else
    {
      TopTools_ListIteratorOfListOfShape itLFIm(aLFIm);
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
void FindShape(const TopoDS_Shape& theSWhat,
               const TopoDS_Shape& theSWhere,
               TopoDS_Shape& theSFound)
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
void GetValidSolids(BOPAlgo_MakerVolume& theMV,
                    const TopTools_MapOfShape& theFacesToCheckOri,
                    const TopTools_ListOfShape& aSharedFaces,
                    const TopoDS_Shape& theOrigFaces,
                    const Standard_Integer theNbSol,
                    TopTools_ListOfShape& theLSRes,
                    TopTools_ListOfShape& theRemovedShapes)
{
  TopExp_Explorer anExpS(theMV.Shape(), TopAbs_SOLID);
  for (; anExpS.More(); anExpS.Next())
    theLSRes.Append(anExpS.Current());

  if (theLSRes.Extent() > theNbSol)
  {
    // Find Solids filling the holes in the initial shape
    TopTools_MapOfShape aSolidsToAvoid;
    TopTools_IndexedDataMapOfShapeListOfShape aFSMap;
    TopExp::MapShapesAndAncestors(theMV.Shape(), TopAbs_FACE, TopAbs_SOLID, aFSMap);
    FindExtraShapes(aFSMap, theFacesToCheckOri, theMV, aSolidsToAvoid);

    TopTools_ListIteratorOfListOfShape itLS(theLSRes);
    for (; itLS.More(); )
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
    TopTools_MapOfShape anOrigFacesRes;
    TopExp_Explorer anExpF(theOrigFaces, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
      TakeModified(anExpF.Current(), theMV, anOrigFacesRes);

    TopTools_ListIteratorOfListOfShape itLS(theLSRes);
    for (; itLS.More(); )
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
void FindExtraShapes(const TopTools_IndexedDataMapOfShapeListOfShape& theConnectionMap,
                     const TopTools_MapOfShape& theShapesToCheckOri,
                     BOPAlgo_Builder& theBuilder,
                     TopTools_MapOfShape& theShapesToAvoid,
                     TopTools_MapOfShape* theValidShapes)
{
  Handle(IntTools_Context) aCtx = theBuilder.Context();
  TopTools_MapOfShape aValidShapes;
  TopTools_MapOfShape* pValidShapes = theValidShapes ? theValidShapes : &aValidShapes;
  TopTools_MapIteratorOfMapOfShape itM(theShapesToCheckOri);
  for (; itM.More(); itM.Next())
  {
    const TopoDS_Shape& aSToCheckOri = itM.Value();
    // Check modification of the shape during intersection
    TopTools_ListOfShape aLSIm;
    TakeModified(aSToCheckOri, theBuilder, aLSIm);

    TopTools_ListIteratorOfListOfShape itLSIm(aLSIm);
    for (; itLSIm.More(); itLSIm.Next())
    {
      const TopoDS_Shape& aSIm = itLSIm.Value();

      const TopTools_ListOfShape* pShapesToValidate = theConnectionMap.Seek(aSIm);
      if (!pShapesToValidate)
        continue;

      TopTools_ListIteratorOfListOfShape itSV(*pShapesToValidate);
      for (; itSV.More(); itSV.Next())
      {
        const TopoDS_Shape& aShapeToValidate = itSV.Value();
        if (pValidShapes->Contains(aShapeToValidate))
          continue;

        TopoDS_Face aSInShape;
        FindShape(aSIm, aShapeToValidate, aSInShape);

        Standard_Boolean bSameOri =
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
void AvoidExtraSharedFaces(TopTools_ListOfShape& theLSolids,
                           const TopTools_ListOfShape& theLFSharedToAvoid,
                           BOPAlgo_Builder& theBuilder,
                           TopTools_ListOfShape& theExtraFaces)
{
  // Get all splits of shared faces to avoid in the check
  TopTools_MapOfShape aMFSharedSp;
  {
    TopTools_ListOfShape aLFSharedSp;
    TopTools_ListIteratorOfListOfShape itLFS(theLFSharedToAvoid);
    for (; itLFS.More(); itLFS.Next())
      TakeModified(itLFS.Value(), theBuilder, aMFSharedSp);
  }

  TopTools_IndexedDataMapOfShapeListOfShape aFSMap;
  TopTools_ListIteratorOfListOfShape itLS(theLSolids);
  for (; itLS.More(); itLS.Next())
    TopExp::MapShapesAndAncestors(itLS.Value(), TopAbs_FACE, TopAbs_SOLID, aFSMap);

  TopTools_ListOfShape anExtraFaces;
  TopTools_ListOfShape aLFArguments;
  itLS.Initialize(theLSolids);
  for (; itLS.More(); itLS.Next())
  {
    const TopoDS_Shape& aSol = itLS.Value();
    TopExp_Explorer anExpF(aSol, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
    {
      const TopoDS_Shape& aF = anExpF.Current();
      const TopTools_ListOfShape& aLSol = aFSMap.FindFromKey(aF);
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
  aBS.SetAvoidInternalShapes(Standard_True);
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
void FillSolidsHistory(const TopTools_IndexedMapOfShape& theSolIn,
                       TopTools_ListOfShape& theSolidsOut,
                       const TopTools_IndexedDataMapOfShapeListOfShape& theAdjFaces,
                       BOPAlgo_Builder& theBuilder,
                       BRepTools_History& theSolidsHistory)
{
  const Standard_Integer aNbS = theSolIn.Extent();
  for (Standard_Integer i = 1; i <= aNbS; ++i)
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
      TopTools_ListIteratorOfListOfShape itLS(theSolidsOut);
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
void TakeModified(const TopoDS_Shape& theS,
                  BOPAlgo_Builder& theBuilder,
                  TopTools_ListOfShape& theList)
{
  const TopTools_ListOfShape& aModified = theBuilder.Modified(theS);
  if (aModified.IsEmpty() && !theBuilder.IsDeleted(theS))
    theList.Append(theS);
  else
  {
    TopTools_ListIteratorOfListOfShape itM(aModified);
    for (; itM.More(); itM.Next())
      theList.Append(itM.Value());
  }
}
//=======================================================================
// function: TakeModified
// purpose: Stores the modified object into the map
//=======================================================================
void TakeModified(const TopoDS_Shape& theS,
                  BOPAlgo_Builder& theBuilder,
                  TopTools_MapOfShape& theMap)
{
  const TopTools_ListOfShape& aModified = theBuilder.Modified(theS);
  if (aModified.IsEmpty() && !theBuilder.IsDeleted(theS))
    theMap.Add(theS);
  else
  {
    TopTools_ListIteratorOfListOfShape itM(aModified);
    for (; itM.More(); itM.Next())
      theMap.Add(itM.Value());
  }
}

//=======================================================================
// function: FindSolid
// purpose: Looks for the image of the solid in the list of resulting solids
//=======================================================================
void FindSolid(const TopoDS_Shape& theSolIn,
               const TopTools_ListOfShape& theSolidsRes,
               const TopTools_IndexedDataMapOfShapeListOfShape& theAdjFaces,
               BOPAlgo_Builder& theBuilder,
               TopoDS_Shape& theSolOut)
{
  Handle(IntTools_Context) aCtx = theBuilder.Context();

  // Take the face in the IN solid, and find it in the OUT list
  TopExp_Explorer anExpF(theSolIn, TopAbs_FACE);
  for (; anExpF.More(); anExpF.Next())
  {
    const TopoDS_Shape& aFS = anExpF.Current();
    // Images of the face
    TopTools_MapOfShape aMFSIm;
    const TopTools_ListOfShape* pLFA = theAdjFaces.Seek(aFS);
    if (pLFA)
    {
      TopTools_ListIteratorOfListOfShape itLFA(*pLFA);
      for (; itLFA.More(); itLFA.Next())
        TakeModified(itLFA.Value(), theBuilder, aMFSIm);
    }
    else
    {
      TakeModified(aFS, theBuilder, aMFSIm);
    }

    // Find any of these faces in the list of solids
    TopTools_ListIteratorOfListOfShape itLS(theSolidsRes);
    for (; itLS.More(); itLS.Next())
    {
      const TopoDS_Shape& aSol = itLS.Value();
      TopExp_Explorer anExpFOut(aSol, TopAbs_FACE);
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
