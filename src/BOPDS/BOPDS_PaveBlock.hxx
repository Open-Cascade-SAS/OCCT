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

#ifndef _BOPDS_PaveBlock_HeaderFile
#define _BOPDS_PaveBlock_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BOPCol_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_ListOfPave.hxx>
#include <Standard_Real.hxx>
#include <Bnd_Box.hxx>
#include <BOPCol_MapOfInteger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
class BOPDS_Pave;
class Bnd_Box;


class BOPDS_PaveBlock;
DEFINE_STANDARD_HANDLE(BOPDS_PaveBlock, Standard_Transient)


//! The class BOPDS_PaveBlock is to store
//! the information about pave block on an edge.
//! Two adjacent paves on edge make up pave block.
class BOPDS_PaveBlock : public Standard_Transient
{

public:

  

  //! Empty contructor
  Standard_EXPORT BOPDS_PaveBlock();
  

  //! Contructor
  //! <theAllocator> - the allocator to manage the memory
  Standard_EXPORT BOPDS_PaveBlock(const BOPCol_BaseAllocator& theAllocator);
  

  //! Modifier
  //! Sets the first pave <thePave>
  Standard_EXPORT void SetPave1 (const BOPDS_Pave& thePave);
  

  //! Selector
  //! Returns the first pave
  Standard_EXPORT const BOPDS_Pave& Pave1() const;
  

  //! Modifier
  //! Sets the second pave <thePave>
  Standard_EXPORT void SetPave2 (const BOPDS_Pave& thePave);
  

  //! Selector
  //! Returns the second pave
  Standard_EXPORT const BOPDS_Pave& Pave2() const;
  

  //! Modifier
  //! Sets the index of edge of pave block <theEdge>
  Standard_EXPORT void SetEdge (const Standard_Integer theEdge);
  

  //! Selector
  //! Returns the index of edge of pave block
  Standard_EXPORT Standard_Integer Edge() const;
  

  //! Query
  //! Returns true if the pave block has edge
  Standard_EXPORT Standard_Boolean HasEdge() const;
  

  //! Query
  //! Returns true if the pave block has edge
  //! Returns the index of edge <theEdge>
  Standard_EXPORT Standard_Boolean HasEdge (Standard_Integer& theEdge) const;
  

  //! Modifier
  //! Sets the index of original edge
  //! of the pave block <theEdge>
  Standard_EXPORT void SetOriginalEdge (const Standard_Integer theEdge);
  

  //! Selector
  //! Returns the index of original edge of pave block
  Standard_EXPORT Standard_Integer OriginalEdge() const;
  

  //! Query
  //! Returns true if the edge is equal to the original edge
  //! of the pave block
  Standard_EXPORT Standard_Boolean IsSplitEdge() const;
  

  //! Selector
  //! Returns the parametric range <theT1,theT2>
  //! of the pave block
  Standard_EXPORT void Range (Standard_Real& theT1, Standard_Real& theT2) const;
  

  //! Query
  //! Returns true if the pave block has pave indices
  //! that equal to the  pave indices of the pave block
  //! <theOther>
  Standard_EXPORT Standard_Boolean HasSameBounds (const Handle(BOPDS_PaveBlock)& theOther) const;
  

  //! Selector
  //! Returns the pave indices  <theIndex1,theIndex2>
  //! of the pave block
  Standard_EXPORT void Indices (Standard_Integer& theIndex1, 
                                Standard_Integer& theIndex2) const;
  

  //! Query
  //! Returns true if the pave block contains extra paves
  Standard_EXPORT Standard_Boolean IsToUpdate() const;
  

  //! Modifier
  //! Appends extra paves <thePave>
  Standard_EXPORT void AppendExtPave(const BOPDS_Pave& thePave);
  

  //! Modifier
  //! Appends extra pave <thePave>
  Standard_EXPORT void AppendExtPave1 (const BOPDS_Pave& thePave);
  
  //! Modifier
  //! Removes a pave with the given vertex number from extra paves
  Standard_EXPORT void RemoveExtPave(const Standard_Integer theVertNum);

  //! Selector
  //! Returns the  extra paves
  Standard_EXPORT const BOPDS_ListOfPave& ExtPaves() const;
  

  //! Selector / Modifier
  //! Returns the extra paves
  Standard_EXPORT BOPDS_ListOfPave& ChangeExtPaves();
  

  //! Modifier
  //! Updates the pave block. The extra paves are used
  //! to create new pave blocks <theLPB>.
  //! <theFlag> - if true, the first pave and the second
  //! pave are used to produce new pave blocks.
  Standard_EXPORT void Update (BOPDS_ListOfPaveBlock& theLPB, 
                               const Standard_Boolean theFlag = Standard_True);
  

  //! Query
  //! Returns true if the extra paves contain the pave
  //! with given value of the parameter <thePrm>
  //! <theTol>  - the value of the tolerance to compare
  //! <theInd>  - index of the found pave
  Standard_EXPORT Standard_Boolean ContainsParameter (const Standard_Real thePrm, 
                                                      const Standard_Real theTol,
                                                      Standard_Integer& theInd) const;
  

  //! Modifier
  //! Sets the shrunk data for the pave block
  //! <theTS1>,  <theTS2> - shrunk range
  //! <theBox> - the bounding box
  //! <theIsSplittable> - defines whether the edge can be split
  Standard_EXPORT void SetShrunkData (const Standard_Real theTS1, 
                                      const Standard_Real theTS2, 
                                      const Bnd_Box& theBox,
                                      const Standard_Boolean theIsSplittable);
  

  //! Selector
  //! Returns  the shrunk data for the pave block
  //! <theTS1>,  <theTS2> - shrunk range
  //! <theBox> - the bounding box
  //! <theIsSplittable> - defines whether the edge can be split
  Standard_EXPORT void ShrunkData (Standard_Real& theTS1, 
                                   Standard_Real& theTS2, 
                                   Bnd_Box& theBox,
                                   Standard_Boolean& theIsSplittable) const;
  

  //! Query
  //! Returns true if the pave block contains
  //! the shrunk data
  Standard_EXPORT Standard_Boolean HasShrunkData() const;
  
  Standard_EXPORT void Dump() const;

  //! Query
  //! Returns FALSE if the pave block has a too short
  //! shrunk range and cannot be split, otherwise returns TRUE
  Standard_Boolean IsSplittable() const
  {
    return myIsSplittable;
  }



  DEFINE_STANDARD_RTTIEXT(BOPDS_PaveBlock,Standard_Transient)

protected:


  BOPCol_BaseAllocator myAllocator;
  Standard_Integer myEdge;
  Standard_Integer myOriginalEdge;
  BOPDS_Pave myPave1;
  BOPDS_Pave myPave2;
  BOPDS_ListOfPave myExtPaves;
  Standard_Real myTS1;
  Standard_Real myTS2;
  Bnd_Box myShrunkBox;
  BOPCol_MapOfInteger myMFence;
  Standard_Boolean myIsSplittable;

private:




};







#endif // _BOPDS_PaveBlock_HeaderFile
