// Created on: 1992-05-14
// Created by: NW,JPB,CAL
// Copyright (c) 1992-1999 Matra Datavision
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


#include <Aspect_GenId.hxx>
#include <Aspect_IdentDefinitionError.hxx>

// =======================================================================
// function : Aspect_GenId
// purpose  :
// =======================================================================
Aspect_GenId::Aspect_GenId()
: myFreeCount  (INT_MAX / 2 + 1),
  myLength     (INT_MAX / 2 + 1),
  myLowerBound (0),
  myUpperBound (INT_MAX / 2)
{
  //
}

// =======================================================================
// function : Aspect_GenId
// purpose  :
// =======================================================================
Aspect_GenId::Aspect_GenId (const Standard_Integer theLow,
                            const Standard_Integer theUpper)
: myFreeCount  (theUpper - theLow + 1),
  myLength     (theUpper - theLow + 1),
  myLowerBound (theLow),
  myUpperBound (theUpper)
{
  if (theLow > theUpper)
  {
    throw Aspect_IdentDefinitionError("GenId Create Error: wrong interval");
  }
}

// =======================================================================
// function : HasFree
// purpose  :
// =======================================================================
Standard_Boolean Aspect_GenId::HasFree() const
{
  return myFreeCount > 0
      || myFreeIds.Extent() > 0;
}

// =======================================================================
// function : Available
// purpose  :
// =======================================================================
Standard_Integer Aspect_GenId::Available() const
{
  return myFreeCount + myFreeIds.Extent();
}

// =======================================================================
// function : Free
// purpose  :
// =======================================================================
void Aspect_GenId::Free()
{
  myFreeCount = myLength;
  myFreeIds.Clear();
}

// =======================================================================
// function : Free
// purpose  :
// =======================================================================
void Aspect_GenId::Free (const Standard_Integer theId)
{
  if (theId >= myLowerBound
   && theId <= myUpperBound)
  {
    if (myFreeCount + myFreeIds.Extent() + 1 == myLength)
    {
      myFreeCount = myLength;
      myFreeIds.Clear();
    }
    else
    {
      myFreeIds.Prepend (theId);
    }
  }
}

// =======================================================================
// function : Lower
// purpose  :
// =======================================================================
Standard_Integer Aspect_GenId::Lower() const
{
  return myLowerBound;
}

// =======================================================================
// function : Next
// purpose  :
// =======================================================================
Standard_Integer Aspect_GenId::Next()
{
  if (!myFreeIds.IsEmpty())
  {
    const Standard_Integer anId = myFreeIds.First();
    myFreeIds.RemoveFirst();
    return anId;
  }
  else if (myFreeCount < 1)
  {
    throw Aspect_IdentDefinitionError("GenId Next Error: Available == 0");
  }

  --myFreeCount;
  const Standard_Integer anId = myLowerBound + myLength - myFreeCount - 1;
  return anId;
}

// =======================================================================
// function : Upper
// purpose  :
// =======================================================================
Standard_Integer Aspect_GenId::Upper() const
{
  return myUpperBound;
}
