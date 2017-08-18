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

//:o8 abv 19.02.99: CTS18541.stp #18559: coeff 1.0001 added in CheckVertexTol
//:p1 abv 22.02.99: protection against edges with no vertices (infinite)
//szv#4 S4163
//:s1 abv 22.04.99: PRO7226 #489490: protect against Null 3d curve
//:s4 abv 26.04.99: sim6049.igs 21677: copy of curve is necessary to get True SP
//    abv 06.05.99: S4137: adding methods GetTangent2d()

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeExtend.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : ShapeAnalysis_Edge
//purpose  : 
//=======================================================================
ShapeAnalysis_Edge::ShapeAnalysis_Edge()
{
  myStatus = 0;//ShapeExtend::EncodeStatus (ShapeExtend_OK);
}


//=======================================================================
//function : BoundUV
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::BoundUV (const TopoDS_Edge& edge,
					      const TopoDS_Face& face,
					      gp_Pnt2d& first, gp_Pnt2d& last) const
{
  TopLoc_Location L;
  const Handle(Geom_Surface) S = BRep_Tool::Surface(face, L);
  return BoundUV (edge, S, L, first, last);
}


//=======================================================================
//function : BoundUV
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::BoundUV (const TopoDS_Edge& edge,
					      const Handle(Geom_Surface)& surface,
					      const TopLoc_Location& location,
					      gp_Pnt2d& first, gp_Pnt2d& last) const
{
  Handle(Geom2d_Curve) c2d;
  Standard_Real uf,ul;
  if (!PCurve (edge, surface, location, c2d, uf, ul)) return Standard_False;
  first = c2d->Value (uf);
  last  = c2d->Value (ul);
  return Standard_True;
}


//=======================================================================
//function : HasCurve3d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::HasCurve3d (const TopoDS_Edge& edge) const
{
  Standard_Real cf, cl;
  Handle(Geom_Curve) c3d = BRep_Tool::Curve (edge, cf, cl);
  return !c3d.IsNull();
}


//=======================================================================
//function : Curve3d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::Curve3d (const TopoDS_Edge& edge,
					      Handle(Geom_Curve)& C3d,
					      Standard_Real& cf, Standard_Real& cl,
					      const Standard_Boolean orient) const
{
  TopLoc_Location L;
  C3d = BRep_Tool::Curve (edge, L, cf, cl);
  if( !C3d.IsNull() && !L.IsIdentity() ) {
    C3d = Handle(Geom_Curve)::DownCast(C3d->Transformed(L.Transformation()));
    cf = C3d->TransformedParameter(cf, L.Transformation());
    cl = C3d->TransformedParameter(cl, L.Transformation());
  }
  if (orient) {
    if (edge.Orientation() == TopAbs_REVERSED)
      {Standard_Real tmp = cf; cf = cl; cl = tmp;}
  }
  return !C3d.IsNull();
}


//=======================================================================
//function : IsClosed3d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::IsClosed3d (const TopoDS_Edge& edge) const
{
  Standard_Real cf,cl;
  Handle(Geom_Curve) c3d = BRep_Tool::Curve (edge,cf,cl);
  if (c3d.IsNull()) return Standard_False;
  if (!c3d->IsClosed()) return Standard_False;
  return FirstVertex (edge).IsSame (LastVertex (edge));
}


//=======================================================================
//function : HasPCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::HasPCurve (const TopoDS_Edge& edge,
						const TopoDS_Face& face) const
{
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(face, L);
  return HasPCurve (edge, S, L);
}


