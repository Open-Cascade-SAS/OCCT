// Created on: 1997-04-14
// Created by: Olga PILLOT
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _BRepFeat_MakeLinearForm_HeaderFile
#define _BRepFeat_MakeLinearForm_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Vec.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepFeat_RibSlot.hxx>
#include <Standard_Integer.hxx>
class Geom_Curve;
class Geom_Plane;
class TopoDS_Shape;
class TopoDS_Wire;
class TopoDS_Edge;
class TopoDS_Face;
class gp_Pnt;

//! Builds a rib or a groove along a developable, planar surface.
//! The semantics of mechanical features is built around
//! giving thickness to a contour. This thickness can either
//! be symmetrical - on one side of the contour - or
//! dissymmetrical - on both sides. As in the semantics of
//! form features, the thickness is defined by construction of
//! shapes in specific contexts.
//! The development contexts differ, however, in case of
//! mechanical features. Here they include extrusion:
//! -   to a limiting face of the basis shape
//! -   to or from a limiting plane
//! -   to a height.
class BRepFeat_MakeLinearForm : public BRepFeat_RibSlot
{
public:
  DEFINE_STANDARD_ALLOC

  //! initializes the linear form class
  BRepFeat_MakeLinearForm();

  //! contour W, a shape Sbase and a
  //! plane P are initialized to serve as the basic
  //! elements in the construction of the rib or groove.
  //! Direction and Direction1 give The vectors for
  //! defining the direction(s) in which thickness will be built up.
  //! Fuse offers a choice between:
  //! -   removing matter with a Boolean cut using the
  //! setting 0 in case of the groove
  //! -   adding matter with Boolean fusion using the
  //! setting 1 in case of the rib.
  BRepFeat_MakeLinearForm(const TopoDS_Shape&       Sbase,
                          const TopoDS_Wire&        W,
                          const Handle(Geom_Plane)& P,
                          const gp_Vec&             Direction,
                          const gp_Vec&             Direction1,
                          const Standard_Integer    Fuse,
                          const Standard_Boolean    Modify);

  //! Initializes this construction algorithm.
  //! A contour W, a shape Sbase and a plane P are
  //! initialized to serve as the basic elements in the
  //! construction of the rib or groove. The vectors for
  //! defining the direction(s) in which thickness will be built
  //! up are given by Direction and Direction1.
  //! Fuse offers a choice between:
  //! -   removing matter with a Boolean cut using the setting
  //! 0 in case of the groove
  //! -   adding matter with Boolean fusion using the setting 1
  //! in case of the rib.
  Standard_EXPORT void Init(const TopoDS_Shape&       Sbase,
                            const TopoDS_Wire&        W,
                            const Handle(Geom_Plane)& P,
                            const gp_Vec&             Direction,
                            const gp_Vec&             Direction1,
                            const Standard_Integer    Fuse,
                            const Standard_Boolean    Modify);

  //! Indicates that the edge <E> will slide on the face
  //! <OnFace>.
  //! Raises ConstructionError if the  face does not belong to the
  //! basis shape, or the edge to the prismed shape.
  Standard_EXPORT void Add(const TopoDS_Edge& E, const TopoDS_Face& OnFace);

  //! Performs a prism from the wire to the plane along the
  //! basis shape Sbase. Reconstructs the feature topologically.
  Standard_EXPORT void Perform();

  //! Limits construction of the linear form feature by using
  //! one of the following three semantics:
  //! -   from a limiting plane
  //! -   to a limiting plane
  //! -   from one limiting plane to another.
  //! The setting is provided by a flag, flag, which can be set
  //! to from and/or until. The third semantic possibility above
  //! is selected by showing both from and until at the same time.
  Standard_EXPORT void TransformShapeFU(const Standard_Integer flag);

  Standard_EXPORT Standard_Boolean Propagate(TopTools_ListOfShape& L,
                                             const TopoDS_Face&    F,
                                             const gp_Pnt&         FPoint,
                                             const gp_Pnt&         LPoint,
                                             Standard_Boolean&     falseside);

protected:
private:
  Handle(Geom_Curve)                 myCrv;
  gp_Vec                             myDir;
  gp_Vec                             myDir1;
  Handle(Geom_Plane)                 myPln;
  Standard_Real                      myBnd;
  TopTools_DataMapOfShapeListOfShape mySlface;
  TopTools_ListOfShape               myListOfEdges;
  Standard_Real                      myTol;
};

#include <BRepFeat_MakeLinearForm.lxx>

#endif // _BRepFeat_MakeLinearForm_HeaderFile
