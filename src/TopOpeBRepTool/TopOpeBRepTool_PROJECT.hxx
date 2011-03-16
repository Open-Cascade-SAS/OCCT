// File:	TopOpeBRepTool_PROJECT.hxx
// Created:	Fri Nov 28 15:47:56 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#ifndef _TopOpeBRepTool_PROJECT_HeaderFile
#define _TopOpeBRepTool_PROJECT_HeaderFile

#include <TopOpeBRepTool_define.hxx>

#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPC2d.hxx>

// ----------------------------------------------------------------------
//  project point <P> on geometries (curve <C>,surface <S>)
// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_bounds(const TopoDS_Edge& E,Standard_Real& f,Standard_Real& l);
Standard_EXPORT Standard_Integer FUN_tool_getindex(const Extrema_ExtPC& ponc);
Standard_EXPORT Standard_Integer FUN_tool_getindex(const Extrema_ExtPC2d& ponc);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC(const gp_Pnt& P,const Standard_Real tole,const BRepAdaptor_Curve& BAC,const Standard_Real pmin,const Standard_Real pmax,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC(const gp_Pnt& P,const BRepAdaptor_Curve& BAC,const Standard_Real pmin,const Standard_Real pmax,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC(const gp_Pnt& P,const BRepAdaptor_Curve& BAC,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC2D(const gp_Pnt& P,const Standard_Real tole,const BRepAdaptor_Curve2d& BAC2D,const Standard_Real pmin,const Standard_Real pmax,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC2D(const gp_Pnt& P,const BRepAdaptor_Curve2d& BAC2D,const Standard_Real pmin,const Standard_Real pmax,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC2D(const gp_Pnt& P,const BRepAdaptor_Curve2d& BAC2D,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonS(const gp_Pnt& P,const Handle(Geom_Surface)& S,gp_Pnt2d& UV,Standard_Real& dist);

// ----------------------------------------------------------------------
//  project point <P> on topologies (edge <E>,face <F>)
// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonE(const gp_Pnt& P,const Standard_Real tole,const TopoDS_Edge& E,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonE(const gp_Pnt& P,const TopoDS_Edge& E,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonboundedF(const gp_Pnt& P,const TopoDS_Face& F,gp_Pnt2d& UV,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonF(const gp_Pnt& P,const TopoDS_Face& F,gp_Pnt2d& UV,Standard_Real& dist);

#endif