//=======================================================================
//function : HasPCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::HasPCurve (const TopoDS_Edge& edge,
						const Handle(Geom_Surface)& surface,
						const TopLoc_Location& location) const
{
  //try { //szv#4:S4163:12Mar99 waste try
    Standard_Real cf, cl;
    Handle(Geom2d_Curve) c2d = BRep_Tool::CurveOnSurface (edge, surface, location, cf, cl);
    return !c2d.IsNull();
  /* }
  catch (Standard_Failure) {
  }
  return Standard_False; */
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::PCurve (const TopoDS_Edge& edge,
					     const TopoDS_Face& face,
					     Handle(Geom2d_Curve)& C2d,
					     Standard_Real& cf, Standard_Real& cl,
					     const Standard_Boolean orient) const
{
  //:abv 20.05.02: take into account face orientation
  // COMMENTED BACK - NEEDS MORE CHANGES IN ALL SHAPEHEALING
//   C2d = BRep_Tool::CurveOnSurface (edge, face, cf, cl);
//   if (orient && edge.Orientation() == TopAbs_REVERSED) {
//     Standard_Real tmp = cf; cf = cl; cl = tmp;
//   }
//   return !C2d.IsNull();
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(face, L);
  return PCurve (edge, S, L, C2d, cf, cl, orient);
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::PCurve (const TopoDS_Edge& edge,
					     const Handle(Geom_Surface)& surface,
					     const TopLoc_Location& location,
					     Handle(Geom2d_Curve)& C2d,
					     Standard_Real& cf, Standard_Real& cl,
					     const Standard_Boolean orient) const
{
  C2d = BRep_Tool::CurveOnSurface (edge, surface, location, cf, cl);
  if ( orient && edge.Orientation() == TopAbs_REVERSED ) {
    Standard_Real tmp = cf; cf = cl; cl = tmp;
  }
  return !C2d.IsNull();
}


//=======================================================================
//function : IsSeam
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::IsSeam (const TopoDS_Edge& edge,
					     const TopoDS_Face& face) const
{
  return BRep_Tool::IsClosed (edge, face);
}


//=======================================================================
//function : IsSeam
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::IsSeam (const TopoDS_Edge& edge,
					     const Handle(Geom_Surface)& surface,
					     const TopLoc_Location& location) const
{
  return BRep_Tool::IsClosed (edge, surface, location);
}


//=======================================================================
//function : FirstVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ShapeAnalysis_Edge::FirstVertex (const TopoDS_Edge& edge) const
{
  TopoDS_Vertex V;
  if (edge.Orientation() == TopAbs_REVERSED) {
    V = TopExp::LastVertex(edge);
    V.Reverse();
  }
  else {
    V = TopExp::FirstVertex (edge);
  }
  return V;
}


//=======================================================================
//function : LastVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex ShapeAnalysis_Edge::LastVertex (const TopoDS_Edge& edge) const
{
  TopoDS_Vertex V;
  if (edge.Orientation() == TopAbs_REVERSED) {
    V = TopExp::FirstVertex(edge);
    V.Reverse();
  }
  else {
    V = TopExp::LastVertex (edge);
  }
  return V;
}


//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::Status (const ShapeExtend_Status Status) const
{
  return ShapeExtend::DecodeStatus (myStatus, Status);
}


//=======================================================================
//function : GetEndTangent2d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::GetEndTangent2d (const TopoDS_Edge &edge, 
						      const TopoDS_Face &face,
						      const Standard_Boolean atend1, /* skl : change "atend" to "atend1" */ 
						      gp_Pnt2d &pnt, 
						      gp_Vec2d &v,
                                                      const Standard_Real dparam) const
{
  TopLoc_Location L;
  const Handle(Geom_Surface) S = BRep_Tool::Surface ( face, L );
  return GetEndTangent2d ( edge, S, L, atend1, pnt, v, dparam );
}


//=======================================================================
//function : GetEndTangent2d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::GetEndTangent2d (const TopoDS_Edge &edge, 
						      const Handle(Geom_Surface)& S,
						      const TopLoc_Location& L,
						      const Standard_Boolean atend2, /* skl : change "atend" to "atend2" */
						      gp_Pnt2d &pnt, 
						      gp_Vec2d &v,
                                                      const Standard_Real dparam) const
{
  Standard_Real cf, cl;
  Handle(Geom2d_Curve) c2d;
  if ( ! PCurve ( edge, S, L, c2d, cf, cl ) ) {
    v = gp_Vec2d(0,0);
    return Standard_False;
  }
  Standard_Real dpnew = dparam;

  if(dpnew>Precision::Confusion()) {
    gp_Pnt2d ptmp;
    Standard_Real par1,par2,delta=(cl-cf)*dpnew;
    if(Abs(delta)<Precision::PConfusion()) {
      dpnew=0.0;
    }
    else {
      if(atend2) {
        par1 = cl;
        par2 = cl - delta;
        c2d->D0(par1,pnt);
        c2d->D0(par2,ptmp);
        v = pnt.XY() - ptmp.XY();
      }
      else {
        par1 = cf;
        par2 = cf + delta;
        c2d->D0(par1,pnt);
        c2d->D0(par2,ptmp);
        v = ptmp.XY() - pnt.XY();
      }
      if( v.SquareMagnitude() < Precision::PConfusion()*Precision::PConfusion() ) {
        dpnew=0.0;
      }
    }
  }
  
  if(dpnew<=Precision::Confusion()) {
    // get non-null tangency searching until 3rd derivative, or as straight btw ends
    Standard_Real par = ( atend2 ? cl : cf );
    c2d->D1 ( par, pnt, v );
    if ( v.SquareMagnitude() < Precision::PConfusion()*Precision::PConfusion() ) {
      gp_Vec2d d1;
      c2d->D2 ( par, pnt, d1, v );
      if ( v.SquareMagnitude() < Precision::PConfusion()*Precision::PConfusion() ) {
        gp_Vec2d d2;
        c2d->D3 ( par, pnt, d1, d2, v );
        if ( v.SquareMagnitude() < Precision::PConfusion()*Precision::PConfusion() ) {
          gp_Pnt2d p2;
          c2d->D0 ( ( atend2 ? cf : cl ), p2 );
          v = p2.XY() - pnt.XY();
          if ( v.SquareMagnitude() < Precision::PConfusion()*Precision::PConfusion() )
            return Standard_False;
        }
      }
    }
    if ( edge.Orientation() == TopAbs_REVERSED ) v.Reverse();
  }

  //if ( edge.Orientation() == TopAbs_REVERSED ) v.Reverse();
  return Standard_True;
}
	    

