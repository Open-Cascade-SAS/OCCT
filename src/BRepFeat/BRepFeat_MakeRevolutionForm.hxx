// Created on: 1997-10-14
// Created by: Olga KOULECHOVA
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

#ifndef _BRepFeat_MakeRevolutionForm_HeaderFile
#define _BRepFeat_MakeRevolutionForm_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax1.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepFeat_RibSlot.hxx>
#include <Standard_Integer.hxx>
class Geom_Plane;
class TopoDS_Shape;
class TopoDS_Wire;
class TopoDS_Edge;
class TopoDS_Face;
class gp_Pnt;


//! MakeRevolutionForm Generates a surface of
//! revolution in the feature as it slides along a
//! revolved face in the basis shape.
//! The semantics of mechanical features is built
//! around giving thickness to a contour. This
//! thickness can either be unilateral - on one side
//! of the contour - or bilateral - on both sides. As
//! in the semantics of form features, the thickness
//! is defined by construction of shapes in specific contexts.
//! The development contexts differ, however,in
//! case of mechanical features. Here they include extrusion:
//! -   to a limiting face of the basis shape
//! -   to or from a limiting plane
//! -   to a height.
class BRepFeat_MakeRevolutionForm  : public BRepFeat_RibSlot
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! initializes the linear form class.
    BRepFeat_MakeRevolutionForm();
  
  //! a contour W, a shape Sbase and a plane P are initialized to serve as
  //! the basic elements in the construction of the rib or groove. The axis Axis of the
  //! revolved surface in the basis shape defines the feature's axis of revolution.
  //! Height1 and Height2 may be used as limits to the construction of the feature.
  //! Fuse offers a choice between:
  //! -   removing matter with a Boolean cut using the setting 0 in case of the groove
  //! -   adding matter with Boolean fusion using the setting 1 in case of the rib.
    BRepFeat_MakeRevolutionForm(const TopoDS_Shape& Sbase, const TopoDS_Wire& W, const Handle(Geom_Plane)& Plane, const gp_Ax1& Axis, const Standard_Real Height1, const Standard_Real Height2, const Standard_Integer Fuse, Standard_Boolean& Sliding);
  
  //! Initializes this construction algorithm
  //! A contour W, a shape Sbase and a plane P are initialized to serve as the basic elements
  //! in the construction of the rib or groove. The axis Axis of the revolved surface in the basis
  //! shape defines the feature's axis of revolution. Height1 and Height2 may be
  //! used as limits to the construction of the feature.
  //! Fuse offers a choice between:
  //! -   removing matter with a Boolean cut using the setting 0 in case of the groove
  //! -   adding matter with Boolean fusion using the setting 1 in case of the rib.
  Standard_EXPORT void Init (const TopoDS_Shape& Sbase, const TopoDS_Wire& W, const Handle(Geom_Plane)& Plane, const gp_Ax1& Axis, const Standard_Real Height1, const Standard_Real Height2, const Standard_Integer Fuse, Standard_Boolean& Sliding);
  
  //! Indicates that the edge <E> will slide on the face
  //! <OnFace>. Raises ConstructionError  if the  face does not belong to the
  //! basis shape, or the edge to the prismed shape.
  Standard_EXPORT void Add (const TopoDS_Edge& E, const TopoDS_Face& OnFace);
  
  //! Performs a prism from the wire to the plane
  //! along the basis shape S. Reconstructs the feature topologically.
  Standard_EXPORT void Perform();
  
  Standard_EXPORT Standard_Boolean Propagate (TopTools_ListOfShape& L, const TopoDS_Face& F, const gp_Pnt& FPoint, const gp_Pnt& LPoint, Standard_Boolean& falseside);




protected:





private:



  gp_Ax1 myAxe;
  Standard_Real myHeight1;
  Standard_Real myHeight2;
  Standard_Boolean mySliding;
  Handle(Geom_Plane) myPln;
  Standard_Real myBnd;
  TopTools_DataMapOfShapeListOfShape mySlface;
  TopTools_ListOfShape myListOfEdges;
  Standard_Real myTol;
  Standard_Real myAngle1;
  Standard_Real myAngle2;


};


#include <BRepFeat_MakeRevolutionForm.lxx>





#endif // _BRepFeat_MakeRevolutionForm_HeaderFile
