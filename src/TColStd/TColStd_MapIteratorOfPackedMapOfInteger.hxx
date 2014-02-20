// Created on: 2005-11-26
// Created by: Alexander GRIGORIEV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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
