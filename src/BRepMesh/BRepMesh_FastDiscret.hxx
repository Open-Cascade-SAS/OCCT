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

#ifndef _BRepMesh_FastDiscret_HeaderFile
#define _BRepMesh_FastDiscret_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <BRepMesh_FastDiscret.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <BRepMesh_Status.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MutexForShapeProvider.hxx>
#include <Standard_Transient.hxx>
#include <Handle_BRepAdaptor_HSurface.hxx>
#include <Handle_Geom2d_Curve.hxx>
#include <BRepMesh_Delaun.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_FaceAttribute.hxx>
#include <BRepMesh.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh_ShapeTool.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

class BRepMesh_DataStructureOfDelaun;
class Bnd_Box;
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Edge;
class BRepAdaptor_HSurface;
class Geom2d_Curve;
class BRepMesh_Edge;
class BRepMesh_Vertex;
class gp_Pnt;
class BRepMesh_FaceAttribute;
class TopTools_DataMapOfShapeReal;


//! Algorithm to mesh a shape with respect of the <br>
//! frontier the deflection and by option the shared <br>
//! components. <br>
class BRepMesh_FastDiscret : public Standard_Transient
{
public:
  
  Standard_EXPORT BRepMesh_FastDiscret(
    const Standard_Real defle,
    const Standard_Real angle,
    const Bnd_Box& B,
    const Standard_Boolean withShare = Standard_True,
    const Standard_Boolean inshape = Standard_False,
    const Standard_Boolean relative = Standard_False,
    const Standard_Boolean shapetrigu = Standard_False,
    const Standard_Boolean isInParallel = Standard_False,
    const Standard_Real    theMinSize   = Precision::Confusion(),
    const Standard_Boolean isInternalVerticesMode = Standard_True,
    const Standard_Boolean isControlSurfaceDeflection = Standard_True);

  //! if the boolean <relative> is True, the <br>
  //! deflection used for the polygonalisation of <br>
  //! each edge will be <defle> * Size of Edge. <br>
  //! the deflection used for the faces will be the maximum <br>
  //! deflection of their edges. <br>
  //! <br>
  //! if <shapetrigu> is True, the triangulation, if exists <br>
  //! with a correct deflection, can be used to re-triangulate <br>
  //! the shape. <br>
  //! <br>
  //! if <inshape> is True, the calculated <br>
  //! triangulation will be stored in the shape. <br>
  Standard_EXPORT BRepMesh_FastDiscret(
    const TopoDS_Shape& shape,
    const Standard_Real defle,
    const Standard_Real angle,
    const Bnd_Box& B,
    const Standard_Boolean withShare = Standard_True,
    const Standard_Boolean inshape = Standard_False,
    const Standard_Boolean relative = Standard_False,
    const Standard_Boolean shapetrigu = Standard_False,
    const Standard_Boolean isInParallel = Standard_False,
    const Standard_Real    theMinSize   = Precision::Confusion(),
    const Standard_Boolean isInternalVerticesMode = Standard_True,
    const Standard_Boolean isControlSurfaceDeflection = Standard_True);

  //! Build triangulation on the whole shape.
  Standard_EXPORT void Perform(const TopoDS_Shape& shape);

  //! Record a face for further processing.
  //! @return status flags collected during discretization 
  //! of boundaries of the given face.
  Standard_EXPORT Standard_Integer Add(const TopoDS_Face& face);

  //! Triangulate a face previously recorded for 
  //! processing by call to Add(). Can be executed in 
  //! parallel threads.
  Standard_EXPORT void Process(const TopoDS_Face& face) const;

  void operator ()(const TopoDS_Face& face) const
  {
    Process(face);
  }
  
  //! Request algorithm to launch in multiple threads <br>
  //! to improve performance (should be supported by plugin). <br>
  inline void SetParallel(const Standard_Boolean theInParallel)
  {
    myInParallel = theInParallel;
  }
  
  //! Returns the multi-threading usage flag. <br>
  inline Standard_Boolean IsParallel() const
  {
    return myInParallel;
  }
  
