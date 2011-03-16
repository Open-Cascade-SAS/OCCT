// File:      TColStd_MapIteratorOfPackedMapOfInteger.hxx
// Created:   26.11.05 16:47:34
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


#ifndef TColStd_MapIteratorOfPackedMapOfInteger_HeaderFile
#define TColStd_MapIteratorOfPackedMapOfInteger_HeaderFile

#include <TCollection_BasicMapIterator.hxx>
#include <Standard_NoSuchObject.hxx>

class TColStd_PackedMapOfInteger;

/**
 * Iterator of class TColStd_PackedMapOfInteger
 */

class TColStd_MapIteratorOfPackedMapOfInteger: public TCollection_BasicMapIterator
{
 public:
  // ---------- PUBLIC METHODS ----------

  /// Empty Constructor.
  inline TColStd_MapIteratorOfPackedMapOfInteger()
    : myIntMask (~0U), myKey (0) {}

  /// Constructor.
  Standard_EXPORT TColStd_MapIteratorOfPackedMapOfInteger
                                (const TColStd_PackedMapOfInteger&);

  /// Re-initialize with the same or another Map instance.
  Standard_EXPORT void        Initialize (const TColStd_PackedMapOfInteger&);

  /// Restart the iteraton
  Standard_EXPORT void        Reset();

  /// Query the iterated key. Defined in TColStd_PackedMapOfInteger.cxx
  inline Standard_Integer     Key () const
  {
    Standard_NoSuchObject_Raise_if ((myIntMask == ~0U),
                               "TColStd_MapIteratorOfPackedMapOfInteger::Key");
    return myKey;
  }

  /// Increment the iterator
  Standard_EXPORT void        Next();

private:
  unsigned int     myIntMask;   ///< all bits set above the iterated position
  Standard_Integer myKey;       ///< Currently iterated key
};


#endif
