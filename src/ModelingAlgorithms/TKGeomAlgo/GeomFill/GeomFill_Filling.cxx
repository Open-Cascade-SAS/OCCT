// Created on: 1993-09-28
// Created by: Bruno DUMORTIER
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

#include <GeomFill_Filling.hxx>
#include <gp_Pnt.hxx>

//=================================================================================================

GeomFill_Filling::GeomFill_Filling()
    : IsRational(Standard_False)
{
}

//=================================================================================================

Standard_Integer GeomFill_Filling::NbUPoles() const
{
  return myPoles->ColLength();
}

//=================================================================================================

Standard_Integer GeomFill_Filling::NbVPoles() const
{
  return myPoles->RowLength();
}

//=================================================================================================

void GeomFill_Filling::Poles(TColgp_Array2OfPnt& Poles) const
{
  Poles = myPoles->ChangeArray2();
}

//=================================================================================================

Standard_Boolean GeomFill_Filling::isRational() const
{
  return IsRational;
}

//=================================================================================================

void GeomFill_Filling::Weights(TColStd_Array2OfReal& Weights) const
{
  Weights = myWeights->ChangeArray2();
}
