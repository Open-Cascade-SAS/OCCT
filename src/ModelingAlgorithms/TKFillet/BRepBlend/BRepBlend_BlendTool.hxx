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
  //! If the methods returns true, the projection is
  //! successful, and Paramproj is the parameter on the arc
  //! of the projected point, Dist is the distance between
  //! P and the curve..
  //! If the method returns false, Param proj and Dist
  //! are not significant.
  Standard_EXPORT static bool Project(const gp_Pnt2d&                       P,
                                      const occ::handle<Adaptor3d_Surface>& S,
                                      const occ::handle<Adaptor2d_Curve2d>& C,
                                      double&                               Paramproj,
                                      double&                               Dist);

  Standard_EXPORT static bool Inters(const gp_Pnt2d&                       P1,
                                     const gp_Pnt2d&                       P2,
                                     const occ::handle<Adaptor3d_Surface>& S,
                                     const occ::handle<Adaptor2d_Curve2d>& C,
                                     double&                               Param,
                                     double&                               Dist);

  //! Returns the parameter of the vertex V on the edge A.
  static double Parameter(const occ::handle<Adaptor3d_HVertex>& V,
                          const occ::handle<Adaptor2d_Curve2d>& A);

  //! Returns the parametric tolerance on the arc A
  //! used to consider that the vertex and another point meet,
  //! i-e if std::abs(Parameter(Vertex)-Parameter(OtherPnt))<=
  //! Tolerance, the points are "merged".
  static double Tolerance(const occ::handle<Adaptor3d_HVertex>& V,
                          const occ::handle<Adaptor2d_Curve2d>& A);

  static bool SingularOnUMin(const occ::handle<Adaptor3d_Surface>& S);

  static bool SingularOnUMax(const occ::handle<Adaptor3d_Surface>& S);

  static bool SingularOnVMin(const occ::handle<Adaptor3d_Surface>& S);

  static bool SingularOnVMax(const occ::handle<Adaptor3d_Surface>& S);

  Standard_EXPORT static int NbSamplesU(const occ::handle<Adaptor3d_Surface>& S,
                                        const double                          u1,
                                        const double                          u2);

  Standard_EXPORT static int NbSamplesV(const occ::handle<Adaptor3d_Surface>& S,
                                        const double                          v1,
                                        const double                          v2);

  //! Returns the parametric limits on the arc C.
  //! These limits must be finite : they are either
  //! the real limits of the arc, for a finite arc,
  //! or a bounding box for an infinite arc.
  Standard_EXPORT static void Bounds(const occ::handle<Adaptor2d_Curve2d>& C,
                                     double&                               Ufirst,
                                     double&                               Ulast);

  static occ::handle<Adaptor2d_Curve2d> CurveOnSurf(const occ::handle<Adaptor2d_Curve2d>& C,
                                                    const occ::handle<Adaptor3d_Surface>& S);
};

#include <BRepBlend_BlendTool.lxx>

#endif // _BRepBlend_BlendTool_HeaderFile
