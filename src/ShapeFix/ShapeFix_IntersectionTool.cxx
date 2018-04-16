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


#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <gp_Pnt.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntRes2d_Position.hxx>
#include <NCollection_Sequence.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeAnalysis_TransferParametersProj.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_DataMapOfShapeBox2d.hxx>
#include <ShapeFix_IntersectionTool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_SequenceOfShape.hxx>

//gka 06.09.04 BUG 6555 shape is modified always independently either intersection was fixed or not 
//=======================================================================
//function : ShapeFix_IntersectionTool
//purpose  : 
//=======================================================================
ShapeFix_IntersectionTool::ShapeFix_IntersectionTool(const Handle(ShapeBuild_ReShape)& context,
                                                     const Standard_Real preci,
                                                     const Standard_Real maxtol)
{
  myContext = context;
  myPreci = preci;
  myMaxTol = maxtol;
}


//=======================================================================
//function : GetPointOnEdge
//purpose  : auxiliary
//:h0 abv 29 May 98: PRO10105 1949: like in BRepCheck, point is to be taken 
// from 3d curve (but only if edge is SameParameter)
//=======================================================================
static gp_Pnt GetPointOnEdge(const TopoDS_Edge &edge, 
                             const Handle(ShapeAnalysis_Surface) &surf,
                             const Geom2dAdaptor_Curve &Crv2d, 
                             const Standard_Real param )
{
  if( BRep_Tool::SameParameter(edge) ) {
    Standard_Real f,l;
    TopLoc_Location L;
    const Handle(Geom_Curve) ConS = BRep_Tool::Curve ( edge, L, f, l );
    if( !ConS.IsNull() )
      return ConS->Value(param).Transformed(L.Transformation());
  }
  gp_Pnt2d aP2d = Crv2d.Value(param);
  return surf->Adaptor3d()->Value(aP2d.X(), aP2d.Y());
}


//=======================================================================
//function : SplitEdge
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_IntersectionTool::SplitEdge(const TopoDS_Edge& edge,
                                                      const Standard_Real param,
                                                      const TopoDS_Vertex& vert,
                                                      const TopoDS_Face& face,
                                                      TopoDS_Edge& newE1,
                                                      TopoDS_Edge& newE2,
                                                      const Standard_Real preci) const
{
  Standard_Real a, b;
  ShapeAnalysis_Edge sae;

  TopoDS_Vertex V1 = sae.FirstVertex(edge);
  TopoDS_Vertex V2 = sae.LastVertex(edge);
  if( V1.IsSame(vert) || V2.IsSame(vert) )
    return Standard_False;

  Handle(Geom2d_Curve) c2d;
  sae.PCurve(edge,face,c2d,a,b,Standard_True );
  if( Abs(a-param)<0.01*preci || Abs(b-param)<0.01*preci )
    return Standard_False;
  // check distanse between edge and new vertex
  gp_Pnt P1;
  TopLoc_Location L;
  if(BRep_Tool::SameParameter(edge) && !BRep_Tool::Degenerated(edge)) {
    Standard_Real f,l;
    const Handle(Geom_Curve) c3d = BRep_Tool::Curve(edge,L,f,l);
    if(c3d.IsNull())
      return Standard_False;
    P1 = c3d->Value(param);
    if(!L.IsIdentity()) P1 = P1.Transformed(L.Transformation());
  }
  else {
    Handle(Geom_Surface) surf = BRep_Tool::Surface(face,L);
    Handle(ShapeAnalysis_Surface) sas = new ShapeAnalysis_Surface(surf);
    P1 = sas->Value(c2d->Value(param));
    if(!L.IsIdentity()) P1 = P1.Transformed(L.Transformation());
  }
  gp_Pnt P2 = BRep_Tool::Pnt(vert);
  if(P1.Distance(P2)>preci) {
    //return Standard_False;
    BRep_Builder B;
    B.UpdateVertex(vert,P1.Distance(P2));
  }
  
  Handle(ShapeAnalysis_TransferParametersProj) transferParameters =
    new ShapeAnalysis_TransferParametersProj;
  transferParameters->SetMaxTolerance(preci);
  transferParameters->Init(edge,face);
  Standard_Real first, last;
  if (a < b ) {
    first = a; 
    last = b;
  }
  else {
    first = b; 
    last = a;
  }
  
  ShapeBuild_Edge sbe;
  TopAbs_Orientation orient = edge.Orientation();
  BRep_Builder B;
  TopoDS_Edge wE = edge;
  wE.Orientation ( TopAbs_FORWARD );
  TopoDS_Shape aTmpShape = vert.Oriented(TopAbs_REVERSED); //for porting
  newE1 = sbe.CopyReplaceVertices ( wE, sae.FirstVertex(wE), TopoDS::Vertex(aTmpShape) );
  sbe.CopyPCurves ( newE1, wE  );
  transferParameters->TransferRange(newE1,first,param,Standard_True);
  B.SameRange(newE1,Standard_False);
  B.SameParameter(newE1,Standard_False);
  aTmpShape = vert.Oriented(TopAbs_FORWARD);
  newE2 = sbe.CopyReplaceVertices ( wE, TopoDS::Vertex(aTmpShape),sae.LastVertex(wE) );
  sbe.CopyPCurves ( newE2, wE  );
  transferParameters->TransferRange(newE2,param,last,Standard_True);
  B.SameRange(newE2,Standard_False);
  B.SameParameter(newE2,Standard_False);

  newE1.Orientation(orient);
  newE2.Orientation(orient);
  if (orient==TopAbs_REVERSED) { 
    TopoDS_Edge tmp = newE2; newE2 = newE1; newE1=tmp;
  }

  return Standard_True;
}


//=======================================================================
//function : CutEdge
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_IntersectionTool::CutEdge(const TopoDS_Edge &edge,
                                                    const Standard_Real pend,
                                                    const Standard_Real cut,
                                                    const TopoDS_Face &face,
                                                    Standard_Boolean &iscutline) const
{
  if( Abs(cut-pend)<10.*Precision::PConfusion() ) return Standard_False;
  Standard_Real aRange = Abs(cut-pend);
  Standard_Real a, b;
  BRep_Tool::Range(edge, a, b);
  
  if( aRange<10.*Precision::PConfusion() ) return Standard_False;

  // case pcurve is trimm of line
  if( !BRep_Tool::SameParameter(edge) ) {
    ShapeAnalysis_Edge sae;
    Handle(Geom2d_Curve) Crv;
    Standard_Real fp,lp;
    if ( sae.PCurve(edge,face,Crv,fp,lp,Standard_False) ) {
      if(Crv->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) {
        Handle(Geom2d_TrimmedCurve) tc = Handle(Geom2d_TrimmedCurve)::DownCast(Crv);
        if(tc->BasisCurve()->IsKind(STANDARD_TYPE(Geom2d_Line))) {
          BRep_Builder B;
          B.Range(edge,Min(pend,cut),Max(pend,cut));
          if( Abs(pend-lp)<Precision::PConfusion() ) { // cut from the begining
            Standard_Real cut3d = (cut-fp)*(b-a)/(lp-fp);
            B.Range(edge, a+cut3d, b, Standard_True);
            iscutline = Standard_True;
          }
          else if( Abs(pend-fp)<Precision::PConfusion() ) { // cut from the end
            Standard_Real cut3d = (lp-cut)*(b-a)/(lp-fp);
            B.Range(edge, a, b-cut3d, Standard_True);
            iscutline = Standard_True;
          }
        }

        return Standard_True;
      }
     
    }
    return Standard_False;
  }

  // det-study on 03/12/01 checking the old and new ranges
  if( Abs(Abs(a-b)-aRange) < Precision::PConfusion() ) return Standard_False;
  if( aRange<10.*Precision::PConfusion() ) return Standard_False;

  BRep_Builder B;
  B.Range( edge, Min(pend,cut), Max(pend,cut) );

  return Standard_True;
}


//=======================================================================
//function : SplitEdge1
//purpose  : split edge[a,b] om two part e1[a,param]
//           and e2[param,b] using vertex vert
//=======================================================================

Standard_Boolean ShapeFix_IntersectionTool::SplitEdge1(const Handle(ShapeExtend_WireData)& sewd,
                                                       const TopoDS_Face& face,
                                                       const Standard_Integer num,
                                                       const Standard_Real param,
                                                       const TopoDS_Vertex& vert,
                                                       const Standard_Real preci,
                                                       ShapeFix_DataMapOfShapeBox2d& boxes) const
{
  Standard_ASSERT_RETURN (num > 0 && num <= sewd->NbEdges(), "Edge index out of range", Standard_False);

  TopoDS_Edge edge = sewd->Edge(num);
  TopoDS_Edge newE1, newE2;
  if(!SplitEdge(edge,param,vert,face,newE1,newE2,preci)) return Standard_False;

  // change context
  Handle(ShapeExtend_WireData) wd = new ShapeExtend_WireData;
  wd->Add(newE1);
  wd->Add(newE2);
  if(!myContext.IsNull()) myContext->Replace( edge, wd->Wire() );
  for (TopExp_Explorer exp ( wd->Wire(), TopAbs_EDGE ); exp.More(); exp.Next() ) {
    TopoDS_Edge E = TopoDS::Edge ( exp.Current() );
    BRepTools::Update(E);
  }
  
  // change sewd
  sewd->Set(newE1,num);
  if(num==sewd->NbEdges())
    sewd->Add(newE2);
  else
    sewd->Add(newE2,num+1);

  // change boxes
  boxes.UnBind(edge);
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(face,L);
  Handle(Geom2d_Curve) c2d;
  Standard_Real cf,cl;
  ShapeAnalysis_Edge sae;
  if(sae.PCurve(newE1,S,L,c2d,cf,cl,Standard_False)) {
    Bnd_Box2d box;
    Geom2dAdaptor_Curve gac;
    Standard_Real aFirst = c2d->FirstParameter();
    Standard_Real aLast = c2d->LastParameter();
    if(c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) 
       && (cf < aFirst || cl > aLast)) {
      //pdn avoiding problems with segment in Bnd_Box
      gac.Load(c2d);
    }
    else
      gac.Load(c2d,cf,cl);
    BndLib_Add2dCurve::Add(gac,::Precision::Confusion(),box);
    boxes.Bind(newE1,box);
  }
  if(sae.PCurve(newE2,S,L,c2d,cf,cl,Standard_False)) {
    Bnd_Box2d box;
    Geom2dAdaptor_Curve gac;
    Standard_Real aFirst = c2d->FirstParameter();
    Standard_Real aLast = c2d->LastParameter();
    if(c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) 
       && (cf < aFirst || cl > aLast)) {
      //pdn avoiding problems with segment in Bnd_Box
      gac.Load(c2d);
    }
    else
      gac.Load(c2d,cf,cl);
    BndLib_Add2dCurve::Add(gac,::Precision::Confusion(),box);
    boxes.Bind(newE2,box);
  }

  return Standard_True;
}


//=======================================================================
//function : SplitEdge2
//purpose  : auxilary: split edge[a,b] om two part e1[a,param1]
//                     and e2[param2,b] using vertex vert
//                     (remove segment (param1,param2) from edge)
//=======================================================================

