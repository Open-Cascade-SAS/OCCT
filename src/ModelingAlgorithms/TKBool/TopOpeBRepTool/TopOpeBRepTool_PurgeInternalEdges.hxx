// Created on: 1998-11-19
// Created by: Jean-Michel BOULCOURT
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

#ifndef _TopOpeBRepTool_PurgeInternalEdges_HeaderFile
#define _TopOpeBRepTool_PurgeInternalEdges_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Standard_Integer.hxx>

//! remove from a shape, the internal edges that are
//! not connected to any face in the shape. We can
//! get the list of the edges as a
//! DataMapOfShapeListOfShape with a Face of the Shape
//! as the key and a list of internal edges as the
//! value. The list of internal edges means edges
//! that are not connected to any face in the shape.
//!
//! Example of use:
//! NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> mymap;
//! TopOpeBRepTool_PurgeInternalEdges
//! mypurgealgo(mysolid); mypurgealgo.GetFaces(mymap);
class TopOpeBRepTool_PurgeInternalEdges
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initialize members and begin exploration of shape
  //! depending of the value of PerformNow
  Standard_EXPORT TopOpeBRepTool_PurgeInternalEdges(
    const TopoDS_Shape&    theShape,
    const bool PerformNow = true);

  //! returns the list internal edges associated with
  //! the faces of the myShape. If PerformNow was False
  //! when created, then call the private Perform method
  //! that do the main job.
  Standard_EXPORT void Faces(NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theMapFacLstEdg);

  //! returns myShape modified with the list of internal
  //! edges removed from it.
  Standard_EXPORT TopoDS_Shape& Shape();

  //! returns the number of edges candidate to be removed
  Standard_EXPORT int NbEdges() const;

  //! returns False if the list of internal edges has
  //! not been extracted
  bool IsDone() const { return myIsDone; }

  //! Using the list of internal edges from each face,
  //! rebuild myShape by removing those edges.
  Standard_EXPORT void Perform();

protected:
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myMapEdgLstFac;

private:
  //! Do the main job. Explore all the edges of myShape and
  //! build a map with faces as a key and list of internal
  //! edges(without connected faces) as value.
  Standard_EXPORT void BuildList();

private:
  TopoDS_Shape                       myShape;
  bool                   myIsDone;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myMapFacLstEdg;
};

#endif // _TopOpeBRepTool_PurgeInternalEdges_HeaderFile