//=======================================================================
//function : CheckCurve3dWithPCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::CheckCurve3dWithPCurve (const TopoDS_Edge& edge,
							     const TopoDS_Face& face) 
{
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(face, L);
  return CheckCurve3dWithPCurve (edge, S, L);
}


//=======================================================================
//function : CheckCurve3dWithPCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::CheckCurve3dWithPCurve (const TopoDS_Edge& edge,
							     const Handle(Geom_Surface)& surface,
							     const TopLoc_Location& location) 
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);

  if(surface->IsKind(STANDARD_TYPE(Geom_Plane)))
     return Standard_False;
  
  Handle (Geom2d_Curve) c2d;
  Standard_Real f2d, l2d; //szv#4:S4163:12Mar99 moved down f3d, l3d
  if (!PCurve (edge, surface, location, c2d, f2d ,l2d, Standard_False)) {
    myStatus |= ShapeExtend::EncodeStatus (ShapeExtend_FAIL1);
    return Standard_False;
  }

  Handle (Geom_Curve) c3d; //szv#4:S4163:12Mar99 moved
  Standard_Real f3d, l3d; //szv#4:S4163:12Mar99 moved
  if (!Curve3d (edge, c3d, f3d, l3d, Standard_False)) {
    myStatus |= ShapeExtend::EncodeStatus (ShapeExtend_FAIL2);
    return Standard_False;
  }

  TopoDS_Vertex aFirstVert = FirstVertex (edge);
  TopoDS_Vertex aLastVert  = LastVertex (edge);

  if (aFirstVert.IsNull() || aLastVert.IsNull())
    return Standard_False;

  Standard_Real preci1 = BRep_Tool::Tolerance (aFirstVert),
                preci2 = BRep_Tool::Tolerance (aLastVert);

  gp_Pnt2d p2d1 = c2d->Value (f2d),
           p2d2 = c2d->Value (l2d);

  //#39 rln 17.11.98 S4054, annie_surf.igs entity 39
  return CheckPoints (c3d->Value (f3d)/*.Transformed (location.Transformation())*/,
		      c3d->Value (l3d)/*.Transformed (location.Transformation())*/,
		      surface->Value (p2d1.X(), p2d1.Y()).Transformed (location.Transformation()),
		      surface->Value (p2d2.X(), p2d2.Y()).Transformed (location.Transformation()),
		      preci1, preci2);
}


//=======================================================================
//function : CheckPoints
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::CheckPoints (const gp_Pnt& P1A,
						  const gp_Pnt& P1B,
						  const gp_Pnt& P2A,
						  const gp_Pnt& P2B,
						  const Standard_Real preci1,
						  const Standard_Real preci2) 
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
  if (P1A.SquareDistance (P2A) <= preci1 * preci1 &&
      P1B.SquareDistance (P2B) <= preci2 * preci2)
    return Standard_False;
  else if (P1A.Distance (P2B) + (P1B.Distance (P2A)) <
	   P1A.Distance (P2A) + (P1B.Distance (P2B)))
    myStatus |= ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
  return Standard_True;
}


