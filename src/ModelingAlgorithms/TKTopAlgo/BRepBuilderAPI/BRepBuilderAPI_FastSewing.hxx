//! Created on: 2015-04-24
//! Created by: NIKOLAI BUKHALOV
//! Copyright (c) 2015 OPEN CASCADE SAS
//!
//! This file is part of Open CASCADE Technology software library.
//!
//! This library is free software; you can redistribute it and/or modify it under
//! the terms of the GNU Lesser General Public License version 2.1 as published
//! by the Free Software Foundation, with special exception defined in the file
//! OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
//! distribution for complete text of the license and disclaimer of any warranty.
//!
//! Alternatively, this file may be used under the terms of Open CASCADE
//! commercial license or contractual agreement.

#ifndef _BRepBuilderAPI_FastSewing_HeaderFile
#define _BRepBuilderAPI_FastSewing_HeaderFile

#include <Standard_Transient.hxx>
#include <BRep_Builder.hxx>

#include <NCollection_List.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_CellFilter.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

//! This class performs fast sewing of surfaces (faces). It supposes
//! that all surfaces are finite and are naturally restricted by their bounds.
//! Moreover, it supposes that stitched together surfaces have the same parameterization
//! along common boundaries, therefore it does not perform time-consuming check for
//! SameParameter property of edges.
//!
//! For sewing, use this function as following:
//! - set tolerance value (default tolerance is 1.E-06)
//! - add all necessary surfaces (faces)
//! - check status if adding is correctly completed.
//! - compute -> Perform
//! - retrieve the error status if any
//! - retrieve the resulted shape
class BRepBuilderAPI_FastSewing : public Standard_Transient
{
public:
  typedef unsigned int FS_VARStatuses;

  //! Enumeration of result statuses
  // ATTENTION!!! If you add new status, please
  //     describe it in GetStatuses() method
  enum FS_Statuses
  {
    FS_OK                   = 0x00000000,
    FS_Degenerated          = 0x00000001,
    FS_FindVertexError      = 0x00000002,
    FS_FindEdgeError        = 0x00000004,
    FS_FaceWithNullSurface  = 0x00000008,
    FS_NotNaturalBoundsFace = 0x00000010,
    FS_InfiniteSurface      = 0x00000020,
    FS_EmptyInput           = 0x00000040,
    FS_Exception            = 0x00000080
  };

  //! Creates an object with tolerance of connexity
  Standard_EXPORT BRepBuilderAPI_FastSewing(const double theTolerance = 1.0e-06);

  //! Adds faces of a shape
  Standard_EXPORT bool Add(const TopoDS_Shape& theShape);

  //! Adds a surface
  Standard_EXPORT bool Add(const occ::handle<Geom_Surface>& theSurface);

  //! Compute resulted shape
  Standard_EXPORT void Perform();

  //! Sets tolerance
  void SetTolerance(const double theToler) { myTolerance = theToler; }

  //! Returns tolerance
  double GetTolerance() const { return myTolerance; }

  //! Returns resulted shape
  const TopoDS_Shape& GetResult() const { return myResShape; }

  //! Returns list of statuses. Print message if theOS != 0
  Standard_EXPORT FS_VARStatuses GetStatuses(Standard_OStream* const theOS = nullptr);

  DEFINE_STANDARD_RTTIEXT(BRepBuilderAPI_FastSewing, Standard_Transient)

protected:
  class NodeInspector;

  Standard_EXPORT void FindVertexes(const int                              theSurfID,
                                    NCollection_CellFilter<NodeInspector>& theCells);
  Standard_EXPORT void FindEdges(const int theSurfID);
  Standard_EXPORT void UpdateEdgeInfo(const int theIDPrevVertex,
                                      const int theIDCurrVertex,
                                      const int theFaceID,
                                      const int theIDCurvOnFace);
  Standard_EXPORT void CreateNewEdge(const int theIDPrevVertex,
                                     const int theIDCurrVertex,
                                     const int theFaceID,
                                     const int theIDCurvOnFace);

  Standard_EXPORT double Compute3DRange();

  //! Sets status. Returns numeric value of the status set
  FS_VARStatuses SetStatus(FS_Statuses theStatus)
  {
    const FS_VARStatuses aStatusID = (FS_VARStatuses)(theStatus);
    myStatusList |= aStatusID;
    return aStatusID;
  }

  class FS_Edge;

  // Classes FS_Vertex, FS_Face and FS_Edge keep information about
  // relations between resulted members (e.g. which faces share this vertex? etc.)

  //! The struct corresponding to a vertex
  struct FS_Vertex
  {
  public:
    FS_Vertex()
        : myID(-1) {};

    //! Creates topological member (vertex)
    void CreateTopologicalVertex(const double theToler)
    {
      BRep_Builder aBuilder;
      aBuilder.MakeVertex(myTopoVert, myPnt, theToler);
    }

    //! Geometry point of this Vertex
    gp_Pnt        myPnt;
    TopoDS_Vertex myTopoVert;

