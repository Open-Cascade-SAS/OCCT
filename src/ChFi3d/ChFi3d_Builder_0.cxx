// Created on: 1993-12-16
// Created by: Isabelle GRIGNON
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

//  modified by ofv - Thu Feb 26 11:18:16 2004 OCC5246
//  Modified by skv - Fri Oct 24 14:24:47 2003 OCC4077
//  Modified by skv - Mon Jun 16 15:50:44 2003 OCC615

#include <ChFi3d.hxx>
#include <Precision.hxx>

#include <Standard_NotImplemented.hxx>
#include <Standard_ConstructionError.hxx>

#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Lin2d.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <BSplCLib.hxx>
#include <GeomLib.hxx>

#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfXYZ.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <Geom_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <GeomAdaptor_HSurface.hxx>

#include <FairCurve_Batten.hxx>
#include <FairCurve_AnalysisCode.hxx>
#include <Convert_ParameterisationType.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomLib_Interpolate.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GC_MakeCircle.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTopAdaptor_HVertex.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepLib_MakeFace.hxx>

#include <TopAbs.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_Array1OfShape.hxx>


#include <GeomAbs_Shape.hxx>
#include <Bnd_Box2d.hxx>

//#include <math_FunctionSample.hxx>
//#include <math_FunctionAllRoots.hxx>
#include <GCPnts_AbscissaPoint.hxx>

#include <IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntWalk_PWalking.hxx>
#include <IntPatch_WLine.hxx>
#include <Geom2dInt_GInter.hxx>
#include <GeomInt_WLApprox.hxx>
#include <GeomInt_IntSS.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <Approx_SameParameter.hxx>

#include <TopAbs.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp.hxx>

#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_SolidSurfaceInterference.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_InterferenceIterator.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <ProjLib_ProjectedCurve.hxx>

#include <BRepBlend_PointOnRst.hxx>

#include <ChFiDS_HData.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_FilSpine.hxx>
#include <ChFiDS_SequenceOfSurfData.hxx>
#include <ChFiDS_Regul.hxx>
#include <Law_Function.hxx>
#include <Law_Composite.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomLProp_CLProps.hxx>

#include <ChFi3d_Builder_0.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean ChFi3d_GetcontextFORCEBLEND(); 
extern Standard_Boolean ChFi3d_GettraceDRAWINT();
extern Standard_Boolean ChFi3d_GettraceDRAWENLARGE();
extern Standard_Boolean ChFi3d_GettraceDRAWSPINE();
extern Standard_Real  t_sameparam, t_batten;
extern void ChFi3d_SettraceDRAWINT(const Standard_Boolean b);
extern void ChFi3d_SettraceDRAWSPINE(const Standard_Boolean b);
#endif

#include <stdio.h>

#include <GeomAdaptor_HCurve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>

//=======================================================================
//function : ChFi3d_InPeriod
//purpose  : 
//=======================================================================
Standard_Real ChFi3d_InPeriod(const Standard_Real U, 
  const Standard_Real UFirst, 
  const Standard_Real ULast,
  const Standard_Real Eps)
{
  const Standard_Real period = ULast - UFirst;
  Standard_Real u = U;
  while (Eps < (UFirst-u)) u += period;
  while (Eps > (ULast -u)) u -= period;
  if ( u < UFirst) u = UFirst;
  return u;
}
//=======================================================================
//function : Box 
//purpose  : Calculation of min/max uv of the fillet to intersect.
//=======================================================================
void ChFi3d_Boite(const gp_Pnt2d& p1,const gp_Pnt2d& p2,
  Standard_Real& mu,Standard_Real& Mu,
  Standard_Real& mv,Standard_Real& Mv)
{
  mu = Min(p1.X(),p2.X()); Mu = Max(p1.X(),p2.X());
  mv = Min(p1.Y(),p2.Y()); Mv = Max(p1.Y(),p2.Y());
}
//=======================================================================
//function : Box
//purpose  : Calculation of min/max uv of the fillet to intersect.
//=======================================================================
void ChFi3d_Boite(const gp_Pnt2d& p1,const gp_Pnt2d& p2,
  const gp_Pnt2d& p3,const gp_Pnt2d& p4,
  Standard_Real& Du,Standard_Real& Dv,
  Standard_Real& mu,Standard_Real& Mu,
  Standard_Real& mv,Standard_Real& Mv)
{
  Standard_Real a,b;
  a = Min(p1.X(),p2.X());  b = Min(p3.X(),p4.X()); mu = Min(a,b);
  a = Max(p1.X(),p2.X());  b = Max(p3.X(),p4.X()); Mu = Max(a,b);
  a = Min(p1.Y(),p2.Y());  b = Min(p3.Y(),p4.Y()); mv = Min(a,b);
  a = Max(p1.Y(),p2.Y());  b = Max(p3.Y(),p4.Y()); Mv = Max(a,b);
  Du = Mu - mu;
  Dv = Mv - mv;
}
//=======================================================================
//function : EnlargeBox and its friends.
//purpose  : 
//=======================================================================
static Handle(Adaptor3d_HSurface) Geometry(TopOpeBRepDS_DataStructure& DStr,
  const Standard_Integer      ind)
{
  if(ind == 0) return Handle(Adaptor3d_HSurface)();
  if(ind > 0) {
    TopoDS_Face F = TopoDS::Face(DStr.Shape(ind));
    if(F.IsNull()) return Handle(Adaptor3d_HSurface)();
    Handle(BRepAdaptor_HSurface) HS = new BRepAdaptor_HSurface();
    HS->ChangeSurface().Initialize(F,0);
    return HS;
  }
  else{
    Handle(Geom_Surface) S  = DStr.Surface(-ind).Surface();
    if(S.IsNull()) return Handle(Adaptor3d_HSurface)();
    return new GeomAdaptor_HSurface(S);
  }
}
//=======================================================================
//function : ChFi3d_SetPointTolerance
//purpose  : 
//=======================================================================
void ChFi3d_SetPointTolerance(TopOpeBRepDS_DataStructure& DStr,
  const Bnd_Box&              box,
  const Standard_Integer      IP)
{
  Standard_Real a,b,c,d,e,f,vtol;
  box.Get(a,b,c,d,e,f); 
  d-=a; e-=b; f-=c; 
  d*=d; e*=e; f*=f;
  vtol = sqrt(d + e + f) * 1.5;// on prend un petit rab.
  DStr.ChangePoint(IP).Tolerance(vtol);
}
//=======================================================================
//function : ChFi3d_EnlargeBox
//purpose  : 
//=======================================================================
void ChFi3d_EnlargeBox(const Handle(Geom_Curve)& C,
  const Standard_Real       wd,
  const Standard_Real       wf,
  Bnd_Box&                  box1,
  Bnd_Box&                  box2)
{
  box1.Add(C->Value(wd));
  box2.Add(C->Value(wf));
}
//=======================================================================
//function : ChFi3d_EnlargeBox
//purpose  : 
//=======================================================================
void ChFi3d_EnlargeBox(const Handle(Adaptor3d_HSurface)& S,
  const Handle(Geom2d_Curve)&     PC,
  const Standard_Real             wd,
  const Standard_Real             wf,
  Bnd_Box&                        box1,
  Bnd_Box&                        box2)
{
  Standard_Real u,v;
  PC->Value(wd).Coord(u,v);
  box1.Add(S->Value(u,v));
  PC->Value(wf).Coord(u,v);
  box2.Add(S->Value(u,v));
}
//=======================================================================
//function : ChFi3d_EnlargeBox
//purpose  : 
//=======================================================================
void ChFi3d_EnlargeBox(const TopoDS_Edge&           E,
  const TopTools_ListOfShape&  LF,
  const Standard_Real          w,
  Bnd_Box&                     box)

