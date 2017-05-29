// Created by: Peter KURNEV
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_WireSplitter_HeaderFile
#define _BOPAlgo_WireSplitter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_PWireEdgeSet.hxx>
#include <BOPTools_ListOfConnexityBlock.hxx>
#include <BOPAlgo_Algo.hxx>
#include <BOPCol_BaseAllocator.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPTools_ConnexityBlock.hxx>
#include <IntTools_Context.hxx>
class BOPAlgo_WireEdgeSet;
class TopoDS_Wire;
class TopoDS_Face;


//! The class is to build loops from the given set of edges.
//!
//! It returns the following Error statuses
//! - *BOPAlgo_AlertNullInputShapes* - in case there no input edges to build the loops.
//!
class BOPAlgo_WireSplitter  : public BOPAlgo_Algo
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BOPAlgo_WireSplitter();
  Standard_EXPORT virtual ~BOPAlgo_WireSplitter();
  
  Standard_EXPORT BOPAlgo_WireSplitter(const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT void SetWES (const BOPAlgo_WireEdgeSet& theWES);
  
  Standard_EXPORT BOPAlgo_WireEdgeSet& WES();

  //! Sets the context for the algorithm
  Standard_EXPORT void SetContext(const Handle(IntTools_Context)& theContext);

  //! Returns the context
  Standard_EXPORT const Handle(IntTools_Context)& Context();

  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;
  
  static void MakeWire(BOPCol_ListOfShape& theLE, TopoDS_Wire& theW);
  
  Standard_EXPORT static void SplitBlock (const TopoDS_Face& theF,
                                          BOPTools_ConnexityBlock& theCB,
                                          const Handle(IntTools_Context)& theContext);

protected:

  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;
  
  Standard_EXPORT void MakeConnexityBlocks();
  
  Standard_EXPORT void MakeWires();

  BOPAlgo_PWireEdgeSet myWES;
  BOPTools_ListOfConnexityBlock myLCB;
  Handle(IntTools_Context) myContext;

};

#include <BOPAlgo_WireSplitter.lxx>

#endif // _BOPAlgo_WireSplitter_HeaderFile
