// Created on: 1997-10-14
// Created by: Olga KOULECHOVA
// Copyright (c) 1997-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgo.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepExtrema_ExtCF.hxx>
#include <BRepExtrema_ExtPC.hxx>
#include <BRepFeat.hxx>
#include <BRepFeat_MakeRevolutionForm.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <CSLib.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAPI_ExtremaCurveCurve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomProjLib.hxx>
#include <gp_Ax1.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <LocOpe.hxx>
#include <LocOpe_CSIntersector.hxx>
#include <LocOpe_FindEdges.hxx>
#include <LocOpe_Gluer.hxx>
#include <LocOpe_PntFace.hxx>
#include <LocOpe_RevolutionForm.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColGeom_Array1OfCurve.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean BRepFeat_GettraceFEAT();
extern Standard_Boolean BRepFeat_GettraceFEATRIB();
#endif

static void MajMap(const TopoDS_Shape&, // base
		   const LocOpe_RevolutionForm&,
		   TopTools_DataMapOfShapeListOfShape&, // myMap
		   TopoDS_Shape&,  // myFShape
		   TopoDS_Shape&); // myLShape

static void SetGluedFaces(const TopTools_DataMapOfShapeListOfShape& theSlmap,
                          LocOpe_RevolutionForm&,
			  const TopTools_DataMapOfShapeListOfShape& SlidingMap,
			  TopTools_DataMapOfShapeShape&);

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepFeat_MakeRevolutionForm::Init(const TopoDS_Shape& Sbase,
				       const TopoDS_Wire& W,
				       const Handle(Geom_Plane)& Plane,
				       const gp_Ax1& Axis,
				       const Standard_Real H1,
				       const Standard_Real H2,
				       const Standard_Integer Mode,
				       Standard_Boolean& Modify)
{ 
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeRevolutionForm::Init" << endl;
#endif
  Standard_Boolean RevolRib = Standard_True;
  Done();

// modify = 0 if it is not required to make sliding
//        = 1 if it is intended to try to make sliding
  Standard_Boolean Sliding = Modify;

  myAxe = Axis;
  Handle(Geom_Line) Line = new Geom_Line(Axis); 
  Standard_Real LineFirst, LineLast;
  
  LocOpe_CSIntersector ASI(Sbase);
  TColGeom_SequenceOfCurve scur;
  scur.Clear();
  scur.Append(Line);
  ASI.Perform(scur);
  if(ASI.IsDone() && ASI.NbPoints(1) >= 2) {
    LineLast = ASI.Point(1, ASI.NbPoints(1)).Parameter();
    LineFirst = ASI.Point(1, 1).Parameter();
  }
  else {
    LineFirst = RealFirst();
    LineLast = RealLast();
  }
  
  Handle(Geom2d_Curve) ln2d = GeomAPI::To2d(Line, Plane->Pln());
  
  TopExp_Explorer exx;
  Standard_Real Rad = RealLast();
  
  exx.Init(W, TopAbs_EDGE);
  for(; exx.More(); exx.Next()) {
    const TopoDS_Edge& e = TopoDS::Edge(exx.Current());
    Standard_Real f, l;
    Handle(Geom_Curve) c = BRep_Tool::Curve(e, f, l);
    Handle(Geom2d_Curve) c2d = GeomAPI::To2d(c, Plane->Pln());
    Geom2dAPI_ExtremaCurveCurve extr(ln2d, c2d, LineFirst, LineLast,f,l);
    Standard_Real L = RealLast();
    if(extr.NbExtrema() >= 1) {
      L = extr.LowerDistance();
    }
    gp_Pnt p1 = c->Value(f);
    gp_Pnt p2 = c->Value(l);
    GeomAPI_ProjectPointOnCurve proj1(p1, Line);
    GeomAPI_ProjectPointOnCurve proj2(p2, Line);
    if(proj1.NbPoints() < 1 || proj2.NbPoints() < 1) {
#ifdef OCCT_DEBUG
      if (trc) cout << " No projection points" << endl;
#endif
      myStatusError = BRepFeat_NoProjPt;
      NotDone();
      return;
    }
    Standard_Real par1 = proj1.Distance(1);
    Standard_Real par2 = proj2.Distance(1);
    Standard_Real Par  = Min(par1, par2);
    if(Par<L) L = Par;
    if(L<Rad && L > 0.) Rad = L;
  }
  
  Standard_Real height = Min(H1, H2);
  
  if(Rad <= height) Rad = height + 0.01*height;  
  
  myAngle1 = asin(H1/Rad) + M_PI/10.;
  myAngle2 = asin(H2/Rad) + M_PI/10.;
  
  if((myAngle1 - M_PI/2) > Precision::Confusion())
    myAngle1 = M_PI/2;
  if((myAngle2 - M_PI/2) > Precision::Confusion())
    myAngle2 = M_PI/2;
  
  mySkface.Nullify();
  myPbase.Nullify();

  if(Mode == 0) 
    myFuse   = Standard_False;
  else // if(Mode == 1) 
    myFuse   = Standard_True;
#ifdef OCCT_DEBUG
  if (trc) {
    if (myFuse)  cout << " Fuse" << endl;
    if (!myFuse)  cout << " Cut" << endl;
  }
#endif

// ---Determination Tolerance : tolerance max on parameters
  myTol = Precision::Confusion();
  
  exx.Init(W, TopAbs_VERTEX);
  for(; exx.More(); exx.Next()) {
    const Standard_Real& tol = BRep_Tool::
      Tolerance(TopoDS::Vertex(exx.Current()));
    if(tol > myTol) myTol = tol;
  }
  
  exx.Init(Sbase, TopAbs_VERTEX);
  for(; exx.More(); exx.Next()) {
    const Standard_Real& tol = BRep_Tool::
      Tolerance(TopoDS::Vertex(exx.Current()));
    if(tol > myTol) myTol = tol;
  }

  TopoDS_Shape aLocalShapeW = W.Oriented(TopAbs_FORWARD);
  myWire = TopoDS::Wire(aLocalShapeW);
//  myWire = TopoDS::Wire(W.Oriented(TopAbs_FORWARD));
  myPln = Plane;
  myHeight1 = H1;
  myHeight2 = H2;
  
  mySbase  = Sbase;
  mySlface.Clear();
  myShape.Nullify();
  myMap.Clear();
  myFShape.Nullify();
  myLShape.Nullify();

// ---Calculate bounding box
  BRep_Builder BB;
  
  TopTools_ListOfShape theList;  
  
  TopoDS_Shape U;
  U.Nullify();
  gp_Pnt FirstCorner, LastCorner;
  Standard_Real bnd = HeightMax(mySbase, U, FirstCorner, LastCorner);
  myBnd = bnd;
  
  BRepPrimAPI_MakeBox Bndbox(FirstCorner, LastCorner);
  TopoDS_Solid BndBox = Bndbox.Solid();


// ---Construction of the working plane face (section bounding box)
  BRepLib_MakeFace PlaneF(myPln->Pln(), -6.*myBnd, 
			  6.*myBnd, -6.*myBnd, 6.*myBnd);
  TopoDS_Face PlaneFace = TopoDS::Face(PlaneF.Shape());
  
  BRepAlgoAPI_Common PlaneS(BndBox, PlaneFace);
  TopExp_Explorer EXP;
  TopoDS_Shape PlaneSect = PlaneS.Shape();
  EXP.Init(PlaneSect, TopAbs_WIRE);
  TopoDS_Wire www = TopoDS::Wire(EXP.Current());
  BRepLib_MakeFace Bndface(myPln->Pln(), www, Standard_True);
  TopoDS_Face BndFace = TopoDS::Face(Bndface.Shape());


// ---Find base faces of the rib    
  TopoDS_Edge FirstEdge, LastEdge;
  TopoDS_Face FirstFace, LastFace;
  TopoDS_Vertex FirstVertex, LastVertex;
  
  Standard_Boolean OnFirstFace = Standard_False;
  Standard_Boolean OnLastFace = Standard_False;
  Standard_Boolean PtOnFirstEdge = Standard_False;
  Standard_Boolean PtOnLastEdge = Standard_False;
  TopoDS_Edge OnFirstEdge, OnLastEdge;
  OnFirstEdge.Nullify();
  OnLastEdge.Nullify();

  Standard_Boolean Data = ExtremeFaces(RevolRib, myBnd, myPln, FirstEdge, LastEdge, 
				       FirstFace, LastFace, FirstVertex, 
				       LastVertex, OnFirstFace, OnLastFace,
				       PtOnFirstEdge, PtOnLastEdge,
				       OnFirstEdge, OnLastEdge);
  
  if(!Data) {
#ifdef OCCT_DEBUG
    if (trc) cout << " No Extreme faces" << endl;
#endif
    myStatusError = BRepFeat_NoExtFace;
    NotDone();
    return;
  }


// ---Proofing Point for the side of the wire to be filled - material side
  gp_Pnt CheckPnt = CheckPoint(FirstEdge, bnd/10., myPln);
  
//  Standard_Real f, l;

// ---Control sliding valid
// Many cases when the sliding is abandoned
  Standard_Integer Concavite = 3;  // a priori the profile is not concave
  
  myFirstPnt = BRep_Tool::Pnt(FirstVertex);
  myLastPnt  = BRep_Tool::Pnt(LastVertex);

// SliList : list of faces concerned by the rib
  TopTools_ListOfShape SliList;
  SliList.Append(FirstFace);
  
  if(Sliding) {    // sliding
#ifdef OCCT_DEBUG
    if (trc) cout << " Sliding" << endl;
#endif
    Handle(Geom_Surface) s = BRep_Tool::Surface(FirstFace);
    if (s->DynamicType() == 
	STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
      s = Handle(Geom_RectangularTrimmedSurface)::
	DownCast(s)->BasisSurface();
    }
    if(s->DynamicType() != STANDARD_TYPE(Geom_Plane) && 
       s->DynamicType() != STANDARD_TYPE(Geom_CylindricalSurface) &&
       s->DynamicType() != STANDARD_TYPE(Geom_ConicalSurface) &&
       s->DynamicType() != STANDARD_TYPE(Geom_ToroidalSurface)) 
      Sliding = Standard_False;
  }

  if(Sliding) {     // sliding
    Handle(Geom_Surface) ss = BRep_Tool::Surface(LastFace);
    if (ss->DynamicType() == 
	STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
      ss = Handle(Geom_RectangularTrimmedSurface)::
	DownCast(ss)->BasisSurface();
    }
    if(ss->DynamicType() != STANDARD_TYPE(Geom_Plane) && 
       ss->DynamicType() != STANDARD_TYPE(Geom_CylindricalSurface) &&
       ss->DynamicType() != STANDARD_TYPE(Geom_ConicalSurface) &&
       ss->DynamicType() != STANDARD_TYPE(Geom_ToroidalSurface)) 
      Sliding = Standard_False;
  }

// Control only start and end points no control at the middle to improve
// If make a control between Surface and segment 2 points limited
// -> too expensive - to improve  
  //gp_Pnt FirstCenter, LastCenter;
  gp_Circ FirstCircle, LastCircle;
  Handle(Geom_Curve) FirstCrv, LastCrv;
  
  if(Sliding) {    // sliding
    GeomAPI_ProjectPointOnCurve proj(myFirstPnt, Line);
    if(proj.NbPoints() < 1) {
#ifdef OCCT_DEBUG
      if (trc) cout << " No First Point projection" << endl;
#endif
      myStatusError = BRepFeat_NoProjPt;
      NotDone();
      return;
    }
    Standard_Real FirstRayon = proj.Distance(1);
    gp_Pnt FirstCenter = proj.Point(1);
    
    GeomAPI_ProjectPointOnCurve proj1(myLastPnt, Line);
    if(proj.NbPoints() < 1) {
#ifdef OCCT_DEBUG
      if (trc) cout << " No Last Point projection" << endl;
#endif
      myStatusError = BRepFeat_NoProjPt;
      NotDone();
      return;
    }
    Standard_Real LastRayon = proj1.Distance(1);
    gp_Pnt LastCenter = proj1.Point(1);
    
    gp_Vec axv(myAxe.Direction());
    gp_Ax2 ax2(FirstCenter, axv);
    gp_Ax2 ax2p(LastCenter, axv);
    gp_Circ theFC(ax2, FirstRayon);
    gp_Circ theLC(ax2p, LastRayon);
    
    gp_Pnt RFirstPnt1 = myFirstPnt.Rotated(myAxe, myAngle1);
    gp_Pnt RLastPnt1 = myLastPnt.Rotated(myAxe, myAngle1);
    gp_Pnt RFirstPnt2 = myFirstPnt.Rotated(myAxe, -myAngle2);
    gp_Pnt RLastPnt2 = myLastPnt.Rotated(myAxe, -myAngle2);
    
    BRep_Builder Bu;
    TopoDS_Vertex v1, v2, v3, v4;
    Bu.MakeVertex(v1, RFirstPnt2, Precision::Confusion());
    Bu.MakeVertex(v2, RFirstPnt1, Precision::Confusion());
    Bu.MakeVertex(v3, RLastPnt2, Precision::Confusion());
    Bu.MakeVertex(v4, RLastPnt1, Precision::Confusion());
    
    BRepLib_MakeEdge ee1(theFC, v1, v2);
    BRepLib_MakeEdge ee2(theLC, v3, v4);
    
    if(Sliding && !PtOnFirstEdge) {
      BRepExtrema_ExtCF ext1(TopoDS::Edge(ee1.Shape()), FirstFace);
      if(ext1.NbExt() < 1 || ext1.SquareDistance(1) > Precision::SquareConfusion())
	Sliding = Standard_False;
    }
    if(Sliding && !PtOnLastEdge) {
      BRepExtrema_ExtCF ext2(ee2, LastFace); // ExtCF : curves and surfaces
      if(ext2.NbExt() < 1 || ext2.SquareDistance(1) > Precision::SquareConfusion())
	Sliding = Standard_False;
    }
    if(Sliding && PtOnFirstEdge) {
      Standard_Real f, l;
      FirstCrv = BRep_Tool::Curve(OnFirstEdge, f, l);
      if(FirstCrv->DynamicType() != STANDARD_TYPE(Geom_Circle)) 
	Sliding = Standard_False;
      else {
	Handle(Geom_Circle) C1 = Handle(Geom_Circle)::DownCast(FirstCrv);
	gp_Circ Circ = C1->Circ();
	FirstCircle = Circ;
	gp_Ax1 circax = FirstCircle.Axis();
	if(!circax.IsCoaxial(myAxe, Precision::Confusion(), 
			     Precision::Confusion())) 
	  Sliding = Standard_False;
	else {
//#ifndef OCCT_DEBUG
	  if(fabs(FirstCircle.Radius()-FirstRayon) >=
//#else
//	  if(abs(FirstCircle.Radius()-FirstRayon) >=
//#endif
	     Precision::Confusion()) 
	    Sliding = Standard_False;
	}	
      }
    }
    
    if(Sliding && PtOnLastEdge) {
      Standard_Real f, l;
      LastCrv = BRep_Tool::Curve(OnLastEdge, f, l);
      if(LastCrv->DynamicType() != STANDARD_TYPE(Geom_Circle)) 
	Sliding = Standard_False;
      else {
	Handle(Geom_Circle) C1 = Handle(Geom_Circle)::DownCast(LastCrv);
	gp_Circ Circ = C1->Circ();
	LastCircle = Circ;
	gp_Ax1 circax = LastCircle.Axis();
	if(!circax.IsCoaxial(myAxe, Precision::Confusion(), 
			     Precision::Confusion())) 
	  Sliding = Standard_False;
	else {
	  Standard_Real rad = LastCircle.Radius();
//#ifndef OCCT_DEBUG
	  if(fabs(rad - LastRayon) >= Precision::Confusion()) { 
//#else
//	  if(abs(rad - LastRayon) >= Precision::Confusion()) { 
//#endif
	    Sliding = Standard_False;
	  }	
	}
      }
    }
  }


// Construct a great profile that goes till the bounding box
// -> by tangency with first and last edge of the Wire
// -> by normals to base faces : statistically better
// Intersect everythin to find the final profile


// ---case of sliding : construction of the face profile
  if(Sliding) {
#ifdef OCCT_DEBUG
    if (trc) cout << " still Sliding" << endl;
#endif
    TopoDS_Face Prof;
    Standard_Boolean ProfileOK;
    ProfileOK = SlidingProfile(Prof,RevolRib,myTol,Concavite,myPln,BndFace,CheckPnt,
			       FirstFace,LastFace,FirstVertex,LastVertex,
			       FirstEdge,LastEdge);

    if (!ProfileOK) {
#ifdef OCCT_DEBUG
      if (trc)
      {
        cout << "Not computable" << endl;
        cout << "Face profile not computable" << endl;
      }
#endif
      myStatusError = BRepFeat_NoFaceProf;
      NotDone();
      return;
    }


// ---Propagation on faces of the initial shape
// to find the faces concerned by the rib
    Standard_Boolean falseside = Standard_True;
    Sliding = Propagate(SliList, Prof, myFirstPnt, myLastPnt, falseside);
// Control if there is everything required to have the material at the proper side
    if(falseside == Standard_False) {
#ifdef OCCT_DEBUG
      cout << " Verify plane and wire orientation" << endl;
#endif
      myStatusError = BRepFeat_FalseSide;
      NotDone();
      return;
    }
  }


// ---Generation of the base profile of the rib

  TopoDS_Wire w;
  BB.MakeWire(w);
  TopoDS_Edge thePreviousEdge;
  TopoDS_Vertex theFV;
  thePreviousEdge.Nullify();

// counter of the number of edges to fill the map
  Standard_Integer counter = 1;

// ---case of sliding 
  if(Sliding && !myListOfEdges.IsEmpty()) {
    BRepTools_WireExplorer EX1(myWire);
    for(; EX1.More(); EX1.Next()) {
      const TopoDS_Edge& E = EX1.Current();
      if(!myLFMap.IsBound(E)) {
        TopTools_ListOfShape theTmpList;
	myLFMap.Bind(E, theTmpList);
      }      
      if(E.IsSame(FirstEdge)) {
	Standard_Real f, l;
	Handle(Geom_Curve) cc = BRep_Tool::Curve(E, f, l);
	gp_Pnt pt;
	if(!FirstEdge.IsSame(LastEdge)) {
	  pt = BRep_Tool::Pnt(TopExp::LastVertex(E,Standard_True)); 
	}
	else {
	  pt = myLastPnt;
	  Standard_Real fpar = IntPar(cc, myFirstPnt);
	  Standard_Real lpar = IntPar(cc, pt);
	  if(fpar > lpar) {
	    cc = cc->Reversed();
	  }
	}
	TopoDS_Edge ee1;
	if(thePreviousEdge.IsNull()) {
	  BRepLib_MakeVertex v1(myFirstPnt);
	  BRepLib_MakeVertex v2(pt);	  
	  BRepLib_MakeEdge e(cc, v1, v2);
	  ee1 = TopoDS::Edge(e.Shape());
	} 
	else {
	  const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge,Standard_True);
	  BRepLib_MakeVertex v2(pt);
	  
	  BRepLib_MakeEdge e(cc, v1, v2);
	  ee1 = TopoDS::Edge(e.Shape());
	}
	TopoDS_Shape aLocalShape = ee1.Oriented(E.Orientation());
	ee1 = TopoDS::Edge(aLocalShape);
//	ee1 = TopoDS::Edge(ee1.Oriented(E.Orientation()));
	if(counter == 1) theFV = TopExp::FirstVertex(ee1,Standard_True);
	myLFMap(E).Append(ee1);
	BB.Add(w, ee1);
	thePreviousEdge = ee1;
	counter++;
	EX1.Next();
	break;
      }
    }

// Case of several edges
    if(!FirstEdge.IsSame(LastEdge)) {
      for(; EX1.More(); EX1.Next()) {
	const TopoDS_Edge& E = EX1.Current();
	if(!myLFMap.IsBound(E)) {
          TopTools_ListOfShape thelist1;
	  myLFMap.Bind(E, thelist1);
	}      
	theList.Append(E);
	Standard_Real f, l;
	if(!E.IsSame(LastEdge)) {
	  Handle(Geom_Curve) ccc = BRep_Tool::Curve(E, f, l);
	  TopoDS_Vertex v1, v2;
	  if(!thePreviousEdge.IsNull()) {
	    v1 = TopExp::LastVertex(thePreviousEdge,Standard_True);
	    v2 = TopExp::LastVertex(E,Standard_True);
	  }
	  else {
//	    v1 = TopExp::LastVertex(E,Standard_True);
	    v1 = TopExp::FirstVertex(E,Standard_True);
	    v2 = TopExp::LastVertex(E,Standard_True);
	  }
	  BRepLib_MakeEdge E1(ccc, v1, v2);
	  TopoDS_Edge E11 = TopoDS::Edge(E1.Shape());
	  TopoDS_Shape aLocalShape = E11.Oriented(E.Orientation());
	  E11 = TopoDS::Edge(aLocalShape);
//	  E11 = TopoDS::Edge(E11.Oriented(E.Orientation()));
	  thePreviousEdge = E11;
	  myLFMap(E).Append(E11);
	  BB.Add(w, E11);
	  if(counter == 1) theFV = TopExp::FirstVertex(E11,Standard_True);
	  counter++;
	}
	else {
	  Handle(Geom_Curve) cc = BRep_Tool::Curve(E, f, l);
	  gp_Pnt pf = BRep_Tool::Pnt(TopExp::FirstVertex(E,Standard_True));
	  gp_Pnt pl = myLastPnt;
	  TopoDS_Edge ee;
	  if(thePreviousEdge.IsNull()) {
	    BRepLib_MakeEdge e(cc, pf , pl); 
	    ee = TopoDS::Edge(e.Shape());
	  }
	  else {
	    const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge,Standard_True);
	    BRepLib_MakeVertex v2(pl);
	    BRepLib_MakeEdge e(cc, v1, v2);
	    ee = TopoDS::Edge(e.Shape());
	  }
	  TopoDS_Shape aLocalShape = ee.Oriented(E.Orientation());
	  ee = TopoDS::Edge(aLocalShape);
//	  ee = TopoDS::Edge(ee.Oriented(E.Orientation()));
	  BB.Add(w, ee);
	  myLFMap(E).Append(ee);
	  if(counter == 1) theFV = TopExp::FirstVertex(ee,Standard_True);
	  thePreviousEdge = ee;
	  counter++;
	  break;
	}
      }
    }
    
    TopTools_ListIteratorOfListOfShape it(myListOfEdges);
    Standard_Boolean FirstOK = Standard_False;
    Standard_Boolean LastOK = Standard_False;
    
    gp_Pnt theLastPnt = myLastPnt;
    Standard_Integer sens = 0;
    TopoDS_Edge theEdge, theLEdge, theFEdge;
    Standard_Integer counter1 = counter;
    TopTools_ListOfShape NewListOfEdges;
    NewListOfEdges.Clear();
    while (!FirstOK) {
      const TopoDS_Edge& edg = TopoDS::Edge(it.Value());
      gp_Pnt fp, lp;
      Standard_Real f, l;
      Handle(Geom_Curve) ccc = BRep_Tool::Curve(edg, f, l);
      Handle(Geom_TrimmedCurve) cc = new Geom_TrimmedCurve(ccc, f, l);
      if ( edg.Orientation() == TopAbs_REVERSED) cc->Reverse();
      
      fp = cc->Value(cc->FirstParameter());
      lp = cc->Value(cc->LastParameter());
      Standard_Real dist = fp.Distance(theLastPnt);
      if(dist <= myTol) {
	sens = 1;
	LastOK = Standard_True;
      }
      else {
	dist = lp.Distance(theLastPnt);
	if(dist <= myTol) {
	  sens = 2;
	  LastOK = Standard_True;
	  cc->Reverse();
	}
      }
      Standard_Integer FirstFlag = 0;
      if(sens==1 && lp.Distance(myFirstPnt) <= myTol) {
	FirstOK = Standard_True;
	FirstFlag = 1;
      }
      else if(sens==2 && fp.Distance(myFirstPnt) <= myTol) {
	FirstOK = Standard_True;
	FirstFlag = 2;
      }
      
      if (LastOK) {
	TopoDS_Edge eeee;
	Standard_Real fpar = cc->FirstParameter();
	Standard_Real lpar = cc->LastParameter();
	if(!FirstOK) {
	  if(thePreviousEdge.IsNull()) {
	    BRepLib_MakeEdge e(cc, fpar, lpar);
	    eeee = TopoDS::Edge(e.Shape());
	  }
	  else {
	    const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge,Standard_True);
	    BRepLib_MakeVertex v2(cc->Value(lpar));
	    BRepLib_MakeEdge e(cc, v1, v2);
	    eeee = TopoDS::Edge(e.Shape());
	  }
	}
	else {
	  if(thePreviousEdge.IsNull()) {
	    BRepLib_MakeVertex v1(cc->Value(fpar)); 
	    BRepLib_MakeEdge e(cc, v1, theFV);
	    eeee = TopoDS::Edge(e.Shape());
	  }
	  else {
	    const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge,Standard_True);
	    BRepLib_MakeEdge e(cc, v1, theFV);
	    eeee = TopoDS::Edge(e.Shape());
	  }
	}

	thePreviousEdge = eeee;
	BB.Add(w, eeee);
	if(counter == 1) theFV = TopExp::FirstVertex(eeee,Standard_True);
	counter1++;
	NewListOfEdges.Append(edg);
	theEdge = eeee;

	if(dist <= myTol) 
	  theFEdge = edg;
	theLastPnt = BRep_Tool::Pnt(TopExp::LastVertex(theEdge,Standard_True));
      }

      if(FirstFlag == 1) {
	theLEdge = edg;
      }
      else if(FirstFlag == 2) {
	theLEdge = theEdge;
      }

      if(LastOK) {
	it.Initialize(myListOfEdges);
	LastOK = Standard_False;
      }
      else if(it.More()) it.Next();
      else {
	Sliding = Standard_False;
	break;
      }	
      sens = 0;
    }
    
    
    TopTools_DataMapOfShapeListOfShape SlidMap;
    SlidMap.Clear();
    
    if(Sliding && counter1 > counter) {
      TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm;
      TopExp_Explorer EX2(w, TopAbs_EDGE);
      Standard_Integer ii = 0;
      for(; EX2.More(); EX2.Next()) {
	const TopoDS_Edge& E = TopoDS::Edge(EX2.Current());
	ii++;	
	if(ii >= counter && ii <= counter1) {
	  it.Initialize(NewListOfEdges);
	  Standard_Integer jj = 0;
	  for(; it.More(); it.Next()) {
	    const TopoDS_Edge& e2 = TopoDS::Edge(it.Value());
	    jj++;
	    if(jj== (ii - counter +1)) {	  
	      itm.Initialize(mySlface);
	      for(; itm.More(); itm.Next()) {
		const TopoDS_Face& fac = TopoDS::Face(itm.Key());
		const TopTools_ListOfShape& ledg = itm.Value();
		TopTools_ListIteratorOfListOfShape itedg(ledg);
		//Standard_Integer iiii = 0;
		for(; itedg.More(); itedg.Next()) {
		  const TopoDS_Edge& e1 = TopoDS::Edge(itedg.Value());
		  if(e1.IsSame(e2)){
		    if(!SlidMap.IsBound(fac)) {
                      TopTools_ListOfShape thelist2;
		      SlidMap.Bind(fac, thelist2);
		    }
		    SlidMap(fac).Append(E);
		  }
		}		
	      }
	    }
	  } 
	}
      }
    }
    
    mySlface.Clear();
    mySlface = SlidMap;
  }

