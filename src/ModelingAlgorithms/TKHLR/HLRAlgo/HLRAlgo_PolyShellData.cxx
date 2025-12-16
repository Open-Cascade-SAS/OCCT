// Created on: 1995-05-05
// Created by: Christophe MARION
// Copyright (c) 1995-1999 Matra Datavision
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

#include <HLRAlgo_PolyShellData.hxx>

#include <HLRAlgo_BiPoint.hxx>
#include <HLRAlgo_ListIteratorOfListOfBPoint.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRAlgo_PolyShellData, Standard_Transient)

//=================================================================================================

HLRAlgo_PolyShellData::HLRAlgo_PolyShellData(const Standard_Integer nbFace)
    : myPolyg(1, nbFace)
{
}

//=================================================================================================

void HLRAlgo_PolyShellData::UpdateGlobalMinMax(Bnd_Box& theBox)
{
  for (HLRAlgo_ListIteratorOfListOfBPoint it(mySegList); it.More(); it.Next())
  {
    const HLRAlgo_BiPoint::PointsT& aPoints = it.Value().Points();
    theBox.Update(aPoints.PntP1.X(), aPoints.PntP1.Y(), aPoints.PntP1.Z());
    theBox.Update(aPoints.PntP2.X(), aPoints.PntP2.Y(), aPoints.PntP2.Z());
  }
  for (Standard_Integer i = myPolyg.Lower(); i <= myPolyg.Upper(); i++)
  {
    const Handle(HLRAlgo_PolyData)& aPd = myPolyg.ChangeValue(i);
    aPd->UpdateGlobalMinMax(theBox);
  }
}

//=================================================================================================

void HLRAlgo_PolyShellData::UpdateHiding(const Standard_Integer nbHiding)
{
  if (nbHiding > 0)
  {
    myHPolHi.Resize(1, nbHiding, false);
  }
  else
  {
    NCollection_Array1<Handle(HLRAlgo_PolyData)> anEmpty;
    myHPolHi.Move(anEmpty);
  }
}
