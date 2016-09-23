// Created on: 2016-06-07
// Created by: Nikolai BUKHALOV
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _Bnd_Range_HeaderFile
#define _Bnd_Range_HeaderFile

#include <Standard_Real.hxx>
#include <Standard_ConstructionError.hxx>

//! This class describes a range in 1D space restricted
//! by two real values.
//! A range can be void indicating there is no point included in the range.

class Bnd_Range
{
public:

  //! Default constructor. Creates VOID range.
  Bnd_Range() : myFirst(0.0), myLast(-1.0)
  {
  };

  //! Constructor. Never creates VOID range.
  Bnd_Range(const Standard_Real theMin, const Standard_Real theMax) : 
                                                    myFirst(theMin), myLast(theMax)
  {
    if(myLast < myFirst)
      Standard_ConstructionError::Raise("Last < First");
  };

  //! Replaces <this> with common-part of <this> and theOther
  Standard_EXPORT void Common(const Bnd_Range& theOther);

  //! Extends <this> to include theParameter
  void Add(const Standard_Real theParameter)
  {
    if(IsVoid())
    {
      myFirst = myLast = theParameter;
      return;
    }

    myFirst = Min(myFirst, theParameter);
    myLast = Max(myLast, theParameter);
  }

  //! Obtain MIN boundary of <this>.
  //! If <this> is VOID the method returns false.
  Standard_Boolean GetMin(Standard_Real& thePar) const
  {
    if(IsVoid())
    {
      return Standard_False;
    }

    thePar = myFirst;
    return Standard_True;
  }

  //! Obtain MAX boundary of <this>.
  //! If <this> is VOID the method returns false.
  Standard_Boolean GetMAX(Standard_Real& thePar) const
  {
    if(IsVoid())
    {
      return Standard_False;
    }

    thePar = myLast;
    return Standard_True;
  }

  //! Returns range value (MAX-MIN). Returns negative value for VOID range.
  Standard_Real Delta() const
  {
    return (myLast - myFirst);
  }

  //! Is <this> initialized.
  Standard_Boolean IsVoid() const
  {
    return (myLast < myFirst);
  }

  //! Initializes <this> by default parameters. Makes <this> VOID.
  void SetVoid()
  {
    myLast = -1.0;
    myFirst = 0.0;
  }

  //! Extends this to the given value (in both side)
  void Enlarge(const Standard_Real theDelta)
  {
    if (IsVoid())
    {
      return;
    }

    myFirst -= theDelta;
    myLast += theDelta;
  }

private:
  //! Start of range
  Standard_Real myFirst;

  //! End of range
  Standard_Real myLast;
};

#endif