//=======================================================================
//function : CheckVerticesWithCurve3d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::CheckVerticesWithCurve3d (const TopoDS_Edge& edge,
							       const Standard_Real preci,
							       const Standard_Integer vtx)
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);

  TopoDS_Vertex V1 = FirstVertex (edge);
  TopoDS_Vertex V2 = LastVertex (edge);
  gp_Pnt p1v = BRep_Tool::Pnt (V1);
  gp_Pnt p2v = BRep_Tool::Pnt (V2);

  Standard_Real cf,cl;
  Handle(Geom_Curve) c3d;
  if ( ! Curve3d (edge,c3d,cf,cl) ) {
    myStatus |= ShapeExtend::EncodeStatus (ShapeExtend_FAIL1);
    return Standard_False;
  }

  //  on va faire les checks ...
  if (vtx != 2) {
    //  1er VTX
    gp_Pnt p13d = c3d->Value(cf);
    //szv#4:S4163:12Mar99 optimized
    if (p1v.Distance(p13d) > (preci < 0 ? BRep_Tool::Tolerance (V1) : preci))
      myStatus |= ShapeExtend_DONE1;
  }

  if (vtx != 1) {
    //  2me VTX
    gp_Pnt p23d = c3d->Value(cl);
    //szv#4:S4163:12Mar99 optimized
    if (p2v.Distance(p23d) > (preci < 0 ? BRep_Tool::Tolerance (V2) : preci))
      myStatus |= ShapeExtend_DONE2;
  }

  return Status ( ShapeExtend_DONE );
}


//=======================================================================
//function : CheckVerticesWithPCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::CheckVerticesWithPCurve (const TopoDS_Edge& edge,
							      const TopoDS_Face& face,
							      const Standard_Real preci,
							      const Standard_Integer vtx)
{
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(face, L);
  return CheckVerticesWithPCurve (edge, S, L, preci, vtx); //szv#4:S4163:12Mar99 `vtx,preci` wrong parameters order
}


//=======================================================================
//function : CheckVerticesWithPCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::CheckVerticesWithPCurve (const TopoDS_Edge& edge,
							      const Handle(Geom_Surface)& surf,
							      const TopLoc_Location& loc,
							      const Standard_Real preci,
							      const Standard_Integer vtx)
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);

  TopoDS_Vertex V1 = FirstVertex (edge);
  TopoDS_Vertex V2 = LastVertex (edge);
  gp_Pnt p1v = BRep_Tool::Pnt (V1);
  gp_Pnt p2v = BRep_Tool::Pnt (V2);

  Standard_Real cf, cl;
  Handle(Geom2d_Curve) c2d;
  if ( ! PCurve ( edge, surf, loc, c2d, cf, cl ) ) {
    myStatus |= ShapeExtend::EncodeStatus (ShapeExtend_FAIL1);
    return Standard_False;
  }

  // on va faire les checks ...
  if (vtx != 2) { //  1er VTX
    gp_Pnt2d p1uv = c2d->Value (cf);  
    gp_Pnt p12d = surf->Value (p1uv.X(), p1uv.Y());
    if (!loc.IsIdentity())
      p12d.Transform(loc.Transformation());
    // szv#4:S4163:12Mar99 optimized
    if ( p1v.Distance(p12d) > (preci < 0 ? BRep_Tool::Tolerance (V1) : preci) )
      myStatus |= ShapeExtend_DONE1;
  }

  if (vtx != 1) { //  2me VTX
    gp_Pnt2d p2uv = c2d->Value (cl);  
    gp_Pnt p22d = surf->Value (p2uv.X(), p2uv.Y());
    if (!loc.IsIdentity())
      p22d.Transform(loc.Transformation());
    // szv#4:S4163:12Mar99 optimized
    if ( p2v.Distance(p22d) > (preci < 0 ? BRep_Tool::Tolerance (V2) : preci) )
      myStatus |= ShapeExtend_DONE2;
  }

  return Status ( ShapeExtend_DONE );
}


//=======================================================================
//function : CheckVertexTolerance
//purpose  : 
//=======================================================================