    //! List of faces and edges which share this vertex
    NCollection_List<int> myFaces;
    NCollection_List<int> myEdges;

    //! Identifies the place of this Vertex in
    //! BRepBuilderAPI_FastSewing::myVertexVec list
    int myID;
  };

  //! The struct corresponding to an face
  struct FS_Face
  {
    FS_Face()
        : myID(-1)
    {
      for (int i = 0; i < 4; i++)
      {
        myVertices[i] = -1;
        myEdges[i]    = -1;
      }
    };

    //! Creates topological members (wire and face)
    void CreateTopologicalWire(const NCollection_Vector<FS_Edge>& theEdgeVec,
                               const double                       theToler);
    void CreateTopologicalFace();

    //! Sets vertex
    void SetVertex(const int thePlaceID, const int theVertID)
    {
      Standard_RangeError_Raise_if((thePlaceID < 0) || (thePlaceID > 3),
                                   "FS_Face::SetVertex(): OUT of Range");

      myVertices[thePlaceID] = theVertID;
    }

    //! Sets edge
    void SetEdge(const int thePlaceID, const int theEdgeID)
    {
      Standard_RangeError_Raise_if((thePlaceID < 0) || (thePlaceID > 3),
                                   "FS_Face::SetEdge(): OUT of Range");

      myEdges[thePlaceID] = theEdgeID;
    }

    TopoDS_Face mySrcFace;
    TopoDS_Wire myWire;
    TopoDS_Face myRetFace;

    //! myEdges[i] number of the edge in myEdgeVec
    //! (i==0) <-> (V=Vf); (i==1) <-> (U=Ul);
    //! (i==2) <-> (V=Vl); (i==3) <-> (U=Uf)
    int myEdges[4];
    //! myVertices[i] is Start point of myEdges[i]
    int myVertices[4];

    //! Identifies the place of this Face in
    //! BRepBuilderAPI_FastSewing::myFaceVec list
    int myID;
  };

  //! The struct corresponding to a edge
  class FS_Edge
  {
  public:
    FS_Edge()
        : myID(-1)
    {
      myVertices[0] = -1;
      myVertices[1] = -1;
    }

    FS_Edge(const int theIDVert1, const int theIDVert2)
        : myID(-1)
    {
      myVertices[0] = theIDVert1;
      myVertices[1] = theIDVert2;
    };

    //! Creates topological member (TopoDS_Edge)
    void CreateTopologicalEdge(const NCollection_Vector<FS_Vertex>& theVertexVec,
                               const NCollection_Vector<FS_Face>&   theFaceVec,
                               const double                         theTol);

    //! Sets vertex
    void SetVertex(const int thePlaceID, const int theVertID)
    {
      Standard_RangeError_Raise_if((thePlaceID < 0) || (thePlaceID > 1),
                                   "FS_Face::SetVertex(): OUT of Range");

      myVertices[thePlaceID] = theVertID;
    }

    bool IsDegenerated() const { return (myVertices[0] == myVertices[1]); }

    //! List of faces which are shared with this edge
    //! Value is the index of this shape in myFaceVec array
    NCollection_Sequence<int> myFaces;

    //! Identifies the place of this Edge in
    //! BRepBuilderAPI_FastSewing::myEdgeVec list
    int myID;

    TopoDS_Edge myTopoEdge;

  private:
    //! Index of the vertex in myVertexVec array
    int myVertices[2];
  };

  //! This inspector will find a node nearest to the given point
  //! not far than on the given tolerance
  class NodeInspector
  {
  public:
    static constexpr int Dimension = 3;

    typedef gp_XYZ Point;
    typedef int    Target;

    static double Coord(int i, const Point& thePnt) { return thePnt.Coord(i + 1); }

    Point Shift(const Point& thePnt, double theTol) const
    {
      return Point(thePnt.X() + theTol, thePnt.Y() + theTol, thePnt.Z() + theTol);
    }

    NodeInspector(const NCollection_Vector<FS_Vertex>& theVec,
                  const gp_Pnt&                        thePnt,
                  const double                         theTol);

    Standard_EXPORT NCollection_CellFilter_Action Inspect(const Target theId);

    Target GetResult() { return myResID; }

  private:
    NodeInspector&                       operator=(const NodeInspector&) = delete;
    const NCollection_Vector<FS_Vertex>& myVecOfVertexes;
    gp_Pnt                               myPoint;
    double                               mySQToler;
    Target                               myResID;
  };

private:
  TopoDS_Shape myResShape;

  // myFaceVec, myVertexVec and myEdgeVec lists are filled only once!!!!!

  //! Vector of faces
  NCollection_Vector<FS_Face> myFaceVec;
  //! Vector of Vertices
  NCollection_Vector<FS_Vertex> myVertexVec;
  //! Vector of edges
  NCollection_Vector<FS_Edge> myEdgeVec;

  //! Tolerance
  double myTolerance;

  //! Bits of computation status
  FS_VARStatuses myStatusList;
};

#endif // _BRepBuilderAPI_FastSewing_HeaderFile
