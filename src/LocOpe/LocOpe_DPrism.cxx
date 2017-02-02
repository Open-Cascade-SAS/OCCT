// Created on: 1996-09-04
// Created by: Olga PILLOT
// Copyright (c) 1996-1999 Matra Datavision
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
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepTools.hxx>
#include <BSplCLib.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <LocOpe.hxx>
#include <LocOpe_BuildShape.hxx>
#include <LocOpe_DPrism.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean BRepFeat_GettraceFEAT();
#endif

#define NECHANT 7 // voir BRepFeat.cxx

//=======================================================================
//function : LocOpe_DPrism
//purpose  : 
//=======================================================================

LocOpe_DPrism::LocOpe_DPrism(const TopoDS_Face&  Spine,
			     const Standard_Real Height1,
			     const Standard_Real Height2,
			     const Standard_Real Angle):
  mySpine(Spine)
{
  Standard_Integer i ;

  myHeight = Height1 + Height2;
  Standard_Real y =  Height1*sin(Angle);
  Standard_Real z =  Height1*cos(Angle);

  TopoDS_Vertex Vert2  = BRepLib_MakeVertex(gp_Pnt(0,y,z));

  Standard_Real y1 =  -Height2*sin(Angle);
  Standard_Real z1 =  -Height2*cos(Angle);

  TopoDS_Vertex Vert1  = BRepLib_MakeVertex(gp_Pnt(0,y1,z1));  

  myProfile2 = BRepLib_MakeEdge(Vert1, Vert2);
  
  Standard_Real Umin,Umax,Vmin,Vmax;
  Umax = 0.;
  Umin = 0.;
  Vmin = 0.;
  Vmax = 0.;

  BRepTools::UVBounds(Spine,Umin,Umax,Vmin,Vmax);
  Standard_Real Deltay = Max(Umax-Umin,Vmax-Vmin) + Abs(y);
  Deltay*=2;
  
  TopoDS_Vertex Vert3  = BRepLib_MakeVertex(gp_Pnt(0, y + Deltay, z));
  myProfile3 = BRepLib_MakeEdge(Vert2, Vert3);

  Umax = 0.;
  Umin = 0.;
  Vmin = 0.;
  Vmax = 0.;

  BRepTools::UVBounds(Spine,Umin,Umax,Vmin,Vmax);
  Standard_Real Deltay1 = Max(Umax-Umin,Vmax-Vmin) + Abs(y1);
  Deltay1*=2;

  TopoDS_Vertex Vert4  = BRepLib_MakeVertex(gp_Pnt(0, y1+Deltay1, z1));
  myProfile1 = BRepLib_MakeEdge(Vert4, Vert1);

  myProfile  = BRepLib_MakeWire(myProfile1,myProfile2,myProfile3);

  myDPrism.Perform(mySpine,myProfile,gp::XOY()); 


  if (myDPrism.IsDone()) {
    LocOpe_BuildShape BS;
    BRep_Builder B;
    TopoDS_Compound C;
    TopoDS_Compound D;
    TopTools_ListOfShape lfaces,lcomplete;
    
    
    B.MakeCompound(C);
    TopTools_ListIteratorOfListOfShape it;
    TopExp_Explorer ExpS(mySpine,TopAbs_EDGE);
    TopTools_MapOfShape View;
    for (; ExpS.More(); ExpS.Next()) {
      const TopoDS_Shape& ES = ExpS.Current();
      const TopTools_ListOfShape& lffs = 
	myDPrism.GeneratedShapes(ES, myProfile1);
      for (it.Initialize(lffs); it.More(); it.Next()) {
	if (View.Add(it.Value()))
	  B.Add(C,it.Value());
      }
    }
    
    TopTools_IndexedDataMapOfShapeListOfShape theMapEF;
    TopExp::MapShapesAndAncestors(C,TopAbs_EDGE,TopAbs_FACE,theMapEF);
    View.Clear();
    
    for ( i = 1; i<=theMapEF.Extent(); i++) {
      if (theMapEF(i).Extent() == 1) {
	const TopoDS_Edge& edg = TopoDS::Edge(theMapEF.FindKey(i));
	const TopoDS_Face& fac = TopoDS::Face(theMapEF(i).First());
	if (View.Add(fac)) {
	  TopoDS_Shape aLocalShape = fac.EmptyCopied();
	  TopoDS_Face newFace(TopoDS::Face(aLocalShape));
//	  TopoDS_Face newFace(TopoDS::Face(fac.EmptyCopied()));
	  TopExp_Explorer exp;
	  for (exp.Init(fac.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
	       exp.More(); exp.Next()) {
//	    for (TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE); 
            TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE) ;
	    for ( ; exp2.More(); exp2.Next()) {
	      if (exp2.Current().IsSame(edg)) {
		B.Add(newFace,exp.Current());
		lfaces.Append(newFace);
		lcomplete.Append(newFace);
		break;
	      }
	    }
	    if (exp2.More()) {
	      break;
	    }
	  }
	}
      }
    }
    
    BS.Perform(lfaces);
    myFirstShape = BS.Shape();


    B.MakeCompound(D);
    
    ExpS.ReInit();
    View.Clear();
    
    for (; ExpS.More(); ExpS.Next()) {
      const TopoDS_Shape& ES = ExpS.Current();
      const TopTools_ListOfShape& lfls = 
	myDPrism.GeneratedShapes(ES, myProfile3);
      for (it.Initialize(lfls); it.More(); it.Next()) {
	if (View.Add(it.Value()))
	  B.Add(D,it.Value());
      }
    }
    
    lfaces.Clear();
    theMapEF.Clear();
    TopExp::MapShapesAndAncestors(D,TopAbs_EDGE,TopAbs_FACE,theMapEF);
    View.Clear();
    
    for (i = 1; i<=theMapEF.Extent(); i++) {
      if (theMapEF(i).Extent() == 1) {
	const TopoDS_Edge& edg = TopoDS::Edge(theMapEF.FindKey(i));
	const TopoDS_Face& fac = TopoDS::Face(theMapEF(i).First());
	if (View.Add(fac)) {
	  TopoDS_Shape aLocalShape = fac.EmptyCopied();
	  TopoDS_Face newFace(TopoDS::Face(aLocalShape));
//	  TopoDS_Face newFace(TopoDS::Face(fac.EmptyCopied()));
	  TopExp_Explorer exp;
	  for (exp.Init(fac.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
	       exp.More(); exp.Next()) {
//	    for (TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE); 
            TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE) ;
	    for ( ; exp2.More(); exp2.Next()) {
	      if (exp2.Current().IsSame(edg)) {
		B.Add(newFace,exp.Current());
		lfaces.Append(newFace);
		lcomplete.Append(newFace);
		break;
	      }
	    }
	    if (exp2.More()) {
	      break;
	    }
	  }
	}
      }
    }
    BS.Perform(lfaces);
    myLastShape = BS.Shape();


    View.Clear();

    for (ExpS.ReInit(); ExpS.More(); ExpS.Next()) {
      const TopoDS_Shape& ES = ExpS.Current();
      const TopTools_ListOfShape& lffs =
	myDPrism.GeneratedShapes(ES, myProfile2);

      for (it.Initialize(lffs); it.More(); it.Next()) {
	if (it.Value().ShapeType() == TopAbs_EDGE) {
	  break;
	}
      }
      if (it.More()) {
	TopoDS_Shape RemovedEdge = it.Value();
	TopoDS_Face NewFace;
	TopoDS_Wire NewWire;
	B.MakeWire(NewWire);
	TopAbs_Orientation Orref = TopAbs_FORWARD;
	TopExp_Explorer exp;
	for (it.Initialize(lffs); it.More(); it.Next()) {
	  if (it.Value().ShapeType() == TopAbs_FACE) {
	    exp.Init(it.Value().Oriented(TopAbs_FORWARD),TopAbs_WIRE);
	    const TopoDS_Shape theWire = exp.Current();
	    if (NewFace.IsNull()) {
	      Handle(Geom_Surface) S = 
		BRep_Tool::Surface(TopoDS::Face(it.Value()));
	      if (S->DynamicType() == 
		  STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
		S = Handle(Geom_RectangularTrimmedSurface)::
		  DownCast(S)->BasisSurface();
	      }
	      if (S->DynamicType() != STANDARD_TYPE(Geom_Plane)) {
		break;
	      }

	      B.MakeFace(NewFace,S,BRep_Tool::
			 Tolerance(TopoDS::Face(it.Value())));
	      NewFace.Orientation(TopAbs_FORWARD);
	      Orref = theWire.Orientation();
	      for (exp.Init(theWire.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
		   exp.More(); exp.Next()) {
		if (!exp.Current().IsSame(RemovedEdge)) {
		  B.Add(NewWire,exp.Current());
		}
	      }
	    }
	    else {
	      for (exp.Init(theWire.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
		   exp.More(); exp.Next()) {
		if (!exp.Current().IsSame(RemovedEdge)) {
		  if (theWire.Orientation() != Orref) { // Les 2 faces planes ont des normales opposees
		    B.Add(NewWire,exp.Current());
		  }
		  else {
		    B.Add(NewWire,exp.Current().Reversed());
		  }
		}
	      }
	    }
	  }
	}
	if (!NewFace.IsNull()) {
	  B.Add(NewFace,NewWire.Oriented(Orref));
	  lcomplete.Append(NewFace);
          TopTools_ListOfShape thelist;
	  myMap.Bind(ES, thelist);
	  myMap(ES).Append(NewFace);
	}
	else {
	  for (it.Initialize(lffs); it.More(); it.Next()) {
	    if (View.Add(it.Value()) && 
		it.Value().ShapeType() == TopAbs_FACE) {
	      lcomplete.Append(it.Value());
	      
	    }
	  }
	}
      }
      else {
	for (it.Initialize(lffs); it.More(); it.Next()) {
	  if (View.Add(it.Value()) && it.Value().ShapeType() 
	      == TopAbs_FACE) {
	    lcomplete.Append(it.Value());
	  }
	}
      }

      TopExp_Explorer ExpS2;
      for (ExpS2.Init(ES,TopAbs_VERTEX);ExpS2.More(); ExpS2.Next()) {
	const TopTools_ListOfShape& ls2 = 
	  myDPrism.GeneratedShapes(ExpS2.Current(), myProfile2);
	for (it.Initialize(ls2); it.More(); it.Next()) {
	  if (View.Add(it.Value()) && it.Value().ShapeType() 
	      == TopAbs_FACE) {
	    lcomplete.Append(it.Value());
	  }
	}
      }
    }

    BS.Perform(lcomplete);
    myRes = BS.Shape();
    BRepLib::UpdateTolerances(myRes);
  }
}



//=======================================================================
//function : LocOpe_DPrism
//purpose  : 
//=======================================================================

LocOpe_DPrism::LocOpe_DPrism(const TopoDS_Face&   Spine,
			     const Standard_Real Height,
			     const Standard_Real Angle):
   mySpine(Spine)
{
  Standard_Integer i ;
  myHeight = Height;
  Standard_Real y =  Height*sin(Angle);
  Standard_Real z =  Height*cos(Angle);

  TopoDS_Vertex Vert1  = BRepLib_MakeVertex(gp_Pnt(0, 0, 0));
  TopoDS_Vertex Vert2  = BRepLib_MakeVertex(gp_Pnt(0,y,z));
  myProfile2 = BRepLib_MakeEdge(Vert1, Vert2);
  
  Standard_Real Umin,Umax,Vmin,Vmax;
  BRepTools::UVBounds(Spine,Umin,Umax,Vmin,Vmax);
  Standard_Real Deltay = Max(Umax-Umin,Vmax-Vmin) + Abs(y);
  Deltay*=2;
  
  TopoDS_Vertex Vert3  = BRepLib_MakeVertex(gp_Pnt(0, y + Deltay,z));
  myProfile3 = BRepLib_MakeEdge(Vert2, Vert3);
  
  TopoDS_Vertex Vert4  = BRepLib_MakeVertex(gp_Pnt(0, Deltay,0));
  myProfile1 = BRepLib_MakeEdge(Vert4, Vert1);
  
  myProfile = BRepLib_MakeWire(myProfile1,myProfile2,myProfile3);
  myDPrism.Perform(mySpine,myProfile,gp::XOY()); 


  if (myDPrism.IsDone()) {
    LocOpe_BuildShape BS;
    BRep_Builder B;
    TopoDS_Compound C;
    TopoDS_Compound D;
    TopTools_ListOfShape lfaces,lcomplete;
    
    
    B.MakeCompound(C);
    TopTools_ListIteratorOfListOfShape it;
    TopExp_Explorer ExpS(mySpine,TopAbs_EDGE);
    TopTools_MapOfShape View;
    for (; ExpS.More(); ExpS.Next()) {
      const TopoDS_Shape& ES = ExpS.Current();
      const TopTools_ListOfShape& lffs = 
	myDPrism.GeneratedShapes(ES, myProfile1);
      for (it.Initialize(lffs); it.More(); it.Next()) {
	if (View.Add(it.Value()))
	  B.Add(C,it.Value());
      }
    }
    
    TopTools_IndexedDataMapOfShapeListOfShape theMapEF;
    TopExp::MapShapesAndAncestors(C,TopAbs_EDGE,TopAbs_FACE,theMapEF);
    View.Clear();
    
    for ( i = 1; i<=theMapEF.Extent(); i++) {
      if (theMapEF(i).Extent() == 1) {
	const TopoDS_Edge& edg = TopoDS::Edge(theMapEF.FindKey(i));
	const TopoDS_Face& fac = TopoDS::Face(theMapEF(i).First());
	if (View.Add(fac)) {
	  TopoDS_Shape aLocalShape = fac.EmptyCopied();
	  TopoDS_Face newFace(TopoDS::Face(aLocalShape));
//	  TopoDS_Face newFace(TopoDS::Face(fac.EmptyCopied()));
	  TopExp_Explorer exp;
	  for (exp.Init(fac.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
	       exp.More(); exp.Next()) {
//	    for (TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE); 
            TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE) ;
	    for ( ; exp2.More(); exp2.Next()) {
	      if (exp2.Current().IsSame(edg)) {
		B.Add(newFace,exp.Current());
		lfaces.Append(newFace);
		lcomplete.Append(newFace);
		break;
	      }
	    }
	    if (exp2.More()) {
	      break;
	    }
	  }
	}
      }
    }
    
    BS.Perform(lfaces);
    myFirstShape = BS.Shape();


    B.MakeCompound(D);
    
    ExpS.ReInit();
    View.Clear();
    
    for (; ExpS.More(); ExpS.Next()) {
      const TopoDS_Shape& ES = ExpS.Current();
      const TopTools_ListOfShape& lfls = 
	myDPrism.GeneratedShapes(ES, myProfile3);
      for (it.Initialize(lfls); it.More(); it.Next()) {
	if (View.Add(it.Value()))
	  B.Add(D,it.Value());
      }
    }
    
    lfaces.Clear();
    theMapEF.Clear();
    TopExp::MapShapesAndAncestors(D,TopAbs_EDGE,TopAbs_FACE,theMapEF);
    View.Clear();
    
    for (i = 1; i<=theMapEF.Extent(); i++) {
      if (theMapEF(i).Extent() == 1) {
	const TopoDS_Edge& edg = TopoDS::Edge(theMapEF.FindKey(i));
	const TopoDS_Face& fac = TopoDS::Face(theMapEF(i).First());
	if (View.Add(fac)) {
	  TopoDS_Shape aLocalShape = fac.EmptyCopied();
	  TopoDS_Face newFace(TopoDS::Face(aLocalShape));
//	  TopoDS_Face newFace(TopoDS::Face(fac.EmptyCopied()));
	  TopExp_Explorer exp;
	  for (exp.Init(fac.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
	       exp.More(); exp.Next()) {
//	    for (TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE); 
            TopExp_Explorer exp2(exp.Current(),TopAbs_EDGE) ;
	    for ( ; exp2.More(); exp2.Next()) {
	      if (exp2.Current().IsSame(edg)) {
		B.Add(newFace,exp.Current());
		lfaces.Append(newFace);
		lcomplete.Append(newFace);
		break;
	      }
	    }
	    if (exp2.More()) {
	      break;
	    }
	  }
	}
      }
    }
    BS.Perform(lfaces);
    myLastShape = BS.Shape();


    View.Clear();
    for (ExpS.ReInit(); ExpS.More(); ExpS.Next()) {
      const TopoDS_Shape& ES = ExpS.Current();
      const TopTools_ListOfShape& ls = 
	myDPrism.GeneratedShapes(ES, myProfile2);
      for (it.Initialize(ls); it.More(); it.Next()) {
	if (View.Add(it.Value())) {
	  lcomplete.Append(it.Value());
	}
      }
      TopExp_Explorer ExpS2;
      for (ExpS2.Init(ES,TopAbs_VERTEX);ExpS2.More(); ExpS2.Next()) {
	const TopTools_ListOfShape& ls2 = 
	  myDPrism.GeneratedShapes(ExpS2.Current(), myProfile2);
	for (it.Initialize(ls2); it.More(); it.Next()) {
	  if (View.Add(it.Value()) && it.Value().
	      ShapeType() == TopAbs_FACE) {
	    lcomplete.Append(it.Value());
	  }
	}
      }
    }

    BS.Perform(lcomplete);
    myRes = BS.Shape();
    BRepLib::UpdateTolerances(myRes);
  }

}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe_DPrism::IsDone() const
{
  return myDPrism.IsDone();
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& LocOpe_DPrism::Shape () const
{
  if (!myDPrism.IsDone()) {
    throw StdFail_NotDone();
  }
  return myRes;
}


//=======================================================================
//function : Spine
//purpose  : 
//=======================================================================

const TopoDS_Shape& LocOpe_DPrism::Spine () const
{
  return mySpine;
}

//=======================================================================
//function : Profile
//purpose  : 
//=======================================================================

const TopoDS_Shape& LocOpe_DPrism::Profile () const
{
  return myProfile;
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& LocOpe_DPrism::FirstShape () const
{
  return myFirstShape;
}

//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& LocOpe_DPrism::LastShape () const
{
  return myLastShape;
}


//=======================================================================
//function : Shapes
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& 
 LocOpe_DPrism::Shapes (const TopoDS_Shape& S)const
{
  if (!myDPrism.IsDone()) {
    throw StdFail_NotDone();
  }
  if (myMap.IsBound(S)) {
    return myMap(S);
  }
  else {
    return myDPrism.GeneratedShapes(S,myProfile2);
  }
}


//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================

void LocOpe_DPrism::Curves(TColGeom_SequenceOfCurve& Scurves) const
{
  // Retrieves dy and dz with myProfile2
  TopoDS_Vertex V1,V2;
  TopExp::Vertices(myProfile2,V1,V2);
  gp_Pnt P1 = BRep_Tool::Pnt(V1);
  gp_Pnt P2 = BRep_Tool::Pnt(V2);
  Standard_Real dy = P2.Y() - P1.Y(); 
  Standard_Real dz = P2.Z() - P1.Z(); 
  Scurves.Clear();
  Handle(Geom_Surface) S = BRep_Tool::Surface(mySpine);
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    S = Handle(Geom_RectangularTrimmedSurface)::
      DownCast(S)->BasisSurface();
  }

  Handle(Geom_Plane) PP = Handle(Geom_Plane)::DownCast(S);
  if (PP.IsNull()) {
    throw Standard_ConstructionError();
  }

  gp_Pln P = PP->Pln();
  gp_Dir Normale(P.Axis().Direction());
  if (!P.Direct()) {
    Normale.Reverse();
  }

  TopTools_MapOfShape theMap;
  TopExp_Explorer exp(mySpine.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
  TopLoc_Location Loc;
  Handle(Geom_Curve) C;
  Standard_Real f,l,prm;
  Standard_Integer i;

  for (; exp.More(); exp.Next()) {
    const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
    if (!theMap.Add(edg)) {
      continue;
    }
    if (!BRep_Tool::Degenerated(edg)) {
      C = BRep_Tool::Curve(edg,Loc,f,l);
      C = Handle(Geom_Curve)::
	DownCast(C->Transformed(Loc.Transformation()));
      Standard_Real u1 = -2*Abs(myHeight);
      Standard_Real u2 = 2*Abs(myHeight);
	
      for (i=0; i<=NECHANT; i++) {
	prm = ((NECHANT-i)*f+i*l)/NECHANT;
	gp_Pnt pt;
	gp_Vec d1;
	C->D1(prm,pt,d1);
	if (exp.Current().Orientation() == TopAbs_REVERSED) {
	  d1.Reverse();
	}
	d1.Normalize();
	gp_Dir locy = Normale.Crossed(d1);
	gp_Vec ldir = dy*locy.XYZ() + dz*Normale.XYZ();
	gp_Lin lin(pt, ldir);
	Handle(Geom_Line) Lin = new Geom_Line(lin);
	Handle(Geom_TrimmedCurve) trlin = 
	  new Geom_TrimmedCurve(Lin, u1, u2, Standard_True);
	Scurves.Append(trlin);
      }
    }
  }
}


//=======================================================================
//function : BarycCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve) LocOpe_DPrism::BarycCurve() const
{
  TopoDS_Vertex V1,V2;
  TopExp::Vertices(myProfile2,V1,V2);
  gp_Pnt P1 = BRep_Tool::Pnt(V1);
  gp_Pnt P2 = BRep_Tool::Pnt(V2);
  Standard_Real dz = P2.Z() - P1.Z(); 

  Handle(Geom_Surface) S = BRep_Tool::Surface(mySpine);
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    S = Handle(Geom_RectangularTrimmedSurface)::
      DownCast(S)->BasisSurface();
  }

  Handle(Geom_Plane) PP = Handle(Geom_Plane)::DownCast(S);
  if (PP.IsNull()) {
    throw Standard_ConstructionError();
  }

  gp_Pln P = PP->Pln();
  gp_Dir Normale(P.Axis().Direction());
  if (!P.Direct()) {
    Normale.Reverse();
  }
  if (mySpine.Orientation() == TopAbs_REVERSED) {
#ifdef OCCT_DEBUG
    Standard_Boolean trc = BRepFeat_GettraceFEAT();
    if (trc) {
      cout << "LocOpe_DPrism::BarycCurve()" << endl;
      cout << " Reversed Spine orientation" << endl;
    }
#endif
//    Normale.Reverse();  //cts20871
  }
  gp_Vec Vec = dz*Normale.XYZ();

  gp_Pnt bar(0., 0., 0.);
  TColgp_SequenceOfPnt spt;
  if(!myFirstShape.IsNull()) {
    LocOpe::SampleEdges(myFirstShape,spt);
  }
  else {
    LocOpe::SampleEdges(mySpine,spt);
  }
  for (Standard_Integer jj=1;jj<=spt.Length(); jj++) {
    const gp_Pnt& pvt = spt(jj);
    bar.ChangeCoord() += pvt.XYZ();
  }
  bar.ChangeCoord().Divide(spt.Length());
  gp_Ax1 newAx(bar,Vec);
  Handle(Geom_Line) theLin = new Geom_Line(newAx);
  return theLin;
}
