// Created on: 1999-06-17
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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


#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <gp_Pnt.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeConstruct.hxx>
#include <ShapeCustom_ConvertToBSpline.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeCustom_ConvertToBSpline,ShapeCustom_Modification)

//=======================================================================
//function : ShapeCustom_ConvertToBSpline
//purpose  : 
//=======================================================================
ShapeCustom_ConvertToBSpline::ShapeCustom_ConvertToBSpline():
       myExtrMode(Standard_True), myRevolMode(Standard_True),
       myOffsetMode(Standard_True), myPlaneMode(Standard_False)
{
}

//=======================================================================
//function : IsToConvert
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_ConvertToBSpline::IsToConvert(const Handle(Geom_Surface) &S,
							   Handle(Geom_Surface) &SS) const
{
  SS = S;
  if(S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) RTS = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast ( S );
    SS = RTS->BasisSurface();
  }
  if(SS->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    if(myOffsetMode)
      return Standard_True;
    else {
      Handle(Geom_OffsetSurface) OS = Handle(Geom_OffsetSurface)::DownCast ( SS );
      Handle(Geom_Surface) basis = OS->BasisSurface();
      Handle(Geom_Surface) tmp;
      return IsToConvert(basis,tmp);
    }
  }
  if ( SS->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) )
    return myExtrMode;
  if ( SS->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)) )
    return myRevolMode;
  if ( SS->IsKind(STANDARD_TYPE(Geom_Plane)) )
    return myPlaneMode;  
  return Standard_False;
}
    
//=======================================================================
//function : NewSurface
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_ConvertToBSpline::NewSurface (const TopoDS_Face& F,
							   Handle(Geom_Surface)& S,
							   TopLoc_Location& L,
							   Standard_Real& Tol,
							   Standard_Boolean& RevWires,
							   Standard_Boolean& RevFace) 
{
  S = BRep_Tool::Surface(F,L);
  Standard_Real U1,U2,V1,V2;
  S->Bounds(U1,U2,V1,V2);
  Standard_Real Umin, Umax,Vmin,Vmax;
  BRepTools::UVBounds(F,Umin, Umax, Vmin, Vmax);
  if(Precision::IsInfinite(U1) || Precision::IsInfinite(U2)) {
    U1 = Umin;
    U2 = Umax;
  }
  if(Precision::IsInfinite(V1) || Precision::IsInfinite(V2)) {
    V1 = Vmin;
    V2 = Vmax;
  }
   
  Handle(Geom_Surface) surf;
  if ( ! IsToConvert ( S, surf) ) return Standard_False;
  
  Handle(Geom_Surface) res;
  if(surf->IsKind(STANDARD_TYPE(Geom_OffsetSurface))&&!myOffsetMode) {
    Handle(Geom_OffsetSurface) OS = Handle(Geom_OffsetSurface)::DownCast ( surf );
    Handle(Geom_Surface) basis = OS->BasisSurface();
    Standard_Real offset = OS->Offset();
    Handle(Geom_BSplineSurface) bspl = ShapeConstruct::
      ConvertSurfaceToBSpline(basis,U1,U2,V1,V2,Precision::Approximation(),
				surf->Continuity(),10000,15);
    Handle(Geom_OffsetSurface) nOff = new Geom_OffsetSurface(bspl,offset);
    res = nOff;
  }
  else {
    GeomAbs_Shape cnt = surf->Continuity();
    if(surf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
      cnt = GeomAbs_C0; //pdn 30.06.99 because of hang-up in GeomConvert_ApproxSurface
    res = ShapeConstruct::ConvertSurfaceToBSpline(surf,U1,U2,V1,V2,Precision::Approximation(),
						  cnt,10000,15);
  }
  if(S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)) ) {
    Handle(Geom_RectangularTrimmedSurface) RTS = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast ( S );
    Standard_Real UF, UL, VF, VL;
    RTS->Bounds ( UF, UL, VF, VL );
    S = new Geom_RectangularTrimmedSurface ( res, UF, UL, VF, VL );
  }
  else 
    S = res;
  
  SendMsg( F, Message_Msg("ConvertToBSpline.NewSurface.MSG0"));

  Tol = BRep_Tool::Tolerance(F);
  RevWires = Standard_False;
  RevFace = Standard_False;
  return Standard_True;
}
  
