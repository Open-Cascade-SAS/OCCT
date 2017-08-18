// Created on: 1993-07-07
// Created by: Remi LEQUETTE
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


#include <BRep_Curve3D.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveOnSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_PolygonOnClosedSurface.hxx>
#include <BRep_PolygonOnClosedTriangulation.hxx>
#include <BRep_PolygonOnSurface.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
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
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <BRep_GCurve.hxx>

//modified by NIZNHY-PKV Fri Oct 17 14:13:29 2008f
static 
  Standard_Boolean IsPlane(const Handle(Geom_Surface)& aS);
//modified by NIZNHY-PKV Fri Oct 17 14:13:33 2008t
//
//=======================================================================
//function : Surface
//purpose  : Returns the geometric surface of the face. Returns
//           in <L> the location for the surface.
//=======================================================================

const Handle(Geom_Surface)& BRep_Tool::Surface(const TopoDS_Face& F,
                                               TopLoc_Location& L)
{
  const BRep_TFace* TF = static_cast<const BRep_TFace*>(F.TShape().get());
  L = F.Location() * TF->Location();
  return TF->Surface();
}

//=======================================================================
//function : Surface
//purpose  : Returns the geometric  surface of the face. It can
//           be a copy if there is a Location.
//=======================================================================

Handle(Geom_Surface) BRep_Tool::Surface(const TopoDS_Face& F)
{
  const BRep_TFace* TF = static_cast<const BRep_TFace*>(F.TShape().get());
  const Handle(Geom_Surface)& S = TF->Surface();

  if(S.IsNull()) return S;

  TopLoc_Location L = F.Location() * TF->Location();
  if (!L.IsIdentity()) {
    Handle(Geom_Geometry) aCopy = S->Transformed(L.Transformation());
    Geom_Surface* aGS = static_cast<Geom_Surface*>(aCopy.get());
    return Handle(Geom_Surface)(aGS);
  }
  return S;
}

//=======================================================================
//function : Triangulation
//purpose  : Returns  the Triangulation of  the  face. It  is a
//           null handle if there is no triangulation.
//=======================================================================

const Handle(Poly_Triangulation)& BRep_Tool::Triangulation(const TopoDS_Face& F,
                                                           TopLoc_Location&   L)
{
  L = F.Location();
  const BRep_TFace* TF = static_cast<const BRep_TFace*>(F.TShape().get());
  return TF->Triangulation();
}

//=======================================================================
//function : Tolerance
//purpose  : Returns the tolerance of the face.
//=======================================================================

Standard_Real  BRep_Tool::Tolerance(const TopoDS_Face& F)
{
  const BRep_TFace* TF = static_cast<const BRep_TFace*>(F.TShape().get());
  Standard_Real p = TF->Tolerance();
  Standard_Real pMin = Precision::Confusion();
  if (p > pMin) return p;
  else          return pMin;
}

//=======================================================================
//function : NaturalRestriction
//purpose  : Returns the  NaturalRestriction  flag of the  face.
//=======================================================================

Standard_Boolean  BRep_Tool::NaturalRestriction(const TopoDS_Face& F)
{
  const BRep_TFace* TF = static_cast<const BRep_TFace*>(F.TShape().get());
  return TF->NaturalRestriction();
}

//=======================================================================
//function : Curve
//purpose  : Returns the 3D curve  of the edge.  May be  a Null
//           handle. Returns in <L> the location for the curve.
//           In <First> and <Last> the parameter range.
//=======================================================================

static const Handle(Geom_Curve) nullCurve;

const Handle(Geom_Curve)&  BRep_Tool::Curve(const TopoDS_Edge& E,
                                            TopLoc_Location& L,
                                            Standard_Real& First,
                                            Standard_Real& Last)
{
  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves()); 

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurve3D()) {
      const BRep_Curve3D* GC = static_cast<const BRep_Curve3D*>(cr.get());
      L = E.Location() * GC->Location();
      GC->Range(First,Last);
      return GC->Curve3D();
    }
    itcr.Next();
  }
  L.Identity();
  First = Last = 0.;
  return nullCurve;
}

//=======================================================================
//function : Curve
//purpose  : Returns the 3D curve  of the edge. May be a Null handle.
//           In <First> and <Last> the parameter range.
//           It can be a copy if there is a Location.
//=======================================================================

Handle(Geom_Curve)  BRep_Tool::Curve(const TopoDS_Edge& E,
                                     Standard_Real& First,
                                     Standard_Real& Last)
{
  TopLoc_Location L;
  const Handle(Geom_Curve)& C = Curve(E,L,First,Last);
  if ( !C.IsNull() ) {
    if ( !L.IsIdentity() ) {
      Handle(Geom_Geometry) aCopy = C->Transformed(L.Transformation());
      Geom_Curve* aGC = static_cast<Geom_Curve*>(aCopy.get());
      return Handle(Geom_Curve)(aGC);
    }
  }
  return C;
}

//=======================================================================
//function : IsGeometric
//purpose  : Returns True if <E> is a 3d curve or a curve on
//           surface.
//=======================================================================

Standard_Boolean  BRep_Tool::IsGeometric(const TopoDS_Edge& E)
{
  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurve3D()) {
      Handle(BRep_Curve3D) GC (Handle(BRep_Curve3D)::DownCast (cr));
      if (! GC.IsNull() && ! GC->Curve3D().IsNull())
        return Standard_True;
    }
    else if (cr->IsCurveOnSurface()) return Standard_True;
    itcr.Next();
  }
  return Standard_False;
}

//=======================================================================
//function : Polygon3D
//purpose  : Returns the 3D polygon of the edge. May be   a Null
//           handle. Returns in <L> the location for the polygon.
//=======================================================================

static const Handle(Poly_Polygon3D) nullPolygon3D;

