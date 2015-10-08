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
  Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*) &F.TShape());
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
  Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*) &F.TShape());
  TopLoc_Location L = F.Location() * TF->Location();
  Handle(Geom_Surface) S = TF->Surface();

  if(S.IsNull()) return S;

  Handle(Geom_Geometry) S1;
  if (!L.IsIdentity()) {
    S1 = S->Copy();
    S = Handle(Geom_Surface)::DownCast (S1);
    S->Transform(L.Transformation());
  }
  return S;
}

//=======================================================================
//function : Triangulation
//purpose  : Returns  the Triangulation of  the  face. It  is a
//           null handle if there is no triangulation.
//=======================================================================

const Handle(Poly_Triangulation)&
BRep_Tool::Triangulation(const TopoDS_Face& F,
                         TopLoc_Location&   L)
{
  L = F.Location();
  return (*((Handle(BRep_TFace)*)&F.TShape()))->Triangulation();
}

//=======================================================================
//function : Tolerance
//purpose  : Returns the tolerance of the face.
//=======================================================================

Standard_Real  BRep_Tool::Tolerance(const TopoDS_Face& F)
{
  Standard_Real p = (*((Handle(BRep_TFace)*)&F.TShape()))->Tolerance();
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
  return (*((Handle(BRep_TFace)*) &F.TShape()))->NaturalRestriction();
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurve3D()) {
      Handle(BRep_Curve3D) GC (Handle(BRep_Curve3D)::DownCast (cr));
      L = E.Location() * GC->Location();
      GC->Range(First,Last);
      return GC->Curve3D();
    }
    itcr.Next();
  }
  L.Identity();
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
  Handle(Geom_Curve) C = Curve(E,L,First,Last);
  if ( !C.IsNull() ) {
    Handle(Geom_Geometry) C1;
    if ( !L.IsIdentity() ) {
      C1 = C->Copy();
      C = Handle(Geom_Curve)::DownCast (C1);
      C->Transform(L.Transformation());
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurve3D()) {
      Standard_Real first, last;
      TopLoc_Location L;
      const Handle(Geom_Curve)&  C = BRep_Tool::Curve(E, L, first, last);
      if (!C.IsNull()) return Standard_True;
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygon3D()) {
      Handle(BRep_Polygon3D) GC (Handle(BRep_Polygon3D)::DownCast (cr));
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,loc)) {
      Handle(BRep_GCurve) GC (Handle(BRep_GCurve)::DownCast (cr));
      GC->Range(First,Last);
      if (GC->IsCurveOnClosedSurface() && Eisreversed)
        return GC->PCurve2();
      else
        return GC->PCurve();
    }
    itcr.Next();
  }

  // for planar surface and 3d curve try a projection
  // modif 21-05-97 : for RectangularTrimmedSurface, try a projection
  Handle(Geom_Plane) GP;
  Handle(Geom_RectangularTrimmedSurface) GRTS;
  GRTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
  if(!GRTS.IsNull())
    GP = Handle(Geom_Plane)::DownCast(GRTS->BasisSurface());
  else
    GP = Handle(Geom_Plane)::DownCast(S);
  //fin modif du 21-05-97

  if (!GP.IsNull())
  {
    Handle(GeomAdaptor_HCurve) HC;
    Handle(GeomAdaptor_HSurface) HS;

    HC = new GeomAdaptor_HCurve();
    HS = new GeomAdaptor_HSurface();

    TopLoc_Location aCurveLocation;

    Standard_Real f, l;// for those who call with (u,u).
    Handle(Geom_Curve) C3d = BRep_Tool::Curve(E, aCurveLocation, f, l);

    if (C3d.IsNull())
    {
      return nullPCurve;
    }

    aCurveLocation = aCurveLocation.Predivided(L);
    First = f; Last = l; //Range of edge must not be modified

    if (!aCurveLocation.IsIdentity())
    {
      const gp_Trsf& T = aCurveLocation.Transformation();
      Handle(Geom_Geometry) GC3d = C3d->Transformed(T);
      C3d = Handle(Geom_Curve)::DownCast (GC3d);
      f = C3d->TransformedParameter(f, T);
      l = C3d->TransformedParameter(l, T);
    }
    GeomAdaptor_Surface& GAS = HS->ChangeSurface();
    GAS.Load(GP);

    Handle(Geom_Curve) ProjOnPlane = 
      GeomProjLib::ProjectOnPlane(new Geom_TrimmedCurve(C3d,f,l,Standard_True,Standard_False),
                                  GP,
                                  GP->Position().Direction(),
                                  Standard_True);

    GeomAdaptor_Curve& GAC = HC->ChangeCurve();
    GAC.Load(ProjOnPlane);

    ProjLib_ProjectedCurve Proj(HS,HC);
    Handle(Geom2d_Curve) pc = Geom2dAdaptor::MakeCurve(Proj);

    if (pc->DynamicType() == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
      Handle(Geom2d_TrimmedCurve) TC = 
        Handle(Geom2d_TrimmedCurve)::DownCast (pc);
      pc = TC->BasisCurve();
    }

    return pc;
  }
  
  return nullPCurve;
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface()) {
      Handle(BRep_GCurve) GC (Handle(BRep_GCurve)::DownCast (cr));
      C = GC->PCurve();
      S = GC->Surface();
      L = E.Location() * GC->Location();
      GC->Range(First,Last);
      return;
    }
    itcr.Next();
  }
  
  C = Handle(Geom2d_Curve)();
  S = Handle(Geom_Surface)();
  L = TopLoc_Location();
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
  Standard_Integer i = 0;
  Standard_Boolean Eisreversed = (E.Orientation() == TopAbs_REVERSED);

  // find the representation
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface()) {
      Handle(BRep_GCurve) GC (Handle(BRep_GCurve)::DownCast (cr));
      i++;
      if (i > Index) break;
      if (i == Index) {
        // JMB le 21 Mai 1999
        // it is done as in the other CurveOnSurface methods, ie. take into account
        // the orientation in case of cut edges (return PCurve2)
        // otherwise there is a risk to loop curves or to not get the prover one.
        if (GC->IsCurveOnClosedSurface() && Eisreversed)
          C = GC->PCurve2();
        else
          C = GC->PCurve();
        S = GC->Surface();
        L = E.Location() * GC->Location();
        GC->Range(First,Last);
        return;
      }
    }
    itcr.Next();
  }
  
  C = Handle(Geom2d_Curve)();
  S = Handle(Geom_Surface)();
  L = TopLoc_Location();
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

