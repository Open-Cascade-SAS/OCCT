// Created on: 1994-10-07
// Created by: Yves FRICAUD
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


#include <BRepMAT2d_BisectingLocus.hxx>
#include <BRepMAT2d_DataMapOfShapeSequenceOfBasicElt.hxx>
#include <BRepMAT2d_Explorer.hxx>
#include <BRepMAT2d_LinkTopoBilo.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Geometry.hxx>
#include <gp_Pnt2d.hxx>
#include <MAT_BasicElt.hxx>
#include <MAT_Graph.hxx>
#include <MAT_SequenceOfBasicElt.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_SequenceOfShape.hxx>

//=======================================================================
//function : BRepMAT2d_LinkTopoBilo
//purpose  : 
//=======================================================================
BRepMAT2d_LinkTopoBilo::BRepMAT2d_LinkTopoBilo()
{
}


//=======================================================================
//function : BRepMAT2d_LinkTopoBilo
//purpose  : 
//=======================================================================

BRepMAT2d_LinkTopoBilo::BRepMAT2d_LinkTopoBilo(
   const BRepMAT2d_Explorer&       Explo,
   const BRepMAT2d_BisectingLocus& BiLo)
{
  Perform (Explo,BiLo);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepMAT2d_LinkTopoBilo::Perform(const BRepMAT2d_Explorer&       Explo, 
				     const BRepMAT2d_BisectingLocus& BiLo)
{
  myMap.Clear();
  myBEShape.Clear();

  TopoDS_Shape     S          = Explo.Shape();
  Standard_Integer IndContour = 1;

  if (S.ShapeType() == TopAbs_FACE) {
    TopExp_Explorer  Exp (S,TopAbs_WIRE);
    
    while (Exp.More()) {
      LinkToWire(TopoDS::Wire (Exp.Current()),Explo,IndContour,BiLo);
      Exp.Next();
      IndContour++;
    }
  }
  else {
    throw Standard_ConstructionError("BRepMAT2d_LinkTopoBilo::Perform");
  }
  
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepMAT2d_LinkTopoBilo::Init(const TopoDS_Shape& S)
{
  isEmpty = Standard_False;
  current = 1;
  if (myMap.IsBound(S)) myKey = S; else isEmpty = Standard_True;
}


//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean  BRepMAT2d_LinkTopoBilo::More()
{
  if (isEmpty) return Standard_False;
  return (current <=  myMap(myKey).Length());
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void BRepMAT2d_LinkTopoBilo::Next()
{
  current++;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(MAT_BasicElt) BRepMAT2d_LinkTopoBilo::Value() const 
{
  return myMap(myKey).Value(current);
}

//=======================================================================
//function : GeneratingShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepMAT2d_LinkTopoBilo::GeneratingShape
(const Handle(MAT_BasicElt)& BE) const
{
  return myBEShape(BE);
}

static void LinkToContour(const BRepMAT2d_Explorer&              Explo,
			  const Standard_Integer                 IndC,
			  const BRepMAT2d_BisectingLocus&        BiLo,
			        TColStd_DataMapOfIntegerInteger& Link);

//=======================================================================
//function : LinkToWire
//purpose  : 
//=======================================================================

void BRepMAT2d_LinkTopoBilo::LinkToWire(const TopoDS_Wire&              W,
					const BRepMAT2d_Explorer&       Explo, 
					const Standard_Integer          IndC,
					const BRepMAT2d_BisectingLocus& BiLo)
{
  BRepTools_WireExplorer       TheExp (W);
  Standard_Integer             KC;
  TopoDS_Vertex                VF,VL;
  TopoDS_Shape                 S;
  Handle(MAT_BasicElt)         BE;
  Handle(Standard_Type)        Type;
  TopTools_SequenceOfShape     TopoSeq;
  MAT_SequenceOfBasicElt EmptySeq;

  TColStd_DataMapIteratorOfDataMapOfIntegerInteger Ite;
  TColStd_DataMapOfIntegerInteger LinkBECont;


  for (;TheExp.More();TheExp.Next()) {
    TopoSeq.Append(TheExp.Current());
  }
  
  //-----------------------------------------------------
  // Construction Links BasicElt => Curve of contour IndC.
  //-----------------------------------------------------
  LinkToContour(Explo,IndC,BiLo,LinkBECont);
  

  //---------------------------------------------------------------
  // Iteration on BasicElts. The associated index is the same for
  // the curves of the contour and the edges.               .
  //---------------------------------------------------------------
  for (Ite.Initialize(LinkBECont); Ite.More(); Ite.Next()) {
    BE     = BiLo.Graph()->BasicElt(Ite.Key());    
    Type   = BiLo.GeomElt(BE)->DynamicType();
    KC     = Ite.Value();
    S      = TopoSeq.Value(Abs(KC));

    if (Type == STANDARD_TYPE(Geom2d_CartesianPoint)) {
      if (S.Orientation() == TopAbs_REVERSED) {
	TopExp::Vertices(TopoDS::Edge(S),VL,VF);
      }
      else {
	TopExp::Vertices(TopoDS::Edge(S),VF,VL);
      }
      if (KC > 0) S = VL; else S = VF;
    }
    if (!myMap.IsBound(S)) {       
      myMap.Bind(S,EmptySeq);
    }
    myMap(S).Append(BE);

    if (KC < 0) 
      myBEShape.Bind(BE, S.Oriented(TopAbs::Reverse(S.Orientation())));
    else
      myBEShape.Bind(BE, S);
  }
}


//=======================================================================
//function : LinkToContour
//purpose  : Association to each basicElt of the curre of the initial
//           contour from which it comes.
//=======================================================================

void LinkToContour (const BRepMAT2d_Explorer&              Explo,
		    const Standard_Integer                 IndC,
		    const BRepMAT2d_BisectingLocus&        BiLo,
		    TColStd_DataMapOfIntegerInteger&       Link)
{
  Handle (MAT_BasicElt)    BE;
  Handle (Geom2d_Geometry) GeomBE;
  Handle (Standard_Type)   Type;
  Standard_Boolean         DirectSense   = Standard_True;
  Standard_Boolean         LastPoint     = Standard_False;
  Standard_Integer         NbSect,ISect;     

  //---------------------------------------------------
  // NbSect : number of sections on the current curve.
  // ISect  : Counter on sections.
  //---------------------------------------------------

  const TColGeom2d_SequenceOfCurve&  Cont = Explo.Contour(IndC);
  
  //------------------------------------------------------------------
  //Initialization of the explorer on the first curve of the contour.
  //------------------------------------------------------------------
  Standard_Integer         IndOnCont     =  1;   
  Standard_Integer         PrecIndOnCont = -1;
  NbSect = BiLo.NumberOfSections(IndC,1);
  ISect  = 0;

  //------------------------------------------------------------------
  // Parsing of base elements associated to contour IndC.
  // Rq : the base elements are ordered.
  //------------------------------------------------------------------
  for (Standard_Integer i = 1; i <= BiLo.NumberOfElts(IndC); i++) {

    BE     = BiLo.BasicElt(IndC,i);
    GeomBE = BiLo.GeomElt (BE);
    Type   = GeomBE->DynamicType();

    if (Type != STANDARD_TYPE(Geom2d_CartesianPoint)) {
      ISect++;
      //----------------------------------------------------------------
      // The base element is a curve associated with the current curve.
      //----------------------------------------------------------------
      if (DirectSense) {
	Link.Bind(BE->Index(),  IndOnCont);
      }
      else {
	Link.Bind(BE->Index(), -IndOnCont);
      }
    }
    else {      
      //-----------------------------------------------------------------
      // The base element is a point associated with the previous curve.
      //-----------------------------------------------------------------
      if (DirectSense || LastPoint) {
	Link.Bind(BE->Index(),  PrecIndOnCont);
      }
      else {
	Link.Bind(BE->Index(), -PrecIndOnCont);
      }
    }

    PrecIndOnCont = IndOnCont;
    //----------------------------------------------------------------------
    // Passage to the next curve in Explo, when all parts 
    // of curves corresponding to the initial curve have been parsed.
    //---------------------------------------------------------------------
    if (Type != STANDARD_TYPE(Geom2d_CartesianPoint) && ISect == NbSect) {
      if (IndOnCont < Cont.Length() && DirectSense) {
	IndOnCont++;  
	NbSect = BiLo.NumberOfSections(IndC,IndOnCont);
	ISect  = 0;
      }
      else {
	//-----------------------------------------------------
	// For open lines restart in the other direction.
	//-----------------------------------------------------
	if (!DirectSense) {
	  IndOnCont--;
	  if (IndOnCont != 0) NbSect = BiLo.NumberOfSections(IndC,IndOnCont);
	  LastPoint = Standard_False;
	}
	else {
	  LastPoint = Standard_True;
	}
	ISect       = 0;
	DirectSense = Standard_False;
      }
    }
  }
}
