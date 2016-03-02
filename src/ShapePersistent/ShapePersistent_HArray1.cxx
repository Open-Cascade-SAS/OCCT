// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <ShapePersistent_HArray1.hxx>
#include <StdObjMgt_ReadData.hxx>
#include <StdObject_gp.hxx>


template <class ArrayClass>
void ShapePersistent_HArray1::instance<ArrayClass>::readValue
  (StdObjMgt_ReadData& theReadData, const Standard_Integer theIndex)
{
  theReadData.ReadObject (StdObject_gp::Ref (this->myArray->ChangeValue (theIndex)));
}

template<>
void ShapePersistent_HArray1::instance<Poly_HArray1OfTriangle>::readValue
  (StdObjMgt_ReadData& theReadData, const Standard_Integer theIndex)
{
  Value<Standard_Integer> N1, N2, N3;
  theReadData >> N1 >> N2 >> N3;
  this->myArray->ChangeValue (theIndex).Set (N1, N2, N3);
}

template class ShapePersistent_HArray1::instance<TColgp_HArray1OfXYZ>;
template class ShapePersistent_HArray1::instance<TColgp_HArray1OfPnt>;
template class ShapePersistent_HArray1::instance<TColgp_HArray1OfDir>;
template class ShapePersistent_HArray1::instance<TColgp_HArray1OfVec>;
template class ShapePersistent_HArray1::instance<TColgp_HArray1OfXY>;
template class ShapePersistent_HArray1::instance<TColgp_HArray1OfPnt2d>;
template class ShapePersistent_HArray1::instance<TColgp_HArray1OfDir2d>;
template class ShapePersistent_HArray1::instance<TColgp_HArray1OfVec2d>;
template class ShapePersistent_HArray1::instance<TColgp_HArray1OfLin2d>;
template class ShapePersistent_HArray1::instance<TColgp_HArray1OfCirc2d>;
template class ShapePersistent_HArray1::instance<Poly_HArray1OfTriangle>;