// ---Arguments of LocOpe_LinearForm : arguments of the prism
// sliding
  if(Sliding) {
    TopoDS_Face F;
    BB.MakeFace(F, myPln, myTol);
    w.Closed (BRep_Tool::IsClosed (w));
    BB.Add(F, w);
    mySkface = F;
    myPbase  = mySkface;
    mySUntil.Nullify();
  }


// ---Case without sliding : construction of the face profile  
  if(!Sliding) {
#ifdef OCCT_DEBUG
    if (trc) {
      if (Modify) cout << " Sliding failure" << endl;
      cout << " no Sliding" << endl;
    }
#endif
    TopExp_Explorer explo1(BndFace, TopAbs_WIRE);
    TopoDS_Wire WWW = TopoDS::Wire(explo1.Current());
    BRepTools_WireExplorer explo(WWW);
    BRep_Builder Bu;
    TopoDS_Wire Wiwiwi;
    Bu.MakeWire(Wiwiwi);
    TopoDS_Vertex NewV1, NewV2, LastV, v; 
    NewV1.Nullify(); NewV2.Nullify(); LastV.Nullify();

    for(; explo.More(); explo.Next()) {
      const TopoDS_Edge& e = TopoDS::Edge(explo.Current());
      TopoDS_Vertex v1 = TopExp::FirstVertex(e,Standard_True);
      TopoDS_Vertex v2 = TopExp::LastVertex(e,Standard_True);
            
      Standard_Real f, l;//, t;
      Handle(Geom_Curve) ln = BRep_Tool::Curve(e, f, l);
//      Handle(Geom_Curve) lln = BRep_Tool::Curve(e, f, l);
//      Handle(Geom_Curve) ln;
//      if(e.Orientation() == TopAbs_REVERSED) {
//	ln = Handle(Geom_Curve)::DownCast(lln->Reversed());
//	v = v1; v1 = v2; v2= v;
//	f = IntPar(ln, BRep_Tool::Pnt(v1));
//	l = IntPar(ln, BRep_Tool::Pnt(v2));
//      }
//      else ln = lln;

      Handle(Geom2d_Curve) l2d = GeomAPI::To2d(ln, Plane->Pln());
      Geom2dAPI_InterCurveCurve intcc(l2d, ln2d, Precision::Confusion());
      TopoDS_Vertex VV; VV.Nullify();

      if(intcc.NbPoints() > 0) {
	gp_Pnt2d P = intcc.Point(1);
	gp_Pnt point;
	myPln->D0(P.X(), P.Y(), point);
	Standard_Real par = IntPar(ln, point);
	if(f <= par && l >= par) {
	  Bu.MakeVertex(VV, point, Precision::Confusion());
	}
      }
      
      if(VV.IsNull() && NewV1.IsNull()) continue;

      if(!VV.IsNull() && NewV1.IsNull()) {
	NewV1 = VV;
	LastV = v2;
	BRepLib_MakeEdge ee1(NewV1, LastV);
	Bu.Add(Wiwiwi, ee1); 
	continue;
      } 

      if(VV.IsNull() && !NewV1.IsNull()) {
	BRepLib_MakeEdge ee1(LastV, v2);
	LastV = v2;
	Bu.Add(Wiwiwi, e); 
	continue;
      } 
      
      if(!VV.IsNull() && !NewV1.IsNull()) {
	NewV2 = VV;
	BRepLib_MakeEdge ee1(LastV, NewV2);
	LastV = NewV2;
	Bu.Add(Wiwiwi, ee1); 
	BRepLib_MakeEdge ee2(LastV, NewV1);
	Bu.Add(Wiwiwi, ee2);
	break;
      } 
    }
    Wiwiwi.Closed (BRep_Tool::IsClosed (Wiwiwi));
    
    BRepLib_MakeFace newbndface(myPln->Pln(), Wiwiwi, Standard_True);
    TopoDS_Face NewBndFace = TopoDS::Face(newbndface.Shape());

    BRepTopAdaptor_FClass2d Cl(NewBndFace, Precision::Confusion());
    Standard_Real paru, parv;
    ElSLib::Parameters(myPln->Pln(), CheckPnt, paru, parv);
    gp_Pnt2d checkpnt2d(paru, parv);
    if(Cl.Perform(checkpnt2d, Standard_True) == TopAbs_OUT) {
      BRepAlgoAPI_Cut c(BndFace, NewBndFace);     
      TopExp_Explorer exp(c.Shape(), TopAbs_WIRE);
      const TopoDS_Wire& aCurWire = TopoDS::Wire(exp.Current());
      BRepLib_MakeFace ff(myPln->Pln(), aCurWire, Standard_True);
      NewBndFace = TopoDS::Face(ff.Shape());
    }
   
    
    if(!BRepAlgo::IsValid(NewBndFace)) {
#ifdef OCCT_DEBUG
      cout << "Invalid new bounding face" << endl;
#endif
      myStatusError = BRepFeat_InvShape;
      NotDone();
      return;      
    }
    
    BndFace = NewBndFace;


    TopoDS_Face Prof;
    Standard_Boolean ProfileOK;
    ProfileOK = NoSlidingProfile(Prof,RevolRib,myTol,Concavite,myPln,
				 bnd,BndFace,CheckPnt,
				 FirstFace,LastFace,FirstVertex,LastVertex,
				 FirstEdge,LastEdge,OnFirstFace,OnLastFace);

    if (!ProfileOK) {
#ifdef OCCT_DEBUG
      if (trc)
      {
        cout << "Not computable" << endl;
        cout << " Face profile not computable" << endl;
      }
#endif
      myStatusError = BRepFeat_NoFaceProf;
      NotDone();
      return;
    }


// ---Propagation on the faces of the initial shape
// to find the faces concerned by the rib
    Standard_Boolean falseside = Standard_True;
    Propagate(SliList, Prof, myFirstPnt, myLastPnt, falseside);
// Control if there is everything required to have the material at the proper side
    if(falseside == Standard_False) {
#ifdef OCCT_DEBUG
      cout << " Verify plane and wire orientation" << endl;
#endif
      myStatusError = BRepFeat_FalseSide;
      NotDone();
      return;
    }
    
    mySlface.Clear();

    TopTools_ListIteratorOfListOfShape it;
    it.Initialize(SliList);
    
    TopoDS_Shape comp;
    
    BB.MakeShell(TopoDS::Shell(comp));
    
    for(; it.More(); it.Next()) {
      BB.Add(comp, it.Value());
    }
    comp.Closed (BRep_Tool::IsClosed (comp));
    
    mySUntil = comp;
   
    mySkface = Prof;
    myPbase  = Prof;
  }

  mySliding = Sliding;

  TopExp_Explorer exp;
  for ( exp.Init(mySbase,TopAbs_FACE);exp.More();exp.Next()) {
    TopTools_ListOfShape thelist3;
    myMap.Bind(exp.Current(), thelist3);
    myMap(exp.Current()).Append(exp.Current());
  }
}



