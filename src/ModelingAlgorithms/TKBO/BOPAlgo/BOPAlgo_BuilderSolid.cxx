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
#include <BOPAlgo_BuilderSolid.hxx>
#include <BOPAlgo_ShellSplitter.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_BoxTree.hxx>
#include <BOPTools_Parallel.hxx>
#include <Bnd_Tools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
#include <TopTools_MapOfShape.hxx>

//
static Standard_Boolean IsGrowthShell(const TopoDS_Shape&, const TopTools_IndexedMapOfShape&);
static Standard_Boolean IsHole(const TopoDS_Shape&, Handle(IntTools_Context)&);
static Standard_Boolean IsInside(const TopoDS_Shape&,
                                 const TopoDS_Shape&,
                                 Handle(IntTools_Context)&);
static void MakeInternalShells(const TopTools_IndexedMapOfShape&, TopTools_ListOfShape&);

//=================================================================================================

BOPAlgo_BuilderSolid::BOPAlgo_BuilderSolid()
    : BOPAlgo_BuilderArea()
{
}

//=================================================================================================

BOPAlgo_BuilderSolid::BOPAlgo_BuilderSolid(const Handle(NCollection_BaseAllocator)& theAllocator)
    : BOPAlgo_BuilderArea(theAllocator)
{
}

//=================================================================================================

BOPAlgo_BuilderSolid::~BOPAlgo_BuilderSolid() {}

//=================================================================================================

void BOPAlgo_BuilderSolid::Perform(const Message_ProgressRange& theRange)
{
  Message_ProgressScope aPS(theRange, NULL, 100);

  GetReport()->Clear();
  //
  if (myShapes.IsEmpty())
    return;

  if (myContext.IsNull())
  {
    myContext = new IntTools_Context;
  }

  myBoxes.Clear();

  TopoDS_Compound                    aC;
  BRep_Builder                       aBB;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aBB.MakeCompound(aC);
  aIt.Initialize(myShapes);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aF = aIt.Value();
    aBB.Add(aC, aF);
  }
  //
  PerformShapesToAvoid(aPS.Next(1));
  if (HasErrors())
  {
    return;
  }
  //
  PerformLoops(aPS.Next(10));
  if (HasErrors())
  {
    return;
  }
  //
  PerformAreas(aPS.Next(80));
  if (HasErrors())
  {
    return;
  }
  //
  PerformInternalShapes(aPS.Next(9));
}

//=================================================================================================

void BOPAlgo_BuilderSolid::PerformShapesToAvoid(const Message_ProgressRange& theRange)
{
  Standard_Boolean                          bFound;
  Standard_Integer                          i, aNbE, aNbF;
  TopAbs_Orientation                        aOrE;
  TopTools_IndexedDataMapOfShapeListOfShape aMEF;
  TopTools_ListIteratorOfListOfShape        aIt;
  //
  myShapesToAvoid.Clear();
  //
  Message_ProgressScope aPS(theRange, NULL, 1);
  //
  for (;;)
  {
    if (UserBreak(aPS))
    {
      return;
    }
    bFound = Standard_False;
    //
    // 1. MEF
    aMEF.Clear();
    aIt.Initialize(myShapes);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aF = aIt.Value();
      if (!myShapesToAvoid.Contains(aF))
      {
        TopExp::MapShapesAndAncestors(aF, TopAbs_EDGE, TopAbs_FACE, aMEF);
      }
    }
    aNbE = aMEF.Extent();
    //
    // 2. myFacesToAvoid
    for (i = 1; i <= aNbE; ++i)
    {
      const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aMEF.FindKey(i)));
      if (BRep_Tool::Degenerated(aE))
      {
        continue;
      }
      //
      TopTools_ListOfShape& aLF = aMEF.ChangeFromKey(aE);
      aNbF                      = aLF.Extent();
      if (!aNbF)
      {
        continue;
      }
      //
      aOrE = aE.Orientation();
      //
      const TopoDS_Face& aF1 = (*(TopoDS_Face*)(&aLF.First()));
      if (aNbF == 1)
      {
        if (aOrE == TopAbs_INTERNAL)
        {
          continue;
        }
        bFound = Standard_True;
        myShapesToAvoid.Add(aF1);
      }
      else if (aNbF == 2)
      {
        const TopoDS_Face& aF2 = (*(TopoDS_Face*)(&aLF.Last()));
        if (aF2.IsSame(aF1))
        {
          if (BRep_Tool::IsClosed(aE, aF1))
          {
            continue;
          }
          //
          if (aOrE == TopAbs_INTERNAL)
          {
            continue;
          }
          //
          bFound = Standard_True;
          myShapesToAvoid.Add(aF1);
          myShapesToAvoid.Add(aF2);
        }
      }
    } // for (i=1; i<=aNbE; ++i) {
    //
    if (!bFound)
    {
      break;
    }
    //
  } // for(;;) {
}

