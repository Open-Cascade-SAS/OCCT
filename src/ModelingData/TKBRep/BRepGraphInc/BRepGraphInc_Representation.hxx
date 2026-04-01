// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _BRepGraphInc_Representation_HeaderFile
#define _BRepGraphInc_Representation_HeaderFile

#include <BRepGraph_RepId.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>

//! @brief Geometry and mesh representation structs for the incidence-table model.
//!
//! Each representation struct wraps a single piece of geometry or discretization
//! data (surface, curve, triangulation, polygon) with a typed RepId address
//! and lifecycle tracking fields. Representations are stored in flat per-kind
//! vectors in BRepGraphInc_Storage and referenced from definitions by typed RepId.
namespace BRepGraphInc
{

//! Fields shared by every representation entity.
struct BaseRep
{
  BRepGraph_RepId Id;                //!< Typed address (Kind + per-kind index)
  uint32_t        OwnGen    = 0;     //!< Per-rep mutation counter
  bool            IsRemoved = false; //!< Soft-removal flag
};

//! Surface geometry representation for faces.
struct SurfaceRep : public BaseRep
{
  occ::handle<Geom_Surface> Surface; //!< The geometric surface
};

//! 3D curve geometry representation for edges.
struct Curve3DRep : public BaseRep
{
  occ::handle<Geom_Curve> Curve; //!< The 3D curve geometry
};

//! 2D parametric curve (PCurve) representation for coedges.
struct Curve2DRep : public BaseRep
{
  occ::handle<Geom2d_Curve> Curve; //!< The 2D parametric curve
};

//! Triangulation mesh representation for faces.
struct TriangulationRep : public BaseRep
{
  occ::handle<Poly_Triangulation> Triangulation; //!< The mesh
};

//! 3D polygon discretization for edges.
struct Polygon3DRep : public BaseRep
{
  occ::handle<Poly_Polygon3D> Polygon; //!< The 3D polygon
};

//! 2D polygon-on-surface discretization for coedges.
struct Polygon2DRep : public BaseRep
{
  occ::handle<Poly_Polygon2D> Polygon; //!< The 2D polygon on surface parametric space
};

//! Polygon-on-triangulation for coedges.
//! Links a polygon to a specific triangulation rep (global index, not face-local).
struct PolygonOnTriRep : public BaseRep
{
  occ::handle<Poly_PolygonOnTriangulation> Polygon; //!< Polygon indices into triangulation
  BRepGraph_TriangulationRepId TriangulationRepId;  //!< Typed id into myTriangulationsRep
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_Representation_HeaderFile
