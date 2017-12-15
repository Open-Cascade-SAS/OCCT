// Created on: 1995-12-12
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

// Modified by dpf, Fri Dec 19 15:31:03 1997 
//  Processing of closing in 2d.
//  modified by eap Tue Dec 18 14:14:25 2001 (bug OCC23)
//   Check self-intersection in case of closed edge
//  modified by eap Fri Dec 21 17:36:55 2001 (bug OCC35)
//   Closed2d() added
//  Modified by skv - Wed Jul 23 12:22:20 2003 OCC1764 

#include <Bnd_Array1OfBox2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_Curve.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_Intersection.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntRes2d_Transition.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TColGeom2d_Array1OfCurve.hxx>
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
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfOrientedShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(BRepCheck_Wire,BRepCheck_Result)

static void Propagate(const TopTools_IndexedDataMapOfShapeListOfShape&,
                      const TopoDS_Shape&,   // edge
                      TopTools_MapOfShape&); // mapofedge


static TopAbs_Orientation GetOrientation(const TopTools_MapOfShape&,
                                         const TopoDS_Edge&);


static 
  void ChoixUV(const TopoDS_Vertex&,
	       const TopoDS_Edge&,
	       const TopoDS_Face&,
	       TopTools_ListOfShape&);

//      20/03/02 akm vvv (OCC234)
// static 
//   Standard_Boolean CheckLoopOrientation( const TopoDS_Vertex&,
// 					const TopoDS_Edge&,
// 					const TopoDS_Edge&,
// 					const TopoDS_Face&,
// 					TopTools_ListOfShape&);
//      20/03/02 akm ^^^

inline Standard_Boolean IsOriented(const TopoDS_Shape& S)
{
  return (S.Orientation() == TopAbs_FORWARD ||
          S.Orientation() == TopAbs_REVERSED);
}

static
  void CurveDirForParameter(const Geom2dAdaptor_Curve& aC2d,
			    const Standard_Real aPrm,
			    gp_Pnt2d& Pnt,
			    gp_Vec2d& aVec2d);

//  Modified by Sergey KHROMOV - Thu Jun 20 11:21:51 2002 OCC325 Begin
static Standard_Boolean IsClosed2dForPeriodicFace
                        (const TopoDS_Face   &theFace,
			 const gp_Pnt2d      &theP1,
			 const gp_Pnt2d      &theP2,
			 const TopoDS_Vertex &theVertex);

static Standard_Boolean GetPnt2d(const TopoDS_Vertex    &theVertex,
				 const TopoDS_Edge      &theEdge,
				 const TopoDS_Face      &theFace,
				       gp_Pnt2d         &aPnt);
//  Modified by Sergey KHROMOV - Wed May 22 10:44:08 2002 End

//=======================================================================
//function : BRepCheck_Wire
//purpose  : 
//=======================================================================
BRepCheck_Wire::BRepCheck_Wire(const TopoDS_Wire& W)
{
  Init(W);
}
//=======================================================================
//function : Minimum
//purpose  : 
//=======================================================================
void BRepCheck_Wire::Minimum()
{
  myCdone = Standard_False;
  myGctrl = Standard_True;
  if (!myMin) {
    BRepCheck_ListOfStatus thelist;
    myMap.Bind(myShape, thelist);
    BRepCheck_ListOfStatus& lst = myMap(myShape);

    // check that the wire is "connex"
    TopExp_Explorer exp(myShape,TopAbs_EDGE);
    Standard_Integer nbedge = 0;
    myMapVE.Clear();
    // fill myMapVE
    for (; exp.More(); exp.Next()) {
      nbedge++;
      TopExp_Explorer expv;
      for (expv.Init(exp.Current(),TopAbs_VERTEX);
           expv.More(); expv.Next()) {
        const TopoDS_Shape& vtx = expv.Current();
        Standard_Integer index = myMapVE.FindIndex(vtx);
        if (index == 0) {
          TopTools_ListOfShape theListOfShape;
          index = myMapVE.Add(vtx, theListOfShape);
        }
        myMapVE(index).Append(exp.Current());
      }
    }
    // wire must have at least one edge
    if (nbedge == 0) {
      BRepCheck::Add(lst,BRepCheck_EmptyWire);
    }
    // check if all edges are connected through vertices
    else if (nbedge >= 2) {
      TopTools_MapOfShape mapE;
      exp.ReInit();
      Propagate(myMapVE,exp.Current(),mapE);
      for (exp.ReInit(); exp.More(); exp.Next()) {
        if (!mapE.Contains(exp.Current())) {
          BRepCheck::Add(lst,BRepCheck_NotConnected);
          break;
        }
      }
    }
    if (lst.IsEmpty()) {
      lst.Append(BRepCheck_NoError);
    }
    myMapVE.Clear();
    myMin = Standard_True;
  }
}
//=======================================================================
//function : InContext
//purpose  : 
//=======================================================================
void BRepCheck_Wire::InContext(const TopoDS_Shape& S)
{

  if (myMap.IsBound(S)) {
    return;
  }
  BRepCheck_ListOfStatus thelist;
  myMap.Bind(S, thelist);

  BRepCheck_ListOfStatus& lst = myMap(S);

  // check if my wire is in <S>
  TopExp_Explorer exp(S,TopAbs_WIRE); 
  for ( ; exp.More(); exp.Next()) {
    if (exp.Current().IsSame(myShape)) {
      break;
    }
  }
  if (!exp.More()) {
    BRepCheck::Add(lst,BRepCheck_SubshapeNotInShape);
    return;
  }

  BRepCheck_Status st = BRepCheck_NoError;
  TopAbs_ShapeEnum styp = S.ShapeType();
  switch (styp) {

  case TopAbs_FACE:
    {
      TopoDS_Edge ed1,ed2; 
      if (myGctrl) 
        st = SelfIntersect(TopoDS::Face(S),ed1,ed2,Standard_True);
      if (st != BRepCheck_NoError) break;
      st = Closed();
      if (st != BRepCheck_NoError) break;
      st = Orientation(TopoDS::Face(S));
      if (st != BRepCheck_NoError) break;
      st = Closed2d(TopoDS::Face(S));
    }
    break;
  default:
    break;
  }
  
  if (st != BRepCheck_NoError) 
    BRepCheck::Add(lst,st);
      
  if (lst.IsEmpty()) 
    lst.Append(BRepCheck_NoError);
}
//=======================================================================
//function : Blind
//purpose  : 
//=======================================================================
void BRepCheck_Wire::Blind()
{
  if (!myBlind) {
    // nothing more that the minimum
    myBlind = Standard_True;
  }
}
//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================
BRepCheck_Status BRepCheck_Wire::Closed(const Standard_Boolean Update)
{

  if (myCdone) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myCstat);
    }
    return myCstat;
  }

  myCdone = Standard_True;

  BRepCheck_ListIteratorOfListOfStatus itl(myMap(myShape));
  if (itl.Value() != BRepCheck_NoError) {
    myCstat = itl.Value();
    return myCstat; // already saved 
  }

  myCstat = BRepCheck_NoError;

  TopExp_Explorer exp,expv;
  TopTools_MapOfShape mapS;
  TopTools_DataMapOfShapeListOfShape Cradoc;
  myMapVE.Clear();
  // Checks if the oriented edges of the wire give a "closed" wire,
  // i-e if each oriented vertex on oriented edges is found 2 times...
  // myNbori = 0;
  for (exp.Init(myShape,TopAbs_EDGE);exp.More(); exp.Next()) {
    if (IsOriented(exp.Current())) {
      // myNbori++;
      if (!Cradoc.IsBound(exp.Current())) {
        TopTools_ListOfShape theListOfShape;
        Cradoc.Bind(exp.Current(), theListOfShape);
      }
      Cradoc(exp.Current()).Append(exp.Current());

      mapS.Add(exp.Current());
      for (expv.Init(exp.Current(),TopAbs_VERTEX); expv.More(); expv.Next()) {
        if (IsOriented(expv.Current())) {
          Standard_Integer index = myMapVE.FindIndex(expv.Current());
          if (index == 0) {
            TopTools_ListOfShape theListOfShape1;
            index = myMapVE.Add(expv.Current(), theListOfShape1);
          }
          myMapVE(index).Append(exp.Current());
        }
      }
    }
  }

  Standard_Integer theNbori = mapS.Extent();
  if (theNbori >= 2) {
    mapS.Clear();
    for (exp.ReInit(); exp.More(); exp.Next()) {
      if (IsOriented(exp.Current())) {
        break;
      }
    }
    Propagate(myMapVE,exp.Current(),mapS);
  }
  if (theNbori != mapS.Extent()) {
    myCstat = BRepCheck_NotConnected;
    if (Update) {
      BRepCheck::Add(myMap(myShape),myCstat);
    }
    return myCstat;
  }

  // Checks the number of occurence of an edge : maximum 2, and in this 
  // case, one time FORWARD and one time REVERSED

  Standard_Boolean yabug = Standard_False;
  for (TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itdm(Cradoc);
       itdm.More(); itdm.Next()) {
    if (itdm.Value().Extent() >= 3) {
      yabug = Standard_True;
    }
    else if (itdm.Value().Extent() == 2) {
      if (itdm.Value().First().Orientation() == 
          itdm.Value().Last().Orientation()) {
        yabug = Standard_True;
      }
    }
    if (yabug) {
      myCstat = BRepCheck_RedundantEdge;
      if (Update) {
        BRepCheck::Add(myMap(myShape),myCstat);
      }
      return myCstat;
    }
  }

  for (Standard_Integer i = 1; i<= myMapVE.Extent(); i++) {
    if (myMapVE(i).Extent()%2 != 0) {
      myCstat=BRepCheck_NotClosed;
      if (Update) {
        BRepCheck::Add(myMap(myShape),myCstat);
      }
      return myCstat;
    }
  }

  if (Update) {
    BRepCheck::Add(myMap(myShape),myCstat);
  }
  return myCstat;
}

