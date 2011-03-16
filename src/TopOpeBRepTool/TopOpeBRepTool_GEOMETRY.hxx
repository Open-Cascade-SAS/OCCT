// File:	TopOpeBRepTool_GEOMETRY.hxx
// Created:	Tue Oct  6 12:33:18 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>


#ifndef _TopOpeBRepTool_GEOMETRY_HeaderFile
#define _TopOpeBRepTool_GEOMETRY_HeaderFile

#include <TopOpeBRepTool_define.hxx>

#include <GeomAbs_CurveType.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <Bnd_Box2d.hxx>

Standard_EXPORT Handle(Geom2d_Curve) BASISCURVE2D(const Handle(Geom2d_Curve)& C);
//Standard_EXPORT Standard_Boolean FUN_tool_IsUViso(const Handle(Geom2d_Curve)& PC,Standard_Boolean& isoU,Standard_Boolean& isoV,gp_Dir2d& d2d,gp_Pnt2d& O2d);
Standard_EXPORT gp_Dir FUN_tool_dirC(const Standard_Real par,const Handle(Geom_Curve)& C);
Standard_EXPORT Standard_Boolean FUN_tool_onapex(const gp_Pnt2d& p2d,const Handle(Geom_Surface)& S);
Standard_EXPORT gp_Dir FUN_tool_ngS(const gp_Pnt2d& p2d,const Handle(Geom_Surface)& S);

// ----------------------------------------------------------------------
// curve type,surface type :
// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_line(const Handle(Geom_Curve)& C3d);
Standard_EXPORT Standard_Boolean FUN_tool_line(const Handle(Geom2d_Curve)& C2d);
Standard_EXPORT Standard_Boolean FUN_quadCT(const GeomAbs_CurveType& CT);
Standard_EXPORT Standard_Boolean FUN_tool_quad(const Handle(Geom_Curve)& C3d);
Standard_EXPORT Standard_Boolean FUN_tool_quad(const Handle(Geom2d_Curve)& pc);
Standard_EXPORT Standard_Boolean FUN_tool_quad(const Handle(Geom_Surface)& S);
Standard_EXPORT Standard_Boolean FUN_tool_closed(const Handle(Geom_Surface)& S,Standard_Boolean& uclosed,Standard_Real& uperiod,Standard_Boolean& vclosed,Standard_Real& vperiod);
Standard_EXPORT void FUN_tool_UpdateBnd2d(Bnd_Box2d& B2d,const Bnd_Box2d& newB2d);

// ----------------------------------------------------------------------
// oriented vectors :
// ----------------------------------------------------------------------
Standard_EXPORT gp_Dir FUN_tool_nCinsideS(const gp_Dir& tgC,const gp_Dir& ngS);
Standard_EXPORT gp_Dir2d FUN_tool_nC2dINSIDES(const gp_Dir2d& tgC2d);

#endif
