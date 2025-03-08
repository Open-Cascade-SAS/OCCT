// Created on: 1994-01-25
// Created by: Jacques GOUSSARD
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

#ifndef _BRepBlend_Extremity_HeaderFile
#define _BRepBlend_Extremity_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepBlend_SequenceOfPointOnRst.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Adaptor3d_HVertex;
class IntSurf_Transition;
class BRepBlend_PointOnRst;

class BRepBlend_Extremity
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepBlend_Extremity();

  //! Creates an extremity on a surface
  Standard_EXPORT BRepBlend_Extremity(const gp_Pnt&       P,
                                      const Standard_Real U,
                                      const Standard_Real V,
                                      const Standard_Real Param,
                                      const Standard_Real Tol);

  //! Creates an extremity on a surface. This extremity matches
  //! the vertex <Vtx>.
  Standard_EXPORT BRepBlend_Extremity(const gp_Pnt&                    P,
                                      const Standard_Real              U,
                                      const Standard_Real              V,
                                      const Standard_Real              Param,
                                      const Standard_Real              Tol,
                                      const Handle(Adaptor3d_HVertex)& Vtx);

  //! Creates an extremity on a curve
  Standard_EXPORT BRepBlend_Extremity(const gp_Pnt&       P,
                                      const Standard_Real W,
                                      const Standard_Real Param,
                                      const Standard_Real Tol);

  //! Set the values for an extremity on a surface.
  Standard_EXPORT void SetValue(const gp_Pnt&       P,
                                const Standard_Real U,
                                const Standard_Real V,
                                const Standard_Real Param,
                                const Standard_Real Tol);

  //! Set the values for an extremity on a surface.This
  //! extremity matches the vertex <Vtx>.
  Standard_EXPORT void SetValue(const gp_Pnt&                    P,
                                const Standard_Real              U,
                                const Standard_Real              V,
                                const Standard_Real              Param,
                                const Standard_Real              Tol,
                                const Handle(Adaptor3d_HVertex)& Vtx);

  //! Set the values for an extremity on curve.
  Standard_EXPORT void SetValue(const gp_Pnt&       P,
                                const Standard_Real W,
                                const Standard_Real Param,
                                const Standard_Real Tol);

  //! This method returns the value of the point in 3d space.
  const gp_Pnt& Value() const;

  //! Set the tangent   vector  for an extremity on  a
  //! surface.
  void SetTangent(const gp_Vec& Tangent);

  //! Returns TRUE if the Tangent is  stored.
  Standard_Boolean HasTangent() const;

  //! This  method returns the   value of tangent  in 3d
  //! space.
  const gp_Vec& Tangent() const;

  //! This method returns the fuzziness on the point
  //! in 3d space.
  Standard_Real Tolerance() const;

  //! Set the values for an extremity on a curve.
  Standard_EXPORT void SetVertex(const Handle(Adaptor3d_HVertex)& V);

  //! Sets the values of a point which is on the arc
  //! A, at parameter Param.
  Standard_EXPORT void AddArc(const Handle(Adaptor2d_Curve2d)& A,
                              const Standard_Real              Param,
                              const IntSurf_Transition&        TLine,
                              const IntSurf_Transition&        TArc);

  //! This method returns the parameters of the point
  //! on the concerned surface.
  void Parameters(Standard_Real& U, Standard_Real& V) const;

  //! Returns Standard_True when the point coincide with
  //! an existing vertex.
  Standard_Boolean IsVertex() const;

  //! Returns the vertex when IsVertex returns Standard_True.
  const Handle(Adaptor3d_HVertex)& Vertex() const;

  //! Returns the number of arc containing the extremity.
  //! If the method returns 0, the point is inside the
  //! surface.
  //! Otherwise, the extremity lies on at least 1 arc,
  //! and all the information (arc, parameter, transitions)
  //! are given by the point on restriction (PointOnRst)
  //! returned by the next method.
  Standard_Integer NbPointOnRst() const;

  const BRepBlend_PointOnRst& PointOnRst(const Standard_Integer Index) const;

  Standard_Real Parameter() const;

  Standard_Real ParameterOnGuide() const;

protected:
private:
  Handle(Adaptor3d_HVertex)      vtx;
  BRepBlend_SequenceOfPointOnRst seqpt;
  gp_Pnt                         pt;
  gp_Vec                         tang;
  Standard_Real                  param;
  Standard_Real                  u;
  Standard_Real                  v;
  Standard_Real                  tol;
  Standard_Boolean               isvtx;
  Standard_Boolean               hastang;
};

#include <BRepBlend_Extremity.lxx>

#endif // _BRepBlend_Extremity_HeaderFile
