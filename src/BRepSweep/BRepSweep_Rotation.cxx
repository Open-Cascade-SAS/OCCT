// Created on: 1993-02-15
// Created by: Laurent BOURESCHE
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


#include <GeomAdaptor_SurfaceOfRevolution.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepSweep_Rotation.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Quilt.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Sweep_NumShape.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>

#include <TopExp_Explorer.hxx>
static Standard_Real ComputeTolerance(TopoDS_Edge& E,
				      const TopoDS_Face& F,
				      const Handle(Geom2d_Curve)& C)

{
  if(BRep_Tool::Degenerated(E)) return BRep_Tool::Tolerance(E);

  Standard_Real first,last;
  
  Handle(Geom_Surface) surf = BRep_Tool::Surface(F);
  Handle(Geom_Curve)   c3d  = BRep_Tool::Curve(E,first,last);
  
  Standard_Real d2 = 0.;
  Standard_Integer nn = 23;
  Standard_Real unsurnn = 1./nn;
  for(Standard_Integer i = 0; i <= nn; i++){
    Standard_Real t = unsurnn*i;
    Standard_Real u = first*(1.-t) + last*t;
    gp_Pnt Pc3d  = c3d->Value(u);
    gp_Pnt2d UV  = C->Value(u);
    gp_Pnt Pcons = surf->Value(UV.X(),UV.Y());
    if (Precision::IsInfinite(Pcons.X()) ||
	Precision::IsInfinite(Pcons.Y()) ||
	Precision::IsInfinite(Pcons.Z())) {
      d2=Precision::Infinite();
      break;
    }
    Standard_Real temp = Pc3d.SquareDistance(Pcons);
    if(temp > d2) d2 = temp;
  }
  d2 = 1.5*sqrt(d2);
  if(d2<1.e-7) d2 = 1.e-7;
  return d2;
}

static void SetThePCurve(const BRep_Builder& B,
			 TopoDS_Edge& E,
			 const TopoDS_Face& F,
			 const TopAbs_Orientation O,
			 const Handle(Geom2d_Curve)& C)
{
  // check if there is already a pcurve
  Standard_Real f,l;
  Handle(Geom2d_Curve) OC;
  TopLoc_Location SL;
  Handle(Geom_Plane) GP = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(F,SL));
  if (GP.IsNull())
    OC = BRep_Tool::CurveOnSurface(E,F,f,l);
  if (OC.IsNull()) 
    B.UpdateEdge(E,C,F,ComputeTolerance(E,F,C));
  else {
    if (O == TopAbs_REVERSED) 
      B.UpdateEdge(E,OC,C,F,ComputeTolerance(E,F,C));
    else 
      B.UpdateEdge(E,C,OC,F,ComputeTolerance(E,F,C));
  }
}

//=======================================================================
//function : BRepSweep_Rotation
//purpose  : 
//=======================================================================

BRepSweep_Rotation::BRepSweep_Rotation(const TopoDS_Shape& S, 
				       const Sweep_NumShape& N,
				       const TopLoc_Location& L,
				       const gp_Ax1& A,
				       const Standard_Real D,
				       const Standard_Boolean C):
       BRepSweep_Trsf(BRep_Builder(),S,N,L,C),
       myAng(D),
       myAxe(A)
     
{
  Standard_ConstructionError_Raise_if(D < Precision::Angular(),
				      "BRepSweep_Rotation::Constructor");
  Init();
}


//=======================================================================
//function : MakeEmptyVertex
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Rotation::MakeEmptyVertex
  (const TopoDS_Shape& aGenV, 
   const Sweep_NumShape& aDirV)
{
  //call only in construction mode with copy.
  Standard_ConstructionError_Raise_if
    (!myCopy,"BRepSweep_Translation::MakeEmptyVertex");
  gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(aGenV));
  TopoDS_Vertex V;
  if (aDirV.Index()==2) P.Transform(myLocation.Transformation());
  ////// modified by jgv, 1.10.01, for buc61005 //////
  //myBuilder.Builder().MakeVertex(V,P,Precision::Confusion());
  myBuilder.Builder().MakeVertex( V, P, BRep_Tool::Tolerance(TopoDS::Vertex(aGenV)) );
  ////////////////////////////////////////////////////
  if (aDirV.Index() == 1 && 
      IsInvariant(aGenV) && 
      myDirShapeTool.NbShapes() == 3) {
    myBuiltShapes(myGenShapeTool.Index(aGenV),3) = Standard_True;
    myShapes(myGenShapeTool.Index(aGenV),3) = V;
  }    
  return V;
}