{
  BRepAdaptor_Curve BC(E);
  box.Add(BC.Value(w));
  TopTools_ListIteratorOfListOfShape It;
  for(It.Initialize(LF); It.More(); It.Next()) {
    TopoDS_Face F = TopoDS::Face(It.Value());
    if(!F.IsNull()) {
      BC.Initialize(E,F);
      box.Add(BC.Value(w));
    }
  }
}
//=======================================================================
//function : ChFi3d_EnlargeBox
//purpose  : 
//=======================================================================
void ChFi3d_EnlargeBox(TopOpeBRepDS_DataStructure&    DStr,
  const Handle(ChFiDS_Stripe)&   st, 
  const Handle(ChFiDS_SurfData)& sd,
  Bnd_Box&                       b1,
  Bnd_Box&                       b2,
  const Standard_Boolean         isfirst)
{
  Standard_Real u,v;
  const ChFiDS_CommonPoint& cp1 = sd->Vertex(isfirst,1);
  const ChFiDS_CommonPoint& cp2 = sd->Vertex(isfirst,2);
  b1.Add(cp1.Point());
  b2.Add(cp2.Point());
  const ChFiDS_FaceInterference& fi1 = sd->InterferenceOnS1();
  const ChFiDS_FaceInterference& fi2 = sd->InterferenceOnS2();
  const Handle(Geom_Surface)& S = DStr.Surface(sd->Surf()).Surface();
  const Handle(Geom2d_Curve)& pcs1 = fi1.PCurveOnSurf();
  const Handle(Geom2d_Curve)& pcs2 = fi2.PCurveOnSurf();
  const Handle(Geom_Curve)& c3d1 = DStr.Curve(fi1.LineIndex()).Curve();
  const Handle(Geom_Curve)& c3d2 = DStr.Curve(fi2.LineIndex()).Curve();
  Handle(Adaptor3d_HSurface) F1 = Geometry(DStr,sd->IndexOfS1());
  Handle(Adaptor3d_HSurface) F2 = Geometry(DStr,sd->IndexOfS2());
  Standard_Real p1 = fi1.Parameter(isfirst);
  if(!c3d1.IsNull()) b1.Add(c3d1->Value(p1));
  if(!pcs1.IsNull()) {
    pcs1->Value(p1).Coord(u,v);
    b1.Add(S->Value(u,v));
  }
  if(!F1.IsNull()) {
    const Handle(Geom2d_Curve)& pcf1 = fi1.PCurveOnFace();
    if(!pcf1.IsNull()) {
      pcf1->Value(p1).Coord(u,v);
      b1.Add(F1->Value(u,v));
    }
  }
  Standard_Real p2 = fi2.Parameter(isfirst);
  if(!c3d2.IsNull()) b2.Add(c3d2->Value(p2));
  if(!pcs2.IsNull()) {
    pcs2->Value(p2).Coord(u,v);
    b2.Add(S->Value(u,v));
  }
  if(!F2.IsNull()) {
    const Handle(Geom2d_Curve)& pcf2 = fi2.PCurveOnFace();
    if(!pcf2.IsNull()) {
      pcf2->Value(p2).Coord(u,v);
      b2.Add(F2->Value(u,v));
    }
  }
  if(!st.IsNull()) {
    const Handle(Geom_Curve)& c3d = DStr.Curve(st->Curve(isfirst)).Curve();
    const Handle(Geom2d_Curve)& c2d = st->PCurve(isfirst);
    if(st->Orientation(isfirst) == TopAbs_FORWARD) st->Parameters(isfirst,p1,p2);
    else st->Parameters(isfirst,p2,p1);
    if(!c3d.IsNull()) {
      b1.Add(c3d->Value(p1));
      b2.Add(c3d->Value(p2));
    }
    if(!c2d.IsNull()) {
      c2d->Value(p1).Coord(u,v);
      b1.Add(S->Value(u,v)); 
      c2d->Value(p2).Coord(u,v);
      b2.Add(S->Value(u,v));
    }
  }
}
//=======================================================================
//function : conexfaces
//purpose  : 
//=======================================================================
void ChFi3d_conexfaces(const TopoDS_Edge& E,
  TopoDS_Face&       F1,
  TopoDS_Face&       F2,
  const ChFiDS_Map&  EFMap)
{
  TopTools_ListIteratorOfListOfShape It;
  F1.Nullify();
  F2.Nullify();
  for(It.Initialize(EFMap(E));It.More();It.Next()) {  
    if (F1.IsNull()) {
      F1 = TopoDS::Face(It.Value());
    }
    else {
      F2 = TopoDS::Face(It.Value());
      if(!F2.IsSame(F1) || BRep_Tool::IsClosed(E,F1)) {
        break;
      }
      else F2.Nullify();
    }
  }  
}
//=======================================================================
//function : EdgeState
//purpose  : check concavities for the tops with 3 edges.
//=======================================================================
ChFiDS_State ChFi3d_EdgeState(TopoDS_Edge* E,
  const ChFiDS_Map&  EFMap)
{
  ChFiDS_State sst;
  Standard_Integer i,j;
  //TopoDS_Face F[3];
  TopoDS_Face F1,F2,F3,F4,F5,F6;
  ChFi3d_conexfaces(E[0],F1,F2,EFMap);
  ChFi3d_conexfaces(E[1],F3,F4,EFMap);
  ChFi3d_conexfaces(E[2],F5,F6,EFMap);

  /*
  if(F1.IsSame(F2)) {
    F[0] = F[1] = F1;
    if(F1.IsSame(F3)) F[2] = F4;
    else F[2] = F3;
  }
  else if(F3.IsSame(F4)) {
    F[0] = F[2] = F3;
    if(F3.IsSame(F1)) F[1] = F2;
    else F[1] = F1;
  }
  else if(F5.IsSame(F6)) {
    F[1] = F[2] = F5;
    if(F5.IsSame(F1)) F[0] = F2;
    else F[0] = F1;
  }
  else{
    if(F1.IsSame(F3) || F1.IsSame(F4)) F[0] = F1;
    else F[0] = F2;
    if(F3.IsSame(F[0])) F[2] = F4;
    else F[2] = F3;
    if(F5.IsSame(F[2])) F[1] = F6;
    else F[1] = F5;

  }
  */
  
  //if(F[0].IsNull() || F[1].IsNull() || F[2].IsNull()) sst = ChFiDS_FreeBoundary;
  if (F2.IsNull() || F4.IsNull() || F6.IsNull())
    sst = ChFiDS_FreeBoundary;
  else{
    TopAbs_Orientation o01,o02,o11,o12,o21,o22;
    /*
    i=ChFi3d::ConcaveSide(F[0],F[1],E[0],o01,o02);
    i=ChFi3d::ConcaveSide(F[0],F[2],E[1],o11,o12);
    j=ChFi3d::ConcaveSide(F[1],F[2],E[2],o21,o22);
    */
    i=ChFi3d::ConcaveSide(F1, F2, E[0], o01, o02);
    i=ChFi3d::ConcaveSide(F3, F4, E[1], o11, o12);
    j=ChFi3d::ConcaveSide(F5, F6, E[2], o21, o22);
    
    if(o01==o11 && o02==o21 && o12==o22) sst = ChFiDS_AllSame;
    else if(o12==o22 || i ==10 || j ==10) sst = ChFiDS_OnDiff;
    else sst = ChFiDS_OnSame;
  }
  return sst;
}
//=======================================================================
//function : evalconti
//purpose  : Method very fast to code regularities CN. It is necessary to 
//           refine the processing.
//=======================================================================
GeomAbs_Shape ChFi3d_evalconti(const TopoDS_Edge& /*E*/,
  const TopoDS_Face& F1,
  const TopoDS_Face& F2)
{
  GeomAbs_Shape cont = GeomAbs_G1;
  if(!F1.IsSame(F2)) return cont;
  TopoDS_Face F = F1;
  F.Orientation(TopAbs_FORWARD);
  BRepAdaptor_Surface S(F,Standard_False);
  GeomAbs_SurfaceType typ = S.GetType();
  if(typ != GeomAbs_Cone && 
    typ != GeomAbs_Sphere && 
    typ != GeomAbs_Torus) return cont;
  return GeomAbs_CN;
}
//modified by NIZNHY-PKV Wed Dec 15 11:22:35 2010f
//=======================================================================
//function : KParticular
//purpose  : 
//=======================================================================
Standard_Boolean ChFi3d_KParticular (const Handle(ChFiDS_Spine)& Spine,
  const Standard_Integer      IE,
  const BRepAdaptor_Surface&  S1,
  const BRepAdaptor_Surface&  S2)
{
  Standard_Boolean bRet;
  //
  bRet=Standard_True;
  //
  Handle(ChFiDS_FilSpine) fs = Handle(ChFiDS_FilSpine)::DownCast(Spine);
  if(!fs.IsNull() && !fs->IsConstant(IE)) {
    return !bRet;
  }
  //
  Standard_Boolean bIsPlane1, bIsPlane2;
  Standard_Real aPA;
  GeomAbs_CurveType aCT;
  GeomAbs_SurfaceType aST1, aST2;
  //
  aST1=S1.GetType();
  aST2=S2.GetType();
  bIsPlane1=(aST1==GeomAbs_Plane);
  bIsPlane2=(aST2==GeomAbs_Plane);
  if (!(bIsPlane1 || bIsPlane2)) {
    return !bRet;
  }
  //
  const BRepAdaptor_Surface& aS1=(bIsPlane1)? S1 : S2;
  const BRepAdaptor_Surface& aS2=(bIsPlane1)? S2 : S1;
  aST1=aS1.GetType();
  aST2=aS2.GetType();
  //
  if (!(aST2==GeomAbs_Plane || aST2==GeomAbs_Cylinder || aST2==GeomAbs_Cone)) {
    return !bRet;
  } 
  //
  const BRepAdaptor_Curve& bc = Spine->CurrentElementarySpine(IE);
  aCT = bc.GetType();
  if (!(aCT==GeomAbs_Line || aCT==GeomAbs_Circle)) {
    return !bRet;
  }
  //
  aPA=Precision::Angular();
  //
  if (aST2==GeomAbs_Plane){
    if (aCT==GeomAbs_Line) { 
      return bRet;
    }
  }
  else if (aST2==GeomAbs_Cylinder) {
    const gp_Dir& aD1=aS1.Plane().Axis().Direction();
    const gp_Dir& aD2=aS2.Cylinder().Axis().Direction();
    //
    if (aCT==GeomAbs_Line && aD1.IsNormal(aD2, aPA)) {
      return bRet;
    }
    else if (aCT==GeomAbs_Circle && aD1.IsParallel(aD2, aPA)) {
      return bRet;
    }
  }
  else if(aST2==GeomAbs_Cone) {
    const gp_Dir& aD1=aS1.Plane().Axis().Direction();
    const gp_Dir& aD2=aS2.Cone().Axis().Direction();
    if (aCT == GeomAbs_Circle && aD1.IsParallel(aD2, aPA)) {
      return bRet;
    }
  }  
  return !bRet;    
}
//modified by NIZNHY-PKV Wed Dec 15 11:22:43 2010t
//=======================================================================
//function : BoundFac
//purpose  : Resize the limits of surface adjacent to the given box 
//           Useful for intersections with known extremities. 
//=======================================================================
void ChFi3d_BoundFac(BRepAdaptor_Surface& S,
  const Standard_Real uumin,
  const Standard_Real uumax,
  const Standard_Real vvmin,
  const Standard_Real vvmax,
  const Standard_Boolean checknaturalbounds)
{
  ChFi3d_BoundSrf(S.ChangeSurface(), uumin,uumax,vvmin,vvmax,checknaturalbounds);
}
//=======================================================================
//function : ChFi3d_BoundSrf
//purpose  : Resize the limits of surface adjacent to the given box 
//           Useful for intersections with known extremities.
//=======================================================================
void ChFi3d_BoundSrf(GeomAdaptor_Surface& S,
  const Standard_Real uumin,
  const Standard_Real uumax,
  const Standard_Real vvmin,
  const Standard_Real vvmax,
  const Standard_Boolean checknaturalbounds)
{
  Standard_Real umin = uumin, umax = uumax, vmin = vvmin, vmax = vvmax; 
  Handle(Geom_Surface) surface = S.Surface();
  Handle(Geom_RectangularTrimmedSurface) 
    trs = Handle(Geom_RectangularTrimmedSurface)::DownCast(surface);
  if(!trs.IsNull()) surface = trs->BasisSurface();
  Standard_Real u1,u2,v1,v2;
  surface->Bounds(u1,u2,v1,v2);
  Standard_Real peru=0, perv=0;
  if(surface->IsUPeriodic()) {
    peru = surface->UPeriod();
  }
  if(surface->IsVPeriodic()) {
    perv = surface->VPeriod();
  }
  Standard_Real Stepu = umax - umin;
  Standard_Real Stepv = vmax - vmin;

  //It is supposed that box uv is not null in at least 
  //one direction.
  Standard_Real scalu = S.UResolution(1.);
  Standard_Real scalv = S.VResolution(1.);

  Standard_Real step3du = Stepu/scalu; 
  Standard_Real step3dv = Stepv/scalv;

  if(step3du > step3dv) Stepv = step3du*scalv;
  if(step3dv > step3du) Stepu = step3dv*scalu;

  if (peru > 0) Stepu = 0.1 * (peru - (umax - umin));
  if (perv > 0) Stepv = 0.1 * (perv - (vmax - vmin));

  Standard_Real uu1 = umin - Stepu;
  Standard_Real uu2 = umax + Stepu;
  Standard_Real vv1 = vmin - Stepv;
  Standard_Real vv2 = vmax + Stepv;
  if(checknaturalbounds) {
    if(!S.IsUPeriodic()) {uu1 = Max(uu1,u1);  uu2 = Min(uu2,u2);}
    if(!S.IsVPeriodic()) {vv1 = Max(vv1,v1);  vv2 = Min(vv2,v2);}
  }
  S.Load(surface,uu1,uu2,vv1,vv2);
}
//=======================================================================
//function : ChFi3d_InterPlaneEdge
//purpose  : 
//=======================================================================
Standard_Boolean  ChFi3d_InterPlaneEdge (const Handle(Adaptor3d_HSurface)& Plan,
  const Handle(Adaptor3d_HCurve)&  C,
  Standard_Real& W,
  const Standard_Boolean Sens,
  const Standard_Real tolc)
{ 
  IntCurveSurface_HInter Intersection;
  Standard_Integer isol = 0, nbp ,iip;
  Standard_Real uf = C->FirstParameter(),ul = C->LastParameter();
  Standard_Real CW;

  Intersection.Perform(C,Plan);

  if(Intersection.IsDone()) {
    nbp = Intersection.NbPoints();
    for (iip = 1; iip <= nbp; iip++) {
      CW = Intersection.Point(iip).W();
      if(C->IsPeriodic())
        CW = ElCLib::InPeriod(CW,uf-tolc,uf-tolc+C->Period());
      if(uf - tolc <= CW && ul + tolc >= CW) {
        if (isol == 0) {
          isol = iip; W = CW;
        }
        else {
          if      ( Sens && CW < W) {
            W = CW; isol = iip;
          }
          else if (!Sens && CW > W) {
            W = CW; isol = iip;
          }
        }
      }
    }
  }
  if(isol == 0) return Standard_False;
  return Standard_True;
}
//=======================================================================
//function : ExtrSpineCarac
//purpose  : 
//=======================================================================
void ChFi3d_ExtrSpineCarac(const TopOpeBRepDS_DataStructure& DStr,
  const Handle(ChFiDS_Stripe)& cd,
  const Standard_Integer i,
  const Standard_Real p,
  const Standard_Integer jf,
  const Standard_Integer sens,
  gp_Pnt& P,
  gp_Vec& V,
  Standard_Real& R) //check if it is necessary to add D1,D2 and DR
{
  // Attention for approximated surfaces it is assumed that e
  // the parameters of the pcurve are the same as of  
  // elspine used for its construction.
  const Handle(Geom_Surface)& fffil = 
    DStr.Surface(cd->SetOfSurfData()->Value(i)->Surf()).Surface();
  gp_Pnt2d pp = cd->SetOfSurfData()->Value(i)->Interference(jf).
    PCurveOnSurf()->Value(p);
  GeomAdaptor_Surface gs(fffil);
  P = fffil->Value(pp.X(),pp.Y());
  gp_Pnt Pbid; gp_Vec Vbid;
  switch (gs.GetType()) {
  case GeomAbs_Cylinder :
    {
      gp_Cylinder cyl = gs.Cylinder();
      R = cyl.Radius();
      ElSLib::D1(pp.X(),pp.Y(),cyl,Pbid,Vbid,V);
    }
    break;
  case GeomAbs_Torus :
    {
      gp_Torus tor = gs.Torus();
      R = tor.MinorRadius();
      ElSLib::D1(pp.X(),pp.Y(),tor,Pbid,V,Vbid);
    }
    break;
  default:
    { Standard_Integer nbelspine;
    const Handle(ChFiDS_Spine)& sp = cd->Spine();
    Handle(ChFiDS_FilSpine) fsp = Handle(ChFiDS_FilSpine)::DownCast(sp);
    nbelspine=sp->NbEdges();
    Handle(ChFiDS_HElSpine) hels;
    if   (nbelspine==1) hels = sp->ElSpine(1);
    else hels = sp->ElSpine(p);
    if(fsp->IsConstant()) { R = fsp->Radius(); }
    else { R = fsp->Law(hels)->Value(p); }
    hels->D1(p,Pbid,V);
    }
    break;
  }
  V.Normalize();
  if(sens == 1) V.Reverse();
}
//=======================================================================
//function : ChFi3d_CircularSpine
//purpose  : Calculate a cicular guideline for the corner created from  
//           tangent points and vectors calculated at the extremities 
//           of guidelines of start and end fillets.
//=======================================================================
Handle(Geom_Circle) ChFi3d_CircularSpine(Standard_Real&      WFirst,
  Standard_Real&      WLast,
  const gp_Pnt&       Pdeb,
  const gp_Vec&       Vdeb,
  const gp_Pnt&       Pfin,
  const gp_Vec&       Vfin,
  const Standard_Real rad)
{
  gp_Circ ccc;
  gp_Pln Pl1(Pdeb,gp_Dir(Vdeb)),Pl2(Pfin,gp_Dir(Vfin));
  IntAna_QuadQuadGeo LInt (Pl1,Pl2,Precision::Angular(),
    Precision::Confusion());
  gp_Lin li;
  if (LInt.IsDone()) {
    li = LInt.Line(1);
    gp_Pnt cendeb = ElCLib::Value(ElCLib::Parameter(li,Pdeb),li);
    gp_Pnt cenfin = ElCLib::Value(ElCLib::Parameter(li,Pfin),li);
    gp_Vec vvdeb(cendeb,Pdeb);
    gp_Vec vvfin(cenfin,Pfin);
    gp_Dir dddeb(vvdeb);
    gp_Dir ddfin(vvfin);
    if(Vdeb.Crossed(vvdeb).Dot(Vfin.Crossed(vvfin)) > 0.) {
      return Handle(Geom_Circle)();
    }
    gp_Ax2 circax2(cendeb,dddeb^ddfin,dddeb);
    ccc.SetPosition(circax2);
    ccc.SetRadius(rad);
    WFirst = 0.;
    WLast = dddeb.Angle(ddfin);
    return new Geom_Circle(ccc);
  }

  return Handle(Geom_Circle)();
}
//=======================================================================
//function : ChFi3d_Spine
//purpose  : Calculates the poles of the guideline for the corner from
//           tangent points and vectors calculated at the extremities of
//           guidelines of start and end fillets.
//=======================================================================
Handle(Geom_BezierCurve) ChFi3d_Spine(const gp_Pnt&       pd,
  gp_Vec&             vd,
  const gp_Pnt&       pf,
  gp_Vec&             vf,
  const Standard_Real R)
{     
  TColgp_Array1OfPnt pol(1,4);
  const Standard_Real fac = 0.5 * tan((M_PI-vd.Angle(vf)) * 0.5);
  pol(1) = pd;
  vd.Multiply(fac*R);
  pol(2).SetCoord(pd.X()+vd.X(),pd.Y()+vd.Y(),pd.Z()+vd.Z());
  pol(4) = pf;
  vf.Multiply(fac*R);
  pol(3).SetCoord(pf.X()+vf.X(),pf.Y()+vf.Y(),pf.Z()+vf.Z());
  return new Geom_BezierCurve(pol);
}
//=======================================================================
//function : IsInFront
//purpose  : Checks if surfdata i1 and i2 are face to face
//=======================================================================
Standard_Boolean ChFi3d_IsInFront(TopOpeBRepDS_DataStructure& DStr,
  const Handle(ChFiDS_Stripe)& cd1, 
  const Handle(ChFiDS_Stripe)& cd2,
  const Standard_Integer i1,
  const Standard_Integer i2,
  const Standard_Integer sens1,
  const Standard_Integer sens2,
  Standard_Real& p1,
  Standard_Real& p2,
  TopoDS_Face& face,
  Standard_Boolean& sameside,
  Standard_Integer& jf1,
  Standard_Integer& jf2,
  Standard_Boolean& visavis,
  const TopoDS_Vertex& Vtx,
  const Standard_Boolean Check2dDistance,
  const Standard_Boolean enlarge)
{
  Standard_Boolean isf1 = (sens1 == 1), isf2 = (sens2 == 1);
  const Handle(ChFiDS_SurfData)& fd1 = cd1->SetOfSurfData()->Value(i1);
  const Handle(ChFiDS_SurfData)& fd2 = cd2->SetOfSurfData()->Value(i2);

  TopAbs_Orientation Or,OrSave1,OrSave2,OrFace1,OrFace2;
  visavis = Standard_False;
  Standard_Real u1 = 0.,u2 = 0.; 
  Standard_Boolean ss = 0,ok = 0;
  Standard_Integer j1 = 0,j2 = 0;
  TopoDS_Face ff;
  if(fd1->IndexOfS1() == fd2->IndexOfS1()) { 
    jf1 = 1; jf2 = 1; 
    face = TopoDS::Face(DStr.Shape(fd1->Index(jf1)));
    OrSave1 = cd1->Orientation(jf1);
    Or = OrFace1 = face.Orientation();
    OrSave2 = cd2->Orientation(jf2);
    OrFace2 = DStr.Shape(fd2->Index(jf2)).Orientation();
    visavis = Standard_True;
    sameside = ChFi3d::SameSide(Or,OrSave1,OrSave2,OrFace1,OrFace2);
    // The parameters of the other side are not used for orientation. This would raise problems
    Standard_Integer kf1 = jf1, kf2 = jf2;
    Standard_Real pref1 = fd1->Interference(kf1).Parameter(isf1);
    Standard_Real pref2 = fd2->Interference(kf2).Parameter(isf2);
    gp_Pnt2d P2d;
    if (Check2dDistance)
      P2d = BRep_Tool::Parameters( Vtx, face );
    if(ChFi3d_IntTraces(fd1,pref1,p1,jf1,sens1,fd2,pref2,p2,jf2,sens2,P2d,Check2dDistance,enlarge)) {
      u1 = p1; u2 = p2; ss = sameside; j1 = jf1; j2 = jf2; ff = face;
      ok = 1;
    }
  }
  if(fd1->IndexOfS2() == fd2->IndexOfS1()) { 
    jf1 = 2; jf2 = 1; 
    face = TopoDS::Face(DStr.Shape(fd1->Index(jf1)));
    OrSave1 = cd1->Orientation(jf1);
    Or = OrFace1 = face.Orientation();
    OrSave2 = cd2->Orientation(jf2);
    OrFace2 = DStr.Shape(fd2->Index(jf2)).Orientation();
    visavis = Standard_True;
    sameside = ChFi3d::SameSide(Or,OrSave1,OrSave2,OrFace1,OrFace2);
    // The parameters of the other side are not used for orientation. This would raise problems
    Standard_Integer kf1 = jf1, kf2 = jf2;
    Standard_Real pref1 = fd1->Interference(kf1).Parameter(isf1);
    Standard_Real pref2 = fd2->Interference(kf2).Parameter(isf2);
    gp_Pnt2d P2d;
    if (Check2dDistance)
      P2d = BRep_Tool::Parameters( Vtx, face );
    if(ChFi3d_IntTraces(fd1,pref1,p1,jf1,sens1,fd2,pref2,p2,jf2,sens2,P2d,Check2dDistance,enlarge)) {
      Standard_Boolean restore = 
        ok && ((j1 == jf1 && sens1*(p1 - u1) > 0.) || 
        (j2 == jf2 && sens2*(p2 - u2) > 0.));
      ok = 1;
      if(restore) {
        p1 = u1; p2 = u2; sameside = ss; jf1 = j1; jf2 = j2; face = ff;
      }
      else {
        u1 = p1; u2 = p2; ss = sameside; j1 = jf1; j2 = jf2; ff = face;
      }
    }
    //the re-initialization is added in case p1,... take wrong values
    else if (ok) {
      p1 = u1; p2 = u2; sameside = ss; jf1 = j1; jf2 = j2; face = ff;
    }
  }
  if(fd1->IndexOfS1() == fd2->IndexOfS2()) { 
    jf1 = 1; jf2 = 2; 
    face = TopoDS::Face(DStr.Shape(fd1->Index(jf1)));
    OrSave1 = cd1->Orientation(jf1);
    Or = OrFace1 = face.Orientation();
    OrSave2 = cd2->Orientation(jf2);
    OrFace2 = DStr.Shape(fd2->Index(jf2)).Orientation();
    visavis = Standard_True;
    sameside = ChFi3d::SameSide(Or,OrSave1,OrSave2,OrFace1,OrFace2);
    // The parameters of the other side are not used for orientation.
    Standard_Integer kf1 = jf1, kf2 = jf2;
    Standard_Real pref1 = fd1->Interference(kf1).Parameter(isf1);
    Standard_Real pref2 = fd2->Interference(kf2).Parameter(isf2);
    gp_Pnt2d P2d;
    if (Check2dDistance)
      P2d = BRep_Tool::Parameters( Vtx, face );
    if(ChFi3d_IntTraces(fd1,pref1,p1,jf1,sens1,fd2,pref2,p2,jf2,sens2,P2d,Check2dDistance,enlarge)) {
      Standard_Boolean restore = 
        ok && ((j1 == jf1 && sens1*(p1 - u1) > 0.) || 
        (j2 == jf2 && sens2*(p2 - u2) > 0.));
      ok = 1;
      if(restore) {
        p1 = u1; p2 = u2; sameside = ss; jf1 = j1; jf2 = j2; face = ff;
      }
      else {
        u1 = p1; u2 = p2; ss = sameside; j1 = jf1; j2 = jf2; ff = face;
      }
    }
    //the re-initialization is added in case p1,... take wrong values
    else if (ok) {
      p1 = u1; p2 = u2; sameside = ss; jf1 = j1; jf2 = j2; face = ff;
    }
  }
  if(fd1->IndexOfS2() == fd2->IndexOfS2()) { 
    jf1 = 2; jf2 = 2; 
    face = TopoDS::Face(DStr.Shape(fd1->Index(jf1)));
    OrSave1 = cd1->Orientation(jf1);
    Or = OrFace1 = face.Orientation();
    OrSave2 = cd2->Orientation(jf2);
    OrFace2 = DStr.Shape(fd2->Index(jf2)).Orientation();
    visavis = Standard_True;
    sameside = ChFi3d::SameSide(Or,OrSave1,OrSave2,OrFace1,OrFace2);
    // The parameters of the other side are not used for orientation.
    Standard_Integer kf1 = jf1, kf2 = jf2;
    Standard_Real pref1 = fd1->Interference(kf1).Parameter(isf1);
    Standard_Real pref2 = fd2->Interference(kf2).Parameter(isf2);
    gp_Pnt2d P2d;
    if (Check2dDistance)
      P2d = BRep_Tool::Parameters( Vtx, face );
    if(ChFi3d_IntTraces(fd1,pref1,p1,jf1,sens1,fd2,pref2,p2,jf2,sens2,P2d,Check2dDistance,enlarge)) {
      Standard_Boolean restore = 
        ok && ((j1 == jf1 && sens1*(p1 - u1) > 0.) || 
        (j2 == jf2 && sens2*(p2 - u2) > 0.));
      ok = 1;
      if(restore) {
        p1 = u1; p2 = u2; sameside = ss; jf1 = j1; jf2 = j2; face = ff;
      }
      else {
        u1 = p1; u2 = p2; ss = sameside; j1 = jf1; j2 = jf2; ff = face;
      }
    }
    //the re-initialization is added in case p1,... take wrong values
    else if (ok) {
      p1 = u1; p2 = u2; sameside = ss; jf1 = j1; jf2 = j2; face = ff;
    }
  }
  return ok;
}
//=======================================================================
//function : recadre
//purpose  : 
//=======================================================================
static Standard_Real recadre(const Standard_Real p,
  const Standard_Real ref,
  const Standard_Integer sens,
  const Standard_Real first,
  const Standard_Real last)
{
  const Standard_Real pp = p + (sens > 0 ? (first - last) : (last - first));
  return ((Abs(pp - ref) < Abs(p - ref))? pp : p);
}
//=======================================================================
//function : ChFi3d_IntTraces
//purpose  : 
//=======================================================================
Standard_Boolean ChFi3d_IntTraces(const Handle(ChFiDS_SurfData)& fd1,
  const Standard_Real            pref1,
  Standard_Real&                 p1,
  const Standard_Integer         jf1,
  const Standard_Integer         sens1,
  const Handle(ChFiDS_SurfData)& fd2,
  const Standard_Real            pref2,
  Standard_Real&                 p2,
  const Standard_Integer         jf2,
  const Standard_Integer         sens2,
  const gp_Pnt2d&                RefP2d,
  const Standard_Boolean         Check2dDistance,
  const Standard_Boolean         enlarge)
{
  Geom2dAdaptor_Curve C1;
  Geom2dAdaptor_Curve C2;
  // pcurves are enlarged to be sure that there is intersection
  // additionally all periodic curves are taken and points on 
  // them are filtered using a specific criterion.

  Standard_Real first,last,delta = 0.;
  first = fd1->Interference(jf1).FirstParameter();
  last = fd1->Interference(jf1).LastParameter();
  if ((last-first) < Precision::PConfusion())
    return Standard_False;
  if(enlarge) delta = Min(0.1,0.05*(last-first));
  Handle(Geom2d_Curve) pcf1 = fd1->Interference(jf1).PCurveOnFace();
  if(pcf1.IsNull()) return Standard_False;
  Standard_Boolean isper1 = pcf1->IsPeriodic();
  if(isper1) {
    Handle(Geom2d_TrimmedCurve) tr1 = Handle(Geom2d_TrimmedCurve)::DownCast(pcf1);
    if(!tr1.IsNull()) pcf1 = tr1->BasisCurve();
    C1.Load(pcf1);
  }
  else C1.Load(pcf1,first-delta,last+delta);
  Standard_Real first1 = pcf1->FirstParameter(), last1 = pcf1->LastParameter();

  first = fd2->Interference(jf2).FirstParameter();
  last = fd2->Interference(jf2).LastParameter();
  if ((last-first) < Precision::PConfusion())
    return Standard_False;
  if(enlarge) delta = Min(0.1,0.05*(last-first));
  Handle(Geom2d_Curve) pcf2 = fd2->Interference(jf2).PCurveOnFace();
  if(pcf2.IsNull()) return Standard_False;
  Standard_Boolean isper2 = pcf2->IsPeriodic();
  if(isper2) {
    Handle(Geom2d_TrimmedCurve) tr2 = Handle(Geom2d_TrimmedCurve)::DownCast(pcf2);
    if(!tr2.IsNull()) pcf2 = tr2->BasisCurve();
    C2.Load(pcf2);
  }
  else C2.Load(fd2->Interference(jf2).PCurveOnFace(),first-delta,last+delta);
  Standard_Real first2 = pcf2->FirstParameter(), last2 = pcf2->LastParameter();

  IntRes2d_IntersectionPoint int2d;
  Geom2dInt_GInter Intersection;
  Standard_Integer nbpt,nbseg;
  gp_Pnt2d p2d;
  if(fd1->Interference(jf1).PCurveOnFace() == fd2->Interference(jf2).PCurveOnFace()) {
    Intersection.Perform(C1,
      Precision::PIntersection(),
      Precision::PIntersection());
  }
  else{
    Intersection.Perform(C1,C2,
      Precision::PIntersection(),
      Precision::PIntersection());
  }
  if (Intersection.IsDone()) {
    if (!Intersection.IsEmpty()) {
      nbseg = Intersection.NbSegments();
      if ( nbseg > 0 ) { 
      }
      nbpt = Intersection.NbPoints();
      if ( nbpt >= 1 ) {
        // The criteria sets to filter the found points in a strict way 
        // are missing. Two different criterions chosen somewhat randomly 
        // are used :
        // - periodic curves : closest to the border.
        // - non-periodic curves : the closest to the left of 2 curves
        //                             modulo sens1 and sens2
        int2d = Intersection.Point(1);
        p2d = int2d.Value();
        p1 = int2d.ParamOnFirst();
        p2 = int2d.ParamOnSecond();
        if(isper1) p1 = recadre(p1,pref1,sens1,first1,last1);
        if(isper2) p2 = recadre(p2,pref2,sens2,first2,last2);
        for(Standard_Integer i = 2; i<=nbpt; i++) {
          int2d = Intersection.Point(i);
          if(isper1) {
            Standard_Real pp1 = int2d.ParamOnFirst();
            pp1 = recadre(pp1,pref1,sens1,first1,last1);
            if((Abs(pp1 - pref1) < Abs(p1 - pref1))) {
              p1 = pp1;
              p2 = int2d.ParamOnSecond();
              p2d = int2d.Value();
            }
            //  Modified by skv - Mon Jun 16 15:51:21 2003 OCC615 Begin
            else if (Check2dDistance &&
              RefP2d.Distance(int2d.Value()) < RefP2d.Distance(p2d)) {
                Standard_Real pp2 = int2d.ParamOnSecond();

                if(isper2)
                  pp2 = recadre(pp2,pref2,sens2,first2,last2);

                p1  = pp1;
                p2  = pp2;
                p2d = int2d.Value();
            }
            //  Modified by skv - Mon Jun 16 15:51:22 2003 OCC615 End
          }
          else if(isper2) {
            Standard_Real pp2 = int2d.ParamOnSecond();
            pp2 = recadre(pp2,pref2,sens2,first2,last2);
            if((Abs(pp2 - pref2) < Abs(p2 - pref2))) {
              p2 = pp2;
              p1 = int2d.ParamOnFirst();
              p2d = int2d.Value();
            }
            //  Modified by skv - Mon Jun 16 15:51:21 2003 OCC615 Begin
            else if (Check2dDistance &&
              RefP2d.Distance(int2d.Value()) < RefP2d.Distance(p2d)) {
                Standard_Real pp1 = int2d.ParamOnFirst();

                if(isper1)
                  pp1 = recadre(pp1,pref1,sens1,first1,last1);

                p1  = pp1;
                p2  = pp2;
                p2d = int2d.Value();
            }
            //  Modified by skv - Mon Jun 16 15:51:22 2003 OCC615 End
          }
          else if(((int2d.ParamOnFirst() - p1)*sens1 < 0.) &&
            ((int2d.ParamOnSecond() - p2)*sens2 < 0.)) {
              p1 = int2d.ParamOnFirst();
              p2 = int2d.ParamOnSecond();
              p2d = int2d.Value();
          }
          else if((Abs(int2d.ParamOnFirst() - pref1) < Abs(p1 - pref1)) &&
            (Abs(int2d.ParamOnSecond() - pref2) < Abs(p2 - pref2))) {
              p1 = int2d.ParamOnFirst();
              p2 = int2d.ParamOnSecond();
              p2d = int2d.Value();
          }
          else if (Check2dDistance && RefP2d.Distance(int2d.Value()) < RefP2d.Distance(p2d))
          {
            p1 = int2d.ParamOnFirst();
            p2 = int2d.ParamOnSecond();
            p2d = int2d.Value();
          }
        }
        return Standard_True; 
      }
      return Standard_False; 
    }
    else { return Standard_False; }
  }
  else { return Standard_False; }
} 
//=======================================================================
//function : Coefficient
//purpose  : 
//=======================================================================
void ChFi3d_Coefficient(const gp_Vec& V3d,
  const gp_Vec& D1u,
  const gp_Vec& D1v,
  Standard_Real& DU,
  Standard_Real& DV) 
{
  const Standard_Real AA = D1u.SquareMagnitude();
  const Standard_Real BB = D1u.Dot(D1v);
  const Standard_Real CC = D1v.SquareMagnitude();
  const Standard_Real DD = D1u.Dot(V3d);
  const Standard_Real EE = D1v.Dot(V3d);
  const Standard_Real Delta = AA*CC-BB*BB;
  DU = (DD*CC-EE*BB)/Delta;
  DV = (AA*EE-BB*DD)/Delta;
}
//=======================================================================
//function : ReparamPcurv
//purpose  : Dans le cas ou la pcurve est une BSpline on verifie 
//           ses parametres et on la reparametre eventuellement.
//=======================================================================
void ChFi3d_ReparamPcurv(const Standard_Real Uf, 
  const Standard_Real Ul, 
  Handle(Geom2d_Curve)& Pcurv) 
{
  if(Pcurv.IsNull()) return;
  Standard_Real upcf = Pcurv->FirstParameter();
  Standard_Real upcl = Pcurv->LastParameter();
  Handle(Geom2d_Curve) basis = Pcurv;
  Handle(Geom2d_TrimmedCurve) trpc = Handle(Geom2d_TrimmedCurve)::DownCast(Pcurv);
  if(!trpc.IsNull()) basis = trpc->BasisCurve();
  Handle(Geom2d_BSplineCurve) pc = Handle(Geom2d_BSplineCurve)::DownCast(basis);
  if(pc.IsNull()) return;
  if(Abs(upcf - pc->FirstParameter()) > Precision::PConfusion() ||
    Abs(upcl - pc->LastParameter()) > Precision::PConfusion()) {
      pc->Segment(upcf,upcl);
  }
  if(Abs(Uf - pc->FirstParameter()) > Precision::PConfusion() ||
    Abs(Ul - pc->LastParameter()) > Precision::PConfusion()) {
      TColgp_Array1OfPnt2d pol(1,pc->NbPoles());
      pc->Poles(pol);
      TColStd_Array1OfReal kn(1,pc->NbKnots());
      pc->Knots(kn);
      TColStd_Array1OfInteger mu(1,pc->NbKnots());
      pc->Multiplicities(mu);
      Standard_Integer deg = pc->Degree();
      BSplCLib::Reparametrize(Uf,Ul,kn);
      pc = new Geom2d_BSplineCurve(pol,kn,mu,deg);
  }
  Pcurv = pc;
}
//=======================================================================
//function : ProjectPCurv
//purpose  : Calculation of the pcurve corresponding to a line of intersection
//           3d. Should be called only in analytic cases.
//=======================================================================
void ChFi3d_ProjectPCurv(const Handle(Adaptor3d_HCurve)&   HCg, 
  const Handle(Adaptor3d_HSurface)& HSg, 
  Handle(Geom2d_Curve)&           Pcurv,
  const Standard_Real             tol,
  Standard_Real&                  tolreached) 
{
  if (HSg->GetType() != GeomAbs_BezierSurface &&
    HSg->GetType() != GeomAbs_BSplineSurface) {

      ProjLib_ProjectedCurve Projc (HSg,HCg,tol);
      tolreached = Projc.GetTolerance();
      switch (Projc.GetType()) {
      case GeomAbs_Line : 
        {
          Pcurv = new Geom2d_Line(Projc.Line());
        }
        break;
      case GeomAbs_Circle :
        {
          Pcurv = new Geom2d_Circle(Projc.Circle());
        }
        break;
      case GeomAbs_Ellipse :
        {
          Pcurv = new Geom2d_Ellipse(Projc.Ellipse());
        }
        break;
      case GeomAbs_Hyperbola :
        {
          Pcurv = new Geom2d_Hyperbola(Projc.Hyperbola());
        }
        break;
      case GeomAbs_Parabola :
        {
          Pcurv = new Geom2d_Parabola(Projc.Parabola());
        }
        break;
      case GeomAbs_BezierCurve :
        {
          Pcurv = Projc.Bezier(); 
        }
        break;
      case GeomAbs_BSplineCurve :
        {
          Pcurv = Projc.BSpline();
        }
        break;
      default:
        throw Standard_NotImplemented("echec approximation de la pcurve ");
      }
  }
}
//=======================================================================
//function : CheckSameParameter
//purpose  : Controls a posteriori that sameparameter worked well
//=======================================================================
Standard_Boolean ChFi3d_CheckSameParameter (const Handle(Adaptor3d_HCurve)&   C3d,
  Handle(Geom2d_Curve)&           Pcurv,
  const Handle(Adaptor3d_HSurface)& S,
  const Standard_Real             tol3d,
  Standard_Real&                  tolreached)
{
  tolreached = 0.;
  Standard_Real f = C3d->FirstParameter();
  Standard_Real l = C3d->LastParameter();
  Standard_Integer nbp = 45;
  Standard_Real step = 1./(nbp -1);
  for(Standard_Integer i = 0; i < nbp; i++) {
    Standard_Real t,u,v;
    t = step * i;
    t = (1-t) * f + t * l;
    Pcurv->Value(t).Coord(u,v);
    gp_Pnt pS = S->Value(u,v);
    gp_Pnt pC = C3d->Value(t);
    Standard_Real d2 = pS.SquareDistance(pC);
    tolreached = Max(tolreached,d2);
  }
  tolreached = sqrt(tolreached);
  if(tolreached > tol3d) {
    tolreached *= 2.;
    return Standard_False;
  }
  tolreached *= 2.;
  tolreached = Max(tolreached,Precision::Confusion());
  return Standard_True;
}
//=======================================================================
//function : SameParameter
//purpose  : Encapsulation of Sameparameter
//=======================================================================
Standard_Boolean ChFi3d_SameParameter(const Handle(Adaptor3d_HCurve)&   C3d,
  Handle(Geom2d_Curve)&           Pcurv,
  const Handle(Adaptor3d_HSurface)& S,
  const Standard_Real             tol3d,
  Standard_Real&                  tolreached)
{
  if(ChFi3d_CheckSameParameter(C3d,Pcurv,S,tol3d,tolreached)) return Standard_True;
  Approx_SameParameter sp(C3d,Pcurv,S,tol3d);
  if(sp.IsDone() && !sp.IsSameParameter()) Pcurv = sp.Curve2d();
  else if(!sp.IsDone() && !sp.IsSameParameter()) {
    return Standard_False;
  }
  tolreached = sp.TolReached();
  return Standard_True;
}
//=======================================================================
//function : SameParameter
//purpose  : Encapsulation de Sameparameter
//=======================================================================
Standard_Boolean ChFi3d_SameParameter(const Handle(Geom_Curve)&   C3d,
  Handle(Geom2d_Curve)&       Pcurv,
  const Handle(Geom_Surface)& S,
  const Standard_Real         Pardeb,
  const Standard_Real         Parfin,
  const Standard_Real         tol3d,
  Standard_Real&              tolreached)
{
  /*szv:static*/ Handle(GeomAdaptor_HSurface) hs(new GeomAdaptor_HSurface(S));
  /*szv:static*/ Handle(GeomAdaptor_HCurve) hc(new GeomAdaptor_HCurve(C3d,Pardeb,Parfin));
  return ChFi3d_SameParameter(hc,Pcurv,hs,tol3d,tolreached);
}
//=======================================================================
//function : ComputePCurv 
//purpose  : Calculates a straight line in form of BSpline 
//           to guarantee the same range and parameters as of the 
//           reference 3D curve.
//=======================================================================
void ChFi3d_ComputePCurv(const Handle(Adaptor3d_HCurve)&   C3d,
  const gp_Pnt2d&                 UV1,
  const gp_Pnt2d&                 UV2,
  Handle(Geom2d_Curve)&           Pcurv,
  const Handle(Adaptor3d_HSurface)& S,
  const Standard_Real             Pardeb,
  const Standard_Real             Parfin,
  const Standard_Real             tol3d,
  Standard_Real&                  tolreached,
  const Standard_Boolean          reverse)
{
  ChFi3d_ComputePCurv(UV1,UV2,Pcurv,Pardeb,Parfin,reverse);
  ChFi3d_SameParameter(C3d,Pcurv,S,tol3d,tolreached);
}
//=======================================================================
//function : ComputePCurv 
//purpose  : Calculates a straight line in form of BSpline 
//           to guarantee the same range and parameters as of the 
//           reference 3D curve.
//=======================================================================
void ChFi3d_ComputePCurv(const Handle(Geom_Curve)&   C3d,
  const gp_Pnt2d&             UV1,
  const gp_Pnt2d&             UV2,
  Handle(Geom2d_Curve)&       Pcurv,
  const Handle(Geom_Surface)& S,
  const Standard_Real         Pardeb,
  const Standard_Real         Parfin,
  const Standard_Real         tol3d,
  Standard_Real&              tolreached,
  const Standard_Boolean      reverse)
{
  Handle(Adaptor3d_HSurface) hs(new GeomAdaptor_HSurface(S));
  Handle(Adaptor3d_HCurve) hc(new GeomAdaptor_HCurve(C3d,Pardeb,Parfin));
  ChFi3d_ComputePCurv(hc,UV1,UV2,Pcurv,hs,Pardeb,Parfin,tol3d,tolreached,reverse);
}
//=======================================================================
//function : ComputePCurv 
//purpose  : Calculates a straight line in form of BSpline 
//           to guarantee the same range.
//=======================================================================
void ChFi3d_ComputePCurv(const gp_Pnt2d& UV1,
  const gp_Pnt2d& UV2,
  Handle(Geom2d_Curve)& Pcurv,
  const Standard_Real Pardeb,
  const Standard_Real Parfin,
  const Standard_Boolean reverse)
{
  const Standard_Real tol = Precision::PConfusion();
  gp_Pnt2d p1,p2;
  if (!reverse) {
    p1 = UV1;
    p2 = UV2;
  }
  else {
    p1 = UV2;
    p2 = UV1;
  }

  if (Abs(p1.X()-p2.X()) <= tol &&
    Abs((p2.Y()-p1.Y())-(Parfin-Pardeb)) <= tol) {
      gp_Pnt2d ppp(p1.X(),p1.Y()-Pardeb);
      Pcurv = new Geom2d_Line(ppp,gp::DY2d());
  }
  else if (Abs(p1.X()-p2.X()) <= tol &&
    Abs((p1.Y()-p2.Y())-(Parfin-Pardeb)) <= tol) {
      gp_Pnt2d ppp(p1.X(),p1.Y()+Pardeb);
      Pcurv = new Geom2d_Line(ppp,gp::DY2d().Reversed());
  }
  else if (Abs(p1.Y()-p2.Y()) <= tol &&
    Abs((p2.X()-p1.X())-(Parfin-Pardeb)) <= tol) {
      gp_Pnt2d ppp(p1.X()-Pardeb,p1.Y());
      Pcurv = new Geom2d_Line(ppp,gp::DX2d());
  }
  else if (Abs(p1.Y()-p2.Y()) <= tol &&
    Abs((p1.X()-p2.X())-(Parfin-Pardeb)) <= tol) {
      gp_Pnt2d ppp(p1.X()+Pardeb,p1.Y());
      Pcurv = new Geom2d_Line(ppp,gp::DX2d().Reversed());
  }
  else{
    TColgp_Array1OfPnt2d p(1,2);
    TColStd_Array1OfReal k(1,2);
    TColStd_Array1OfInteger m(1,2);
    m.Init(2);
    k(1) = Pardeb;
    k(2) = Parfin;
    p(1) = p1;
    p(2) = p2;
    Pcurv = new Geom2d_BSplineCurve(p,k,m,1);
  }
  Pcurv = new Geom2d_TrimmedCurve(Pcurv,Pardeb,Parfin);
}
//=======================================================================
//function : ChFi3d_mkbound
//purpose  : 
//=======================================================================
Handle(GeomFill_Boundary) ChFi3d_mkbound(const Handle(Adaptor3d_HSurface)& Fac,
  Handle(Geom2d_Curve)& curv, 
  const Standard_Integer sens1,
  const gp_Pnt2d& pfac1,
  const gp_Vec2d& vfac1,
  const Standard_Integer sens2,
  const gp_Pnt2d& pfac2,
  const gp_Vec2d& vfac2,
  const Standard_Real t3d,
  const Standard_Real ta)
{
  gp_Dir2d v1(vfac1);
  if(sens1 == 1) v1.Reverse();
  gp_Dir2d v2(vfac2);
  if(sens2 == 1) v2.Reverse();
  curv = ChFi3d_BuildPCurve(Fac,pfac1,v1,pfac2,v2,Standard_False);
  return ChFi3d_mkbound(Fac,curv,t3d,ta);
}
//=======================================================================
//function : ChFi3d_mkbound
//purpose  : 
//=======================================================================
Handle(GeomFill_Boundary) ChFi3d_mkbound(const Handle(Adaptor3d_HSurface)& Surf,
  Handle(Geom2d_Curve)& curv,
  const Standard_Integer sens1,
  const gp_Pnt2d& p1,
  gp_Vec&   v1,
  const Standard_Integer sens2,
  const gp_Pnt2d& p2,
  gp_Vec& v2,
  const Standard_Real t3d,
  const Standard_Real ta)
{
  if(sens1 == 1) v1.Reverse();
  if(sens2 == 1) v2.Reverse();
  curv = ChFi3d_BuildPCurve(Surf,p1,v1,p2,v2);
  return ChFi3d_mkbound(Surf,curv,t3d,ta);
}
//=======================================================================
//function : ChFi3d_mkbound
//purpose  : 
//=======================================================================
Handle(GeomFill_Boundary) ChFi3d_mkbound(const Handle(Geom_Surface)& s,
  const gp_Pnt2d& p1,
  const gp_Pnt2d& p2,
  const Standard_Real t3d,
  const Standard_Real ta,
  const Standard_Boolean isfreeboundary)
{
  Handle(Adaptor3d_HSurface) HS = new GeomAdaptor_HSurface(s);
  return ChFi3d_mkbound(HS,p1,p2,t3d,ta,isfreeboundary);
}
//=======================================================================
//function : ChFi3d_mkbound
//purpose  : 
//=======================================================================
Handle(GeomFill_Boundary) ChFi3d_mkbound(const Handle(Adaptor3d_HSurface)& HS,
  const gp_Pnt2d& p1,
  const gp_Pnt2d& p2,
  const Standard_Real t3d,
  const Standard_Real ta,
  const Standard_Boolean isfreeboundary)
{
  TColgp_Array1OfPnt2d pol(1,2);
  pol(1)=p1;
  pol(2)=p2;
  Handle(Geom2d_Curve) curv = new Geom2d_BezierCurve(pol);
  return ChFi3d_mkbound(HS,curv,t3d,ta,isfreeboundary);
}
//=======================================================================
//function : ChFi3d_mkbound
//purpose  : 
//=======================================================================
Handle(GeomFill_Boundary) ChFi3d_mkbound(const Handle(Adaptor3d_HSurface)& HS,
  const Handle(Geom2d_Curve)& curv,
  const Standard_Real t3d,
  const Standard_Real ta,
  const Standard_Boolean isfreeboundary)
{
  Handle(Geom2dAdaptor_HCurve) HC = new Geom2dAdaptor_HCurve(curv);
  Adaptor3d_CurveOnSurface COnS(HC,HS);
  if (isfreeboundary) {
    Handle(Adaptor3d_HCurveOnSurface) HCOnS = new Adaptor3d_HCurveOnSurface(COnS);
    return new GeomFill_SimpleBound(HCOnS,t3d,ta); 
  }
  return new GeomFill_BoundWithSurf(COnS,t3d,ta);
}
//=======================================================================
//function : ChFi3d_mkbound
//purpose  : 
//=======================================================================
Handle(GeomFill_Boundary) ChFi3d_mkbound(const Handle(Adaptor3d_HSurface)& Fac,
  Handle(Geom2d_Curve)& curv, 
  const gp_Pnt2d& p1,
  const gp_Pnt2d& p2,
  const Standard_Real t3d,
  const Standard_Real ta,
  const Standard_Boolean isfreeboundary)
{
  TColgp_Array1OfPnt2d pol(1,2);
  pol(1)=p1;
  pol(2)=p2;
  curv = new Geom2d_BezierCurve(pol);
  return ChFi3d_mkbound(Fac,curv,t3d,ta,isfreeboundary);
}
//=======================================================================
//function : ChFi3d_BuildPCurve
//purpose  : 
//=======================================================================
Handle(Geom2d_Curve) ChFi3d_BuildPCurve(const gp_Pnt2d& p1,
  gp_Dir2d& d1,
  const gp_Pnt2d& p2,
  gp_Dir2d& d2,
  const Standard_Boolean redresse)
{
  gp_Vec2d vref(p1,p2);
  gp_Dir2d dref(vref);
  Standard_Real mref = vref.Magnitude();
  if(redresse) {
    if(d1.Dot(dref) < 0.) d1.Reverse();
    if(d2.Dot(dref) > 0.) d2.Reverse();
  }
  //On fait une cubique a la mords moi le noeud
  TColgp_Array1OfPnt2d pol(1,4);
  pol(1)=p1;
  pol(4)=p2;
  Standard_Real Lambda1 = Max(Abs(d2.Dot(d1)),Abs(dref.Dot(d1)));
  Lambda1 = Max(0.5*mref*Lambda1,1.e-5);
  pol(2) = gp_Pnt2d(p1.XY()+Lambda1*d1.XY());
  Standard_Real Lambda2 = Max(Abs(d1.Dot(d2)),Abs(dref.Dot(d2)));
  Lambda2 = Max(0.5*mref*Lambda2,1.e-5);
  pol(3)=gp_Pnt2d(p2.XY()+Lambda2*d2.XY());
  return new Geom2d_BezierCurve(pol);
}
//=======================================================================
//function : ChFi3d_BuildPCurve
//purpose  : 
//=======================================================================
Handle(Geom2d_Curve) ChFi3d_BuildPCurve(const Handle(Adaptor3d_HSurface)& Surf,
  const gp_Pnt2d&                 p1,
  const gp_Vec2d&                 v1,
  const gp_Pnt2d&                 p2,
  const gp_Vec2d&                 v2,
  const Standard_Boolean          redresse)
{
  gp_Pnt2d pp1 = p1, pp2 = p2;
  gp_Vec2d vv1 = v1, vv2 = v2;
  const Standard_Real ures = Surf->UResolution(1.);
  const Standard_Real vres = Surf->VResolution(1.);
  const Standard_Real invures = 1./ures;
  const Standard_Real invvres = 1./vres;
  pp1.SetX(invures*pp1.X()); pp1.SetY(invvres*pp1.Y()); 
  pp2.SetX(invures*pp2.X()); pp2.SetY(invvres*pp2.Y()); 
  vv1.SetX(invures*vv1.X()); vv1.SetY(invvres*vv1.Y()); 
  vv2.SetX(invures*vv2.X()); vv2.SetY(invvres*vv2.Y()); 
  gp_Dir2d d1(vv1), d2(vv2);
  Handle(Geom2d_Curve) g2dc = ChFi3d_BuildPCurve(pp1,d1,pp2,d2,redresse);
  Handle(Geom2d_BezierCurve) pc = Handle(Geom2d_BezierCurve)::DownCast(g2dc);
  const Standard_Integer nbp = pc->NbPoles();
  for(Standard_Integer ip = 1; ip <= nbp; ip++) {
    gp_Pnt2d pol = pc->Pole(ip);
    pol.SetX(ures*pol.X()); pol.SetY(vres*pol.Y());
    pc->SetPole(ip,pol);
  }
  return pc;
}
//=======================================================================
//function : ChFi3d_BuildPCurve
//purpose  : 
//=======================================================================
Handle(Geom2d_Curve) ChFi3d_BuildPCurve(const Handle(Adaptor3d_HSurface)& Surf,
  const gp_Pnt2d&                 p1,
  const gp_Vec&                   v1,
  const gp_Pnt2d&                 p2,
  const gp_Vec&                   v2,
  const Standard_Boolean          redresse)
{
  gp_Vec D1u,D1v;
  gp_Pnt PP1,PP2;
  Standard_Real DU,DV;
  Surf->D1(p1.X(),p1.Y(),PP1,D1u,D1v);
  ChFi3d_Coefficient(v1,D1u,D1v,DU,DV);
  gp_Vec2d vv1(DU,DV);
  Surf->D1(p2.X(),p2.Y(),PP2,D1u,D1v);
  ChFi3d_Coefficient(v2,D1u,D1v,DU,DV);
  gp_Vec2d vv2(DU,DV);
  gp_Vec Vref(PP1,PP2);
  if(redresse) {
    if(Vref.Dot(v1) < 0.) vv1.Reverse();
    if(Vref.Dot(v2) > 0.) vv2.Reverse();
  }
  return ChFi3d_BuildPCurve(Surf,p1,vv1,p2,vv2,0);
}
//=======================================================================
//function : ComputeArete
//purpose  : 
// to fill with s.d. a fillet with pcurves constructed as follows
// firstpoint on S1 -------------edge:curve3d/pcurves--->lastpoint on S1
//  |                                                              |
//  |                                                              |
//  |                                                              |
// edge:curve 3d/pcurves           fillet                         edge
//  |         attention it is necessary to test orientation of the fillet before|
//  |         determining the transitions pcurves/fillet           |
//  |                                                              |
//  \/                                                             \/
// firstpoint sur S2 -------------edge:courbe3d/pcurves--->lastpoint sur S2
//
//=======================================================================
void  ChFi3d_ComputeArete(const ChFiDS_CommonPoint&   P1,
  const gp_Pnt2d&             UV1,
  const ChFiDS_CommonPoint&   P2,
  const gp_Pnt2d&             UV2,
  const Handle(Geom_Surface)& Surf,
  Handle(Geom_Curve)&         C3d,
  Handle(Geom2d_Curve)&       Pcurv,
  Standard_Real&              Pardeb,
  Standard_Real&              Parfin,
  const Standard_Real         tol3d,
  const Standard_Real         tol2d,
  Standard_Real&              tolreached,
  const Standard_Integer      IFlag)
  // IFlag=0 pcurve et courbe 3d 
  // IFlag>0 pcurve (parametrage impose si IFlag=2)
{
  /*szv:static*/ Handle(GeomAdaptor_HSurface) hs(new GeomAdaptor_HSurface());
  /*szv:static*/ Handle(GeomAdaptor_HCurve) hc(new GeomAdaptor_HCurve());

  tolreached = tol3d;

  if (Abs(UV1.X()-UV2.X()) <= tol2d) {
    if (IFlag == 0) {
      Pardeb = UV1.Y();
      Parfin = UV2.Y();
      C3d = Surf->UIso(UV1.X());
      if(Pardeb > Parfin) {
        Pardeb = C3d->ReversedParameter(Pardeb);
        Parfin = C3d->ReversedParameter(Parfin);
        C3d->Reverse();
      }
      Handle(Geom_TrimmedCurve) tc = Handle(Geom_TrimmedCurve)::DownCast(C3d);
      if(!tc.IsNull()) {
        C3d = tc->BasisCurve();
        if (C3d->IsPeriodic()) {
          ElCLib::AdjustPeriodic(C3d->FirstParameter(),C3d->LastParameter(),
            tol2d,Pardeb,Parfin);
        }
      }
    }
    if(IFlag != 1) {
      hs->ChangeSurface().Load(Surf);
      hc->ChangeCurve().Load(C3d,Pardeb,Parfin);
      const Handle(Adaptor3d_HCurve)& aHCurve = hc; // to avoid ambiguity
      ChFi3d_ComputePCurv(aHCurve,UV1,UV2,Pcurv,hs,Pardeb,Parfin,tol3d,tolreached,Standard_False);
    }
    else{
      Pcurv = new Geom2d_Line(UV1,gp_Vec2d(UV1,UV2));
    }
  }
  else if (Abs(UV1.Y()-UV2.Y())<=tol2d) {
    //iso v
    if (IFlag == 0) {
      Pardeb = UV1.X();
      Parfin = UV2.X();
      C3d = Surf->VIso(UV1.Y());
      if(Pardeb > Parfin) {
        Pardeb = C3d->ReversedParameter(Pardeb);
        Parfin = C3d->ReversedParameter(Parfin);
        C3d->Reverse();
      }
      Handle(Geom_TrimmedCurve) tc = Handle(Geom_TrimmedCurve)::DownCast(C3d);
      if(!tc.IsNull()) {
        C3d = tc->BasisCurve();
        if (C3d->IsPeriodic()) {
          ElCLib::AdjustPeriodic(C3d->FirstParameter(),C3d->LastParameter(),
            tol2d,Pardeb,Parfin);
        }
      }
    }
    if(IFlag != 1) {
      hs->ChangeSurface().Load(Surf);
      hc->ChangeCurve().Load(C3d,Pardeb,Parfin);
      const Handle(Adaptor3d_HCurve)& aHCurve = hc; // to avoid ambiguity
      ChFi3d_ComputePCurv(aHCurve,UV1,UV2,Pcurv,hs,Pardeb,Parfin,tol3d,tolreached,Standard_False);
    }
    else{
      Pcurv = new Geom2d_Line(UV1,gp_Vec2d(UV1,UV2));
    }
  }
  else if (IFlag == 0) {

    if (P1.IsVertex() || P2.IsVertex() || !P1.IsOnArc() || !P2.IsOnArc()) {
      // A straight line is constructed to avoid  
      // arc and tangent.
      TColgp_Array1OfPnt2d qoles(1,2);
      qoles(1)=UV1;
      qoles(2)=UV2;
      Pcurv = new Geom2d_BezierCurve(qoles);
    }
    else {
      BRepAdaptor_Curve C1(P1.Arc());
      gp_Pnt Pp;
      gp_Vec Vv1;
      C1.D1(P1.ParameterOnArc(),Pp,Vv1);
      C1.Initialize(P2.Arc());
      gp_Vec Vv2;
      C1.D1(P2.ParameterOnArc(),Pp,Vv2);
      hs->ChangeSurface().Load(Surf);
      Pcurv = ChFi3d_BuildPCurve(hs,UV1,Vv1,UV2,Vv2,Standard_True); 
      // There are some cases when PCurve constructed in this way  
      // leaves the surface, in particular if it results from an 
      // extension. A posteriori checking is required and if
      // the curve leaves the surface it is replaced by straight line UV1 UV2
      // non regarding the tangency with neighboring arcs!
      Bnd_Box2d bs;
      Standard_Real umin,umax,vmin,vmax;
      Surf->Bounds(umin,umax,vmin,vmax);
      bs.Update(umin,vmin,umax,vmax);
      Standard_Boolean aIN = Standard_True;
      for(Standard_Integer ii = 1; ii <= 4 && aIN; ii++) {
	if(bs.IsOut(Handle(Geom2d_BezierCurve)::DownCast (Pcurv)->Pole(ii))) {
          aIN = Standard_False;
          TColgp_Array1OfPnt2d qoles(1,2);
          qoles(1)=UV1;
          qoles(2)=UV2;
          Pcurv = new Geom2d_BezierCurve(qoles);
        }
      }
    }
    Geom2dAdaptor_Curve AC(Pcurv);
    Handle(Geom2dAdaptor_HCurve) AHC = 
      new Geom2dAdaptor_HCurve(AC);
    GeomAdaptor_Surface AS(Surf);
    Handle(GeomAdaptor_HSurface) AHS = 
      new GeomAdaptor_HSurface(AS);
    Adaptor3d_CurveOnSurface Cs(AHC,AHS);
    Pardeb = Cs.FirstParameter();
    Parfin = Cs.LastParameter();
    Standard_Real avtol;
    GeomLib::BuildCurve3d(tol3d,Cs,Pardeb,Parfin,C3d,tolreached,avtol);
  }
  else {
    hs->ChangeSurface().Load(Surf);
    hc->ChangeCurve().Load(C3d,Pardeb,Parfin);
    ChFi3d_ProjectPCurv(hc,hs,Pcurv,tol3d,tolreached);
    gp_Pnt2d p2d = Pcurv->Value(Pardeb);
    if(!UV1.IsEqual(p2d,Precision::PConfusion())) {
      gp_Vec2d v2d(p2d,UV1);
      Pcurv->Translate(v2d);
    }
  }
}
//=======================================================================
//function : FilCurveInDS
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_SurfaceCurveInterference)  ChFi3d_FilCurveInDS
  (const Standard_Integer Icurv,
  const Standard_Integer Isurf,
  const Handle(Geom2d_Curve)& Pcurv,
  const TopAbs_Orientation Et)
{
  Handle(TopOpeBRepDS_SurfaceCurveInterference) SC1;
  SC1 = new TopOpeBRepDS_SurfaceCurveInterference(TopOpeBRepDS_Transition(Et),
    TopOpeBRepDS_SURFACE,
    Isurf,TopOpeBRepDS_CURVE,Icurv,
    Pcurv);
  return SC1;
}
//=======================================================================
//function : TrsfTrans
//purpose  : 
//           
//=======================================================================
TopAbs_Orientation ChFi3d_TrsfTrans(const IntSurf_TypeTrans T1) 
{
  switch (T1)  {
  case IntSurf_In:  return TopAbs_FORWARD;
  case IntSurf_Out: return TopAbs_REVERSED;
  default:
    break;
  }
  return TopAbs_INTERNAL;
}
//=======================================================================
//function : FilCommonPoint
//purpose  : Loading of the common point
//           management of the case when it happens on already existing vertex.
//=======================================================================
Standard_EXPORT void ChFi3d_FilCommonPoint(const BRepBlend_Extremity& SP,
  const IntSurf_TypeTrans TransLine,
  const Standard_Boolean Start,
  ChFiDS_CommonPoint& CP,
  const Standard_Real Tol)
{
  //  BRep_Tool Outil;
  Standard_Real Dist, maxtol = Max(Tol,CP.Tolerance());

  CP.SetPoint(SP.Value()); // One starts with the point and the vector
  if (SP.HasTangent()) {
    if (Start) {
      CP.SetVector(SP.Tangent().Reversed()); // The tangent is oriented to the exit
    }
    else {
      CP.SetVector(SP.Tangent());
    }
  }

  CP.SetParameter(SP.ParameterOnGuide()); // and the parameter of the spine

  if (SP.IsVertex()) { // the Vertex is loaded if required
    // (inside of a face)
    TopoDS_Vertex V =  
      Handle(BRepTopAdaptor_HVertex)::DownCast(SP.Vertex())->Vertex();

    CP.SetVertex(V);  
    Dist = (SP.Value()).Distance(BRep_Tool::Pnt(V));
    //// modified by jgv, 18.09.02 for OCC571 ////
    //maxtol += Dist;
    maxtol = Max( Dist, maxtol );
    //////////////////////////////////////////////
    CP.SetPoint(BRep_Tool::Pnt(V));

    //the sequence of arcs the information is known by thee vertex (ancestor)
    //in this case the transitions are not computed, it is done by this program
  }

  if (SP.NbPointOnRst() != 0) { //  An arc, and/or a vertex is loaded

    const BRepBlend_PointOnRst& PR = SP.PointOnRst(1);
    Handle(BRepAdaptor_HCurve2d) 
      Harc = Handle(BRepAdaptor_HCurve2d)::DownCast(PR.Arc());
    if(!Harc.IsNull()) {

      Standard_Real DistF, DistL, LeParamAmoi;
      Standard_Integer Index_min;
      TopoDS_Edge E = Harc->ChangeCurve2d().Edge();

      TopoDS_Vertex V[2];
      TopExp::Vertices(E, V[0], V[1]);

      DistF = (SP.Value()).Distance(BRep_Tool::Pnt(V[0]));
      DistL = (SP.Value()).Distance(BRep_Tool::Pnt(V[1]));
      if (DistF<DistL) { Index_min = 0;
      Dist = DistF; }
      else             { Index_min = 1;
      Dist = DistL; }

      if (Dist <= maxtol + BRep_Tool::Tolerance(V[Index_min]) ) { 
        // a prexisting vertex has been met
        CP.SetVertex(V[Index_min]); //the old vertex is loaded
        CP.SetPoint( BRep_Tool::Pnt(V[Index_min]) );
        maxtol = Max(BRep_Tool::Tolerance(V[Index_min]),maxtol);
        //// modified by jgv, 18.09.02 for OCC571 ////
        //maxtol += Dist;
        maxtol = Max( Dist, maxtol );
        //////////////////////////////////////////////
        LeParamAmoi = BRep_Tool::Parameter(V[Index_min], E);    
      }
      else {   // Creation of an arc only
        maxtol = Max(BRep_Tool::Tolerance(E),maxtol);
        maxtol = Max(SP.Tolerance(),maxtol);
        LeParamAmoi = PR.ParameterOnArc();
      }

      // Definition of the arc
      TopAbs_Orientation Tr;
      TopAbs_Orientation Or = E.Orientation();
      if (Start) {
        Tr = TopAbs::Reverse(TopAbs::Compose(ChFi3d_TrsfTrans(TransLine),Or));
      }
      else {
        Tr = TopAbs::Compose(ChFi3d_TrsfTrans(TransLine),Or);
      }
      CP.SetArc(maxtol, E, LeParamAmoi, Tr);
    }
  }
  CP.SetTolerance(maxtol); // Finally, the tolerance.
}

