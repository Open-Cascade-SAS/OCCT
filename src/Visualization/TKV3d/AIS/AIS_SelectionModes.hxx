// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _AIS_SelectionModes_HeaderFile
#define _AIS_SelectionModes_HeaderFile

#include <NCollection_LinearVector.hxx>
#include <Standard.hxx>
#include <Standard_OutOfRange.hxx>

//! Stores active AIS selection modes with stable iteration order.
//! Backed by a contiguous vector - the typical population is 1-5 modes,
//! so linear scans dominate hashed lookup on cache locality.
class AIS_SelectionModes
{
public:
  //! Iterator over active selection modes in insertion order.
  class Iterator
  {
  public:
    //! Constructor from selection modes container.
    Iterator(const AIS_SelectionModes& theModes)
        : myModes(&theModes.myModes),
          myIndex(0)
    {
    }

    //! Return TRUE if iterator points to a valid mode.
    bool More() const { return myIndex < myModes->Size(); }

    //! Advance iterator.
    void Next() { ++myIndex; }

    //! Return current selection mode.
    int Value() const { return myModes->Value(myIndex); }

  private:
    const NCollection_LinearVector<int>* myModes;
    size_t                               myIndex;
  };

public:
  //! Empty constructor.
  AIS_SelectionModes() {}

  //! Returns number of active modes.
  size_t Size() const { return myModes.Size(); }

  //! Return TRUE when no modes are stored.
  bool IsEmpty() const { return myModes.IsEmpty(); }

  //! Return TRUE when the only registered mode matches the given one.
  bool HasOnly(const int theMode) const
  {
    return myModes.Size() == 1 && myModes.Value(0) == theMode;
  }

  //! Return TRUE when the mode is stored.
  bool Contains(const int theMode) const
  {
    for (size_t i = 0; i < myModes.Size(); ++i)
    {
      if (myModes.Value(i) == theMode)
      {
        return true;
      }
    }
    return false;
  }

  //! Return mode by zero-based ordered index.
  int Mode(const int theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 0 || static_cast<size_t>(theIndex) >= myModes.Size(),
                                 "AIS_SelectionModes::Mode");
    return myModes.Value(static_cast<size_t>(theIndex));
  }

  //! Add selection mode if it is not stored yet.
  bool Add(const int theMode)
  {
    if (Contains(theMode))
    {
      return false;
    }
    myModes.Append(theMode);
    return true;
  }

  //! Remove selection mode.
  bool Remove(const int theMode)
  {
    for (size_t i = 0; i < myModes.Size(); ++i)
    {
      if (myModes.Value(i) == theMode)
      {
        myModes.Erase(i);
        return true;
      }
    }
    return false;
  }

  //! Remove all selection modes.
  void Clear() { myModes.Clear(); }

private:
  NCollection_LinearVector<int> myModes;
};

#endif // _AIS_SelectionModes_HeaderFile
