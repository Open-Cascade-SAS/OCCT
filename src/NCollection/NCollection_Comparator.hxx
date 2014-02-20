// Created on: 2011-01-27
// Created by: KGV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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
