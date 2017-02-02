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

//#4  szv          S4163: optimizing
//    pdn 20.04.99 S4181  Moving algorithm for transforming pcurves from IGES processor
//    abv 05.05.99 S4137: adding methods for copying ranges, reassigning pcurves etc.

#include <BRep_Builder.hxx>
#include <BRep_Curve3D.hxx>
#include <BRep_CurveOnSurface.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepLib.hxx>
#include <ElCLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf2d.hxx>
#include <Precision.hxx>
#include <ShapeBuild_Edge.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_SequenceOfShape.hxx>

//=======================================================================
//function : CopyReplaceVertices
//purpose  : 
//=======================================================================
TopoDS_Edge ShapeBuild_Edge::CopyReplaceVertices (const TopoDS_Edge& edge,
  const TopoDS_Vertex& V1,
  const TopoDS_Vertex& V2) const
{
  TopTools_SequenceOfShape aNMVertices;
  TopoDS_Vertex newV1 = V1, newV2 = V2;
  if ( newV1.IsNull() || newV2.IsNull() ) {
    TopoDS_Iterator it;
    if(edge.Orientation() == TopAbs_FORWARD ||
       edge.Orientation() == TopAbs_REVERSED)
    {
      it.Initialize(edge, Standard_True, Standard_True);
    }
    else
    {
      it.Initialize(edge, Standard_False, Standard_True);
    }
    for ( ; it.More(); it.Next() ) {
      TopoDS_Vertex V = TopoDS::Vertex ( it.Value() );
      if ( V.Orientation() == TopAbs_FORWARD ) {
        if ( newV1.IsNull() ) newV1 = V;
      }
      else if ( V.Orientation() == TopAbs_REVERSED ) {
        if ( newV2.IsNull() ) newV2 = V;
      }
      else if(V1.IsNull() && V2.IsNull())
        aNMVertices.Append(V);
    }
  }
  newV1.Orientation ( TopAbs_FORWARD );
  newV2.Orientation ( TopAbs_REVERSED );

  //szv#4:S4163:12Mar99 SGI warns
  TopoDS_Shape sh = edge.EmptyCopied();
  TopoDS_Edge E = TopoDS::Edge( sh );

  BRep_Builder B;
  if ( ! newV1.IsNull() ) B.Add ( E, newV1 );
  if ( ! newV2.IsNull() ) B.Add ( E, newV2 );

  //addition of the internal or external vertices to edge
  Standard_Integer i =1; 
  for( ; i <= aNMVertices.Length(); i++)
    B.Add ( E,TopoDS::Vertex(aNMVertices.Value(i)));

  //S4054, rln 17.11.98 annie_surf.igs entity D77, 3D and pcurve have different
  //ranges, after B.Range all the ranges become as 3D
  CopyRanges ( E, edge );
  /*
  for (BRep_ListIteratorOfListOfCurveRepresentation itcr
  ((*((Handle(BRep_TEdge)*)&edge.TShape()))->ChangeCurves()); itcr.More(); itcr.Next()) {
  Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itcr.Value());
  if ( GC.IsNull() ) continue;
  Standard_Real first, last;
  GC->Range ( first, last );
  if ( GC->IsCurve3D() ) 
  B.Range ( E, first, last );
  else if ( GC->IsCurveOnSurface() )
  B.Range (E, GC->Surface(), edge.Location().Multiplied (GC->Location()), first, last);//BUC50003 entity 132 edge 1
  }
  */
  return E;
}

//=======================================================================
//function : CopyRanges
//purpose  : 
//=======================================================================

// Added, cause invoke ShapeAnalysis leads to cyclic dependancy.
static Standard_Real AdjustByPeriod(const Standard_Real Val,
  const Standard_Real ToVal,
  const Standard_Real Period)
{
  Standard_Real diff = Val - ToVal;
  Standard_Real D = Abs ( diff );
  Standard_Real P = Abs ( Period );
  if ( D <= 0.5 * P ) return 0.;
  if ( P < 1e-100 ) return diff;
  return ( diff >0 ? -P : P ) * (Standard_Integer)( D / P + 0.5 );
}