//=================================================================================================

void BOPAlgo_BuilderSolid::PerformLoops(const Message_ProgressRange& theRange)
{
  Standard_Integer                   i, aNbSh;
  TopTools_ListIteratorOfListOfShape aIt;
  TopoDS_Iterator                    aItS;
  Handle(NCollection_BaseAllocator)  aAlr;
  //
  myLoops.Clear();
  //
  aAlr = NCollection_BaseAllocator::CommonBaseAllocator();
  BOPAlgo_ShellSplitter aSSp(aAlr);
  //
  Message_ProgressScope aMainScope(theRange, "Building shells", 10);

  // 1. Shells Usual
  aIt.Initialize(myShapes);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Face& aF = *((TopoDS_Face*)&aIt.Value());
    if (myContext->IsInfiniteFace(aF))
    {
      TopoDS_Shell aSh;
      BRep_Builder aBB;
      //
      aBB.MakeShell(aSh);
      aBB.Add(aSh, aF);
      myLoops.Append(aSh);
      continue;
    }
    //
    if (!myShapesToAvoid.Contains(aF))
    {
      aSSp.AddStartElement(aF);
    }
  }
  //
  aSSp.SetRunParallel(myRunParallel);
  aSSp.Perform(aMainScope.Next(9));
  if (aSSp.HasErrors())
  {
    // add warning status
    if (aMainScope.More())
    {
      TopoDS_Compound aFacesSp;
      BRep_Builder().MakeCompound(aFacesSp);
      TopTools_ListIteratorOfListOfShape aItLF(aSSp.StartElements());
      for (; aItLF.More(); aItLF.Next())
      {
        BRep_Builder().Add(aFacesSp, aItLF.Value());
      }
      AddWarning(new BOPAlgo_AlertShellSplitterFailed(aFacesSp));
    }
    return;
  }
  //
  const TopTools_ListOfShape& aLSh = aSSp.Shells();
  aIt.Initialize(aLSh);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aSh = aIt.Value();
    myLoops.Append(aSh);
  }
  //=================================================
  //
  // 2. Post Treatment
  BRep_Builder                              aBB;
  TopTools_MapOfOrientedShape               AddedFacesMap;
  TopTools_IndexedDataMapOfShapeListOfShape aEFMap;
  TopTools_MapOfOrientedShape               aMP;
  //
  // a. collect all edges that are in loops
  aIt.Initialize(myLoops);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();
    aItS.Initialize(aS);
    for (; aItS.More(); aItS.Next())
    {
      const TopoDS_Shape& aF = aItS.Value();
      aMP.Add(aF);
    }
  }
  if (UserBreak(aMainScope))
  {
    return;
  }
  //
  // b. collect all edges that are to avoid
  aNbSh = myShapesToAvoid.Extent();
  for (i = 1; i <= aNbSh; ++i)
  {
    const TopoDS_Shape& aF = myShapesToAvoid(i);
    aMP.Add(aF);
  }
  //
  // c. add all edges that are not processed to myShapesToAvoid
  aIt.Initialize(myShapes);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Face& aF = *((TopoDS_Face*)&aIt.Value());
    if (!myContext->IsInfiniteFace(aF))
    {
      if (!aMP.Contains(aF))
      {
        myShapesToAvoid.Add(aF);
      }
    }
  }
  //=================================================
  if (UserBreak(aMainScope))
  {
    return;
  }
  //
  // 3.Internal Shells
  myLoopsInternal.Clear();
  //
  aEFMap.Clear();
  AddedFacesMap.Clear();
  //
  aNbSh = myShapesToAvoid.Extent();
  for (i = 1; i <= aNbSh; ++i)
  {
    const TopoDS_Shape& aFF = myShapesToAvoid(i);
    TopExp::MapShapesAndAncestors(aFF, TopAbs_EDGE, TopAbs_FACE, aEFMap);
  }
  //
  for (i = 1; i <= aNbSh; ++i)
  {
    if (UserBreak(aMainScope))
    {
      return;
    }
    const TopoDS_Shape& aFF = myShapesToAvoid(i);
    if (!AddedFacesMap.Add(aFF))
    {
      continue;
    }
    //
    // make a new shell
    TopExp_Explorer aExp;
    TopoDS_Shell    aShell;
    aBB.MakeShell(aShell);
    aBB.Add(aShell, aFF);
    //
    aItS.Initialize(aShell);
    for (; aItS.More(); aItS.Next())
    {
      const TopoDS_Face& aF = (*(TopoDS_Face*)(&aItS.Value()));
      //
      aExp.Init(aF, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Edge&          aE  = (*(TopoDS_Edge*)(&aExp.Current()));
        const TopTools_ListOfShape& aLF = aEFMap.FindFromKey(aE);
        aIt.Initialize(aLF);
        for (; aIt.More(); aIt.Next())
        {
          const TopoDS_Face& aFL = (*(TopoDS_Face*)(&aIt.Value()));
          if (AddedFacesMap.Add(aFL))
          {
            aBB.Add(aShell, aFL);
          }
        }
      }
    }
    aShell.Closed(BRep_Tool::IsClosed(aShell));
    myLoopsInternal.Append(aShell);
  }
}