//=======================================================================
//function : Add
//purpose  : add elements of gluing
//=======================================================================

void BRepFeat_MakeRevolutionForm::Add(const TopoDS_Edge& E,
			     const TopoDS_Face& F)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeRevolutionForm::Add" << endl;
#endif 
  if(mySlface.IsEmpty()) {
    TopExp_Explorer exp;
    for (exp.Init(mySbase,TopAbs_FACE);exp.More();exp.Next()) {
      if (exp.Current().IsSame(F)) {
	break;
      }
    }
    if (!exp.More()) {
      throw Standard_ConstructionError();
    }
  
    if (!mySlface.IsBound(F)) {
      TopTools_ListOfShape thelist;
      mySlface.Bind(F, thelist);
    }
    TopTools_ListIteratorOfListOfShape itl(mySlface(F));
    for (; itl.More();itl.Next()) {
      if (itl.Value().IsSame(E)) {
	break;
      }
    }
    if (!itl.More()) {
      mySlface(F).Append(E);
    }
  } 
}

  

//=======================================================================
//function : Perform
//purpose  : construction
//=======================================================================

void BRepFeat_MakeRevolutionForm::Perform()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeRevolutionForm::Perform()" << endl;
#endif
  if(mySbase.IsNull() || mySkface.IsNull() || myPbase.IsNull()) {
#ifdef OCCT_DEBUG
    if (trc) cout << " Fields not initialized" << endl;
#endif
    myStatusError = BRepFeat_NotInitialized;
    NotDone();
    return;
  }

  gp_Pnt Pt;

  TopExp_Explorer exx(myPbase, TopAbs_VERTEX);
  for(; exx.More(); exx.Next()) {
    const TopoDS_Vertex& vv = TopoDS::Vertex(exx.Current());
    if(!vv.IsNull()) {
      Pt = BRep_Tool::Pnt(vv);
      break;
    }
  }

  if(myAngle2 != 0) {
    gp_Trsf T;
    T.SetRotation(myAxe, -myAngle2);
    BRepBuilderAPI_Transform trsf(T);
    trsf.Perform(myPbase, Standard_False);
    TopoDS_Face Pbase = TopoDS::Face(trsf.Shape());
    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape iter(myLFMap);
    for(; iter.More(); iter.Next()) {
      const TopoDS_Shape& e1 = iter.Value().First();
      TopExp_Explorer ex1(myPbase, TopAbs_EDGE); 
      TopExp_Explorer ex2(Pbase, TopAbs_EDGE);
      for(; ex1.More(); ex1.Next()) {
	if(ex1.Current().IsSame(e1)) {
	  myLFMap(iter.Key()).Clear();
	  myLFMap(iter.Key()).Append(ex2.Current());
	}
	ex2.Next();	  
      }
    }

    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape iter1(mySlface);
    for(; iter1.More(); iter1.Next()) {
      const TopoDS_Shape& f1 = iter1.Key();
      const TopoDS_Shape& e1 = iter1.Value().First();
      TopExp_Explorer ex1(myPbase, TopAbs_EDGE); 
      TopExp_Explorer ex2(Pbase, TopAbs_EDGE);
      for(; ex1.More(); ex1.Next()) {
	const TopoDS_Shape& E1 = ex1.Current();
	const TopoDS_Shape& E2 = ex2.Current();
	if(E1.IsSame(e1)) {
	  mySlface(f1).Clear();
	  mySlface(f1).Append(E2);
	}
	ex2.Next();	  
      }
    }
    myPbase = Pbase;
    trsf.Perform(mySkface, Standard_False);
// flo : check if it is required to reattributr the field mySkface
//    TopoDS_Face mySkface = TopoDS::Face(trsf.Shape());
    mySkface = TopoDS::Face(trsf.Shape());
  }

  LocOpe_RevolutionForm theForm;
  theForm.Perform(myPbase, myAxe, (myAngle1+myAngle2));
  TopoDS_Shape VraiForm = theForm.Shape();   // uncut  primitive