const Handle(Poly_Polygon3D)& BRep_Tool::Polygon3D(const TopoDS_Edge& E,
                                                   TopLoc_Location&   L)
{
  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygon3D()) {
      const BRep_Polygon3D* GC = static_cast<const BRep_Polygon3D*>(cr.get());
      L = E.Location() * GC->Location();
      return GC->Polygon3D();
    }
    itcr.Next();
  }
  L.Identity();
  return nullPolygon3D;
}

//=======================================================================
//function : CurveOnSurface
//purpose  : Returns the curve  associated to the  edge in  the
//           parametric  space of  the  face.  Returns   a NULL
//           handle  if this curve  does not exist.  Returns in
//           <First> and <Last> the parameter range.
//=======================================================================

Handle(Geom2d_Curve) BRep_Tool::CurveOnSurface(const TopoDS_Edge& E, 
                                               const TopoDS_Face& F,
                                               Standard_Real& First,
                                               Standard_Real& Last)
{
  TopLoc_Location l;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,l);
  TopoDS_Edge aLocalEdge = E;
  if (F.Orientation() == TopAbs_REVERSED) {
    aLocalEdge.Reverse();
//    return CurveOnSurface(E,S,l,First,Last);
  }
//    return CurveOnSurface(TopoDS::Edge(E.Reversed()),S,l,First,Last);
//  else
//    return CurveOnSurface(E,S,l,First,Last);
  return CurveOnSurface(aLocalEdge,S,l,First,Last);
}

//=======================================================================
//function : CurveOnSurface
//purpose  : Returns the  curve associated to   the edge in the
//           parametric  space of the   surface. Returns a NULL
//           handle  if this curve does  not exist.  Returns in
//           <First> and <Last> the parameter range.
//=======================================================================

static const Handle(Geom2d_Curve) nullPCurve;

Handle(Geom2d_Curve) BRep_Tool::CurveOnSurface(const TopoDS_Edge& E, 
                                               const Handle(Geom_Surface)& S,
                                               const TopLoc_Location& L,
                                               Standard_Real& First,
                                               Standard_Real& Last)
{
  TopLoc_Location loc = L.Predivided(E.Location());
  Standard_Boolean Eisreversed = (E.Orientation() == TopAbs_REVERSED);

  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,loc)) {
      const BRep_GCurve* GC = static_cast<const BRep_GCurve*>(cr.get());
      GC->Range(First,Last);
      if (GC->IsCurveOnClosedSurface() && Eisreversed)
        return GC->PCurve2();
      else
        return GC->PCurve();
    }
    itcr.Next();
  }

  // Curve is not found. Try projection on plane
  return CurveOnPlane(E, S, L, First, Last);
}

//=======================================================================
//function : CurveOnPlane
//purpose  : For planar surface returns projection of the edge on the plane
//=======================================================================
Handle(Geom2d_Curve) BRep_Tool::CurveOnPlane(const TopoDS_Edge& E,
                                             const Handle(Geom_Surface)& S,
                                             const TopLoc_Location& L,
                                             Standard_Real& First,
                                             Standard_Real& Last)
{
  First = Last = 0.;

  // Check if the surface is planar
  Handle(Geom_Plane) GP;
  Handle(Geom_RectangularTrimmedSurface) GRTS;
  GRTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
  if(!GRTS.IsNull())
    GP = Handle(Geom_Plane)::DownCast(GRTS->BasisSurface());
  else
    GP = Handle(Geom_Plane)::DownCast(S);

  if (GP.IsNull())
    // not a plane
    return nullPCurve;

  // Check existence of 3d curve in edge
  Standard_Real f, l;
  TopLoc_Location aCurveLocation;
  Handle(Geom_Curve) C3D = BRep_Tool::Curve(E, aCurveLocation, f, l);

  if (C3D.IsNull())
    // no 3d curve
    return nullPCurve;

  aCurveLocation = aCurveLocation.Predivided(L);
  First = f; Last = l;

  // Transform curve and update parameters in account of scale factor
  if (!aCurveLocation.IsIdentity())
  {
    const gp_Trsf& aTrsf = aCurveLocation.Transformation();
    C3D = Handle(Geom_Curve)::DownCast(C3D->Transformed(aTrsf));
    f = C3D->TransformedParameter(f, aTrsf);
    l = C3D->TransformedParameter(l, aTrsf);
  }

  // Perform projection
  Handle(Geom_Curve) ProjOnPlane =
    GeomProjLib::ProjectOnPlane(new Geom_TrimmedCurve(C3D, f, l, Standard_True, Standard_False),
                                GP,
                                GP->Position().Direction(),
                                Standard_True);

  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface(GP);
  Handle(GeomAdaptor_HCurve)   HC = new GeomAdaptor_HCurve(ProjOnPlane);

  ProjLib_ProjectedCurve Proj(HS, HC);
  Handle(Geom2d_Curve) pc = Geom2dAdaptor::MakeCurve(Proj);

  if (pc->DynamicType() == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    Handle(Geom2d_TrimmedCurve) TC = Handle(Geom2d_TrimmedCurve)::DownCast(pc);
    pc = TC->BasisCurve();
  }

  return pc;
}

//=======================================================================
//function : CurveOnSurface
//purpose  : 
//=======================================================================

void  BRep_Tool::CurveOnSurface(const TopoDS_Edge& E, 
                                Handle(Geom2d_Curve)& C, 
                                Handle(Geom_Surface)& S, 
                                TopLoc_Location& L,
                                Standard_Real& First,
                                Standard_Real& Last)
{
  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface()) {
      const BRep_GCurve* GC = static_cast<const BRep_GCurve*>(cr.get());
      C = GC->PCurve();
      S = GC->Surface();
      L = E.Location() * GC->Location();
      GC->Range(First,Last);
      return;
    }
    itcr.Next();
  }
  
  C.Nullify();
  S.Nullify();
  L.Identity();
  First = Last = 0.;
}

//=======================================================================
//function : CurveOnSurface
//purpose  : 
//=======================================================================