Standard_Boolean ShapeFix_IntersectionTool::SplitEdge2(const Handle(ShapeExtend_WireData)& sewd,
                                                       const TopoDS_Face& face,
                                                       const Standard_Integer num,
                                                       const Standard_Real param1,
                                                       const Standard_Real param2,
                                                       const TopoDS_Vertex& vert,
                                                       const Standard_Real preci,
                                                       ShapeFix_DataMapOfShapeBox2d& boxes) const
{
  TopoDS_Edge edge = sewd->Edge(num);
  TopoDS_Edge newE1, newE2;
  Standard_Real param = (param1+param2)/2;
  if(!SplitEdge(edge,param,vert,face,newE1,newE2,preci)) return Standard_False;
  // cut new edges by param1 and param2
  Standard_Boolean IsCutLine;
  Handle(Geom2d_Curve) Crv1, Crv2;
  Standard_Real fp1,lp1,fp2,lp2;
  ShapeAnalysis_Edge sae;
  if(sae.PCurve ( newE1, face, Crv1, fp1, lp1, Standard_False )) {
    if(sae.PCurve ( newE2, face, Crv2, fp2, lp2, Standard_False )) {
      if(lp1==param) {
        if( (lp1-fp1)*(lp1-param1)>0 ) {
          CutEdge(newE1, fp1, param1, face, IsCutLine);
          CutEdge(newE2, lp2, param2, face, IsCutLine);
        }
        else {
          CutEdge(newE1, fp1, param2, face, IsCutLine);
          CutEdge(newE2, lp2, param1, face, IsCutLine);
        }
      }
      else {
        if( (fp1-lp1)*(fp1-param1)>0 ) {
          CutEdge(newE1, lp1, param1, face, IsCutLine);
          CutEdge(newE2, fp2, param2, face, IsCutLine);
        }
        else {
          CutEdge(newE1, lp1, param2, face, IsCutLine);
          CutEdge(newE2, fp2, param1, face, IsCutLine);
        }
      }
    }
  }
    
  // change context
  Handle(ShapeExtend_WireData) wd = new ShapeExtend_WireData;
  wd->Add(newE1);
  wd->Add(newE2);
  if(!myContext.IsNull()) myContext->Replace( edge, wd->Wire() );
  for (TopExp_Explorer exp ( wd->Wire(), TopAbs_EDGE ); exp.More(); exp.Next() ) {
      TopoDS_Edge E = TopoDS::Edge ( exp.Current() );
      BRepTools::Update(E);
    }
  
  // change sewd
  sewd->Set(newE1,num);
  if(num==sewd->NbEdges())
    sewd->Add(newE2);
  else
    sewd->Add(newE2,num+1);

  // change boxes
  boxes.UnBind(edge);
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(face,L);
  Handle(Geom2d_Curve) c2d;
  Standard_Real cf,cl;
  if(sae.PCurve(newE1,S,L,c2d,cf,cl,Standard_False)) {
    Bnd_Box2d box;
    Geom2dAdaptor_Curve gac;
    Standard_Real aFirst = c2d->FirstParameter();
    Standard_Real aLast = c2d->LastParameter();
    if(c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) 
       && (cf < aFirst || cl > aLast)) {
      //pdn avoiding problems with segment in Bnd_Box
      gac.Load(c2d);
    }
    else
      gac.Load(c2d,cf,cl);
    BndLib_Add2dCurve::Add(gac,::Precision::Confusion(),box);
    boxes.Bind(newE1,box);
  }
  if(sae.PCurve(newE2,S,L,c2d,cf,cl,Standard_False)) {
    Bnd_Box2d box;
    Geom2dAdaptor_Curve gac;
    Standard_Real aFirst = c2d->FirstParameter();
    Standard_Real aLast = c2d->LastParameter();
    if(c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) 
       && (cf < aFirst || cl > aLast)) {
      //pdn avoiding problems with segment in Bnd_Box
      gac.Load(c2d);
    }
    else
      gac.Load(c2d,cf,cl);
    BndLib_Add2dCurve::Add(gac,::Precision::Confusion(),box);
    boxes.Bind(newE2,box);
  }

  return Standard_True;
}


//=======================================================================
//function : UnionVertexes
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_IntersectionTool::UnionVertexes(const Handle(ShapeExtend_WireData)& sewd,
                                                          TopoDS_Edge& edge1,
                                                          TopoDS_Edge& edge2,
                                                          const Standard_Integer num2,
                                                          ShapeFix_DataMapOfShapeBox2d& boxes,
                                                          const Bnd_Box2d& B2) const
{
  // union vertexes
  Standard_Boolean res = Standard_False;
  ShapeBuild_Edge sbe;
  ShapeAnalysis_Edge sae;
  BRep_Builder B;
  TopoDS_Vertex V;
  TopoDS_Vertex V1F = sae.FirstVertex(edge1);
  gp_Pnt PV1F = BRep_Tool::Pnt(V1F);
  TopoDS_Vertex V1L = sae.LastVertex(edge1);
  gp_Pnt PV1L = BRep_Tool::Pnt(V1L);
  TopoDS_Vertex V2F = sae.FirstVertex(edge2);
  gp_Pnt PV2F = BRep_Tool::Pnt(V2F);
  TopoDS_Vertex V2L = sae.LastVertex(edge2);
  gp_Pnt PV2L = BRep_Tool::Pnt(V2L);
  Standard_Real d11 = PV1F.Distance(PV2F);
  Standard_Real d12 = PV1F.Distance(PV2L);
  Standard_Real d21 = PV1L.Distance(PV2F);
  Standard_Real d22 = PV1L.Distance(PV2L);
  if(d11<d12 && d11<d21 && d11<d22) {
    Standard_Real tolv = Max(BRep_Tool::Tolerance(V1F),BRep_Tool::Tolerance(V2F));
    if( !V2F.IsSame(V1F) && d11<tolv ) {
      // union vertexes V1F and V2F
      B.UpdateVertex(V1F,tolv);
      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V1F,V2L);
//      cout<<"union vertexes V1F and V2F"<<endl;
//      gp_Pnt Ptmp = BRep_Tool::Pnt(V1F);
//      B.MakeVertex(V,Ptmp,tolv);
//      myContext->Replace(V1F,V);
//      myContext->Replace(V2F,V);
//      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V,V2L);
      myContext->Replace(edge2,NewE);
      sewd->Set(NewE,num2);
      edge2 = NewE;
      boxes.Bind(NewE,B2); // update boxes
      // replace vertex in other edge
      Standard_Integer num21,num22;
      if(num2>1) num21=num2-1;
      else num21=sewd->NbEdges();
      if(num2<sewd->NbEdges()) num22=num2+1;
      else num22=1;
      TopoDS_Edge edge21 = sewd->Edge(num21);
      TopoDS_Edge edge22 = sewd->Edge(num22);
      TopoDS_Vertex V21F = sae.FirstVertex(edge21);
      TopoDS_Vertex V21L = sae.LastVertex(edge21);
      TopoDS_Vertex V22F = sae.FirstVertex(edge22);
      TopoDS_Vertex V22L = sae.LastVertex(edge22);
      if(V21F.IsSame(V2F)) {
        NewE = sbe.CopyReplaceVertices(edge21,V1F,V21L);
        //NewE = sbe.CopyReplaceVertices(edge21,V,V21L);
        if (boxes.IsBound(edge21))
          boxes.Bind(NewE,boxes.Find(edge21)); // update boxes
        myContext->Replace(edge21,NewE);
        sewd->Set(NewE,num21);
      }
      if(V21L.IsSame(V2F)) {
        NewE = sbe.CopyReplaceVertices(edge21,V21F,V1F);
        //NewE = sbe.CopyReplaceVertices(edge21,V21F,V);
        if (boxes.IsBound(edge21))
          boxes.Bind(NewE,boxes.Find(edge21)); // update boxes
        myContext->Replace(edge21,NewE);
        sewd->Set(NewE,num21);
      }
      if(V22F.IsSame(V2F)) {
        NewE = sbe.CopyReplaceVertices(edge22,V1F,V22L);
        //NewE = sbe.CopyReplaceVertices(edge22,V,V22L);
        if (boxes.IsBound(edge22))
          boxes.Bind(NewE,boxes.Find(edge22)); // update boxes
        myContext->Replace(edge22,NewE);
        sewd->Set(NewE,num22);
      }
      if(V22L.IsSame(V2F)) {
        NewE = sbe.CopyReplaceVertices(edge22,V22F,V1F);
        //NewE = sbe.CopyReplaceVertices(edge22,V22F,V);
        if (boxes.IsBound(edge22))
          boxes.Bind(NewE,boxes.Find(edge22)); // update boxes
        myContext->Replace(edge22,NewE);
        sewd->Set(NewE,num22);
      }
      res = Standard_True;
    }
  }
  else if(d12<d21 && d12<d22) {
    Standard_Real tolv = Max(BRep_Tool::Tolerance(V1F),BRep_Tool::Tolerance(V2L));
    if( !V2L.IsSame(V1F) && d12<tolv ) {
      // union vertexes V1F and V2L
      B.UpdateVertex(V1F,tolv);
      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V2F,V1F);
//      cout<<"union vertexes V1F and V2L"<<endl;
//      gp_Pnt Ptmp = BRep_Tool::Pnt(V1F);
//      B.MakeVertex(V,Ptmp,tolv);
//      myContext->Replace(V1F,V);
//      myContext->Replace(V2L,V);
//      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V2F,V);
      myContext->Replace(edge2,NewE);
      sewd->Set(NewE,num2);
      edge2 = NewE;
      //boxes.Bind(NewE,boxes.Find(edge2)); // update boxes
      boxes.Bind(NewE,B2); // update boxes
      // replace vertex in other edge
      Standard_Integer num21,num22;
      if(num2>1) num21=num2-1;
      else num21=sewd->NbEdges();
      if(num2<sewd->NbEdges()) num22=num2+1;
      else num22=1;
      TopoDS_Edge edge21 = sewd->Edge(num21);
      TopoDS_Edge edge22 = sewd->Edge(num22);
      TopoDS_Vertex V21F = sae.FirstVertex(edge21);
      TopoDS_Vertex V21L = sae.LastVertex(edge21);
      TopoDS_Vertex V22F = sae.FirstVertex(edge22);
      TopoDS_Vertex V22L = sae.LastVertex(edge22);
      if(V21F.IsSame(V2L)) {
        NewE = sbe.CopyReplaceVertices(edge21,V1F,V21L);
        //NewE = sbe.CopyReplaceVertices(edge21,V,V21L);
        if (boxes.IsBound(edge21))
          boxes.Bind(NewE,boxes.Find(edge21)); // update boxes
        myContext->Replace(edge21,NewE);
        sewd->Set(NewE,num21);
      }
      if(V21L.IsSame(V2L)) {
        NewE = sbe.CopyReplaceVertices(edge21,V21F,V1F);
        //NewE = sbe.CopyReplaceVertices(edge21,V21F,V);
        if (boxes.IsBound(edge21))
          boxes.Bind(NewE,boxes.Find(edge21)); // update boxes
        myContext->Replace(edge21,NewE);
        sewd->Set(NewE,num21);
      }
      if(V22F.IsSame(V2L)) {
        NewE = sbe.CopyReplaceVertices(edge22,V1F,V22L);
        //NewE = sbe.CopyReplaceVertices(edge22,V,V22L);
        if (boxes.IsBound(edge22))
          boxes.Bind(NewE,boxes.Find(edge22)); // update boxes
        myContext->Replace(edge22,NewE);
        sewd->Set(NewE,num22);
      }
      if(V22L.IsSame(V2L)) {
        NewE = sbe.CopyReplaceVertices(edge22,V22F,V1F);
        //NewE = sbe.CopyReplaceVertices(edge22,V22F,V);
        if (boxes.IsBound(edge22))
          boxes.Bind(NewE,boxes.Find(edge22)); // update boxes
        myContext->Replace(edge22,NewE);
        sewd->Set(NewE,num22);
      }
      res = Standard_True;
    }
  }
  else if(d21<d22) {
    Standard_Real tolv = Max(BRep_Tool::Tolerance(V1L),BRep_Tool::Tolerance(V2F));
    if( !V2F.IsSame(V1L) && d21<tolv ) {
      // union vertexes V1L and V2F
      B.UpdateVertex(V1L,tolv);
      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V1L,V2L);
//      cout<<"union vertexes V1L and V2F"<<endl;
//      gp_Pnt Ptmp = BRep_Tool::Pnt(V1L);
//      B.MakeVertex(V,Ptmp,tolv);
//      myContext->Replace(V1L,V);
//      myContext->Replace(V2F,V);
//      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V,V2L);
      myContext->Replace(edge2,NewE);
      sewd->Set(NewE,num2);
      edge2 = NewE;
      boxes.Bind(NewE,B2); // update boxes
      // replace vertex in other edge
      Standard_Integer num21,num22;
      if(num2>1) num21=num2-1;
      else num21=sewd->NbEdges();
      if(num2<sewd->NbEdges()) num22=num2+1;
      else num22=1;
      TopoDS_Edge edge21 = sewd->Edge(num21);
      TopoDS_Edge edge22 = sewd->Edge(num22);
      TopoDS_Vertex V21F = sae.FirstVertex(edge21);
      TopoDS_Vertex V21L = sae.LastVertex(edge21);
      TopoDS_Vertex V22F = sae.FirstVertex(edge22);
      TopoDS_Vertex V22L = sae.LastVertex(edge22);
      if(V21F.IsSame(V2F)) {
        NewE = sbe.CopyReplaceVertices(edge21,V1L,V21L);
        //NewE = sbe.CopyReplaceVertices(edge21,V,V21L);
        if (boxes.IsBound(edge21))
          boxes.Bind(NewE,boxes.Find(edge21)); // update boxes
        myContext->Replace(edge21,NewE);
        sewd->Set(NewE,num21);
      }
      if(V21L.IsSame(V2F)) {
        NewE = sbe.CopyReplaceVertices(edge21,V21F,V1L);
        //NewE = sbe.CopyReplaceVertices(edge21,V21F,V);
        if (boxes.IsBound(edge21))
          boxes.Bind(NewE,boxes.Find(edge21)); // update boxes
        myContext->Replace(edge21,NewE);
        sewd->Set(NewE,num21);
      }
      if(V22F.IsSame(V2F)) {
        NewE = sbe.CopyReplaceVertices(edge22,V1L,V22L);
        //NewE = sbe.CopyReplaceVertices(edge22,V,V22L);
        if (boxes.IsBound(edge22))
          boxes.Bind(NewE,boxes.Find(edge22)); // update boxes
        myContext->Replace(edge22,NewE);
        sewd->Set(NewE,num22);
      }
      if(V22L.IsSame(V2F)) {
        NewE = sbe.CopyReplaceVertices(edge22,V22F,V1L);
        //NewE = sbe.CopyReplaceVertices(edge22,V22F,V);
        if (boxes.IsBound(edge22))
          boxes.Bind(NewE,boxes.Find(edge22)); // update boxes
        myContext->Replace(edge22,NewE);
        sewd->Set(NewE,num22);
      }
      res = Standard_True;
    }
  }
  else {
    Standard_Real tolv = Max(BRep_Tool::Tolerance(V1L),BRep_Tool::Tolerance(V2L));
    if( !V2L.IsSame(V1L) && d22<tolv ) {
      // union vertexes V1L and V2L
      B.UpdateVertex(V1L,tolv);
      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V2F,V1L);
//      cout<<"union vertexes V1L and V2L"<<endl;
//      gp_Pnt Ptmp = BRep_Tool::Pnt(V1L);
//      B.MakeVertex(V,Ptmp,tolv);
//      myContext->Replace(V1L,V);
//      myContext->Replace(V2L,V);
//      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V2F,V);
      myContext->Replace(edge2,NewE);
      sewd->Set(NewE,num2);
      edge2 = NewE;
      boxes.Bind(NewE,B2); // update boxes
      // replace vertex in other edge
      Standard_Integer num21,num22;
      if(num2>1) num21=num2-1;
      else num21=sewd->NbEdges();
      if(num2<sewd->NbEdges()) num22=num2+1;
      else num22=1;
      TopoDS_Edge edge21 = sewd->Edge(num21);
      TopoDS_Edge edge22 = sewd->Edge(num22);
      TopoDS_Vertex V21F = sae.FirstVertex(edge21);
      TopoDS_Vertex V21L = sae.LastVertex(edge21);
      TopoDS_Vertex V22F = sae.FirstVertex(edge22);
      TopoDS_Vertex V22L = sae.LastVertex(edge22);
      if(V21F.IsSame(V2L)) {
        NewE = sbe.CopyReplaceVertices(edge21,V1L,V21L);
        //NewE = sbe.CopyReplaceVertices(edge21,V,V21L);
        if (boxes.IsBound(edge21))
          boxes.Bind(NewE,boxes.Find(edge21)); // update boxes
        myContext->Replace(edge21,NewE);
        sewd->Set(NewE,num21);
      }
      if(V21L.IsSame(V2L)) {
        NewE = sbe.CopyReplaceVertices(edge21,V21F,V1L);
        //NewE = sbe.CopyReplaceVertices(edge21,V21F,V);
        if (boxes.IsBound(edge21))
          boxes.Bind(NewE,boxes.Find(edge21)); // update boxes
        myContext->Replace(edge21,NewE);
        sewd->Set(NewE,num21);
      }
      if(V22F.IsSame(V2L)) {
        NewE = sbe.CopyReplaceVertices(edge22,V1L,V22L);
        //NewE = sbe.CopyReplaceVertices(edge22,V,V22L);
        if (boxes.IsBound(edge22))
          boxes.Bind(NewE,boxes.Find(edge22)); // update boxes
        myContext->Replace(edge22,NewE);
        sewd->Set(NewE,num22);
      }
      if(V22L.IsSame(V2L)) {
        NewE = sbe.CopyReplaceVertices(edge22,V22F,V1L);
        //NewE = sbe.CopyReplaceVertices(edge22,V22F,V);
        if (boxes.IsBound(edge22))
          boxes.Bind(NewE,boxes.Find(edge22)); // update boxes
        myContext->Replace(edge22,NewE);
        sewd->Set(NewE,num22);
      }
      res = Standard_True;
    }
  }

  return res;
}


