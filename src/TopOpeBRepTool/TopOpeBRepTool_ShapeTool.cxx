// Created on: 1994-02-09
// Created by: Jean Yves LEBEY
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLProp_CLProps.hxx>
#include <BRepTools.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================
Standard_Real TopOpeBRepTool_ShapeTool::Tolerance(const TopoDS_Shape& S)
{
  if ( S.IsNull() ) return 0. ;
  Standard_Real tol=0;
  switch (S.ShapeType()) {
    case TopAbs_FACE   : tol = BRep_Tool::Tolerance(TopoDS::Face(S)); break;
    case TopAbs_EDGE   : tol = BRep_Tool::Tolerance(TopoDS::Edge(S)); break;
    case TopAbs_VERTEX : tol = BRep_Tool::Tolerance(TopoDS::Vertex(S)); break;
    default : throw Standard_ProgramError("TopOpeBRepTool_ShapeTool : Shape has no tolerance"); break;
  }
  return tol;
}


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

gp_Pnt TopOpeBRepTool_ShapeTool::Pnt(const TopoDS_Shape& S)
{
  if ( S.ShapeType() != TopAbs_VERTEX ) {
    throw Standard_ProgramError("TopOpeBRepTool_ShapeTool::Pnt");
  }
  return BRep_Tool::Pnt(TopoDS::Vertex(S));
}


#include <Geom_OffsetCurve.hxx>
#include <Geom_TrimmedCurve.hxx>

//=======================================================================
//function : BASISCURVE
//purpose  : 
//=======================================================================
Handle(Geom_Curve) TopOpeBRepTool_ShapeTool::BASISCURVE(const Handle(Geom_Curve)& C)
{
  Handle(Standard_Type) T = C->DynamicType();
  if      ( T == STANDARD_TYPE(Geom_OffsetCurve) ) 
    return BASISCURVE(Handle(Geom_OffsetCurve)::DownCast(C)->BasisCurve());
  else if ( T == STANDARD_TYPE(Geom_TrimmedCurve) )
    return BASISCURVE(Handle(Geom_TrimmedCurve)::DownCast(C)->BasisCurve());
  else return C;
}

Handle(Geom_Curve) TopOpeBRepTool_ShapeTool::BASISCURVE(const TopoDS_Edge& E)
{
  Standard_Real f, l;
  Handle(Geom_Curve) C = BRep_Tool::Curve(E, f, l);
  if ( C.IsNull() ) return C;
  return BASISCURVE(C);
}

#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>


//=======================================================================
//function : BASISSURFACE
//purpose  : 
//=======================================================================
Handle(Geom_Surface) TopOpeBRepTool_ShapeTool::BASISSURFACE(const Handle(Geom_Surface)& S)
{
  Handle(Standard_Type) T = S->DynamicType();
  if      ( T == STANDARD_TYPE(Geom_OffsetSurface) ) 
    return BASISSURFACE(Handle(Geom_OffsetSurface)::DownCast(S)->BasisSurface());
  else if ( T == STANDARD_TYPE(Geom_RectangularTrimmedSurface) )
    return BASISSURFACE(Handle(Geom_RectangularTrimmedSurface)::DownCast(S)->BasisSurface());
  else return S;
}

Handle(Geom_Surface) TopOpeBRepTool_ShapeTool::BASISSURFACE(const TopoDS_Face& F)
{
  TopLoc_Location L;Handle(Geom_Surface) S = BRep_Tool::Surface(F,L);
  return BASISSURFACE(S);
}

//=======================================================================
//function : UVBOUNDS
//purpose  : 
//=======================================================================
void TopOpeBRepTool_ShapeTool::UVBOUNDS
(const Handle(Geom_Surface)& S,
 Standard_Boolean& UPeriodic,
 Standard_Boolean& VPeriodic,
 Standard_Real& Umin, Standard_Real& Umax,
 Standard_Real& Vmin, Standard_Real& Vmax)
{
  const Handle(Geom_Surface) BS = BASISSURFACE(S);
  Handle(Standard_Type) T = BS->DynamicType();

  if      ( T == STANDARD_TYPE(Geom_SurfaceOfRevolution) ) {
    Handle(Geom_SurfaceOfRevolution) 
      SR = Handle(Geom_SurfaceOfRevolution)::DownCast(BS);
    Handle(Geom_Curve) C = BASISCURVE(SR->BasisCurve());
    if (C->IsPeriodic()) {
      UPeriodic = Standard_False; 
      VPeriodic = Standard_True; 
      Vmin = C->FirstParameter(); Vmax = C->LastParameter();
    }
  }
  else if ( T == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion) ) {
    Handle(Geom_SurfaceOfLinearExtrusion)
      SE = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(BS);
    Handle(Geom_Curve) C = BASISCURVE(SE->BasisCurve());
    if (C->IsPeriodic()) {
      UPeriodic = Standard_True; 
      Umin = C->FirstParameter(); Umax = C->LastParameter();
      VPeriodic = Standard_False;
    }
  }
  else { 
    UPeriodic = BS->IsUPeriodic();
    VPeriodic = BS->IsVPeriodic();
    BS->Bounds(Umin,Umax,Vmin,Vmax);
  }
}