void  BRep_Tool::CurveOnSurface(const TopoDS_Edge& E, 
                                Handle(Geom2d_Curve)& C, 
                                Handle(Geom_Surface)& S, 
                                TopLoc_Location& L,
                                Standard_Real& First,
                                Standard_Real& Last,
                                const Standard_Integer Index)
{
  if (Index < 1)
    return;

  Standard_Integer i = 0;
  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
  for (; itcr.More(); itcr.Next())
  {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface())
    {
      const BRep_GCurve* GC = static_cast<const BRep_GCurve*>(cr.get());
      ++i;
      // Compare index taking into account the fact that for the curves on
      // closed surfaces there are two PCurves
      if (i == Index)
        C = GC->PCurve();
      else if (GC->IsCurveOnClosedSurface() && (++i == Index))
        C = GC->PCurve2();
      else
        continue;

      S = GC->Surface();
      L = E.Location() * GC->Location();
      GC->Range(First, Last);
      return;
    }
  }
  
  C.Nullify();
  S.Nullify();
  L.Identity();
  First = Last = 0.;
}

//=======================================================================
//function : PolygonOnSurface
//purpose  : Returns the polygon associated to the  edge in  the
//           parametric  space of  the  face.  Returns   a NULL
//           handle  if this polygon  does not exist.
//=======================================================================

Handle(Poly_Polygon2D) BRep_Tool::PolygonOnSurface(const TopoDS_Edge& E, 
                                                   const TopoDS_Face& F)
{
  TopLoc_Location l;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,l);
  TopoDS_Edge aLocalEdge = E;
  if (F.Orientation() == TopAbs_REVERSED) {
    aLocalEdge.Reverse();
//    return PolygonOnSurface(E,S,l);
  }
  //    return PolygonOnSurface(TopoDS::Edge(E.Reversed()),S,l);
//  else
//    return PolygonOnSurface(E,S,l);
  return PolygonOnSurface(aLocalEdge,S,l);
}

//=======================================================================
//function : PolygonOnSurface
//purpose  : Returns the polygon associated to the  edge in  the
//           parametric  space of  the surface. Returns   a NULL
//           handle  if this polygon  does not exist.
//=======================================================================

static const Handle(Poly_Polygon2D) nullPolygon2D;

Handle(Poly_Polygon2D) BRep_Tool::PolygonOnSurface(const TopoDS_Edge& E, 
                                                   const Handle(Geom_Surface)& S,
                                                   const TopLoc_Location& L)
{
  TopLoc_Location l = L.Predivided(E.Location());
  Standard_Boolean Eisreversed = (E.Orientation() == TopAbs_REVERSED);

  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygonOnSurface(S,l)) {
      if (cr->IsPolygonOnClosedSurface() && Eisreversed )
        return cr->Polygon2();
      else
        return cr->Polygon();
    }
    itcr.Next();
  }
  
  return nullPolygon2D;
}

//=======================================================================
//function : PolygonOnSurface
//purpose  : 
//=======================================================================

void BRep_Tool::PolygonOnSurface(const TopoDS_Edge&      E,
                                 Handle(Poly_Polygon2D)& P,
                                 Handle(Geom_Surface)&   S,
                                 TopLoc_Location&        L)
{
  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygonOnSurface()) {
      const BRep_PolygonOnSurface* PS = static_cast<const BRep_PolygonOnSurface*>(cr.get());
      P = PS->Polygon();
      S = PS->Surface();
      L = E.Location() * PS->Location();
      return;
    }
    itcr.Next();
  }
  
  L.Identity();
  P.Nullify();
  S.Nullify();
}

//=======================================================================
//function : PolygonOnSurface
//purpose  : 
//=======================================================================

void BRep_Tool::PolygonOnSurface(const TopoDS_Edge&      E,
                                 Handle(Poly_Polygon2D)& P,
                                 Handle(Geom_Surface)&   S,
                                 TopLoc_Location&        L,
                                 const Standard_Integer  Index)
{
  Standard_Integer i = 0;

  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygonOnSurface()) {
      const BRep_PolygonOnSurface* PS = static_cast<const BRep_PolygonOnSurface*>(cr.get());
      i++;
      if (i > Index) break;
      if (i == Index) {
        P = PS->Polygon();
        S = PS->Surface();
        L = E.Location() * PS->Location();
        return;
      }
    }
    itcr.Next();
  }
  
  L.Identity();
  P.Nullify();
  S.Nullify();
}

//=======================================================================
//function : PolygonOnTriangulation
//purpose  : Returns the polygon associated to the  edge in  the
//           parametric  space of  the  face.  Returns   a NULL
//           handle  if this polygon  does not exist.
//=======================================================================

static const Handle(Poly_PolygonOnTriangulation) nullArray;

const Handle(Poly_PolygonOnTriangulation)&
BRep_Tool::PolygonOnTriangulation(const TopoDS_Edge&                E, 
                                  const Handle(Poly_Triangulation)& T,
                                  const TopLoc_Location&            L)
{
  TopLoc_Location l = L.Predivided(E.Location());
  Standard_Boolean Eisreversed = (E.Orientation() == TopAbs_REVERSED);

  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if ( cr->IsPolygonOnTriangulation(T,l)) {
      if ( cr->IsPolygonOnClosedTriangulation() && Eisreversed )
        return cr->PolygonOnTriangulation2();
      else
        return cr->PolygonOnTriangulation();
    }
    itcr.Next();
  }
  
  return nullArray;
}

//=======================================================================
//function : PolygonOnTriangulation
//purpose  : 
//=======================================================================

void 
BRep_Tool::PolygonOnTriangulation(const TopoDS_Edge&                   E,
                                  Handle(Poly_PolygonOnTriangulation)& P,
                                  Handle(Poly_Triangulation)&          T,
                                  TopLoc_Location&                     L)
{
  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygonOnTriangulation()) {
      const BRep_PolygonOnTriangulation* PT =
        static_cast<const BRep_PolygonOnTriangulation*>(cr.get());
      P = PT->PolygonOnTriangulation();
      T = PT->Triangulation();
      L = E.Location() * PT->Location();
      return;
    }
    itcr.Next();
  }
  
  L.Identity();
  P.Nullify();
  T.Nullify();
}