  //! returns the deflection value. <br>
  inline Standard_Real GetDeflection() const
  {
    return myDeflection;
  }

  //! returns the deflection value. <br>
  inline Standard_Real GetAngle() const
  {
    return myAngle;
  }
  
  inline Standard_Boolean WithShare() const
  {
    return myWithShare;
  }
  
  inline Standard_Boolean InShape() const
  {
    return myInshape;
  }
  
  inline Standard_Boolean ShapeTrigu() const
  {
    return myShapetrigu;
  }

  Standard_EXPORT void InitSharedFaces(const TopoDS_Shape& theShape);

  inline const TopTools_IndexedDataMapOfShapeListOfShape& SharedFaces() const
  {
    return mySharedFaces;
  }

  //! Gives face attribute.
  Standard_EXPORT Standard_Boolean GetFaceAttribute
    ( const TopoDS_Face& theFace, Handle(BRepMesh_FaceAttribute)& theAttribute ) const;

  //! Remove face attribute as useless to free locate memory.
  Standard_EXPORT void RemoveFaceAttribute( const TopoDS_Face& theFace );

  //! Returns number of boundary 3d points.
  inline Standard_Integer NbBoundaryPoints() const
  {
    return myBoundaryPoints->Extent();
  }

  DEFINE_STANDARD_RTTI(BRepMesh_FastDiscret)

private:

  //! Auxiliary class used to extract geometrical parameters of TopoDS_Vertex.
  class TopoDSVExplorer
  {
  public:
    TopoDSVExplorer(
      const TopoDS_Vertex&   theVertex,
      const Standard_Boolean isSameUV,
      const TopoDS_Vertex&   theSameVertex)
      : myVertex(theVertex),
        myIsSameUV(isSameUV),
        mySameVertex(theSameVertex)
    {
    }
    virtual ~TopoDSVExplorer() {
    }
    const TopoDS_Vertex& Vertex() const
    {
      return myVertex;
    }

    Standard_Boolean IsSameUV() const
    {
      return myIsSameUV;
    }

    const TopoDS_Vertex& SameVertex() const
    {
      return mySameVertex;
    }

    virtual gp_Pnt Point() const
    {
      return BRep_Tool::Pnt(myVertex);
    }

  private:

    void operator =(const TopoDSVExplorer& /*theOther*/)
    {
    }

  private:
    const TopoDS_Vertex& myVertex;
    Standard_Boolean     myIsSameUV;
    const TopoDS_Vertex& mySameVertex;
  };


  //! Auxiliary class used to extract polygonal parameters of TopoDS_Vertex.
  class PolyVExplorer : public TopoDSVExplorer
  {
  public:
    PolyVExplorer(
      const TopoDS_Vertex&      theVertex,
      const Standard_Boolean    isSameUV,
      const TopoDS_Vertex&      theSameVertex,
      const Standard_Integer    theVertexIndex,
      const TColgp_Array1OfPnt& thePolygon,
      const TopLoc_Location&    theLoc)
      : TopoDSVExplorer(theVertex, isSameUV, theSameVertex),
        myVertexIndex(theVertexIndex),
        myPolygon(thePolygon),
        myLoc(theLoc)
    {
    }

    virtual gp_Pnt Point() const
    {
      return BRepMesh_ShapeTool::UseLocation(myPolygon(myVertexIndex), myLoc);
    }

  private:

    void operator =(const PolyVExplorer& /*theOther*/)
    {
    }

  private:
    Standard_Integer          myVertexIndex;
    const TColgp_Array1OfPnt& myPolygon;
    const TopLoc_Location     myLoc;
  };

  //! Structure keeps common parameters of edge
  //! used for tessellation.
  struct EdgeAttributes
  {
    TopoDS_Vertex                       FirstVertex;
    TopoDS_Vertex                       LastVertex;

    Standard_Real                       FirstParam;
    Standard_Real                       LastParam;

    gp_Pnt2d                            FirstUV;
    gp_Pnt2d                            LastUV;

    Standard_Boolean                    IsSameUV;
    Standard_Real                       MinDist;