//=================================================================================================

void BOPAlgo_BuilderSolid::PerformAreas(const Message_ProgressRange& theRange)
{
  myAreas.Clear();
  BRep_Builder aBB;
  // The new solids
  TopTools_ListOfShape aNewSolids;
  // The hole shells which has to be classified relatively new solids
  TopTools_IndexedMapOfShape aHoleShells;
  // Map of the faces of the hole shells for quick check of the growths.
  // If the analyzed shell contains any of the hole faces, it is considered as growth.
  TopTools_IndexedMapOfShape aMHF;

  Message_ProgressScope aMainScope(theRange, "Building solids", 10);

  // Analyze the shells
  Message_ProgressScope aPSClass(aMainScope.Next(5), "Classify solids", myLoops.Size());
  TopTools_ListIteratorOfListOfShape aItLL(myLoops);
  for (; aItLL.More(); aItLL.Next(), aPSClass.Next())
  {
    if (UserBreak(aPSClass))
    {
      return;
    }
    const TopoDS_Shape& aShell = aItLL.Value();

    Standard_Boolean bIsGrowth = IsGrowthShell(aShell, aMHF);
    if (!bIsGrowth)
    {
      // Fast check did not give the result, run classification
      bIsGrowth = !IsHole(aShell, myContext);
    }

    // Save the solid
    if (bIsGrowth)
    {
      TopoDS_Solid aSolid;
      aBB.MakeSolid(aSolid);
      aBB.Add(aSolid, aShell);
      aNewSolids.Append(aSolid);
    }
    else
    {
      aHoleShells.Add(aShell);
      TopExp::MapShapes(aShell, TopAbs_FACE, aMHF);
    }
  }

  if (aHoleShells.IsEmpty())
  {
    // No holes, stop the analysis
    TopTools_ListIteratorOfListOfShape aItLS(aNewSolids);
    for (; aItLS.More(); aItLS.Next())
    {
      const TopoDS_Shape& aSol = aItLS.Value();
      myAreas.Append(aSol);
      // Build box
      Bnd_Box aBox;
      BRepBndLib::Add(aSol, aBox);
      myBoxes.Bind(aSol, aBox);
    }
    return;
  }

  // Classify holes relatively solids

  // Prepare tree with the boxes of the hole shells
  BOPTools_BoxTree aBBTree;
  Standard_Integer i, aNbH = aHoleShells.Extent();
  aBBTree.SetSize(aNbH);
  for (i = 1; i <= aNbH; ++i)
  {
    const TopoDS_Shape& aHShell = aHoleShells(i);
    //
    Bnd_Box aBox;
    BRepBndLib::Add(aHShell, aBox);
    aBBTree.Add(i, Bnd_Tools::Bnd2BVH(aBox));

    myBoxes.Bind(aHShell, aBox);
  }

  // Build BVH
  aBBTree.Build();

  // Find outer growth shell that is most close to each hole shell
  TopTools_IndexedDataMapOfShapeShape aHoleSolidMap;

  Message_ProgressScope              aPSH(aMainScope.Next(4), "Adding holes", aNewSolids.Size());
  TopTools_ListIteratorOfListOfShape aItLS(aNewSolids);
  for (; aItLS.More(); aItLS.Next(), aPSH.Next())
  {
    if (UserBreak(aPSH))
    {
      return;
    }
    const TopoDS_Shape& aSolid = aItLS.Value();

    // Build box
    Bnd_Box aBox;
    BRepBndLib::Add(aSolid, aBox);

    myBoxes.Bind(aSolid, aBox);

    BOPTools_BoxTreeSelector aSelector;
    aSelector.SetBox(Bnd_Tools::Bnd2BVH(aBox));
    aSelector.SetBVHSet(&aBBTree);
    aSelector.Select();

    const TColStd_ListOfInteger&        aLI = aSelector.Indices();
    TColStd_ListIteratorOfListOfInteger aItLI(aLI);
    for (; aItLI.More(); aItLI.Next())
    {
      Standard_Integer    k     = aItLI.Value();
      const TopoDS_Shape& aHole = aHoleShells(k);
      // Check if it is inside
      if (!IsInside(aHole, aSolid, myContext))
        continue;

      // Save the relation
      TopoDS_Shape* pSolidWas = aHoleSolidMap.ChangeSeek(aHole);
      if (pSolidWas)
      {
        if (IsInside(aSolid, *pSolidWas, myContext))
        {
          *pSolidWas = aSolid;
        }
      }
      else
      {
        aHoleSolidMap.Add(aHole, aSolid);
      }
    }
  }

  // Make the back map from solids to holes
  TopTools_IndexedDataMapOfShapeListOfShape aSolidHolesMap;

  aNbH = aHoleSolidMap.Extent();
  for (i = 1; i <= aNbH; ++i)
  {
    const TopoDS_Shape& aHole  = aHoleSolidMap.FindKey(i);
    const TopoDS_Shape& aSolid = aHoleSolidMap(i);
    //
    TopTools_ListOfShape* pLHoles = aSolidHolesMap.ChangeSeek(aSolid);
    if (!pLHoles)
      pLHoles = &aSolidHolesMap(aSolidHolesMap.Add(aSolid, TopTools_ListOfShape()));
    pLHoles->Append(aHole);
  }

  // Add Holes to Solids and add them to myAreas
  Message_ProgressScope aPSU(aMainScope.Next(), NULL, aNewSolids.Size());
  aItLS.Initialize(aNewSolids);
  for (; aItLS.More(); aItLS.Next(), aPSU.Next())
  {
    if (UserBreak(aPSU))
    {
      return;
    }
    TopoDS_Solid&               aSolid  = *(TopoDS_Solid*)&aItLS.Value();
    const TopTools_ListOfShape* pLHoles = aSolidHolesMap.Seek(aSolid);
    if (pLHoles)
    {
      // update solid
      TopTools_ListIteratorOfListOfShape aItLH(*pLHoles);
      for (; aItLH.More(); aItLH.Next())
      {
        const TopoDS_Shape& aHole = aItLH.Value();
        aBB.Add(aSolid, aHole);
      }

      // update classifier
      myContext->SolidClassifier(aSolid).Load(aSolid);
    }

    myAreas.Append(aSolid);
  }

  // Add holes that outside the solids to myAreas
  aNbH = aHoleShells.Extent();
  for (i = 1; i <= aNbH; ++i)
  {
    const TopoDS_Shape& aHole = aHoleShells(i);
    if (!aHoleSolidMap.Contains(aHole))
    {
      TopoDS_Solid aSolid;
      aBB.MakeSolid(aSolid);
      aBB.Add(aSolid, aHole);
      //
      myAreas.Append(aSolid);
      // Make an infinite box for the hole
      Bnd_Box aBox;
      aBox.SetWhole();
      myBoxes.Bind(aSolid, aBox);
    }

    myBoxes.UnBind(aHole);
  }
}