//=======================================================================
//function : PolygonOnTriangulation
//purpose  : 
//=======================================================================

void 
BRep_Tool::PolygonOnTriangulation(const TopoDS_Edge&                   E,
                                  Handle(Poly_PolygonOnTriangulation)& P,
                                  Handle(Poly_Triangulation)&          T,
                                  TopLoc_Location&                     L,
                                  const Standard_Integer               Index)
{
  Standard_Integer i = 0;

  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygonOnTriangulation()) {
      const BRep_PolygonOnTriangulation* PT =
        static_cast<const BRep_PolygonOnTriangulation*>(cr.get());
      i++;
      if (i > Index) break;
      if (i == Index) {
        T = PT->Triangulation();
        P = PT->PolygonOnTriangulation();
        L = E.Location() * PT->Location();
        return;
      }
    }
    itcr.Next();
  }
  
  L.Identity();
  P.Nullify();
  T.Nullify();
}

//=======================================================================
//function : IsClosed
//purpose  : Returns  True  if  <E>  has  two  PCurves  in  the
//           parametric space of <F>. i.e.  <F>  is on a closed
//           surface and <E> is on the closing curve.
//=======================================================================

Standard_Boolean BRep_Tool::IsClosed(const TopoDS_Edge& E, 
                                     const TopoDS_Face& F)
{
  TopLoc_Location l;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,l);
  if (IsClosed(E,S,l)) return Standard_True;
  const Handle(Poly_Triangulation)& T = BRep_Tool::Triangulation(F,l);
  return IsClosed(E, T, l);
}

//=======================================================================
//function : IsClosed
//purpose  : Returns  True  if  <E>  has  two  PCurves  in  the
//           parametric space  of <S>.  i.e.   <S>  is a closed
//           surface and <E> is on the closing curve.
//=======================================================================

Standard_Boolean BRep_Tool::IsClosed(const TopoDS_Edge& E,
                                     const Handle(Geom_Surface)& S,
                                     const TopLoc_Location& L)
{
  //modified by NIZNHY-PKV Fri Oct 17 12:16:58 2008f
  if (IsPlane(S)) {
    return Standard_False;
  }
  //modified by NIZNHY-PKV Fri Oct 17 12:16:54 2008t
  //
  TopLoc_Location      l = L.Predivided(E.Location());

  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,l) &&
        cr->IsCurveOnClosedSurface())
      return Standard_True;
    itcr.Next();
  }
  return Standard_False;
}

//=======================================================================
//function : IsClosed
//purpose  : Returns  True  if <E> has two arrays of indices in
//           the triangulation <T>.
//=======================================================================

Standard_Boolean BRep_Tool::IsClosed(const TopoDS_Edge&                E, 
                                     const Handle(Poly_Triangulation)& T,
                                     const TopLoc_Location& L)
{
  TopLoc_Location l = L.Predivided(E.Location());

  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygonOnTriangulation(T,l) &&
        cr->IsPolygonOnClosedTriangulation()) 
      return Standard_True;
    itcr.Next();
  }
  return Standard_False;
}

//=======================================================================
//function : Tolerance
//purpose  : Returns the tolerance for <E>.
//=======================================================================

Standard_Real  BRep_Tool::Tolerance(const TopoDS_Edge& E)
{
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  Standard_Real p = TE->Tolerance();
  Standard_Real pMin = Precision::Confusion();
  if (p > pMin) return p;
  else          return pMin;
}

//=======================================================================
//function : SameParameter
//purpose  : Returns the SameParameter flag for the edge.
//=======================================================================

Standard_Boolean  BRep_Tool::SameParameter(const TopoDS_Edge& E)
{
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  return TE->SameParameter();
}

//=======================================================================
//function : SameRange
//purpose  : Returns the SameRange flag for the edge.
//=======================================================================

Standard_Boolean  BRep_Tool::SameRange(const TopoDS_Edge& E)
{
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  return TE->SameRange();
}

//=======================================================================
//function : Degenerated
//purpose  : Returns True  if the edge is degenerated.
//=======================================================================

Standard_Boolean  BRep_Tool::Degenerated(const TopoDS_Edge& E)
{
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  return TE->Degenerated();
}

//=======================================================================
//function : Range
//purpose  : 
//=======================================================================

void  BRep_Tool::Range(const TopoDS_Edge& E, 
                       Standard_Real& First, 
                       Standard_Real& Last)
{
  //  set the range to all the representations
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
  
  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurve3D()) {
      const BRep_Curve3D* CR = static_cast<const BRep_Curve3D*>(cr.get());
      if (!CR->Curve3D().IsNull()) {
        First = CR->First(); 
        Last = CR->Last();
        return;
      }
    }
    else if (cr->IsCurveOnSurface()) {
      const BRep_GCurve* CR = static_cast<const BRep_GCurve*>(cr.get());
      First = CR->First(); 
      Last = CR->Last();
      return;
    }
    itcr.Next();
  }
  First = Last = 0.;
}

//=======================================================================
//function : Range
//purpose  : 
//=======================================================================

void  BRep_Tool::Range(const TopoDS_Edge& E, 
                       const Handle(Geom_Surface)& S,
                       const TopLoc_Location& L,
                       Standard_Real& First, 
                       Standard_Real& Last)
{
  TopLoc_Location l = L.Predivided(E.Location());
  
  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
  
  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,l)) {
      const BRep_CurveOnSurface* CR = static_cast<const BRep_CurveOnSurface*>(cr.get());
      CR->Range(First,Last);
      break;
    }
    itcr.Next();
  }
  if (!itcr.More()) {
    Range(E,First,Last);
  }
  E.TShape()->Modified(Standard_True);
}

//=======================================================================
//function : Range
//purpose  : 
//=======================================================================

