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
#include <BOPCol_BoxBndTree.hxx>
#include <BOPCol_DataMapOfShapeListOfShape.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeShape.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfOrientedShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_CoupleOfShape.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <TColStd_MapIntegerHasher.hxx>
#include <TopAbs.hxx>
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
#include <TopoDS_Vertex.hxx>

//
static
  Standard_Boolean IsGrowthShell(const TopoDS_Shape& ,
                                 const BOPCol_IndexedMapOfShape& );
static
  Standard_Boolean IsHole(const TopoDS_Shape& ,
                          Handle(IntTools_Context)& );
static
  Standard_Boolean IsInside(const TopoDS_Shape& ,
                            const TopoDS_Shape& ,
                            Handle(IntTools_Context)& );
static
  void MakeInternalShells(const BOPCol_IndexedMapOfShape& ,
                          BOPCol_ListOfShape& );

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_BuilderSolid::BOPAlgo_BuilderSolid()
:
  BOPAlgo_BuilderArea()
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_BuilderSolid::BOPAlgo_BuilderSolid
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_BuilderArea(theAllocator)
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_BuilderSolid::~BOPAlgo_BuilderSolid()
{
}
//=======================================================================
//function : SetSolid 
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::SetSolid(const TopoDS_Solid& aS)
{
  mySolid=aS;
}
//=======================================================================
//function : Solid 
//purpose  : 
//=======================================================================
const TopoDS_Solid& BOPAlgo_BuilderSolid::Solid()const
{
  return mySolid;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::Perform()
{
  GetReport()->Clear();
  //
  if (myShapes.IsEmpty())
    return;

  if (myContext.IsNull()) {
    myContext=new IntTools_Context;
  }

  myBoxes.Clear();

  TopoDS_Compound aC;
  BRep_Builder aBB;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  aBB.MakeCompound(aC);
  aIt.Initialize(myShapes);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF=aIt.Value();
    aBB.Add(aC, aF);
  }
  //
  UserBreak();
  //
  PerformShapesToAvoid();
  if (HasErrors()) {
    return;
  }
  //
  UserBreak();
  //
  PerformLoops();
  if (HasErrors()) {
    return;
  }
  //
  UserBreak();
  //
  PerformAreas();
  if (HasErrors()) {
    return;
  }
  //
  UserBreak();
  //
  PerformInternalShapes();
  if (HasErrors()) {
    return;
  }
}
//=======================================================================
//function :PerformShapesToAvoid
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::PerformShapesToAvoid()
{
  Standard_Boolean bFound;
  Standard_Integer i, iCnt, aNbE, aNbF;
  TopAbs_Orientation aOrE;
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  myShapesToAvoid.Clear();
  //
  iCnt=0;
  for(;;) {
    ++iCnt;
    bFound=Standard_False;
    //
    // 1. MEF
    aMEF.Clear();
    aIt.Initialize (myShapes);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aF=aIt.Value();
      if (!myShapesToAvoid.Contains(aF)) {
        BOPTools::MapShapesAndAncestors(aF, 
                                        TopAbs_EDGE, 
                                        TopAbs_FACE, 
                                        aMEF);
      }
    }
    aNbE=aMEF.Extent();
    //
    // 2. myFacesToAvoid
    for (i=1; i<=aNbE; ++i) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&aMEF.FindKey(i)));
      if (BRep_Tool::Degenerated(aE)) {
        continue;
      }
      //
      BOPCol_ListOfShape& aLF=aMEF.ChangeFromKey(aE);
      aNbF=aLF.Extent();
      if (!aNbF) {
        continue;
      }
      //
      aOrE=aE.Orientation();
      //
      const TopoDS_Face& aF1=(*(TopoDS_Face*)(&aLF.First()));
      if (aNbF==1) {
        if (aOrE==TopAbs_INTERNAL) {
          continue;
        }
        bFound=Standard_True;
        myShapesToAvoid.Add(aF1);
      }
      else if (aNbF==2) {
        const TopoDS_Face& aF2=(*(TopoDS_Face*)(&aLF.Last()));
        if (aF2.IsSame(aF1)) {
          if (BRep_Tool::IsClosed(aE, aF1)) {
            continue;
          }
          //
          if (aOrE==TopAbs_INTERNAL) {
            continue;
          }
          //
          bFound=Standard_True;
          myShapesToAvoid.Add(aF1);
          myShapesToAvoid.Add(aF2);
        }
      }
    }// for (i=1; i<=aNbE; ++i) {
    //
    if (!bFound) {
      break;
    }
    //
  }//for(;;) {
}  
//=======================================================================
//function : PerformLoops
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::PerformLoops()
{
  Standard_Integer i, aNbSh;
  BOPCol_ListIteratorOfListOfShape aIt;
  TopoDS_Iterator aItS;
  Handle(NCollection_BaseAllocator) aAlr;
  // 
  myLoops.Clear();
  //
  aAlr=
    NCollection_BaseAllocator::CommonBaseAllocator();
  BOPAlgo_ShellSplitter aSSp(aAlr);
  //
  // 1. Shells Usual
  aIt.Initialize (myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Face& aF=*((TopoDS_Face*)&aIt.Value());
    if (myContext->IsInfiniteFace(aF)) {
      TopoDS_Shell aSh;
      BRep_Builder aBB;
      //
      aBB.MakeShell(aSh);
      aBB.Add(aSh, aF);
      myLoops.Append(aSh);
      continue;
    }
    //
    if (!myShapesToAvoid.Contains(aF)) {
      aSSp.AddStartElement(aF);
    }
  }
  //
  aSSp.SetRunParallel(myRunParallel);
  aSSp.Perform();
  if (aSSp.HasErrors()) {
    // add warning status
    {
      TopoDS_Compound aFacesSp;
      BRep_Builder().MakeCompound(aFacesSp);
      BOPCol_ListIteratorOfListOfShape aItLF(aSSp.StartElements());
      for (; aItLF.More(); aItLF.Next()) {
        BRep_Builder().Add(aFacesSp, aItLF.Value());
      }
      AddWarning (new BOPAlgo_AlertShellSplitterFailed (aFacesSp));
    }
    return;
  }
  //
  const BOPCol_ListOfShape& aLSh=aSSp.Shells();
  aIt.Initialize (aLSh);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSh=aIt.Value();
    myLoops.Append(aSh);
  }
  //=================================================
  //
  // 2. Post Treatment
  BRep_Builder aBB;
  BOPCol_MapOfOrientedShape AddedFacesMap;
  BOPCol_IndexedDataMapOfShapeListOfShape aEFMap;
  BOPCol_MapOfOrientedShape aMP;
  //
  // a. collect all edges that are in loops
  aIt.Initialize (myLoops);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aItS.Initialize(aS);
    for (; aItS.More(); aItS.Next()) {
      const TopoDS_Shape& aF=aItS.Value();
      aMP.Add(aF);
    }
  }
  // 
  // b. collect all edges that are to avoid
  aNbSh = myShapesToAvoid.Extent();
  for (i = 1; i <= aNbSh; ++i) {
    const TopoDS_Shape& aF = myShapesToAvoid(i);
    aMP.Add(aF);
  }
  //
  // c. add all edges that are not processed to myShapesToAvoid
  aIt.Initialize (myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Face& aF=*((TopoDS_Face*)&aIt.Value());
    if (!myContext->IsInfiniteFace(aF)) {
      if (!aMP.Contains(aF)) {
        myShapesToAvoid.Add(aF);
      }
    }
  }
  //=================================================
  //
  // 3.Internal Shells
  myLoopsInternal.Clear();
  //
  aEFMap.Clear();
  AddedFacesMap.Clear();
  //
  aNbSh = myShapesToAvoid.Extent();
  for (i = 1; i <= aNbSh; ++i) {
    const TopoDS_Shape& aFF = myShapesToAvoid(i);
    BOPTools::MapShapesAndAncestors(aFF, 
        TopAbs_EDGE, TopAbs_FACE, 
        aEFMap);
  }
  //
  for (i = 1; i <= aNbSh; ++i) {
    const TopoDS_Shape& aFF = myShapesToAvoid(i);
    if (!AddedFacesMap.Add(aFF)) {
      continue;
    }
    //
    // make a new shell
    TopExp_Explorer aExp;
    TopoDS_Shell aShell;
    aBB.MakeShell(aShell);
    aBB.Add(aShell, aFF);
    //
    aItS.Initialize(aShell);
    for (; aItS.More(); aItS.Next()) {
      const TopoDS_Face& aF = (*(TopoDS_Face*)(&aItS.Value()));
      //
      aExp.Init(aF, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aExp.Current()));
        const BOPCol_ListOfShape& aLF=aEFMap.FindFromKey(aE);
        aIt.Initialize(aLF);
        for (; aIt.More(); aIt.Next()) { 
          const TopoDS_Face& aFL=(*(TopoDS_Face*)(&aIt.Value()));
          if (AddedFacesMap.Add(aFL)){
            aBB.Add(aShell, aFL);
          }
        }
      }
    }
    aShell.Closed (BRep_Tool::IsClosed (aShell));
    myLoopsInternal.Append(aShell);
  }
}
//=======================================================================
//function : PerformAreas
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::PerformAreas()
{
  myAreas.Clear();
  BRep_Builder aBB;
  // The new solids
  BOPCol_ListOfShape aNewSolids;
  // The hole shells which has to be classified relatively new solids
  BOPCol_IndexedMapOfShape aHoleShells;
  // Map of the faces of the hole shells for quick check of the growths.
  // If the analyzed shell contains any of the hole faces, it is considered as growth.
  BOPCol_IndexedMapOfShape aMHF;

  // Analyze the shells
  BOPCol_ListIteratorOfListOfShape aItLL(myLoops);
  for (; aItLL.More(); aItLL.Next())
  {
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
      aBB.Add (aSolid, aShell);
      aNewSolids.Append (aSolid);
    }
    else
    {
      aHoleShells.Add(aShell);
      BOPTools::MapShapes(aShell, TopAbs_FACE, aMHF);
    }
  }

  if (aHoleShells.IsEmpty())
  {
    // No holes, stop the analysis
    BOPCol_ListIteratorOfListOfShape aItLS(aNewSolids);
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

  // Prepare tree filler with the boxes of the hole shells
  BOPCol_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);

  Standard_Integer i, aNbH = aHoleShells.Extent();
  for (i = 1; i <= aNbH; ++i)
  {
    const TopoDS_Shape& aHShell = aHoleShells(i);
    //
    Bnd_Box aBox;
    BRepBndLib::Add(aHShell, aBox);
    aTreeFiller.Add(i, aBox);

    myBoxes.Bind(aHShell, aBox);
  }

  // Shake TreeFiller
  aTreeFiller.Fill();

  // Find outer growth shell that is most close to each hole shell
  BOPCol_IndexedDataMapOfShapeShape aHoleSolidMap;

  BOPCol_ListIteratorOfListOfShape aItLS(aNewSolids);
  for (; aItLS.More(); aItLS.Next())
  {
    const TopoDS_Shape& aSolid = aItLS.Value();

    // Build box
    Bnd_Box aBox;
    BRepBndLib::Add(aSolid, aBox);

    myBoxes.Bind(aSolid, aBox);

    BOPCol_BoxBndTreeSelector aSelector;
    aSelector.SetBox(aBox);
    aBBTree.Select(aSelector);

    const BOPCol_ListOfInteger& aLI = aSelector.Indices();
    BOPCol_ListIteratorOfListOfInteger aItLI(aLI);
    for (; aItLI.More(); aItLI.Next())
    {
      Standard_Integer k = aItLI.Value();
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
  BOPCol_IndexedDataMapOfShapeListOfShape aSolidHolesMap;

  aNbH = aHoleSolidMap.Extent();
  for (i = 1; i <= aNbH; ++i)
  {
    const TopoDS_Shape& aHole = aHoleSolidMap.FindKey(i);
    const TopoDS_Shape& aSolid = aHoleSolidMap(i);
    //
    BOPCol_ListOfShape* pLHoles = aSolidHolesMap.ChangeSeek(aSolid);
    if (!pLHoles)
      pLHoles = &aSolidHolesMap(aSolidHolesMap.Add(aSolid, BOPCol_ListOfShape()));
    pLHoles->Append(aHole);
  }

  // Add Holes to Solids and add them to myAreas
  aItLS.Initialize(aNewSolids);
  for ( ; aItLS.More(); aItLS.Next())
  {
    TopoDS_Solid& aSolid = *(TopoDS_Solid*)&aItLS.Value();
    const BOPCol_ListOfShape* pLHoles = aSolidHolesMap.Seek(aSolid);
    if (pLHoles)
    {
      // update solid
      BOPCol_ListIteratorOfListOfShape aItLH(*pLHoles);
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
      aBB.Add (aSolid, aHole);
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
//=======================================================================
//function : PerformInternalShapes
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderSolid::PerformInternalShapes()
{
  if (myAvoidInternalShapes)
    // user-defined option to avoid internal parts is in force
    return;

  if (myLoopsInternal.IsEmpty())
    // no internal parts
    return;

  // Get all faces to classify
  BOPCol_IndexedMapOfShape aMFs;
  BOPCol_ListIteratorOfListOfShape aItLS(myLoopsInternal);
  for (; aItLS.More(); aItLS.Next())
  {
    const TopoDS_Shape& aShell = aItLS.Value();
    TopoDS_Iterator aIt(aShell);
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
    BOPCol_ListOfShape aLSI;
    MakeInternalShells(aMFs, aLSI);
    //
    aItLS.Initialize(aLSI);
    for (; aItLS.More(); aItLS.Next())
      aBB.Add(aSolid, aItLS.Value());

    myAreas.Append(aSolid);
    return;
  }

  // Classify faces relatively solids

  // Prepare list of faces to classify
  BOPCol_ListOfShape aLFaces;
  Standard_Integer i, aNbF = aMFs.Extent();
  for (i = 1; i <= aNbF; ++i)
    aLFaces.Append(aMFs(i));

  // Map of solids with IN faces
  BOPCol_IndexedDataMapOfShapeListOfShape aMSLF;

  // Perform classification
  BOPAlgo_Tools::ClassifyFaces(aLFaces, myAreas, myRunParallel, myContext, aMSLF, myBoxes);

  // Update Solids by internal Faces

  BOPCol_MapOfShape aMFDone;

  Standard_Integer aNbS = aMSLF.Extent();
  for (i = 1; i <= aNbS; ++i)
  {
    const TopoDS_Shape& aSolid = aMSLF.FindKey(i);
    TopoDS_Shape *pSolid = (TopoDS_Shape*)&aSolid;

    const BOPCol_ListOfShape& aLF = aMSLF(i);
    if (aLF.IsEmpty())
      continue;

    BOPCol_IndexedMapOfShape aMF;
    aItLS.Initialize(aLF);
    for (; aItLS.More(); aItLS.Next())
    {
      const TopoDS_Shape& aF = aItLS.Value();
      aMF.Add(aF);
      aMFDone.Add(aF);
    }
    //
    BOPCol_ListOfShape aLSI;
    MakeInternalShells(aMF, aLSI);
    //
    aItLS.Initialize(aLSI);
    for (; aItLS.More(); aItLS.Next())
    {
      const TopoDS_Shape& aSI = aItLS.Value();
      aBB.Add (*pSolid, aSI);
    }
  }

  // Make solid from the unused faces (if any)
  BOPCol_IndexedMapOfShape aMFUnUsed;
  for (i = 1; i <= aNbF; ++i)
  {
    const TopoDS_Shape& aF = aMFs(i);
    if (!aMFDone.Contains(aF))
      aMFUnUsed.Add(aF);
  }

  if (aMFUnUsed.Extent())
  {
    TopoDS_Solid aSolid;
    aBB.MakeSolid(aSolid);
    //
    BOPCol_ListOfShape aLSI;
    MakeInternalShells(aMFUnUsed, aLSI);
    //
    aItLS.Initialize(aLSI);
    for (; aItLS.More(); aItLS.Next())
    {
      const TopoDS_Shape& aSI = aItLS.Value();
      aBB.Add (aSolid, aSI);

      Bnd_Box aBox;
      BRepBndLib::Add(aSolid, aBox);
      myBoxes.Bind(aSolid, aBox);
    }
    myAreas.Append(aSolid);
  }
}
//=======================================================================
//function : MakeInternalShells
//purpose  : 
//=======================================================================
void MakeInternalShells(const BOPCol_IndexedMapOfShape& theMF,
                        BOPCol_ListOfShape& theShells)
{
  Standard_Integer i, aNbF;
  BRep_Builder aBB;
  BOPCol_ListIteratorOfListOfShape aItF;
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF;
  BOPCol_MapOfShape aAddedFacesMap;
  //
  aNbF = theMF.Extent();
  for (i = 1; i <= aNbF; ++i) {
    TopoDS_Shape aF = theMF(i);
    BOPTools::MapShapesAndAncestors(aF, 
        TopAbs_EDGE, TopAbs_FACE, 
        aMEF);
  }
  //
  for (i = 1; i <= aNbF; ++i) {
    TopoDS_Shape aFF = theMF(i);
    if (!aAddedFacesMap.Add(aFF)) {
      continue;
    }
    //
    // make a new shell
    TopoDS_Shell aShell;
    aBB.MakeShell(aShell);    
    aFF.Orientation(TopAbs_INTERNAL);
    aBB.Add(aShell, aFF);
    //
    TopoDS_Iterator aItAddedF (aShell);
    for (; aItAddedF.More(); aItAddedF.Next()) {
      const TopoDS_Shape& aF =aItAddedF.Value();
      //
      TopExp_Explorer aEdgeExp(aF, TopAbs_EDGE);
      for (; aEdgeExp.More(); aEdgeExp.Next()) {
        const TopoDS_Shape& aE =aEdgeExp.Current();
        const BOPCol_ListOfShape& aLF=aMEF.FindFromKey(aE);
        aItF.Initialize(aLF);
        for (; aItF.More(); aItF.Next()) { 
          TopoDS_Shape aFL=aItF.Value();
          if (aAddedFacesMap.Add(aFL)){
            aFL.Orientation(TopAbs_INTERNAL);
            aBB.Add(aShell, aFL);
          }
        }
      }
    }
    aShell.Closed (BRep_Tool::IsClosed (aShell));
    theShells.Append(aShell);
  }
}
//=======================================================================
//function : IsHole
//purpose  : 
//=======================================================================
Standard_Boolean IsHole(const TopoDS_Shape& theS2,
                        Handle(IntTools_Context)& theContext)
{
  TopoDS_Solid *pS2=(TopoDS_Solid *)&theS2;
  BRepClass3d_SolidClassifier& aClsf=theContext->SolidClassifier(*pS2);
  //
  aClsf.PerformInfinitePoint(::RealSmall());
  //
  return (aClsf.State()==TopAbs_IN);
}
//=======================================================================
//function : IsInside
//purpose  : 
//=======================================================================
Standard_Boolean IsInside(const TopoDS_Shape& theS1,
                          const TopoDS_Shape& theS2,
                          Handle(IntTools_Context)& theContext)
{
  TopExp_Explorer aExp;
  TopAbs_State aState;
  //
  TopoDS_Solid *pS2=(TopoDS_Solid *)&theS2;
  //
  aExp.Init(theS1, TopAbs_FACE);
  if (!aExp.More()){
    BRepClass3d_SolidClassifier& aClsf=theContext->SolidClassifier(*pS2);
    aClsf.PerformInfinitePoint(::RealSmall());
    aState=aClsf.State();
  }
  else {
    BOPCol_IndexedMapOfShape aBounds;
    BOPTools::MapShapes(*pS2, TopAbs_EDGE, aBounds);
    const TopoDS_Face& aF = (*(TopoDS_Face*)(&aExp.Current()));
    aState=BOPTools_AlgoTools::ComputeState(aF, *pS2,
                                            Precision::Confusion(),
                                            aBounds, theContext);
  }
  return (aState==TopAbs_IN);
}
//=======================================================================
//function : IsGrowthShell
//purpose  : 
//=======================================================================
Standard_Boolean IsGrowthShell(const TopoDS_Shape& theShell,
                               const BOPCol_IndexedMapOfShape& theMHF)
{
  if (theMHF.Extent())
  {
    TopoDS_Iterator aIt(theShell);
    for(; aIt.More(); aIt.Next())
    {
      if (theMHF.Contains(aIt.Value()))
        return Standard_True;
    }
  }
  return Standard_False;
}
