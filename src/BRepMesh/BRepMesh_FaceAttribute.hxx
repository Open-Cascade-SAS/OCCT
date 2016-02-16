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

#ifndef _BRepMesh_FaceAttribute_HeaderFile
#define _BRepMesh_FaceAttribute_HeaderFile

#include <Standard.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

#include <BRepMesh_Status.hxx>
#include <BRepMesh.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>

class BRepAdaptor_HSurface;

//! Auxiliary class for FastDiscret and FastDiscretFace classes.
class BRepMesh_FaceAttribute : public Standard_Transient
{
public:

  //! Constructor. Initializes empty attribute.
  //! @param theBoundaryVertices shared map of shape vertices.
  //! @param theBoundaryPoints shared discretization points of shape boundaries.
  Standard_EXPORT BRepMesh_FaceAttribute(
    const BRepMesh::HDMapOfVertexInteger& theBoundaryVertices,
    const BRepMesh::HDMapOfIntegerPnt&    theBoundaryPoints);

  //! Constructor.
  //! @param theFace face the attribute is created for. 
  //! Used for default initialization. Attribute keeps reference 
  //! to the source face with forward orientation.
  //! @param theBoundaryVertices shared map of shape vertices.
  //! @param theBoundaryPoints shared discretization points of shape boundaries.
  //! @param theAdaptiveMin switches on adaptive computation of minimal parametric
  //! tolerance (if true).
  Standard_EXPORT BRepMesh_FaceAttribute(
    const TopoDS_Face&                    theFace,
    const BRepMesh::HDMapOfVertexInteger& theBoundaryVertices,
    const BRepMesh::HDMapOfIntegerPnt&    theBoundaryPoints,
    const Standard_Boolean                theAdaptiveMin);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_FaceAttribute();

public: //! @name main geometrical properties.

  //! Returns face's surface.
  inline const Handle(BRepAdaptor_HSurface)& Surface() const
  {
    return mySurface;
  }

  //! Returns True in case if this attribute has already been intialized.
  inline Standard_Boolean IsInitialized () const
  {
    return !myFace.IsNull ();
  }

  //! Initializes this attribute by the given face.
  Standard_EXPORT void SetFace (
    const TopoDS_Face&     theFace, 
    const Standard_Boolean theAdaptiveMin);

  //! Returns forward oriented face to be used for calculations.
  inline const TopoDS_Face& Face() const
  {
    return myFace;
  }

  //! Sets boundary vertices map.
  inline void SetBoundaryVertices(const BRepMesh::HDMapOfVertexInteger& theVertices)
  {
    myBoundaryVertices = theVertices;
  }

  //! Sets boundary points map.
  inline void SetBoundaryPoints(const BRepMesh::HDMapOfIntegerPnt& theBoundaryPoints)
  {
    myBoundaryPoints = theBoundaryPoints;
  }

  //! Returns U tolerance of face calculated regarding its parameters.
  Standard_EXPORT Standard_Real ToleranceU() const;
  
  //! Returns V tolerance of face calculated regarding its parameters.
  Standard_EXPORT Standard_Real ToleranceV() const;
  
  //! Gives face deflection parameter.
  inline Standard_Real GetDefFace() const
  {
    return myDefFace;
  }

  //! Sets face deflection.
  inline void SetDefFace(const Standard_Real theDefFace)
  {
    myDefFace = theDefFace;
  }

  //! Gives minimal value in U domain.
  inline Standard_Real GetUMin() const
  {
    return myUMin;
  }

  //! Sets minimal value in U domain.
  inline void SetUMin(const Standard_Real theUMin)
  {
    myUMin = theUMin;
  }

  //! Gives minimal value in V domain.
  inline Standard_Real GetVMin() const
  {
    return myVMin;
  }

  //! Sets minimal value in V domain.
  inline void SetVMin(const Standard_Real theVMin)
  {
    myVMin = theVMin;
  }

  //! Gives maximal value in U domain.
  inline Standard_Real GetUMax() const
  {
    return myUMax;
  }

  //! Sets maximal value in U domain.
  inline void SetUMax(const Standard_Real theUMax)
  {
    myUMax = theUMax;
  }

  //! Gives maximal value in V domain.
  inline Standard_Real GetVMax() const
  {
    return myVMax;
  }

  //! Sets maximal value in V domain.
  inline void SetVMax(const Standard_Real theVMax)
  {
    myVMax = theVMax;
  }

