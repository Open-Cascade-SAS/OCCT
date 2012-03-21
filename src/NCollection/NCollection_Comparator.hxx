// Created on: 2011-01-27
// Created by: KGV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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