static Standard_Boolean IsPeriodic(const Handle(Geom_Curve)& theCurve)
{
  // 15.11.2002 PTV OCC966
  // remove regressions in DE tests (diva, divb, divc, toe3) in KAS:dev
  // ask IsPeriodic on BasisCurve
  Handle(Geom_Curve) aTmpCurve = theCurve;
  while ( (aTmpCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) ||
    (aTmpCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) ) {
      if (aTmpCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
        aTmpCurve = Handle(Geom_OffsetCurve)::DownCast(aTmpCurve)->BasisCurve();
      if (aTmpCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
        aTmpCurve = Handle(Geom_TrimmedCurve)::DownCast(aTmpCurve)->BasisCurve();
  }
  return aTmpCurve->IsPeriodic();
}

Standard_Boolean IsPeriodic(const Handle(Geom2d_Curve)& theCurve)
{
  // 15.11.2002 PTV OCC966
  // remove regressions in DE tests (diva, divb, divc, toe3) in KAS:dev
  // ask IsPeriodic on BasisCurve
  Handle(Geom2d_Curve) aTmpCurve = theCurve;
  while ( (aTmpCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))) ||
    (aTmpCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) ) {
      if (aTmpCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
        aTmpCurve = Handle(Geom2d_OffsetCurve)::DownCast(aTmpCurve)->BasisCurve();
      if (aTmpCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
        aTmpCurve = Handle(Geom2d_TrimmedCurve)::DownCast(aTmpCurve)->BasisCurve();
  }
  return aTmpCurve->IsPeriodic();
}

void ShapeBuild_Edge::CopyRanges (const TopoDS_Edge& toedge, 
  const TopoDS_Edge& fromedge,
  const Standard_Real alpha,
  const Standard_Real beta) const
{
  /*  BRep_Builder B;
  for (BRep_ListIteratorOfListOfCurveRepresentation itcr
  ((*((Handle(BRep_TEdge)*)&fromedge.TShape()))->ChangeCurves()); itcr.More(); itcr.Next()) {
  Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itcr.Value());
  if ( GC.IsNull() ) continue;
  Standard_Real first, last;
  GC->Range ( first, last );
  if ( GC->IsCurve3D() ) 
  B.Range ( toedge, first, last );
  else if ( GC->IsCurveOnSurface() )
  B.Range ( toedge, GC->Surface(), fromedge.Location().Multiplied (GC->Location()), first, last);
  }
  */
  for (BRep_ListIteratorOfListOfCurveRepresentation fromitcr
    ((*((Handle(BRep_TEdge)*)&fromedge.TShape()))->ChangeCurves()); fromitcr.More(); fromitcr.Next()) {
      Handle(BRep_GCurve) fromGC = Handle(BRep_GCurve)::DownCast(fromitcr.Value());
      if ( fromGC.IsNull() ) continue;
      Standard_Boolean isC3d = fromGC->IsCurve3D();
      if(isC3d) {
        if(fromGC->Curve3D().IsNull()) continue; }
      else {
        if(fromGC->PCurve().IsNull()) continue; }

      if ( ! isC3d && ! fromGC->IsCurveOnSurface()) continue; // only 3d curves and pcurves are treated

      Handle(Geom_Surface) surface;
      TopLoc_Location L;
      if ( ! isC3d ) {
        surface = fromGC->Surface();
        L = fromGC->Location();
      } 

      BRep_ListOfCurveRepresentation& tolist = (*((Handle(BRep_TEdge)*)&toedge.TShape()))->ChangeCurves();
      Handle(BRep_GCurve) toGC;
      for (BRep_ListIteratorOfListOfCurveRepresentation toitcr (tolist); toitcr.More(); toitcr.Next()) {
        toGC = Handle(BRep_GCurve)::DownCast(toitcr.Value());
        if ( toGC.IsNull() ) continue;
        if ( isC3d ) {
          if ( ! toGC->IsCurve3D() ) continue;
        }
        else if ( ! toGC->IsCurveOnSurface() || 
          surface != toGC->Surface() || L != toGC->Location() ) continue;
        Standard_Real first = fromGC->First();
        Standard_Real last = fromGC->Last();
        Standard_Real len = last - first;
        Standard_Real newF = first+alpha*len;
        Standard_Real newL = first+beta*len;

        // PTV: 22.03.2002 fix for edge range. 
        // file test-m020306-v2.step Shell #665 (Faces #40110. #40239).
        Standard_Real aPeriod=1., aCrvF=0., aCrvL=1.;
        Standard_Boolean doCheck = Standard_False;
        if (toGC->IsKind(STANDARD_TYPE(BRep_Curve3D))) {
          Handle(Geom_Curve) aCrv3d = Handle(BRep_Curve3D)::DownCast(toGC)->Curve3D();
          // 15.11.2002 PTV OCC966
          if ( ! aCrv3d.IsNull() && IsPeriodic(aCrv3d) ) {
            aPeriod = aCrv3d->Period();
            aCrvF = aCrv3d->FirstParameter();
            aCrvL = aCrv3d->LastParameter();
            doCheck = Standard_True;
          }
        }
        else if  (toGC->IsKind(STANDARD_TYPE(BRep_CurveOnSurface))) {
          Handle(Geom2d_Curve) aCrv2d = Handle(BRep_CurveOnSurface)::DownCast(toGC)->PCurve();
          // 15.11.2002 PTV OCC966
          if (!aCrv2d.IsNull() && IsPeriodic(aCrv2d)) {
            aPeriod = aCrv2d->Period();
            aCrvF = aCrv2d->FirstParameter();
            aCrvL = aCrv2d->LastParameter();
            doCheck = Standard_True;
          }
        }
        if ( doCheck && ( (fabs(newF -aCrvF ) > Precision::PConfusion() && newF < aCrvF) || newF >= aCrvL ) ) {
          Standard_Real aShift = AdjustByPeriod(newF, 0.5*(aCrvF+aCrvL), aPeriod);
          newF += aShift;
          newL += aShift;
          BRep_Builder().SameRange(toedge,Standard_False);
          BRep_Builder().SameParameter(toedge,Standard_False);
        }

        toGC->SetRange ( newF, newL );
        break;
      }
  }
}

//=======================================================================
//function : SetRange3d
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::SetRange3d (const TopoDS_Edge& edge, 
  const Standard_Real first,
  const Standard_Real last) const
{

  for (BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&edge.TShape()))->ChangeCurves()); itcr.More(); itcr.Next()) {
      Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itcr.Value());
      if ( GC.IsNull() || !GC->IsCurve3D() ) continue;
      GC->SetRange ( first, last );
      break;
  }
}