void TopOpeBRepTool_ShapeTool::UVBOUNDS
(const TopoDS_Face& F,
 Standard_Boolean& UPeriodic, Standard_Boolean& VPeriodic,
 Standard_Real& Umin, Standard_Real& Umax,
 Standard_Real& Vmin, Standard_Real& Vmax)
{
  TopLoc_Location L;Handle(Geom_Surface) S = BRep_Tool::Surface(F,L);
  UVBOUNDS(S, UPeriodic, VPeriodic, Umin, Umax, Vmin, Vmax);
}

//=======================================================================
//function : AdjustOnPeriodic
//purpose  : 
//=======================================================================

void TopOpeBRepTool_ShapeTool::AdjustOnPeriodic(const TopoDS_Shape& F,
						Standard_Real& u,
						Standard_Real& v)
{
  TopoDS_Face FF = TopoDS::Face(F);
  TopLoc_Location Loc;
  const Handle(Geom_Surface) Surf = BRep_Tool::Surface(FF,Loc);
  
//  Standard_Real Ufirst,Ulast,Vfirst,Vlast;
  Standard_Boolean isUperio,isVperio;
  isUperio = Surf->IsUPeriodic();
  isVperio = Surf->IsVPeriodic();

  // exit if surface supporting F is not periodic on U or V
  if (!isUperio && !isVperio) return;

  Standard_Real UFfirst,UFlast,VFfirst,VFlast;
  BRepTools::UVBounds(FF,UFfirst,UFlast,VFfirst,VFlast);

  Standard_Real tol = Precision::PConfusion();

  if (isUperio) {
    Standard_Real Uperiod = Surf->UPeriod();

//    Standard_Real ubid = UFfirst;

//    ElCLib::AdjustPeriodic(UFfirst,UFfirst + Uperiod,tol,ubid,u);
    if (Abs(u - UFfirst-Uperiod) > tol)
      u = ElCLib::InPeriod(u,UFfirst,UFfirst + Uperiod);
  }
  if (isVperio) {
    Standard_Real Vperiod = Surf->VPeriod();

//    Standard_Real vbid = VFfirst;

//    ElCLib::AdjustPeriodic(VFfirst,VFfirst + Vperiod,tol,vbid,v);
    if (Abs(v - VFfirst-Vperiod) > tol)
      v = ElCLib::InPeriod(v,VFfirst,VFfirst + Vperiod);
  }
}


//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_ShapeTool::Closed(const TopoDS_Shape& S1, 
						  const TopoDS_Shape& S2)
{
  const TopoDS_Edge& E = TopoDS::Edge(S1);
  const TopoDS_Face& F = TopoDS::Face(S2);
  Standard_Boolean brepclosed = BRep_Tool::IsClosed(E,F);
  if ( brepclosed ) {
    Standard_Integer n = 0;
    for ( TopExp_Explorer x(F,TopAbs_EDGE); x.More(); x.Next() ) 
      if ( x.Current().IsSame(E) ) n++;
    if ( n < 2 ) return Standard_False;
    else return Standard_True;
  }
  return Standard_False;
}


#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepTool_GettraceVC();
extern Standard_Boolean TopOpeBRepTool_GettraceNYI();
#endif


inline Standard_Boolean PARINBOUNDS(const Standard_Real par,
				    const Standard_Real first,
				    const Standard_Real last,
				    const Standard_Real tol)
{
  Standard_Boolean b = ( ((first+tol) <= par) && (par <= (last-tol)) );
  return b;
}

inline Standard_Boolean PARONBOUND(const Standard_Real par,
				   const Standard_Real bound,
				   const Standard_Real tol)
{
  Standard_Boolean b = ( ((bound-tol) <= par) && (par <= (bound+tol)) );
  return b;
}


