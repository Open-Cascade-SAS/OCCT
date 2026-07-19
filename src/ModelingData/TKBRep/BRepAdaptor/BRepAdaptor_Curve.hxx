// Created on: 1993-02-19
// Created by: Remi LEQUETTE
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

#ifndef _BRepAdaptor_Curve_HeaderFile
#define _BRepAdaptor_Curve_HeaderFile

#include <GeomAdaptor_TransformedCurve.hxx>
#include <TopoDS_Edge.hxx>

class TopoDS_Face;

//! The Curve from BRepAdaptor allows to use an Edge
//! of the BRep topology like a 3D curve.
//!
//! It has the methods the class Curve from Adaptor3d.
//!
//! It is created or Initialized with an Edge. It
//! takes into account local coordinate systems. If
//! the Edge has a 3D curve it is use with priority.
//! If the edge has no 3D curve one of the curves on
//! surface is used. It is possible to enforce using a
//! curve on surface by creating or initialising with
//! an Edge and a Face.
class BRepAdaptor_Curve : public GeomAdaptor_TransformedCurve
{
  DEFINE_STANDARD_RTTIEXT(BRepAdaptor_Curve, GeomAdaptor_TransformedCurve)
public:
  //! Creates an undefined Curve with no Edge loaded.
  Standard_EXPORT BRepAdaptor_Curve();

  //! Creates a Curve to access the geometry of edge <E>.
  Standard_EXPORT BRepAdaptor_Curve(const TopoDS_Edge& E);

  //! Creates a Curve to access the geometry of edge
  //! <E>. The geometry will be computed using the
  //! parametric curve of <E> on the face <F>. An Error
  //! is raised if the edge does not have a pcurve on
  //! the face.
  Standard_EXPORT BRepAdaptor_Curve(const TopoDS_Edge& E, const TopoDS_Face& F);

  //! Shallow copy of adaptor
  Standard_EXPORT occ::handle<Adaptor3d_Curve> ShallowCopy() const override;

  //! Reset currently loaded curve (undone Load()).
  Standard_EXPORT void Reset();

  //! Sets the Curve <me> to access the geometry of
  //! edge <E>.
  Standard_EXPORT void Initialize(const TopoDS_Edge& E);

  //! Sets the Curve <me> to access the geometry of
  //! edge <E>. The geometry will be computed using the
  //! parametric curve of <E> on the face <F>. An Error
  //! is raised if the edge does not have a pcurve on
  //! the face.
  Standard_EXPORT void Initialize(const TopoDS_Edge& E, const TopoDS_Face& F);

  //! Returns the edge.
  Standard_EXPORT const TopoDS_Edge& Edge() const;

  //! Returns the edge tolerance.
  Standard_EXPORT double Tolerance() const;

  //! Returns a curve equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  Standard_EXPORT occ::handle<Adaptor3d_Curve> Trim(const double First,
                                                    const double Last,
                                                    const double Tol) const override;

  // Note: Most methods are inherited from GeomAdaptor_TransformedCurve.
  // The following methods provide access to the underlying curve/transformation:
  // - Curve() - returns const GeomAdaptor_Curve&
  // - ChangeCurve() - returns GeomAdaptor_Curve&
  // - Trsf() - returns const gp_Trsf&
  // - Is3DCurve() - returns true if 3D curve is used
  // - IsCurveOnSurface() - returns true if COS is used
  // - CurveOnSurface() - returns const Adaptor3d_CurveOnSurface&
  //
  // Value, D0, D1, D2, D3, DN methods are inherited and marked as final.
  // They apply the transformation automatically.

private:
  TopoDS_Edge myEdge;
};

#endif // _BRepAdaptor_Curve_HeaderFile
