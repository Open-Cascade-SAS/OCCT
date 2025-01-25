// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _BRepMeshData_Model_HeaderFile
#define _BRepMeshData_Model_HeaderFile

#include <IMeshData_Model.hxx>
#include <IMeshData_Types.hxx>
#include <NCollection_IncAllocator.hxx>
#include <IMeshData_Edge.hxx>

//! Default implementation of model entity.
class BRepMeshData_Model : public IMeshData_Model
{
public:
  //! Constructor.
  //! Initializes empty model.
  Standard_EXPORT BRepMeshData_Model(const TopoDS_Shape& theShape);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMeshData_Model();

  //! Returns maximum size of shape's bounding box.
  virtual Standard_Real GetMaxSize() const Standard_OVERRIDE { return myMaxSize; }

  //! Sets maximum size of shape's bounding box.
  void SetMaxSize(const Standard_Real theValue) { myMaxSize = theValue; }

  DEFINE_STANDARD_RTTIEXT(BRepMeshData_Model, IMeshData_Model)

public: //! @name discrete faces
  //! Returns number of faces in discrete model.
  Standard_EXPORT virtual Standard_Integer FacesNb() const Standard_OVERRIDE;

  //! Adds new face to shape model.
  Standard_EXPORT virtual const IMeshData::IFaceHandle& AddFace(const TopoDS_Face& theFace)
    Standard_OVERRIDE;

  //! Gets model's face with the given index.
  Standard_EXPORT virtual const IMeshData::IFaceHandle& GetFace(
    const Standard_Integer theIndex) const Standard_OVERRIDE;

public: //! @name discrete edges
  //! Returns number of edges in discrete model.
  Standard_EXPORT virtual Standard_Integer EdgesNb() const Standard_OVERRIDE;

  //! Adds new edge to shape model.
  Standard_EXPORT virtual const IMeshData::IEdgeHandle& AddEdge(const TopoDS_Edge& theEdge)
    Standard_OVERRIDE;

  //! Gets model's edge with the given index.
  Standard_EXPORT virtual const IMeshData::IEdgeHandle& GetEdge(
    const Standard_Integer theIndex) const Standard_OVERRIDE;

private:
  Standard_Real                    myMaxSize;
  Handle(NCollection_IncAllocator) myAllocator;
  IMeshData::VectorOfIFaceHandles  myDFaces;
  IMeshData::VectorOfIEdgeHandles  myDEdges;
};

#endif