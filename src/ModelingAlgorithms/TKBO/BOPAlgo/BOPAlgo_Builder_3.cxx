// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
//
#include <BOPAlgo_Builder.hxx>
//
//
#include <TopAbs_State.hxx>
//
#include <TopoDS.hxx>
#include <TopoDS_AlertWithShape.hxx>
#include <TopoDS_Shape.hxx>
//
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
//
#include <BRep_Builder.hxx>
//
#include <BOPAlgo_BuilderSolid.hxx>
//
#include <IntTools_Context.hxx>
//
#include <BOPDS_DS.hxx>
#include <BOPDS_ShapeInfo.hxx>
//
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Set.hxx>
#include <NCollection_Map.hxx>
#include <BOPTools_Parallel.hxx>
//
#include <BOPAlgo_Tools.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Vector.hxx>

#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedDataMap.hxx>

#include <algorithm>

static void OwnInternalShapes(const TopoDS_Shape&,
                              NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&);

//=================================================================================================

void BOPAlgo_Builder::FillImagesSolids(const Message_ProgressRange& theRange)
{
  int i = 0, aNbS = myDS->NbSourceShapes();
  for (i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() == TopAbs_SOLID)
    {
      break;
    }
  }
  if (i >= aNbS)
  {
    return;
  }

  Message_ProgressScope aPS(theRange, "Building splits of solids", 10);
  // Draft solids
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aDraftSolids;
  // Find all IN faces for all IN faces
  FillIn3DParts(aDraftSolids, aPS.Next(4));
  if (HasErrors())
  {
    return;
  }
  // Build split of the solids
  BuildSplitSolids(aDraftSolids, aPS.Next(5));
  if (HasErrors())
  {
    return;
  }
  // Fill solids with internal parts
  FillInternalShapes(aPS.Next());
}

//=================================================================================================

void BOPAlgo_Builder::FillIn3DParts(
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theDraftSolids,
  const Message_ProgressRange&                                              theRange)
{
  Message_ProgressScope aPS(theRange, nullptr, 2);

  occ::handle<NCollection_BaseAllocator> anAlloc = new NCollection_IncAllocator;

  // Find all faces that are IN solids

  // Store boxes of the shapes into a map
  NCollection_DataMap<TopoDS_Shape, Bnd_Box, TopTools_ShapeMapHasher> aShapeBoxMap(1, anAlloc);

  // Fence map
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence(1, anAlloc);

  // Get all faces
  NCollection_List<TopoDS_Shape> aLFaces(anAlloc);

  int i, aNbS = myDS->NbSourceShapes();
  for (i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE)
      continue;

    if (UserBreak(aPS))
    {
      return;
    }

    const TopoDS_Shape&                   aS    = aSI.Shape();
    const NCollection_List<TopoDS_Shape>* pLSIm = myImages.Seek(aS);

    if (pLSIm)
    {
      NCollection_List<TopoDS_Shape>::Iterator aItLSIm(*pLSIm);
      for (; aItLSIm.More(); aItLSIm.Next())
      {
        const TopoDS_Shape& aSIm = aItLSIm.Value();
        if (aMFence.Add(aSIm))
          aLFaces.Append(aSIm);
      }
    }
    else
    {
      aLFaces.Append(aS);
      aShapeBoxMap.Bind(aS, aSI.Box());
    }
  }

  BRep_Builder aBB;

  // Get all solids
  NCollection_List<TopoDS_Shape> aLSolids(anAlloc);
  // Keep INTERNAL faces of the solids
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aSolidsIF(1, anAlloc);
  // Draft solids
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aDraftSolid(
    1,
    anAlloc);

  for (i = 0; i < aNbS; ++i)
  {
    BOPDS_ShapeInfo& aSI = myDS->ChangeShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_SOLID)
    {
      continue;
    }
    if (UserBreak(aPS))
    {
      return;
    }
    const TopoDS_Shape& aS     = aSI.Shape();
    const TopoDS_Solid& aSolid = (*(TopoDS_Solid*)(&aS));
    //
    // Bounding box for the solid aS
    Bnd_Box& aBoxS = aSI.ChangeBox();
    if (aBoxS.IsVoid())
      myDS->BuildBndBoxSolid(i, aBoxS, myCheckInverted);

    // Build Draft Solid
    NCollection_List<TopoDS_Shape> aLIF;
    TopoDS_Solid                   aSD;
    aBB.MakeSolid(aSD);
    BuildDraftSolid(aSolid, aSD, aLIF);

    aLSolids.Append(aSD);
    aSolidsIF.Bind(aSD, aLIF);
    aShapeBoxMap.Bind(aSD, aBoxS);
    aDraftSolid.Add(aS, aSD);
  }

  // Perform classification of the faces
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anInParts;

  BOPAlgo_Tools::ClassifyFaces(aLFaces,
                               aLSolids,
                               myRunParallel,
                               myContext,
                               anInParts,
                               aShapeBoxMap,
                               aSolidsIF,
                               aPS.Next());

  // Analyze the results of classification
  int aNbSol = aDraftSolid.Extent();
  for (i = 1; i <= aNbSol; ++i)
  {
    if (UserBreak(aPS))
    {
      return;
    }
    const TopoDS_Solid&                   aSolid     = TopoDS::Solid(aDraftSolid.FindKey(i));
    const TopoDS_Solid&                   aSDraft    = TopoDS::Solid(aDraftSolid(i));
    const NCollection_List<TopoDS_Shape>& aLInFaces  = anInParts.FindFromKey(aSDraft);
    const NCollection_List<TopoDS_Shape>& aLInternal = aSolidsIF.Find(aSDraft);

    int aNbIN = aLInFaces.Extent();

    if (!aNbIN)
    {
      bool bHasImage = false;
      // Check if the shells of the solid have image
      for (TopoDS_Iterator it(aSolid); it.More() && !bHasImage; it.Next())
        bHasImage = myImages.IsBound(it.Value());

      if (!bHasImage)
        // no need to split the solid
        continue;
    }

    theDraftSolids.Bind(aSolid, aSDraft);

    int aNbInt = aLInternal.Extent();
    if (aNbInt || aNbIN)
    {
      // Combine the lists
      NCollection_List<TopoDS_Shape>* pLIN =
        myInParts.Bound(aSolid, NCollection_List<TopoDS_Shape>());

      NCollection_List<TopoDS_Shape>::Iterator aItLS(aLInFaces);
      for (; aItLS.More(); aItLS.Next())
        pLIN->Append(aItLS.Value());

      aItLS.Initialize(aLInternal);
      for (; aItLS.More(); aItLS.Next())
        pLIN->Append(aItLS.Value());
    }
  }
}