//=======================================================================
//function : CopyPCurves
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::CopyPCurves (const TopoDS_Edge& toedge, const TopoDS_Edge& fromedge) const
{
  TopLoc_Location fromLoc = fromedge.Location();
  TopLoc_Location toLoc = toedge.Location();
  for (BRep_ListIteratorOfListOfCurveRepresentation fromitcr
    ((*((Handle(BRep_TEdge)*)&fromedge.TShape()))->ChangeCurves()); fromitcr.More(); fromitcr.Next()) {
      Handle(BRep_GCurve) fromGC = Handle(BRep_GCurve)::DownCast(fromitcr.Value());
      if ( fromGC.IsNull() ) continue;
      if ( fromGC->IsCurveOnSurface() ) {
        Handle(Geom_Surface) surface = fromGC->Surface();
        TopLoc_Location L = fromGC->Location();
        Standard_Boolean found = Standard_False;
        BRep_ListOfCurveRepresentation& tolist = (*((Handle(BRep_TEdge)*)&toedge.TShape()))->ChangeCurves();
        Handle(BRep_GCurve) toGC;
        for (BRep_ListIteratorOfListOfCurveRepresentation toitcr (tolist); toitcr.More() && !found; toitcr.Next()) {
          toGC = Handle(BRep_GCurve)::DownCast(toitcr.Value());
          if ( toGC.IsNull() || !toGC->IsCurveOnSurface() || 
            surface != toGC->Surface() || L != toGC->Location() ) continue;
          found = Standard_True;
          break;
        }
        if (!found) {
          toGC = Handle(BRep_GCurve)::DownCast(fromGC->Copy());
          tolist.Append (toGC);
        }
        Handle(Geom2d_Curve) pcurve = fromGC->PCurve();
        toGC->PCurve(Handle(Geom2d_Curve)::DownCast(pcurve->Copy()));

        //bug OCC209 invalid location of pcurve in the edge after copying
        TopLoc_Location newLoc = (fromLoc*L).Predivided(toLoc);//(L * fromLoc).Predivided(toLoc);
        toGC->Location(newLoc);
        if ( fromGC->IsCurveOnClosedSurface() ) {
          pcurve = fromGC->PCurve2();
          toGC->PCurve2(Handle(Geom2d_Curve)::DownCast(pcurve->Copy()));
        }
      }
  }
}
//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