//=======================================================================
//function : MakeEmptyDirectingEdge
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Rotation::MakeEmptyDirectingEdge
  (const TopoDS_Shape& aGenV, 
   const Sweep_NumShape&)
{
  TopoDS_Edge E;
  gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(aGenV));
  gp_Dir Dirz(myAxe.Direction());
  gp_Vec V(Dirz);
  gp_Pnt O(myAxe.Location());
  O.Translate(V.Dot(gp_Vec(O,P)) * V);
  if (O.IsEqual(P,Precision::Confusion())) {
    // make a degenerated edge
    // temporary make 3D curve null so that 
    // parameters should be registered.
    // myBuilder.Builder().MakeEdge(E);
    gp_Ax2 Axis(O,Dirz);
    Handle(Geom_Circle) GC = new Geom_Circle(Axis,0.);
    myBuilder.Builder().
      MakeEdge(E,GC,BRep_Tool::Tolerance(TopoDS::Vertex(aGenV)));
    myBuilder.Builder().Degenerated(E,Standard_True);
  }
  else {
    gp_Ax2 Axis(O,Dirz,gp_Dir(gp_Vec(O,P)));
    Handle(Geom_Circle) GC = new Geom_Circle(Axis,O.Distance(P));
    Standard_Real tol = BRep_Tool::Tolerance(TopoDS::Vertex(aGenV));
    myBuilder.Builder().MakeEdge(E, GC, tol);
  }
  return E;
}


//=======================================================================
//function : MakeEmptyGeneratingEdge
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Rotation::MakeEmptyGeneratingEdge
  (const TopoDS_Shape& aGenE, 
   const Sweep_NumShape& aDirV)
{
  //call in case of construction with copy, or only when meridian touches myaxe.
  TopoDS_Edge E; 
  if(BRep_Tool::Degenerated(TopoDS::Edge(aGenE)))
  {
    myBuilder.Builder().MakeEdge(E);
    myBuilder.Builder().UpdateEdge(E, BRep_Tool::Tolerance(TopoDS::Edge(aGenE)));
    myBuilder.Builder().Degenerated(E, Standard_True);
  }
  else
  {
    Standard_Real First,Last;
    TopLoc_Location Loc;
    Handle(Geom_Curve) C = Handle(Geom_Curve)::DownCast
      (BRep_Tool::Curve(TopoDS::Edge(aGenE),Loc,First,Last)->Copy());
    if(!C.IsNull())
    {
      C->Transform(Loc.Transformation());
      if(aDirV.Index() == 2) C->Transform(myLocation.Transformation()); 
    }
    myBuilder.Builder().MakeEdge(E,C,BRep_Tool::Tolerance(TopoDS::Edge(aGenE)));
  }
  if (aDirV.Index() == 1 && 
      IsInvariant(aGenE) && 
      myDirShapeTool.NbShapes() == 3) {
    myBuiltShapes(myGenShapeTool.Index(aGenE),3) = Standard_True;
    myShapes(myGenShapeTool.Index(aGenE),3) = E;
  }
  return E;
}


//=======================================================================
//function : SetParameters
//purpose  : 
//=======================================================================

void  BRepSweep_Rotation::SetParameters
  (const TopoDS_Shape& aNewFace, 
   TopoDS_Shape& aNewVertex, 
   const TopoDS_Shape& aGenF, 
   const TopoDS_Shape& aGenV, 
   const Sweep_NumShape&)
{
  //Glue the parameter of vertices directly included in cap faces.
  gp_Pnt2d pnt2d = BRep_Tool::Parameters(TopoDS::Vertex(aGenV),
					 TopoDS::Face(aGenF));
  myBuilder.Builder().UpdateVertex
    (TopoDS::Vertex(aNewVertex),pnt2d.X(),pnt2d.Y(),
     TopoDS::Face(aNewFace),Precision::PConfusion());
}

//=======================================================================
//function : SetDirectingParameter
//purpose  : 
//=======================================================================

