// Created on: 2009-01-29
// Created by: Pavel TELKOV
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

/*
* Purpose: This class represent pair of integer indices
*          It is restricted to store more than two indices in it       
*          This pair uses to store element indices connected to link
*/ 

#ifndef BRepMesh_PairOfIndex_HeaderFile
#define BRepMesh_PairOfIndex_HeaderFile

#include <Standard_OutOfRange.hxx>

class BRepMesh_PairOfIndex
{
public:
  BRepMesh_PairOfIndex()
  { myIndx1 = myIndx2 = -1; }

  BRepMesh_PairOfIndex(const BRepMesh_PairOfIndex& theOther)
  {
    myIndx1 = theOther.myIndx1;
    myIndx2 = theOther.myIndx2;
  }

  //! Clear indices
  void Clear()
  {
    myIndx1 = myIndx2 = -1;
  }

  //! append index (store first of last index of pair)
  void Append(const Standard_Integer theIndx)
  {
    if ( myIndx1 < 0 )
      myIndx1 = theIndx;
    else
    {
      if ( myIndx2 >= 0 )
        Standard_OutOfRange::Raise ("BRepMesh_PairOfIndex::Append, more than two index to store");
      myIndx2 = theIndx;
    }
  }

  //! prepend index (store first index)
  void Prepend(const Standard_Integer theIndx)
  {
    if ( myIndx2 >= 0 )
      Standard_OutOfRange::Raise ("BRepMesh_PairOfIndex::Append, more than two index to store");
    myIndx2 = myIndx1;
    myIndx1 = theIndx;
  }

  //! returns is pair not initialized by index
  Standard_Boolean IsEmpty() const
  {
    return (myIndx1 < 0 /*optimisation && myIndx2 < 0*/);
  }

  //! returns numner of initialized indeces
  Standard_Integer Extent() const
  {
    return (myIndx1 < 0 ? 0 : (myIndx2 < 0 ? 1 : 2));
  }

  //! returns first index from pair
  Standard_Integer FirstIndex() const
  {
    return myIndx1;
  }

  //! returns last index
  Standard_Integer LastIndex() const
  {
    return (myIndx2 < 0 ? myIndx1 : myIndx2);
  }

  Standard_Integer Index(const Standard_Integer theNum) const
  {
    return (theNum == 1 ? myIndx1 : myIndx2 /*(theNum == 2 ? myIndx2 : -1 )*/);
  }

  void SetIndex(const Standard_Integer theNum,
    const Standard_Integer theIndex)
  {
    theNum == 1 ? myIndx1 = theIndex : myIndx2 = theIndex;
  }

  //! remove indicated
  void RemoveIndex (const Standard_Integer theNum)
  {
    if ( theNum == 1 )
      myIndx1 = myIndx2;
    myIndx2 = -1;
  }
  //! fields
private:
  Standard_Integer myIndx1;
  Standard_Integer myIndx2;
};

#endif