// management of descendants
  MajMap(myPbase,theForm,myMap,myFShape,myLShape);

  myGluedF.Clear();



  gp_Pln Pln0 = myPln->Pln();
  BRepLib_MakeFace f(Pln0);
  

  gp_Vec vec1 = myHeight1*Normal(f, Pt);
  gp_Vec vec2 = -myHeight2*Normal(f, Pt);

  gp_Pln Pln1 = Pln0.Translated(vec1);
  gp_Pln Pln2 = Pln0.Translated(vec2);
  
  BRepLib_MakeFace ff1(Pln1);
  BRepLib_MakeFace ff2(Pln2);
  TopoDS_Face f1 = TopoDS::Face(ff1.Shape());
  TopoDS_Face f2 = TopoDS::Face(ff2.Shape());
  BRepFeat::FaceUntil(mySbase, f1);
  BRepFeat::FaceUntil(mySbase, f2);

  LocOpe_CSIntersector ASI1(f1);
  LocOpe_CSIntersector ASI2(f2);

  Handle(Geom_Line) normale = new Geom_Line(Pt, vec1);
  TColGeom_SequenceOfCurve scur;
  scur.Append(normale);

  ASI1.Perform(scur);
  ASI2.Perform(scur);

  if(!ASI1.IsDone() || !ASI2.IsDone() ||
     ASI1.NbPoints(1) != 1 || ASI2.NbPoints(1) != 1) {
#ifdef OCCT_DEBUG
    if (trc) cout << " Intersection failure" << endl;
#endif
    myStatusError = BRepFeat_BadIntersect;
    NotDone();
    return;
  }

  TopAbs_Orientation Ori1 = ASI1.Point(1,1).Orientation();
  TopAbs_Orientation Ori2 = TopAbs::Reverse(ASI2.Point(1,1).Orientation());
  TopoDS_Face FF1 = ASI1.Point(1,1).Face();
  TopoDS_Face FF2 = ASI2.Point(1,1).Face();

  TopoDS_Shape Comp;
  BRep_Builder B;
  B.MakeCompound(TopoDS::Compound(Comp));
  TopoDS_Solid S1 = BRepFeat::Tool(f1,FF1,Ori1);
  TopoDS_Solid S2 = BRepFeat::Tool(f2,FF2,Ori2);
  if (!S1.IsNull()) B.Add(Comp,S1);
  if (!S2.IsNull()) B.Add(Comp,S2);

  BRepAlgoAPI_Cut trP(VraiForm,Comp);    
  // coupe de la nervure par deux plans parallels
  TopTools_DataMapOfShapeListOfShape SlidingMap;

