// File:	TopOpeBRepTool_2d.hxx
// Created:	Mon Mar 23 17:13:14 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>

#ifndef _TopOpeBRepTool_2d_HeaderFile
#define _TopOpeBRepTool_2d_HeaderFile
#include <Geom2d_Curve.hxx>
#include <TopOpeBRepTool_define.hxx>

Standard_EXPORT Standard_Integer FC2D_Prepare(const TopoDS_Shape& S1,const TopoDS_Shape& S2);
Standard_EXPORT Standard_Boolean FC2D_HasC3D(const TopoDS_Edge& E);
Standard_EXPORT Standard_Boolean FC2D_HasCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F);
Standard_EXPORT Standard_Boolean FC2D_HasOldCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Handle(Geom2d_Curve)& C2D,Standard_Real& f,Standard_Real& l,Standard_Real& tol);
Standard_EXPORT Standard_Boolean FC2D_HasOldCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Handle(Geom2d_Curve)& C2D);
Standard_EXPORT Standard_Boolean FC2D_HasNewCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Handle(Geom2d_Curve)& C2D,Standard_Real& f,Standard_Real& l,Standard_Real& tol);
Standard_EXPORT Standard_Boolean FC2D_HasNewCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Handle(Geom2d_Curve)& C2D);
Standard_EXPORT Handle(Geom2d_Curve) FC2D_CurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Standard_Real& f,Standard_Real& l,Standard_Real& tol,const Standard_Boolean trim3d = Standard_False);
Standard_EXPORT Handle(Geom2d_Curve) FC2D_CurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,const TopoDS_Edge& EF,Standard_Real& f,Standard_Real& l,Standard_Real& tol,const Standard_Boolean trim3d = Standard_False);
//modified by NIZHNY-MZV
Standard_EXPORT Handle(Geom2d_Curve) FC2D_MakeCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Standard_Real& f,Standard_Real& l,Standard_Real& tol,const Standard_Boolean trim3d = Standard_False);
Standard_EXPORT Handle(Geom2d_Curve) FC2D_EditableCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Standard_Real& f,Standard_Real& l,Standard_Real& tol,const Standard_Boolean trim3d = Standard_False);
Standard_EXPORT Standard_Integer FC2D_AddNewCurveOnSurface(Handle(Geom2d_Curve) PC,const TopoDS_Edge& E,const TopoDS_Face& F,const Standard_Real& f,const Standard_Real& l,const Standard_Real& tol);
Standard_EXPORT Standard_Integer FC2D_RemNewCurveOnSurface(Handle(Geom2d_Curve) PC,const TopoDS_Edge& E,const TopoDS_Face& F);
#endif