static Standard_Integer CheckVertexTolerance(const TopoDS_Edge& edge,
                                             const TopoDS_Face& face,
                                             const Standard_Boolean checkAll,
                                             Standard_Real& toler1,
                                             Standard_Real& toler2)
{
  Standard_Integer Status = ShapeExtend::EncodeStatus ( ShapeExtend_OK );

  ShapeAnalysis_Edge sae;
  TopoDS_Vertex V1 = sae.FirstVertex (edge);
  TopoDS_Vertex V2 = sae.LastVertex (edge);
  if ( V1.IsNull() || V2.IsNull() ) { //:p1 abv 22 Feb 99: r76sy.stp
    Status |= ShapeExtend::EncodeStatus (ShapeExtend_FAIL1);
    return Status;
  }

  Standard_Real old1 = BRep_Tool::Tolerance (V1);
  Standard_Real old2 = BRep_Tool::Tolerance (V2);
  gp_Pnt pnt1 = BRep_Tool::Pnt (V1);
  gp_Pnt pnt2 = BRep_Tool::Pnt (V2);

  Standard_Real a, b;
  Handle(Geom_Curve) c3d;
  if ( ! sae.Curve3d(edge, c3d, a, b, Standard_True)) {
    if ( ! BRep_Tool::Degenerated ( edge ) ) 
      Status |= ShapeExtend::EncodeStatus (ShapeExtend_FAIL2);
    toler1 = toler2 = 0.;
//    return Standard_False;
  }
  else {
    toler1 = pnt1.SquareDistance (c3d->Value (a));
    toler2 = pnt2.SquareDistance (c3d->Value (b));
  }

  if ( checkAll ) {
    Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&edge.TShape());
    for (BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves()); itcr.More(); itcr.Next() ) {
      Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itcr.Value());
      if (GC.IsNull() || !GC->IsCurveOnSurface()) continue;
      Handle(Geom2d_Curve) pcurve;
      Handle(Geom_Surface) S = GC->Surface();
      TopLoc_Location L = edge.Location() * GC->Location();
      sae.PCurve (edge, S, L, pcurve, a, b, Standard_True);
      gp_Pnt2d p1 = pcurve->Value (a);
      gp_Pnt2d p2 = pcurve->Value (b);
      gp_Pnt P1 = S->Value (p1.X(), p1.Y()).Transformed (L.Transformation());
      gp_Pnt P2 = S->Value (p2.X(), p2.Y()).Transformed (L.Transformation());
      toler1 = Max (toler1, pnt1.SquareDistance (P1));
      toler2 = Max (toler2, pnt2.SquareDistance (P2));
    }
  }
  //:abv 10.06.02: porting C40 -> dev (CC670-12608.stp)
  // Check with given face is needed for plane surfaces (if no stored pcurves)
  else if ( ! face.IsNull() ) {
    Handle(Geom2d_Curve) pcurve;
    TopLoc_Location L;
    const Handle(Geom_Surface)& S = BRep_Tool::Surface(face, L);
    if ( sae.PCurve(edge, S, L, pcurve, a, b, Standard_True)) {
      gp_Pnt2d p1 = pcurve->Value (a);
      gp_Pnt2d p2 = pcurve->Value (b);
      gp_Pnt P1 = S->Value (p1.X(), p1.Y()).Transformed (L.Transformation());
      gp_Pnt P2 = S->Value (p2.X(), p2.Y()).Transformed (L.Transformation());
      toler1 = Max (toler1, pnt1.SquareDistance (P1));
      toler2 = Max (toler2, pnt2.SquareDistance (P2));
    }
    else Status |= ShapeExtend::EncodeStatus (ShapeExtend_FAIL3);
  }

  //:o8 abv 19 Feb 99: CTS18541.stp #18559: coeff 1.0001 added
  //szv 18 Aug 99: edge tolerance is taken in consideration
  Standard_Real tole = BRep_Tool::Tolerance (edge);
  toler1 = Max (1.0000001 * Sqrt (toler1), tole);
  toler2 = Max (1.0000001 * Sqrt (toler2), tole);
  if ( toler1 > old1) 
    Status |= ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
  if ( toler2 > old2) 
    Status |= ShapeExtend::EncodeStatus (ShapeExtend_DONE2);

  return Status;
}


//=======================================================================
//function : CheckVertexTolerance
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::CheckVertexTolerance(const TopoDS_Edge& edge,
							  const TopoDS_Face& face,
                                                          Standard_Real& toler1,
                                                          Standard_Real& toler2)
{
  myStatus = ::CheckVertexTolerance ( edge, face, Standard_False, toler1, toler2 );
  return Status ( ShapeExtend_DONE );
}


//=======================================================================
//function : CheckVertexTolerance
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::CheckVertexTolerance(const TopoDS_Edge& edge,
                                                          Standard_Real& toler1,
                                                          Standard_Real& toler2)
{
  TopoDS_Face F;
  myStatus = ::CheckVertexTolerance ( edge, F, Standard_True, toler1, toler2 );
  return Status ( ShapeExtend_DONE );
}