//=======================================================================
//function : SolidIndex
//purpose  : 
//=======================================================================
Standard_Integer ChFi3d_SolidIndex(const Handle(ChFiDS_Spine)&  sp,
  TopOpeBRepDS_DataStructure&  DStr,
  ChFiDS_Map&                  MapESo,
  ChFiDS_Map&                  MapESh)
{
  if(sp.IsNull() || sp->NbEdges() == 0) 
    throw Standard_Failure("SolidIndex : Spine incomplete");
  TopoDS_Shape edref= sp->Edges(1);
  TopoDS_Shape shellousolid;
  if(!MapESo(edref).IsEmpty()) shellousolid = MapESo(edref).First();
  else shellousolid = MapESh(edref).First();
  const Standard_Integer solidindex = DStr.AddShape(shellousolid);
  return solidindex;
}
//=======================================================================
//function : IndexPointInDS
//purpose  : 
//=======================================================================
Standard_Integer  ChFi3d_IndexPointInDS(const ChFiDS_CommonPoint& P1,
  TopOpeBRepDS_DataStructure& DStr) 
{
  if (P1.IsVertex()) {
    // --------------------------------->  !*!*!* 
    // Attention : it is necessary ti implement a mechanism 
    // controlling tolerance.
    BRep_Builder B;
    B.UpdateVertex(P1.Vertex(), P1.Point(), P1.Tolerance());
    return DStr.AddShape(P1.Vertex());
  }
  return DStr.AddPoint(TopOpeBRepDS_Point(P1.Point(),P1.Tolerance()));
}
//=======================================================================
//function : FilPointInDS
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_CurvePointInterference) 
  ChFi3d_FilPointInDS(const TopAbs_Orientation Et,
  const Standard_Integer Ic,
  const Standard_Integer Ip,
  const Standard_Real Par,
  const Standard_Boolean IsVertex)
{
  Handle(TopOpeBRepDS_CurvePointInterference) CP1;
  if (IsVertex)    
    CP1 = new TopOpeBRepDS_CurvePointInterference (TopOpeBRepDS_Transition(Et),
    TopOpeBRepDS_CURVE,Ic,
    TopOpeBRepDS_VERTEX,Ip,Par); 
  else
    CP1 = new TopOpeBRepDS_CurvePointInterference (TopOpeBRepDS_Transition(Et),
    TopOpeBRepDS_CURVE,Ic,
    TopOpeBRepDS_POINT,Ip,Par);
  return CP1;
}
//=======================================================================
//function : FilVertexInDS
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_CurvePointInterference) 
  ChFi3d_FilVertexInDS(const TopAbs_Orientation Et,
  const Standard_Integer Ic,
  const Standard_Integer Ip,
  const Standard_Real Par)
{

  Handle(TopOpeBRepDS_CurvePointInterference) CP1 = new
    TopOpeBRepDS_CurvePointInterference (TopOpeBRepDS_Transition(Et),
    TopOpeBRepDS_CURVE,Ic,
    TopOpeBRepDS_VERTEX,Ip,Par);
  return CP1;
}
//=======================================================================
//function : Orientation
//purpose  : returns the orientation of the interference (the first found
//           in the list).
//=======================================================================