TopoDS_Edge ShapeBuild_Edge::Copy (const TopoDS_Edge &edge, 
  const Standard_Boolean sharepcurves) const
{
  TopoDS_Vertex dummy1, dummy2;
  TopoDS_Edge newedge = CopyReplaceVertices ( edge, dummy1, dummy2 );
  if ( ! sharepcurves ) CopyPCurves ( newedge, edge );
  return newedge;
}

//=======================================================================
//function : RemovePCurve
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::RemovePCurve (const TopoDS_Edge& edge, 
  const TopoDS_Face& face) const
{
  BRep_Builder B;
  Handle(Geom2d_Curve) c2dNull;
  //:S4136  Standard_Real tol = BRep_Tool::Tolerance ( edge );
  if ( BRep_Tool::IsClosed ( edge, face ) )
    B.UpdateEdge ( edge, c2dNull, c2dNull, face, 0. ); //:S4136: tol
  else B.UpdateEdge ( edge, c2dNull,          face, 0. ); //:S4136: tol
}

//=======================================================================
//function : RemovePCurve
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::RemovePCurve (const TopoDS_Edge& edge,
  const Handle(Geom_Surface)& surf) const
{
  RemovePCurve ( edge, surf, TopLoc_Location() );
}

//=======================================================================
//function : RemovePCurve
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::RemovePCurve (const TopoDS_Edge& edge,
  const Handle(Geom_Surface)& surf,
  const TopLoc_Location &loc) const
{
  BRep_Builder B;
  Handle(Geom2d_Curve) c2dNull;
  //:S4136  Standard_Real tol = BRep_Tool::Tolerance ( edge );
  if ( BRep_Tool::IsClosed ( edge, surf, loc ) )
    B.UpdateEdge ( edge, c2dNull, c2dNull, surf, loc, 0. ); //:S4136: tol
  else B.UpdateEdge ( edge, c2dNull,          surf, loc, 0. ); //:S4136: tol
}

//=======================================================================
//function : ReplacePCurve
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::ReplacePCurve (const TopoDS_Edge& edge,
  const Handle(Geom2d_Curve)& pcurve,
  const TopoDS_Face& face) const
{
  BRep_Builder B;
  Standard_Real f,l;
  TopoDS_Shape dummy = edge.Reversed();
  TopoDS_Edge edgerev = TopoDS::Edge(dummy);
  // reverse face to take second pcurve for seams like SA_Edge::PCurve() does
  TopoDS_Face F = TopoDS::Face(face.Oriented ( TopAbs_FORWARD ) );
  Handle(Geom2d_Curve) pcurve0 = BRep_Tool::CurveOnSurface(edge,F,f,l);
  Handle(Geom2d_Curve) c2d = BRep_Tool::CurveOnSurface(edgerev,F,f,l);
  // Add pcurve to edge (either as single, or as seam)
  if ( c2d.IsNull() || c2d == pcurve0 ) { // non-seam 
    B.UpdateEdge(edge,pcurve,face,0);
  }
  else { // seam
    if(edge.Orientation()==TopAbs_FORWARD)
      B.UpdateEdge(edge,pcurve,c2d,face,0);
    else
      B.UpdateEdge(edge,c2d,pcurve,face,0);
  }
  B.Range ( edge, face, f, l );
}

//=======================================================================
//function : ReassignPCurve
//purpose  : 
//=======================================================================