void  BRep_Tool::Range(const TopoDS_Edge& E, 
                       const TopoDS_Face& F, 
                       Standard_Real& First, 
                       Standard_Real& Last)
{
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,L);
  Range(E,S,L,First,Last);
}

//=======================================================================
//function : UVPoints
//purpose  : 
//=======================================================================

void  BRep_Tool::UVPoints(const TopoDS_Edge& E, 
                          const Handle(Geom_Surface)& S, 
                          const TopLoc_Location& L, 
                          gp_Pnt2d& PFirst, 
                          gp_Pnt2d& PLast)
{
  TopLoc_Location l = L.Predivided(E.Location());
  Standard_Boolean Eisreversed = (E.Orientation() == TopAbs_REVERSED);

  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,l)) {
      if (cr->IsCurveOnClosedSurface() && Eisreversed)
      {
        const BRep_CurveOnClosedSurface* CR =
          static_cast<const BRep_CurveOnClosedSurface*>(cr.get());
        CR->UVPoints2(PFirst, PLast);
      }
      else
      {
        const BRep_CurveOnSurface* CR =
          static_cast<const BRep_CurveOnSurface*>(cr.get());
        CR->UVPoints(PFirst, PLast);
      }
      return;
    }
    itcr.Next();
  }

  // for planar surface project the vertices
  // modif 21-05-97 : for RectangularTrimmedSurface, project the vertices
  Handle(Geom_Plane) GP;
  Handle(Geom_RectangularTrimmedSurface) GRTS;
  GRTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
  if(!GRTS.IsNull())
    GP = Handle(Geom_Plane)::DownCast(GRTS->BasisSurface());
  else
    GP = Handle(Geom_Plane)::DownCast(S);
  //fin modif du 21-05-97
  if (!GP.IsNull()) {
    // get the two vertices
    TopoDS_Vertex Vf,Vl;
    TopExp::Vertices(E,Vf,Vl);

    TopLoc_Location Linverted = L.Inverted();
    Vf.Move(Linverted);
    Vl.Move(Linverted);
    Standard_Real u,v;
    gp_Pln pln = GP->Pln();

    u=v=0.;
    if (!Vf.IsNull()) {
      gp_Pnt PF = BRep_Tool::Pnt(Vf);
      ElSLib::Parameters(pln,PF,u,v);
    }
    PFirst.SetCoord(u,v);

    u=v=0.;
    if (!Vl.IsNull()) {
      gp_Pnt PL = BRep_Tool::Pnt(Vl);
      ElSLib::Parameters(pln,PL,u,v);
    }
    PLast.SetCoord(u,v);
  }
  else
  {
    PFirst.SetCoord (0., 0.);
    PLast.SetCoord (0., 0.);
  }
}

//=======================================================================
//function : UVPoints
//purpose  : 
//=======================================================================

void  BRep_Tool::UVPoints(const TopoDS_Edge& E,
                          const TopoDS_Face& F, 
                          gp_Pnt2d& PFirst, 
                          gp_Pnt2d& PLast)
{
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,L);
  TopoDS_Edge aLocalEdge = E;
  if (F.Orientation() == TopAbs_REVERSED) {
    aLocalEdge.Reverse();
//    UVPoints(E,S,L,PFirst,PLast);
  }
//    UVPoints(TopoDS::Edge(E.Reversed()),S,L,PFirst,PLast);
//  else
//    UVPoints(E,S,L,PFirst,PLast);
  UVPoints(aLocalEdge,S,L,PFirst,PLast);
}

//=======================================================================
//function : SetUVPoints
//purpose  : 
//=======================================================================

void  BRep_Tool::SetUVPoints(const TopoDS_Edge& E,
                             const Handle(Geom_Surface)& S,
                             const TopLoc_Location& L, 
                             const gp_Pnt2d& PFirst, 
                             const gp_Pnt2d& PLast)
{
  TopLoc_Location l = L.Predivided(E.Location());
  Standard_Boolean Eisreversed = (E.Orientation() == TopAbs_REVERSED);

  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,l)) {
      if (cr->IsCurveOnClosedSurface() && Eisreversed)
      {
        BRep_CurveOnClosedSurface* CS = static_cast<BRep_CurveOnClosedSurface*>(cr.get());
        CS->SetUVPoints2(PFirst, PLast);
      }
      else
      {
        BRep_CurveOnSurface* CS = static_cast<BRep_CurveOnSurface*>(cr.get());
        CS->SetUVPoints(PFirst, PLast);
      }
    }
    itcr.Next();
  }
}

//=======================================================================
//function : SetUVPoints
//purpose  : 
//=======================================================================

void  BRep_Tool::SetUVPoints(const TopoDS_Edge& E,
                             const TopoDS_Face& F, 
                             const gp_Pnt2d& PFirst, 
                             const gp_Pnt2d& PLast)
{
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,L);
  TopoDS_Edge aLocalEdge = E;
  if (F.Orientation() == TopAbs_REVERSED) {
    aLocalEdge.Reverse();
//    SetUVPoints(TopoDS::Edge(E.Reversed()),S,L,PFirst,PLast);
  }
//  else
//    SetUVPoints(E,S,L,PFirst,PLast);
  SetUVPoints(aLocalEdge,S,L,PFirst,PLast);
}

//=======================================================================
//function : HasContinuity
//purpose  : Returns True if the edge is on the surfaces of the
//           two faces.
//=======================================================================

Standard_Boolean BRep_Tool::HasContinuity(const TopoDS_Edge& E, 
                                          const TopoDS_Face& F1, 
                                          const TopoDS_Face& F2)
{
  TopLoc_Location l1,l2;
  const Handle(Geom_Surface)& S1 = BRep_Tool::Surface(F1,l1);
  const Handle(Geom_Surface)& S2 = BRep_Tool::Surface(F2,l2);
  return HasContinuity(E,S1,S2,l1,l2);
}

//=======================================================================
//function : Continuity
//purpose  : Returns the continuity.
//=======================================================================

