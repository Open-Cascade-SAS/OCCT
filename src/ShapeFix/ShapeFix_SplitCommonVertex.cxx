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

#include <ShapeFix_SplitCommonVertex.ixx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <gp_Pnt.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_WireData.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>


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

