// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
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

#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <BOPAlgo_BuilderFace.hxx>
#include <BOPAlgo_WireEdgeSet.hxx>
#include <BOPAlgo_WireSplitter.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPTools_BoxSelector.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_FClass2d.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <TColStd_MapIntegerHasher.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
//
static
  Standard_Boolean IsGrowthWire(const TopoDS_Shape& ,
                                const TopTools_IndexedMapOfShape& );

static 
  Standard_Boolean IsInside(const TopoDS_Shape& ,
                            const TopoDS_Shape& ,
                            Handle(IntTools_Context)& );
static
  void MakeInternalWires(const TopTools_IndexedMapOfShape& ,
                         TopTools_ListOfShape& );

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_BuilderFace::BOPAlgo_BuilderFace()
:
  BOPAlgo_BuilderArea()
{
  myOrientation=TopAbs_EXTERNAL;
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_BuilderFace::BOPAlgo_BuilderFace
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_BuilderArea(theAllocator)
{ 
  myOrientation=TopAbs_EXTERNAL;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  BOPAlgo_BuilderFace::~BOPAlgo_BuilderFace()
{
}
//=======================================================================
//function : SetFace
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderFace::SetFace(const TopoDS_Face& theFace)
{
  myOrientation=theFace.Orientation();
  myFace=theFace;
  myFace.Orientation(TopAbs_FORWARD);
}
//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================
TopAbs_Orientation BOPAlgo_BuilderFace::Orientation()const
{
  return myOrientation;
}
//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
const TopoDS_Face& BOPAlgo_BuilderFace::Face()const
{
  return myFace;
}
//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderFace::CheckData()
{
  if (myFace.IsNull()) {
    AddError (new BOPAlgo_AlertNullInputShapes);
    return;
  }
  if (myContext.IsNull()) {
    myContext = new IntTools_Context;
  }
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderFace::Perform()
{
  GetReport()->Clear();
  //
  CheckData();
  if (HasErrors()) {
    return;
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
void BOPAlgo_BuilderFace::PerformShapesToAvoid()
{
  Standard_Boolean bFound;
  Standard_Integer i, iCnt, aNbV, aNbE;
  TopTools_IndexedDataMapOfShapeListOfShape aMVE;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  myShapesToAvoid.Clear();
  //
  iCnt=0;
  for(;;) {
    ++iCnt;
    bFound=Standard_False;
    //
    // 1. MEF
    aMVE.Clear();
    aIt.Initialize (myShapes);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE=aIt.Value();
      if (!myShapesToAvoid.Contains(aE)) {
        TopExp::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
      }
    }
    aNbV=aMVE.Extent();
    //
    // 2. myEdgesToAvoid
    for (i=1; i<=aNbV; ++i) {
      const TopoDS_Vertex& aV=(*(TopoDS_Vertex *)(&aMVE.FindKey(i)));
      //
      TopTools_ListOfShape& aLE=aMVE.ChangeFromKey(aV);
      aNbE=aLE.Extent();
      if (!aNbE) {
        continue;
      }
      //
      const TopoDS_Edge& aE1=(*(TopoDS_Edge *)(&aLE.First()));
      if (aNbE==1) {
        if (BRep_Tool::Degenerated(aE1)) {
          continue;
        }
        if (aV.Orientation()==TopAbs_INTERNAL) {
          continue;
        }
        bFound=Standard_True;
        myShapesToAvoid.Add(aE1);
      }
      else if (aNbE==2) {
        const TopoDS_Edge& aE2=(*(TopoDS_Edge *)(&aLE.Last()));
        if (aE2.IsSame(aE1)) {
          TopoDS_Vertex aV1x, aV2x;
          //
          TopExp::Vertices(aE1, aV1x, aV2x);
          if (aV1x.IsSame(aV2x)) {
            continue;
          }
          bFound=Standard_True;
          myShapesToAvoid.Add(aE1);
          myShapesToAvoid.Add(aE2);
        }
      }
    }// for (i=1; i<=aNbE; ++i) {
    //
    if (!bFound) {
      break;
    }
    //
  }//while (1) 
  //printf(" EdgesToAvoid=%d, iCnt=%d\n", EdgesToAvoid.Extent(), iCnt);
}  
//=======================================================================
//function : PerformLoops
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderFace::PerformLoops()
{
  Standard_Boolean bFlag;
  Standard_Integer i, aNbEA;
  TopTools_ListIteratorOfListOfShape aIt;
  TopTools_IndexedDataMapOfShapeListOfShape aVEMap;
  TopTools_MapOfOrientedShape aMAdded;
  TopoDS_Iterator aItW;
  BRep_Builder aBB; 
  BOPAlgo_WireEdgeSet aWES(myAllocator);
  BOPAlgo_WireSplitter aWSp(myAllocator);
  //
  // 1. 
  myLoops.Clear();
  aWES.SetFace(myFace);
  //
  aIt.Initialize(myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE=aIt.Value();
    if (!myShapesToAvoid.Contains(aE)) {
      aWES.AddStartElement(aE);
    }
  }
  //
  aWSp.SetWES(aWES);
  aWSp.SetRunParallel(myRunParallel);
  aWSp.SetContext(myContext);
  aWSp.Perform();
  if (aWSp.HasErrors()) {
    return;
  }
  //
  const TopTools_ListOfShape& aLW=aWES.Shapes();
  aIt.Initialize (aLW);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aW=aIt.Value();
    myLoops.Append(aW);
  }
  // Post Treatment
  TopTools_MapOfOrientedShape aMEP;
  // 
  // a. collect all edges that are in loops
  aIt.Initialize (myLoops);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aW=aIt.Value();
    aItW.Initialize(aW);
    for (; aItW.More(); aItW.Next()) {
      const TopoDS_Shape& aE=aItW.Value();
      aMEP.Add(aE);
    }
  }
  // 
  // b. collect all edges that are to avoid
  aNbEA = myShapesToAvoid.Extent();
  for (i = 1; i <= aNbEA; ++i) {
    const TopoDS_Shape& aE = myShapesToAvoid(i);
    aMEP.Add(aE);
  }
  //
  // c. add all edges that are not processed to myShapesToAvoid
  aIt.Initialize (myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE=aIt.Value();
    if (!aMEP.Contains(aE)) {
      myShapesToAvoid.Add(aE);
    }
  }
  //
  // 2. Internal Wires
  myLoopsInternal.Clear();
  //
  aNbEA = myShapesToAvoid.Extent();
  for (i = 1; i <= aNbEA; ++i) {
    const TopoDS_Shape& aEE = myShapesToAvoid(i);
    TopExp::MapShapesAndAncestors(aEE, 
                                    TopAbs_VERTEX, 
                                    TopAbs_EDGE, 
                                    aVEMap);
  }
  //
  bFlag=Standard_True;
  for (i = 1; (i <= aNbEA) && bFlag; ++i) {
    const TopoDS_Shape& aEE = myShapesToAvoid(i);
    if (!aMAdded.Add(aEE)) {
      continue;
    }
    //
    // make new wire
    TopoDS_Wire aW;
    aBB.MakeWire(aW);
    aBB.Add(aW, aEE);
    //
    aItW.Initialize(aW);
    for (; aItW.More()&&bFlag; aItW.Next()) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aItW.Value()));
      //
      TopoDS_Iterator aItE(aE);
      for (; aItE.More()&&bFlag; aItE.Next()) {
        const TopoDS_Vertex& aV = (*(TopoDS_Vertex *)(&aItE.Value()));
        const TopTools_ListOfShape& aLE=aVEMap.FindFromKey(aV);
        aIt.Initialize(aLE);
        for (; aIt.More()&&bFlag; aIt.Next()) { 
          const TopoDS_Shape& aEx=aIt.Value();
          if (aMAdded.Add(aEx)) {
            aBB.Add(aW, aEx);
            if(aMAdded.Extent()==aNbEA) {
              bFlag=!bFlag;
            }
          }
        }//for (; aIt.More(); aIt.Next()) { 
      }//for (; aItE.More(); aItE.Next()) {
    }//for (; aItW.More(); aItW.Next()) {
    aW.Closed(BRep_Tool::IsClosed(aW));
    myLoopsInternal.Append(aW);
  }//for (i = 1; (i <= aNbEA) && bFlag; ++i) {
}
//=======================================================================
//function : PerformAreas
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderFace::PerformAreas()
{
  myAreas.Clear();
  BRep_Builder aBB;
  // Location of the myFace
  TopLoc_Location aLoc;
  // Get surface from myFace
  const Handle(Geom_Surface)& aS = BRep_Tool::Surface(myFace, aLoc);
  // Get tolerance of myFace
  Standard_Real aTol = BRep_Tool::Tolerance(myFace);

  // Check if there are no loops at all
  if (myLoops.IsEmpty())
  {
    if (myContext->IsInfiniteFace(myFace))
    {
      TopoDS_Face aFace;
      aBB.MakeFace(aFace, aS, aLoc, aTol);
      if (BRep_Tool::NaturalRestriction(myFace))
        aBB.NaturalRestriction(aFace, Standard_True);
      myAreas.Append(aFace);
    }
    return;
  }

  // The new faces
  TopTools_ListOfShape aNewFaces;
  // The hole faces which has to be classified relatively new faces
  TopTools_IndexedMapOfShape aHoleFaces;
  // Map of the edges of the hole faces for quick check of the growths.
  // If the analyzed wire contains any of the edges from the hole faces
  // it is considered as growth.
  TopTools_IndexedMapOfShape aMHE;

  // Analyze the new wires - classify them to be the holes and growths
  TopTools_ListIteratorOfListOfShape aItLL(myLoops);
  for (; aItLL.More(); aItLL.Next())
  {
    const TopoDS_Shape& aWire = aItLL.Value();

    TopoDS_Face aFace;
    aBB.MakeFace(aFace, aS, aLoc, aTol);
    aBB.Add(aFace, aWire);

    Standard_Boolean bIsGrowth = IsGrowthWire(aWire, aMHE);
    if (!bIsGrowth)
    {
      // Fast check did not give the result, run classification
      IntTools_FClass2d& aClsf = myContext->FClass2d(aFace);
      bIsGrowth = !aClsf.IsHole();
    }

    // Save the face
    if (bIsGrowth)
    {
      aNewFaces.Append(aFace);
    }
    else
    {
      aHoleFaces.Add(aFace);
      TopExp::MapShapes(aWire, TopAbs_EDGE, aMHE);
    }
  }

  if (aHoleFaces.IsEmpty())
  {
    // No holes, stop the analysis
    myAreas.Append(aNewFaces);
  }

  // Classify holes relatively faces

  // Prepare tree filler with the boxes of the hole faces
  NCollection_UBTree<Standard_Integer, Bnd_Box2d> aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box2d> aTreeFiller(aBBTree);

  Standard_Integer i, aNbH = aHoleFaces.Extent();
  for (i = 1; i <= aNbH; ++i)
  {
    const TopoDS_Face& aHFace = TopoDS::Face(aHoleFaces(i));
    //
    Bnd_Box2d aBox;
    BRepTools::AddUVBounds(aHFace, aBox);
    aTreeFiller.Add(i, aBox);
  }

  // Shake TreeFiller
  aTreeFiller.Fill();

  // Find outer growth face that is most close to each hole face
  TopTools_IndexedDataMapOfShapeShape aHoleFaceMap;

  // Selector
  BOPTools_BoxSelector<Bnd_Box2d> aSelector;

  TopTools_ListIteratorOfListOfShape aItLS(aNewFaces);
  for (; aItLS.More(); aItLS.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aItLS.Value());

    // Build box
    Bnd_Box2d aBox;
    BRepTools::AddUVBounds(aFace, aBox);

    aSelector.Clear();
    aSelector.SetBox(aBox);
    aBBTree.Select(aSelector);

    const TColStd_ListOfInteger& aLI = aSelector.Indices();
    TColStd_ListIteratorOfListOfInteger aItLI(aLI);
    for (; aItLI.More(); aItLI.Next())
    {
      Standard_Integer k = aItLI.Value();
      const TopoDS_Shape& aHole = aHoleFaces(k);
      // Check if it is inside
      if (!IsInside(aHole, aFace, myContext))
        continue;

      // Save the relation
      TopoDS_Shape* pFaceWas = aHoleFaceMap.ChangeSeek(aHole);
      if (pFaceWas)
      {
        if (IsInside(aFace, *pFaceWas, myContext))
        {
          *pFaceWas = aFace;
        }
      }
      else
      {
        aHoleFaceMap.Add(aHole, aFace);
      }
    }
  }

  // Make the back map from faces to holes
  TopTools_IndexedDataMapOfShapeListOfShape aFaceHolesMap;

  aNbH = aHoleFaceMap.Extent();
  for (i = 1; i <= aNbH; ++i)
  {
    const TopoDS_Shape& aHole = aHoleFaceMap.FindKey(i);
    const TopoDS_Shape& aFace = aHoleFaceMap(i);
    //
    TopTools_ListOfShape* pLHoles = aFaceHolesMap.ChangeSeek(aFace);
    if (!pLHoles)
      pLHoles = &aFaceHolesMap(aFaceHolesMap.Add(aFace, TopTools_ListOfShape()));
    pLHoles->Append(aHole);
  }

  // Add unused holes to the original face
  if (aHoleFaces.Extent() != aHoleFaceMap.Extent())
  {
    Bnd_Box aBoxF;
    BRepBndLib::Add(myFace, aBoxF);
    if (aBoxF.IsOpenXmin() || aBoxF.IsOpenXmax() ||
        aBoxF.IsOpenYmin() || aBoxF.IsOpenYmax() ||
        aBoxF.IsOpenZmin() || aBoxF.IsOpenZmax())
    {
      TopoDS_Face aFace;
      aBB.MakeFace(aFace, aS, aLoc, aTol);
      TopTools_ListOfShape& anUnUsedHoles = aFaceHolesMap(aFaceHolesMap.Add(aFace, TopTools_ListOfShape()));
      aNbH = aHoleFaces.Extent();
      for (i = 1; i <= aNbH; ++i)
      {
        const TopoDS_Shape& aHole = aHoleFaces(i);
        if (!aHoleFaceMap.Contains(aHole))
          anUnUsedHoles.Append(aHole);
      }
      // Save it
      aNewFaces.Append(aFace);
    }
  }

  // Add Holes to Faces and add them to myAreas
  aItLS.Initialize(aNewFaces);
  for ( ; aItLS.More(); aItLS.Next())
  {
    TopoDS_Face& aFace = *(TopoDS_Face*)&aItLS.Value();
    const TopTools_ListOfShape* pLHoles = aFaceHolesMap.Seek(aFace);
    if (pLHoles)
    {
      // update faces with the holes
      TopTools_ListIteratorOfListOfShape aItLH(*pLHoles);
      for (; aItLH.More(); aItLH.Next())
      {
        const TopoDS_Shape& aFHole = aItLH.Value();
        // The hole face contains only one wire
        TopoDS_Iterator aItW(aFHole);
        aBB.Add(aFace, aItW.Value());
      }

      // update classifier
      myContext->FClass2d(aFace).Init(aFace, aTol);
    }

    // The face is just a draft that does not contain any internal shapes
    myAreas.Append(aFace);
  }
}
//=======================================================================
//function : PerformInternalShapes
//purpose  : 
//=======================================================================
void BOPAlgo_BuilderFace::PerformInternalShapes()
{
  if (myAvoidInternalShapes) {
    return;
  }
  //
  Standard_Integer aNbWI=myLoopsInternal.Extent();
  if (!aNbWI) {// nothing to do
    return;
  }
  // 
  //Standard_Real aTol;
  Standard_Integer i;
  BRep_Builder aBB;
  TopTools_ListIteratorOfListOfShape aIt1, aIt2;
  TopoDS_Iterator aIt; 
  TopTools_IndexedMapOfShape aME1, aME2, aMEP;
  TopTools_IndexedDataMapOfShapeListOfShape aMVE;
  TopTools_ListOfShape aLSI;
  //
  // 1. All internal edges
  aIt1.Initialize(myLoopsInternal);
  for (; aIt1.More(); aIt1.Next()) {
    const TopoDS_Shape& aWire=aIt1.Value();
    aIt.Initialize(aWire);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE=aIt.Value();
      aME1.Add(aE);
    }
  }
  //
  // 2 Process faces
  aIt2.Initialize(myAreas);
  for ( ; aIt2.More(); aIt2.Next()) {
    TopoDS_Face& aF=(*(TopoDS_Face *)(&aIt2.Value()));
    //
    aMVE.Clear();
    TopExp::MapShapesAndAncestors(aF, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
    //
    // 2.1 Separate faces to process aMEP
    aME2.Clear();
    aMEP.Clear();
    aNbWI = aME1.Extent();
    for (i = 1; i <= aNbWI; ++i) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&aME1(i)));
      if (IsInside(aE, aF, myContext)) {
        aMEP.Add(aE);
      }
      else {
        aME2.Add(aE);
      }
    }
    //
    // 2.2 Make Internal Wires
    aLSI.Clear();
    MakeInternalWires(aMEP, aLSI);
    //
    // 2.3 Add them to aF
    aIt1.Initialize(aLSI);
    for (; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aSI=aIt1.Value();
      aBB.Add (aF, aSI);
    }
    //
    // 2.4 Remove faces aMFP from aMF
    aME1 = aME2;
    //
    aNbWI = aME1.Extent();
    if (!aNbWI) {
      break;
    }
  } //for ( ; aIt2.More(); aIt2.Next()) {
}
//=======================================================================
//function : MakeInternalWires
//purpose  : 
//=======================================================================
void MakeInternalWires(const TopTools_IndexedMapOfShape& theME,
                       TopTools_ListOfShape& theWires)
{
  Standard_Integer i, aNbE;
  TopTools_MapOfShape aAddedMap;
  TopTools_ListIteratorOfListOfShape aItE;
  TopTools_IndexedDataMapOfShapeListOfShape aMVE;
  BRep_Builder aBB;
  //
  aNbE = theME.Extent();
  for (i = 1; i <= aNbE; ++i) {
    const TopoDS_Shape& aE = theME(i);
    TopExp::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  }
  //
  for (i = 1; i <= aNbE; ++i) {
    TopoDS_Shape aEE = theME(i);
    if (!aAddedMap.Add(aEE)) {
      continue;
    }
    //
    // make a new shell
    TopoDS_Wire aW;
    aBB.MakeWire(aW);    
    aEE.Orientation(TopAbs_INTERNAL);
    aBB.Add(aW, aEE);
    //
    TopoDS_Iterator aItAdded (aW);
    for (; aItAdded.More(); aItAdded.Next()) {
      const TopoDS_Shape& aE =aItAdded.Value();
      //
      TopExp_Explorer aExp(aE, TopAbs_VERTEX);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aV =aExp.Current();
        const TopTools_ListOfShape& aLE=aMVE.FindFromKey(aV);
        aItE.Initialize(aLE);
        for (; aItE.More(); aItE.Next()) { 
          TopoDS_Shape aEL=aItE.Value();
          if (aAddedMap.Add(aEL)){
            aEL.Orientation(TopAbs_INTERNAL);
            aBB.Add(aW, aEL);
          }
        }
      }
    }
    aW.Closed(BRep_Tool::IsClosed(aW));
    theWires.Append(aW);
  }
}
//=======================================================================
//function : IsInside
//purpose  : 
//=======================================================================
Standard_Boolean IsInside(const TopoDS_Shape& theWire,
                          const TopoDS_Shape& theF,
                          Handle(IntTools_Context)& theContext)
{
  // Check if the wire is located inside the face:
  // take unique point from the wire and classify it relatively the face

  // Avoid edges of the face
  TopTools_IndexedMapOfShape aFaceEdgesMap;
  TopExp::MapShapes(theF, TopAbs_EDGE, aFaceEdgesMap);

  // Get classification tool from the context
  const TopoDS_Face& aF = TopoDS::Face(theF);
  IntTools_FClass2d& aClassifier = theContext->FClass2d(aF);

  Standard_Boolean isInside = Standard_False;

  // Iterate on wire edges until first classification is performed
  TopExp_Explorer anExp(theWire, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& aE = TopoDS::Edge(anExp.Current());
    if (BRep_Tool::Degenerated(aE))
      // Avoid checking degenerated edges.
      continue;

    if (aFaceEdgesMap.Contains(aE))
      // Face contains the edge from the wire, thus the wire cannot be
      // inside that face.
      return isInside;

    // Get 2d curve of the edge on the face
    Standard_Real aT1, aT2;
    const Handle(Geom2d_Curve)& aC2D = BRep_Tool::CurveOnSurface(aE, aF, aT1, aT2);
    if (aC2D.IsNull())
      continue;

    // Get middle point on the curve
    gp_Pnt2d aP2D = aC2D->Value((aT1 + aT2) / 2.);

    // Classify the point
    TopAbs_State aState = aClassifier.Perform(aP2D);
    isInside = (aState == TopAbs_IN);
    break;
  }
  return isInside;
}
//=======================================================================
//function : IsGrowthWire
//purpose  : 
//=======================================================================
Standard_Boolean IsGrowthWire(const TopoDS_Shape& theWire,
                              const TopTools_IndexedMapOfShape& theMHE)
{
  if (theMHE.Extent())
  {
    TopoDS_Iterator aIt(theWire);
    for(; aIt.More(); aIt.Next())
    {
      if (theMHE.Contains(aIt.Value()))
        return Standard_True;
    }
  }
  return Standard_False;
}
