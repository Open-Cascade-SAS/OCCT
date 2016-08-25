// Created on: 2005-11-05
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

#ifndef TColStd_PackedMapOfInteger_HeaderFile
#define TColStd_PackedMapOfInteger_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <TCollection_BasicMap.hxx>

/**
 * Optimized Map of integer values. Each block of 32 integers is stored in 8 bytes in memory.
 */
class TColStd_PackedMapOfInteger : private TCollection_BasicMap
{
 public:
  // operators new and delete must be defined explicitly 
  // since inherited ones are not accessible
  DEFINE_STANDARD_ALLOC
  
 public:
  // ---------- PUBLIC METHODS ----------

  /// Constructor
  inline  TColStd_PackedMapOfInteger  (const Standard_Integer NbBuckets = 1)
    : TCollection_BasicMap (NbBuckets, Standard_True),
      myExtent             (0) {}

  /// Copy constructor
  inline TColStd_PackedMapOfInteger (const TColStd_PackedMapOfInteger& theOther)
    : TCollection_BasicMap (1, Standard_True),
      myExtent             (0)
  { Assign(theOther); }

  inline TColStd_PackedMapOfInteger&
                          operator =  (const TColStd_PackedMapOfInteger& Other) 
  { return Assign(Other); }

  Standard_EXPORT TColStd_PackedMapOfInteger&
                          Assign        (const TColStd_PackedMapOfInteger&);
  Standard_EXPORT  void   ReSize        (const Standard_Integer NbBuckets);
  Standard_EXPORT  void   Clear         ();
  ~TColStd_PackedMapOfInteger() { Clear(); }
  Standard_EXPORT  Standard_Boolean
                          Add           (const Standard_Integer aKey);
  Standard_EXPORT  Standard_Boolean
                          Contains      (const Standard_Integer aKey) const;
  Standard_EXPORT  Standard_Boolean
                          Remove        (const Standard_Integer aKey);

  inline Standard_Integer NbBuckets     () const
  { return TCollection_BasicMap::NbBuckets(); }

  inline Standard_Integer Extent        () const
  { return Standard_Integer (myExtent); }

  inline Standard_Boolean IsEmpty       () const
  { return TCollection_BasicMap::IsEmpty(); }

  inline void             Statistics    (Standard_OStream& outStream) const
  { TCollection_BasicMap::Statistics (outStream); }

  /**
   * Query the minimal contained key value.
   */
  Standard_EXPORT Standard_Integer GetMinimalMapped () const;

  /**
   * Query the maximal contained key value.
   */
  Standard_EXPORT Standard_Integer GetMaximalMapped () const;

public:
  //!@name Boolean operations with maps as sets of integers
  //!@{

  /**
   * Sets this Map to be the result of union (aka addition, fuse, merge, boolean OR) operation between two given Maps.
   * The new Map contains the values that are contained either in the first map or in the second map or in both.
   * All previous contents of this Map is cleared. This map (result of the boolean operation) can also be passed as one of operands.
   */
  Standard_EXPORT void Union (const TColStd_PackedMapOfInteger&,
                              const TColStd_PackedMapOfInteger&);