//=======================================================================
//static : Validate
//purpose: For SameParameter: compute it for two curves
//note: This function is made from Validate() in BRepCheck_Edge.cxx
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::ComputeDeviation (const Adaptor3d_Curve& CRef,
                                                       const Adaptor3d_Curve& Other,
                                                       const Standard_Boolean SameParameter,
                                                       Standard_Real &dev,
                                                       const Standard_Integer NCONTROL)
{
  Standard_Boolean OK = Standard_True;
  Standard_Real dev2 = dev*dev;
  
  Standard_Real First = CRef.FirstParameter(), Last = CRef.LastParameter();
  Standard_Real OFirst = Other.FirstParameter(), OLast  = Other.LastParameter(); //szv#4:S4163:12Mar99 moved

  Standard_Boolean proj = (!SameParameter || First != OFirst || Last != OLast); //szv#4:S4163:12Mar99 optimized

  Standard_Integer NCtrl = ( NCONTROL < 1 )? 1 : NCONTROL; //szv#4:S4163:12Mar99 anti-exception

  if (!proj) {
    for (Standard_Integer i = 0; i <= NCtrl; i++) {
      Standard_Real prm = ((NCtrl-i)*First + i*Last)/NCtrl;
      gp_Pnt pref = CRef.Value(prm);
      gp_Pnt pother = Other.Value(prm);
      Standard_Real dist2 = pref.SquareDistance(pother);
      if ( dev2 < dist2 ) dev2 = dist2; 
    }
    dev = Sqrt ( dev2 );
  }
  else {
    gp_Pnt pd = CRef.Value(First);
    gp_Pnt pdo = Other.Value(OFirst);
    Standard_Real dist2 = pd.SquareDistance(pdo);
    if ( dev2 < dist2 ) dev = Sqrt ( dev2 = dist2 );

    pd = CRef.Value(Last);
    pdo = Other.Value(OLast);
    dist2 = pd.SquareDistance(pdo);
    if ( dev2 < dist2 ) dev = Sqrt ( dev2 = dist2 );

    Extrema_LocateExtPC refd, otherd; //szv#4:S4163:12Mar99 warning
    refd.Initialize(CRef,First,Last,Precision::PConfusion());
    otherd.Initialize(Other,OFirst,OLast,Precision::PConfusion());

    for (Standard_Integer i = 1; i < NCtrl; i++) { //szv#4:S4163:12Mar99 was bug
      Standard_Real rprm = ((NCtrl-i)*First + i*Last)/NCtrl;
      gp_Pnt pref = CRef.Value(rprm);
      Standard_Real oprm = ((NCtrl-i)*OFirst + i*OLast)/NCtrl;
      gp_Pnt pother = Other.Value(oprm);

      refd.Perform(pother,rprm);
      if ( ! refd.IsDone() ) OK = Standard_False;
      else if ( dev2 < refd.SquareDistance() ) {dev2 = refd.SquareDistance(); dev = sqrt (dev2);}

      otherd.Perform(pref,oprm);
      if ( ! otherd.IsDone() ) OK = Standard_False;
      else if ( dev2 < otherd.SquareDistance() ) {dev2 = otherd.SquareDistance(); dev = sqrt (dev2);}
    }
  }
  dev *= 1.00001;//ims007 entity 8067 edge 3; 1e-07USA60022 (4255, 4-th edge) SA_Check and BRepCh find distinct points001; // ensure that dev*dev >= dev2
  
  return OK;
}

//=======================================================================
//function : CheckSameParameter
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::CheckSameParameter (const TopoDS_Edge& edge,
                                                         Standard_Real& maxdev,
                                                         const Standard_Integer NbControl)
{
  TopoDS_Face anEmptyFace;
  return CheckSameParameter(edge, anEmptyFace, maxdev, NbControl);
}


