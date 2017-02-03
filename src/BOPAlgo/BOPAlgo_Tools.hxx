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

#ifndef _BOPAlgo_Tools_HeaderFile
#define _BOPAlgo_Tools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPCol_IndexedDataMapOfIntegerListOfInteger.hxx>
#include <BOPCol_DataMapOfIntegerListOfInteger.hxx>
#include <BOPCol_BaseAllocator.hxx>
#include <BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock.hxx>
#include <BOPDS_DataMapOfIntegerListOfPaveBlock.hxx>
#include <BOPDS_PDS.hxx>
#include <Standard_Integer.hxx>
#include <BOPDS_IndexedDataMapOfPaveBlockListOfInteger.hxx>

class BOPDS_PaveBlock;
class BOPDS_CommonBlock;
class IntTools_Context;
class TopoDS_Shape;

class BOPAlgo_Tools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void MakeBlocksCnx (const BOPCol_IndexedDataMapOfIntegerListOfInteger& theMILI, BOPCol_DataMapOfIntegerListOfInteger& theMBlocks, const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT static void MakeBlocks (const BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock& theMILI, BOPDS_DataMapOfIntegerListOfPaveBlock& theMBlocks, const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT static void PerformCommonBlocks (BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock& theMBlocks, const BOPCol_BaseAllocator& theAllocator, BOPDS_PDS& pDS);
  
  Standard_EXPORT static void FillMap (const Standard_Integer tneN1, const Standard_Integer tneN2, BOPCol_IndexedDataMapOfIntegerListOfInteger& theMILI, const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT static void FillMap (const Handle(BOPDS_PaveBlock)& tnePB1, const Handle(BOPDS_PaveBlock)& tnePB2, BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock& theMILI, const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT static void FillMap (const Handle(BOPDS_PaveBlock)& tnePB1, const Standard_Integer tneF, BOPDS_IndexedDataMapOfPaveBlockListOfInteger& theMILI, const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT static void PerformCommonBlocks (const BOPDS_IndexedDataMapOfPaveBlockListOfInteger& theMBlocks, const BOPCol_BaseAllocator& theAllocator, BOPDS_PDS& pDS);

  Standard_EXPORT static Standard_Real ComputeToleranceOfCB
                                        (const Handle(BOPDS_CommonBlock)& theCB,
                                         const BOPDS_PDS theDS,
                                         const Handle(IntTools_Context)& theContext);

  //! Creates planar wires from the given edges.<br>
  //! The input edges are expected to be planar. And for the performance
  //! sake the method does not check if the edges are really planar.<br>
  //! Thus, the result wires will also be not planar if the input edges are not planar.<br>
  //! The edges may be not shared, but the resulting wires will be sharing the
  //! coinciding parts and intersecting parts.<br>
  //! The output wires may be non-manifold and contain free and multi-connected vertices.<br>
  //! Parameters:
  //! <theEdges> - input edges;<br>
  //! <theWires> - output wires;<br>
  //! <theShared> - boolean flag which defines whether the input edges are already
  //!               shared or have to be intersected;<br>
  //! <theAngTol> - the angular tolerance which will be used for distinguishing
  //!               the planes in which the edges are located. Default value is
  //!               1.e-8 which is used for intersection of planes in IntTools_FaceFace.<br>
  //! Method returns the following error statuses:<br>
  //! 0 - in case of success (at least one wire has been built);<br>
  //! 1 - in case there are no edges in the given shape;<br>
  //! 2 - sharing of the edges has failed.<br>
  Standard_EXPORT static Standard_Integer EdgesToWires(const TopoDS_Shape& theEdges,
                                                       TopoDS_Shape& theWires,
                                                       const Standard_Boolean theShared = Standard_False,
                                                       const Standard_Real theAngTol = 1.e-8);

  //! Creates planar faces from given planar wires.<br>
  //! The method does not check if the wires are really planar.<br>
  //! The input wires may be non-manifold but should be shared.<br>
  //! The wires located in the same planes and included into other wires will create
  //! holes in the faces built from outer wires.<br>
  //! The tolerance values of the input shapes may be modified during the operation
  //! due to projection of the edges on the planes for creation of 2D curves.<br>
  //! Parameters:
  //! <theWires> - the given wires;<br>
  //! <theFaces> - the output faces;<br>
  //! <theAngTol> - the angular tolerance for distinguishing the planes in which
  //!               the wires are located. Default value is 1.e-8 which is used
  //!               for intersection of planes in IntTools_FaceFace.<br>
  //! Method returns TRUE in case of success, i.e. at least one face has been built.<br>
  Standard_EXPORT static Standard_Boolean WiresToFaces(const TopoDS_Shape& theWires,
                                                       TopoDS_Shape& theFaces,
                                                       const Standard_Real theAngTol = 1.e-8);
};

#endif // _BOPAlgo_Tools_HeaderFile