//=================================================================================================

void BOPAlgo_BuilderSolid::PerformInternalShapes(const Message_ProgressRange& theRange)
{
  if (myAvoidInternalShapes)
    // user-defined option to avoid internal parts is in force
    return;

  if (myLoopsInternal.IsEmpty())
    // no internal parts
    return;

  Message_ProgressScope aMainScope(theRange, "Adding internal shapes", 2);

  // Get all faces to classify
  TopTools_IndexedMapOfShape         aMFs;
  TopTools_ListIteratorOfListOfShape aItLS(myLoopsInternal);
  for (; aItLS.More(); aItLS.Next())
  {
    const TopoDS_Shape& aShell = aItLS.Value();
    TopoDS_Iterator     aIt(aShell);
    for (; aIt.More(); aIt.Next())
      aMFs.Add(aIt.Value());
  }

  BRep_Builder aBB;
  // Check existence of the growths solids
  if (myAreas.IsEmpty())
  {
    // No areas.
    // Just make solid of the faces
    TopoDS_Solid aSolid;
    aBB.MakeSolid(aSolid);
    //
    TopTools_ListOfShape aLSI;
    MakeInternalShells(aMFs, aLSI);
    //
    aItLS.Initialize(aLSI);
    for (; aItLS.More(); aItLS.Next())
      aBB.Add(aSolid, aItLS.Value());

    myAreas.Append(aSolid);
    return;
  }

  if (UserBreak(aMainScope))
  {
    return;
  }

  // Classify faces relatively solids

  // Prepare list of faces to classify
  TopTools_ListOfShape aLFaces;
  Standard_Integer     i, aNbF = aMFs.Extent();
  for (i = 1; i <= aNbF; ++i)
    aLFaces.Append(aMFs(i));

  // Map of solids with IN faces
  TopTools_IndexedDataMapOfShapeListOfShape aMSLF;

  // Perform classification
  BOPAlgo_Tools::ClassifyFaces(aLFaces,
                               myAreas,
                               myRunParallel,
                               myContext,
                               aMSLF,
                               myBoxes,
                               TopTools_DataMapOfShapeListOfShape(),
                               aMainScope.Next());

  // Update Solids by internal Faces

  TopTools_MapOfShape aMFDone;

  Standard_Integer      aNbS = aMSLF.Extent();
  Message_ProgressScope aPSLoop(aMainScope.Next(), NULL, aNbS);
  for (i = 1; i <= aNbS; ++i, aPSLoop.Next())
  {
    if (UserBreak(aPSLoop))
    {
      return;
    }
    const TopoDS_Shape& aSolid = aMSLF.FindKey(i);
    TopoDS_Shape*       pSolid = (TopoDS_Shape*)&aSolid;

    const TopTools_ListOfShape& aLF = aMSLF(i);
    if (aLF.IsEmpty())
      continue;

    TopTools_IndexedMapOfShape aMF;
    aItLS.Initialize(aLF);
    for (; aItLS.More(); aItLS.Next())
    {
      const TopoDS_Shape& aF = aItLS.Value();
      aMF.Add(aF);
      aMFDone.Add(aF);
    }
    //
    TopTools_ListOfShape aLSI;
    MakeInternalShells(aMF, aLSI);
    //
    aItLS.Initialize(aLSI);
    for (; aItLS.More(); aItLS.Next())
    {
      const TopoDS_Shape& aSI = aItLS.Value();
      aBB.Add(*pSolid, aSI);
    }
  }

  // Find all unclassified faces and warn the user about them.
  // Do not put such faces into result as they will form not closed solid.
  TopTools_IndexedMapOfShape aMFUnUsed;

  for (i = 1; i <= aNbF; ++i)
  {
    const TopoDS_Shape& aF = aMFs(i);
    if (!aMFDone.Contains(aF))
      aMFUnUsed.Add(aF);
  }

  if (aMFUnUsed.Extent())
  {
    TopTools_ListOfShape aLSI;
    MakeInternalShells(aMFUnUsed, aLSI);

    TopoDS_Shape aWShape;
    if (aLSI.Extent() == 1)
      aWShape = aLSI.First();
    else
    {
      aBB.MakeCompound(TopoDS::Compound(aWShape));
      aItLS.Initialize(aLSI);
      for (; aItLS.More(); aItLS.Next())
        aBB.Add(aWShape, aItLS.Value());
    }

    AddWarning(new BOPAlgo_AlertSolidBuilderUnusedFaces(aWShape));
  }
}

