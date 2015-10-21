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

#ifndef BOPDS_CoupleOfPaveBlocks_HeaderFile
#define BOPDS_CoupleOfPaveBlocks_HeaderFile

#include <BOPDS_PaveBlock.hxx>

/**
 * The Class BOPDS_CoupleOfPaveBlocks is to store
 * the information about two pave blocks 
 * and some satellite information 
 *
*/
//=======================================================================
//class : BOPDS_CoupleOfPaveBlocks
//purpose  : 
//=======================================================================
class BOPDS_CoupleOfPaveBlocks {
 public:
  /**
   * Constructor
   */
  BOPDS_CoupleOfPaveBlocks() :
    myIndexInterf(-1),
    myIndex(-1),
    myTolerance(0)
  {}
  //
  /**
   * Constructor
   * @param thePB1
   *  first pave block
   * @param thePB2
   *  secondt pave block
   */
  BOPDS_CoupleOfPaveBlocks(const Handle(BOPDS_PaveBlock)& thePB1,
    const Handle(BOPDS_PaveBlock)& thePB2) :
    myIndexInterf(-1),
    myIndex(-1),
    myTolerance(0)
  {
    SetPaveBlocks(thePB1, thePB2);
  }
  //
  /**
   * Destructor
   */
  ~BOPDS_CoupleOfPaveBlocks() {
  }
  //
  /**
   * Sets an index
   * @param theIndex
   *  index
   */
  void SetIndex(const Standard_Integer theIndex) {
    myIndex=theIndex;
  } 
  //
  /**
   * Returns the index
   * @return 
   *   index
   */
  Standard_Integer Index()const {
    return myIndex;
  } 
  // 
  /**
   * Sets an index of an interference 
   * @param theIndex
   *  index of an interference 
   */ 
  void SetIndexInterf(const Standard_Integer theIndex) {
    myIndexInterf=theIndex;
  } 
  //
  /**
   * Returns the index of an interference 
   * @return 
   *   index of an interference 
   */
  Standard_Integer IndexInterf()const {
    return myIndexInterf;
  } 
  // 
  /**
   * Sets pave blocks
   * @param thePB1
   *  first pave block
   * @param thePB2
   *  secondt pave block
   */ 
  void SetPaveBlocks(const Handle(BOPDS_PaveBlock)& thePB1,
		     const Handle(BOPDS_PaveBlock)& thePB2) {
    myPB[0]=thePB1;
    myPB[1]=thePB2;
  } 
  // 
  /**
   * Returns pave blocks
   * @param thePB1
   *  the first pave block
   * @param thePB2
   *  the second pave block
   */ 
  void PaveBlocks(Handle(BOPDS_PaveBlock)& thePB1,
		  Handle(BOPDS_PaveBlock)& thePB2) const {
    thePB1=myPB[0];
    thePB2=myPB[1];       
  }
  // 
  /**
   * Sets the first pave block
   * @param thePB
   *  the first pave block
   */  
  void SetPaveBlock1(const Handle(BOPDS_PaveBlock)& thePB) {
    myPB[0]=thePB;
  } 
  /**
   * Returns the first pave block
   * @return 
   * the first pave block
   */  
  const Handle(BOPDS_PaveBlock)& PaveBlock1()const {
    return myPB[0];
  }
  //
  /**
   * Sets the second pave block
   * @param thePB
   *  the second pave block
   */  
  void SetPaveBlock2(const Handle(BOPDS_PaveBlock)& thePB) {
    myPB[1]=thePB;
  } 
  //
  /**
   * Returns the second pave block
   * @return 
   * the second pave block
   */  
  const Handle(BOPDS_PaveBlock)& PaveBlock2()const {
    return myPB[1];
  }

  /**
  * Sets the tolerance associated with this couple
  */
  void SetTolerance(const Standard_Real theTol) {
    myTolerance = theTol;
  }
  //
  /**
  * Returns the tolerance associated with this couple
  */
  Standard_Real Tolerance()const {
    return myTolerance;
  }

 protected:
  Standard_Integer myIndexInterf;
  Standard_Integer myIndex;
  Handle(BOPDS_PaveBlock) myPB[2];
  Standard_Real    myTolerance;
};
//
#endif
