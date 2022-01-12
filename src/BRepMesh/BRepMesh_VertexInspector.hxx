// Created on: 2011-06-01
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_VertexInspector_HeaderFile
#define _BRepMesh_VertexInspector_HeaderFile

#include <Precision.hxx>
#include <gp_XY.hxx>
#include <IMeshData_Types.hxx>
#include <NCollection_CellFilter.hxx>
#include <BRepMesh_Vertex.hxx>

//! Class intended for fast searching of the coincidence points.
class BRepMesh_VertexInspector : public NCollection_CellFilter_InspectorXY
{
public:
  typedef Standard_Integer Target;

  //! Constructor.
  //! @param theAllocator memory allocator to be used by internal collections.
  BRepMesh_VertexInspector(
    const Handle(NCollection_IncAllocator)& theAllocator)
    : myIndex(0),
      myMinSqDist(RealLast()),
      myVertices(new IMeshData::VectorOfVertex),
      myDelNodes(theAllocator)
  {
    SetTolerance(Precision::Confusion());
  }

  //! Registers the given vertex.
  //! @param theVertex vertex to be registered.
  Standard_Integer Add(const BRepMesh_Vertex& theVertex)
  {
    if( myDelNodes.IsEmpty() )
    {
      myVertices->Append(theVertex);
      return myVertices->Length();
    }
    
    Standard_Integer aNodeIndex = myDelNodes.First();
    myVertices->ChangeValue(aNodeIndex - 1) = theVertex;
    myDelNodes.RemoveFirst();
    return aNodeIndex;
  }
  

  //! Sets the tolerance to be used for identification of 
  //! coincident vertices equal for both dimensions.
  void SetTolerance(const Standard_Real theTolerance)
  {
    myTolerance[0] = theTolerance * theTolerance;
    myTolerance[1] = 0.;
  }
  
  //! Sets the tolerance to be used for identification of 
  //! coincident vertices.
  //! @param theToleranceX tolerance for X dimension.
  //! @param theToleranceY tolerance for Y dimension.
  void SetTolerance(const Standard_Real theToleranceX,
                    const Standard_Real theToleranceY)
  {
    myTolerance[0] = theToleranceX * theToleranceX;
    myTolerance[1] = theToleranceY * theToleranceY;
  }
  
  //! Clear inspector's internal data structures.
  void Clear()
  {
    myVertices->Clear();
    myDelNodes.Clear();
  }

  //! Deletes vertex with the given index.
  //! @param theIndex index of vertex to be removed.
  void Delete(const Standard_Integer theIndex)
  {
    myVertices->ChangeValue(theIndex - 1).SetMovability(BRepMesh_Deleted);
    myDelNodes.Append(theIndex);
  }
  
  //! Returns number of registered vertices.
  Standard_Integer NbVertices() const
  {
    return myVertices->Length(); 
  }

  //! Returns vertex with the given index.
  BRepMesh_Vertex& GetVertex(Standard_Integer theIndex)
  {
    return myVertices->ChangeValue(theIndex - 1);
  }
  
  //! Set reference point to be checked.
  void SetPoint(const gp_XY& thePoint) 
  { 
    myIndex     = 0;
    myMinSqDist = RealLast();
    myPoint     = thePoint;
  }

  //! Returns index of point coinciding with regerence one.
  Standard_Integer GetCoincidentPoint() const
  {
    return myIndex;
  }
  
  //! Returns list with indexes of vertices that have movability attribute 
  //! equal to BRepMesh_Deleted and can be replaced with another node.
  const IMeshData::ListOfInteger& GetListOfDelPoints() const
  {
    return myDelNodes;
  }

  //! Returns set of mesh vertices.
  const Handle(IMeshData::VectorOfVertex)& Vertices() const
  {
    return myVertices;
  }

  //! Returns set of mesh vertices for modification.
  Handle(IMeshData::VectorOfVertex)& ChangeVertices()
  {
    return myVertices;
  }

  //! Performs inspection of a point with the given index.
  //! @param theTargetIndex index of a circle to be checked.
  //! @return status of the check.
  Standard_EXPORT NCollection_CellFilter_Action Inspect(const Standard_Integer theTargetIndex);

  //! Checks indices for equlity.
  static Standard_Boolean IsEqual(const Standard_Integer theIndex,
                                                  const Standard_Integer theTargetIndex)
  {
    return (theIndex == theTargetIndex);
  }

private:

  Standard_Integer                  myIndex;
  Standard_Real                     myMinSqDist;
  Standard_Real                     myTolerance[2];
  Handle(IMeshData::VectorOfVertex) myVertices;
  IMeshData::ListOfInteger          myDelNodes;
  gp_XY                             myPoint;
};

#endif
