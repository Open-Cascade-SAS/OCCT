// Created on: 1994-10-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRep_EdgesIntersector.hxx>
#include <TopOpeBRep_Point2d.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

//=======================================================================
//function : Segment1
//purpose  : 
//=======================================================================
const IntRes2d_IntersectionSegment& TopOpeBRep_EdgesIntersector::Segment1() const
{
  if ( ! IsPointOfSegment1() ) 
    throw Standard_Failure("TopOpeBRep_EdgesIntersector : Not a segment point");
  Standard_Integer iseg = 1 + (myPointIndex - myNbPoints - 1) / 2;
  return mylseg.Value(iseg);
}

//=======================================================================
//function : IsOpposite1
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_EdgesIntersector::IsOpposite1() const
{
  Standard_Boolean b = Segment1().IsOpposite();
  return b;
}

//=======================================================================
//function : InitPoint1
//purpose  : 
//=======================================================================
void TopOpeBRep_EdgesIntersector::InitPoint1()
{
  myPointIndex = 1;
  myIsVertexPointIndex = 0;
  myIsVertexIndex = 0;
  myIsVertexValue = Standard_False;
}

//=======================================================================
//function : MorePoint1
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_EdgesIntersector::MorePoint1() const 
{
  return myPointIndex <= myTrueNbPoints;
}

//=======================================================================
//function : NextPoint1
//purpose  : 
//=======================================================================
void TopOpeBRep_EdgesIntersector::NextPoint1()
{
  myPointIndex++;
}

//=======================================================================
//function : Point1
//purpose  : 
//=======================================================================
const IntRes2d_IntersectionPoint& TopOpeBRep_EdgesIntersector::Point1() const
{
  if ( ! IsPointOfSegment1() ) { // point is an intersection point
    return mylpnt.Value(myPointIndex);
  }
  else { // point is a point of segment
    Standard_Integer i = myPointIndex - myNbPoints - 1;
    if (i % 2 == 0) return Segment1().FirstPoint();
    else            return Segment1().LastPoint();
  }
}

//=======================================================================
//function : Status1
//purpose  : 
//=======================================================================
TopOpeBRep_P2Dstatus TopOpeBRep_EdgesIntersector::Status1() const
{
  if ( ! IsPointOfSegment1() ) { // point is an intersection point
    return TopOpeBRep_P2DINT;
  }
  else { // point is a point of segment
    Standard_Integer i = myPointIndex - myNbPoints - 1;
    if (i % 2 == 0) return TopOpeBRep_P2DSGF;
    else            return TopOpeBRep_P2DSGL;
  }
}

//=======================================================================
//function : IsPointOfSegment1
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_EdgesIntersector::IsPointOfSegment1() const
{
  Standard_Boolean b = (myPointIndex > myNbPoints);
  return b;
}

//=======================================================================
//function : Index1
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRep_EdgesIntersector::Index1() const
{
  return myPointIndex;
}

//=======================================================================
//function : EdgesConfig1
//purpose  : 
//=======================================================================
TopOpeBRepDS_Config TopOpeBRep_EdgesIntersector::EdgesConfig1() const 
{
  TopOpeBRepDS_Config c = TopOpeBRepDS_UNSHGEOMETRY;
  Standard_Boolean ps = IsPointOfSegment1();
  if ( ps ) {
    Standard_Boolean so;
    so = TopOpeBRepTool_ShapeTool::EdgesSameOriented(myEdge2,myEdge1);
    c = (so) ? TopOpeBRepDS_SAMEORIENTED : TopOpeBRepDS_DIFFORIENTED;
  }
  return c;
}