//=======================================================================
//function : CreateBoxes2d
//purpose  : auxilary
//=======================================================================
static Bnd_Box2d CreateBoxes2d(const Handle(ShapeExtend_WireData)& sewd, 
                                      const TopoDS_Face& face,
                                      ShapeFix_DataMapOfShapeBox2d& boxes) 
{
  // create box2d for edges from wire
  Bnd_Box2d aTotalBox;
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(face,L);
  Handle(Geom2d_Curve) c2d;
  Standard_Real cf,cl;
  ShapeAnalysis_Edge sae;
  for(Standard_Integer i=1; i<=sewd->NbEdges(); i++){
    TopoDS_Edge E = sewd->Edge(i);
    if(sae.PCurve(E,S,L,c2d,cf,cl,Standard_False)) {
      Bnd_Box2d box;
      Geom2dAdaptor_Curve gac;
      Standard_Real aFirst = c2d->FirstParameter();
      Standard_Real aLast = c2d->LastParameter();
      if(c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) 
         && (cf < aFirst || cl > aLast)) {
        //pdn avoiding problems with segment in Bnd_Box
        gac.Load(c2d);
      }
      else
        gac.Load(c2d,cf,cl);
      BndLib_Add2dCurve::Add(gac,::Precision::Confusion(),box);
      boxes.Bind(E,box);
      aTotalBox.Add (box);
    }
  }
  return aTotalBox;
}


//=======================================================================
//function : SelectIntPnt
//purpose  : auxilary
//=======================================================================
static void SelectIntPnt(const Geom2dInt_GInter& Inter,
                         IntRes2d_IntersectionPoint& IP,
                         IntRes2d_Transition& Tr1,
                         IntRes2d_Transition& Tr2)
{
  IP = Inter.Point(1);
  Tr1 = IP.TransitionOfFirst();
  Tr2 = IP.TransitionOfSecond();
  if(Inter.NbPoints()==2) {
    // possible second point is better?
    Standard_Integer status1=0,status2=0;
    if(Tr1.PositionOnCurve()==IntRes2d_Middle) status1+=1;
    if(Tr2.PositionOnCurve()==IntRes2d_Middle) status1+=2;
    IntRes2d_IntersectionPoint IP2;
    IntRes2d_Transition Tr12, Tr22;
    IP2 = Inter.Point(2);
    Tr12 = IP2.TransitionOfFirst();
    Tr22 = IP2.TransitionOfSecond();
    if(Tr12.PositionOnCurve()==IntRes2d_Middle) status2+=1;
    if(Tr22.PositionOnCurve()==IntRes2d_Middle) status2+=2;
    if(status2>status1) {
      IP=IP2; Tr1=Tr12; Tr2=Tr22;
    }
  }
}


//=======================================================================
//function : FindVertAndSplitEdge
//purpose  : auxilary
//=======================================================================
Standard_Boolean ShapeFix_IntersectionTool::FindVertAndSplitEdge
           (const Standard_Real param1,
            const TopoDS_Edge& edge1, const TopoDS_Edge& edge2,
            const Handle(Geom2d_Curve)& Crv1,
            Standard_Real& MaxTolVert,
            Standard_Integer& num1,
            const Handle(ShapeExtend_WireData)& sewd, 
            const TopoDS_Face& face,
            ShapeFix_DataMapOfShapeBox2d& boxes,
            const Standard_Boolean aTmpKey) const
{
  // find needed vertex from edge2 and split edge1 using it
  ShapeAnalysis_Edge sae;
  Handle(ShapeAnalysis_Surface) sas = new ShapeAnalysis_Surface(BRep_Tool::Surface(face));
  gp_Pnt pi1 = GetPointOnEdge(edge1,sas,Crv1,param1);
  BRep_Builder B;
  TopoDS_Vertex V;
  Standard_Real tolV;
  TopoDS_Vertex V1 = sae.FirstVertex(edge2);
  gp_Pnt PV1 = BRep_Tool::Pnt(V1);
  TopoDS_Vertex V2 = sae.LastVertex(edge2);
  gp_Pnt PV2 = BRep_Tool::Pnt(V2);
  TopoDS_Vertex V11 = sae.FirstVertex(edge1);
  TopoDS_Vertex V12 = sae.LastVertex(edge1);
  Standard_Boolean NeedSplit = Standard_True;
  if(pi1.Distance(PV1)<pi1.Distance(PV2)) {
    if( V1.IsSame(V11) || V1.IsSame(V12) ) {
      NeedSplit = Standard_False;
    }
    V = V1;
    tolV = Max( (pi1.Distance(PV1)/2)*1.00001, BRep_Tool::Tolerance(V1) );
  }
  else {
    if( V2.IsSame(V11) || V2.IsSame(V12) ) {
      NeedSplit = Standard_False;
    }
    V = V2;
    tolV = Max( (pi1.Distance(PV2)/2)*1.00001, BRep_Tool::Tolerance(V2) );
  }
  if( NeedSplit || aTmpKey ) {
    if(SplitEdge1(sewd, face, num1, param1, V, tolV, boxes)) {
      B.UpdateVertex(V,tolV);
      MaxTolVert = Max(MaxTolVert,tolV);
//      NbSplit++;
      num1--;
      return Standard_True;
      //break;
    }
  }
  return Standard_False;
}


