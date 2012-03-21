// Created on: 1993-11-18
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <TopOpeBRep_VPointInterClassifier.ixx>

#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <Standard_ProgramError.hxx>
#include <BRepAdaptor_Surface.hxx>
//modified by NIZHNY-MKK  Fri Jun 16 15:04:09 2000.BEGIN
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <TopoDS_Vertex.hxx>
//modified by NIZHNY-MKK  Fri Jun 16 15:04:09 2000.END


//modified by NIZHNY-MKK  Mon Jun 19 11:47:48 2000.BEGIN
static TopAbs_State SlowClassifyOnBoundary(const gp_Pnt& thePointToClassify, 
					   const gp_Pnt2d& thePoint2dToClassify,
					   BRepClass_FaceClassifier& theSlowClassifier, 
					   const TopoDS_Face& theFace);
//modified by NIZHNY-MKK  Mon Jun 19 11:47:51 2000.END

//=======================================================================
//function : TopOpeBRep_VPointInterClassifier
//purpose  : 
//=======================================================================

TopOpeBRep_VPointInterClassifier::TopOpeBRep_VPointInterClassifier() :
myState(TopAbs_UNKNOWN)
{
}

//=======================================================================
//function : VPointPosition
//purpose  : 
//=======================================================================

TopAbs_State TopOpeBRep_VPointInterClassifier::VPointPosition
(const TopoDS_Shape&  F,
 TopOpeBRep_VPointInter& VP,
 const Standard_Integer FaceClassifyIndex,
 TopOpeBRep_PointClassifier& PC,
 const Standard_Boolean AssumeINON,
 const Standard_Real Tol)
{
  myState = TopAbs_UNKNOWN;

  Standard_Real u,v;
  switch (FaceClassifyIndex) {
  case 1 : { 
    VP.ParametersOnS1(u,v); 
    if(VP.IsOnDomS1()) { 
      VP.State(TopAbs_ON,1);
      const TopoDS_Edge& E = TopoDS::Edge(VP.ArcOnS1());
      const Standard_Real pE = VP.ParameterOnArc1();
      VP.EdgeON(E,pE,1);
      myState = TopAbs_ON;
      return myState;
    }
    break;
  }
  case 2 : {
    VP.ParametersOnS2(u,v); 
    if(VP.IsOnDomS2()) { 
      VP.State(TopAbs_ON,2);
      const TopoDS_Edge& E = TopoDS::Edge(VP.ArcOnS2());
      const Standard_Real pE = VP.ParameterOnArc2();
      VP.EdgeON(E,pE,2);
      myState = TopAbs_ON;
      return myState;
    }
    break;
  }
  default : Standard_ProgramError::Raise("VPointClassifier : wrong Index");
  }

  if (myState == TopAbs_ON) {
    return myState;
  }

  TopoDS_Face FF = TopoDS::Face(F);
  TopOpeBRepTool_ShapeTool::AdjustOnPeriodic(FF,u,v);
  gp_Pnt2d p2d(u,v);
  TopAbs_State statefast = PC.Classify(FF,p2d,Tol);
  myState = statefast;
  VP.State(myState,FaceClassifyIndex);

  // AssumeINON = True <=> on considere que tout VP rendu par les
  // intersections est sur une frontiere de la face FF (ON)ou dans la face FF (IN)  
  Standard_Integer VPSI = VP.ShapeIndex();
  if (AssumeINON && FaceClassifyIndex == VPSI) {
    mySlowFaceClassifier.Perform(FF,p2d,Tol);
    myState = mySlowFaceClassifier.State();
    if      (myState == TopAbs_ON) {
//modified by NIZHNY-MKK  Mon Jun 19 11:45:36 2000.BEGIN
      myState = SlowClassifyOnBoundary(VP.Value(), p2d, mySlowFaceClassifier, FF);      
      if(myState == TopAbs_ON) {
//modified by NIZHNY-MKK  Mon Jun 19 11:45:36 2000.END
	VP.EdgeON(mySlowFaceClassifier.Edge().Edge(),
		  mySlowFaceClassifier.EdgeParameter(),
		  FaceClassifyIndex);
      }
    }
    else if ( myState == TopAbs_OUT) {
      myState = TopAbs_IN; 
      // ou biwn myState = TopAbs_OUT ce qui va entrainer VP.UnKeep()
      // si on privilegie le classifieur / donnees d'intersection.
    }
  } // (AssumeINON && FaceClassifyIndex == VPSI)

  else if (!AssumeINON) {
    if (statefast == TopAbs_OUT || statefast == TopAbs_ON) {
      mySlowFaceClassifier.Perform(FF,p2d,Tol);
      myState = mySlowFaceClassifier.State();
      if      (myState == TopAbs_ON) {
//modified by NIZHNY-MKK  Mon Jun 19 11:45:36 2000.BEGIN
	myState = SlowClassifyOnBoundary(VP.Value(), p2d, mySlowFaceClassifier, FF);
	if(myState == TopAbs_ON) {
//modified by NIZHNY-MKK  Mon Jun 19 11:45:36 2000.END
	  VP.EdgeON(mySlowFaceClassifier.Edge().Edge(),
		    mySlowFaceClassifier.EdgeParameter(),
		    FaceClassifyIndex);
	}
      }
    }
  }
 
  else if (FaceClassifyIndex != VPSI) {
    // AssumeINON = True
    //modified by NIZNHY-PKV Mon Feb  5 19:04:01 2001 f
    if (statefast == TopAbs_ON || statefast == TopAbs_OUT) {
    //if (statefast == TopAbs_ON) {
      mySlowFaceClassifier.Perform(FF, p2d, Tol);
      myState = mySlowFaceClassifier.State();
      
      if (myState == TopAbs_ON || myState == TopAbs_OUT) {
      //if  (myState == TopAbs_ON) {
      //modified by NIZNHY-PKV Mon Feb  5 19:04:49 2001 t
	myState = SlowClassifyOnBoundary(VP.Value(), p2d, mySlowFaceClassifier, FF);
	if(myState==TopAbs_ON) {
	  VP.EdgeON(mySlowFaceClassifier.Edge().Edge(),
		    mySlowFaceClassifier.EdgeParameter(),
		    FaceClassifyIndex);
	}
      }
    }
  }
  
  VP.State(myState,FaceClassifyIndex);
  return myState;
}


