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

#ifndef _TopOpeBRepTool_ShapeTool_HeaderFile
#define _TopOpeBRepTool_ShapeTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class TopoDS_Shape;
class gp_Pnt;
class Geom_Curve;
class TopoDS_Edge;
class Geom_Surface;
class TopoDS_Face;
class BRepAdaptor_Surface;
class BRepAdaptor_Curve;
class gp_Dir;

class TopOpeBRepTool_ShapeTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the tolerance of the shape <S>.
  //! If the shape <S> is Null, returns 0.
  Standard_EXPORT static Standard_Real Tolerance(const TopoDS_Shape& S);

  //! Returns 3D point of vertex <S>.
  Standard_EXPORT static gp_Pnt Pnt(const TopoDS_Shape& S);

  Standard_EXPORT static Handle(Geom_Curve) BASISCURVE(const Handle(Geom_Curve)& C);

  Standard_EXPORT static Handle(Geom_Curve) BASISCURVE(const TopoDS_Edge& E);

  Standard_EXPORT static Handle(Geom_Surface) BASISSURFACE(const Handle(Geom_Surface)& S);

  Standard_EXPORT static Handle(Geom_Surface) BASISSURFACE(const TopoDS_Face& F);

  Standard_EXPORT static void UVBOUNDS(const Handle(Geom_Surface)& S,
                                       Standard_Boolean&           UPeri,
                                       Standard_Boolean&           VPeri,
                                       Standard_Real&              Umin,
                                       Standard_Real&              Umax,
                                       Standard_Real&              Vmin,
                                       Standard_Real&              Vmax);

  Standard_EXPORT static void UVBOUNDS(const TopoDS_Face& F,
                                       Standard_Boolean&  UPeri,
                                       Standard_Boolean&  VPeri,
                                       Standard_Real&     Umin,
                                       Standard_Real&     Umax,
                                       Standard_Real&     Vmin,
                                       Standard_Real&     Vmax);

  //! adjust u,v values in UVBounds of the domain of the
  //! geometric shape <S>, according to Uperiodicity and
  //! VPeriodicity of the domain.
  //! <S> is assumed to be a face.
  //! u and/or v is/are not modified when the domain is
  //! not periodic in U and/or V .
  Standard_EXPORT static void AdjustOnPeriodic(const TopoDS_Shape& S,
                                               Standard_Real&      u,
                                               Standard_Real&      v);

  //! indicates whether shape S1 is a closing shape on S2 or not.
  Standard_EXPORT static Standard_Boolean Closed(const TopoDS_Shape& S1, const TopoDS_Shape& S2);

  Standard_EXPORT static Standard_Real PeriodizeParameter(const Standard_Real par,
                                                          const TopoDS_Shape& EE,
                                                          const TopoDS_Shape& FF);

  Standard_EXPORT static Standard_Boolean ShapesSameOriented(const TopoDS_Shape& S1,
                                                             const TopoDS_Shape& S2);

  Standard_EXPORT static Standard_Boolean SurfacesSameOriented(const BRepAdaptor_Surface& S1,
                                                               const BRepAdaptor_Surface& S2);

  Standard_EXPORT static Standard_Boolean FacesSameOriented(const TopoDS_Shape& F1,
                                                            const TopoDS_Shape& F2);

  Standard_EXPORT static Standard_Boolean CurvesSameOriented(const BRepAdaptor_Curve& C1,
                                                             const BRepAdaptor_Curve& C2);

  Standard_EXPORT static Standard_Boolean EdgesSameOriented(const TopoDS_Shape& E1,
                                                            const TopoDS_Shape& E2);

  //! Compute tangent T, normal N, curvature C at point of parameter
  //! P on curve BRAC. Returns the tolerance indicating if T,N are null.
  Standard_EXPORT static Standard_Real EdgeData(const BRepAdaptor_Curve& BRAC,
                                                const Standard_Real      P,
                                                gp_Dir&                  T,
                                                gp_Dir&                  N,
                                                Standard_Real&           C);

  //! Same as previous on edge E.
  Standard_EXPORT static Standard_Real EdgeData(const TopoDS_Shape& E,
                                                const Standard_Real P,
                                                gp_Dir&             T,
                                                gp_Dir&             N,
                                                Standard_Real&      C);

  Standard_EXPORT static Standard_Real Resolution3dU(const Handle(Geom_Surface)& SU,
                                                     const Standard_Real         Tol2d);

  Standard_EXPORT static Standard_Real Resolution3dV(const Handle(Geom_Surface)& SU,
                                                     const Standard_Real         Tol2d);

  Standard_EXPORT static Standard_Real Resolution3d(const Handle(Geom_Surface)& SU,
                                                    const Standard_Real         Tol2d);

  Standard_EXPORT static Standard_Real Resolution3d(const TopoDS_Face&  F,
                                                    const Standard_Real Tol2d);

protected:
private:
};

#endif // _TopOpeBRepTool_ShapeTool_HeaderFile
