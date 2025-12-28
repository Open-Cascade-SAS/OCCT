// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef _BOPAlgo_BuilderArea_HeaderFile
#define _BOPAlgo_BuilderArea_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_Algo.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedMap.hxx>
class IntTools_Context;

//! The root class for algorithms to build
//! faces/solids from set of edges/faces
class BOPAlgo_BuilderArea : public BOPAlgo_Algo
{
public:
  DEFINE_STANDARD_ALLOC

  //! Sets the context for the algorithms
  void SetContext(const occ::handle<IntTools_Context>& theContext) { myContext = theContext; }

  //! Returns the input shapes
  const NCollection_List<TopoDS_Shape>& Shapes() const { return myShapes; }

  //! Sets the shapes for building areas
  void SetShapes(const NCollection_List<TopoDS_Shape>& theLS) { myShapes = theLS; }

  //! Returns the found loops
  const NCollection_List<TopoDS_Shape>& Loops() const { return myLoops; }

  //! Returns the found areas
  const NCollection_List<TopoDS_Shape>& Areas() const { return myAreas; }

  //! Defines the preventing of addition of internal parts into result.
  //! The default value is FALSE, i.e. the internal parts are added into result.
  void SetAvoidInternalShapes(const bool theAvoidInternal)
  {
    myAvoidInternalShapes = theAvoidInternal;
  }

  //! Returns the AvoidInternalShapes flag
  bool IsAvoidInternalShapes() const { return myAvoidInternalShapes; }

protected:
  Standard_EXPORT BOPAlgo_BuilderArea();
  Standard_EXPORT virtual ~BOPAlgo_BuilderArea();

  Standard_EXPORT BOPAlgo_BuilderArea(const occ::handle<NCollection_BaseAllocator>& theAllocator);

  virtual void PerformShapesToAvoid(const Message_ProgressRange& theRange) = 0;

  virtual void PerformLoops(const Message_ProgressRange& theRange) = 0;

  virtual void PerformAreas(const Message_ProgressRange& theRange) = 0;

  virtual void PerformInternalShapes(const Message_ProgressRange& theRange) = 0;

  occ::handle<IntTools_Context>        myContext;
  NCollection_List<TopoDS_Shape>       myShapes;
  NCollection_List<TopoDS_Shape>       myLoops;
  NCollection_List<TopoDS_Shape>       myLoopsInternal;
  NCollection_List<TopoDS_Shape>       myAreas;
  NCollection_IndexedMap<TopoDS_Shape> myShapesToAvoid;
  bool                                 myAvoidInternalShapes;
};

#endif // _BOPAlgo_BuilderArea_HeaderFile
