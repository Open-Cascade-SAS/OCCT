// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
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


#include <Geom2d_Curve.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>

//=======================================================================
//function : TopOpeBRepDS_CurveIterator
//purpose  : 
//=======================================================================
TopOpeBRepDS_CurveIterator::TopOpeBRepDS_CurveIterator
  (const TopOpeBRepDS_ListOfInterference& L) :
  TopOpeBRepDS_InterferenceIterator(L)
{
  Match();
}

//=======================================================================
//function : MatchInterference
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_CurveIterator::MatchInterference
   (const Handle(TopOpeBRepDS_Interference)& I) const
{
  TopOpeBRepDS_Kind GT = I->GeometryType();
  Standard_Boolean r = ( GT == TopOpeBRepDS_CURVE );
  return r;
}


//=======================================================================
//function : Current
//purpose  : 
//=======================================================================

Standard_Integer  TopOpeBRepDS_CurveIterator::Current()const 
{
  Handle(TopOpeBRepDS_Interference) I = Value();
  Standard_Integer G = I->Geometry();
  return G;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation  TopOpeBRepDS_CurveIterator::Orientation
  (const TopAbs_State S)const 
{
  Handle(TopOpeBRepDS_Interference) I = Value();
  const TopOpeBRepDS_Transition& T = I->Transition();
  TopAbs_Orientation o = T.Orientation(S);
  return o;
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)&  TopOpeBRepDS_CurveIterator::PCurve()const 
{
  return 
    (*((Handle(TopOpeBRepDS_SurfaceCurveInterference)*)&Value()))->PCurve();
}

