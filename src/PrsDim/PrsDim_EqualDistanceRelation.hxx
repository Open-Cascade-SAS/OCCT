// Created on: 1998-01-24
// Created by: Julia GERASIMOVA
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _PrsDim_EqualDistanceRelation_HeaderFile
#define _PrsDim_EqualDistanceRelation_HeaderFile

#include <PrsDim_Relation.hxx>
#include <PrsDim_TypeOfDist.hxx>
#include <DsgPrs_ArrowSide.hxx>

class Geom_Plane;

DEFINE_STANDARD_HANDLE(PrsDim_EqualDistanceRelation, PrsDim_Relation)

//! A framework to display equivalent distances between
//! shapes and a given plane.
//! The distance is the length of a projection from the
//! shape to the plane.
//! These distances are used to compare shapes by this vector alone.
class PrsDim_EqualDistanceRelation : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_EqualDistanceRelation, PrsDim_Relation)
public:
  //! Constructs a framework to display equivalent
  //! distances between the shapes aShape1, aShape2,
  //! aShape3, aShape4 and the plane aPlane.
  //! The distance is the length of a projection from the
  //! shape to the plane.
  Standard_EXPORT PrsDim_EqualDistanceRelation(const TopoDS_Shape&       aShape1,
                                               const TopoDS_Shape&       aShape2,
                                               const TopoDS_Shape&       aShape3,
                                               const TopoDS_Shape&       aShape4,
                                               const Handle(Geom_Plane)& aPlane);

  //! Sets the shape aShape to be used as the shape
  //! aShape3 in the framework created at construction time.
  void SetShape3(const TopoDS_Shape& aShape) { myShape3 = aShape; }

  //! Returns the shape aShape3 from the framework
  //! created at construction time.
  const TopoDS_Shape& Shape3() const { return myShape3; }

  //! Sets the shape aShape to be used as the shape
  //! aShape4 in the framework created at construction time.
  void SetShape4(const TopoDS_Shape& aShape) { myShape4 = aShape; }

  //! Returns the shape aShape4 from the framework
  //! created at construction time.
  const TopoDS_Shape& Shape4() const { return myShape4; }

public:
  //! Computes the location of an intreval between
  //! between two edges. FirstAttach , SecondAttach
  //! are the returned extreme points of the interval.
  Standard_EXPORT static void ComputeTwoEdgesLength(const Handle(Prs3d_Presentation)& aPresentation,
                                                    const Handle(Prs3d_Drawer)&       aDrawer,
                                                    const Standard_Real               ArrowSize,
                                                    const TopoDS_Edge&                FirstEdge,
                                                    const TopoDS_Edge&                SecondEdge,
                                                    const Handle(Geom_Plane)&         Plane,
                                                    const Standard_Boolean            AutomaticPos,
                                                    const Standard_Boolean            IsSetBndBox,
                                                    const Bnd_Box&                    BndBox,
                                                    gp_Pnt&                           Position,
                                                    gp_Pnt&                           FirstAttach,
                                                    gp_Pnt&                           SecondAttach,
                                                    gp_Pnt&                           FirstExtreme,
                                                    gp_Pnt&                           SecondExtreme,
                                                    DsgPrs_ArrowSide&                 SymbolPrs);

  //! Computes the interval position between two vertexs. FirstAttach,
  //! SecondAttach are the returned extreme points of the interval.
  Standard_EXPORT static void ComputeTwoVerticesLength(
    const Handle(Prs3d_Presentation)& aPresentation,
    const Handle(Prs3d_Drawer)&       aDrawer,
    const Standard_Real               ArrowSize,
    const TopoDS_Vertex&              FirstVertex,
    const TopoDS_Vertex&              SecondVertex,
    const Handle(Geom_Plane)&         Plane,
    const Standard_Boolean            AutomaticPos,
    const Standard_Boolean            IsSetBndBox,
    const Bnd_Box&                    BndBox,
    const PrsDim_TypeOfDist           TypeDist,
    gp_Pnt&                           Position,
    gp_Pnt&                           FirstAttach,
    gp_Pnt&                           SecondAttach,
    gp_Pnt&                           FirstExtreme,
    gp_Pnt&                           SecondExtreme,
    DsgPrs_ArrowSide&                 SymbolPrs);

  //! Compute the interval location between a vertex and an edge. Edge may be
  //! a line or a circle.
  Standard_EXPORT static void ComputeOneEdgeOneVertexLength(
    const Handle(Prs3d_Presentation)& aPresentation,
    const Handle(Prs3d_Drawer)&       aDrawer,
    const Standard_Real               ArrowSize,
    const TopoDS_Shape&               FirstShape,
    const TopoDS_Shape&               SecondShape,
    const Handle(Geom_Plane)&         Plane,
    const Standard_Boolean            AutomaticPos,
    const Standard_Boolean            IsSetBndBox,
    const Bnd_Box&                    BndBox,
    gp_Pnt&                           Position,
    gp_Pnt&                           FirstAttach,
    gp_Pnt&                           SecondAttach,
    gp_Pnt&                           FirstExtreme,
    gp_Pnt&                           SecondExtreme,
    DsgPrs_ArrowSide&                 SymbolPrs);

private:
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                       const Handle(Prs3d_Presentation)&         thePrs,
                                       const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& theSel,
                                                const Standard_Integer theMode) Standard_OVERRIDE;

private:
  TopoDS_Shape myShape3;
  TopoDS_Shape myShape4;
  gp_Pnt       myAttachPoint1;
  gp_Pnt       myAttachPoint2;
  gp_Pnt       myAttachPoint3;
  gp_Pnt       myAttachPoint4;
  gp_Pnt       myPoint1;
  gp_Pnt       myPoint2;
  gp_Pnt       myPoint3;
  gp_Pnt       myPoint4;
};

#endif // _PrsDim_EqualDistanceRelation_HeaderFile
