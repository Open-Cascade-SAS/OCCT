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

//! Stores information about two pave blocks and satellite data.
class BOPDS_CoupleOfPaveBlocks
{
public:
  //! Default constructor.
  BOPDS_CoupleOfPaveBlocks()
      : myIndexInterf(-1),
        myIndex(-1),
        myTolerance(0)
  {
  }

  //! Constructor with two pave blocks.
  //! @param[in] thePB1 first pave block
  //! @param[in] thePB2 second pave block
  BOPDS_CoupleOfPaveBlocks(const occ::handle<BOPDS_PaveBlock>& thePB1,
                           const occ::handle<BOPDS_PaveBlock>& thePB2)
      : myIndexInterf(-1),
        myIndex(-1),
        myTolerance(0)
  {
    SetPaveBlocks(thePB1, thePB2);
  }

  ~BOPDS_CoupleOfPaveBlocks() = default;

  //! Sets the index.
  //! @param[in] theIndex the index
  void SetIndex(const int theIndex) { myIndex = theIndex; }

  //! Returns the index.
  //! @return the index
  int Index() const { return myIndex; }

  //! Sets the index of an interference.
  //! @param[in] theIndex index of an interference
  void SetIndexInterf(const int theIndex) { myIndexInterf = theIndex; }

  //! Returns the index of an interference.
  //! @return index of an interference
  int IndexInterf() const { return myIndexInterf; }

  //! Sets both pave blocks.
  //! @param[in] thePB1 first pave block
  //! @param[in] thePB2 second pave block
  void SetPaveBlocks(const occ::handle<BOPDS_PaveBlock>& thePB1,
                     const occ::handle<BOPDS_PaveBlock>& thePB2)
  {
    myPB[0] = thePB1;
    myPB[1] = thePB2;
  }

  //! @deprecated Use PaveBlock1() and PaveBlock2() instead.
  Standard_DEPRECATED("Use PaveBlock1() and PaveBlock2() instead")
  void PaveBlocks(occ::handle<BOPDS_PaveBlock>& thePB1, occ::handle<BOPDS_PaveBlock>& thePB2) const
  {
    thePB1 = PaveBlock1();
    thePB2 = PaveBlock2();
  }

  //! Sets the first pave block.
  //! @param[in] thePB the first pave block
  void SetPaveBlock1(const occ::handle<BOPDS_PaveBlock>& thePB) { myPB[0] = thePB; }

  //! Returns the first pave block.
  //! @return handle to the first pave block
  const occ::handle<BOPDS_PaveBlock>& PaveBlock1() const { return myPB[0]; }

  //! Sets the second pave block.
  //! @param[in] thePB the second pave block
  void SetPaveBlock2(const occ::handle<BOPDS_PaveBlock>& thePB) { myPB[1] = thePB; }

  //! Returns the second pave block.
  //! @return handle to the second pave block
  const occ::handle<BOPDS_PaveBlock>& PaveBlock2() const { return myPB[1]; }

  //! Sets the tolerance associated with this couple.
  //! @param[in] theTol the tolerance value
  void SetTolerance(const double theTol) { myTolerance = theTol; }

  //! Returns the tolerance associated with this couple.
  //! @return the tolerance value
  double Tolerance() const { return myTolerance; }

protected:
  int                          myIndexInterf;
  int                          myIndex;
  occ::handle<BOPDS_PaveBlock> myPB[2];
  double                       myTolerance;
};

#endif
