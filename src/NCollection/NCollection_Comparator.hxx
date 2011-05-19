// File       NCollection_Comparator.hxx
// Created    27 January 2011
// Author     KGV
// Copyright  OpenCASCADE 2011

#ifndef _NCollection_Comparator_HeaderFile
#define _NCollection_Comparator_HeaderFile

#include <Precision.hxx>

/**
 * Class to define basic compare operations.
 * Basic implementation use redirection to standard C++ operators.
 * You can use standard C++ templates mechanisms to redefine these methods
 * or to inherit basic implementation to create multiple comparators
 * for same type with different rules.
 */
template<class TheItemType>
class NCollection_Comparator
{
public:

  NCollection_Comparator (const Standard_Real theTolerance = Precision::Confusion())
  : myTolerance (theTolerance) {}

  virtual ~NCollection_Comparator() {}

public:
  //! Comparison functions which should be overridden
  //! if standard operators are not defined for user type.

  //! Should return true if Left value is greater than Right
  virtual Standard_Boolean IsGreater (const TheItemType& theLeft, const TheItemType& theRight) const
  {
    return theLeft > theRight;
  }

  //! Should return true if values are equal
  virtual Standard_Boolean IsEqual (const TheItemType& theLeft, const TheItemType& theRight) const
  {
    return theLeft == theRight;
  }

public:
  //! Comparison functions which may be overridden for performance reasons

  //! Should return true if Left value is lower than Right
  virtual Standard_Boolean IsLower (const TheItemType& theLeft, const TheItemType& theRight) const
  {
    return !IsGreater (theLeft, theRight) && !IsEqual (theLeft, theRight);
  }

  virtual Standard_Boolean IsLowerEqual (const TheItemType& theLeft, const TheItemType& theRight) const
  {
    return !IsGreater (theLeft, theRight);
  }

  virtual Standard_Boolean IsGreaterEqual (const TheItemType& theLeft, const TheItemType& theRight) const
  {
    return IsGreater (theLeft, theRight) || IsEqual (theLeft, theRight);
  }

protected:

  Standard_Real myTolerance;

};

#endif /*_NCollection_Comparator_HeaderFile*/