  /**
   * Apply to this Map the boolean operation union (aka addition, fuse, merge, boolean OR) with another (given) Map.
   * The result contains the values that were previously contained in this map or contained in the given (operand) map.
   * This algorithm is similar to method Union().
   * @return True if content of this map is changed
   */
  Standard_EXPORT Standard_Boolean Unite (const TColStd_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Unite().
   */
  TColStd_PackedMapOfInteger& operator |= (const TColStd_PackedMapOfInteger& MM)
  { Unite(MM); return *this; }

  /**
   * Sets this Map to be the result of intersection (aka multiplication, common, boolean AND) operation between two given Maps.
   * The new Map contains only the values that are contained in both map operands.
   * All previous contents of this Map is cleared. This same map (result of the boolean operation) can also be used as one of operands.
   * The order of operands makes no difference; the method minimizes internally the number of iterations using the smallest map for the loop.
   */
  Standard_EXPORT void Intersection (const TColStd_PackedMapOfInteger&,
                                     const TColStd_PackedMapOfInteger&);

  /**
   * Apply to this Map the intersection operation (aka multiplication, common,  boolean AND) with another (given) Map.
   * The result contains only the values that are contained in both this and the given maps.
   * This algorithm is similar to method Intersection().
   * @return True if content of this map is changed
   */
  Standard_EXPORT Standard_Boolean Intersect (const TColStd_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Intersect().
   */
  TColStd_PackedMapOfInteger& operator &= (const TColStd_PackedMapOfInteger& MM)
  { Intersect(MM); return *this; }

  /**
   * Sets this Map to be the result of subtraction
   * (aka set-theoretic difference, relative complement, exclude, cut, boolean NOT) operation between two given Maps.
   * The new Map contains only the values that are contained in the first map operands and not contained in the second one.
   * All previous contents of this Map is cleared.
   * This map (result of the boolean operation) can also be used as the first operand.
   */
  Standard_EXPORT void Subtraction (const TColStd_PackedMapOfInteger&,
                                    const TColStd_PackedMapOfInteger&);

  /**
   * Apply to this Map the subtraction (aka set-theoretic difference, relative complement, exclude, cut, boolean NOT) operation with another (given) Map.
   * The result contains only the values that were previously contained in this map and not contained in this map.
   * This algorithm is similar to method Subtract() with two operands.
   * @return True if contents of this map is changed
   */
  Standard_EXPORT Standard_Boolean Subtract (const TColStd_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Subtract().
   */
  TColStd_PackedMapOfInteger& operator -= (const TColStd_PackedMapOfInteger& MM)
  { Subtract(MM); return *this; }

  /**
   * Sets this Map to be the result of symmetric difference (aka exclusive disjunction, boolean XOR) operation between two given Maps.
   * The new Map contains the values that are contained only in the first or the second operand maps but not in both.
   * All previous contents of this Map is cleared.
   * This map (result of the boolean operation) can also be used as one of operands.
   */
  Standard_EXPORT void Difference (const TColStd_PackedMapOfInteger&,
                                   const TColStd_PackedMapOfInteger&);

  /**
   * Apply to this Map the symmetric difference (aka exclusive disjunction, boolean XOR) operation with another (given) Map.
   * The result contains the values that are contained only in this or the operand map, but not in both.
   * This algorithm is similar to method Difference().
   * @return True if contents of this map is changed
   */
  Standard_EXPORT Standard_Boolean Differ (const TColStd_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Differ().
   */
  TColStd_PackedMapOfInteger& operator ^= (const TColStd_PackedMapOfInteger& MM)
  { Differ(MM); return *this; }

  /**
   * Returns True if this map is equal to the given one, i.e. they contain the
   * same sets of elements
   */
  Standard_EXPORT Standard_Boolean IsEqual (const TColStd_PackedMapOfInteger&) const;

  /**
   * Overloaded operator version of IsEqual().
   */
  Standard_Boolean operator == (const TColStd_PackedMapOfInteger& MM) const
  { return IsEqual(MM); }

  /**
   * Returns True if this map is subset of the given one, i.e. all elements 
   * contained in this map is contained also in the operand map.
   * if this map is empty that this method returns true for any operand map.
   */
  Standard_EXPORT Standard_Boolean IsSubset (const TColStd_PackedMapOfInteger&) const;

  /**
   * Overloaded operator version of IsSubset().
   */
  Standard_Boolean operator <= (const TColStd_PackedMapOfInteger& MM) const
  { return IsSubset(MM); }

  /**
   * Returns True if this map has common items with the given one.
   */
  Standard_EXPORT Standard_Boolean HasIntersection (const TColStd_PackedMapOfInteger&) const;

  //!@}
  
 protected:
  // ---------- PROTECTED METHODS ----------
  inline Standard_Integer InternalExtent () const
  { return TCollection_BasicMap::Extent(); }


 private:
  // ---------- PRIVATE FIELDS ----------

  size_t                myExtent;

  friend class TColStd_MapIteratorOfPackedMapOfInteger;
};

#endif