//=================================================================================================

void MakeInternalShells(const TopTools_IndexedMapOfShape& theMF, TopTools_ListOfShape& theShells)
{
  Standard_Integer                          i, aNbF;
  BRep_Builder                              aBB;
  TopTools_ListIteratorOfListOfShape        aItF;
  TopTools_IndexedDataMapOfShapeListOfShape aMEF;
  TopTools_MapOfShape                       aAddedFacesMap;
  //
  aNbF = theMF.Extent();
  for (i = 1; i <= aNbF; ++i)
  {
    const TopoDS_Shape& aF = theMF(i);
    TopExp::MapShapesAndAncestors(aF, TopAbs_EDGE, TopAbs_FACE, aMEF);
  }
  //
  for (i = 1; i <= aNbF; ++i)
  {
    TopoDS_Shape aFF = theMF(i);
    if (!aAddedFacesMap.Add(aFF))
    {
      continue;
    }
    //
    // make a new shell
    TopoDS_Shell aShell;
    aBB.MakeShell(aShell);
    aFF.Orientation(TopAbs_INTERNAL);
    aBB.Add(aShell, aFF);
    //
    TopoDS_Iterator aItAddedF(aShell);
    for (; aItAddedF.More(); aItAddedF.Next())
    {
      const TopoDS_Shape& aF = aItAddedF.Value();
      //
      TopExp_Explorer aEdgeExp(aF, TopAbs_EDGE);
      for (; aEdgeExp.More(); aEdgeExp.Next())
      {
        const TopoDS_Shape&         aE  = aEdgeExp.Current();
        const TopTools_ListOfShape& aLF = aMEF.FindFromKey(aE);
        aItF.Initialize(aLF);
        for (; aItF.More(); aItF.Next())
        {
          TopoDS_Shape aFL = aItF.Value();
          if (aAddedFacesMap.Add(aFL))
          {
            aFL.Orientation(TopAbs_INTERNAL);
            aBB.Add(aShell, aFL);
          }
        }
      }
    }
    aShell.Closed(BRep_Tool::IsClosed(aShell));
    theShells.Append(aShell);
  }
}

