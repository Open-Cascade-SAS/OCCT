// Created on: 2001-04-09
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <BOP_WireSplitter.ixx>

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_Orientation.hxx>

#include <BRep_Tool.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TColgp_SequenceOfPnt2d.hxx>

#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BOPTColStd_ListOfListOfShape.hxx>
#include <BOPTColStd_ListIteratorOfListOfListOfShape.hxx>

#include <BOPTools_Tools2D.hxx>

#include <BOP_EdgeInfo.hxx>
#include <BOP_ListOfEdgeInfo.hxx>
#include <BOP_ListIteratorOfListOfEdgeInfo.hxx>
#include <BOP_IndexedDataMapOfVertexListEdgeInfo.hxx>


#include <BRepAdaptor_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Precision.hxx>

static
  void Path (const GeomAdaptor_Surface& aGAS,
	     const TopoDS_Face& myFace,
	     const TopoDS_Vertex& aVa,
	     const TopoDS_Edge& aEOuta,
	     BOP_EdgeInfo& anEdgeInfo,
	     TopTools_SequenceOfShape& aLS,
	     TopTools_SequenceOfShape& aVertVa,
	     TColgp_SequenceOfPnt2d& aCoordVa,
	     BOPTColStd_ListOfListOfShape& myShapes,
	     BOP_IndexedDataMapOfVertexListEdgeInfo& mySmartMap);


static
  Standard_Real Angle (const gp_Dir2d& aDir2D);


static
  void GetNextVertex(const TopoDS_Vertex& aV,
		     const TopoDS_Edge& aE,
		     TopoDS_Vertex& aV1);
static
  Standard_Real ClockWiseAngle(const Standard_Real aAngleIn,
			       const Standard_Real aAngleOut);

static
  Standard_Real AngleIn(const TopoDS_Edge& aEIn,
			const BOP_ListOfEdgeInfo& aLEInfo);

static
  Standard_Real Angle2D (const TopoDS_Vertex& aV,
			 const TopoDS_Edge& anEdge,
			 const TopoDS_Face& myFace,
			 const GeomAdaptor_Surface& aGAS,
			 const Standard_Boolean aFlag);
static
  gp_Pnt2d Coord2d (const TopoDS_Vertex& aV1,
		    const TopoDS_Edge& aE1,
		    const TopoDS_Face& aF);
static
  gp_Pnt2d Coord2dVf (const TopoDS_Edge& aE,
		      const TopoDS_Face& aF);
static
  Standard_Real Tolerance2D (const TopoDS_Vertex& aV,
			    const GeomAdaptor_Surface& aGAS);	

static
Standard_Real UTolerance2D (const TopoDS_Vertex& aV,
			   const GeomAdaptor_Surface& aGAS);
static
Standard_Real VTolerance2D (const TopoDS_Vertex& aV,
			   const GeomAdaptor_Surface& aGAS);

static
Standard_Boolean RecomputeAngles(const BOP_ListOfEdgeInfo& aLEInfo, 
				 const TopoDS_Face&        theFace, 
				 const gp_Pnt2d&           thePb, 
				 const TopoDS_Vertex&      theVb,
				 const GeomAdaptor_Surface& theGAS,
				 const TopoDS_Edge&        theEOuta, 
				 const Standard_Boolean&   bHasClosed,
				 const Standard_Real&      theTol2D,
				 TColStd_SequenceOfReal&   theRecomputedAngles);
//
static
  Standard_Integer NbWaysOut(const BOP_ListOfEdgeInfo& );
//