// Count exact number of pcurves STORED in edge for face
// This makes difference for faces based on plane surfaces where pcurves can be 
// not stored but returned by BRep_Tools::CurveOnSurface
static Standard_Integer CountPCurves (const TopoDS_Edge &edge, 
  const TopoDS_Face &face)
{
  TopLoc_Location L;
  Handle(Geom_Surface) S = BRep_Tool::Surface ( face, L );
  TopLoc_Location l = L.Predivided(edge.Location());

  for (BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&edge.TShape()))->ChangeCurves()); itcr.More(); itcr.Next()) {
      Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itcr.Value());
      if ( ! GC.IsNull() && GC->IsCurveOnSurface ( S, l ) ) 
        return ( GC->IsCurveOnClosedSurface() ? 2 : 1 );
  }
  return 0;
}

Standard_Boolean ShapeBuild_Edge::ReassignPCurve (const TopoDS_Edge& edge,
  const TopoDS_Face& old,
  const TopoDS_Face& sub) const
{
  Standard_Integer npcurves = CountPCurves ( edge, old );
  //if ( npcurves <1 ) return Standard_False; //gka

  Standard_Real f, l;
  Handle(Geom2d_Curve) pc;
  pc = BRep_Tool::CurveOnSurface ( edge, old, f, l );
  if ( pc.IsNull() ) return Standard_False;
  else if( npcurves == 0)  npcurves =1; //gka


  BRep_Builder B;

  // if the pcurve was only one, remove; else leave second one
  if ( npcurves >1 ) {
    //smh#8 Porting AIX
    TopoDS_Shape tmpshape = edge.Reversed();
    TopoDS_Edge erev = TopoDS::Edge (tmpshape);
    Handle(Geom2d_Curve) pc2 = BRep_Tool::CurveOnSurface ( erev, old, f, l );
    B.UpdateEdge ( edge, pc2, old, 0. );
    B.Range ( edge, old, f, l );
  }
  else RemovePCurve ( edge, old );

  // if edge does not have yet pcurves on sub, just add; else add as first
  Standard_Integer npcs = CountPCurves ( edge, sub );
  if ( npcs <1 ) B.UpdateEdge ( edge, pc, sub, 0. );
  else {
    //smh#8 Porting AIX
    TopoDS_Shape tmpshape = edge.Reversed();
    TopoDS_Edge erev = TopoDS::Edge (tmpshape);
    Standard_Real cf, cl;
    Handle(Geom2d_Curve) pcs = BRep_Tool::CurveOnSurface ( erev, sub, cf, cl );
    if ( edge.Orientation() == TopAbs_REVERSED ) // because B.UpdateEdge does not check edge orientation
      B.UpdateEdge ( edge, pcs, pc, sub, 0. );
    else B.UpdateEdge ( edge, pc, pcs, sub, 0. );
  }

  B.Range ( edge, sub, f, l );

  return Standard_True;
}