//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRep_VPointInterClassifier::Edge() const
{
  if (myState == TopAbs_ON) {
    const TopoDS_Shape& S = mySlowFaceClassifier.Edge().Edge();
    return S;
  }
  else {
    return myNullShape;
  }
}


//=======================================================================
//function : EdgeParameter
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRep_VPointInterClassifier::EdgeParameter() const
{
  if (myState == TopAbs_ON) return mySlowFaceClassifier.EdgeParameter();
  else return 0;
}

//modified by NIZHNY-MKK  Mon Jun 19 11:47:23 2000.BEGIN
//=======================================================================
//static function : SlowClassifyOnBoundary
//purpose  : 
//=======================================================================
static TopAbs_State SlowClassifyOnBoundary(const gp_Pnt& thePointToClassify, 
					   const gp_Pnt2d& thePoint2dToClassify,
					   BRepClass_FaceClassifier& theSlowClassifier, 
					   const TopoDS_Face& theFace) {   

  Standard_Real aParameterOnEdge = theSlowClassifier.EdgeParameter();
  const TopoDS_Edge& anEdge = theSlowClassifier.Edge().Edge();

  Standard_Real parf, parl;
  Handle(Geom_Curve) anEdgeCurve = BRep_Tool::Curve(anEdge, parf, parl);
  
  if(!anEdgeCurve.IsNull()) {
    Standard_Real minparameterdiff = parl - parf;
    Standard_Real aDistanceToCompare = 0;
    Standard_Boolean samewithvertex = Standard_False;
    TopExp_Explorer anExp(anEdge, TopAbs_VERTEX);
    for(;anExp.More() && !samewithvertex; anExp.Next()) {
      TopoDS_Vertex aVertex = TopoDS::Vertex(anExp.Current());
      Standard_Real aVertexTolerance = BRep_Tool::Tolerance(aVertex);
      gp_Pnt anEdgeVertexPoint = BRep_Tool::Pnt(aVertex);
      if(thePointToClassify.IsEqual(anEdgeVertexPoint, aVertexTolerance))
	samewithvertex = Standard_True;
    }
    if(samewithvertex)
      return TopAbs_ON;

    GeomAPI_ProjectPointOnCurve aProjTool(thePointToClassify, anEdgeCurve);

    for(Standard_Integer i=1; i<=aProjTool.NbPoints(); i++) {
      Standard_Real curparamdiff = Abs(aProjTool.Parameter(i) - aParameterOnEdge);
      if(curparamdiff < minparameterdiff) {
	minparameterdiff = curparamdiff;
	  aDistanceToCompare = aProjTool.Distance(i);
      }
    }
    
    Standard_Real anEdgeTolerance = BRep_Tool::Tolerance(anEdge);
      
    if((aProjTool.NbPoints()==0) || (aDistanceToCompare >= anEdgeTolerance)) {
      Handle(Geom2d_Curve) anEdgePCurve = BRep_Tool::CurveOnSurface(anEdge, theFace, parf, parl);
	  
      if(!anEdgePCurve.IsNull()) {
	gp_Pnt2d aPoint2dOnEdge = anEdgePCurve->Value(aParameterOnEdge);
	Standard_Real aTol2d = thePoint2dToClassify.Distance(aPoint2dOnEdge) / 3;
	theSlowClassifier.Perform(theFace, thePoint2dToClassify, aTol2d);
	if(theSlowClassifier.State() == TopAbs_IN)
	  return TopAbs_IN;
	else
	  return TopAbs_OUT;
      }
      else {
	return TopAbs_OUT;
      }
    }
  } //end if(!anEdgeCurve.IsNull())

  return TopAbs_ON;
}
//modified by NIZHNY-MKK  Mon Jun 19 11:47:26 2000.END