Handle(Poly_Polygon2D) 
     BRep_Tool::PolygonOnSurface(const TopoDS_Edge& E, 
                                 const Handle(Geom_Surface)& S,
                                 const TopLoc_Location& L)
{
  TopLoc_Location l = L.Predivided(E.Location());
  Standard_Boolean Eisreversed = (E.Orientation() == TopAbs_REVERSED);

  // find the representation
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygonOnSurface()) {
      Handle(BRep_PolygonOnSurface) PS (Handle(BRep_PolygonOnSurface)::DownCast (cr));
      P = PS->Polygon();
      S = PS->Surface();
      L = E.Location() * PS->Location();
      return;
    }
    itcr.Next();
  }
  
  L = TopLoc_Location();
  P = Handle(Poly_Polygon2D)();
  S = Handle(Geom_Surface)();
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygonOnSurface()) {
      Handle(BRep_PolygonOnSurface) PS (Handle(BRep_PolygonOnSurface)::DownCast (cr));
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
  
  L = TopLoc_Location();
  P = Handle(Poly_Polygon2D)();
  S = Handle(Geom_Surface)();
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygonOnTriangulation()) {
      Handle(BRep_PolygonOnTriangulation) PT (Handle(BRep_PolygonOnTriangulation)::DownCast (cr));
      P = PT->PolygonOnTriangulation();
      T = PT->Triangulation();
      L = E.Location() * PT->Location();
      return;
    }
    itcr.Next();
  }
  
  L = TopLoc_Location();
  P = Handle(Poly_PolygonOnTriangulation)();
  T = Handle(Poly_Triangulation)();
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsPolygonOnTriangulation()) {
      Handle(BRep_PolygonOnTriangulation) PT (Handle(BRep_PolygonOnTriangulation)::DownCast (cr));
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
  
  L = TopLoc_Location();
  P = Handle(Poly_PolygonOnTriangulation)();
  T = Handle(Poly_Triangulation)();
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

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
  Standard_Real p = (*((Handle(BRep_TEdge)*)&E.TShape()))->Tolerance();
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
  return (*((Handle(BRep_TEdge)*)&E.TShape()))->SameParameter();
}