Standard_Real ADJUST(const Standard_Real par,
		     const Standard_Real first,
		     const Standard_Real last,
		     const Standard_Real tol)
{
  Standard_Real period = last - first, periopar = par;

  if      (PARINBOUNDS(par,first,last,tol)) {
    periopar = par + period;
  }
  else if (PARONBOUND(par,first,tol)) {
    periopar = par + period;
  }
  else if (PARONBOUND(par,last,tol)) {
    periopar = par - period;
  }
  return periopar;
}


//=======================================================================
//function : PeriodizeParameter
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_ShapeTool::PeriodizeParameter
  (const Standard_Real par,
   const TopoDS_Shape& EE,
   const TopoDS_Shape& FF)
{
  Standard_Real periopar = par;
  if ( ! TopOpeBRepTool_ShapeTool::Closed(EE,FF) ) return periopar;

  TopoDS_Edge E = TopoDS::Edge(EE);
  TopoDS_Face F = TopoDS::Face(FF);

  TopLoc_Location Loc;
  const Handle(Geom_Surface) Surf = BRep_Tool::Surface(F,Loc);
  Standard_Boolean isUperio = Surf->IsUPeriodic();
  Standard_Boolean isVperio = Surf->IsVPeriodic();
  if (!isUperio && !isVperio) return periopar;

  Standard_Real Ufirst,Ulast,Vfirst,Vlast;
  Surf->Bounds(Ufirst,Ulast,Vfirst,Vlast);

  Standard_Real first,last,tolpc;
  const Handle(Geom2d_Curve) PC = FC2D_CurveOnSurface(E,F,first,last,tolpc);
  if (PC.IsNull()) throw Standard_ProgramError("ShapeTool::PeriodizeParameter : no 2d curve");

  Handle(Standard_Type) TheType = PC->DynamicType();
  if (TheType == STANDARD_TYPE(Geom2d_Line)) {

    Handle(Geom2d_Line) HL (Handle(Geom2d_Line)::DownCast (PC));
    const gp_Dir2d&  D = HL->Direction();

    Standard_Real    tol = Precision::Angular();
    Standard_Boolean isoU = Standard_False, isoV = Standard_False;
    if      (D.IsParallel(gp_Dir2d(0.,1.),tol)) isoU = Standard_True;
    else if (D.IsParallel(gp_Dir2d(1.,0.),tol)) isoV = Standard_True;
    if      (isoU) {
      periopar = ADJUST(par,Ufirst,Ulast,tol);
    }
    else if (isoV) {
      periopar = ADJUST(par,Vfirst,Vlast,tol);
    }

#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceVC()) {
      cout<<"TopOpeBRepTool_ShapeTool PC on edge is ";
      if      (isoU) cout<<"isoU f,l "<<Ufirst<<" "<<Ulast<<endl;
      else if (isoV) cout<<"isoV f,l "<<Vfirst<<" "<<Vlast<<endl;
      else           cout<<"not isoU, not isoV"<<endl;
      cout<<"par = "<<par<<" --> "<<periopar<<endl;
    }
#endif

  }
  // NYI : BSpline ... 

  return periopar;
}


//=======================================================================
//function : ShapesSameOriented
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_ShapeTool::ShapesSameOriented
(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  Standard_Boolean so = Standard_True;

  Standard_Boolean sam = S1.IsSame(S2);
  if (sam) {
    const TopAbs_Orientation o1 = S1.Orientation();
    const TopAbs_Orientation o2 = S2.Orientation();
    if ((o1 == TopAbs_FORWARD || o1 == TopAbs_REVERSED) &&
        (o2 == TopAbs_FORWARD || o2 == TopAbs_REVERSED)) {
      so = (o1 == o2);
      return so;
    }
  }

  TopAbs_ShapeEnum t1 = S1.ShapeType(), t2 = S2.ShapeType();
  if      ( (t1 == TopAbs_SOLID) && (t2 == TopAbs_SOLID) ) {
    so = Standard_True;
  }
  else if ( (t1 == TopAbs_FACE) && (t2 == TopAbs_FACE) )  {
    so = FacesSameOriented(S1,S2);
  }
  else if ( (t1 == TopAbs_EDGE) && (t2 == TopAbs_EDGE) )  {
    so = EdgesSameOriented(S1,S2);
  }
  else if ( (t1 == TopAbs_VERTEX) && (t2 == TopAbs_VERTEX) ) {
    TopAbs_Orientation o1 = S1.Orientation();
    TopAbs_Orientation o2 = S2.Orientation();
    if (o1==TopAbs_EXTERNAL||o1==TopAbs_INTERNAL||o2==TopAbs_EXTERNAL||o2==TopAbs_INTERNAL)
      so = Standard_True;
    else 
      so = ( o1 == o2 );
  }

  return so;
}

