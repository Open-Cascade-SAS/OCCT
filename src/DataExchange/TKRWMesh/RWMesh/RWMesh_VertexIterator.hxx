// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _RWMesh_VertexIterator_HeaderFile
#define _RWMesh_VertexIterator_HeaderFile

#include <NCollection_DataMap.hxx>
#include <Poly_Polygon3D.hxx>
#include <RWMesh_ShapeIterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopoDS_Vertex.hxx>
#include <XCAFPrs_Style.hxx>
#include <gp_Trsf.hxx>

#include <algorithm>

class TDF_Label;

//! Auxiliary class to iterate through vertices.
//! Provides functionality to iterate through the vertices of a shape.
//! It inherits from `RWMesh_ShapeIterator` and implements
//! methods to access and manipulate vertex data.
class RWMesh_VertexIterator : public RWMesh_ShapeIterator
{
public:
  //! Main constructor.
  //! @param[in] theLabel The label of the shape.
  //! @param[in] theLocation The location of the shape.
  //! @param[in] theToMapColors Flag to indicate if colors should be mapped.
  //! @param[in] theStyle The style of the shape.
  Standard_EXPORT RWMesh_VertexIterator(const TDF_Label&       theLabel,
                                        const TopLoc_Location& theLocation,
                                        const Standard_Boolean theToMapColors = false,
                                        const XCAFPrs_Style&   theStyle       = XCAFPrs_Style());

  //! Auxiliary constructor.
  //! @param[in] theShape The shape to iterate.
  //! @param[in] theStyle The style of the shape.
  Standard_EXPORT RWMesh_VertexIterator(const TopoDS_Shape&  theShape,
                                        const XCAFPrs_Style& theStyle = XCAFPrs_Style());

  //! Return true if iterator points to the valid triangulation.
  bool More() const Standard_OVERRIDE { return !myVertex.IsNull(); }

  //! Find next value.
  Standard_EXPORT void Next() Standard_OVERRIDE;

  //! Return current edge.
  const TopoDS_Vertex& Vertex() const { return myVertex; }

  //! Return current vertex.
  const TopoDS_Shape& Shape() const Standard_OVERRIDE { return myVertex; }

  //! Return current vertex data.
  const gp_Pnt& Point() const { return myPoint; }

  //! Return true if geometry data is defined.
  bool IsEmpty() const Standard_OVERRIDE { return myVertex.IsNull(); }

public:
  //! Lower element index in current triangulation.
  Standard_Integer ElemLower() const Standard_OVERRIDE { return 1; }

  //! Upper element index in current triangulation.
  Standard_Integer ElemUpper() const Standard_OVERRIDE { return 1; }

public:
  //! Return number of nodes for the current edge.
  Standard_Integer NbNodes() const Standard_OVERRIDE { return 1; }

  //! Lower node index in current triangulation.
  Standard_Integer NodeLower() const Standard_OVERRIDE { return 1; }

  //! Upper node index in current triangulation.
  Standard_Integer NodeUpper() const Standard_OVERRIDE { return 1; }

public:
  //! Return the node with specified index with applied transformation.
  gp_Pnt node(const Standard_Integer /*theNode*/) const Standard_OVERRIDE { return myPoint; }

private:
  //! Reset information for current vertex.
  void resetVertex()
  {
    myVertex.Nullify();
    resetShape();
  }

  //! Initialize vertex properties.
  void initVertex();

private:
  TopoDS_Vertex myVertex; //!< current vertex
  gp_Pnt        myPoint;  //!< geometry of current vertex
};

#endif // _RWMesh_VertexIterator_HeaderFile
