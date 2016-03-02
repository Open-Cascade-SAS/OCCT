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

#include <ShapePersistent_HArray2.hxx>
#include <StdObjMgt_ReadData.hxx>
#include <StdObject_gp.hxx>


template <class ArrayClass>
void ShapePersistent_HArray2::instance<ArrayClass>::readValue (
  StdObjMgt_ReadData&    theReadData,
  const Standard_Integer theRow,
  const Standard_Integer theCol)
{
  theReadData.ReadObject (
    StdObject_gp::Ref (this->myArray->ChangeValue (theRow, theCol)));
}

template class ShapePersistent_HArray2::instance<TColgp_HArray2OfXYZ>;
template class ShapePersistent_HArray2::instance<TColgp_HArray2OfPnt>;
template class ShapePersistent_HArray2::instance<TColgp_HArray2OfDir>;
template class ShapePersistent_HArray2::instance<TColgp_HArray2OfVec>;
template class ShapePersistent_HArray2::instance<TColgp_HArray2OfXY>;
template class ShapePersistent_HArray2::instance<TColgp_HArray2OfPnt2d>;
template class ShapePersistent_HArray2::instance<TColgp_HArray2OfDir2d>;
template class ShapePersistent_HArray2::instance<TColgp_HArray2OfVec2d>;
template class ShapePersistent_HArray2::instance<TColgp_HArray2OfLin2d>;
template class ShapePersistent_HArray2::instance<TColgp_HArray2OfCirc2d>;
