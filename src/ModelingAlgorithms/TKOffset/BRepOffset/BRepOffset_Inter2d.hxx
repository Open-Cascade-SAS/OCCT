// Created on: 1996-08-30
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BRepOffset_Inter2d_HeaderFile
#define _BRepOffset_Inter2d_HeaderFile

#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Message_ProgressRange.hxx>

class BRepAlgo_AsDes;
class BRepAlgo_Image;
class BRepOffset_Analyse;
class BRepOffset_Offset;
class TopoDS_Edge;
class TopoDS_Face;

//! Computes the intersections between edges on a face
//! stores result is SD as AsDes from BRepOffset.
class BRepOffset_Inter2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the intersections between the edges stored
  //! is AsDes as descendants of <F> . Intersections is computed
  //! between two edges if one of them is bound in NewEdges.
  //! When all faces of the shape are treated the intersection
  //! vertices have to be fused using the FuseVertices method.
  //! theDMVV contains the vertices that should be fused
  Standard_EXPORT static void Compute(const occ::handle<BRepAlgo_AsDes>&              AsDes,
                                      const TopoDS_Face&                         F,
                                      const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&          NewEdges,
                                      const double                        Tol,
                                      const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&  theEdgeIntEdges,
                                      NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theDMVV,
                                      const Message_ProgressRange&               theRange);

  //! Computes the intersection between the offset edges of the <FI>.
  //! All intersection vertices will be stored in AsDes2d.
  //! When all faces of the shape are treated the intersection vertices
  //! have to be fused using the FuseVertices method.
  //! theDMVV contains the vertices that should be fused.
  Standard_EXPORT static bool ConnexIntByInt(
    const TopoDS_Face&                         FI,
    BRepOffset_Offset&                         OFI,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&              MES,
    const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&        Build,
    const occ::handle<BRepAlgo_AsDes>&              theAsDes,
    const occ::handle<BRepAlgo_AsDes>&              AsDes2d,
    const double                        Offset,
    const double                        Tol,
    const BRepOffset_Analyse&                  Analyse,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                FacesWithVerts,
    BRepAlgo_Image&                            theImageVV,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&        theEdgeIntEdges,
    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theDMVV,
    const Message_ProgressRange&               theRange);

  //! Computes the intersection between the offset edges generated
  //! from vertices and stored into AsDes as descendants of the <FI>.
  //! All intersection vertices will be stored in AsDes2d.
  //! When all faces of the shape are treated the intersection vertices
  //! have to be fused using the FuseVertices method.
  //! theDMVV contains the vertices that should be fused.
  Standard_EXPORT static void ConnexIntByIntInVert(
    const TopoDS_Face&                         FI,
    BRepOffset_Offset&                         OFI,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&              MES,
    const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&        Build,
    const occ::handle<BRepAlgo_AsDes>&              AsDes,
    const occ::handle<BRepAlgo_AsDes>&              AsDes2d,
    const double                        Tol,
    const BRepOffset_Analyse&                  Analyse,
    NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theDMVV,
    const Message_ProgressRange&               theRange);

  //! Fuses the chains of vertices in the theDMVV
  //! and updates AsDes by replacing the old vertices
  //! with the new ones.
  Standard_EXPORT static bool FuseVertices(
    const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theDMVV,
    const occ::handle<BRepAlgo_AsDes>&                    theAsDes,
    BRepAlgo_Image&                                  theImageVV);

  //! extents the edge
  Standard_EXPORT static bool ExtentEdge(const TopoDS_Edge&  E,
                                                     TopoDS_Edge&        NE,
                                                     const double theOffset);
};

#endif // _BRepOffset_Inter2d_HeaderFile