//=======================================================================
//function : CheckSameParameter
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::CheckSameParameter (const TopoDS_Edge& edge,
                                                         const TopoDS_Face& face,
                                                         Standard_Real& maxdev,
                                                         const Standard_Integer NbControl)
{
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
  if (BRep_Tool::Degenerated (edge)) return Standard_False;

  maxdev = 0;

  // Get same parameter flag
  Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&edge.TShape());
  Standard_Boolean SameParameter = TE->SameParameter();

  // Get 3D curve of the edge
  Standard_Real aFirst, aLast;
  TopLoc_Location aCurveLoc;
  Handle(Geom_Curve) aC3D = BRep_Tool::Curve(edge, aCurveLoc, aFirst, aLast);
  if (aC3D.IsNull()) {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return Standard_False;
  }

  if (!aCurveLoc.IsIdentity())
  {
    const gp_Trsf& aTrsf = aCurveLoc.Transformation();
    aC3D = Handle(Geom_Curve)::DownCast(aC3D->Transformed(aTrsf));
    aFirst = aC3D->TransformedParameter(aFirst, aTrsf);
    aLast = aC3D->TransformedParameter(aLast, aTrsf);
  }

  // Create adaptor for the curve
  GeomAdaptor_Curve aGAC(aC3D, aFirst, aLast);

  Handle(Geom_Surface) aFaceSurf;
  TopLoc_Location aFaceLoc;
  if (!face.IsNull())
    aFaceSurf = BRep_Tool::Surface(face, aFaceLoc);

  Standard_Boolean IsPCurveFound = Standard_False;
  Standard_Integer i = 1;

  // Iterate on all curve representations
  for (;;)
  {
    Handle(Geom2d_Curve) aPC;
    Handle(Geom_Surface) aS;
    TopLoc_Location aLoc;
    Standard_Real f, l;

    BRep_Tool::CurveOnSurface(edge, aPC, aS, aLoc, f, l, i);

    if (aPC.IsNull())
      // No more curves
      break;

    ++i;

    // If the input face is not null, check that the curve is on its surface
    if (!aFaceSurf.IsNull())
    {
      if (aFaceSurf != aS || aFaceLoc != aLoc)
      {
        continue;
      }
    }

    IsPCurveFound = Standard_True;

    // Apply transformations for the surface
    Handle(Geom_Surface) aST = Handle(Geom_Surface)::
      DownCast(aS->Transformed(aLoc.Transformation()));

    // Compute deviation between curves
    Handle(Geom2dAdaptor_HCurve) GHPC = new Geom2dAdaptor_HCurve(aPC, f, l);
    Handle(GeomAdaptor_HSurface) GAHS = new GeomAdaptor_HSurface(aST);

    Adaptor3d_CurveOnSurface ACS(GHPC, GAHS);
    if (!ComputeDeviation(aGAC, ACS, SameParameter, maxdev, NbControl - 1))
    {
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL2 );
    }
  }

  // For the planar face and non-existing  2d curve
  // check the deviation for the projection of the 3d curve on plane
  if (!IsPCurveFound && !aFaceSurf.IsNull())
  {
    Standard_Real f, l;
    Handle(Geom2d_Curve) aPC = BRep_Tool::CurveOnPlane(edge, aFaceSurf, aFaceLoc, f, l);
    if (!aPC.IsNull())
    {
      Handle(Geom2dAdaptor_HCurve) GHPC = new Geom2dAdaptor_HCurve(aPC, aFirst, aLast);

      Handle(Geom_Surface) aST =
        Handle(Geom_Surface)::DownCast(aFaceSurf->Transformed(aFaceLoc.Transformation()));
      Handle(GeomAdaptor_HSurface) GAHS = new GeomAdaptor_HSurface(aST);

      Adaptor3d_CurveOnSurface ACS(GHPC, GAHS);

      if (!ComputeDeviation(aGAC, ACS, SameParameter, maxdev, NbControl - 1))
      {
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      }
    }
  }

  if (maxdev > TE->Tolerance())
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  if (!SameParameter)
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);

  return Status ( ShapeExtend_DONE );
}

//=======================================================================
//function : IsOverlapPartEdges
//purpose  : 
//=======================================================================

static Standard_Boolean IsOverlapPartEdges(const TopoDS_Edge& theFirstEdge,
                                           const TopoDS_Edge& theSecEdge,
                                           const Standard_Real& theTolerance,
                                           const Standard_Real& theStep,
                                           const Standard_Real& theStartLength,
                                           const Standard_Real& theEndLenght)
{
  TColStd_SequenceOfInteger aSeqIntervals;
  BRepAdaptor_Curve aAdCurve1(theFirstEdge);

  BRepExtrema_DistShapeShape aMinDist;
  aMinDist.LoadS1(theSecEdge);

  for(Standard_Real aS = theStartLength; aS <= theEndLenght; aS+=theStep/2) {
    
    gp_Pnt aPoint;
    if(aS <= Precision::Confusion()) {
      TopoDS_Vertex V1 =  TopExp::FirstVertex(theFirstEdge,Standard_True);
      aPoint = BRep_Tool::Pnt(V1);
    }
    else {
      GCPnts_AbscissaPoint aAbsPoint(Precision::Confusion(),aAdCurve1,aS,aAdCurve1.FirstParameter());
      if(aAbsPoint.IsDone()) 
        aAdCurve1.D0(aAbsPoint.Parameter(),aPoint);
      else continue;
    }
    BRep_Builder aB;
    TopoDS_Vertex aV;
    aB.MakeVertex(aV,aPoint,Precision::Confusion());
    aMinDist.LoadS2(aV);
    aMinDist.Perform();
    if( aMinDist.IsDone() && aMinDist.Value() >= theTolerance)
      return Standard_False;
  }
  return Standard_True;
}


