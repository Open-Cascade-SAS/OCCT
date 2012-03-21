// Copyright (c) 1998-1999 Matra Datavision
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

#include <AIS_LocalStatus.ixx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>


AIS_LocalStatus::AIS_LocalStatus(const Standard_Boolean IsTemp,
				 const Standard_Boolean Decomp,
				 const Standard_Integer DMode,
				 const Standard_Integer SMode,
				 const Standard_Integer HMode,
				 const Standard_Boolean SubIntensity,
				 const Quantity_NameOfColor HiCol):
myDecomposition(Decomp),
myIsTemporary(IsTemp),
myDMode(DMode),
myFirstDisplay(Standard_False),
myHMode(HMode),
mySubIntensity(SubIntensity),
myHiCol(HiCol)
{
  if(SMode!=-1) mySModes.Append(SMode);
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