//=======================================================================
//function : Transition1
//purpose  : 
//=======================================================================
TopOpeBRepDS_Transition TopOpeBRep_EdgesIntersector::Transition1(const Standard_Integer Index,const TopAbs_Orientation EdgeOrientation) const 
{
  Standard_Boolean pointofsegment = IsPointOfSegment1();
  Standard_Boolean pur1d = (pointofsegment && mySameDomain);

  TopAbs_State staB=TopAbs_UNKNOWN,staA=TopAbs_UNKNOWN;
  TopAbs_ShapeEnum shaB=TopAbs_COMPOUND,shaA=TopAbs_COMPOUND; Standard_Boolean pextremity; 
  
  TopAbs_State staINON = TopAbs_IN;
  Standard_Integer dim = myDimension;
  if      (dim == 1)           { shaA = shaB = TopAbs_EDGE; }
  else if (dim == 2 &&  pur1d) { shaA = shaB = TopAbs_EDGE; }
  else if (dim == 2 && !pur1d) { shaA = shaB = TopAbs_FACE; }

  if ( (EdgeOrientation == TopAbs_INTERNAL) || 
       (EdgeOrientation == TopAbs_EXTERNAL) ) {
    TopOpeBRepDS_Transition TR(staINON,staINON,shaB,shaA);
    TR.Set(EdgeOrientation);
    return TR;
  }
  
  pextremity = Standard_False; // JYL290998 corr regr cto100K1 fex6 fex4 : 5eme inters E/E 

  const IntRes2d_IntersectionPoint& IP = Point1();
  const IntRes2d_Transition& T = (Index == 1) ? 
    IP.TransitionOfFirst() : IP.TransitionOfSecond();

  switch (T.TransitionType()) {
    
  case IntRes2d_In :
    staB = TopAbs_OUT;
    staA = staINON;
    break;
    
  case IntRes2d_Out :
    staB = staINON;
    staA = TopAbs_OUT;
    break;
    
  case IntRes2d_Touch :
    switch (T.Situation()) {
      
    case IntRes2d_Inside :
      staB = staINON; 
      staA = staINON;
      break;
      
    case IntRes2d_Outside :
      staB = TopAbs_OUT;
      staA = TopAbs_OUT;
      break;
      
    case IntRes2d_Unknown : {
      
      // get posindex = position on of point on edge <Index>
      IntRes2d_Position posindex = 
	(Index == 1) ? 
	    IP.TransitionOfFirst().PositionOnCurve() : 
	    IP.TransitionOfSecond().PositionOnCurve();
      
      if (pointofsegment) {
	
	// get posother = position of point on the other edge
	IntRes2d_Position posother = 
	  (Index == 1) ? 
	    IP.TransitionOfSecond().PositionOnCurve() :
	    IP.TransitionOfFirst().PositionOnCurve();
	
	if (posother == IntRes2d_Middle) {
	  if (posindex != IntRes2d_Middle) {
	    staB = staINON;
	    staA = staINON;
	  }
	  else // Middle/Middle is impossible
	    throw Standard_Failure("TopOpeBRep_EdgesIntersector : Situation Unknown MM");
	}
	else { // posother = Head or End
	  Standard_Boolean opposite = IsOpposite1();
	  if (opposite) {
	    if (posother == IntRes2d_Head) {
	      staB = staINON;
	      staA = TopAbs_OUT;
	    }
	    else if (posother == IntRes2d_End) {
	      staB = TopAbs_OUT;
	      staA = staINON;
	    }
	  }
	  else {
	    if (posother == IntRes2d_Head) {
	      staB = TopAbs_OUT;
	      staA = staINON;
	    }
	    else if (posother == IntRes2d_End) {
	      staB = staINON;
	      staA = TopAbs_OUT;
	    }
	  }
	}
      } // point is a segment point
      
      else { // point is not a segment point
	// two edges intersect on a vertex
	// the vertex is shared by the two edges
	// the edges are tangent on vertex.
	pextremity = Standard_True;
	shaA = shaB = TopAbs_EDGE;
	
	if ( posindex == IntRes2d_Head ) {
	  staB = staINON;
	  staA = TopAbs_OUT;
	}
	else if (posindex == IntRes2d_End) {
	  staB = TopAbs_OUT;
	  staA = staINON;
	}
	else {  // Middle is impossible
	  throw Standard_Failure("TopOpeBRep_EdgesIntersector : Situation Unknown M");
	}
      } // point is not a segment point
      
    } // T.Situation == IntRes2d_Unknown
      break; 
      
    } // switch T.Situation()
    break;
    
  case IntRes2d_Undecided : 
    throw Standard_Failure("TopOpeBRep_EdgesIntersector : TransitionType Undecided");
    break;
    
  } // switch TransitionType()

  TopOpeBRepDS_Transition TR;
  if (pur1d || pextremity) {
    TR.Set(staB,staA,shaB,shaA);
  }
  else { // +ooOO
    Standard_Boolean composori = Standard_False;
    composori = composori || ((Index == 1) && (!myf2surf1F_sameoriented));
    composori = composori || ((Index == 2) && (!myf1surf1F_sameoriented));
    // Index = 1  <==> on demande la transition sur
    // une arete de la 1ere face par rapport a une arete orientee de 
    // la 2eme face.
    // EdgeOrientation est l'orientation d'une arete de la 2eme face
    // de l'appel SetFaces(), i.e ume arete de la face dont la surface 
    // n'est PAS la surface de reference de l'intersecteur 2d.
    // Cette orientation d'arete dans la face doit etre composee avec
    // l'orientation relative de la topologie de la 2eme face par rapport
    // a la topologie de la 1ere face orientee FORWARD (car la
    // geometrie naturelle de la 1ere face est la reference).
    TopAbs_Orientation eori = EdgeOrientation;
    if (composori) {
      eori = TopAbs::Reverse(eori);
    }
    
    // retournement des etats en fonction de l'orientation de l'arete
    // croisee dans l'espace geometrique de reference.
    TR.Set(staB,staA,shaB,shaA);
    if (eori == TopAbs_REVERSED) {
      TR = TR.Complement();
    }
  }
  return TR;
}