  //! Gives value of step in U domain.
  inline Standard_Real GetDeltaX() const
  {
    return myDeltaX;
  }

  //! Sets value of step in U domain.
  inline void SetDeltaX(const Standard_Real theDeltaX)
  {
    myDeltaX = theDeltaX;
  }

  //! Gives value of step in V domain.
  inline Standard_Real GetDeltaY() const
  {
    return myDeltaY;
  }

  //! Sets value of step in V domain.
  inline void SetDeltaY(const Standard_Real theDeltaY)
  {
    myDeltaY = theDeltaY;
  }

  //! Sets set of status flags for this face.
  inline Standard_Integer GetStatus() const
  {
    return myStatus;
  }

  //! Sets status flag for this face.
  inline void SetStatus(const BRepMesh_Status theStatus)
  {
    myStatus |= theStatus;
  }

  //! Returns TRUE in case if computed data is valid.
  inline Standard_Boolean IsValid() const
  {
    return (myStatus == BRepMesh_NoError || myStatus == BRepMesh_ReMesh);
  }

public: //! @name auxiliary structures

  //! Clear face attribute.
  Standard_EXPORT void Clear();

  //! Gives reference to map of internal edges of face.
  inline BRepMesh::HDMapOfShapePairOfPolygon& ChangeInternalEdges()
  {
    return myInternalEdges;
  }

  //! Gives reference to map of 2D points of discretization.
  inline BRepMesh::HDMapOfIntegerListOfXY& ChangeLocation2D()
  {
    return myLocation2D;
  }

  //! Gives reference to map of 3D points of discretization.
  inline BRepMesh::HDMapOfIntegerPnt& ChangeSurfacePoints()
  {
    return mySurfacePoints;
  }

  //! Gives reference to map of vertices of discretization.
  inline BRepMesh::HDMapOfVertexInteger& ChangeSurfaceVertices()
  {
    return mySurfaceVertices;
  }

  //! Gives reference on map of (vertex, edge) pairs of face.
  inline BRepMesh::HIMapOfInteger& ChangeVertexEdgeMap()
  {
    return myVertexEdgeMap;
  }

  //! Gives Delaunay data structure.
  inline Handle(BRepMesh_DataStructureOfDelaun)& ChangeStructure()
  {
    return myStructure;
  }

  //! Returns classifier.
  inline BRepMesh::HClassifier& ChangeClassifier()
  {
    return myClassifier;
  }

  //! Returns mesh nodes calculated for boundaries.
  inline BRepMesh::HVectorOfVertex& ChangeMeshNodes()
  {
    return myMeshNodes;
  }

public: //! @name Point/Vertex/Node manipulators

  //! Gives the number of different locations in 3D space.
  inline Standard_Integer LastPointId() const
  {
    return (myBoundaryPoints.IsNull() ? 0 : myBoundaryPoints->Extent()) + 
      (mySurfacePoints.IsNull() ? 0 : mySurfacePoints->Extent());
  }

  //! Gives the 3D location of the vertex.
  inline const gp_Pnt& GetPoint(const BRepMesh_Vertex& theVertex) const
  {
    return GetPoint(theVertex.Location3d());
  }

  //! Gives the 3D location of the vertex.
  inline const gp_Pnt& GetPoint(const Standard_Integer theIndex) const
  {
    if (!mySurfacePoints.IsNull() && theIndex > myBoundaryPoints->Extent())
      return mySurfacePoints->Find(theIndex);

    return myBoundaryPoints->Find(theIndex);
  }

  //! Returns index of the given vertex if it exists in cache, 
  //! elsewhere adds it to cache and returns cached index.
  //! @param theVertexExplorer template parameter intended to transfer
  //! parameters of vertex to method. Explorer class can implement different
  //! approaches of extraction of target parameters.
  //! @param isFillEdgeVertices if TRUE adds vertex to shared map of 
  //! edges vertices, elsewhere adds it map of face vertices.
  template<class HVertexExplorer>
    Standard_Integer GetVertexIndex(
      const HVertexExplorer& theVertexExplorer,
      const Standard_Boolean isFillEdgeVertices = Standard_False)
  {
    const TopoDS_Vertex& aVertex = theVertexExplorer->Vertex();
    Standard_Integer aNewVertexIndex = 0;
    if (getVertexIndex(aVertex, aNewVertexIndex))
      return aNewVertexIndex;

    if (!theVertexExplorer->IsSameUV() ||
        !getVertexIndex(theVertexExplorer->SameVertex(), aNewVertexIndex))
    {
      aNewVertexIndex = LastPointId() + 1;

      BRepMesh::DMapOfIntegerPnt& aPointsMap = isFillEdgeVertices ?
        *myBoundaryPoints : *mySurfacePoints;

      aPointsMap.Bind(aNewVertexIndex, theVertexExplorer->Point());
    }

    BRepMesh::DMapOfVertexInteger& aVertexMap = isFillEdgeVertices ?
      *myBoundaryVertices : *mySurfaceVertices;

    aVertexMap.Bind(aVertex, aNewVertexIndex);

    return aNewVertexIndex;
  }