void  BRepSweep_Rotation::SetDirectingParameter
  (const TopoDS_Shape& aNewEdge, 
   TopoDS_Shape& aNewVertex, 
   const TopoDS_Shape&, 
   const Sweep_NumShape&, 
   const Sweep_NumShape& aDirV)
{
  Standard_Real param = 0;
  TopAbs_Orientation ori = TopAbs_FORWARD;
  if (aDirV.Index() == 2) {
    param = myAng;
    ori = TopAbs_REVERSED;
  }
  TopoDS_Vertex V_wnt = TopoDS::Vertex(aNewVertex);
  V_wnt.Orientation(ori);
  myBuilder.Builder().UpdateVertex(V_wnt,
				   param,TopoDS::Edge(aNewEdge),
				   Precision::PConfusion());
}


//=======================================================================
//function : SetGeneratingParameter
//purpose  : 
//=======================================================================

void  BRepSweep_Rotation::SetGeneratingParameter
  (const TopoDS_Shape& aNewEdge, 
   TopoDS_Shape& aNewVertex, 
   const TopoDS_Shape& aGenE, 
   const TopoDS_Shape& aGenV, 
   const Sweep_NumShape&)
{
  TopoDS_Vertex vbid = TopoDS::Vertex(aNewVertex); 
  vbid.Orientation(aGenV.Orientation());
  myBuilder.Builder().UpdateVertex
    (vbid,
     BRep_Tool::Parameter(TopoDS::Vertex(aGenV),TopoDS::Edge(aGenE)),
     TopoDS::Edge(aNewEdge),Precision::PConfusion());
}


//=======================================================================
//function : MakeEmptyFace
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Rotation::MakeEmptyFace
  (const TopoDS_Shape& aGenS, 
   const Sweep_NumShape& aDirS)
{
  Standard_Real toler;
  TopoDS_Face F;
  Handle(Geom_Surface) S;
  if(aGenS.ShapeType()==TopAbs_EDGE){
    TopLoc_Location L;
    Standard_Real First,Last;
    Handle(Geom_Curve) C = BRep_Tool::Curve(TopoDS::Edge(aGenS),L,First,Last);
    toler = BRep_Tool::Tolerance(TopoDS::Edge(aGenS));
    gp_Trsf Tr = L.Transformation();
    C = Handle(Geom_Curve)::DownCast(C->Copy());
    //// modified by jgv, 9.12.03 ////
    C = new Geom_TrimmedCurve( C, First, Last );
    //////////////////////////////////
    C->Transform(Tr);

    Handle(GeomAdaptor_HCurve) HC = new GeomAdaptor_HCurve();
    HC->ChangeCurve().Load(C,First,Last);
    GeomAdaptor_SurfaceOfRevolution AS(HC,myAxe);
    switch(AS.GetType()){
    case GeomAbs_Plane :
      {
	Handle(Geom_Plane) Pl = new Geom_Plane(AS.Plane());
	S = Pl;
      }
      break;
    case GeomAbs_Cylinder :
      {
	Handle(Geom_CylindricalSurface) Cy = 
	  new Geom_CylindricalSurface(AS.Cylinder());
	S = Cy;
      }
      break;
    case GeomAbs_Sphere :
      {
	Handle(Geom_SphericalSurface) Sp = 
	  new Geom_SphericalSurface(AS.Sphere());
	S = Sp;
      }
      break;
    case GeomAbs_Cone :
      {
	Handle(Geom_ConicalSurface) Co = 
	  new Geom_ConicalSurface(AS.Cone());
	S = Co;
      }
      break;
    case GeomAbs_Torus :
      {
	Handle(Geom_ToroidalSurface) To = 
	  new Geom_ToroidalSurface(AS.Torus());
	S = To;
      }
      break;
    default :
      {
	Handle(Geom_SurfaceOfRevolution) Se = 
	  new Geom_SurfaceOfRevolution(C,myAxe);
	S = Se;
      }
      break;
    }
  }
  else{
    TopLoc_Location L;
    S = BRep_Tool::Surface(TopoDS::Face(aGenS),L);
    toler = BRep_Tool::Tolerance(TopoDS::Face(aGenS));
    gp_Trsf Tr = L.Transformation();
    S = Handle(Geom_Surface)::DownCast(S->Copy());
    S->Transform(Tr);
    if (aDirS.Index()==2) S->Transform(myLocation.Transformation());
  }
  myBuilder.Builder().MakeFace(F,S,toler);
  return F;
}


