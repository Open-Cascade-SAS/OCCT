// Created on: 1997-01-24
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <AIS_GlobalStatus.ixx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

AIS_GlobalStatus::AIS_GlobalStatus():
myStatus(AIS_DS_None),
myLayerIndex(0),
myIsHilit(Standard_False),
myHiCol(Quantity_NOC_WHITE),
mySubInt(Standard_False)
{  
}

AIS_GlobalStatus::AIS_GlobalStatus(const AIS_DisplayStatus DS,
				   const Standard_Integer DMode,
				   const Standard_Integer SMode,
				   const Standard_Boolean ishilighted,
				   const Quantity_NameOfColor TheHiCol,
				   const Standard_Integer Layer):
myStatus(DS),
myLayerIndex(Layer),
myIsHilit(Standard_False),
myHiCol(TheHiCol),
mySubInt(Standard_False)
{
  myDispModes.Append(DMode);
  mySelModes.Append(SMode);
}

void AIS_GlobalStatus::RemoveDisplayMode(const Standard_Integer aMode)
{
  TColStd_ListIteratorOfListOfInteger anIt (myDispModes);
  for (; anIt.More(); anIt.Next())
  {
    if (anIt.Value() == aMode)
    {
      myDispModes.Remove (anIt);
      return;
    }
  }
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

Standard_Boolean AIS_GlobalStatus::IsDModeIn(const Standard_Integer aMode) const 
{
  TColStd_ListIteratorOfListOfInteger anIt (myDispModes);
  for (; anIt.More(); anIt.Next())
  {
    if (anIt.Value() == aMode)
    {
      return Standard_True;
    }
  }
  return Standard_False;
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
