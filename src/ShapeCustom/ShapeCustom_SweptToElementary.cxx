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


#include <GeomAdaptor_SurfaceOfLinearExtrusion.hxx>
#include <GeomAdaptor_SurfaceOfRevolution.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <Message_Msg.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeCustom_SweptToElementary.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeCustom_SweptToElementary,ShapeCustom_Modification)

//=======================================================================
//function : ShapeCustom_SweptToElementary
//purpose  : 
//=======================================================================
ShapeCustom_SweptToElementary::ShapeCustom_SweptToElementary()
{
}


//=======================================================================
//function : IsToConvert
//purpose  : auxilary (Analyze surface: is it to be converted?)
//=======================================================================
static Standard_Boolean IsToConvert (const Handle(Geom_Surface) &S,
				     Handle(Geom_SweptSurface) &SS)
{
  Handle(Geom_Surface) Stmp;

  if(S->IsKind(STANDARD_TYPE(Geom_SweptSurface))) {
    SS = Handle(Geom_SweptSurface)::DownCast(S);
    return Standard_True;
  }
  if(S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) RTS = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
    Stmp = RTS->BasisSurface();
  }
  else if(S->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    Handle(Geom_OffsetSurface) OS = Handle(Geom_OffsetSurface)::DownCast(S);
    Stmp = OS->BasisSurface();
  }
  if(Stmp.IsNull() ) return Standard_False;
  if(S->IsKind(STANDARD_TYPE(Geom_SweptSurface))) {
    SS = Handle(Geom_SweptSurface)::DownCast(Stmp);
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : NewSurface
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_SweptToElementary::NewSurface(const TopoDS_Face& F,
                                                           Handle(Geom_Surface)& S,
                                                           TopLoc_Location& L,
                                                           Standard_Real& Tol,
                                                           Standard_Boolean& RevWires,
                                                           Standard_Boolean& RevFace) 
{
  S = BRep_Tool::Surface(F,L);
  Handle(Geom_SweptSurface) SS;
  if(!IsToConvert(S,SS)) return Standard_False;

  // case SurfaceOfRevolution
  if(SS->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
    Handle(Geom_SurfaceOfRevolution) SR = Handle(Geom_SurfaceOfRevolution)::DownCast(SS);
    Handle(Geom_Curve) bc = SR->BasisCurve();
    gp_Ax1 ax1 = SR->Axis();
    Handle(GeomAdaptor_HCurve) HC = new GeomAdaptor_HCurve();
    HC->ChangeCurve().Load(bc,bc->FirstParameter(),bc->LastParameter());
    GeomAdaptor_SurfaceOfRevolution AS(HC,ax1);
    switch(AS.GetType()){
    // skl 18.12.2003 - plane not used, problems in PRO14665.igs
    //case GeomAbs_Plane : {
    //  Handle(Geom_Plane) Pl = new Geom_Plane(AS.Plane());
    //  S = Pl;
    //} break;
    case GeomAbs_Cylinder : {
      Handle(Geom_CylindricalSurface) Cy = 
        new Geom_CylindricalSurface(AS.Cylinder());
      S = Cy;
    } break;
    case GeomAbs_Sphere : {
      Handle(Geom_SphericalSurface) Sp = 
        new Geom_SphericalSurface(AS.Sphere());
      S = Sp;
    } break;
    case GeomAbs_Cone : {
      Handle(Geom_ConicalSurface) Co = 
        new Geom_ConicalSurface(AS.Cone());
      S = Co;
    } break;
    case GeomAbs_Torus : {
      Handle(Geom_ToroidalSurface) To = 
        new Geom_ToroidalSurface(AS.Torus());
      S = To;
    } break;
    default : return Standard_False; break;
    }
  }
  // case SurfaceOfLinearExtrusion
  else if(SS->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) {
    Handle(Geom_SurfaceOfLinearExtrusion) SLE = 
      Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(SS);
    Handle(Geom_Curve) bc = SLE->BasisCurve();
    gp_Dir dir = SLE->Direction();
    Handle(GeomAdaptor_HCurve) HC = new GeomAdaptor_HCurve();
    HC->ChangeCurve().Load(bc,bc->FirstParameter(),bc->LastParameter());
    GeomAdaptor_SurfaceOfLinearExtrusion AS(HC,dir);
    switch(AS.GetType()){
    // skl 18.12.2003 - plane not used, problems in ims013.igs
    //case GeomAbs_Plane : {
    //  Handle(Geom_Plane) Pl = new Geom_Plane(AS.Plane());
    //  S = Pl;
    //} break;
    case GeomAbs_Cylinder : {
      Handle(Geom_CylindricalSurface) Cy = 
        new Geom_CylindricalSurface(AS.Cylinder());
      S = Cy;
    } break;
    default : return Standard_False; break;
    }
  }

  SendMsg( F, Message_Msg("SweptToElementary.NewSurface.MSG0"));

  Tol = BRep_Tool::Tolerance(F);
  RevWires = Standard_False;
  RevFace = Standard_False;
  return Standard_True;
}


//=======================================================================
//function : NewCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_SweptToElementary::NewCurve(const TopoDS_Edge& E,
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
    Handle(Geom_SweptSurface) SS;
    if(!IsToConvert(S,SS)) continue;
    Standard_Real f, l;
    C = BRep_Tool::Curve ( E, L, f, l );
    if ( ! C.IsNull() ) C = Handle(Geom_Curve)::DownCast ( C->Copy() );
    Tol = BRep_Tool::Tolerance ( E );
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : NewPoint
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_SweptToElementary::NewPoint(const TopoDS_Vertex& /*V*/,
                                                         gp_Pnt& /*P*/,Standard_Real& /*Tol*/) 
{
  // 3d points are never modified
  return Standard_False;
}


//=======================================================================
//function : NewCurve2d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_SweptToElementary::NewCurve2d(const TopoDS_Edge& E,
                                                           const TopoDS_Face& F,
                                                           const TopoDS_Edge& NewE,
                                                           const TopoDS_Face& NewF,
                                                           Handle(Geom2d_Curve)& C,
                                                           Standard_Real& Tol) 
{
  TopLoc_Location L;
  Handle(Geom_Surface) S = BRep_Tool::Surface(F,L);
  Handle(Geom_SweptSurface) SS;

  // just copy pcurve if either its surface is changing or edge was copied
  if ( !IsToConvert(S,SS) && E.IsSame(NewE) ) return Standard_False;
  
  Standard_Real f, l;
  C = BRep_Tool::CurveOnSurface(E,F,f,l);
  if ( ! C.IsNull() ) {
    C = Handle(Geom2d_Curve)::DownCast ( C->Copy() );
    Handle(Geom_Surface) NS = BRep_Tool::Surface(NewF,L);
    if ( !NS.IsNull() && NS->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)) ) {
      if(SS->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
        Handle(Geom_SurfaceOfRevolution) SR = Handle(Geom_SurfaceOfRevolution)::DownCast(SS);
        Standard_Real U1,U2,V1,V2;
        SR->Bounds(U1,U2,V1,V2);
        gp_Pnt P0;
        SR->D0(U1,V1,P0);
        Handle(ShapeAnalysis_Surface) sas = new ShapeAnalysis_Surface(NS);
        gp_Pnt2d p2d = sas->ValueOfUV(P0,Precision::Confusion());
        gp_Vec2d shift(p2d.X()-U1,p2d.Y()-V1);
        C->Translate(shift);
      }
    }
    if ( !NS.IsNull() && NS->IsKind(STANDARD_TYPE(Geom_SphericalSurface)) ) {
      if(SS->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
       Handle(Geom_SurfaceOfRevolution) SR = Handle(Geom_SurfaceOfRevolution)::DownCast(SS);
        gp_Pnt PR,PS;
        Handle(Geom_SphericalSurface) SPH = Handle(Geom_SphericalSurface)::DownCast(NS);
        Standard_Real US1,US2,VS1,VS2;
        SPH->Bounds(US1,US2,VS1,VS2);
        SPH->D0(US1,VS1,PS);
        Standard_Real UR1,UR2,VR1,VR2;
        SR->Bounds(UR1,UR2,VR1,VR2);
        SR->D0(UR1,VR1,PR);
        gp_Pnt P0 = SPH->Location();
        gp_Vec VS(P0,PS);
        gp_Vec VR(P0,PR);
        Standard_Real angle = VS.Angle(VR);
        gp_Vec2d shift(0,VS1-VR1+angle);
        C->Translate(shift);
      }
    }
  }
  
  Tol = BRep_Tool::Tolerance ( E );
  return Standard_True;
}


//=======================================================================
//function : NewParameter
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_SweptToElementary::NewParameter(const TopoDS_Vertex& /*V*/,
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

GeomAbs_Shape ShapeCustom_SweptToElementary::Continuity(const TopoDS_Edge& E,
                                                        const TopoDS_Face& F1,
                                                        const TopoDS_Face& F2,
                                                        const TopoDS_Edge& /*NewE*/,
                                                        const TopoDS_Face& /*NewF1*/,
                                                        const TopoDS_Face& /*NewF2*/) 
{
  return BRep_Tool::Continuity(E,F1,F2);
}