//=======================================================================
//function : SetPCurve
//purpose  : 
//=======================================================================

void  BRepSweep_Rotation::SetPCurve
  (const TopoDS_Shape& aNewFace, 
   TopoDS_Shape& aNewEdge, 
   const TopoDS_Shape& aGenF, 
   const TopoDS_Shape& aGenE, 
   const Sweep_NumShape&,
   const TopAbs_Orientation orien)
{
  //Set on edges of cap faces the same pcurves as 
  //on edges of the generator face.
  Standard_Real First,Last;
  SetThePCurve(myBuilder.Builder(),
	       TopoDS::Edge(aNewEdge),
	       TopoDS::Face(aNewFace),
	       orien,
	       BRep_Tool::CurveOnSurface
	       (TopoDS::Edge(aGenE),TopoDS::Face(aGenF),First,Last));
}


//=======================================================================
//function : SetGeneratingPCurve
//purpose  : 
//=======================================================================

void  BRepSweep_Rotation::SetGeneratingPCurve
  (const TopoDS_Shape& aNewFace, 
   TopoDS_Shape& aNewEdge, 
   const TopoDS_Shape&, 
   const Sweep_NumShape&, 
   const Sweep_NumShape& aDirV,
   const TopAbs_Orientation orien)
{
  TopLoc_Location Loc;
  GeomAdaptor_Surface AS(BRep_Tool::Surface(TopoDS::Face(aNewFace),Loc));
  Standard_Real First,Last;
  Standard_Real u,v;
  gp_Pnt point;
  gp_Pnt2d pnt2d;
  gp_Dir2d dir2d;
  gp_Lin2d L;
  if (AS.GetType()==GeomAbs_Plane){
    gp_Pln pln = AS.Plane();
    gp_Ax3 ax3 = pln.Position();
    Handle(Geom_Curve) aC = BRep_Tool::Curve(TopoDS::Edge(aNewEdge),Loc,First,Last);
    Handle(Geom_Line) GL = Handle(Geom_Line)::DownCast(aC);
    if (GL.IsNull()) {
      Handle(Geom_TrimmedCurve) aTrimmedCurve = Handle(Geom_TrimmedCurve)::DownCast(aC);
      if (!aTrimmedCurve.IsNull()) {
        GL = Handle(Geom_Line)::DownCast(aTrimmedCurve->BasisCurve());
        if (GL.IsNull()) {
            throw Standard_ConstructionError("BRepSweep_Rotation::SetGeneratingPCurve");
        }
      }
    }
    gp_Lin gl = GL->Lin();
    gl.Transform(Loc.Transformation());
    point = gl.Location();
    gp_Dir dir = gl.Direction();
    ElSLib::PlaneParameters(ax3,point,u,v);
    pnt2d.SetCoord(u,v);
    dir2d.SetCoord(dir.Dot(ax3.XDirection()),dir.Dot(ax3.YDirection()));
    L.SetLocation(pnt2d);
    L.SetDirection(dir2d);
  }
  else if (AS.GetType()==GeomAbs_Torus){
    gp_Torus tor = AS.Torus();
    BRepAdaptor_Curve BC(TopoDS::Edge(aNewEdge));
    Standard_Real U = BC.FirstParameter();
    point = BC.Value(U);
    if (point.Distance(tor.Location()) < Precision::Confusion()) {
      v = M_PI;
//  modified by NIZHNY-EAP Wed Mar  1 17:49:29 2000 ___BEGIN___
      u = 0.;
    }
    else {
      ElSLib::TorusParameters(tor.Position(),tor.MajorRadius(),
			      tor.MinorRadius(),point,u,v);
    }
//    u = 0.;
    v = ElCLib::InPeriod(v,0.,2*M_PI);
    if((2*M_PI - v) <= Precision::PConfusion()) v -= 2*M_PI;
    if (aDirV.Index() == 2) {
      Standard_Real uLeft = u-myAng;
      ElCLib::AdjustPeriodic(-M_PI,M_PI,Precision::PConfusion(),uLeft,u);
    }
    else {
      Standard_Real uRight = u+myAng;
      ElCLib::AdjustPeriodic(-M_PI,M_PI,Precision::PConfusion(),u,uRight);
    }
//  modified by NIZHNY-EAP Wed Mar  1 17:49:32 2000 ___END___
    pnt2d.SetCoord(u,v-U);
    L.SetLocation(pnt2d);
    L.SetDirection(gp::DY2d());
  }
  else if (AS.GetType()==GeomAbs_Sphere){
    gp_Sphere sph = AS.Sphere();
    BRepAdaptor_Curve BC(TopoDS::Edge(aNewEdge));
    Standard_Real U = BC.FirstParameter();
    point = BC.Value(U);
    ElSLib::SphereParameters(sph.Position(),sph.Radius(),point,u,v);
    u = 0.;
    if (aDirV.Index() == 2) u = myAng;
    pnt2d.SetCoord(u,v-U);
    L.SetLocation(pnt2d);
    L.SetDirection(gp::DY2d());
  }
  else{
    Standard_Real anAngleTemp = 0;
    if (aDirV.Index() == 2) anAngleTemp = myAng;
    L.SetLocation(gp_Pnt2d(anAngleTemp,0));
    L.SetDirection(gp::DY2d());
  }
  Handle(Geom2d_Line) GL = new Geom2d_Line(L);
  SetThePCurve(myBuilder.Builder(),
	       TopoDS::Edge(aNewEdge),
	       TopoDS::Face(aNewFace),
	       orien,
	       GL);
}