// management of descendants

  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape it1;
  it1.Initialize(myMap);
  for(; it1.More(); it1.Next()) {
    const TopoDS_Shape& orig = it1.Key();
    if(it1.Value().IsEmpty()) continue;
    const TopoDS_Shape& sh = it1.Value().First();
    exx.Init(VraiForm, TopAbs_FACE);
    for(; exx.More(); exx.Next()) {
      const TopoDS_Face& fac = TopoDS::Face(exx.Current());
      TopExp_Explorer exx1(fac, TopAbs_WIRE);
      const TopoDS_Wire& thew = TopoDS::Wire(exx1.Current());
      if(thew.IsSame(myFShape)) {
	const TopTools_ListOfShape& desfaces = trP.Modified(f2);
	myMap(myFShape) = desfaces;
	continue;
      }
      else if(thew.IsSame(myLShape)) {
	const TopTools_ListOfShape& desfaces = trP.Modified(f1);
	myMap(myLShape) = desfaces;
	continue;
      }
      if(fac.IsSame(sh)) { 
	if (trP.IsDeleted(fac)) {
	}
	else {
	  const TopTools_ListOfShape& desfaces = trP.Modified(fac);
	  if(!desfaces.IsEmpty()) {
	    myMap(orig).Clear();
	    myMap(orig) = trP.Modified(fac);
	  }
	}
      }
    }
  }

  exx.Init(VraiForm, TopAbs_FACE);
  for(; exx.More(); exx.Next()) {
    const TopoDS_Face& fac = TopoDS::Face(exx.Current());
    TopTools_ListOfShape thelist;
    SlidingMap.Bind(fac, thelist);
    if (trP.IsDeleted(fac)) {
    }
    else {
      const TopTools_ListOfShape& desfaces = trP.Modified(fac);
      if(!desfaces.IsEmpty()) 
	SlidingMap(fac) = desfaces;
      else 
	SlidingMap(fac).Append(fac);
    }
  }