//=======================================================================
//function : SameRange
//purpose  : Returns the SameRange flag for the edge.
//=======================================================================

Standard_Boolean  BRep_Tool::SameRange(const TopoDS_Edge& E)
{
  return (*((Handle(BRep_TEdge)*)&E.TShape()))->SameRange();
}

//=======================================================================
//function : Degenerated
//purpose  : Returns True  if the edge is degenerated.
//=======================================================================

Standard_Boolean  BRep_Tool::Degenerated(const TopoDS_Edge& E)
{
  return (*((Handle(BRep_TEdge)*)&E.TShape()))->Degenerated();
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurve3D()) {
      Handle(BRep_Curve3D) CR (Handle(BRep_Curve3D)::DownCast (cr));
      if (!CR->Curve3D().IsNull()) {
        First = CR->First(); 
        Last = CR->Last();
        break;
      }
    }
    else if (cr->IsCurveOnSurface()) {
      Handle(BRep_GCurve) CR (Handle(BRep_GCurve)::DownCast (cr));
      First = CR->First(); 
      Last = CR->Last();
      break;
    }
    itcr.Next();
  }
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());
  
  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,l)) {
      Handle(BRep_GCurve)::DownCast (cr)->Range(First,Last);
      break;
    }
    itcr.Next();
  }
  if (!itcr.More()) {
    Range(E,First,Last);
  }
  (*((Handle(BRep_TEdge)*)&E.TShape()))->Modified(Standard_True);
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,l)) {
      if (cr->IsCurveOnClosedSurface() && Eisreversed)
        Handle(BRep_CurveOnClosedSurface)::DownCast (cr)->UVPoints2(PFirst,PLast);
      else
        Handle(BRep_CurveOnSurface)::DownCast (cr)->UVPoints(PFirst,PLast);
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,l)) {
      if (cr->IsCurveOnClosedSurface() && Eisreversed)
        Handle(BRep_CurveOnClosedSurface)::DownCast (cr)->
          SetUVPoints2(PFirst,PLast);
      else
        Handle(BRep_CurveOnSurface)::DownCast (cr)->
          SetUVPoints(PFirst,PLast);
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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

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
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->Curves());

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
  Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*) &V.TShape());

  if (TV.IsNull())
  {
    Standard_NullObject::Raise("BRep_Tool:: TopoDS_Vertex hasn't gp_Pnt");
  }

  gp_Pnt P = TV->Pnt();
  P.Transform(V.Location().Transformation());
  return P;
}

//=======================================================================
//function : Tolerance
//purpose  : Returns the tolerance.
//=======================================================================

Standard_Real  BRep_Tool::Tolerance(const TopoDS_Vertex& V)
{
  Handle(BRep_TVertex)& aTVert = *((Handle(BRep_TVertex)*)&V.TShape());

  if (aTVert.IsNull())
  {
    Standard_NullObject::Raise("BRep_Tool:: TopoDS_Vertex hasn't gp_Pnt");
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
      BRep_ListIteratorOfListOfPointRepresentation itpr
        ((*((Handle(BRep_TVertex)*) &V.TShape()))->Points());

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
      BRep_ListIteratorOfListOfPointRepresentation itpr
        ((*((Handle(BRep_TVertex)*) &V.TShape()))->Points());

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
  
  Standard_NoSuchObject::Raise("BRep_Tool:: no parameter on edge");
  return 0;
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
   BRep_ListIteratorOfListOfPointRepresentation itpr
     ((*((Handle(BRep_TVertex)*) &V.TShape()))->Points());

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
    BRep_ListIteratorOfListOfPointRepresentation itpr
      ((*((Handle(BRep_TVertex)*) &V.TShape()))->Points());

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
 
  Standard_NoSuchObject::Raise("BRep_Tool:: no parameter on edge");
  return 0;
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
  BRep_ListIteratorOfListOfPointRepresentation itpr
    ((*((Handle(BRep_TVertex)*) &V.TShape()))->Points());
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
  Standard_NoSuchObject::Raise("BRep_Tool:: no parameters on surface");
  return gp_Pnt2d(0,0);
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