    NCollection_Handle<TopoDSVExplorer> FirstVExtractor;
    NCollection_Handle<TopoDSVExplorer> LastVExtractor;
  };

  //! Structure keeps geometrical parameters of edge's PCurve.
  //! Used for caching.
  struct EdgePCurve
  {
    Handle(Geom2d_Curve) Curve2d;
    Standard_Real        FirstParam;
    Standard_Real        LastParam;
  };

  //! Fills structure of by attributes of the given edge.
  //! @return TRUE on success, FALSE elsewhere.
  Standard_Boolean getEdgeAttributes(
    const TopoDS_Edge&  theEdge,
    const EdgePCurve&   thePCurve,
    const Standard_Real theDefEdge,
    EdgeAttributes&     theAttributes) const;

  //! Registers end vertices of the edge in mesh data 
  //! structure of currently processed face.
  void registerEdgeVertices(
    EdgeAttributes&   theAttributes,
    Standard_Integer& ipf,
    Standard_Integer& ivf, 
    Standard_Integer& isvf,
    Standard_Integer& ipl,
    Standard_Integer& ivl,
    Standard_Integer& isvl);

  //! Adds tessellated representation of the given edge to
  //! mesh data structure of currently processed face.
  void add(const TopoDS_Edge&  theEdge,
           const EdgePCurve&   theCurve2D,
           const Standard_Real theEdgeDeflection);

  //! Updates tessellated representation of the given edge.
  //! If edge already has a polygon which deflection satisfies
  //! the given value, retrieves tessellation from polygon.
  //! Computes tessellation using edge's geometry elsewhere.
  void update(
    const TopoDS_Edge&          theEdge,
    const Handle(Geom2d_Curve)& theCurve2D,
    const Standard_Real         theEdgeDeflection,
    EdgeAttributes&             theAttributes);

  //! Stores polygonal model of the given edge.
  //! @param theEdge edge which polygonal model is stored.
  //! @param thePolygon polygonal model of the edge.
  //! @param theDeflection deflection with which polygonalization is performed.
  //! This value is stored inside the polygon.
  void storePolygon(
    const TopoDS_Edge&                         theEdge,
    Handle(Poly_PolygonOnTriangulation)&       thePolygon,
    const Standard_Real                        theDeflection);

  //! Caches polygonal model of the given edge to be used in further.
  //! @param theEdge edge which polygonal data is stored.
  //! @param thePolygon shared polygonal data of the edge.
  //! @param theDeflection deflection with which polygonalization is performed.
  //! This value is stored inside the polygon.
  void storePolygonSharedData(
    const TopoDS_Edge&                         theEdge,
    Handle(Poly_PolygonOnTriangulation)&       thePolygon,
    const Standard_Real                        theDeflection);

  //! Resets temporary data structure used to collect unique nodes.
  void resetDataStructure();

private:

  TopoDS_Face                                      myFace;
  Standard_Real                                    myAngle;
  Standard_Real                                    myDeflection;
  Standard_Real                                    myDtotale;
  Standard_Boolean                                 myWithShare;
  Standard_Boolean                                 myInParallel;
  BRepMesh::DMapOfShapePairOfPolygon               myEdges;
  BRepMesh::DMapOfFaceAttribute                    myAttributes;
  Standard_Boolean                                 myRelative;
  Standard_Boolean                                 myShapetrigu;
  Standard_Boolean                                 myInshape;
  TopTools_DataMapOfShapeReal                      myMapdefle;

  // Data shared for whole shape
  BRepMesh::HDMapOfVertexInteger                   myBoundaryVertices;
  BRepMesh::HDMapOfIntegerPnt                      myBoundaryPoints;

  // Fast access to attributes of current face
  Handle(BRepMesh_FaceAttribute)                   myAttribute;
  TopTools_IndexedDataMapOfShapeListOfShape        mySharedFaces;

  Standard_Real                                    myMinSize;
  Standard_Boolean                                 myInternalVerticesMode;
  Standard_Boolean                                 myIsControlSurfaceDeflection;
};

DEFINE_STANDARD_HANDLE(BRepMesh_FastDiscret, Standard_Transient)

#endif