//=======================================================================
// function: DoWithFace
// purpose: 
//=======================================================================
  void BOP_WireSplitter::DoWithFace()
{
  myEdges.Clear();

  TopExp_Explorer anExpEdges (myFace, TopAbs_EDGE);
  for (; anExpEdges.More(); anExpEdges.Next()) {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExpEdges.Current());
    //
    if (anEdge.Orientation()==TopAbs_INTERNAL){
      continue;
    }
    //
    myEdges.Append(anEdge);
  }
  Do();
}
//=======================================================================
// function: DoWithListOfEdges
// purpose: 
//=======================================================================
  void BOP_WireSplitter::DoWithListOfEdges(const TopTools_ListOfShape& aLE)
{
  myEdges.Clear();
 
  TopTools_ListIteratorOfListOfShape anItList;

  anItList.Initialize(aLE);
  for (; anItList.More(); anItList.Next()) {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anItList.Value());
    //
    if (anEdge.Orientation()==TopAbs_INTERNAL){
      continue;
    }
    //
    myEdges.Append(anEdge);
  }
  Do();
}
//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOP_WireSplitter::Do()
{
  myIsDone=Standard_False;
  myNothingToDo=Standard_True;

  Standard_Integer index, i, aNb, aCntIn, aCntOut;
  Standard_Boolean anIsIn;
  Standard_Real anAngle;
  
  BOP_ListOfEdgeInfo emptyInfo;
  TopTools_ListIteratorOfListOfShape anItList;
  //
  // 1.Filling mySmartMap
  mySmartMap.Clear();

  anItList.Initialize(myEdges);
  for (; anItList.More(); anItList.Next()) {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anItList.Value());
    //
    if (!BOPTools_Tools2D::HasCurveOnSurface (anEdge, myFace)) {
      continue;
    }
    //
    TopExp_Explorer anExpVerts (anEdge, TopAbs_VERTEX);
    for (; anExpVerts.More(); anExpVerts.Next()) {
      const TopoDS_Shape& aVertex= anExpVerts.Current();

      index = mySmartMap.FindIndex(aVertex);
      if (!index) {
	index=mySmartMap.Add(aVertex, emptyInfo);
      }
      
      BOP_ListOfEdgeInfo& aListOfEInfo=mySmartMap(index);

      BOP_EdgeInfo aEInfo;
      aEInfo.SetEdge(anEdge);
      
      TopAbs_Orientation anOr=aVertex.Orientation();

      if (anOr==TopAbs_FORWARD) {
	aEInfo.SetInFlag(Standard_False);
      }

      else if (anOr==TopAbs_REVERSED) {
	aEInfo.SetInFlag(Standard_True);
      }

      aListOfEInfo.Append(aEInfo);
    }
  }
  //
  aNb=mySmartMap.Extent();
  //
  // 2. myNothingToDo 
  myNothingToDo=Standard_True;
  
  for (i=1; i<=aNb; i++) {
    aCntIn=0;
    aCntOut=0;
    const BOP_ListOfEdgeInfo& aLEInfo= mySmartMap(i);
    BOP_ListIteratorOfListOfEdgeInfo anIt(aLEInfo);
    for (; anIt.More(); anIt.Next()) {
      const BOP_EdgeInfo& anEdgeInfo=anIt.Value();
      anIsIn=anEdgeInfo.IsIn();
      if (anIsIn) {
	aCntIn++;
      }
      else {
	aCntOut++;
      }
    }
    if (aCntIn!=1 || aCntOut!=1) {
      myNothingToDo=Standard_False;
      break;
    }
  }
  //
  // Each vertex has one edge In and one - Out. Good. But it is not enought
  // to consider that nothing to do with this. We must check edges on TShape
  // coinsidence. If there are such edges there is something to do with.
  // 
  if (myNothingToDo) {
    Standard_Integer aNbE, aNbMapEE;
    TopTools_IndexedDataMapOfShapeListOfShape aMapEE;
    aNbE=myEdges.Extent();
    
    anItList.Initialize(myEdges);
    for (; anItList.More(); anItList.Next()) {
      const TopoDS_Shape& aE = anItList.Value();
      
      if (!aMapEE.Contains(aE)) {
	TopTools_ListOfShape aLEx;
	aLEx.Append(aE);
	aMapEE.Add(aE, aLEx);
      }
      else {
	TopTools_ListOfShape& aLEx=aMapEE.ChangeFromKey(aE);
	aLEx.Append(aE);
      }
    }
    
    Standard_Boolean bFlag;
    bFlag=Standard_True;
    aNbMapEE=aMapEE.Extent();
    for (i=1; i<=aNbMapEE; i++) {
      const TopTools_ListOfShape& aLEx=aMapEE(i);
      aNbE=aLEx.Extent();
      if (aNbE==1) {
	// usual case
	continue;
      }
      else if (aNbE==2){
	const TopoDS_Shape& aE1=aLEx.First();
	const TopoDS_Shape& aE2=aLEx.Last();
	if (aE1.IsSame(aE2)) {
	  bFlag=Standard_False;
	  break;
	}
      }
      else {
	bFlag=Standard_False;
	break;
      }
    }
    myNothingToDo=myNothingToDo && bFlag;
  }
  // 
  //
  if (myNothingToDo) {
    myIsDone=Standard_True;
    return;
  }
  //
  // 3. Angles in mySmartMap
  BRepAdaptor_Surface aBAS(myFace);
  const GeomAdaptor_Surface& aGAS=aBAS.Surface();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Vertex& aV=TopoDS::Vertex (mySmartMap.FindKey(i));
    const BOP_ListOfEdgeInfo& aLEInfo= mySmartMap(i);

    BOP_ListIteratorOfListOfEdgeInfo anIt(aLEInfo);
    for (; anIt.More(); anIt.Next()) {
      BOP_EdgeInfo& anEdgeInfo=anIt.Value();
      const TopoDS_Edge& aE=anEdgeInfo.Edge();
      //
      TopoDS_Vertex aVV=aV;
      //
      anIsIn=anEdgeInfo.IsIn();
      if (anIsIn) {
	//
	aVV.Orientation(TopAbs_REVERSED);
	anAngle=Angle2D (aVV, aE, myFace, aGAS, Standard_True);
      }
      // 
      else { // OUT
	//
	aVV.Orientation(TopAbs_FORWARD);
	anAngle=Angle2D (aVV, aE, myFace, aGAS, Standard_False);
      }
      anEdgeInfo.SetAngle(anAngle);
      
    }
  }
  //
  // 4. Do
  //
  Standard_Boolean anIsOut, anIsNotPassed;
  
  TopTools_SequenceOfShape aLS, aVertVa;
  TColgp_SequenceOfPnt2d aCoordVa;
  
  BOP_ListIteratorOfListOfEdgeInfo anIt;

  for (i=1; i<=aNb; i++) {
    const TopoDS_Vertex aVa=TopoDS::Vertex (mySmartMap.FindKey(i));
    const BOP_ListOfEdgeInfo& aLEInfo=mySmartMap(i);
    
    anIt.Initialize(aLEInfo);
    for (; anIt.More(); anIt.Next()) {
      BOP_EdgeInfo& anEdgeInfo=anIt.Value();
      const TopoDS_Edge& aEOuta=anEdgeInfo.Edge();
      
      anIsOut=!anEdgeInfo.IsIn();
      anIsNotPassed=!anEdgeInfo.Passed();
      
      if (anIsOut && anIsNotPassed) {
	//
	aLS.Clear();
	aVertVa.Clear();
	aCoordVa.Clear();
	//
	Path(aGAS, myFace, aVa, aEOuta, anEdgeInfo, aLS, 
	     aVertVa, aCoordVa, myShapes, mySmartMap);
      }
    }
  }
  //
  {
    Standard_Integer aNbV, aNbE;
    TopoDS_Vertex aV1, aV2;
    BOPTColStd_ListOfListOfShape aShapes;
    BOPTColStd_ListIteratorOfListOfListOfShape anItW(myShapes);
    
    for (; anItW.More(); anItW.Next()) {
      TopTools_IndexedMapOfShape aMV, aME;
      const TopTools_ListOfShape& aLE=anItW.Value();
      TopTools_ListIteratorOfListOfShape anItE(aLE);
      for (; anItE.More(); anItE.Next()) {
	const TopoDS_Edge& aE=TopoDS::Edge(anItE.Value());
	aME.Add(aE);
	TopExp::Vertices(aE, aV1, aV2);
	aMV.Add(aV1);
	aMV.Add(aV2);
      }
      aNbV=aMV.Extent();
      aNbE=aME.Extent();
      if (aNbV<=aNbE) {
	aShapes.Append(aLE);
      }
    }
    //
    myShapes.Clear();
    anItW.Initialize(aShapes);
    for (; anItW.More(); anItW.Next()) {
      const TopTools_ListOfShape& aLE=anItW.Value();
      myShapes.Append(aLE);
    }
  }
  //
  myIsDone=Standard_True;
}
//=======================================================================
// function: Path
// purpose: 
//=======================================================================
  void Path (const GeomAdaptor_Surface& aGAS,
	     const TopoDS_Face& myFace,
	     const TopoDS_Vertex& aVa,
	     const TopoDS_Edge& aEOuta,
	     BOP_EdgeInfo& anEdgeInfo,
	     TopTools_SequenceOfShape& aLS,
	     TopTools_SequenceOfShape& aVertVa,
	     TColgp_SequenceOfPnt2d& aCoordVa,
	     BOPTColStd_ListOfListOfShape& myShapes,
	     BOP_IndexedDataMapOfVertexListEdgeInfo& mySmartMap)
			       