GeomAbs_Shape  BRep_Tool::Continuity(const TopoDS_Edge& E, 
                                     const TopoDS_Face& F1, 
                                     const TopoDS_Face& F2)
{
  TopLoc_Location l1,l2;
  const Handle(Geom_Surface)& S1 = BRep_Tool::Surface(F1,l1);
  const Handle(Geom_Surface)& S2 = BRep_Tool::Surface(F2,l2);
  return Continuity(E,S1,S2,l1,l2);
}

//=======================================================================
//function : HasContinuity
//purpose  : Returns True if the edge is on the surfaces.
//=======================================================================

Standard_Boolean BRep_Tool::HasContinuity(const TopoDS_Edge& E, 
                                          const Handle(Geom_Surface)& S1, 
                                          const Handle(Geom_Surface)& S2, 
                                          const TopLoc_Location& L1, 
                                          const TopLoc_Location& L2)
{
  const TopLoc_Location& Eloc = E.Location();
  TopLoc_Location l1 = L1.Predivided(Eloc);
  TopLoc_Location l2 = L2.Predivided(Eloc);

  // find the representation
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsRegularity(S1,S2,l1,l2))
      return Standard_True;
    itcr.Next();
  }
  return Standard_False;
}

//=======================================================================
//function : Continuity
//purpose  : Returns the continuity.
//=======================================================================

GeomAbs_Shape  BRep_Tool::Continuity(const TopoDS_Edge& E, 
                                     const Handle(Geom_Surface)& S1, 
                                     const Handle(Geom_Surface)& S2, 
                                     const TopLoc_Location& L1, 
                                     const TopLoc_Location& L2)
{
  TopLoc_Location l1 = L1.Predivided(E.Location());
  TopLoc_Location l2 = L2.Predivided(E.Location());

  // find the representation
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsRegularity(S1,S2,l1,l2))
      return cr->Continuity();
    itcr.Next();
  }
  return GeomAbs_C0;
}

//=======================================================================
//function : HasContinuity
//purpose  : Returns True if the edge is on some two surfaces.
//=======================================================================

Standard_Boolean BRep_Tool::HasContinuity(const TopoDS_Edge& E)
{
  const BRep_TEdge* TE = static_cast<const BRep_TEdge*>(E.TShape().get());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());

  for (; itcr.More(); itcr.Next())
  {
    const Handle(BRep_CurveRepresentation)& CR = itcr.Value();
    if (CR->IsRegularity())
      return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Pnt
//purpose  : Returns the 3d point.
//=======================================================================

gp_Pnt  BRep_Tool::Pnt(const TopoDS_Vertex& V)
{
  const BRep_TVertex* TV = static_cast<const BRep_TVertex*>(V.TShape().get());
  
  if (TV == 0)
  {
    throw Standard_NullObject("BRep_Tool:: TopoDS_Vertex hasn't gp_Pnt");
  }

  const gp_Pnt& P = TV->Pnt();
  if (V.Location().IsIdentity())
  {
    return P;
  }

  return P.Transformed(V.Location().Transformation());
}

//=======================================================================
//function : Tolerance
//purpose  : Returns the tolerance.
//=======================================================================

Standard_Real  BRep_Tool::Tolerance(const TopoDS_Vertex& V)
{
  const BRep_TVertex* aTVert = static_cast<const BRep_TVertex*>(V.TShape().get());

  if (aTVert == 0)
  {
    throw Standard_NullObject("BRep_Tool:: TopoDS_Vertex hasn't gp_Pnt");
  }

  Standard_Real p = aTVert->Tolerance();
  Standard_Real pMin = Precision::Confusion();
  if (p > pMin) return p;
  else          return pMin;
}

//=======================================================================
//function : Parameter
//purpose  : Returns the parameter of <V> on <E>.
//=======================================================================

Standard_Real  BRep_Tool::Parameter(const TopoDS_Vertex& V, 
                                    const TopoDS_Edge& E)
{
  
  // Search the vertex in the edge

  Standard_Boolean rev = Standard_False;
  TopoDS_Shape VF;
  TopAbs_Orientation orient = TopAbs_INTERNAL;

  TopoDS_Iterator itv(E.Oriented(TopAbs_FORWARD));

  // if the edge has no vertices
  // and is degenerated use the vertex orientation
  // RLE, june 94

  if (!itv.More() && Degenerated(E)) {
    orient = V.Orientation();
  }

  while (itv.More()) {
    const TopoDS_Shape& Vcur = itv.Value();
    if (V.IsSame(Vcur)) {
      if (VF.IsNull()) {
        VF = Vcur;
      }
      else {
        rev = E.Orientation() == TopAbs_REVERSED;
        if (Vcur.Orientation() == V.Orientation()) {
          VF = Vcur;
        }
      }
    }
    itv.Next();
  }
  
  if (!VF.IsNull()) orient = VF.Orientation();
 
  Standard_Real f,l;

  if (orient ==  TopAbs_FORWARD) {
    BRep_Tool::Range(E,f,l);
    return (rev) ? l : f;
  }
 
  else if (orient ==  TopAbs_REVERSED) {
    BRep_Tool::Range(E,f,l);
    return (rev) ? f : l;
   }

  else {
    TopLoc_Location L;
    const Handle(Geom_Curve)& C = BRep_Tool::Curve(E,L,f,l);
    L = L.Predivided(V.Location());
    if (!C.IsNull() || Degenerated(E)) {
      const BRep_TVertex* TV = static_cast<const BRep_TVertex*>(V.TShape().get());
      BRep_ListIteratorOfListOfPointRepresentation itpr(TV->Points());

      while (itpr.More()) {
        const Handle(BRep_PointRepresentation)& pr = itpr.Value();
        if (pr->IsPointOnCurve(C,L)) {
          Standard_Real p = pr->Parameter();
          Standard_Real res = p;// SVV 4 nov 99 - to avoid warnings on Linux
          if (!C.IsNull()) {
            // Closed curves RLE 16 june 94
            if (Precision::IsNegativeInfinite(f)) return pr->Parameter();//p;
            if (Precision::IsPositiveInfinite(l)) return pr->Parameter();//p;
            gp_Pnt Pf = C->Value(f).Transformed(L.Transformation());
            gp_Pnt Pl = C->Value(l).Transformed(L.Transformation());
            Standard_Real tol = BRep_Tool::Tolerance(V);
            if (Pf.Distance(Pl) < tol) {
              if (Pf.Distance(BRep_Tool::Pnt(V)) < tol) {
                if (V.Orientation() == TopAbs_FORWARD) res = f;//p = f;
                else                                   res = l;//p = l;
              }
            }
          }
          return res;//p;
        }
        itpr.Next();
      }
    }
    else {
      // no 3d curve !!
      // let us try with the first pcurve
      Handle(Geom2d_Curve) PC;
      Handle(Geom_Surface) S;
      BRep_Tool::CurveOnSurface(E,PC,S,L,f,l);
      L = L.Predivided(V.Location());
      const BRep_TVertex* TV = static_cast<const BRep_TVertex*>(V.TShape().get());
      BRep_ListIteratorOfListOfPointRepresentation itpr(TV->Points());

      while (itpr.More()) {
        const Handle(BRep_PointRepresentation)& pr = itpr.Value();
        if (pr->IsPointOnCurveOnSurface(PC,S,L)) {
          Standard_Real p = pr->Parameter();
          // Closed curves RLE 16 june 94
          if (PC->IsClosed()) {
            if ((p == PC->FirstParameter()) || 
                (p == PC->LastParameter())) {
              if (V.Orientation() == TopAbs_FORWARD) p = PC->FirstParameter();
              else                                   p = PC->LastParameter();
            }
          }
          return p;
        }
        itpr.Next();
      }
    }
  }
  
  throw Standard_NoSuchObject("BRep_Tool:: no parameter on edge");
}

//=======================================================================
//function : Parameter
//purpose  : Returns the  parameters  of   the  vertex   on the
//           pcurve of the edge on the face.
//=======================================================================

Standard_Real BRep_Tool::Parameter(const TopoDS_Vertex& V, 
                                   const TopoDS_Edge& E, 
                                   const TopoDS_Face& F)
{
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,L);
  return BRep_Tool::Parameter(V,E,S,L);
}

