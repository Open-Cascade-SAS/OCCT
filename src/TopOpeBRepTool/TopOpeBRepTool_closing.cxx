// Created on: 1998-11-24
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

#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Vec2d.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>

//Standard_IMPORT void FUN_tool_ttranslate(const gp_Vec2d& tvector, const TopoDS_Face& fF, TopoDS_Edge& fyE);

Standard_EXPORT Standard_Boolean FUN_tool_UVonclosing(const TopoDS_Edge& E, const TopoDS_Face& F, const Standard_Boolean onU,
					 const Standard_Real xfirst, const Standard_Real xperiod,
					 const Standard_Real toluv)
// purpose : returns TRUE if E has UVrep on F, on closing UVrep
// prequesitory : F is x-closed, its x-2drep describes [xfirst,xfirst+xperiod] 
//                if (onU=1), F is u-closed
{
  // E :
  Standard_Real f,l,tol; Handle(Geom2d_Curve) PC = FC2D_CurveOnSurface(E,F,f,l,tol);
  Standard_Boolean isou,isov; gp_Pnt2d o2d; gp_Dir2d d2d; 
  Standard_Boolean isouv = TopOpeBRepTool_TOOL::UVISO(PC,isou,isov,d2d,o2d);  
  if (!isouv) return Standard_False;

  Standard_Boolean onX = (onU && isou) || ((!onU) && isov);
  if (!onX) return Standard_False;

  Standard_Real dxx=0;
  if (onU) dxx = Abs(o2d.X()-xfirst);
  else     dxx = Abs(o2d.Y()-xfirst);

  Standard_Boolean onclo = (dxx < toluv);
  onclo = onclo || (Abs(xperiod-dxx) < toluv);
  return onclo;
}//FUN_UVonclosing

/*Standard_EXPORT Standard_Boolean FUN_tool_getEclo(const TopoDS_Face& F, TopoDS_Edge& Eclo)
// purpose : get Eclo / closing edge of F
//           rep(Eclo,F) is x-iso, parx=xmin/xmax 
{
  Eclo.Nullify();
  TopExp_Explorer ex(F, TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Edge& e = TopoDS::Edge(ex.Current());
    Standard_Boolean clo = BRep_Tool::IsClosed(e,F);
    if (clo) {Eclo=e; return Standard_True;}
  } 
  return Standard_False; 
}*/

Standard_EXPORT Standard_Boolean FUN_tool_correctCLO(TopoDS_Edge& E, const TopoDS_Face& F)
// purpose : correcting "closing edges", returns Standard_True if E pcurve 
//           is translated
{
  TopoDS_Shape aLocalShape  = F.Oriented(TopAbs_FORWARD);
  TopoDS_Face FFOR = TopoDS::Face(aLocalShape);
//  TopoDS_Face FFOR = TopoDS::Face(F.Oriented(TopAbs_FORWARD));
  // ************************************************************
  // prequesitory : in process add(E,F), we'll keep original
  // E orientation (orientation of E in FFORWARD)
  // ************************************************************
  Standard_Boolean inU; Standard_Real xmin,xper;
  Standard_Boolean closed = FUN_tool_closedS(F,inU,xmin,xper);
  if (!closed) return Standard_False; // F is not periodic
  Standard_Real tolu,tolv; FUN_tool_tolUV(TopoDS::Face(F),tolu,tolv);
  Standard_Real tolx = inU ? tolu : tolv;
  
  Standard_Real dx=0.45678; 
//  Standard_Real f,l,tolpc; Standard_Boolean trim3d = Standard_True; 
//  Handle(Geom2d_Curve) PC = FC2D_CurveOnSurface(E,F,f,l,tolpc,trim3d);  
  Standard_Real f,l,tol; Handle(Geom2d_Curve) PC;
  PC = FC2D_EditableCurveOnSurface(E,FFOR,f,l,tol);  

  Standard_Boolean isoU,isoV; gp_Pnt2d o2d; gp_Dir2d d2d; 
  TopOpeBRepTool_TOOL::UVISO(PC,isoU,isoV,d2d,o2d);
  Standard_Boolean xiso = (inU && isoU)||((!inU) && isoV);
  if (!xiso) return Standard_False;      
  Standard_Real par = dx*f + (1-dx)*l; gp_Vec2d dxx;

  FUN_tool_getdxx(FFOR,E,par,dxx);

  TopExp_Explorer ex(FFOR, TopAbs_EDGE);
  for (; ex.More(); ex.Next()){    
    const TopoDS_Edge& e1 = TopoDS::Edge(ex.Current());
    Standard_Boolean closing = BRep_Tool::IsClosed(e1,F);
    if (!closing) continue;

//    Standard_Real f1,l1; Handle(Geom2d_Curve) PC1 = BRep_Tool::CurveOnSurface(e1,F,f1,l1);  
    Standard_Real f1,l1,tol1; Handle(Geom2d_Curve) PC1;
    PC1 = FC2D_EditableCurveOnSurface(e1,FFOR,f1,l1,tol1);

    Standard_Boolean isoU1,isoV1; gp_Pnt2d o2d1; gp_Dir2d d2d1; 
    TopOpeBRepTool_TOOL::UVISO(PC1,isoU1,isoV1,d2d1,o2d1);

    // 2d(e1,FFOR) and 2d(E,FFOR) describe the same side of matter 
    Standard_Real par1 = dx*f1 + (1-dx)*l1; gp_Vec2d dxx1;
    FUN_tool_getdxx(FFOR,e1,par1,dxx1);
    Standard_Real dot = dxx.Dot(dxx1);
    if (dot < 0.) continue;

    gp_Vec2d dxx; Standard_Real dd=0;
    if (inU) {dd = o2d1.X()-o2d.X(); dxx = gp_Vec2d(dd,0.);}
    else     {dd = o2d1.Y()-o2d.Y(); dxx = gp_Vec2d(0.,dd);}        

    if (Abs(dd)<tolx) return Standard_False;
    TopOpeBRepTool_TOOL::TrslUVModifE(dxx,FFOR,E); //FUN_tool_ttranslate(dxx,FFOR,E);
  } // ex(FFOR)
  return Standard_False;
}