{
  Standard_Integer i,j, aNb, aNbj;
  Standard_Real aD, aTol=1.e-7, anAngleIn, anAngleOut, anAngle, aMinAngle; 
  Standard_Real aTol2D, aTolVb, aTolVPrev;
  Standard_Boolean anIsSameV2d, anIsSameV, anIsFound, anIsOut, anIsNotPassed;
  BOP_ListIteratorOfListOfEdgeInfo anIt;
  
  TopoDS_Vertex aVb;
  TopoDS_Edge aEOutb;
  //
  // append block
  //
  // Do not escape through edge from which you enter 
  aNb=aLS.Length();
  if (aNb==1) {
    const TopoDS_Shape& anEPrev=aLS(aNb);

    if (anEPrev.IsSame(aEOuta)) {
      return;
    }
  }
  //
  //
  anEdgeInfo.SetPassed(Standard_True);
  aLS.Append(aEOuta);
  aVertVa.Append(aVa);
  
  TopoDS_Vertex pVa=aVa;
  pVa.Orientation(TopAbs_FORWARD);
  gp_Pnt2d aPa=Coord2d(pVa, aEOuta, myFace);
  aCoordVa.Append(aPa);
  
  GetNextVertex (pVa, aEOuta, aVb);

  gp_Pnt2d aPb=Coord2d(aVb, aEOuta, myFace);

  const BOP_ListOfEdgeInfo& aLEInfoVb=mySmartMap.FindFromKey(aVb);

  TopoDS_Vertex aV1, aV2;
  TopExp::Vertices(aEOuta, aV1, aV2);
  Standard_Boolean bHasClosedEdge = aV1.IsNull() || aV2.IsNull() || aV1.IsSame(aV2);
  Standard_Boolean bHasDegenerated = BRep_Tool::Degenerated(aEOuta);
  Standard_Boolean bHasSeam = BRep_Tool::IsClosed(aEOuta, myFace);
  anIt.Initialize(aLEInfoVb);
  
  for (; anIt.More(); anIt.Next()) {
    BOP_EdgeInfo& anEI=anIt.Value();
    const TopoDS_Edge& aE=anEI.Edge();
    bHasDegenerated = bHasDegenerated || BRep_Tool::Degenerated(aE);
    bHasSeam = bHasSeam || BRep_Tool::IsClosed(aE, myFace);
    aV1.Nullify();
    aV2.Nullify();
    TopExp::Vertices(aE, aV1, aV2);
    bHasClosedEdge = bHasClosedEdge || aV1.IsNull() || aV2.IsNull() || aV1.IsSame(aV2);
  }

  aNb=aLS.Length();
  if (aNb>0) {
    //
    TopTools_ListOfShape aBuf;
    for (i=aNb; i>0; i--) {
      const TopoDS_Shape& aVPrev=aVertVa(i);
      const gp_Pnt2d& aPaPrev=aCoordVa(i);
      const TopoDS_Shape& aEPrev=aLS(i);

      aBuf.Append(aEPrev);

      anIsSameV=aVPrev.IsSame(aVb);
      anIsSameV2d = Standard_False;

      if (anIsSameV) {
	anIsSameV2d = Standard_True;

	if(bHasDegenerated || bHasSeam || bHasClosedEdge) {
	  aTolVb   =BRep_Tool::Tolerance(TopoDS::Vertex(aVb));
	  aTolVPrev=BRep_Tool::Tolerance(TopoDS::Vertex(aVPrev));
	  aTol=aTolVb+aTolVPrev;
	  //
	  aTol=2.*Tolerance2D(aVb, aGAS);
	  aD=aPaPrev.Distance(aPb);
	  anIsSameV2d = (aD < aTol);

	  if(anIsSameV2d) {
	    Standard_Real udist = fabs(aPaPrev.X() - aPb.X());
	    Standard_Real vdist = fabs(aPaPrev.Y() - aPb.Y());
	    Standard_Real aTolU = 2. * UTolerance2D(aVb, aGAS);
	    Standard_Real aTolV = 2. * VTolerance2D(aVb, aGAS);

	    if((udist > aTolU) ||
	       (vdist > aTolV)) {
	      anIsSameV2d = Standard_False;
	    }
	  }
	}
      }

      //
      if (anIsSameV && anIsSameV2d) {
	myShapes.Append(aBuf);
	//
	TopTools_SequenceOfShape aLSt, aVertVat;
	TColgp_SequenceOfPnt2d aCoordVat;
	//
	aNbj=i-1;
	if (aNbj<1) {
	  //
	  aLS.Clear();
	  aVertVa.Clear();
	  aCoordVa.Clear();
	  //
	  return;
	}

	aVb=TopoDS::Vertex(aVertVa(i));

	for (j=1; j<=aNbj; j++) {
	  aLSt.Append(aLS(j));
	  aVertVat.Append(aVertVa(j));
	  aCoordVat.Append(aCoordVa(j));
	}
	//
	aLS.Clear();
	aVertVa.Clear();
	aCoordVa.Clear();

	aLS=aLSt;
	aVertVa=aVertVat;
	aCoordVa=aCoordVat;
	//
	break;
      }
    }
  }
  //
  aTol2D=2.*Tolerance2D(aVb, aGAS);
  //
  // anAngleIn in Vb from edge aEOuta
  const BOP_ListOfEdgeInfo& aLEInfo=mySmartMap.FindFromKey(aVb);
  //
  anAngleIn=AngleIn(aEOuta, aLEInfo);
  //
  // aEOutb
  BOP_EdgeInfo *pEdgeInfo=NULL;

  aMinAngle=100.;
  anIsFound=Standard_False;

  TColStd_SequenceOfReal aRecomputedAngles;

  Standard_Boolean bRecomputeAngle = 
    RecomputeAngles(aLEInfo, myFace, aPb, aVb, aGAS, aEOuta, 
		    (bHasDegenerated || bHasSeam || bHasClosedEdge),
		    aTol2D, aRecomputedAngles);

  Standard_Integer aCurIndexE = 0;

  anIt.Initialize(aLEInfo);
  for (; anIt.More(); anIt.Next()) {
    BOP_EdgeInfo& anEI=anIt.Value();
    const TopoDS_Edge& aE=anEI.Edge();
    anIsOut=!anEI.IsIn();
    anIsNotPassed=!anEI.Passed();
    
    if (anIsOut && anIsNotPassed) {
      aCurIndexE++;
      //
      // Is there one way to go out of the vertex 
      // we have to use it only.
      Standard_Integer iCnt;
      iCnt=NbWaysOut (aLEInfo);
      //
      if (!iCnt) {
	// no way to go . (Error)
	return ;
      }
      //
      if (iCnt==1) {
	// the one and only way to go out .
	pEdgeInfo=&anEI;
	anIsFound=Standard_True;
	break;
      }
      //
      // Look for minimal angle and make the choice.
      gp_Pnt2d aP2Dx;
      //
      aP2Dx=Coord2dVf(aE, myFace);
      //
      aD=aP2Dx.Distance(aPb);
      if (aD > aTol2D){
	continue;
      }
      //
      //
      anAngleOut=anEI.Angle();
      //
      if(bRecomputeAngle) {
	if(aCurIndexE <= aRecomputedAngles.Length()) {
	  anAngleOut = aRecomputedAngles.Value(aCurIndexE);
	}
      }
      //
      anAngle=ClockWiseAngle(anAngleIn, anAngleOut);
      if (anAngle < aMinAngle) {
	aMinAngle=anAngle;
	pEdgeInfo=&anEI;
	anIsFound=Standard_True;
      }
    }
  } // for (; anIt.More(); anIt.Next()) 
  //
  if (!anIsFound) {
    // no way to go . (Error)
    return;
  }
  
  aEOutb=pEdgeInfo->Edge();
  Path (aGAS, myFace, aVb, aEOutb, *pEdgeInfo, aLS, 
	aVertVa, aCoordVa, myShapes, mySmartMap);
}
//=======================================================================
// function:  Coord2dVf
// purpose:
//=======================================================================
 gp_Pnt2d Coord2dVf (const TopoDS_Edge& aE,
                     const TopoDS_Face& aF)
{
  TopExp_Explorer anExp(aE, TopAbs_VERTEX);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Vertex& aVx=TopoDS::Vertex(anExp.Current());
    if (aVx.Orientation()==TopAbs_FORWARD)
       return Coord2d(aVx, aE, aF);
  }
  const Standard_Real aCoord=99.;
  const gp_Pnt2d aP2D1(aCoord, aCoord);
  return aP2D1;
}
//=======================================================================
// function:  Tolerance2D
// purpose:
//=======================================================================
 Standard_Real Tolerance2D (const TopoDS_Vertex& aV,
                            const GeomAdaptor_Surface& aGAS)
{
  const GeomAbs_SurfaceType aType=aGAS.GetType();
  const Standard_Real aTolV3D=BRep_Tool::Tolerance(aV);
  const Standard_Real aUr=aGAS.UResolution(aTolV3D);
  const Standard_Real aVr=aGAS.VResolution(aTolV3D);
  //
  Standard_Real aTol2D=(aVr>aUr) ? aVr : aUr;
  //
  if (aType==GeomAbs_BSplineSurface || aType==GeomAbs_Sphere) {
    if (aTol2D < aTolV3D)
      aTol2D=aTolV3D;
  }
  //modified by NIZNHY-PKV Wed Jul  5 16:44:59 2006f
  else if (aType==GeomAbs_BSplineSurface) {
    aTol2D=1.1*aTol2D;
  }
  //modified by NIZNHY-PKV Wed Jul  5 16:45:02 2006t
  //
  return aTol2D;
}