//=======================================================================
//function : IsDistanceIn3DTolerance
//purpose  : Return Standard_True if distance between thePnt_f and
//           thePnt_l is not more, than aTol3d
//=======================================================================
Standard_Boolean IsDistanceIn3DTolerance (const gp_Pnt& thePnt_f,
                                          const gp_Pnt& thePnt_l,
                                          const Standard_Real aTol3d)
  {
  Standard_Real Dist		= thePnt_f.Distance(thePnt_l);
  
  if (Dist < aTol3d)
    return Standard_True;

#ifdef OCCT_DEBUG
  cout << endl;
  cout << "--------Function IsDistanceIn3DTolerance(...)----------"												<< endl;
  cout << "--- BRepCheck Wire: Closed3d -> Error"																					<< endl;
  cout << "--- Dist (" << Dist << ") > Tol3d (" << aTol3d << ")"													<< endl;
  cout << "Pnt1(" << thePnt_f.X() << "; " << thePnt_f.Y() << "; " << thePnt_f.Z() << ")"	<< endl;
  cout << "Pnt2(" << thePnt_l.X() << "; " << thePnt_l.Y() << "; " << thePnt_l.Z() << ")"	<< endl;
  cout << "------------------------------------------------------"												<< endl;
#endif

  return Standard_False;
  }

//=======================================================================
//function : IsDistanceIn2DTolerance
//purpose  : 
//=======================================================================
static 
Standard_Boolean IsDistanceIn2DTolerance (const BRepAdaptor_Surface& aFaceSurface,
                                          const gp_Pnt2d& thePnt,
                                          const gp_Pnt2d& thePntRef,
                                          const Standard_Real aTol3d,
#ifdef OCCT_DEBUG
                                          const Standard_Boolean PrintWarnings = Standard_True)
#else
                                          const Standard_Boolean = Standard_True)
#endif
{
  Standard_Real dumax = 0.01 * (aFaceSurface.LastUParameter() - aFaceSurface.FirstUParameter());
  Standard_Real dvmax = 0.01 * (aFaceSurface.LastVParameter() -	aFaceSurface.FirstVParameter());
  Standard_Real dumin = Abs(thePnt.X() - thePntRef.X());
  Standard_Real dvmin = Abs(thePnt.Y() - thePntRef.Y());
  
  if((dumin < dumax) && (dvmin < dvmax))
    return Standard_True;

#ifdef OCCT_DEBUG
  if(PrintWarnings)
    {
    cout << endl;
    cout << "--------Function IsDistanceIn2DTolerance(...)----------"								<< endl;
    cout << "--- BRepCheck Wire: Not closed in 2D"																  << endl;
    cout << "*****************************************************"									<< endl;
    cout << "*dumin = " << dumin << "; dumax = " << dumax														<< endl;
    cout << "* dvmin = " << dvmin << "; dvmax = " << dvmax													<< endl;
    cout << "* (dumin > dumax) or (dvmin > dvmax)."																	<< endl;
    cout << "*****************************************************"									<< endl;
    cout << endl;
    cout << "UFirst = "  << aFaceSurface.FirstUParameter();
    cout << "; ULast = " << aFaceSurface.LastUParameter()														<< endl;
    cout << "VFirst = " << aFaceSurface.FirstVParameter();
    cout << "; VLast = " << aFaceSurface.LastVParameter()														<< endl;
    }
#endif
  dumax = aFaceSurface.UResolution(aTol3d);
  dvmax = aFaceSurface.VResolution(aTol3d);
  gp_Pnt aP;
  gp_Vec aDU, aDV;
  Standard_Real um = (thePnt.X() + thePntRef.X()) / 2.;
  Standard_Real vm = (thePnt.Y() + thePntRef.Y()) / 2.;
  aFaceSurface.D1(um, vm, aP, aDU, aDV);
  Standard_Real aMDU = aDU.Magnitude();
  if (aMDU > Precision::Confusion())
  {
    dumax = Max((aTol3d / aMDU), dumax);
  }
  Standard_Real aMDV = aDV.Magnitude();
  if (aMDV > Precision::Confusion())
  {
    dvmax = Max((aTol3d / aMDV), dvmax);
  }

#ifdef OCCT_DEBUG
  if(PrintWarnings)
    {
    cout << "aTol3d = " << aTol3d <<"; URes = " << dumax << "; VRes = " << dvmax		<< endl;
    cout << "thePnt(" << thePnt.X() << "; " << thePnt.Y() << ")"										<< endl;
    cout << "thePntRef(" << thePntRef.X() << "; " << thePntRef.Y() << ")"						<< endl;
    }
#endif

  Standard_Real aTol2d = 2*Max(	dumax, dvmax);
  
#ifdef OCCT_DEBUG
  if((aTol2d <= 0.0) && (PrintWarnings))
    {
    cout<<"BRepCheck_Wire : UResolution and VResolution = 0.0 (Face too small ?)"<<endl;
    cout.flush();
    }
#endif

  Standard_Real Dist = Max(dumin, dvmin);
  
  if (Dist < aTol2d)
    return Standard_True;

#ifdef OCCT_DEBUG
  if(PrintWarnings)
    {
    cout << endl;
    cout << "--------Function IsDistanceIn2DTolerance(...)----------"							<< endl;
    cout << "--- BRepCheck Wire: Not closed in 2d"  															<< endl;
    cout << "*****************************************************"								<< endl;
    cout << "* Dist = " << Dist	<< " > Tol2d = " <<	aTol2d												<< endl;
    cout << "*****************************************************"								<< endl;
    cout << "aTol3d = " << aTol3d <<"; URes = " << dumax << "; VRes = " << dvmax	<< endl;
    cout << "thePnt(" << thePnt.X() << "; " << thePnt.Y() << ")"									<< endl;
    cout << "thePntRef(" << thePntRef.X() << "; " << thePntRef.Y() << ")"					<< endl;
    }
#endif

  return Standard_False;
  }

