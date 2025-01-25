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

#ifndef _IMeshData_Model_HeaderFile
#define _IMeshData_Model_HeaderFile

#include <IMeshData_Shape.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>
#include <IMeshData_Types.hxx>

class TopoDS_Face;
class TopoDS_Edge;

//! Interface class representing discrete model of a shape.
class IMeshData_Model : public IMeshData_Shape
{
public:
  //! Destructor.
  virtual ~IMeshData_Model() {}

  //! Returns maximum size of shape model.
  Standard_EXPORT virtual Standard_Real GetMaxSize() const = 0;

  DEFINE_STANDARD_RTTIEXT(IMeshData_Model, IMeshData_Shape)

public: //! @name discrete faces
  //! Returns number of faces in discrete model.
  Standard_EXPORT virtual Standard_Integer FacesNb() const = 0;

  //! Adds new face to shape model.
  Standard_EXPORT virtual const IMeshData::IFaceHandle& AddFace(const TopoDS_Face& theFace) = 0;

  //! Gets model's face with the given index.
  Standard_EXPORT virtual const IMeshData::IFaceHandle& GetFace(
    const Standard_Integer theIndex) const = 0;

public: //! @name discrete edges
  //! Returns number of edges in discrete model.
  Standard_EXPORT virtual Standard_Integer EdgesNb() const = 0;

  //! Adds new edge to shape model.
  Standard_EXPORT virtual const IMeshData::IEdgeHandle& AddEdge(const TopoDS_Edge& theEdge) = 0;

  //! Gets model's edge with the given index.
  Standard_EXPORT virtual const IMeshData::IEdgeHandle& GetEdge(
    const Standard_Integer theIndex) const = 0;

protected:
  //! Constructor.
  //! Initializes empty model.
  IMeshData_Model(const TopoDS_Shape& theShape)
      : IMeshData_Shape(theShape)
  {
  }
};

#endif