//=======================================================================
//function : FixSelfIntersectWire
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_IntersectionTool::FixSelfIntersectWire
  (Handle(ShapeExtend_WireData)& sewd, const TopoDS_Face& face,
   Standard_Integer& NbSplit, Standard_Integer& NbCut,
   Standard_Integer& NbRemoved) const
{
  if(myContext.IsNull() || face.IsNull()) return Standard_False;

  //Standard_Real area2d = ShapeAnalysis::TotCross2D(sewd,face);
  //if(area2d<Precision::PConfusion()*Precision::PConfusion()) return Standard_False; //gka 06.09.04 BUG 6555

  TopoDS_Shape SF = Context()->Apply(face);
  Standard_Real MaxTolVert=0.0;
  for(TopExp_Explorer exp(SF,TopAbs_VERTEX); exp.More(); exp.Next()) { 
    Standard_Real tolV = BRep_Tool::Tolerance(TopoDS::Vertex(exp.Current()));
    MaxTolVert = Max(MaxTolVert,tolV);
  }
  MaxTolVert = Min(MaxTolVert,myMaxTol);
  ShapeAnalysis_Edge sae;

  // step 1 : intersection of adjacent edges

  // step 2 : intersection of non-adjacent edges
  ShapeFix_DataMapOfShapeBox2d boxes;
  (void)CreateBoxes2d(sewd,face,boxes);
  Handle(ShapeAnalysis_Surface) sas = new ShapeAnalysis_Surface(BRep_Tool::Surface(face));

  NbSplit=0;
  NbCut=0;
  Standard_Integer nbReplaced =0;
  Standard_Boolean isDone = Standard_False;
  for(Standard_Integer num1=1; num1<sewd->NbEdges() && NbSplit<30; num1++) {
    // for each edge from first wire
    for(Standard_Integer num2=num1+2; num2<=sewd->NbEdges() && NbSplit<30; num2++) {
      // for each edge from second wire
      if( num1==1 && num2==sewd->NbEdges() ) continue;
      TopoDS_Edge edge1 = sewd->Edge(num1);
      TopoDS_Edge edge2 = sewd->Edge(num2);
      if(edge1.IsSame(edge2)) continue;
      if( BRep_Tool::Degenerated(edge1) || BRep_Tool::Degenerated(edge2) ) continue;
      if( !boxes.IsBound(edge1) || !boxes.IsBound(edge2) ) continue;
      Bnd_Box2d B1 = boxes.Find(edge1);
      Bnd_Box2d B2 = boxes.Find(edge2);
      if(!B1.IsOut(B2)) {
        // intersection is possible...
        Standard_Real a1, b1, a2, b2;
        Handle(Geom2d_Curve) Crv1, Crv2;
        if( !sae.PCurve(edge1, face, Crv1, a1, b1, Standard_False) ) return Standard_False;
        if( !sae.PCurve(edge2, face, Crv2, a2, b2, Standard_False) ) return Standard_False;
        Standard_Real tolint = 1.0e-10; 
        Geom2dAdaptor_Curve C1(Crv1), C2(Crv2);
        IntRes2d_Domain d1(C1.Value(a1),a1,tolint,C1.Value(b1),b1,tolint);
        IntRes2d_Domain d2(C2.Value(a2),a2,tolint,C2.Value(b2),b2,tolint);
        Geom2dInt_GInter Inter;
        Inter.Perform( C1, d1, C2, d2, tolint, tolint );
        if(!Inter.IsDone()) continue;
        // intersection is point
        if( Inter.NbPoints()>0 && Inter.NbPoints()<3 ) {
          IntRes2d_IntersectionPoint IP;
          IntRes2d_Transition Tr1, Tr2;
          SelectIntPnt(Inter,IP,Tr1,Tr2);
          if( Tr1.PositionOnCurve() == IntRes2d_Middle &&
             Tr2.PositionOnCurve() == IntRes2d_Middle ) {
            Standard_Real param1 = IP.ParamOnFirst(); 
            Standard_Real param2 = IP.ParamOnSecond();
            gp_Pnt pi1 = GetPointOnEdge(edge1,sas,C1,param1);
            gp_Pnt pi2 = GetPointOnEdge(edge2,sas,C2,param2);
            BRep_Builder B;
            TopoDS_Vertex V;
            Standard_Real tolV=0;
            // analysis for edge1
            Standard_Boolean ModifE1 = Standard_False;
            TopoDS_Vertex VF1 = sae.FirstVertex(edge1);
            gp_Pnt PVF1 = BRep_Tool::Pnt(VF1);
            TopoDS_Vertex VL1 = sae.LastVertex(edge1);
            gp_Pnt PVL1 = BRep_Tool::Pnt(VL1);
            Standard_Real dist1 = pi1.Distance(PVF1);
            Standard_Real dist2 = pi1.Distance(PVL1);
            Standard_Real distmin = Min(dist1, dist2);
            if( dist1 != dist2 && distmin < MaxTolVert ) {
              if (dist1 < dist2) {
                tolV = Max( dist1*1.00001, BRep_Tool::Tolerance(VF1) );
                B.UpdateVertex(VF1,tolV);
                V = VF1;
              } else {
                tolV = Max( dist2*1.00001, BRep_Tool::Tolerance(VL1) );
                B.UpdateVertex(VL1,tolV);
                V = VL1;
              }
              
              Standard_Real dista = Abs(a1-param1);
              Standard_Real distb = Abs(b1-param1);
              Standard_Boolean IsCutLine;
              ModifE1 = CutEdge(edge1, (( dista > distb ) ? a1 : b1 ), param1, face, IsCutLine);
              if (ModifE1)
                NbCut++;
              //not needed split edge, if one of parts is too small
              ModifE1 = ModifE1 || distmin < Precision::Confusion();
            }
            // analysis for edge2
            Standard_Boolean ModifE2 = Standard_False;
            TopoDS_Vertex VF2 = sae.FirstVertex(edge2);
            gp_Pnt PVF2 = BRep_Tool::Pnt(VF2);
            TopoDS_Vertex VL2 = sae.LastVertex(edge2);
            gp_Pnt PVL2 = BRep_Tool::Pnt(VL2);
            dist1 = pi2.Distance(PVF2);
            dist2 = pi2.Distance(PVL2);
            distmin = Min(dist1, dist2);
            if( dist1 != dist2 && distmin < MaxTolVert ) {
              if (dist1 < dist2) {
                tolV = Max( dist1*1.00001, BRep_Tool::Tolerance(VF2) );
                B.UpdateVertex(VF2,tolV);
                V = VF2;
              } else {
                tolV = Max( dist2*1.00001, BRep_Tool::Tolerance(VL2) );
                B.UpdateVertex(VL2,tolV);
                V = VL2;
              }

              Standard_Real dista = Abs(a2-param2);
              Standard_Real distb = Abs(b2-param2);
              Standard_Boolean IsCutLine;
              ModifE2 = CutEdge(edge2, (( dista > distb ) ? a2 : b2 ), param2, face, IsCutLine);
              if (ModifE2)
                NbCut++;
              //not needed split edge, if one of parts is too small
              ModifE2 = ModifE2 || distmin < Precision::Confusion();
            }
            if( ModifE1 && !ModifE2 ) {
              if(SplitEdge1(sewd, face, num2, param2, V, tolV, boxes)) {
                NbSplit++;
                num2--; 
                continue;
                
              }
            }
            if( !ModifE1 && ModifE2 ) {
              if(SplitEdge1(sewd, face, num1, param1, V, tolV, boxes)) {
                NbSplit++;
                num1--;
                break;
              }
            }
            if( !ModifE1 && !ModifE2 ) {
              gp_Pnt P0( (pi1.X()+pi2.X())/2, (pi1.Y()+pi2.Y())/2, (pi1.Z()+pi2.Z())/2 );
              tolV = Max( (pi1.Distance(pi2)/2)*1.00001, Precision::Confusion() );
              B.MakeVertex(V,P0,tolV);
              MaxTolVert = Max(MaxTolVert,tolV);
              Standard_Boolean isEdgeSplit2 = SplitEdge1(sewd, face, num2, param2,
                                                         V, tolV, boxes);
              if(isEdgeSplit2) {
                NbSplit++;
                num2--;
              }
              if(SplitEdge1(sewd, face, num1, param1, V, tolV, boxes)) {
                NbSplit++;
                num1--;
                break;
              }
              if(isEdgeSplit2)
                continue;
            }
          }
          if( Tr1.PositionOnCurve() == IntRes2d_Middle &&
             Tr2.PositionOnCurve() != IntRes2d_Middle ) {
            // find needed vertex from edge2 and split edge1 using it
            Standard_Real param1 = IP.ParamOnFirst(); 
            if(FindVertAndSplitEdge(param1, edge1, edge2, Crv1, MaxTolVert,
                                    num1, sewd, face, boxes, Standard_False) ) {
              NbSplit++;
              break;
            }
          }
          if( Tr1.PositionOnCurve() != IntRes2d_Middle &&
             Tr2.PositionOnCurve() == IntRes2d_Middle ) {
            // find needed vertex from edge1 and split edge2 using it
            Standard_Real param2 = IP.ParamOnSecond(); 
            if(FindVertAndSplitEdge(param2, edge2, edge1, Crv2, MaxTolVert,
                                    num2, sewd, face, boxes, Standard_False) ) {
              NbSplit++;
              continue;
            }
          }
          if( Tr1.PositionOnCurve() != IntRes2d_Middle &&
             Tr2.PositionOnCurve() != IntRes2d_Middle ) {
            // union vertexes
            if( UnionVertexes(sewd, edge1, edge2, num2, boxes, B2) )
              nbReplaced ++; //gka 06.09.04
          }
        }
        // intersection is segment
        if( Inter.NbSegments()==1 ) {
          IntRes2d_IntersectionSegment IS = Inter.Segment(1);
          if ( IS.HasFirstPoint() && IS.HasLastPoint() ) {
            Standard_Boolean IsModified1 = Standard_False;
            Standard_Boolean IsModified2 = Standard_False;
            TopoDS_Vertex NewV;
            BRep_Builder B;
            Standard_Real newtol=0.0;
            IntRes2d_IntersectionPoint IPF = IS.FirstPoint();
            Standard_Real p11 = IPF.ParamOnFirst();
            Standard_Real p21 = IPF.ParamOnSecond();
            IntRes2d_IntersectionPoint IPL = IS.LastPoint();
            Standard_Real p12 = IPL.ParamOnFirst();
            Standard_Real p22 = IPL.ParamOnSecond();
            gp_Pnt Pnt11 = GetPointOnEdge(edge1,sas,C1,p11);
            gp_Pnt Pnt12 = GetPointOnEdge(edge1,sas,C1,p12);
            gp_Pnt Pnt21 = GetPointOnEdge(edge2,sas,C2,p21);
            gp_Pnt Pnt22 = GetPointOnEdge(edge2,sas,C2,p22);
            // next string commented by skl 29.12.2004 for OCC7624
            //if( Pnt11.Distance(Pnt21)>myPreci || Pnt12.Distance(Pnt22)>myPreci ) continue;
            if( Pnt11.Distance(Pnt21)>MaxTolVert || Pnt12.Distance(Pnt22)>MaxTolVert ) continue;
            // analysis for edge1
            TopoDS_Vertex V1 = sae.FirstVertex(edge1);
            gp_Pnt PV1 = BRep_Tool::Pnt(V1);
            TopoDS_Vertex V2 = sae.LastVertex(edge1);
            gp_Pnt PV2 = BRep_Tool::Pnt(V2);
            //Standard_Real tol1 = BRep_Tool::Tolerance(V1);
            //Standard_Real tol2 = BRep_Tool::Tolerance(V2);
            //Standard_Real maxtol = Max(tol1,tol2);
            Standard_Real dist1 = Pnt11.Distance(PV1);
            Standard_Real dist2 = Pnt12.Distance(PV1);
            Standard_Real maxdist = Max(dist1,dist2);
            Standard_Real pdist;
            if(edge1.Orientation()==TopAbs_REVERSED)
              pdist = Max(Abs(b1-p11),Abs(b1-p12));
            else
              pdist = Max(Abs(a1-p11),Abs(a1-p12));
            if(maxdist<MaxTolVert || pdist<Abs(b1-a1)*0.01) {
            //if(maxdist<maxtol || pdist<Abs(b1-a1)*0.01) {
              newtol = maxdist;
              NewV = V1;
              IsModified1 = Standard_True;
            }
            dist1 = Pnt11.Distance(PV2);
            dist2 = Pnt12.Distance(PV2);
            maxdist = Max(dist1,dist2);
            if(edge1.Orientation()==TopAbs_REVERSED)
              pdist = Max(Abs(a1-p11),Abs(a1-p12));
            else
              pdist = Max(Abs(b1-p11),Abs(b1-p12));
            //if(maxdist<maxtol || pdist<Abs(b1-a1)*0.01) {
            if(maxdist<MaxTolVert || pdist<Abs(b1-a1)*0.01) {
              if( ( IsModified1 && maxdist<newtol ) || !IsModified1 ) {
                newtol = maxdist;
                NewV = V2;
                IsModified1 = Standard_True;
              }
            }
            if(IsModified1) {
              // cut edge1 and update tolerance NewV
              Standard_Real dista = Abs(a1-p11)+Abs(a1-p12);
              Standard_Real distb = Abs(b1-p11)+Abs(b1-p12);
              Standard_Real pend,cut;
              if(dista>distb) pend=a1;
              else pend=b1;
              if(Abs(pend-p11)>Abs(pend-p12)) cut=p12;
              else cut=p11;
              Standard_Boolean IsCutLine;
              if (CutEdge(edge1, pend, cut, face, IsCutLine))
                NbCut++;
              if(newtol>BRep_Tool::Tolerance(NewV)) {
                B.UpdateVertex(NewV,newtol);
              }
              else newtol = BRep_Tool::Tolerance(NewV);
            }
            // analysis for edge2
            TopoDS_Vertex V12 = sae.FirstVertex(edge2);
            gp_Pnt PV12 = BRep_Tool::Pnt(V12);
            TopoDS_Vertex V22 = sae.LastVertex(edge2);
            gp_Pnt PV22 = BRep_Tool::Pnt(V22);
            //tol1 = BRep_Tool::Tolerance(V1);
            //tol2 = BRep_Tool::Tolerance(V2);
            //maxtol = Max(tol1,tol2);
            dist1 = Pnt21.Distance(PV12);
            dist2 = Pnt22.Distance(PV12);
            maxdist = Max(dist1,dist2);
            if(edge2.Orientation()==TopAbs_REVERSED)
              pdist = Max(Abs(b2-p21),Abs(b2-p22));
            else
              pdist = Max(Abs(a2-p21),Abs(a2-p22));
            //if(maxdist<maxtol || pdist<Abs(b2-a2)*0.01) {
            if(maxdist<MaxTolVert || pdist<Abs(b2-a2)*0.01) {
              newtol = maxdist;
              NewV = V12;
              IsModified2 = Standard_True;
            }
            dist1 = Pnt21.Distance(PV22);
            dist2 = Pnt22.Distance(PV22);
            maxdist = Max(dist1,dist2);
            if(edge2.Orientation()==TopAbs_REVERSED)
              pdist = Max(Abs(a2-p21),Abs(a2-p22));
            else
              pdist = Max(Abs(b2-p21),Abs(b2-p22));
            //if(maxdist<maxtol || pdist<Abs(b2-a2)*0.01) {
            if(maxdist<MaxTolVert || pdist<Abs(b2-a2)*0.01) {
              if( ( IsModified2 && maxdist<newtol ) || !IsModified2 ) {
                newtol = maxdist;
                NewV = V22;
                IsModified2 = Standard_True;
              }
            }
            if(IsModified2) {
              // cut edge1 and update tolerance NewV
              Standard_Real dista = Abs(a2-p21)+Abs(a2-p22);
              Standard_Real distb = Abs(b2-p21)+Abs(b2-p22);
              Standard_Real pend,cut;
              if(dista>distb) pend=a2;
              else pend=b2;
              if(Abs(pend-p21)>Abs(pend-p22)) cut=p22;
              else cut=p21;
              Standard_Boolean IsCutLine;
              if (CutEdge(edge2, pend, cut, face, IsCutLine))
                NbCut++;
              if(newtol>BRep_Tool::Tolerance(NewV)) {
                B.UpdateVertex(NewV,newtol);
              }
              else newtol = BRep_Tool::Tolerance(NewV);
            }

            if( IsModified1 && !IsModified2 ) {
              if(SplitEdge2(sewd, face, num2, p21, p22, NewV, newtol, boxes)) {
                NbSplit++;
                num2--;
                continue;
              }
            }
            if( !IsModified1 && IsModified2 ) {
              if(SplitEdge2(sewd, face, num1, p11, p12, NewV, newtol, boxes)) {
                NbSplit++;
                num1--;
                break;
              }
            }
            if( !IsModified1 && !IsModified2 ) {
              Standard_Real param1 = (p11+p12)/2;
              Standard_Real param2 = (p21+p22)/2;
              gp_Pnt Pnt10 = GetPointOnEdge(edge1,sas,C1,param1);
              gp_Pnt Pnt20 = GetPointOnEdge(edge2,sas,C2,param2);
              gp_Pnt P0( (Pnt10.X()+Pnt20.X())/2, (Pnt10.Y()+Pnt20.Y())/2,
                         (Pnt10.Z()+Pnt20.Z())/2 );
              dist1 = Max(Pnt11.Distance(P0),Pnt12.Distance(P0));
              dist2 = Max(Pnt21.Distance(P0),Pnt22.Distance(P0));
              Standard_Real tolV = Max(dist1,dist2);
              tolV = Max(tolV,Pnt10.Distance(Pnt20))*1.00001;
              Standard_Boolean FixSegment = Standard_True;
              if(tolV<MaxTolVert) {
                // create new vertex and split each intersecting edge on two edges
                B.MakeVertex(NewV,Pnt10,tolV);
                if(SplitEdge2(sewd, face, num2, p21, p22, NewV, tolV, boxes)) {
                  NbSplit++;
                  num2--;
                }
                if(SplitEdge2(sewd, face, num1, p11, p12, NewV, tolV, boxes)) {
                  NbSplit++;
                  num1--;
                  break;
                }
              }
              else if(FixSegment) {
                //if( Abs(p12-p11)>Abs(b1-a1)/2 || Abs(p22-p21)>Abs(b2-a2)/2 ) {
                // segment is big and we have to split each intersecting edge
                // on 3 edges --> middle edge - edge based on segment
                // after we can remove edges maked from segment
                gp_Pnt P01( (Pnt11.X()+Pnt21.X())/2, (Pnt11.Y()+Pnt21.Y())/2,
                            (Pnt11.Z()+Pnt21.Z())/2 );
                gp_Pnt P02( (Pnt12.X()+Pnt22.X())/2, (Pnt12.Y()+Pnt22.Y())/2,
                            (Pnt12.Z()+Pnt22.Z())/2 );
                Standard_Real tolV1 = Max(Pnt11.Distance(P01),Pnt21.Distance(P01));
                tolV1 = Max(tolV1,Precision::Confusion())*1.00001;
                Standard_Real tolV2 = Max(Pnt12.Distance(P02),Pnt22.Distance(P02));
                tolV2 = Max(tolV2,Precision::Confusion())*1.00001;
                if( tolV1>MaxTolVert || tolV2>MaxTolVert ) continue; 
                TopoDS_Vertex NewV1,NewV2;
/*
                // parameters for splitting: a1..p11..p12..b1 and a2..p21..p22..b2 ???
                Standard_Integer nbseg1=3, nbseg2=3, kv1=0, kv2=0;
                if(P01.Distance(PV1)<Max(tolV1,BRep_Tool::Tolerance(V1))) {
                  NewV1 = V1;
                  if(tolV1>BRep_Tool::Tolerance(V1))
                    B.UpdateVertex(NewV1,tolV1);
                  //akey1++;
                  nbseg1--;
                  kv1 = 1;
                }
                else if(P01.Distance(PV2)<Max(tolV1,BRep_Tool::Tolerance(V2))) {
                  NewV1 = V2;
                  if(tolV1>BRep_Tool::Tolerance(V2))
                    B.UpdateVertex(NewV1,tolV1);
                  //akey1++;
                  nbseg1--;
                  kv1 = 1;
                }
*/
                TopoDS_Edge tmpE,SegE;
                // split edge1
                Standard_Integer akey1=0, akey2=0;
                Standard_Real newTolerance;
                // analysis fo P01
                newTolerance = Max(tolV1,BRep_Tool::Tolerance(V1));
                if(P01.Distance(PV1)<newTolerance) {
                  B.MakeVertex(NewV1,BRep_Tool::Pnt(V1),newTolerance);
                  NewV1.Orientation(V1.Orientation());
                  akey1++;
                }
                newTolerance = Max(tolV1,BRep_Tool::Tolerance(V2));
                if(P01.Distance(PV2)<newTolerance) {
                  B.MakeVertex(NewV1,BRep_Tool::Pnt(V2),newTolerance);
                  NewV1.Orientation(V2.Orientation());
                  akey1++;
                }
                // analysis fo P02
                newTolerance = Max(tolV2,BRep_Tool::Tolerance(V1));
                if(P02.Distance(PV1)<newTolerance) {
                  B.MakeVertex(NewV2,BRep_Tool::Pnt(V1),newTolerance);
                  NewV2.Orientation(V1.Orientation());
                  akey2++;
                }
                newTolerance = Max(tolV2,BRep_Tool::Tolerance(V2));
                if(P02.Distance(PV2)<newTolerance) {
                  B.MakeVertex(NewV2,BRep_Tool::Pnt(V2),newTolerance);
                  NewV2.Orientation(V2.Orientation());
                  akey2++;
                }
                if( akey1>1 || akey2>1 ) continue;
                Standard_Integer dnum1=0, numseg1=num1;
                // prepare vertices
                if(akey1==0) B.MakeVertex(NewV1,P01,tolV1);
                if(akey2==0) B.MakeVertex(NewV2,P02,tolV2);
                // split
                if( akey1==0 && akey2>0 ) {
                  if(SplitEdge1(sewd, face, num1, p11, NewV1, tolV1, boxes)) {
                    NbSplit++;
                    dnum1=1;
                    numseg1=num1+1;
                  }
                }
                if( akey1>0 && akey2==0 ) {
                  if(SplitEdge1(sewd, face, num1, p12, NewV2, tolV2, boxes) ) {
                    NbSplit++;
                    dnum1=1;
                    numseg1=num1;
                  }
                }
                if( akey1==0 && akey2==0 ) {
                  if(SplitEdge1(sewd, face, num1, p11, NewV1, tolV1, boxes)) {
                    NbSplit++;
                    dnum1=1;
                  }
                  tmpE = sewd->Edge(num1);
                  Standard_Real a,b;
                  Handle(Geom2d_Curve) c2d;
                  sae.PCurve(tmpE,face,c2d,a,b,Standard_False);
                  if( (a-p12)*(b-p12)>0 ) { // p12 - external for [a,b] => split next edge
                    if(SplitEdge1(sewd, face, num1+1, p12, NewV2, tolV2, boxes) ) {
                      NbSplit++;
                      dnum1++;
                      numseg1=num1+1;
                    }
                  }
                  else {
                    if(SplitEdge1(sewd, face, num1, p12, NewV2, tolV2, boxes) ) {
                      NbSplit++;
                      dnum1++;
                      numseg1=num1+1;
                    }
                  }
                }
                //SegE = sewd->Edge(numseg1); // get edge from segment
                // split edge2
                // replace vertices if it is necessary
                ShapeBuild_Edge sbe;
                akey1=0, akey2=0;

                if(P01.Distance(PV12)<tolV1) {
                  tolV1 += P01.Distance(PV12);
                  B.UpdateVertex(NewV1,tolV1);
                  if(V12.Orientation()==NewV1.Orientation()) {
                    myContext->Replace(V12,NewV1);
                    V12 = NewV1;
                  }
                  else {
                    myContext->Replace(V12,NewV1.Reversed());
                    V12 = TopoDS::Vertex(NewV1.Reversed());
                  }
                  nbReplaced++; //gka 06.09.04
                  TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,NewV1,V22);
                  myContext->Replace(edge2,NewE);
                  sewd->Set(NewE,num2+dnum1);
                  boxes.Bind(NewE,B2); // update boxes
                  edge2 = NewE;
                  akey1 = 1;
                }
                if(P01.Distance(PV22)<tolV1) {
                  tolV1 += P01.Distance(PV22);
                  B.UpdateVertex(NewV1,tolV1);
                  if(V22.Orientation()==NewV1.Orientation()) {
                    myContext->Replace(V22,NewV1);
                    V22 = NewV1;
                  }
                  else {
                    myContext->Replace(V22,NewV1.Reversed());
                    V22 = TopoDS::Vertex(NewV1.Reversed());
                  }
                  nbReplaced++; //gka 06.09.04
                  TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V12,NewV1);
                  myContext->Replace(edge2,NewE);
                  sewd->Set(NewE,num2+dnum1);
                  boxes.Bind(NewE,B2); // update boxes
                  edge2 = NewE;
                  akey1 = 2;
                }
                if(P02.Distance(PV12)<tolV2) {
                  tolV2 += P02.Distance(PV12);
                  B.UpdateVertex(NewV2,tolV2);
                  if(V12.Orientation()==NewV2.Orientation()) {
                    myContext->Replace(V12,NewV2);
                    V12 = NewV2;
                  }
                  else {
                    myContext->Replace(V12,NewV2.Reversed());
                    V12 = TopoDS::Vertex(NewV2.Reversed());
                  }
                  nbReplaced++; //gka 06.09.04
                  TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,NewV2,V22);
                  myContext->Replace(edge2,NewE);
                  sewd->Set(NewE,num2+dnum1);
                  boxes.Bind(NewE,B2); // update boxes
                  edge2 = NewE;
                  akey2 = 1;
                }
                if(P02.Distance(PV22)<tolV2) {
                  tolV2 += P02.Distance(PV22);
                  B.UpdateVertex(NewV2,tolV2);
                  if(V22.Orientation()==NewV2.Orientation()) {
                    myContext->Replace(V22,NewV2);
                    V22 = NewV2;
                  }
                  else {
                    myContext->Replace(V22,NewV2.Reversed());
                    V22 = TopoDS::Vertex(NewV2.Reversed());
                  }
                  nbReplaced++; //gka 06.09.04
                  TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V12,NewV2);
                  myContext->Replace(edge2,NewE);
                  sewd->Set(NewE,num2+dnum1);
                  boxes.Bind(NewE,B2); // update boxes
                  edge2 = NewE;
                  akey2 = 2;
                }
                Standard_Integer dnum2=0, numseg2=num2+dnum1;
                // split
                if( akey1==0 && akey2>0 ) {
                  if(SplitEdge1(sewd, face, num2+dnum1, p21, NewV1, tolV1, boxes)) {
                    NbSplit++;
                    dnum2=1;
                    //numseg2=num2+dnum1+1;
                    numseg2=num2+dnum1;
                  }
                }
                if( akey1>0 && akey2==0 ) {
                  if(SplitEdge1(sewd, face, num2+dnum1, p22, NewV2, tolV2, boxes)) {
                    NbSplit++;
                    dnum2=1;
                    //numseg2=num2+dnum1;
                    numseg2=num2+dnum1+1;
                  }
                }
                if( akey1==0 && akey2==0 ) {
                  if(SplitEdge1(sewd, face, num2+dnum1, p21, NewV1, tolV1, boxes)) {
                    NbSplit++;
                    dnum2=1;
                  }
                  tmpE = sewd->Edge(num2+dnum1);
                  Standard_Real a,b;
                  Handle(Geom2d_Curve) c2d;
                  sae.PCurve(tmpE,face,c2d,a,b,Standard_False);
                  if( (a-p22)*(b-p22)>0 ) { // p22 - external for [a,b] => split next edge
                    if(SplitEdge1(sewd, face, num2+dnum1+dnum2, p22, NewV2, tolV2, boxes) ) {
                      NbSplit++;
                      numseg2=num2+dnum1+dnum2;
                      dnum2++;
                    }
                  }
                  else {
                    if(SplitEdge1(sewd, face, num2+dnum1, p22, NewV2, tolV2, boxes) ) {
                      NbSplit++;
                      dnum2++;
                      numseg2=num2+dnum1+1;
                    }
                  }
                }
                // remove segment
                sewd->Remove(numseg2);
                sewd->Remove(numseg1);
                NbRemoved = NbRemoved+2;
                //num1--;
                //break;
              }
            }
          }
        }
      }
    }
  }
  isDone = (NbSplit || NbCut || nbReplaced || NbRemoved);
  return isDone;
}