//=======================================================================
//function : Closed2d
//purpose  : for periodic faces
//=======================================================================
BRepCheck_Status BRepCheck_Wire::Closed2d(const TopoDS_Face& theFace,
                                          const Standard_Boolean Update)
  {
  // 3d closure checked too
  BRepCheck_Status aClosedStat = Closed();
  if (aClosedStat != BRepCheck_NoError)
    {
    if (Update)
      BRepCheck::Add(myMap(myShape),aClosedStat);

    return aClosedStat;
    }

// 20/03/02 akm vvv : (OCC234) Hence this method will be used to check
//                    both periodic and non-periodic faces
//   // this check is for periodic faces 
  BRepAdaptor_Surface aFaceSurface (theFace, Standard_False);
// if (!aFaceSurface.IsUPeriodic() && !aFaceSurface.IsVPeriodic())
// {
//   if (Update) 
//     BRepCheck::Add(myMap(myShape),aClosedStat);
//   return aClosedStat;
// }
// 20/03/02 akm ^^^

// count edges having FORWARD or REVERSED orientation
  Standard_Integer aNbOrirntedEdges = 0;
  TopExp_Explorer anEdgeExp(myShape,TopAbs_EDGE);
  for (;anEdgeExp.More(); anEdgeExp.Next())
    {
    if (IsOriented(anEdgeExp.Current())) 
      aNbOrirntedEdges++;
    }

  if (aNbOrirntedEdges==0)
    {
    if (Update)
      BRepCheck::Add(myMap(myShape),aClosedStat);

    return aClosedStat;
    }

// all those edges must form a closed 2d contour and be found by WireExplorer

  Standard_Integer aNbFoundEdges = 0;
  BRepTools_WireExplorer aWireExp(TopoDS::Wire(myShape), theFace);
  TopoDS_Edge aFirstEdge = aWireExp.Current();
  TopoDS_Vertex aFirstVertex = aWireExp.CurrentVertex();
  TopoDS_Edge aLastEdge;

  for (;aWireExp.More(); aWireExp.Next())
    {
    aNbFoundEdges++;
    aLastEdge = aWireExp.Current();
    }

  if (aNbFoundEdges != aNbOrirntedEdges)
    {
    aClosedStat = BRepCheck_NotClosed;
    if (Update) 
      BRepCheck::Add(myMap(myShape),aClosedStat);
    
    return aClosedStat;
    }

// Check distance between 2d ends of first and last edges
//  Modified by Sergey KHROMOV - Mon May 13 12:42:10 2002 Begin
//   First check if first and last edges are infinite:
  Standard_Real      aF;
  Standard_Real      aL;
  Standard_Boolean   isFirstInfinite = Standard_False;
  Standard_Boolean   isLastInfinite  = Standard_False;
  TopAbs_Orientation anOri;

  anOri = aFirstEdge.Orientation();
  BRep_Tool::Range(aFirstEdge, aF, aL);
  if ((anOri == TopAbs_FORWARD  && Precision::IsNegativeInfinite( aF )) ||
         (anOri == TopAbs_REVERSED && Precision::IsPositiveInfinite( aL )))
    isFirstInfinite = Standard_True;

  anOri = aLastEdge.Orientation();
  BRep_Tool::Range(aLastEdge, aF, aL);
  
  if ((anOri == TopAbs_FORWARD  && Precision::IsPositiveInfinite( aL )) ||
         (anOri == TopAbs_REVERSED && Precision::IsNegativeInfinite( aF )))
    isLastInfinite = Standard_True;

  if (isFirstInfinite && isLastInfinite)
    {
    if (Update)
      BRepCheck::Add(myMap(myShape),aClosedStat);

    return aClosedStat;
    }
  else if (aFirstVertex.IsNull())
    {
    aClosedStat = BRepCheck_NotClosed;
    
    if (Update) 
      BRepCheck::Add(myMap(myShape),aClosedStat);
    
    return aClosedStat;
    }
//  Modified by Sergey KHROMOV - Mon May 13 12:42:10 2002 End

  gp_Pnt2d aP_first, aP_last, aP_temp; // ends of prev edge, next edge, bidon
  
// get last point
  BRep_Tool::UVPoints(aLastEdge, theFace, aP_temp, aP_last);
  if (aLastEdge.Orientation() == TopAbs_REVERSED)
    aP_last = aP_temp;

//  Modified by Sergey KHROMOV - Mon Apr 22 10:36:33 2002 Begin
//   Standard_Real aTol, aUResol, aVResol;
//   // find 2d tolerance
//   aTol  = BRep_Tool::Tolerance(aFirstVertex);
//   aUResol = 2*aFaceSurface.UResolution(aTol);
//   aVResol = 2*aFaceSurface.VResolution(aTol);

// get first point
  if (aFirstEdge.Orientation() == TopAbs_REVERSED)
    BRep_Tool::UVPoints(aFirstEdge, theFace, aP_temp, aP_first);
  else 
    BRep_Tool::UVPoints(aFirstEdge, theFace, aP_first, aP_temp);

//  Modified by Sergey KHROMOV - Thu Jun 20 10:55:42 2002 OCC325 Begin
// Check 2d distance for periodic faces with seam edge
  if (!IsClosed2dForPeriodicFace(theFace, aP_first, aP_last, aFirstVertex))
    {
    aClosedStat = BRepCheck_NotClosed;
    if (Update)
      BRepCheck::Add(myMap(myShape),aClosedStat);
    
    return aClosedStat;
    }
//  Modified by Sergey KHROMOV - Thu Jun 20 10:58:05 2002 End

// check distance
//   Standard_Real dfUDist=Abs(p.X()-p1.X());
//   Standard_Real dfVDist=Abs(p.Y()-p1.Y());
//   if (dfUDist > aUResol || dfVDist > aVResol)
//   {

  Standard_Real aTol3d	= Max(BRep_Tool::Tolerance(aFirstVertex),BRep_Tool::Tolerance(aWireExp.CurrentVertex()));

  gp_Pnt aPntRef = BRep_Tool::Pnt(aFirstVertex);
  gp_Pnt aPnt		 = BRep_Tool::Pnt(aWireExp.CurrentVertex());

  if (!(IsDistanceIn2DTolerance(aFaceSurface, aP_first, aP_last, aTol3d)))
    aClosedStat = BRepCheck_NotClosed;

  if(!IsDistanceIn3DTolerance(aPntRef, aPnt, aTol3d))
    aClosedStat = BRepCheck_NotClosed;

  if (Update) 
    BRepCheck::Add(myMap(myShape),aClosedStat);

  return aClosedStat;
  }
