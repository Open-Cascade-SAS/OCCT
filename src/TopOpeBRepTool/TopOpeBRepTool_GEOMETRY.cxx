// Created on: 1998-10-06
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#include <TopOpeBRepTool_GEOMETRY.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Vec.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <Precision.hxx>

// ----------------------------------------------------------------------
Standard_EXPORT Handle(Geom2d_Curve) BASISCURVE2D(const Handle(Geom2d_Curve)& C)
{
  Handle(Standard_Type) T = C->DynamicType();
  if      ( T == STANDARD_TYPE(Geom2d_OffsetCurve) ) 
    return ::BASISCURVE2D(Handle(Geom2d_OffsetCurve)::DownCast(C)->BasisCurve());
  else if ( T == STANDARD_TYPE(Geom2d_TrimmedCurve) )
    return ::BASISCURVE2D(Handle(Geom2d_TrimmedCurve)::DownCast(C)->BasisCurve());
  else return C;
}

/*// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_IsUViso(const Handle(Geom2d_Curve)& PC,
				     Standard_Boolean& isoU,Standard_Boolean& isoV,
				     gp_Dir2d& d2d,gp_Pnt2d& o2d)
{
  isoU = isoV = Standard_False;
  if (PC.IsNull()) return Standard_False;
  Handle(Geom2d_Curve) LLL = BASISCURVE2D(PC);
  Handle(Standard_Type) T2 = LLL->DynamicType();
  Standard_Boolean isline2d = (T2 == STANDARD_TYPE(Geom2d_Line));
  if (!isline2d) return Standard_False;

  Handle(Geom2d_Line) L = Handle(Geom2d_Line)::DownCast(LLL);
  d2d = L->Direction();
  isoU = (Abs(d2d.X()) < Precision::Parametric(Precision::Confusion()));
  isoV = (Abs(d2d.Y()) < Precision::Parametric(Precision::Confusion()));
  Standard_Boolean isoUV = isoU || isoV;
  if (!isoUV) return Standard_False;

  o2d = L->Location();
  return Standard_True;
}*/

