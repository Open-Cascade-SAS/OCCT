// File:      MeshDS_PairOfIndex.hxx
// Created:   29.01.09 15:55:25
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2009

/*
* Purpose: This class represent pair of integer indices
*          It is restricted to store more than two indices in it       
*          This pair uses to store element indices connected to link
*/ 

#ifndef MeshDS_PairOfIndex_HeaderFile
#define MeshDS_PairOfIndex_HeaderFile

#include <Standard_OutOfRange.hxx>

class MeshDS_PairOfIndex
{
 public:
  MeshDS_PairOfIndex()
  { myIndx1 = myIndx2 = -1; }

  MeshDS_PairOfIndex(const MeshDS_PairOfIndex& theOther)
  {
    myIndx1 = theOther.myIndx1;
    myIndx1 = theOther.myIndx2;
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
	Standard_OutOfRange::Raise ("MeshDS_PairOfIndex::Append, more than two index to store");
      myIndx2 = theIndx;
    }
  }
  
  //! prepend index (store first index)
  void Prepend(const Standard_Integer theIndx)
  {
    if ( myIndx2 >= 0 )
      Standard_OutOfRange::Raise ("MeshDS_PairOfIndex::Append, more than two index to store");
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
