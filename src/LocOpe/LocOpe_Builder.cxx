// Created on: 1995-04-25
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <LocOpe_Builder.ixx>

#include <LocOpe_DataMapOfShapePnt.hxx>
#include <LocOpe_DataMapIteratorOfDataMapOfShapePnt.hxx>
#include <LocOpe.hxx>


//#include <TopOpeBRepDS_HDataStructure.hxx>
//#include <TopOpeBRepDS_DataStructure.hxx>
#include <BRepAlgo_Tool.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <TColStd_Array1OfInteger.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp.hxx>
#include <Precision.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <gp_Pln.hxx>
#include <BRepAlgo.hxx>
#include <Standard_ErrorHandler.hxx>
#include <BRepAlgo_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BOPTools_DSFiller.hxx>
#include <BOPTools_PDSFiller.hxx>
#include <BOP_Builder.hxx>
#include <BOP_PBuilder.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <stdio.h>

//#define DRAW
#ifdef DRAW
#include <TestTopOpe.hxx>
#include <DBRep.hxx>
#endif

void RemoveFaces(const TopoDS_Shape& theComp, const BOPTools_PDSFiller& theDSFiller,
		 TopTools_MapOfShape& theMap) 
{
  const BooleanOperations_ShapesDataStructure& aDS = theDSFiller->DS();
  const BOPTools_InterferencePool& anInterfPool = theDSFiller->InterfPool();
  BOPTools_InterferencePool* pInterfPool = (BOPTools_InterferencePool*) &anInterfPool;
  BOPTools_CArray1OfSSInterference& aFFs = pInterfPool->SSInterferences();

  TopTools_MapOfShape aCompMap;
  TopExp_Explorer anExp(theComp, TopAbs_EDGE);
  for(; anExp.More(); anExp.Next()) {
    aCompMap.Add(anExp.Current());
  }

  Standard_Integer aNbFFs=aFFs.Extent(), i, j, aNbS, aNbCurves, nSect;

    for (i=1; i<=aNbFFs; ++i) {
      BOPTools_SSInterference& aFFi=aFFs(i);
      //
      //
      // Old Section Edges
      const BOPTools_ListOfPaveBlock& aSectList=aFFi.PaveBlocks();
      aNbS=aSectList.Extent();
      BOPTools_ListIteratorOfListOfPaveBlock anIt(aSectList);
      for (; anIt.More();anIt.Next()) {
	const BOPTools_PaveBlock& aPB=anIt.Value();
	nSect=aPB.Edge();
	const TopoDS_Shape& aS=aDS.GetShape(nSect);
	
	const TopoDS_Edge& aE = TopoDS::Edge(aS);

	if(!aCompMap.Contains(aE)) continue;
	
	Standard_Integer nF1 = aFFi.Index1();
	Standard_Integer nF2 = aFFi.Index2();

	const TopoDS_Shape& aF1 = aDS.Shape(nF1);
	const TopoDS_Shape& aF2 = aDS.Shape(nF2);

	if(theMap.Contains(aF1)) theMap.Remove(aF1);
	if(theMap.Contains(aF2)) theMap.Remove(aF2);
	

      }
      //
      // New Section Edges
      BOPTools_SequenceOfCurves& aBCurves=aFFi.Curves();
      aNbCurves=aBCurves.Length();
      for (j=1; j<=aNbCurves; j++) {
	BOPTools_Curve& aBC=aBCurves(j);
	const BOPTools_ListOfPaveBlock& aSectEdges=aBC.NewPaveBlocks();
	aNbS=aSectEdges.Extent();
	
	BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSectEdges);
	for (; aPBIt.More(); aPBIt.Next()) {
	  BOPTools_PaveBlock& aPB=aPBIt.Value();
	  nSect=aPB.Edge();
	  const TopoDS_Shape& aS=aDS.GetShape(nSect);
	  const TopoDS_Edge& aE = TopoDS::Edge(aS);

	  if(!aCompMap.Contains(aE)) continue;
	
	  Standard_Integer nF1 = aFFi.Index1();
	  Standard_Integer nF2 = aFFi.Index2();

	  const TopoDS_Shape& aF1 = aDS.Shape(nF1);
	  const TopoDS_Shape& aF2 = aDS.Shape(nF2);

	  if(theMap.Contains(aF1)) theMap.Remove(aF1);
	  if(theMap.Contains(aF2)) theMap.Remove(aF2);
	
	}
      }
    }
  
 }

