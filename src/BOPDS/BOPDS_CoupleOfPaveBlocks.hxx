// Created by: Peter KURNEV
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
  BOPDS_CoupleOfPaveBlocks() {
    myIndex=-1;
    myIndexInterf=-1;
  };
  //
  /**
   * Constructor
   * @param thePB1
   *  first pave block
   * @param thePB2
   *  secondt pave block
   */
  BOPDS_CoupleOfPaveBlocks(const Handle(BOPDS_PaveBlock)& thePB1,
			   const Handle(BOPDS_PaveBlock)& thePB2) {
    myIndex=-1;
    myIndexInterf=-1;
    SetPaveBlocks(thePB1, thePB2);
  };
  //
  /**
   * Destructor
   */
  ~BOPDS_CoupleOfPaveBlocks() {
  };
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
  //
 protected:
  Standard_Integer myIndexInterf;
  Standard_Integer myIndex;
  Handle(BOPDS_PaveBlock) myPB[2];
};
//
#endif
