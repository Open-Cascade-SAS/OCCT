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
#include <Standard_Type.hxx>
#include <BRepMesh_FastDiscret.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <BRepMesh_Status.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MutexForShapeProvider.hxx>
#include <Standard_Transient.hxx>
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
class Geom2dAdaptor_HCurve;
class BRepAdaptor_HSurface;
class BRepMesh_Edge;
class BRepMesh_Vertex;
class gp_Pnt;
class BRepMesh_FaceAttribute;

//! Algorithm to mesh a shape with respect of the <br>
//! frontier the deflection and by option the shared <br>
//! components. <br>
class BRepMesh_FastDiscret : public Standard_Transient
{
public:


  //! Structure storing meshing parameters
  struct Parameters {

    //! Default constructor
    Parameters()
      :
     Angle(0.1),
     Deflection(0.001),
     MinSize(Precision::Confusion()),
     InParallel(Standard_False),
     Relative(Standard_False),
     AdaptiveMin(Standard_False),
     InternalVerticesMode(Standard_True),
     ControlSurfaceDeflection(Standard_True)
     {
     }
    
    //! Angular deflection
    Standard_Real                                    Angle;
    
    //! Deflection
    Standard_Real                                    Deflection;  

    //! Minimal allowed size of mesh element
    Standard_Real                                    MinSize; 

    //! Switches on/off multy thread computation
    Standard_Boolean                                 InParallel; 

    //! Switches on/off relative computation of edge tolerance<br>
    //! If trur, deflection used for the polygonalisation of each edge will be 
    //! <defle> * Size of Edge. The deflection used for the faces will be the 
    //! maximum deflection of their edges.
    Standard_Boolean                                 Relative;

    //! Adaptive parametric tolerance flag. <br>
    //! If this flag is set to true the minimal parametric tolerance
    //! is computed taking minimal parametric distance between vertices
    //! into account
    Standard_Boolean                                 AdaptiveMin;

    //! Mode to take or ont to take internal face vertices into account
    //! in triangulation process
    Standard_Boolean                                 InternalVerticesMode;

    //! Prameter to check the deviation of triangulation and interior of
    //! the face
    Standard_Boolean                                 ControlSurfaceDeflection;
  };

public:
  
  
  //! Constructor. 
  //! Sets the meshing parameters and updates
  //! relative defletion according to bounding box
  //! @param B - bounding box encompasing shape
  //! @param theParams - meshing algo parameters    
  Standard_EXPORT BRepMesh_FastDiscret (const Bnd_Box& B,
                                        const Parameters& theParams);

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

  void operator () (const TopoDS_Face& face) const
  {
    Process(face);
  }
  
  //! Returns parameters of meshing
  inline const Parameters& MeshParameters() const
  {
    return myParameters;
  }

  //! Returns modificable mesh parameters
  inline Parameters& ChangeMeshParameters()
  {
    return myParameters;
  }
    
  
  Standard_EXPORT void InitSharedFaces(const TopoDS_Shape& theShape);

  inline const TopTools_IndexedDataMapOfShapeListOfShape& SharedFaces() const
  {
    return mySharedFaces;
  }

  //! Returns attribute descriptor for the given face.
  //! @param theFace face the attribute should be returned for.
  //! @param[out] theAttribute attribute found for the specified face.
  //! @param isForceCreate if True creates new attribute in case if there 
  //! is no data for the given face.
  Standard_EXPORT Standard_Boolean GetFaceAttribute (
    const TopoDS_Face&              theFace, 
    Handle(BRepMesh_FaceAttribute)& theAttribute,
    const Standard_Boolean          isForceCreate = Standard_False) const;

  //! Remove face attribute as useless to free locate memory.
  Standard_EXPORT void RemoveFaceAttribute( const TopoDS_Face& theFace );

  //! Returns number of boundary 3d points.
  inline Standard_Integer NbBoundaryPoints() const
  {
    return myBoundaryPoints->Extent();
  }

  DEFINE_STANDARD_RTTIEXT(BRepMesh_FastDiscret,Standard_Transient)

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

    Standard_Real                       Deflection;
    Standard_Boolean                    IsSameUV;

    NCollection_Handle<TopoDSVExplorer> FirstVExtractor;
    NCollection_Handle<TopoDSVExplorer> LastVExtractor;
  };

  //! Fills structure of by attributes of the given edge.
  //! @return TRUE on success, FALSE elsewhere.
  Standard_Boolean getEdgeAttributes(
    const TopoDS_Edge&  theEdge,
    const Handle(Geom2dAdaptor_HCurve)& thePCurve,
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
           const Handle(Geom2dAdaptor_HCurve)& theCurve2D,
           const Standard_Real theEdgeDeflection);

  //! Updates tessellated representation of the given edge.
  //! If edge already has a polygon which deflection satisfies
  //! the given value, retrieves tessellation from polygon.
  //! Computes tessellation using edge's geometry elsewhere.
  void update(
    const TopoDS_Edge&          theEdge,
    const Handle(Geom2dAdaptor_HCurve)& theCurve2D,
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

  BRepMesh::DMapOfShapePairOfPolygon               myEdges;
  mutable BRepMesh::DMapOfFaceAttribute            myAttributes;
  TopTools_DataMapOfShapeReal                      myMapdefle;

  // Data shared for whole shape
  BRepMesh::HDMapOfVertexInteger                   myBoundaryVertices;
  BRepMesh::HDMapOfIntegerPnt                      myBoundaryPoints;

  // Fast access to attributes of current face
  Handle(BRepMesh_FaceAttribute)                   myAttribute;
  TopTools_IndexedDataMapOfShapeListOfShape        mySharedFaces;

  Parameters                                       myParameters;

  Standard_Real                                    myDtotale;
};

DEFINE_STANDARD_HANDLE(BRepMesh_FastDiscret, Standard_Transient)

#endif