//=======================================================================
//function : SurfacesSameOriented
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_ShapeTool::SurfacesSameOriented
(const BRepAdaptor_Surface& S1,const BRepAdaptor_Surface& Sref)
{
  const BRepAdaptor_Surface& S2 = Sref;
  GeomAbs_SurfaceType ST1 = S1.GetType();
  GeomAbs_SurfaceType ST2 = S2.GetType();

  Standard_Boolean so = Standard_True;

  if (ST1 == GeomAbs_Plane && ST2 == GeomAbs_Plane) { 

    Standard_Real u1 = S1.FirstUParameter();
    Standard_Real v1 = S1.FirstVParameter();
    gp_Pnt p1; gp_Vec d1u,d1v; S1.D1(u1,v1,p1,d1u,d1v); 
    gp_Vec n1 = d1u.Crossed(d1v);
    
    Standard_Real u2 = S2.FirstUParameter();
    Standard_Real v2 = S2.FirstVParameter();
    gp_Pnt p2; gp_Vec d2u,d2v; S2.D1(u2,v2,p2,d2u,d2v); 
    gp_Vec n2 = d2u.Crossed(d2v);
    
    Standard_Real d = n1.Dot(n2);
    so = (d > 0.);
  }
  else if (ST1 == GeomAbs_Cylinder && ST2 == GeomAbs_Cylinder) { 

    // On peut projeter n'importe quel point.
    // prenons donc l'origine
    Standard_Real u1 = 0.;
    Standard_Real v1 = 0.;
    gp_Pnt p1; gp_Vec d1u,d1v; S1.D1(u1,v1,p1,d1u,d1v); 
    gp_Vec n1 = d1u.Crossed(d1v);

    Handle(Geom_Surface) HS2 = S2.Surface().Surface();
    HS2 = Handle(Geom_Surface)::DownCast(HS2->Transformed(S2.Trsf()));
    gp_Pnt2d p22d; Standard_Real dp2;
    Standard_Boolean ok = FUN_tool_projPonS(p1,HS2,p22d,dp2);
    if ( !ok ) return so; // NYI : raise
    
    Standard_Real u2 = p22d.X();
    Standard_Real v2 = p22d.Y();
    gp_Pnt p2; gp_Vec d2u,d2v; S2.D1(u2,v2,p2,d2u,d2v); 
    gp_Vec n2 = d2u.Crossed(d2v);
    
    Standard_Real d = n1.Dot(n2);
    so = (d > 0.);
  }
  else { 
    // prendre u1,v1 et projeter sur 2 pour calcul des normales
    // au meme point 3d.
#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceNYI()) {
      cout<<"TopOpeBRepTool_ShapeTool::SurfacesSameOriented surfaces non traitees : NYI";
      cout<<endl;
    }
#endif
  }

  return so;
}


//=======================================================================
//function : FacesSameOriented
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_ShapeTool::FacesSameOriented
(const TopoDS_Shape& S1, const TopoDS_Shape& Sref)
{
  const TopoDS_Shape& S2 = Sref;
  const TopoDS_Face& F1 = TopoDS::Face(S1);
  const TopoDS_Face& F2 = TopoDS::Face(S2);
  TopAbs_Orientation o1 = F1.Orientation();
  TopAbs_Orientation o2 = F2.Orientation();
  if ( o1 == TopAbs_EXTERNAL || o1 == TopAbs_INTERNAL ||
       o2 == TopAbs_EXTERNAL || o2 == TopAbs_INTERNAL ) {
    return Standard_True;
  }

  Standard_Boolean computerestriction = Standard_False;
  BRepAdaptor_Surface BAS1(F1,computerestriction);
  BRepAdaptor_Surface BAS2(F2,computerestriction);
  Standard_Boolean so = F1.IsSame(F2) || SurfacesSameOriented(BAS1,BAS2);
  Standard_Boolean b = so;
  if ( o1 != o2 ) b = !so; 
  return b;
}