static Standard_Boolean
  ChFi3d_Orientation(const TopOpeBRepDS_ListOfInterference& LI,
  const Standard_Integer                 igros,
  const Standard_Integer                 ipetit,
  TopAbs_Orientation&                    Or,
  const Standard_Boolean                 isvertex = Standard_False,
  const Standard_Boolean                 aprendre = Standard_False)
{
  //In case, when it is necessary to insert a point/vertex, it should be 
  //known if this is a point or a vertex, because their index can be the same.
  TopOpeBRepDS_Kind typepetit;
  if (isvertex)
    typepetit =  TopOpeBRepDS_VERTEX;
  else
    typepetit =  TopOpeBRepDS_POINT;
  TopOpeBRepDS_ListIteratorOfListOfInterference itLI(LI);
  for (; itLI.More(); itLI.Next() ) {
    const Handle(TopOpeBRepDS_Interference)& cur = itLI.Value(); 
    TopOpeBRepDS_Kind GK;
    TopOpeBRepDS_Kind SK;
    Standard_Integer S;
    Standard_Integer G;
    cur->GKGSKS(GK,G,SK,S);
    if (aprendre) {
      if ( S == igros && G == ipetit && GK == typepetit) {
        Or = cur->Transition().Orientation(TopAbs_IN);
        return Standard_True;
      }
    }
    else  {
      if ( S == igros && G == ipetit) {
        Or = cur->Transition().Orientation(TopAbs_IN);
        return Standard_True;
      } 
    }
  }
  return Standard_False;
}

//=======================================================================
//function : Contains
//purpose  : Check if the interference does not already exist.
//====================================================================

static Standard_Boolean ChFi3d_Contains
  (const TopOpeBRepDS_ListOfInterference& LI,
  const Standard_Integer                 igros,
  const Standard_Integer                 ipetit,
  const Standard_Boolean                 isvertex = Standard_False,
  const Standard_Boolean                 aprendre = Standard_False)                             
{
  TopAbs_Orientation bidOr;
  return ChFi3d_Orientation(LI,igros,ipetit,bidOr,isvertex,aprendre);
}
//=======================================================================
//function : QueryAddVertexInEdge
//purpose  : 
//=======================================================================
static void QueryAddVertexInEdge(TopOpeBRepDS_ListOfInterference& LI,
  const Standard_Integer                 IC,
  const Standard_Integer                 IV,
  const Standard_Real                    par,
  const TopAbs_Orientation               Or)
{
  TopOpeBRepDS_ListIteratorOfListOfInterference it(LI);
  for (; it.More(); it.Next() ) {
    const Handle(TopOpeBRepDS_Interference)& cur = it.Value();
    Handle(TopOpeBRepDS_CurvePointInterference) cpi (Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(cur));
    if(!cpi.IsNull()) {
      Standard_Integer newIV = cpi->Geometry();
      TopOpeBRepDS_Kind kv = cpi->GeometryType();
      TopAbs_Orientation newOr = cpi->Transition().Orientation(TopAbs_IN);
      Standard_Real newpar = cpi->Parameter();
      if(IV == newIV && kv == TopOpeBRepDS_VERTEX && 
        Or == newOr && Abs(par - newpar) < 1.e-10) {
          return;
      }
    }
  }
  Handle(TopOpeBRepDS_CurvePointInterference) interf = 
    ChFi3d_FilVertexInDS(Or,IC,IV,par);
  LI.Append(interf);
}

//=======================================================================
//function : CutEdge
//purpose  : 
//=======================================================================
static void CutEdge(const TopoDS_Vertex&           V,
  const Handle(ChFiDS_SurfData)& SD,
  TopOpeBRepDS_DataStructure&    DStr,
  const Standard_Boolean         ,
  const Standard_Integer         ons)
{
  if(!SD->IsOnCurve(ons)) return;
  Standard_Integer IC = SD->IndexOfC(ons);
  Standard_Integer IV = DStr.AddShape(V);
  TopOpeBRepDS_ListOfInterference& LI = DStr.ChangeShapeInterferences(IC);
  TopoDS_Edge E = TopoDS::Edge(DStr.Shape(IC));
  E.Orientation(TopAbs_FORWARD);
  TopExp_Explorer ex;

  // process them checking that it has not been done already.
  for(ex.Init(E,TopAbs_VERTEX);ex.More();ex.Next()) {
    const TopoDS_Vertex& vv = TopoDS::Vertex(ex.Current());
    if(vv.IsSame(V)) {
      TopAbs_Orientation Or = TopAbs::Reverse(vv.Orientation());
      Standard_Real par = BRep_Tool::Parameter(vv,E);
      QueryAddVertexInEdge(LI,IC,IV,par,Or);
    }
  }
}
//=======================================================================
//function : findIndexPoint
//purpose  : returns in <ipon> index of point bounding a courve interfering
//           with <Fd> and coinciding with last common point on <OnS> face
//=======================================================================
static Standard_Boolean 
  findIndexPoint(const TopOpeBRepDS_DataStructure& DStr,
  const Handle(ChFiDS_SurfData)&    Fd,
  const Standard_Integer            OnS,
  Standard_Integer&                 ipoin)
{
  ipoin = 0;
  gp_Pnt P = Fd->Vertex(Standard_False,OnS).Point();

  TopOpeBRepDS_ListIteratorOfListOfInterference SCIIt, CPIIt;

  SCIIt.Initialize (DStr.SurfaceInterferences(Fd->Surf()));
  for (; SCIIt.More(); SCIIt.Next()) {
    Handle(TopOpeBRepDS_SurfaceCurveInterference) SCI =
      Handle(TopOpeBRepDS_SurfaceCurveInterference)::DownCast(SCIIt.Value());
    if (SCI.IsNull()) continue;
    CPIIt.Initialize (DStr.CurveInterferences(SCI->Geometry()));
    for (; CPIIt.More(); CPIIt.Next()) {
      Handle(TopOpeBRepDS_CurvePointInterference) CPI =
        Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(CPIIt.Value());
      if (CPI.IsNull()) continue;
      Standard_Integer iPoint = CPI->Geometry();
      TopOpeBRepDS_Point tp = DStr.Point(iPoint);
      if (P.IsEqual(tp.Point(), tp.Tolerance())) {
        ipoin = iPoint;
        return Standard_True;
      }
    }
  }
  return Standard_False;
}
//=======================================================================
//function : FilDS
//purpose  : 
//=======================================================================
void  ChFi3d_FilDS(const Standard_Integer       SolidIndex,
  const Handle(ChFiDS_Stripe)& CorDat,
  TopOpeBRepDS_DataStructure&  DStr,
  ChFiDS_Regularities&         reglist,
  const Standard_Real          tol3d,
  const Standard_Real          tol2d) 
{
  //  BRep_Tool Outil;
  TopExp_Explorer ex;
  Handle(ChFiDS_Spine) spine = CorDat->Spine();
  Standard_Boolean Closed = Standard_False;
  Standard_Boolean Degene = 0, isVertex1 = 0, isVertex2 = 0, Singulier_en_Bout = 0;
  if(!spine.IsNull()) {
    Closed = spine->IsPeriodic();
  }
  const ChFiDS_SequenceOfSurfData& SeqFil = 
    CorDat->SetOfSurfData()->Sequence();
  Standard_Integer Ipoin1 = CorDat->IndexFirstPointOnS1();
  Standard_Integer Ipoin2 = CorDat->IndexFirstPointOnS2();
  Standard_Integer NumEdge = 1;
  TopoDS_Vertex BoutdeVtx; 
  Standard_Integer Icurv = 0;
  Standard_Integer Iarc1 = 0,Iarc2 = 0;
  TopAbs_Orientation trafil1 = TopAbs_FORWARD, trafil2 = TopAbs_FORWARD;
  Standard_Integer IcFil1,IcFil2,Isurf,Ishape1,Ishape2;
  Standard_Real Pardeb = 0.,Parfin = 0.;
  TopAbs_Orientation ET1;
  Handle(TopOpeBRepDS_CurvePointInterference) Interfp1,Interfp2;
  Handle(TopOpeBRepDS_SurfaceCurveInterference) Interfc1,Interfc2;
  Handle(TopOpeBRepDS_SurfaceCurveInterference) Interfc3,Interfc4;
  Handle(TopOpeBRepDS_CurvePointInterference) Interfp3,Interfp4;
  Handle(TopOpeBRepDS_CurvePointInterference) Interfp5,Interfp6;
  TopoDS_Face F;
  Handle(Geom2d_Curve) PCurv;
  TopOpeBRepDS_Curve Crv;

  TopOpeBRepDS_ListOfInterference& SolidInterfs = 
    DStr.ChangeShapeInterferences(SolidIndex);

  ChFiDS_Regul regcout; // for closed and tangent CD
  ChFiDS_Regul regfilfil; // for connections Surf/Surf

  ChFiDS_CommonPoint V3;
  ChFiDS_CommonPoint V4;

  // Nullify degenerated ChFi/Faces interferences, eap occ293
  Standard_Integer j;
  if (SeqFil.Length() > 1) {
    for (j=1; j<=SeqFil.Length(); j++) {
      Handle(ChFiDS_SurfData) Fd = SeqFil(j);
      Standard_Integer onS;
      for (onS=1; onS<=2; onS++) {
        const ChFiDS_FaceInterference& Fi = Fd->Interference(onS);
        IcFil1 = Fi.LineIndex();
        if (!IcFil1) continue;
        Standard_Real FiLen = Abs(Fi.FirstParameter()-Fi.LastParameter());
        if (FiLen > Precision::PConfusion()) continue;
        TopOpeBRepDS_Curve& cc = DStr.ChangeCurve(IcFil1);
        cc.ChangeCurve().Nullify();

        // care of CommonPoint, eap occ354
        if (j!=1 && j!=SeqFil.Length()) continue;
        Standard_Boolean isfirst = (j==1);
        Standard_Integer i = isfirst ? j+1 : j-1;
        ChFiDS_CommonPoint& CP1 = SeqFil(i)->ChangeVertex(isfirst,onS);
        if (Fd->Vertex(isfirst,onS).IsOnArc() && CP1.IsOnArc()) {
          ChFiDS_CommonPoint& CP2 = Fd->ChangeVertex(!isfirst,onS);
          CP1.Reset();
          CP1.SetPoint(CP2.Point());
          CP2.Reset();
          CP2.SetPoint(CP1.Point());
        }
      }
    }
  }

  for (j=1; j<=SeqFil.Length(); j++) {

    const Handle(ChFiDS_SurfData)& Fd = SeqFil(j);
    Isurf= Fd->Surf();
    Ishape1 = Fd->IndexOfS1();
    Ishape2 = Fd->IndexOfS2();

    // eap, Apr 29 2002, occ 293
    // now IsInDS() returns nb of surfaces at end being in DS;
    // vars showing which end is in DS
    Standard_Boolean isInDS1 = Standard_False, isInDS2 = Standard_False;
    if (j <= CorDat->IsInDS(Standard_True)) {
      isInDS1 = Standard_True;
      isInDS2 = (j+1 <= CorDat->IsInDS(Standard_True));
    }
    if (SeqFil.Length()-j < CorDat->IsInDS(Standard_False)) {
      isInDS2 = Standard_True;
      isInDS1 = isInDS1 || SeqFil.Length()-j+1 < CorDat->IsInDS(Standard_False);
    }

    // creation of SolidSurfaceInterference

    Handle(TopOpeBRepDS_SolidSurfaceInterference) 
      SSI = new TopOpeBRepDS_SolidSurfaceInterference
      (TopOpeBRepDS_Transition(Fd->Orientation()),
      TopOpeBRepDS_SOLID,
      SolidIndex,
      TopOpeBRepDS_SURFACE,
      Isurf);

    SolidInterfs.Append(SSI);

    const ChFiDS_FaceInterference& Fi1 = Fd->InterferenceOnS1();
    const ChFiDS_FaceInterference& Fi2 = Fd->InterferenceOnS2();     
    const ChFiDS_CommonPoint& V1 = Fd->VertexFirstOnS1();
    const ChFiDS_CommonPoint& V2 = Fd->VertexFirstOnS2();

    // Processing to manage double interferences
    if (j>1) {
      if (V1.IsOnArc() && V3.IsOnArc() && V1.Arc().IsSame(V3.Arc())) {
        //Iarc1 is initialized
        //Iarc1 = DStr.AddShape(V1.Arc());
        if (ChFi3d_Contains(DStr.ShapeInterferences(Iarc1),Iarc1,Ipoin1) && 
          (V1.TransitionOnArc() != V3.TransitionOnArc()) ) {
            Interfp1= ChFi3d_FilPointInDS(V1.TransitionOnArc(),Iarc1,Ipoin1,
              V1.ParameterOnArc());
            DStr.ChangeShapeInterferences(V1.Arc()).Append(Interfp1);
        }
      }

      if (V2.IsOnArc() && V4.IsOnArc() && V2.Arc().IsSame(V4.Arc())) {
        //Iarc2 is initialized
        //Iarc2 = DStr.AddShape(V2.Arc());
        if ( ChFi3d_Contains(DStr.ShapeInterferences(Iarc2),Iarc2,Ipoin2)  && 
          (V2.TransitionOnArc() != V4.TransitionOnArc()) ) {
            Interfp2= ChFi3d_FilPointInDS(V2.TransitionOnArc(),Iarc2,Ipoin2,
              V2.ParameterOnArc());
            DStr.ChangeShapeInterferences(V2.Arc()).Append(Interfp2);
        }
      }
    }

    V3 = Fd->VertexLastOnS1();
    V4 = Fd->VertexLastOnS2();

    if(Ishape1 != 0) {
      if(Ishape1 > 0) {
        trafil1 = DStr.Shape(Ishape1).Orientation();
      }
      else{
        ChFi3d_Orientation(SolidInterfs,SolidIndex,-Ishape1,trafil1);
      }
      trafil1 = TopAbs::Compose(trafil1,Fd->Orientation());
      trafil1 = TopAbs::Compose(TopAbs::Reverse(Fi1.Transition()),trafil1);
      trafil2 = TopAbs::Reverse(trafil1);
    }
    else{
      if(Ishape2 > 0) {
        trafil2 = DStr.Shape(Ishape2).Orientation();
      }
      else{
        ChFi3d_Orientation(SolidInterfs,SolidIndex,-Ishape2,trafil2);
      }
      trafil2 = TopAbs::Compose(trafil2,Fd->Orientation());
      trafil2 = TopAbs::Compose(TopAbs::Reverse(Fi2.Transition()),trafil2);
      trafil1 = TopAbs::Reverse(trafil2);
    }

    ET1 = TopAbs::Reverse(trafil1);

    // A small paragraph to process contacts of edges, which touch 
    // a vertex of the obstacle.
    if(V1.IsVertex() && Fd->IsOnCurve1()) {
      const TopoDS_Vertex& vv1 = V1.Vertex();
      CutEdge(vv1,Fd,DStr,1,1);
    }
    if(V2.IsVertex() && Fd->IsOnCurve2()) {
      const TopoDS_Vertex& vv2 = V2.Vertex();
      CutEdge(vv2,Fd,DStr,1,2);
    }
    if(V3.IsVertex() && Fd->IsOnCurve1()) {
      const TopoDS_Vertex& vv3 = V3.Vertex();
      CutEdge(vv3,Fd,DStr,0,1);
    }
    if(V4.IsVertex() && Fd->IsOnCurve2()) {
      const TopoDS_Vertex& vv4 = V4.Vertex();
      CutEdge(vv4,Fd,DStr,0,2);
    }

    if (j == 1) {
      isVertex1 = V1.IsVertex();
      isVertex2 = V2.IsVertex();
      Singulier_en_Bout =  (V1.Point().IsEqual(V2.Point(), 0));

      if (Singulier_en_Bout) {
        // Queue de Billard
        if ((!V1.IsVertex()) || (!V2.IsVertex())) {

        }
        else {
          isVertex1 = isVertex2 = Standard_True; //caution...
          // The edge is removed from spine starting on this vertex.
          TopoDS_Edge Arcspine = spine->Edges(1);
          BoutdeVtx = V1.Vertex();
          Standard_Integer IArcspine = DStr.AddShape(Arcspine);
          Standard_Integer IVtx = CorDat->IndexFirstPointOnS1();

          TopAbs_Orientation OVtx = TopAbs_FORWARD;;

          for(ex.Init(Arcspine.Oriented(TopAbs_FORWARD),TopAbs_VERTEX); 
            ex.More(); ex.Next()) {
              if(BoutdeVtx.IsSame(ex.Current())) {
                OVtx = ex.Current().Orientation();
                break;
              }
          }
          OVtx = TopAbs::Reverse(OVtx);
          Standard_Real parVtx = BRep_Tool::Parameter(BoutdeVtx,Arcspine);
          Handle(TopOpeBRepDS_CurvePointInterference) 
            interfv = ChFi3d_FilVertexInDS(OVtx,IArcspine,IVtx,parVtx);
          DStr.ChangeShapeInterferences(IArcspine).Append(interfv);
        }
      }
      else {
        if (V1.IsOnArc()) {
          Iarc1 = DStr.AddShape(V1.Arc()); 
          if ( !ChFi3d_Contains(DStr.ShapeInterferences(Iarc1),Iarc1,Ipoin1) ) {
            Interfp1= ChFi3d_FilPointInDS(V1.TransitionOnArc(),Iarc1,Ipoin1,
              V1.ParameterOnArc(), isVertex1);
            DStr.ChangeShapeInterferences(V1.Arc()).Append(Interfp1);
          }
        }

        if (V2.IsOnArc()) {
          Iarc2 = DStr.AddShape(V2.Arc());
          if ( !ChFi3d_Contains(DStr.ShapeInterferences(Iarc2),Iarc2,Ipoin2) ) {
            Interfp2= ChFi3d_FilPointInDS(V2.TransitionOnArc(),Iarc2,Ipoin2,
              V2.ParameterOnArc(),isVertex2);
            DStr.ChangeShapeInterferences(V2.Arc()).Append(Interfp2);
          }
        }
      }

      if (!isInDS1) {
        ET1 = TopAbs::Compose(ET1,CorDat->FirstPCurveOrientation());
        Icurv = CorDat->FirstCurve();
        if(Closed && !Singulier_en_Bout) {
          regcout.SetCurve(Icurv);
          regcout.SetS1(Isurf,Standard_False);
        }
        PCurv = CorDat->FirstPCurve();
        CorDat->FirstParameters(Pardeb,Parfin);

        TopOpeBRepDS_ListOfInterference& Li = DStr.ChangeCurveInterferences(Icurv);
        if (Li.IsEmpty()) {
          if(CorDat->FirstPCurveOrientation()==TopAbs_REVERSED) {
            Interfp1=ChFi3d_FilPointInDS
              (TopAbs_REVERSED,Icurv,Ipoin1,Parfin,isVertex1);
            Interfp2=ChFi3d_FilPointInDS
              (TopAbs_FORWARD,Icurv,Ipoin2,Pardeb,isVertex2);
          }
          else{
            Interfp1=ChFi3d_FilPointInDS
              (TopAbs_FORWARD,Icurv,Ipoin1,Pardeb,isVertex1);
            Interfp2=ChFi3d_FilPointInDS
              (TopAbs_REVERSED,Icurv,Ipoin2,Parfin,isVertex2);
          }
          Li.Append(Interfp1);
          Li.Append(Interfp2);
        }
        Interfc1= ChFi3d_FilCurveInDS (Icurv,Isurf,PCurv,ET1);
        DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc1);
        if (Ipoin1 == Ipoin2) {
          TopOpeBRepDS_Curve& TCurv = DStr.ChangeCurve(Icurv);
          TCurv.ChangeCurve().Nullify();
          Handle(TopOpeBRepDS_Interference) bidinterf;
          TCurv.SetSCI(Interfc1,bidinterf);	    
        }
      }
    } // End of the Initial Processing (j==1)
    else {
      // ---- Interference between Fillets ------

      if (!isInDS1) {// eap, Apr 29 2002, occ 293 

        if (Degene && isVertex1) {
          // The edge is removed from the spine starting on this vertex.
          NumEdge++; // The previous edge of the vertex has already been found.
          TopoDS_Edge Arcspine = spine->Edges(NumEdge);
          Standard_Integer IArcspine = DStr.AddShape(Arcspine);
          Standard_Integer IVtx = DStr.AddShape(BoutdeVtx);
          TopAbs_Orientation OVtx = TopAbs_FORWARD;
          for(ex.Init(Arcspine.Oriented(TopAbs_FORWARD),TopAbs_VERTEX); 
            ex.More(); ex.Next()) {
              if(BoutdeVtx.IsSame(ex.Current())) {
                OVtx = ex.Current().Orientation();
                break;
              }
          }
          OVtx = TopAbs::Reverse(OVtx);
          Standard_Real parVtx = BRep_Tool::Parameter(BoutdeVtx,Arcspine);
          Handle(TopOpeBRepDS_CurvePointInterference) 
            interfv = ChFi3d_FilVertexInDS(OVtx,IArcspine,IVtx,parVtx);
          DStr.ChangeShapeInterferences(IArcspine).Append(interfv);
        } // End of the removal

        gp_Pnt2d UV1 = Fd->InterferenceOnS1().PCurveOnSurf()->
          Value(Fd->InterferenceOnS1().FirstParameter());
        gp_Pnt2d UV2 = Fd->InterferenceOnS2().PCurveOnSurf()-> 
          Value(Fd->InterferenceOnS2().FirstParameter());
        TopOpeBRepDS_Curve& TCurv = DStr.ChangeCurve(Icurv);
        if (Degene) {
          // pcurve is associated via SCI to TopOpeBRepDSCurve.
          ChFi3d_ComputePCurv(UV1,UV2,PCurv,Pardeb,Parfin);       
          Interfc1= ChFi3d_FilCurveInDS (Icurv,Isurf,PCurv,ET1);
          DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc1);
          TCurv.ChangeCurve().Nullify();
          Handle(TopOpeBRepDS_Interference) bidinterf;
          TCurv.SetSCI(Interfc1,bidinterf);           
        }
        else {
          regfilfil.SetS2(Isurf,Standard_False);
          reglist.Append(regfilfil);
          Standard_Real tolreached;
          ChFi3d_ComputePCurv(TCurv.ChangeCurve(),UV1,UV2,PCurv,
            DStr.Surface(Fd->Surf()).Surface(),
            Pardeb,Parfin,tol3d,tolreached);
          TCurv.Tolerance(Max(TCurv.Tolerance(),tolreached));
          Interfc1= ChFi3d_FilCurveInDS (Icurv,Isurf,PCurv,ET1);
          DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc1);
        }
      }
    } // End of Interference between fillets 

    // ---- Interference Fillets / Faces
    IcFil1 = Fi1.LineIndex();

    if (IcFil1!=0 ) {
      Interfc3= ChFi3d_FilCurveInDS (IcFil1,Isurf,
        Fi1.PCurveOnSurf(),trafil1);
      DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc3);
      Ishape1 = Fd->IndexOfS1();
      // Case of degenerated edge : pcurve is associated via SCI 
      // to TopOpeBRepDSCurve.
      TopOpeBRepDS_Curve& cc = DStr.ChangeCurve(IcFil1);
      if(cc.Curve().IsNull()) {
        Handle(TopOpeBRepDS_Interference) bidinterf;
        cc.SetSCI(Interfc3,bidinterf);
      }
      else{
        ChFiDS_Regul regon1;
        regon1.SetCurve(IcFil1);
        regon1.SetS1(Isurf,Standard_False);
        if ( Ishape1 < 0 ) {
          Ishape1 = -Ishape1;
          regon1.SetS2(Ishape1,Standard_False);
          Interfc1=ChFi3d_FilCurveInDS(IcFil1,Ishape1,Fi1.PCurveOnFace(),
            Fi1.Transition()); 
          DStr.ChangeSurfaceInterferences(Ishape1).Append(Interfc1);      
        }
        else if ( Ishape1 > 0 ) {
          regon1.SetS2(Ishape1,Standard_True);
          Interfc1=ChFi3d_FilCurveInDS(IcFil1,Ishape1,Fi1.PCurveOnFace(),
            Fi1.Transition()); 
          DStr.ChangeShapeInterferences(Ishape1).Append(Interfc1);      
        }
        reglist.Append(regon1);
      }
      // Indice and type of the point at End
      Standard_Integer ipoin;
      Standard_Boolean isVertex = Fd->VertexLastOnS1().IsVertex();
      if (j == SeqFil.Length()) ipoin = CorDat->IndexLastPointOnS1();
      else if ( j == (SeqFil.Length()-1)  &&  /*Closed &&*/
        (DStr.Curve(SeqFil.Last()->InterferenceOnS1().
        LineIndex()).Curve().IsNull())) {
          if (Closed) {
            ipoin = CorDat->IndexFirstPointOnS1();
            isVertex = SeqFil(1)->VertexFirstOnS1().IsVertex();
          } else {
            ipoin = CorDat->IndexLastPointOnS1();
            isVertex = SeqFil.Last()->VertexLastOnS1().IsVertex();
          }
      }
      else if(DStr.Curve(IcFil1).Curve().IsNull()) {// Rotation !!
        ipoin = Ipoin1;
        isVertex = isVertex1;
      }
      else if ( ((j==1) || (j== SeqFil.Length()-1)) && 
        ( (Fd->VertexLastOnS1().Point().IsEqual(
        SeqFil(1)->VertexFirstOnS1().Point(), 1.e-7)) ||
        (Fd->VertexLastOnS1().Point().IsEqual(
        SeqFil(SeqFil.Length())->VertexLastOnS1().Point(), 1.e-7))) )
        // Case of SurfData cut in "Triangular" way.   
        ipoin=CorDat->IndexLastPointOnS1();

      // eap, Apr 29 2002, occ 293
      else if (isInDS2 && findIndexPoint(DStr, Fd, 1, ipoin)) {

      }
      else ipoin = ChFi3d_IndexPointInDS(Fd->VertexLastOnS1(),DStr);

      TopOpeBRepDS_ListOfInterference& Li = DStr.ChangeCurveInterferences(IcFil1);

      if (!ChFi3d_Contains(Li,IcFil1,Ipoin1)) { 

        Interfp1 = ChFi3d_FilPointInDS(TopAbs_FORWARD,IcFil1,Ipoin1,
          Fi1.FirstParameter(),isVertex1);
        DStr.ChangeCurveInterferences(IcFil1).Append(Interfp1);
      }
      if (ipoin == Ipoin1 || !ChFi3d_Contains(Li,IcFil1,ipoin)) { 
        Interfp3 = ChFi3d_FilPointInDS(TopAbs_REVERSED,IcFil1,ipoin,
          Fi1.LastParameter(), isVertex);
        DStr.ChangeCurveInterferences(IcFil1).Append(Interfp3);
      }
      Ipoin1 = ipoin;
      isVertex1 = isVertex;
    }

    IcFil2 = Fi2.LineIndex();
    if (IcFil2!=0) {
      Interfc4=ChFi3d_FilCurveInDS(IcFil2,Isurf,
        Fi2.PCurveOnSurf(),trafil2);
      DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc4);
      Ishape2 = Fd->IndexOfS2();
      // Case of degenerated edge : pcurve is associated via SCI 
      // to TopOpeBRepDSCurve.
      TopOpeBRepDS_Curve& cc = DStr.ChangeCurve(IcFil2);
      if(cc.Curve().IsNull()) {
        Handle(TopOpeBRepDS_Interference) bidinterf;
        cc.SetSCI(Interfc4,bidinterf);
      }
      else{
        ChFiDS_Regul regon2;
        regon2.SetCurve(IcFil2);
        regon2.SetS1(Isurf,Standard_False);
        if ( Ishape2 < 0 ) {
          Ishape2 = -Ishape2;
          regon2.SetS2(Ishape2,Standard_False);
          Interfc2=ChFi3d_FilCurveInDS(IcFil2,Ishape2,Fi2.PCurveOnFace(),
            Fi2.Transition());
          DStr.ChangeSurfaceInterferences(Ishape2).Append(Interfc2);      
        }
        else if ( Ishape2 > 0 ) {
          regon2.SetS2(Ishape2,Standard_True);
          Interfc2=ChFi3d_FilCurveInDS(IcFil2,Ishape2,Fi2.PCurveOnFace(),
            Fi2.Transition());
          DStr.ChangeShapeInterferences(Ishape2).Append(Interfc2);      
        }
        reglist.Append(regon2);
      }
      // Indice and type of the point in End
      Standard_Integer ipoin;
      Standard_Boolean isVertex = Fd->VertexLastOnS2().IsVertex();
      if (j == SeqFil.Length() ) ipoin = CorDat->IndexLastPointOnS2();
      else if ( j == (SeqFil.Length()-1)  && /*Closed &&*/
        (DStr.Curve(SeqFil.Last()->InterferenceOnS2().
        LineIndex()).Curve().IsNull())) {
          if (Closed) {
            ipoin = CorDat->IndexFirstPointOnS2();
            isVertex = SeqFil(1)->VertexFirstOnS2().IsVertex();
          } else {
            ipoin = CorDat->IndexLastPointOnS2();
            isVertex = SeqFil.Last()->VertexLastOnS2().IsVertex();
          }
      }
      else if(DStr.Curve(IcFil2).Curve().IsNull()) { // Rotation !!
        ipoin = Ipoin2;
        isVertex = isVertex2;
      }
      else if(Fd->VertexLastOnS2().Point().IsEqual(
        Fd->VertexLastOnS1().Point(), 0) ) {  //Pinch !!
          ipoin = Ipoin1;
          isVertex = isVertex1;
      }
      else if ( ((j==1) || (j==SeqFil.Length()-1)) && 
        ( (Fd->VertexLastOnS2().Point().IsEqual(
        SeqFil(1)->VertexFirstOnS2().Point(), 1.e-7)) ||
        (Fd->VertexLastOnS2().Point().IsEqual(
        SeqFil(SeqFil.Length())->VertexLastOnS2().Point(), 1.e-7))) )
        // Case of SurfData cut in "Triangular" way.   
        ipoin=CorDat->IndexLastPointOnS2();

      // eap, Apr 29 2002, occ 293
      else if (isInDS2 && findIndexPoint(DStr, Fd, 2, ipoin)) {

      }
      else ipoin = ChFi3d_IndexPointInDS(Fd->VertexLastOnS2(),DStr);

      TopOpeBRepDS_ListOfInterference& Li = DStr.ChangeCurveInterferences(IcFil2);

      if (!ChFi3d_Contains(Li,IcFil2,Ipoin2)) { 
        Interfp2 = ChFi3d_FilPointInDS(TopAbs_FORWARD,IcFil2,Ipoin2,
          Fi2.FirstParameter(), isVertex2);
        DStr.ChangeCurveInterferences(IcFil2).Append(Interfp2);
      }
      if (ipoin == Ipoin2 || !ChFi3d_Contains(Li,IcFil2,ipoin)) { 
        Interfp4= ChFi3d_FilPointInDS(TopAbs_REVERSED,IcFil2,ipoin,
          Fi2.LastParameter(), isVertex );
        DStr.ChangeCurveInterferences(IcFil2).Append(Interfp4);
      }
      Ipoin2 = ipoin;
      isVertex2 = isVertex;      
    }

    ET1 = trafil1;
    if (j == SeqFil.Length()) {
      if (!isInDS2) {
        Icurv = CorDat->LastCurve();
        if(Closed && !Singulier_en_Bout && (Ipoin1!=Ipoin2)) {
          regcout.SetS2(Isurf,Standard_False);
          reglist.Append(regcout);
        }
        PCurv = CorDat->LastPCurve();
        ET1 = TopAbs::Compose(ET1,CorDat->LastPCurveOrientation());
        CorDat->LastParameters(Pardeb,Parfin);
        TopOpeBRepDS_ListOfInterference& Li = DStr.ChangeCurveInterferences(Icurv);
        if (Li.IsEmpty()) {
          if(CorDat->LastPCurveOrientation()==TopAbs_REVERSED) {
            Interfp5=ChFi3d_FilPointInDS
              (TopAbs_REVERSED,Icurv,Ipoin1,Parfin, isVertex1);
            Interfp6=ChFi3d_FilPointInDS
              (TopAbs_FORWARD,Icurv,Ipoin2,Pardeb, isVertex2);
          }
          else{
            Interfp5=ChFi3d_FilPointInDS
              (TopAbs_FORWARD,Icurv,Ipoin1,Pardeb, isVertex1);
            Interfp6=ChFi3d_FilPointInDS
              (TopAbs_REVERSED,Icurv,Ipoin2,Parfin, isVertex2);
          }
          Li.Append(Interfp5);
          Li.Append(Interfp6);
        }
        Interfc1= ChFi3d_FilCurveInDS(Icurv,Isurf,PCurv,ET1);
        DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc1);
        if (Ipoin1 == Ipoin2) {
          TopOpeBRepDS_Curve& TCurv = DStr.ChangeCurve(Icurv);
          TCurv.ChangeCurve().Nullify();
          Handle(TopOpeBRepDS_Interference) bidinterf;
          TCurv.SetSCI( Interfc1, bidinterf);
          //         bidinterf = TCurv.GetSCI1(); 
          //	  TCurv.SetSCI(bidinterf, Interfc1);	    
        }
      }
    }
    else {
      //      Degene = (Fd->VertexLastOnS1().Point().IsEqual(
      //                Fd->VertexLastOnS2().Point(), 0) );

      // eap, Apr 29 2002, occ 293 
      if (!isInDS2) {

        Handle(Geom_Curve) C3d;
        Standard_Real tolreached;
        ChFi3d_ComputeArete(Fd->VertexLastOnS1(),
          Fd->InterferenceOnS1().PCurveOnSurf()->
          Value(Fd->InterferenceOnS1().LastParameter()),
          Fd->VertexLastOnS2(),
          Fd->InterferenceOnS2().PCurveOnSurf()->
          Value(Fd->InterferenceOnS2().LastParameter()),
          DStr.Surface(Fd->Surf()).Surface(),C3d,PCurv,
          Pardeb,Parfin,tol3d,tol2d,tolreached,0);
        Crv = TopOpeBRepDS_Curve(C3d,tolreached);
        Icurv = DStr.AddCurve(Crv);
        regfilfil.SetCurve(Icurv);
        regfilfil.SetS1(Isurf,Standard_False);
        Interfp5 = ChFi3d_FilPointInDS(TopAbs_FORWARD,Icurv,Ipoin1,Pardeb, isVertex1);
        DStr.ChangeCurveInterferences(Icurv).Append(Interfp5);
        Interfp6= ChFi3d_FilPointInDS(TopAbs_REVERSED,Icurv,Ipoin2,Parfin, isVertex2);
        DStr.ChangeCurveInterferences(Icurv).Append(Interfp6);
        Interfc1= ChFi3d_FilCurveInDS(Icurv,Isurf,PCurv,ET1);
        DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc1);      
      }
    }

    Degene = V3.Point().IsEqual(V4.Point(), 0);

    // Processing of degenerated case     
    if (Degene) {
      // Queue de Billard
      Standard_Boolean Vertex = (V3.IsVertex()) && (V4.IsVertex());
      if (!Vertex) {

      }
      else {
        // The edge of the spine starting on this vertex is removed.
        Standard_Boolean Trouve = Standard_False;
        TopoDS_Edge Arcspine;
        TopAbs_Orientation OVtx = TopAbs_FORWARD;
        BoutdeVtx = V3.Vertex();

        while (NumEdge<= spine->NbEdges() && !Trouve) { 
          Arcspine = spine->Edges(NumEdge);
          for(ex.Init(Arcspine.Oriented(TopAbs_FORWARD),TopAbs_VERTEX); 
            ex.More() && (!Trouve); ex.Next()) {
              if(BoutdeVtx.IsSame(ex.Current())) {
                OVtx = ex.Current().Orientation();
                if (Closed &&  (NumEdge == 1)) 
                  Trouve = (spine->NbEdges() == 1);
                else Trouve = Standard_True;
              }
          }
          if (!Trouve) NumEdge++; // Go to the next edge
        }
        Standard_Integer IArcspine = DStr.AddShape(Arcspine);
        Standard_Integer IVtx;
        if  (j == SeqFil.Length()) {
          IVtx = CorDat->IndexLastPointOnS1();
        }
        else { IVtx = DStr.AddShape(BoutdeVtx); }
        OVtx = TopAbs::Reverse(OVtx);
        Standard_Real parVtx = BRep_Tool::Parameter(BoutdeVtx,Arcspine);
        Handle(TopOpeBRepDS_CurvePointInterference) 
          interfv = ChFi3d_FilVertexInDS(OVtx,IArcspine,IVtx,parVtx);
        DStr.ChangeShapeInterferences(IArcspine).Append(interfv);
      }
    } // end of degenerated case
    else if (!(Closed && j == SeqFil.Length())) {
      // Processing of interference Point / Edges
      if (V3.IsOnArc()) {
        if(!(V3.IsVertex() && Fd->IsOnCurve1())) {
          Iarc1 = DStr.AddShape(V3.Arc());
          if ( !ChFi3d_Contains(DStr.ShapeInterferences(Iarc1),Iarc1,Ipoin1,V3.IsVertex(),Standard_True) ) {
            Handle(TopOpeBRepDS_CurvePointInterference) Interfpp = 
              ChFi3d_FilPointInDS(V3.TransitionOnArc(),
              Iarc1,Ipoin1,V3.ParameterOnArc(), V3.IsVertex());
            DStr.ChangeShapeInterferences(V3.Arc()).Append(Interfpp);
          }
        }
      }

      if (V4.IsOnArc()) {
        if(!(V4.IsVertex() && Fd->IsOnCurve2())) {
          Iarc2 = DStr.AddShape(V4.Arc());
          if ( !ChFi3d_Contains(DStr.ShapeInterferences(Iarc2),Iarc2,Ipoin2,V4.IsVertex(),Standard_True) ) {
            Handle(TopOpeBRepDS_CurvePointInterference) Intfpp=
              ChFi3d_FilPointInDS(V4.TransitionOnArc(),
              Iarc2,Ipoin2,V4.ParameterOnArc(), V4.IsVertex());
            DStr.ChangeShapeInterferences(V4.Arc()).Append(Intfpp);
          }
        }
      }
    }
  }
}
//=======================================================================
//function : StripeEdgeInter
//purpose  : This function examines two stripes for an intersection 
//           between curves of interference with faces. If the intersection
//           exists, it will cause bad result, so it's better to quit.
//remark   : If someone somewhen computes the interference between stripes, 
//           this function will become useless.
//author   : akm, 06/02/02. Against bug OCC119.
//=======================================================================
void ChFi3d_StripeEdgeInter (const Handle(ChFiDS_Stripe)& theStripe1,
  const Handle(ChFiDS_Stripe)& theStripe2,
  TopOpeBRepDS_DataStructure&  /*DStr*/,
  const Standard_Real          tol2d)
{
  // Do not check the stripeshaving common corner points
  for (Standard_Integer iSur1=1; iSur1<=2; iSur1++)
    for (Standard_Integer iSur2=1; iSur2<=2; iSur2++)
      if (theStripe1->IndexPoint(0,iSur1)==theStripe2->IndexPoint(0,iSur2) ||
        theStripe1->IndexPoint(0,iSur1)==theStripe2->IndexPoint(1,iSur2) ||
        theStripe1->IndexPoint(1,iSur1)==theStripe2->IndexPoint(0,iSur2) ||
        theStripe1->IndexPoint(1,iSur1)==theStripe2->IndexPoint(1,iSur2))
        return;

  Handle(ChFiDS_HData) aSurDat1 = theStripe1->SetOfSurfData();
  Handle(ChFiDS_HData) aSurDat2 = theStripe2->SetOfSurfData();

  Geom2dInt_GInter anIntersector;
  Standard_Integer iPart1, iPart2;
  Standard_Integer Ishape11, Ishape12, Ishape21, Ishape22;
  // Loop on parts of the first stripe
  for (iPart1=1; iPart1<=aSurDat1->Length(); iPart1++) 
  {
    Handle(ChFiDS_SurfData) aDat1 = aSurDat1->Value(iPart1);
    Ishape11 = aDat1->IndexOfS1();
    Ishape12 = aDat1->IndexOfS2();
    // Loop on parts of the second stripe
    for (iPart2=1; iPart2<=aSurDat2->Length(); iPart2++) 
    {
      Handle(ChFiDS_SurfData) aDat2 = aSurDat2->Value(iPart2);
      Ishape21 = aDat2->IndexOfS1();
      Ishape22 = aDat2->IndexOfS2();

      // Find those FaceInterferences able to intersect
      ChFiDS_FaceInterference aFI1, aFI2;
      if (Ishape11 == Ishape21)
      {
        aFI1 = aDat1->InterferenceOnS1();
        aFI2 = aDat2->InterferenceOnS1();
      } 
      else if (Ishape11 == Ishape22)
      {
        aFI1 = aDat1->InterferenceOnS1();
        aFI2 = aDat2->InterferenceOnS2();
      } 
      else if (Ishape12 == Ishape21)
      {
        aFI1 = aDat1->InterferenceOnS2();
        aFI2 = aDat2->InterferenceOnS1();
      } 
      else if (Ishape12 == Ishape22)
      {
        aFI1 = aDat1->InterferenceOnS2();
        aFI2 = aDat2->InterferenceOnS2();
      }
      else
      {
        // No common faces
        continue;
      }

      if (IsEqual (aFI1.FirstParameter(),aFI1.LastParameter()) ||
        IsEqual (aFI2.FirstParameter(),aFI2.LastParameter()) ||
        aFI1.PCurveOnFace().IsNull() ||
        aFI2.PCurveOnFace().IsNull())
        // Do not waste time on degenerates
        continue;
      // Examine for intersections
      Geom2dAdaptor_Curve aPCurve1 (aFI1.PCurveOnFace(),
        aFI1.FirstParameter(),
        aFI1.LastParameter());
      Geom2dAdaptor_Curve aPCurve2 (aFI2.PCurveOnFace(),
        aFI2.FirstParameter(),
        aFI2.LastParameter());
      anIntersector.Perform (aPCurve1,
        aPCurve2,
        tol2d,
        Precision::PConfusion());
      if (anIntersector.NbSegments() > 0 ||
        anIntersector.NbPoints() > 0)
        throw StdFail_NotDone("StripeEdgeInter : fillets have too big radiuses");
    }
  }
}

