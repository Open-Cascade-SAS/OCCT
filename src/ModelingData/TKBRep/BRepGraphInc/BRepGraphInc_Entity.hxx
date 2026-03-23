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

#ifndef _BRepGraphInc_Entity_HeaderFile
#define _BRepGraphInc_Entity_HeaderFile

#include <BRepGraph_NodeCache.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

#include <GeomAbs_Shape.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangulation.hxx>
#include <gp_Pnt.hxx>

#include <NCollection_Vector.hxx>

//! Entity structs for the incidence-table model.
//!
//! Each entity holds its intrinsic geometry plus forward-direction children
//! (incidence refs).  Locations are stored only at Shell/Solid/Compound level
//! where they can be non-identity.  No separate Usage objects are needed.
namespace BRepGraphInc
{

//! Fields shared by every entity.
struct BaseEntity
{
  BRepGraph_NodeId   Id;            //!< Typed address (kind + per-kind index)
  BRepGraph_NodeCache Cache;        //!< Lazily-computed derived quantities + user attributes
  bool               IsModified = false; //!< True when mutated since Build()
  bool               IsRemoved = false;  //!< Soft-removal flag
};

//! Vertex entity: 3D point + tolerance.
struct VertexEntity : public BaseEntity
{
  gp_Pnt Point;
  double Tolerance = 0.0;
};

//! Edge entity: 3D curve, parameter range, boundary vertices, flags, polygon.
//! Regularity entries stored inline.
struct EdgeEntity : public BaseEntity
{
  Handle(Geom_Curve) Curve3d;         //!< Null for degenerate edges
  double             ParamFirst = 0.0;
  double             ParamLast = 0.0;
  double             Tolerance = 0.0;

  bool IsDegenerate  = false;
  bool SameParameter = false;
  bool SameRange     = false;

  int StartVertexIdx = -1;            //!< Index into VertexEntity vector
  int EndVertexIdx   = -1;            //!< Index into VertexEntity vector

  Handle(Poly_Polygon3D) Polygon3D;   //!< Optional 3D polygon discretization

  //! Edge regularity between adjacent face pairs.
  struct RegularityEntry
  {
    int           FaceIdx1 = -1;
    int           FaceIdx2 = -1;
    GeomAbs_Shape Continuity = GeomAbs_C0;
  };
  NCollection_Vector<RegularityEntry> Regularities;

  //! Return start vertex index adjusted for orientation.
  int OrientedStartVertex(TopAbs_Orientation theOri) const
  {
    if (theOri == TopAbs_FORWARD)  return StartVertexIdx;
    if (theOri == TopAbs_REVERSED) return EndVertexIdx;
    return -1;
  }

  //! Return end vertex index adjusted for orientation.
  int OrientedEndVertex(TopAbs_Orientation theOri) const
  {
    if (theOri == TopAbs_FORWARD)  return EndVertexIdx;
    if (theOri == TopAbs_REVERSED) return StartVertexIdx;
    return -1;
  }
};

//! Wire entity: ordered edges with orientations, closure flag.
struct WireEntity : public BaseEntity
{
  bool IsClosed = false;
  NCollection_Vector<EdgeRef> EdgeRefs;  //!< Ordered edge references
};

//! Face entity: surface, triangulations, wires.
struct FaceEntity : public BaseEntity
{
  Handle(Geom_Surface) Surface;
  NCollection_Vector<Handle(Poly_Triangulation)> Triangulations;
  int    ActiveTriangulationIndex = -1;
  double Tolerance = 0.0;
  bool   NaturalRestriction = false;

  //! Convenience: return the active triangulation handle, or null if none.
  Handle(Poly_Triangulation) ActiveTriangulation() const
  {
    if (ActiveTriangulationIndex >= 0 && ActiveTriangulationIndex < Triangulations.Length())
      return Triangulations.Value(ActiveTriangulationIndex);
    return Handle(Poly_Triangulation)();
  }

  //! Wire references: outer wire first (if present), then inner wires.
  NCollection_Vector<WireRef> WireRefs;

  //! Return index of the outer wire, or -1 if none.
  int OuterWireIdx() const
  {
    for (int i = 0; i < WireRefs.Length(); ++i)
    {
      if (WireRefs.Value(i).IsOuter)
        return WireRefs.Value(i).WireIdx;
    }
    return -1;
  }
};

//! Shell entity: ordered face references with local locations.
struct ShellEntity : public BaseEntity
{
  NCollection_Vector<FaceRef> FaceRefs;
};

//! Solid entity: ordered shell references with local locations.
struct SolidEntity : public BaseEntity
{
  NCollection_Vector<ShellRef> ShellRefs;
};

//! Compound entity: heterogeneous child references.
struct CompoundEntity : public BaseEntity
{
  NCollection_Vector<ChildRef> ChildRefs;
};

//! Comp-solid entity: ordered solid references.
struct CompSolidEntity : public BaseEntity
{
  NCollection_Vector<SolidRef> SolidRefs;
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_Entity_HeaderFile