//=======================================================================
//function : CheckOverlapping
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Edge::CheckOverlapping(const TopoDS_Edge& theEdge1,
                                                      const TopoDS_Edge& theEdge2,
                                                      Standard_Real& theTolOverlap,
                                                      const Standard_Real theDomainDist) 
{
  Standard_Boolean isOverlap = Standard_False;
  BRepAdaptor_Curve aAdCurve1(theEdge1);
  Standard_Real aLength1 = GCPnts_AbscissaPoint::Length (aAdCurve1);
  BRepAdaptor_Curve aAdCurve2(theEdge2);
  Standard_Real aLength2 = GCPnts_AbscissaPoint::Length (aAdCurve2);
  TopoDS_Edge aFirstEdge = (aLength1 >= aLength2 ? theEdge2: theEdge1);
  TopoDS_Edge aSecEdge = (aLength1 >= aLength2 ? theEdge1: theEdge2);
  Standard_Real aLength = Min(aLength1,aLength2);
  
  //check overalpping between edges on whole edges
  Standard_Real aStep = Min(aLength1,aLength2)/2;
  isOverlap = IsOverlapPartEdges(aFirstEdge,aSecEdge,theTolOverlap,aStep,0.,Min(aLength1,aLength2)); 
  
  if(isOverlap)    {
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3);
    return isOverlap;
  }
  if(theDomainDist ==0.0)
    return isOverlap;
  
  //check overalpping between edges on segment with length less than theDomainDist
  
  Standard_Real aDomainTol = (theDomainDist > Min( aLength1,aLength2) ? Min( aLength1,aLength2) :theDomainDist);
  BRepExtrema_DistShapeShape aMinDist(aFirstEdge,aSecEdge,theTolOverlap);
  Standard_Real aresTol = theTolOverlap;
  if(aMinDist.IsDone()) {
    aresTol = aMinDist.Value();
    if(aresTol >= theTolOverlap) return Standard_False;
    Standard_Integer NbSol = aMinDist.NbSolution();
    for(Standard_Integer i =1; i<= NbSol && !isOverlap; i++) {
      BRepExtrema_SupportType aType1 = aMinDist.SupportTypeShape1(i);
      Standard_Real aEndLength, aStartLength, aLengthP; 
      if(aType1 == BRepExtrema_IsVertex) {
        TopoDS_Shape aSupportShape1 = aMinDist.SupportOnShape1(i);
        TopoDS_Vertex aV1,aV2;
        TopExp::Vertices (aFirstEdge, aV1, aV2, Standard_True);
        if( aV1.IsSame(aSupportShape1)) 
          aLengthP = 0.0;
        else
          aLengthP =aLength;
      }
      else if(aType1 == BRepExtrema_IsOnEdge) {
        Standard_Real aParam1, aFirst, aLast;
        aMinDist.ParOnEdgeS1 ( i, aParam1 );
        BRep_Tool::Range(aFirstEdge,aFirst,aLast);
        BRepAdaptor_Curve anAdaptor(aFirstEdge);
        aLengthP = GCPnts_AbscissaPoint::Length(anAdaptor,aFirst,aParam1);
      }
      else continue;
      aStartLength = aLengthP - aDomainTol/2;
      if( aStartLength <0.0) {
        aStartLength =0;
        aEndLength = aDomainTol;
      }
      aEndLength = aLengthP + aDomainTol/2;
      if(aEndLength > aLength) {
        aEndLength = aLength;
        aStartLength = aEndLength - aDomainTol;
      }
      aStep = (aEndLength - aStartLength)/5;
      isOverlap = (IsOverlapPartEdges(aFirstEdge,aSecEdge,theTolOverlap,aStep,aStartLength,aEndLength));
    }
  }
  if(isOverlap)    
    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE4);
   
  theTolOverlap = aresTol;
   return isOverlap;
}
