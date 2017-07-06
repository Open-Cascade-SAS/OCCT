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

#ifndef No_Exception
//#define No_Exception
#endif


#include <HLRAlgo_BiPoint.hxx>
#include <HLRAlgo_ListIteratorOfListOfBPoint.hxx>
#include <HLRAlgo_PolyData.hxx>
#include <HLRAlgo_PolyShellData.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRAlgo_PolyShellData,Standard_Transient)

//=======================================================================
//function : HLRAlgo_PolyShellData
//purpose  : 
//=======================================================================

HLRAlgo_PolyShellData::
HLRAlgo_PolyShellData (const Standard_Integer nbFace)
:  myPolyg(0,nbFace)
{}

//=======================================================================
//function : UpdateGlobalMinMax
//purpose  : 
//=======================================================================

void 
HLRAlgo_PolyShellData::UpdateGlobalMinMax(HLRAlgo_PolyData::Box& theBox)
{
  HLRAlgo_ListIteratorOfListOfBPoint it;
  
  for (it.Initialize(mySegList); it.More(); it.Next()) {      
    HLRAlgo_BiPoint& BP = it.Value();
    HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
    if (aPoints.PntP1.X() < aPoints.PntP2.X()) {
      if      (theBox.XMin > aPoints.PntP1.X()) theBox.XMin = aPoints.PntP1.X();
      else if (theBox.XMax < aPoints.PntP2.X()) theBox.XMax = aPoints.PntP2.X();
    }
    else             {
      if      (theBox.XMin > aPoints.PntP2.X()) theBox.XMin = aPoints.PntP2.X();
      else if (theBox.XMax < aPoints.PntP1.X()) theBox.XMax = aPoints.PntP1.X();
    }
    if (aPoints.PntP1.Y() < aPoints.PntP2.Y()) {
      if      (theBox.YMin > aPoints.PntP1.Y()) theBox.YMin = aPoints.PntP1.Y();
      else if (theBox.YMax < aPoints.PntP2.Y()) theBox.YMax = aPoints.PntP2.Y();
    }
    else             {
      if      (theBox.YMin > aPoints.PntP2.Y()) theBox.YMin = aPoints.PntP2.Y();
      else if (theBox.YMax < aPoints.PntP1.Y()) theBox.YMax = aPoints.PntP1.Y();
    }
    if (aPoints.PntP1.Z() < aPoints.PntP2.Z()) {
      if      (theBox.ZMin > aPoints.PntP1.Z()) theBox.ZMin = aPoints.PntP1.Z();
      else if (theBox.ZMax < aPoints.PntP2.Z()) theBox.ZMax = aPoints.PntP2.Z();
    }
    else             {
      if      (theBox.ZMin > aPoints.PntP2.Z()) theBox.ZMin = aPoints.PntP2.Z();
      else if (theBox.ZMax < aPoints.PntP1.Z()) theBox.ZMax = aPoints.PntP1.Z();
    }
  }
  Standard_Integer nbFace      = myPolyg.Upper();
  Handle(HLRAlgo_PolyData)* pd = NULL;
  if(nbFace > 0) pd =  (Handle(HLRAlgo_PolyData)*)&(myPolyg.ChangeValue(1));
  
  for (Standard_Integer i = 1; i <= nbFace; i++) {
    (*pd)->UpdateGlobalMinMax(theBox);
    pd++;
  }
}

//=======================================================================
//function : UpdateHiding
//purpose  : 
//=======================================================================

void HLRAlgo_PolyShellData::
UpdateHiding (const Standard_Integer nbHiding)
{
  if (nbHiding > 0)
    myHPolHi = new TColStd_HArray1OfTransient(1,nbHiding);
  else myHPolHi.Nullify();
}
