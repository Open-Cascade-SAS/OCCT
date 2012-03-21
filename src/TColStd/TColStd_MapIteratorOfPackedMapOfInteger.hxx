// Created on: 2005-11-26
// Created by: Alexander GRIGORIEV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
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
