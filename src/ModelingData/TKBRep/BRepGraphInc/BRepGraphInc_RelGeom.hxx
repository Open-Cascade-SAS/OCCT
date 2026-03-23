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

#ifndef _BRepGraphInc_RelGeom_HeaderFile
#define _BRepGraphInc_RelGeom_HeaderFile

#include <Geom2d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TopAbs_Orientation.hxx>
#include <gp_Pnt2d.hxx>

//! Relationship geometry structs for the incidence-table model.
//!
//! These structs capture geometry that lives "between" two entities
//! (edge-face incidence).  They replace the PCurveEntry, PolyOnSurfEntry,
//! and PolyOnTriEntry vectors that were previously inlined on EdgeDef.
//!
//! Seam edges produce two rows with the same (edgeIdx, faceIdx) pair
//! but opposite EdgeOrientation values.
namespace BRepGraphInc
{

//! PCurve and polygon-on-surface geometry for an (edge, face) pair.
struct EdgeFaceGeom
{
  int                EdgeIdx = -1;
  int                FaceIdx = -1;
  TopAbs_Orientation EdgeOrientation = TopAbs_FORWARD;

  //! PCurve on the face surface.
  Handle(Geom2d_Curve) PCurve;
  double               ParamFirst = 0.0;
  double               ParamLast = 0.0;
  gp_Pnt2d             UV1;              //!< UV at ParamFirst
  gp_Pnt2d             UV2;              //!< UV at ParamLast
  GeomAbs_Shape        Continuity = GeomAbs_C0;

  //! Polygon-on-surface (optional, may be null).
  Handle(Poly_Polygon2D) PolyOnSurf;
};

//! Polygon-on-triangulation geometry for an (edge, face, triangulation) triple.
struct EdgeFaceTriGeom
{
  int                EdgeIdx = -1;
  int                FaceIdx = -1;
  int                TriangulationIndex = 0;
  TopAbs_Orientation EdgeOrientation = TopAbs_FORWARD;

  Handle(Poly_PolygonOnTriangulation) PolyOnTri;
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_RelGeom_HeaderFile
