// Created on: 1993-03-08
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//=================================================================================================

inline void TopOpeBRepBuild_BlockIterator::Initialize()
{
  myValue = myLower;
}

//=================================================================================================

inline bool TopOpeBRepBuild_BlockIterator::More() const
{
  bool b = (myValue <= myUpper);
  return b;
}

//=================================================================================================

inline void TopOpeBRepBuild_BlockIterator::Next()
{
  myValue++;
}

//=================================================================================================

inline int TopOpeBRepBuild_BlockIterator::Value() const
{
  return myValue;
}

//=================================================================================================

inline int TopOpeBRepBuild_BlockIterator::Extent() const
{
  if (myLower != 0)
  {
    int n = myUpper - myLower + 1;
    return n;
  }
  return 0;
}