//=================================================================================================

Standard_Boolean IsHole(const TopoDS_Shape& theS2, Handle(IntTools_Context)& theContext)
{
  TopoDS_Solid*                pS2   = (TopoDS_Solid*)&theS2;
  BRepClass3d_SolidClassifier& aClsf = theContext->SolidClassifier(*pS2);
  //
  aClsf.PerformInfinitePoint(::RealSmall());
  //
  return (aClsf.State() == TopAbs_IN);
}

//=================================================================================================

Standard_Boolean IsInside(const TopoDS_Shape&       theS1,
                          const TopoDS_Shape&       theS2,
                          Handle(IntTools_Context)& theContext)
{
  TopExp_Explorer aExp;
  TopAbs_State    aState;
  //
  TopoDS_Solid* pS2 = (TopoDS_Solid*)&theS2;
  //
  aExp.Init(theS1, TopAbs_FACE);
  if (!aExp.More())
  {
    BRepClass3d_SolidClassifier& aClsf = theContext->SolidClassifier(*pS2);
    aClsf.PerformInfinitePoint(::RealSmall());
    aState = aClsf.State();
  }
  else
  {
    TopTools_IndexedMapOfShape aBounds;
    TopExp::MapShapes(*pS2, TopAbs_EDGE, aBounds);
    const TopoDS_Face& aF = (*(TopoDS_Face*)(&aExp.Current()));
    aState =
      BOPTools_AlgoTools::ComputeState(aF, *pS2, Precision::Confusion(), aBounds, theContext);
  }
  return (aState == TopAbs_IN);
}

//=================================================================================================

Standard_Boolean IsGrowthShell(const TopoDS_Shape&               theShell,
                               const TopTools_IndexedMapOfShape& theMHF)
{
  if (theMHF.Extent())
  {
    TopoDS_Iterator aIt(theShell);
    for (; aIt.More(); aIt.Next())
    {
      if (theMHF.Contains(aIt.Value()))
        return Standard_True;
    }
  }
  return Standard_False;
}
