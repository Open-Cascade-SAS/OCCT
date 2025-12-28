// Created on: 2012-06-01
// Created by: Eugeny MALTCHIKOV
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

#ifndef _BRepFeat_Builder_HeaderFile
#define _BRepFeat_Builder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <BOPAlgo_BOP.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
class TopoDS_Shape;
class TopoDS_Face;

//! Provides a basic tool to implement features topological
//! operations. The main goal of the algorithm is to perform
//! the result of the operation according to the
//! kept parts of the tool.
//! Input data: a) DS;
//! b) The kept parts of the tool;
//! If the map of the kept parts of the tool
//! is not filled boolean operation of the
//! given type will be performed;
//! c) Operation required.
//! Steps: a) Fill myShapes, myRemoved maps;
//! b) Rebuild edges and faces;
//! c) Build images of the object;
//! d) Build the result of the operation.
//! Result: Result shape of the operation required.
class BRepFeat_Builder : public BOPAlgo_BOP
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepFeat_Builder();
  Standard_EXPORT virtual ~BRepFeat_Builder();

  //! Clears internal fields and arguments.
  Standard_EXPORT virtual void Clear() override;

  //! Initializes the object of local boolean operation.
  Standard_EXPORT void Init(const TopoDS_Shape& theShape);

  //! Initializes the arguments of local boolean operation.
  Standard_EXPORT void Init(const TopoDS_Shape& theShape, const TopoDS_Shape& theTool);

  //! Sets the operation of local boolean operation.
  //! If theFuse = 0 than the operation is CUT, otherwise FUSE.
  Standard_EXPORT void SetOperation(const int theFuse);

  //! Sets the operation of local boolean operation.
  //! If theFlag = TRUE it means that no selection of parts
  //! of the tool is needed, t.e. no second part. In that case
  //! if theFuse = 0 than operation is COMMON, otherwise CUT21.
  //! If theFlag = FALSE SetOperation(theFuse) function is called.
  Standard_EXPORT void SetOperation(const int theFuse, const bool theFlag);

  //! Collects parts of the tool.
  Standard_EXPORT void PartsOfTool(NCollection_List<TopoDS_Shape>& theLT);

  //! Initializes parts of the tool for second step of algorithm.
  //! Collects shapes and all sub-shapes into myShapes map.
  Standard_EXPORT void KeepParts(const NCollection_List<TopoDS_Shape>& theIm);

  //! Adds shape theS and all its sub-shapes into myShapes map.
  Standard_EXPORT void KeepPart(const TopoDS_Shape& theS);

  //! Main function to build the result of the
  //! local operation required.
  Standard_EXPORT void PerformResult(
    const Message_ProgressRange& theRange = Message_ProgressRange());

  //! Rebuilds faces in accordance with the kept parts of the tool.
  Standard_EXPORT void RebuildFaces();

  //! Rebuilds edges in accordance with the kept parts of the tool.
  Standard_EXPORT void RebuildEdge(
    const TopoDS_Shape&                                           theE,
    const TopoDS_Face&                                            theF,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theME,
    NCollection_List<TopoDS_Shape>&                               aLEIm);

  //! Collects the images of the object, that contains in
  //! the images of the tool.
  Standard_EXPORT void CheckSolidImages();

  //! Collects the removed parts of the tool into myRemoved map.
  Standard_EXPORT void FillRemoved();

  //! Adds the shape S and its sub-shapes into myRemoved map.
  Standard_EXPORT void FillRemoved(const TopoDS_Shape&                                     theS,
                                   NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theM);

protected:
  //! Prepares builder of local operation.
  Standard_EXPORT virtual void Prepare() override;

  //! Function is redefined to avoid the usage of removed faces.
  Standard_EXPORT virtual void FillIn3DParts(
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theDraftSolids,
    const Message_ProgressRange&                                              theRange) override;

  //! Avoid the check for open solids and always use the splits
  //! of solids for building the result shape.
  virtual bool CheckArgsForOpenSolid() override { return false; }

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> myShapes;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> myRemoved;
  int                                                    myFuse;
};

#endif // _BRepFeat_Builder_HeaderFile