//=======================================================================
//function : Parameter
//purpose  : Returns the  parameters  of   the  vertex   on the
//           pcurve of the edge on the surface.
//=======================================================================

Standard_Real BRep_Tool::Parameter(const TopoDS_Vertex& V, 
                                   const TopoDS_Edge& E, 
                                   const Handle(Geom_Surface)& S,
                                   const TopLoc_Location& L)
{
  // Search the vertex in the edge

  Standard_Boolean rev = Standard_False;
  TopoDS_Shape VF;
  TopoDS_Iterator itv(E.Oriented(TopAbs_FORWARD));

  while (itv.More()) {
    if (V.IsSame(itv.Value())) {
      if (VF.IsNull()) VF = itv.Value();
      else {
        rev = E.Orientation() == TopAbs_REVERSED;
        if (itv.Value().Orientation() == V.Orientation()) 
        VF = itv.Value();
      }
    }
    itv.Next();
  }

 TopAbs_Orientation orient = TopAbs_INTERNAL;
  if (!VF.IsNull()) orient = VF.Orientation();
 
 Standard_Real f,l;

 if (orient ==  TopAbs_FORWARD) {
   BRep_Tool::Range(E,S,L,f,l);
   return (rev) ? l : f;
 }
 
 else if (orient ==  TopAbs_REVERSED) {
   BRep_Tool::Range(E,S,L,f,l);
   return (rev) ? f : l;
 }

 else {
   Handle(Geom2d_Curve) PC = BRep_Tool::CurveOnSurface(E,S,L,f,l);
   const BRep_TVertex* TV = static_cast<const BRep_TVertex*>(V.TShape().get());
   BRep_ListIteratorOfListOfPointRepresentation itpr(TV->Points());

   while (itpr.More()) {
     if (itpr.Value()->IsPointOnCurveOnSurface(PC,S,L))
       return itpr.Value()->Parameter();
     itpr.Next();
   }
 }

 //----------------------------------------------------------
   
  TopLoc_Location L1;
  const Handle(Geom_Curve)& C = BRep_Tool::Curve(E,L1,f,l);
  L1 = L1.Predivided(V.Location());
  if (!C.IsNull() || Degenerated(E)) {
    const BRep_TVertex* TV = static_cast<const BRep_TVertex*>(V.TShape().get());
    BRep_ListIteratorOfListOfPointRepresentation itpr(TV->Points());

    while (itpr.More()) {
      const Handle(BRep_PointRepresentation)& pr = itpr.Value();
      if (pr->IsPointOnCurve(C,L1)) {
        Standard_Real p = pr->Parameter();
        Standard_Real res = p;
        if (!C.IsNull()) {
          // Closed curves RLE 16 june 94
          if (Precision::IsNegativeInfinite(f)) return res;
          if (Precision::IsPositiveInfinite(l)) return res;
          gp_Pnt Pf = C->Value(f).Transformed(L1.Transformation());
          gp_Pnt Pl = C->Value(l).Transformed(L1.Transformation());
          Standard_Real tol = BRep_Tool::Tolerance(V);
          if (Pf.Distance(Pl) < tol) {
            if (Pf.Distance(BRep_Tool::Pnt(V)) < tol) {
              if (V.Orientation() == TopAbs_FORWARD) res = f;
              else                                   res = l;
            }
          }
        }
        return res;
      }
      itpr.Next();
    }
  }
  
//----------------------------------------------------------   
 
  throw Standard_NoSuchObject("BRep_Tool:: no parameter on edge");
}

