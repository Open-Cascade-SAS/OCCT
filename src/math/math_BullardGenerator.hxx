// Created on: 2014-07-18
// Created by: Alexander Malyshev
// Copyright (c) 2014-2014 OPEN CASCADE SAS
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

#ifndef _math_BullardGenerator_HeaderFile
#define _math_BullardGenerator_HeaderFile

#include <Standard_Real.hxx>

//! Fast random number generator (the algorithm proposed by Ian C. Bullard).
class math_BullardGenerator
{
public:

  //! Creates new Xorshift 64-bit RNG.
  math_BullardGenerator (unsigned int theSeed = 1)
  : myStateHi (theSeed)
  {
    SetSeed (theSeed);
  }

  //! Setup new seed / reset defaults.
  void SetSeed (unsigned int theSeed = 1)
  {
    myStateHi = theSeed;
    myStateLo = theSeed ^ 0x49616E42;
  }

  //! Generates new 64-bit integer value.
  unsigned int NextInt()
  {
    myStateHi = (myStateHi >> 2) + (myStateHi << 2);

    myStateHi += myStateLo;
    myStateLo += myStateHi;

    return myStateHi;
  }

  //! Generates new floating-point value.
  Standard_Real NextReal()
  {
    return NextInt() / static_cast<Standard_Real> (0xFFFFFFFFu);
  }

private:

  unsigned int myStateHi;
  unsigned int myStateLo;

};

#endif