//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================
BRepCheck_Status BRepCheck_Wire::Orientation(const TopoDS_Face& F,
                                             const Standard_Boolean Update)
{
  BRepCheck_Status theOstat = Closed();
  if (theOstat != BRepCheck_NotClosed && theOstat != BRepCheck_NoError) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),theOstat);
    }
    return theOstat;
  }

  theOstat = BRepCheck_NoError;

  TopoDS_Vertex VF,VL;
  TopAbs_Orientation orient, ortmp = TopAbs_FORWARD;
  TopTools_ListOfShape ledge, ListOfPassedEdge;
  TopExp_Explorer exp,vte;
  TopTools_MapOfShape mapS;
  TopoDS_Edge theEdge,theRef;

  // Checks the orientation of the edges
  for (exp.Init(myShape,TopAbs_EDGE); exp.More(); exp.Next()) {
    const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
    orient = edg.Orientation();
    if (IsOriented(edg)) {
      mapS.Add(edg);
      theEdge = edg;
      theRef = edg;
      for (vte.Init(edg,TopAbs_VERTEX);vte.More(); vte.Next()) {
        TopAbs_Orientation vto = vte.Current().Orientation();
        if (vto == TopAbs_FORWARD) {
          VF = TopoDS::Vertex(vte.Current());
        }
        else if (vto == TopAbs_REVERSED) {
          VL = TopoDS::Vertex(vte.Current());
        }
        if (!VF.IsNull() && !VL.IsNull()) {
          break;
        }
      }
      if (VF.IsNull() && VL.IsNull())
        theOstat = BRepCheck_InvalidDegeneratedFlag;
      break;
    }
  }
 
  if (theOstat == BRepCheck_NoError) {
    Standard_Integer Index = 1;
    Standard_Integer nbOriNoDegen=myMapVE.Extent();
//  Modified by Sergey KHROMOV - Tue May 21 17:12:45 2002 Begin
    Standard_Boolean isGoFwd     = Standard_True;

    if (VL.IsNull())
      isGoFwd = Standard_False;
//  Modified by Sergey KHROMOV - Tue May 21 17:12:45 2002 End

    while (Index < nbOriNoDegen) {
      ledge.Clear();
      ListOfPassedEdge.Clear();
      // find edges that make a chain on VL if !VL.IsNull 
      // otherwise on VF.
      
      Standard_Integer ind;
      if (!VL.IsNull()) {
        ind = myMapVE.FindIndex(VL);
      }
      else if (!VF.IsNull()) {
        ind = myMapVE.FindIndex(VF);
      }
      else {
        theOstat = BRepCheck_InvalidDegeneratedFlag;
        break;
      }

      for (TopTools_ListIteratorOfListOfShape itls(myMapVE(ind));
           itls.More(); itls.Next()) {
        const TopoDS_Edge & edg = TopoDS::Edge(itls.Value());

        orient = edg.Orientation();
        if (mapS.Contains(edg)) ortmp = GetOrientation(mapS,edg);

        //Add to list already passed outcoming edges
        if (mapS.Contains(edg) && ortmp == orient && !edg.IsSame(theEdge))
          for (vte.Init(edg,TopAbs_VERTEX); vte.More(); vte.Next())
            {
              TopAbs_Orientation vto = vte.Current().Orientation();
              if (!VL.IsNull())
                {
                  if (vto == TopAbs_FORWARD && VL.IsSame(vte.Current()))
                    {
                      ListOfPassedEdge.Append(edg);
                      break;
                    }
                }
              else // VF is not null
                {
                  if (vto == TopAbs_REVERSED && VF.IsSame(vte.Current()))
                    {
                      ListOfPassedEdge.Append(edg);
                      break;
                    }
                }
            }

        if (!mapS.Contains(edg) || ortmp != orient) {
          for (vte.Init(edg,TopAbs_VERTEX);vte.More(); vte.Next()) {
            TopAbs_Orientation vto = vte.Current().Orientation();
            if (!VL.IsNull()) {
              if (vto == TopAbs_FORWARD && VL.IsSame(vte.Current())) {
                // If the processing is in 2d (face not null) or 
                // if the edge is not degenerated it is added
                if (!F.IsNull() || !BRep_Tool::Degenerated(edg))
                  ledge.Append(edg);
                break;
              }
            }
            else { // VF is not null
              if (vto == TopAbs_REVERSED && VF.IsSame(vte.Current())) {
                //    // If the processing is in 2d (face not null) or 
                // if the edge is not degenerated it is added
                if (!F.IsNull() || !BRep_Tool::Degenerated(edg))
                  ledge.Append(edg);
                break;
              }
            }
          }
        }
      }
      Standard_Integer nbconnex = ledge.Extent();
      Standard_Boolean Changedesens = Standard_False;
      if (nbconnex == 0) {
        if (myCstat == BRepCheck_NotClosed) {
          if (VL.IsNull()) {
            if (Update) {
              BRepCheck::Add(myMap(myShape),theOstat);
            }
            return theOstat; // leave
          }
          else {
            Index--; // because after Index++ and if there is no chain,
            VL.Nullify(); // chain on VF is forced
            theEdge = theRef;
            Changedesens = Standard_True;
          }
        }
        else {
          theOstat = BRepCheck_BadOrientationOfSubshape;
          if (Update) {
            BRepCheck::Add(myMap(myShape),theOstat);
	    }
          return theOstat;
        }
      }

      // JAG 03/07   else if (nbconnex >= 2 && !F.IsNull())  // Try to see in 2d
      else if (!F.IsNull()) { // Try to see in 2d
        TopoDS_Vertex pivot;
        if (!VL.IsNull()) {
          pivot = VL;
        }
        else {
          pivot = VF;
        }

        ChoixUV(pivot,theEdge,F,ledge);
        nbconnex = ledge.Extent();
//      20/03/02 akm vvv : (OCC234) - The 2d exploration of wire with necessary
//                         checks is performed in Closed2d, here it's useless
//         if (nbconnex == 1 && !CheckLoopOrientation( pivot, theEdge, TopoDS::Edge(ledge.First()), F, ListOfPassedEdge ))
//         {
//           theOstat = BRepCheck_BadOrientationOfSubshape;
//           if (Update)
//             BRepCheck::Add(myMap(myShape),theOstat);
//           return theOstat;
//         }
//      20/03/02 akm ^^^
      }

      if (nbconnex >= 2) {
        theOstat = BRepCheck_BadOrientationOfSubshape;
        if (Update) {
          BRepCheck::Add(myMap(myShape),theOstat);
          }
        return theOstat;
      }
      else if (nbconnex == 1) {
        // offset the vertex
        for (vte.Init(ledge.First(),TopAbs_VERTEX);vte.More(); vte.Next()) {
          TopAbs_Orientation vto = vte.Current().Orientation();
          if (!VL.IsNull()) {
            if (vto == TopAbs_REVERSED) {
              VL = TopoDS::Vertex(vte.Current());
              break;
            }
          }
          else { // VF is not null
            if (vto == TopAbs_FORWARD) {
              VF = TopoDS::Vertex(vte.Current());
              break;
            }
          }
        }
        mapS.Add(ledge.First());
        theEdge = TopoDS::Edge(ledge.First());
        if (!vte.More()) {
          if (!VL.IsNull()) {
            VL.Nullify();
          }
          else {
            VF.Nullify();
          }
        }
      }
      else if (!Changedesens) { //nbconnex == 0
        theOstat = BRepCheck_NotClosed;
        if (Update) {
          BRepCheck::Add(myMap(myShape),theOstat);
	  }
	return theOstat;
      }

      // Check the closure of the wire in 2d (not done in Closed())
      
      TopoDS_Vertex    aVRef;
      Standard_Boolean isCheckClose = Standard_False;

      if (isGoFwd && !VF.IsNull()) {
	aVRef        = VF;
	isCheckClose = Standard_True;
      } else if (!isGoFwd && !VL.IsNull()) {
	aVRef        = VL;
	isCheckClose = Standard_True;
      }

//       if (Index==1 && myCstat!=BRepCheck_NotClosed && 
// 	  !VF.IsNull() && !F.IsNull()) {
      if (Index==1 && myCstat!=BRepCheck_NotClosed && 
	  isCheckClose && !F.IsNull()) {
	ledge.Clear();
// 	ind = myMapVE.FindIndex(VF);
	ind = myMapVE.FindIndex(aVRef);
	for (TopTools_ListIteratorOfListOfShape itlsh(myMapVE(ind));
	     itlsh.More(); itlsh.Next()) {
	  const TopoDS_Edge & edg = TopoDS::Edge(itlsh.Value());
	  orient = edg.Orientation();
	  if (!theRef.IsSame(edg)) {
	    for (vte.Init(edg,TopAbs_VERTEX);vte.More(); vte.Next()) {
	      TopAbs_Orientation vto = vte.Current().Orientation();
// 	      if (vto == TopAbs_REVERSED && VF.IsSame(vte.Current())) {
	      if (vto == TopAbs_REVERSED && aVRef.IsSame(vte.Current())) {
		ledge.Append(edg);
		break;
	      }
	    }
	  }
	}
// 	ChoixUV(VF, theRef, F, ledge);
	ChoixUV(aVRef, theRef, F, ledge);
	if (ledge.Extent()==0) {
	  theOstat = BRepCheck_NotClosed;
	  if (Update) {
	    BRepCheck::Add(myMap(myShape),theOstat);
	  }
	  return theOstat;
	}
      }
      // End control closure 2d

      Index ++;
    }
  }
  if (Update) {
    BRepCheck::Add(myMap(myShape),theOstat);
  }
  return theOstat;
}
//=======================================================================
//function : SelfIntersect
//purpose  : 
//=======================================================================
BRepCheck_Status BRepCheck_Wire::SelfIntersect(const TopoDS_Face& F,
					       TopoDS_Edge& retE1,
					       TopoDS_Edge& retE2,
					       const Standard_Boolean Update)
{


  Standard_Integer i,j,Nbedges;
  Standard_Real first1,last1,first2,last2, tolint;
  gp_Pnt2d pfirst1,plast1,pfirst2,plast2;
  gp_Pnt P3d, P3d2;
  Handle(BRepAdaptor_HSurface) HS;
  Geom2dAdaptor_Curve C1, C2;
  Geom2dInt_GInter      Inter;
  IntRes2d_Domain myDomain1;
  TopTools_IndexedMapOfOrientedShape EMap;
  TopTools_MapOfOrientedShape auxmape;
  //
  //-- check with proper tolerances if there is no 
  //-- point in the tolerance of a vertex.
  tolint = 1.e-10; 
  HS = new BRepAdaptor_HSurface();
  HS->ChangeSurface().Initialize(F,Standard_False);
  //
  for (TopoDS_Iterator Iter1(myShape);Iter1.More();Iter1.Next()) {
    if (Iter1.Value().ShapeType() == TopAbs_EDGE) {
      EMap.Add(Iter1.Value());
    }
  }
  //
  Nbedges=EMap.Extent();
  if (!Nbedges) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),BRepCheck_EmptyWire);
    }
    return(BRepCheck_EmptyWire);
  }
  //
  IntRes2d_Domain *tabDom = new IntRes2d_Domain[Nbedges];
  TColGeom2d_Array1OfCurve tabCur(1,Nbedges);
  Bnd_Array1OfBox2d boxes(1,Nbedges);
  //
  for(i = 1; i <= Nbedges; i++) { 
    const TopoDS_Edge& E1 = TopoDS::Edge(EMap.FindKey(i));
    if (i == 1) {
      Handle(Geom2d_Curve) pcu = BRep_Tool::CurveOnSurface(E1, F, first1, last1);
      if (pcu.IsNull()) {
	retE1=E1;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),BRepCheck_SelfIntersectingWire);
	}
	delete [] tabDom;
	return(BRepCheck_SelfIntersectingWire);
      }
      //
      C1.Load(pcu);
      // To avoid exeption in Segment if C1 is BSpline - IFV
      if(!C1.IsPeriodic()) {
	if(C1.FirstParameter() > first1) {
	  first1 = C1.FirstParameter();
	}
	if(C1.LastParameter()  < last1 ){
	  last1  = C1.LastParameter();
	}
      }
      //
      BRep_Tool::UVPoints(E1, F, pfirst1, plast1);
      myDomain1.SetValues(pfirst1,first1,tolint, plast1,last1,tolint);
      //
      BndLib_Add2dCurve::Add(C1, first1, last1, Precision::PConfusion(), boxes(i));
    }//if (i == 1) {
    else {
      C1.Load(tabCur(i));
      myDomain1 = tabDom[i-1];
    }
    //
    // Self intersect of C1
    Inter.Perform(C1, myDomain1, tolint, tolint);
    //
    if(Inter.IsDone()) { 
      Standard_Integer nbp = Inter.NbPoints();
      //Standard_Integer nbs = Inter.NbSegments();
      //
      for(Standard_Integer p=1;p<=nbp;p++) {
	const IntRes2d_IntersectionPoint& IP=Inter.Point(p);
	const IntRes2d_Transition& Tr1 = IP.TransitionOfFirst();
	const IntRes2d_Transition& Tr2 = IP.TransitionOfSecond();
	if(   Tr1.PositionOnCurve() == IntRes2d_Middle
	   || Tr2.PositionOnCurve() == IntRes2d_Middle) { 
	  //-- Checking of points with true tolerances (ie Tol in 3d)
	  //-- If the point of intersection is within the tolearnce of a vertex
	  //-- this intersection is considered correct (no error)
	  Standard_Boolean localok = Standard_False;
	  Standard_Real f,l;
	  TopLoc_Location L;
	  const Handle(Geom_Curve) ConS = BRep_Tool::Curve(E1,L,f,l);
	  if(!ConS.IsNull()) { 
	    //-- try to test in 3d. (ParamOnSecond gives the same result)
	    P3d = ConS->Value(IP.ParamOnFirst()); 
	    P3d.Transform(L.Transformation());
	    //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 Begin
	  } 
	  else {
	    gp_Pnt2d aP2d  = C1.Value(IP.ParamOnFirst());
	    P3d = HS->Value(aP2d.X(), aP2d.Y());
	  }
	  //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 End
	  TopExp_Explorer ExplVtx;
	  for(ExplVtx.Init(E1,TopAbs_VERTEX); 
	      localok==Standard_False && ExplVtx.More();
	      ExplVtx.Next()) { 
	    gp_Pnt p3dvtt;
	    Standard_Real tolvtt, p3dvttDistanceP3d;
	    //
	    const TopoDS_Vertex& vtt = TopoDS::Vertex(ExplVtx.Current());
	    p3dvtt = BRep_Tool::Pnt(vtt);
	    tolvtt =  BRep_Tool::Tolerance(vtt);
	    tolvtt=tolvtt*tolvtt;
	    p3dvttDistanceP3d=p3dvtt.SquareDistance(P3d);
	    if(p3dvttDistanceP3d <=  tolvtt) { 
	      localok=Standard_True;
	    }
	  }
	  if(localok==Standard_False) { 
	    retE1=E1;
	    if (Update) {
	      BRepCheck::Add(myMap(myShape),BRepCheck_SelfIntersectingWire);
	      }
	    delete [] tabDom;
#ifdef OCCT_DEBUG
	    static Standard_Integer numpoint=0;
	    cout<<"point p"<<++numpoint<<" "<<P3d.X()<<" "<<P3d.Y()<<" "<<P3d.Z()<<endl;cout.flush();
#endif
	    return(BRepCheck_SelfIntersectingWire);
	  }
	}
      }
    }// if(Inter.IsDone()) { 
    //
    for(j=i+1; j<=Nbedges; j++) {
      const TopoDS_Edge& E2 = TopoDS::Edge(EMap.FindKey(j));
      if (i == 1) {
	tabCur(j) = BRep_Tool::CurveOnSurface(E2,F,first2,last2);
	if (!tabCur(j).IsNull() && last2 > first2) {
	  C2.Load(tabCur(j));
	  // To avoid exeption in Segment if C2 is BSpline - IFV
	  if(!C2.IsPeriodic()) {
	    if(C2.FirstParameter() > first2) {
	      first2 = C2.FirstParameter();
	    }
	    if(C2.LastParameter()  < last2 ) {
	      last2  = C2.LastParameter();
	    }
	  }
	  //
	  BRep_Tool::UVPoints(E2,F,pfirst2,plast2);
	  tabDom[j-1].SetValues(pfirst2,first2,tolint,plast2,last2,tolint);
	  
	  BndLib_Add2dCurve::Add( C2, first2, last2, Precision::PConfusion(), boxes(j) );
	}
	else {
	  delete [] tabDom;
#ifdef OCCT_DEBUG
	  cout<<"BRepCheck_NoCurveOnSurface or BRepCheck_InvalidRange"<<endl;cout.flush();
#endif
	  if(tabCur(j).IsNull()) {
	    return(BRepCheck_NoCurveOnSurface);
	  }
	  return (BRepCheck_InvalidRange);
	}
      }// if (i == 1) {
      else {
	C2.Load(tabCur(j));
      }
      //
      if (boxes(i).IsOut( boxes(j))) {
	continue;
      }
      //modified by NIZNHY-PKV Fri Oct 29 10:09:01 2010f
      if (E1.IsSame(E2)) {
	continue;
      }
      //modified by NIZNHY-PKV Fri Oct 29 10:09:02 2010t
      //
      //-- ************************************************************
      //-- ******* I n t e r s e c t i o n   C 1   and   C 2   ********
      //-- ************************************************************
      Inter.Perform(C1,myDomain1,C2,tabDom[j-1],tolint,tolint);
      //
      if(Inter.IsDone()) { 
	Standard_Integer nbp, nbs;
	Standard_Real IP_ParamOnFirst, IP_ParamOnSecond;
	IntRes2d_Transition Tr1,Tr2;
	TopTools_ListOfShape CommonVertices;
	TopTools_ListIteratorOfListOfShape itl;
	TopTools_MapOfShape Vmap;
	//
	TopoDS_Iterator it( E1 );
	for (; it.More(); it.Next()) {
	  Vmap.Add( it.Value() );
	}
	//
	it.Initialize( E2 );
	for (; it.More(); it.Next()) {
	  const TopoDS_Shape& V = it.Value();
	  if (Vmap.Contains( V )) {
	    CommonVertices.Append( V );
	  }
	}
	//
	nbp = Inter.NbPoints();
	nbs = Inter.NbSegments();
	IP_ParamOnFirst  = 0.;
	IP_ParamOnSecond = 0.;
	//
	//// **** Points of intersection **** ////
	for (Standard_Integer p = 1; p <= nbp; p++)  {
	  const IntRes2d_IntersectionPoint& IP = Inter.Point(p);
	  IP_ParamOnFirst  = IP.ParamOnFirst();
	  IP_ParamOnSecond = IP.ParamOnSecond();
	  Tr1 = IP.TransitionOfFirst();
	  Tr2 = IP.TransitionOfSecond();
	  if(   Tr1.PositionOnCurve() == IntRes2d_Middle
	     || Tr2.PositionOnCurve() == IntRes2d_Middle)   {
	    //-- Checking of points with true tolerances (ie Tol in 3d)
	    //-- If the point of intersection is within the tolerance of a vertex
	    //-- this intersection is considered correct (no error)
	    Standard_Boolean localok = Standard_False;  
	    Standard_Real f1,l1, f2, l2;
	    TopLoc_Location L, L2;
	    //
	    const Handle(Geom_Curve) ConS = BRep_Tool::Curve(E1,L,f1,l1);    
	    const Handle(Geom_Curve) ConS2 = BRep_Tool::Curve(E2,L2,f2,l2);  
	    //gka protect against working out of edge range
	    if ( f1-IP_ParamOnFirst > ::Precision::PConfusion() || 
		IP_ParamOnFirst-l1 > ::Precision::PConfusion() || 
		f2-IP_ParamOnSecond > ::Precision::PConfusion() || 
		IP_ParamOnSecond-l2 > ::Precision::PConfusion() ) 
	      continue;
	    Standard_Real tolvtt = 0.;
	    //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 Begin
	    if (!ConS.IsNull()) { 
	      P3d = ConS->Value(IP_ParamOnFirst); 
	      P3d.Transform(L.Transformation());
	    } 
	    else {
	      gp_Pnt2d aP2d  = C1.Value(IP_ParamOnFirst);
	      P3d = HS->Value(aP2d.X(), aP2d.Y());
	    }
	    //
	    if (!ConS2.IsNull()) {
	      P3d2 = ConS2->Value(IP_ParamOnSecond); 
	      P3d2.Transform(L2.Transformation());
	    } 
	    else {
	      gp_Pnt2d aP2d  = C2.Value(IP_ParamOnSecond);
	      P3d2 = HS->Value(aP2d.X(), aP2d.Y());
	    }
	    //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 End
	    itl.Initialize( CommonVertices );
	    for (; itl.More(); itl.Next()) {
	      Standard_Real p3dvttDistanceP3d, p3dvttDistanceP3d2;
	      gp_Pnt p3dvtt;
	      //
	      const TopoDS_Vertex& vtt = TopoDS::Vertex(itl.Value());
	      p3dvtt = BRep_Tool::Pnt(vtt);
	      tolvtt =  BRep_Tool::Tolerance(vtt);
	      tolvtt=1.1*tolvtt;
	      tolvtt=tolvtt*tolvtt;
	      p3dvttDistanceP3d  = p3dvtt.SquareDistance(P3d);
	      p3dvttDistanceP3d2 = p3dvtt.SquareDistance(P3d2);
	      //
	      if (p3dvttDistanceP3d<=tolvtt && p3dvttDistanceP3d2<=tolvtt)  { 
		localok = Standard_True;
		break;
	      }
	    }
	    
	    //-- --------------------------------------------------------
	    //-- Check maximum yawn between 2 edges
	    //--
	    //-- Check distance from edges to the curve joining 
	    //-- the point of intersection with vertex (if exists)
	    if (localok == Standard_False && !CommonVertices.IsEmpty()) {
#ifdef OCCT_DEBUG
	      cout << "\n------------------------------------------------------\n" <<endl;
	      cout << "\n--- BRepCheck Wire: AutoIntersection Phase1 -> Erreur \n" <<endl;
	      
#endif
	      Standard_Real distauvtxleplusproche,VParaOnEdge1,VParaOnEdge2;
	      gp_Pnt VertexLePlusProche;
	      //
	      VParaOnEdge1 =0.;
	      VParaOnEdge2 =0.;
	      distauvtxleplusproche=RealLast();
	      //Find the nearest common vertex
	      itl.Initialize( CommonVertices );
	      for (; itl.More(); itl.Next())   {
		Standard_Real disptvtx;
		gp_Pnt p3dvtt;
		//
		const TopoDS_Vertex& vtt = TopoDS::Vertex(itl.Value());
		p3dvtt = BRep_Tool::Pnt(vtt);
		disptvtx = P3d.Distance(p3dvtt);
		if (disptvtx < distauvtxleplusproche)	{
		  VertexLePlusProche = p3dvtt; 
		  distauvtxleplusproche = disptvtx;
		  VParaOnEdge1 = BRep_Tool::Parameter(vtt,E1);
		  VParaOnEdge2 = BRep_Tool::Parameter(vtt,E2);
		}
		// eap: case of closed edge
		else if (IsEqual(distauvtxleplusproche, disptvtx)) {
		  Standard_Real newVParaOnEdge1 = BRep_Tool::Parameter(vtt,E1);
		  Standard_Real newVParaOnEdge2 = BRep_Tool::Parameter(vtt,E2);
		  if (Abs(IP_ParamOnFirst - VParaOnEdge1) + Abs(IP_ParamOnSecond - VParaOnEdge2)
		      >
		      Abs(IP_ParamOnFirst - newVParaOnEdge1) + Abs(IP_ParamOnSecond - newVParaOnEdge2)) {
		    VertexLePlusProche = p3dvtt;
		    VParaOnEdge1 = newVParaOnEdge1;
		    VParaOnEdge2 = newVParaOnEdge2;
		  }
		}
	      }
	      //Patch: extraordinar situation (e.g. tolerance(v) == 0.)
	      //  Modified by skv - Wed Jul 23 12:28:11 2003 OCC1764 Begin
	      // if (VertexLePlusProche.Distance( P3d ) <= gp::Resolution())
	      if (VertexLePlusProche.Distance(P3d)  <= gp::Resolution() ||
		  VertexLePlusProche.Distance(P3d2) <= gp::Resolution()) {
		    //  Modified by skv - Wed Jul 23 12:28:12 2003 OCC1764 End
		localok = Standard_True;
	      }
	      else {
		gp_Lin Lig( VertexLePlusProche, gp_Vec(VertexLePlusProche,P3d) );
		Standard_Real du1 = 0.1*(IP_ParamOnFirst -VParaOnEdge1);
		Standard_Real du2 = 0.1*(IP_ParamOnSecond-VParaOnEdge2);
		Standard_Real maxd1 = 0., maxd2 = 0.;
		Standard_Integer k;
		
		localok = Standard_True;
		Standard_Real tole1 = BRep_Tool::Tolerance(E1);
		for (k = 2; localok && k < 9; k++)	{ 
		  Standard_Real u = VParaOnEdge1 + k*du1;  // check if it works
		  gp_Pnt P1;
		  //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 Begin
		  if (!ConS.IsNull()) {
		    P1 = ConS->Value(u);
		    P1.Transform(L.Transformation());
		  } 
		  else {
		    gp_Pnt2d aP2d  = C1.Value(u);
		    P1 = HS->Value(aP2d.X(), aP2d.Y());
		  }
		  //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 End
		  Standard_Real d1 = Lig.Distance(P1);
		  if (d1 > maxd1) {
		    maxd1 = d1;
		  }
		  if (d1 > tole1*2.0){
		    localok = Standard_False;
		  }
		}
		//-- same for edge2
		//  Modified by skv - Wed Jul 23 12:22:20 2003 OCC1764 Begin
		gp_Dir aTmpDir(P3d2.XYZ().Subtracted(VertexLePlusProche.XYZ()));
		
		Lig.SetDirection(aTmpDir);
		//  Modified by skv - Wed Jul 23 12:22:23 2003 OCC1764 End
		Standard_Real tole2 = BRep_Tool::Tolerance(E2);
		for (k = 2; localok && k < 9; k++) {
		  Standard_Real u = VParaOnEdge2 + k*du2;  // check if it works
		  gp_Pnt        P2;
		  //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 Begin
		  if (!ConS2.IsNull()) {
		    P2 = ConS2->Value(u);
		    P2.Transform(L2.Transformation());
		  }
		  else {
		    gp_Pnt2d aP2d  = C2.Value(u);
		    P2 = HS->Value(aP2d.X(), aP2d.Y());
		  }
		  //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 End
		  Standard_Real d2 = Lig.Distance(P2);
		  if (d2 > maxd2) {
		    maxd2 = d2;
		  }
		  if (d2 > tole2*2.0){
		    localok = Standard_False;
		  }
		}
#ifdef OCCT_DEBUG
		if(localok) { 
		  printf("--- BRepCheck Wire: AutoIntersection Phase2 -> Bon \n");
		  printf("--- distance Point Vertex : %10.7g (tol %10.7g)\n",distauvtxleplusproche,tolvtt);
		  printf("--- Erreur Max sur E1 : %10.7g  Tol_Edge:%10.7g\n",maxd1,tole1);
		  printf("--- Erreur Max sur E2 : %10.7g  Tol_Edge:%10.7g\n",maxd2,tole2);
		  fflush(stdout);
		}
		else { 
		  printf("--- BRepCheck Wire: AutoIntersection Phase2 -> Erreur \n");
		  printf("--- distance Point Vertex : %10.7g (tol %10.7g)\n",distauvtxleplusproche,tolvtt);
		  printf("--- Erreur Max sur E1 : %10.7g  Tol_Edge:%10.7g\n",maxd1,tole1);
		  printf("--- Erreur Max sur E2 : %10.7g  Tol_Edge:%10.7g\n",maxd2,tole2);
		  fflush(stdout);
		}
#endif
	      } //end of else (construction of the line Lig)
	    } //end of if (localok == Standard_False && !CommonVertices.IsEmpty())
	    //
	    if(localok==Standard_False)	  { 
	      retE1=E1;
	      retE2=E2;
	      if (Update) {
		BRepCheck::Add(myMap(myShape),BRepCheck_SelfIntersectingWire);
		}
#ifdef OCCT_DEBUG
	      static Standard_Integer numpoint1=0;
	      cout<<"point p"<<++numpoint1<<" "<<P3d.X()<<" "<<P3d.Y()<<" "<<P3d.Z()<<endl;
	      cout.flush();
#endif
	      delete [] tabDom;
	      return(BRepCheck_SelfIntersectingWire);
	    } //-- localok == False
	  } //end of if(Tr1.PositionOnCurve() == IntRes2d_Middle || Tr2.PositionOnCurve() == IntRes2d_Middle)
	} //end of for (Standard_Integer p=1; p <= nbp; p++)    
	////
	//// **** Segments of intersection **** ////
	for (Standard_Integer s = 1; s <= nbs; ++s) {
	  const IntRes2d_IntersectionSegment& Seg = Inter.Segment(s);
	  if (Seg.HasFirstPoint() && Seg.HasLastPoint())   { 
	    Standard_Boolean localok;
	    Standard_Integer k;
	    IntRes2d_IntersectionPoint PSeg [2];
	    IntRes2d_Position aPCR1, aPCR2;
	    //
	    localok = Standard_False;
	    PSeg[0] = Seg.FirstPoint();
	    PSeg[1] = Seg.LastPoint();
	    // At least one of extremities of the segment must be inside
	    // the tolerance of a common vertex
	    for (k = 0; k < 2; ++k) {
	      IP_ParamOnFirst  = PSeg[k].ParamOnFirst();
	      IP_ParamOnSecond = PSeg[k].ParamOnSecond();
	      Tr1 = PSeg[k].TransitionOfFirst();
	      Tr2 = PSeg[k].TransitionOfSecond();
	      aPCR1=Tr1.PositionOnCurve();
	      aPCR2=Tr2.PositionOnCurve();
	      //
	      if(aPCR1!=IntRes2d_Middle && aPCR2!=IntRes2d_Middle)  {
		GeomAbs_CurveType aCT1, aCT2;
		//ZZ
		aCT1=C1.GetType();
		aCT2=C2.GetType();
		if (aCT1==GeomAbs_Line && aCT2==GeomAbs_Line) {
		  // check for the two lines coincidence
		  Standard_Real aPAR_T, aT11, aT12, aT21, aT22, aT1m, aT2m;
		  Standard_Real aD2, aTolE1, aTolE2,  aTol2, aDot;
		  gp_Lin2d aL1, aL2;
		  gp_Pnt2d aP1m;
		  //
		  aPAR_T=0.43213918;
		  //
		  aTolE1=BRep_Tool::Tolerance(E1);
		  aTolE2=BRep_Tool::Tolerance(E2);
		  aTol2=aTolE1+aTolE2;
		  aTol2=aTol2*aTol2;
		  //
		  aL1=C1.Line();
		  aL2=C2.Line();
		  //
		  aT11=PSeg[0].ParamOnFirst();
		  aT12=PSeg[1].ParamOnFirst();
		  aT21=PSeg[0].ParamOnSecond();
		  aT22=PSeg[1].ParamOnSecond();
		  //
		  aT1m=(1.-aPAR_T)*aT11 + aPAR_T*aT12;
		  aP1m=C1.Value(aT1m);
		  //
		  aD2=aL2.SquareDistance(aP1m);
		  if (aD2<aTol2) {
		    aT2m=ElCLib::Parameter(aL2, aP1m);
		    if (aT2m>aT21 && aT2m<aT22) {
		      const gp_Dir2d& aDir1=aL1.Direction();
		      const gp_Dir2d& aDir2=aL2.Direction();
		      aDot=aDir1*aDir2;
		      if (aDot<0.) {
			aDot=-aDot;
		      }
		      //
		      if ((1.-aDot)<5.e-11){//0.00001 rad
			localok = Standard_False;
			break;// from for (k = 0; k < 2; ++k){...
		      }
		    }//if (aT2m>aT21 && aT2m<aT22) {
		  }//if (aD2<aTol2) {
		}//if (aCT1==GeomAbs_Line && aCT2==GeomAbs_Line) {
		//ZZ
		localok = Standard_True;
		break;
	      }
	      //
	      Standard_Real f,l, tolvtt;
	      TopLoc_Location L, L2;
	      const Handle(Geom_Curve)& ConS = BRep_Tool::Curve(E1,L,f,l);    
	      const Handle(Geom_Curve)& ConS2 = BRep_Tool::Curve(E2,L2,f,l);    
	      //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 Begin
	      if (!ConS.IsNull()) { 
		P3d = ConS->Value(IP_ParamOnFirst); 
		P3d.Transform(L.Transformation());
	      } else {
		gp_Pnt2d aP2d  = C1.Value(IP_ParamOnFirst);
		P3d = HS->Value(aP2d.X(), aP2d.Y());
	      }
	      if (!ConS2.IsNull()) {
		P3d2 = ConS2->Value(IP_ParamOnSecond); 
		P3d2.Transform(L2.Transformation());
	      } else {
		gp_Pnt2d aP2d  = C2.Value(IP_ParamOnSecond);
		P3d2 = HS->Value(aP2d.X(), aP2d.Y());
	      }
	      //  Modified by Sergey KHROMOV - Mon Apr 15 12:34:22 2002 End
	      itl.Initialize( CommonVertices );
	      for (; itl.More(); itl.Next()) {
		Standard_Real p3dvttDistanceP3d, p3dvttDistanceP3d2;
		gp_Pnt p3dvtt;
		//
		const TopoDS_Vertex& vtt = TopoDS::Vertex(itl.Value());
		p3dvtt = BRep_Tool::Pnt(vtt);
		tolvtt =  BRep_Tool::Tolerance(vtt);
		tolvtt=1.1*tolvtt;
		tolvtt=tolvtt*tolvtt;
		p3dvttDistanceP3d  = p3dvtt.SquareDistance(P3d);
		p3dvttDistanceP3d2 = p3dvtt.SquareDistance(P3d2);
		if (p3dvttDistanceP3d <= tolvtt && p3dvttDistanceP3d2 <= tolvtt) { 
		  localok = Standard_True;
		  break;
		}
	      }
	      if (localok == Standard_True) {
		break;
	      }
	    } //end of for (k = 0; k < 2; k++)
	    //
	    if(localok==Standard_False)	  { 
	      retE1=E1;
	      retE2=E2;
	      if (Update) {
		BRepCheck::Add(myMap(myShape),BRepCheck_SelfIntersectingWire);
	      }
#ifdef OCCT_DEBUG
	      static Standard_Integer numpoint1=0;
	      cout<<"point p"<<++numpoint1<<" "<<P3d.X()<<" "<<P3d.Y()<<" "<<P3d.Z()<<endl;
	      cout.flush();
#endif
	      delete [] tabDom;
	      return(BRepCheck_SelfIntersectingWire);
	    } //-- localok == False
	  } //end of if(Seg.HasFirstPoint() && Seg.HasLastPoint())
	} //end of for (Standard_Integer s = 1; s <= nbs; p++)
      } //-- Inter.IsDone()
    } //end of for( j = i+1; j<=Nbedges; j++)
  } //end of for(i = 1; i <= Nbedges; i++)
  //
  delete [] tabDom;
  if (Update) {
    BRepCheck::Add(myMap(myShape),BRepCheck_NoError);
  }
  //
  return (BRepCheck_NoError);
}