//=======================================================================
//function : SetDirectingPCurve
//purpose  : 
//=======================================================================

void  BRepSweep_Rotation::SetDirectingPCurve
  (const TopoDS_Shape& aNewFace, 
   TopoDS_Shape& aNewEdge, 
   const TopoDS_Shape& aGenE, 
   const TopoDS_Shape& aGenV, 
   const Sweep_NumShape&,
   const TopAbs_Orientation orien)
{
  TopLoc_Location Loc;
  GeomAdaptor_Surface AS(BRep_Tool::Surface(TopoDS::Face(aNewFace),Loc));
  Standard_Real 
    par = BRep_Tool::Parameter(TopoDS::Vertex(aGenV),TopoDS::Edge(aGenE));
  gp_Pnt p2 = BRep_Tool::Pnt(TopoDS::Vertex(aGenV));
  gp_Pnt2d p22d;
  Standard_Real u,v;
  Handle(Geom2d_Curve) thePCurve;

  switch(AS.GetType()){

  case GeomAbs_Plane : 
    {
      gp_Pln pln = AS.Plane();
      gp_Ax3 ax3 = pln.Position();
      gp_Pnt p1 = pln.Location();
      Standard_Real R = p1.Distance(p2);
      ElSLib::PlaneParameters(ax3,p2,u,v);
      gp_Dir2d dx2d(u,v);
      gp_Ax22d axe(gp::Origin2d(),dx2d,gp::DY2d());
      gp_Circ2d C(axe,R);
      Handle(Geom2d_Circle) GC = new Geom2d_Circle(C);
      thePCurve = GC;
    }
    break;

  case GeomAbs_Cone : 
    {
      gp_Cone cone = AS.Cone();
      ElSLib::ConeParameters(cone.Position(),cone.RefRadius(),
			     cone.SemiAngle(),p2,u,v);
      p22d.SetCoord(0.,v);
      gp_Lin2d L(p22d,gp::DX2d());
      Handle(Geom2d_Line) GL = new Geom2d_Line(L);
      thePCurve = GL;
    }
    break;

  case GeomAbs_Sphere : 
    {
      gp_Sphere sph = AS.Sphere();
      ElSLib::SphereParameters(sph.Position(),sph.Radius(),p2,u,v);
      p22d.SetCoord(0.,v);
      gp_Lin2d L(p22d,gp::DX2d());
      Handle(Geom2d_Line) GL = new Geom2d_Line(L);
      thePCurve = GL;
    }
    break;

  case GeomAbs_Torus : 
    {
      gp_Pnt p1;
      Standard_Real u1,u2,v1,v2;
      gp_Torus tor = AS.Torus();
      BRepAdaptor_Curve BC(TopoDS::Edge(aGenE));
      p1 = BC.Value(BC.FirstParameter());
      if (p1.Distance(tor.Location()) < Precision::Confusion()){
	v1 = M_PI;
//  modified by NIZHNY-EAP Thu Mar  2 09:43:26 2000 ___BEGIN___
	u1 = 0.;
//  modified by NIZHNY-EAP Thu Mar  2 15:28:59 2000 ___END___
      }
      else {
	ElSLib::TorusParameters(tor.Position(),tor.MajorRadius(),
				tor.MinorRadius(),p1,u1,v1);
      }
      p2 = BC.Value(BC.LastParameter());
      if (p2.Distance(tor.Location()) < Precision::Confusion()){
	v2 = M_PI;
      }
      else {
	ElSLib::TorusParameters(tor.Position(),tor.MajorRadius(),
				tor.MinorRadius(),p2,u2,v2);
      }
      ElCLib::AdjustPeriodic(0.,2*M_PI,Precision::PConfusion(),v1,v2);
//  modified by NIZHNY-EAP Thu Mar  2 15:29:04 2000 ___BEGIN___
      u2 = u1 + myAng;
      ElCLib::AdjustPeriodic(-M_PI,M_PI,Precision::PConfusion(),u1,u2);
      if (aGenV.Orientation()==TopAbs_FORWARD){
	p22d.SetCoord(u1,v1);
      }
      else {
	p22d.SetCoord(u1,v2);
//  modified by NIZHNY-EAP Thu Mar  2 09:43:32 2000 ___END___
      }
      gp_Lin2d L(p22d,gp::DX2d());
      Handle(Geom2d_Line) GL = new Geom2d_Line(L);
      thePCurve = GL;
    }
    break;

  default :
    {
      p22d.SetCoord(0.,par);
      gp_Lin2d L(p22d,gp::DX2d());
      Handle(Geom2d_Line) GL = new Geom2d_Line(L);
      thePCurve = GL;
    }
    break;
  }
  SetThePCurve(myBuilder.Builder(),
	       TopoDS::Edge(aNewEdge),
	       TopoDS::Face(aNewFace),
	       orien,
	       thePCurve);
}

