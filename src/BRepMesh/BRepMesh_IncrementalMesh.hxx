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

#include <Handle_BRepMesh_FastDiscret.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <Bnd_Box.hxx>
#include <BRepMesh_DiscretRoot.hxx>
#include <BRepMesh_PDiscretRoot.hxx>

#include <vector>

class BRepMesh_FastDiscret;
class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Face;

//! Builds the mesh of a shape with respect of their <br>
//! correctly triangulated parts <br>
//! <br>
class BRepMesh_IncrementalMesh : public BRepMesh_DiscretRoot {

public:

  Standard_EXPORT   BRepMesh_IncrementalMesh();

  Standard_EXPORT virtual ~BRepMesh_IncrementalMesh();

  //! If the boolean flag <isRelative> is TRUE, the <br>
  //! deflection used for the polygonalisation of <br>
  //! each edge will be <theLinDeflection> * Size of Edge. <br>
  //! the deflection used for the faces will be the maximum <br>
  //! deflection of their edges. <br>
  Standard_EXPORT BRepMesh_IncrementalMesh(const TopoDS_Shape&    theShape,
                                           const Standard_Real    theLinDeflection,
                                           const Standard_Boolean isRelative       = Standard_False,
                                           const Standard_Real    theAngDeflection = 0.5,
                                           const Standard_Boolean isInParallel     = Standard_False);
  
  Standard_EXPORT void SetRelative(const Standard_Boolean theFlag);
  
  Standard_EXPORT Standard_Boolean Relative() const;
  
  Standard_EXPORT virtual void Perform();

  //! Builds the incremental mesh of the shape <br>
  Standard_EXPORT void Update(const TopoDS_Shape& theShape);
  
  Standard_EXPORT Standard_Boolean IsModified() const;
  
  Standard_EXPORT Standard_Integer GetStatusFlags() const;
  
  //! Request algorithm to launch in multiple threads to improve performance. <br>
  Standard_EXPORT void SetParallel(const Standard_Boolean isInParallel);
  
  //! Returns the multi-threading usage flag. <br>
  Standard_EXPORT Standard_Boolean IsParallel() const;
  
  //! Plugin interface for the Mesh Factories. <br>
  Standard_EXPORT static Standard_Integer Discret(const TopoDS_Shape&    theShape,
                                                  const Standard_Real    theLinDeflection,
                                                  const Standard_Real    theAngDeflection,
                                                  BRepMesh_PDiscretRoot& theAlgo);
  
  //! Returns multi-threading usage flag set by default in <br>
  //! Discret() static method (thus applied only to Mesh Factories). <br>
  Standard_EXPORT static Standard_Boolean IsParallelDefault();
  
  //! Setup multi-threading usage flag set by default in <br>
  //! Discret() static method (thus applied only to Mesh Factories). <br>
  Standard_EXPORT static void SetParallelDefault(const Standard_Boolean isInParallel) ;


  DEFINE_STANDARD_RTTI(BRepMesh_IncrementalMesh)

protected:

  Standard_EXPORT virtual void Init();

  //! Collects faces suitable for meshing.
  Standard_EXPORT void collectFaces();

protected:

  Standard_Boolean                          myRelative;
  Standard_Boolean                          myInParallel;
  TopTools_MapOfShape                       myMap;
  Handle_BRepMesh_FastDiscret               myMesh;
  Standard_Boolean                          myModified;
  TopTools_DataMapOfShapeReal               mymapedge;
  TopTools_IndexedDataMapOfShapeListOfShape myancestors;
  Standard_Real                             mydtotale;
  Bnd_Box                                   myBox;
  Standard_Integer                          myStatus;
  std::vector<TopoDS_Face>                  myFaces;

private:
  //! Checks is the shape to be meshed has correct poly data, <br>
  //! i.e. PolygonOnTriangulation of particular edge connected <br>
  //! to the same Triangulation data structure as stored inside <br>
  //! a parent face. <br>
  Standard_Boolean isCorrectPolyData();

  //! Locate a correct discretisation if it exists <br>
  //! Set no one otherwise <br>
  void Update(const TopoDS_Edge& theEdge);

  //! If the face is not correctly triangulated, or if one <br>
  //! of its edges is to be discretisated correctly, the <br>
  //! triangulation of this face is built. <br>
  void Update(const TopoDS_Face& theFace);
};

DEFINE_STANDARD_HANDLE(BRepMesh_IncrementalMesh,BRepMesh_DiscretRoot)

// other Inline functions and methods (like "C++: function call" methods)

#endif