//=======================================================================
//function : TransformPCurve
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) ShapeBuild_Edge::TransformPCurve(const Handle(Geom2d_Curve)& pcurve,
  const gp_Trsf2d& trans,
  const Standard_Real uFact,
  Standard_Real& aFirst,
  Standard_Real& aLast) const
{
  Handle(Geom2d_Curve) result = Handle(Geom2d_Curve)::DownCast(pcurve->Copy());
  if(trans.Form()!=gp_Identity) {
    result->Transform (trans);
    aFirst = result->TransformedParameter(aFirst,trans);
    aLast  = result->TransformedParameter(aLast, trans);
  }
  if(uFact==1.) 
    return result;

  if (result->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) { 
    Handle(Geom2d_TrimmedCurve) thecurve = Handle(Geom2d_TrimmedCurve)::DownCast(result);
    result = thecurve->BasisCurve();
  }

  gp_GTrsf2d  tMatu;
  tMatu.SetAffinity(gp::OY2d(), uFact);
  gp_XY  pXY;

  if (result->IsKind(STANDARD_TYPE(Geom2d_Line))) {
    Handle(Geom2d_Line) aLine2d = Handle(Geom2d_Line)::DownCast(result);
    gp_Pnt2d Pf, Pl;
    aLine2d->D0(aFirst,Pf);
    pXY = Pf.XY();
    tMatu.Transforms(pXY);
    Pf.SetXY(pXY);
    aLine2d->D0(aLast, Pl);
    pXY = Pl.XY();
    tMatu.Transforms(pXY);
    Pl.SetXY(pXY);
    gp_Lin2d line2d(Pf, gp_Dir2d(gp_Vec2d(Pf,Pl)));
    aFirst = ElCLib::Parameter(line2d, Pf);
    aLast = ElCLib::Parameter(line2d, Pl);
    Handle(Geom2d_Line) Gline2d = new Geom2d_Line(line2d);
    return Gline2d;
  } 
  else if(result->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))) {
    Handle(Geom2d_BezierCurve) bezier = Handle(Geom2d_BezierCurve)::DownCast(result);
    // transform the Poles of the BSplineCurve 
    Standard_Integer nbPol = bezier->NbPoles();
    gp_Pnt2d Pt1;
    for (Standard_Integer i = 1; i<=nbPol ; i++) {
      pXY = bezier->Pole(i).XY();
      tMatu.Transforms(pXY);
      Pt1.SetXY(pXY);
      bezier->SetPole(i, Pt1);
    }
    return bezier;
  } else {
    Handle(Geom2d_BSplineCurve) aBSpline2d; 
    if(result->IsKind(STANDARD_TYPE(Geom2d_Conic))) {
      //gp_Pln pln(gp_Pnt(0,0,0),gp_Dir(0,0,1));
      //Handle(Geom_Curve) curve = GeomAPI::To3d(result,pln);
      Handle(Geom2d_Curve) tcurve = new Geom2d_TrimmedCurve(result,aFirst,aLast); //protection agains parabols ets
      Geom2dConvert_ApproxCurve approx (tcurve, Precision::Approximation(), 
        GeomAbs_C1, 100, 6 );
      if ( approx.HasResult() )
        aBSpline2d = approx.Curve();
      else
        aBSpline2d = Geom2dConvert::CurveToBSplineCurve(tcurve,Convert_QuasiAngular);
      aFirst = aBSpline2d->FirstParameter();
      aLast =  aBSpline2d->LastParameter();
    } 
    else if(!result->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
      aBSpline2d = Geom2dConvert::CurveToBSplineCurve(result,Convert_QuasiAngular);
    }
    else
      aBSpline2d = Handle(Geom2d_BSplineCurve)::DownCast(result);

    // transform the Poles of the BSplineCurve 
    Standard_Integer nbPol = aBSpline2d->NbPoles();
    gp_Pnt2d Pt1;
    for (Standard_Integer i = 1; i<=nbPol ; i++) {
      pXY = aBSpline2d->Pole(i).XY();
      tMatu.Transforms(pXY);
      Pt1.SetXY(pXY);
      aBSpline2d->SetPole(i, Pt1);
    }
    return aBSpline2d;
  }
}

//=======================================================================
//function : RemoveCurve3d
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::RemoveCurve3d (const TopoDS_Edge& edge) const
{
  BRep_Builder B;
  Handle(Geom_Curve) c3dNull;
  //:S4136  Standard_Real tol = BRep_Tool::Tolerance (edge);
  B.UpdateEdge (edge, c3dNull, 0. ); //:S4136: tol
}

//=======================================================================
//function : BuildCurve3d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeBuild_Edge::BuildCurve3d (const TopoDS_Edge& edge) const
{
  try {
    OCC_CATCH_SIGNALS
      //#48 rln 10.12.98 S4054 UKI60107-5 entity 365
      //C0 surface (but curve 3d is required as C1) and tolerance is 1e-07
      //lets use maximum of tolerance and default parameter 1.e-5
      //Another solutions: use quite big Tolerance or require C0 curve on C0 surface
      if ( BRepLib::BuildCurve3d (edge, Max (1.e-5, BRep_Tool::Tolerance(edge) ) ) ) {
        //#50 S4054 rln 14.12.98 write cylinder in BRep mode into IGES and read back
        //with 2DUse_Forced - pcurve and removed 3D curves have different ranges
        if (BRep_Tool::SameRange (edge)) {
          Standard_Real first, last;
          BRep_Tool::Range (edge, first, last);
          BRep_Builder().Range (edge, first, last);//explicit setting for all reps
        }
        Handle(Geom_Curve) c3d;
        Standard_Real f,l;
        c3d = BRep_Tool::Curve(edge,f,l);
        if (c3d.IsNull())
          return Standard_False;
        // 15.11.2002 PTV OCC966
        if(!IsPeriodic(c3d)) {
          Standard_Boolean isLess = Standard_False;
          if(f < c3d->FirstParameter()) {
            isLess = Standard_True;
            f = c3d->FirstParameter();
          }
          if(l > c3d->LastParameter()) {
            isLess = Standard_True;
            l = c3d->LastParameter();
          }
          if(isLess) {
            SetRange3d(edge,f,l);
            BRep_Builder().SameRange(edge,Standard_False);
          }
        }

        return Standard_True;
      }
  }
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout << "\nWarning: ShapeBuild_Edge: Exception in BuildCurve3d: "; 
    anException.Print(cout); cout << endl;
#endif
    (void)anException;
  }
  return Standard_False;
}