//=================================================================================================

void BOPAlgo_Builder::BuildDraftSolid(const TopoDS_Shape&             theSolid,
                                      TopoDS_Shape&                   theDraftSolid,
                                      NCollection_List<TopoDS_Shape>& theLIF)
{
  bool                                     bToReverse;
  int                                      iFlag;
  TopAbs_Orientation                       aOrF, aOrSh, aOrSd;
  TopoDS_Iterator                          aIt1, aIt2;
  TopoDS_Shell                             aShD;
  TopoDS_Shape                             aFx;
  BRep_Builder                             aBB;
  NCollection_List<TopoDS_Shape>::Iterator aItS;
  //
  aOrSd = theSolid.Orientation();
  theDraftSolid.Orientation(aOrSd);
  //
  aIt1.Initialize(theSolid);
  for (; aIt1.More(); aIt1.Next())
  {
    const TopoDS_Shape& aSh = aIt1.Value();
    if (aSh.ShapeType() != TopAbs_SHELL)
    {
      continue; // mb internal edges,vertices
    }
    //
    aOrSh = aSh.Orientation();
    aBB.MakeShell(aShD);
    aShD.Orientation(aOrSh);
    iFlag = 0;
    //
    aIt2.Initialize(aSh);
    for (; aIt2.More(); aIt2.Next())
    {
      const TopoDS_Shape& aF = aIt2.Value();
      aOrF                   = aF.Orientation();
      //
      if (myImages.IsBound(aF))
      {
        const NCollection_List<TopoDS_Shape>& aLSp = myImages.Find(aF);
        aItS.Initialize(aLSp);
        for (; aItS.More(); aItS.Next())
        {
          aFx = aItS.Value();
          //
          if (myShapesSD.IsBound(aFx))
          {
            //
            if (aOrF == TopAbs_INTERNAL)
            {
              aFx.Orientation(aOrF);
              theLIF.Append(aFx);
            }
            else
            {
              bToReverse =
                BOPTools_AlgoTools::IsSplitToReverseWithWarn(aFx, aF, myContext, myReport);
              if (bToReverse)
              {
                aFx.Reverse();
              }
              //
              iFlag = 1;
              aBB.Add(aShD, aFx);
            }
          } // if (myShapesSD.IsBound(aFx)) {
          else
          {
            aFx.Orientation(aOrF);
            if (aOrF == TopAbs_INTERNAL)
            {
              theLIF.Append(aFx);
            }
            else
            {
              iFlag = 1;
              aBB.Add(aShD, aFx);
            }
          }
        }
      } // if (myImages.IsBound(aF)) {
      //
      else
      {
        if (aOrF == TopAbs_INTERNAL)
        {
          theLIF.Append(aF);
        }
        else
        {
          iFlag = 1;
          aBB.Add(aShD, aF);
        }
      }
    } // for (; aIt2.More(); aIt2.Next()) {
    //
    if (iFlag)
    {
      aShD.Closed(BRep_Tool::IsClosed(aShD));
      aBB.Add(theDraftSolid, aShD);
    }
  } // for (; aIt1.More(); aIt1.Next()) {
}