//=======================================================================
//function : IndexOfSurfData
//purpose  : 
//=======================================================================
Standard_Integer ChFi3d_IndexOfSurfData(const TopoDS_Vertex& V1,
  const Handle(ChFiDS_Stripe)& CD,
  Standard_Integer& sens)
{
  Handle(ChFiDS_Spine) spine = CD->Spine();
  Standard_Integer Index = 0;
  sens = 1;
  TopoDS_Vertex Vref;
  const TopoDS_Edge& E = spine->Edges(1);
  if (E.Orientation() == TopAbs_REVERSED) Vref = TopExp::LastVertex(E);
  else Vref = TopExp::FirstVertex(E); 
  if (Vref.IsSame(V1)) Index =1;
  else {
    const TopoDS_Edge& E1 = spine->Edges(spine->NbEdges());
    if (E1.Orientation() == TopAbs_REVERSED) Vref = TopExp::FirstVertex(E1);
    else Vref = TopExp::LastVertex(E1); 
    sens = -1;
    if(CD->SetOfSurfData().IsNull()) return 0;
    else if (Vref.IsSame(V1)) Index = CD->SetOfSurfData()->Length();
    else throw Standard_ConstructionError ("ChFi3d_IndexOfSurfData() - wrong construction parameters");
  }
  return Index; 
}  
//=======================================================================
//function : EdgeFromV1
//purpose  : 
//=======================================================================

TopoDS_Edge ChFi3d_EdgeFromV1(const TopoDS_Vertex& V1,
  const Handle(ChFiDS_Stripe)& CD,
  Standard_Integer& sens)
{
  Handle(ChFiDS_Spine) spine = CD->Spine();
  sens = 1;
  TopoDS_Vertex Vref;
  const TopoDS_Edge& E = spine->Edges(1);
  if (E.Orientation() == TopAbs_REVERSED) Vref = TopExp::LastVertex(E);
  else Vref = TopExp::FirstVertex(E); 
  if (Vref.IsSame(V1)) return E;
  else
  {
    const TopoDS_Edge& E1 = spine->Edges(spine->NbEdges());
    if (E1.Orientation() == TopAbs_REVERSED) Vref = TopExp::FirstVertex(E1);
    else Vref = TopExp::LastVertex(E1); 
    sens = -1;
    if (Vref.IsSame(V1)) return E1;
    else throw Standard_ConstructionError ("ChFi3d_IndexOfSurfData() - wrong construction parameters");
  }
}
//=======================================================================
//function : ConvTol2dToTol3d
//purpose  : Comme son nom l indique.
//=======================================================================

Standard_Real ChFi3d_ConvTol2dToTol3d(const Handle(Adaptor3d_HSurface)& S,
  const Standard_Real             tol2d)
{
  Standard_Real ures = S->UResolution(1.e-7);
  Standard_Real vres = S->VResolution(1.e-7);
  Standard_Real uresto3d = 1.e-7*tol2d/ures;
  Standard_Real vresto3d = 1.e-7*tol2d/vres;
  return Max(uresto3d,vresto3d);
}
//=======================================================================
//function : EvalTolReached
//purpose  : The function above is too hard because 
//           parametrization of surfaces is not homogenous.
//=======================================================================

Standard_Real ChFi3d_EvalTolReached(const Handle(Adaptor3d_HSurface)& S1,
  const Handle(Geom2d_Curve)&     pc1,
  const Handle(Adaptor3d_HSurface)& S2,
  const Handle(Geom2d_Curve)&     pc2,
  const Handle(Geom_Curve)&       C)
{
  Standard_Real distmax = 0.;

  Standard_Real f = C->FirstParameter();
  Standard_Real l = C->LastParameter();
  Standard_Integer nbp = 45;
  Standard_Real step = 1./(nbp -1);
  for(Standard_Integer i = 0; i < nbp; i++) {
    Standard_Real t,u,v;
    t = step * i;
    t = (1-t) * f + t * l;
    pc1->Value(t).Coord(u,v);
    gp_Pnt pS1 = S1->Value(u,v);
    pc2->Value(t).Coord(u,v);
    gp_Pnt pS2 = S2->Value(u,v);
    gp_Pnt pC = C->Value(t);
    Standard_Real d = pS1.SquareDistance(pC);
    if(d>distmax) distmax = d;
    d = pS2.SquareDistance(pC);
    if(d>distmax) distmax = d;
    d = pS1.SquareDistance(pS2);
    if(d>distmax) distmax = d;
  }
  distmax = 1.5*sqrt(distmax);
  distmax = Max(distmax, Precision::Confusion());
  return distmax;
}

//=======================================================================
//function : trsfsurf
//purpose  : 
//=======================================================================
Handle(Geom_Surface) trsfsurf(const Handle(Adaptor3d_HSurface)& HS,
  Handle(Adaptor3d_TopolTool)&      /*dom*/)
{
  //Pour l utilisation des domaines voir avec BUBUCH!!
  Handle(Geom_Surface) res;
  Handle(BRepAdaptor_HSurface) hbs = Handle(BRepAdaptor_HSurface)::DownCast(HS);
  Handle(GeomAdaptor_HSurface) hgs = Handle(GeomAdaptor_HSurface)::DownCast(HS);
  if(!hbs.IsNull()) {
    res = hbs->ChangeSurface().Surface().Surface();
    gp_Trsf trsf = hbs->ChangeSurface().Trsf();
    res = Handle(Geom_Surface)::DownCast(res->Transformed(trsf));
  }
  else if(!hgs.IsNull()) {
    res = hgs->ChangeSurface().Surface();
  }
  Handle(Geom_RectangularTrimmedSurface) 
    tr = Handle(Geom_RectangularTrimmedSurface)::DownCast(res);
  if(!tr.IsNull()) res = tr->BasisSurface();

  Standard_Real U1 = HS->FirstUParameter(), U2 = HS->LastUParameter();
  Standard_Real V1 = HS->FirstVParameter(), V2 = HS->LastVParameter();
  if(!res.IsNull()) {
    // Protection against Construction Errors
    Standard_Real u1, u2, v1, v2;
    res->Bounds( u1, u2, v1, v2);
    if (!res->IsUPeriodic()) {
      if (U1 < u1) U1 = u1;
      if (U2 > u2) U2 = u2;
    }
    if (!res->IsVPeriodic()) {
      if (V1 < v1) V1 = v1;
      if (V2 > v2) V2 = v2;
    }
    res = new Geom_RectangularTrimmedSurface(res,U1,U2,V1,V2);
  }
  //  Handle(GeomAdaptor_HSurface) temp = new GeomAdaptor_HSurface(res,U1,U2,V1,V2);
  //  dom = new Adaptor3d_TopolTool(temp);
  return res;
}
//=======================================================================
//function : CurveCleaner
//purpose  : Makes a BSpline as much continued as possible
//           at a given tolerance
//=======================================================================
static void CurveCleaner(Handle(Geom_BSplineCurve)& BS, 
  const Standard_Real Tol,
  const Standard_Integer MultMin)