//=======================================================================
//function :   SetStatus
//purpose  : 
//=======================================================================

void BRepCheck_Wire::SetStatus(const BRepCheck_Status theStatus)
{
    BRepCheck::Add(myMap(myShape),theStatus);
}

//=======================================================================
//function : GeometricControls
//purpose  : 
//=======================================================================
void BRepCheck_Wire::GeometricControls(const Standard_Boolean B)
{
  if (myGctrl != B) {
    if (B) {
      myCdone = Standard_False;
    }
    myGctrl = B;
  }
}
//=======================================================================
//function : GeometricControls
//purpose  : 
//=======================================================================
Standard_Boolean BRepCheck_Wire::GeometricControls() const
{
  return myGctrl;
}

//=======================================================================
//function : Propagate
//purpose  : fill <mapE> with edges connected to <edg> through vertices
//           contained in <mapVE>
//=======================================================================
static void Propagate(const TopTools_IndexedDataMapOfShapeListOfShape& mapVE,
                      const TopoDS_Shape& edg,
                      TopTools_MapOfShape& mapE)
{
  TopTools_ListOfShape currentEdges;
  currentEdges.Append(edg);

  do
  {
    TopTools_ListOfShape nextEdges;
    TopTools_ListIteratorOfListOfShape itrc(currentEdges);
    for (; itrc.More(); itrc.Next())
    {
      const TopoDS_Shape& Edge = itrc.Value();
      if (!mapE.Contains(Edge)) 
        mapE.Add(Edge);

      TopExp_Explorer ex(Edge, TopAbs_VERTEX);
      for (; ex.More(); ex.Next())
      {
        const TopoDS_Vertex& vtx = TopoDS::Vertex(ex.Current());
        Standard_Integer indv = mapVE.FindIndex(vtx);
        if (indv != 0)
        {
          const TopTools_ListOfShape& edges = mapVE(indv);

          TopTools_ListIteratorOfListOfShape itl(edges);
          for (; itl.More(); itl.Next())
          {
            const TopoDS_Shape& E = itl.Value();
            if (!Edge.IsSame(E) && !mapE.Contains(E))
            {
              mapE.Add(E);
              nextEdges.Append(E);
            }
          }
        }
      }
    }
    currentEdges = nextEdges;
  }
  while (!currentEdges.IsEmpty());
}

