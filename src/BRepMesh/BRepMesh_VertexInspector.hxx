// File:        BRepMesh_VertexInspector.hxx
// Created:     Jun 1 18:13:23 2011
// Author:      Oleg AGASHIN
// Copyright:   Open CASCADE SAS 2011


#ifndef _BRepMesh_VertexInspector_HeaderFile
#define _BRepMesh_VertexInspector_HeaderFile

#include <BRepMesh_ListOfInteger.hxx> 
#ifndef _Precision_HeaderFile
#include <Precision.hxx>
#endif
#ifndef _gp_XY_HeaderFile
#include <gp_XY.hxx>
#endif
#ifndef _gp_XYZ_HeaderFile
#include <gp_XYZ.hxx>
#endif

#ifndef NCollection_CellFilter_HeaderFile
#include <NCollection_CellFilter.hxx>
#endif
#ifndef _BRepMesh_Vertex_HeaderFile
#include <BRepMesh_Vertex.hxx>
#endif
#ifndef _BRepMesh_VectorOfVertex_HeaderFile
#include <BRepMesh_VectorOfVertex.hxx>
#endif
#ifndef _TColStd_Array1OfReal_HeaderFile
#include <TColStd_Array1OfReal.hxx>
#endif
#include <BRepMesh_BaseAllocator.hxx>

//=======================================================================
//! The class to find in the coincidence points 
//=======================================================================

class BRepMesh_VertexInspector : public NCollection_CellFilter_InspectorXY
{
public:
  typedef Standard_Integer Target;
  //! Constructor; remembers tolerance and collector data structure.
  //! theTol can be Real or Array1OfReal with two elements which describe
  //! tolerance for each dimension.
  BRepMesh_VertexInspector (const Standard_Integer nbComp,
                            const BRepMesh_BaseAllocator& theAlloc);
                            
  BRepMesh_VertexInspector (const Standard_Integer nbComp,
                            const Standard_Real    theTol,
                            const BRepMesh_BaseAllocator& theAlloc);
                            
  BRepMesh_VertexInspector (const Standard_Integer nbComp,
                            const Standard_Real    aTolX,
                            const Standard_Real    aTolY,
                            const BRepMesh_BaseAllocator& theAlloc);

  Standard_Integer Add(const BRepMesh_Vertex& theVertex);
  
  void SetTolerance(const Standard_Real theTol)
  {
    myTol(0) = theTol*theTol;
    myTol(1) = 0.;
  }
  
  void SetTolerance(const Standard_Real theTolX, const Standard_Real theTolY)
  {
    myTol(0) = theTolX*theTolX;
    myTol(1) = theTolY*theTolY;
  }
  
  void Clear()
  {
    myVertices.Clear();
    myDelNodes.Clear();
  }

  void Delete(const Standard_Integer theIndex)
  {
    myVertices(theIndex-1).SetMovability(BRepMesh_Deleted);
    myDelNodes.Append(theIndex);
  }
  
  Standard_Integer GetNbVertices() const
  {
    return myVertices.Length(); 
  }

  BRepMesh_Vertex& GetVertex(Standard_Integer theInd)
  {
    return myVertices(theInd-1);
  }
  
  //! Set current node to be checked
  void SetCurrent (const gp_XY& theCurVertex, const Standard_Boolean theExactVal = Standard_False) 
  { 
    myResInd.Clear();
    myCurrent = theCurVertex;
  }

  //!Get result index of node
  const Standard_Integer GetCoincidentInd() const
  {
    if ( myResInd.Size() > 0 )
    {
      return myResInd.First();
    }
    return 0;
  }
  
  const BRepMesh_ListOfInteger& GetListOfDelNodes() const
  {
    return myDelNodes;
  }

  //! Implementation of inspection method
  NCollection_CellFilter_Action Inspect (const Standard_Integer theTarget); 

  static Standard_Boolean IsEqual (Standard_Integer theIdx, const Standard_Integer theTarget)
  {
    return (theIdx == theTarget);
  }

private:
  TColStd_Array1OfReal                 myTol;
  BRepMesh_ListOfInteger               myResInd;
  BRepMesh_VectorOfVertex              myVertices;
  BRepMesh_ListOfInteger               myDelNodes;
  gp_XY                                myCurrent;
};

#endif
