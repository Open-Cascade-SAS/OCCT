// Created on: 1997-01-24
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
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

#include <AIS_GlobalStatus.hxx>

#include <AIS_DisplayMode.hxx>
#include <Standard_Type.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_GlobalStatus, Standard_Transient)

AIS_GlobalStatus::AIS_GlobalStatus():
myStatus(AIS_DS_None),
myDispMode(AIS_WireFrame),
myLayerIndex(0),
myIsHilit(Standard_False),
mySubInt(Standard_False)
{  
}

AIS_GlobalStatus::AIS_GlobalStatus(const AIS_DisplayStatus DS,
				   const Standard_Integer DMode,
				   const Standard_Integer SMode,
				   const Standard_Boolean /*ishilighted*/,
				   const Standard_Integer Layer):
myStatus(DS),
myDispMode(DMode),
myLayerIndex(Layer),
myIsHilit(Standard_False),
mySubInt(Standard_False)
{
  mySelModes.Append(SMode);
}

void AIS_GlobalStatus::RemoveSelectionMode(const Standard_Integer aMode)
{
  TColStd_ListIteratorOfListOfInteger anIt (mySelModes);
  for (; anIt.More(); anIt.Next())
  {
    if (anIt.Value() == aMode)
    {
      mySelModes.Remove (anIt);
      return;
    }
  }
}

void AIS_GlobalStatus::ClearSelectionModes()
{
  mySelModes.Clear();
}

Standard_Boolean AIS_GlobalStatus::IsSModeIn(const Standard_Integer aMode) const 
{
  TColStd_ListIteratorOfListOfInteger anIt (mySelModes);
  for (; anIt.More(); anIt.Next())
  {
    if (anIt.Value() == aMode)
    {
      return Standard_True;
    }
  }
  return Standard_False;
}