//=======================================================================

//=======================================================================
// class : BOPAlgo_SplitSolid
// purpose  : Auxiliary class to extend the BOPAlgo_BuilderSolid with the solid to split
//=======================================================================
class BOPAlgo_SplitSolid : public BOPAlgo_BuilderSolid
{
public:
  //! Sets the solid
  void SetSolid(const TopoDS_Solid& theSolid) { mySolid = theSolid; }

  //! Returns the solid
  const TopoDS_Solid& Solid() const { return mySolid; }

  //! Sets progress range
  void SetProgressRange(const Message_ProgressRange& theRange) { myRange = theRange; }

  // New perform method, using own progress range
  void Perform()
  {
    Message_ProgressScope aPS(myRange, nullptr, 1);
    if (!aPS.More())
    {
      return;
    }
    BOPAlgo_BuilderSolid::Perform(aPS.Next());
  }

private:
  //! Disable the range enabled method
  virtual void Perform(const Message_ProgressRange& /* theRange*/) {}

private:
  TopoDS_Solid          mySolid; //!< Solid to split
  Message_ProgressRange myRange;
};

// Vector of Solid Builders
typedef NCollection_Vector<BOPAlgo_SplitSolid> BOPAlgo_VectorOfBuilderSolid;

//=================================================================================================

