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

#ifndef _BRepOffset_Inter3d_HeaderFile
#define _BRepOffset_Inter3d_HeaderFile

#include <Message_ProgressRange.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <TopAbs_State.hxx>
#include <BRepOffset_Offset.hxx>
class BRepAlgo_AsDes;
class BRepAlgo_Image;
class TopoDS_Face;
class TopoDS_Shape;
class BRepOffset_Analyse;

//! Computes the connection of the offset and not offset faces
//! according to the connection type required.
//! Store the result in AsDes tool.
class BRepOffset_Inter3d
{
public:
  DEFINE_STANDARD_ALLOC

public:
  //! Constructor
  Standard_EXPORT BRepOffset_Inter3d(const occ::handle<BRepAlgo_AsDes>& AsDes,
                                     const TopAbs_State                 Side,
                                     const double                       Tol);

  // Computes intersection of the given faces among each other
  Standard_EXPORT void CompletInt(const NCollection_List<TopoDS_Shape>& SetOfFaces,
                                  const BRepAlgo_Image&                 InitOffsetFace,
                                  const Message_ProgressRange&          theRange);

  //! Computes intersection of pair of faces
  Standard_EXPORT void FaceInter(const TopoDS_Face&    F1,
                                 const TopoDS_Face&    F2,
                                 const BRepAlgo_Image& InitOffsetFace);

  //! Computes connections of the offset faces that have to be connected by arcs.
  Standard_EXPORT void ConnexIntByArc(const NCollection_List<TopoDS_Shape>& SetOfFaces,
                                      const TopoDS_Shape&                   ShapeInit,
                                      const BRepOffset_Analyse&             Analyse,
                                      const BRepAlgo_Image&                 InitOffsetFace,
                                      const Message_ProgressRange&          theRange);

  //! Computes intersection of the offset faces that have to be connected by
  //! sharp edges, i.e. it computes intersection between extended offset faces.
  Standard_EXPORT void ConnexIntByInt(
    const TopoDS_Shape&                                                                  SI,
    const NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher>& MapSF,
    const BRepOffset_Analyse&                                                            A,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&            MES,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&            Build,
    NCollection_List<TopoDS_Shape>&                                                      Failed,
    const Message_ProgressRange&                                                         theRange,
    const bool bIsPlanar = false);

  //! Computes intersection with not offset faces .
  Standard_EXPORT void ContextIntByInt(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& ContextFaces,
    const bool                                                           ExtentContext,
    const NCollection_DataMap<TopoDS_Shape, BRepOffset_Offset, TopTools_ShapeMapHasher>& MapSF,
    const BRepOffset_Analyse&                                                            A,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&            MES,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&            Build,
    NCollection_List<TopoDS_Shape>&                                                      Failed,
    const Message_ProgressRange&                                                         theRange,
    const bool bIsPlanar = false);

  //! Computes connections of the not offset faces that have to be connected by arcs
  Standard_EXPORT void ContextIntByArc(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& ContextFaces,
    const bool                                                           ExtentContext,
    const BRepOffset_Analyse&                                            Analyse,
    const BRepAlgo_Image&                                                InitOffsetFace,
    BRepAlgo_Image&                                                      InitOffsetEdge,
    const Message_ProgressRange&                                         theRange);

  //! Marks the pair of faces as already intersected
  Standard_EXPORT void SetDone(const TopoDS_Face& F1, const TopoDS_Face& F2);

  //! Checks if the pair of faces has already been treated.
  Standard_EXPORT bool IsDone(const TopoDS_Face& F1, const TopoDS_Face& F2) const;

  //! Returns touched faces
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& TouchedFaces()
  {
    return myTouched;
  };

  //! Returns AsDes tool
  occ::handle<BRepAlgo_AsDes> AsDes() const { return myAsDes; }

  //! Returns new edges
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& NewEdges() { return myNewEdges; }

private:
  //! Stores the intersection results into AsDes
  Standard_EXPORT void Store(const TopoDS_Face&                    F1,
                             const TopoDS_Face&                    F2,
                             const NCollection_List<TopoDS_Shape>& LInt1,
                             const NCollection_List<TopoDS_Shape>& LInt2);

private:
  occ::handle<BRepAlgo_AsDes>                                   myAsDes;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myTouched;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myDone;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myNewEdges;
  TopAbs_State                                                  mySide;
  double                                                        myTol;
};
#endif // _BRepOffset_Inter3d_HeaderFile