//=======================================================================
//function : GetOrientation
//purpose  : 
//=======================================================================

static TopAbs_Orientation GetOrientation(const TopTools_MapOfShape& mapE,
					 const TopoDS_Edge& edg)
{
  TopTools_MapIteratorOfMapOfShape itm(mapE);
  for ( ; itm.More(); itm.Next()) {
    if (itm.Key().IsSame(edg)) {
      break;
    }
  }
  return itm.Key().Orientation();
}
//=======================================================================
//function : ChoixUV
//purpose  : For vertex theVertex given function find an edge along 
//           that we should go further.
//=======================================================================
void ChoixUV(const TopoDS_Vertex& theVertex,
             const TopoDS_Edge& theEdge,
             const TopoDS_Face& theFace,
             TopTools_ListOfShape& theLOfShape)
  {
  TopTools_ListIteratorOfListOfShape It( theLOfShape );
  while (It.More())
    {
    if (theEdge.IsSame( It.Value() ))
      theLOfShape.Remove( It );
    else
      It.Next();
    }

  Standard_Real aTol3d	= BRep_Tool::Tolerance(theVertex);

  Standard_Integer anIndex = 0, anIndMin = 0;
  TopoDS_Edge anEFound;
  gp_Pnt2d aPntRef, aPnt;
  gp_Vec2d aDerRef, aDer;
  Standard_Real aMinAngle, aMaxAngle, anAngle;
  Standard_Real a_gpResolution=gp::Resolution();
  TopAbs_Orientation aVOrientation, anEdgOrientation;
  Standard_Real aParam = 0.0, aFirstParam = 0.0, aLastParam = 0.0, aParPiv = 0.0;
  BRepAdaptor_Surface aFaceSurface(theFace,Standard_False); // no restriction
  
  Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(theEdge, theFace, aFirstParam, aLastParam);
  if (C2d.IsNull())// JAG 10.12.96
    return;

  aVOrientation = theVertex.Orientation();
  anEdgOrientation = theEdge.Orientation();
  
  aParPiv =(aVOrientation==anEdgOrientation) ? aFirstParam : aLastParam;
  aMinAngle = RealLast();
  aMaxAngle = RealFirst();

  CurveDirForParameter(C2d, aParPiv, aPntRef, aDerRef);
  
  if (aVOrientation != anEdgOrientation)
    aDerRef.Reverse();

  It.Initialize(theLOfShape);

  for (; It.More(); It.Next())
    {
    anIndex++;
    const TopoDS_Edge& anE=TopoDS::Edge(It.Value());
    C2d = BRep_Tool::CurveOnSurface(anE, theFace, aFirstParam, aLastParam);
    if(C2d.IsNull())
      continue;
    Geom2dAdaptor_Curve aCA(C2d);

    aParam =(aVOrientation != anE.Orientation()) ? aFirstParam : aLastParam;
    aPnt = aCA.Value(aParam);

    if(!IsDistanceIn2DTolerance(aFaceSurface, aPnt, aPntRef, aTol3d, Standard_False))
      continue;

    CurveDirForParameter(aCA, aParam, aPnt, aDer);

    if (aVOrientation == anE.Orientation())
      aDer.Reverse();

    if ((aDerRef.Magnitude() <= a_gpResolution) || 
                 (aDer.Magnitude() <= a_gpResolution))
//Vector length is too small
      continue;

    anAngle = -aDerRef.Angle( aDer );

    if ( anAngle < 0. )
      anAngle += 2.*M_PI;

    if ( theFace.Orientation() == TopAbs_FORWARD )
      {
      if ( anAngle < aMinAngle )
        {
        anIndMin = anIndex;
        aMinAngle = anAngle;
        }
      }
    else //theFace.Orientation() != TopAbs_FORWARD
      {
      if ( anAngle > aMaxAngle )
        {
        anIndMin = anIndex;
        aMaxAngle = anAngle;
        }
      }
    }//end of for

// Update edge
  if (anIndMin == 0)
    if (theLOfShape.Extent() == 1)
      {
      Standard_Boolean IsFound = Standard_True; //all right
      anEFound = TopoDS::Edge(theLOfShape.First());

      if(anEFound.IsNull() || BRep_Tool::Degenerated(theEdge) ||
                                  BRep_Tool::Degenerated(anEFound))
        IsFound = Standard_False; //bad
      else if (!IsDistanceIn2DTolerance(aFaceSurface, aPnt, aPntRef, aTol3d))
        IsFound = Standard_False; //bad
      else 
        // clousureness in 3D
        {
//IsDistanceIn3DTolerance
        BRepAdaptor_Curve bcEdg(theEdge, theFace);
        BRepAdaptor_Curve bcEvois(anEFound, theFace);
        gp_Pnt pEdg = bcEdg.Value(aParPiv);
        gp_Pnt pEFound = bcEvois.Value(aParam);

        if(!IsDistanceIn3DTolerance(pEdg, pEFound, aTol3d))
          IsFound = Standard_False;
        else
//angle was not defined but points are close
          IsFound = Standard_True; //all right
        }

      if(!IsFound)
        {
        theLOfShape.Clear();
        }
      }//if (theLOfShape.Extent() == 1)
    else //if (anIndMin == 0)
      {
      theLOfShape.Clear();
      }
  else
    {
    anIndex = 1;

    while (anIndex < anIndMin)
      {
      theLOfShape.RemoveFirst();
      anIndex++;
      }

    It.Initialize(theLOfShape);
    It.Next();

    while (It.More())
      theLOfShape.Remove(It);
    }
  }//End of function


