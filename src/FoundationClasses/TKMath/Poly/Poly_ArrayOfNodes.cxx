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

//=================================================================================================

Poly_ArrayOfNodes::Poly_ArrayOfNodes(const Poly_ArrayOfNodes& theOther)

  = default;

//=================================================================================================

Poly_ArrayOfNodes::~Poly_ArrayOfNodes() = default;

//=================================================================================================

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
  if (myStride == sizeof(gp_Pnt))
  {
    gp_Pnt*                        aTarget = ChangeData<gp_Pnt>();
    const NCollection_Vec3<float>* aSource = theOther.Data<NCollection_Vec3<float>>();
    for (size_t anIter = 0; anIter < mySize; ++anIter)
    {
      aTarget[anIter].SetCoord(aSource[anIter].x(), aSource[anIter].y(), aSource[anIter].z());
    }
  }
  else
  {
    NCollection_Vec3<float>* aTarget = ChangeData<NCollection_Vec3<float>>();
    const gp_Pnt*            aSource = theOther.Data<gp_Pnt>();
    for (size_t anIter = 0; anIter < mySize; ++anIter)
    {
      aTarget[anIter].SetValues((float)aSource[anIter].X(),
                                (float)aSource[anIter].Y(),
                                (float)aSource[anIter].Z());
    }
  }
  return *this;
}
