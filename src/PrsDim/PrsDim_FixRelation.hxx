// Created on: 1996-12-05
// Created by: Flore Lantheaume/Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _PrsDim_FixRelation_HeaderFile
#define _PrsDim_FixRelation_HeaderFile

#include <gp_Circ.hxx>
#include <TopoDS_Wire.hxx>
#include <PrsDim_Relation.hxx>

class Geom_Plane;

DEFINE_STANDARD_HANDLE(PrsDim_FixRelation, PrsDim_Relation)

//! Constructs and manages a constraint by a fixed
//! relation between two or more interactive datums. This
//! constraint is represented by a wire from a shape -
//! point, vertex, or edge - in the first datum and a
//! corresponding shape in the second.
//! Warning: This relation is not bound with any kind of parametric
//! constraint : it represents the "status" of an parametric
//! object.
class PrsDim_FixRelation : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_FixRelation, PrsDim_Relation)
public:
  //! initializes the vertex aShape, the
  //! plane aPlane and the wire aWire, which connects
  //! the two vertices in a fixed relation.
  Standard_EXPORT PrsDim_FixRelation(const TopoDS_Shape&       aShape,
                                     const Handle(Geom_Plane)& aPlane,
                                     const TopoDS_Wire&        aWire);

  //! initializes the vertex aShape, the
  //! plane aPlane and the wire aWire, the position
  //! aPosition, the arrow size anArrowSize and the
  //! wire aWire, which connects the two vertices in a fixed relation.
  Standard_EXPORT PrsDim_FixRelation(const TopoDS_Shape&       aShape,
                                     const Handle(Geom_Plane)& aPlane,
                                     const TopoDS_Wire&        aWire,
                                     const gp_Pnt&             aPosition,
                                     const Standard_Real       anArrowSize = 0.01);

  //! initializes the edge aShape and the plane aPlane.
  Standard_EXPORT PrsDim_FixRelation(const TopoDS_Shape& aShape, const Handle(Geom_Plane)& aPlane);

  //! initializes the edge aShape, the
  //! plane aPlane, the position aPosition and the arrow
  //! size anArrowSize.
  Standard_EXPORT PrsDim_FixRelation(const TopoDS_Shape&       aShape,
                                     const Handle(Geom_Plane)& aPlane,
                                     const gp_Pnt&             aPosition,
                                     const Standard_Real       anArrowSize = 0.01);

  //! Returns the wire which connects vertices in a fixed relation.
  const TopoDS_Wire& Wire() { return myWire; }

  //! Constructs the wire aWire. This connects vertices
  //! which are in a fixed relation.
  void SetWire(const TopoDS_Wire& aWire) { myWire = aWire; }

  //! Returns true if the Interactive Objects in the relation
  //! are movable.
  virtual Standard_Boolean IsMovable() const Standard_OVERRIDE { return Standard_True; }

private:
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                       const Handle(Prs3d_Presentation)&         thePrs,
                                       const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& theSel,
                                                const Standard_Integer theMode) Standard_OVERRIDE;

  //! computes the presentation for <myFixShape> if it's a vertex.
  Standard_EXPORT void ComputeVertex(const TopoDS_Vertex& FixVertex, gp_Pnt& curpos);

  Standard_EXPORT gp_Pnt ComputePosition(const Handle(Geom_Curve)& curv1,
                                         const Handle(Geom_Curve)& curv2,
                                         const gp_Pnt&             firstp1,
                                         const gp_Pnt&             lastp1,
                                         const gp_Pnt&             firstp2,
                                         const gp_Pnt&             lastp2) const;

  Standard_EXPORT gp_Pnt ComputePosition(const Handle(Geom_Curve)& curv,
                                         const gp_Pnt&             firstp,
                                         const gp_Pnt&             lastp) const;

  //! computes the presentation for <myFixShape> if it's a
  //! edge.
  Standard_EXPORT void ComputeEdge(const TopoDS_Edge& FixEdge, gp_Pnt& curpos);

  Standard_EXPORT void ComputeLinePosition(const gp_Lin&  glin,
                                           gp_Pnt&        pos,
                                           Standard_Real& pfirst,
                                           Standard_Real& plast);

  Standard_EXPORT void ComputeCirclePosition(const gp_Circ& gcirc,
                                             gp_Pnt&        pos,
                                             Standard_Real& pfirst,
                                             Standard_Real& plast);

  Standard_EXPORT static Standard_Boolean ConnectedEdges(const TopoDS_Wire&   aWire,
                                                         const TopoDS_Vertex& aVertex,
                                                         TopoDS_Edge&         Edge1,
                                                         TopoDS_Edge&         Edge2);

private:
  TopoDS_Wire myWire;
  gp_Pnt      myPntAttach;
};

#endif // _PrsDim_FixRelation_HeaderFile