//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::MakeEdge(TopoDS_Edge& edge,const Handle(Geom_Curve)& curve,const TopLoc_Location& L) const
{
  MakeEdge (edge,curve, L, curve->FirstParameter(), curve->LastParameter());
}

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::MakeEdge(TopoDS_Edge& edge,const Handle(Geom_Curve)& curve,const TopLoc_Location& L,const Standard_Real p1,const Standard_Real p2) const
{
  BRepBuilderAPI_MakeEdge ME (curve, p1, p2);
  if (!ME.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "\nWarning: ShapeBuild_Edge::MakeEdge BRepAPI_NotDone";
#endif
    return;
  }
  TopoDS_Edge E = ME.Edge();
  if (!L.IsIdentity()) {
    BRep_Builder B;
    B.UpdateEdge (E, curve, L, 0.);
    B.Range (E, p1, p2);

    TopoDS_Vertex V1, V2;
    TopExp::Vertices (E, V1, V2);
    gp_Pnt P1 = BRep_Tool::Pnt (V1), P2 = BRep_Tool::Pnt (V2);
    B.UpdateVertex (V1, P1.Transformed (L.Transformation()), 0.);
    B.UpdateVertex (V2, P2.Transformed (L.Transformation()), 0.);
  }
  edge = E;
  return;
}

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::MakeEdge(TopoDS_Edge& edge,const Handle(Geom2d_Curve)& pcurve,const TopoDS_Face& face) const
{
  MakeEdge (edge, pcurve, face, pcurve->FirstParameter(), pcurve->LastParameter());
}

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::MakeEdge(TopoDS_Edge& edge,const Handle(Geom2d_Curve)& pcurve,const TopoDS_Face& face,const Standard_Real p1,const Standard_Real p2) const
{
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(face, L);
  MakeEdge (edge, pcurve, S, L, p1, p2);
}

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::MakeEdge(TopoDS_Edge& edge,const Handle(Geom2d_Curve)& pcurve,
  const Handle(Geom_Surface)& S,const TopLoc_Location& L) const
{
  MakeEdge(edge, pcurve, S, L, pcurve->FirstParameter(), pcurve->LastParameter());
}

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void ShapeBuild_Edge::MakeEdge(TopoDS_Edge& edge,const Handle(Geom2d_Curve)& pcurve,
  const Handle(Geom_Surface)& S,const TopLoc_Location& L,
  const Standard_Real p1,const Standard_Real p2) const
{
  BRepBuilderAPI_MakeEdge ME (pcurve, S, p1, p2);
  if (!ME.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "\nWarning: ShapeBuild_Edge::MakeEdge BRepAPI_NotDone";
#endif
    return;
  }
  TopoDS_Edge E = ME.Edge();
  if (!L.IsIdentity()) {
    RemovePCurve (E, S);
    BRep_Builder B;
    B.UpdateEdge (E, pcurve, S, L, 0.);
    B.Range (E, S, L, p1, p2);

    TopoDS_Vertex V1, V2;
    TopExp::Vertices (E, V1, V2);
    gp_Pnt P1 = BRep_Tool::Pnt (V1), P2 = BRep_Tool::Pnt (V2);
    B.UpdateVertex (V1, P1.Transformed (L.Transformation()), 0.);
    B.UpdateVertex (V2, P2.Transformed (L.Transformation()), 0.);
  }
  edge = E;
  return;
}