//=======================================================================
//function : NewCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_ConvertToBSpline::NewCurve (const TopoDS_Edge& E,
							 Handle(Geom_Curve)& C,
							 TopLoc_Location& L,
							 Standard_Real& Tol) 
{
  //:p5 abv 26 Feb 99: force copying of edge if any its pcurve will be replaced
  Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&E.TShape());

  // iterate on pcurves
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
  for ( ; itcr.More(); itcr.Next() ) {
    Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itcr.Value());
    if ( GC.IsNull() || ! GC->IsCurveOnSurface() ) continue;
    Handle(Geom_Surface) S = GC->Surface();
    Handle(Geom_Surface) ES;
    if ( ! IsToConvert ( S, ES ) ) continue;
    Standard_Real f, l;
    C = BRep_Tool::Curve ( E, L, f, l );
    if ( ! C.IsNull() ) C = Handle(Geom_Curve)::DownCast ( C->Copy() );
    Tol = BRep_Tool::Tolerance ( E );
    SendMsg( E, Message_Msg("ConvertToBSpline.NewCurve.MSG0"));
    return Standard_True;
  } 
  return Standard_False;
}

//=======================================================================
//function : NewPoint
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_ConvertToBSpline::NewPoint (const TopoDS_Vertex& /*V*/,
							 gp_Pnt& /*P*/, Standard_Real& /*Tol*/)
{
  return Standard_False;
}

//=======================================================================
//function : NewCurve2d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_ConvertToBSpline::NewCurve2d (const TopoDS_Edge& E,
							   const TopoDS_Face& F,
							   const TopoDS_Edge& NewE,
							   const TopoDS_Face& /*NewF*/,
							   Handle(Geom2d_Curve)& C,
							   Standard_Real& Tol) 
{
  TopLoc_Location L;
  Handle(Geom_Surface) S = BRep_Tool::Surface(F,L);
  Handle(Geom_Surface) ES;
  
  // just copy pcurve if either its surface is changing or edge was copied
  if ( ! IsToConvert ( S, ES ) && E.IsSame ( NewE ) ) return Standard_False;
  
  Standard_Real f, l;
  C = BRep_Tool::CurveOnSurface(E,F,f,l);
  if ( ! C.IsNull() ) 
    C = Handle(Geom2d_Curve)::DownCast ( C->Copy() );
  
  Tol = BRep_Tool::Tolerance ( E );
  return Standard_True;
}

//=======================================================================
//function : NewParameter
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_ConvertToBSpline::NewParameter (const TopoDS_Vertex& /*V*/,
							     const TopoDS_Edge& /*E*/,
							     Standard_Real& /*P*/,
							     Standard_Real& /*Tol*/)
{
  return Standard_False;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape ShapeCustom_ConvertToBSpline::Continuity (const TopoDS_Edge& E,
							const TopoDS_Face& F1,
							const TopoDS_Face& F2,
							const TopoDS_Edge& /*NewE*/,
							const TopoDS_Face& /*NewF1*/,
							const TopoDS_Face& /*NewF2*/)
{
  return BRep_Tool::Continuity(E,F1,F2);
}

void ShapeCustom_ConvertToBSpline::SetExtrusionMode(const Standard_Boolean extrMode)
{
  myExtrMode = extrMode;
}

void ShapeCustom_ConvertToBSpline::SetRevolutionMode(const Standard_Boolean revolMode)
{
  myRevolMode = revolMode;
}

void ShapeCustom_ConvertToBSpline::SetOffsetMode(const Standard_Boolean offsetMode)
{
  myOffsetMode = offsetMode;
}

void ShapeCustom_ConvertToBSpline::SetPlaneMode(const Standard_Boolean planeMode)
{
  myPlaneMode = planeMode;
}
