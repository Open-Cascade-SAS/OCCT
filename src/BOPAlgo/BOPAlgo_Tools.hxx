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

};

#endif // _BOPAlgo_Tools_HeaderFile
