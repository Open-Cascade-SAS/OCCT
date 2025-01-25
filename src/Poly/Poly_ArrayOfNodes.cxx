// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <Poly_ArrayOfNodes.hxx>

// =======================================================================
// function : Poly_ArrayOfNodes
// purpose  :
// =======================================================================
Poly_ArrayOfNodes::Poly_ArrayOfNodes(const Poly_ArrayOfNodes& theOther)
    : NCollection_AliasedArray(theOther)
{
  //
}

// =======================================================================
// function : ~Poly_ArrayOfNodes
// purpose  :
// =======================================================================
Poly_ArrayOfNodes::~Poly_ArrayOfNodes()
{
  //
}

// =======================================================================
// function : Assign
// purpose  :
// =======================================================================
Poly_ArrayOfNodes& Poly_ArrayOfNodes::Assign(const Poly_ArrayOfNodes& theOther)
{
  if (&theOther == this)
  {
    return *this;
  }

  if (myStride == theOther.myStride)
  {
    // fast copy
    NCollection_AliasedArray::Assign(theOther);
    return *this;
  }

  // slow copy
  if (mySize != theOther.mySize)
  {
    throw Standard_DimensionMismatch("Poly_ArrayOfNodes::Assign(), arrays have different sizes");
  }
  for (int anIter = 0; anIter < mySize; ++anIter)
  {
    const gp_Pnt aPnt = theOther.Value(anIter);
    SetValue(anIter, aPnt);
  }
  return *this;
}
