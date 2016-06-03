// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BRepMesh_VertexTool_HeaderFile
#define _BRepMesh_VertexTool_HeaderFile

#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <BRepMesh_VertexInspector.hxx>
#include <BRepMesh.hxx>
#include <Standard_OStream.hxx>
#include <gp_XYZ.hxx>
#include <gp_XY.hxx>

class BRepMesh_Vertex;

//! Describes data structure intended to keep mesh nodes 
//! defined in UV space and implements functionality 
//! providing their uniqueness regarding thir position.
class BRepMesh_VertexTool
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! Constructor.
  //! @param theAllocator memory allocator to be used by internal collections.
  Standard_EXPORT BRepMesh_VertexTool(
    const Handle(NCollection_IncAllocator)& theAllocator);

  //! Sets new size of cell for cellfilter equal in both directions.
  void SetCellSize(const Standard_Real theSize)
  {
    myCellFilter.Reset(theSize, myAllocator);
    mySelector.Clear();
  }

  //! Sets new size of cell for cellfilter.
  //! @param theSizeX size for X dimension.
  //! @param theSizeY size for Y dimension.
  void SetCellSize(const Standard_Real theSizeX,
                   const Standard_Real theSizeY)
  {
    Standard_Real aCellSizeC[2] = { theSizeX, theSizeY };
    NCollection_Array1<Standard_Real> aCellSize(aCellSizeC[0], 1, 2);
    myCellFilter.Reset(aCellSize, myAllocator);
    mySelector.Clear();
  }

  //! Sets the tolerance to be used for identification of 
  //! coincident vertices equal for both dimensions.
  void SetTolerance(const Standard_Real theTolerance)
  {
    mySelector.SetTolerance( theTolerance );
    myTolerance[0] = theTolerance;
    myTolerance[1] = theTolerance;
  }

  //! Sets the tolerance to be used for identification of 
  //! coincident vertices.
  //! @param theToleranceX tolerance for X dimension.
  //! @param theToleranceY tolerance for Y dimension.
  void SetTolerance(const Standard_Real theToleranceX,
                    const Standard_Real theToleranceY)
  {
    mySelector.SetTolerance( theToleranceX, theToleranceY );
    myTolerance[0] = theToleranceX;
    myTolerance[1] = theToleranceY;
  }

  //! Gets the tolerance to be used for identification of 
  //! coincident vertices.
  //! @param theToleranceX tolerance for X dimension.
  //! @param theToleranceY tolerance for Y dimension.
  void GetTolerance(Standard_Real& theToleranceX,
                    Standard_Real& theToleranceY)
  {
    theToleranceX = myTolerance[0];
    theToleranceY = myTolerance[1];
  }

  //! Adds vertex with empty data to the tool.
  //! @param theVertex node to be added to the mesh.
  //! @param isForceAdd adds the given node to structure without 
  //! checking on coincidence with other nodes.
  //! @return index of the node in the structure.
  Standard_EXPORT Standard_Integer Add(
    const BRepMesh_Vertex& theVertex,
    const Standard_Boolean isForceAdd);

  //! Deletes vertex with the given index from the tool.
  Standard_EXPORT void Delete(const Standard_Integer theIndex);

  //! Returns set of mesh vertices.
  inline const BRepMesh::HVectorOfVertex& Vertices() const
  {
    return mySelector.Vertices();
  }

  //! Returns set of mesh vertices.
  inline BRepMesh::HVectorOfVertex& ChangeVertices()
  {
    return mySelector.ChangeVertices();
  }

  //! Returns vertex by the given index.
  inline const BRepMesh_Vertex& FindKey(const Standard_Integer theIndex)
  {
    return mySelector.GetVertex(theIndex);
  }

  //! Returns index of the given vertex.
  Standard_Integer FindIndex(const BRepMesh_Vertex& theVertex)
  {
    mySelector.SetPoint(theVertex.Coord());
    myCellFilter.Inspect (theVertex.Coord(), mySelector);
    return mySelector.GetCoincidentPoint();
  }

  //! Returns a number of vertices.
  inline Standard_Integer Extent() const
  {
    return mySelector.NbVertices();
  }

  //! Returns True when the map contains no keys. <br>
  inline Standard_Boolean IsEmpty() const
  {
    return (Extent() == 0);
  }

  //! Substitutes vertex with the given by the given vertex with attributes.
  //! @param theIndex index of vertex to be substituted.
  //! @param theVertex replacement vertex.
  Standard_EXPORT void Substitute(const Standard_Integer theIndex,
                                  const BRepMesh_Vertex& theVertex);

  //! Remove last node from the structure.
  inline void RemoveLast()
  {
    Delete(Extent());
  }

  //! Returns the list with indexes of vertices that have movability attribute
  //! equal to BRepMesh_Deleted and can be replaced with another node.
  inline const BRepMesh::ListOfInteger& GetListOfDelNodes() const
  {
    return mySelector.GetListOfDelPoints();
  }

  //! Prints statistics.
  Standard_EXPORT void Statistics(Standard_OStream& theStream) const;

private:
  
  //! Expands the given point according to specified tolerance.
  //! @param thePoint point to be expanded.
  //! @param[out] theMinPoint bottom left corner of area defined by expanded point.
  //! @param[out] theMaxPoint top right corner of area defined by expanded point.
  inline void expandPoint(const gp_XY& thePoint,
                          gp_XY&       theMinPoint,
                          gp_XY&       theMaxPoint)
  {
    theMinPoint.SetX(thePoint.X() - myTolerance[0]);
    theMinPoint.SetY(thePoint.Y() - myTolerance[1]);
    theMaxPoint.SetX(thePoint.X() + myTolerance[0]);
    theMaxPoint.SetY(thePoint.Y() + myTolerance[1]);
  }

private:

  Handle(NCollection_IncAllocator)      myAllocator;
  BRepMesh::VertexCellFilter            myCellFilter;
  BRepMesh_VertexInspector              mySelector;
  Standard_Real                         myTolerance[2];
};

#endif