inline Standard_Boolean Egal(const gp_Pnt& P1, const gp_Pnt& P2)
{

  return ((P1.X() == P2.X()) && 
	  (P1.Y() == P2.Y()) && 
	  (P1.Z() == P2.Z()));

}


//=======================================================================
//function : LocOpe_Builder
//purpose  : 
//=======================================================================

LocOpe_Builder::LocOpe_Builder() : 
   myDone(Standard_False), myPdone(Standard_False),myPerfdone(Standard_False),
   myInvDone(Standard_False)
{

}


//=======================================================================
//function : LocOpe_Builder
//purpose  : 
//=======================================================================

LocOpe_Builder::LocOpe_Builder(const TopoDS_Shape& S) : 
  myDone(Standard_False), myShape(S), myTool(S),
  myPdone(Standard_False), myPerfdone(Standard_False),
  myInvDone(Standard_False)
{
  
  if (S.IsNull()) {
    Standard_ConstructionError::Raise();
  }
}


//=======================================================================
//function : LocOpe_Builder
//purpose  : 
//=======================================================================

LocOpe_Builder::LocOpe_Builder(const TopoDS_Shape& S,
			       const TopoDS_Shape& T) : 
  myDone(Standard_False), myShape(S), myTool(T),
  myPdone(Standard_False), myPerfdone(Standard_False),
  myInvDone(Standard_False)
{
  
  if (S.IsNull()|| T.IsNull()) {
    Standard_ConstructionError::Raise();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void LocOpe_Builder::Init(const TopoDS_Shape& S)
{
  if (S.IsNull()) {
    Standard_ConstructionError::Raise();
  }

  myDone = Standard_False;
  myPdone = Standard_False;
  myPerfdone = Standard_False;
  myInvDone = Standard_False;
  myShape = S;
  myTool = S;
  myParts.Clear();
  myRemoved.Clear();
  myResult.Nullify();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void LocOpe_Builder::Init(const TopoDS_Shape& S,
			  const TopoDS_Shape& T)
{
  if (S.IsNull() || T.IsNull()) {
    Standard_ConstructionError::Raise();
  }

  myDone = Standard_False;
  myPdone = Standard_False;
  myPerfdone = Standard_False;
  myInvDone = Standard_False;
  myShape = S;
  myTool = T;
  myParts.Clear();
  myRemoved.Clear();
  myResult.Nullify();
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void LocOpe_Builder::Perform (const TopoDS_Shape& Tool,
			      const TopTools_ListOfShape& L,
			      const Standard_Boolean Fuse)
{

  if (Tool.IsNull() || myShape.IsSame(Tool)) {
    Standard_ConstructionError::Raise();
  }
  myTool = Tool;
  TopTools_ListOfShape LTool;
  TopExp_Explorer exp;
  for (exp.Init(myTool,TopAbs_FACE); exp.More(); exp.Next()) {
//  for (TopExp_Explorer exp(myTool,TopAbs_FACE); exp.More(); exp.Next()) {
    LTool.Append(exp.Current());
  }
  Perform(L,LTool,Fuse);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void LocOpe_Builder::Perform (const TopTools_ListOfShape& LShape,
			      const TopTools_ListOfShape& LTool,
			      const Standard_Boolean Fuse)
{

  
  myPerfdone = Standard_False;
  myInvDone = Standard_False;

  myDone = Standard_False;
  myPdone = Standard_False;

  myFuse = Fuse;
  TopAbs_State St2 = (myFuse) ? TopAbs_OUT : TopAbs_IN; 


  TopTools_MapOfShape mapUsedShape,mapUsedTool,mapShape,
                      mapTool,mapAdded, NewEdges;
  TopTools_IndexedDataMapOfShapeListOfShape EFMap;
  TopTools_ListIteratorOfListOfShape its(LShape),itt(LTool);
  TopTools_MapIteratorOfMapOfShape itm,itm2;
  TopTools_ListOfShape newFS,newFT;
  BRep_Builder B;
 
  Standard_Boolean found;

  TopExp::MapShapesAndAncestors(myShape,TopAbs_EDGE,TopAbs_FACE,EFMap);
  if (!myTool.IsSame(myShape)) {
    TopExp::MapShapesAndAncestors(myTool,TopAbs_EDGE,TopAbs_FACE,EFMap);
  }

  // on veut l`orientation relative correcte des faces dans le shape
//  for (TopExp_Explorer it(myShape.Oriented(TopAbs_FORWARD),TopAbs_FACE);
  TopExp_Explorer it(myShape.Oriented(TopAbs_FORWARD),TopAbs_FACE);
  for ( ; it.More(); it.Next()) {
    mapShape.Add(it.Current());
  }
  
  for (it.Init(myTool.Oriented(TopAbs_FORWARD),TopAbs_FACE);
       it.More();
       it.Next()) {
    mapTool.Add(it.Current());
  }
  
  if (!LShape.IsEmpty()) {

    // on verifie que les faces de LShape appartiennent a myShape
    while (its.More()) {
      Standard_Boolean found = Standard_False;
      for (itm.Initialize(mapShape);itm.More(); itm.Next()) {
	if (itm.Key().IsSame(its.Value())) {
	  found = Standard_True;
	  break;
	}
      }
      if (!found) {
	return;
      }
    
      TopAbs_Orientation orient = itm.Key().Orientation();
      TopoDS_Face theF = TopoDS::Face(its.Value());
      theF.Orientation(orient);
      if (mapAdded.Add(theF)) {
	newFS.Append(theF);
      }
      its.Next();
    }
  }
  else {
    for (it.Init(myShape.Oriented(TopAbs_FORWARD),TopAbs_FACE);
       it.More();
       it.Next()) {
      if (mapAdded.Add(it.Current())) {
	newFS.Append(it.Current());
      }
    }
  }


  if (!LTool.IsEmpty()) {
    // on verifie que les faces de LTool appartiennent a myTool
    while (itt.More()) {
      found = Standard_False;
      for (itm.Initialize(mapTool);itm.More(); itm.Next()) {
	if (itm.Key().IsSame(itt.Value())) {
	  found = Standard_True;
	  break;
	}
      }
      if (!found) {
	return;
      }
      
      TopAbs_Orientation orient = itm.Key().Orientation();
      TopoDS_Face theF = TopoDS::Face(itt.Value());
      theF.Orientation(orient);
      if (mapAdded.Add(theF)) {
	newFT.Append(theF);
      }
      itt.Next();
    }
  }
  else {
    for (it.Init(myTool.Oriented(TopAbs_FORWARD),TopAbs_FACE);
	 it.More();
	 it.Next()) {
      if (mapAdded.Add(it.Current())) {
	newFT.Append(it.Current());
      }
    }
  }

#ifdef DRAW
  Standard_Integer ii = 0;
  char namep[10];
  for (its.Initialize(newFS); its.More(); its.Next()) {
    ii++;
    sprintf(namep,"F_%d",ii);
    DBRep::Set(namep,its.Value());
  }
  ii = 0;
  for (itt.Initialize(newFT); itt.More(); itt.Next()) {
    ii++;
    sprintf(namep,"T_%d",ii);
    DBRep::Set(namep,itt.Value());
  }
#endif
#ifdef DRAW
      DBRep::Set("myShape", myShape);
      DBRep::Set("myTool", myTool);
#endif

  myShapeMap.Clear();
  myToolMap.Clear(); //--------------
  
#ifdef DRAW
  ii = 0;
#endif
  do {
#ifdef DRAW
    ii++;
#endif
    TopoDS_Compound CoFS, CoFT, CoTool, CoShape;
    B.MakeCompound(CoFS);
    B.MakeCompound(CoFT);
    B.MakeCompound(CoTool);
    B.MakeCompound(CoShape);
    for (its.Initialize(newFS); its.More(); its.Next()) {
      myShapeMap.Add(its.Value());
      B.Add(CoFS, its.Value());
    }
#ifdef DRAW
    sprintf(namep,"CoFS_%d",ii);
    DBRep::Set(namep, CoFS);
#endif

    for (itt.Initialize(newFT); itt.More(); itt.Next()) {
      myToolMap.Add(itt.Value());
      B.Add(CoFT, itt.Value());
    }
#ifdef DRAW
    sprintf(namep,"CoFT_%d",ii);
    DBRep::Set(namep, CoFT);
#endif

    for (itm2.Initialize(mapUsedTool); itm2.More(); itm2.Next()) {
      myToolMap.Add(itm2.Key());
      B.Add(CoTool, itm2.Key());
    }
#ifdef DRAW
    sprintf(namep,"CoTool_%d",ii);
    DBRep::Set(namep, CoTool);
#endif

    for (itm.Initialize(mapUsedShape); itm.More(); itm.Next()) {
      myShapeMap.Add(itm.Key());
      B.Add(CoShape, itm.Key());
    }
#ifdef DRAW
    sprintf(namep,"CoShape_%d",ii);
    DBRep::Set(namep, CoShape);
#endif

//---------------------------------------------------------------
    myAlgo.Init();
    if (St2 != TopAbs_OUT) {
      myAlgo.Load(myShape, myTool);
    }
    else {
      myAlgo.Load(myTool, myShape);
    }

    TopoDS_Compound aCoShape, aCoTool;
    B.MakeCompound(aCoShape);
    B.MakeCompound(aCoTool);

    TopTools_MapIteratorOfMapOfShape anIt1(myShapeMap), anIt2(myToolMap);

    for(; anIt1.More(); anIt1.Next()) {
      B.Add(aCoShape, anIt1.Key());
    }

    for(; anIt2.More(); anIt2.Next()) {
      B.Add(aCoTool, anIt2.Key());
    }

    if (St2 != TopAbs_OUT) myAlgo.Intersect(aCoShape, aCoTool);
    else                   myAlgo.Intersect(aCoTool, aCoShape);

//-------------------------------------------------------------------

    for (its.Initialize(newFS); its.More(); its.Next()) {
      mapUsedShape.Add(its.Value());
    }
    for (itt.Initialize(newFT); itt.More(); itt.Next()) {
      mapUsedTool.Add(itt.Value());
    }

    newFS.Clear();
    newFT.Clear();
    
    const BOPTools_PDSFiller& aFiller = myAlgo.DSFiller();
    const BooleanOperations_ShapesDataStructure& aDS = aFiller->DS();
    const BOPTools_InterferencePool& anIntrf =  aFiller->InterfPool();
    //Checks if the set of faces is complete
    for (Standard_Integer i=1; i<=EFMap.Extent(); i++) {
      const TopoDS_Edge& Ed = TopoDS::Edge(EFMap.FindKey(i));

      Standard_Integer index = aDS.ShapeIndex(Ed, 1);
      if(index <= 0)  index = aDS.ShapeIndex(Ed, 2);
      if(index > 0 ) {

	if(!anIntrf.HasInterference(index)) continue;

	const TopTools_ListOfShape& Lf = EFMap.FindFromIndex(i);
	Standard_Boolean InShape = Standard_False;
	for (its.Initialize(Lf); its.More(); its.Next()) {
	  if (mapUsedShape.Contains(its.Value())) {
	    InShape = Standard_True;
	    break;
	  }
	  else if (mapUsedTool.Contains(its.Value())) {
	    InShape = Standard_False;
	    break;
	  }
	}
	if (!its.More()) {
	  Standard_ConstructionError::Raise();
	}

	for (its.Initialize(Lf); its.More(); its.Next()) {
	  if (InShape) {
	    if (!mapUsedShape.Contains(its.Value())) {
	      for (itm.Initialize(mapShape);itm.More(); itm.Next()) {
		if (itm.Key().IsSame(its.Value())) {
		  break;
		}
	      }
	      TopAbs_Orientation orient = itm.Key().Orientation();
	      TopoDS_Face newF = TopoDS::Face(its.Value());
	      newF.Orientation(orient);
	      if (mapAdded.Add(newF)) {
		newFS.Append(newF);
	      }
	    }
	  }
	  else {
	    if (!mapUsedTool.Contains(its.Value())) {
	      for (itm.Initialize(mapTool);itm.More(); itm.Next()) {
		if (itm.Key().IsSame(its.Value())) {
		  break;
		}
	      }
	      TopAbs_Orientation orient = itm.Key().Orientation();
	      TopoDS_Face newF = TopoDS::Face(its.Value());
	      newF.Orientation(orient);
	      if (mapAdded.Add(newF)) {
		newFT.Append(newF);
	      }
	    }
	  }
	}
      }
    }
  } while (!(newFS.IsEmpty() && newFT.IsEmpty()));


  //-----------------------------------------
  myAlgo.ToCompleteIntersection();
  //-----------------------------------------


  myPerfdone = Standard_True;
  
}


//=======================================================================
//function : BuildPartsOfTool
//purpose  : 
//=======================================================================

void LocOpe_Builder::BuildPartsOfTool ()
{
  if (!myPerfdone) {
    StdFail_NotDone::Raise();
  }

  myPdone = Standard_False;
  myParts.Clear();
  myRemoved.Clear();

  TopAbs_State St2 = (myFuse) ? TopAbs_OUT : TopAbs_IN; 

  TopoDS_Shape SS;
  static Standard_Boolean OK;
  OK = Standard_True;

  try {
    OCC_CATCH_SIGNALS
    if (St2 != TopAbs_OUT) {
      SS = myAlgo.Merge(TopAbs_IN, St2);
    }
    else {
      SS = myAlgo.Merge(St2, TopAbs_IN);
    }
  }
  catch (Standard_Failure){
    OK = Standard_False;
  }


//--------------------------------------------
#ifdef DRAW
      DBRep::Set("SS", SS);
#endif
//------------------------------------------------------------
  TopTools_MapOfShape DummyMap;
  if (!SS.IsNull()) SS = BRepAlgo_Tool::Deboucle3D(SS, DummyMap);
  if (SS.IsNull()) OK = Standard_False;
  
  // Est-ce que le shape est valide ?
  TopExp_Explorer ex;
  if (OK) {
    ex.Init(SS, TopAbs_FACE);
    OK = ex.More();
    if (OK) {
      if (!BRepAlgo::IsTopologicallyValid(SS)) {
	OK = Standard_False;
#ifdef DRAW
    cout <<"LocOpe_Builder::BuildPartsOfTool: Shape non valide, on intersecte tout."<< endl;
#endif
      }
    }
    else {
#ifdef DRAW
    cout <<"LocOpe_Builder: Pas de faces dans le resultat, on intersecte tout."<< endl;
#endif
    }
  }


  if (!OK) {
    myAlgo.Init();
    if (St2 != TopAbs_OUT) {
      myAlgo.Load(myShape, myTool);
      myAlgo.Intersect();
      SS = myAlgo.Merge(TopAbs_IN, St2);
    }
    else {
      myAlgo.Load(myTool, myShape);
      myAlgo.Intersect();
      SS = myAlgo.Merge(St2, TopAbs_IN);
    }
  }

  TopExp_Explorer exp;
  for (exp.Init(SS,TopAbs_SHELL); 
       exp.More(); exp.Next()) {
    TopoDS_Shape S1 = exp.Current();
    if (!S1.IsNull())	S1 = BRepAlgo_Tool::Deboucle3D(S1, DummyMap);
    if (!S1.IsNull()) myParts.Append(S1);
  }

  myPdone = Standard_True;
}


//=======================================================================
//function : PartsOfTool
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& LocOpe_Builder::PartsOfTool () const
{
  if (!myPdone) {
    StdFail_NotDone::Raise();
  }
  return myParts;
}


//=======================================================================
//function : RemovePart
//purpose  : 
//=======================================================================

void LocOpe_Builder::RemovePart (const TopoDS_Shape& S)
{
  if (!myPdone) {
    StdFail_NotDone::Raise();
  }

  TopTools_ListIteratorOfListOfShape its(myParts);
  for (;its.More(); its.Next()) {
    if (S.IsSame(its.Value())) {
      break;
    }
  }
  if (!its.More()) { // S ne peut etre enlever
#ifdef DRAW    
    cout << "Invalid Shape" << endl;
#endif
  }
  else {
    for (its.Initialize(myRemoved); its.More(); its.Next()) {
      if (S.IsSame(its.Value())) {
	break;
      }
    }
    if (its.More()) { // S deja enleve
#ifdef DRAW    
      cout << "S has already been removed" << endl;
#endif
    }
    else {
      myRemoved.Append(S);
    }
  }
}



//=======================================================================
//function : ActivatePart
//purpose  : 
//=======================================================================

void LocOpe_Builder::ActivatePart (const TopoDS_Shape& S)
{
  if (!myPdone) {
    StdFail_NotDone::Raise();
  }
  TopTools_ListIteratorOfListOfShape its(myRemoved);
  for (;its.More(); its.Next()) {
    if (S.IsSame(its.Value())) {
      break;
    }
  }
  if (!its.More()) { // S ne peut etre re-active
#ifdef DRAW    
    cout << "Invalid Shape" << endl;
#endif
  }
  else {
    myRemoved.Remove(its);
  }
}


//=======================================================================
//function : PerformResult
//purpose  : 
//=======================================================================

void LocOpe_Builder::PerformResult()
{
  if (!myPerfdone) {
    StdFail_NotDone::Raise();
  }
  

  InvalidateParts();

  TopTools_MapOfShape DummyMap;
  Standard_Boolean OK = Standard_True;
  TopAbs_State St1;

  if (!myShape.IsSame(myTool)) {
    St1 = TopAbs_OUT;
    TopAbs_State St2 = (myFuse) ? TopAbs_OUT : TopAbs_IN;

    if (St2 != TopAbs_OUT) myResult = myAlgo.Merge(St1, St2);
    else                   myResult = myAlgo.Merge(St2, St1);

#ifdef DRAW
    DBRep::Set("res1", myResult);
#endif
    if (!myResult.IsNull())
      myResult = BRepAlgo_Tool::Deboucle3D(myResult, DummyMap);
#ifdef DRAW
    DBRep::Set("res2", myResult);
#endif

    if (myResult.IsNull()) OK = Standard_False;

    if (OK) {
      if (!BRepAlgo::IsTopologicallyValid(myResult)) {
	OK = Standard_False;
#ifdef DRAW
	cout <<"LocOpe_Builder::PerformResult: Shape non valide, on intersecte tout."<< endl;
#endif
      }
    }
/*
    if (!OK) {
      myShapeMap.Clear();
      myAlgo.Init();
      TopExp_Explorer anExp(myShape, TopAbs_FACE);
      for(; anExp.More(); anExp.Next()) {
	myShapeMap.Add(anExp.Current());
      }
      if (St2 != TopAbs_OUT) {
	myAlgo.Load(myShape, myTool);
	myAlgo.Intersect();
	InvalidateParts();
 	myResult = myAlgo.Merge(St1, St2);
      }
      else {
	myAlgo.Load(myTool, myShape);
	myAlgo.Intersect();
	InvalidateParts();
	myResult = myAlgo.Merge(St2, St1);
      }
#ifdef DRAW
  DBRep::Set("res3", myResult);
#endif
      if (!myResult.IsNull()) myResult = BRepAlgo_Tool::Deboucle3D(myResult, DummyMap);
#ifdef DRAW
  DBRep::Set("res4", myResult);
#endif
    }
*/
  }
  else {
    St1 = (myFuse) ? TopAbs_OUT : TopAbs_IN;
    myResult = myAlgo.Merge(St1); // debouclage
#ifdef DRAW
    DBRep::Set("res5", myResult);
#endif
    if (!myResult.IsNull()) myResult = BRepAlgo_Tool::Deboucle3D(myResult, DummyMap);
#ifdef DRAW
    DBRep::Set("res6", myResult);
#endif
  }


#ifdef DRAW
  DBRep::Set("myRes", myResult);
#endif


  myEdges.Clear();
  myTgtEdges.Clear();
  TopExp_Explorer ex, ex1;

  if (myResult.IsNull() || !OK) {
    myDone = Standard_False;
  }
  else {

    TopTools_ListIteratorOfListOfShape is(myAlgo.GetSectionEdgeSet());
    while (is.More()) {
      for (ex.Init(is.Value(), TopAbs_EDGE); ex.More(); ex.Next()) {
	const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
	ex1.Init(myResult, TopAbs_EDGE);
	for(; ex1.More(); ex1.Next()) {
	  if(E.IsSame(ex1.Current()))  break;
	}
	if(ex1.More()) {
	  myEdges.Append(E);
	}
      }
      is.Next();
    }
    
    TopTools_IndexedDataMapOfShapeListOfShape ma;
    TopTools_ListIteratorOfListOfShape its3;
    
    TopExp::MapShapesAndAncestors(myResult, TopAbs_EDGE, TopAbs_FACE, ma);
    its3.Initialize(myEdges);
    for(; its3.More(); its3.Next()) {
      const TopoDS_Edge& edg = TopoDS::Edge(its3.Value());
      const TopTools_ListOfShape& L0 = ma.FindFromKey(its3.Value());
      const TopoDS_Face& F1 = TopoDS::Face(L0.First());
      const TopoDS_Face& F2 = TopoDS::Face(L0.Last());
      if(!F1.IsSame(F2)) {
	if(LocOpe::TgtFaces(edg, F1, F2)) {
	  myTgtEdges.Append(its3.Value());
	}
      }
    }
    myDone = Standard_True;
  }

  if (!myInvDone) {
    myDone = Standard_False;
  }

}



//=======================================================================
//function : InvalidateParts
//purpose  : 
//=======================================================================

void LocOpe_Builder::InvalidateParts ()
{

  myInvDone = Standard_True;
  const TopTools_ListOfShape& L = myAlgo.GetSectionEdgeSet();
  TopTools_ListOfShape *pL = (TopTools_ListOfShape*)&L;

  TopTools_ListIteratorOfListOfShape its;

#ifdef DRAW
  char namep[12];
  Standard_Integer ii = 0;
  for (its.Initialize(L); its.More(); its.Next()) {
    ii++;
    sprintf(namep,"CO_%d",ii);
    DBRep::Set(namep, its.Value());
  }

  ii = 0;
  for (its.Initialize(myParts);its.More(); its.Next()) {
    ii++;
    sprintf(namep,"thePart_%d",ii);
    DBRep::Set(namep,its.Value());
  }
#endif


  if (myRemoved.IsEmpty() && myParts.IsEmpty()) {
    return;
  }

  Standard_Integer NbFaces = myShapeMap.Extent();

  TopExp_Explorer exp, exp1;

  its.Initialize(*pL);
  while (its.More()) {
    TopoDS_Compound C = TopoDS::Compound(its.Value());
    Standard_Boolean Finish = Standard_False;

    for (exp.Init(C,TopAbs_VERTEX); !Finish && exp.More(); exp.Next()) {
      gp_Pnt P1 = BRep_Tool::Pnt(TopoDS::Vertex(exp.Current()));
      TopTools_ListIteratorOfListOfShape its2(myRemoved);

      for (; !Finish && its2.More(); its2.Next()) {
	for (exp1.Init(its2.Value(),TopAbs_VERTEX); exp1.More(); exp1.Next()) {
	  gp_Pnt P2 = BRep_Tool::Pnt(TopoDS::Vertex(exp1.Current()));
	  if (Egal(P1, P2)) {
	    // le compound est a enlever.
	    Finish = Standard_True;
	    break;
	  }
	}
      }
    }

    Standard_Boolean ToSuppress = Standard_True;
      
    for (exp.Init(C,TopAbs_VERTEX); ToSuppress && exp.More(); exp.Next()) {
      gp_Pnt P1 = BRep_Tool::Pnt(TopoDS::Vertex(exp.Current()));
// its2 iterateur sur myRemoved = morceaux outils enleves
// its3 iteratuer sur myParts = tous les morceaux outil
      TopTools_ListIteratorOfListOfShape its3(myParts);
      
      for (;ToSuppress && its3.More(); its3.Next()) {
	  
	Standard_Integer myKeep = Standard_True;
	TopTools_ListIteratorOfListOfShape its2(myRemoved);
	for (; its2.More(); its2.Next()) {
	  if (its3.Value() == its2.Value()) myKeep = Standard_False;
	}
	if (!myKeep) continue;
	
	for (exp1.Init(its3.Value(),TopAbs_VERTEX); exp1.More(); exp1.Next()) {
	  gp_Pnt P2 = BRep_Tool::Pnt(TopoDS::Vertex(exp1.Current()));
	  if (Egal(P1, P2)) {
	    // le compound est a garder.
	    ToSuppress = Standard_False;
	    break;
	  }
	}
      }
    }
      
    if (Finish || ToSuppress) {
      myAlgo.SuppressEdgeSet();
      pL->Remove(its);
      RemoveFaces(C, myAlgo.DSFiller(), myShapeMap);
    }

#ifdef DRAW
    if (!Finish && ToSuppress) {
      cout << "LocOpe_Builder::InvalidateParts : suppress intersection line" << endl;
    }
#endif

    if (Finish && !ToSuppress) {
#ifdef DRAW
      cout << "LocOpe_Builder::InvalidateParts : intersection line conflict" << endl;
#endif
      myInvDone = Standard_False;
    }

    if (Finish || ToSuppress)  continue;
    its.Next();
  }

  if(NbFaces > myShapeMap.Extent()) {
    
    TopAbs_State St2 = (myFuse) ? TopAbs_OUT : TopAbs_IN;
    BRep_Builder B;
 
    TopoDS_Compound aCoShape, aCoTool;
    B.MakeCompound(aCoShape);
    B.MakeCompound(aCoTool);

    TopTools_MapIteratorOfMapOfShape anIt1(myShapeMap), anIt2(myToolMap);

    for(; anIt1.More(); anIt1.Next()) {
      B.Add(aCoShape, anIt1.Key());
    }

    for(; anIt2.More(); anIt2.Next()) {
      B.Add(aCoTool, anIt2.Key());
    }

#ifdef DRAW
    DBRep::Set("aCoShape", aCoShape);
#endif
    myAlgo.Init();
    if (St2 != TopAbs_OUT) {
      myAlgo.Load(myShape, myTool);
    }
    else {
      myAlgo.Load(myTool, myShape);
    }

    if (St2 != TopAbs_OUT) myAlgo.Intersect(aCoShape, aCoTool);
    else                   myAlgo.Intersect(aCoTool, aCoShape);

    myAlgo.ToCompleteIntersection();

  }
}