// ----------------------------------------------------------------------
Standard_EXPORT gp_Dir FUN_tool_dirC(const Standard_Real par,const Handle(Geom_Curve)& C)
{
  gp_Pnt p; gp_Vec tgE; C->D1(par,p,tgE); 
  gp_Dir dirC(tgE);
  return dirC;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_onapex(const gp_Pnt2d& p2d,const Handle(Geom_Surface)& S)
{  
  Standard_Boolean isapex = Standard_False;
  GeomAdaptor_Surface GS(S);
  Standard_Real tol = Precision::Confusion();
  GeomAbs_SurfaceType ST = GS.GetType();
  Standard_Real toluv = 1.e-8;
  if (ST == GeomAbs_Cone) {
    gp_Cone co = GS.Cone();
    gp_Pnt apex = co.Apex();
    gp_Pnt pnt = GS.Value(p2d.X(),p2d.Y());
    Standard_Real dist = pnt.Distance(apex);
    isapex = (dist < tol);
  }
  if (ST == GeomAbs_Sphere) {
    Standard_Real pisur2 = M_PI*.5;
    Standard_Real v = p2d.Y();
    Standard_Boolean vpisur2 = (Abs(v-pisur2) < toluv);
    Standard_Boolean vmoinspisur2 = (Abs(v+pisur2) < toluv);
    isapex = vpisur2 || vmoinspisur2;
  }
  return isapex;
}

// ----------------------------------------------------------------------
Standard_EXPORT gp_Dir FUN_tool_ngS(const gp_Pnt2d& p2d,const Handle(Geom_Surface)& S)
{
  // ###############################
  // nyi : all geometries are direct
  // ###############################
  gp_Pnt p; gp_Vec d1u,d1v;
  S->D1(p2d.X(),p2d.Y(),p,d1u,d1v);  

  Standard_Real du = d1u.Magnitude();
  Standard_Real dv = d1v.Magnitude();
  Standard_Real tol = Precision::Confusion();
  Standard_Boolean kpart = (du < tol) || (dv < tol);
  if (kpart) { 
    GeomAdaptor_Surface GS(S);
    GeomAbs_SurfaceType ST = GS.GetType();
    Standard_Real toluv = 1.e-8;
    if (ST == GeomAbs_Cone) {
      Standard_Boolean nullx = (Abs(p2d.X()) < toluv);
      Standard_Boolean apex = nullx && (Abs(p2d.Y()) < toluv);
      if (apex) {
        gp_Dir axis = GS.Cone().Axis().Direction();
        gp_Vec ng(axis);
        ng.Reverse();
        return ng;
      }
      else if (du < tol) {
	Standard_Real vf = GS.FirstVParameter();
	Standard_Boolean onvf = Abs(p2d.Y()-vf)<toluv;

	Standard_Real x = p2d.X(); Standard_Real y = p2d.Y();
	//NYIXPU : devrait plutot etre fait sur les faces & TopOpeBRepTool_TOOL::minDUV...
	if (onvf) y += 1.;
	else      y -= 1.;
	S->D1(x,y,p,d1u,d1v); 	
	gp_Vec ng = d1u^d1v;
	return ng;
      }
    }
    if (ST == GeomAbs_Sphere) {
//      Standard_Real deuxpi = 2*M_PI;
      Standard_Real pisur2 = M_PI*.5;
      Standard_Real u = p2d.X(),v = p2d.Y();
//      Standard_Boolean u0  =(Abs(u) < toluv);
//      Standard_Boolean u2pi=(Abs(u-deuxpi) < toluv);
//      Standard_Boolean apex = u0 || u2pi;
      Standard_Boolean vpisur2 = (Abs(v-pisur2) < toluv);
      Standard_Boolean vmoinspisur2 = (Abs(v+pisur2) < toluv);
      Standard_Boolean apex = vpisur2 || vmoinspisur2;
      if (apex) {
	gp_Pnt center = GS.Sphere().Location();
	gp_Pnt value  = GS.Value(u,v); 
	gp_Vec ng(center,value); 
//	ng.Reverse();
	return ng;
      }
    }
#ifdef OCCT_DEBUG
    cout<<"FUN_tool_nggeomF NYI"<<endl;
#endif
    return gp_Dir(0,0,1);
  }

  gp_Dir udir(d1u);
  gp_Dir vdir(d1v);
  gp_Dir ngS(udir^vdir);
  return ngS;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_line(const Handle(Geom_Curve)& C3d)
{
  Handle(Geom_Curve) C = TopOpeBRepTool_ShapeTool::BASISCURVE(C3d);
  GeomAdaptor_Curve GC(C);
  Standard_Boolean line = (GC.GetType() == GeomAbs_Line);
  return line;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_quadCT(const GeomAbs_CurveType& CT)
{
  Standard_Boolean isquad = Standard_False;
  if (CT == GeomAbs_Line) isquad = Standard_True;
  if (CT == GeomAbs_Circle) isquad = Standard_True;
  if (CT == GeomAbs_Ellipse) isquad = Standard_True;
  if (CT == GeomAbs_Hyperbola) isquad = Standard_True;
  if (CT == GeomAbs_Parabola) isquad = Standard_True;
  return isquad;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_quad(const Handle(Geom_Curve)& C3d)
{
  Handle(Geom_Curve) C = TopOpeBRepTool_ShapeTool::BASISCURVE(C3d);
  if (C.IsNull()) return Standard_False;
  GeomAdaptor_Curve GC(C);
  GeomAbs_CurveType CT = GC.GetType();
  Standard_Boolean quad = FUN_quadCT(CT);
  return quad;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_quad(const Handle(Geom2d_Curve)& pc)
{
  Handle(Geom2d_Curve) pcb = BASISCURVE2D(pc); // NYI TopOpeBRepTool_ShapeTool
  if (pcb.IsNull()) return Standard_False;
  Geom2dAdaptor_Curve GC2d(pcb);
  GeomAbs_CurveType typ = GC2d.GetType();
  Standard_Boolean isquad = Standard_False;
  if (typ == GeomAbs_Line) isquad = Standard_True;
  if (typ == GeomAbs_Circle) isquad = Standard_True;
  if (typ == GeomAbs_Ellipse) isquad = Standard_True;
  if (typ == GeomAbs_Hyperbola) isquad = Standard_True;
  if (typ == GeomAbs_Parabola) isquad = Standard_True;
  return isquad;
}
// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_line(const Handle(Geom2d_Curve)& pc)
{
  Handle(Geom2d_Curve) pcb = BASISCURVE2D(pc); // NYI TopOpeBRepTool_ShapeTool
  if (pcb.IsNull()) return Standard_False;
  Geom2dAdaptor_Curve GC2d(pcb);
  GeomAbs_CurveType typ = GC2d.GetType();

  if (typ == GeomAbs_Line) return Standard_True;

  return Standard_False ;

}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_quad(const Handle(Geom_Surface)& S)
{
  if (S.IsNull()) return Standard_False;   
  GeomAdaptor_Surface GAS(S);  
  GeomAbs_SurfaceType typ = GAS.GetType();  
  Standard_Boolean isquad = Standard_False;
  if (typ == GeomAbs_Plane) isquad = Standard_True;
  if (typ == GeomAbs_Cylinder) isquad = Standard_True;
  if (typ == GeomAbs_Cone) isquad = Standard_True;
  if (typ == GeomAbs_Sphere) isquad = Standard_True;
  if (typ == GeomAbs_Torus) isquad = Standard_True;
  return isquad;
}


// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_closed(const Handle(Geom_Surface)& S,
				    Standard_Boolean& uclosed,Standard_Real& uperiod,
				    Standard_Boolean& vclosed,Standard_Real& vperiod)
{
  uperiod = vperiod = 0.;
  if (S.IsNull()) return Standard_False;  
  uclosed = S->IsUClosed(); if (uclosed) uclosed = S->IsUPeriodic(); //xpu261098 (BUC60382)
  if (uclosed) uperiod = S->UPeriod();
  vclosed = S->IsVClosed(); if (vclosed) vclosed = S->IsVPeriodic(); 
  if (vclosed) vperiod = S->VPeriod();
  Standard_Boolean closed = uclosed || vclosed;
  return closed;
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_UpdateBnd2d(Bnd_Box2d& B2d,const Bnd_Box2d& newB2d)
{
//  B2d.SetVoid(); -> DOESN'T EMPTY THE  BOX
  B2d = newB2d;
}
