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

#ifndef _BRepMesh_IncrementalMesh_HeaderFile
#define _BRepMesh_IncrementalMesh_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

#include <BRepMesh_FastDiscret.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <BRepMesh_DiscretRoot.hxx>
#include <Handle_Poly_Triangulation.hxx>
#include <BRepMesh.hxx>

#include <vector>

class Poly_Triangulation;
class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Face;

//! Builds the mesh of a shape with respect of their 
//! correctly triangulated parts 
class BRepMesh_IncrementalMesh : public BRepMesh_DiscretRoot
{
public: //! @name mesher API

  //! Default constructor
  Standard_EXPORT BRepMesh_IncrementalMesh();

  //! Destructor
  Standard_EXPORT virtual ~BRepMesh_IncrementalMesh();

  //! Constructor.
  //! Automatically calls method Perform.
  //! @param theShape shape to be meshed.
  //! @param theLinDeflection linear deflection.
  //! @param isRelative if TRUE deflection used for discretization of 
  //! each edge will be <theLinDeflection> * <size of edge>. Deflection 
  //! used for the faces will be the maximum deflection of their edges.
  //! @param theAngDeflection angular deflection.
  //! @param isInParallel if TRUE shape will be meshed in parallel.
  Standard_EXPORT BRepMesh_IncrementalMesh(
    const TopoDS_Shape&    theShape,
    const Standard_Real    theLinDeflection,
    const Standard_Boolean isRelative       = Standard_False,
    const Standard_Real    theAngDeflection = 0.5,
    const Standard_Boolean isInParallel     = Standard_False);

  //! Performs meshing ot the shape.
  Standard_EXPORT virtual void Perform();
  
public: //! @name accessing to parameters.

  //! Enables using relative deflection.
  //! @param isRelative if TRUE deflection used for discretization of 
  //! each edge will be <theLinDeflection> * <size of edge>. Deflection 
  //! used for the faces will be the maximum deflection of their edges.
  inline void SetRelative(const Standard_Boolean isRelative)
  {
    myRelative = isRelative;
  }
  
  //! Returns relative deflection flag.
  inline Standard_Boolean IsRelative() const
  {
    return myRelative;
  }
  
  //! Returns modified flag.
  inline Standard_Boolean IsModified() const
  {
    return myModified;
  }
  
  //! Returns accumulated status flags faced during meshing.
  inline Standard_Integer GetStatusFlags() const
  {
    return myStatus;
  }
  
  //! Request algorithm to launch in multiple threads to improve performance.
  inline void SetParallel(const Standard_Boolean isInParallel)
  {
    myInParallel = isInParallel;
  }
  
  //! Returns the multi-threading usage flag.
  inline Standard_Boolean IsParallel() const
  {
    return myInParallel;
  }

  //! Sets min size parameter.
  inline void SetMinSize(const Standard_Real theMinSize)
  {
    myMinSize = Max(theMinSize, Precision::Confusion());
  }

  //! Returns min size parameter.
  inline Standard_Real GetMinSize() const
  {
    return myMinSize;
  }

  //! Enables/disables internal vertices mode (enabled by default).
  inline void SetInternalVerticesMode(const Standard_Boolean isEnabled)
  {
    myInternalVerticesMode = isEnabled;
  }
  
  //! Returns flag indicating is internal vertices mode enabled/disabled.
  inline Standard_Boolean IsInternalVerticesMode() const
  {
    return myInternalVerticesMode;
  }

  //! Enables/disables control of deflection of mesh from real surface 
  //! (enabled by default).
  inline void SetControlSurfaceDeflection(const Standard_Boolean isEnabled)
  {
    myIsControlSurfaceDeflection = isEnabled;
  }

  //! Returns flag indicating is adaptive reconfiguration 
  //! of mesh enabled/disabled.
  inline Standard_Boolean IsControlSurfaceDeflection() const
  {
    return myIsControlSurfaceDeflection;
  }

public: //! @name plugin API

