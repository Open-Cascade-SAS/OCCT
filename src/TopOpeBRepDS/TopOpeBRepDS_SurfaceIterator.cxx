// Created on: 1994-06-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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


#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>

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