//=======================================================================
//function : CurvesSameOriented
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_ShapeTool::CurvesSameOriented
(const BRepAdaptor_Curve& C1, const BRepAdaptor_Curve& Cref)
{
  const BRepAdaptor_Curve& C2 = Cref;
  GeomAbs_CurveType CT1 = C1.GetType();
  GeomAbs_CurveType CT2 = C2.GetType();
  Standard_Boolean so = Standard_True;

  if (CT1 == GeomAbs_Line && CT2 == GeomAbs_Line) { 
    Standard_Real p1 = C1.FirstParameter();
    gp_Dir t1,n1; Standard_Real c1; EdgeData(C1,p1,t1,n1,c1);
    Standard_Real p2 = C2.FirstParameter();
    gp_Dir t2,n2; Standard_Real c2; EdgeData(C2,p2,t2,n2,c2);
    Standard_Real d = t1.Dot(t2);
    so = (d > 0.);
  }
  else { 
    // prendre p1 et projeter sur 2 pour calcul des normales
    // au meme point 3d.
#ifdef OCCT_DEBUG
    if (TopOpeBRepTool_GettraceNYI()) { 
      cout<<"TopOpeBRepTool_ShapeTool::CurvesSameOriented non lineaires : NYI";
      cout<<endl;
    }
#endif
  }

  return so;
}

//=======================================================================
//function : EdgesSameOriented
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_ShapeTool::EdgesSameOriented
(const TopoDS_Shape& S1, const TopoDS_Shape& Sref)
{
  const TopoDS_Shape& S2 = Sref;
  const TopoDS_Edge& E1 = TopoDS::Edge(S1);
  const TopoDS_Edge& E2 = TopoDS::Edge(S2);
  TopAbs_Orientation o1 = E1.Orientation();
  TopAbs_Orientation o2 = E2.Orientation();
  if ( o1 == TopAbs_EXTERNAL || o1 == TopAbs_INTERNAL ||
       o2 == TopAbs_EXTERNAL || o2 == TopAbs_INTERNAL ) {
    return Standard_True;
  }
  BRepAdaptor_Curve BAC1(E1);
  BRepAdaptor_Curve BAC2(E2);
  Standard_Boolean so = CurvesSameOriented(BAC1,BAC2);
  Standard_Boolean b = so;
  if ( o1 != o2 ) b = !so;
  return b;
}


//=======================================================================
//function : EdgeData
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_ShapeTool::EdgeData
(const BRepAdaptor_Curve& BAC, const Standard_Real P, 
 gp_Dir& T, gp_Dir& N, Standard_Real& C)
     
{
  Standard_Real tol = Precision::Angular();

  BRepLProp_CLProps BL(BAC,P,2,tol);
  BL.Tangent(T);
  C = BL.Curvature();

  // xpu150399 cto900R4
  Standard_Real tol1 = Epsilon(0.), tol2 = RealLast();
  Standard_Real tolm = Max(tol,Max(tol1,tol2));

  if ( Abs(C) > tolm ) BL.Normal(N);
  return tol;
}


//=======================================================================
//function : EdgeData
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_ShapeTool::EdgeData
(const TopoDS_Shape& E, const Standard_Real P, 
 gp_Dir& T, gp_Dir& N, Standard_Real& C)
{
  BRepAdaptor_Curve BAC(TopoDS::Edge(E));
  Standard_Real d = EdgeData(BAC,P,T,N,C);
  return d;
}


//=======================================================================
//function : Resolution3dU
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_ShapeTool::Resolution3dU(const Handle(Geom_Surface)& SU,
						      const Standard_Real Tol2d)
{
  GeomAdaptor_Surface GAS(SU);
  Standard_Real r3dunit = 0.00001;  // petite valeur (1.0 -> RangeError sur un tore)
  Standard_Real ru = GAS.UResolution(r3dunit);
  Standard_Real r3du = r3dunit*(Tol2d/ru);
  return r3du;
}


//=======================================================================
//function : Resolution3dV
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_ShapeTool::Resolution3dV(const Handle(Geom_Surface)& SU,
						      const Standard_Real Tol2d)
{
  GeomAdaptor_Surface GAS(SU);
  Standard_Real r3dunit = 0.00001; // petite valeur (1.0 -> RangeError sur un tore)
  Standard_Real rv = GAS.VResolution(r3dunit);
  Standard_Real r3dv = r3dunit*(Tol2d/rv);
  return r3dv;
}


//=======================================================================
//function : Resolution3d
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_ShapeTool::Resolution3d(const Handle(Geom_Surface)& SU,
						     const Standard_Real Tol2d)
{
  Standard_Real ru = Resolution3dU(SU,Tol2d);
  Standard_Real rv = Resolution3dV(SU,Tol2d);
  Standard_Real r = Max(ru,rv);
  return r;
}


//=======================================================================
//function : Resolution3d
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_ShapeTool::Resolution3d(const TopoDS_Face& F,
						     const Standard_Real Tol2d)
{
  TopLoc_Location L; const Handle(Geom_Surface)& SU = BRep_Tool::Surface(F,L);
  Standard_Real r = Resolution3d(SU,Tol2d);
  return r;
}