//=======================================================================
//function : Parameters
//purpose  : Returns the parameters of the vertex on the face.
//=======================================================================

gp_Pnt2d  BRep_Tool::Parameters(const TopoDS_Vertex& V, 
                                const TopoDS_Face& F)
{
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,L);
  L = L.Predivided(V.Location());
  const BRep_TVertex* TV = static_cast<const BRep_TVertex*>(V.TShape().get());
  BRep_ListIteratorOfListOfPointRepresentation itpr(TV->Points());
  
  // It is checked if there is PointRepresentation (case non Manifold)
  while (itpr.More()) {
    if (itpr.Value()->IsPointOnSurface(S,L)) {
      return gp_Pnt2d(itpr.Value()->Parameter(),
                      itpr.Value()->Parameter2());
    }
    itpr.Next();
  }

 TopoDS_Vertex Vf,Vl;
 TopoDS_Edge E;
 // Otherwise the edges are searched (PMN 4/06/97) It is not possible to succeed 999/1000!
 // even if often there is a way to make more economically than above...
 TopExp_Explorer exp;
 for (exp.Init(F, TopAbs_EDGE); exp.More(); exp.Next()) { 
    E = TopoDS::Edge(exp.Current());  
    TopExp::Vertices(E, Vf, Vl);
    if ((V.IsSame(Vf)) || (V.IsSame(Vl))) {
      gp_Pnt2d Pf, Pl;
      UVPoints(E, F, Pf, Pl);
      if (V.IsSame(Vf)) return Pf;
      else              return Pl;//Ambiguity (natural) for degenerated edges.
    }
  }
  throw Standard_NoSuchObject("BRep_Tool:: no parameters on surface");
}
//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================
Standard_Boolean BRep_Tool::IsClosed (const TopoDS_Shape& theShape)
{
  if (theShape.ShapeType() == TopAbs_SHELL)
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMap (101, new NCollection_IncAllocator);
    TopExp_Explorer exp (theShape.Oriented(TopAbs_FORWARD), TopAbs_EDGE);
    Standard_Boolean hasBound = Standard_False;
    for (; exp.More(); exp.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
      if (BRep_Tool::Degenerated(E) || E.Orientation() == TopAbs_INTERNAL || E.Orientation() == TopAbs_EXTERNAL)
        continue;
      hasBound = Standard_True;
      if (!aMap.Add(E))
        aMap.Remove(E);
    }
    return hasBound && aMap.IsEmpty();
  }
  else if (theShape.ShapeType() == TopAbs_WIRE)
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMap (101, new NCollection_IncAllocator);
    TopExp_Explorer exp (theShape.Oriented(TopAbs_FORWARD), TopAbs_VERTEX);
    Standard_Boolean hasBound = Standard_False;
    for (; exp.More(); exp.Next())
    {
      const TopoDS_Shape& V = exp.Current();
      if (V.Orientation() == TopAbs_INTERNAL || V.Orientation() == TopAbs_EXTERNAL)
        continue;
      hasBound = Standard_True;
      if (!aMap.Add(V))
        aMap.Remove(V);
    }
    return hasBound && aMap.IsEmpty();
  }
  else if (theShape.ShapeType() == TopAbs_EDGE)
  {
    TopoDS_Vertex aVFirst, aVLast;
    TopExp::Vertices(TopoDS::Edge(theShape), aVFirst, aVLast);
    return !aVFirst.IsNull() && aVFirst.IsSame(aVLast);
  }
  return theShape.Closed();
}

//modified by NIZNHY-PKV Fri Oct 17 14:09:58 2008 f 
//=======================================================================
//function : IsPlane
//purpose  : 
//=======================================================================
Standard_Boolean IsPlane(const Handle(Geom_Surface)& aS)
{
  Standard_Boolean bRet;
  Handle(Geom_Plane) aGP;
  Handle(Geom_RectangularTrimmedSurface) aGRTS;
  Handle(Geom_OffsetSurface) aGOFS;
  //
  aGRTS=Handle(Geom_RectangularTrimmedSurface)::DownCast(aS);
  aGOFS=Handle(Geom_OffsetSurface)::DownCast(aS);
  //
  if(!aGOFS.IsNull()) {
    aGP=Handle(Geom_Plane)::DownCast(aGOFS->BasisSurface());
  }
  else if(!aGRTS.IsNull()) {
    aGP=Handle(Geom_Plane)::DownCast(aGRTS->BasisSurface());
  }
  else {
    aGP=Handle(Geom_Plane)::DownCast(aS);
  }
  //
  bRet=!aGP.IsNull();
  //
  return bRet;
}

//=======================================================================
//function : MaxTolerance
//purpose  : 
//=======================================================================
Standard_Real BRep_Tool::MaxTolerance (const TopoDS_Shape& theShape,
                                       const TopAbs_ShapeEnum theSubShape)
{
  Standard_Real aTol = 0.0;

  // Explorer Shape-Subshape.
  TopExp_Explorer anExpSS(theShape, theSubShape);
  if (theSubShape == TopAbs_FACE)
  {
    for( ; anExpSS.More() ;  anExpSS.Next() )
    {
      const TopoDS_Shape& aCurrentSubShape = anExpSS.Current();
      aTol = Max(aTol, Tolerance(TopoDS::Face(aCurrentSubShape)));
    }
  }
  else if (theSubShape == TopAbs_EDGE)
  {
    for( ; anExpSS.More() ;  anExpSS.Next() )
    {
      const TopoDS_Shape& aCurrentSubShape = anExpSS.Current();
      aTol = Max(aTol, Tolerance(TopoDS::Edge(aCurrentSubShape)));
    }
  }
  else if (theSubShape == TopAbs_VERTEX)
  {
    for( ; anExpSS.More() ;  anExpSS.Next() )
    {
      const TopoDS_Shape& aCurrentSubShape = anExpSS.Current();
      aTol = Max(aTol, Tolerance(TopoDS::Vertex(aCurrentSubShape)));
    }
  }

  return aTol;
}