void BOPAlgo_Builder::BuildSplitSolids(
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theDraftSolids,
  const Message_ProgressRange&                                              theRange)
{
  bool                                     bFlagSD;
  int                                      i, aNbS;
  TopExp_Explorer                          aExp;
  NCollection_List<TopoDS_Shape>::Iterator aIt;
  //
  occ::handle<NCollection_BaseAllocator> aAlr0;
  aAlr0 = NCollection_BaseAllocator::CommonBaseAllocator();
  //
  NCollection_List<TopoDS_Shape>                         aSFS(aAlr0), aLSEmpty(aAlr0);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence(100, aAlr0);
  NCollection_Map<BOPTools_Set>                          aMST(100, aAlr0);
  BOPAlgo_VectorOfBuilderSolid                           aVBS;
  //
  Message_ProgressScope aPSOuter(theRange, nullptr, 10);
  // 0. Find same domain solids for non-interfered solids
  aNbS = myDS->NbSourceShapes();
  for (i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    //
    if (aSI.ShapeType() != TopAbs_SOLID)
    {
      continue;
    }
    if (UserBreak(aPSOuter))
    {
      return;
    }
    //
    const TopoDS_Shape& aS = aSI.Shape();
    if (!aMFence.Add(aS))
    {
      continue;
    }
    if (theDraftSolids.IsBound(aS))
    {
      continue;
    }
    //
    BOPTools_Set aST;
    //
    aST.Add(aS, TopAbs_FACE);
    aMST.Add(aST);
    //
  } // for (i=1; i<=aNbS; ++i)
  //
  // Build temporary map of solids images to avoid rebuilding
  // of the solids without internal faces
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aSolidsIm;
  // 1. Build solids for interfered source solids
  for (i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_SOLID)
      continue;

    const TopoDS_Shape& aS     = aSI.Shape();
    const TopoDS_Solid& aSolid = (*(TopoDS_Solid*)(&aS));
    if (!theDraftSolids.IsBound(aS))
      continue;

    const TopoDS_Shape&                   aSD   = theDraftSolids.Find(aS);
    const NCollection_List<TopoDS_Shape>* pLFIN = myInParts.Seek(aS);
    if (!pLFIN || pLFIN->IsEmpty())
    {
      aSolidsIm(aSolidsIm.Add(aS, NCollection_List<TopoDS_Shape>())).Append(aSD);
      continue;
    }

    aSFS.Clear();
    //
    // 1.1 Fill Shell Faces Set
    aExp.Init(aSD, TopAbs_FACE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Shape& aF = aExp.Current();
      aSFS.Append(aF);
    }
    //
    // 1.2 Fill internal faces
    aIt.Initialize(*pLFIN);
    for (; aIt.More(); aIt.Next())
    {
      TopoDS_Shape aF = aIt.Value();
      //
      aF.Orientation(TopAbs_FORWARD);
      aSFS.Append(aF);
      aF.Orientation(TopAbs_REVERSED);
      aSFS.Append(aF);
    }
    //
    // 1.3 Build new solids
    BOPAlgo_SplitSolid& aBS = aVBS.Appended();
    aBS.SetSolid(aSolid);
    aBS.SetShapes(aSFS);
    aBS.SetRunParallel(myRunParallel);
  } // for (i=0; i<aNbS; ++i) {
  //
  int k, aNbBS;
  //
  aNbBS = aVBS.Length();
  // Set progress range for each task to be run in parallel
  Message_ProgressScope aPSParallel(aPSOuter.Next(9), "Splitting solids", aNbBS);
  for (int iS = 0; iS < aNbBS; iS++)
  {
    BOPAlgo_SplitSolid& aSplitSolid = aVBS.ChangeValue(iS);
    aSplitSolid.SetProgressRange(aPSParallel.Next());
  }
  //
  //===================================================
  BOPTools_Parallel::Perform(myRunParallel, aVBS);
  //===================================================
  if (UserBreak(aPSOuter))
  {
    return;
  }
  //
  for (k = 0; k < aNbBS; ++k)
  {
    BOPAlgo_SplitSolid& aBS = aVBS(k);
    aSolidsIm.Add(aBS.Solid(), aBS.Areas());

    // Merge BuilderSolid's report into main report,
    // assigning the solid with the warnings/errors which
    // have been generated for it.
    // Convert all errors of BuilderSolid into warnings for main report.
    const occ::handle<Message_Report>& aBSReport       = aBS.GetReport();
    Message_Gravity                    anAlertTypes[2] = {Message_Warning, Message_Fail};
    for (int iGravity = 0; iGravity < 2; iGravity++)
    {
      const NCollection_List<occ::handle<Message_Alert>>& anLAlerts =
        aBSReport->GetAlerts(anAlertTypes[iGravity]);
      for (NCollection_List<occ::handle<Message_Alert>>::Iterator itA(anLAlerts); itA.More();
           itA.Next())
      {
        occ::handle<Message_Alert> anAlert = itA.Value();

        occ::handle<TopoDS_AlertWithShape> anAlertWithShape =
          occ::down_cast<TopoDS_AlertWithShape>(itA.Value());
        if (!anAlertWithShape.IsNull())
        {
          TopoDS_Shape aWarnShape;
          BRep_Builder().MakeCompound(TopoDS::Compound(aWarnShape));
          BRep_Builder().Add(aWarnShape, aBS.Solid());
          BRep_Builder().Add(aWarnShape, anAlertWithShape->GetShape());

          anAlertWithShape->SetShape(aWarnShape);
          AddWarning(anAlertWithShape);
        }
        else
          AddWarning(anAlert);
      }
    }
  }
  //
  // Add new solids to images map
  aNbBS = aSolidsIm.Extent();
  for (k = 1; k <= aNbBS; ++k)
  {
    const TopoDS_Shape&                   aS   = aSolidsIm.FindKey(k);
    const NCollection_List<TopoDS_Shape>& aLSR = aSolidsIm(k);
    //
    if (!myImages.IsBound(aS))
    {
      NCollection_List<TopoDS_Shape>* pLSx = myImages.Bound(aS, NCollection_List<TopoDS_Shape>());
      //
      aIt.Initialize(aLSR);
      for (; aIt.More(); aIt.Next())
      {
        BOPTools_Set aST;
        //
        const TopoDS_Shape& aSR = aIt.Value();
        aST.Add(aSR, TopAbs_FACE);
        //
        bFlagSD = aMST.Contains(aST);
        //
        const BOPTools_Set& aSTx = aMST.Added(aST);
        const TopoDS_Shape& aSx  = aSTx.Shape();
        pLSx->Append(aSx);
        //
        NCollection_List<TopoDS_Shape>* pLOr = myOrigins.ChangeSeek(aSx);
        if (!pLOr)
        {
          pLOr = myOrigins.Bound(aSx, NCollection_List<TopoDS_Shape>());
        }
        pLOr->Append(aS);
        //
        if (bFlagSD)
        {
          myShapesSD.Bind(aSR, aSx);
        }
      }
    }
  }
}