//=======================================================================
//function : FixIntersectingWires
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_IntersectionTool::FixIntersectingWires
  (TopoDS_Face& face) const
{
  if(myContext.IsNull() || face.IsNull()) return Standard_False;
  //TopoDS_Shape S = context->Apply(face);
  //TopoDS_Shape SF = TopoDS::Face(S);
  TopoDS_Shape SF = face;
  TopAbs_Orientation ori = face.Orientation();
  TopTools_SequenceOfShape SeqWir;
  TopTools_SequenceOfShape SeqNMShapes;
  for( TopoDS_Iterator iter(SF,Standard_False); iter.More(); iter.Next() ) { 
    if(iter.Value().ShapeType() != TopAbs_WIRE ||
       (iter.Value().Orientation() != TopAbs_FORWARD && 
        iter.Value().Orientation() != TopAbs_REVERSED)) {
      SeqNMShapes.Append(iter.Value());
      continue;
    }
    TopoDS_Wire wire = TopoDS::Wire(iter.Value());
    SeqWir.Append(wire);
  }
  if(SeqWir.Length()<2) return Standard_False;//gka 06.09.04

  Standard_Real MaxTolVert=0.0;
  for(TopExp_Explorer exp(SF,TopAbs_VERTEX); exp.More(); exp.Next()) { 
    Standard_Real tolV = BRep_Tool::Tolerance(TopoDS::Vertex(exp.Current()));
    MaxTolVert = Max(MaxTolVert,tolV);
  }
  Standard_Boolean isDone = Standard_False; //gka 06.09.04
  ShapeAnalysis_Edge sae;
  Handle(ShapeAnalysis_Surface) sas = new ShapeAnalysis_Surface (BRep_Tool::Surface (face));

  // precompute edge boxes for all wires
  NCollection_Sequence<ShapeFix_DataMapOfShapeBox2d> aSeqWirEdgeBoxes;
  NCollection_Sequence<Bnd_Box2d> aSeqWirBoxes;
  for (Standard_Integer n = 1; n <= SeqWir.Length(); n++)
  {
    const TopoDS_Wire& aWire = TopoDS::Wire (SeqWir.Value (n));
    Handle(ShapeExtend_WireData) aSewd = new ShapeExtend_WireData (aWire);
    ShapeFix_DataMapOfShapeBox2d aBoxes;
    Bnd_Box2d aTotalBox = CreateBoxes2d (aSewd, face, aBoxes);
    aSeqWirEdgeBoxes.Append (aBoxes);
    aSeqWirBoxes.Append (aTotalBox);
  }

  for(Standard_Integer n1=1; n1<=SeqWir.Length()-1; n1++) { 
    TopoDS_Wire wire1 = TopoDS::Wire(SeqWir.Value(n1));
    Handle(ShapeExtend_WireData) sewd1 = new ShapeExtend_WireData(wire1);
    ShapeFix_DataMapOfShapeBox2d& boxes1 = aSeqWirEdgeBoxes.ChangeValue (n1);
    Bnd_Box2d aBox1 = aSeqWirBoxes (n1);
    for(Standard_Integer n2=n1+1; n2<=SeqWir.Length(); n2++) { 
      TopoDS_Wire wire2 = TopoDS::Wire(SeqWir.Value(n2));
      Handle(ShapeExtend_WireData) sewd2 = new ShapeExtend_WireData(wire2);
      ShapeFix_DataMapOfShapeBox2d& boxes2 = aSeqWirEdgeBoxes.ChangeValue (n2);
      Bnd_Box2d aBox2 = aSeqWirBoxes (n2);
      if (!aBox1.IsVoid() && !aBox2.IsVoid() && aBox1.IsOut (aBox2))
      {
        continue;
      }
      // detect possible intersections:
      Standard_Integer NbModif=0;
      Standard_Integer nbReplaced =0;//gka 06.09.04
      Standard_Boolean hasModifWire = Standard_False; //gka 06.09.04
      for(Standard_Integer num1 = 1; num1<=sewd1->NbEdges() && NbModif<30; num1++) {
        // for each edge from first wire
        TopoDS_Edge edge1 = sewd1->Edge(num1); //gka 06.09.04

        for(Standard_Integer num2 = 1; num2<=sewd2->NbEdges() && NbModif<30; num2++) {
          // for each edge from second wire
          TopoDS_Edge edge2 = sewd2->Edge(num2);
          if(edge1.IsSame(edge2)) continue;
          if( BRep_Tool::Degenerated(edge1) || BRep_Tool::Degenerated(edge2) ) continue;
          if( !boxes1.IsBound(edge1) || !boxes2.IsBound(edge2) ) continue;
          Bnd_Box2d B1 = boxes1.Find(edge1);
          Bnd_Box2d B2 = boxes2.Find(edge2);
          if(!B1.IsOut(B2)) {
            // intersection is possible...
            Standard_Real a1, b1, a2, b2;
            Handle(Geom2d_Curve) Crv1, Crv2;
            if( !sae.PCurve(edge1, face, Crv1, a1, b1, Standard_False) ) 
              continue; //return Standard_False; gka 06.09.04
            if( !sae.PCurve(edge2, face, Crv2, a2, b2, Standard_False) ) 
              continue; //return Standard_False;gka 06.09.04
            Standard_Real tolint = 1.0e-10; 
            Geom2dAdaptor_Curve C1(Crv1), C2(Crv2);
            IntRes2d_Domain d1(C1.Value(a1),a1,tolint,C1.Value(b1),b1,tolint);
            IntRes2d_Domain d2(C2.Value(a2),a2,tolint,C2.Value(b2),b2,tolint);
            Geom2dInt_GInter Inter;
            Inter.Perform( C1, d1, C2, d2, tolint, tolint );
            if(!Inter.IsDone()) continue;
            // intersection is point
            if( Inter.NbPoints()>0 && Inter.NbPoints()<3 ) {
              IntRes2d_IntersectionPoint IP;
              IntRes2d_Transition Tr1, Tr2;
              SelectIntPnt(Inter,IP,Tr1,Tr2);
              if( Tr1.PositionOnCurve() == IntRes2d_Middle &&
                 Tr2.PositionOnCurve() == IntRes2d_Middle ) {
                // create new vertex and split both edges
                Standard_Real param1 = IP.ParamOnFirst(); 
                Standard_Real param2 = IP.ParamOnSecond();
                gp_Pnt pi1 = GetPointOnEdge(edge1,sas,C1,param1);
                gp_Pnt pi2 = GetPointOnEdge(edge2,sas,C2,param2);
                gp_Pnt P0( (pi1.X()+pi2.X())/2, (pi1.Y()+pi2.Y())/2, (pi1.Z()+pi2.Z())/2 );
                BRep_Builder B;
                TopoDS_Vertex V;
                Standard_Real tolV = Max( (pi1.Distance(pi2)/2)*1.00001, Precision::Confusion() );
                B.MakeVertex(V,P0,tolV);
                MaxTolVert = Max(MaxTolVert,tolV);
                Standard_Boolean isSplitEdge2 = SplitEdge1(sewd2, face, num2, param2,
                                                           V, tolV, boxes2);
                if(isSplitEdge2) {
                  NbModif++;
                  num2--;
                }
                if(SplitEdge1(sewd1, face, num1, param1, V, tolV, boxes1)) {
                  NbModif++;
                  num1--;
                  break;
                }
                if(isSplitEdge2)
                  continue;
              }
              if( Tr1.PositionOnCurve() == IntRes2d_Middle &&
                 Tr2.PositionOnCurve() != IntRes2d_Middle ) {
                // find needed vertex from edge2 and split edge1 using it
                Standard_Real param1 = IP.ParamOnFirst(); 
                if(FindVertAndSplitEdge(param1, edge1, edge2, Crv1, MaxTolVert,
                                        num1, sewd1, face, boxes1, Standard_True) ) {
                  NbModif++;
                  break;
                }
              }
              if( Tr1.PositionOnCurve() != IntRes2d_Middle &&
                 Tr2.PositionOnCurve() == IntRes2d_Middle ) {
                // find needed vertex from edge1 and split edge2 using it
                Standard_Real param2 = IP.ParamOnSecond(); 
                if(FindVertAndSplitEdge(param2, edge2, edge1, Crv2, MaxTolVert,
                                        num2, sewd2, face, boxes2, Standard_True) ) {
                  NbModif++;
                  continue;
                }
              }
              if( Tr1.PositionOnCurve() != IntRes2d_Middle &&
                 Tr2.PositionOnCurve() != IntRes2d_Middle ) {
                // union vertexes
                if( UnionVertexes(sewd2, edge1, edge2, num2, boxes2, B2) )
                  nbReplaced ++; //gka 06.09.04
              }
            }
            hasModifWire = (hasModifWire || NbModif || nbReplaced);
            // intersection is segment
            if( Inter.NbSegments()==1 ) {
              IntRes2d_IntersectionSegment IS = Inter.Segment(1);
              if ( IS.HasFirstPoint() && IS.HasLastPoint() ) {
                Standard_Boolean IsModified1 = Standard_False;
                Standard_Boolean IsModified2 = Standard_False;
                TopoDS_Vertex NewV;
                BRep_Builder B;
                Standard_Real newtol=0.0;
                IntRes2d_IntersectionPoint IPF = IS.FirstPoint();
                Standard_Real p11 = IPF.ParamOnFirst();
                Standard_Real p21 = IPF.ParamOnSecond();
                IntRes2d_IntersectionPoint IPL = IS.LastPoint();
                Standard_Real p12 = IPL.ParamOnFirst();
                Standard_Real p22 = IPL.ParamOnSecond();
                gp_Pnt Pnt11 = GetPointOnEdge(edge1,sas,C1,p11);
                gp_Pnt Pnt12 = GetPointOnEdge(edge1,sas,C1,p12);
                gp_Pnt Pnt21 = GetPointOnEdge(edge2,sas,C2,p21);
                gp_Pnt Pnt22 = GetPointOnEdge(edge2,sas,C2,p22);

                // analysis for edge1
                TopoDS_Vertex V1 = sae.FirstVertex(edge1);
                gp_Pnt PV1 = BRep_Tool::Pnt(V1);
                TopoDS_Vertex V2 = sae.LastVertex(edge1);
                gp_Pnt PV2 = BRep_Tool::Pnt(V2);
                Standard_Real dist1 = Pnt11.Distance(PV1);
                Standard_Real dist2 = Pnt12.Distance(PV1);
                Standard_Real maxdist = Max(dist1,dist2);
                Standard_Real pdist;
                if(edge1.Orientation()==TopAbs_REVERSED)
                  pdist = Max(Abs(b1-p11),Abs(b1-p12));
                else
                  pdist = Max(Abs(a1-p11),Abs(a1-p12));
                if(maxdist<MaxTolVert || pdist<Abs(b1-a1)*0.01) {
                  newtol = maxdist;
                  NewV = V1;
                  IsModified1 = Standard_True;
                }
                dist1 = Pnt11.Distance(PV2);
                dist2 = Pnt12.Distance(PV2);
                maxdist = Max(dist1,dist2);
                if(edge1.Orientation()==TopAbs_REVERSED)
                  pdist = Max(Abs(a1-p11),Abs(a1-p12));
                else
                  pdist = Max(Abs(b1-p11),Abs(b1-p12));
                if(maxdist<MaxTolVert || pdist<Abs(b1-a1)*0.01) {
                  if( ( IsModified1 && maxdist<newtol ) || !IsModified1 ) {
                    newtol = maxdist;
                    NewV = V2;
                    IsModified1 = Standard_True;
                  }
                }
                if(IsModified1) {
                  // cut edge1 and update tolerance NewV
                  Standard_Real dista = Abs(a1-p11)+Abs(a1-p12);
                  Standard_Real distb = Abs(b1-p11)+Abs(b1-p12);
                  Standard_Real pend,cut;
                  if(dista>distb) pend=a1;
                  else pend=b1;
                  if(Abs(pend-p11)>Abs(pend-p12)) cut=p12;
                    else cut=p11;
                  Standard_Boolean IsCutLine;
                  if(!CutEdge(edge1, pend, cut, face, IsCutLine))
                  {
                    IsModified1 = Standard_False;
                    continue;
                  }
                  if(newtol>BRep_Tool::Tolerance(NewV)) {
                    B.UpdateVertex(NewV,newtol*1.00001);
                  }
                }

                // analysis for edge2
                TopoDS_Vertex V12 = sae.FirstVertex(edge2);
                gp_Pnt PV12 = BRep_Tool::Pnt(V12);
                TopoDS_Vertex V22 = sae.LastVertex(edge2);
                gp_Pnt PV22 = BRep_Tool::Pnt(V22);
                dist1 = Pnt21.Distance(PV12);
                dist2 = Pnt22.Distance(PV12);
                maxdist = Max(dist1,dist2);
                if(edge2.Orientation()==TopAbs_REVERSED)
                  pdist = Max(Abs(b2-p21),Abs(b2-p22));
                else
                  pdist = Max(Abs(a2-p21),Abs(a2-p22));
                if(maxdist<MaxTolVert || pdist<Abs(b2-a2)*0.01) {
                  newtol = maxdist;
                  NewV = V12;
                  IsModified2 = Standard_True;
                }
                dist1 = Pnt21.Distance(PV22);
                dist2 = Pnt22.Distance(PV22);
                maxdist = Max(dist1,dist2);
                if(edge2.Orientation()==TopAbs_REVERSED)
                  pdist = Max(Abs(a2-p21),Abs(a2-p22));
                else
                  pdist = Max(Abs(b2-p21),Abs(b2-p22));
                if(maxdist<MaxTolVert || pdist<Abs(b2-a2)*0.01) {
                  if( ( IsModified2 && maxdist<newtol ) || !IsModified2 ) {
                    newtol = maxdist;
                    NewV = V22;
                    IsModified2 = Standard_True;
                  }
                }
                if(IsModified2) {
                  // cut edge1 and update tolerance NewV
                  Standard_Real dista = Abs(a2-p21)+Abs(a2-p22);
                  Standard_Real distb = Abs(b2-p21)+Abs(b2-p22);
                  Standard_Real pend,cut;
                  if(dista>distb) pend=a2;
                  else pend=b2;
                  if(Abs(pend-p21)>Abs(pend-p22)) cut=p22;
                  else cut=p21;
                  Standard_Boolean IsCutLine;
                  if(!CutEdge(edge2, pend, cut, face, IsCutLine))
                  {
                    IsModified2 = Standard_False;
                    continue;

                  }
                  if(newtol>BRep_Tool::Tolerance(NewV)) {
                    B.UpdateVertex(NewV,newtol*1.00001);
                  }
                }

                if( IsModified1 || IsModified2 ) {
                  //necessary to make intersect with the same pair of the edges once again with modified ranges
                  num2--;
                  hasModifWire = Standard_True; //gka 06.09.04
                  continue;
                }
                else {
                  // create new vertex and split edge1 and edge2 using it
                  if( Abs(p12-p11)>Abs(b1-a1)/2 || Abs(p22-p21)>Abs(b2-a2)/2 ) {
                    // segment is big and we have to split each intersecting edge
                    // on 3 edges --> middle edge - edge based on segment
                    gp_Pnt P01( (Pnt11.X()+Pnt21.X())/2, (Pnt11.Y()+Pnt21.Y())/2,
                                (Pnt11.Z()+Pnt21.Z())/2 );
                    gp_Pnt P02( (Pnt12.X()+Pnt22.X())/2, (Pnt12.Y()+Pnt22.Y())/2,
                                (Pnt12.Z()+Pnt22.Z())/2 );
                    Standard_Real tolV1 = Max(Pnt11.Distance(P01),Pnt21.Distance(P01));
                    tolV1 = Max(tolV1,Precision::Confusion())*1.00001;
                    Standard_Real tolV2 = Max(Pnt12.Distance(P02),Pnt22.Distance(P02));
                    tolV2 = Max(tolV2,Precision::Confusion())*1.00001;
                    if( tolV1>MaxTolVert || tolV2>MaxTolVert ) continue; 
                    
                    hasModifWire = Standard_True; //gka 06.09.04
                    TopoDS_Vertex NewV1,NewV2;
                    TopoDS_Edge tmpE,SegE;
                    // split edge1
                    Standard_Integer akey1=0, akey2=0;
                    // analysis fo P01
                    if(P01.Distance(PV1)<Max(tolV1,BRep_Tool::Tolerance(V1))) {
                      NewV1 = V1;
                      if(tolV1>BRep_Tool::Tolerance(V1))
                        B.UpdateVertex(NewV1,tolV1);
                      akey1++;
                    }
                    if(P01.Distance(PV2)<Max(tolV1,BRep_Tool::Tolerance(V2))) {
                      NewV1 = V2;
                      if(tolV1>BRep_Tool::Tolerance(V2))
                        B.UpdateVertex(NewV1,tolV1);
                      akey1++;
                    }
                    // analysis fo P02
                    if(P02.Distance(PV1)<Max(tolV2,BRep_Tool::Tolerance(V1))) {
                      NewV2 = V1;
                      if(tolV2>BRep_Tool::Tolerance(V1))
                        B.UpdateVertex(NewV2,tolV2);
                      akey2++;
                    }
                    if(P02.Distance(PV2)<Max(tolV2,BRep_Tool::Tolerance(V2))) {
                      NewV2 = V2;
                      if(tolV2>BRep_Tool::Tolerance(V2))
                        B.UpdateVertex(NewV2,tolV2);
                      akey2++;
                    }
                    if( akey1>1 || akey2>1 ) continue;
                    // prepare vertices
                    if(akey1==0) B.MakeVertex(NewV1,P01,tolV1);
                    if(akey2==0) B.MakeVertex(NewV2,P02,tolV2);
                    // split
                    Standard_Integer numseg1=num1;
                    if( akey1==0 && akey2>0 ) {
                      if(SplitEdge1(sewd1, face, num1, p11, NewV1, tolV1, boxes1)) {
                        NbModif++;
                        numseg1=num1+1;
                      }
                    }
                    if( akey1>0 && akey2==0 ) {
                      if(SplitEdge1(sewd1, face, num1, p12, NewV2, tolV2, boxes1) ) {
                        NbModif++;
                        numseg1=num1;
                      }
                    }
                    if( akey1==0 && akey2==0 ) {
                      int num1split2 = num1; // what edge to split by point 2
                      if(SplitEdge1(sewd1, face, num1, p11, NewV1, tolV1, boxes1)) {
                        NbModif++;
                        tmpE = sewd1->Edge (num1);
                        Standard_Real a, b;
                        Handle (Geom2d_Curve) c2d;
                        sae.PCurve (tmpE, face, c2d, a, b, Standard_False);
                        if ((a - p12)*(b - p12) > 0)
                        { // p12 - external for [a,b] => split next edge
                          num1split2++;
                        }
                      }
                      if (SplitEdge1 (sewd1, face, num1split2, p12, NewV2, tolV2, boxes1))
                      {
                        NbModif++;
                        numseg1=num1+1;
                      }
                    }
                    SegE = sewd1->Edge(numseg1); // get edge from segment
                    // split edge2
                    // replace vertices if it is necessary
                    ShapeBuild_Edge sbe;
                    akey1=0, akey2=0;
                    if(P01.Distance(PV12)<tolV1) {
                      tolV1 += P01.Distance(PV12);
                      B.UpdateVertex(NewV1,tolV1);
                      V12 = NewV1;
                      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,NewV1,V22);
                      myContext->Replace(edge2,NewE);
                      sewd2->Set(NewE,num2);
                      boxes2.Bind(NewE,B2); // update boxes2
                      edge2 = NewE;
                      akey1 = 1;
                    }
                    if(P01.Distance(PV22)<tolV1) {
                      tolV1 += P01.Distance(PV22);
                      B.UpdateVertex(NewV1,tolV1);
                      V22 = NewV1;
                      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V12,NewV1);
                      myContext->Replace(edge2,NewE);
                      sewd2->Set(NewE,num2);
                      boxes2.Bind(NewE,B2); // update boxes2
                      edge2 = NewE;
                      akey1 = 2;
                    }
                    if(P02.Distance(PV12)<tolV2) {
                      tolV2 += P02.Distance(PV12);
                      B.UpdateVertex(NewV2,tolV2);
                      V12 = NewV2;
                      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,NewV2,V22);
                      myContext->Replace(edge2,NewE);
                      sewd2->Set(NewE,num2);
                      boxes2.Bind(NewE,B2); // update boxes2
                      edge2 = NewE;
                      akey2 = 1;
                    }
                    if(P02.Distance(PV22)<tolV2) {
                      tolV2 += P02.Distance(PV22);
                      B.UpdateVertex(NewV2,tolV2);
                      V22 = NewV2;
                      TopoDS_Edge NewE = sbe.CopyReplaceVertices(edge2,V12,NewV2);
                      myContext->Replace(edge2,NewE);
                      sewd2->Set(NewE,num2);
                      boxes2.Bind(NewE,B2); // update boxes2
                      edge2 = NewE;
                      akey2 = 2;
                    }
                    // split
                    Standard_Integer numseg2=num2;
                    if( akey1==0 && akey2>0 ) {
                      if(SplitEdge1(sewd2, face, num2, p21, NewV1, tolV1, boxes2)) {
                        NbModif++;
                        numseg2=num2+1;
                      }
                    }
                    if( akey1>0 && akey2==0 ) {
                      if(SplitEdge1(sewd2, face, num2, p22, NewV2, tolV2, boxes2)) {
                        NbModif++;
                        numseg2=num2;
                      }
                    }
                    if( akey1==0 && akey2==0 ) {
                      int num2split2 = num2;
                      if(SplitEdge1(sewd2, face, num2, p21, NewV1, tolV1, boxes2)) {
                        NbModif++;
                        numseg2=num2+1;
                        tmpE = sewd2->Edge (num2);
                        Standard_Real a, b;
                        Handle (Geom2d_Curve) c2d;
                        sae.PCurve (tmpE, face, c2d, a, b, Standard_False);
                        if ((a - p22)*(b - p22) > 0)
                        { // p22 - external for [a,b] => split next edge
                          num2split2++;
                        }
                      }
                      if (SplitEdge1 (sewd2, face, num2split2, p22, NewV2, tolV2, boxes2))
                      {
                        NbModif++;
                        numseg2 = num2 + 1;
                      }
                    }
                    tmpE = sewd2->Edge(numseg2);
                    boxes2.Bind(tmpE,boxes1.Find(SegE)); // update boxes2
                    if(!sae.FirstVertex(SegE).IsSame(sae.FirstVertex(tmpE))) {
                      SegE.Reverse();
                    }
                    myContext->Replace(tmpE,SegE);
                    sewd2->Set(SegE,numseg2);
                    num1--;
                    break;
                  }
                  else {
                    // split each intersecting edge on two edges
                    gp_Pnt P0( (Pnt11.X()+Pnt12.X())/2, (Pnt11.Y()+Pnt12.Y())/2,
                               (Pnt11.Z()+Pnt12.Z())/2 );
                    Standard_Real param1 = (p11+p12)/2;
                    Standard_Real param2 = (p21+p22)/2;
                    gp_Pnt Pnt10 = GetPointOnEdge(edge1,sas,C1,param1);
                    gp_Pnt Pnt20 = GetPointOnEdge(edge2,sas,C2,param2);
                    dist1 = Max(Pnt11.Distance(P0),Pnt12.Distance(Pnt10));
                    dist2 = Max(Pnt21.Distance(P0),Pnt22.Distance(Pnt10));
                    Standard_Real tolV = Max(dist1,dist2);
                    tolV = Max(tolV,Pnt10.Distance(Pnt20))*1.00001;
                    B.MakeVertex(NewV,Pnt10,tolV);
                    MaxTolVert = Max(MaxTolVert,tolV);
                    hasModifWire = Standard_True;
                    if(SplitEdge2(sewd2, face, num2, p21, p22, NewV, tolV, boxes2)) {
                      NbModif++;
                      num2--;
                    }
                    if(SplitEdge2(sewd1, face, num1, p11, p12, NewV, tolV, boxes1)) {
                      NbModif++;
                      num1--;
                      break;
                    }
                  }
                }
              }
            } // end if(Inter.NbSegments()==1)

          }
        }
      }
      if(hasModifWire) {
        isDone = Standard_True;
        SeqWir.SetValue(n1,sewd1->Wire());
        myContext->Replace( wire1, sewd1->Wire() );
        wire1 = sewd1->Wire();
        //recompute boxes for wire1
        boxes1.Clear();
        Bnd_Box2d aNewBox1 = CreateBoxes2d (sewd1, face, boxes1);
        aSeqWirBoxes.SetValue (n1, aNewBox1);
        SeqWir.SetValue(n2,sewd2->Wire());
        myContext->Replace( wire2, sewd2->Wire() );
        wire2 = sewd2->Wire();
        //recompute boxes for wire2
        boxes2.Clear();
        Bnd_Box2d aNewBox2 = CreateBoxes2d (sewd2, face, boxes2);
        aSeqWirBoxes.SetValue (n2, aNewBox2);
      }

    }
  }

  if(isDone) {
    // update face
    TopoDS_Shape emptyCopied = face.EmptyCopied();
    TopoDS_Face newface = TopoDS::Face (emptyCopied);
    newface.Orientation(TopAbs_FORWARD);
    BRep_Builder B;
    Standard_Integer i=1;
    for(i=1  ; i<=SeqWir.Length(); i++) { 
      TopoDS_Wire wire = TopoDS::Wire(SeqWir.Value(i));
      B.Add(newface,wire);
    }
    for(i=1 ; i<=SeqNMShapes.Length(); i++) { 
      TopoDS_Shape aNMS = SeqNMShapes.Value(i);
      B.Add(newface,aNMS);
    }
    newface.Orientation(ori);
    myContext->Replace(face,newface);
    face = newface;
  }
  return isDone;
}

