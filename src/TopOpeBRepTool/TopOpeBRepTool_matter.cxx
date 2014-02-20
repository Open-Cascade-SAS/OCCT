// Created on: 1998-11-20
// Created by: Xuan PHAM PHU
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

#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt2d.hxx>
#include <BRep_Tool.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define  M_FORWARD(ori) (ori == TopAbs_FORWARD) 
#define M_REVERSED(ori) (ori == TopAbs_REVERSED) 
#define M_INTERNAL(ori) (ori == TopAbs_INTERNAL) 
#define M_EXTERNAL(ori) (ori == TopAbs_EXTERNAL) 

// ----------------------------------------------------------------------
// oriented vectors :
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
Standard_EXPORT gp_Dir FUN_tool_nCinsideS(const gp_Dir& tgC,const gp_Dir& ngS)
{
  // Give us a curve C on suface S,<parOnC>,a parameter
  // Purpose : compute normal vector to C,tangent to S at
  //           given point,oriented INSIDE S  
  // <tgC> : geometric tangent at point of <parOnC>
  // <ngS> : geometric normal at point of <parOnC> 
  gp_Dir XX(ngS^tgC);
  return XX;  
}
// ----------------------------------------------------------------------
Standard_EXPORT gp_Dir2d FUN_tool_nC2dINSIDES(const gp_Dir2d& tgC2d)
{
  // ------------------------------------------------------------
  // Give us an edge E of 2d rep. on a face F C2d.
  // E is oriented FORWARD in F,then the matter described by
  // the restriction of E on the surface of F is on the left.
  // Give us UV on S,<ngS> is the normal to S at point given by UV
  //                  <tgC2d> is the tangent to C2d at point UV,
  // (X,Y,Z) describes a RONd with :
  // X = (tgC2d,0),Y = (xx,0),Z =(0,0,1)
  // ------------------------------------------------------------
  gp_Dir X,Y,Z;
  Z = gp_Dir(0.,0.,1.);
  X = gp_Dir(tgC2d.X(),tgC2d.Y(),0.);
  Y = Z^X;
  gp_Dir2d xx(Y.X(),Y.Y());
  return xx;
}

// ----------------------------------------------------------------------
//Standard_EXPORT gp_Vec FUN_tool_getgeomxx(const TopoDS_Face& Fi,
Standard_EXPORT gp_Vec FUN_tool_getgeomxx(const TopoDS_Face& ,
                                          const TopoDS_Edge& Ei,
					  const Standard_Real parEi,
                                          const gp_Dir& ngFi)
{
  // <Ei> is an edge of <Fi>,
  // computing XX a vector normal to <ngFi>,
  // oriented IN the face <Fi>.
  // (<ngFi> normal geometric to <Fi> at point of parameter
  // <parEi> on edge <Ei>)
  // <XX> is oriented inside 2d <F> if <E> is FORWARD in <F>
  
  gp_Vec tgEi; Standard_Boolean ok = TopOpeBRepTool_TOOL::TggeomE(parEi,Ei,tgEi);  
  if (!ok) return gp_Vec(0.,0.,0.); //NYIRAISE
  gp_Dir XX = FUN_tool_nCinsideS(tgEi,ngFi);
  return XX;
}

// ----------------------------------------------------------------------
Standard_EXPORT gp_Vec FUN_tool_getgeomxx(const TopoDS_Face& Fi,const TopoDS_Edge& Ei,
					  const Standard_Real parOnEi)
{
  gp_Vec xx(1.,0.,0.);
  gp_Pnt2d uvi; Standard_Boolean ok = FUN_tool_paronEF(Ei,parOnEi,Fi,uvi);
  if (!ok) return xx; // nyiRaise
  gp_Vec ngFi = FUN_tool_nggeomF(uvi,Fi);
  xx = FUN_tool_getgeomxx(Fi,Ei,parOnEi,ngFi);
  return xx;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_getxx(const TopoDS_Face& Fi,const TopoDS_Edge& Ei,
				   const Standard_Real parEi,const gp_Dir& ngFi,
				   gp_Dir& XX)
{
  gp_Vec xx = FUN_tool_getgeomxx(Fi,Ei,parEi,ngFi);
  if (xx.Magnitude() < gp::Resolution()) return Standard_False;
  XX = gp_Dir(xx);
  TopAbs_Orientation oriEinF; Standard_Boolean ok = FUN_tool_orientEinFFORWARD(Ei,Fi,oriEinF);
  if (!ok) return Standard_False;
  if (M_REVERSED(oriEinF)) XX.Reverse();
  return Standard_True;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_getxx(const TopoDS_Face& Fi,const TopoDS_Edge& Ei, const Standard_Real parEi,
				   gp_Dir& XX)
{ 
  Standard_Real tolFi = BRep_Tool::Tolerance(Fi)*1.e2; // nyitol
  gp_Pnt2d uv; Standard_Boolean ok = FUN_tool_parF(Ei,parEi,Fi,uv,tolFi);
  if (!ok) return Standard_False;
  gp_Vec ng = FUN_tool_nggeomF(uv,Fi);
  ok = FUN_tool_getxx(Fi,Ei,parEi,ng,XX); 
  return ok;
}

// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_getdxx(const TopoDS_Face& F,
                                                 const TopoDS_Edge& E,
//                                                 const Standard_Real parE,
                                                 const Standard_Real ,
				                 gp_Vec2d& dxx)
// E xiso (x=u,v)
// points between uvparE and uvparE+dxx are IN F2d
{  
  dxx = gp_Vec2d(0.,0.);
  TopAbs_Orientation oEinFF; Standard_Boolean ok = FUN_tool_orientEinFFORWARD(E,F,oEinFF);
  if (!ok) return Standard_False;
  if (M_INTERNAL(oEinFF) || M_EXTERNAL(oEinFF)) return Standard_False;

  Standard_Boolean isoU,isoV; gp_Dir2d d2d; gp_Pnt2d o2d;
  Standard_Boolean iso = TopOpeBRepTool_TOOL::UVISO(E,F,isoU,isoV,d2d,o2d);
  if (!iso) return Standard_False;
  Standard_Real u1,u2,v1,v2; ok = FUN_tool_isobounds(F,u1,u2,v1,v2);
  if (!ok) return Standard_False;

  Standard_Real xpar = isoU ? o2d.X() : o2d.Y();
  Standard_Real xinf = isoU ? u1 : v1;
  Standard_Real xsup = isoU ? u2 : v2;

  ok = ::FUN_nearestISO(F,xpar,isoU,xinf,xsup);
  if (!ok) return Standard_False;

  Standard_Real ypar = isoU? d2d.Y() : d2d.X();
  Standard_Boolean matterAFTERxpar = Standard_False;
  if (isoU) matterAFTERxpar = (ypar < 0.);
  if (isoV) matterAFTERxpar = (ypar > 0.);
  if (oEinFF == TopAbs_REVERSED) matterAFTERxpar = !matterAFTERxpar;

  Standard_Real dx = 0.;
  if (matterAFTERxpar) dx = xsup-xpar;
  else                 dx = xinf-xpar;

  if (isoU) dxx = gp_Vec2d(dx,0.);
  if (isoV) dxx = gp_Vec2d(0.,dx);
  return Standard_True;
}