//=======================================================================
//function : Parameter1
//purpose  : 
//=======================================================================
Standard_Real TopOpeBRep_EdgesIntersector::Parameter1(const Standard_Integer Index) const 
{
  if (Index == 1) return Point1().ParamOnFirst();
  else            return Point1().ParamOnSecond();
}

//=======================================================================
//function : IsVertex1
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_EdgesIntersector::IsVertex1(const Standard_Integer Index)
{
  // check if last IsVertex1() call has been performed 
  // on current point and with same <Index>.
  if ( myIsVertexPointIndex == myPointIndex &&
      myIsVertexIndex == Index ) 
    return myIsVertexValue;
  
  // search if current point is a vertex of edge <Index>
  myIsVertexValue = Standard_False;
  IntRes2d_Position pos;
  if (Index == 1) pos = Point1().TransitionOfFirst().PositionOnCurve();
  else            pos = Point1().TransitionOfSecond().PositionOnCurve();
  
  if ( pos == IntRes2d_Middle ) {
    // search for an INTERNAL vertex on edge <Index> with
    // a 2d parameter <parV> equal to current point parameter <par>
    Standard_Real par = Parameter1(Index);
    const TopoDS_Edge *pE = NULL;
    pE = (Index == 1) ? &myEdge1 : &myEdge2;
    const TopoDS_Edge& E = *pE;
    TopExp_Explorer ex;
    for (ex.Init(E,TopAbs_VERTEX); ex.More(); ex.Next()) {
//    for (TopExp_Explorer ex(E,TopAbs_VERTEX); ex.More(); ex.Next()) {
      const TopoDS_Vertex& V = TopoDS::Vertex(ex.Current());
      if ( V.Orientation() == TopAbs_INTERNAL) {
	Standard_Real parV = BRep_Tool::Parameter(V,E,myFace1);
	if (Abs(par-parV) <= Precision::PConfusion()) {
	  myIsVertexValue = Standard_True;
	  myIsVertexVertex = V;
	  break;
	}
      }
    }
  }
  else { // pos = head or end
    TopoDS_Vertex V1,V2;
    if (Index == 1) TopExp::Vertices(myEdge1,V1,V2);
    else            TopExp::Vertices(myEdge2,V1,V2);
    if      ( pos == IntRes2d_Head && !V1.IsNull()) {
      myIsVertexValue = Standard_True;
      myIsVertexVertex = V1;
    }
    else if ( pos == IntRes2d_End && !V2.IsNull()) {
      myIsVertexValue = Standard_True;
      myIsVertexVertex = V2;
    }
    // ... else myIsVertexValue has been set to False
  }
  
  // memorize that IsVertex1() has been called :
  //  - on point myPointIndex
  //  - on edge <Index>          
  myIsVertexPointIndex = myPointIndex;
  myIsVertexIndex = Index;
  
  return myIsVertexValue;
}


//=======================================================================
//function : Vertex1
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRep_EdgesIntersector::Vertex1(const Standard_Integer Index)
{
  if ( ! IsVertex1(Index) )
    throw Standard_Failure("TopOpeBRep_EdgesIntersector : Vertex1");
  return myIsVertexVertex;
}

//=======================================================================
//function : Value1
//purpose  : 
//=======================================================================
gp_Pnt TopOpeBRep_EdgesIntersector::Value1() const 
{
  gp_Pnt2d p2 = Point1().Value();
  gp_Pnt   p;
  if (Precision::IsInfinite(p2.X()) || Precision::IsInfinite(p2.Y())) {
    Standard_Real inf = Precision::Infinite();
    p.SetCoord (inf, inf, inf);
  }
  else
    mySurface1->Surface().D0(p2.X(),p2.Y(), p);
  return   p;
}