// gestion of faces of sliding
  SetGluedFaces(mySlface, theForm, SlidingMap, myGluedF);

  VraiForm = trP.Shape();   // primitive cut

  if(!myGluedF.IsEmpty()) 
    myPerfSelection = BRepFeat_NoSelection;
  else 
    myPerfSelection = BRepFeat_SelectionSh;

  exx.Init(myPbase, TopAbs_EDGE);
  for(; exx.More(); exx.Next()) {
    const TopoDS_Edge& e = TopoDS::Edge(exx.Current());
    if(!myMap.IsBound(e)) {
#ifdef OCCT_DEBUG
      if (trc) cout << " Sliding face not in Base shape" << endl;
#endif
      myStatusError = BRepFeat_IncSlidFace;
      NotDone();
      return;
    }
  }

  myGShape = VraiForm;

  if(!myGluedF.IsEmpty() && !mySUntil.IsNull()) {
#ifdef OCCT_DEBUG
    if (trc)
    {
      cout << "The case is not computable" << endl;
      cout << " Glued faces not empty and Until shape not null" << endl;
    }
#endif
    myStatusError = BRepFeat_InvShape;
    NotDone();
    return;
  }

  LFPerform();    // topological reconstruction
}


//=======================================================================
//function : Propagate
//purpose  : propagation on the faces of the inital shape, find faces 
// concerned by the rib
//=======================================================================

Standard_Boolean BRepFeat_MakeRevolutionForm::Propagate(TopTools_ListOfShape& SliList,
							const TopoDS_Face& fac,
							const gp_Pnt& Firstpnt, 
							const gp_Pnt& Lastpnt, 
							Standard_Boolean& falseside)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEATRIB();
  if (trc) cout << "BRepFeat_MakeRevolutionForm::Propagate" << endl;