//=======================================================================
// function:  Coord2d
// purpose:
//=======================================================================
 gp_Pnt2d Coord2d (const TopoDS_Vertex& aV1,
                   const TopoDS_Edge& aE1,
                   const TopoDS_Face& aF)
{
  const Standard_Real t=BRep_Tool::Parameter (aV1, aE1, aF);

  Standard_Real aFirst, aLast, aToler;
  Handle(Geom2d_Curve) aC2D;
  BOPTools_Tools2D::CurveOnSurface 
    (aE1, aF, aC2D, aFirst, aLast, aToler, Standard_True);

  gp_Pnt2d aP2D1;
  aC2D->D0 (t, aP2D1);

  return aP2D1;
}
//=======================================================================
// function:  AngleIn
// purpose:
//=======================================================================
Standard_Real AngleIn(const TopoDS_Edge& aEIn,
                      const BOP_ListOfEdgeInfo& aLEInfo)
{
  BOP_ListIteratorOfListOfEdgeInfo anIt(aLEInfo);
  for (; anIt.More(); anIt.Next()) {
    const BOP_EdgeInfo& anEdgeInfo=anIt.Value();
    const TopoDS_Edge& aE=anEdgeInfo.Edge();
    const Standard_Boolean anIsIn=anEdgeInfo.IsIn();
    //
    if (anIsIn && aE==aEIn)
      return anEdgeInfo.Angle();
  }
  return 0.;
}
//=======================================================================
// function:  ClockWiseAngle
// purpose:
//=======================================================================
Standard_Real ClockWiseAngle(const Standard_Real aAngleIn,
                             const Standard_Real aAngleOut)
{
  const Standard_Real aTwoPi = M_PI + M_PI;
  Standard_Real dA, A1, A2, AIn, AOut ;

  AIn=aAngleIn;
  AOut=aAngleOut;
  if (AIn >= aTwoPi) {
    AIn=AIn-aTwoPi;
  }
  
  if (AOut >= aTwoPi) {
    AOut=AOut-aTwoPi;
  }

  A1 = AIn + M_PI;
  
  if (A1 >= aTwoPi) {
    A1=A1-aTwoPi;
  }
  
  A2=AOut;
  
  dA=A1-A2;
  if (dA <= 0.) {
    dA=aTwoPi+dA;
  }
  //xx
  //else if (dA <= 1.e-15) {
  else if (dA <= 1.e-14) {
    dA=aTwoPi;
  }
  return dA;
}
//=======================================================================
// function: GetNextVertex
// purpose: 
//=======================================================================
 void GetNextVertex(const TopoDS_Vertex& aV,
                    const TopoDS_Edge& aE,
                    TopoDS_Vertex& aV1)
{
  TopExp_Explorer anExp(aE, TopAbs_VERTEX);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Vertex& aVx=TopoDS::Vertex(anExp.Current());
    if (!aVx.IsEqual(aV)) {
      aV1=aVx;
      return ;
    }
  }
  aV1=aV;
}
//=======================================================================
// function: Angle2D
// purpose: 
//=======================================================================
Standard_Real Angle2D (const TopoDS_Vertex& aV,
                       const TopoDS_Edge& anEdge,
                       const TopoDS_Face& myFace,
                       const GeomAdaptor_Surface& aGAS,
                       const Standard_Boolean aFlag)
{
  const Standard_Real aTV=BRep_Tool::Parameter (aV, anEdge, myFace);
  if (Precision::IsInfinite(aTV))
    return 0.;

  Handle(Geom2d_Curve) aC2D;
  Standard_Real aFirst, aLast, aToler;
  BOPTools_Tools2D::CurveOnSurface (anEdge, myFace, aC2D, aFirst, aLast, aToler, Standard_True);
  if (aC2D.IsNull())
    return 0.;

  //dt=1.e-7;
  Standard_Real dt=Tolerance2D(aV, aGAS);
  const Standard_Real dtmax=(aLast - aFirst) * 0.25;
  if(dt > dtmax) {
    // to save direction of the curve as much as it possible
    // in the case of big tolerances
    dt = dtmax;
  }
  const Standard_Real aTV1 = (fabs (aTV-aFirst) < fabs(aTV - aLast))? aTV + dt : aTV - dt;
  //
  gp_Pnt2d aPV, aPV1;
  aC2D->D0 (aTV, aPV);
  aC2D->D0 (aTV1, aPV1);
  const gp_XY aV2D( aFlag? (aPV.XY()-aPV1.XY()) : (aPV1.XY()-aPV.XY()) );

  //See http://www.opencascade.org/org/forum/thread_17712/
  if (aV2D.SquareModulus() <= gp::Resolution()*gp::Resolution())
    return 0.;

  const gp_Dir2d aDir2D(aV2D);

  return Angle(aDir2D);
}
//=======================================================================
// function: Angle
// purpose: 
//=======================================================================
Standard_Real Angle (const gp_Dir2d& aDir2D)
{
  const Standard_Real anAngle = gp_Dir2d(1.,0.).Angle(aDir2D);
  return ((anAngle < 0.)? anAngle + M_PI + M_PI : anAngle);
}