{
  Standard_Real tol = Tol;
  Standard_Integer Mult, ii;
  const Standard_Integer NbK=BS->NbKnots();

  for (Mult = BS->Degree(); Mult > MultMin; Mult--) {
    tol *= 0.5; // Progressive reduction
    for (ii=NbK; ii>1; ii--) {
      if (BS->Multiplicity(ii) == Mult)
        BS->RemoveKnot(ii, Mult-1, tol);
    }
  }
}
//=======================================================================
//function : ComputeCurves
//purpose  : Calculates intersection between two HSurfaces.
//           It is necessary to know the extremities of intersection and  
//           the surfaces should be processed at input 
//           to fit as good as possible (neither too close nor too far) 
//           the points of beginning and end of the intersection.
//           The analytic intersections are processed separately.
//           <wholeCurv> means that the resulting curve is restricted by
//           boundaries of input surfaces (eap 30 May occ354)
//=======================================================================
Standard_Boolean ChFi3d_ComputeCurves(const Handle(Adaptor3d_HSurface)&   S1,
  const Handle(Adaptor3d_HSurface)&   S2,
  const TColStd_Array1OfReal& Pardeb,
  const TColStd_Array1OfReal& Parfin,
  Handle(Geom_Curve)&         C3d,
  Handle(Geom2d_Curve)&       Pc1,
  Handle(Geom2d_Curve)&       Pc2,
  const Standard_Real         tol3d,
  const Standard_Real         tol2d,
  Standard_Real&              tolreached,
  const Standard_Boolean      wholeCurv) 
{
  gp_Pnt pdeb1 = S1->Value(Pardeb(1),Pardeb(2));
  gp_Pnt pfin1 = S1->Value(Parfin(1),Parfin(2));
  gp_Pnt pdeb2 = S2->Value(Pardeb(3),Pardeb(4));
  gp_Pnt pfin2 = S2->Value(Parfin(3),Parfin(4));

  Standard_Real distrefdeb = pdeb1.Distance(pdeb2);//checks the worthiness 
  Standard_Real distreffin = pfin1.Distance(pfin2);//of input data
  if(distrefdeb < tol3d) distrefdeb = tol3d;
  if(distreffin < tol3d) distreffin = tol3d;

  gp_Pnt pdeb,pfin;
  pdeb.SetXYZ(0.5*(pdeb1.XYZ()+pdeb2.XYZ()));
  pfin.SetXYZ(0.5*(pfin1.XYZ()+pfin2.XYZ()));

  Standard_Real distref = 0.005*pdeb.Distance(pfin);
  if(distref < distrefdeb) distref = distrefdeb;
  if(distref < distreffin) distref = distreffin;

  //Some analytic cases are processed separately.
  //To reorientate the result of the analythic intersection,
  //it is stated that the beginning of the tangent should be
  //in the direction of the start/end line.
  gp_Vec Vint, Vref(pdeb,pfin);
  gp_Pnt Pbid;
  Standard_Real Udeb = 0.,Ufin = 0.;
  Standard_Real tolr1,tolr2;
  tolr1 = tolr2 = tolreached = tol3d;
  if((S1->GetType() == GeomAbs_Cylinder && S2->GetType() == GeomAbs_Plane)||
    (S1->GetType() == GeomAbs_Plane && S2->GetType() == GeomAbs_Cylinder)) { 
      gp_Pln pl;
      gp_Cylinder cyl;
      if(S1->GetType() == GeomAbs_Plane) {
        pl = S1->Plane();
        cyl = S2->Cylinder();
      }
      else{
        pl = S2->Plane();
        cyl = S1->Cylinder();
      }
      IntAna_QuadQuadGeo ImpKK(pl,cyl,Precision::Angular(),tol3d);
      Standard_Boolean isIntDone = ImpKK.IsDone();

      if(ImpKK.TypeInter() == IntAna_Ellipse)
      {
        const gp_Elips anEl = ImpKK.Ellipse(1);
        const Standard_Real aMajorR = anEl.MajorRadius();
        const Standard_Real aMinorR = anEl.MinorRadius();
        isIntDone = (aMajorR < 100000.0 * aMinorR);
      }

      if (isIntDone) {
        Standard_Boolean c1line = 0;
        switch  (ImpKK.TypeInter()) {
        case IntAna_Line:
          {
            c1line = 1;
            Standard_Integer nbsol = ImpKK.NbSolutions();
            gp_Lin C1;
            for(Standard_Integer ilin = 1; ilin <= nbsol; ilin++) {
              C1 = ImpKK.Line(ilin);
              Udeb = ElCLib::Parameter(C1,pdeb);
              gp_Pnt ptest = ElCLib::Value(Udeb,C1);
              if(ptest.Distance(pdeb) < tol3d) break;
            }
            Ufin = ElCLib::Parameter(C1,pfin);
            C3d = new Geom_Line(C1);
            ElCLib::D1(Udeb,C1,Pbid,Vint);
          }
          break;
        case IntAna_Circle:
          {
            gp_Circ C1 = ImpKK.Circle(1);
            C3d = new Geom_Circle(C1);
            Udeb = ElCLib::Parameter(C1,pdeb);
            Ufin = ElCLib::Parameter(C1,pfin);
            ElCLib::D1(Udeb,C1,Pbid,Vint);
          }
          break;
        case IntAna_Ellipse:
          {
            gp_Elips C1 = ImpKK.Ellipse(1);
            C3d = new Geom_Ellipse(C1);
            Udeb = ElCLib::Parameter(C1,pdeb);
            Ufin = ElCLib::Parameter(C1,pfin);
            ElCLib::D1(Udeb,C1,Pbid,Vint);
          }
          break;
        default:
          break;
        }
        if (Vint.Dot(Vref)<0) {
          C3d->Reverse();
          if(c1line) {
            Udeb = -Udeb;
            Ufin = -Ufin;
          }
          else{
            Udeb = 2*M_PI - Udeb;
            Ufin = 2*M_PI - Ufin;
          }
        }
        if(!c1line) ElCLib::AdjustPeriodic(0.,2*M_PI,Precision::Angular(),Udeb,Ufin);
        Handle(GeomAdaptor_HCurve) HC = new GeomAdaptor_HCurve();
        HC->ChangeCurve().Load(C3d,Udeb,Ufin);
        ChFi3d_ProjectPCurv(HC,S1,Pc1,tol3d,tolr1);
        if(S1->GetType() == GeomAbs_Cylinder) {
          Standard_Real x,y;
          Pc1->Value(Udeb).Coord(x,y);
          x = Pardeb(1) - x;
          y = Pardeb(2) - y;
          if(Abs(x) >= tol2d || Abs(y) >= tol2d) Pc1->Translate(gp_Vec2d(x,y));
        }
        ChFi3d_ProjectPCurv(HC,S2,Pc2,tol3d,tolr2);
        if(S2->GetType() == GeomAbs_Cylinder) {
          Standard_Real x,y;
          Pc2->Value(Udeb).Coord(x,y);
          x = Pardeb(3) - x;
          y = Pardeb(4) - y;
          if(Abs(x) >= tol2d || Abs(y) >= tol2d) Pc2->Translate(gp_Vec2d(x,y));
        }
        C3d = new Geom_TrimmedCurve(C3d,Udeb,Ufin);
        tolreached = 1.5*Max(tolr1,tolr2);
        tolreached = Min(tolreached,ChFi3d_EvalTolReached(S1,Pc1,S2,Pc2,C3d));
        return Standard_True;
      }
  }    
  else if(S1->GetType() == GeomAbs_Plane && S2->GetType() == GeomAbs_Plane) { 
    IntAna_QuadQuadGeo LInt(S1->Plane(),S2->Plane(),Precision::Angular(),tol3d);
    if (LInt.IsDone()) {
      gp_Lin L = LInt.Line(1);
      C3d = new Geom_Line(L);
      Udeb = ElCLib::Parameter(L,pdeb);
      Ufin = ElCLib::Parameter(L,pfin);
      ElCLib::D1(Udeb,L,Pbid,Vint);
      if (Vint.Dot(Vref)<0) {
        C3d->Reverse();
        Udeb = - Udeb;
        Ufin = - Ufin;
      }
      Handle(GeomAdaptor_HCurve) HC = new GeomAdaptor_HCurve();
      HC->ChangeCurve().Load(C3d,Udeb,Ufin);
      ChFi3d_ProjectPCurv(HC,S1,Pc1,tol3d,tolr1);
      ChFi3d_ProjectPCurv(HC,S2,Pc2,tol3d,tolr2);
      C3d = new Geom_TrimmedCurve(C3d,Udeb,Ufin);
      return Standard_True;
    }
  }
  else {
    // here GeomInt is approached.
    Handle(Adaptor3d_TopolTool) dom1,dom2;
    Handle(Geom_Surface) gs1 = trsfsurf(S1,dom1);
    Handle(Geom_Surface) gs2 = trsfsurf(S2,dom2);
    Standard_Integer nbl ;
    if(!gs1.IsNull() && !gs2.IsNull()) {
      GeomInt_IntSS inter;
      //  Modified by skv - Fri Oct 24 14:24:47 2003 OCC4077 Begin
      //       Standard_Real tolap = 1.e-7;//car l approx de la wline est faite dans [0,1]
      // Set the lowest tolerance which is used in new boolean operations.
      Standard_Real tolap = 2.e-7;
      //  Modified by skv - Fri Oct 24 14:24:48 2003 OCC4077 End
      inter.Perform(gs1,gs2,tolap,1,1,1);
      if(inter.IsDone()) {
        nbl = inter.NbLines();   
#if defined(IRIX) || defined(__sgi)
        if(nbl==0) {

          //  solution of adjustment for SGI 
          //  if the intersection of gs1 with gs2 doesnot worke 
          //  then the intersection of gs2 with gs1 is attempted.

          inter.Perform(gs2,gs1,tolap,1,1,1);
          //          inter.Perform(gs2,dom2,gs1,dom1,tolap,1,1,1);
          if(!inter.IsDone()) return Standard_False; 
          nbl = inter.NbLines(); 

          //  if GeomInt does not make the intersection the solution of adjustment 
          //  is not attempted 
          if (nbl==0) return Standard_False;
        }
#endif
        GeomAPI_ProjectPointOnCurve proj;
        for(Standard_Integer ilin = 1; ilin <= nbl; ilin++) {
          if(inter.HasLineOnS1(ilin) && inter.HasLineOnS2(ilin)) {
            C3d = inter.Line(ilin);
            Pc1 = inter.LineOnS1(ilin);
            Pc2 = inter.LineOnS2(ilin);
            gp_Pnt ptestdeb, ptestfin;
            Standard_Real Uf=0., Ul=0.;
            if (wholeCurv) {
              Uf = C3d->FirstParameter();
              Ul = C3d->LastParameter();
              ptestdeb = C3d->Value(Uf);
              ptestfin = C3d->Value(Ul);
            }
            else {
              // find end parameters
              Standard_Boolean failedF, failedL;
              failedF = failedL = Standard_False;
              proj.Init( pdeb1, C3d);
              if (proj.NbPoints()==0 && distrefdeb > Precision::Confusion())
                proj.Perform( pdeb2 );
              if (proj.NbPoints()==0)
                failedF = Standard_True;
              else
                Uf = proj.LowerDistanceParameter();
              proj.Perform( pfin1 );
              if (proj.NbPoints()==0 && distreffin > Precision::Confusion())
                proj.Perform( pfin2 );
              if (proj.NbPoints()==0) 
                failedL = Standard_True;
              else
                Ul = proj.LowerDistanceParameter();

              if (failedF && failedL) {
                Uf = C3d->FirstParameter();
                Ul = C3d->LastParameter();
              }
              else if (failedF || failedL) {
                // select right end parameter
                Standard_Real Uok = failedF ? Ul : Uf;
                Standard_Real U1 = C3d->FirstParameter(), U2 = C3d->LastParameter();
                Uok = Abs(Uok-U1) > Abs(Uok-U2) ? U1 : U2;
                if (failedF) Uf = Uok;
                else         Ul = Uok;
              }
              else { // both projected, but where?
                if (Abs(Uf - Ul) < Precision::PConfusion())
                  continue;
              }
              ptestdeb = C3d->Value(Uf);
              ptestfin = C3d->Value(Ul);
              if (C3d->IsPeriodic() && !(failedF && failedL)) {
                // assure the same order of ends, otherwise TrimmedCurve will take
                // the other part of C3d
                gp_Pnt Ptmp;
                gp_Vec DirOld, DirNew(ptestdeb,ptestfin);
                C3d->D1(Uf, Ptmp, DirOld);
                if (DirOld * DirNew < 0) {
                  Standard_Real Utmp = Uf; Uf = Ul; Ul = Utmp;
                  Ptmp = ptestdeb; ptestdeb = ptestfin; ptestfin = Ptmp;
                }
              }
            }
            C3d = new Geom_TrimmedCurve(C3d,Uf,Ul);
            Pc1 = new Geom2d_TrimmedCurve(Pc1,Uf,Ul);
            Pc2 = new Geom2d_TrimmedCurve(Pc2,Uf,Ul);
            //is it necesary to invert ?
            Standard_Real distdeb = ptestdeb.Distance(pdeb);
            Standard_Real distfin = ptestfin.Distance(pfin);
            if(distdeb > distref || distfin > distref) {
              C3d->Reverse();
              Pc1->Reverse();
              Pc2->Reverse();
              ptestdeb = C3d->Value(C3d->FirstParameter());
              ptestfin = C3d->Value(C3d->LastParameter());
              distdeb = ptestdeb.Distance(pdeb);
              distfin = ptestfin.Distance(pfin);
            }
            if(distdeb < distref && distfin < distref) {
              Uf = C3d->FirstParameter();
              Ul = C3d->LastParameter();
              ChFi3d_ReparamPcurv(Uf,Ul,Pc1);
              ChFi3d_ReparamPcurv(Uf,Ul,Pc2);
              Standard_Real x,y;
              Pc1->Value(Uf).Coord(x,y);
              x = Pardeb(1) - x;
              y = Pardeb(2) - y;
              if(Abs(x) > tol2d || Abs(y) > tol2d) Pc1->Translate(gp_Vec2d(x,y));
              Pc2->Value(Uf).Coord(x,y);
              x = Pardeb(3) - x;
              y = Pardeb(4) - y;
              if(Abs(x) > tol2d || Abs(y) > tol2d) Pc2->Translate(gp_Vec2d(x,y));
              tolreached = ChFi3d_EvalTolReached(S1,Pc1,S2,Pc2,C3d);
              return Standard_True;
            }
          }
        }
      }
    }
  }

  // At this stage : 
  // classic intersections have failed, the path is approached in vain.

  Standard_Real Step = 0.1;
  for(;;) {
    //Attention the parameters of arrow for the path and
    //the tolerance for the approximation can't be taken as those of the  
    //Builder, so they are reestimated as much as possible.
    Standard_Real fleche = 1.e-3 * pdeb.Distance(pfin);
    Standard_Real tolap = 1.e-7;
    IntWalk_PWalking
      IntKK(S1,S2,tol3d,tol3d,fleche,Step);

    //The extremities of the intersection (Pardeb,Parfin) are known,
    //one tries to find the start point at the 
    //middle to avoid obstacles on the path.
    Standard_Boolean depok = Standard_False;
    IntSurf_PntOn2S pintdep;
    TColStd_Array1OfReal depart(1,4);
    for(Standard_Integer ipdep = 2; ipdep <= 7 && !depok; ipdep++) {
      Standard_Real alpha = 0.1 * ipdep;
      Standard_Real unmoinsalpha = 1. - alpha;
      depart(1) = alpha*Pardeb(1) + unmoinsalpha*Parfin(1);
      depart(2) = alpha*Pardeb(2) + unmoinsalpha*Parfin(2);
      depart(3) = alpha*Pardeb(3) + unmoinsalpha*Parfin(3);
      depart(4) = alpha*Pardeb(4) + unmoinsalpha*Parfin(4);
      depok = IntKK.PerformFirstPoint(depart,pintdep);
    } 
    if(!depok) {
      return Standard_False;
    }
    pintdep.Parameters(depart(1),depart(2),depart(3),depart(4));
    IntKK.Perform(depart);
    if (!IntKK.IsDone()) return Standard_False;
    if (IntKK.NbPoints() <= 30) {
      Step *= 0.5;
      if (Step <= 0.0001) {
        return Standard_False;
      }
    }
    else{
      // At this stage there is a presentable LineOn2S, it is truncated  
      // between the points closest to known  extremites 
      // in fact there is a WLine and the approximation is launched.
      // Then the result is corrected to get proper start and end points.
      const Handle(IntSurf_LineOn2S)& L2S = IntKK.Line();

      gp_Pnt codeb1 = S1->Value(Pardeb(1),Pardeb(2));
      gp_Pnt codeb2 = S2->Value(Pardeb(3),Pardeb(4));
      Standard_Real tol1 = Max(codeb1.Distance(codeb2),tol3d);
      Standard_Boolean bondeb = (tol1 == tol3d); 
      gp_Pnt pntd(0.5*(codeb1.Coord() + codeb2.Coord()));

      gp_Pnt cofin1 = S1->Value(Parfin(1),Parfin(2));
      gp_Pnt cofin2 = S2->Value(Parfin(3),Parfin(4));
      Standard_Real tol2 = Max(cofin1.Distance(cofin2),tol3d);
      Standard_Boolean bonfin = (tol2 == tol3d); 
      gp_Pnt pntf(0.5*(cofin1.Coord() + cofin2.Coord()));

      Standard_Integer nbp = L2S->NbPoints(), i;
      Standard_Real ddeb = Precision::Infinite(); 
      Standard_Real dfin = Precision::Infinite();
      Standard_Real dd;
      Standard_Integer indd = 0, indf = 0;
      for(i = 1; i <= nbp; i++) {
        dd = L2S->Value(i).Value().Distance(pntd);
        if(dd <= ddeb) { ddeb = dd; indd = i;}
        dd = L2S->Value(i).Value().Distance(pntf);
        if(dd < dfin) { dfin = dd; indf = i;}
      }
      if(indd > indf) {
        L2S->Reverse();
        indd = nbp - indd + 1;
        indf = nbp - indf + 1;
      }
      for (i = 1; i < indd; i++) { L2S->RemovePoint(1); nbp--; indf--; }
      for (i = indf + 1; i <= nbp; i++) { L2S->RemovePoint(indf + 1); }
      nbp = indf;
      if(nbp==1) return Standard_False;
      //The extremities are inserted in the line if the extremity points on it 
      //are too far and if pardeb and parfin are good.
      if(ddeb >= tol3d && bondeb) {
        IntSurf_PntOn2S p1 = L2S->Value(1);
        IntSurf_PntOn2S p2 = L2S->Value(2);

        gp_Vec v1(pntd,p1.Value());
        gp_Vec v2(p1.Value(),p2.Value());
        gp_Vec v3(pntd,p2.Value());
        p1.SetValue(pntd,Pardeb(1),Pardeb(2),Pardeb(3),Pardeb(4));
        if(v1.Dot(v3) < 0) {
          if(v3.Magnitude() < 0.2*v2.Magnitude()) {
            L2S->RemovePoint(1);
            nbp--;
          }
          L2S->Value(1,p1);
        }
        else if(v1.Magnitude() > 0.2*v2.Magnitude()) {
          L2S->InsertBefore(1,p1);
          nbp++;
        }
        else{
          L2S->Value(1,p1);
        }
        ddeb = 0.;
      }
      if(dfin >= tol3d && bonfin) {
        IntSurf_PntOn2S p1 = L2S->Value(nbp);
        IntSurf_PntOn2S p2 = L2S->Value(nbp - 1);
        gp_Vec v1(pntf,p1.Value());
        gp_Vec v2(p1.Value(),p2.Value());
        gp_Vec v3(pntf,p2.Value());
        p1.SetValue(pntf,Parfin(1),Parfin(2),Parfin(3),Parfin(4));
        if(v1.Dot(v3) < 0) {
          if(v3.Magnitude() < 0.2*v2.Magnitude()) {
            L2S->RemovePoint(nbp);
            nbp--;
          }
          L2S->Value(nbp,p1);
        }
        else if(v1.Magnitude() > 0.2*v2.Magnitude()) {
          L2S->Add(p1);
          nbp++;
        }
        else{
          L2S->Value(nbp,p1);
        }
        dfin = 0.;
      }      
      //
      Handle(IntPatch_WLine) 	WL = new IntPatch_WLine(L2S,Standard_False);

#ifdef OCCT_DEBUG
      //WL->Dump(0);
#endif

      GeomInt_WLApprox approx;
      approx.SetParameters(tolap, tol2d, 4, 8, 0, 30, Standard_True);
      // manage here the approximations that are not useful on planes!
      approx.Perform(S1,S2,WL,
        Standard_True,Standard_True,Standard_True,
        1,nbp);
      if(!approx.IsDone()) return Standard_False;
      //      tolreached = approx.TolReached3d();
      //      Standard_Real tolr2d = approx.TolReached2d();
      //      tolreached = Max(tolreached,ChFi3d_ConvTol2dToTol3d(S1,tolr2d));
      //      tolreached = Max(tolreached,ChFi3d_ConvTol2dToTol3d(S2,tolr2d));
      const AppParCurves_MultiBSpCurve& mbs = approx.Value(1);
      Standard_Integer nbpol = mbs.NbPoles();
      TColgp_Array1OfPnt pol3d(1,nbpol);
      mbs.Curve(1,pol3d);
      TColgp_Array1OfPnt2d pol2d1(1,nbpol);
      mbs.Curve(2,pol2d1);
      TColgp_Array1OfPnt2d pol2d2(1,nbpol);
      mbs.Curve(3,pol2d2);
      // The extremities of the intersection are reset on known points.
      if(ddeb >= tol1) {
        pol3d(1) = pntd;
        pol2d1(1).SetCoord(Pardeb(1),Pardeb(2));
        pol2d2(1).SetCoord(Pardeb(3),Pardeb(4));
        //	tolreached = Max(tolreached,ddeb);
      }

      if(dfin >= tol2) {
        pol3d(nbpol) = pntf;
        pol2d1(nbpol).SetCoord(Parfin(1),Parfin(2));
        pol2d2(nbpol).SetCoord(Parfin(3),Parfin(4));
        //	tolreached = Max(tolreached,dfin);
      }
      const TColStd_Array1OfReal& knots = mbs.Knots();
      const TColStd_Array1OfInteger& mults = mbs.Multiplicities();
      Standard_Integer deg = mbs.Degree();
      C3d = new Geom_BSplineCurve(pol3d,knots,mults,deg);
      Pc1 = new Geom2d_BSplineCurve(pol2d1,knots,mults,deg);
      Pc2 = new Geom2d_BSplineCurve(pol2d2,knots,mults,deg);
      tolreached = ChFi3d_EvalTolReached(S1,Pc1,S2,Pc2,C3d);
      tolreached = Max(tolreached,ddeb);
      tolreached = Max(tolreached,dfin);
      return Standard_True;
    }
  }
}

//=======================================================================
//function : IntCS
//purpose  : Fast calculation of the intersection curve surface.
//
//=======================================================================