#endif
  gp_Pnt Firstpoint = Firstpnt;
  gp_Pnt Lastpoint = Lastpnt;

  Standard_Boolean result = Standard_True;
  TopoDS_Face CurrentFace, saveFace;
  CurrentFace = TopoDS::Face(SliList.First());
  saveFace = CurrentFace;
  //  BRepBuilderAPI_MakeFace fac(myPln);
  Standard_Boolean LastOK = Standard_False, FirstOK= Standard_False;
  TopoDS_Vertex v1, v2, v3, v4, Vert;
  BRepAlgoAPI_Section sect (fac, CurrentFace, Standard_False);
  sect.Approximation(Standard_True);
  sect.Build();
  TopExp_Explorer Ex;
  TopoDS_Edge e, e1;
  gp_Pnt FP, LP;
  Standard_Integer ii = 0;
  for (Ex.Init(sect.Shape(), TopAbs_EDGE); Ex.More(); Ex.Next()) {
    ii++;
    if(ii==1){
      e = TopoDS::Edge(Ex.Current());
    }    
    else if (ii > 1) {
      e1 = TopoDS::Edge(Ex.Current());
      break;
    }
  }
  if(e.IsNull()) {
    falseside = Standard_False;
    return Standard_False;
  }
  //
  if(!e1.IsNull()) {
    Standard_Real aTolV1, aTolV2;
    myListOfEdges.Clear();
    TopTools_ListOfShape thelist;
    mySlface.Bind(CurrentFace, thelist);
    mySlface(CurrentFace).Append(e1);
    
    myListOfEdges.Append(e1);
    
    v1 = TopExp::FirstVertex(e1,Standard_True);
    v2 = TopExp::LastVertex (e1,Standard_True);

    FP = BRep_Tool::Pnt(v1);
    LP = BRep_Tool::Pnt(v2);

    aTolV1=BRep_Tool::Tolerance(v1);
    aTolV2=BRep_Tool::Tolerance(v2);

    if(FP.Distance(Firstpoint) <= aTolV1 || 
       FP.Distance(Lastpoint)  <= aTolV1) {
      FirstOK = Standard_True;
    }
    if(LP.Distance(Firstpoint)<= aTolV2 || 
       LP.Distance(Lastpoint) <= aTolV2) {
      LastOK = Standard_True;
    }
    
    if(LastOK && FirstOK) {
      return result;
    }
    
    else {
      myListOfEdges.Clear();
    }
  }
  //
  if(!e1.IsNull()) {
    myListOfEdges.Clear();
    TopTools_ListOfShape thelist1;    
    mySlface.Bind(CurrentFace, thelist1);
    mySlface(CurrentFace).Append(e);
    
    myListOfEdges.Append(e);

//    mySlface.Bind(CurrentFace,TopTools_ListOfShape());
    mySlface(CurrentFace).Append(e1);    
//    myListOfEdges.Append(e1);

    v1 = TopExp::FirstVertex(e,Standard_True); 
    v2 = TopExp::LastVertex(e,Standard_True);
    v3 = TopExp::FirstVertex(e1,Standard_True); 
    v4 = TopExp::LastVertex(e1,Standard_True);
    gp_Pnt p1, p2, p3, p4;
    p1 = BRep_Tool::Pnt(v1); FP = p1;
    p2 = BRep_Tool::Pnt(v2);  LP = p2;
    p3 = BRep_Tool::Pnt(v3);
    p4 = BRep_Tool::Pnt(v4);
    if(p1.Distance(Firstpoint) <= BRep_Tool::Tolerance(v1)) {
      if(p3.Distance(Lastpoint) <= BRep_Tool::Tolerance(v3)) {
	FirstOK = Standard_True;
	Lastpoint = p4;
      } 
      else if(p4.Distance(Lastpoint) <= BRep_Tool::Tolerance(v4)) {
	FirstOK = Standard_True;
	Lastpoint = p3;
      } 
      else {
	e1.Nullify();
      }
    }
    else if(p1.Distance(Lastpoint) <= BRep_Tool::Tolerance(v1)) {
      if(p3.Distance(Firstpoint) <= BRep_Tool::Tolerance(v3)) {
	FirstOK = Standard_True;
	Firstpoint = p4;
      } 
      else if(p4.Distance(Firstpoint) <= BRep_Tool::Tolerance(v4)) {
	FirstOK = Standard_True;
	Firstpoint = p3;
      } 
      else {
	e1.Nullify();
      }
    }
    else if(p2.Distance(Firstpoint) <= BRep_Tool::Tolerance(v2)) {
      if(p3.Distance(Lastpoint) <= BRep_Tool::Tolerance(v3)) {
	LastOK = Standard_True;
	Lastpoint = p4;
      } 
      else if(p4.Distance(Lastpoint) <= BRep_Tool::Tolerance(v4)) {
	LastOK = Standard_True;
	Lastpoint = p3;
      } 
      else {
	e1.Nullify();
      }
    }
    else if(p2.Distance(Lastpoint) <= BRep_Tool::Tolerance(v2)) {
      if(p3.Distance(Firstpoint) <= BRep_Tool::Tolerance(v3)) {
	LastOK = Standard_True;
	Firstpoint = p4;
      } 
      else if(p4.Distance(Firstpoint) <= BRep_Tool::Tolerance(v4)) {
	LastOK = Standard_True;
	Firstpoint = p3;
      } 
      else {
	e1.Nullify();
      }
    }
    else {
      e = e1;
      e1.Nullify();
    }
  }
  if(e1.IsNull()) {
    myListOfEdges.Clear();
    TopTools_ListOfShape thelist2;    
    mySlface.Bind(CurrentFace, thelist2);
    mySlface(CurrentFace).Append(e);
    
    myListOfEdges.Append(e);
    
    v1 = TopExp::FirstVertex(e,Standard_True);
    v2 = TopExp::LastVertex(e,Standard_True);

    FP = BRep_Tool::Pnt(v1);
    LP = BRep_Tool::Pnt(v2);
    
    if(FP.Distance(Firstpoint) <= BRep_Tool::Tolerance(v1)
       || FP.Distance(Lastpoint) <= BRep_Tool::Tolerance(v1)) {
      FirstOK = Standard_True;
    }
    if(LP.Distance(Firstpoint) <= BRep_Tool::Tolerance(v2)
       || LP.Distance(Lastpoint) <= BRep_Tool::Tolerance(v2)) {
      LastOK = Standard_True;
    }
    
    if(LastOK && FirstOK) {
      return result;
    }
  }
  
  TopTools_IndexedDataMapOfShapeListOfShape mapedges;
  TopExp::MapShapesAndAncestors(mySbase, TopAbs_EDGE, TopAbs_FACE, mapedges);
  TopExp_Explorer ex;
  TopoDS_Edge FirstEdge;

  TopoDS_Vertex Vprevious;  Vprevious.Nullify();
  TopoDS_Vertex Vpreprevious;  Vpreprevious.Nullify();

  while(!FirstOK) {
   // find edge connected to v1:
    gp_Pnt pt;
    if(!v1.IsNull()) pt= BRep_Tool::Pnt(v1);
    gp_Pnt ptprev;
    if(!Vprevious.IsNull()) ptprev = BRep_Tool::Pnt(Vprevious);
    gp_Pnt ptpreprev;
    if(!Vpreprevious.IsNull()) ptpreprev = BRep_Tool::Pnt(Vpreprevious);
    
    if((!Vprevious.IsNull() && ptprev.Distance(pt) <= myTol) ||
       (!Vpreprevious.IsNull() && ptpreprev.Distance(pt) <= myTol)) {
      falseside = Standard_False;
      return Standard_False;
    }

    for (ex.Init(CurrentFace, TopAbs_EDGE); ex.More(); ex.Next()) {
      const TopoDS_Edge& aCurEdge = TopoDS::Edge(ex.Current());

      BRepExtrema_ExtPC projF(v1, aCurEdge);

      if(projF.IsDone() && projF.NbExt() >=1) {
	Standard_Real dist2min = RealLast();
	Standard_Integer index = 0;
	for (Standard_Integer sol =1 ; sol <= projF.NbExt(); sol++) {
	  if (projF.SquareDistance(sol) <= dist2min) {
	    index = sol;
	    dist2min = projF.SquareDistance(sol);
	  }
	}
	if (index != 0) {
	  if (dist2min <= BRep_Tool::Tolerance(aCurEdge) * BRep_Tool::Tolerance(aCurEdge)) {
	    FirstEdge = aCurEdge;
	    break;
	  }
	}
      }
    }
    
    const TopTools_ListOfShape& L = mapedges.FindFromKey(FirstEdge);
    TopTools_ListIteratorOfListOfShape It(L);

    for (; It.More(); It.Next()) {
      const TopoDS_Face& FF = TopoDS::Face(It.Value());
      if (!FF.IsSame(CurrentFace)) {
	CurrentFace = FF;
	break;
      }
    }

    BRepAlgoAPI_Section sectf (fac, CurrentFace, Standard_False);
    sectf.Approximation(Standard_True);
    sectf.Build();

    TopoDS_Edge edg1;
    for (Ex.Init(sectf.Shape(), TopAbs_EDGE); Ex.More(); Ex.Next()) {
      edg1 = TopoDS::Edge(Ex.Current());
      gp_Pnt ppp1 = BRep_Tool::Pnt(TopExp::FirstVertex(edg1,Standard_True));
      gp_Pnt ppp2 = BRep_Tool::Pnt(TopExp::LastVertex(edg1,Standard_True));
      if(ppp1.Distance(BRep_Tool::Pnt(v1)) <= BRep_Tool::Tolerance(v1) ||
	 ppp2.Distance(BRep_Tool::Pnt(v1)) <= BRep_Tool::Tolerance(v1))
	break;      
    }    

    TopTools_ListOfShape thelist3;
    mySlface.Bind(CurrentFace, thelist3);
    mySlface(CurrentFace).Append(edg1);
    myListOfEdges.Append(edg1);

    if (!edg1.IsNull()) SliList.Prepend(CurrentFace);
    else return Standard_False;

    Vert = TopExp::FirstVertex(edg1,Standard_True);
    gp_Pnt PP = BRep_Tool::Pnt(Vert);
    FP = BRep_Tool::Pnt(v1);
    Standard_Real tol = BRep_Tool::Tolerance(edg1);
    Standard_Real tol1 = BRep_Tool::Tolerance(v1);
    if(tol1 > tol) tol = tol1;
    Standard_Real dist = PP.Distance(FP);
    if (dist <= tol) {
      Vpreprevious = Vprevious;
      Vprevious = v1;
      v1 = TopExp::LastVertex(edg1,Standard_True);
    }
    else {
      Vpreprevious = Vprevious;
      Vprevious = v1;
      v1 = Vert;
    }

    FP = BRep_Tool::Pnt(v1);
    
    if(FP.Distance(Firstpoint) <= BRep_Tool::Tolerance(v1)
       || FP.Distance(Lastpoint) <= BRep_Tool::Tolerance(v1)) {
      FirstOK = Standard_True;
    }
  }

  CurrentFace = saveFace;
  Vprevious.Nullify();
  Vpreprevious.Nullify();

  while(!LastOK) {
    // find edge connected to v2:
    gp_Pnt pt;
    if(!v2.IsNull()) pt= BRep_Tool::Pnt(v2);
    gp_Pnt ptprev;
    if(!Vprevious.IsNull()) ptprev = BRep_Tool::Pnt(Vprevious);
    gp_Pnt ptpreprev;
    if(!Vpreprevious.IsNull()) ptpreprev = BRep_Tool::Pnt(Vpreprevious);
    
    if((!Vprevious.IsNull() && ptprev.Distance(pt) <= myTol) ||
       (!Vpreprevious.IsNull() && ptpreprev.Distance(pt) <= myTol)) {
      falseside = Standard_False;
      return Standard_False;
    }
    
    for (ex.Init(CurrentFace, TopAbs_EDGE); ex.More(); ex.Next()) {
      const TopoDS_Edge& aCurEdge = TopoDS::Edge(ex.Current());
      BRepExtrema_ExtPC projF(v2, aCurEdge);

      if(projF.IsDone() && projF.NbExt() >=1) {
	Standard_Real dist2min = RealLast();
	Standard_Integer index = 0;
	for (Standard_Integer sol =1 ; sol <= projF.NbExt(); sol++) {
	  if (projF.SquareDistance(sol) <= dist2min) {
	    index = sol;
	    dist2min = projF.SquareDistance(sol);
	  }
	}
	if (index != 0) {
	  if (dist2min <= BRep_Tool::Tolerance(aCurEdge) * BRep_Tool::Tolerance(aCurEdge)) {
	    FirstEdge = aCurEdge;
	    break;
	  }
	}
      }
    }
    
    const TopTools_ListOfShape& L = mapedges.FindFromKey(FirstEdge);
    TopTools_ListIteratorOfListOfShape It(L);

    for (; It.More(); It.Next()) {
      const TopoDS_Face& FF = TopoDS::Face(It.Value());
      if (!FF.IsSame(CurrentFace)) {
	CurrentFace = FF;
	break;
      }
    }

    ii = 0;
 
    BRepAlgoAPI_Section sectf (fac, CurrentFace, Standard_False);
    sectf.Approximation(Standard_True);
    sectf.Build();

    TopoDS_Edge edg2;
    for (Ex.Init(sectf.Shape(), TopAbs_EDGE); Ex.More(); Ex.Next()) {      
      edg2 = TopoDS::Edge(Ex.Current());
      gp_Pnt ppp1 = BRep_Tool::Pnt(TopExp::FirstVertex(edg2,Standard_True));
      gp_Pnt ppp2 = BRep_Tool::Pnt(TopExp::LastVertex(edg2,Standard_True));
      if(ppp1.Distance(BRep_Tool::Pnt(v2)) <= BRep_Tool::Tolerance(v2) ||
	 ppp2.Distance(BRep_Tool::Pnt(v2)) <= BRep_Tool::Tolerance(v2))
	break;
    }    
    TopTools_ListOfShape thelist4;
    mySlface.Bind(CurrentFace, thelist4);
    mySlface(CurrentFace).Append(edg2);
    myListOfEdges.Append(edg2);

    if (!edg2.IsNull()) SliList.Append(CurrentFace);
    else return Standard_False;

    Vert = TopExp::FirstVertex(edg2,Standard_True);
    gp_Pnt PP = BRep_Tool::Pnt(Vert);
    FP = BRep_Tool::Pnt(v2);
    if (PP.Distance(FP)<= BRep_Tool::Tolerance(v2)) {
      Vpreprevious = Vprevious;
      Vprevious = v2;
      v2 = TopExp::LastVertex(edg2,Standard_True);
    }
    else {
      v2 = Vert;
    }
    FP = BRep_Tool::Pnt(v2);

    
    if(FP.Distance(Firstpoint) <= BRep_Tool::Tolerance(v2)
       || FP.Distance(Lastpoint) <= BRep_Tool::Tolerance(v2)) {
      LastOK = Standard_True;
    }
  }
  if(!e1.IsNull())     myListOfEdges.Append(e1);
  return result;
  
}


