// Created by: Peter KURNEV
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

#ifndef _BOPAlgo_ShellSplitter_HeaderFile
#define _BOPAlgo_ShellSplitter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_List.hxx>
#include <BOPTools_ConnexityBlock.hxx>
#include <BOPAlgo_Algo.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <TopoDS_Shape.hxx>
class TopoDS_Shape;

//! The class provides the splitting of the set of connected faces
//! on separate loops
class BOPAlgo_ShellSplitter : public BOPAlgo_Algo
{
public:
  DEFINE_STANDARD_ALLOC

  //! empty constructor
  Standard_EXPORT BOPAlgo_ShellSplitter();
  Standard_EXPORT ~BOPAlgo_ShellSplitter() override;

  //! constructor
  Standard_EXPORT BOPAlgo_ShellSplitter(const occ::handle<NCollection_BaseAllocator>& theAllocator);

  //! adds a face <theS> to process
  Standard_EXPORT void AddStartElement(const TopoDS_Shape& theS);

  //! return the faces to process
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& StartElements() const;

  //! performs the algorithm
  Standard_EXPORT void Perform(
    const Message_ProgressRange& theRange = Message_ProgressRange()) override;

  //! returns the loops
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Shells() const;

  Standard_EXPORT static void SplitBlock(BOPTools_ConnexityBlock& theCB);

protected:
  Standard_EXPORT void MakeShells(const Message_ProgressRange& theRange);

  NCollection_List<TopoDS_Shape>            myStartShapes;
  NCollection_List<TopoDS_Shape>            myShells;
  NCollection_List<BOPTools_ConnexityBlock> myLCB;
};

#endif // _BOPAlgo_ShellSplitter_HeaderFile