//modified by NIZNHY-PKV Tue Jun 14 08:33:55 2011f
//=======================================================================
//function : DirectSolid
//purpose  : 
//=======================================================================
TopAbs_Orientation 
  BRepSweep_Rotation::DirectSolid (const TopoDS_Shape& aGenS,
				   const Sweep_NumShape&)
{  // compare the face normal and the direction
  Standard_Real aU1, aU2, aV1, aV2, aUx, aVx, aX, aMV2, aTol2, aTx;
  TopAbs_Orientation aOr;
  gp_Pnt aP;
  gp_Vec du,dv;
  BRepAdaptor_Surface surf(TopoDS::Face(aGenS));
  //
  aTol2=Precision::Confusion();
  aTol2=aTol2*aTol2;
  //
  const gp_Pnt& aPAxeLoc=myAxe.Location();
  const gp_Dir& aPAxeDir=myAxe.Direction();
  //
  aU1=surf.FirstUParameter();
  aU2=surf.LastUParameter();
  aV1=surf.FirstVParameter();
  aV2=surf.LastVParameter();
  //
  aTx=0.5;
  aUx=aTx*(aU1+aU2);
  aVx=aTx*(aV1+aV2);
  surf.D1(aUx, aVx, aP, du, dv);
  //
  gp_Vec aV(aPAxeLoc, aP);
  aV.Cross(aPAxeDir);
  aMV2=aV.SquareMagnitude();
  if (aMV2<aTol2) {
    aTx=0.43213918;
    aUx=aU1*(1.-aTx)+aU2*aTx;
    aVx=aV1*(1.-aTx)+aV2*aTx;
    surf.D1(aUx, aVx, aP, du, dv);
    aV.SetXYZ(aP.XYZ()-aPAxeLoc.XYZ());
    aV.Cross(aPAxeDir);
  }
  //
  aX = aV.DotCross(du, dv);
  aOr = (aX > 0.) ? TopAbs_FORWARD : TopAbs_REVERSED;
  return aOr;
}
/*
//=======================================================================
//function : DirectSolid
//purpose  : 
//=======================================================================
TopAbs_Orientation 
  BRepSweep_Rotation::DirectSolid (const TopoDS_Shape& aGenS,
				   const Sweep_NumShape&)
{
  // compare the face normal and the direction
  BRepAdaptor_Surface surf(TopoDS::Face(aGenS));
  gp_Pnt P;
  gp_Vec du,dv;
  surf.D1((surf.FirstUParameter() + surf.LastUParameter()) / 2.,
	  (surf.FirstVParameter() + surf.LastVParameter()) / 2.,
	  P,du,dv);
    
  gp_Vec V(myAxe.Location(),P);
  V.Cross(myAxe.Direction());
  Standard_Real x = V.DotCross(du,dv);
  TopAbs_Orientation orient = (x > 0) ? TopAbs_FORWARD : TopAbs_REVERSED;
  return orient;
}
*/
//modified by NIZNHY-PKV Tue Jun 14 08:33:59 2011t