//=======================================================================
// function: NbWaysOut
// purpose: 
//=======================================================================
Standard_Integer NbWaysOut(const BOP_ListOfEdgeInfo& aLEInfo)
{
  Standard_Integer iCnt=0;
  //
  BOP_ListIteratorOfListOfEdgeInfo anIt(aLEInfo);
  for (; anIt.More(); anIt.Next()) {
    const BOP_EdgeInfo& anEI=anIt.Value();
    //
    //const TopoDS_Edge& aE=anEI.Edge();
    const Standard_Boolean bIsOut=!anEI.IsIn();
    const Standard_Boolean bIsNotPassed=!anEI.Passed();
    if (bIsOut && bIsNotPassed)
      iCnt++;
  }
  return iCnt;
}

//=======================================================================
//function : UTolerance2D
//purpose  : 
//=======================================================================
Standard_Real UTolerance2D (const TopoDS_Vertex& aV,
                            const GeomAdaptor_Surface& aGAS)
{
  const Standard_Real aTolV3D = BRep_Tool::Tolerance(aV);
  const Standard_Real anUr = aGAS.UResolution(aTolV3D);
  //
  return anUr;
}

//=======================================================================
//function : VTolerance2D
//purpose  : 
//=======================================================================
Standard_Real VTolerance2D (const TopoDS_Vertex& aV,
                            const GeomAdaptor_Surface& aGAS)
{
  const Standard_Real aTolV3D = BRep_Tool::Tolerance(aV);
  const Standard_Real anVr = aGAS.VResolution(aTolV3D);
  //
  return anVr;
}
//=======================================================================
// function: RecomputeAngles
// purpose: 
//=======================================================================
Standard_Boolean RecomputeAngles(const BOP_ListOfEdgeInfo& aLEInfo, 
				 const TopoDS_Face&        theFace, 
				 const gp_Pnt2d&           thePb, 
				 const TopoDS_Vertex&      theVb,
				 const GeomAdaptor_Surface& theGAS,
				 const TopoDS_Edge&        theEOuta, 
				 const Standard_Boolean&   bHasClosed,
				 const Standard_Real&      theTol2D,
				 TColStd_SequenceOfReal&   theRecomputedAngles)
{
  Standard_Boolean bRecomputeAngle = Standard_False;
  BOP_ListIteratorOfListOfEdgeInfo anIt;
  anIt.Initialize(aLEInfo);

  for (; anIt.More(); anIt.Next()) {
    BOP_EdgeInfo& anEI=anIt.Value();
    const TopoDS_Edge& aE=anEI.Edge();
    Standard_Boolean anIsOut=!anEI.IsIn();
    Standard_Boolean anIsNotPassed=!anEI.Passed();
    
    if (anIsOut && anIsNotPassed) {
      theRecomputedAngles.Append(anEI.Angle());
      Standard_Integer acurindex = theRecomputedAngles.Length();

      Standard_Boolean bRecomputeAngleLocal = Standard_False;
      TopExp_Explorer anExp1(aE, TopAbs_VERTEX);

      for(; anExp1.More(); anExp1.Next()) {
	TopExp_Explorer anExp2(theEOuta, TopAbs_VERTEX);
	Standard_Boolean existsInEdge = Standard_False;

	for(; anExp2.More(); anExp2.Next()) {
	  if(anExp1.Current().IsSame(anExp2.Current())) {
	    existsInEdge = Standard_True;
	    break;
	  }
	}
	
	if(!existsInEdge) {
	  bRecomputeAngleLocal = Standard_False;
	  break;
	}
	bRecomputeAngleLocal = Standard_True;
      }
      bRecomputeAngle = bRecomputeAngle || bRecomputeAngleLocal;

      if(!bRecomputeAngle) {
	BOP_ListIteratorOfListOfEdgeInfo anIt2(aLEInfo);
	
	for(; anIt2.More(); anIt2.Next()) {
	  BOP_EdgeInfo& anEI2=anIt2.Value();
	  const TopoDS_Edge& aE2=anEI2.Edge();

	  if(aE2.IsSame(aE))
	    continue;
	  Standard_Boolean anIsOut2=!anEI2.IsIn();
	  Standard_Boolean anIsNotPassed2=!anEI2.Passed();
    
	  if (anIsOut2 && anIsNotPassed2) {
	    anExp1.Init(aE, TopAbs_VERTEX);

	    for(; anExp1.More(); anExp1.Next()) {
	      TopExp_Explorer anExp2(aE2, TopAbs_VERTEX);
	      Standard_Boolean existsInEdge = Standard_False;

	      for(; anExp2.More(); anExp2.Next()) {
		if(anExp1.Current().IsSame(anExp2.Current())) {
		  existsInEdge = Standard_True;
		  break;
		}
	      }
	
	      if(!existsInEdge) {
		bRecomputeAngleLocal = Standard_False;
		break;
	      }
	      bRecomputeAngleLocal = Standard_True;
	    }
	    bRecomputeAngle = bRecomputeAngle || bRecomputeAngleLocal;
	  }
	}
      }

      bRecomputeAngle = bRecomputeAngle || bRecomputeAngleLocal;

      if(bRecomputeAngle) {
	gp_Pnt2d aP2Dx;
	//
	aP2Dx=Coord2dVf(aE, theFace);
	Standard_Real aD = aP2Dx.Distance(thePb);

	TopoDS_Vertex aVf;
	TopExp_Explorer anExp(aE, TopAbs_VERTEX);
      
	for (; anExp.More(); anExp.Next()) {
	  const TopoDS_Vertex& aVx=TopoDS::Vertex(anExp.Current());
	  if (aVx.Orientation()==TopAbs_FORWARD) {
	    aVf = aVx;
	  }
	}
	Standard_Boolean bIgnore = Standard_False;

	if(bHasClosed || aVf.IsNull() || !aVf.IsSame(theVb)) {
	  bIgnore = (aD > theTol2D);
	}

	if(!bIgnore && (theTol2D > M_PI)) {
	  Standard_Real udist = fabs(aP2Dx.X() - thePb.X());
	  Standard_Real vdist = fabs(aP2Dx.Y() - thePb.Y());
	  Standard_Real aTolU = 2. * UTolerance2D(theVb, theGAS);
	  Standard_Real aTolV = 2. * VTolerance2D(theVb, theGAS);
	  
	  if((udist > aTolU) ||
	     (vdist > aTolV)) {
	    bIgnore = Standard_True;
	  }
	}

	if((aD > Precision::Confusion()) && !bIgnore) {
	  Standard_Real f1, l1;
	  Handle(Geom2d_Curve) ac1 = BRep_Tool::CurveOnSurface(aE, theFace, f1, l1);

	  Standard_Real aTV1 = BRep_Tool::Parameter (aVf, aE, theFace);
	  Standard_Real aTV12 = 0.;
	  Standard_Real dt1 = (l1 - f1) * 0.5;

	  if (fabs (aTV1-f1) < fabs(aTV1 - l1)) {
	    aTV12 = aTV1 + dt1;
	  }
	  else {
	    aTV12 = aTV1 - dt1;
	  }

	  gp_Pnt2d aPointNew = ac1->Value(aTV12);
	  gp_Vec2d aV2DOut(thePb, aPointNew);
     
	  gp_Dir2d aDir2D(aV2DOut);
	  Standard_Real anAngleOut = Angle(aDir2D);
	  theRecomputedAngles.ChangeValue(acurindex) = anAngleOut;
	}
      }
    }
  }
  return bRecomputeAngle;
}