//=======================================================================
//function : MajMap
//purpose  : management of descendants
//=======================================================================

static void MajMap(const TopoDS_Shape& theB,
		   const LocOpe_RevolutionForm& theP,
		   TopTools_DataMapOfShapeListOfShape& theMap, // myMap
		   TopoDS_Shape& theFShape,  // myFShape
		   TopoDS_Shape& theLShape) // myLShape
{
  TopExp_Explorer exp(theP.FirstShape(),TopAbs_WIRE);
  if (exp.More()) {
    theFShape = exp.Current();
    TopTools_ListOfShape thelist;
    theMap.Bind(theFShape, thelist);
    for (exp.Init(theP.FirstShape(),TopAbs_FACE);exp.More();exp.Next()) {
      const TopoDS_Shape& sh = exp.Current();
      theMap(theFShape).Append(sh);
    }
  }
  
  exp.Init(theP.LastShape(),TopAbs_WIRE);
  if (exp.More()) {
    theLShape = exp.Current();
    TopTools_ListOfShape thelist1;
    theMap.Bind(theLShape, thelist1);
    for (exp.Init(theP.LastShape(),TopAbs_FACE);exp.More();exp.Next()) {
      const TopoDS_Shape& sh = exp.Current();
      theMap(theLShape).Append(sh);
    }
  }

  for (exp.Init(theB,TopAbs_EDGE); exp.More(); exp.Next()) {
    if (!theMap.IsBound(exp.Current())) {
      TopTools_ListOfShape thelist2;
      theMap.Bind(exp.Current(), thelist2);
      theMap(exp.Current()) = theP.Shapes(exp.Current());
    }
  }
}


 //=======================================================================
//function : SetGluedFaces
//purpose  : managemnet of sliding faces
//=======================================================================

static void SetGluedFaces(const TopTools_DataMapOfShapeListOfShape& theSlmap,
			  LocOpe_RevolutionForm& thePrism,
			  const TopTools_DataMapOfShapeListOfShape& SlidingMap,
			  TopTools_DataMapOfShapeShape& theMap)
{
  // Slidings
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(theSlmap);
  if(!theSlmap.IsEmpty()) {
    for (; itm.More(); itm.Next()) {
      const TopoDS_Face& fac = TopoDS::Face(itm.Key());
      const TopTools_ListOfShape& ledg = itm.Value();
      TopTools_ListIteratorOfListOfShape it;
      for (it.Initialize(ledg); it.More(); it.Next()) {
	const TopTools_ListOfShape& gfac = thePrism.Shapes(it.Value());
	if (gfac.Extent() != 1) {
#ifdef OCCT_DEBUG
	  cout << "Pb SetGluedFace" << endl;
#endif
	}
	TopTools_DataMapIteratorOfDataMapOfShapeListOfShape iterm(SlidingMap);
	for(; iterm.More(); iterm.Next()) {
	  const TopoDS_Face& ff = TopoDS::Face(iterm.Key());
	  const TopTools_ListOfShape& lfaces = iterm.Value();
	  if(lfaces.IsEmpty()) continue;
	  const TopoDS_Face& fff = TopoDS::Face(lfaces.First());
	  if(gfac.First().IsSame(ff)) theMap.Bind(fff,fac);
	}
      }
    }
  }
}

