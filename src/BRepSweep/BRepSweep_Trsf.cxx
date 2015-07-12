// Created on: 1993-06-10
// Created by: Laurent BOURESCHE
// Copyright (c) 1993-1999 Matra Datavision
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
#include <BRepAdaptor_Curve.hxx>
#include <BRepLProp.hxx>
#include <BRepSweep_Iterator.hxx>
#include <BRepSweep_Trsf.hxx>
#include <GeomAbs_Shape.hxx>
#include <Precision.hxx>
#include <Sweep_NumShape.hxx>
#include <Sweep_NumShapeIterator.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

BRepSweep_Trsf::BRepSweep_Trsf(const BRep_Builder& aBuilder,
			       const TopoDS_Shape& aGenShape,
			       const Sweep_NumShape& aDirWire,
			       const TopLoc_Location& aLocation,
			       const Standard_Boolean aCopy):
       BRepSweep_NumLinearRegularSweep(aBuilder,aGenShape,aDirWire),
       myLocation(aLocation),
       myCopy(aCopy)
{
}

void BRepSweep_Trsf::Init()
{
  if(!myCopy){
    Sweep_NumShapeIterator It;
    for(It.Init(myDirWire);It.More();It.Next()){
      Process(myGenShape,It.Value());
    }
  }
}

Standard_Boolean BRepSweep_Trsf::Process(const TopoDS_Shape& aGenS,
					 const Sweep_NumShape& aDirV)
{
  Standard_Boolean dotrsf = (aDirV.Index()==2 && !myDirWire.Closed());
  Standard_Integer iD = myDirShapeTool.Index(aDirV);
  Standard_Integer iG = myGenShapeTool.Index(aGenS);
  if(IsInvariant(aGenS)){ 
    myShapes(iG,iD) = aGenS;
    myBuiltShapes(iG,iD) = Standard_True;
    return Standard_True;
  }
  else{
    BRepSweep_Iterator Jt;
    Standard_Boolean touch = Standard_False;
    for(Jt.Init(aGenS);Jt.More();Jt.Next()){
      if(Process(Jt.Value(),aDirV)) touch = Standard_True;
    }
    if(!touch || !dotrsf){
      TopoDS_Shape newShape = aGenS;
      if(dotrsf) newShape.Move(myLocation);
      myShapes(iG,iD) = newShape;
      myBuiltShapes(iG,iD) = Standard_True;
    }	
    return touch;
  }
}

//=======================================================================
//function : SetContinuity
//purpose  : 
//=======================================================================

void BRepSweep_Trsf::SetContinuity(const TopoDS_Shape& aGenS, 
				   const Sweep_NumShape& aDirS)
{
  Standard_Real tl = Precision::Confusion(), tol3d;
  //angular etant un peu severe pour les contours sketches.
  Standard_Real ta = 0.00175;//environ 0.1 degre
  GeomAbs_Shape cont;
  BRep_Builder B = myBuilder.Builder();
  if(aGenS.ShapeType() == TopAbs_EDGE){
    if (HasShape(aGenS,aDirS)){
      TopoDS_Edge E = TopoDS::Edge(aGenS);
      BRepAdaptor_Curve e;
      Standard_Real ud,uf;
      TopoDS_Vertex d,f;
      TopExp::Vertices(E,d,f);
      if(d.IsSame(f)){
//	tol3d = Max(tl,BRep_Tool::Tolerance(d));
	tol3d = Max(tl,2.*BRep_Tool::Tolerance(d));//IFV 24.05.00 buc60684
	e.Initialize(E);
	ud = BRep_Tool::Parameter(d,TopoDS::Edge(aGenS));
	uf = BRep_Tool::Parameter(f,TopoDS::Edge(aGenS));
	cont = BRepLProp::Continuity(e,e,ud,uf,tol3d,ta);
	if(cont >= 1){
	  TopoDS_Shape s_wnt = Shape(d,aDirS);
	  TopoDS_Edge e_wnt = TopoDS::Edge(s_wnt);
	  s_wnt = Shape(aGenS,aDirS);
	  TopoDS_Face f_wnt = TopoDS::Face(s_wnt);
	  B.Continuity(e_wnt,f_wnt,f_wnt,cont);
	}
      }
      if(aDirS.Closed()){
	Sweep_NumShape dirv = myDirShapeTool.Shape(2);
	if(GDDShapeIsToAdd(Shape(aGenS,aDirS),
			   Shape(aGenS,dirv),
			   aGenS,aDirS,dirv)){
	  TopLoc_Location Lo;
	  Standard_Real fi,la;
	  cont = BRep_Tool::Curve(E,Lo,fi,la)->Continuity();
	  if(cont >= 1){
	    TopoDS_Shape s_wnt = Shape(aGenS,dirv);
	    TopoDS_Edge e_wnt = TopoDS::Edge(s_wnt);
	    s_wnt = Shape(aGenS,aDirS);
	    TopoDS_Face f_wnt = TopoDS::Face(s_wnt);
	    B.Continuity(e_wnt,f_wnt,f_wnt,cont);
	  }
	}	
      }
    }
  }
  else if(aGenS.ShapeType() == TopAbs_WIRE){
    TopoDS_Edge E1,E2;
    BRepAdaptor_Curve e1,e2;
    Standard_Real u1,u2;
    TopTools_IndexedDataMapOfShapeListOfShape M;
    TopExp::MapShapesAndAncestors(aGenS,TopAbs_VERTEX,TopAbs_EDGE,M);
    TopTools_ListIteratorOfListOfShape It,Jt;    
    for(Standard_Integer i = 1; i <= M.Extent(); i++){
      TopoDS_Vertex V = TopoDS::Vertex(M.FindKey(i));
      Standard_Integer j = 1;
      for(It.Initialize(M.FindFromIndex(i));It.More();It.Next(),j++){
	Jt.Initialize(M.FindFromIndex(i));
	for(Standard_Integer k=1; k <= j; k++) { Jt.Next(); }
	for(;Jt.More();Jt.Next()){
	  E1 = TopoDS::Edge(It.Value());
	  E2 = TopoDS::Edge(Jt.Value());
	  if (!E1.IsSame(E2) && HasShape(E1,aDirS) && HasShape(E2,aDirS)){
	    u1 = BRep_Tool::Parameter(V,E1);
	    u2 = BRep_Tool::Parameter(V,E2);
//	    tol3d = Max(tl,BRep_Tool::Tolerance(V));
	    tol3d = Max(tl,2.*BRep_Tool::Tolerance(V)); //IFV 24.05.00 buc60684
	    e1.Initialize(E1);
	    e2.Initialize(E2);
	    cont = BRepLProp::Continuity(e1,e2,u1,u2,tol3d,ta);
	    if(cont >= 1){
	      TopoDS_Shape s_wnt = Shape(V,aDirS);
	      TopoDS_Edge e_wnt = TopoDS::Edge(s_wnt);
	      s_wnt = Shape(E1,aDirS);
	      TopoDS_Face f1_wnt = TopoDS::Face(s_wnt);
	      s_wnt = Shape(E2,aDirS);
	      TopoDS_Face f2_wnt = TopoDS::Face(s_wnt);
	      B.Continuity(e_wnt,f1_wnt,f2_wnt,cont);
	    }
	  }
	}
      }
    }
  }
}
