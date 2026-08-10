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

#include <BRepGraphInc_RepId.hxx>
#include <BRepGraph_NodeId.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Handle.hxx>

//! @brief Geometry representation records for the BRepGraph incidence storage.
//!
//! Curve parameter ranges live in curve-use records because the use record
//! is not reusable - the interval belongs to the owning edge/coedge curve use,
//! not to a shared geometric curve.
namespace BRepGraphInc
{

//! 3D curve use for edges. Owned by a single edge.
struct EdgeCurve3DRep
{
  using TypeId = BRepGraph_EdgeCurve3DRepId;

  BRepGraph_EdgeId        ParentEdgeId;     //!< Owning edge identifier
  occ::handle<Geom_Curve> Curve;            //!< 3D curve geometry
  double                  ParamFirst = 0.0; //!< First curve parameter
  double                  ParamLast  = 0.0; //!< Last curve parameter
};

//! 3D polygon use for edges. Owned by a single edge.
struct EdgePolygon3DRep
{
  using TypeId = BRepGraph_EdgePolygon3DRepId;

  BRepGraph_EdgeId            ParentEdgeId; //!< Owning edge identifier
  occ::handle<Poly_Polygon3D> Polygon;      //!< 3D polygon geometry
};

//! 2D parametric curve (PCurve) use for coedges. Owned by a single coedge.
struct CoEdgeCurve2DRep
{
  using TypeId = BRepGraph_CoEdgeCurve2DRepId;

  BRepGraph_CoEdgeId        ParentCoEdgeId;   //!< Owning coedge identifier
  occ::handle<Geom2d_Curve> Curve;            //!< 2D parametric curve geometry
  double                    ParamFirst = 0.0; //!< First curve parameter
  double                    ParamLast  = 0.0; //!< Last curve parameter
};

//! 2D polygon-on-surface use for coedges. Owned by a single coedge.
struct CoEdgePolygon2DRep
{
  using TypeId = BRepGraph_CoEdgePolygon2DRepId;

  BRepGraph_CoEdgeId          ParentCoEdgeId; //!< Owning coedge identifier
  occ::handle<Poly_Polygon2D> Polygon;        //!< 2D polygon geometry
};

//! Polygon-on-triangulation use for coedges. Owned by a single coedge.
struct CoEdgePolygonOnTriRep
{
  using TypeId = BRepGraph_CoEdgePolygonOnTriRepId;

  BRepGraph_CoEdgeId                       ParentCoEdgeId; //!< Owning coedge identifier
  occ::handle<Poly_PolygonOnTriangulation> Polygon;        //!< Polygon-on-triangulation geometry
};

//! Surface geometry use for faces. Owned by a single face.
struct FaceSurfaceRep
{
  using TypeId = BRepGraph_FaceSurfaceRepId;

  BRepGraph_FaceId          ParentFaceId; //!< Owning face identifier
  occ::handle<Geom_Surface> Surface;      //!< Surface geometry
};

//! Triangulation mesh use for faces. Owned by a single face.
struct FaceTriangulationRep
{
  using TypeId = BRepGraph_FaceTriangulationRepId;

  BRepGraph_FaceId                ParentFaceId;  //!< Owning face identifier
  occ::handle<Poly_Triangulation> Triangulation; //!< Triangulation mesh
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_Representation_HeaderFile