  //! Plugin interface for the Mesh Factories.
  //! Initializes meshing algorithm with the given parameters.
  //! @param theShape shape to be meshed.
  //! @param theLinDeflection linear deflection.
  //! @param theAngDeflection angular deflection.
  //! @param[out] theAlgo pointer to initialized algorithm.
  Standard_EXPORT static Standard_Integer Discret(const TopoDS_Shape&    theShape,
                                                  const Standard_Real    theLinDeflection,
                                                  const Standard_Real    theAngDeflection,
                                                  BRepMesh_DiscretRoot* &theAlgo);
  
  //! Returns multi-threading usage flag set by default in 
  //! Discret() static method (thus applied only to Mesh Factories).
  Standard_EXPORT static Standard_Boolean IsParallelDefault();
  
  //! Setup multi-threading usage flag set by default in 
  //! Discret() static method (thus applied only to Mesh Factories).
  Standard_EXPORT static void SetParallelDefault(const Standard_Boolean isInParallel);

  DEFINE_STANDARD_RTTI(BRepMesh_IncrementalMesh)

protected:

  Standard_EXPORT virtual void init();

private:

  //! Builds the incremental mesh for the shape.
  void update();

  //! Checks triangulation of the given face for consistency 
  //! with the chosen tolerance. If some edge of face has no
  //! discrete representation triangulation will be calculated.
  //! @param theFace face to be checked.
  void update(const TopoDS_Face& theFace);

  //! Checks discretization of the given edge for consistency 
  //! with the chosen tolerance.
  //! @param theEdge edge to be checked.
  void update(const TopoDS_Edge& theEdge);

  //! Collects faces suitable for meshing.
  void collectFaces();

  //! Discretizes edges that have no associations with faces.
  void discretizeFreeEdges();

  //! Returns deflection of the given edge.
  //! @param theEdge edge which tolerance should be taken.
  Standard_Real edgeDeflection(const TopoDS_Edge& theEdge);

  //! Returns deflection of the given face.
  //! If relative flag is set, calculates relative deflection of the face 
  //! as an average value of relative deflection regarding face's edges.
  //! Returns value of deflection set by user elsewhere.
  Standard_Real faceDeflection(const TopoDS_Face& theFace);

  //! Prepares the given face for meshing.
  //! Nullifies triangulation of face and polygons of face's edges.
  //! @param theFace face to be checked.
  //! @param isWithCheck if TRUE, checks parameters of triangulation 
  //! existing in face. If its deflection satisfies the given value and
  //! each edge of face has polygon corresponded to this triangulation,
  //! method return FALSE.
  //! @return TRUE in case if the given face should be meshed.
  Standard_Boolean toBeMeshed(const TopoDS_Face&     theFace,
                              const Standard_Boolean isWithCheck);

  //! Stores mesh to the shape.
  void commit();

  //! Stores mesh of internal edges to the face.
  void commitEdges(const TopoDS_Face& theFace);
  
  //! Clears internal data structures.
  void clear();

protected:

  Standard_Boolean                            myRelative;
  Standard_Boolean                            myInParallel;
  BRepMesh::DMapOfEdgeListOfTriangulationBool myEdges;
  Handle(BRepMesh_FastDiscret)                myMesh;
  Standard_Boolean                            myModified;
  TopTools_DataMapOfShapeReal                 myEdgeDeflection;
  Standard_Real                               myMaxShapeSize;
  Standard_Integer                            myStatus;
  NCollection_Vector<TopoDS_Face>             myFaces;
  Standard_Real                               myMinSize;
  Standard_Boolean                            myInternalVerticesMode;
  Standard_Boolean                            myIsControlSurfaceDeflection;
};

DEFINE_STANDARD_HANDLE(BRepMesh_IncrementalMesh,BRepMesh_DiscretRoot)

#endif
