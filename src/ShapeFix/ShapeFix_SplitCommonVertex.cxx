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
#include <gp_Pnt.hxx>
#include <Message_Msg.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_SplitCommonVertex.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_SplitCommonVertex,ShapeFix_Root)

//=======================================================================
//function : ShapeFix_SplitCommonVertex
//purpose  : 
//=======================================================================
ShapeFix_SplitCommonVertex::ShapeFix_SplitCommonVertex()
{
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
  SetPrecision(Precision::Confusion());
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeFix_SplitCommonVertex::Init(const TopoDS_Shape& S) 
{
  myShape = S;
  if ( Context().IsNull() ) 
    SetContext ( new ShapeBuild_ReShape );
  myResult = myShape;
  Context()->Apply(myShape);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void ShapeFix_SplitCommonVertex::Perform() 
{
  TopAbs_ShapeEnum st = myShape.ShapeType();
  if(st>TopAbs_FACE) return;
  for(TopExp_Explorer itf(myShape,TopAbs_FACE); itf.More(); itf.Next()) {
    TopoDS_Shape tmpFace = Context()->Apply(itf.Current());
    TopoDS_Face F = TopoDS::Face(tmpFace);
    if(F.IsNull()) continue;
    // analys face and split if necessary
    TopTools_SequenceOfShape wires;
    for(TopoDS_Iterator itw(F,Standard_False); itw.More(); itw.Next()) {
      if(itw.Value().ShapeType() != TopAbs_WIRE)
	continue;
      wires.Append(itw.Value());
    }
    if(wires.Length()<2) continue;
    TopTools_DataMapOfShapeShape MapVV;
    MapVV.Clear();
    for(Standard_Integer nw1=1; nw1<wires.Length(); nw1++) {
      TopoDS_Wire w1 = TopoDS::Wire(wires.Value(nw1));
      Handle(ShapeExtend_WireData) sewd1 = new ShapeExtend_WireData(w1);
      for(Standard_Integer nw2=nw1+1; nw2<=wires.Length(); nw2++) {
        TopoDS_Wire w2 = TopoDS::Wire(wires.Value(nw2));
        Handle(ShapeExtend_WireData) sewd2 = new ShapeExtend_WireData(w2);

        for(TopExp_Explorer expv1(w1,TopAbs_VERTEX); expv1.More(); expv1.Next()) {
          TopoDS_Vertex V1 = TopoDS::Vertex(expv1.Current());
          for(TopExp_Explorer expv2(w2,TopAbs_VERTEX); expv2.More(); expv2.Next()) {
            TopoDS_Vertex V2 = TopoDS::Vertex(expv2.Current());
            if(V1==V2) {
              // common vertex exists
              TopoDS_Vertex Vnew;
              if(MapVV.IsBound(V2)) {
                Vnew = TopoDS::Vertex(MapVV.Find(V2));
              }
              else {
                gp_Pnt P = BRep_Tool::Pnt(V2);
                Standard_Real tol = BRep_Tool::Tolerance(V2);
                BRep_Builder B;
                B.MakeVertex(Vnew,P,tol);
                MapVV.Bind(V2,Vnew);
              }
              ShapeBuild_Edge sbe;
              ShapeAnalysis_Edge sae;
              for(Standard_Integer ne2=1; ne2<=sewd2->NbEdges(); ne2++) {
                TopoDS_Edge E = sewd2->Edge(ne2);
                TopoDS_Vertex FV = sae.FirstVertex(E);
                TopoDS_Vertex LV = sae.LastVertex(E);
                Standard_Boolean IsCoinc = Standard_False;
                if(FV==V2) {
                  FV=Vnew;
                  IsCoinc = Standard_True;
                }
                if(LV==V2) {
                  LV=Vnew;
                  IsCoinc = Standard_True;
                }
                if(IsCoinc) {
                  TopoDS_Edge NewE = sbe.CopyReplaceVertices(E,FV,LV);
                  Context()->Replace(E,NewE);
                }
              }
            }
          }
        }

      }
    }
    if ( !MapVV.IsEmpty() )
      SendWarning( Message_Msg( "Fix.SplitCommonVertex.MSG0" ));
  }

  myShape = Context()->Apply(myShape);  

}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape ShapeFix_SplitCommonVertex::Shape() 
{
  return myShape;
}