//=======================================================================
//function : GGDShapeIsToAdd
//purpose  : 
//=======================================================================

Standard_Boolean BRepSweep_Rotation::GGDShapeIsToAdd
  (const TopoDS_Shape& aNewShape,
   const TopoDS_Shape& aNewSubShape,
   const TopoDS_Shape& aGenS,
   const TopoDS_Shape& aSubGenS,
   const Sweep_NumShape& aDirS )const
{
  Standard_Boolean aRes = Standard_True;
  if (aNewShape.ShapeType()==TopAbs_FACE &&
      aNewSubShape.ShapeType()==TopAbs_EDGE &&
      aGenS.ShapeType()==TopAbs_EDGE &&
      aSubGenS.ShapeType()==TopAbs_VERTEX &&
      aDirS.Type()==TopAbs_EDGE){
    TopLoc_Location Loc;
    GeomAdaptor_Surface AS(BRep_Tool::Surface(TopoDS::Face(aNewShape),Loc));
    if (AS.GetType()==GeomAbs_Plane){
      return (!IsInvariant(aSubGenS));
    }
    else{
      return aRes;
    }
  }
  else{
    return aRes;
  }
}


//=======================================================================
//function : GDDShapeIsToAdd
//purpose  : 
//=======================================================================

Standard_Boolean BRepSweep_Rotation::GDDShapeIsToAdd
  (const TopoDS_Shape& aNewShape,
   const TopoDS_Shape& aNewSubShape,
   const TopoDS_Shape& aGenS,
   const Sweep_NumShape& aDirS,
   const Sweep_NumShape& aSubDirS )const
{
  if ( aNewShape.ShapeType() == TopAbs_SOLID &&
       aNewSubShape.ShapeType() == TopAbs_FACE &&
       aGenS.ShapeType() == TopAbs_FACE &&
       aDirS.Type() == TopAbs_EDGE &&
       aSubDirS.Type() == TopAbs_VERTEX ){
    return ( Abs(myAng - 2 * M_PI) > Precision::Angular() );
  }
  else if ( aNewShape.ShapeType() == TopAbs_FACE &&
       aNewSubShape.ShapeType() == TopAbs_EDGE &&
       aGenS.ShapeType() == TopAbs_EDGE &&
       aDirS.Type() == TopAbs_EDGE &&
       aSubDirS.Type() == TopAbs_VERTEX ){
    TopLoc_Location Loc;
    GeomAdaptor_Surface AS(BRep_Tool::Surface(TopoDS::Face(aNewShape),Loc));
    if (AS.GetType()==GeomAbs_Plane){
      return ( Abs(myAng - 2 * M_PI) > Precision::Angular() );
    }
    else {
      return Standard_True;
    }
  }
  else {
    return Standard_True;
  }
}


//=======================================================================
//function : SeparatedWires
//purpose  : 
//=======================================================================