//=======================================================================
//function : CurveDirForParameter
//purpose  : 
//=======================================================================
void CurveDirForParameter(const Geom2dAdaptor_Curve& aC2d,
                          const Standard_Real aPrm,
                          gp_Pnt2d& Pnt,
                          gp_Vec2d& aVec2d)
{
  Standard_Real aTol=gp::Resolution();
  Standard_Integer i;

  aC2d.D1(aPrm, Pnt, aVec2d);
  //
  if (aVec2d.Magnitude() <= aTol) {
    for (i = 2; i <= 100; i++){
      aVec2d = aC2d.DN(aPrm, i);
      if (aVec2d.Magnitude() > aTol) {
        break;
      }
    }
  }
}

//  Modified by Sergey KHROMOV - Wed May 22 10:44:06 2002 OCC325 Begin
//=======================================================================
//function : GetPnts2d
//purpose  : this function returns the parametric points of theVertex on theFace.
//           If theVertex is a start and end vertex of theEdge hasSecondPnt
//           becomes Standard_True and aPnt2 returns the second parametric point.
//           Returns Standard_True if paraametric points are successfully found.
//=======================================================================

static Standard_Boolean GetPnt2d(const TopoDS_Vertex    &theVertex,
				 const TopoDS_Edge      &theEdge,
				 const TopoDS_Face      &theFace,
				       gp_Pnt2d         &aPnt)
{
  Handle(Geom2d_Curve) aPCurve;
  Standard_Real        aFPar;
  Standard_Real        aLPar;
  Standard_Real        aParOnEdge;
  TopoDS_Vertex        aFirstVtx;
  TopoDS_Vertex        aLastVtx;

  TopExp::Vertices(theEdge, aFirstVtx, aLastVtx);

  if (!theVertex.IsSame(aFirstVtx) && !theVertex.IsSame(aLastVtx))
    return Standard_False;

  aPCurve = BRep_Tool::CurveOnSurface(theEdge, theFace, aFPar, aLPar);

  if (aPCurve.IsNull())
    return Standard_False;

  aParOnEdge = BRep_Tool::Parameter(theVertex, theEdge);
  aPnt       = aPCurve->Value(aParOnEdge);

  return Standard_True;
}