//=================================================================================================

void BOPAlgo_Builder::FillInternalShapes(const Message_ProgressRange& theRange)
{
  int                                      i, j, aNbS, aNbSI, aNbSx;
  TopAbs_ShapeEnum                         aType;
  TopAbs_State                             aState;
  TopoDS_Iterator                          aItS;
  BRep_Builder                             aBB;
  NCollection_List<TopoDS_Shape>::Iterator aIt, aIt1;
  //
  occ::handle<NCollection_BaseAllocator> aAllocator;
  //-----------------------------------------------------scope f
  aAllocator = NCollection_BaseAllocator::CommonBaseAllocator();
  //
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                                aMSx(100, aAllocator);
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMx(100, aAllocator);
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMSI(100, aAllocator);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>        aMFence(100, aAllocator);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>        aMSOr(100, aAllocator);
  NCollection_List<TopoDS_Shape>                                aLSd(aAllocator);
  NCollection_List<TopoDS_Shape>                                aLArgs(aAllocator);
  NCollection_List<TopoDS_Shape>                                aLSC(aAllocator);
  NCollection_List<TopoDS_Shape>                                aLSI(aAllocator);

  Message_ProgressScope aPS(theRange, nullptr, 10);
  //
  // 1. Shapes to process
  //
  // 1.1 Shapes from pure arguments aMSI
  // 1.1.1 vertex, edge, wire
  //
  const NCollection_List<TopoDS_Shape>& aArguments = myDS->Arguments();
  aIt.Initialize(aArguments);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();
    BOPTools_AlgoTools::TreatCompound(aS, aLSC, &aMFence);
  }
  aIt.Initialize(aLSC);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();
    aType                  = aS.ShapeType();
    if (aType == TopAbs_WIRE)
    {
      aItS.Initialize(aS);
      for (; aItS.More(); aItS.Next())
      {
        const TopoDS_Shape& aE = aItS.Value();
        if (aMFence.Add(aE))
        {
          aLArgs.Append(aE);
        }
      }
    }
    else if (aType == TopAbs_VERTEX || aType == TopAbs_EDGE)
    {
      aLArgs.Append(aS);
    }
  }
  aMFence.Clear();
  //
  aIt.Initialize(aLArgs);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();
    aType                  = aS.ShapeType();
    if (aType == TopAbs_VERTEX || aType == TopAbs_EDGE || aType == TopAbs_WIRE)
    {
      if (aMFence.Add(aS))
      {
        if (myImages.IsBound(aS))
        {
          const NCollection_List<TopoDS_Shape>& aLSp = myImages.Find(aS);
          aIt1.Initialize(aLSp);
          for (; aIt1.More(); aIt1.Next())
          {
            const TopoDS_Shape& aSp = aIt1.Value();
            aMSI.Add(aSp);
          }
        }
        else
        {
          aMSI.Add(aS);
        }
      }
    }
  }
  if (UserBreak(aPS))
  {
    return;
  }

  aNbSI = aMSI.Extent();
  //
  // 2. Internal vertices, edges from source solids
  aMFence.Clear();
  aLSd.Clear();
  //
  aNbS = myDS->NbSourceShapes();
  for (i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    //
    if (aSI.ShapeType() != TopAbs_SOLID)
    {
      continue;
    }
    if (UserBreak(aPS))
    {
      return;
    }
    //
    const TopoDS_Shape& aS = aSI.Shape();
    //
    aMx.Clear();
    OwnInternalShapes(aS, aMx);
    //
    aNbSx = aMx.Extent();
    for (j = 1; j <= aNbSx; ++j)
    {
      const TopoDS_Shape& aSi = aMx(j);
      if (myImages.IsBound(aSi))
      {
        const NCollection_List<TopoDS_Shape>& aLSp = myImages.Find(aSi);
        aIt1.Initialize(aLSp);
        for (; aIt1.More(); aIt1.Next())
        {
          const TopoDS_Shape& aSp = aIt1.Value();
          aMSI.Add(aSp);
        }
      }
      else
      {
        aMSI.Add(aSi);
      }
    }
    //
    // build aux map from splits of solids
    if (myImages.IsBound(aS))
    {
      const NCollection_List<TopoDS_Shape>& aLSp = myImages.Find(aS);
      aIt.Initialize(aLSp);
      for (; aIt.More(); aIt.Next())
      {
        const TopoDS_Shape& aSp = aIt.Value();
        if (aMFence.Add(aSp))
        {
          TopExp::MapShapesAndAncestors(aSp, TopAbs_VERTEX, TopAbs_EDGE, aMSx);
          TopExp::MapShapesAndAncestors(aSp, TopAbs_VERTEX, TopAbs_FACE, aMSx);
          TopExp::MapShapesAndAncestors(aSp, TopAbs_EDGE, TopAbs_FACE, aMSx);
          aLSd.Append(aSp);
        }
      }
    }
    else
    {
      if (aMFence.Add(aS))
      {
        TopExp::MapShapesAndAncestors(aS, TopAbs_VERTEX, TopAbs_EDGE, aMSx);
        TopExp::MapShapesAndAncestors(aS, TopAbs_VERTEX, TopAbs_FACE, aMSx);
        TopExp::MapShapesAndAncestors(aS, TopAbs_EDGE, TopAbs_FACE, aMSx);
        aLSd.Append(aS);
        aMSOr.Add(aS);
      }
    }
  } // for (i=0; i<aNbS; ++i) {
  //
  // 3. Some shapes of aMSI can be already tied with faces of
  //    split solids
  aNbSI = aMSI.Extent();
  for (i = 1; i <= aNbSI; ++i)
  {
    const TopoDS_Shape& aSI = aMSI(i);
    if (aMSx.Contains(aSI))
    {
      const NCollection_List<TopoDS_Shape>& aLSx = aMSx.FindFromKey(aSI);
      aNbSx                                      = aLSx.Extent();
      if (!aNbSx)
      {
        aLSI.Append(aSI);
      }
    }
    else
    {
      aLSI.Append(aSI);
    }
  }
  //
  // 4. Just check it
  aNbSI = aLSI.Extent();
  if (!aNbSI)
  {
    return;
  }

  aPS.Next();
  //
  // 5 Settle internal vertices and edges into solids
  aMx.Clear();

  Message_ProgressScope aPSLoop(aPS.Next(9), "Looking for internal shapes", aLSd.Size());

  aIt.Initialize(aLSd);
  for (; aIt.More(); aIt.Next(), aPSLoop.Next())
  {
    TopoDS_Solid aSd = TopoDS::Solid(aIt.Value());
    //
    aIt1.Initialize(aLSI);
    for (; aIt1.More();)
    {
      TopoDS_Shape aSI = aIt1.Value();
      aSI.Orientation(TopAbs_INTERNAL);
      //
      aState = BOPTools_AlgoTools::ComputeStateByOnePoint(aSI, aSd, 1.e-11, myContext);
      //
      if (aState != TopAbs_IN)
      {
        aIt1.Next();
        continue;
      }
      //
      if (aMSOr.Contains(aSd))
      {
        // make new solid
        TopoDS_Solid aSdx;
        //
        aBB.MakeSolid(aSdx);
        aItS.Initialize(aSd);
        for (; aItS.More(); aItS.Next())
        {
          const TopoDS_Shape& aSh = aItS.Value();
          aBB.Add(aSdx, aSh);
        }
        //
        aBB.Add(aSdx, aSI);
        //
        // no need to check for images of aSd as aMSOr contains only original solids
        NCollection_List<TopoDS_Shape>* pLS = myImages.Bound(aSd, NCollection_List<TopoDS_Shape>());
        pLS->Append(aSdx);
        //
        NCollection_List<TopoDS_Shape>* pLOr =
          myOrigins.Bound(aSdx, NCollection_List<TopoDS_Shape>());
        pLOr->Append(aSd);
        //
        aMSOr.Remove(aSd);
        aSd = aSdx;
      }
      else
      {
        aBB.Add(aSd, aSI);
      }
      //
      aLSI.Remove(aIt1);
    } // for (; aIt1.More();) {
  } // for (; aIt.More(); aIt.Next()) {
  //
  //-----------------------------------------------------scope t
  aLArgs.Clear();
  aLSd.Clear();
  aMSOr.Clear();
  aMFence.Clear();
  aMSI.Clear();
  aMx.Clear();
  aMSx.Clear();
}

//=================================================================================================

void OwnInternalShapes(const TopoDS_Shape&                                            theS,
                       NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMx)
{
  TopoDS_Iterator aIt;
  //
  aIt.Initialize(theS);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aSx = aIt.Value();
    if (aSx.ShapeType() != TopAbs_SHELL)
    {
      theMx.Add(aSx);
    }
  }
}