Standard_Boolean ChFi3d_IntCS(const Handle(Adaptor3d_HSurface)& S,
                              const Handle(Adaptor3d_HCurve)& C,
                              gp_Pnt2d& p2dS,
                              Standard_Real& wc)
{
  IntCurveSurface_HInter Intersection;

  Standard_Real uf = C->FirstParameter(), ul = C->LastParameter();
  Standard_Real u1 = S->FirstUParameter(), u2 = S->LastUParameter();
  Standard_Real v1 = S->FirstVParameter(), v2 = S->LastVParameter();
  IntCurveSurface_IntersectionPoint pint;
  Intersection.Perform(C,S);
  Standard_Boolean keepfirst = (wc < -1.e100), keeplast = (wc > 1.e100);
  Standard_Real temp = 0.;
  if(keepfirst) temp = 1.e100;
  if(keeplast) temp = -1.e100;
  Standard_Real dist = 2.e100;
  if(Intersection.IsDone()) {
    Standard_Integer nbp = Intersection.NbPoints(),i,isol = 0;
    for (i = 1; i <= nbp; i++) {
      pint = Intersection.Point(i);
      Standard_Real up = pint.U();
      Standard_Real vp = pint.V();
      if(S->IsUPeriodic()) up = ChFi3d_InPeriod(up,u1,u1+S->UPeriod(),1.e-8);
      if(S->IsVPeriodic()) vp = ChFi3d_InPeriod(vp,v1,v1+S->VPeriod(),1.e-8);
      if(uf <= pint.W() && ul >= pint.W() &&
        u1 <= up && u2 >= up &&
        v1 <= vp && v2 >= vp) {
          if(keepfirst && pint.W() < temp) {
            temp = pint.W();
            isol = i;
          }
          else if(keeplast && pint.W() > temp) {
            temp = pint.W();
            isol = i;
          }
          else if(Abs(pint.W() - wc) < dist) {
            dist = Abs(pint.W() - wc);
            isol = i;
          }
      }
    }
    if(isol == 0) return Standard_False;
    pint = Intersection.Point(isol);
    Standard_Real up = pint.U();
    Standard_Real vp = pint.V();
    if(S->IsUPeriodic()) up = ChFi3d_InPeriod(up,u1,u1+S->UPeriod(),1.e-8);
    if(S->IsVPeriodic()) vp = ChFi3d_InPeriod(vp,v1,v1+S->VPeriod(),1.e-8);
    p2dS.SetCoord(up,vp);
    wc = pint.W();
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : ComputesIntPC
//purpose  : Intersection of two PCurves of type FaceInterference
//           the parameters of the pcurves at the solution point are 
//           UInt1,UInt2
//=======================================================================

void ChFi3d_ComputesIntPC (const ChFiDS_FaceInterference&      Fi1,
  const ChFiDS_FaceInterference&      Fi2,
  const Handle(GeomAdaptor_HSurface)& HS1,
  const Handle(GeomAdaptor_HSurface)& HS2,
  Standard_Real&                      UInt1, 
  Standard_Real&                      UInt2)
{
  gp_Pnt bid;
  ChFi3d_ComputesIntPC(Fi1,Fi2,HS1,HS2,UInt1,UInt2,bid);
}

//=======================================================================
//function : ChFi3d_ComputesIntPC
//purpose  : 
//=======================================================================
void ChFi3d_ComputesIntPC (const ChFiDS_FaceInterference&      Fi1,
  const ChFiDS_FaceInterference&      Fi2,
  const Handle(GeomAdaptor_HSurface)& HS1,
  const Handle(GeomAdaptor_HSurface)& HS2,
  Standard_Real&                      UInt1, 
  Standard_Real&                      UInt2,
  gp_Pnt&                             P)
{    
  // Only one intersection to be carried out, however, the effort
  // is taken to check the extremities by an extrema c3d/c3d
  // created on pcurveonsurf of fillets.

  Standard_Real x,y,distref2;
  Fi1.PCurveOnSurf()->Value(UInt1).Coord(x,y);
  gp_Pnt p3d1 = HS1->Value(x,y);
  Fi2.PCurveOnSurf()->Value(UInt2).Coord(x,y);
  gp_Pnt p3d2 = HS2->Value(x,y);
  distref2 = p3d1.SquareDistance(p3d2);
  P.SetXYZ(0.5*(p3d1.XYZ() + p3d2.XYZ()));
  // recalculation of the extremums
  Standard_Real delt1 = 
    Min(0.1,0.05*(Fi1.LastParameter() - Fi1.FirstParameter()));
  Handle(Geom2dAdaptor_HCurve) hc2d1 = 
    new Geom2dAdaptor_HCurve(Fi1.PCurveOnSurf(),UInt1-delt1,UInt1+delt1);
  Adaptor3d_CurveOnSurface cons1(hc2d1,HS1);
  Standard_Real delt2 = 
    Min(0.1,0.05*(Fi2.LastParameter() - Fi2.FirstParameter()));
  Handle(Geom2dAdaptor_HCurve) hc2d2 = 
    new Geom2dAdaptor_HCurve(Fi2.PCurveOnSurf(),UInt2-delt2,UInt2+delt2);
  Adaptor3d_CurveOnSurface cons2(hc2d2,HS2);
  Extrema_LocateExtCC ext(cons1,cons2,UInt1,UInt2);
  if(ext.IsDone()) {
    Standard_Real dist2 = ext.SquareDistance();
    if(dist2<distref2) {
      Extrema_POnCurv ponc1,ponc2;
      ext.Point(ponc1,ponc2);
      UInt1 = ponc1.Parameter();
      UInt2 = ponc2.Parameter();
      gp_Pnt Pnt1 = ponc1.Value();
      gp_Pnt Pnt2 = ponc2.Value();
      P.SetXYZ(0.5*(Pnt1.XYZ() + Pnt2.XYZ()));
    }
  }
} 

//=======================================================================
//function : BoundSurf
//purpose  : computes a GeomAdaptor_Surface from the surface of the 
//           SurfData Fd1 and trims it to allow the intersection computation

//=======================================================================
Handle(GeomAdaptor_HSurface) ChFi3d_BoundSurf(TopOpeBRepDS_DataStructure&    DStr,
  const Handle(ChFiDS_SurfData)& Fd1,
  const Standard_Integer&        IFaCo1,
  const Standard_Integer&        IFaArc1)
{
  //rmq : as in fact 2 interferences of Fd1 serve only to set limits 
  //      indexes IFaCo1 and IFaArc1 are not useful.
  //      They are preserver here as an option in case it will be necessary to set 
  //      more restrictive limits (with intersection points as additional argument).

  Handle(GeomAdaptor_HSurface) HS1 = new GeomAdaptor_HSurface();
  GeomAdaptor_Surface& S1 = HS1->ChangeSurface();
  S1.Load(DStr.Surface(Fd1->Surf()).Surface());

  if ((IFaCo1 == 0)||(IFaArc1 == 0)) 
    return HS1;

  const ChFiDS_FaceInterference& FiCo1 = Fd1->Interference(IFaCo1);
  const ChFiDS_FaceInterference& FiArc1 = Fd1->Interference(IFaArc1);

  Standard_Real Du,Dv,mu,Mu,mv,Mv;
  gp_Pnt2d UVf1,UVf2,UVl1,UVl2;

  UVf1 = FiCo1.PCurveOnSurf()->Value(FiCo1.FirstParameter());
  UVl1 = FiCo1.PCurveOnSurf()->Value(FiCo1.LastParameter());
  UVf2 = FiArc1.PCurveOnSurf()->Value(FiArc1.FirstParameter());
  UVl2 = FiArc1.PCurveOnSurf()->Value(FiArc1.LastParameter());
  ChFi3d_Boite(UVf1,UVf2,UVl1,UVl2,Du,Dv,mu,Mu,mv,Mv);
  GeomAbs_SurfaceType styp = S1.GetType();
  if (styp == GeomAbs_Cylinder) { 
    Dv = Max(0.5*Dv,4.*S1.Cylinder().Radius());
    Du = 0.;
    S1.Load(DStr.Surface(Fd1->Surf()).Surface(),
      mu,Mu,mv-Dv,Mv+Dv);
  }
  //In the case of a torus or cone, it is not necessary that the bounds create a surface with period more than 2PI. 
  else if (styp == GeomAbs_Torus ||
    styp == GeomAbs_Cone) {
      Du = Min(M_PI-0.5*Du,0.1*Du);
      Dv = 0.;
      S1.Load(DStr.Surface(Fd1->Surf()).Surface(),
        mu-Du,Mu+Du,mv,Mv);
  }
  else if (styp == GeomAbs_Plane) {
    Du = Max(0.5*Du,4.*Dv);
    Dv = 0.;
    S1.Load(DStr.Surface(Fd1->Surf()).Surface(),
      mu-Du,Mu+Du,mv,Mv);
  }
  return HS1;
}
//=======================================================================
//function : SearchPivot
//purpose  : 
//=======================================================================
Standard_Integer ChFi3d_SearchPivot(Standard_Integer* s,
  Standard_Real u[3][3],
  const Standard_Real t)
{
  //           This function finds as pivot a cd the sections which of
  //           do not cross on the opposite face. 
  //         - probably there will be cases asymmetric to the point that
  //           none of tree fillets will match! To be SEEN.
  //         - in case when several fillets match the 
  //           first one taken is not inevitably the best 
  //           it should be refined by comparing the parameters on 
  //           guide lines and (/or) radiuses.

  Standard_Boolean bondeb,bonfin;
  for(Standard_Integer i = 0; i <= 2; i++) {
    if(s[(i+1)%3] == 1) {bondeb = (u[(i+1)%3][i]-u[(i+1)%3][(i+2)%3] >= -t);}
    else {bondeb = (u[(i+1)%3][i]-u[(i+1)%3][(i+2)%3] <= t);}
    if(s[(i+2)%3] == 1) {bonfin = (u[(i+2)%3][i]-u[(i+2)%3][(i+1)%3] >= -t);}
    else {bonfin = (u[(i+2)%3][i]-u[(i+2)%3][(i+1)%3] <= t);}
    if (bondeb && bonfin) { return i; }
  }
  return -1;
}



//=======================================================================
//function : SearchFD
//purpose  : 
//=======================================================================
Standard_Boolean ChFi3d_SearchFD(TopOpeBRepDS_DataStructure& DStr,
  const Handle(ChFiDS_Stripe)& cd1, 
  const Handle(ChFiDS_Stripe)& cd2,
  const Standard_Integer sens1,
  const Standard_Integer sens2,
  Standard_Integer& i1,
  Standard_Integer& i2,
  Standard_Real& p1,
  Standard_Real& p2,
  const Standard_Integer ind1,
  const Standard_Integer ind2,
  TopoDS_Face& face,
  Standard_Boolean& sameside,
  Standard_Integer& jf1,
  Standard_Integer& jf2)
{
  Standard_Boolean found = Standard_False;
  Standard_Integer id1 = ind1, id2 = ind2;
  Standard_Integer if1 = ind1, if2 = ind2;
  Standard_Integer l1 = cd1->SetOfSurfData()->Length();
  Standard_Integer l2 = cd2->SetOfSurfData()->Length();
  Standard_Integer i;
  Standard_Boolean fini1 = Standard_False, fini2 = Standard_False;
  Standard_Boolean visavis,visavisok = Standard_False;
  TopoDS_Vertex Vtx;
  while( !found ) {
    for(i = id1; (i*sens1) <= (if1*sens1) && !found && !fini2; i = i+sens1 ) { 
      if(ChFi3d_IsInFront(DStr,cd1,cd2,i,if2,sens1,sens2,p1,p2,face,sameside,jf1,jf2,visavis,Vtx,Standard_False,0)) {
        i1 = i;
        i2 = if2;
        found = Standard_True;
      }
      else if (visavis && !visavisok) {
        visavisok = Standard_True;
        i1 = i;
        i2 = if2;
      }
    }
    if(!fini1) {
      if1 = if1 + sens1;
      if(if1 < 1 || if1 > l1) { if1 = if1 - sens1; fini1 = Standard_True; }
    }

    for(i = id2; (i*sens2) <= (if2*sens2) && !found && !fini1; i = i+sens2 ) { 
      if(ChFi3d_IsInFront(DStr,cd1,cd2,if1,i,sens1,sens2,p1,p2,face,sameside,jf1,jf2,visavis,Vtx,Standard_False,0)) {
        i1 = if1;
        i2 = i;
        found = Standard_True;
      }
      else if (visavis && !visavisok) {
        visavisok = Standard_True;
        i1 = if1;
        i2 = i;
      }
    }
    if(!fini2) {
      if2 = if2 + sens2;
      if(if2 < 1 || if2 > l2) { if2 = if2 - sens2; fini2 = Standard_True; }
    }
    if(fini1 && fini2) break;
  }
  return found;
}

//=======================================================================
//function : Parameters
//purpose  : compute the parameters <u> and <v> of the 3d point <p3d> 
//           on the surface <S> if it's an analytic surface
//=======================================================================

void ChFi3d_Parameters(const Handle(Geom_Surface)& S,
  const gp_Pnt& p3d,
  Standard_Real& u,
  Standard_Real& v)
{
  GeomAdaptor_Surface gas(S);
  switch ( gas.GetType() ) {
  case GeomAbs_Plane :
    ElSLib::Parameters(gas.Plane(),p3d,u,v);
    break;
  case GeomAbs_Cylinder :
    ElSLib::Parameters(gas.Cylinder(),p3d,u,v);
    break;
  case GeomAbs_Cone :
    ElSLib::Parameters(gas.Cone(),p3d,u,v);
    break;
  case GeomAbs_Sphere :
    ElSLib::Parameters(gas.Sphere(),p3d,u,v);
    break;
  case GeomAbs_Torus :
    ElSLib::Parameters(gas.Torus(),p3d,u,v);
    break;
  case GeomAbs_BezierSurface :
  case GeomAbs_BSplineSurface :  
  default :
    {
      GeomAPI_ProjectPointOnSurf tool(p3d,S);
      if ( tool.NbPoints() != 1 )
        throw StdFail_NotDone ("ChFi3d_Parameters() - no projection results");
      else
        tool.Parameters(1,u,v);
    }
  }
}

//=======================================================================
//function : TrimCurve
//purpose  : trims the curve <gc> between the points <FirstP> and
//           <LastP>. The trimmed curve is <gtc>
//=======================================================================

void ChFi3d_TrimCurve(const Handle(Geom_Curve)& gc,
  const gp_Pnt& FirstP,
  const gp_Pnt& LastP,
  Handle(Geom_TrimmedCurve)& gtc)
{
  Standard_Real uf = 0.,ul = 0.;
  GeomAdaptor_Curve gac(gc);
  switch ( gac.GetType() ) {
  case GeomAbs_Line :
    {
      uf = ElCLib::Parameter(gac.Line(),FirstP);
      ul = ElCLib::Parameter(gac.Line(),LastP);
    }
    break;
  case GeomAbs_Circle :
    {
      uf = ElCLib::Parameter(gac.Circle(),FirstP);
      ul = ElCLib::Parameter(gac.Circle(),LastP);
    }
    break;
  case GeomAbs_Ellipse :
    {
      uf = ElCLib::Parameter(gac.Ellipse(),FirstP);
      ul = ElCLib::Parameter(gac.Ellipse(),LastP);
    }
    break;
  case GeomAbs_Hyperbola :
    {
      uf = ElCLib::Parameter(gac.Hyperbola(),FirstP);
      ul = ElCLib::Parameter(gac.Hyperbola(),LastP);
    }
    break;
  case GeomAbs_Parabola :
    {
      uf = ElCLib::Parameter(gac.Parabola(),FirstP);
      ul = ElCLib::Parameter(gac.Parabola(),LastP);
    }
    break;
  default :
    {
      GeomAPI_ProjectPointOnCurve tool(FirstP,gc);
      if ( tool.NbPoints() != 1 )
        throw StdFail_NotDone ("ChFi3d_TrimCurve() - no projection results for the first point");
      else
        uf = tool.Parameter(1);
      tool.Init(LastP,gc);
      if ( tool.NbPoints() != 1 )
        throw StdFail_NotDone ("ChFi3d_TrimCurve() - no projection results for the second point");
      else
        ul = tool.Parameter(1);
    }
  }
  gtc = new Geom_TrimmedCurve(gc,uf,ul);
}



//=======================================================================
//function : GoodExt
//purpose  : 
//=======================================================================
static Standard_Boolean GoodExt(const Handle(Geom_Curve)& C,
  const gp_Vec&             V,
  const Standard_Real       f,
  const Standard_Real       l,
  const Standard_Real       a)
{
  for(Standard_Integer i = 0; i < 6; i++) {
    gp_Pnt d0; gp_Vec d1;
    const Standard_Real t = i * 0.2;
    C->D1(((1-t)*f+t*l),d0,d1);
    const Standard_Real ang = d1.Angle(V);
    const Standard_Real angref = a*t + 0.002;
    if(ang > angref) return Standard_False;
  }
  return Standard_True;
}
//=======================================================================
//function : PerformElSpine
//purpose  : 
//=======================================================================
Standard_EXPORT 
  void ChFi3d_PerformElSpine(Handle(ChFiDS_HElSpine)& HES,
  Handle(ChFiDS_Spine)&    Spine,
  const GeomAbs_Shape      continuity,
  const Standard_Real      tol) 
{

  Standard_Boolean periodic, Bof, checkdeb, cepadur,bIsSmooth;
  Standard_Integer IEdge,IF,IL,nbed, iToApproxByC2;
  Standard_Real WF, WL, Wrefdeb, Wreffin,nwf,nwl,period,pared = 0.,tolpared;
  Standard_Real First, Last, epsV, urefdeb, tolrac;
  GeomAbs_Shape aContinuity;
  gp_Pnt PDeb, PFin, Bout;
  gp_Vec VrefDeb, VrefFin;
  Handle(Geom_Curve) Cv;
  Handle(Geom_BoundedCurve) TC;
  Handle(Geom_BSplineCurve) BS, BSpline;
  TopoDS_Edge E, Eold;
  TopoDS_Vertex V;
  //
  ChFiDS_ElSpine& ES = HES->ChangeCurve();
  WF = ES.FirstParameter();
  WL = ES.LastParameter();
  Wrefdeb = WF;
  Wreffin = WL;
  nwf = WF;
  nwl = WL;
  nbed = Spine->NbEdges();
  periodic = Spine->IsPeriodic();
  if(periodic) {
    period = Spine->Period();
    nwf = ElCLib::InPeriod(WF,-tol,period-tol);
    IF = Spine->Index(nwf,1);
    nwl = ElCLib::InPeriod(WL,tol,period+tol);
    IL = Spine->Index(nwl,0);
    if(nwl<nwf+tol) IL += nbed;
  }
  else{
    IF = Spine->Index(WF,1);
    IL = Spine->Index(WL,0);
    Wrefdeb = Max(Spine->FirstParameter(IF),WF);
    Wreffin = Min(Spine->LastParameter(IL),WL);
  }
  //
  Spine->D1(WF,PDeb,VrefDeb);
  Spine->D1(WL,PFin,VrefFin);
  VrefDeb.Normalize();
  VrefFin.Normalize();
  //
  TColgp_Array1OfPnt ExtrapPole(1, 5);
  TColgp_Array1OfPnt ExtraCoeffs(1, 5);
  TColgp_Array1OfXYZ Cont(1,5);
  // Attention on segmente eventuellement la premiere et la
  // derniere arete.
  // Traitment de la premiere arete
  cepadur = 0;
  E=Spine->Edges(IF);
  Bof=BRepLib::BuildCurve3d(E);
  const BRepAdaptor_Curve& edc = Spine->CurrentElementarySpine(IF);
  tolpared = edc.Resolution(tol);
  Cv = BRep_Tool::Curve(E, First, Last);
  //Add vertex with tangent
  if (ES.IsPeriodic())
  {
    Standard_Real ParForElSpine = (E.Orientation() == TopAbs_FORWARD)? First : Last;
    gp_Pnt PntForElSpine;
    gp_Vec DirForElSpine;
    Cv->D1(ParForElSpine, PntForElSpine, DirForElSpine);
    ES.AddVertexWithTangent(gp_Ax1(PntForElSpine, DirForElSpine));
  }
  /////////////////////////
  urefdeb = Spine->FirstParameter(IF);
  checkdeb = (nwf > urefdeb);
  if(checkdeb) {
    Spine->Parameter(IF,nwf,pared,0);
  }
  //
  if(E.Orientation() == TopAbs_REVERSED) {
    Standard_Real sov = First;
    First = Cv->ReversedParameter(Last);
    Last = Cv->ReversedParameter(sov);
    if(checkdeb) {
      pared = Cv->ReversedParameter(pared);
    }
    else{
      pared = First;
    }
    if(First < pared) {
      First = pared; 
    }
    if(IL == IF) {
      Standard_Real ureffin = Spine->LastParameter(IL);
      Standard_Boolean checkfin = (nwl < ureffin);
      if(checkfin) {
        Spine->Parameter(IL,nwl,pared,0);
        pared = Cv->ReversedParameter(pared);
      }
      else {
        pared = Last;
      }
      if(pared < Last) {
        Last = pared; 
      }
    }
    Cv = Cv->Reversed();
  }//if(E.Orientation() == TopAbs_REVERSED) 
  else {//#1
    if(!checkdeb) {
      pared = First;
    }
    if(First < pared) {
      First = pared; 
    }
    if(IL == IF) {
      Standard_Real ureffin = Spine->LastParameter(IL);
      Standard_Boolean checkfin = (nwl < ureffin);
      if(checkfin) {
        Spine->Parameter(IL,nwl,pared,0);
      }
      else {
        pared = Last;
      }
      if(pared < Last) {
        Last = pared; 
      }
    }
  }// else {//#1
  //
  if(Abs(Last-First) < tolpared) {
    cepadur = 1;
  }
  //
  //Petite veru pour les cas ou un KPart a bouffe l arete
  //sans parvenir a terminer. On tire une droite.
  if(cepadur) {
    Handle(Geom_Line) L;
    gp_Pnt ptemp; gp_Vec vtemp;
    if(WL < Spine->FirstParameter(1) + tol) {
      ES.LastPointAndTgt(ptemp,vtemp);
      gp_Dir d(vtemp);
      gp_Pnt olin;
      olin.ChangeCoord().SetLinearForm(-WL,d.XYZ(),PFin.XYZ());
      L = new Geom_Line(olin,d);
      ES.SetCurve(L);
    }
    else if(WF > Spine->LastParameter(nbed) - tol) {
      ES.FirstPointAndTgt(ptemp,vtemp);
      gp_Dir d(vtemp);
      gp_Pnt olin;
      olin.ChangeCoord().SetLinearForm(-WF,d.XYZ(),PDeb.XYZ());
      L = new Geom_Line(olin,d);
      ES.SetCurve(L);
    }
    return;// => 
  }
  //
  TC = new (Geom_TrimmedCurve)(Cv, First, Last);
  BS=GeomConvert::CurveToBSplineCurve(TC);
  CurveCleaner(BS, Abs(WL-WF)*1.e-4, 0);
  //
  //Smoothing of the curve
  iToApproxByC2=0;
  aContinuity=TC->Continuity();
  bIsSmooth=ChFi3d_IsSmooth(TC);
  if (aContinuity < GeomAbs_C2 && !bIsSmooth) {
    ++iToApproxByC2;
    BS = ChFi3d_ApproxByC2(TC);
    TC=BS;
  }
  //
  //  Concatenation des aretes suivantes
  GeomConvert_CompCurveToBSplineCurve Concat( TC, Convert_QuasiAngular );
  //
  Eold = E;
  for (IEdge=IF+1; IEdge<=IL; ++IEdge) {
    Standard_Integer iloc = IEdge;
    if(periodic) {
      iloc = (IEdge - 1)%nbed + 1;
    }
    //
    E = Spine->Edges(iloc);
    if (BRep_Tool::Degenerated(E)) {
      continue;
    }
    //  
    epsV = tol;
    Bof = TopExp::CommonVertex(Eold, E, V);
    if (Bof) {
      epsV = BRep_Tool::Tolerance(V);
    }
    //
    Bof = BRepLib::BuildCurve3d(E);
    if (!Bof) {
      throw Standard_ConstructionError("PerformElSpine : BuildCurve3d error");
    }
    //
    Cv = BRep_Tool::Curve(E, First, Last);
    //Add vertex with tangent
    Standard_Real ParForElSpine = (E.Orientation() == TopAbs_FORWARD)? First : Last;
    gp_Pnt PntForElSpine;
    gp_Vec DirForElSpine;
    Cv->D1(ParForElSpine, PntForElSpine, DirForElSpine);
    ES.AddVertexWithTangent(gp_Ax1(PntForElSpine, DirForElSpine));
    /////////////////////////
    if(IEdge == IL) {
      Standard_Real ureffin = Spine->LastParameter(iloc);
      Standard_Boolean checkfin = (nwl < ureffin);
      if(checkfin) {
        Spine->Parameter(iloc,nwl,pared,0);
      }
      else {
        pared = Last;
      }
      if(E.Orientation() == TopAbs_REVERSED) {
        Standard_Real sov = First;
        First = Cv->ReversedParameter(Last);
        Last = Cv->ReversedParameter(sov);
        if(checkfin) {
          pared = Cv->ReversedParameter(pared);
        }
        else{
          pared = Last;
        }
        Cv = Cv->Reversed();
      }
      if(pared < Last) {
        Last = pared; 
      }
    }
    //
    TC = new (Geom_TrimmedCurve)(Cv, First, Last);
    BS = GeomConvert::CurveToBSplineCurve(TC);
    CurveCleaner(BS, Abs(WL-WF)*1.e-4, 0);
    //
    //Smoothing of the curve
    aContinuity=TC->Continuity();
    bIsSmooth=ChFi3d_IsSmooth(TC);
    if (aContinuity < GeomAbs_C2 && !bIsSmooth) {
      ++iToApproxByC2;
      BS = ChFi3d_ApproxByC2( TC );
      TC = BS;
    }
    //
    tolrac = Min(tol, epsV);
    Bof = Concat.Add( TC, 2.*tolrac, Standard_True );
    // si l'ajout ne s'est pas bien passe on essai d'augmenter la tolerance
    if (!Bof) {
      Bof = Concat.Add( TC, 2.*epsV, Standard_True );
    }
    if (!Bof) {
      Bof = Concat.Add( TC, 200.*epsV, Standard_True );
      if (!Bof) {
        throw Standard_ConstructionError("PerformElSpine: spine merged error");
      }
    }
    Eold = E;
  }// for (IEdge=IF+1; IEdge<=IL; ++IEdge) {
  //
  // On a la portion d elspine calculee sans prolongements sur la partie 
  // valide des aretes du chemin.
  BSpline = Concat.BSplineCurve();
  // There is a reparametrisation to maximally connect the abscissas of edges.
  TColStd_Array1OfReal BSNoeuds (1, BSpline->NbKnots());
  BSpline->Knots(BSNoeuds);
  BSplCLib::Reparametrize (Wrefdeb, Wreffin, BSNoeuds);
  BSpline->SetKnots(BSNoeuds);
  //
  // Traitement des Extremites
  Standard_Integer caredeb, carefin;
  Standard_Real LocalWL, LocalWF, Angle;
  GeomAdaptor_Curve gacurve;
  Handle(Geom_BSplineCurve) newc;
  //
  caredeb = 0;
  carefin = 0;
  Angle = M_PI*0.75;
  LocalWL = WL;
  LocalWF = WF;
  if (!ES.IsPeriodic() && !PDeb.IsEqual(BSpline->Pole(1), tol) ) {
    // Prolongement C3 au debut
    // afin d'eviter des pts d'inflexions dans la partie utile de la
    // spine le prolongement se fait jusqu'a un point eloigne.
    if(BSpline->IsRational()) {
      caredeb = 1;
    }
    //
    Standard_Real rabdist = Wrefdeb - WF;
    Bout = PDeb.Translated(-20*rabdist * VrefDeb);
    Standard_Boolean goodext = 0;
    for(Standard_Integer icont = 3; icont>=1 && !goodext; icont--) {
      Handle(Geom_BoundedCurve) anExtCurve = BSpline;
      GeomLib::ExtendCurveToPoint (anExtCurve, Bout, icont, Standard_False);
      newc = Handle(Geom_BSplineCurve)::DownCast (anExtCurve);
      gacurve.Load(newc);
      GCPnts_AbscissaPoint GCP(gacurve,-rabdist,Wrefdeb,WF);
      if(GCP.IsDone()) {
        WF = GCP.Parameter();
        goodext = GoodExt(newc,VrefDeb,Wrefdeb,WF,Angle);
      }
    }
    if(caredeb) {
      caredeb = newc->NbKnots() - BSpline->NbKnots();
    }
    BSpline = newc;
    LocalWF = BSpline->FirstParameter();
  }
  //
  if (!ES.IsPeriodic() && !PFin.IsEqual(BSpline->Pole(BSpline->NbPoles()), tol) ) {
    // Prolongement C3 en fin
    if(BSpline->IsRational()) {
      carefin = 1;
    }
    Standard_Real rabdist = WL - Wreffin;
    Bout = PFin.Translated(20*rabdist * VrefFin);
    Standard_Boolean goodext = 0;
    for(Standard_Integer icont = 3; icont>=1 && !goodext; icont--) {
      Handle(Geom_BoundedCurve) anExtCurve = BSpline;
      GeomLib::ExtendCurveToPoint (anExtCurve, Bout, icont, Standard_True);
      newc = Handle(Geom_BSplineCurve)::DownCast (anExtCurve);
      gacurve.Load(newc);
      GCPnts_AbscissaPoint GCP(gacurve,rabdist,Wreffin,WL);
      if(GCP.IsDone()) {
        WL = GCP.Parameter();
        goodext = GoodExt(newc, VrefFin, Wreffin,WL,Angle);
      }
    }
    if(carefin) {
      carefin = newc->NbKnots() - BSpline->NbKnots();
    }
    BSpline = newc;
    LocalWL = BSpline->LastParameter(); 
  }
  //
  //Reparametrisation et segmentation sur le domaine de la Spine.
  if(Abs(BSpline->FirstParameter() - WF)<tol) {
    WF = BSpline->FirstParameter();
  }
  if(Abs(BSpline->LastParameter() - WL)<tol) {
    WL = BSpline->LastParameter();
  }
  //
  if ( (LocalWF<WF) ||  (LocalWL>WL)) {   // pour eviter des pb avec segment!
    BSpline->Segment(WF, WL);
    ES.FirstParameter(WF);
    ES.LastParameter(WL);
  }
  //
  if (BSpline->IsRational()) {
    Handle(Geom_BSplineCurve) C1;
    C1 =  Handle(Geom_BSplineCurve)::DownCast(BSpline->Copy());
    GeomConvert::C0BSplineToC1BSplineCurve(C1, tol, 0.1);
    // Il faut s'assurer que l'origine n'a pas bouge (cts21158)
    if (C1->FirstParameter() == BSpline->FirstParameter()) {
      BSpline = C1;
    }
    else {
      //cout << "Attention : Echec de C0BSplineToC1 !" << endl;
    }
  }
  //
  Standard_Integer fk, lk, MultMax, ii;
  // Deformation eventuelle pour rendre la spine C2.
  // ou C3 pour des approx C2
  if((caredeb || carefin) && BSpline->Degree() < 8) {
    BSpline->IncreaseDegree(8);
  }
  //
  fk = 2;
  lk = BSpline->NbKnots()-1;
  if(BSpline->IsPeriodic()) {
    fk = 1;
  }
  if(caredeb) {
    fk += caredeb;
  }
  if(carefin) {
    lk -= carefin;
  }
  //
  if (continuity == GeomAbs_C3) {
    if (BSpline->Degree() < 7) {
      BSpline->IncreaseDegree(7);
    }
    MultMax = BSpline->Degree() - 3;
  }
  else {
    if (BSpline->Degree() < 5) {
      BSpline->IncreaseDegree(5);
    }
    MultMax = BSpline->Degree() - 2;
  }
  // correction C2 or C3 (if possible)
  CurveCleaner(BSpline, Abs(WL-WF)*1.e-4, 1);
  CurveCleaner(BSpline, Abs(WL-WF)*1.e-2, MultMax);
  Standard_Integer MultMin = Max(BSpline->Degree() - 4, 1);
  for (ii = fk; ii <= lk; ii++) {
    if( BSpline->Multiplicity(ii) > MultMax ) {
      Bof = BSpline->RemoveKnot(ii, MultMax, Abs(WL-WF)/10);
    }
    // See C4
    if( BSpline->Multiplicity(ii) > MultMin ) {
      Bof = BSpline->RemoveKnot(ii, MultMin, Abs(WL-WF)*1.e-4);
    }       
  }
  // elspine periodic => BSpline Periodic
  if(ES.IsPeriodic()) {
    if(!BSpline->IsPeriodic()) {
      BSpline->SetPeriodic();
      //modified by NIZNHY-PKV Fri Dec 10 12:20:22 2010ft
      if (iToApproxByC2) {
        Bof = BSpline->RemoveKnot(1, MultMax, Abs(WL-WF)/10);
      }
      //Bof = BSpline->RemoveKnot(1, MultMax, Abs(WL-WF)/10);
      //modified by NIZNHY-PKV Mon Dec 13 14:12:54 2010t
    }
  }
  else { 
    // Otherwise is it necessary to move the poles to adapt 
    // them to new tangents ?
    Standard_Boolean adjust = Standard_False; 
    gp_Pnt P1, P2;
    gp_Vec V1, V2;
    BSpline->D1(WF, P1, V1);
    V1.Normalize();
    ES.FirstPointAndTgt(PDeb,VrefDeb);
    Standard_Real scaldeb = VrefDeb.Dot(V1);
    Standard_Real disdeb = PDeb.Distance(P1);
    if((Abs(WF-LocalWF) < 1.e-12) &&
      ((scaldeb <= 0.9999999) ||
      disdeb >= tol)) {   
        // Yes if there was no extension and the tangent is not the good one.
        adjust = Standard_True;
    } 
    BSpline->D1(WL, P2, V2);
    V2.Normalize();
    ES.LastPointAndTgt(PFin,VrefFin);
    Standard_Real scalfin = VrefFin.Dot(V2); 
    Standard_Real disfin = PFin.Distance(P2);
    if((Abs(WL-LocalWL) < 1.e-12) && 
      ((scalfin <= 0.9999999)||
      disfin >= tol)) {
        // the same at the end
        adjust = Standard_True;
    }
    if(adjust) {
      Handle(Geom_BoundedCurve) anExtCurve = BSpline;
      GeomLib::AdjustExtremity(anExtCurve, PDeb, PFin, VrefDeb, VrefFin);
      BSpline = Handle(Geom_BSplineCurve)::DownCast (anExtCurve);
    }
  }

  // Le Resultat       
  ES.SetCurve(BSpline);
}

//=======================================================================
//function : cherche_face1
//purpose  : find face F different from F1 in the map.
// The map contains two faces adjacent to an edge 
//=======================================================================
void ChFi3d_cherche_face1 (const TopTools_ListOfShape & map,
  const TopoDS_Face & F1,
  TopoDS_Face &  F)   
{ 
  TopoDS_Face Fcur;
  Standard_Boolean trouve=Standard_False;
  TopTools_ListIteratorOfListOfShape It;
  for (It.Initialize(map);It.More()&&!trouve;It.Next()) { 
    Fcur=TopoDS::Face (It.Value());
    if (!Fcur.IsSame(F1)) {
      F=Fcur;trouve=Standard_True;}
  }
} 
//=======================================================================
//function : cherche_element
//purpose  : find edge E of F1 other than E1 and containing vertex V
// Vtx is the other vertex of E 
//=======================================================================
void ChFi3d_cherche_element(const TopoDS_Vertex & V,
  const TopoDS_Edge & E1,
  const TopoDS_Face & F1,
  TopoDS_Edge & E , 
  TopoDS_Vertex  & Vtx )
{ 
  Standard_Integer ie; 
  TopoDS_Vertex V1,V2;
  Standard_Boolean trouve=Standard_False;
  TopoDS_Edge Ecur;
  TopTools_IndexedMapOfShape  MapE;
  TopExp::MapShapes( F1,TopAbs_EDGE,MapE);
  for ( ie=1; ie<= MapE.Extent()&& !trouve; ie++) {  
    Ecur = TopoDS::Edge (MapE(ie));
    if (!Ecur.IsSame(E1)) { 
      TopTools_IndexedMapOfShape  MapV;
      TopExp::MapShapes(Ecur, TopAbs_VERTEX, MapV);
      if (MapV.Extent()==2) {
        V1 = TopoDS::Vertex (MapV(1));
        V2 = TopoDS::Vertex (MapV(2));
        if (V1.IsSame(V)) { 
          Vtx=V2;
          E=Ecur;
          trouve=Standard_True;
        }
        else if (V2.IsSame(V)) {
          Vtx=V1;
          E=Ecur;
          trouve=Standard_True;
        }
      }
    }
  }
} 
//=======================================================================
//function : cherche_edge
//purpose  : find edge E of F1 other than the list of edges E1 and
//           containing vertex V  Vtx is the other vertex of E. 
//=======================================================================
void ChFi3d_cherche_edge(const TopoDS_Vertex & V,
  const  TopTools_Array1OfShape & E1,
  const TopoDS_Face & F1,
  TopoDS_Edge & E , 
  TopoDS_Vertex  & Vtx )
{ 
  Standard_Integer ie,i; 
  TopoDS_Vertex V1,V2;
  Standard_Boolean trouve=Standard_False;
  TopoDS_Edge Ecur;
  Standard_Boolean same;
  TopTools_IndexedMapOfShape  MapE;
  TopExp::MapShapes( F1,TopAbs_EDGE,MapE);
  for ( ie=1; ie<= MapE.Extent()&& !trouve; ie++) {  
    Ecur=TopoDS::Edge (MapE(ie));
    same=Standard_False;
    for (i=E1.Lower();i<=E1.Upper() ;i++) {
      if (Ecur.IsSame(E1.Value(i))) same=Standard_True;
    }
    if (!same) {
      TopTools_IndexedMapOfShape  MapV;
      TopExp::MapShapes(Ecur, TopAbs_VERTEX, MapV);
      if (MapV.Extent()==2) {
        V1 = TopoDS::Vertex (MapV(1));
        V2 = TopoDS::Vertex (MapV(2));
        if (V1.IsSame(V)) { 
          Vtx=V2;
          E=Ecur;
          trouve=Standard_True;
        }
        else if (V2.IsSame(V)) {
          Vtx=V1;
          E=Ecur;
          trouve=Standard_True;
        }
      }
    }
  }
}

//=======================================================================
//function : nbface
//purpose  : calculates the number of faces common to a vertex
//           
//=======================================================================
Standard_Integer  ChFi3d_nbface (const TopTools_ListOfShape & mapVF )
{ Standard_Integer nface=0;     
TopTools_ListIteratorOfListOfShape ItF,JtF;
Standard_Integer  fj = 0;
for (ItF.Initialize(mapVF); ItF.More(); ItF.Next()) {
  fj++;
  Standard_Integer kf = 1;
  const TopoDS_Shape& cur = ItF.Value();
  for (JtF.Initialize(mapVF); JtF.More( )&&(kf<fj); JtF.Next(), kf++) {
    if(cur.IsSame(JtF.Value())) break;
  }
  if(kf == fj) nface++;
}
return nface;  
}

//=======================================================================
//function : edge_common_faces 
//purpose  :  determines two faces sharing an edge.
//            F1 = F2 if there is an edge to parce 
//=======================================================================
void ChFi3d_edge_common_faces (const TopTools_ListOfShape & mapEF,
  TopoDS_Face & F1,
  TopoDS_Face &  F2)   
{ TopTools_ListIteratorOfListOfShape It;
TopoDS_Face F;
Standard_Boolean trouve;
It.Initialize(mapEF);  
F1=TopoDS::Face(It.Value());
trouve=Standard_False;
for(It.Initialize(mapEF);It.More()&&!trouve;It.Next()) { 
  F=TopoDS::Face (It.Value());
  if (!F.IsSame(F1)) {
    F2=F;trouve=Standard_True;
  }
}
if (!trouve) F2=F1;
}

/***********************************************************/
// gives the angle between edges E1 and E2 . Vtx is the 
// vertex common to the edges
/************************************************************/
Standard_Real ChFi3d_AngleEdge (const TopoDS_Vertex & Vtx,
  const TopoDS_Edge&  E1,
  const TopoDS_Edge &  E2)
{   Standard_Real angle;
BRepAdaptor_Curve BCurv1(E1);
BRepAdaptor_Curve BCurv2(E2);
Standard_Real parE1,parE2;
gp_Vec dir1,dir2 ;
gp_Pnt P1,P2 ;
parE1=BRep_Tool::Parameter(Vtx,E1);
parE2=BRep_Tool::Parameter(Vtx,E2);
BCurv1.D1(parE1,P1,dir1);
BCurv2.D1(parE2,P2,dir2);
if (!Vtx.IsSame(TopExp::FirstVertex(E1))) dir1.Reverse();
if (!Vtx.IsSame(TopExp::FirstVertex(E2)))  dir2.Reverse();
angle=Abs(dir1.Angle(dir2));
return angle;
}

//==================================================================
// ChercheBordsLibres
// determines if vertex V1 has edges on free borders 
// edgelibre1 and edgelibre2 .
// It is supposed that a top can have only 2 edges on free borders
//===================================================================
void ChFi3d_ChercheBordsLibres(const  ChFiDS_Map & myVEMap,
  const TopoDS_Vertex & V1,
  Standard_Boolean & bordlibre,
  TopoDS_Edge & edgelibre1,
  TopoDS_Edge & edgelibre2) 
{ 
  bordlibre=Standard_False;
  TopTools_ListIteratorOfListOfShape ItE,ItE1;
  Standard_Integer nboccur;
  for (ItE.Initialize(myVEMap(V1)); ItE.More()&&!bordlibre; ItE.Next()) {
    nboccur=0;
    const TopoDS_Edge& cur = TopoDS::Edge(ItE.Value());
    if (!BRep_Tool::Degenerated(cur)) {
      for (ItE1.Initialize(myVEMap(V1)); ItE1.More(); ItE1.Next()) {
        const TopoDS_Edge& cur1 = TopoDS::Edge(ItE1.Value());
        if (cur1.IsSame(cur)) nboccur++;
      }
    }
    if (nboccur==1) {
      edgelibre1=cur;
      bordlibre=Standard_True;
    }
  }
  if (bordlibre) {
    bordlibre=Standard_False;
    for (ItE.Initialize(myVEMap(V1)); ItE.More()&&!bordlibre; ItE.Next()) {
      nboccur=0;
      const TopoDS_Edge& cur = TopoDS::Edge(ItE.Value());
      if (!BRep_Tool::Degenerated(cur)&&!cur.IsSame(edgelibre1)) {
        for (ItE1.Initialize(myVEMap(V1)); ItE1.More(); ItE1.Next()) {
          const TopoDS_Edge& cur1 = TopoDS::Edge(ItE1.Value());
          if (cur1.IsSame(cur)) nboccur++;
        }
      }
      if (nboccur==1) {
        edgelibre2=cur;
        bordlibre=Standard_True;
      }
    }
  }
}

Standard_Boolean ChFi3d_isTangentFaces(const TopoDS_Edge &theEdge,
                                       const TopoDS_Face &theFace1,
                                       const TopoDS_Face &theFace2,
                                       const GeomAbs_Shape Order)
{
  if (Order == GeomAbs_G1 &&
      BRep_Tool::Continuity( theEdge, theFace1, theFace2 ) != GeomAbs_C0)
    return Standard_True;

  Standard_Real TolC0 = Max(0.001, 1.5*BRep_Tool::Tolerance(theEdge));

  Standard_Real aFirst;
  Standard_Real aLast;
    
// Obtaining of pcurves of edge on two faces.
  const Handle(Geom2d_Curve) aC2d1 = BRep_Tool::CurveOnSurface
                                                (theEdge, theFace1, aFirst, aLast);
  const Handle(Geom2d_Curve) aC2d2 = BRep_Tool::CurveOnSurface
                                                (theEdge, theFace2, aFirst, aLast);
  if (aC2d1.IsNull() || aC2d2.IsNull())
    return Standard_False;

// Obtaining of two surfaces from adjacent faces.
  Handle(Geom_Surface) aSurf1 = BRep_Tool::Surface(theFace1);
  Handle(Geom_Surface) aSurf2 = BRep_Tool::Surface(theFace2);

  if (aSurf1.IsNull() || aSurf2.IsNull())
    return Standard_False;

// Computation of the number of samples on the edge.
  BRepAdaptor_Surface              aBAS1(theFace1);
  BRepAdaptor_Surface              aBAS2(theFace2);
  Handle(BRepAdaptor_HSurface)     aBAHS1      = new BRepAdaptor_HSurface(aBAS1);
  Handle(BRepAdaptor_HSurface)     aBAHS2      = new BRepAdaptor_HSurface(aBAS2);
  Handle(BRepTopAdaptor_TopolTool) aTool1      = new BRepTopAdaptor_TopolTool(aBAHS1);
  Handle(BRepTopAdaptor_TopolTool) aTool2      = new BRepTopAdaptor_TopolTool(aBAHS2);
  Standard_Integer                 aNbSamples1 =     aTool1->NbSamples();
  Standard_Integer                 aNbSamples2 =     aTool2->NbSamples();
  Standard_Integer                 aNbSamples  =     Max(aNbSamples1, aNbSamples2);


// Computation of the continuity.
  Standard_Real    aPar;
  Standard_Real    aDelta = (aLast - aFirst)/(aNbSamples - 1);
  Standard_Integer i, nbNotDone = 0;

  for (i = 1, aPar = aFirst; i <= aNbSamples; i++, aPar += aDelta) {
    if (i == aNbSamples) aPar = aLast;

    LocalAnalysis_SurfaceContinuity aCont(aC2d1,  aC2d2,  aPar,
					  aSurf1, aSurf2, Order,
					  0.001, TolC0, 0.1, 0.1, 0.1);
    if (!aCont.IsDone())
      {
	nbNotDone++;
	continue;
      }
    
    if (Order == GeomAbs_G1)
    {
      if (!aCont.IsG1())
        return Standard_False;
    }
    else if (!aCont.IsG2())
      return Standard_False;
  }
  
  if (nbNotDone == aNbSamples)
    return Standard_False;

  //Compare normals of tangent faces in the middle point
  Standard_Real MidPar = (aFirst + aLast)/2.;
  gp_Pnt2d uv1 = aC2d1->Value(MidPar);
  gp_Pnt2d uv2 = aC2d2->Value(MidPar);
  gp_Dir normal1, normal2;
  TopOpeBRepTool_TOOL::Nt( uv1, theFace1, normal1 );
  TopOpeBRepTool_TOOL::Nt( uv2, theFace2, normal2 );
  Standard_Real dot = normal1.Dot(normal2);
  if (dot < 0.)
    return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : NbNotDegeneratedEdges
//purpose  : calculate the number of non-degenerated edges of Map VEMap(Vtx)
// Attention the edges of junctions are taken into account twice
//=======================================================================
Standard_Integer ChFi3d_NbNotDegeneratedEdges (const TopoDS_Vertex& Vtx,
  const ChFiDS_Map& VEMap)
{
  TopTools_ListIteratorOfListOfShape ItE;
  Standard_Integer nba=VEMap(Vtx).Extent();
  for (ItE.Initialize(VEMap(Vtx)); ItE.More(); ItE.Next()) {
    const TopoDS_Edge& cur = TopoDS::Edge(ItE.Value());
    if (BRep_Tool::Degenerated(cur)) nba--;
  }
  return nba;
}

//=======================================================================
//function : NbSharpEdges
//purpose  : calculate the number of sharp edges of Map VEMap(Vtx)
// Attention the edges of junctions are taken into account twice
//=======================================================================
Standard_Integer ChFi3d_NbSharpEdges (const TopoDS_Vertex& Vtx,
                                      const ChFiDS_Map& VEMap,
                                      const ChFiDS_Map& EFMap)
{
  TopTools_ListIteratorOfListOfShape ItE;
  Standard_Integer nba=VEMap(Vtx).Extent();
  for (ItE.Initialize(VEMap(Vtx)); ItE.More(); ItE.Next()) {
    const TopoDS_Edge& cur = TopoDS::Edge(ItE.Value());
    if (BRep_Tool::Degenerated(cur)) nba--;
    else
    {
      TopoDS_Face F1, F2;
      ChFi3d_conexfaces(cur, F1, F2, EFMap);
      if (!F2.IsNull() && ChFi3d_isTangentFaces(cur, F1, F2, GeomAbs_G2))
        nba--;
    }
  }
  return nba;
}

//=======================================================================
//function : NumberOfEdges
//purpose  : calculate the number of edges arriving to the top Vtx
// degenerated edges are not taken into account. 
//=======================================================================
Standard_Integer ChFi3d_NumberOfEdges(const TopoDS_Vertex& Vtx,
  const ChFiDS_Map& VEMap)
{
  Standard_Integer nba;
  Standard_Boolean bordlibre;
  TopoDS_Edge edgelibre1,edgelibre2;
  nba=ChFi3d_NbNotDegeneratedEdges(Vtx, VEMap);
  ChFi3d_ChercheBordsLibres(VEMap,Vtx,bordlibre,edgelibre1,edgelibre2);
  if (bordlibre) nba=(nba-2)/2 +2;
  else  nba=nba/2;
  return nba;
}

//=======================================================================
//function : NumberOfSharpEdges
//purpose  : calculate the number of edges arriving to the top Vtx
// degenerated edges are not taken into account. 
//=======================================================================
Standard_Integer ChFi3d_NumberOfSharpEdges(const TopoDS_Vertex& Vtx,
                                           const ChFiDS_Map& VEMap,
                                           const ChFiDS_Map& EFmap)
{
  Standard_Integer nba;
  Standard_Boolean bordlibre;
  TopoDS_Edge edgelibre1,edgelibre2;
  nba=ChFi3d_NbSharpEdges(Vtx, VEMap, EFmap);
  ChFi3d_ChercheBordsLibres(VEMap,Vtx,bordlibre,edgelibre1,edgelibre2);
  if (bordlibre) nba=(nba-2)/2 +2;
  else  nba=nba/2;
  return nba;
}

//=====================================================
// function cherche_vertex
// finds common vertex between two edges 
//=====================================================

void ChFi3d_cherche_vertex (const TopoDS_Edge & E1,
  const TopoDS_Edge & E2,
  TopoDS_Vertex & vertex,
  Standard_Boolean & trouve)
{ Standard_Integer i,j; 
TopoDS_Vertex Vcur1,Vcur2;
trouve=Standard_False;
TopTools_IndexedMapOfShape  MapV1,MapV2;
TopExp::MapShapes( E1,TopAbs_VERTEX,MapV1);
TopExp::MapShapes( E2,TopAbs_VERTEX,MapV2);
for ( i=1; i<= MapV1.Extent()&&!trouve; i++) {
  TopoDS_Shape alocalshape = TopoDS_Shape (MapV1(i));
  Vcur1=TopoDS::Vertex(alocalshape);
  //    Vcur1=TopoDS::Vertex(TopoDS_Shape (MapV1(i)));
  for ( j=1; j<= MapV2.Extent()&&!trouve; j++) {
    TopoDS_Shape aLocalShape = TopoDS_Shape (MapV2(j));
    Vcur2=TopoDS::Vertex(aLocalShape);
    //      Vcur2=TopoDS::Vertex(TopoDS_Shape (MapV2(j)));
    if (Vcur2.IsSame(Vcur1)) {
      vertex=Vcur1;trouve=Standard_True;
    }
  }
}
}	
//=======================================================================
//function : ChFi3d_Couture
//purpose  : determine si F a une arete de couture
//=======================================================================
void ChFi3d_Couture( const TopoDS_Face & F,
  Standard_Boolean & couture,
  TopoDS_Edge & edgecouture)
{   TopoDS_Edge Ecur;
couture=Standard_False;
TopTools_IndexedMapOfShape  MapE1;
TopExp::MapShapes( F,TopAbs_EDGE,MapE1);
TopLoc_Location Loc;
Handle(Geom_Surface) Surf =BRep_Tool::Surface(F,Loc);
for ( Standard_Integer i=1; i<= MapE1.Extent()&&!couture; i++) {
  TopoDS_Shape aLocalShape = TopoDS_Shape (MapE1(i));
  Ecur=TopoDS::Edge(aLocalShape);
  //      Ecur=TopoDS::Edge(TopoDS_Shape (MapE1(i)));
  if (BRep_Tool::IsClosed(Ecur,Surf,Loc)) {
    couture=Standard_True;
    edgecouture=Ecur;
  } 
}
}

//=======================================================================
//function : ChFi3d_CoutureOnVertex
//purpose  : 
//=======================================================================
void ChFi3d_CoutureOnVertex( const TopoDS_Face & F,
  const TopoDS_Vertex & V,
  Standard_Boolean & couture,
  TopoDS_Edge & edgecouture)
{   TopoDS_Edge Ecur;
couture = Standard_False;
TopTools_IndexedMapOfShape  MapE1;
TopExp::MapShapes( F,TopAbs_EDGE,MapE1);
TopLoc_Location Loc;
Handle(Geom_Surface) Surf = BRep_Tool::Surface(F,Loc);
for ( Standard_Integer i=1; i <= MapE1.Extent(); i++) {
  TopoDS_Shape aLocalShape = TopoDS_Shape (MapE1(i));
  Ecur=TopoDS::Edge(aLocalShape);
  //      Ecur=TopoDS::Edge(TopoDS_Shape (MapE1(i)));
  if (BRep_Tool::IsClosed(Ecur,Surf,Loc)) {
    TopoDS_Vertex Vf, Vl;
    TopExp::Vertices( Ecur, Vf, Vl );
    if (Vf.IsSame(V) || Vl.IsSame(V))
    {
      couture = Standard_True;
      edgecouture = Ecur;
      break;
    }
  } 
}
}
//=======================================================================
//function : ChFi3d_IsPseudoSeam
//purpose  : 
//=======================================================================
Standard_Boolean ChFi3d_IsPseudoSeam( const TopoDS_Edge& E,
  const TopoDS_Face& F )
{
  if (! BRep_Tool::IsClosed( E, F ))
    return Standard_False;

  Standard_Boolean NeighborSeamFound = Standard_False;
  TopoDS_Vertex Vf, Vl, V1, V2;
  TopExp::Vertices( E, Vf, Vl );
  TopExp_Explorer Explo( F, TopAbs_EDGE );
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Edge Ecur = TopoDS::Edge( Explo.Current() );
    if (! Ecur.IsSame(E))
    {
      TopExp::Vertices( Ecur, V1, V2 );
      if ((V1.IsSame(Vf) || V1.IsSame(Vl) || V2.IsSame(Vf) || V2.IsSame(Vl)) &&
        BRepTools::IsReallyClosed( Ecur, F ))
      {
        NeighborSeamFound = Standard_True;
        break;
      }
    }
  }
  return NeighborSeamFound;
}

//=======================================================================
//function : ChFi3d_ApproxByC2
//purpose  : 
//=======================================================================
Handle(Geom_BSplineCurve) ChFi3d_ApproxByC2( const Handle(Geom_Curve)& C )
{
  Standard_Real First = C->FirstParameter(), Last = C->LastParameter();
  Standard_Integer NbPoints = 101;

  TColgp_Array1OfPnt Points( 1, NbPoints );
  Standard_Real delta = (Last - First) / (NbPoints-1);
  for (Standard_Integer i = 1; i <= NbPoints-1; i++)
    Points(i) = C->Value(First + (i-1)*delta);
  Points(NbPoints) = C->Value(Last);

  GeomAPI_PointsToBSpline Approx( Points , Approx_ChordLength, 3, 8, GeomAbs_C2, 1.000001e-3);
  Handle(Geom_BSplineCurve) BS = Approx.Curve();
  return BS;
}
//=======================================================================
//function : ChFi3d_IsSmooth
//purpose  : 
//=======================================================================
Standard_Boolean ChFi3d_IsSmooth( const Handle(Geom_Curve)& C )
{
  GeomAdaptor_Curve GAC( C );

  Standard_Integer ii;
  Standard_Integer intrv, nbintv = GAC.NbIntervals(GeomAbs_CN);
  TColStd_Array1OfReal TI(1,nbintv+1);
  GAC.Intervals(TI,GeomAbs_CN);
  Standard_Real Resolution = gp::Resolution(), Curvature;
  GeomLProp_CLProps LProp(C, 2, Resolution);
  gp_Pnt P1, P2;
  Standard_Integer Discretisation = 30;

  gp_Vec PrevVec;
  Standard_Boolean prevVecFound = Standard_False;
  Standard_Integer intrvFound = 0;
  for( intrv = 1; intrv <= nbintv; intrv++) {
    Standard_Real t = TI(intrv);
    Standard_Real step = (TI(intrv+1) - t) / Discretisation;
    for (ii = 1; ii <= Discretisation; ii++) {
      LProp.SetParameter(t);
      if (!LProp.IsTangentDefined())
        return Standard_False;
      Curvature = Abs(LProp.Curvature());
      if (Curvature > Resolution) {
        C->D0(t, P1);
        LProp.CentreOfCurvature(P2);
        PrevVec = gp_Vec(P1, P2);
        prevVecFound = Standard_True;
        break;
      }
      t += step;
    }
    if( prevVecFound ) {
      intrvFound = intrv;
      break;
    }
  }

  if( !prevVecFound )
    return Standard_True;

  //for (intrv = 1; intrv <= nbintv; intrv++) {
  for (intrv = intrvFound; intrv <= nbintv; intrv++) {
    Standard_Real t = TI(intrv);
    Standard_Real step = (TI(intrv+1) - t) / Discretisation;
    for (ii = 1; ii <= Discretisation; ii++)
    {	 
      LProp.SetParameter(t);
      if (!LProp.IsTangentDefined())
        return Standard_False;
      Curvature = Abs(LProp.Curvature());
      if (Curvature > Resolution)
      {
        C->D0(t, P1);
        LProp.CentreOfCurvature(P2);
        gp_Vec Vec(P1, P2);
        Standard_Real Angle = PrevVec.Angle( Vec );
        if (Angle > M_PI/3.)
          return Standard_False;
        Standard_Real Ratio = Vec.Magnitude() / PrevVec.Magnitude();
        if (Ratio < 1.)
          Ratio = 1. / Ratio;
        if (Ratio > 2. && (intrv != nbintv || ii != Discretisation))
          return Standard_False;
        PrevVec = Vec;
      }
      t += step;
    }
  }

  return Standard_True;
}