//=======================================================================
//function : Closed2dForPeriodicFace
//purpose  : Checks the distance between first point of the first edge
//           and last point of the last edge in 2d for periodic face.
//=======================================================================
static Standard_Boolean IsClosed2dForPeriodicFace
                        (const TopoDS_Face   &theFace,
			 const gp_Pnt2d      &theP1,
			 const gp_Pnt2d      &theP2,
			 const TopoDS_Vertex &theVertex)
{
// Check 2d distance for periodic faces with seam edge.
// Searching for seam edges
  TopTools_ListOfShape aSeamEdges;
  TopTools_MapOfShape  NotSeams;
  TopTools_MapOfShape  ClosedEdges;
  TopExp_Explorer      anExp(theFace, TopAbs_EDGE);

  for (;anExp.More(); anExp.Next()) {
    TopoDS_Edge anEdge = TopoDS::Edge(anExp.Current());

    if (NotSeams.Contains(anEdge))
      continue;

    if (!IsOriented(anEdge) ||
	!BRep_Tool::IsClosed(anEdge, theFace)) {
      NotSeams.Add(anEdge);
      continue;
    }

    if (!ClosedEdges.Add(anEdge))
      aSeamEdges.Append(anEdge);
  }

  if (aSeamEdges.Extent() == 0)
    return Standard_True;

// check if theVertex lies on one of the seam edges
  BRepAdaptor_Surface aFaceSurface (theFace, Standard_False);
  Standard_Real       aTol      = BRep_Tool::Tolerance(theVertex);
  Standard_Real       aUResol   = aFaceSurface.UResolution(aTol);
  Standard_Real       aVResol   = aFaceSurface.VResolution(aTol);
  Standard_Real       aVicinity = Sqrt(aUResol*aUResol + aVResol*aVResol);
  Standard_Real       aDistP1P2 = theP1.Distance(theP2);


  TopTools_ListIteratorOfListOfShape anIter(aSeamEdges);

  for (; anIter.More(); anIter.Next()) {
    TopoDS_Edge aSeamEdge = TopoDS::Edge(anIter.Value());

    anExp.Init(aSeamEdge, TopAbs_VERTEX);
    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &aVtx = anExp.Current();

// We found an edge. Check the distance between two given points
//  to be lower than the computed tolerance.
      if (IsOriented(aVtx) && aVtx.IsSame(theVertex)) {
	gp_Pnt2d         aPnt1;
	gp_Pnt2d         aPnt2;
	Standard_Real    a2dTol;

	if (!GetPnt2d(theVertex, aSeamEdge, theFace, aPnt1))
	  continue;

	aSeamEdge = TopoDS::Edge(aSeamEdge.Reversed());

	if (!GetPnt2d(theVertex, aSeamEdge, theFace, aPnt2))
	  continue;

	a2dTol = aPnt1.Distance(aPnt2)*1.e-2;
	a2dTol = Max(a2dTol, aVicinity);

	if (aDistP1P2 > a2dTol)
	  return Standard_False;
      }
    }
  }

  return Standard_True;
}
//  Modified by Sergey KHROMOV - Thu Jun 20 10:58:05 2002 End