  //! Adds node with the given parameters to mesh.
  //! @param theIndex index of 3D point corresponded to the node.
  //! @param theUV node position.
  //! @param theMovability movability of a node.
  //! @param theNodeIndex index of vertex in mesh structure.
  //! @param theNodeOnEdgeIndex ordered index of node on the boundary.
  Standard_EXPORT void AddNode(const Standard_Integer         theIndex,
                               const gp_XY&                   theUV,
                               const BRepMesh_DegreeOfFreedom theMovability,
                               Standard_Integer&              theNodeIndex,
                               Standard_Integer&              theNodeOnEdgeIndex);

public: //! @name Auxiliary methods
  
  //! Scales the given point from real parametric space 
  //! to face basis and otherwise.
  //! @param thePoint2d point to be scaled.
  //! @param isToFaceBasis if TRUE converts point to face basis,
  //! otherwise performs reverse conversion.
  //! @return scaled point.
  Standard_EXPORT gp_XY Scale(const gp_XY&           thePoint2d, 
                              const Standard_Boolean isToFaceBasis);

  DEFINE_STANDARD_RTTIEXT(BRepMesh_FaceAttribute,Standard_Transient)

private:

  //! Default constructor.
  BRepMesh_FaceAttribute();

  //! Assignment operator.
  void operator =(const BRepMesh_FaceAttribute& /*theOther*/)
  {
  }

  //! Initializes internal data structures.
  void init();

  //! Computes parametric tolerance of a face regarding the given limits.
  Standard_Real computeParametricTolerance(
    const Standard_Real theFirstParam,
    const Standard_Real theLastParam) const;

  //! Clears internal data structures local to face.
  void clearLocal(
    const Standard_Boolean isClearSurfaceDataOnly = Standard_False);

  //! Returns index of the given vertex if it exists in cache.
  //! @param theVertex vertex which index should be retrieved.
  //! @param theVertexIndex index of the given vertex.
  //! @return TRUE if cached value is found, FALSE elsewhere.
  Standard_EXPORT Standard_Boolean getVertexIndex(
    const TopoDS_Vertex& theVertex,
    Standard_Integer&    theVertexIndex) const;

private:

  Standard_Real                           myDefFace;       //!< Restore face deflection
  Standard_Real                           myUMin;          //!< Describes minimal value in U domain
  Standard_Real                           myUMax;          //!< Describes maximal value in U domain
  Standard_Real                           myVMin;          //!< Describes minimal value in V domain
  Standard_Real                           myVMax;          //!< Describes maximal value in V domain
  Standard_Real                           myDeltaX;
  Standard_Real                           myDeltaY;
  Standard_Real                           myMinStep;
  Standard_Integer                        myStatus;
  Standard_Boolean                        myAdaptiveMin;

  BRepMesh::HDMapOfVertexInteger          myBoundaryVertices;
  BRepMesh::HDMapOfIntegerPnt             myBoundaryPoints;

  TopoDS_Face                             myFace;
  Handle(BRepAdaptor_HSurface)            mySurface;
  BRepMesh::HClassifier                   myClassifier;

  BRepMesh::HDMapOfShapePairOfPolygon     myInternalEdges;

  BRepMesh::HDMapOfIntegerListOfXY        myLocation2D;
  BRepMesh::HIMapOfInteger                myVertexEdgeMap;

  // This field is intended to keep calculated mesh nodes to prevent
  // extremely high memory consumption in case if the whole structure is kept.
  BRepMesh::HVectorOfVertex               myMeshNodes;

  BRepMesh::HDMapOfVertexInteger          mySurfaceVertices;
  BRepMesh::HDMapOfIntegerPnt             mySurfacePoints;
  Handle(BRepMesh_DataStructureOfDelaun)  myStructure;
};

DEFINE_STANDARD_HANDLE(BRepMesh_FaceAttribute, Standard_Transient)

#endif
