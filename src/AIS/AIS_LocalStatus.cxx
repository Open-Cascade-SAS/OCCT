// Copyright (c) 1998-1999 Matra Datavision
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

#include <AIS_LocalStatus.hxx>

#include <Standard_Type.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_LocalStatus, Standard_Transient)

AIS_LocalStatus::AIS_LocalStatus (const Standard_Boolean theIsTemporary,
                                  const Standard_Boolean theIsToDecompose,
                                  const Standard_Integer theDisplayMode,
                                  const Standard_Integer theSelectionMode,
                                  const Standard_Integer theHilightMode,
                                  const Standard_Boolean theIsSubIntensity,
                                  const Handle(Prs3d_Drawer)& theStyle)
: myDecomposition (theIsToDecompose),
  myIsTemporary   (theIsTemporary),
  myDMode         (theDisplayMode),
  myFirstDisplay  (Standard_False),
  myHMode         (theHilightMode),
  mySubIntensity  (theIsSubIntensity),
  myHiStyle       (theStyle)
{
  if (theSelectionMode != -1)
    mySModes.Append (theSelectionMode);
}


//=======================================================================
//function : IsActivated
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalStatus::
IsActivated(const Standard_Integer aSelMode) const 
{
  TColStd_ListIteratorOfListOfInteger It(mySModes);
  for(;It.More();It.Next())
    if(It.Value()==aSelMode)
      return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : RemoveSelectionMode
//purpose  : 
//=======================================================================

void AIS_LocalStatus::RemoveSelectionMode(const Standard_Integer aMode)
{
  TColStd_ListIteratorOfListOfInteger It(mySModes);
  for(;It.More();It.Next())
    {
      if(It.Value()==aMode) {
	mySModes.Remove(It);
	return;
      }
    }
}
//=======================================================================
//function : ClearSelectionModes
//purpose  : 
//=======================================================================

void AIS_LocalStatus::ClearSelectionModes()
{mySModes.Clear();}


//=======================================================================
//function : AddSelectionMode
//purpose  : 
//=======================================================================

void AIS_LocalStatus::AddSelectionMode(const Standard_Integer aMode)
{
  if(IsSelModeIn(aMode)) return;

  if(aMode!=-1)
    mySModes.Append(aMode);
  else
    mySModes.Clear();
}

//=======================================================================
//function : IsSelModeIn
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalStatus::IsSelModeIn(const Standard_Integer aMode) const
{
  for(TColStd_ListIteratorOfListOfInteger It(mySModes);
      It.More();
      It.Next()){
    if(It.Value()==aMode)
      return Standard_True;
  }
  return Standard_False;
}
