// Created on: 1994-06-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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


#include <TopOpeBRepDS_Interference.hxx>

#include <TopOpeBRepDS_SurfaceIterator.ixx>

//=======================================================================
//function : TopOpeBRepDS_SurfaceIterator
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceIterator::TopOpeBRepDS_SurfaceIterator
  (const TopOpeBRepDS_ListOfInterference& L) :
  TopOpeBRepDS_InterferenceIterator(L)
{
  TopOpeBRepDS_InterferenceIterator::GeometryKind(TopOpeBRepDS_SURFACE);
}

//=======================================================================
//function : Current
//purpose  : 
//=======================================================================

Standard_Integer  TopOpeBRepDS_SurfaceIterator::Current()const 
{
  Handle(TopOpeBRepDS_Interference) i = Value();
  Standard_Integer g = i->Geometry();
  return g;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation TopOpeBRepDS_SurfaceIterator::Orientation
  (const TopAbs_State S) const 
{
  Handle(TopOpeBRepDS_Interference) i = Value();
  const TopOpeBRepDS_Transition& t = i->Transition();
  TopAbs_Orientation o = t.Orientation(S);
  return o;
}
