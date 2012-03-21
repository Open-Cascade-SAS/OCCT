// Created on: 1998-03-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
