// Created on: 1993-12-06
// Created by: Jacques GOUSSARD
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

#ifndef _BRepBlend_BlendTool_HeaderFile
#define _BRepBlend_BlendTool_HeaderFile

#include <Adaptor3d_Surface.hxx>

class gp_Pnt2d;
class Adaptor2d_Curve2d;
class Adaptor3d_HVertex;

class BRepBlend_BlendTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Projects the point P on the arc C.
  //! If the methods returns Standard_True, the projection is
  //! successful, and Paramproj is the parameter on the arc
  //! of the projected point, Dist is the distance between
  //! P and the curve..
  //! If the method returns Standard_False, Param proj and Dist
  //! are not significant.
  Standard_EXPORT static Standard_Boolean Project(const gp_Pnt2d&                  P,
                                                  const Handle(Adaptor3d_Surface)& S,
                                                  const Handle(Adaptor2d_Curve2d)& C,
                                                  Standard_Real&                   Paramproj,
                                                  Standard_Real&                   Dist);

  Standard_EXPORT static Standard_Boolean Inters(const gp_Pnt2d&                  P1,
                                                 const gp_Pnt2d&                  P2,
                                                 const Handle(Adaptor3d_Surface)& S,
                                                 const Handle(Adaptor2d_Curve2d)& C,
                                                 Standard_Real&                   Param,
                                                 Standard_Real&                   Dist);

  //! Returns the parameter of the vertex V on the edge A.
  static Standard_Real Parameter(const Handle(Adaptor3d_HVertex)& V,
                                 const Handle(Adaptor2d_Curve2d)& A);

  //! Returns the parametric tolerance on the arc A
  //! used to consider that the vertex and another point meet,
  //! i-e if Abs(Parameter(Vertex)-Parameter(OtherPnt))<=
  //! Tolerance, the points are "merged".
  static Standard_Real Tolerance(const Handle(Adaptor3d_HVertex)& V,
                                 const Handle(Adaptor2d_Curve2d)& A);

  static Standard_Boolean SingularOnUMin(const Handle(Adaptor3d_Surface)& S);

  static Standard_Boolean SingularOnUMax(const Handle(Adaptor3d_Surface)& S);

  static Standard_Boolean SingularOnVMin(const Handle(Adaptor3d_Surface)& S);

  static Standard_Boolean SingularOnVMax(const Handle(Adaptor3d_Surface)& S);

  Standard_EXPORT static Standard_Integer NbSamplesU(const Handle(Adaptor3d_Surface)& S,
                                                     const Standard_Real              u1,
                                                     const Standard_Real              u2);

  Standard_EXPORT static Standard_Integer NbSamplesV(const Handle(Adaptor3d_Surface)& S,
                                                     const Standard_Real              v1,
                                                     const Standard_Real              v2);

  //! Returns the parametric limits on the arc C.
  //! These limits must be finite : they are either
  //! the real limits of the arc, for a finite arc,
  //! or a bounding box for an infinite arc.
  Standard_EXPORT static void Bounds(const Handle(Adaptor2d_Curve2d)& C,
                                     Standard_Real&                   Ufirst,
                                     Standard_Real&                   Ulast);

  static Handle(Adaptor2d_Curve2d) CurveOnSurf(const Handle(Adaptor2d_Curve2d)& C,
                                               const Handle(Adaptor3d_Surface)& S);

protected:
private:
};

#include <BRepBlend_BlendTool.lxx>

#endif // _BRepBlend_BlendTool_HeaderFile