Standard_Boolean BRepSweep_Rotation::SeparatedWires
  (const TopoDS_Shape& aNewShape,
   const TopoDS_Shape& aNewSubShape,
   const TopoDS_Shape& aGenS,
   const TopoDS_Shape& aSubGenS,
   const Sweep_NumShape& aDirS )const
{
  if (aNewShape.ShapeType()==TopAbs_FACE &&
      aNewSubShape.ShapeType()==TopAbs_EDGE &&
      aGenS.ShapeType()==TopAbs_EDGE &&
      aSubGenS.ShapeType()==TopAbs_VERTEX &&
      aDirS.Type()==TopAbs_EDGE){
    TopLoc_Location Loc;
    GeomAdaptor_Surface AS(BRep_Tool::Surface(TopoDS::Face(aNewShape),Loc));
    if (AS.GetType()==GeomAbs_Plane){
      return (Abs(myAng-2*M_PI) <= Precision::Angular());
    }
    else{
      return Standard_False;
    }
  }
  else{
    return Standard_False;
  }
}

//=======================================================================
//function : SplitShell
//purpose  : 
//=======================================================================

TopoDS_Shape BRepSweep_Rotation::SplitShell(const TopoDS_Shape& aNewShape)const
{
  BRepTools_Quilt Q;
  Q.Add(aNewShape);
  return Q.Shells();
}

//=======================================================================
//function : HasShape
//purpose  : 
//=======================================================================

Standard_Boolean  BRepSweep_Rotation::HasShape
  (const TopoDS_Shape& aGenS, 
   const Sweep_NumShape& aDirS)const 
{
  if(aDirS.Type()==TopAbs_EDGE&&
     aGenS.ShapeType()==TopAbs_EDGE)
  {
    // Verify that the edge has entrails
    const TopoDS_Edge& anEdge = TopoDS::Edge(aGenS);
    //
    if(BRep_Tool::Degenerated(anEdge)) return Standard_False;

    Standard_Real aPFirst, aPLast;
    TopLoc_Location aLoc;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aLoc, aPFirst, aPLast);
    if(aCurve.IsNull()) return Standard_False;

    if(IsInvariant(aGenS)) return Standard_False;

    //Check seem edge
    TopExp_Explorer FaceExp(myGenShape, TopAbs_FACE);
    for (;FaceExp.More(); FaceExp.Next()) {
      TopoDS_Face F = TopoDS::Face(FaceExp.Current());
      if (BRepTools::IsReallyClosed(anEdge, F))
          return Standard_False;
    }

    return Standard_True;
      
  }
  else
  {
    return Standard_True;
  }
}


//=======================================================================
//function : IsInvariant
//purpose  : 
//=======================================================================

Standard_Boolean  BRepSweep_Rotation::IsInvariant 
  (const TopoDS_Shape& aGenS)const
{
  if(aGenS.ShapeType()==TopAbs_EDGE)
  {
    BRepAdaptor_Curve aC(TopoDS::Edge(aGenS));
    if (aC.GetType() == GeomAbs_Line ||
        aC.GetType() == GeomAbs_BSplineCurve ||
        aC.GetType() == GeomAbs_BezierCurve)
    {
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(TopoDS::Edge(aGenS), V1, V2);
      if (IsInvariant(V1) && IsInvariant(V2))
      {
        if (aC.GetType() == GeomAbs_Line)
          return Standard_True;

        Standard_Real aTol = Max(BRep_Tool::Tolerance(V1), BRep_Tool::Tolerance(V2));
        gp_Lin Lin(myAxe.Location(), myAxe.Direction());
        const TColgp_Array1OfPnt& aPoles = (aC.GetType() == GeomAbs_BSplineCurve
          ? aC.BSpline()->Poles() : aC.Bezier()->Poles());

        for (Standard_Integer i=aPoles.Lower(); i <= aPoles.Upper(); i++)
        {
          if (Lin.Distance(aPoles(i)) > aTol)
            return Standard_False;
        }
        return Standard_True;
      }
    }
  }
  else if(aGenS.ShapeType()==TopAbs_VERTEX){
    gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(aGenS));
    gp_Lin Lin (myAxe.Location(), myAxe.Direction());
    return ( Lin.Distance(P) <= BRep_Tool::Tolerance(TopoDS::Vertex(aGenS))); 
  }
  return Standard_False;
}

//=======================================================================
//function : Angle
//purpose  : 
//=======================================================================

Standard_Real BRepSweep_Rotation::Angle()const 
{
  return myAng;
}

//=======================================================================
//function : Axe
//purpose  : 
//=======================================================================

gp_Ax1 BRepSweep_Rotation::Axe()const 
{
  return myAxe;
}

