// Created on: 2005-12-20
// Created by: Julia GERASIMOVA
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

#ifndef _math_ValueAndWeight_HeaderFile
#define _math_ValueAndWeight_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Simple container storing two reals: value and weight
class math_ValueAndWeight
{
public:
  DEFINE_STANDARD_ALLOC

  math_ValueAndWeight()
      : myValue(0.),
        myWeight(0.)
  {
  }

  math_ValueAndWeight(double theValue, double theWeight)
      : myValue(theValue),
        myWeight(theWeight)
  {
  }

  double Value() const { return myValue; }

  double Weight() const { return myWeight; }

private:
  double myValue;
  double myWeight;
};

//! Comparison operator for math_ValueAndWeight, needed for sorting algorithms
inline bool operator<(const math_ValueAndWeight& theLeft, const math_ValueAndWeight& theRight)
{
  return theLeft.Value() < theRight.Value();
}

#endif // _math_ValueAndWeight_HeaderFile
